/***************************************************************************
    filter.cpp
                             -------------------
    Class Filter implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <time.h>

#include <QDir>
#include <QImage>
#include <QPainter>
#include <QFileInfo>

// Get the SYSTEM_PATH
#include "../qdvdauthor/CONFIG.h"

#include "utils.h"
#include "engine/ffmpeg_enc.h"
#include "filter.h"

namespace Render
{

/****************************************************************************
** Base class Filter
****************************************************************************/
Filter::Filter ( )
{
  m_bUseKenBurns    = false;
  m_iAspectRatio    = 0;
  m_iTotalImages[0] = 0;
  m_iTotalImages[1] = 0;
}

Filter::~Filter ( )
{
}

// static function to create a Filter object based on effect_struct
Filter *Filter::create ( CXmlSlideshow *pSlideshow, CXmlSlideshow::filter_struct *pFilter )
{
  // Here we get the valid filter names from the user selection
  QString qsFilter;
  if ( pSlideshow )  {
    // For now we'll get a random filter ... Later more ...
//    QString qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
//    QString qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    QStringList listOfFilter = pSlideshow->validFilterNames;
    int iFilter = (int)( ( (float)rand ( ) / RAND_MAX ) * listOfFilter.count ( ) );
    qsFilter    = listOfFilter[iFilter];
  }

  if ( qsFilter == "Cross Filter" )
    return new CrossFadeFilter;

  if ( ! pFilter )
    return new AlphaFilter ( qsFilter );

  if ( pFilter->name == FILTER_CROSS_FADE )
    return new CrossFadeFilter;
  if ( pFilter->name == FILTER_KEN_BURNS  )
    return new KenBurnsFilter;
  if ( pFilter->name == FILTER_ALPHA_FADE )
    return new AlphaFilter ( qsFilter );
  // Default to Cross Fade ...
  return new CrossFadeFilter;
}

bool Filter::exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage * )
{
  return false;
}

void Filter::setKenBurnsParams ( CXmlSlideshow::img_struct *pStart, CXmlSlideshow::img_struct *pStop, QImage &background, int iAspectRatio, int iTotal1, int iTotal2 )
{
  if ( pStart )
    m_kenBurns[0] = *pStart;
  if ( pStop )
    m_kenBurns[1] = *pStop;
  if ( pStart || pStop )
    m_bUseKenBurns  = true;
  m_background      = background;
  m_iAspectRatio    = iAspectRatio;
  m_iTotalImages[0] = iTotal1;
  m_iTotalImages[1] = iTotal2;
}

void Filter::encode ( Encoder::FFmpeg *pEncoder, QImage &result, float fProgress, enAudioTrans audioTrans )
{
  float  fVolume;
  // And at the end we will add this to the VOB compliant video TS
  switch ( audioTrans )  {
  case VolumeNull:
    fVolume = 0.0f;
  break;
  case VolumeUp:
    fVolume = fProgress;
  break;
  case VolumeDown:
    fVolume = 1.0f  - fProgress;
  break;
  default:
    fVolume = 1.0f;
  }
  pEncoder->addImage ( &result, 1, fVolume );
}

QStringList Filter::getFilterNames ( )
{
  // Get all supported filter names.
  CrossFadeFilter xFilter;
//  KenBurnsFilter kbFilter;
  AlphaFilter     aFilter;

  QList<Filter *> filterList;
  filterList.append (  &xFilter );
//  filterList.append ( &kbFilter );
  filterList.append (  &aFilter );

  QStringList list, retList;
  QStringList::iterator it;

  QList<Filter *>::iterator itFilter = filterList.begin ( );
  while  ( itFilter !=  filterList.end ( ) )  {
    list=(*itFilter++)->getFilterNames ( );
    it = list.begin ( );
    while ( it != list.end ( ) )
         retList.append ( *it++ );
  }

  return retList;
}

/****************************************************************************
** class CrossFadeFilter
****************************************************************************/
CrossFadeFilter::CrossFadeFilter ( )
{
}

CrossFadeFilter::~CrossFadeFilter ( )
{
}

