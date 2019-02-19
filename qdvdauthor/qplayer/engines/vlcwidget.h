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


// The VideoLAN web site: http://www.videolan.org/
// Documentation and support: http://w.videolan.org/support/
// Forums: http://forum.videolan.org/
// The Developers site: http://developers.videolan.org/vlc/

#ifndef VLCWIDGET_H
#define VLCWIDGET_H

#include "qvlcwidget.h"
//Added by qt3to4:
#include <QPixmap>
#include "mediainterface.h"

class VLCWidget : protected QVLCWidget, public MediaInterface
{
public: 
	VLCWidget(QWidget *parent=0, const char *name=0, void *p=NULL);
	virtual ~VLCWidget();

	// Here are the implementation of the mandatory MediaInterface - class
	virtual QWidget    *getWidget      ();
	virtual const char *getEngineName  ();
	virtual void       *getMediaEngine ();
	virtual void        setupDialog    ();

	virtual void playMRL  (const QString);
	virtual bool openMRL  (const QString);
	virtual void play                  ();
	virtual void pause                 ();
	virtual void stop                  ();
	virtual void setSpeed         (float);
	virtual void setVolume        (float);
	virtual void setPosition       (uint);
	virtual void setPositionByTime (uint);
	
	virtual void setNavigate ( enNavigate );	//xxx
	virtual void setMenu     ( enMenu     );	//xxx
	virtual void setSubtitleTrack   ( int );	//xxx
	virtual void setAudioTrack      ( int );	//xxx

	virtual float volume               ();
	virtual float position             ();
//	virtual MediaInfo *getMediaInfo    ();
	virtual QString supportedExtensions();
	virtual QImage getScreenshot       ();
	virtual QImage getScreenshot(float f  = 0.0f);
	virtual bool   setScreenshot ( long );
	virtual bool   setScreenshot ( QPixmap & );

	// Functions handled in the MediaInterface - calss
	const QString& getMRL();
	void  slotTogglePause();

	// Non mandatory functions. 
	virtual void initMediaEngine  ();
	virtual void setAspectRatio (uint);    // Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
	virtual void setEqualizer (uint, int); // Tune the sound 
	
	virtual bool hasChapters();
	virtual void playChapter(int);	// 0=current / -x-=play x chapters before / +x=play current chapter + x

	virtual bool isPlaying();

private:	// private functions ...
	void loadInitFile ();
	void saveInitFile ();
	
private:	// Private memeber functions ...	
};

#endif // VLCWIDGET_H
