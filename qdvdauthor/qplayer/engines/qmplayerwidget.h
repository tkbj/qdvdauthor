/****************************************************************************
** QMPlayerWidget - class
**
** Created: Fri Dec 10 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class implements the MPLayerWidget
** MediaInterface to this class for integration in QDVDAUthor is MPlayerWidget
** 
****************************************************************************/

#ifndef QMPLAYERWIDGET_H
#define QMPLAYERWIDGET_H

//#include <qwidget.h>		//ooo
#include <QWidget>		//xxx
//#include <qregexp.h>		//ooo
#include <QRegExp>		//xxx
#include <QProcess>

class Q3Process;
class MPlayerInfo;

#define MPLAYER_INIT_FILE "/.qdvdauthor/mplayer.ini"
#define MPLAYER_TEMP_FILE "/tmp/mplayer_screenshot.jpg"


class QMPlayerWidget : public QWidget
{
	Q_OBJECT
public: 
	// define the index to the m_arrayPluginOptions[4]
	enum {VideoPlugin=0, VideoPluginOptions, AudioPlugin, AudioPluginOptions};
	
	// Constructor / Destructor ...
	         //QMPlayerWidget(QWidget *p=0, const char *n=0, Qt::WFlags f=0);	//ooo
		 //QMPlayerWidget(QWidget *p=0, const char *n=0, Qt::WindowFlags f=0);	//xxx
		 QMPlayerWidget(QWidget *p=0);						//xxx
	virtual ~QMPlayerWidget();

	virtual bool start (QStringList *p=0);
	void   setFilename (const  QString &);

public slots:
	virtual void slotReadStdout     ();
	virtual void slotReadStderr     ();
	//virtual void slotProcessExited  ();  //ooo
    virtual void slotProcessExited  (int, QProcess::ExitStatus);    //xxx
	virtual void slotWroteToStdin   ();
	virtual void slotLaunchFinished ();

	void slotForward();
	void slotBack();
	void slotPlay();
	void slotPause();
	void slotStop();
	void slotSetSpeed (float);
	void slotSetVolume(float);
	void slotFullscreen();
	void slotSeek(float, int);
	void slotSetSeek(int);
	void slotScreenshot();

	void slotNavigate      ( int );
	void slotMenu          ( int );
	void slotSubtitleTrack ( int );
	void slotAudioTrack    ( int );

	void setAspectRatio (uint);
	void playChapter(int);	// 0=current / -x-=play x chapters before / +x=play current chapter + x
	bool isPlaying();

signals:
	void signalMPlayerExited (int);
	void signalMPlayerStarted   ();
	void signalPlaybackFinished ();
protected:
	void loadInitFile ();
	void saveInitFile ();

private:
	void writeToStdin (QString &);

protected:
	//Q3Process    *m_pMasterProcess;	//ooo
	QProcess    *m_pMasterProcess;		//xxx

	QString      m_qsScreenshotName;
	QString      m_qsFileName;
	bool         m_bInPauseMode;
	MPlayerInfo *m_pMPLayerInfo;
	QStringList  m_listPluginOptions;

signals:
	void signalNewPosition (int, const QString &);
	void signalNewPosition (long);
	void signalElapsedTime (float);

private:
	int       m_iSeekValue;
	int       m_iExitError;
	float     m_fLength;
	QRegExp   m_rxParseElapsedTime;
	QRegExp   m_rxParseLength;
};

#endif // QMPLAYERWIDGET_H