bool CrossFadeFilter::exec ( Encoder::FFmpeg *pEncoder, QImage &imgStart, QImage &imgStop, int iNumberOfImages, enAudioTrans audioTrans, QImage * )
{
  int    t;
  QImage result;
  qreal  fAlpha;

  for  ( t=0; t<iNumberOfImages; t++ )  {
    result = imgStart;
    fAlpha = (qreal)t / ( iNumberOfImages - 1 );

    QPainter thePainter   ( &result );
    thePainter.setOpacity (  fAlpha ); // 0 = transparent 1=opaque
    thePainter.drawImage  ( 0, 0, imgStop, 0, 0 );

    encode ( pEncoder, result, (float)t/iNumberOfImages, audioTrans );
  }
  return true;
}

QStringList CrossFadeFilter::getFilterNames ( )
{
  QStringList list;
  list.append ( "Cross Fade" );
  return list;
}

/****************************************************************************
** class KenBurnsFilter

****************************************************************************/
KenBurnsFilter::KenBurnsFilter ( )
{
}

KenBurnsFilter::~KenBurnsFilter ( )
{
}

bool KenBurnsFilter::exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage * )
{
  return false;
}

bool KenBurnsFilter::exec ( QImage &origImage, CXmlSlideshow::effect_struct *pEffect, QImage &result, int iFrameNr, int iTotalFrames )
{
  if ( ( ! pEffect ) || ( pEffect->name != EFFECT_TYPE_KENBURNS ) || ( iTotalFrames < 1 ) || ( iFrameNr < 0 )  || ( iTotalFrames < iFrameNr ) ) {
    result = origImage.copy ( );
    return false;
  }
  // Here we apply the KenBurns effect to the original image.
  int x, y, w, h, x2, y2;
  x=0; y=0; w=origImage.width ( ); h=origImage.height ( );
  double fProgress = (double)iFrameNr / (double)iTotalFrames;

  // Calculate current upper right corner.
  x = pEffect->x0 + ( ( pEffect->xe0 - pEffect->x0 ) * fProgress );
  y = pEffect->y0 + ( ( pEffect->ye0 - pEffect->y0 ) * fProgress );

  // Calculate current lower right position
  x2 = pEffect->x1 + ( ( pEffect->xe1 - pEffect->x1 ) * fProgress );
  y2 = pEffect->y1 + ( ( pEffect->ye1 - pEffect->y1 ) * fProgress );

  w = x2 - x;
  h = y2 - y;

  if ( w >= 0 && w < 5 )
    w = 5;
  if ( h >= 0 && h < 5 )
    h = 5;

  bool bHMirror = false, bVMirror = false;
  if ( w < 0 )  {
    bVMirror = true;
    w  = x;
    x  = x2;
    x2 = w;
    w  = x2 - x;
  }
  if ( h < 0 )  {
    bHMirror = true;
    h  = y;
    y  = y2;
    y2 = h;
    h  = y2 - y;
  }
/*
printf ( "f<%d> tf<%d> xywh<%d, %d, %d, %d> start<%d, %d, %d, %d> stop<%d, %d, %d, %d> pogress<%f> origW<%d> origH<%d>\n", iFrameNr, iTotalFrames, x, y, w, h,
pEffect->x0,  pEffect->y0,  pEffect->x1,  pEffect->y1,
pEffect->xe0, pEffect->ye0, pEffect->xe1, pEffect->ye1,
fProgress, origImage.width ( ), origImage.height ( ) );
*/
  result = origImage.copy ( x, y, w, h );
  if ( bHMirror || bVMirror ) 
    result = result.mirrored ( bHMirror, bVMirror );

  return true;
}

QStringList KenBurnsFilter::getFilterNames ( )
{
  QStringList list;
  list.append ( "Ken Burns" );
  return list;
}

/****************************************************************************
** class AlphaFilter ( Move to separate file ??? )
****************************************************************************/
AlphaFilter::AlphaFilter ( )
{
  srand ( time ( NULL ) );
}

AlphaFilter::AlphaFilter ( QString qsFilter )
{
  m_qsFilterName = qsFilter;
  srand ( time ( NULL ) );
}

AlphaFilter::~AlphaFilter ( )
{
}

