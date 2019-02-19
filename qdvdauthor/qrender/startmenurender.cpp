/*****************************************************************************
 ** Class StartMenuRender
 **
 ** This class is part of QDVDAuthor suite and not really of QRender
 ** The purpose of this class is to 
 ** - gather all required information
 ** - create directories
 ** - write xml, and txt files
 ** - keep count of current running threads
 ** - start background threads (calling qrender executable)
 ** - report back status of current render progress
 ** 
 ** NOTE: this class handles all DVDMenus and not just one !!!
 *****************************************************************************/

 /****************************************************************************
 File structure ...
/tmp/ProjectName/+-> dragged Image files.
                 |
                 +->Movie 1/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie 2/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie 3/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie N/ --> clean.avi -=> Cleaned movie file or non-existant
                 |
                 +->Sub Menu 1_menu.mpg                       -=> final movie with sound and buttons.
                 +->Sub Menu 1/ +-> background/ +-> clip.wav
                 |              +-> background/ +-> clean.avi -=> cleaned up movie stream
                 |              +-> background/ +-> clip.avi  -=> Extracted timeframe of input-moviebackground
                 |              +-> background/ +-> menu.m2v  -=> final mpeg2enc menu movie (no sound)
                 |              +-> background/ +-> menu.mp2  -=> sound fileif neccesary
                 |              +-> background/ +-> menu.mpg  -=> final movie with sound
                 |              +-> background/ +-> menu.xml  -=> spumux - file
                 |              +-> background/ +-> -- Extracted images --
                 |              +-> background/ +-> rendered_-- Extracted images --
                 |              |
                 |              +-> Button 1/ +-> clip.wav
                 |              |             +-> clean.avi-=> cleaned up movie stream
                 |              |             +-> clip.avi -=> Extracted timeframe of [movie x]
                 |              |             +-> -- Extracted images -- 
                 |              +-> Button 2/ +-> clip.wav
                 |              |             +-> clip.avi -=> Extracted timeframe of [movie y]
                 |              |             +-> -- Extracted images -- 
                 |              +->MovieObject+-> clip.wav
                 |                            +-> clip.avi -=> Extracted timeframe of [movie z]
                 |                            +-> -- Extracted images -- 
                 |
                 +->Sub Menu 2_menu.mpg
                 +->Sub Menu 2/ +-> background/ +-> clip.wav
                                +-> background/ +-> clip.avi
                                +-> background/ +-> menu.m2v
                                +-> background/ +-> menu.mp2
                                +-> background/ +-> menu.mpg
                                +-> background/ +-> menu.xml
                                +-> background/ +-> -- Extracted images --
                                +-> background/ +-> rendered_-- Extracted images --
                                |
                                +-> Button 1/ +-> clip.wav
                                |             +-> clip.avi -=> Extracted timeframe of [movie a]
                                |             +-> -- Extracted images -- 
                                +-> Button 2/ +-> clip.wav
                                |             +-> clip.avi -=> Extracted timeframe of [movie b]
                                |             +-> -- Extracted images -- 
                                +-> Button 3/ +-> clip.wav
                                              +-> clip.avi -=> Extracted timeframe of [movie c]
                                              +-> -- Extracted images -- 
********************************************************************************/
 
#include <stdlib.h>

#include <qdir.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QTextStream>
//#include <Q3ValueList>	//oxx
#include <QMessageBox>		//xxx

#include "startmenurender.h"
//#include "uidialogextraction.h"	//ooo
#include "dialogextraction.h"		//xxx

#include "../global.h"
#include "../dvdmenu.h"
#include "../movieobject.h"

// First we init the static member variables ...
QList<StartMenuRender *> StartMenuRender::m_listRenderObjects;		//oxx

StartMenuRender::StartMenuRender (MovieObject *pMovieObject)
	: QThread ()
{
	m_pMovieObject = pMovieObject;
	m_pDVDMenu = NULL;
	m_bCalledFromFifo = false;
	m_bForceRender = false;
}

