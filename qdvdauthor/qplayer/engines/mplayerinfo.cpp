/****************************************************************************
** Implementation of class MPlayerInfo
**
**   Created : Tue Nov 23 07:53:05 2004
**        by : Varol Okan using the kate editor
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** This class collects all possible information about
** the current set file.
**
****************************************************************************/
#include <stdlib.h>
#ifdef QDVD_LINUX 
#include <unistd.h>
#endif

//#include <q3process.h>	//ooo
#include <QProcess>		//xxx
#include <qregexp.h>
#include <qtimer.h>
#include <qmutex.h>
#include <qimage.h>
#include <QTime>	//xxx
#include <QFile>	//xxx
#include <QFileInfo>	//xxx

//#include "../../win32.h"	//ooo
#include "../../global.h"
#include "mplayerinfo.h"

//#define _QMPdebug 1
#ifdef  _QMPdebug
#include <pthread.h>
#define debug_out printf ( "%s::%s : %d <%X> ", __FILE__, __FUNCTION__, __LINE__, (unsigned int)pthread_self ( ) ); printf
#define Lock printf ( "  %s : %d <%X> LOCK\n", __FUNCTION__, __LINE__, (unsigned int)pthread_self ( ) ); Lock
#define Unlock printf ( "  %s : %d <%X> UNLOCK\n", __FUNCTION__, __LINE__, (unsigned int)pthread_self ( ) ); Unlock
#define TryLock printf ( "  %s : %d <%X> TryLOCK\n", __FUNCTION__, __LINE__, (unsigned int)pthread_self ( ) ); TryLock
#else
  void dummy ( ... ) {};
  #define debug_out dummy 
#endif

MPlayerInfoProcess::MPlayerInfoProcess ( MPlayerInfo *pParent )
  //: Q3Process ( )	//ooo
  : QProcess ( )	//xxx
{
  m_pParent       = pParent;
  m_bHasExited    = false;
  //connect ( this, SIGNAL ( processExited ( ) ), this, SLOT ( slotHasExited ( ) ) );   //ooo
  connect ( this, SIGNAL ( finished (int, QProcess::ExitStatus) ), this, SLOT ( slotHasExited ( ) ) );          //xxx
  //connect ( this, SIGNAL ( readChannelFinished ( ) ), this, SLOT ( slotHasExited ( ) ) );          //xxx
}

MPlayerInfoProcess::~MPlayerInfoProcess ( )
{
  if ( m_pParent && ( m_pParent->m_pSlaveProcess == this ) )
       m_pParent->m_pSlaveProcess = NULL;
}

void MPlayerInfoProcess::slotHasExited ( )
{
  m_bHasExited = true;
  if ( m_pParent->useEventLoop ( ) )
    Global::eventLoop.exit ( );
}

bool MPlayerInfoProcess::hasExited ( )
{
  return m_bHasExited;
}

MPlayerInfo::MPlayerInfo   ( )
  : QObject ( ), MediaInfo ( )
{
  initMe ( );
}

MPlayerInfo::MPlayerInfo   ( void *pVoid )
  : QObject ( ), MediaInfo ( pVoid )
{
  initMe ( );
}

MPlayerInfo::~MPlayerInfo ( )
{
  // Then we create a new slave process to obtain the length
  if ( m_pSlaveProcess )  {
    debug_out ( "~MPlayerInfo <%p> \n", this );
//    waitForProcess ( );
    m_pMutex->lock ( );
    deleteSlaveProcess ( );
    m_pMutex->unlock   ( );
  }
  delete m_pMutex;
  m_pMutex = NULL;
  debug_out ( "  That's it.\n" );
}

void MPlayerInfo::initMe ( )
{
  //m_pMutex         = new QMutex ( TRUE );	//ooo
  m_pMutex         = new QMutex ( );		//xxx
  m_bAcquireLock   = false;
  m_pSlaveProcess  = NULL;
  m_iRetryCounter  = 200;
  m_qsTempPathName = QFile::encodeName ( QString ( "%1/%2" ).arg ( Global::qsTempPath ).arg ( Global::qsProjectName ) );
}

