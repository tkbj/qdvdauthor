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

#include <stdlib.h>

#include <qstringlist.h>
//#include <q3process.h>	//ooo

//#include <qwidget.h>		//ooo
#include <QWidget>		//xxx
//#include <qregexp.h>		//ooo
#include <QRegExp>		//xxx
//#include <qstring.h>		//ooo
#include <QString>		//xxx
#include <QFileInfo>		//xxx

#include <vlc/vlc.h>

#include "qplayer_global.h"
#include "qvlcwidget.h"

//QVLCWidget::QVLCWidget(QWidget *pParent, const char *pName, Qt::WFlags iFlags)	//ooo
QVLCWidget::QVLCWidget(QWidget *pParent)						//xxx
	//: QWidget (pParent, pName, iFlags)	//ooo
	: QWidget (pParent)			//xxx
{
	m_iExitError = -1;
	m_iSeekValue =  5;
	m_qsScreenshotName = QString ("/tmp/mplayer_screenshot.jpg");

	m_bInPauseMode = false;
	//m_pMasterProcess = new Q3Process;	//ooo
	m_pMasterProcess = new QProcess;	//xxx

	//QObject::connect (m_pMasterProcess, SIGNAL(readyReadStdout()), this, SLOT (slotReadStdout     ()));          //ooo
	QObject::connect (m_pMasterProcess, SIGNAL(readyReadStandardOutput()), this, SLOT (slotReadStdout     ()));    //xxx
	//QObject::connect (m_pMasterProcess, SIGNAL(readyReadStderr()), this, SLOT (slotReadStderr     ()));          //ooo
    QObject::connect (m_pMasterProcess, SIGNAL(readyReadStandardError()), this, SLOT (slotReadStderr     ()));      //xxx
	//QObject::connect (m_pMasterProcess, SIGNAL(processExited  ()), this, SLOT (slotProcessExited  ()));          //ooo
    QObject::connect (m_pMasterProcess, SIGNAL(finished  ()), this, SLOT (slotProcessExited  ()));                  //xxx
	QObject::connect (m_pMasterProcess, SIGNAL(wroteToStdin   ()), this, SLOT (slotWroteToStdin   ()));
	QObject::connect (m_pMasterProcess, SIGNAL(launchFinished ()), this, SLOT (slotLaunchFinished ()));
	
	//const char *pString =  VLC_Version ();			//ooo
//	printf ("QVLCWidget::QVLCWidget <%s>\n", pString);
}

QVLCWidget::~QVLCWidget()
{
	if (m_pMasterProcess)
		delete m_pMasterProcess;
}

bool QVLCWidget::start(QStringList *pEnvironment)
{
	m_bInPauseMode = false;
	//if (m_pMasterProcess->isRunning())	//ooo
	if (m_pMasterProcess->atEnd ())		//xxx
		return false;
	QStringList listCommand;
	m_qsStdoutBuffer="";
	//m_pMasterProcess->clearArguments();	//ooo
	listCommand.append(QString("mplayer")); 
	listCommand.append(QString("-zoom"));
	listCommand.append(QString("-noautosub"));
	listCommand.append(QString("-cache"));
	listCommand.append(QString("1024"));
	listCommand.append(QString("-double"));
//	listCommand.append(QString("-quiet"));
	listCommand.append(QString("-noquiet"));
	listCommand.append(QString("-ontop"));

	listCommand.append(QString("-wid"));
	listCommand.append(QString("%1").arg(winId()));
	listCommand.append(QString("-slave"));
//	listCommand.append(QString("-ao"));
//	listCommand.append(QString("arts"));
	listCommand.append(QString("-vo"));
	listCommand.append(QString("x11"));

	listCommand.append(QString("-xy"));
	listCommand.append(QString("%1").arg(width()));

	listCommand.append(QString("-identify"));
	listCommand.append(QString("-framedrop"));
	listCommand.append(QString("--"));
	listCommand.append(QString("%1").arg(m_qsFileName));

	// -ss 01:10:00 seeks to 1 hour 10 min

	m_pMasterProcess->setArguments (listCommand);
	emit (signalMPlayerStarted());
	//return m_pMasterProcess->start(pEnvironment);		//ooo
	return m_pMasterProcess->waitForStarted();		//xxx
}

void QVLCWidget::setFilename(const QString &qsFileName)
{
	QString qsBuffer;
	// Here we set the global file name.
	m_qsFileName = qsFileName;

	//if (m_pMasterProcess->isRunning ())	{	//ooo
	if (m_pMasterProcess->atEnd ())	{		//xxx
		qsBuffer = QString ("loadfile %1\n").arg(qsFileName);
		writeToStdin (qsBuffer);
	}
}

void QVLCWidget::slotReadStdout     ()
{
	//QString qsStdout(m_pMasterProcess->readStdout());		//ooo
	QString qsStdout(m_pMasterProcess->readAllStandardOutput());	//xxx
	if (qsStdout.length() == 0)
		return;

	m_qsStdoutBuffer+=qsStdout;
}

void QVLCWidget::slotReadStderr     ()
{
	//QString qsStderr(m_pMasterProcess->readStdout());		//ooo
	QString qsStderr(m_pMasterProcess->readAllStandardOutput());	//xxx
	if (qsStderr.length() == 0)
		return;

	m_qsStderrBuffer+=qsStderr;
	parseStderr(qsStderr);
}