StartMenuRender::StartMenuRender (DVDMenu *pDVDMenu, bool bCalledFromFifo)
	: QThread ()
{
	// The calledFromFifo is neccessary so we won't delete it from the Fifo 
	// if scheduled 'normally' but we do want ot remove it from the Fifo
	// when we call it from the Main Thread.
	m_pDVDMenu = pDVDMenu;
	m_pMovieObject = NULL;
	m_bCalledFromFifo = bCalledFromFifo;
	m_bForceRender = false;
}

StartMenuRender::~StartMenuRender ()
{

}

void StartMenuRender::run ()
{
	if (m_pMovieObject)
		renderMovieObject ();
	else if (m_pDVDMenu)
		renderDVDMenu ();
}

void StartMenuRender::renderDVDMenu ()
{
	uint t;
	// At this point we assume that all button-clips have been 
	// extracted and are in their propper directories.
	// We also assume that the background has been extracted 
	// into frames and is located in their propper directory.
	//
	// So this function will render all objects onto the background frames.
	///////////////////////////////////////////////////////////
	// The first step is to check for a status file ...
	QString qsFullPath, qsMenuName;
	
	qsMenuName = QString ("NoMenuError");
	if (m_pDVDMenu)
		qsMenuName = m_pDVDMenu->getInterface()->qsMenuName;

	qsFullPath = QString ("%1/%2/%3/background").arg(Global::qsTempPath).arg(Global::qsProjectName).arg(qsMenuName);

	if (checkStatusFile (qsFullPath) == STATUS_DONE)	// Nothing to be done 
		return;
	// Next step is to check if there is a thread currently working on this menu ...
	// Note : m_listRenderObjects is a static object.
	for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
		StartMenuRender *pSMR = m_listRenderObjects[t];
		if (pSMR->dvdMenu () == m_pDVDMenu)	{
			// Check if the background thread is currently running and wait for it to finish ...
			//if (pSMR->running())	{	//ooo
			if (pSMR->isRunning())	{	//xxx
				pSMR->wait ();
				// and after the thread is done we can remove it from the Fifo
				//m_listRenderObjects.remove (pSMR);	//ooo
				m_listRenderObjects.removeOne (pSMR);	//xxx
				delete pSMR;
				return;
			}
			// otherwise we want to remove this request and do it now and here in this thread (also a background thread)
			else if (!m_bCalledFromFifo)	{
				//m_listRenderObjects.remove (pSMR);	//ooo
				m_listRenderObjects.removeOne (pSMR);	//xxx
				delete pSMR;
			}
		}
	}
	// Hello, still here, which means there is work to be done.
	dvdMenu()->renderMenu ();	//oooo
}

