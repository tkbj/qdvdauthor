/***************************************************************************
    dialogexecute.cpp
                             -------------------
    This is the CommandQueueDialog
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include <QFrame>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QScrollArea>
#include <QTimer>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

#include "dialogexecute.h"
#include "dialogman.h"
#include "qdvdauthorinit.h"
#include "dialogcommandeditor.h"
#include "messagebox.h"

DialogExecute::GuiBlock::GuiBlock ( )
{
  pLabelComment          = NULL;
  pComboTranscode        = NULL;
  pComboCommands         = NULL;
  pButtonMan             = NULL;
  pButtonEdit            = NULL;
  pButtonSetAsDefault    = NULL;
  iBlockID               = -1;
  iBlockExecuteInterface = -1;
}

DialogExecute::DialogExecute(QList<ExecuteInterface *> listExecuteBlocks, bool bShowDialog )
    : QDialog( )
{
  uint t;

  // Init member variables.
  QStringList commandList;
  m_pInitObject      = NULL;	//ooo
  m_pTranscodeParent = NULL;
  m_pTranscodeLayout = NULL;
  m_iTranscodeOffset = 0;
  m_pSubtitlesParent = NULL;
  m_pSubtitlesLayout = NULL;
  m_pSlideshowParent = NULL;
  m_pSlideshowLayout = NULL;
  m_iSubtitlesOffset = 0;
 
  // Here we concat all commands together ...
  //for (t=0;t<(uint)listExecuteBlocks.count();t++)	// Lets get the default commands ...                    //ooo
  for (t=0;t<static_cast<uint> ( listExecuteBlocks.count() );t++)	// Lets get the default commands ...      //xxx
      commandList += listExecuteBlocks[t]->listCommandList[0];
	//ooo  
  //for (t=0;t<listExecuteBlocks.count();t++)	// Lets get the default commands ...
  //  for (i=0;i<listExecuteBlocks[t]->listCommandList.count();i++)	// Lets get the default commands ...
  //    commandList.append(listExecuteBlocks[t]->listCommandList[i][listExecuteBlocks[t]->listDefaults[t]]);

  // debugging the output.
/*  for (t=0;t<(uint)listExecuteBlocks.count();t++)  {
    for (int i=0;i<listExecuteBlocks[t]->listCommandList.count ();i++)  {
      QStringList qsCommandList = listExecuteBlocks[t]->listCommandList[i];
      for (int j=0;j<qsCommandList.count();j++)  {
        //printf ( "DialogExecute::DialogExecute <%d><%d><%d> = <%s>\n", t, i, j, qsCommandList[j].ascii() );			//ooo
	printf ( "DialogExecute::DialogExecute <%d><%d><%d> = <%s>\n", t, i, j, qsCommandList[j].toLatin1().data ( ) );		//xxx
      }
    }
  }
  printf ("\n\n");
*/  //ooo//

  // In case the user does not want to display a execute Dialog but instead prefers the default settings.
  if ( ! bShowDialog )
    m_doSomething.system ( commandList );		//oooo

  createMainForm       ( );

  createPreprocessing  ( );			//ooo
  createDVDAuthorTab   ( listExecuteBlocks );	//oooo

  QTimer::singleShot ( 50, this, SLOT ( slotResize ( ) ) );
}

DialogExecute::~DialogExecute()
{
  // Destroys the object and frees any allocated resources
  // no need to delete child widgets, Qt does it all for us
  int t;
  //for ( t=0; t<(int)m_listGui.count(); t++ )                  //ooo
  for ( t=0; t<static_cast<int> ( m_listGui.count() ); t++ )    //xxx
    delete m_listGui[t];
  m_listGui.clear ( );

  delete m_pInitObject;	//oooo
}

void DialogExecute::slotResize ( )
{
  // Delayed initialization to have the dialog on screen for resize.
  createPostprocessing ( );	//ooo
  languageChange ( );		//ooo
  resize ( QSize ( 877, 662 ).expandedTo ( minimumSizeHint ( ) ) );
  slotCheckBlock ( true );	//ooo
}

void DialogExecute::setInit ( QDVDAuthorInit *pInit )
{ 
  m_pInitObject = pInit;
  // Here we save over the ToolsPaths. This list will be removed from the InitObject ...
  m_listToolsPaths = pInit->getToolsPaths ( );
}

void DialogExecute::createMainForm ( )
{ 
  //setName( "DialogExecute" );		//ooo
  //setObjectName( "DialogExecute" );	//xxx
  setWindowTitle( "DialogExecute" );	//xxx
  //setSizeGripEnabled( TRUE );		//ooo
  setSizeGripEnabled( true );		//xxx
  //DialogExecuteLayout = new Q3GridLayout( this, 1, 1, 11, 6, "DialogExecuteLayout");	//ooo
  DialogExecuteLayout = new QGridLayout( this );					//xxx

  //Layout1 = new Q3HBoxLayout( 0, 0, 6, "Layout1");	//ooo
  Layout1 = new QHBoxLayout( );				//xxx

  //buttonHelp = new QPushButton( this, "buttonHelp" );	//ooo
  buttonHelp = new QPushButton( "&Help", this );	//xxx
  //buttonHelp->setAutoDefault( TRUE );			//ooo
  buttonHelp->setAutoDefault( true );			//xxx
  buttonHelp->setDefault( true );                       //xxxx
  Layout1->addWidget( buttonHelp );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( spacer );

  //buttonExport = new QPushButton( this, "buttonExport" );	//ooo
  buttonExport = new QPushButton( "&Export", this );		//xxx
  //buttonExport->setAutoDefault( TRUE );			//ooo
  buttonExport->setAutoDefault( true );				//xxx
  Layout1->addWidget( buttonExport );

  //buttonOk = new QPushButton( this, "buttonOk" );		//ooo
  buttonOk = new QPushButton( "&Ok", this );			//xxx
  //buttonOk->setAutoDefault( TRUE );				//ooo
  buttonOk->setAutoDefault( true );				//xxx
  //buttonOk->setDefault( TRUE );				//ooo
  //buttonOk->setDefault( true );					//xxxx
  Layout1->addWidget( buttonOk );

  //buttonCancel = new QPushButton( this, "buttonCancel" );	//ooo
  buttonCancel = new QPushButton( "&Cancel", this );		//xxx
  //buttonCancel->setAutoDefault( TRUE );			//ooo
  buttonCancel->setAutoDefault( true );				//xxx
  Layout1->addWidget( buttonCancel );

  DialogExecuteLayout->addLayout( Layout1, 1, 0 );

  //m_pTabWidget = new QTabWidget( this, "m_pTabWidget" );	//ooo
  m_pTabWidget = new QTabWidget( this );			//xxx
  //m_pTabWidget->setEnabled( TRUE );				//ooo
  m_pTabWidget->setEnabled( true );				//xxx

  // signals and slots connections
  connect ( buttonOk     , SIGNAL ( clicked() ), this, SLOT( slotOkay  ( ) ) );
  connect ( buttonCancel , SIGNAL ( clicked() ), this, SLOT( reject    ( ) ) );
  connect ( buttonExport , SIGNAL ( clicked() ), this, SLOT( slotExport( ) ) );
  connect ( buttonHelp ,   SIGNAL ( clicked() ), this, SLOT( slotHelp  ( ) ) );
  connect ( m_pTabWidget , SIGNAL ( currentChanged   ( QWidget * ) ), this, SLOT ( slotTabChanged ( QWidget * ) ) );
  connect (&m_doSomething, SIGNAL ( signalRenderMenu ( QString   ) ), this, SLOT ( slotRenderMenu ( QString   ) ) );	//ooo
}

