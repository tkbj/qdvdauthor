/***************************************************************************
    execute.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file handles the user input to generate a DVD menu.
   Please note that this is work in progress and depends also on the
   developement of dvdauthor, which is at the moment in version 0.67.
   Future versions of dvdauthor might need changes to this file.
    
****************************************************************************/

#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "execute.h"
#include "global.h"

#include <QPushButton>
#include <QTextEdit>
#include <QFile>		//xxx
#include <QSpacerItem>		//xxx
//#include <QPalette>		//xxx
#include <QDir>			//xxx
#include <QMessageBox>
#include <QStringList>
#include <QTimer>
#include <QCheckBox>
#include <QLineEdit>
#include <QSplitter>
#include <QFileDialog>
#include <QTextStream>
#include <QGridLayout>
#include <QHBoxLayout>

#include "win32.h"
#include "utils.h"
#include "messagebox.h"
#include "errormessages.h"

Execute::Execute ( QObject * parent, const char * name)	//ooo
//Execute::Execute ( QObject * parent)			//xxx
	//: Q3Process (parent, name)	//ooo
	: QProcess (parent)		//xxx
{
	initMe();
}

/*Execute::Execute ( const QString & arg0, QObject * parent, const char * name)
	: Q3Process (arg0, parent, name)
{
	initMe ();
}*/	//oooo

/*Execute::Execute ( const QStringList & args, QObject * parent, const char * name)
	: Q3Process (args, parent, name)
{
	initMe ();
}*/	//oooo

Execute::~Execute ()
{

}

void Execute::initMe ()
{
	m_iMaxWait = 10;
	m_iSeconds = 15;
	m_iCurrentCommand = 0;
	m_pDialog = NULL;
}

void Execute::raise ()
{
	if (m_pDialog)
		m_pDialog->raise ();
}

void Execute::createDialog(bool bKeep)
{
  // if the dialog ought to remain, then don't do nothin'
//printf ("bKeep=<%d> dialog=<%X>\n", bKeep, m_pDialog);
  if ( (bKeep) && (m_pDialog) )
    return;
  // else remove the current dialog, and proceed ...
  if (m_pDialog)
    delete m_pDialog;

  m_pDialog = new MyDialog ();
  m_pDialog->show();

  m_pDialog->editCommandText->clear ();

  m_pDialog->setCloseDelay(m_iSeconds);

  //m_pDialog->textOutput->append ("big bang");		//xxx
  
  connect ( m_pDialog, SIGNAL(destroyed()), this, SLOT(slotDialogClosed  ()) );
  //connect( this, SIGNAL(readyReadStderr()), this, SLOT(slotReadFromStderr()) );			//ooo
  connect( this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadFromStderr()) );			//xxx
  //connect( this, SIGNAL(readyReadStdout()), this, SLOT(slotReadFromStdout()) );			//ooo
  connect( this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadFromStdout()) );			//xxx
  //connect( this, SIGNAL(processExited  ()), this, SLOT(slotProcessExited ()) );			//ooo
  connect( this, SIGNAL(finished  ( int, QProcess::ExitStatus)), this, SLOT(slotProcessExited ()) );	//xxx
}

int Execute::system( QStringList &commandList, QStringList *pListEnv)
{
//  if (isRunning ())
//  return -1;
  createDialog();
  
  m_environmentList.clear();
  m_commandList = commandList;
  if (m_commandList.count() == 0)
    m_commandList.append (QString (""));
  if (pListEnv)
    m_environmentList = *pListEnv;
  m_iCurrentCommand = 0;

  return startCommand ();
}

int Execute::system( QString &commandString, QStringList *pListEnv)
{
//	if (isRunning ())
//		return -1;
	// Should work just as the system () call.
	// The only difference is that this call is non blocking ...
	// NOTE : commandString could hold multiple commands.
	createDialog ();

	m_environmentList.clear();
	m_commandList.clear ();
	m_commandList.append (commandString);
	if (pListEnv)
		m_environmentList = *pListEnv;
	m_iCurrentCommand = 0;
	return startCommand ();
}