void MPlayerInfo::slotRetryQuery ( )
{
  // This function is called from the timer every 50 ms
  // it is called a max of 200 times per try.
  // If it fails to acquire the lock it will kill the slave process and delete the slave object.
  bool bTryToLockFailed = false;
  bool bRet = TryLock ( );

  if ( bRet == false ) {
    // Check if 
    if ( --m_iRetryCounter > 0 ) {
      QTimer::singleShot ( 50, this, SLOT ( slotRetryQuery ( ) ) );
      return;
    }
    else
      bTryToLockFailed = true;
  }
  // At this point we have either acquired the lock or we exceeded the max waiting time of 200 * 50ms = 10 sec
  // Either way we will proceed from here.
  deleteSlaveProcess ( );

  // If we could not acquire the lock we should delete the current mutex and create a new one. 
  // Note: this could be crash prone ???
  if ( bTryToLockFailed == true ) {
    debug_out ( " Hard Out !\n" );
    delete m_pMutex;
    //m_pMutex = new QMutex ( TRUE );		//ooo
    m_pMutex = new QMutex ( );			//xxx
    // Hard lock the mutex, Could starve here ...
    Lock ( );
  }
  m_iRetryCounter = 200;
  m_bAcquireLock  = false;
  queryValues     ( false );
}

void MPlayerInfo::deleteSlaveProcess ( )
{
  if ( m_pSlaveProcess ) {
    if ( ! m_pSlaveProcess->hasExited ( ) ) {
      //m_pSlaveProcess->tryTerminate ( );				//ooo
      //usleep ( 20000 ); // sleep for a bit ( 20 ms )			//ooo
      SleeperThread::msleep ( 20 ); // sleep for a bit ( 20 ms )	//xxx
      m_pSlaveProcess->kill ( );
      //usleep ( 20000 ); // sleep for a bit ( 20 ms )			//ooo
      SleeperThread::msleep ( 20 ); // sleep for a bit ( 20 ms )	//xxx
      debug_out ( "kill / delete <%p>\n", m_pSlaveProcess );
    }
    delete m_pSlaveProcess;
    m_pSlaveProcess = NULL;
  }
}

void MPlayerInfo::queryValues ( )
{
  queryValues ( true );
}

void MPlayerInfo::queryValues ( bool bTryToLock ) 
{
  QStringList listCommand;

  // Check if we are in the process of acuiring the lock.
  if ( m_bAcquireLock ) 
    return;
  // The first thing is to try to acquire the lock.
  bool bRet = TryLock ( );
  if ( bTryToLock && bRet == false ) {
    m_iRetryCounter = 200;
    m_bAcquireLock  = true;
    QTimer::singleShot ( 50, this, SLOT ( slotRetryQuery ( ) ) );
    return;
  }

  // At this point we have the lock and can create a new slave process to obtain the length
  deleteSlaveProcess ( );

  QFileInfo fileInfo ( QFile::encodeName ( m_qsFileName ) );
  QString qsAudioExtensions = getExtensions ( false );
  int  iIsVideo = 1;
  //if ( qsAudioExtensions.find ( fileInfo.extension ( FALSE ) ) > -1 )		//ooo
  if ( qsAudioExtensions.indexOf ( fileInfo.suffix ( ) ) > -1 )			//xxx
       iIsVideo = 0;

  m_pSlaveProcess = new MPlayerInfoProcess ( this );
  debug_out ( "For this<%p> = new slave <%p>\n\n", this, m_pSlaveProcess );

  listCommand.append ( QString ( "mplayer"     ) );
  listCommand.append ( QString ( "-ao"         ) );
  listCommand.append ( QString ( "null"        ) );
  listCommand.append ( QString ( "-vo"         ) );
  listCommand.append ( QString ( "null"        ) );
  listCommand.append ( QString ( "-frames"     ) );
  listCommand.append ( QString ( "%1").arg ( iIsVideo ) ); // no frames for audio files.
  listCommand.append ( QString ( "-slave"      ) );
  listCommand.append ( QString ( "-identify"   ) );
  listCommand.append ( QString ( "--"          ) );
  listCommand.append ( QString ( "%1" ).arg ( m_qsFileName ) );

  //connect ( m_pSlaveProcess, SIGNAL ( readyReadStdout ( ) ), this, SLOT ( slotReadStdout ( ) ) );         //ooo
  connect ( m_pSlaveProcess, SIGNAL ( readyReadStandardOutput ( ) ), this, SLOT ( slotReadStdout ( ) ) );   //xxx

  //m_pSlaveProcess->clearArguments ( );		//ooo
  m_pSlaveProcess->setArguments   ( listCommand );

//  QString test ( " " );
//  test = listCommand.join ( test );
//  printf ( "MPLayerInfo::queryValues () <%s>\n", test.ascii ( ) );

  QTimer::singleShot ( 0, this, SLOT ( slotStart ( ) ) );
  //usleep ( 10 );			//ooo
  SleeperThread::msleep ( 10 );		//xxx
//  m_pSlaveProcess->start ( ); 
  Unlock ( );

  if ( useEventLoop ( ) )
    Global::eventLoop.exec ( );
  else
    waitForProcess  ( );
}