void DialogExecute::createPreprocessing ()
{
//	m_pTabPreparing = new QWidget( m_pTabWidget, "m_pTabPreparing" );
//	m_pTabWidget->insertTab( m_pTabPreparing, QString("Preparing") );
}

QGridLayout *DialogExecute::createBlockFrame(QWidget *pParent, QGridLayout *pParentLayout, ExecuteInterface *pInterface, int t, int iOffset)
{   
  QGridLayout *pGroupBoxLayout;
  QCheckBox   *pCheckBox, *pCheckBox2;
  QGroupBox   *pGroupBox;

  //pGroupBox = new Q3GroupBox( pParent, QString ("pGroupBox_%1").arg(t) );	//ooo
  pGroupBox = new QGroupBox( QString ("pGroupBox_%1").arg(t), pParent  );		//xxx
  //pGroupBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pGroupBox->sizePolicy().hasHeightForWidth() ) );	//ooo
  //pGroupBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );	//xxxx
  //pGroupBox->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );	//xxx
  pGroupBox->setAlignment( Qt::AlignLeft ); //xxx
  pGroupBox->sizePolicy().setHeightForWidth(true);				//xxxx
  //pGroupBox->sizePolicy().hasHeightForWidth();				        //xxx
  //pGroupBox->sizePolicy().setVerticalPolicy(QSizePolicy::Minimum);		//xxx
  //pGroupBox->setPaletteBackgroundColor(pInterface->backgroundColor);		//ooo
  QPalette palette;								//xxx
  //palette.setBrush(QPalette::Window, QBrush(pInterface->backgroundColor));	//xxx
  palette.setBrush(QPalette::Base, QBrush(pInterface->backgroundColor));	//xxx
  pGroupBox->setPalette(palette);						//xxx
  //pGroupBox->setColumnLayout(0, Qt::Vertical );	//ooo
  //pGroupBox->layout()->setSpacing( 6 );		//ooo
  //pGroupBox->setSpacing( 6 );				//xxx
  //pGroupBox->layout()->setMargin( 11 );		//ooo
  m_listGroupBox.append (pGroupBox);
 
  //pGroupBoxLayout = new Q3GridLayout( pGroupBox->layout() );	//oooo
  pGroupBoxLayout = new QGridLayout( pGroupBox );	//xxx
  pGroupBoxLayout->setAlignment( Qt::AlignTop );
 
  //pCheckBox = new QCheckBox (pGroupBox, QString ("pCheckBox_%1").arg(t));	//ooo
  pCheckBox = new QCheckBox (QString ("pCheckBox_%1").arg(t), pGroupBox );	//xxx
  pCheckBox->setChecked (pInterface->bShowBlock);
  pCheckBox->setMaximumWidth (25);
  pCheckBox->move (15, 0);
  connect( pCheckBox, SIGNAL( toggled(bool) ), this, SLOT( slotCheckBlock(bool) ) );
  m_listCheckBox.append(pCheckBox);

  if (iOffset == -1)	{
    pGroupBox->setTitle( QString ("           ") + pInterface->qsBlockName );
    //pCheckBox2 = new QCheckBox (tr ("        Don't execute ") + 			//ooo
    //	pInterface->qsBlockName, pParent, QString ("pCheckBox2_%1").arg(t));		//ooo
    pCheckBox2 = new QCheckBox ( tr ("        Don't execute ") +pInterface->qsBlockName, pParent);	//xxx
    pCheckBox2->setAccessibleName(QString ("pCheckBox2_%1").arg(t));    //xxx
  }
  else	{ // this will get the fileBaseName (as spec in the ini after the two possible conv tools)    
    //pGroupBox->setTitle (QString ("           Convert : ") + pInterface->listCommandList[0][pInterface->listSplitAt.count()] );   //ooo
    pGroupBox->setTitle (QString ("           Convert : ") /*+ pInterface->listCommandList[0][pInterface->listSplitAt.count()-1]*/);   //xxx
    pGroupBox->setAlignment(Qt::AlignLeft); //xxx
    //pCheckBox2 = new QCheckBox (tr ("        Don't convert ") + 			//ooo
    //	pInterface->listCommandList[0][pInterface->listSplitAt.count()], 	//ooo
    //	pParent, QString ("pCheckBox2_%1").arg(t));				//ooo
    pCheckBox2 = new QCheckBox (tr ("        Don't convert ") /*+ pInterface->listCommandList[0][pInterface->listSplitAt.count()-1]*/, pParent);	//xxx
    pCheckBox2->setAccessibleName(QString ("pCheckBox2_%1").arg(t));    //xxx
  }
  //pCheckBox2->setPaletteBackgroundColor(pInterface->backgroundColor);		//ooo
  //QPalette palette2;								//xxx
  //palette.setBrush(QPalette::Window, QBrush(pInterface->backgroundColor));	//xxx
  palette.setBrush(QPalette::Base, QBrush(pInterface->backgroundColor));	//xxx
  pCheckBox2->setPalette(palette);						//xxx
  //palette = pCheckBox2->palette();						//xxx
  //palette.setColor(QPalette::Button, pInterface->backgroundColor);		//xxx
  connect( pCheckBox2, SIGNAL( toggled(bool) ), this, SLOT( slotCheckBlock(bool) ) );
  pCheckBox2->hide ();
  m_listCheckBox2.append(pCheckBox2);

  pParentLayout->addWidget( pCheckBox2, 2*t+1, 0 );   //ooo
  //pParentLayout->addWidget( pCheckBox2, 2*t+2, 0 );   //xxx
  //pParentLayout->addWidget( pGroupBox, 2*t+2, 0 );    //ooo
  pParentLayout->addWidget( pGroupBox, 2*t+2, 0, Qt::AlignLeft );    //xxx
  //pParentLayout->addWidget( pGroupBox, 2*t+3, 0 );    //xxx

  return pGroupBoxLayout;
}

