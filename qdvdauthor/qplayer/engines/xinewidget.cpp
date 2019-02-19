/****************************************************************************
** XineWidget - class
**
** Created: Tue Nov 16 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class derives from the QXineWidget - class and implements the 
** MediaInterface functions ...
** 
****************************************************************************/

#include <stdlib.h>

#include <qdir.h>
#include <qimage.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QPixmap>
#include <QTime>		//xxx
#include <QMessageBox>		//xxx

#include "dialogqxinesetup.h"
#include "xinewidget.h"

XineWidget::XineWidget(QWidget *parent, const char *name, void *pExistingEngine)
	: QXineWidget(parent, pExistingEngine, name), MediaInterface()
{
	m_qsCurrentVideoPlugin = QString ("xshm");
	m_qsCurrentAudioPlugin = QString ("auto");

	m_pMediaInfo = NULL; // NULL or hold all infos of the current/last media stream
	m_fVolume    = 0.0f;
	m_fPosition  = 0.0f;
	// 1:1 mapping of signals ...
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalNewPosition (int, const QString&)), 
                           (MediaInterface *)this, SIGNAL(signalNewPosition (int, const QString&)) );
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalNewPosition (long)), 
                           (MediaInterface *)this, SIGNAL(signalNewPosition (long)) );
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalNewInfo (const QString&)), 
                           (MediaInterface *)this, SIGNAL(signalNewInfo (const QString&)) );
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalPlaybackFinished ()), 
                           (MediaInterface *)this, SIGNAL(signalPlaybackFinished ())  );
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalToggleFullscreen ()), 
                           (MediaInterface *)this, SIGNAL(signalToggleFullscreen ())  );
	MediaInterface::connect ( (QXineWidget *)this, SIGNAL(signalQuit ()), 
                           (MediaInterface *)this, SIGNAL(signalQuit ())  );

// This is done in the original widget (QXineWidget) by changing the init file name ...
//	loadInitFile ();
}

XineWidget::~XineWidget ()
{

}

QWidget *XineWidget::getWidget        ()
{
	return (QWidget *)this;
}

const char *XineWidget::getEngineName ()
{
	return "QXineWidget";	// has to match MediaCreator::getAvailableEngines - strings
}

void *XineWidget::getMediaEngine ()
{
	return (void *)GetXineEngine();
}

void XineWidget::setupDialog ()
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
	xine_t *pXineEngine = (xine_t *)GetXineEngine();
	QStringList listVideoPlugins;
	QStringList listAudioPlugins;
	QStringList listFileExtensions;
	QStringList listMimeTypes;

	const char *const *ppVideoPlugins = xine_list_video_output_plugins (pXineEngine);
	t = -1;
	while (ppVideoPlugins[++t])	{
		listVideoPlugins.append (ppVideoPlugins[t]);
	}
	const char *const *ppAudioPlugins = xine_list_audio_output_plugins (pXineEngine);
	t = -1;
	while (ppAudioPlugins[++t])	{
		listAudioPlugins.append (ppAudioPlugins[t]);
	}

	pDialog->setVideoPlugins   (listVideoPlugins, m_qsCurrentVideoPlugin);
	pDialog->setAudioPlugins   (listAudioPlugins, m_qsCurrentAudioPlugin);

	if (pDialog->exec() == QDialog::Rejected)
		return;
	if (m_qsCurrentVideoPlugin != pDialog->getVideoPlugin())
		SetVisualPlugin (pDialog->getVideoPlugin());
//	if (m_qsCurrentAudioPlugin != pDialog->getAudioPlugin())
//		SetAudioPlugin (pDialog->getAudioPlugin());

	m_qsCurrentVideoPlugin = pDialog->getVideoPlugin();
	m_qsCurrentAudioPlugin = pDialog->getAudioPlugin();

	saveInitFile ();
}

