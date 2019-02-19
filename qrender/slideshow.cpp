/***************************************************************************
    slideshow.cpp
                             -------------------
    Class Slideshow implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class takes care of actually rendering the slideshow.
    It will take the inut data collected in XmlSlideshow
    and use ffmpeg to generate the slideshow accordingly.
    
****************************************************************************/

#include <QColor>
#include <QPainter>
#include <QFileInfo>
#include <QPixmap>  //xxx

#include "utils.h"
#include "logger.h"
#include "parser.h"
#include "render.h"
#include "filter.h"
#include "slideshow.h"

namespace Render
{

Slideshow::Slideshow ( Render *pParent )
{  
  m_pParser = NULL;
  m_pParent = pParent;
  m_size    = QSize ( 720, 480 ); // NTSC
}

Slideshow::Slideshow ( Parser *pParser )
{   
  m_pParent = NULL;
  m_pParser = pParser;
  m_size    = QSize ( 720, 480 ); // NTSC
}

Slideshow::~Slideshow ( )
{
}

void Slideshow::initMe ( CXmlSlideshow *pSlideshow, QString &qsTempPath )
{
  m_pSlideshow = pSlideshow;
  m_qsTempPath = qsTempPath;
  QString qsFileName = pSlideshow->slideshow_name;
  qsFileName.replace ( "/", "_" );
  m_qsFileName = m_qsTempPath + qsFileName + ".vob";
}

void Slideshow::setFileName ( QString &qsFileName )
{
  if ( qsFileName.isEmpty ( ) )  {
    QString qsFile = m_pSlideshow->slideshow_name;
    qsFile.replace ( "/", "_" );
    m_qsFileName = m_qsTempPath + qsFile + ".vob";
  }
  else
    m_qsFileName = qsFileName;
}

void Slideshow::displayProgress ( float fProgress )
{   
  if ( m_pParent )  {
    m_pParent->sendProgress ( fProgress );
    if ( m_pParent->logger ( ).logLevel ( ) <= Logger::logInformation )
         m_pParent->logger ( ).iLOG ( "Progress : %f\n", fProgress );
  }
  else {
    if ( m_pParser && m_pParser->getLogLevel ( ) <= Logger::logInformation )
      printf ( "Progress : %f\n", fProgress );
    else
      printf ( "." );
    if ( fProgress >= 100.0f )
      printf ( "\n" );
    fflush ( stdout );
  }
}

void Slideshow::log ( QString qsLogMessage, int iLogLevel )
{
  if ( m_pParent )  {
    if ( m_pParent->logger ( ).logLevel ( ) <= iLogLevel )
         m_pParent->logger ( ).LOG ( (Logger::type)iLogLevel, (const char *)qsLogMessage.toUtf8 ( ) );
  }
  else {
    if ( m_pParser && m_pParser->getLogLevel ( ) <= iLogLevel )
      printf ( "%s", (const char *)qsLogMessage.toUtf8 ( ) );
  }
}

QString Slideshow::getHashName ( QString qsInput )
{
  if ( m_pParent )
    return m_pParent->getHashName ( qsInput );
  return qsInput;
}

float Slideshow::getFilterDuration ( CXmlSlideshow::img_struct *pXmlImage )
{
  if ( ! pXmlImage )
    return (float)m_pSlideshow->filter_delay;

  float fDuration  = 0.0f;
  if  ( pXmlImage->pTransition ) // transitions duration will overwrite global slideshow filter duration
        fDuration  = (float)pXmlImage->pTransition->fDuration;
  if  ( fDuration <= 0.0f )
        fDuration  = (float)m_pSlideshow->filter_delay;
  if  ( fDuration <= 0.0f )
        fDuration  = 3.0f;
  fDuration *= 1000.0f; // in mSeconds please.
  return fDuration;
}

float Slideshow::getDuration ( CXmlSlideshow::img_struct *pXmlImage )
{
  if ( ! pXmlImage )
    return (float)m_pSlideshow->delay;

  float fDuration = pXmlImage->fDuration;
  if (  fDuration < 0.0f )
        fDuration = (float)m_pSlideshow->delay;
  if (  fDuration < 0.0f )
        fDuration = 5.0f;
  fDuration *= 1000.0f; // in mSeconds please.
  return fDuration;
}

void Slideshow::prescaleImage ( QImage &theImage )
{
  float fAspect = 720.0/480.0 / ( 16.0/9.0 );
  if  ( m_pSlideshow && ( m_pSlideshow->aspect == 0 ) )
        fAspect = 720.0/480.0 / (  4.0/3.0 );

  // Current only other aspect ratio is 16:9.
  // Since the DVD spec uses the same resolution for both ratios
  // the 16:9 images are being distorted.
  // To counter this effect we will have to pre-scale the image to
  // eliminate this effect
  // For details check: http://lipas.uwasa.fi/~f76998/video/conversion/
  int   iWidth  = (int)( fAspect * theImage.width ( ) );
  int   iHeight = theImage.height ( );
  theImage      = theImage.scaled ( iWidth, iHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

// Main function to generate the slideshow.
// This function will iterate through the images and videos and generate the appropriate
// transitions, and KenBurns effects ( images only )
bool Slideshow::exec ( )
{   
  if ( ! m_pSlideshow )
    return false;

  // The slideshow has to start with something
  int iX = m_pSlideshow->xres;
  int iY = m_pSlideshow->yres;
  if ( iX < 10 )
       iX = 720;
  if ( iY < 0 )
       iY = 480;
  m_size = QSize ( iX, iY );

  double fFPS = 29.97;
  Encoder::Encoder::enVideo enFormat = Encoder::Encoder::vfNTSC;
  if ( ( iX == 720) && ( iY == 576 ) )
    enFormat = Encoder::Encoder::vfPAL;
  else if ( ( iX == 704 ) && ( iY == 576 ) )
    enFormat = Encoder::Encoder::vfPAL;
  else if ( ( iX == 352 ) && ( iY == 576 ) )
    enFormat = Encoder::Encoder::vfPAL;
  else if ( ( iX == 352 ) && ( iY == 288 ) )
    enFormat = Encoder::Encoder::vfPAL;
  if ( enFormat == Encoder::Encoder::vfPAL )
    fFPS = 25.0;

  displayProgress ( 0.0001f );
  
  CXmlSlideshow::img_struct  *pXmlStart = createStartEnd (  true );
  CXmlSlideshow::img_struct  *pXmlEnd   = createStartEnd ( false );
  CXmlSlideshow::img_struct  *pXmlCurrent, *pXmlPrevious;
  CXmlSlideshow::time_object *pTimeObject;

  QString qsLog = QString ( "Starting slideshow %1\n" ).arg ( m_pSlideshow->slideshow_name );
  log ( qsLog, Logger::logInformation );
  createBackground ( );

  Encoder::FFmpeg theEncoder;
  theEncoder.setAudioList    ( m_pSlideshow->audio_list );
  if ( m_pSlideshow->aspect == 1 ) // 16:9
    theEncoder.setVideoParam ( 6000, Encoder::Encoder::arAspect16to9 );
  theEncoder.initStream      ( m_qsFileName, enFormat, Encoder::Encoder::afAC3, 1000 );
  
  pXmlPrevious    = pXmlStart;
  uint t, iCount  = m_pSlideshow->count ( );
  float fProgress = 0.0f;
  float fCount2   = 2.0f * iCount + 2.0f;
  int iProgress   = 0;
  int iStart      = 0;

  if ( ! m_pSlideshow->intro_page )  {
    // The user chose to skip the intro page with the slideshow title
    iStart        = 1;
    pXmlPrevious  = (CXmlSlideshow::img_struct *)m_pSlideshow->getTimeObject ( 0 );
    fCount2      -= 2.0f;
  }
  if ( ! m_pSlideshow->fade_out )
    fCount2      -= 1.0f;

  for  ( t=iStart; t<iCount; t++ )  {
    if ( m_pParent && m_pParent->killClient ( ) )  {
      // The user terminated this slideshow generation
      theEncoder.endStream ( );
      delete pXmlStart;
      delete pXmlEnd;
      return false;
    }
    if ( t<iCount )
      pTimeObject = m_pSlideshow->getTimeObject ( t );
    else
      pTimeObject = (CXmlSlideshow::time_object *)pXmlEnd;
    // The frame is to be displayed for a bit ...
    pXmlCurrent = (CXmlSlideshow::img_struct *)pTimeObject;

    if ( pXmlPrevious->node_name == "img" )
      createFromImage ( pXmlPrevious, &theEncoder, fFPS );
    else if ( pXmlPrevious->node_name == "vid" )
      createFromVid   ( (CXmlSlideshow::vid_struct *)pXmlPrevious, &theEncoder, fFPS );
    else
      continue;

    pXmlCurrent = (CXmlSlideshow::img_struct *)pTimeObject;
    //fProgress = 100.0f * ( ((float)++iProgress) / fCount2 );  //ooo
    fProgress = 100.0f * ( static_cast<float>(++iProgress) / fCount2 ); //xxx
    displayProgress ( fProgress );
    if ( ( pXmlPrevious->node_name == "img" ) && 
         ( pXmlCurrent ->node_name == "img" ) )
      createIITransition ( pXmlPrevious, pXmlCurrent, &theEncoder, fFPS );
    else if ( ( pXmlPrevious->node_name == "vid" ) && 
              ( pXmlCurrent ->node_name == "img" ) )
      createVITransition ( (CXmlSlideshow::vid_struct *)pXmlPrevious, pXmlCurrent, &theEncoder, fFPS );
    else if ( ( pXmlPrevious->node_name == "img" ) && 
              ( pXmlCurrent ->node_name == "vid" ) )
      createIVTransition ( pXmlPrevious, (CXmlSlideshow::vid_struct *)pXmlCurrent, &theEncoder, fFPS );
    else if ( ( pXmlPrevious->node_name == "vid" ) && 
              ( pXmlCurrent ->node_name == "vid" ) )
      createVVTransition ( (CXmlSlideshow::vid_struct *)pXmlPrevious, (CXmlSlideshow::vid_struct *)pXmlCurrent, &theEncoder, fFPS );

    //fProgress = 100.0f * ( (float)++iProgress / fCount2 );    //ooo
    fProgress = 100.0f * ( static_cast<float>(++iProgress) / fCount2 );  //xxx
    displayProgress ( fProgress );

    pXmlPrevious = pXmlCurrent;
  }
  
  // The frame is to be displayed for a bit ...
  if ( pXmlPrevious->node_name == "img" )
    createFromImage ( pXmlPrevious, &theEncoder, fFPS );
  else if ( pXmlPrevious->node_name == "vid" )
    createFromVid   ( (CXmlSlideshow::vid_struct *)pXmlPrevious, &theEncoder, fFPS );

  if ( m_pSlideshow->fade_out )  {
    fProgress = 100.0f * ( (float)++iProgress / fCount2 );
    displayProgress ( fProgress );
    if ( pXmlPrevious->node_name == "img" )
      createIITransition ( pXmlPrevious, pXmlEnd, &theEncoder, fFPS );
    else
      createVITransition ( (CXmlSlideshow::vid_struct *)pXmlPrevious, pXmlEnd, &theEncoder, fFPS );
  }
  
  displayProgress ( 100.0f );

  theEncoder.endStream  ( );
  
  delete pXmlStart;
  delete pXmlEnd;
  
  return true;
}

void Slideshow::createBackground ( )
{
  log ( "Creating Background\n", Logger::logInformation );

  m_background = QImage ( m_size, QImage::Format_ARGB32_Premultiplied );
  QString qsBackground = m_pSlideshow->background;
  bool bTryImageBackground = true;
  if ( ! qsBackground.isEmpty ( ) )  {
    if ( qsBackground[0] != '/' ) {
      QColor theColor ( qsBackground );
      if ( theColor.isValid ( ) )  {
        m_background.fill   ( theColor.rgba ( ) );
        bTryImageBackground = false;
      }
    }
    if ( bTryImageBackground )  {
      QFileInfo fileInfo ( qsBackground );
      if ( fileInfo.exists ( ) )  {
        QPainter thePainter;
        QImage img ( qsBackground );
        img  = img.scaled ( m_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        thePainter.begin  ( &m_background );
        thePainter.drawImage ( 0, 0, img, 0, 0 );
        thePainter.end ( );
      }
      else
        m_background.fill ( 0xFF000000 );
    }
  }
  else
    m_background.fill ( 0xFF000000 );
}

CXmlSlideshow::img_struct *Slideshow::createStartEnd ( bool bStart )
{  
  if ( bStart )
    log ( "Creating intro page\n", Logger::logInformation );
  else
    log ( "Creating extro page\n", Logger::logInformation );
  
  // This frunction creates the first frame and the last frame.
  int iX = m_size.width  ( );
  int iY = m_size.height ( );

  QFont theFont       ( "Helvetica [Cronyx]", 24, QFont::Bold );
  QImage canvas       (    m_size, QImage::Format_ARGB32_Premultiplied ); //ooo
  
  QPainter thePainter (   &canvas );
  thePainter.setPen   ( QColor ( "#AFAF15" ) );  // Yellow
  thePainter.setBrush ( QColor ( "#000000" ) );  // Qt::blue );
  thePainter.drawRect ( 0,0,iX,iY );
  thePainter.setFont  (   theFont );

  QString qsFileName = m_pSlideshow->slideshow_name;
  
  qsFileName.replace ( "/", "_" );
  
  if ( bStart )  {
    //thePainter.drawText ( (int)( iX / 3.0 ), (int)( iY / 2.0 - 10 ), m_pSlideshow->slideshow_name );  //oooo
    //thePainter.drawText ( static_cast<int>( iX / 3.0 ), static_cast<int>( iY / 2.0 - 10 ), m_pSlideshow->slideshow_name );  //xxx
    //thePainter.drawText ( static_cast<int>( iX / 3.0 ), static_cast<int>( iY / 2.0 - 10 ), m_pSlideshow->slideshow_name );  //xxx
    //thePainter.drawLine ( 0, 0, iX , iY );  //xxx
    //thePainter.drawPoint( static_cast<int>( iX / 3.0 ), static_cast<int>( iY / 2.0 - 10 ) );    //xxx
    qsFileName = m_qsTempPath + qsFileName + "-start.jpg";
  }
  else
    qsFileName = m_qsTempPath + qsFileName + "-end.jpg";

  // Note this is the same algol as in Render::fileFromSocket
  QString qsNewFileName = getHashName ( qsFileName );
  // Store the hash file name so we can access
  canvas.save ( qsNewFileName, "JPEG", 100 );

  CXmlSlideshow::img_struct *pXml = new CXmlSlideshow::img_struct;
  pXml->src    = qsFileName;
  pXml->width  = iX;
  pXml->height = iY;
  if ( m_pSlideshow && m_pSlideshow->kenBurns ( ) )
    pXml->effect[0].randomKenBurns ( 0.5f, iX, iY, true );
  
  return pXml;
}

void Slideshow::createFromImage ( CXmlSlideshow::img_struct *pXmlImage, Encoder::FFmpeg *pEncoder, double fFPS )
{
  if ( ! pXmlImage )
    return;

  bool   bKenBurns = ( pXmlImage->effect[0].name == EFFECT_TYPE_KENBURNS );
  if ( ( bKenBurns ) && ( m_pSlideshow->kenBurns ( ) ) )
    return createKBFromImage ( pXmlImage, pEncoder, fFPS );

  QFileInfo fileInfo ( pXmlImage->src );
  QString qsLog = QString ( "Creating from Image %1\n" ).arg ( fileInfo.fileName ( ) );
  log ( qsLog, Logger::logInformation );

  Utils theUtils;
  QString  qsFileName = getHashName ( pXmlImage->src );
  QImage   imgStart ( qsFileName );

  if ( pXmlImage->pMatrix )
       imgStart = imgStart.transformed ( *pXmlImage->pMatrix, Qt::SmoothTransformation );
  theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );
  float fDuration = getDuration ( pXmlImage );

  // And at the end we will add this to the VOB compliant video TS
  int iNumberOfImages = (int)( fFPS * fDuration / 1000.0f );
  pEncoder->addImage ( &imgStart, iNumberOfImages );
}

void Slideshow::createKBFromImage ( CXmlSlideshow::img_struct *pXmlImage, Encoder::FFmpeg *pEncoder, double fFPS )
{
  // Called to create the KenBurns effect. The start frame is 0, the end frame is
  if ( ! pXmlImage )
    return;

  int iFrame;
  fFPS = fFPS / 1000.0f;

  // Note: a bit more complex as we have to account for the duration of the filter before and after
  int iStartFrame  = (int)(   getFilterDuration ( pXmlImage ) * fFPS ); // Assumes previous filter length is the same as current filter length
  int iEndFrame    = (int)(   getDuration ( pXmlImage ) * fFPS ) + iStartFrame;
  int iTotalFrames = (int)( ( getDuration ( pXmlImage ) + getFilterDuration ( pXmlImage ) * 2.0 ) * fFPS ); // The effect totals both start and end filter length

  if ( ( pXmlImage == m_pSlideshow->getTimeObject ( 0 ) ) && ! m_pSlideshow->intro_page )
    iStartFrame = 0;

  QFileInfo fileInfo ( pXmlImage->src );
  QString qsLog = QString ( "Creating KenBurns from Image %1\n" ).arg ( fileInfo.fileName ( ) );
  log ( qsLog, Logger::logInformation );

  KenBurnsFilter filter;
  QString qsFileName = getHashName ( pXmlImage->src );
  Utils   theUtils;
  QImage  origImage ( qsFileName );
  if ( pXmlImage->pMatrix )
       origImage = origImage.transformed ( *pXmlImage->pMatrix, Qt::SmoothTransformation );

  // Make sure we have some kenBurns data avail
  pXmlImage->effect[0].randomKenBurns ( 0.5f, origImage.width ( ), origImage.height ( ), false );
  for ( iFrame=iStartFrame; iFrame<iEndFrame; iFrame++ )  {
    QImage imgStart;
    filter.exec ( origImage, &pXmlImage->effect[0], imgStart, iFrame, iTotalFrames );

    theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
    theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );
    // And at the end we will add this to the VOB compliant video TS
    pEncoder->addImage ( &imgStart, 1 );

    if ( m_pParent && m_pParent->killClient ( ) )
      return;
  }
}

void Slideshow::createFromVid ( CXmlSlideshow::vid_struct *pXmlVideo, Encoder::FFmpeg *pEncoder, double ) // fFPS )
{
  if ( ! pXmlVideo )
    return;

  QFileInfo fileInfo ( pXmlVideo->src );
  QString qsLog = QString ( "Creating from vid %1\n" ).arg ( fileInfo.fileName ( ) );
  log ( qsLog, Logger::logInformation );

  QString qsFileName = getHashName ( pXmlVideo->src );

  unsigned int iColor = 0x000000;
  QColor theColor ( m_pSlideshow->background );
  if ( theColor.isValid  ( ) )
    iColor = theColor.rgba ( );

  // I do not think we'll be able to support Matrix operations. ( pXmlVideo->pMatrix )
  pEncoder->addVid ( qsFileName, m_size.width ( ), m_size.height ( ), m_pSlideshow->aspect, (int)iColor, pXmlVideo->rotate );
}

// Image to Image Transition
void Slideshow::createIITransition ( CXmlSlideshow::img_struct *pStart, CXmlSlideshow::img_struct *pStop, Encoder::FFmpeg *pEncoder, double fFPS )
{
  if ( ! pStart || ! pStop )
    return;

  log ( "Creating II Transition\n", Logger::logInformation );
  QString qsFileName = getHashName ( pStart->src );
  Utils   theUtils;
  QImage  imgStart ( qsFileName );

  // Generate random KenBurns but only if not already set.
  pStart->effect[0].randomKenBurns ( 0.5f, imgStart.width ( ), imgStart.height ( ), false );
  if ( pStart->pMatrix )
       imgStart = imgStart.transformed ( *pStart->pMatrix, Qt::SmoothTransformation );
  theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  qsFileName = getHashName ( pStop->src );
  QImage imgStop ( qsFileName );

  // Generate random KenBurns but only if not already set.
  pStop->effect[0].randomKenBurns ( 0.5f, imgStop.width ( ), imgStop.height ( ), false );
  if ( pStop->pMatrix )
       imgStop  = imgStop.transformed ( *pStop->pMatrix, Qt::SmoothTransformation );
  theUtils.prescaleImage ( imgStop, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStop, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  fFPS /= 1000.0;
  float fDuration     = getFilterDuration ( pStop );
  int iNumberOfImages = (int)( fFPS * fDuration );
  if ( m_pParent && m_pParent->killClient ( ) )
    return;

  Filter *pFilter = Filter::create ( m_pSlideshow, pStop->pTransition );
  bool bStartKenBurns = ( pStart->effect[0].name == EFFECT_TYPE_KENBURNS );
  bool bStopKenBurns  = ( pStop ->effect[0].name == EFFECT_TYPE_KENBURNS );
  if ( ( ( bStartKenBurns ) || ( bStopKenBurns ) ) && ( m_pSlideshow->kenBurns ( ) ) )  {
    int iTotal1 = (int)( ( getDuration ( pStart ) + getFilterDuration ( pStart ) * 2.0 ) * fFPS );
    int iTotal2 = (int)( ( getDuration ( pStop  ) + getFilterDuration ( pStop  ) * 2.0 ) * fFPS );
    int iAspect = m_pSlideshow ? m_pSlideshow->aspect : 0;
    pFilter->setKenBurnsParams ( pStart, pStop, m_background, iAspect, iTotal1, iTotal2 );
  }

  if ( pFilter  )  {
    if ( m_pSlideshow->imgBkgImg ( ) )
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull, &m_background );
    else
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull );
    delete pFilter;
  }
}

// Image to Vid transition
void Slideshow::createIVTransition ( CXmlSlideshow::img_struct *pStart, CXmlSlideshow::vid_struct *pStop, Encoder::FFmpeg *pEncoder, double fFPS )
{
  if ( ! pStart || ! pStop )
    return;

  log ( "Creating IV Transition\n", Logger::logInformation );

  // Here we have to 
  // o Get the first frame of the Video
  // o turn down the background music during the duration of the transition
  // o Transition over
  QPainter thePainter;
  QString  qsFileName = getHashName ( pStart->src );
  QImage   imgStart ( qsFileName );
  Utils    theUtils;

  // Generate random KenBurns but only if not already set.
  pStart->effect[0].randomKenBurns ( 0.5f, imgStart.width ( ), imgStart.height ( ), false );
  if ( pStart->pMatrix )
       imgStart = imgStart.transformed ( *pStart->pMatrix, Qt::SmoothTransformation );
  theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
  imgStart = imgStart.scaled ( m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation );
  theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  qsFileName = getHashName ( pStop->src );
  QImage imgStop;
  if ( pEncoder->initVid   ( qsFileName ) )
     imgStop = pEncoder->getVidFrame( 0.0 );
  if ( imgStop.isNull ( ) )
     imgStop = imgStart;

  // We need a image with transparency ...
  imgStop = imgStop.convertToFormat ( QImage::Format_ARGB32 );//_Premultiplied );
  if ( pStop->pMatrix )
       imgStop  = imgStop.transformed  ( *pStop->pMatrix, Qt::SmoothTransformation );
  else if ( pStop->rotate )  {
    QMatrix matrix;
    matrix.rotate (  pStop->rotate );
    imgStop  = imgStop.transformed  ( matrix, Qt::SmoothTransformation ); 
  }

  theUtils.prescaleImage ( imgStop, m_pSlideshow->aspect );
  imgStop = imgStop.scaled ( m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation );
  theUtils.frameImage ( imgStop, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  fFPS /= 1000.0f;
  float fDuration     = getFilterDuration ( pStop );
  int iNumberOfImages = (int)( fFPS * fDuration );
  if ( m_pParent && m_pParent->killClient ( ) )
    return;

  Filter *pFilter  = Filter::create ( m_pSlideshow, pStop->pTransition );
  bool   bKenBurns = (  pStart->effect[0].name == EFFECT_TYPE_KENBURNS );
  if ( ( bKenBurns ) && ( m_pSlideshow->kenBurns ( ) ) )  {
    int iTotal1 = (int)( ( getDuration ( pStart ) + getFilterDuration ( pStart ) * 2.0 ) * fFPS );
    int iAspect = m_pSlideshow ? m_pSlideshow->aspect : 0;
    pFilter->setKenBurnsParams ( pStart, NULL, m_background, iAspect, iTotal1, 0 );
  }

  if ( pFilter  )  {
    if ( m_pSlideshow->imgBkgImg ( ) )
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull, &m_background );
    else
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull );
    delete pFilter;
  }
}

