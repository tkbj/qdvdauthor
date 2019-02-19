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

#ifndef XINEWIDGET_H
#define XINEWIDGET_H

#include "qxinewidget.h"
//Added by qt3to4:
#include <QPixmap>
#include "mediainterface.h"

#include <QThread>	//xxx

#define XINE_INIT_FILE "/.qdvdauthor/xine_config.ini"

class SleeperThread3 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

class XineWidget : protected QXineWidget, public MediaInterface
{
public: 
	XineWidget(QWidget *parent=0, const char *name=0, void *p=NULL);
	virtual ~XineWidget();

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
	xine_stream_t* getStream();	// ENsure that the engine has been initialized ...
	void loadInitFile ();
	void saveInitFile ();
	
private:	// Private memeber functions ...	
	QString m_qsCurrentVideoPlugin;
	QString m_qsCurrentAudioPlugin;
};

#endif // XINEWIDGET_H
