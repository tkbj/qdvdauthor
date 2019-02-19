/****************************************************************************
** KaffeinesWidget - class
**
** Created: Tue Dec 7 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class derives from the KXineWidget - class and implements the 
** MediaInterface functions ...
** 
****************************************************************************/

#include <stdlib.h>

//#include <qdir.h>	//ooo
#include <QDir>		//xxx
//#include <qimage.h>	//ooo
#include <QImage>	//xxx
//Added by qt3to4:
#include <QPixmap>

#include "dialogqxinesetup.h"		//ooo
#include "kaffeineswidget.h"
#include "xineinfo.h"
#include "../mediainfo.h"

KaffeinesWidget::KaffeinesWidget(QWidget *parent, const char *name, void *)	//pExistingEngine)
	: KXineWidget (parent, name, QString::null, QString::null, "auto", "xshm"), 
	  MediaInterface()
{
	m_qsCurrentVideoPlugin = QString ("xshm");
	m_qsCurrentAudioPlugin = QString ("auto");

	m_pMediaInfo = NULL; // NULL or hold all infos of the current/last media stream
	m_fVolume    = 0.0f;
	m_fPosition  = 0.0f;
	// 1:1 mapping of signals ...
	MediaInterface::connect ( (KXineWidget *)this, SIGNAL(signalNewPosition (int, const QString&)), 
                           (MediaInterface *)this, SIGNAL(signalNewPosition (int, const QString&)) );
	MediaInterface::connect ( (KXineWidget *)this, SIGNAL(signalNewPosition (long)), 
                           (MediaInterface *)this, SIGNAL(signalNewPosition (long)) );
//	MediaInterface::connect ( (KXineWidget *)this, SIGNAL(signalNewInfo (const QString&)), 
//                           (MediaInterface *)this, SIGNAL(signalNewInfo (const QString&)) );
	MediaInterface::connect ( (KXineWidget *)this, SIGNAL(signalPlaybackFinished ()), 
                           (MediaInterface *)this, SIGNAL(signalPlaybackFinished ())  );

// This is done in the original widget (KXineWidget) by changing te init file name ...
//	loadInitFile ();
}

KaffeinesWidget::~KaffeinesWidget ()
{
  if ( m_pMediaInfo )
	  delete m_pMediaInfo;
  m_pMediaInfo = NULL;
}

QWidget *KaffeinesWidget::getWidget ()
{
	return (QWidget *)this;
}

const char *KaffeinesWidget::getEngineName ()
{
	return "KXineWidget";
}

void *KaffeinesWidget::getMediaEngine ()
{
	return (void *)getXineEngine();
}

void KaffeinesWidget::setupDialog ()
{
	int t;
	// We open a dialog where the user
	// o	Sees the mime - types
	// o	sees the file extensions
	// o	can select the video driver
	// o	can select the audio driver ...
	/////////////////////////////////////////
	// Here we create the SetupDialog - object
	DialogQXineSetup *pDialog = new DialogQXineSetup (this);

// get lists of available audio and video output plugins
	xine_t *pXineEngine = (xine_t *)getXineEngine();
	QStringList listVideoPlugins;
	QStringList listAudioPlugins;
	QStringList listFileExtensions;
	QStringList listMimeTypes;

	const char *const *ppVideoPlugins = xine_list_video_output_plugins (pXineEngine);
	t = -1;
	while (ppVideoPlugins[++t])	{
		listVideoPlugins.append (ppVideoPlugins[t]);
//printf ("<%d><%s>\n", t, ppVideoPlugins[t]);
	}
	const char *const *ppAudioPlugins = xine_list_audio_output_plugins (pXineEngine);
	t = -1;
	while (ppAudioPlugins[++t])	{
		listAudioPlugins.append (ppAudioPlugins[t]);
//printf ("<%d><%s>\n", t, ppAudioPlugins[t]);
	}

	pDialog->setVideoPlugins   (listVideoPlugins, m_qsCurrentVideoPlugin);
	pDialog->setAudioPlugins   (listAudioPlugins, m_qsCurrentAudioPlugin);

	if (pDialog->exec() == QDialog::Rejected)
		return;
	if (m_qsCurrentVideoPlugin != pDialog->getVideoPlugin())
		slotSetVisualPlugin (pDialog->getVideoPlugin());
//	if (m_qsCurrentAudioPlugin != pDialog->getAudioPlugin())
//		SetAudioPlugin (pDialog->getAudioPlugin());

	m_qsCurrentVideoPlugin = pDialog->getVideoPlugin();
	m_qsCurrentAudioPlugin = pDialog->getAudioPlugin();

	saveInitFile ();
}