void DialogExecute::createCommandBlock (QWidget *pParent, QGridLayout *pParentLayout, ExecuteInterface *pInterface, int t, int i)
{
        int iYPos, iDefaultComment;
	QLabel		*pComment;
	QComboBox	*pCommand;
	QPushButton	*pButton, *pButtonEdit, *pButtonDefault;

	// Add another Layout with d/e - buttons oriented Vertically and in between the man button and the Combo box
	QVBoxLayout *pEDButtons;
	// setting : m_listCheckBox/m_listCheckBox2/m_listGroupBox
	iYPos = 2*i;
	//pEDButtons   = new QVBoxLayout( 2, QString ("buttonbox_%1-%2t").arg(t).arg(i));		//ooo
	pEDButtons   = new QVBoxLayout( );								//xxx
    pEDButtons->setObjectName(QString ("buttonbox_%1-%2t").arg(t).arg(i));  //xxx
	//pComment     = new QLabel( pParent,   QString ("comment_%1-%2t").arg(t).arg(i));		//ooo
	pComment     = new QLabel( QString ("comment_%1-%2t").arg(t).arg(i), pParent );			//xxx
	//pCommand     = new QComboBox( FALSE, pParent, QString ("command_%1-%2t").arg(t).arg(i));	//ooo
	pCommand     = new QComboBox( pParent );							//xxx
    pCommand->setAccessibleName(QString ("command_%1-%2t").arg(t).arg(i)); //xxx
	//pButton      = new QPushButton( pParent, QString ("button_%1-%2t" ).arg(t).arg(i));		//ooo
	pButton      = new QPushButton( QString ("button_%1-%2t" ).arg(t).arg(i), pParent );		//xxx
	pButton->setText( tr( "man" ) );
	//pButtonEdit  = new QPushButton( pParent, QString ("button_%1-%2t" ).arg(t).arg(i));		//ooo
	pButtonEdit  = new QPushButton( QString ("button_%1-%2t" ).arg(t).arg(i), pParent );		//xxx
	pButtonEdit->setMaximumSize ( 25, 25 );
	pButtonEdit->setText( tr( "e" ) );
	// If this is the internal render then we don't want to allow the user to change the command.
	//pButtonDefault  = new QPushButton( pParent, QString ("button_%1-%2t" ).arg(t).arg(i));	//ooo
	pButtonDefault  = new QPushButton( QString ("button_%1-%2t" ).arg(t).arg(i), pParent );		//xxx
	pButtonDefault->setMaximumSize ( 25, 25 );
	pButtonDefault->setText( tr( "d" ) );
	//if (pInterface->listCommandList[i][0].find (QString (RENDER_MENU)) > -1)	{	//ooo
	if (pInterface->listCommandList[i][0].indexOf (QString (RENDER_MENU)) > -1)	{	//xxx
		pCommand->setEditable(false);
		pButtonEdit->setEnabled (false);
	}
	else
		pCommand->setEditable(true);
	iDefaultComment = pInterface->listDefaults[i];
	//if ( iDefaultComment >= (int)pInterface->listCommentList[i].count ( ) )              //ooo
        if ( iDefaultComment >= static_cast<int> ( pInterface->listCommentList[i].count ( ) ) ) //xxx
	     iDefaultComment = 0;

	//for (int x=0; x<pInterface->listCommentList.count ( ); x++ ) {
	//  for (int y=0;y<pInterface->listCommentList[x].count ( ); y++ ) {
	//    printf ( "Norm  [%d][%d]=<%s>\n", x, y, pInterface->listCommentList[x][y].ascii () );
	//  }
	//}

	//pCommand->insertStringList ( pInterface->listCommandList[i] );			//ooo
	pCommand->addItems ( pInterface->listCommandList[i] );					//xxx
	pComment->setText          ( pInterface->listCommentList[i][iDefaultComment] );
	//pCommand->setCurrentItem   ( pInterface->listDefaults   [i] );			//ooo
	pCommand->setCurrentIndex   ( pInterface->listDefaults   [i] );				//xxx

	pEDButtons->addWidget( pButtonEdit );
	pEDButtons->addWidget( pButtonDefault );
	//pParentLayout->addMultiCellWidget( pComment, iYPos, iYPos, 0, 2 );	//ooo
	pParentLayout->addWidget( pComment, iYPos, 0, 1, 1 );		//xxx
	//pParentLayout->addWidget( pCommand,   1+iYPos, 0 );  //ooo
        //pParentLayout->addWidget( pCommand,   1+iYPos, 0, Qt::AlignLeft );     //xxx
        pParentLayout->addWidget( pCommand,   1+iYPos, 0, 1, 1 );               //xxx
	//pParentLayout->addLayout( pEDButtons, 1+iYPos, 1 );  //ooo
        pParentLayout->addLayout( pEDButtons, 1+iYPos, 1, Qt::AlignRight   );     //xxx
        //pParentLayout->addLayout( pEDButtons, 1+iYPos, 1, 1, 1 );     //xxx
	//pParentLayout->addWidget( pButton,    1+iYPos, 2 );  //ooo
        pParentLayout->addWidget( pButton,    1+iYPos, 2, Qt::AlignRight   );     //xxx
        //pParentLayout->addWidget( pButton,    1+iYPos, 2, 1, 1 );     //xxx
        
        //pParentLayout->setColumnStretch(1, 2000);     //xxx
	
	GuiBlock *pGuiBlock               = new GuiBlock;
	pGuiBlock->listComments           = pInterface->listCommentList[i];
	pGuiBlock->pComboCommands         = pCommand;
	pGuiBlock->pLabelComment          = pComment;
	pGuiBlock->pButtonMan             = pButton;
	pGuiBlock->pButtonEdit            = pButtonEdit;
	pGuiBlock->pButtonSetAsDefault    = pButtonDefault;
	pGuiBlock->iBlockID               = pInterface->listBlockIDs[i];
	pGuiBlock->iBlockExecuteInterface = t;// this connects the ExecuteInterface with the generated Block
	m_listGui.append ( pGuiBlock );

	connect (pCommand,       SIGNAL(activated (int)), this, SLOT(slotNormalSelectionChanged(int)));	//oooo
	connect (pButton,        SIGNAL(clicked ()), this, SLOT(slotManButtonClicked()));
	connect (pButtonEdit,    SIGNAL(clicked ()), this, SLOT(slotEditButtonClicked()));
	connect (pButtonDefault, SIGNAL(clicked ()), this, SLOT(slotDefaultButtonClicked()));			//oooo
}