bool AlphaFilter::exec ( Encoder::FFmpeg *pEncoder, QImage &imgStart, QImage &imgStop, int iNumberOfImages, enAudioTrans audioTrans, QImage *pBackground )
{
  if ( m_bUseKenBurns )
    return execKB ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans, pBackground );

  // If no user selection, then we'll have to randomly get one ...
  QString qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  QString qsFilterName, qsBaseName, qsExt;

  if ( m_qsFilterName.size ( ) < 1 )  {
    QStringList listOfFilter = getFilterNames ( );
    if ( listOfFilter.count ( ) < 1 )  {  // Alpha filter are missing from the system. Default to XFade Filter
      CrossFadeFilter alternate;
      return alternate.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans, pBackground );
    }
    int iFilter = (int)( ( (float)rand ( ) / RAND_MAX ) * listOfFilter.count ( ) );
    qsBaseName  = listOfFilter[iFilter];
  }
  else
    qsBaseName = m_qsFilterName;

  qsExt = ".jpg";
  qsFilterName = qsTransitionPath + qsBaseName + qsExt;

  QFile  filter ( qsFilterName );
  if ( ! filter.exists ( ) )  {
    qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    qsExt = ".png"; // Luma directory contains only png's
    qsFilterName = qsTransitionPath + qsBaseName + qsExt;
  }
  filter.setFileName ( qsFilterName );
  if ( ! filter.exists ( ) )  {
    CrossFadeFilter alternate;
    return alternate.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans, pBackground );
  }

  QImage filterImage ( qsFilterName );
  filterImage = filterImage.scaled ( imgStart.size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
  filterImage = filterImage.convertToFormat ( QImage::Format_RGB32, Qt::AutoColor | Qt::DiffuseDither );

  // At this point the filter, start, and end image must have all the same size.
  if ( ( filterImage.size ( ) != imgStart.size ( ) ) ||
       ( filterImage.size ( ) != imgStop.size  ( ) ) )  {
    CrossFadeFilter xFilter;
    return xFilter.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans );
  }

  int    t;
  float  fDeltaA; //, fVolume;
  QImage result ( filterImage.size ( ), QImage::Format_RGB32 );

  // Each step the alpha value of each pixel is increased / decreased by this much
  //fDeltaA = 512.0f / ( iNumberOfImages - 1 );
  int  iDiff = 0;
  if ( pBackground )  {
    iDiff = 25; // this is the gap between start, and end image, which will be filled with the background
    if ( iNumberOfImages < iDiff * 2 )
         iDiff = (int)( (float)iNumberOfImages / 2.0f );
  }
  fDeltaA   = 512.0f / ( iNumberOfImages - iDiff - 1 );
  for ( t=0; t<iNumberOfImages; t++ )  {
    if ( pBackground )
      augmentAlpha ( (int)(fDeltaA * t), (int)(fDeltaA * (t-iDiff)), filterImage, imgStart, imgStop, result, pBackground );
    else
      augmentAlpha ( (int)(fDeltaA * t), filterImage, imgStart, imgStop, result );

    encode ( pEncoder, result, (float)t/iNumberOfImages, audioTrans );
  }
  return true; //false;
}

void AlphaFilter::augmentAlpha ( int iDeltaA, int iDeltaB, QImage &filter, QImage &start, QImage &stop, QImage &result, QImage *pBackground )
{
  int   r, g, b, x, y;
  float fVal1, fVal2, fVal3 = 0.0f;
  QRgb alpha, pix1, pix2, pix3;
  // First we copy the filter to start and end ...
  // Next we run through every pixel to increase / decrease the filters
  for  ( y=0; y<filter.height ( ); y++ )  {
    for ( x=0; x<filter.width ( ); x++ )  {
      alpha = filter.pixel ( x, y );
      pix1  = start.pixel  ( x, y );
      pix2  = stop.pixel   ( x, y );
      fVal1 = (float)( qRed ( alpha ) - 256 + iDeltaA ) / 255.0f; // represents the alpha value ...
      if ( fVal1 < 0.0f )
           fVal1 = 0.0f;
      if ( fVal1 > 1.0f )
           fVal1 = 1.0f;
      fVal2 = (float)( qRed ( alpha ) - 256 + iDeltaB ) / 255.0f; // represents the alpha value ...
      if ( fVal2 < 0.0f )
           fVal2 = 0.0f;
      if ( fVal2 > 1.0f )
           fVal2 = 1.0f;

      r = (int)( qRed   ( pix1 ) * ( 1.0f - fVal1 ) + qRed   ( pix2 ) * fVal2 );
      g = (int)( qGreen ( pix1 ) * ( 1.0f - fVal1 ) + qGreen ( pix2 ) * fVal2 );
      b = (int)( qBlue  ( pix1 ) * ( 1.0f - fVal1 ) + qBlue  ( pix2 ) * fVal2 );

      if ( pBackground )  {
        //fVal3 = 1.0f-fVal1 + fVal2;
        fVal3 = fVal2 - fVal1;
        if ( fVal3 < 0.0f )
             fVal3 = -fVal3;
        pix3  = pBackground->pixel ( x, y );
        r += (int)( qRed   ( pix3 ) * fVal3 );
        g += (int)( qGreen ( pix3 ) * fVal3 );
        b += (int)( qBlue  ( pix3 ) * fVal3 );
      }

      pix1 = qRgb     ( r, g, b );
      result.setPixel ( x, y, pix1 );
    }
  }
}

