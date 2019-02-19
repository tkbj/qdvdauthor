/****************************************************************************
v** DummyWidget - class
**
** Created: Thu Dec 9 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class implements a dummy MediaInterface so we have something to 
** Fall back in case there was an error with creating one of the other
** MediaInterface's (XineWidget, KaffeinesWidget, MPlayerWidget VLCWidget)
** 
****************************************************************************/
//#include <qimage.h>		//ooo
#include <QImage>		//xxx
//Added by qt3to4:
#include <QPixmap>

#include "dummywidget.h"
#include "mplayerinfo.h"

QImage DummyInfo::getScreenshot( long )
{
	return QImage ();
}

DummyWidget::DummyWidget(QWidget *parent, const char *pName, void *)
	//: QWidget(parent, pName), MediaInterface()	//ooo
	: QWidget(parent), MediaInterface()		//xxx
{
	m_pMediaInfo = new DummyInfo;
}

DummyWidget::~DummyWidget ()
{
	if ( m_pMediaInfo )
		delete m_pMediaInfo;
	m_pMediaInfo = NULL;
}

QWidget *DummyWidget::getWidget ( )
{
	return (QWidget *)this;
}

const char *DummyWidget::getEngineName ()
{
	return "DummyWidget";
}

void *DummyWidget::getMediaEngine ()
{
	return NULL;
}

void DummyWidget::setupDialog ()
{
}

void DummyWidget::loadInitFile ()
{
}

void DummyWidget::saveInitFile ()
{
}

void DummyWidget::playMRL (const QString)
{
}

bool DummyWidget::openMRL (const QString)
{
	return false;
}

void DummyWidget::play ()
{
}

void DummyWidget::pause()
{
}

void DummyWidget::stop ()
{
}

void DummyWidget::setSpeed(float)
{
}

void DummyWidget::setVolume(float)
{
}

void DummyWidget::setPosition(uint)
{
}

void DummyWidget::setPositionByTime(uint)
{
}

void DummyWidget::setNavigate ( enNavigate )
{
}

void DummyWidget::setMenu( enMenu )
{
}

void DummyWidget::setSubtitleTrack ( int )
{
}

void DummyWidget::setAudioTrack ( int )
{
}

float DummyWidget::volume ()
{
	return 0.0f;
}

float DummyWidget::position ()
{
	return 0.0f;
}

MediaInfo *DummyWidget::getMediaInfo ()
{
	return m_pMediaInfo;
}

QString DummyWidget::supportedExtensions()
{
	return QString ();
}

void DummyWidget::initMediaEngine ()
{
}

QImage DummyWidget::getScreenshot ()
{
	return QImage ();
}

QImage DummyWidget::getScreenshot ( float )
{
	return QImage ();
}

bool DummyWidget::setScreenshot ( long )
{
  return false;
}

bool DummyWidget::setScreenshot ( QPixmap & )
{
  return false;
}

void DummyWidget::setAspectRatio (uint)
{
}

void DummyWidget::setEqualizer (uint, int)
{
}
	
bool DummyWidget::hasChapters ()
{
	return false;
}

void DummyWidget::playChapter(int)
{
}

bool DummyWidget::isPlaying()
{
	return false;
}