void DialogExecute::createStandardBlock (QWidget *pParent, QGridLayout *pParentLayout, ExecuteInterface *pInterface, int t)
{ 
	int i;
	QGridLayout	*pGroupBoxLayout;
	// setting : m_listCheckBox/m_listCheckBox2/m_listGroupBox
	pGroupBoxLayout = createBlockFrame (pParent, pParentLayout, pInterface, t); 

	//for (i=0;i<(int)pInterface->listCommandList.count();i++)                 //ooo
        for (i=0;i<static_cast<int> ( pInterface->listCommandList.count() );i++)    //xxx
		createCommandBlock (m_listGroupBox.last(), pGroupBoxLayout, pInterface, t, i);
}

void DialogExecute::createTranscodeBlock (ExecuteInterface *pInterface, int t)
{
  int i, j, iYPos, iDefaultComment;
  QLabel		*pComment;
  QComboBox	*pCommand;
  QPushButton	*pButtonDefault;
  QGridLayout *pGroupBoxLayout = NULL;
  
  // m_listCheckBox/m_listCheckBox2/m_listGroupBox
  if  ( ! m_pTranscodeLayout )	{
    m_pTranscodeLayout = createBlockFrame (m_pScrollFrame, m_pScrollFrameLayout, pInterface, t); 
    m_pTranscodeParent = m_listGroupBox.last();
  }
  iYPos = 2*m_iTranscodeOffset++;

  pGroupBoxLayout = createBlockFrame (m_pTranscodeParent, m_pTranscodeLayout, pInterface, t, m_iTranscodeOffset);
  //pComment = new QLabel( m_listGroupBox.last(),   QString ("comment_%1-0").arg(t));		//ooo
  pComment = new QLabel( QString ("comment_%1-0").arg(t), m_listGroupBox.last() );		//xxx
  pComment->setAlignment( Qt::AlignLeft );    //xxx
  //pCommand = new QComboBox( FALSE, m_listGroupBox.last(), QString ("command_%1-0").arg(t));	//ooo
  pCommand = new QComboBox( m_listGroupBox.last() );						//xxx
  pCommand->setAccessibleName(QString ("command_%1-0").arg(t)); //xxx
  // If this is the internal render then we don't want to allow the user to change the command.
  //pButtonDefault  = new QPushButton( m_listGroupBox.last(), QString ("button_%1-0" ).arg(t));	//ooo
  pButtonDefault  = new QPushButton( QString ("button_%1-0" ).arg(t), m_listGroupBox.last() );	//xxx
  pButtonDefault->setMaximumSize ( 25, 25 );
  pButtonDefault->setText( tr( "d" ) );
  //pCommand->setEditable(FALSE);		//ooo
  pCommand->setEditable(false);		//xxx
  iDefaultComment = pInterface->listDefaults[0];

  //if ( iDefaultComment >= (int)pInterface->listCommentList[0].count ( ) )                 //ooo
  if ( iDefaultComment >= static_cast<int> ( pInterface->listCommentList[0].count ( ) ) )   //xxx
    iDefaultComment = 0;
  //for (i=0;i<(int)pInterface->listSplitAt.count ();i++)                   //ooo
  for (i=0;i<static_cast<int> ( pInterface->listSplitAt.count () );i++) {     //xxx  
    //pCommand->insertItem ( pInterface->listCommandList[0][i] );	//ooo
    pCommand->addItem ( pInterface->listCommandList[0][i] );	//xxx
  }

  pComment->setText            ( pInterface->listCommentList[0][iDefaultComment] );   //ooo-bug
   
  //pCommand->setCurrentItem     ( pInterface->listDefaults   [0] );	//oooo
  pCommand->	setCurrentIndex     ( pInterface->listDefaults   [0] );	//xxx

  //pGroupBoxLayout->addMultiCellWidget ( pComment, iYPos, iYPos, 0, 1 );       //ooo
  //pGroupBoxLayout->addWidget ( pComment, iYPos, 0, 0, 1, Qt::AlignHCenter );  //xxx
  //pGroupBoxLayout->addWidget ( pComment, iYPos, 0, 0, 1, Qt::AlignLeft );     //xxx
  pGroupBoxLayout->addWidget ( pComment, iYPos, 0, 1, 1 );     //xxx
  iYPos++;
  //pGroupBoxLayout->addMultiCellWidget ( pCommand, iYPos, iYPos, 0, 1 );       //ooo
  ///pGroupBoxLayout->addWidget ( pCommand, iYPos, 0, 0, 1, Qt::AlignLeft );     //xxx
  pGroupBoxLayout->addWidget ( pCommand, iYPos, 0, 1, 1, Qt::AlignLeft );       //xxx
  //pGroupBoxLayout->addWidget ( pButtonDefault, iYPos, 2 );                    //ooo
  //pGroupBoxLayout->addWidget ( pButtonDefault, iYPos, 1, Qt::AlignLeft );     //xxx
  pGroupBoxLayout->addWidget ( pButtonDefault, iYPos, 0, Qt::AlignHCenter  );   //xxx
  iYPos++;

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //for (int x=0; x<pInterface->listCommandList.count ( ); x++ ) {
  //  for (int y=0;y<pInterface->listCommandList[x].count ( ); y++ ) {
  //    printf ( "Trans Command [%d][%d]=<%s>\n", x, y, pInterface->listCommandList[x][y].ascii ( ) );
  //  }
  //}
  //for (int x=0; x<pInterface->listCommentList.count ( ); x++ ) {
  //  for (int y=0;y<pInterface->listCommentList[x].count ( ); y++ ) {
  //    printf ( "Trans Comment [%d][%d]=<%s>\n", x, y, pInterface->listCommentList[x][y].ascii ( ) );
  //  }
  //}
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  GuiBlock *pGuiBlock               = new GuiBlock;
  pGuiBlock->listComments           = pInterface->listCommentList[0];
  pGuiBlock->pComboTranscode        = pCommand;
  pGuiBlock->pComboCommands         = pCommand;
  pGuiBlock->pLabelComment          = pComment;
  pGuiBlock->pButtonMan             = NULL;
  pGuiBlock->pButtonEdit            = NULL;
  pGuiBlock->pButtonSetAsDefault    = pButtonDefault;
  pGuiBlock->iBlockID               = pInterface->listBlockIDs[0];
  pGuiBlock->iBlockExecuteInterface = t;// this connects the ExecuteInterface with the generated Block
  m_listGui.append ( pGuiBlock );

  connect (pCommand,       SIGNAL(activated (int)), this, SLOT(slotTranscodeSelectionChanged(int)));
  connect (pButtonDefault, SIGNAL(clicked ()), this, SLOT(slotDefaultButtonClicked()));

  QGridLayout *pGroupBoxLayout2;
  QGroupBox   *pGroupBox;
  int iSplitAtStart, iSplitAtEnd;

  //for (i=0;i<(int)pInterface->listSplitAt.count ();i++)	{             //ooo
  for (i=0;i<static_cast<int> ( pInterface->listSplitAt.count () );i++)	{   //xxx   
    iSplitAtStart = pInterface->listSplitAt[i] + 1;
 
    //if (i+1 < (int)pInterface->listSplitAt.count ())                  //ooo
    if (i+1 < static_cast<int> ( pInterface->listSplitAt.count () ) )   //xxx
      iSplitAtEnd = pInterface->listSplitAt[i+1] + 1;
    else
      iSplitAtEnd = pInterface->listCommandList.count ();
  
//printf ("DialogExecute::createTranscodeBlock i<%d> count<%d> count<%d> <%d>\n", i, pInterface->listDefaults.count (), pInterface->listCommandList.count (),  pInterface->listDefaults[i]);
    //pGroupBox = new Q3GroupBox( m_listGroupBox.last(), QString ("pGroupBox_%1").arg(t) );		//ooo
    pGroupBox = new QGroupBox( QString ("pGroupBox_%1").arg(t), m_listGroupBox.last() );		//xxx
    //pGroupBox->setPaletteBackgroundColor(pInterface->backgroundColor);	//oooo
    //pGroupBox->setColumnLayout(0, Qt::Vertical );	//oooo

    //pGroupBoxLayout2 = new Q3GridLayout( pGroupBox->layout() );	//oooo
    pGroupBoxLayout2 = new QGridLayout( pGroupBox );	//xxx
    pGroupBoxLayout2->setAlignment( Qt::AlignTop );

    //pGroupBoxLayout->addMultiCellWidget ( pGroupBox, iYPos, iYPos, 0, 2 );	//oooo
    pGroupBoxLayout->addWidget ( pGroupBox, iYPos, 0 );				//xxx
//		pGroupBoxLayout->addWidget ( pGroupBox, iYPos, 0 );
    iYPos ++;
  
    for (j=iSplitAtStart;j<iSplitAtEnd;j++)
      createCommandBlock (pGroupBox, pGroupBoxLayout2, pInterface, t, j);	//oooo
    // Hide everything but the default selection.
    if (pInterface->listDefaults[0] != i)
      pGroupBox->hide ();
    pGuiBlock->listTranscodeFrames.append ( pGroupBox );
    //		m_listTranscodeFrames.append ( pGroupBox );
  }
}

