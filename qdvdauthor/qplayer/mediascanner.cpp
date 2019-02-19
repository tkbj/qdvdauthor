/***************************************************************************
    mediascanner.cpp
                             -------------------
    MediaScanner class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class spinns off a new thread and generate a Preview of the Job 
    which was set through MediaCreator::registerWithMediaScanner.
 
    Once the preview is generated we will call the update function in 
    the main thread ( Currently MovieObject::updatePixmap () )

    if after 5 seconds after termination of the thread there are no new 
    requests in the queue then the thread will be termintaed to free 
    some memory.
    
****************************************************************************/

#include <stdlib.h>

#include <qapplication.h>
#include <qfileinfo.h>
//#include <qimage.h>   //ooo
#include <QImage>       //xxx
#include <qdir.h>
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	//oxx
#include <QMessageBox>		//xxx
#include <QUrl> //xxx

#include "../utils.h"
#include "../importsub.h"
#include "../sourcefileentry.h"
#include "mediascanner.h"
#include "mediacreator.h"
#include "mediainfo.h"


ExecuteJob::ExecuteJob ( )
{
	bFinished       = false;
	bUpdateAllInfo  = false;
	fFPS            = 0.0f;
	iMSecondsOffset = 0;
	pImage          = NULL;
	pOrigObject     = NULL;
	pSourceFileInfo = NULL;
	pSubtitles      = NULL;
	theType         = TYPE_UNKNOWN; 
}

MediaScanner::MediaScanner ( )
  : QThread ( )
{
  m_bHasFinished = false;
  m_bSemaphore   = false;
  m_iWaitCounter = 50;
  //m_pMutex       = new QMutex ( true );	//ooo
  m_pMutex       = new QMutex ( );		//xxx
}

MediaScanner::~MediaScanner ( )
{
  uint t;
  if ( m_pMutex )
    delete m_pMutex;
  for ( t=0; t< (uint)m_listExecuteFifo.count ( ); t++ )	//oxx
    delete m_listExecuteFifo[t];
  m_listExecuteFifo.clear ( );
}

bool MediaScanner::hasFinished ()
{
  return m_bHasFinished;
}

int MediaScanner::waitCounter ( int iNewWaitCounter )
{
  int iOrig      = m_iWaitCounter;
  m_iWaitCounter = iNewWaitCounter;
  return iOrig;
}

void MediaScanner::append ( ExecuteJob *pNewJob )
{
  // Please remember that this is executed in the main thread
 if ( ! setSemaphore ( true ) )
    return;

  m_listExecuteFifo.append ( pNewJob );
  setSemaphore   ( false );
  //if ( ! running ( ) )  {	//ooo
  if ( ! isRunning ( ) )  {	//xxx
#if (QT_VERSION > 0x0301FF)
    start ( QThread::LowPriority );
#else
    start ( );
#endif
  }
}

void MediaScanner::resetWait ( )
{
  m_iWaitCounter = 0;
}

void MediaScanner::remove ( SourceFileInfo *pSourceFileInfo )
{
	// Please remember that this is executed in the main thread
	uint t;
	ExecuteJob *pJob = NULL;
	if ( ( m_listExecuteFifo.count ( ) < 1 ) || 
	     ( hasFinished ( ) ) )
	  return;

	if ( ! setSemaphore ( true ) )
		return;
	for (t=0;t<(uint)m_listExecuteFifo.count();t++)	{	//oxx
		pJob = m_listExecuteFifo[t];
		if (pJob->pSourceFileInfo == pSourceFileInfo)	{
			pJob->pSourceFileInfo = NULL;
			pJob->pOrigObject     = NULL;
		}
	}
	setSemaphore ( false );
}

void MediaScanner::remove ( QObject *pObject )
{
	// Please remember that this is executed in the main thread
	uint t;
	ExecuteJob *pJob = NULL;
	if ( ( m_listExecuteFifo.count ( ) < 1 ) || 
	     ( hasFinished ( ) ) )
	  return;

	if ( ! setSemaphore ( true ) )
		return;
	for ( t=0; t<(uint)m_listExecuteFifo.count ( ); t++ )  {	//oxx
		pJob = m_listExecuteFifo[t];
		if ( pJob->pOrigObject == pObject )  {
			pJob->pOrigObject = NULL;
		}
	}
	setSemaphore ( false );
}