void MPlayerInfo::slotStart ( )
{
  // Called through timer, as the timer is execued in the main thread.
  // the above function queryValues () is executed in another thread, 
  // thus crashing in m_pSlaveProcess->start ( ); -> XFlush()'s call
  m_pSlaveProcess->start ( );
}

void MPlayerInfo::waitForProcess ( )
{
  if ( ! m_pSlaveProcess )
    return;
  int  iMaxCounter = 200;
  bool bContinue   = true;
  try {
    for ( ; bContinue ; )  {
debug_out ( "this<%p> slave<%p> iCount<%d>\n", this, m_pSlaveProcess, iMaxCounter );
      Lock ( );
      if ( m_pSlaveProcess && ! m_pSlaveProcess->hasExited ( ) && ( iMaxCounter > 0 ) ) {
        Unlock ( );
        //usleep ( 50000 ); // 50 ms = 0.05sec			//ooo
	SleeperThread::msleep ( 50 ); // 50 ms = 0.05sec	//xxx
        iMaxCounter --;
      }
      else {
        bContinue = false;
        Unlock ( );
        break;
      }
    }
  }
  catch ( ... ) {
    // Simply do not crash ...
  }
}

void MPlayerInfo::slotReadStdout ()
{
  if ( ! m_pSlaveProcess )
    return;
  Lock ( );
//printf ("MPlayerInfo::slotReadStdout\n");
  QString qsWidth, qsHeight, qsWindowWidth, qsWindowHeight;
  QString qsLength, qsBuffer, qsAspect;
  //QString qsStdout ( m_pSlaveProcess->readStdout ( ) );		//ooo
  QString qsStdout ( m_pSlaveProcess->readAllStandardOutput ( ) );	//xxx
  float fAspect = 0.0f;
  int iPos;

  if ( qsStdout.length ( ) == 0) {
    Unlock ( );
    return;
  }

  //m_listOfStdout.append ( qsStdout );
  //m_pMutex->unlock ( );

//printf ("    %s\n", (const char *)qsStdout);
printf ("***    %s\n", qsStdout.toLatin1().data());    //xxx
	// Okay and here we process the received data.
	// VO: [xv] 352x240 => 352x264 Planar YV12
	QRegExp	regSize ("ID_VIDEO_WIDTH=([0-9]+)");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		qsWidth   = regSize.cap(1);
		m_iResolutionWidth  = qsWidth. toInt();
	}
	regSize = QRegExp ("ID_VIDEO_HEIGHT=([0-9]+)");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		qsHeight   = regSize.cap(1);
		m_iResolutionHeight = qsHeight.toInt();
	}
	// Here we precesso both values (if both are in ...
	if ( (m_iResolutionWidth > 0) && (m_iResolutionHeight > 0) )	{
		m_bVideoHandled = true;
		m_qsVideoFormat = getVideoFormat(m_iResolutionWidth, m_iResolutionHeight);
		m_qsResolution  = QString ("%1x%2").arg(m_iResolutionWidth).arg(m_iResolutionHeight);
	}
	regSize = QRegExp ("ID_VIDEO_FPS=([0-9\\.]+)");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_fFPS  = regSize.cap(1).toFloat();
		m_qsFPS = regSize.cap(1);
	}
	regSize = QRegExp ("ID_VIDEO_BITRATE=([0-9]+)");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_iVideoBPS  = regSize.cap(1).toInt();
		m_qsVideoBPS = regSize.cap(1);
	}
	regSize = QRegExp ("AO:.*");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_bAudioHandled = true;
	}
	// Movie-Aspect is 1.33:1 - prescaling to correct movie aspect.
	regSize = QRegExp ("Movie-Aspect is (\\S+).*"); //  1.33:1 OR undefined ...
	iPos = 0;
	//while ( ( iPos = regSize.search ( qsStdout, iPos ) ) != -1 )  {	//ooo
	while ( ( iPos = regSize.indexIn ( qsStdout, iPos ) ) != -1 )  {	//xxx
	  iPos += regSize.matchedLength ( );
	  if ( fAspect == 0.0 )  {
	    qsAspect = regSize.cap    ( 1 );
	    fAspect  = qsAspect.toFloat ( );
	    if ( fAspect > 0.0 )  {
	      if ( fAspect < 1.5 )
	        m_qsRatio = "4:3";
	      else if ( fAspect < 2.0 )
	        m_qsRatio = "16:9";
	      else
	        m_qsRatio = "4:3";
	    }
	  }
	}
	regSize = QRegExp ("ID_VIDEO_ASPECT=([0123456789.]*)"); //  1.33:1 OR undefined ...
	iPos = 0;
	//while ( ( iPos = regSize.search ( qsStdout, iPos ) ) != -1 )  {	//ooo
	while ( ( iPos = regSize.indexIn ( qsStdout, iPos ) ) != -1 )  {	//xxx
	  iPos += regSize.matchedLength ( );
	  if ( fAspect == 0.0 )  {
	    qsAspect = regSize.cap    ( 1 );
	    fAspect  = qsAspect.toFloat ( );
	    if ( fAspect > 0.0 )  {
	      if ( fAspect < 1.5 )
	        m_qsRatio = "4:3";
	      else if ( fAspect < 2.0 )
	        m_qsRatio = "16:9";
	      else
	        m_qsRatio = "4:3";
	    }
	  }
	}
	// Selected video codec: [mpeg12] vfm:libmpeg2 (MPEG 1 or 2 (libmpeg2))
	regSize = QRegExp ("ID_VIDEO_FORMAT=(\\S*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		if (regSize.cap(1)[0] != '0')	// discard in case we get some cryptic 0x10000002 style of info ...
			m_qsVideoCodec = regSize.cap(1);
	}
	regSize = QRegExp ("Selected video codec:\\s*(\\S*)\\s*(\\S*).*");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		if (m_qsVideoCodec.isEmpty())
			m_qsVideoCodec = regSize.cap(1) + QString (" ") + regSize.cap(2);
		m_bHasVideo = true;
		m_bVideoHandled = true;
	}
	// Selected audio codec: [mp3] afm:mp3lib (mp3lib MPEG layer-2, layer-3)
	regSize = QRegExp ("Selected audio codec:\\s*(\\S*)\\s*(\\S*).*");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_bHasAudio = true;
		m_bAudioHandled = true;
	}
	// AUDIO: 44100 Hz, 2 ch, 16 bit (0x10), ratio: 28000->176400 (224.0 kbit)
	regSize = QRegExp ("ID_AUDIO_CODEC=(\\S*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_qsAudioCodec = regSize.cap(1);
		m_bHasAudio = true;
	}
	// What the F... was I thinking ???
	//	regSize = QRegExp ("ID_AUDIO_BITRATE=(\\S*)");
	//	if (regSize.searchRev(qsStdout) > -1)	{
	//		m_qsAudioCodec = regSize.cap(1);
	//		m_bHasAudio = true;
	//	}
	regSize = QRegExp ("ID_AUDIO_BITRATE=([0-9]*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_iAudioBPS  = regSize.cap(1).toInt();
		m_qsAudioBPS = regSize.cap(1);
		m_bHasAudio = true;
	}
	
	regSize = QRegExp ("ID_AUDIO_RATE=([0-9]*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_iSample  = regSize.cap(1).toInt();
		m_qsSample = regSize.cap(1);
		m_bHasAudio = true;
	}
	regSize = QRegExp ("ID_AUDIO_NCH=([0-9]*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		int iChannels = regSize.cap(1).toInt();
		m_qsAudioFormat = QString ("Undef.");
		if (iChannels == 2)
			m_qsAudioFormat = QString ("Stereo");
		else if (iChannels == 1)
			m_qsAudioFormat = QString ("Mono");
		m_bHasAudio = true;
	}
	regSize = QRegExp ("ID_AUDIO_BITRATE=([0-9]*)");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		m_iAudioBPS  = regSize.cap(1).toInt();
		m_qsAudioBPS = regSize.cap(1);
		m_bHasAudio = true;
	}
	// AUDIO: 44100 Hz, 2 ch, 16 bit (0x10), ratio: 28000->176400 (224.0 kbit)
	regSize = QRegExp ("AUDIO:\\s*([0-9]+\\s*Hz),\\s([0-9]+\\s*ch),\\s([0-9]+\\s*bit).*([0-9\\.]+\\skbit).");
	//if (regSize.searchRev(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		QString qsBits = regSize.cap(3);
		m_iBits        = qsBits.left(qsBits.length() - 4).toInt();
		m_bHasAudio = true;
	}
	// Das ist das absolut letzte !!!
	regSize = QRegExp ("ID_LENGTH=([0-9]*)");
	//if (regSize.search(qsStdout) > -1)	{	//ooo
	if (regSize.indexIn(qsStdout) > -1)	{	//xxx
		qsLength   = regSize.cap(1);
		m_iLength  = qsLength.toInt() * 1000;
		m_qsLength = QString ("00:00:00");

		float fTemp = qsLength.toFloat();
		if(fTemp != 0.0)	{
			int iHours, iMinutes, iSeconds;
			iHours  = (int)(fTemp/3600.0);
			fTemp    -= (iHours*3600.0);
			iMinutes= (int)(fTemp/60.0);
			fTemp    -= (iMinutes*60.0);
			iSeconds= (int)fTemp;
			m_qsLength.sprintf ("%02d:%02d:%02d", iHours, iMinutes, iSeconds);
		}
	}
	// Next we check the status of the media stream ...
	// Note: this info can be validated at any time ...
	if 	(   (m_bHasAudio && m_bAudioHandled) || 
		  ( (m_bHasVideo && m_bVideoHandled) && (m_iResolutionWidth > 0) && (m_iResolutionHeight) ) )
		m_qsStatus = QString ("Ok"); // E.g. Ok	-=> to say this file we can autmatically convert to a DVD video

	// Finally we can release the mutex...
	Unlock ( );
