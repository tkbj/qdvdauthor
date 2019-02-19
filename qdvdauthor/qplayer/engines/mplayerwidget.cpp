/****************************************************************************
** MPlayerWidget - class
**
** Created: Wed Nov 24 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class derives from the KumaPlayerMplayer - class and implements the
** MediaInterface functions ...
**
****************************************************************************/
#include <qimage.h>
#include <qlineedit.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QPixmap>

#include "mplayerinfo.h"
#include "mplayerwidget.h"
#include "dialogmplayersetup.h"

MPlayerWidget::MPlayerWidget(QWidget *pParent, const char *pName, void *pExistingEngine)	//ooo
//MPlayerWidget::MPlayerWidget(QWidget *pParent)							//xxx
	//: QMPlayerWidget(pParent, pName), MediaInterface()	//ooo
	: QMPlayerWidget(pParent), MediaInterface()		//xxx
{
	pExistingEngine = pExistingEngine;
	m_pMediaInfo = NULL; // NULL or hold all infos of the current/last media stream
	m_fVolume    = 0.0f;
	m_fPosition  = 0.0f;
// This is done in the original widget (QXineWidget) by changing the init file name ...
//	loadInitFile ();
//	connect (this, SIGNAL(signalElapsedTime (float)), this, SLOT (slotElapsedTime (float)));
	MediaInterface::connect	( (QMPlayerWidget *) this, SIGNAL(signalNewPosition ( long )), 
				  (MediaInterface *) this, SIGNAL(signalNewPosition ( long )) );
	MediaInterface::connect	( (QMPlayerWidget *) this, SIGNAL(signalNewPosition (int, const QString&)), 
				  (MediaInterface *) this, SIGNAL(signalNewPosition (int, const QString&)) );
	MediaInterface::connect	( (QMPlayerWidget *) this, SIGNAL(signalPlaybackFinished ()), 
				  (MediaInterface *) this, SIGNAL(signalPlaybackFinished ()) );
}

MPlayerWidget::~MPlayerWidget ()
{
//	if ( m_pMediaInfo )
//		delete m_pMediaInfo;
//	m_pMediaInfo = NULL;
}

QWidget *MPlayerWidget::getWidget        ()
{
	return (QWidget *)this;
}

const char *MPlayerWidget::getEngineName ()
{
	return "QMPlayerWidget";	// has to match MediaCreator::getAvailableEngines - strings
}

void *MPlayerWidget::getMediaEngine ()
{
	return m_pMasterProcess;
}

void MPlayerWidget::setupDialog ()
{
	DialogMPlayerSetup dialogSetup(this);
	dialogSetup.initMe(&m_listPluginOptions);
	if (dialogSetup.exec() == QDialog::Accepted)	{
		m_listPluginOptions[MPlayerWidget::VideoPluginOptions] = dialogSetup.m_pEditVideoOptions->text();
		m_listPluginOptions[MPlayerWidget::AudioPluginOptions] = dialogSetup.m_pEditAudioOptions->text();
		m_listPluginOptions[MPlayerWidget::VideoPlugin]        = dialogSetup.m_pComboVideo->currentText();
		m_listPluginOptions[MPlayerWidget::AudioPlugin]        = dialogSetup.m_pComboAudio->currentText();
		saveInitFile();
	}	//ooo
}

void MPlayerWidget::playMRL (const QString qsMRL)
{
	openMRL(qsMRL);
	// and then play.
	slotPlay();
}

bool MPlayerWidget::openMRL (const QString qsMRL)
{
  if ( m_qsMRL != qsMRL )	{
    // set the filename
    setFilename ( qsMRL );
    // The we create som information about the stream ...
    if ( m_pMediaInfo )
      delete m_pMediaInfo;
    m_pMediaInfo = new MPlayerInfo;
    m_pMediaInfo->setFileName ( qsMRL );
    //printf ("MPlayerWidget::openMRL <%s><%s><%s>\n", (const char *)m_qsMRL, (const char *)qsMRL, (const char *)m_pMediaInfo->getFileName ());
    m_qsMRL = qsMRL;
  }
  return true;
}

void MPlayerWidget::play ()
{
	slotPlay();
	m_bPaused = false;
}

void MPlayerWidget::pause()
{
	slotPause ();
	//m_bPaused = true;
	m_bPaused = m_bInPauseMode;
}

void MPlayerWidget::stop ()
{
	slotStop ();
	m_qsMRL   = "";
	m_bPaused = false;
}

void MPlayerWidget::setSpeed(float fValue)
{
	if (fValue == 0.0)
		pause ();
	else
		QMPlayerWidget::slotSetSpeed (fValue);
}

