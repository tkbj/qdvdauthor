/***************************************************************************
    mediainfo.cpp
                             -------------------
    Implementation of class MediaInfo
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class collects all possible information about
    the current set stream.
    
****************************************************************************/

#include <qfileinfo.h>
#include <qobject.h>
#include <qmessagebox.h>

#include "../global.h"
#include "mediainfo.h"

MediaInfo::MediaInfo (void *pMediaEngine)
{
  m_bUseEventLoop = true;
  m_pMediaEngine  = NULL;
  initMe ( );
  setMediaEngine ( pMediaEngine );
}

MediaInfo::~MediaInfo ()
{
}

void MediaInfo::initMe ()
{
	// Initializing the variables for the MediaFile.
	m_bHaveValues       = false;
	m_bHasAudio         = false;
	m_bHasVideo         = false;
	//F.J.Cruz
	m_bIsSeekeable	    = false;
	m_bAudioHandled     = false;
	m_bVideoHandled     = false;
	m_iResolutionWidth  = -1;
	m_iResolutionHeight = -1;
	m_fFPS              = 0.0f;
	m_iLength           = 0;
	m_iBits             = 0;
	m_iSample           = 0;
	m_iAudioBPS         = 0;
	m_iVideoBPS         = 0;
	m_qsStatus = QObject::tr ("Not Ok");
}

bool MediaInfo::useEventLoop ( )
{
  return m_bUseEventLoop;
}

void MediaInfo::setUseEventLoop ( bool bUseEventLoop )
{
  m_bUseEventLoop = bUseEventLoop;
}

void *MediaInfo::getMediaEngine ()
{
  return m_pMediaEngine;
}
	
void MediaInfo::setMediaEngine  (void *pMediaEngine)
{
	if (pMediaEngine)
		m_pMediaEngine = pMediaEngine;
}

void MediaInfo::setFileName ( QString qsStream, bool bQueryValues )
{
  // First we chack if we want to play a DVD, no info avail at this point in time ...
  //if ( qsStream.find ( "dvd://" ) > -1 )				//ooo
  if ( qsStream.indexOf ( "dvd://", Qt::CaseInsensitive ) > -1 )	//xxx
    return;

  // Next we check if the file exists ...
  QFileInfo fileInfo ( qsStream );
  if ( ! fileInfo.exists ( ) )  {
    printf ("File not found %s\n", (const char *)QFile::encodeName ( qsStream ) );
    return;
  }

  // Set the file size ...
  m_iSize = fileInfo.size();
  if (m_iSize < 1)
    m_qsSize = QString ("? B");
  if (m_iSize < 1024)
    m_qsSize = QString ("%1 B").arg(m_iSize);
  else if (m_iSize < 1024*1024)
    m_qsSize = QString ("%1kB").arg((float)m_iSize/1024);
  else if (m_iSize < 1024*1024*1024)
    m_qsSize = QString ("%1MB").arg((float)m_iSize/(1024*1024));
  else
    m_qsSize = QString ("%1GB").arg((float)m_iSize/(1024*1024*1024));

  // set the file name ...
  m_qsFileName = qsStream;
  // reset the previous values ...
  initMe ( );
  // and now let us query some values ...
  if ( bQueryValues )
        queryValues ( );
}

QString MediaInfo::getFileName (bool bFullName)
{
	if (!bFullName)	{
		QFileInfo fileInfo(m_qsFileName);
		QString qsFileName = fileInfo.fileName();
		return qsFileName;
	}
	return m_qsFileName;
}

QString MediaInfo::getVideoFormat (int iWidth, int iHeight)
{
	QString qsFormat = ("custom");
	if ( ((iWidth == 720) && (iHeight == 480) ) ||
	     ((iWidth == 704) && (iHeight == 480) ) ||
	     ((iWidth == 352) && (iHeight == 480) ) ||
	     ((iWidth == 352) && (iHeight == 240) ) )
		 qsFormat = QString ("ntsc");

	if ( ((iWidth == 720) && (iHeight == 576) ) ||
	     ((iWidth == 704) && (iHeight == 576) ) ||
	     ((iWidth == 352) && (iHeight == 576) ) ||
	     ((iWidth == 352) && (iHeight == 288) ) )
		 qsFormat = QString ("pal");

	return qsFormat;
}