int StartMenuRender::checkStatusFile (QString qsFullPath)
{
	uint t;
	// Here we check if all's well ...
	QFile statusFile (qsFullPath + QString ("/") + QString (STATUS_FILE_NAME));
	// Okay if the file does not yet exists we have to do the full monty ...
	if (!statusFile.exists ())
		return 0;

	// First lets find out if the user wants to force doing the full monty ...
	if (m_bForceRender)	{
		int t;
		// and if so, we want to remove all files from this directory ...
		statusFile.remove ();
		QDir theDir (qsFullPath);
		for (t=0;t<(int)theDir.count();t++)
			theDir.remove (theDir[t]);
		return 0;
	}
	// Now we read in the status file (only a few lines anyways.
	QStringList statusLines;
	if ( statusFile.open( QIODevice::ReadOnly ) ) {
		QTextStream stream( &statusFile );
		while ( !stream.atEnd() )
			statusLines.append(stream.readLine()); // line of text excluding '\n'
		statusFile.close();
	}
	if (statusLines.count () < 1)
		return 0;
	// Okay we have a running number for the status's a'la " [00] startTime endTime / [01] cmd 1 ...
	// First we are intersted in the extracted time frame.
	QRegExp findTheTime("^\\[00\\]\\s+(\\S+)\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findTheTime.search (statusLines[t]) > -1)	{	//ooo
		if (findTheTime.indexIn (statusLines[t]) > -1)	{	//xxx
			QTime extractedOffset, extractedDuration;
			extractedOffset   = extractedOffset.fromString   ( findTheTime.cap(1) );
			extractedDuration = extractedDuration.fromString ( findTheTime.cap(2) );
			// Check if the time frame has changed between the extraction and now ...
			if ( (m_pMovieObject->offset () != extractedOffset) || (m_pMovieObject->duration() != extractedDuration) )
				return 0;
		}
	}
	// Next we check if the Video STream has been cleaned okay.
	QRegExp findClean("^\\[02\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findClean.search (statusLines[t]) > -1)	{	//ooo
		if (findClean.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findClean.cap(1) != QString ("Okay."))
				return 1;
		}
	}
	// So there have been problems with the extraction of the clip (the timeframe)
	QRegExp findClip("^\\[04\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findClip.search (statusLines[t]) > -1)	{	//ooo
		if (findClip.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findClip.cap(1) != QString ("Okay."))	{
				// Since the clean'ed up file will get deleted we should 
				// generate it again, thus return 1 rather then return 2
				return 1;
			}
		}
	}
	// Checking if the audio file exists
	QRegExp findAudio("^\\[06\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findAudio.search (statusLines[t]) > -1)	{	//ooo
		if (findAudio.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findAudio.cap(1) != QString ("Okay."))
				return 3;
		}
	}
	// Here we check if the frames have been extracted
	QRegExp findExtract("^\\[08\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findExtract.search (statusLines[t]) > -1)	{	//ooo
		 if (findExtract.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findExtract.cap(1) != QString ("Okay."))
				return 4;
		}
	}
	// Okay just to be nice, did rm work ? 
	QRegExp findRM("^\\[10\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findRM.search (statusLines[t]) > -1)	{	//ooo
		if (findRM.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findRM.cap(1) != QString ("Okay."))
				return 5;
		}
	}
	QRegExp findDone("^\\[11\\]\\s+(\\S+)");
	for (t=0;t<(uint)statusLines.count ();t++)	{
		//if (findDone.search (statusLines[t]) > -1)	{	//ooo
		if (findDone.indexIn (statusLines[t]) > -1)	{	//xxx
			if (findDone.cap(1) == QString ("Done."))
				return 6;
		}
	}
	// Nothing so far, let us do it all over again.
	return 0;
}