void AlphaFilter::augmentAlpha ( int iDeltaA, QImage &filter, QImage &start, QImage &stop, QImage &result )
{
  int   r, g, b, x, y;
  float fVal;
  QRgb  alpha, pix1, pix2;
  // First we copy the filter to start and end ...
  // Next we run through every pixel to increase / decrease the filters
  for  ( y=0; y<filter.height ( ); y++ )  {
    for ( x=0; x<filter.width ( ); x++ )  {
      alpha = filter.pixel ( x, y );
      pix1  = start.pixel  ( x, y );
      pix2  = stop.pixel   ( x, y );
      fVal = (float)( qRed ( alpha ) - 256 + iDeltaA ) / 255.0f; // represents the alpha value ...
      if ( fVal < 0.0f )
           fVal = 0.0f;
      if ( fVal > 1.0f )
           fVal = 1.0f;

      r = (int)( qRed   ( pix1 ) * ( 1.0f - fVal ) + qRed   ( pix2 ) * fVal );
      g = (int)( qGreen ( pix1 ) * ( 1.0f - fVal ) + qGreen ( pix2 ) * fVal );
      b = (int)( qBlue  ( pix1 ) * ( 1.0f - fVal ) + qBlue  ( pix2 ) * fVal );

      pix1 = qRgb     ( r, g, b );
      result.setPixel ( x, y, pix1 );
    }
  }
}