void DialogExecute::createSubtitlesBlock ( ExecuteInterface *pInterface, int t )
{
  if  ( ! m_pSubtitlesLayout )	{
    QString qsBlockName     = pInterface->qsBlockName;
    pInterface->qsBlockName = tr ( "Subtitles Block" );
    m_pSubtitlesLayout      = createBlockFrame    ( m_pScrollFrame, m_pScrollFrameLayout, pInterface, t ); 
    m_pSubtitlesParent      = m_listGroupBox.last ( );
    pInterface->qsBlockName = qsBlockName;
  }
  createStandardBlock ( m_pSubtitlesParent, m_pSubtitlesLayout, pInterface, t );
}

void DialogExecute::createSlideshowBlock ( ExecuteInterface *pInterface, int t )
{ 
  if  ( ! m_pSlideshowLayout )	{
    QString qsBlockName     = pInterface->qsBlockName;
    pInterface->qsBlockName = tr ( "Slideshow Block" );
    m_pSlideshowLayout      = createBlockFrame    ( m_pScrollFrame, m_pScrollFrameLayout, pInterface, t ); 
    m_pSlideshowParent      = m_listGroupBox.last ( );
    pInterface->qsBlockName = qsBlockName;
  }
  createStandardBlock ( m_pSlideshowParent, m_pSlideshowLayout, pInterface, t );
}

