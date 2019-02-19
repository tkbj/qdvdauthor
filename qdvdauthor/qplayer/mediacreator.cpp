/***************************************************************************
    mediacreator.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Class hirachy ...


      QXineWidget            => Found on the web widget ...
   	   |
   	   +-> XineWidget       => implementation of interface to the QXineWidget
   	   |
    MediaInterface	=> pure virtual interface description
   	   |
   	   +-> MPLayerWidget    => implementation of interface to KmpMPlayerWidget
   	   |
     KmpMPLayerWidget        => Found on the web widget ...
    
    we have the following class hirarchiy:

    QXineWidget : public QWidget, public QThread (Xine player widget found on the web)
    +-> XineWidget : protected QXineWidget, public MediaInterface

    KmpMPlayer : public QWidget  (MPlayer player widget found on the web)
    +-> MPlayerWidget : protected KmpMPlayer, public MediaInterface

    VLCWindow : public QWidget  (VLC player widget found on the web)
    +-> VLCWidget : protected VLCWindow, public MediaInterface
    
****************************************************************************/
 
#include "mediacreator.h"
//#include "../mediascanner.h"		//ooo
#include "mediascanner.h"		//xxx
#include <QMessageBox>			//xxx
#include <cstdio>			//xxx
//#include "engines/mediainterface.h" already included in the header ...

// Here we include the header according to the set-up
#include "MEDIA_CONFIG.h"

#if (MPLAYER_SUPPORT == 1)
	#include "engines/mplayerinfo.h"
	#include "engines/mplayerwidget.h"
	#include "engines/qmplayerwidget.h"
#endif
#if (XINE_SUPPORT == 1)
	#include "engines/xineinfo.h"
	#include "engines/xinewidget.h"
	#include "engines/kxinewidget.h"
	#include "engines/qxinewidget.h"
	#include "engines/kaffeineswidget.h"
#endif
#if (VLC_SUPPORT == 1)
	#include "engines/vlcinfo.h"
	#include "engines/vlcwidget.h"
	//#include "engines/qvlcwidget.h"		//xxx
#endif
#include "engines/dummywidget.h"


QString MediaCreator::m_qsPreferredEngine = QString ("QXineWidget");
MediaScanner *MediaCreator::pPreviewObject = NULL;

MediaCreator::MediaCreator () 
{

}

MediaCreator::~MediaCreator () 
{
  delete pPreviewObject;    //xxx 
}

QString MediaCreator::getEngineDescription (QString qsEngineName)
{
  QString qsEngineInfo;
  if (qsEngineName == QString ("DummyWidget"))
    qsEngineInfo = QString ("This class is a empty implementation which does nothing.\n"
    "Choose Xine, MPlayer or VLC instead for some usefull functionality.");
  else if (qsEngineName == QString ("QMPlayerWidget"))
    qsEngineInfo = QString ("Using the MPlayer engine in shell wrappers\n"
    "Note: As of this version of MPlayer you can not display DVDMenus with it.\n"
    "Choose Xine or VLC instead for this task");
  else if (qsEngineName == QString ("QXineWidget"))
    qsEngineInfo = QString ("From an older version of the QDVDAuthor project.\n"
    "This version is working but no longer actively maintained.\n"
    "KXineWidget is preffered over this widget.");
  else if (qsEngineName == QString ("KXineWidget"))
    qsEngineInfo = QString ("From the kaffeine - project.\n"
    "This is the real widget form the kaffeine project and actively maintained.\n"
    "I will try to keep this version in synch with the lates version from kaffeine.");
  else if (qsEngineName == QString ("VLCWidget"))
    qsEngineInfo = QString ("From the Videolan - project.\n"
    "This widget is using the vidnewThumbnailRequesteolan media engine.\n"
    "It is also the least tested media engine in 'Q' DVD-Author.");
  else 
    qsEngineInfo = QString ("Engine <%1> unknown.").arg (qsEngineName);
  return qsEngineInfo;
}

QStringList MediaCreator::getAvailableEngines ()
{
  // Note: 1) The first entry needs to stay the DummyWidget
  //       2) The order will create the best match if the requested engine can not be created
  static QStringList listEngines;
  listEngines.clear();
  listEngines.append(QString("DummyWidget"));	// Dummy interface in case we encounter an error
#if (MPLAYER_SUPPORT == 1)
  listEngines.append(QString("QMPlayerWidget"));	// Self designed Widget -> interface class = MPlayerWidget
#endif
#if (XINE_SUPPORT == 1)
  listEngines.append(QString("KXineWidget"));	// From the kaffeine - project -> interface class = KaffeinesWidget
  listEngines.append(QString("QXineWidget"));	// From an older version of the QDVDAuthor project -> interface class = XineWidget
#endif
#if (VLC_SUPPORT == 1)
  listEngines.append(QString("QVLCWidget"));	// interface class = VLCWidget
#endif
  return listEngines;
}

