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

#include <stdlib.h>

#include <qstringlist.h>
#include <qfileinfo.h>
//#include <q3process.h>	//ooo
#include <QProcess>		//xxx
#include <QTextStream>		//xxx
#include <qstring.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdir.h>
//#include <qdom.h>		//ooo
#include <QtXml/QDomDocument>	//xxx
#include <QtXml/QDomElement>	//xxx
#include <QtXml/QDomNode>	//xxx

#include "xml_mplayer.h"
//#include "qplayer_global.h"	//ooo
#include "../qplayer_global.h"	//xxx
#include "qmplayerwidget.h"

//QMPlayerWidget::QMPlayerWidget(QWidget *pParent, const char *pName, Qt::WFlags iFlags)	//ooo
//QMPlayerWidget::QMPlayerWidget(QWidget *pParent, const char *pName, Qt::WindowFlags iFlags)	//xxx
QMPlayerWidget::QMPlayerWidget(QWidget *pParent)						//xxx
	//: QWidget (pParent, pName, iFlags)	//ooo
	: QWidget (pParent)			//xxx
{
	m_iExitError = -1;
	m_iSeekValue =  5;					
	m_fLength    =  0.0;
	m_qsScreenshotName = QString (MPLAYER_TEMP_FILE);

	m_bInPauseMode = false;
	//m_pMasterProcess = new Q3Process ( this );	//ooo
	m_pMasterProcess = new QProcess ( this );	//xxx

	m_rxParseElapsedTime = QRegExp ("V:(.*[0-9]) ");
	m_rxParseElapsedTime.setMinimal(true);
	m_rxParseLength = QRegExp ("ID_LENGTH=([0-9]*)");
	// Setting some defaults
	m_listPluginOptions.append (QString("x11"));
	m_listPluginOptions.append (QString(""));
	m_listPluginOptions.append (QString("alsa"));
	m_listPluginOptions.append (QString(""));

	//QObject::connect (m_pMasterProcess, SIGNAL(readyReadStdout()), this, SLOT (slotReadStdout     ()));          //ooo
    QObject::connect (m_pMasterProcess, SIGNAL(readyReadStandardOutput()), this, SLOT (slotReadStdout     ()));     //xxx
	//QObject::connect (m_pMasterProcess, SIGNAL(readyReadStderr()), this, SLOT (slotReadStderr     ()));          //ooo
    QObject::connect (m_pMasterProcess, SIGNAL(readyReadStandardError()), this, SLOT (slotReadStderr     ()));      //xxx
	//QObject::connect (m_pMasterProcess, SIGNAL(processExited  ()), this, SLOT (slotProcessExited  ()));          //ooo
    QObject::connect (m_pMasterProcess, SIGNAL(finished  (int, QProcess::ExitStatus)), this, SLOT (slotProcessExited  (int, QProcess::ExitStatus))); //xxx
	QObject::connect (m_pMasterProcess, SIGNAL(wroteToStdin   ()), this, SLOT (slotWroteToStdin   ()));
	QObject::connect (m_pMasterProcess, SIGNAL(launchFinished ()), this, SLOT (slotLaunchFinished ()));
	QObject::connect (m_pMasterProcess, SIGNAL(destroyed      ()), m_pMasterProcess, SLOT (kill   ()));

	// And finally load the initFile
	loadInitFile();
}

QMPlayerWidget::~QMPlayerWidget ( )
{
  if ( m_pMasterProcess ) {
    //if ( m_pMasterProcess->processIdentifier ( ) > 0 ) {	//ooo
    if ( m_pMasterProcess->pid ( ) > 0 ) {			//xxx
      QString qsCommand = QString ( "kill -9 `ps -ef | grep -e mplayer -e %1 | awk '{print $2}'`" ).arg ( winId ( ) ); 
      //int iRet = system ( qsCommand.utf8 ( ) );	//ooo
      int iRet = system ( qsCommand.toUtf8 ( ) );	//xxx
      iRet = iRet;
    }
    //if ( m_pMasterProcess->isRunning ( ) )	//ooo
    if ( !m_pMasterProcess->atEnd ( ) )		//xxx
      m_pMasterProcess->kill   ( );
    delete  m_pMasterProcess;
  }
}