void DialogExecute::createDVDAuthorTab   (QList<ExecuteInterface *> listExecuteBlocks)
{ 
  uint t;
  m_listExecuteInterface = listExecuteBlocks;
  ExecuteInterface *pExecute = NULL;

  // Here we create the ScrollView and Layouts ...
  //m_pTabExpert   = new QWidget( m_pTabWidget, "m_pTabExpert" );			//ooo
  m_pTabExpert   = new QWidget( m_pTabWidget );					//xxx
  //tabLayout      = new Q3GridLayout( m_pTabExpert, 1, 1, 11, 6, "tabLayout");	//ooo
  tabLayout      = new QGridLayout( m_pTabExpert );				//xxx
  //m_pScrollView  = new Q3ScrollView (m_pTabExpert, "ScrollView");		//ooo
  m_pScrollView  = new QScrollArea ( m_pTabExpert );				//xxx
  m_pScrollView->setWidgetResizable( true );					//xxx
  //m_pScrollFrame = new Q3Frame( m_pScrollView, "m_pScrollFrame" );		//ooo
  m_pScrollFrame = new QFrame( m_pScrollView->widget() );					//xxx
  //m_pScrollFrame->setPaletteBackgroundColor( QColor( 238, 230, 234 ) );		//oooo
  //m_pScrollFrame->setFrameShape( Q3Frame::StyledPanel );			//ooo
  m_pScrollFrame->setFrameShape( QFrame::StyledPanel );				//xxx
  //m_pScrollFrame->setFrameShadow( Q3Frame::Raised );				//ooo
  m_pScrollFrame->setFrameShadow( QFrame::Raised );				//xxx
  //m_pScrollFrameLayout = new Q3GridLayout( m_pScrollFrame, 1, 1, 11, 6, "m_pScrollFrameLayout");	//ooo
  m_pScrollFrameLayout = new QGridLayout( m_pScrollFrame );						//xxx

  //for ( t=0; t<(uint)listExecuteBlocks.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( listExecuteBlocks.count ( ) ); t++ )  {  //xxx
    pExecute = listExecuteBlocks[t];

    // Check if this is a transcoding block ...
    //if ( pExecute->enType == ExecuteInterface::TYPE_TRANSCODING )//( pExecute->listSplitAt.count ( ) > 0 )
    if ( pExecute->listSplitAt.count ( ) > 0 )
      createTranscodeBlock ( pExecute, t );
    else if ( pExecute->enType == ExecuteInterface::TYPE_SLIDESHOW )
      //createStandardBlock  ( m_pScrollFrame, m_pScrollFrameLayout, pExecute, t );
      createSlideshowBlock ( pExecute, t );	//oooo
    else if ( pExecute->enType == ExecuteInterface::TYPE_SUBTITLES )
      //createStandardBlock  ( m_pScrollFrame, m_pScrollFrameLayout, pExecute, t );
      createSubtitlesBlock ( pExecute, t );	//oooo
    else // TYPE_DEFAULT
      createStandardBlock  ( m_pScrollFrame, m_pScrollFrameLayout, pExecute, t );
  }

  //m_pScrollView->addChild  ( m_pScrollFrame );		//ooo
  m_pScrollView->setWidget  ( m_pScrollFrame );		//xxx

  //m_pFrameExplain = new Q3Frame( m_pScrollFrame, "m_pFrameExplain" );	//ooo
  m_pFrameExplain = new QFrame( m_pScrollFrame );				//xxx
  //m_pFrameExplain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_pFrameExplain->sizePolicy().hasHeightForWidth() ) );	//ooo
  m_pFrameExplain->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );	//xxxx
  //pGroupBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );	//xxx
  m_pFrameExplain->sizePolicy().setHeightForWidth(true);				//xxxx
  //m_pFrameExplain->sizePolicy().hasHeightForWidth();				//xxx
  //m_pFrameExplain->setPaletteBackgroundColor( QColor( 177, 211, 238 ) );	//ooo
  m_pFrameExplain->setFrameShape( QFrame::StyledPanel );
  //m_pFrameExplain->setFrameShadow( Q3Frame::Sunken );	//ooo
  m_pFrameExplain->setFrameShadow( QFrame::Sunken );	//xxx
  m_pFrameExplain->setMaximumHeight(150);
  //m_pFrameExplainLayout = new Q3GridLayout( m_pFrameExplain, 1, 1, 11, 6, "m_pFrameExplainLayout");	//ooo
  m_pFrameExplainLayout = new QGridLayout( m_pFrameExplain );						//xxx

  //m_pLabelExplain = new QLabel( m_pFrameExplain, "m_pLabelExplain" );		//ooo
  m_pLabelExplain = new QLabel( m_pFrameExplain );				//xxx
  //m_pLabelExplain->setPaletteBackgroundColor( QColor( 177, 211, 238 ) );	//xxx
  QPalette palette;							        //xxx
  //palette.setBrush(m_pLabelExplain->backgroundRole(), QColor( 177, 211, 238 ));	//xxx
  palette.setBrush(QPalette::Base, QColor( 177, 211, 238 ));	               //xxx
  m_pLabelExplain->setPalette(palette);						//xxx

  m_pFrameExplainLayout->addWidget( m_pLabelExplain, 0, 0 );          //ooo
  //m_pFrameExplainLayout->addWidget( m_pLabelExplain, 0, 0, 1, 1 );    //xxx

  m_pScrollFrameLayout->addWidget( m_pFrameExplain, 0, 0 );       //ooo
  //m_pScrollFrameLayout->addWidget( m_pFrameExplain, 0, 0, 1, 1 );   //xxx

  tabLayout->addWidget( m_pScrollView, 0, 0 );        //ooo
  //tabLayout->addWidget( m_pScrollView, 0, 0, 1, 1 );    //xxx
  //m_pTabWidget->insertTab( m_pTabExpert, QString("") );		//ooo
  m_pTabWidget->addTab( m_pTabExpert, QString("") );		//xxx

  DialogExecuteLayout->addWidget( m_pTabWidget, 0, 0 );       //ooo
  //DialogExecuteLayout->addWidget( m_pTabWidget, 0, 0, 1, 1 );   //xxx
}

void DialogExecute::createPostprocessing ()
{
//	m_pTabPostprocessing = new QWidget( m_pTabWidget, "m_pTabPostprocessing" );
//	m_pTabWidget->insertTab( m_pTabPostprocessing, QString("Cleaning") );
}

void DialogExecute::slotNormalSelectionChanged (int iNewSelection)
{
  uint t;
  GuiBlock *pGuiBlock;
  // Called when the user changes from e.g Transcode, to MEncoder ...
  //for ( t=0; t<(uint)m_listGui.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( m_listGui.count ( ) ); t++ )  {  //xxx
    pGuiBlock = m_listGui[t];
    if ( ( ! pGuiBlock ) || ( ! pGuiBlock->pComboCommands ) )
      continue;
    if ( pGuiBlock->pComboCommands->hasFocus ( ) )	{
      //if ( iNewSelection >= (int)pGuiBlock->listComments.count ( ) )                  //ooo
      if ( iNewSelection >= static_cast<int> ( pGuiBlock->listComments.count ( ) ) )    //xxx
	   iNewSelection = 0;
      pGuiBlock->pLabelComment->setText ( pGuiBlock->listComments[iNewSelection] );
      slotTabChanged (m_pTabExpert);
      return;
    }
  }
}

void DialogExecute::slotTabChanged(QWidget *pTab)
{
  int iWidth, iViewWidth, iOffset;
  uint t;
  QPoint pos, globPos;
  GuiBlock *pGuiBlock;
 
  if (pTab == m_pTabExpert)	{
    m_pScrollFrame->adjustSize();
    //iViewWidth = m_pScrollView->visibleWidth();	//ooo
    iViewWidth = m_pScrollView->width();		//xxx
    if (iViewWidth < 100)
      iViewWidth = 200;
    //for ( t=0; t<(uint)m_listGui.count(); t++ )	{             //ooo
    for ( t=0; t<static_cast<uint> ( m_listGui.count() ); t++ )	{   //xxx
      pGuiBlock = m_listGui[t];
      iOffset = 0;
      pos = pGuiBlock->pComboCommands->pos();
      globPos = pGuiBlock->pComboCommands->mapTo ( m_pScrollFrame, pos );
      // If the this is not a SubSubFrame, then we can extend it by 11 (Layout margin)
      if ( pGuiBlock->pComboCommands->parentWidget ( )->parent ( ) == m_pScrollFrame )
	iOffset = 11;
      iWidth = iViewWidth - 139 + iOffset - globPos.x();
      if (iWidth < 100)
	iWidth = 200;
      pGuiBlock->pComboCommands->setFixedWidth(iWidth);
    }
  }
}