// Video To Image transition
void Slideshow::createVITransition ( CXmlSlideshow::vid_struct *pStart, CXmlSlideshow::img_struct *pStop, Encoder::FFmpeg *pEncoder, double fFPS )
{
  if ( ! pStart || ! pStop )
    return;

  log ( "Creating VI Transition\n", Logger::logInformation );

  // Here we have to 
  // o Get the last frame of the Video
  // o turn up the background music during the duration of the transition
  // o Transition over
  QString qsFileName = getHashName ( pStart->src );
  Utils   theUtils;
  QImage  imgStart;

  if ( pEncoder->initVid   ( qsFileName ) )
       imgStart = pEncoder->getVidFrame ( -1.0 );
  // In case something went bad ...
  if ( imgStart.isNull ( ) )
       imgStart = QImage ( qsFileName );
  if ( imgStart.isNull ( ) )
       imgStart = QImage ( qsFileName );

  // Don't forget to close the current Vid stream in the encoder.
  pEncoder->closeVid ( );

  if ( imgStart.isNull ( ) )
    return;

  if ( pStart->pMatrix )
       imgStart = imgStart.transformed ( *pStart->pMatrix, Qt::SmoothTransformation );
  else if ( pStart->rotate )  {
    QMatrix matrix;
    matrix.rotate ( pStart->rotate );
    imgStart  = imgStart.transformed  ( matrix, Qt::SmoothTransformation );
  }

  theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  qsFileName = getHashName ( pStop->src );
  QImage imgStop ( qsFileName );
  if ( imgStop.isNull ( ) )
       imgStop = imgStart;

  // Generate random KenBurns but only if not already set.
  pStop->effect[0].randomKenBurns ( 0.5f, imgStop.width ( ), imgStop.height ( ), false );
  if ( pStop->pMatrix )
       imgStop  = imgStop.transformed ( *pStop->pMatrix, Qt::SmoothTransformation );
  theUtils.prescaleImage ( imgStop, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStop, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  fFPS /= 1000.0f;
  float fDuration     = getFilterDuration ( pStop );
  int iNumberOfImages = (int)( fFPS * fDuration );
  if ( m_pParent && m_pParent->killClient ( ) )
    return;

  Filter *pFilter  = Filter::create ( m_pSlideshow, pStop->pTransition );
  bool   bKenBurns =   ( pStop->effect[0].name == EFFECT_TYPE_KENBURNS );
  if ( ( bKenBurns )&& ( m_pSlideshow->kenBurns ( ) ) )  {
    int iTotal2 = (int)( ( getDuration ( pStop  ) + getFilterDuration ( pStop  ) * 2.0 ) * fFPS );
    int iAspect = m_pSlideshow ? m_pSlideshow->aspect : 0;
    pFilter->setKenBurnsParams ( NULL, pStop, m_background, iAspect,  0, iTotal2 );
  }

  if ( pFilter  )  {
    if ( m_pSlideshow->imgBkgImg ( ) )
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull, &m_background );
    else
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull );
    delete pFilter;
  }
}