bool MediaCreator::setPreferredEngine (QString qsPreferredEngine)
{
  QStringList listEngines = MediaCreator::getAvailableEngines();
  bool bFound = false;
  int t=0;
  // Sanity check if the requested engine is supported.
  for (t=0;t<(int)listEngines.count();t++)	{
    if (qsPreferredEngine == listEngines[t])
      bFound = true;
  }
  if (!bFound)	{
    // Here we put som brains into this.
    // SO we can not give the user what he wants
    // Lets check if we have a replacement ...
    m_qsPreferredEngine = listEngines[1];
    // Only if there is no replacement, 
    // then we default to the Dummy no function Widget
  if (listEngines[1] == "")
      m_qsPreferredEngine = listEngines[0];
    //printf ("Could not create %s engine, will default to %s\n\n", (const char *)qsPreferredEngine, (const char *)m_qsPreferredEngine);		//ooo
    printf ("Could not create %s engine, will default to %s\n\n", qsPreferredEngine.toLatin1().data(), m_qsPreferredEngine.toLatin1().data());	//xxx
    return false;
  }
  // All good to set the preferred engine
  m_qsPreferredEngine = qsPreferredEngine;
  return true;
}

QString MediaCreator::preferredEngine ()
{
  return m_qsPreferredEngine;
}

MediaInterface *MediaCreator::createPreferredWidget(QWidget *pParent, const char *pName, void *pExistingEngine)
{
  MediaInterface *pNewInterface = NULL;
  if (m_qsPreferredEngine == QString("QXineWidget"))
    pNewInterface = createXineWidget (pParent, pName, pExistingEngine);
  else if (m_qsPreferredEngine == QString("KXineWidget"))
    pNewInterface = createKXineWidget (pParent, pName, pExistingEngine);
  else if (m_qsPreferredEngine == QString("QMPlayerWidget"))
    pNewInterface = createMPlayerWidget (pParent, pName, pExistingEngine);
  else if (m_qsPreferredEngine == QString("VLCWidget"))		//ooo
  //else if (m_qsPreferredEngine == QString("QVLCWidget"))		//xxx
    pNewInterface = createVLCWidget (pParent, pName, pExistingEngine);
  else if (m_qsPreferredEngine == QString("DummyWidget"))
    pNewInterface = (MediaInterface *) new DummyWidget (pParent, "DummyWidget", pExistingEngine);
  if (!pNewInterface)	{
    printf ("\nError could not create preferred MediaEngine.\nCreate Dummy MediaWidget with no functionality\n\n");
    pNewInterface = (MediaInterface *) new DummyWidget (pParent, "DummyWidget", pExistingEngine);
  }
  return pNewInterface;
}

MediaInterface *MediaCreator::createMPlayerWidget(QWidget *pParent, const char *pName, void *pExistingEngine)
{
  MediaInterface *pMediaInterface = NULL;
  pParent = pParent;
  pName = pName;
  pExistingEngine = pExistingEngine;
#if (MPLAYER_SUPPORT == 1)
  pMediaInterface = (MediaInterface *) new MPlayerWidget (pParent, "MPlayerPart", pExistingEngine);
#endif
  return pMediaInterface;
}

MediaInterface *MediaCreator::createXineWidget(QWidget *pParent, const char *pName, void *pExistingEngine)
{
  MediaInterface *pMediaInterface = NULL;
  pParent = pParent;
  pName = pName;
  pExistingEngine = pExistingEngine;
#if (XINE_SUPPORT == 1)
  pMediaInterface = (MediaInterface *) new XineWidget (pParent, "XineWidget", pExistingEngine);
#endif
  return pMediaInterface;
}

MediaInterface *MediaCreator::createKXineWidget(QWidget *pParent, const char *pName, void *pExistingEngine)
{
  MediaInterface *pMediaInterface = NULL;
  pParent = pParent;
  pName = pName;
  pExistingEngine = pExistingEngine;
#if (XINE_SUPPORT == 1)
  pMediaInterface = (MediaInterface *) new KaffeinesWidget (pParent, "KaffeinesWidget", pExistingEngine);
#endif
  return pMediaInterface;
}