// This function is not used. The ini file is loaded in InitXine in KXineWidget.
void KaffeinesWidget::loadInitFile ()
{
	//QString configFile = QDir::homeDirPath();	//ooo
	QString configFile = QDir::homePath();		//xxx
	configFile.append(XINE_INIT_FILE);

	int iCounter = 0;
	xine_t *pXineEngine = (xine_t *)getXineEngine();
	while ( (pXineEngine == NULL) && (iCounter++ < 5) )   {
//		if (pXineEngine == NULL)
//			sleep (1);
		pXineEngine = (xine_t *)getXineEngine();
	}
	if (pXineEngine == NULL)
		return;

	QFile theFile (configFile);
	if (theFile.exists())
		//xine_config_load (pXineEngine, configFile);				//ooo
		xine_config_load (pXineEngine, configFile.toLatin1().data());		//xxx
	else	{
		//printf("No config file found, will create \n%s\n", (const char *)configFile);		//ooo
		printf("No config file found, will create \n%s\n", configFile.toLatin1().data());	//xxx
		// First we check if the directory exists or if we should create it first ...
		//QString qsDirPath = QDir::homeDirPath() + QString ("/.qdvdauthor");			//ooo
		QString qsDirPath = QDir::homePath() + QString ("/.qdvdauthor");			//xxx
		QDir theDir(qsDirPath);
		if (!theDir.exists())
			theDir.mkdir (qsDirPath);
		//xine_config_save  (pXineEngine, configFile);				//ooo
		xine_config_save  (pXineEngine, configFile.toLatin1().data());		//xxx
	}
}

void KaffeinesWidget::saveInitFile ()
{
	xine_cfg_entry_t audioConfig, videoConfig;
	xine_t *pXineEngine = (xine_t *)getXineEngine();
	if (!pXineEngine)
		return;

	//QString qsConfigFile = QDir::homeDirPath();		//ooo
	QString qsConfigFile = QDir::homePath();		//xxx
	qsConfigFile.append(XINE_INIT_FILE);

	xine_config_lookup_entry (pXineEngine, "gui.audiodriver", &audioConfig);
	xine_config_lookup_entry (pXineEngine, "gui.videodriver", &videoConfig);
	audioConfig.type = XINE_CONFIG_TYPE_STRING;
	videoConfig.type = XINE_CONFIG_TYPE_STRING;
	xine_config_register_string (pXineEngine, "gui.videodriver", "", "Videodriver to use (default: auto)", "This value holds Video file", 0 , NULL, NULL);
	xine_config_register_string (pXineEngine, "gui.audiodriver", "", "Audiodriver to use (default: auto)", "This value holds Audioo file", 0 , NULL, NULL);
	videoConfig.key = "gui.videodriver";
	audioConfig.key = "gui.audiodriver";
	//videoConfig.str_value = (char *)(const char *)m_qsCurrentVideoPlugin;		//ooo
	videoConfig.str_value = m_qsCurrentVideoPlugin.toLatin1().data();		//xxx
	//audioConfig.str_value = (char *)(const char *)m_qsCurrentAudioPlugin;		//ooo
	audioConfig.str_value = m_qsCurrentAudioPlugin.toLatin1().data();		//xxx
	xine_config_update_entry (pXineEngine, &videoConfig);
	xine_config_update_entry (pXineEngine, &audioConfig);

	//xine_config_save (pXineEngine, (const char *)qsConfigFile);		//ooo
	xine_config_save (pXineEngine, qsConfigFile.toLatin1().data());		//xxx
}

void KaffeinesWidget::playMRL ( const QString qsMRL )
{
  m_qsMRL = qsMRL;
  // Ensure the initialization is done ...
  clearQueue    ( );
  appendToQueue ( qsMRL );
  slotPlay      ( );
  m_bPaused = false;
}

bool KaffeinesWidget::openMRL ( const QString qsMRL )
{
  m_qsMRL = qsMRL;
  appendToQueue ( qsMRL );
  slotPlay      ( );
  stop          ( );

  return true;
}

void KaffeinesWidget::play ()
{
  if ( isPaused ( ) )
    slotSpeedPause ( );
  else
    slotPlay ( );
  m_bPaused = false;
}

void KaffeinesWidget::pause()
{
  slotSpeedPause();
  m_bPaused = true;
}

void KaffeinesWidget::stop ()
{
  m_posTimer.stop( );
  if ( m_lengthInfoTimer.isActive ( ) ) 
       m_lengthInfoTimer.stop ( );
  if ( isPlaying ( ) )
       xine_stop ( m_xineStream );
}