int Execute::renderMenu (QString qsMenuName)
{
	// Here we start rendering the menu ...
	// This will go to QDVDAuthor::slotRenderMenu and from there to the propper DVDMenu
	emit (signalRenderMenu(qsMenuName));
	// Increase the command counter ...
	m_iCurrentCommand ++;
	// Start the next process, or start the timer to terminate the whole thing ...
//	if (startCommand() == 0)
//		m_pDialog->processingFinished (m_iSeconds);
	return 1;
}

int Execute::startCommand ()
{
  QStringList *pListEnv = NULL;
  // This function is called from system, and from slotProcessExited ();

  // First we check if there are any commands left in the QStringList.
  if (m_iCurrentCommand > (uint)m_commandList.count()-1)	{
    // Do some cleaning up. Not needed but better to be done ...
    m_commandList.clear ();
    m_iCurrentCommand = 0;
    return 0;
  }
  // second we take care of the environment.
  if (!m_environmentList.isEmpty())
    pListEnv = &m_environmentList;
  // Here we add the command to the top (command line) in the GUI.
  m_pDialog->editCommandText->append (QString ("cmd> ") + m_commandList[m_iCurrentCommand]);

  // if the command is to render a menu it is done internally and not through a command line (yet)
  // Thus we do the rendering and return here ...
  //if (m_commandList[m_iCurrentCommand].find( QString(RENDER_MENU)) > -1)	//ooo
  if (m_commandList[m_iCurrentCommand].indexOf( QString(RENDER_MENU)) > -1)	//xxx
    return renderMenu(m_commandList[m_iCurrentCommand].right(m_commandList[m_iCurrentCommand].length() - QString (RENDER_MENU).length()));	//oooo

  // Here we generate a simple shell script to execute the commands under the shell.
  // QProcess is currently not flexible enough to handle this internal.
  QString qsExecute = Global::qsTempPath + QString ( "/execute.sh" );

  QFile file ( qsExecute );
  if ( file.open( QIODevice::WriteOnly ) ) {
    QTextStream stream( &file );
    stream << Global::qsBashString << m_commandList[m_iCurrentCommand];
    /*stream << "\n";		//xxx
    m_iCurrentCommand++;	//xxx
    while (m_iCurrentCommand < (uint)m_commandList.size()) {	//xxx
	stream << m_commandList[m_iCurrentCommand];
	stream << "\n";		//xxx
	m_iCurrentCommand++;	//xxx
    }*/
    file.close();
  }

  // And here we grant read/write/execute permission.
  //chmod ( qsExecute.utf8 ( ), S_IXGRP | S_IWGRP | S_IRGRP | S_IEXEC | S_IRUSR | S_IWRITE | S_IROTH | S_IXOTH | S_IWOTH );	//oooo
  chmod ( qsExecute.toUtf8 ( ), S_IXGRP | S_IWGRP | S_IRGRP | S_IEXEC | S_IRUSR | S_IWRITE | S_IROTH | S_IXOTH | S_IWOTH );	//xxx
  // Increase the command counter ...
  m_iCurrentCommand ++;		//ooo
  // Set the script to execute ...
  //clearArguments ( );			//oooo
  //addArgument    ( qsExecute );	//ooo
  //setArguments    ( qsExecute );	//xxx
  // And eh voila, ici on executer les commands.
  //start ( pListEnv );			//oooo
  //start ( pListEnv );			//xxx
  start ( qsExecute );				//xxx
  // return to the calling process (thread)
  
  /*while (m_iCurrentCommand < m_commandList.size()) {	//xxx
    if ( file.open( QIODevice::WriteOnly ) ) {
      QTextStream stream( &file );
      stream << Global::qsBashString << m_commandList[m_iCurrentCommand];
      file.close();
    }
    chmod ( qsExecute.toUtf8 ( ), S_IXGRP | S_IWGRP | S_IRGRP | S_IEXEC | S_IRUSR | S_IWRITE | S_IROTH | S_IXOTH | S_IWOTH );
    start ( qsExecute );
    m_iCurrentCommand++;
    
    if (m_iCurrentCommand < m_commandList.size())
      m_pDialog->editCommandText->append (QString ("cmd> ") + m_commandList[m_iCurrentCommand]);
  }*/		//xxxx
  
  return 1;
}
// How about adding another timer, which verifies that the process did not die for what reasons ever ?
// Also the tatus could indicate <Running>...<blank>...<Running>...<blank>...
// I have to think about it since a avi->mpeg2 conversion would def. take longer then a few seconds ...