void DialogExecute::slotCheckBlock(bool)
{
	static bool bWorking = false;
	// This function activates/deactivates all command lines in the checked // unchecked command block
	uint t;
	bool bShowCommandBlock;
	// This check will prevent the setChecked function calls in this function to mess up the logic.
	if (bWorking)
		return;
	bWorking = true;
	//for (t=0;t<(uint)m_listGroupBox.count();t++)	{                   //ooo
        for (t=0;t<static_cast<uint> ( m_listGroupBox.count() );t++)	{  //xxx
		bShowCommandBlock = true;
		// First we need t figure out the logic ...
		// We are only interested in the negatives (I.e. when the block is NOT to be shown)
		// since the default is to show the command block.
		if ( (m_listGroupBox[t]->isVisible () ) && (!m_listCheckBox[t]->isChecked()) )
			bShowCommandBlock = false;
		if ( (!m_listGroupBox[t]->isVisible ()) && (!m_listCheckBox2[t]->isChecked()) )
			bShowCommandBlock = false;

		if (bShowCommandBlock)	{
			m_listCheckBox2[t]->hide();
			m_listGroupBox[t]->show ();
			m_listCheckBox[t]->setChecked (true);
		}
		else	{
			m_listGroupBox[t]->hide ();
			m_listCheckBox2[t]->show();
			m_listCheckBox2[t]->setChecked (false);
		}
	}
	bWorking = false;
}

// Sets the strings of the subwidgets using the current language.
void DialogExecute::languageChange()
{
    //setCaption( tr( "Command Queue Dialog" ) );		//ooo
    setWindowTitle( tr( "Command Queue Dialog" ) );		//xxx
    /*buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    buttonExport->setText( tr( "&Export" ) );
    buttonExport->setAccel( QKeySequence( QString::null ) );*/		//ooo
//    m_pTabWidget->changeTab( tab, tr( "Simple" ) );
    //m_pLabelExplain->setText( tr( "<p align=\"center\"><b><h2>Here is the list of commands which will be executed. You can modify each single Command or accept the default settings.</h2></b></p>" ) );	//ooo
    m_pLabelExplain->setText( tr( "<p align=\"center\"><b><h2>Here is the list of commands which will be executed.<br> You can modify each single Command or accept the default settings.</h2></b></p>" ) );	//xxx
    //m_pTabWidget->changeTab( m_pTabExpert, tr( "Authoring" ) );	//ooo
    m_pTabWidget->setTabText( 0, tr( "Authoring" ) );			//xxx
}

void DialogExecute::slotHelp ()
{
  MessageBox::warning (NULL, tr ("Help"), 
    tr ("In this dialog you see all commands that are going to get executed to create the DVD.\n\n"
	"[Export] Allows you to export all those commands to a script and execute one it fomr the command line.\n"
	"         Note: You can execute one command at a time. E.g. script.sh 1; script.sh 2 etc.\n"
	"[Help]   Displays this message.\n"
	"[e]      Opens the CommandEditorDialog. Using this button allows you to modify (and store) the command.\n"
	"[d]      Will set the current selected choice in the combo box as the default command.\n"
	"[man]    Will open a new dialog, which displays the man pages of all toools used in this command line.\n"
	"         Note: You can easily search for text through this dialog.\n"
	"[X]      (checkbox) this will enable / disable the commands for the whole selected command block.\n"
     ), QMessageBox::Ok , QMessageBox::NoButton );
  raise ();
}

void DialogExecute::slotManButtonClicked ( )
{
  QStringList listOfCommandsUsed;
  QWidget    *pFromList;
  QWidget    *pButton = childAt ( mapFromGlobal ( QCursor::pos ( ) ) );
  GuiBlock *pGuiBlock;

  if (!pButton)
    return;
  WId buttonID = pButton->winId();
  //for ( uint t=0; t<(uint)m_listGui.count ( ); t++ ) {                //ooo
  for ( uint t=0; t<static_cast<uint> ( m_listGui.count ( ) ); t++ ) {  //xxx
    pGuiBlock = m_listGui[t];
    pFromList = pGuiBlock->pButtonMan;
    if ( ! pFromList )
      continue;
    if ( pGuiBlock->pButtonMan->winId ( ) == buttonID )	{
      // Here we create the list of the commands in this commandString.
      QString qsCommand = pGuiBlock->pComboCommands->currentText ( );
      //QStringList partialCommandList = QStringList::split (";", qsCommand);	//ooo
      QStringList partialCommandList = qsCommand.split (";");			//xxx
      //for (uint i=0;i<(uint)partialCommandList.count();i++)	{                     //ooo
      for (uint i=0;i<static_cast<uint> ( partialCommandList.count() );i++)	{   //xxx
	//QStringList partialCommandList2 = QStringList::split ("|", partialCommandList[i]);	//ooo
	QStringList partialCommandList2 = partialCommandList[i].split ("|");			//xxx
	//for (uint j=0;j<(uint)partialCommandList2.count();j++)	{             //ooo
        for (uint j=0;j<static_cast<uint> ( partialCommandList2.count() );j++)	{    //xxx
	  //QStringList argumentList =  QStringList::split (" ", partialCommandList2[j]);	//ooo
	  QStringList argumentList =  partialCommandList2[j].split (" ");			//xxx
	  // the first argument is always the command.
	  // but we don't want the cd command.
	  if (argumentList[0] != "cd")
	    listOfCommandsUsed.append(argumentList[0]);
	}
      }
      createManPages (listOfCommandsUsed);
      return;
    }
  }
}

