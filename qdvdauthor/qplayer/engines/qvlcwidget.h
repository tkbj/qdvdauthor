/****************************************************************************
** QVLCWidget - class
**
** Created: Fri Dec 10 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class implements the MPLayerWidget
** MediaInterface to this class for integration in QDVDAUthor is MPlayerWidget
** 
****************************************************************************/

#ifndef QVLCWIDGET_H
#define QVLCWIDGET_H

#include <vlc/vlc.h>
#include <qwidget.h>		//ooo
//#include <QWidget>		//xxx
#include <QProcess>		//xxx


class Q3Process;
class MPlayerInfo;		//ooo
//class VLCInfo;			//xxx

class QVLCWidget : public QWidget
{
	Q_OBJECT
public: 
	         //QVLCWidget(QWidget *p=0, const char *n=0, Qt::WFlags f=0);		//ooo
		 //QVLCWidget(QWidget *p=0, const char *n=0, Qt::WindowFlags f=0);	//xxx
		 QVLCWidget(QWidget *p=0);						//xxx
	virtual ~QVLCWidget();

	virtual bool start (QStringList *p=0);
	void   setFilename (const  QString &);

public slots:
	virtual void slotReadStdout     ();
	virtual void slotReadStderr     ();
	virtual void slotProcessExited  ();
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

	void setAspectRatio (uint);
	bool isPlaying();

signals:
	void signalMPlayerExited(int);
	void signalMPlayerStarted();

private:
	void writeToStdin (QString &);
	void parseStderr(QString &);

protected:
	//Q3Process    *m_pMasterProcess;	//ooo
	QProcess    *m_pMasterProcess;		//xxx

	QString      m_qsScreenshotName;
	QString      m_qsFileName;
	QString      m_qsStdoutBuffer;
	QString      m_qsStderrBuffer;
	bool         m_bInPauseMode;
	MPlayerInfo *m_pMPLayerInfo;		//ooo
	//VLCInfo     *m_pVLCInfo;		//xxx

// Obsolete later on ...
signals:
	void signalElapsedTime(int,int,int);
	void signalElapsedTime(int);
	void signalTotalLengthUpdated(int,int,int);
	void signalTotalLengthUpdated(int);

private:
	int       m_iSeekValue;
	int       m_iExitError;
};

#endif // QVLCWIDGET_H