void Execute::slotProcessExited ()
{ 
  if (m_pDialog)	{
    slotReadFromStdout();
    slotReadFromStderr();
  }
  else
    //printf ("<%s><%s>\n", (const char *)readStdout(), (const char *)readStderr());	//ooo
    printf ("<%s><%s>\n", readAllStandardOutput ().data(), readAllStandardError ().data());
  // Start the next process, or start the timer to terminate the whole thing ...
  if (startCommand() == 0)
    m_pDialog->processingFinished (m_iSeconds);	//oooo
}

void Execute::setCloseDelay (uint iSeconds)
{
	// This function sets the number of seconds the GUI should wait
	// AFTER the process has terminated before auto closing the GUI.
	// range can be [0 - int]
	m_iSeconds = iSeconds;
}

void Execute::slotReadFromStderr()
{
  static QColor errorColor = QColor (255, 0, 0);
  if (m_pDialog)	{
    // Okay, errors red, warnings yellow please ...
    //QColor stdColor = m_pDialog->textOutput->color();		//ooo
    QColor stdColor = m_pDialog->textOutput->textColor();	//xxx
    //QString strError = readStderr ();				//ooo
    QString strError = readAllStandardError ().data();		//xxx
    //if (strError.lower().find ("err") > -1)			//ooo
    if (strError.toLower().indexOf ("err") > -1)		//xxx
      errorColor = QColor(255, 0, 0);
    else
      errorColor = QColor (220,220,0);
    //m_pDialog->textOutput->setColor (errorColor);		//ooo
    m_pDialog->textOutput->setTextColor (errorColor);		//xxx
    m_pDialog->textOutput->append (strError);
    //m_pDialog->textOutput->setColor (stdColor);		//ooo
    m_pDialog->textOutput->setTextColor (stdColor);		//xxx
  }
  else
    //printf ("<%s>\n", (const char *)readStderr());		//ooo
    printf ("<%s>\n", readAllStandardError().data());		//xxx
}

void Execute::slotReadFromStdout()
{
  if (m_pDialog)	{
    //m_pDialog->textOutput->setColor (QColor( 27, 212, 7 ) );		//ooo
    m_pDialog->textOutput->setTextColor (QColor( 27, 212, 7 ) );	//xxx
    //m_pDialog->textOutput->append (readStdout ());			//ooo
    m_pDialog->textOutput->append (readAllStandardOutput ().data());	//xxx
  }
  else
    //printf ("<%s>\n", (const char *)readStdout());		//oooo
    printf ("<%s>\n", readAllStandardOutput().data());		//xxx
}