bool MediaScanner::setSemaphore ( bool bSet )
{
  if ( bSet )  {
    int iCount = 1000;
    while ( iCount-- > 0 )  {
      if  ( m_pMutex->tryLock ( ) )
        return true;
      usleep ( 1000 ); 
    }
    return false;
//    m_pMutex->lock   ( );
  }
  else
    m_pMutex->unlock ( );
  return true;

  uint iMaxWait = 0;
  // FALSE== freeing the semaphore does not require a check ...
  if ( bSet )	{
    // wait for max 3 seconds ...
    while ( ( iMaxWait++ < 3000 ) && m_bSemaphore )
      usleep ( 1000 );	// wait for 1 mSec
    // Status check if we reached max waiting time without success ...
    if (iMaxWait > 2998)
      return false;
  }
  m_bSemaphore = bSet;
  return true;
}

void MediaScanner::run ( )
{
  // And this is executed in the child thread ...
  uint t;
  m_pMediaInfo = MediaCreator::createInfo ( );
  m_pMediaInfo->setUseEventLoop ( false ); // EventLoop would crash app in other then MainThread.
  QList<ExecuteJob *> listOfExecutedJobs;		//oxx
  QString qsFileName;
  QImage  theImage; //ooo
  //QImage  theImage(QSize(720,576), QImage::Format_RGB32);   //xxx
  //QImage  *theImage; //xxx
  // Generate the preview(s)
  while ( m_listExecuteFifo.count ( ) )  {
    // I need the two loops because after running through the inner loop we see if the user 
    // will generate a new request within 5 seconds or not.
    while ( m_listExecuteFifo.count ( ) ) {
      if ( ! setSemaphore  ( true ) )
        return;	// timed out.
      ExecuteJob *pCurrentJob = m_listExecuteFifo.first();
      // Check if the object was destroyed from some other thread ...
      if ( ! pCurrentJob->pOrigObject )  {
        //m_listExecuteFifo.remove  ( pCurrentJob );	//ooo
        m_listExecuteFifo.removeOne  ( pCurrentJob );	//xxx
        listOfExecutedJobs.append ( pCurrentJob );
        setSemaphore ( false );
        continue;
      }

      switch ( pCurrentJob->theType ) {
      case ExecuteJob::TYPE_INFO :  {  // pCurrentJob->pSourceFileInfo)	{
        qsFileName = pCurrentJob->pSourceFileInfo->qsFileName;
        setSemaphore ( false );// unblock since the next function can take a few seconds.

        m_pMediaInfo->setFileName ( qsFileName, pCurrentJob->bUpdateAllInfo );

        if ( ( setSemaphore ( true ) ) && (pCurrentJob->pOrigObject) && ( pCurrentJob->bUpdateAllInfo ) )	{
          pCurrentJob->pSourceFileInfo->qsVideoFormat = m_pMediaInfo->getFormat     ( true  );
          pCurrentJob->pSourceFileInfo->qsResolution  = m_pMediaInfo->getResolutionString ( );
          pCurrentJob->pSourceFileInfo->qsLength      = m_pMediaInfo->getLengthString     ( );
          pCurrentJob->pSourceFileInfo->qsSize        = m_pMediaInfo->getSizeString       ( );   
          pCurrentJob->pSourceFileInfo->qsRatio       = m_pMediaInfo->getRatio            ( );
          pCurrentJob->pSourceFileInfo->qsStatus      = m_pMediaInfo->getStatus           ( );
          pCurrentJob->pSourceFileInfo->qsFPS         = QString ("%1").arg ( m_pMediaInfo->getFPS  ( ) );
          // And also store the audio props ...
          pCurrentJob->pSourceFileInfo->bHasAudioTrack= m_pMediaInfo->hasAudio ( );
          pCurrentJob->pSourceFileInfo->qsBits        = QString ("%1").arg ( m_pMediaInfo->getBits ( ) );
          pCurrentJob->pSourceFileInfo->qsAudioFormat = m_pMediaInfo->getFormat     ( false );
          pCurrentJob->pSourceFileInfo->qsBPS         = m_pMediaInfo->getBPSString  ( false );
          pCurrentJob->pSourceFileInfo->qsSample      = m_pMediaInfo->getSampleString     ( );
        }
        setSemaphore ( false );// unblock since the next function can take a few seconds.

        //theImage = QImage(m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset ).size(), QImage::Format_RGB32);     //xxx
	    //theImage.fill(0);   //xxx
        theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );  //ooo
        //theImage.fill(QColor(23,45,135));   //xxx
        //QImage  theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );    //xxx
        //theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset ).copy(0,0,150,150);  //xxx
        //theImage ( m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset ).copy(0,0,150,150) );  //xxx
        //theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset ).scaled(163,223,Qt::KeepAspectRatio);  //xxx
        //theImage  = new QImage( m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset ) );  //xxx
        
        // Sanity check ... and copying the image over.
        if ( ( setSemaphore ( true ) ) && ( pCurrentJob->pOrigObject ) )  {
          QSize sizeButton = pCurrentJob->pSourceFileInfo->pPreview->size();
          if (theImage.isNull()) {    //ooo
          //if (theImage->isNull()) {     //xxx
            // *pCurrentJob->pSourceFileInfo->pPreview = QImage().fromMimeSource( "error.jpg" ).smoothScale(sizeButton, Qt::IgnoreAspectRatio);	//ooo
            *pCurrentJob->pSourceFileInfo->pPreview = QImage( ":/images/error.jpg" ).scaled(sizeButton, Qt::IgnoreAspectRatio);
	  } else {
            // *pCurrentJob->pSourceFileInfo->pPreview = theImage.smoothScale(sizeButton, Qt::KeepAspectRatio);	//ooo
            *pCurrentJob->pSourceFileInfo->pPreview = theImage.scaled(sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);		//xxx
            //*pCurrentJob->pSourceFileInfo->pPreview = theImage;		//xxx
            //*pCurrentJob->pSourceFileInfo->pPreview = QImage(theImage).scaled(sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);		//xxx
            //*pCurrentJob->pSourceFileInfo->pPreview = (m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset )).copy().scaled(sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);		//xxx
            //*pCurrentJob->pSourceFileInfo->pPreview = QImage(theImage->scaled(sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation));		//xxx
	  }	//oooo
          pCurrentJob->pSourceFileInfo->bUpdateInfo = true;
        }
        setSemaphore ( false );
      }
      break;
      case ExecuteJob::TYPE_IMAGE   :
      case ExecuteJob::TYPE_REQUEST : {
        m_pMediaInfo->setFileName ( pCurrentJob->qsFileName, false );
        setSemaphore ( false );	// unblock since the next function can take a few seconds.
        theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );  //ooo
        //QImage theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );  //xxx
        //*theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );   //xxx

        // Sanity check ...
        if ( ( setSemaphore ( true ) ) && (pCurrentJob->pOrigObject) )	{

          if ( theImage.isNull ( ) ) {    //ooo
          //if ( theImage->isNull ( ) ) {     //xxx
            //*pCurrentJob->pImage   = QImage().fromMimeSource( "error.jpg" );	//ooo
            *pCurrentJob->pImage   = QImage(":/images/error_xxx.jpg" );			//xxx
             pCurrentJob->bSuccess = false;
          }
          else
            *pCurrentJob->pImage = theImage;  //ooo
            //*pCurrentJob->pImage = *theImage;   //xxx
        }
        pCurrentJob->response ( );
        setSemaphore ( false );
      }
      break;
      case ExecuteJob::TYPE_SUBTITLES : {
        setSemaphore ( false ); // handling could take a few seconds ...
        handleSubtitles ( pCurrentJob );
        //setSemaphore ( false ); already cleared in handleSubtitles
      }
      break;
      case ExecuteJob::TYPE_MULTI_THUMB : {
        Utils theUtils;
        uint  iCount = 0;
        m_pMediaInfo->setFileName ( pCurrentJob->qsFileName, true );
        setSemaphore ( false );  // unblock since the next function can take a few seconds.

        // First we determint the length in MSec
        long iMSec = theUtils.getMsFromString ( m_pMediaInfo->getLengthString ( ) );
        pCurrentJob->qsFileName      = m_pMediaInfo->getLengthString ( );
        pCurrentJob->iMSecondsOffset = 0;
        if ( iMSec >= 1000 ) {
          // Since we have already the first preview we should increase the offset.
          if ( ! pCurrentJob->response ( ) )
              break;
          // max 100 thumbnails per video
          while ( ( pCurrentJob->iMSecondsOffset < iMSec ) && ( iCount++ <= 100 ) ) { 
            theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );  //ooo
            //QImage theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );  //xxx
            //*theImage = m_pMediaInfo->getScreenshot ( pCurrentJob->iMSecondsOffset );   //xxx
            // Sanity check ...
            if ( ( setSemaphore ( true ) ) && ( pCurrentJob->pOrigObject ) )	{
              if ( theImage.isNull ( ) ) {    //ooo
              //if ( theImage->isNull ( ) ) {     //xxx
               //*pCurrentJob->pImage   = QImage().fromMimeSource( "error.jpg" );	//ooo
               *pCurrentJob->pImage   = QImage( ":/images/error_xxx.jpg" );		//xxx
               pCurrentJob->bSuccess = false;
              }
              else
                *pCurrentJob->pImage = theImage;  //ooo
                //*pCurrentJob->pImage = *theImage;   //xxx
            }
            // response will increase the iMSecOffsetValue
            if ( ! pCurrentJob->response ( ) )  {
              setSemaphore ( false );
              break;
            }
            setSemaphore ( false );
          } // end while loop
          //printf ( "MediaSCanner : Exit While Loop <%s>\n\n",  pCurrentJob->qsFileName.ascii ( ) );
        }
        else { // Error media length wrong or invalid
          pCurrentJob->iMSecondsOffset = -1;
          pCurrentJob->response ( );
        }
      }
      break;
      default:
      break;
      }; // end switch statement.

      if ( setSemaphore ( true ) )  {	// Protect the Fifo ...
        //m_listExecuteFifo.remove  ( pCurrentJob );		//ooo
        m_listExecuteFifo.removeOne  ( pCurrentJob );		//xxx
        listOfExecutedJobs.append ( pCurrentJob );
        if ( pCurrentJob->pOrigObject )
          QApplication::postEvent ( pCurrentJob->pOrigObject, new QTimerEvent(MEDIASCANNER_EVENT + (int)pCurrentJob->theType ) );
      }
      setSemaphore ( false );
    } // end of while ( m_listExecuteFifo.count ( ) ...
        m_iWaitCounter = 50; // 50 * 100ms = 5 seconds
    while ( m_iWaitCounter-- > 0 )	//ooo
      //usleep ( 100000 );  // wait for 5 seconds. In this time the user could start another task ...			//ooo
      SleeperThread9::msleep ( 100 );  // wait for 5 seconds. In this time the user could start another task ...		//xxx
