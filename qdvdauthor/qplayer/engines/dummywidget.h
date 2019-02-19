/****************************************************************************
** DummyWidget - class
**
** Created: Thu Dec 09 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class implements a dummy MediaInterface so we have something to 
** Fall back in case there was an error with creating one of the other
** MediaInterface's (XineWidget, KaffeinesWidget, MPlayerWidget VLCWidget)
** 
****************************************************************************/

#ifndef DUMMYWIDGET_H
#define DUMMYWIDGET_H

//#include <qwidget.h>		//ooo
#include <QWidget>		//xxx
//Added by qt3to4:
#include <QPixmap>

#include "mediainfo.h"
#include "mediainterface.h"

class DummyInfo : public MediaInfo
{
public:
	DummyInfo ( )          { };
	DummyInfo (  void  * ) { };
	virtual ~DummyInfo ( ) { };

	// Mandatory function getScreenshot ()
	virtual QImage getScreenshot ( long );
protected:
	// Mandatory function queryValues ()
	virtual void queryValues ( ) { };
};

class DummyWidget : protected QWidget, public MediaInterface
{
public:
	DummyWidget(QWidget *parent=0, const char *name=0, void *p=NULL);
	virtual ~DummyWidget();

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

	virtual void setNavigate( enNavigate );
	virtual void setMenu        ( enMenu );
	virtual void setSubtitleTrack  ( int );
	virtual void setAudioTrack     ( int );

	virtual float volume               ();
	virtual float position             ();
	virtual MediaInfo *getMediaInfo    ();
	virtual QString supportedExtensions();
	virtual QImage getScreenshot       ();
	virtual QImage getScreenshot ( float f=0.0f );
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

#endif // DUMMYWIDGET_H