bool QMPlayerWidget::start(QStringList *pEnvironment)
{
	m_bInPauseMode = false;
	//if (m_pMasterProcess->isRunning())	//ooo
	if (m_pMasterProcess->atEnd())		//xxx
		return false;
	QStringList listCommand;	//ooo
	//QStringList *listCommand;	//xxx
	//m_pMasterProcess->clearArguments();		//ooo
	//listCommand.append(QString("mplayer")); 	//ooo
	listCommand.append(QString("-zoom"));	// Automatically adjust to window size	//ooo
	//listCommand->append(QString("-zoom"));	// Automatically adjust to window size	//xxx
	listCommand.append(QString("-noautosub"));	//ooo
	//listCommand->append(QString("-noautosub"));	//xxx
	listCommand.append(QString("-cache"));	//ooo
	//listCommand->append(QString("-cache"));		//xxx
	listCommand.append(QString("1024"));		//ooo
	//listCommand->append(QString("1024"));		//xxx
	listCommand.append(QString("-double"));	//ooo
	//listCommand->append(QString("-double"));	//xxx
//	listCommand.append(QString("-quiet"));
	listCommand.append(QString("-noquiet"));	//ooo
	//listCommand->append(QString("-noquiet"));	//xxx
	listCommand.append(QString("-ontop"));	//ooo
	//listCommand->append(QString("-ontop"));		//xxx

	listCommand.append(QString("-wid"));		//ooo
	//listCommand->append(QString("-wid"));		//xxx
	listCommand.append(QString("%1").arg((long)winId()));		//ooo
	//listCommand->append(QString("%1").arg((long)winId()));		//xxx
	listCommand.append(QString("-slave"));	//ooo
	//listCommand->append(QString("-slave"));		//xxx
	if (!m_listPluginOptions[QMPlayerWidget::AudioPlugin].isEmpty())	{
		listCommand.append(QString("-ao"));	//ooo
		//listCommand->append(QString("-ao"));	//xxx
		listCommand.append(QString("%1%2").arg(m_listPluginOptions[QMPlayerWidget::AudioPlugin]).arg(m_listPluginOptions[QMPlayerWidget::AudioPluginOptions]));	//ooo
		//listCommand->append(QString("%1%2").arg(m_listPluginOptions[QMPlayerWidget::AudioPlugin]).arg(m_listPluginOptions[QMPlayerWidget::AudioPluginOptions]));	//xxx
	}
	listCommand.append(QString("-vo"));	//ooo
	//listCommand->append(QString("-vo"));	//xxx
	if (m_listPluginOptions[QMPlayerWidget::VideoPlugin].isEmpty())	
		listCommand.append(QString("x11"));	//ooo
		//listCommand->append(QString("x11"));	//xxx
	else
		listCommand.append(QString("%1%2").arg(m_listPluginOptions[QMPlayerWidget::VideoPlugin]).arg(m_listPluginOptions[QMPlayerWidget::VideoPluginOptions]));	//ooo
		//listCommand->append(QString("%1%2").arg(m_listPluginOptions[QMPlayerWidget::VideoPlugin]).arg(m_listPluginOptions[QMPlayerWidget::VideoPluginOptions]));	//xxx

	listCommand.append(QString("-xy"));	// Scales it by keeping aspect ratio	//ooo
	//listCommand->append(QString("-xy"));	// Scales it by keeping aspect ratio	//xxx
	listCommand.append(QString("%1").arg(width()));	//ooo
	//listCommand->append(QString("%1").arg(width()));	//xxx

	listCommand.append(QString("-identify"));	//ooo
	//listCommand->append(QString("-identify"));	//xxx
	listCommand.append(QString("-framedrop"));	//ooo
	//listCommand->append(QString("-framedrop"));	//xxx

	//listCommand.append ( ">/dev/null" );
        //listCommand.append ( "2>&1" );

	//if ( m_qsFileName.find ( "dvd://" ) > -1 ) {		//ooo
	if ( m_qsFileName.indexOf ( "dvd://" ) > -1 ) {		//xxx
	  QString qsTemp = m_qsFileName;
	  qsTemp.remove ( "dvd:/" );
	  QFileInfo fileInfo ( qsTemp );
	  if ( fileInfo.isDir  ( ) ) {
	    listCommand.append ( "-dvd-device" );	//ooo
	    //listCommand->append ( "-dvd-device" );	//xxx
	    listCommand.append ( qsTemp );		//ooo
	    //listCommand->append ( qsTemp );		//xxx
	    listCommand.append ( QString  ( "--" ) );	//ooo
	    //listCommand->append ( QString  ( "--" ) );	//xxx
	    listCommand.append ( "dvd://1" );		//ooo
	    //listCommand->append ( "dvd://1" );		//xxx
	  }
	  else if ( fileInfo.exists ( ) ) {
	    listCommand.append ( QString  ( "--" ) );	//ooo
	    //listCommand->append ( QString  ( "--" ) );	//xxx
	    listCommand.append ( qsTemp );		//ooo
	    //listCommand->append ( qsTemp );		//xxx
	  }
	}
	else {
	  listCommand.append ( QString ( "--" ) );	//ooo
	  //listCommand->append ( QString ( "--" ) );	//xxx
	  listCommand.append ( m_qsFileName ); //QString("%1").arg(m_qsFileName));	//ooo
	  //listCommand->append ( m_qsFileName ); //QString("%1").arg(m_qsFileName));	//xxx
	}
//	listCommand.append ( ">/dev/null" );
//	listCommand.append ( "2>&1" );

	// -ss 01:10:00 seeks to 1 hour 10 min
	m_fLength = 0.0;
	//printf ( "<%p>command < %s >\n", m_pMasterProcess, listCommand.join ( " " ).ascii ( ) );

	m_pMasterProcess->setArguments ( listCommand  );
	emit (signalMPlayerStarted());
	//return m_pMasterProcess->start ( pEnvironment );				//ooo
	//m_pMasterProcess->start ( QString("%1").arg("mplayer"), listCommand );	//xxx
	return m_pMasterProcess->waitForStarted();					//xxx

}