// This function is not used. The ini file is loaded in InitXine in QXineWidget.
void XineWidget::loadInitFile ()
{
	//QString configFile = QDir::homeDirPath();	//ooo
	QString configFile = QDir::homePath();		//xxx
	configFile.append(XINE_INIT_FILE);

	int iCounter = 0;
	xine_t *pXineEngine = (xine_t *)GetXineEngine();
	while ( (pXineEngine == NULL) && (iCounter++ < 5) )   {
		if (pXineEngine == NULL) {
			//sleep (1);				//ooo
			SleeperThread3::msleep ( 1000 );	//xxx
		}
		pXineEngine = (xine_t *)GetXineEngine();
	}
	if (pXineEngine == NULL)
		return;

	QFile theFile (configFile);
	if (theFile.exists())
		//xine_config_load (pXineEngine, configFile);			//ooo
		xine_config_load (pXineEngine, configFile.toLatin1().data());	//xxx
	else	{
		//printf("No config file found, will create \n%s\n", (const char *)configFile);		//ooo
		printf("No config file found, will create \n%s\n", configFile.toLatin1().data());	//xxx
		// First we check if the directory exists or if we should create it first ...
		//QString qsDirPath = QDir::homeDirPath() + QString ("/.qdvdauthor");	//ooo
		QString qsDirPath = QDir::homePath() + QString ("/.qdvdauthor");	//xxx
		QDir theDir(qsDirPath);
		if (!theDir.exists())
			theDir.mkdir (qsDirPath);
		//xine_config_save  (pXineEngine, configFile);			//ooo
		xine_config_save  (pXineEngine, configFile.toLatin1().data());	//xxx
	}
}