void KaffeinesWidget::setSpeed(float fValue)
{
  if (fValue > 1.0)
    slotSpeedFaster();
  else if (fValue == 1.0)
    slotSpeedNormal ();
  else if (fValue == 0.0)
    slotSpeedPause ();
  else if (fValue < 1.0)
    slotSpeedSlower();
}

void KaffeinesWidget::setVolume(float fValue)
{
  // Volume seems to work between [0 ..100]
  slotSetVolume((int)(fValue*100.0));
  return;
  // older version ... seems to blow volume away ...
  if (m_softwareMixer)
    slotSetVolume((int)(fValue*1000.0));
  else
    slotSetVolume((int)(fValue*100.0));
}

void KaffeinesWidget::setPosition(uint iPosition)
{
  if ( isSeekable ( ) )
    slotSeekToPosition ( iPosition );
}

void KaffeinesWidget::setPositionByTime ( uint iPosition )
{
  QTime seekTime;
  seekTime = seekTime.addSecs ( iPosition );
  slotSeekToTime ( seekTime );
}

void KaffeinesWidget::setNavigate( enNavigate nav )
{
  switch ( nav ) {
  case NavigateUp:
    slotDVDMenuUp ( );
  break;
  case NavigateDown:
    slotDVDMenuDown ( );
  break;
  case NavigateLeft:
    slotDVDMenuLeft ( );
  break;
  case NavigateRight:
    slotDVDMenuRight ( );
  break;
  case NavigateSelect:
    slotDVDMenuSelect ( );
  break;
 }
}

void KaffeinesWidget::setMenu ( enMenu menu )
{
  if ( menu == MenuRoot )
    slotMenuRoot ( );
  else if ( menu == MenuTitle )
    slotMenuTitle ( );
}

void KaffeinesWidget::setSubtitleTrack ( int iTrackNumber )
{
  slotSetSubtitleChannel ( iTrackNumber );
}

void KaffeinesWidget::setAudioTrack ( int iTrackNumber )
{
  slotSetAudioChannel ( iTrackNumber );
}

float KaffeinesWidget::volume               ()
{
  return 0.0f;
}

float KaffeinesWidget::position             ()
{
//	slotGetPosition();
	return 0.0f;
//	slotEmitLengthInfo();
}

QString KaffeinesWidget::supportedExtensions()
{
	return getSupportedExtensions();
}

void KaffeinesWidget::initMediaEngine ()
{
	initXine ();
//	loadInitFile ();
}

QImage KaffeinesWidget::getScreenshot ()
{
	return getScreenshot ( 0.0f );
}

QImage KaffeinesWidget::getScreenshot ( float fTime )
{
	if ( ! m_pMediaInfo )
		m_pMediaInfo = new XineInfo ( (void *) getXineEngine ( ) );
	QString qsFileName = getMRL ( );
	if ( m_pMediaInfo->getFileName ( ) != qsFileName )
	 	 m_pMediaInfo->setFileName ( getMRL ( ) );

	return m_pMediaInfo->getScreenshot ( (long)( fTime * 1000.0 ) );

	/*
	// fTime in Seconds ...
	QTime seekTime;
	seekTime.addSecs ((int)fTime);
	fTime -= (int)fTime;
	fTime *= 1000;
	seekTime.addMSecs ((int)fTime);
	slotSeekToTime (seekTime);
	return KXineWidget::getScreenshot ();
	*/
}

bool KaffeinesWidget::setScreenshot ( long iMSecOffset )
{
  QTime seekTime;
  seekTime = seekTime.addMSecs ( iMSecOffset );
  playMRL ( m_qsMRL );
  //slotPlay ( );
  slotSeekToTime ( seekTime );
  stop           ( );
  return true;
}

bool KaffeinesWidget::setScreenshot ( QPixmap & )
{
  return false;
}

// Non mandatory functions. 
void KaffeinesWidget::setAspectRatio (uint)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
}

void KaffeinesWidget::setEqualizer (uint, int)
{
	// Tune the sound 
}
	
bool KaffeinesWidget::hasChapters()
{
	return KXineWidget::hasChapters  ();;
}

void KaffeinesWidget::playChapter(int iHowFarFromHere)
{
	// 0=current / -x-=play x chapters before / +x=play current chapter + x
	if ( (iHowFarFromHere == 0) || (!hasChapters()) )
		return;
	else if (iHowFarFromHere > 0)
		for (;iHowFarFromHere > 0; iHowFarFromHere --)
			playNextChapter();
	else if (iHowFarFromHere < 0)
		for (;iHowFarFromHere < 0; iHowFarFromHere ++)
			playPreviousChapter();
}

bool KaffeinesWidget::isPlaying()
{
	if (KXineWidget::isPlaying() && !isPaused())
		return true;
	return false;
}




