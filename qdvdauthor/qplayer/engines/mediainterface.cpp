/***************************************************************************
 ** Class MediaInterface
 ** 
 ** This class is a pure virtual class which defines the interface to 
 ** the MediaEngines (Xine, MPlayer, and or VLC so far.
 ** 
 ** 
 ** 
 ***************************************************************************/

//#include <qimage.h>   //ooo
#include <QImage>       //xxx
#include <QMessageBox>  //xxx
  
#include "mediainterface.h"
#include "mediainfo.h"

MediaInterface::MediaInterface()
 : QObject ()
{
	m_pMediaInfo = NULL;
	m_bPaused    = false;
}

MediaInterface::~MediaInterface()
{
	if ( m_pMediaInfo )
		delete m_pMediaInfo;
	m_pMediaInfo = NULL;
}

QImage MediaInterface::getScreenshot(float)
{
	return QImage ();
}

// Functions handled in the MediaInterface - calss
const QString& MediaInterface::getMRL()
{
	return m_qsMRL;
}

void  MediaInterface::slotTogglePause()
{
	if (isPlaying ())
		pause ();
	else
		play ();
}

// Non mandatory functions. 
void MediaInterface::setAspectRatio (uint)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
}
void MediaInterface::setEqualizer (uint, int)
{
	// Tune the sound 
}

MediaInfo *MediaInterface::getMediaInfo ()
{
	return m_pMediaInfo;
}

bool MediaInterface::hasChapters()
{
	return false;
}

void MediaInterface::playChapter(int)
{
	// 0=current / -x-=play x chapters before / +x=play current chapter + x
}

bool MediaInterface::isPaused()
{
	return m_bPaused;
}

bool MediaInterface::isPlaying()
{
	return false;
}

void MediaInterface::initMediaEngine ()
{

}

QString MediaInterface::supportedExtensions ()
{
	if (m_pMediaInfo)
		return m_pMediaInfo->getExtensions(true);
	return QString ();
}