// Vid to Vid transition
void Slideshow::createVVTransition ( CXmlSlideshow::vid_struct *pStart, CXmlSlideshow::vid_struct *pStop, Encoder::FFmpeg *pEncoder, double fFPS )
{
  if ( ! pStart || ! pStop )
    return;

  log ( "Creating VV Transition\n", Logger::logInformation );

  // Here we have to 
  // o Get the last frame of the Video
  // o turn up the background music during the duration of the transition
  // o Transition over
  QString qsFileName = getHashName ( pStart->src );
  Utils   theUtils;
  QImage  imgStart;
  // In case something went bad ...
  if ( pEncoder->initVid ( qsFileName ) )
       imgStart = pEncoder->getVidFrame ( -1.0 );

  if ( imgStart.isNull ( ) )
       imgStart = QImage ( qsFileName );

  // Don't forget to close the current Vid stream in the encoder.
  pEncoder->closeVid ( );

  if ( imgStart.isNull ( ) )
    return;

  if ( pStart->pMatrix )
       imgStart = imgStart.transformed ( *pStart->pMatrix, Qt::SmoothTransformation );
  else if ( pStart->rotate )  {
    QMatrix matrix;
    matrix.rotate ( pStart->rotate );
    imgStart = imgStart.transformed  ( matrix, Qt::SmoothTransformation );
  }

  theUtils.prescaleImage ( imgStart, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStart, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  qsFileName = getHashName ( pStop->src );
  QImage imgStop;
  if ( pEncoder->initVid   ( qsFileName ) )
       imgStop = pEncoder->getVidFrame ( 0.0 );
  if ( imgStop.isNull ( ) )
       imgStop = imgStart;
  else if ( pStop->rotate )  {
    QMatrix matrix;
    matrix.rotate ( pStop->rotate );
    imgStop = imgStop.transformed  ( matrix, Qt::SmoothTransformation );
  }

  // We need a image with transparency ...
  imgStop = imgStop.convertToFormat ( QImage::Format_ARGB32 );//_Premultiplied );
  theUtils.prescaleImage ( imgStop, m_pSlideshow->aspect );
  theUtils.frameImage ( imgStop, m_size.width ( ), m_size.height ( ), &m_background, NULL );

  float fDuration       = getFilterDuration ( pStop );
  int   iNumberOfImages = (int)( fFPS * fDuration / 1000.0f );
  if  ( m_pParent && m_pParent->killClient ( ) )
    return;

  Filter *pFilter = Filter::create ( m_pSlideshow, pStop->pTransition );
  if ( pFilter  )  {
    if ( m_pSlideshow->imgBkgImg ( ) )
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull, &m_background );
    else
       pFilter->exec ( pEncoder, imgStart, imgStop, iNumberOfImages, Filter::VolumeFull );
    delete pFilter;
  }
}

}; // end of namespace Render