//	test_print ();
}

QImage MPlayerInfo::getScreenshot( long iMSecOffset )
{
	// FIXME:
	// check out "-fixed-vo" "-aid" "-vid" "-sid" "-slang" options.
	// This function will generate screenshots and store them
	// under the defined <temp drive>/<BaseFileName>/00000001.jpeg
	// E.g. /tmp/movie/00000001.jpeg for ~/myMovies/movie.mpg

	debug_out ( "obj<%p>\n", this );
	//m_pMutex->lock ( );

        int iRet;
	QImage theImage;
	QString qsCommand, qsTempPath, m_qsScreenshotName, qsFileName;
	QTime timeOffset;

	qsFileName = getFileName ( );
	// Little surprise, that ` is not converted through QFile::convertName ...
	qsFileName.replace ( "`", "\\`" );

	timeOffset = timeOffset.addMSecs ( iMSecOffset );
		QFileInfo fileInfo ( qsFileName );

	qsTempPath = QFile::encodeName ( QString ( "%1/%2" ).arg ( m_qsTempPathName ).arg ( fileInfo.baseName ( ) ) );
	qsCommand  = QString ( "mkdir -p \"%1\"" ).arg ( qsTempPath );
	//iRet = system ( qsCommand.utf8 ( ) ); iRet = iRet;	//ooo
	iRet = system ( qsCommand.toUtf8 ( ) ); iRet = iRet;	//xxx
	
	m_qsScreenshotName  = QString ("%1/00000001.jpg").arg ( qsTempPath );
	if (iMSecOffset != 0)	// For some reasons Mplayer produces always the first image from start
		m_qsScreenshotName  = QString ( "%1/00000002.jpg" ).arg ( qsTempPath );

	// Next we verify that the file has been created.
	//fileInfo.setFile ( QFile::decodeName ( m_qsScreenshotName.utf8 ( ) ) );	//ooo
	fileInfo.setFile ( QFile::decodeName ( m_qsScreenshotName.toUtf8 ( ) ) );	//xxx
	if ( fileInfo.exists ( ) )  {
		qsCommand = QString ( "rm -f \"%1\"" ).arg ( m_qsScreenshotName );
		//iRet = system ( qsCommand.utf8 ( ) );		//ooo
		iRet = system ( qsCommand.toUtf8 ( ) );		//xxx
	}
	// Here we generate the screenshot ...
//	qsCommand = QString("mplayer -ao null -vo jpeg:outdir=\"%1/\" -vf spp,scale -frames 2 -quiet \"%2\" &>/dev/null").arg(qsTempPath).arg(getFileName());
	if ( iMSecOffset == 0 ) {
		//qsCommand = QString ( "mplayer -ao null -vo jpeg:outdir=\"%1/\" -vf scale -frames 2 -quiet \"%2\" >/dev/null 2>&1 &" ).arg ( qsTempPath ).arg ( QFile::encodeName ( qsFileName.utf8 ( ) ) );		//ooo
		qsCommand = QString ( "mplayer -ao null -vo jpeg:outdir=\"%1/\" -vf scale -frames 2 -quiet \"%2\" >/dev/null 2>&1 &" ).arg ( qsTempPath ).arg ( qsFileName );						//xxx
	} else {
		//qsCommand = QString ( "mplayer -ao null -vo jpeg:outdir=\"%1/\" -vf scale -ss %2 -forceidx -frames 3 -quiet \"%3\" >/dev/null 2>&1 &" ).arg ( qsTempPath ).arg ( timeOffset.toString ( ) ).arg ( QFile::encodeName ( qsFileName.utf8 ( ) ) );		//ooo
		qsCommand = QString ( "mplayer -ao null -vo jpeg:outdir=\"%1/\" -vf scale -ss %2 -forceidx -frames 3 -quiet \"%3\" >/dev/null 2>&1 &" ).arg ( qsTempPath ).arg ( timeOffset.toString ( ) ).arg ( qsFileName );						//xxx
	}
	//iRet = system ( qsCommand.utf8 ( ) );		//ooo
	iRet = system ( qsCommand.toUtf8 ( ) );		//xxx
	// printf ("MPlayerInfo::getScreenshot iRet<%d> = <%s>\n", iRet,(const char *)qsCommand);

	int iMaxWait = 0;
	//usleep ( 100000 ); // wait 100 MS because the command was executed in the background			//ooo
	SleeperThread::msleep ( 100 ); // wait 100 MS because the command was executed in the background	//xxx
	while ( iMaxWait++ < 8 )  {
		{       // Note: Need to create a new object each time, else the 'exist' info will be buffered. This is No Good here ...
			//QFileInfo fileInfo ( QFile::decodeName ( m_qsScreenshotName.utf8 ( ) ) );	//ooo
			QFileInfo fileInfo ( QFile::decodeName ( m_qsScreenshotName.toUtf8 ( ) ) );	//xxx
			if ( fileInfo.exists  ( ) )  {
				//if ( ! theImage.load ( QFile::decodeName ( m_qsScreenshotName.utf8 ( ) ) ) )  {	//ooo
				if ( ! theImage.load ( QFile::decodeName ( m_qsScreenshotName.toUtf8 ( ) ) ) )  {	//xxx
					//debug_out ( "Could not load preview from file <%s>\n", (const char *)m_qsScreenshotName.utf8 ( ) );	//ooo
					debug_out ( "Could not load preview from file <%s>\n", (const char *)m_qsScreenshotName.toUtf8 ( ) );	//xxx
				}
				if ( theImage.width ( ) > 0 )
					return theImage;
			}
			//sleep ( 1 );				//ooo
			SleeperThread::msleep ( 1 );		//xxx
		}
	}

	//m_pMutex->unlock ( );
	return theImage;
}

bool MPlayerInfo::hasChapters ()
{
  return false;
}