void StartMenuRender::renderMovieObject ()
{
	QFileInfo fileInfo;
	int     iStatusCheck;
	QTime   timeEndpos, zeroTime;
	QString qsFullPath, qsMenuName, qsCommand, qsOffset, qsEndpos, qsFormat;
	qsFormat = QString ("hh:mm:ss.zzz");

	// This is the separate thread which extracts the images of the movie clips for this menu
	// (after generating the status / xml - files)
	// First we should generate the directories to store the data in ...
	createDirectories ();
 
	// then we create the images
	qsMenuName = QString ("NoMenuError");
	if (m_pMovieObject->dvdMenu())
		qsMenuName = m_pMovieObject->dvdMenu()->getInterface()->qsMenuName;

	qsFullPath = QString ("%1/%2/%3/%4").arg ( Global::qsTempPath ).arg ( Global::qsProjectName ).arg ( qsMenuName ).arg ( m_pMovieObject->extractionPath ( ) );

	// Okay we should check the status file ...
	iStatusCheck = checkStatusFile (qsFullPath);

	// First we move the current status file to a backup file.
	qsCommand = QString ("mv \"%1/%2\" \"%3/%4.prev\"").arg(qsFullPath).arg(QString (STATUS_FILE_NAME)).arg(qsFullPath).arg(QString (STATUS_FILE_NAME));
	//if ( system (qsCommand) == -1 )			//ooo
	if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
		return;

	QFile statusFile (qsFullPath + QString ("/") + QString (STATUS_FILE_NAME));
	// Now we read in the status ...
	if ( !statusFile.open( QIODevice::ReadWrite ) ) 
		return;
	QTextStream statusStream ( &statusFile );

	statusStream << QString ("-+-+-+-+-+-+ Starting new extraction of file %1 -+-+-+-+-+-+ \n").arg(m_pMovieObject->fileName());

	qsOffset   = m_pMovieObject->offset   ( ).toString ( qsFormat );
	//timeEndpos = m_pMovieObject->offset   ( ).addMSecs ( zeroTime.msecsTo ( m_pMovieObject->duration( ) ) );
	// endpos is duration in conjunction with -ss
	qsEndpos   = m_pMovieObject->duration ( ).toString ( qsFormat );
	statusStream << "[00] " << qsOffset << " " << qsEndpos << "\n";
	statusFile.flush();

	// 1) extracting the wanted time frame ...
	//    mencoder -oac pcm -ovc lavc -ss 0 -endpos 10 -o "snl/clip.avi" "snl/clean.avi"
	qsCommand = QString ("mencoder -oac pcm -ovc lavc -ss %1 -endpos %2 -o \"%3/clip.avi\" \"%4\" 2>/dev/null").arg(qsOffset).arg(qsEndpos).arg(qsFullPath).arg( m_pMovieObject->fileName ( ) );
	statusStream << "[01] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 2)  {
		//if ( system (qsCommand) == -1 )			//ooo
		if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
	fileInfo.setFile(qsFullPath + QString ("/clip.avi"));
	if (fileInfo.exists())
		statusStream << "[02] Okay.\n";
	else
		statusStream << "[02] Error.\n";

	// 2) Cleaning the movie material ...
	//    mencoder -oac pcm -ovc lavc -o "snl/clean.avi" "./snl.mpg" 
	qsCommand = QString ("mencoder -oac pcm -ovc lavc -o \"%1/clean.avi\" \"%2/clip.avi\" 2>/dev/null").arg(qsFullPath).arg( qsFullPath );
	statusStream << "[03] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 1)  {
		//if ( system (qsCommand) == -1 )			//ooo
		if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
	fileInfo.setFile(qsFullPath + QString ("/clean.avi"));
	if ( (fileInfo.exists()) || (iStatusCheck > 0) )
		statusStream << "[04] Okay.\n";
	else
		statusStream << "[04] Error.\n";
/*
	// 1) Cleaning the movie material ...
	//    mencoder -oac pcm -ovc lavc -o "snl/clean.avi" "./snl.mpg"
	qsCommand = QString ("mencoder -oac pcm -ovc lavc -o \"%1/clean.avi\" \"%2\" 2>/dev/null").arg(qsFullPath).arg(m_pMovieObject->fileName());
	statusStream << "[01] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 1)  {
		if ( system (qsCommand) == -1 )
			return;
	}
	fileInfo.setFile(qsFullPath + QString ("/clean.avi"));
	if ( (fileInfo.exists()) || (iStatusCheck > 0) )
		statusStream << "[02] Okay.\n";
	else
		statusStream << "[02] Error.\n";

	// 2) extracting the wanted time frame ...
	//    mencoder -oac pcm -ovc lavc -ss 0 -endpos 10 -o "snl/clip.avi" "snl/clean.avi"
	qsCommand = QString ("mencoder -oac pcm -ovc lavc -ss %1 -endpos %2 -o \"%3/clip.avi\" \"%4/clean.avi\" 2>/dev/null").arg(qsOffset).arg(qsDuration).arg(qsFullPath).arg(qsFullPath);
	statusStream << "[03] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 2)  {
		if ( system (qsCommand) == -1 ) 
			return; 
	} 
	fileInfo.setFile(qsFullPath + QString ("/clip.avi"));
	if (fileInfo.exists())
		statusStream << "[04] Okay.\n";
	else
		statusStream << "[04] Error.\n";
*/
	// 3) extracting audio (audiodump.wav) : 
	//    mplayer -ao pcm -vo null -vc dummy -aofile "snl/clip.wav" "snl/clip.avi"
	// mplayer changed switches ... so no more aofile=...
	//qsCommand = QString ("mplayer -ao pcm -vo null -vc dummy -aofile \"%1/clip.wav\" \"%2/clip.avi\" 2>/dev/null").arg(qsFullPath).arg(qsFullPath);
	qsCommand = QString ("mplayer -ao pcm:file=\"%1/clean.wav\" -vo null -vc dummy \"%2/clean.avi\" 2>/dev/null").arg(qsFullPath).arg(qsFullPath);
	statusStream  << "[05] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 3)  {
		//if ( system (qsCommand) == -1 )			//ooo
		 if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
	fileInfo.setFile(qsFullPath + QString ("/clip.wav"));
	if (fileInfo.exists())
		statusStream << "[06] Okay.\n";
	else
		statusStream << "[06] Error.\n";

	// 4) extracting 300 frames from background.mwv
	//    mplayer -ao null -vo jpeg:outdir="./snl" "snl/clip.avi"
	qsCommand = QString ("rm \"%1/*.jpg\"").arg(qsFullPath);
	if (iStatusCheck < 4)  {
		//if ( system (qsCommand) == -1 )			//ooo
		if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
		
	qsCommand = QString ("mplayer -ao null -vo jpeg:outdir=\"%1\" \"%2/clean.avi\" 2>/dev/null").arg(qsFullPath).arg(qsFullPath);
	statusStream  << "[07] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 4)  {
		//if ( system (qsCommand) == -1 )			//ooo
		if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
	QDir theDir (qsFullPath);
	//theDir.setNameFilter ("*.jpg");			//ooo
	theDir.setNameFilters (QStringList(QString("*.jpg")));	//xxx
	if (theDir.count() > 0)
		statusStream << "[08] Okay.\n";
	else
		statusStream << "[08] Error.\n";

	// 5) rm snl/clip.avi snl/clean.avi
//	qsCommand = QString ("rm \"%1/clip.avi\" \"%2/clean.avi\" 2>/dev/null").arg(qsFullPath).arg(qsFullPath);
	qsCommand = QString ("rm \"%1/clean.avi\" 2>/dev/null").arg(qsFullPath);
	statusStream  << "[09] " << QDate::currentDate().toString() << " " << QTime::currentTime().toString() << " cmd > " << qsCommand << "\n";
	statusFile.flush();
	if (iStatusCheck < 5)  {
		//if ( system (qsCommand) == -1 )			//ooo
		if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
			return;
	}
	fileInfo.setFile(qsFullPath + QString ("/clip.avi"));
	if (fileInfo.exists())
		statusStream << "[10] Okay.\n";
	else
		statusStream << "[10] Error.\n";

	m_pMovieObject->setExtracted(true);
	statusStream  << "[11] Done.\n";
	statusFile.flush();
	// And last but not least we close the status file again.
	statusFile.close();
}

void StartMenuRender::createDirectories ()
{ 
	QString qsCommand, qsFullPath, qsMenuName;
	QFileInfo fileInfo;
	QDir theTempDir;
	// Here we create the neccesary directories for this MovieObject
	// Note that for the movies we follow the scheme :
	// /temp-Path/ProjectName/MenuName/MovieName/0000001.jpg
	///////////////////////////////////////////////////////
//	Global::qsProjectFileName = QString (PROJECT_FILENAME);
//	Global::qsProjectName     = QString (PROJECT_NAME);
//	Global::qsTempPath        = QString ("/tmp");

	qsMenuName = QString ( "NoMenuError" );
	if ( m_pMovieObject->dvdMenu ( ) )
		qsMenuName = m_pMovieObject->dvdMenu()->getInterface()->qsMenuName;

	qsFullPath = QString ( "%1/%2/%3/%4" ).arg ( Global::qsTempPath ).arg ( Global::qsProjectName ).arg ( qsMenuName ). arg ( m_pMovieObject->extractionPath ( ) );

	qsCommand  = QString ( "mkdir -p '%1'" ).arg ( qsFullPath );
	//if ( system ( (const char *)qsCommand ) == -1 )	//ooo
	if ( system ( qsCommand.toLatin1().data() ) == -1 )	//xxx
		return;
	// and now we check that the path has been created succesfully
	theTempDir.setPath ( qsFullPath );
	if ( ! theTempDir.exists ( ) )  {
		QMessageBox::warning(NULL, QObject::tr("Could not create Path"), QObject::tr("Could not create Path \n%1").arg(qsFullPath), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
}

void StartMenuRender::createQRenderXml  ()
{

}

void StartMenuRender::executeQRender    ()
{

}

void StartMenuRender::registerToExtract (MovieObject *pMovieObject)
{
  // This function will add the MovieObject to the Fifo and 
  // o Check if there are threads which are done and delete them
  // o Add the MovieObject to the pipe 
  // o Start as many threads as there are allowed through Global::iMaxRunningThreads.
//printf ("StartMenuRender::registerToExtract -> And here we go out into the sea of Threads independand and free, to create and live ...\n");
  if (!pMovieObject)
    return;
  uint t, iCurrentRunningThreads;
  bool bAlreadyRegistered = false;

  // Here we do the same for the MovieObject
  bAlreadyRegistered = false;
  iCurrentRunningThreads = 0;
  for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
    // First let us get the DVDMenu pointer and append it to the listOfDVDMenus to be rendered ...
    if (m_listRenderObjects[t]->movieObject() == pMovieObject)
      bAlreadyRegistered = true;
    // And since we are running through the list we can also get the count of currently running threads ...
    //if (m_listRenderObjects[t]->running())	//ooo
    if (m_listRenderObjects[t]->isRunning())	//xxx
      iCurrentRunningThreads ++;
  }
  // Okay let us append the new MovieObject to the list ...
  if (!bAlreadyRegistered)	{
    // Now we create a StartMenuRenderObject wrapped around the MovieObject ...
    StartMenuRender *pNewRenderObject = new StartMenuRender (pMovieObject);
    m_listRenderObjects.append (pNewRenderObject);
  }
  // Then we check which threads can be started, deleted or ignored ...
  for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
    //if (m_listRenderObjects[t]->running())	//ooo
    if (m_listRenderObjects[t]->isRunning())	//xxx
      continue;
    //else if (m_listRenderObjects[t]->finished())	{	//ooo
    else if (m_listRenderObjects[t]->isFinished())	{	//xxx
      StartMenuRender *pSMR = m_listRenderObjects[t];
      //m_listRenderObjects.remove (pSMR);	//ooo
      m_listRenderObjects.removeOne (pSMR);	//xxx
      delete pSMR;
    }
    else if (iCurrentRunningThreads < Global::iMaxRunningThreads)	{
        
#if (QT_VERSION > 0x0301FF)
      m_listRenderObjects[t]->start (QThread::LowPriority);
#else
      m_listRenderObjects[t]->start ();
#endif
      iCurrentRunningThreads ++;
    }
  }
}

void StartMenuRender::waitForExtraction (DVDMenu *pDVDMenu)
{
	uint t, iCurrentRunningThreads = 0;
	bool bInnerLoop = true;
	// This function will wait until all requests for the given Menu are done and over with ...
	// I.e. All MovieObjects need to be extracted before we continue.
	// Then we check which threads can be started, deleted or ignored ...
	for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
		//if (m_listRenderObjects[t]->running())	//ooo
		 if (m_listRenderObjects[t]->isRunning())	//xxx
			iCurrentRunningThreads ++;
		// first we should remove the finished tasks
		//if (m_listRenderObjects[t]->finished ())	{	//ooo
		if (m_listRenderObjects[t]->isFinished ())	{	//xxx
			StartMenuRender *pSMR = m_listRenderObjects[t];
			//m_listRenderObjects.remove (pSMR);	//ooo
			m_listRenderObjects.removeOne (pSMR);	//xxx
			delete pSMR;
		}
	}

	//uiDialogExtraction *pDialog = NULL;		//ooo
	DialogExtraction *pDialog = NULL;		//xxx
	// Then we check which threads can be started, deleted or ignored ...
	QList <StartMenuRender *> listToDelete;		//oxx
	while (bInnerLoop == true)	{
		bInnerLoop = false;
		// So we go through the full list
		for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
			MovieObject *pMovieObject = m_listRenderObjects[t]->movieObject();
			// first we should mark the finished tasks for removal
			//if (m_listRenderObjects[t]->finished ())	{	//ooo
			if (m_listRenderObjects[t]->isFinished ())	{	//xxx
				listToDelete.append (m_listRenderObjects[t]);
				iCurrentRunningThreads --;
				pMovieObject = NULL;
			}
			// We check if we have a MovieObject 
			if (pMovieObject)	{
				// and if we find a MovieObject that belongs to the specified Menu the better :)
				if (pMovieObject->dvdMenu() == pDVDMenu)	{
					bInnerLoop = true;	// Naehhhh lets try again later 
					// we wait ...
//					if (m_listRenderObjects[t]->running   ())
//						m_listRenderObjects[t]->wait  ();
					//if (m_listRenderObjects[t]->running   ())	{	//ooo
					if (m_listRenderObjects[t]->isRunning   ())	{	//xxx
						if ( ! pDialog ) 	{
							//pDialog = new uiDialogExtraction (NULL);	//ooo
							pDialog = new DialogExtraction (NULL);		//xxx
							pDialog->setMenuName (pDVDMenu->name());
							pDialog->show ();
						}
						MovieObject *pMovieObject = m_listRenderObjects[t]->movieObject();
						if (pMovieObject)	{
							QFileInfo fileInfo (pMovieObject->fileName());
							pDialog->setObjectName ( fileInfo.fileName());
						}
						else
							pDialog->m_pTextObject->hide();
						//while (m_listRenderObjects[t]->running   ())	{	//ooo
						while (m_listRenderObjects[t]->isRunning   ())	{	//xxx
							QTimer::singleShot (900, pDialog, SLOT(slotAddASecond()));
							
							sleep (1);	// check back in a sec.
							//qApp->processEvents (1000);				//ooo
							qApp->processEvents (QEventLoop::AllEvents, 1000);	//xxx
						}
					}
					else if ( (iCurrentRunningThreads < Global::iMaxRunningThreads) && 
						  //(!m_listRenderObjects[t]->finished ()) )	{		//ooo
						  (!m_listRenderObjects[t]->isFinished ()) )	{		//xxx
						m_listRenderObjects[t]->start ();// (QThread::LowPriority);
						iCurrentRunningThreads ++;
					}
				}
			}
		}
		for (t=0;t<(uint)listToDelete.count();t++)	{
			//m_listRenderObjects.remove (listToDelete[t]);		//ooo
			m_listRenderObjects.removeOne (listToDelete[t]);	//xxx
			delete listToDelete[t];
		}
		listToDelete.clear();
	}
	delete pDialog;
}

