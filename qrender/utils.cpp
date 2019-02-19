/***************************************************************************
    utils.cpp
                             -------------------
    Class Utils implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Here we have some misc functions which are needed by a few classes 
    but can not really be assigned to any of those classes.
    
****************************************************************************/

#include <QDir>
#include <QImage>
#include <QPainter>

#include "utils.h"

Utils::Utils()
{

}

Utils::~Utils ()
{

}

void Utils::frameImage ( QImage &theImage, int iWidth, int iHeight, QImage *pBackground, unsigned int *pBGColor )
{
  QPainter thePainter;
  QImage canvas ( iWidth, iHeight, QImage::Format_ARGB32_Premultiplied );
  if ( pBackground )
    canvas = *pBackground;
  else if ( pBGColor )
    canvas.fill ( *pBGColor );

  // scale the image to its proper size.
  theImage = theImage.scaled ( iWidth, iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation );

  int iX, iY;
  iX = (int)( ( iWidth  - theImage.width  ( ) ) / 2.0 );
  iY = (int)( ( iHeight - theImage.height ( ) ) / 2.0 );

  thePainter.begin ( &canvas );
  thePainter.drawImage ( iX, iY, theImage, 0, 0 );
  thePainter.end ( );
  theImage = canvas;
}

void Utils::prescaleImage ( QImage &theImage, int iAspect )
{
  float fAspect = 720.0/480.0 / ( 16.0/9.0 );
  if  ( iAspect == 0 )
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

unsigned long long Utils::SDBMHash ( QString &str )
{
   unsigned long long hash = 0;
   for ( unsigned int i = 0; i < (unsigned int)str.length ( ); i++ )
     hash = str[i].toLatin1 ( ) + (hash << 6) + (hash << 16) - hash;

   return hash;
}

bool Utils::recMkdir ( const QString &qsDir )
{
  QDir theDir ( qsDir );
  if ( theDir.exists ( ) )
    return true;

  int t;
  QString qsFullPath, qsPartialDir;
  qsFullPath = theDir.absolutePath ( );
  QStringList dirList = qsFullPath.split ( "/" );

  for ( t=0; t<(int)dirList.count ( ); t++ )  {
    qsPartialDir += "/" + dirList[t];
    theDir.setPath ( qsPartialDir );
    if ( theDir.exists ( ) )
      continue;
    if ( ! theDir.mkdir ( qsPartialDir ) )
      return false;
  }

  // Sanity check if the dir exists ...
  theDir.setPath ( qsFullPath );
  if ( ! theDir.exists ( ) )  {
    printf ( "Failed to create temp directory\n%s\nPlease make sure you have the read/write access.", (const char *)qsFullPath.toUtf8 ( ) );
    return false;
  }
  return true;
}

void Utils::writeZoneCode ( unsigned char iZoneCode, QString qsDVDPath )
{
  const unsigned char arrayCodes[] = { 0x00, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF };
  // * Region 1 - The U.S., U.S. territories and Canada
  // * Region 2 - Europe, Japan, the Middle East, Egypt, South Africa, Greenland
  // * Region 3 - Taiwan, Korea, the Philippines, Indonesia, Hong Kong
  // * Region 4 - Mexico, South America, Central America, Australia, New Zealand, Pacific Islands, Caribbean
  // * Region 5 - Russia (okay, former Russia), Eastern Europe, India, most of Africa, North Korea, Mongolia
  // * Region 6 - China 
  if ( ( iZoneCode > 6 ) || ( qsDVDPath.isEmpty ( ) ) )
    return;

  QFile ifo ( qsDVDPath + "/VIDEO_TS/VIDEO_TS.IFO" );
  for ( int t=0; t<2; t++ )  {
    if  ( ifo.open ( QIODevice::ReadWrite ) == true )  {
      ifo.seek     ( 0x23 );
      ifo.putChar  ( arrayCodes[iZoneCode] );
    }
    ifo.close      ( );
    ifo.setFileName( qsDVDPath + "/VIDEO_TS/VIDEO_TS.BUP" );
  }
}

