/****************************************************************************
** XineWidget - class
**
** Created: Wed Nov 24 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class derives from the xyz - class and implements the 
** MediaInterface functions ...
** 
****************************************************************************/

#include <qimage.h>
//Added by qt3to4:
#include <QPixmap>

//#include "mediainfo.h"	//ooo
#include "../mediainfo.h"	//xxx
#include "qvlcwidget.h"
#include "vlcwidget.h"

VLCWidget::VLCWidget(QWidget *pParent, const char *pName, void *pExistingEngine)
	//: QVLCWidget(pParent, pName=0), MediaInterface ()	//ooo
	: QVLCWidget(pParent), MediaInterface ()		//xxx
{
	pExistingEngine = pExistingEngine;
}

VLCWidget::~VLCWidget ()
{

}

QWidget *VLCWidget::getWidget        ()
{
	return (QWidget *)this;
}

const char *VLCWidget::getEngineName ()
{
	return "VLCWidget";	// has to match MediaCreator::getAvailableEngines - strings
}

void *VLCWidget::getMediaEngine ()
{
	return NULL;
}

void VLCWidget::setupDialog ()
{
}

void VLCWidget::loadInitFile ()
{
}

void VLCWidget::saveInitFile ()
{
}

void VLCWidget::playMRL (const QString qsMRL)
{
	m_qsMRL = qsMRL;
}

bool VLCWidget::openMRL (const QString qsMRL)
{
	m_qsMRL = qsMRL;
	return true;
}

void VLCWidget::play ()
{
}

void VLCWidget::pause()
{
}

void VLCWidget::stop ()
{
}

void VLCWidget::setNavigate ( enNavigate nav )			//xxx
{
}

void VLCWidget::setMenu ( enMenu menu )				//xxx
{
}

void VLCWidget::setSubtitleTrack ( int iTrackNumber )		//xxx
{
}

void VLCWidget::setAudioTrack ( int iTrackNumber )		//xxx
{
}

void VLCWidget::setSpeed(float fValue)
{
	fValue = fValue;
}

void VLCWidget::setVolume(float fValue)
{
	fValue = fValue;
}

void VLCWidget::setPosition(uint iPosition)
{
	iPosition = iPosition;
}

void VLCWidget::setPositionByTime (uint iPosition)
{
	iPosition = iPosition;
}

float VLCWidget::volume               ()
{
	return 0.0f;
}

float VLCWidget::position             ()
{
	return 0.0f;
}

QString VLCWidget::supportedExtensions()
{
	if (m_pMediaInfo)
		return m_pMediaInfo->getExtensions();
	return QString ();
}

void VLCWidget::initMediaEngine ()
{
}

QImage VLCWidget::getScreenshot       ()
{
	return getScreenshot (0.0f);
}

QImage VLCWidget::getScreenshot(float)
{
	QImage screenShot;
	return screenShot;
}

bool VLCWidget::setScreenshot ( long iMSecOffset )
{

  return false;
}

bool VLCWidget::setScreenshot ( QPixmap &pix )
{

  return false;
}

// Non mandatory functions.
void VLCWidget::setAspectRatio (uint)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
}

void VLCWidget::setEqualizer (uint, int)
{
	// Tune the sound 
}
	
bool VLCWidget::hasChapters()
{
	return false;
}

void VLCWidget::playChapter(int iHowFarFromHere)
{
	// 0=current / -x-=play x chapters before / +x=play current chapter + x
	iHowFarFromHere = iHowFarFromHere;
}

bool VLCWidget::isPlaying()
{
	return false;
}