/* Old version, which was blocking while waiting.
void StartMenuRender::waitForExtraction (DVDMenu *pDVDMenu)
{
	uint t, iCurrentRunningThreads = 0;
	bool bInnerLoop = true;
	// This function will wait until all requests for the given Menu are done and over with ...
	// I.e. All MovieObjects need to be extracted before we continue.
	// Then we check which threads can be started, deleted or ignored ...
	for (t=0;t<m_listRenderObjects.count();t++)	{
		if (m_listRenderObjects[t]->running())
			iCurrentRunningThreads ++;
		// first we should remove the finished tasks
		if (m_listRenderObjects[t]->finished ())	{
			StartMenuRender *pSMR = m_listRenderObjects[t];
			m_listRenderObjects.remove (pSMR);
			delete pSMR;
		}
	}
	// Then we check which threads can be started, deleted or ignored ...
	QValueList <StartMenuRender *> listToDelete;
	while (bInnerLoop == true)	{
		bInnerLoop = false;
		// So we go through the full list
		for (t=0;t<m_listRenderObjects.count();t++)	{
			MovieObject *pMovieObject = m_listRenderObjects[t]->movieObject();
			// first we should mark the finished tasks for removal
			if (m_listRenderObjects[t]->finished ())	{
				listToDelete.append (m_listRenderObjects[t]);
				iCurrentRunningThreads --;
				pMovieObject = NULL;
			}
			// We check if we have a MovieObject 
			if (pMovieObject)	{
				// and if we find a MovieObject that belongs to the specified Menu the better :)
				if (pMovieObject->dvdMenu() == pDVDMenu)	{
					bInnerLoop = true;	// Naehhhh lets try again later 
					// we wait ...
					if (m_listRenderObjects[t]->running   ())
						m_listRenderObjects[t]->wait  ();
					else if ( (iCurrentRunningThreads < Global::iMaxRunningThreads) && 
						  (!m_listRenderObjects[t]->finished ()) )	{
						m_listRenderObjects[t]->start ();// (QThread::LowPriority);
						iCurrentRunningThreads ++;
					}
				}
			}
		}
		for (t=0;t<listToDelete.count();t++)	{
			m_listRenderObjects.remove (listToDelete[t]);
			delete listToDelete[t];
		}
		listToDelete.clear();
	}
}
*/