//printf ( "MediaScanner::run -=> Finished waitCounter\n" );
  }

  for (t=0;t<(uint)listOfExecutedJobs.count();t++)	//oxx
    delete listOfExecutedJobs[t];

  m_bHasFinished = true;

  delete m_pMediaInfo;
  m_pMediaInfo   = NULL;
}

void MediaScanner::handleSubtitles ( ExecuteJob *pCurrentJob )
{
  try {
    if ( ( ! pCurrentJob ) || 
	 ( ! pCurrentJob->pSubtitles ) ||
	 ( ! pCurrentJob->pSourceFileInfo ) )
      return;

    QFileInfo  fileInfo ( pCurrentJob->pSourceFileInfo->qsFileName );
    QString qsOutputFileName, qsBasePath, qsCommand, qsTool;
    Utils   theUtils;
    Import::Sub theImporter;
    //bool    bOkay;

    // Okay the subtitles should be named like $TMP/$PROJ/BASE_NAME/sub_<start>.png
    //qsBasePath = theUtils.getTempFile( fileInfo.baseName ( true ) );		//ooo
    qsBasePath = theUtils.getTempFile( fileInfo.baseName ( ) );			//xxx
    QDir tempDir ( qsBasePath );
    if ( ! tempDir.exists ( ) )
      tempDir.mkdir ( qsBasePath );

    qsOutputFileName = qsBasePath + "/" + fileInfo.baseName ( ) + ".sub";

    qsTool = theUtils.getToolPath ( QString ( "dv2sub" ) );
    if ( qsTool.length ( ) < 6 )
      qsTool = "dv2sub";

    qsCommand = QString ( "%1 --subtitles-out %2 %3" ).arg ( qsTool ).arg ( qsOutputFileName ).arg ( pCurrentJob->pSourceFileInfo->qsFileName );
    //if ( system ( qsCommand.utf8 ( ) ) == -1 )  { // failed ...	//ooo
    if ( system ( qsCommand.toUtf8 ( ) ) == -1 )  { // failed ...	//xxx
      // Note: no MessageBox from background thread ..
      return;
      //MessageBox::warning ( this, tr ( "Subtitle creation failed." ), tr ( "Could not create subttiles from source DV material<P>You can try invoking the following command manually : <BR><B>%1</B>" ).arg ( qsCommand ) );
    }

    if ( theImporter.readFile ( qsOutputFileName, pCurrentJob->fFPS ) ) {
      QList<Subtitles::entry *> listOfEntries = theImporter.m_listOfEntries;	//oxx
      theImporter.m_listOfEntries.clear ( );
      setSemaphore (  true ); // protect the coping of the list 
      pCurrentJob->pSubtitles->m_listOfSubtitles = listOfEntries;
      pCurrentJob->pSubtitles->m_subtitleState = Subtitles::STATE_META_INFO;
      setSemaphore ( false );
    }
  }
  catch ( ... ) {
    // Error handling ...
  };
}