QString MediaInfo::getExtensions (bool bVideo)
{
	QString qsExtension;
	if ( bVideo )
		qsExtension = QString ( "*.mpg *.mpeg *.mpeg2 *.avi *.mwa *.mov *.wma *.vob *.m2v *.dv *.wmv *.mjpeg *.mve *.asf *.asx *.MPG *.MPEG *.MPEG2 *.AVI *.MWA *.MOV *.WMA *.VOB *.M2V *.DV *.WMV *.MJPEG *.MVE *.ASF *.ASX" );
	else
		qsExtension = QString ( "*.wav *.mp3 *.mp2 *.ac3 *.m2a *.mpa *.mpega *.ogg *.WAV *.MP3 *.MP2 *.AC3 *.M2A *.MPA *.MPEGA *.OGG" );

	return qsExtension;
}

unsigned long int MediaInfo::getLength ()
{
	// info in milli Seconds
	return m_iLength;
}

QString MediaInfo::getLengthString ()
{
	return m_qsLength;
}

QString MediaInfo::getRatio ()
{
  // here we process a bit if neccesary ...
  // checking for something like 1.3333:1
  if  ( ( m_qsRatio == "1:1" ) ||
	( m_qsRatio == "4:3" ) ||
	( m_qsRatio == "16:9" ) ||
	( m_qsRatio == "2.21:1" ) )
    return m_qsRatio;

  QString qsRatio = m_qsRatio;
  //int iPos = qsRatio.find ( ":" );				//ooo
  int iPos = qsRatio.indexOf ( ":", Qt::CaseSensitive );	//xxx
  float fValue = 0.0;

  if ( iPos ) {
    QString qsVal1 = qsRatio.left  ( iPos );
    QString qsVal2 = qsRatio.right ( qsRatio.length () - iPos-1 );
    float fVal1 = qsVal1.toFloat ();
    float fVal2 = qsVal2.toFloat ();
    if ( fVal2 != 0.0 )
      fValue = fVal1 / fVal2;
  }
  else 
    fValue = qsRatio.toFloat ();

  // Okay for sanity check ...
  if ( fValue != 0.0 ) {
    if ( ( fValue > 0.9 ) && ( fValue < 1.1 ) )
      qsRatio= "1:1";
    else if ( ( fValue > 1.23 ) && ( fValue < 1.43 ) )
      qsRatio = "4:3";
    else if ( ( fValue > 1.68 ) && ( fValue < 1.88 ) )
      qsRatio = "16:9";
    else if ( ( fValue > 2.11 ) && ( fValue < 2.31 ) )
      qsRatio = "2.21:1";
  }

  return qsRatio;
}

int MediaInfo::getBits ()
{
	return m_iBits;
}

QString MediaInfo::getFormat (bool bVideo)
{ 
	if (bVideo)
		return m_qsVideoFormat;
	return m_qsAudioFormat;
}

QString MediaInfo::getCodec (bool bVideo)
{
	if (bVideo)
		return m_qsVideoCodec;
	return m_qsAudioCodec;
}

uint MediaInfo::getSize ()
{
	// in bytes
	return m_iSize;
}

QString MediaInfo::getSizeString ()
{
	return m_qsSize;
}

int MediaInfo::getResolution (bool bWidth)
{
	if (bWidth)
		return m_iResolutionWidth;
	else 
		return m_iResolutionHeight;
}

QString MediaInfo::getResolutionString ()
{
	return m_qsResolution;
}

float MediaInfo::getFPS ()
{
	return m_fFPS;
}

QString MediaInfo::getStatus ()
{
	return m_qsStatus;
}