void QMPlayerWidget::loadInitFile ()
{
	QFileInfo fileInfo;
	QString qsPlayerIniFile;
	//qsPlayerIniFile = QDir::homeDirPath();	//ooo
	qsPlayerIniFile = QDir::homePath();		//xxx
	qsPlayerIniFile.append(MPLAYER_INIT_FILE);

	// Assign the file
	QFile iniFile(qsPlayerIniFile);
	if (!iniFile.open(QIODevice::ReadOnly))
		return;
	// Try to get the right xml contents ...
	QDomDocument xmlDoc( XML_DOCTYPE_MPLAYER );
	if (!xmlDoc.setContent (&iniFile))	{
		// Error handling ...
		iniFile.close ( );
		QString qsOut = QObject::tr ( "MPlayer xml init file <%1> seems to be defective. Re-setting values.\n" ).arg( qsPlayerIniFile );
		//printf ( "%s", qsOut.ascii ( ) );		//ooo
		printf ( "%s", qsOut.toLatin1().data() );	//xxx
 		saveInitFile ( );
		return;
	}

	// Here is the main loop to extract the info ...
	QDomElement searchXMLTree;
	QDomElement docElem = xmlDoc.documentElement();
	QString  domText    = docElem.tagName();
	QDomNode xmlNode    = docElem.firstChild();
	QString qsVersion   = docElem.attributeNode(MPLAYER_VERSION).value();

	if (domText == MPLAYER_INI)	{
		// First we take care of the QDVDAuthor variables ...
		while( !xmlNode.isNull() ) {
			searchXMLTree = xmlNode.toElement();
			domText = searchXMLTree.text();
			if (MPLAYER_VIDEO_PLUGIN  == searchXMLTree.tagName () )	{
				 m_listPluginOptions[QMPlayerWidget::VideoPluginOptions] = searchXMLTree.attributeNode(MPLAYER_ATTRIB_PLUGIN_OPTIONS).value();
				 m_listPluginOptions[QMPlayerWidget::VideoPlugin] = searchXMLTree.text();
			}
			else if (MPLAYER_AUDIO_PLUGIN  == searchXMLTree.tagName () )	{
				 m_listPluginOptions[QMPlayerWidget::AudioPluginOptions] = searchXMLTree.attributeNode(MPLAYER_ATTRIB_PLUGIN_OPTIONS).value();
				 m_listPluginOptions[QMPlayerWidget::AudioPlugin] = searchXMLTree.text();
			}
			else
				printf ("Warning: MPlayer::loadHistory -=> wrong XML Node <%s>\nContinuing ...\n",
						//(const char *)searchXMLTree.tagName());		//ooo
						 searchXMLTree.tagName().toLatin1().data());		//xxx
			// Go to the next node ...
			xmlNode = xmlNode.nextSibling();
		}
	}
	iniFile.close();
}