void QVLCWidget::slotProcessExited  ()
{
	//if (m_pMasterProcess->normalExit())		//ooo
	if (m_pMasterProcess->NormalExit)		//xxx
		m_iExitError = m_pMasterProcess->exitStatus();
	emit (signalMPlayerExited(m_iExitError));
}

void QVLCWidget::slotWroteToStdin   ()
{
}

void QVLCWidget::slotLaunchFinished ()
{
}

void QVLCWidget::writeToStdin ( QString &qsBuffer )
{
	//m_pMasterProcess->writeToStdin(qsBuffer.latin1());	//ooo
	//m_pMasterProcess->writeToStdin ( qsBuffer );		//xxx
	m_pMasterProcess->write ( qsBuffer.toLatin1() );	//xxx
}

void QVLCWidget::parseStderr ( QString &qsBuffer )
{
	QRegExp rx("V:(.*[0-9]) ");
	int iTemp;
	rx.setMinimal(true);
	//iTemp=rx.match(qsBuffer);	//ooo
	iTemp=rx.indexIn(qsBuffer);	//xxx
	if (iTemp != -1)
	{
		iTemp= (int)(rx.cap(1)).toFloat();
		emit (signalElapsedTime(iTemp));
	}
}

void QVLCWidget::slotForward()
{
	slotSeek(m_iSeekValue,0);
}

void QVLCWidget::slotBack()
{
	slotSeek(-m_iSeekValue,0);
}

void QVLCWidget::slotPlay()
{
	if (m_bInPauseMode)
		slotPause ();
	//if (m_pMasterProcess->isRunning())	//ooo
	if (m_pMasterProcess->atEnd())		//xxx
		return;
	start();
}

void QVLCWidget::slotPause()
{
	m_bInPauseMode = !m_bInPauseMode;
	QString qsBuffer="pause\n";
	writeToStdin (qsBuffer);
}

void QVLCWidget::slotStop()
{
	if (m_bInPauseMode)
		slotPause ();
	//if (m_pMasterProcess->isRunning())	{	//ooo
	if (m_pMasterProcess->atEnd())	{		//xxx
		QString qsBuffer="quit\n";
		writeToStdin (qsBuffer);
	}
	m_bInPauseMode = false;
}

void QVLCWidget::slotSetSpeed(float fValue)
{
	QString qsBuffer = QString("speed_set %1\n").arg(fValue);
	writeToStdin (qsBuffer);
}

void QVLCWidget::slotSetVolume(float fValue)
{
	QString qsBuffer = QString("volume %1\n").arg(fValue);
	writeToStdin (qsBuffer);
}

void QVLCWidget::slotScreenshot()
{
	//system (QString("rm %1").arg(m_qsScreenshotName));			//ooo
	system (QString("rm %1").arg(m_qsScreenshotName).toLatin1().data());	//xxx
	QString qsBuffer = QString("screenshot %1\n").arg(m_qsScreenshotName);
	writeToStdin (qsBuffer);
	// Next we verify that the file has been created.
	QFileInfo fileInfo(m_qsScreenshotName);
	if (fileInfo.exists())	{

	}
}
/*
    stop ();
    initProcess ();
    QString outdir = locateLocal ("data", "kmplayer/");
    m_grabfile = outdir + QString ("00000001.jpg");
    unlink (m_grabfile.ascii ());
    QString myurl (url.isLocalFile () ? url.path () : url.url ());
    QString args ("mplayer -vo jpeg -jpeg outdir=");
    args += KProcess::quote (outdir);
    args += QString (" -frames 1 -nosound -quiet ");
    if (pos > 0)
        args += QString ("-ss %1 ").arg (pos);
    args += KProcess::quote (QString (QFile::encodeName (myurl)));
    *m_process << args;
    kdDebug () << args << endl;
    m_process->start (KProcess::NotifyOnExit, KProcess::NoCommunication);
    return m_process->isRunning ();
*/


void QVLCWidget::slotFullscreen()
{
	QString qsBuffer="vo_fullscreen\n";
	writeToStdin(qsBuffer);
}

void QVLCWidget::slotSeek(float fValue,int iMode)
{
	QString qsBuffer;
	if (iMode == 0)
		qsBuffer.sprintf("seek %d %d\n", (int)fValue, iMode);
	else if (iMode == 1)
		qsBuffer.sprintf("seek %f %d\n", fValue, iMode);
	else	{
		printf ("Bad seeking mode, only 1 and 0 allowed\n");
		return;
	}
	writeToStdin(qsBuffer);
}

void QVLCWidget::slotSetSeek(int iNewValue)
{
	m_iSeekValue=iNewValue;
}

// Non mandatory functions.
void QVLCWidget::setAspectRatio ( uint iAspectRatio )
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
	QString qsBuffer;
	float fAspectRatio = 1.0f;
	switch (iAspectRatio)	{
	case ASPECT_AUTO:
		return;
	break;
	case ASPECT_34:
		fAspectRatio = 3.0 / 4.0;
	break;
	case ASPECT_169:
		fAspectRatio = 16.0 / 9.0;
	break;
	case ASPECT_SQUARE:
	default:
		fAspectRatio = 1.0;
	break;
	}
	qsBuffer.sprintf("switch_aspect %f\n", fAspectRatio);
	writeToStdin(qsBuffer);
}

bool QVLCWidget::isPlaying()
{
	//return m_pMasterProcess->isRunning();		//ooo
	return m_pMasterProcess->atEnd();		//xxx
}