bool AlphaFilter::execKB ( Encoder::FFmpeg *pEncoder, QImage &imgStart, QImage &imgStop, int iNumberOfImages, enAudioTrans audioTrans, QImage *pBackground )
{
  // If no user selection, then we'll have to randomly get one ...
  QString qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  QString qsFilterName, qsBaseName, qsExt;

  if ( m_qsFilterName.size ( ) < 1 )  {
    QStringList listOfFilter = getFilterNames ( );
    if ( listOfFilter.count ( ) < 1 )  {  // Alpha filter are missing from the system. Default to XFade Filter
      CrossFadeFilter alternate;
      return alternate.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans, pBackground );
    }
    int iFilter = (int)( ( (float)rand ( ) / RAND_MAX ) * listOfFilter.count ( ) );
    qsBaseName  = listOfFilter[iFilter];
  }
  else
    qsBaseName = m_qsFilterName;

  qsExt = ".jpg";
  qsFilterName = qsTransitionPath + qsBaseName + qsExt;

  QFile  filter ( qsFilterName );
  if ( ! filter.exists ( ) )  {
    qsTransitionPath =  + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    qsExt = ".png"; // Luma directory contains only png's
    qsFilterName = qsTransitionPath + qsBaseName + qsExt;
  }
  filter.setFileName ( qsFilterName );
  if ( ! filter.exists ( ) )  {
    CrossFadeFilter alternate;
    return alternate.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans, pBackground );
  }
  QImage filterImage ( qsFilterName );
  filterImage = filterImage.scaled ( imgStart.size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
  filterImage = filterImage.convertToFormat ( QImage::Format_RGB32, Qt::AutoColor | Qt::DiffuseDither );

  // At this point the filter, start, and end image must have all the same size.
  if ( ( filterImage.size ( ) != imgStart.size ( ) ) ||
       ( filterImage.size ( ) != imgStop.size  ( ) ) )  {
    CrossFadeFilter xFilter;
    return xFilter.exec ( pEncoder, imgStart, imgStop, iNumberOfImages, audioTrans );
  }

  // Both start and end image have to set the KenBurns properly.
  Utils theUtils;
  int iStartImage[2], iEndImage[2], iTotalImages[2];
  // Lets start with Start
  iStartImage[0]  = 0; // unused
  iEndImage[0]    = m_iTotalImages[0] - iNumberOfImages;
  iTotalImages[0] = m_iTotalImages[0];
  // and continue with Stop
  iStartImage[1]  = 0; // unused
  iEndImage[1]    = m_iTotalImages[1] - iNumberOfImages;
  iTotalImages[1] = m_iTotalImages[1];

  int    t;
  float  fDeltaA;
  QImage result ( filterImage.size ( ), QImage::Format_RGB32 );
  QImage imgStart1, imgStop1, *pImgStart, *pImgStop;
  KenBurnsFilter kb;

  QString  qsFileName = m_kenBurns[0].src;
  QImage   origImage1  ( qsFileName );
  qsFileName = m_kenBurns[1].src;
  QImage   origImage2  ( qsFileName );

  // Each step the alpha value of each pixel is increased / decreased by this much
  int  iDiff = 0;
  if ( pBackground )  {
    iDiff = 25; // this is the gap between start, and end image, which will be filled with the background
    if ( iNumberOfImages < iDiff * 2 )
         iDiff = (int)( (float)iNumberOfImages / 2.0f );
  }
  fDeltaA   = 512.0f / ( iNumberOfImages - iDiff - 1 );

  bool bStartKenBurns = ( m_kenBurns[0].effect[0].name == EFFECT_TYPE_KENBURNS );
  bool bStopKenBurns  = ( m_kenBurns[1].effect[0].name == EFFECT_TYPE_KENBURNS );

  if ( m_kenBurns[0].pMatrix )
       origImage1 = origImage1.transformed ( *m_kenBurns[0].pMatrix, Qt::SmoothTransformation );
  if ( m_kenBurns[1].pMatrix )
       origImage2 = origImage2.transformed ( *m_kenBurns[1].pMatrix, Qt::SmoothTransformation );

  for ( t=0; t<iNumberOfImages; t++ )  {
    pImgStart = &imgStart;
    pImgStop  = &imgStop;
    if ( ( m_iTotalImages[0] != 0 ) && ( bStartKenBurns ) )  { // m_kenBurns based on original img size ...
      kb.exec ( origImage1, &m_kenBurns[0].effect[0], imgStart1, iEndImage[0] + t, iTotalImages[0] );
      theUtils.prescaleImage ( imgStart1, m_iAspectRatio );
      theUtils.frameImage ( imgStart1, filterImage.width ( ), filterImage.height ( ), &m_background, NULL );
      pImgStart = &imgStart1;
    }
    if ( ( m_iTotalImages[1] != 0 ) && ( bStopKenBurns ) )  {
      kb.exec ( origImage2, &m_kenBurns[1].effect[0], imgStop1, iStartImage[1] + t, iTotalImages[1] );
      theUtils.prescaleImage ( imgStop1, m_iAspectRatio );
      theUtils.frameImage ( imgStop1, filterImage.width ( ), filterImage.height ( ), &m_background, NULL );
      pImgStop = &imgStop1;
    }
    if ( pBackground )
      augmentAlpha ( (int)(fDeltaA * t), (int)(fDeltaA * (t-iDiff)), filterImage, *pImgStart, *pImgStop, result, pBackground );
    else
      augmentAlpha ( (int)(fDeltaA * t), filterImage, *pImgStart, *pImgStop, result );

    encode ( pEncoder, result, (float)t/iNumberOfImages, audioTrans );
  }
  return true;
}

QStringList AlphaFilter::getFilterNames ( )
{
  QStringList list;
  QString qsTransitionPath = + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  for ( int t=0; t<2; t++ )  {
    QDir theDir ( qsTransitionPath );
    if ( theDir.exists  ( ) )  {
         theDir.setFilter (  QDir::Files | QDir::NoSymLinks );
         const QFileInfoList  fileList    = theDir.entryInfoList ( );
         QFileInfoList::const_iterator it = fileList.begin ( );
         //while ( ( fi = it.current ( ) ) != 0 ) {
         while ( it != fileList.end ( ) )
           list.append ( (*it++).baseName ( ) );
    }
    qsTransitionPath = + SYSTEM_DIRECTORY + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
  }
  return list;
}

}; // end of namespace Render