//MyDialog::MyDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )	//ooo
MyDialog::MyDialog( QWidget* parent )							//xxx
    //: QDialog( parent, name, modal, fl )	//ooo
    : QDialog( parent )				//xxx
{
  
	/*if ( !name )
		setName( "ExecuteDialog" );
	setSizeGripEnabled( TRUE );*/		//oooo
	//MyDialogLayout = new Q3GridLayout( this, 1, 1, 11, 6, "MyDialogLayout");	//ooo
	MyDialogLayout = new QGridLayout( this ); 					//xxx

	//layout2 = new Q3HBoxLayout( 0, 0, 6, "layout2");	//ooo
	layout2 = new QHBoxLayout( ); 				//xxx

	//buttonHelp = new QPushButton( this, "buttonHelp" );	//ooo
	buttonHelp = new QPushButton( this );			//xxx
	buttonHelp->setAutoDefault( true );
	layout2->addWidget( buttonHelp );

	//checkKeepOpen = new QCheckBox( this, "checkKeepOpen" );	//ooo
	checkKeepOpen = new QCheckBox( "checkKeepOpen", this  );	//xxx
	layout2->addWidget( checkKeepOpen );
	QSpacerItem* spacer = new QSpacerItem( 310, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout2->addItem( spacer );

	//buttonAnalyze = new QPushButton( this, "buttonAnalyze" );	//ooo
	buttonAnalyze = new QPushButton( this );			//xxx
	buttonAnalyze->setAutoDefault  ( true  );
	//buttonAnalyze->setEnabled      ( false );
	layout2->addWidget             ( buttonAnalyze );

	//buttonExport = new QPushButton( this, "buttonExport" );	//ooo
	buttonExport = new QPushButton( this );				//xxx
	buttonExport->setAutoDefault( true );
	layout2->addWidget( buttonExport );

	//buttonPlay = new QPushButton( this, "buttonPlay" );	//ooo
	buttonPlay = new QPushButton( this );			//xxx
	buttonPlay->setAutoDefault( true );
	buttonPlay->setEnabled(false);
	layout2->addWidget( buttonPlay );

	//buttonOk = new QPushButton( this, "buttonOk" );	//ooo
	buttonOk = new QPushButton( this );			//xxx
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	layout2->addWidget( buttonOk );

	//buttonCancel = new QPushButton( this, "buttonCancel" );	//ooo
	buttonCancel = new QPushButton( this );				//xxx
	buttonCancel->setAutoDefault( true );
	layout2->addWidget( buttonCancel );

	MyDialogLayout->addLayout( layout2, 2, 0 );

	//editCloseText = new QLineEdit( this, "editCloseText" );		//ooo
	editCloseText = new QLineEdit( this );					//xxx
	//editCloseText->setPaletteForegroundColor( QColor( 170, 0, 255 ) );	//oooo
	//editCloseText->setPaletteBackgroundColor( QColor( 170, 170, 255 ) );	//oooo
	QPalette palette;	//xxx
	palette.setBrush(editCloseText->backgroundRole(), QColor( 170, 170, 255 ));	//xxx
	editCloseText->setPalette(palette);						//xxx
	//editCloseText->setFrameShape( QLineEdit::NoFrame );	//ooo
	editCloseText->setFrame( true );			//xxx
	//editCloseText->setFrameShadow( QLineEdit::Plain );	//ooo
	editCloseText->setReadOnly( true );

	MyDialogLayout->addWidget( editCloseText, 1, 0 );

	//splitter1 = new QSplitter( this, "splitter1" );	//oooo
	splitter1 = new QSplitter( this );			//xxx
	splitter1->setOrientation( Qt::Vertical );

	//editCommandText = new Q3TextEdit( splitter1, "editCommandText" );	//ooo
	editCommandText = new QTextEdit( splitter1 );				//xxx
	//editCommandText->setAutoFillBackground(true);				//xxx
	editCommandText->setMinimumSize( QSize( 0, 50 ) );
	editCommandText->setMaximumSize( QSize( 32767, 32767 ) );
	editCommandText->setBaseSize (QSize (editCommandText->width(), 50));
	//editCommandText->setPaletteForegroundColor( QColor( 255, 85, 0 ) );	//oooo
	editCommandText->setTextColor( QColor( 255, 85, 0 ) );			//xxx
	//editCommandText->setPaletteBackgroundColor( QColor( 0, 0, 0 ) );	//oooo
	//editCommandText->setTextBackgroundColor( QColor( 0, 0, 0 ) );		//xxx
	QPalette palette2;	//xxx
	//palette2.setBrush(editCommandText->backgroundRole(), QColor( 0, 0, 0 ));	//xxx
	palette2.setBrush(QPalette::Base, QColor( 0, 0, 0 ));			//xxx
	editCommandText->setPalette(palette2);					//xxx
	//editCommandText->setWordWrap( QTextEdit::WidgetWidth );		//ooo
	editCommandText->setWordWrapMode( QTextOption::WordWrap );		//xxx

	//textOutput = new Q3TextEdit( splitter1, "textOutput" );		//ooo
	textOutput = new QTextEdit( splitter1 );				//xxx
	//textOutput->setAutoFillBackground(true);				//xxx
	//textOutput->setPaletteForegroundColor( QColor( 27, 212, 7 ) );	//oooo
	textOutput->setTextColor( QColor( 27, 212, 7 ) );			//xxx
	//textOutput->setPaletteBackgroundColor( QColor( 0, 0, 0 ) );		//oooo
	//textOutput->setTextBackgroundColor( QColor( 0, 0, 0 ) );		//xxx
	QPalette palette3;	//xxx
	//palette3.setBrush(textOutput->backgroundRole(), QColor( 0, 0, 0 ));	//xxx
	palette3.setBrush(QPalette::Base, QColor( 0, 0, 0 ));	//xxx
	textOutput->setPalette(palette3);					//xxx
	//textOutput->setWordWrap( QTextEdit::WidgetWidth );			//ooo
	textOutput->setWordWrapMode( QTextOption::WordWrap );			//xxx

	MyDialogLayout->addWidget( splitter1, 0, 0 );
	languageChange();
	resize( QSize(740, 471).expandedTo(minimumSizeHint()) );
	//clearWState( WState_Polished );			//ooo
	setAttribute(Qt::WA_WState_Polished, false);		//xxx

	// signals and slots connections
	connect( buttonAnalyze, SIGNAL( clicked ( ) ),     this, SLOT( slotAnalyze ( ) ) );
	connect( buttonExport,  SIGNAL( clicked ( ) ),     this, SLOT( slotExport  ( ) ) );
	connect( buttonPlay,    SIGNAL( clicked ( ) ),     this, SLOT( slotPlay    ( ) ) );
	connect( buttonOk,      SIGNAL( clicked ( ) ),     this, SLOT( accept      ( ) ) );
	connect( buttonCancel,  SIGNAL( clicked ( ) ),     this, SLOT( reject      ( ) ) );
	connect( checkKeepOpen, SIGNAL( toggled ( bool ) ),this, SLOT( slotKeepOpen( bool ) ) );

	char *pCheckClosed = getenv ( "QDVD_KEEPDLGOPEN" );
	if (  pCheckClosed )
	  checkKeepOpen->setChecked ( true );

	m_bDoneProcessing = false;
	m_pTimer = NULL;
	QList<int> listSizes;	//oxx
	listSizes.append    (    50     );
	listSizes.append    (   1000    );
	splitter1->setSizes ( listSizes );
}

MyDialog::~MyDialog()
{
	// I know Qt does this but it is good programming style, 
	// to take care of all allocated objects.
	if (m_pTimer)
		delete m_pTimer;
}

void MyDialog::setCloseDelay (uint iSeconds)
{
	// This function sets the number of seconds the GUI should wait
	// AFTER the process has terminated before auto closing the GUI.
	// range can be [0 - int]
	m_iSeconds = iSeconds;
}

void MyDialog::languageChange()
{
	//setCaption( tr( "Execution Dialog" ) );			//ooo
	buttonHelp->setText( tr( "&Help" ) );
	//buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );		//ooo
	checkKeepOpen->setText( tr( "Keep Open" ) );
	buttonAnalyze->setText( tr( "&Analyze log" ) );
	//buttonAnalyze->setAccel( QKeySequence( QString::null ) );	//ooo
	buttonExport->setText( tr( "&Export" ) );
	//buttonExport->setAccel( QKeySequence( QString::null ) );	//ooo
	buttonPlay->setText( tr( "&Play" ) );
	//buttonPlay->setAccel( QKeySequence( QString::null ) );	//ooo
	buttonOk->setText( tr( "&OK" ) );
	//buttonOk->setAccel( QKeySequence( QString::null ) );		//ooo
	buttonCancel->setText( tr( "&Cancel" ) );
	//buttonCancel->setAccel( QKeySequence( QString::null ) );	//ooo
	editCloseText->setText( tr( "Status : " ) );
}

void MyDialog::slotTimerDone ()
{
	static uint iCounter = 0;

	iCounter ++;
	if (iCounter > m_iSeconds)	{
		m_pTimer->stop();
		iCounter = 0;
		editCloseText->setText (tr ("Status : Done."));
		accept ();
	}
	editCloseText->setText (tr ("Status : Close dialog in %1 seconds.").arg (m_iSeconds - iCounter + 1));
}

void MyDialog::slotAnalyze ( )
{
  // this function will scan through the output and report 
  // the most common error messages.

  ErrorMessages errors;
  int t, iErrorCounter = 0;
  QSize dialogSize ( 700, 500 );
  QString qsHTML, qsCaption;
  //QString qsOutputText = textOutput->text( );		//ooo
  QString qsOutputText = textOutput->toPlainText( );	//xxx

  qsHTML  = "<TABLE WIDTH=\"100%\" ><TR><TD BGCOLOR=\"#9A3CFF\"><FONT SIZE=+2><B><CENTER>"; 
  qsHTML += tr ( "Please note this dialog only displays <B>KNOWN</B> errors.<P>" );
  qsHTML += "</CENTER></B></FONT>";
  qsHTML += tr ( "If the DVD still fails to build consider sending the file created through " );
  qsHTML += tr ( "[<FONT COLOR=WHITE>Tools -> feedback</FONT>] to the author of QDVDAuthor." );
  qsHTML += "</TD></TR></TABLE><HR>";
  for ( t=0; t<errors.count ( );t++ ) {
    //if ( qsOutputText.find ( errors.errorString ( t ) ) > -1 ) {	//ooo
    if ( qsOutputText.indexOf ( errors.errorString ( t ) ) > -1 ) {	//xxx
      iErrorCounter ++;
      qsHTML += errors.helpMessage ( t );
    }
  }

  if ( iErrorCounter == 0 ) {
    qsHTML = tr ( "<P><FONT COLOR=DARKGREEN>Did not find any known error.</FONT><P>" );
    qsCaption = tr ( "No errors found." );
  }
  else if ( iErrorCounter == 1 )
    qsCaption = tr ( "Found 1 error" );
  else
    qsCaption = tr ( "Found %1 errors" ).arg ( iErrorCounter );
  MessageBox::html ( NULL, qsCaption, qsHTML, dialogSize  );

}

void MyDialog::processingFinished (int iSeconds)
{  
  buttonAnalyze->setEnabled ( true );
  // We're done, timer stuff now ok
  m_bDoneProcessing = true;
  QString qsStorage = Global::qsTempPath + QString ("/") + Global::qsProjectName + QString ("/output.txt");
  saveOutput (qsStorage);
  // Start the timer if the checkbox is clear
  if (!checkKeepOpen->isChecked())        {
    startTimer(iSeconds);
  }
  // First we check if qplayer is there and executable ...
  QString qsPlayer = Global::qsExternalPlayer;
  QFileInfo fileInfo    ( qsPlayer );

  if ( !fileInfo.exists ( ) ) {
    Utils theUtils;
    qsPlayer = theUtils.checkForExe( qsPlayer );
    if ( qsPlayer.isEmpty ( ) )
      return;
    fileInfo.setFile ( qsPlayer );
  }

  // next we check if it is executable ...
  if (!fileInfo.isExecutable())
    return;
  // last we check if there has been something clreated to play at all ...
  fileInfo.setFile ( Global::qsProjectPath + QString ( "/VIDEO_TS/VIDEO_TS.BUP" ) );
  if ( ! fileInfo.exists ( ) )
    return;
  fileInfo.setFile ( Global::qsProjectPath + QString ( "/VIDEO_TS/VIDEO_TS.IFO" ) );
  if ( ! fileInfo.exists ( ) )
    return;
  fileInfo.setFile ( Global::qsProjectPath + QString ( "/VIDEO_TS/VIDEO_TS.VOB" ) );
  if ( ! fileInfo.exists ( ) )
    return;

  writeZoneCode ( );
  buttonPlay->setEnabled ( true );
}

void MyDialog::writeZoneCode ( )
{
  if ( ( Global::iRegionalZone == 0 ) || ( Global::iRegionalZone > 6 ) )
    return;

  const unsigned char arrayCodes[] = { 0x00, 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF };
  // * Region 1 - The U.S., U.S. territories and Canada
  // * Region 2 - Europe, Japan, the Middle East, Egypt, South Africa, Greenland
  // * Region 3 - Taiwan, Korea, the Philippines, Indonesia, Hong Kong
  // * Region 4 - Mexico, South America, Central America, Australia, New Zealand, Pacific Islands, Caribbean
  // * Region 5 - Russia (okay, former Russia), Eastern Europe, India, most of Africa, North Korea, Mongolia
  // * Region 6 - China 

  QFile ifo ( Global::qsProjectPath + "/VIDEO_TS/VIDEO_TS.IFO" );
  for ( int t=0; t<2; t++ )  {
    if  ( ifo.open ( QIODevice::ReadWrite ) == true )  {
      //if ( ifo.at  ( 0x23 ) )		//ooo
      if ( ifo.seek  ( 0x23 ) )		//xxx
        //ifo.putch  ( arrayCodes[Global::iRegionalZone] );	//ooo
	ifo.putChar  ( arrayCodes[Global::iRegionalZone] );	//xxx
    }
    ifo.close      ( );
    //ifo.setName    ( Global::qsProjectPath + "/VIDEO_TS/VIDEO_TS.BUP" );	//ooo
    ifo.setFileName    ( Global::qsProjectPath + "/VIDEO_TS/VIDEO_TS.BUP" );	//xxx
  }
}

// The next three are related to the Keep Open countdown
void MyDialog::startTimer (int iSeconds)
{
	m_iSeconds = iSeconds;
	if (m_iSeconds > 0)     {
		if (m_pTimer)
			delete m_pTimer;
		m_pTimer = new QTimer ( this );
		connect( m_pTimer, SIGNAL(timeout()), this, SLOT(slotTimerDone()) );
		m_pTimer->start (1000);
	}
}

void MyDialog::stopTimer ()
{
	if (m_pTimer)	{
		if (m_pTimer->isActive())	{
			m_pTimer->stop ();
		}
	}
}

void MyDialog::slotKeepOpen (bool bKeepOpen)
{
	// This could be called before processingFinished()
	if (! m_bDoneProcessing)
		return;
	if (bKeepOpen)	{
		stopTimer ();
		editCloseText->setText (tr ("Status : "));
	}
	else
		startTimer (m_iSeconds);
}

void MyDialog::slotPlay ()
{
  QString qsCommand, qsPlayer, qsFullPath;
  Utils theUtils;
  QDir thePath ( Global::qsProjectPath );

  qsPlayer = Global::qsExternalPlayer;

  qsPlayer = theUtils.checkForExe( qsPlayer );
  if ( qsPlayer.isEmpty ( ) )
    qsPlayer = Global::qsExternalPlayer;

  //qsCommand = QString ( "%1 dvd://%2/VIDEO_TS/ &" ).arg ( qsPlayer ).arg ( thePath.absPath ( ) );	//ooo
  qsCommand = QString ( "%1 dvd://%2/VIDEO_TS/ &" ).arg ( qsPlayer ).arg ( thePath.absolutePath ( ) );	//xxx

  //if ( system ( qsCommand.ascii ( ) ) == -1 )		//ooo
  if ( system ( qsCommand.toLatin1().data ( ) ) == -1 )	//xxx
    return; // error
	
}

void MyDialog::slotExport ()
{
  //QString qsSaveFileName = Q3FileDialog::getSaveFileName( Global::qsProjectPath, tr("Text files (*.txt *.TXT)"),	//ooo
  //  this, tr("Save file dialog. Choose a filename to save under"));							//ooo
  QString qsSaveFileName = QFileDialog::getSaveFileName( this, tr("Save file dialog. Choose a filename to save under"),	//xxx
    Global::qsProjectPath, tr("Text files (*.txt *.TXT)"));								//xxx

  if (qsSaveFileName.isEmpty())
    return;
  saveOutput (qsSaveFileName);
}

void MyDialog::saveOutput (QString qsFileName)
{
	QFile file( qsFileName );
	if ( file.open( QIODevice::WriteOnly ) ) {
		QTextStream stream( &file );
		//stream << editCommandText->text() << "\n";		//ooo
		stream << editCommandText->toPlainText() << "\n";	//xxx
		stream << "\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n\n";
		//stream << textOutput->text() << "\n";		//ooo
		stream << textOutput->toPlainText() << "\n";	//xxx
		file.close();
	}
}