void QMPlayerWidget::saveInitFile ()
{
	QFileInfo fileInfo;
	QString qsPlayerIniFile;
	//qsPlayerIniFile = QDir::homeDirPath();	//ooo
	qsPlayerIniFile = QDir::homePath();		//xxx
	qsPlayerIniFile.append(MPLAYER_INIT_FILE);

	// First we create the document, ...
	QDomText domText;
	QDomElement elem, rootElem;
	QDomDocument xmlDoc( XML_DOCTYPE_MPLAYER );
	rootElem = xmlDoc.createElement  ( MPLAYER_INI );
	rootElem.setAttribute( MPLAYER_VERSION, "0.0.9" );

	elem = xmlDoc.createElement  ( MPLAYER_VIDEO_PLUGIN );
	domText = xmlDoc.createTextNode ( m_listPluginOptions[QMPlayerWidget::VideoPlugin] );
	elem.setAttribute( MPLAYER_ATTRIB_PLUGIN_OPTIONS, m_listPluginOptions[QMPlayerWidget::VideoPluginOptions] );
	elem.appendChild ( domText );
	rootElem.appendChild( elem );

	elem = xmlDoc.createElement  ( MPLAYER_AUDIO_PLUGIN );
	domText = xmlDoc.createTextNode ( m_listPluginOptions[QMPlayerWidget::AudioPlugin] );
	elem.setAttribute( MPLAYER_ATTRIB_PLUGIN_OPTIONS, m_listPluginOptions[QMPlayerWidget::AudioPluginOptions] );
	elem.appendChild ( domText );
	rootElem.appendChild( elem );

	xmlDoc.appendChild( rootElem );

	// Assign the file
	QFile iniFile(qsPlayerIniFile);
	if (!iniFile.open(QIODevice::WriteOnly))
		return;

	QString xml = xmlDoc.toString();
	//iniFile.writeBlock(xml, qstrlen (xml));					//ooo
	iniFile.write(xml.toLatin1().data(), qstrlen (xml.toLatin1().data()));	//xxx
	QTextStream out(&iniFile);		//xxx
	out << xml.toLatin1().data();		//xxx
	iniFile.close();
}

void QMPlayerWidget::setFilename(const QString &qsFileName)
{
	QString qsBuffer;
	// Here we set the global file name.
	m_qsFileName = qsFileName;

	//if (m_pMasterProcess->isRunning ())	{	//ooo
	if (!m_pMasterProcess->atEnd ())	{	//xxx
    //if (m_pMasterProcess->Running ())	{	//xxx
		qsBuffer = QString ("loadfile %1\n").arg(qsFileName);
		writeToStdin (qsBuffer);
		m_fLength = 0.0;
	}
}

void QMPlayerWidget::slotReadStdout     ()
{
	//QString qsStdout(m_pMasterProcess->readStdout());		//ooo
	QString qsStdout(m_pMasterProcess->readAllStandardOutput());	//xxx
	if (qsStdout.length() == 0)
		return;

	int iTemp, iElapsedTime, iElapsedLength, iSec, iMin, iHour;
	QString qsElapsedTime;
	//iTemp = m_rxParseElapsedTime.match(qsStdout);		//ooo
	iTemp = m_rxParseElapsedTime.indexIn(qsStdout);		//xxx
	iElapsedLength = 0;
	if (iTemp != -1)	{
		float fElapsedTime = m_rxParseElapsedTime.cap(1).toFloat();
		iElapsedTime = (int)fElapsedTime;
		if ( m_fLength > 0.0 )
		  //iElapsedLength = (int)(fElapsedTime / m_fLength * 65535.0 );  //ooo
          iElapsedLength = static_cast<int>(fElapsedTime / m_fLength * 65535.0 );   //xxx
		//iHour = (int) (fElapsedTime / 3600.0); //(iElapsedTime - iSec - iMin*60)%3600;  //ooo
        iHour = static_cast<int> (fElapsedTime / 3600.0); //(iElapsedTime - iSec - iMin*60)%3600;   //xxx
		//iMin  = (int)((fElapsedTime-iHour*3600)/60.0);  //ooo
        iMin  = static_cast<int>((fElapsedTime-iHour*3600)/60.0);   //xxx
		//iSec  = (int) (fElapsedTime-iHour*3600-iMin*60);    //ooo
        iSec  = static_cast<int> (fElapsedTime-iHour*3600-iMin*60); //xxxx
		//qsElapsedTime = qsElapsedTime.sprintf ("%02d:%02d:%02d.%03d", iHour, iMin, iSec, (uint)((fElapsedTime - iElapsedTime)*1000.0)); //ooo
        qsElapsedTime = qsElapsedTime.sprintf ("%02d:%02d:%02d.%03d", iHour, iMin, iSec, static_cast<uint>((fElapsedTime - iElapsedTime)*1000.0));  //xxx
		emit (signalNewPosition ( iElapsedLength, qsElapsedTime ) );
		emit (signalNewPosition ( (int)(fElapsedTime * 1000.0) ) );
	}

	//if ( m_rxParseLength.search(qsStdout) > -1 )		//ooo
	if ( m_rxParseLength.indexIn(qsStdout) > -1 )		//xxx
	  m_fLength = m_rxParseLength.cap (1).toFloat ();
}