void StartMenuRender::registerToRenderMenu (DVDMenu *pDVDMenu)
{
	// This function will add the DVDMenu to the Fifo and 
	// o Check if there are threads which are done and delete them
	// o Add the MovieObject to the pipe 
	// o Start as many threads as there are allowed through Global::iMaxRunningThreads.
	//
	// Note: this function is not yet used (01/12/2005) but the function renderDVDMenu
	//       is called in the main thread when creating the DVD in order to be in synch with the 
	//       other generated files.
	// Note: this function will later be used to allow the user to render the menu as a background thread
	//       to preview the result.
//printf ("StartMenuRender::registerToRenderMenu -> And here we go out into the sea of Threads independand and free, to create and live ...\n");
	if (!pDVDMenu)
		return;
	uint t, iCurrentRunningThreads;
	bool bAlreadyRegistered = false;
	
	// Here we do the same for the MovieObject
	bAlreadyRegistered = false;
	iCurrentRunningThreads = 0;
	for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
		// First let us get the DVDMenu pointer and append it to the listOfDVDMenus to be rendered ...
		if (m_listRenderObjects[t]->dvdMenu () == pDVDMenu)
			bAlreadyRegistered = true;
		// And since we are running through the list we can also get the count of currently running threads ...
		//if (m_listRenderObjects[t]->running())	//ooo
		if (m_listRenderObjects[t]->isRunning())	//xxx
			iCurrentRunningThreads ++;
	}
	// Okay let us append the new MovieObject to the list ...
	if (!bAlreadyRegistered)	{
		// Now we create a StartMenuRenderObject wrapped around the MovieObject ...
		StartMenuRender *pNewRenderObject = new StartMenuRender (pDVDMenu, true);
		m_listRenderObjects.append (pNewRenderObject);
	}
	// Then we check which threads can be started, deleted or ignored ...
	for (t=0;t<(uint)m_listRenderObjects.count();t++)	{
		//if (m_listRenderObjects[t]->running())	//ooo
		if (m_listRenderObjects[t]->isRunning())	//xxx
			continue;
		//else if (m_listRenderObjects[t]->finished())	{	//ooo
		else if (m_listRenderObjects[t]->isFinished())	{	//xxx
			//m_listRenderObjects.remove (m_listRenderObjects[t]);		//ooo
			m_listRenderObjects.removeOne (m_listRenderObjects[t]);		//xxx
			delete m_listRenderObjects[t];
		}
		else if (iCurrentRunningThreads < Global::iMaxRunningThreads)	{
#if (QT_VERSION > 0x0301FF)
			m_listRenderObjects[t]->start (QThread::LowPriority);
#else
			m_listRenderObjects[t]->start ();
#endif
			iCurrentRunningThreads ++;
		}
	}
}
/*
QThread::IdlePriority - scheduled only when no other threads are running. 
QThread::LowestPriority - scheduled less often than LowPriority. 
QThread::LowPriority - scheduled less often than NormalPriority. 
QThread::NormalPriority - the default priority of the operating system. 
QThread::HighPriority - scheduled more often than NormalPriority. 
QThread::HighestPriority - scheduled more often then HighPriority. 
QThread::TimeCriticalPriority - scheduled as often as possible. 
QThread::InheritPriority - use the same priority as the creatin
*/
MovieObject *StartMenuRender::movieObject ()
{
	return m_pMovieObject;
}

DVDMenu *StartMenuRender::dvdMenu ()
{
	return m_pDVDMenu;
}