void XineWidget::saveInitFile ()
{
	xine_cfg_entry_t audioConfig, videoConfig;
	xine_t *pXineEngine = (xine_t *)GetXineEngine();
	if (!pXineEngine)
		return;

	//QString qsConfigFile = QDir::homeDirPath();	//ooo
	QString qsConfigFile = QDir::homePath();	//xxx
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

void XineWidget::playMRL (const QString qsMRL)
{
	m_qsMRL = qsMRL;
	m_bPaused = false;
	// Ensure the initialization is done ...
	if (!getStream ())
		return;
	PlayMRL(qsMRL, "Xine Engine", false);
}

xine_stream_t* XineWidget::getStream()
{
	int iCounter = 0;
	xine_stream_t *pStream = GetStream();
	while ( (pStream == NULL) && (iCounter++ < 5) )   {
		if (pStream == NULL) {
			//sleep (1);				//ooo
			SleeperThread3::msleep ( 1000 );	//xxx
		}
		pStream = GetStream();
	}
	return pStream;
}

bool XineWidget::openMRL (const QString qsMRL)
{
  xine_stream_t *pStream = getStream();

  if ( ! pStream ) 
    return false;

  //if (xine_open(pStream, qsMRL.local8Bit())) /** pass mrl local 8Bit encoded **/              //ooo
  if (xine_open(pStream, qsMRL.toLocal8Bit().data())) { /** pass mrl local 8Bit encoded **/	//xxx
    xine_play(pStream, 0,0);
  }

  slotStopPlayback();

	  //oooo
  m_qsMRL = qsMRL;        //ooo
  //m_qsMRL.append(qsMRL);  //xxx
  
  return true;
}

void XineWidget::play ()
{
	m_bPaused = false;
	slotSpeedNormal();
}

void XineWidget::pause()
{
	m_bPaused = true;
	slotSpeedPause();
}

void XineWidget::stop ()
{
	m_bPaused = false;
	if (xineStream)
		slotStopPlayback();
}

void XineWidget::setSpeed(float fValue)
{
	if (fValue == 0.0)
		pause ();
	else
		slotSpeed((int) fValue);
}

void XineWidget::setVolume(float fValue)
{
  //slotSetVolume((int)(fValue*100.0f));            //ooo
  slotSetVolume(static_cast<int>(fValue*100.0f));    //xxx
}

void XineWidget::setPosition(uint iPosition)
{
	slotChangePosition (iPosition);
}

void XineWidget::setPositionByTime(uint iPosition)
{
        slotChangePositionByTime (iPosition);
}

void XineWidget::setNavigate ( enNavigate nav )
{
  xine_event_t xev;
  xev.data = NULL;
  xev.data_length = 0;
  xev.type = XINE_EVENT_INPUT_UP;

  switch ( nav ) {
  case NavigateUp:
    xev.type = XINE_EVENT_INPUT_UP;
  break;
  case NavigateDown:
    xev.type = XINE_EVENT_INPUT_DOWN;
  break;
  case NavigateLeft:
    xev.type = XINE_EVENT_INPUT_LEFT;
  break;
  case NavigateRight:
    xev.type = XINE_EVENT_INPUT_RIGHT;
  break;
  case NavigateSelect:
    xev.type = XINE_EVENT_INPUT_SELECT;
  break;
  }
  xine_event_send( xineStream, &xev );
}

void XineWidget::setMenu ( enMenu menu )
{
  if ( menu == MenuRoot )
    slotMenu3 ( );
  else if ( menu == MenuTitle )
    slotMenu2 ( );
}

void XineWidget::setSubtitleTrack ( int iTrackNumber )
{
  slotSetSubtitleChannel ( iTrackNumber );
}

void XineWidget::setAudioTrack ( int iTrackNumber )
{
  slotSetAudioChannel ( iTrackNumber );
}

float XineWidget::volume               ()
{
	return 0.0f;
}

float XineWidget::position             ()
{
//	slotGetPosition();
	return 0.0f;
//	slotEmitLengthInfo();
}

QString XineWidget::supportedExtensions()
{
	int t;
	QStringList listExtensions;
	QString qsExtensions, qsFromXine;
	
	qsFromXine = QXineWidget::GetSupportedExtensions();
	//listExtensions.split(" ", qsFromXine);	//ooo
	listExtensions = qsFromXine.split(" ");		//xxx
	for (t=0;t<(int)listExtensions.count();t++)	{
		qsExtensions += QString ("*.") + listExtensions[t] + QString (" ");
	}
	return qsExtensions;
}

void XineWidget::initMediaEngine ()
{
	InitXine ();
//	loadInitFile ();
//	sleep (2);
}

QImage XineWidget::getScreenshot       ()
{
	return getScreenshot (0.0f);
}

QImage XineWidget::getScreenshot(float)
{  
  uchar *pRgbPile = NULL;
  int iWidth, iHeight;
  double fScaleFactor;

  GetScreenshot (pRgbPile, iWidth, iHeight, fScaleFactor);
  
  if (!pRgbPile)
    return QImage();
  //QImage screenShot(pRgbPile, iWidth, iHeight, 32, 0, 0, QImage::IgnoreEndian);    //ooo
  QImage screenShot(pRgbPile, iWidth, iHeight, QImage::Format_RGB32, 0, 0);          //xxx
  if (fScaleFactor >= 1.0)
    iWidth = (int)((double) iWidth * fScaleFactor);
  else
    iHeight = (int) ((double) iHeight / fScaleFactor);

  //screenShot = screenShot.smoothScale(iWidth, iHeight);		//ooo
  screenShot = screenShot.scaled(iWidth, iHeight);		//xxx
  delete []pRgbPile;
  return screenShot;	//ooo
}

bool XineWidget::setScreenshot ( long iMSecOffset )
{
  QTime seekTime;
  seekTime = seekTime.addMSecs ( iMSecOffset );
  playMRL ( m_qsMRL );
  slotChangePositionByTime ( iMSecOffset );
  //  xine_play(xineStream, 0, iMSecOffset );
  slotStopPlayback ( );
  return true;
}

bool XineWidget::setScreenshot ( QPixmap & )
{
  return false;
}

// Non mandatory functions. 
void XineWidget::setAspectRatio (uint)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
}

void XineWidget::setEqualizer (uint, int)
{
	// Tune the sound 
}
	
bool XineWidget::hasChapters()
{
	return false;
}

void XineWidget::playChapter(int iHowFarFromHere)
{
	// 0=current / -x-=play x chapters before / +x=play current chapter + x
	if (iHowFarFromHere == 0)
		return;
	else if (iHowFarFromHere > 0)
		for (;iHowFarFromHere > 0; iHowFarFromHere --)
			PlayNextChapter();
	else if (iHowFarFromHere < 0)
		for (;iHowFarFromHere < 0; iHowFarFromHere ++)
			PlayPreviousChapter();
}

bool XineWidget::isPlaying()
{
	return posTimer.isActive();
}