void QMPlayerWidget::slotReadStderr     ()
{
	//QString qsStderr(m_pMasterProcess->readStdout());		//ooo
	QString qsStderr(m_pMasterProcess->readAllStandardOutput());	//xxx
	if (qsStderr.length() == 0)
		return;
}

//void QMPlayerWidget::slotProcessExited  ()    //ooo
void QMPlayerWidget::slotProcessExited  (int, QProcess::ExitStatus) //xxx
{
	//if (m_pMasterProcess->normalExit())	//ooo
	if (m_pMasterProcess->NormalExit)	//xxx
		m_iExitError = m_pMasterProcess->exitStatus();
	emit (signalMPlayerExited(m_iExitError));
	emit (signalPlaybackFinished ());
	m_fLength = 0.0;
}

void QMPlayerWidget::slotWroteToStdin   ()
{
}

void QMPlayerWidget::slotLaunchFinished ()
{
}

void QMPlayerWidget::slotNavigate ( int nav )
{
  QString qsBuffer="dvdnav up\n";
  switch ( nav ) {
  case 0:
    qsBuffer="dvdnav up\n";
  break;
  case 1:
    qsBuffer="dvdnav down\n";
  break;
  case 2:
    qsBuffer="dvdnav left\n";
  break;
  case 3:
    qsBuffer="dvdnav right\n";
  break;
  case 4:
    qsBuffer="dvdnav select\n";
  break;
  };
  writeToStdin ( qsBuffer );
}

void QMPlayerWidget::slotMenu ( int menu )
{
  //if ( m_qsFileName.find ( "dvd://" ) > -1 ) {	//ooo
  if ( m_qsFileName.indexOf ( "dvd://" ) > -1 ) {	//xxx
    QString qsFile = m_qsFileName;
    qsFile = qsFile.remove ( "dvd:/" );
    qsFile += "/VIDEO_TS.VOB";
    QFileInfo fileInfo ( qsFile );
    if ( fileInfo.exists ( ) )
      m_qsFileName = qsFile;
  }
  //if ( m_qsFileName.find ( "VIDEO_TS.VOB" ) > -1 ) {		//ooo
  if ( m_qsFileName.indexOf ( "VIDEO_TS.VOB" ) > -1 ) {		//xxx
    slotStop ( );
    QTimer::singleShot ( 100, this, SLOT ( slotPlay ( ) ) );
    return;
  }

  // Does currently not work in MPLayer ...
  QString qsBuffer="set_menu root\n";
  if ( menu == 0 )
    qsBuffer="set_menu root\n";
  else if ( menu == 1 )
    qsBuffer="set_menu title\n";
    
  writeToStdin ( qsBuffer );
}

void QMPlayerWidget::slotSubtitleTrack ( int iTrackNumber )
{
  QString qsBuffer = QString ( "sub_select %1\n" ).arg ( iTrackNumber );
  writeToStdin ( qsBuffer );
}

void QMPlayerWidget::slotAudioTrack ( int iTrackNumber )
{
  QString qsBuffer = QString ( "switch_audio %1\n" ).arg ( iTrackNumber );
  writeToStdin ( qsBuffer );
}

void QMPlayerWidget::writeToStdin(QString &qsBuffer)
{
	//m_pMasterProcess->writeToStdin(qsBuffer.latin1());	//ooo
	m_pMasterProcess->write(qsBuffer.toLatin1().data());		//xxx
}

void QMPlayerWidget::slotForward()
{
	slotSeek(m_iSeekValue,0);
}

void QMPlayerWidget::slotBack()
{
	slotSeek(-m_iSeekValue,0);
}

void QMPlayerWidget::slotPlay()
{
	if (m_bInPauseMode)
		slotPause ();
	//if (m_pMasterProcess->isRunning())	//ooo
	if (!m_pMasterProcess->atEnd())		//xxx
		return;
	start();
}

void QMPlayerWidget::slotPause()
{
	m_bInPauseMode = !m_bInPauseMode;
	QString qsBuffer="pause\n";
	writeToStdin (qsBuffer);
}