MediaInterface *MediaCreator::createVLCWidget (QWidget *pParent, const char *pName, void *pExistingEngine)
{
  MediaInterface *pMediaInterface = NULL;
  pParent = pParent;
  pName = pName;
  pExistingEngine = pExistingEngine;
#if (VLC_SUPPORT == 1)
  pMediaInterface = (MediaInterface *) new VLCWidget (pParent, "VLCPart", pExistingEngine);
#endif
  return pMediaInterface;
}

MediaInfo *MediaCreator::createInfo(void *pVoid)
{
  MediaInfo *pInfo = NULL;
#if (XINE_SUPPORT == 1)
  if ( (m_qsPreferredEngine == "QXineWidget") || (m_qsPreferredEngine == "KXineWidget") )
    pInfo = (MediaInfo *) new XineInfo (pVoid);
#endif
#if (MPLAYER_SUPPORT == 1)
  if (m_qsPreferredEngine == "QMPlayerWidget")
    pInfo = (MediaInfo *) new MPlayerInfo (pVoid);
#endif
#if (VLC_SUPPORT == 1)
  if (m_qsPreferredEngine == "VLCWidget")		//ooo
  //if (m_qsPreferredEngine == "QVLCWidget")		//xxx
    pInfo = (MediaInfo *) new VLCInfo (pVoid);
#endif
  if (m_qsPreferredEngine == "DummyWidget")
    pInfo = (MediaInfo *) new DummyInfo (pVoid);
  return pInfo;
} 

void MediaCreator::registerWithMediaScanner (QObject *pOrigObject, QString &qsFileName, QImage *pImage, long iMSecondsOffset)
{
  // The first action to take is a sanity check ...
  if ( (!pImage) || (!pOrigObject) )
    return;
  // First we create the ExecuteObject (Container to store the information) 
  ExecuteJob *pNewJob = new ExecuteJob (pOrigObject, qsFileName, pImage, iMSecondsOffset);
  MediaCreator::ensureMediaScannerExists ( );

  MediaCreator::pPreviewObject->append (pNewJob);
}

void MediaCreator::registerWithMediaScanner ( QObject *pOrigObject, SourceFileInfo *pSourceFileInfo, long iMSecondsOffset, bool bUpdateAllInfo )
{
  // THe first action to take is a sanity check ...
  if ( (!pSourceFileInfo) || (!pOrigObject) )
    return;
  // First we create the ExecuteObject (Container to store the information) 
  ExecuteJob *pNewJob = new ExecuteJob (pOrigObject, pSourceFileInfo, iMSecondsOffset, bUpdateAllInfo );
  MediaCreator::ensureMediaScannerExists ( );

  MediaCreator::pPreviewObject->append (pNewJob);
}

void MediaCreator::registerWithMediaScanner ( QObject *pOrigObject, SourceFileInfo *pSourceFileInfo, Subtitles *pSubtitles, float fFPS )
{
  // THe first action to take is a sanity check ...
  if ( ( ! pSourceFileInfo ) || ( ! pSubtitles ) )
    return;
  // First we create the ExecuteObject   ( Container to store the information ) 
  ExecuteJob *pNewJob = new ExecuteJob   ( pOrigObject, pSourceFileInfo, pSubtitles, fFPS );
  MediaCreator::ensureMediaScannerExists ( );
  MediaCreator::pPreviewObject->append   ( pNewJob );
}

void MediaCreator::registerWithMediaScanner ( ExecuteJob *pJob )
{
  // THe first action to take is a sanity check ...
  if ( ! pJob )
    return;

  MediaCreator::ensureMediaScannerExists ( );
  MediaCreator::pPreviewObject->append   ( pJob );
}

void MediaCreator::ensureMediaScannerExists ( )
{
  // Check if we need to create a new object ...
  if ( MediaCreator::pPreviewObject == NULL )
       MediaCreator::pPreviewObject = new MediaScanner;
  //else if ( ( ! MediaCreator::pPreviewObject->running     ( ) ) &&		//ooo
  else if ( ( ! MediaCreator::pPreviewObject->isRunning     ( ) ) && 		//xxx
	    (   MediaCreator::pPreviewObject->hasFinished ( ) ) ) {
    // If the object already exist and it has finished running, 
    // THEN delete the old and create a new one
    delete MediaCreator::pPreviewObject;
    MediaCreator::pPreviewObject = new MediaScanner;
  }
}

void MediaCreator::unregisterFromMediaScanner (QObject *pObject)
{
  if ( MediaCreator::pPreviewObject )
       MediaCreator::pPreviewObject->remove ( pObject );
}

void MediaCreator::unregisterFromMediaScanner (SourceFileInfo *pSourceFileInfo)
{
  if ( MediaCreator::pPreviewObject )
       MediaCreator::pPreviewObject->remove ( pSourceFileInfo );
}
