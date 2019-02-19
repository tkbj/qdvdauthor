/****************************************************************************
** XineWidget - class
**
** Created: Wed Nov 24 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class derives from the KumaPlayerMplayer - class and implements the
** MediaInterface functions ...
** 
****************************************************************************/

#ifndef MPLAYERWIDGET_H
#define MPLAYERWIDGET_H

#include "qmplayerwidget.h"
//Added by qt3to4:
#include <QPixmap>
#include "mediainterface.h"

class MPlayerWidget : protected QMPlayerWidget, public MediaInterface
{
public:
	MPlayerWidget(QWidget *p=0, const char *n=0, void *p2=NULL);		//ooo
	//MPlayerWidget(QWidget *p=0);						//xxx
	virtual ~MPlayerWidget();

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

	virtual bool hasChapters();
	virtual void playChapter(int);	// 0=current / -x-=play x chapters before / +x=play current chapter + x

	virtual bool isPlaying();
};

#endif // MPLAYERWIDGET_H