void QMPlayerWidget::slotStop()
{
	if (m_bInPauseMode)
		slotPause ();
	//if (m_pMasterProcess->isRunning())	{	//ooo
	if (!m_pMasterProcess->atEnd())	{		//xxx
		QString qsBuffer="quit\n";
		writeToStdin (qsBuffer);
	}
	m_bInPauseMode = false;
	m_fLength = 0.0;
}

void QMPlayerWidget::slotSetSpeed(float fValue)
{
	QString qsBuffer = QString("speed_set %1\n").arg(fValue);
	writeToStdin (qsBuffer);
}

void QMPlayerWidget::slotSetVolume(float fValue)
{
  // There are two volume modes. Static and dynamic.
  // Static is in the range between [0 and 100]
  // Dynamic inc / dec the volume percentage wise [ -100 .. +100 ]
  int     iVolume  = (int)( fValue );
  QString qsBuffer = QString ( "volume %1 1\n" ).arg( iVolume );
  writeToStdin ( qsBuffer );
}
/*
void QMPlayerWidget::slotSetVolume(float fValue)
{ // Dynamic mode
  // The slave mode seems to take relative values for the volume,
  // thus to quiet things down values between [0.0 and 1.0[
  // To increase volume values between [1.0 and oo[
  // are to be set.
  // Note: The volume does only depend on the number of times you set the 
  //       volume and not on the actual value. 
  //       I.e. 10*1.1 is the same as 10*1.0000001
  static float m_fOldVolume = 1.0f;
  double fVolume = fValue - m_fOldVolume + 1.0;
  m_fOldVolume   = fValue;
  QString qsBuffer = QString ( "volume %1\n" ).arg( fVolume );
  writeToStdin ( qsBuffer );
}
*/
void QMPlayerWidget::slotScreenshot ( )
{
	QString qsCommand = QString ( "rm -f %1" ).arg ( m_qsScreenshotName );
	//int iRet = system ( qsCommand.utf8 ( ) );	//ooo
	int iRet = system ( qsCommand.toUtf8 ( ) );	//xxx
	iRet = iRet;
	QString qsBuffer = QString ( "screenshot %1\n").arg ( m_qsScreenshotName );
	writeToStdin ( qsBuffer );
	// Next we verify that the file has been created.
	QFileInfo fileInfo ( m_qsScreenshotName );
	if ( fileInfo.exists ( ) )  {

	}
}

void QMPlayerWidget::slotFullscreen()
{
	QString qsBuffer="vo_fullscreen\n";
	writeToStdin(qsBuffer);
}

void QMPlayerWidget::slotSeek(float fValue,int iMode)
{
	QString qsBuffer;
	if (iMode == 0)	// seek relative
		qsBuffer.sprintf("seek %d %d\n", (int)fValue, iMode);
	else if (iMode == 1)	// seek in % of total movie
		qsBuffer.sprintf("seek %f %d\n", fValue, iMode);
	else if (iMode == 2)	// seek in absolute total movie
		qsBuffer.sprintf("seek %d %d\n", (int)fValue, iMode);
	else	{
		printf ("Bad seeking mode, only 1 and 0 allowed\n");
		return;
	}
	writeToStdin(qsBuffer);
}

void QMPlayerWidget::slotSetSeek(int iNewValue)
{
	m_iSeekValue=iNewValue;
}

// Non mandatory functions.
void QMPlayerWidget::setAspectRatio (uint iAspectRatio)
{
	// Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
	QString qsBuffer;
	float fAspectRatio = 1.0f;
	switch (iAspectRatio)	{
	case ASPECT_AUTO:
		return;
	break;
	case ASPECT_34:
		fAspectRatio = 3.0f / 4.0f;
	break;
	case ASPECT_169:
		fAspectRatio = 16.0f / 9.0f;
	break;
	case ASPECT_SQUARE:
	default:
		fAspectRatio = 1.0f;
	break;
	}
	qsBuffer.sprintf("switch_aspect %f\n", fAspectRatio);
	writeToStdin(qsBuffer);
}

bool QMPlayerWidget::isPlaying()
{
	//return m_pMasterProcess->isRunning();		//ooo
	return !m_pMasterProcess->atEnd();		//xxx
}

void QMPlayerWidget::playChapter (int iChapter)
{
	QString qsBuffer;
	qsBuffer.sprintf("pt_step %d\n", iChapter);
	writeToStdin(qsBuffer);
}