void DialogExecute::slotEditButtonClicked()
{
  uint t, i;
  QStringList listOfCommandsUsed;
  QWidget *pFromList;
  int iWhichKeyset = Keyword::Keys;
  GuiBlock *pGuiBlock;
  // In order to get the button we clicked we use the Cursor position ...
  QWidget *pButton = childAt ( mapFromGlobal ( QCursor::pos ( ) ) );
  if ( ! pButton )
    return;
  
  WId buttonID = pButton->winId ( );
  //for ( t=0; t<(uint)m_listGui.count  ( ); t++ )  {               //ooo
  for ( t=0; t<static_cast<uint> ( m_listGui.count  ( ) ); t++ )  { //xxx
    pGuiBlock = m_listGui[t];
    pFromList = pGuiBlock->pButtonEdit;
    if (!pFromList)
      continue;
    if (pFromList->winId() == buttonID)	{
      // Here we open the CommandEditDialog ...
      if (m_pInitObject)	{
	QDVDAuthorInit::MenuBlock::Block *pBlock = m_pInitObject->getBlock (pGuiBlock->iBlockID, &iWhichKeyset );
	if (pBlock)	{
	  // Here we get the index to the appropriate ExecuteInterface for the given Block
	  int iInterface = pGuiBlock->iBlockExecuteInterface;
	  Keyword *pKeyword = m_pInitObject->getKeyword ( );
	  DialogCommandEditor *pEditor = new DialogCommandEditor (this);
	  //pEditor->initMe (pBlock, pGuiBlock->pComboCommands->currentItem ( ), pKeyword->getKeys ( iWhichKeyset ), m_listExecuteInterface[iInterface]->listKeywords.count ( ) );	//ooo
	  pEditor->initMe (pBlock, pGuiBlock->pComboCommands->currentIndex ( ), pKeyword->getKeys ( iWhichKeyset ), m_listExecuteInterface[iInterface]->listKeywords.count ( ) );	//xxx
	  if ( pEditor->exec ( ) != QDialog::Rejected ) {
	    // Here we set the command line to the modified strings. 
	    // and then we set the keywords for the found ExecuteInterface - object.
	    pKeyword->setKeywords( iWhichKeyset, m_listExecuteInterface[iInterface]->listKeywords );
	    // and finally we generate the command / header with the correct keywords ...
	    pGuiBlock->pComboCommands->clear ( );
	    for ( i=0; i<(uint)pBlock->listCommands.count ( ); i++ )
		  //pGuiBlock->pComboCommands->insertItem( pKeyword->resolvString( iWhichKeyset, pBlock->listCommands[i] ) );	//ooo
	          pGuiBlock->pComboCommands->addItem( pKeyword->resolvString( iWhichKeyset, pBlock->listCommands[i] ) );	//xxx
	    pGuiBlock->pLabelComment->setText ( pKeyword->resolvString( iWhichKeyset, pBlock->listComments[0] ) );
	    m_pInitObject->setToolsPaths ( m_listToolsPaths );
	    m_pInitObject->saveIniFile   ( );
	    // this is to re-set the ToolsPaths information (keep it in here)
	    m_pInitObject->getToolsPaths ( );
	  }
	  delete pEditor;
	}
      }
      return;
    }
  }
}

void DialogExecute::slotDefaultButtonClicked()
{
  QWidget *pButton = childAt (mapFromGlobal(QCursor::pos()));
  GuiBlock *pGuiBlock;
  if (!pButton)
    return;
  WId buttonID = pButton->winId();
  //for ( uint t=0; t<(uint)m_listGui.count ( ); t++ )  {               //ooo
  for ( uint t=0; t<static_cast<uint> ( m_listGui.count ( ) ); t++ )  { //xxx
    pGuiBlock = m_listGui[t];
    if ( pGuiBlock->pButtonSetAsDefault->winId() == buttonID)	{
      if (m_pInitObject)	{
	//m_pInitObject->setDefault ( pGuiBlock->iBlockID, pGuiBlock->pComboCommands->currentItem ( ) );	//ooo
	m_pInitObject->setDefault ( pGuiBlock->iBlockID, pGuiBlock->pComboCommands->currentIndex ( ) );		//xxx
	m_pInitObject->setToolsPaths ( m_listToolsPaths );
	m_pInitObject->saveIniFile();
	// this is to re-set the ToolsPaths information (keep it in here)
	m_pInitObject->getToolsPaths ( );
      }
      return;
    }
  }
}

void DialogExecute::slotRenderMenu (QString qsMenuName)
{
	// Only here to forward the signal from the Execute - object (m_doSomething) to QDVDAuthor ...
	emit (signalRenderMenu(qsMenuName));
}

void DialogExecute::returnFromRenderMenu    ()
{
	// here we continue where we had a short break while rendering the menu.
	m_doSomething.raise();
	m_doSomething.startCommand ();
}

void DialogExecute::createManPages(QStringList &listOfCommandsUsed)
{
	// Create a modeless dialog, so we can work with DialogExecute and the DialogMan at the same time.
	DialogMan *pManDialog = new DialogMan(listOfCommandsUsed, this);
	pManDialog->show();
}

void DialogExecute::slotOkay()
{ 
  uint t;
  QStringList listCommands;
  GuiBlock *pGuiBlock;
  // first let us create the masks ...
  emit ( signalGenerateMasks   ( ) );

  //for ( t=0; t<(uint)m_listGui.count ( ); t++ )	{         //ooo
  for ( t=0; t<static_cast<uint> ( m_listGui.count ( ) ); t++ )	{   //xxx
  
    pGuiBlock = m_listGui[t];
    if ( pGuiBlock->pComboCommands->isVisible())
      listCommands.append ( getToolsPath ( pGuiBlock->pComboCommands->currentText ( ) ) );
  }

  m_doSomething.system ( listCommands );

  hide ();

  // for a blocking execution one has to start a timer, which checks m_doSomthing.isRunning()
  // And calls accept () only after all commands have been executed.
  // Also one needs to tak care of the user closing the execution-output-dialog.
  //	accept();
}

QString DialogExecute::getToolsPath(QString qsCommandLine)
{
	// This function will take the command line as an input and add the full path to each of the tools used in this cmmand line.
	// E.g. 
	// jpeg2yuv -n 50 -j "/tmp/Main Menu VMGMbackground.jpg" | mpeg2enc -n p -o "/tmp//Main Menu VMGM.m2v"
	// will be converted into 
	// /usr/bin/jpeg2yuv -n 50 -j "/tmp/Main Menu VMGMbackground.jpg" | /usr/local/bin/mpeg2enc -n p -o "/tmp//Main Menu VMGM.m2v"
	/////////////////////////////////////////////////////////////////////

	uint t;
	QString qsReturn;
	QString qsCommand;
	QRegExp regExp;
	qsReturn = qsCommandLine;

	//for (t=0;t<(uint)m_listToolsPaths.count ();t++)	{            //ooo
        for (t=0;t<static_cast<uint> ( m_listToolsPaths.count () );t++)	{   //xxx
		if (m_listToolsPaths[t]->bPresent)	{
 			// The first regExp takes care of all occurances of the tool which do NOT start with a '/'
			regExp = QRegExp ("(^|\\s(?!/+))"+m_listToolsPaths[t]->qsExecutableName+" ");
			qsReturn.replace (regExp, m_listToolsPaths[t]->qsFullPath+" ");
			// The second regExp takes care of the case where the user does not leave a blank between the pipe symbol '|' and the tool.
 			regExp = QRegExp ("\\|"+m_listToolsPaths[t]->qsExecutableName+" ");
			qsReturn.replace (regExp, "| "+m_listToolsPaths[t]->qsFullPath+" ");
		}
//		else
//			printf ("WARNING, %s not present in system.\n", (const char *) m_listToolsPaths[t]->qsExecutableName);
	}
	return qsReturn;
}