void MPlayerWidget::setVolume(float fValue)
{
  // MPlayer static volume range is between [0 and 100] %
  QMPlayerWidget::slotSetVolume( fValue * 100.0 );
}
/*
void MPlayerWidget::setVolume(float fValue)
{
  // MPlayer takes values above 1.0 as positive
  // and below as negative for the volume.
  // the volume is NOT dependand on the value itself but
  // rather on the number of times the function is called.
  // For a slider 0..100 it seems best to only 
  // let every 2'nd call pass.
  static bool bEveryOther = true;
  
  if ( bEveryOther )
    QMPlayerWidget::slotSetVolume( fValue );
  bEveryOther = bEveryOther ? false : true;
}
*/
void MPlayerWidget::setPosition(uint iPosition)
{
	// iPosition defined as [0 .. 65535]
	// fPosition as [0% .. 100%]
	//float fPosition = (float) iPosition / 65535.0 * 100.0;   //ooo
    float fPosition = static_cast<float> ( iPosition ) / 65535.0 * 100.0;   //xxx

	slotSeek (fPosition, 1);
}

void MPlayerWidget::setPositionByTime(uint iPosition)
{
	int iTotalSeconds = m_pMediaInfo->getLength();
	//float fPosition = (float)iPosition / iTotalSeconds * 100.0 * 1000;   //ooo
    float fPosition = static_cast<float> ( iPosition ) / iTotalSeconds * 100.0 * 1000;  //xxx

	slotSeek (fPosition, 1);
//	slotSeek ((float)iPosition, 2);
}

void MPlayerWidget::setNavigate ( enNavigate nav )
{
  slotNavigate ( ( int) nav );
}

void MPlayerWidget::setMenu ( enMenu menu )
{
  slotMenu ( (int) menu );
}

void MPlayerWidget::setSubtitleTrack ( int iTrackNumber )
{
  slotSubtitleTrack ( iTrackNumber );
}

void MPlayerWidget::setAudioTrack ( int iTrackNumber )
{
  slotAudioTrack ( iTrackNumber );
}

float MPlayerWidget::volume ()
{
	return 0.0f;
}

float MPlayerWidget::position ()
{
	return 0.0f;
}

void MPlayerWidget::initMediaEngine ()
{
}

QImage MPlayerWidget::getScreenshot ()
{
	return getScreenshot (0.0f);
}

QImage MPlayerWidget::getScreenshot ( float fSeconds )
{
  if ( m_pMediaInfo )
    //return m_pMediaInfo->getScreenshot ( (long)(fSeconds * 1000.0) ); //ooo
    return m_pMediaInfo->getScreenshot ( static_cast<long> (fSeconds * 1000.0) );  //xxx
  //return QImage ( QImage ( ).fromMimeSource ( "error.jpg" ) );	//ooo
  return QImage ( ":/error.jpg" );					//xxx
}

bool MPlayerWidget::setScreenshot ( long iMSecOffset )
{
  // set the current image to be displayed. 
  //QImage theImage = getScreenshot ( (float) iMSecOffset / 1000.0 );   //ooo
  QImage theImage = getScreenshot ( static_cast<float> ( iMSecOffset ) / 1000.0 );      //xxx
  if ( ! theImage.isNull ( ) ) {
    //theImage = theImage.smoothScale ( width (), height (), Qt::KeepAspectRatio );	//ooo
    theImage = theImage.scaled ( width (), height (), Qt::KeepAspectRatio );		//xxx
    QPixmap pix;
    pix.convertFromImage ( theImage );
    return setScreenshot ( pix );
  }
  return false;
}
 
bool MPlayerWidget::setScreenshot ( QPixmap &pix )
{
  if ( pix.isNull ( ) )
    return false;
  //getWidget ( )->setScaledContents         ( true );		//xxx	
  getWidget ( )->setAutoFillBackground     ( true );		//xxx
  //getWidget ( )->setPaletteBackgroundPixmap ( pix );		//ooo
  QPalette palette;						//xxx
  palette.setBrush(QPalette::Window, QBrush(pix));		//xxx
  getWidget ( )->setPalette(palette);				//xxx
  getWidget ( )->resize ( pix.width ( ), pix.height ( ) );
  return true;
}

// Non mandatory functions. 
void MPlayerWidget::setAspectRatio (uint)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
}

bool MPlayerWidget::hasChapters ()
{
	if (!m_pMediaInfo)
		return false;
	MPlayerInfo *pMPlayerInfo = (MPlayerInfo *)m_pMediaInfo;
	return pMPlayerInfo->hasChapters();
}

void MPlayerWidget::playChapter(int iHowFarFromHere)
{
	// 0=current / -x-=play x chapters before / +x=play current chapter + x
	QMPlayerWidget::playChapter(iHowFarFromHere);
}

bool MPlayerWidget::isPlaying()
{
	return QMPlayerWidget::isPlaying();
}