int MediaInfo::getSample ()
{
	return m_iSample;
}

QString MediaInfo::getSampleString ()
{
	return m_qsSample;
}

int MediaInfo::getBPS (bool bVideo)
{
	if (bVideo)
		return m_iVideoBPS;
	return m_iAudioBPS;	
}

QString MediaInfo::getBPSString (bool bVideo)
{
	if (bVideo)
		return m_qsVideoBPS;
	return m_qsAudioBPS;
}

bool MediaInfo::hasAudio ()
{
	return m_bHasAudio;
}

bool MediaInfo::hasVideo ()
{
	return m_bHasVideo;
}
//F.J.Cruz
bool MediaInfo::isSeekeable()
{
	return m_bIsSeekeable;
}

bool MediaInfo::audioHandled ()
{
	return m_bAudioHandled;
}

bool MediaInfo::videoHandled ()
{
	return m_bVideoHandled;
}

void MediaInfo::test_print ()
{
	//printf ("FileName : %s\n", (const char *)getFileName ());									//ooo
	printf ("FileName : %s\n", getFileName ().toLatin1().data());									//xxx
	//printf ("Length   : %d <%s>\n", (uint)getLength (), (const char *)getLengthString ());					//ooo
	printf ("Length   : %d <%s>\n", (uint)getLength (), getLengthString ().toLatin1().data());					//xxx
	//printf ("Ratio    : %s\n", (const char *)getRatio ());									//ooo
	printf ("Ratio    : %s\n", getRatio ().toLatin1().data());									//xxx
	//printf ("Format   : Video<%s> Audio<%s>\n", (const char *)getFormat  (true), (const char *)getFormat (false));		//ooo
	printf ("Format   : Video<%s> Audio<%s>\n", getFormat  (true).toLatin1().data(), getFormat (false).toLatin1().data());		//xxx
	//printf ("Codec    : Video<%s> Audio<%s>\n", (const char *)getCodec (true), (const char *)getCodec (false));			//ooo
	printf ("Codec    : Video<%s> Audio<%s>\n", getCodec (true).toLatin1().data(), getCodec (false).toLatin1().data());		//xxx
	//printf ("Size     : %d <%s>\n", getSize (), (const char *) getSizeString ());							//ooo
	printf ("Size     : %d <%s>\n", getSize (), getSizeString ().toLatin1().data());						//xxx
	//printf ("Resolution : %d - %d <%s>\n", getResolution (true),  getResolution (false), (const char *) getResolutionString ());	//ooo
	printf ("Resolution : %d - %d <%s>\n", getResolution (true),  getResolution (false), getResolutionString ().toLatin1().data());	//xxx
	printf ("FPS        : %f\n", getFPS ());
	//printf ("Status     : %s\n", (const char *)getStatus ());									//ooo
	printf ("Status     : %s\n", getStatus ().toLatin1().data());									//xxx
	//printf ("Sample     : %d <%s>\n", getSample (), (const char *)getSampleString ());						//ooo
	printf ("Sample     : %d <%s>\n", getSample (), getSampleString ().toLatin1().data());						//xxx
	//printf ("BPS (Video): %d <%s>\n", getBPS (true), (const char *)getBPSString (true));						//ooo
	printf ("BPS (Video): %d <%s>\n", getBPS (true), getBPSString (true).toLatin1().data());					//xxx
	//printf ("BPS (Audio): %d <%s>\n", getBPS (false), (const char *)getBPSString (false));					//ooo
	printf ("BPS (Audio): %d <%s>\n", getBPS (false), getBPSString (false).toLatin1().data());					//xxx
	printf ("Bits       : %d\n", getBits ());
	printf ("Has Video  : %s Supported %s\n", hasVideo () ? "true" : "false", videoHandled () ? "true" : "false");
	printf ("Has Audio  : %s Supported %s\n", hasAudio () ? "true" : "false", audioHandled () ? "true" : "false");
	printf ("Is Seekable : %s\n", isSeekeable () ? "true" : "false");
}

