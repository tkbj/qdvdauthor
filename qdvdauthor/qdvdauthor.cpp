/***************************************************************************
    qdvdauthor.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file handles the user input to generate a DVD menu.
   Please note that this is work in progress and depends also oin the
   developement of dvdauthor, which is at the moment in version 0.67.
   Future versions of dvdauthort might need changes to this file.
    
****************************************************************************/
#include <iostream>

#include <sys/stat.h>
#include <stdlib.h>

#include <QToolBar>
#include <QIcon>
//#include <QAction>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QTextEdit>
#include <QMessageBox>
#include <QTimer>
#include <QGridLayout>
#include <QDir>
#include <QPixmap>
#include <QPalette>
#include <QTextStream>
#include <QDomElement>
#include <QDomNode>

// Include the generated config files ...
#include "CONFIG.h"

#include "global.h"
#include "sourcefileentry.h"
#include "dialogabout.h"
#include "newwizard.h"
#include "messagebox.h"
#include "dialogsetup.h"
#include "qdvdauthor.h"
#include "dialogexecute.h"
#include "buttonobject.h"
#include "movieobject.h"
#include "colortoolbar.h"
#include "sourcetoolbar.h"
#include "structuretoolbar.h"
#include "dvdmenu.h"
#include "xml_dvd.h"
#include "qdvdauthorinit.h"
#include "undobuffer.h"
#include "dvdmenuundoobject.h"
#include "menupreview.h"
#include "qplayer/mediacreator.h"
#include "qrender/startmenurender.h"
//#include "xml_dvdauthor.h"	//ooo
#include "exportdvdauthor.h"	//ooo
//#include "subtitlesgui.h"     //ooo
#include "dvdlayoutgui.h"
#include "dialogundostack.h"
#include "cachethumbs.h"    //xxx

#include "render_client.h"
#include "plugins/interface.h"
#include "plugins/menuslide/menuslide.h"

/*QCustomEvent::QCustomEvent(int type)
   : QEvent(QCustomEvent::type())
{
  if ( type == EVENT_RENDER_PROGRESS )
  {
    customEventType = myType_Event_Render_Progress;
  }
}*/ //xxx

QDVDAuthor::QDVDAuthor(QWidget *parent)
    : QMainWindow(parent)
{
  Global::pApp = this;
  m_bModified            = false;
  m_pVMGMenu             = NULL;
  
  // Init the variables
  m_pDialogExecute       = NULL;
  m_pDVDLayoutGui        = NULL;
  m_pUndoStackDialog     = NULL;
  m_mainTabBuffer[0]     = NULL;
  m_mainTabBuffer[1]     = NULL;
  m_mainTabBuffer[2]     = NULL;
  m_pToolbar             = NULL;
  m_iMaxHistoryFiles     = 10;
  
  // Next we load init files and init the callbacks and GUI parts.
  initMe ( );
  
  /*setMinimumSize(1160, 720);
  
  QPixmap newpix("new.png");
  QPixmap openpix("open.png");
  QPixmap quitpix("quit.png");

  QAction *fileNewAction = new QAction("&New Project", this);
  QAction *fileWizardAction = new QAction("Project &Wizard ...", this);
  QAction *fileOpenAction = new QAction("&Open Project ...", this);
  QAction *fileSaveAction = new QAction("&Save Project", this);
  QAction *fileSaveAsAction = new QAction("Save Project &As...", this);
  QAction *m_pActionConvert = new QAction("&Convert Project", this);
  QAction *m_pActionImportDvdauthor = new QAction("Import &dvdauthor/spumux ...", this);
  QAction *m_pActionImportKino = new QAction("Import &Kino (smil)", this);
  QAction *m_pActionExportDVDAuthorXML = new QAction("&Export dvdauthor (xml)", this);
  QAction *fileExitAction = new QAction("&Exit", this);

  QMenu *fileMenu;
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(fileNewAction);
  fileMenu->addAction(fileWizardAction);
  fileMenu->addAction(fileOpenAction);
  fileMenu->addAction(fileSaveAction);
  fileMenu->addAction(fileSaveAsAction);
  fileMenu->addAction(m_pActionConvert);
  fileMenu->addSeparator();
  fileMenu->addAction(m_pActionImportDvdauthor);
  fileMenu->addAction(m_pActionImportKino);
  fileMenu->addAction(m_pActionExportDVDAuthorXML);
  fileMenu->addSeparator();
  fileMenu->addAction(fileExitAction);
  
  editUndoAction     = new QAction ( QIcon( ":/images/undo"        ), tr("&Undo"), this);
  editRedoAction     = new QAction ( QIcon( ":/images/redo"        ), tr("&Redo"), this);
  m_pActionUndoStack = new QAction ( QIcon ( ":/images/text_left.png"     ), tr("Undostack" ), this );
  
  fileMenu = menuBar()->addMenu("&Edit");
  fileMenu->addAction(editUndoAction);
  fileMenu->addAction(editRedoAction);
  fileMenu->addAction(m_pActionUndoStack);
  
  fileMenu = menuBar()->addMenu("DVD&Author");
  
  fileMenu = menuBar()->addMenu("DVD&Menu");

  QAction *toolsSetupAction = new QAction("&Setup ...", this);;
  
  fileMenu = menuBar()->addMenu("&Tools");
  fileMenu->addAction(toolsSetupAction);
  
  helpContentsAction = new QAction("&Contents...", this);
  helpGuideAction = new QAction("&Quick-Start Guide", this);
  helpAboutAction = new QAction("&About", this);
  
  fileMenu = menuBar()->addMenu("&Help");
  fileMenu->addAction(helpContentsAction);
  fileMenu->addSeparator();
  fileMenu->addAction(helpGuideAction);
  fileMenu->addAction(helpAboutAction);

  //fileNewAction->setObjectName(QString::fromUtf8("fileNewAction"));
  //fileNewAction->setName("fileNewAction");

  //fileNewAction->setIconText(QApplication::translate("CFormMain", "&New Project", 0, QApplication::UnicodeUTF8));

  connect(fileNewAction,    SIGNAL(triggered()), this, SLOT(fileNew()));
  connect(fileWizardAction, SIGNAL(activated()), qApp, SLOT(fileWizard()));
  connect(fileOpenAction,   SIGNAL(activated()), qApp, SLOT(fileOpen()));
  connect(fileSaveAction,   SIGNAL(activated()), this, SLOT(fileSave()));
  connect(fileSaveAsAction, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
  connect(editUndoAction,   SIGNAL(triggered()), this, SLOT(editUndo()));
  connect(editRedoAction,   SIGNAL(triggered()), this, SLOT(editRedo()));
  connect(m_pActionExportDVDAuthorXML, SIGNAL(activated()), qApp, SLOT(slotExportDVDAuthorXML()));
  connect(fileExitAction,   SIGNAL(triggered()), qApp, SLOT(quit()));
  
  connect(m_pActionUndoStack,       SIGNAL ( triggered()), this, SLOT ( slotUndoStack        ( ) ) );
  
  connect(helpContentsAction, SIGNAL(triggered()), this, SLOT(helpContents()));
  connect(helpGuideAction, SIGNAL(triggered()), this, SLOT(helpGuide()));
  connect(helpAboutAction, SIGNAL(triggered()), this, SLOT(helpAbout()));
  connect(toolsSetupAction, SIGNAL(triggered()), this, SLOT(slotSetup()));*/	//xxxx
  
  //QToolBar *toolbar = addToolBar("main toolbar");
  //toolbar->addAction(QIcon(newpix), "New File");
  //toolbar->addAction(QIcon(openpix), "Open File");
  //toolbar->addSeparator();
  
  //QAction *quit2 = toolbar->addAction(QIcon(quitpix), 
  //    "Quit Application");
  //connect(quit2, SIGNAL(triggered()), qApp, SLOT(quit()));

  //QTextEdit *edit = new QTextEdit(this);  

  //setCentralWidget(edit);

  statusBar()->showMessage("Ready");
  
  //m_pActionCreateDVD = new QAction(CFormMain);
  
  // And disabling the Undo/Redo/Cut/Copy/Paste - option because nothing is selected and undo is empty
  editUndoAction-> setEnabled ( false );
  editRedoAction-> setEnabled ( false );
}

QDVDAuthor::~QDVDAuthor ( )
{
  uint t;
/*  SaveIni ( );

  // This might not be needed since the destructor of the Widget takes care of this ...
  if ( Global::pSlideDefaults )
    delete Global::pSlideDefaults;
  Global::pSlideDefaults = NULL;

  if ( m_pSubtitlesGui )
    delete m_pSubtitlesGui;
  m_pSubtitlesGui = NULL;
*/	//oooo
  delete m_pSourceToolbar;
  m_pSourceToolbar = NULL;

  if ( m_pDialogExecute )
      delete m_pDialogExecute;
  m_pDialogExecute = NULL;

/*  if ( m_pDialogLibrary )
      delete m_pDialogLibrary;
  m_pDialogLibrary = NULL;*/	//oooo

  for (t=0;t<(uint)Global::listToolsPaths.count();t++)	//ox
      delete Global::listToolsPaths[t];
  Global::listToolsPaths.clear ( );

  for (t=0;t<(uint)m_listHistoryPix.count();t++)	//ox
      delete m_listHistoryPix[t];
  m_listHistoryPix.clear ( );

/*  m_iAutosave = 0;
  setAutosave ( );  // Note : must be done AFTER calling SaveIni () ...
*/	//oooo
  if ( m_pUndoStackDialog )
    delete m_pUndoStackDialog;

  if ( m_pVMGMenu )
       m_pVMGMenu->createStructure     ( NULL );
  for ( t=0; t<(uint)m_listDVDMenus.count  ( ); t++ )	//ox
    m_listDVDMenus[t]->createStructure ( NULL );

  // Okay let us start with deleting the SubMenus ...
  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )	//ox
    delete m_listDVDMenus[t];
  m_listDVDMenus.clear ( );

  // Here we delete the Main VMGM - menu ...
  delete m_pVMGMenu;
  m_pVMGMenu = NULL;

/*  // Clear the static data stored in DIalogFiles.
  Input::DialogFiles::clearMapOfChange ( );

  delete m_pActionToolbarNew;
  delete m_pActionToolbarWizard;
  delete m_pActionToolbarOpen;
  delete m_pActionToolbarSave;
  delete m_pActionToolbarSaveAs;
  delete m_pActionToolbarExit;
  delete m_pToolbar;
  delete m_pStructureToolbar;
*/
  // Just for good measure ...
  Global::listToolsPaths.clear ( );
}

void QDVDAuthor::initMe ( )
{
  
  setMinimumSize(1160, 720);  //xxx
  
  QPixmap newpix("new.png");
  QPixmap openpix("open.png");
  QPixmap quitpix("quit.png");

  QAction *fileNewAction = new QAction("&New Project", this);
  QAction *fileWizardAction = new QAction("Project &Wizard ...", this);
  QAction *fileOpenAction = new QAction("&Open Project ...", this);
  QAction *fileSaveAction = new QAction("&Save Project", this);
  QAction *fileSaveAsAction = new QAction("Save Project &As...", this);
  QAction *m_pActionConvert = new QAction("&Convert Project", this);
  QAction *m_pActionImportDvdauthor = new QAction("Import &dvdauthor/spumux ...", this);
  QAction *m_pActionImportKino = new QAction("Import &Kino (smil)", this);
  QAction *m_pActionExportDVDAuthorXML = new QAction("&Export dvdauthor (xml)", this);
  QAction *fileExitAction = new QAction("&Exit", this);

  QMenu *fileMenu = new QMenu(this);
  QMenu *editMenu = new QMenu(this);
  QMenu *dvdauthorMenu = new QMenu(this);
  QMenu *dvdmenuMenu = new QMenu(this);
  QMenu *toolsMenu = new QMenu(this);
  QMenu *helpMenu = new QMenu(this);
  
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(fileNewAction);
  fileMenu->addAction(fileWizardAction);
  fileMenu->addAction(fileOpenAction);
  fileMenu->addAction(fileSaveAction);
  fileMenu->addAction(fileSaveAsAction);
  fileMenu->addAction(m_pActionConvert);
  fileMenu->addSeparator();
  fileMenu->addAction(m_pActionImportDvdauthor);
  fileMenu->addAction(m_pActionImportKino);
  fileMenu->addAction(m_pActionExportDVDAuthorXML);
  fileMenu->addSeparator();
  fileMenu->addAction(fileExitAction);
  
  editUndoAction     = new QAction ( QIcon( ":/images/undo"        ), tr("&Undo"), this);
  editRedoAction     = new QAction ( QIcon( ":/images/redo"        ), tr("&Redo"), this);
  m_pActionUndoStack = new QAction ( QIcon ( ":/images/text_left.png"     ), tr("Undostack" ), this );
  
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(editUndoAction);
  editMenu->addAction(editRedoAction);
  editMenu->addAction(m_pActionUndoStack);
  
  dvdauthorMenu = menuBar()->addMenu("DVD&Author");
  
  QAction *m_pActionAddSubMenu = new QAction("Add &Menu", this);
  
  dvdauthorMenu->addAction(m_pActionAddSubMenu);
  
  dvdmenuMenu = menuBar()->addMenu("DVD&Menu");
  
  QAction *m_pActionRenameMenu = new QAction("&Rename Menu", this);
  QAction *m_pActionCloneMenu = new QAction("&Clone Menu", this);
  QAction *m_pActionDeleteMenu = new QAction("&Delete Menu", this);
  QAction *m_pActionAddFrame = new QAction("Add &Frame", this);
  QAction *m_pActionAddText = new QAction("Add &Text", this);
  QAction *m_pActionAddImage = new QAction("Add &Image", this);
  QAction *m_pActionAddMovie = new QAction("Add &Movie", this);
  QAction *m_pActionAddBackground = new QAction("Add &Background", this);
  
  dvdmenuMenu->addAction(m_pActionRenameMenu);
  dvdmenuMenu->addAction(m_pActionCloneMenu);
  dvdmenuMenu->addAction(m_pActionDeleteMenu);
  dvdmenuMenu->addSeparator();
  dvdmenuMenu->addAction(m_pActionAddFrame);
  dvdmenuMenu->addAction(m_pActionAddText);
  dvdmenuMenu->addAction(m_pActionAddImage);
  dvdmenuMenu->addAction(m_pActionAddMovie);
  dvdmenuMenu->addSeparator();
  dvdmenuMenu->addAction(m_pActionAddBackground);
  
  toolsMenu = menuBar()->addMenu("&Tools");
  
  QAction *toolsSetupAction = new QAction("&Setup ...", this);
  QAction *m_pActionFeedback = new QAction("&Feedback", this);
  
  toolsMenu->addAction(toolsSetupAction);
  toolsMenu->addAction(m_pActionFeedback);
  
  helpContentsAction = new QAction("&Contents...", this);
  helpGuideAction = new QAction("&Quick-Start Guide", this);
  helpAboutAction = new QAction("&About", this);
  
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(helpContentsAction);
  helpMenu->addSeparator();
  helpMenu->addAction(helpGuideAction);
  helpMenu->addAction(helpAboutAction);

  //fileNewAction->setObjectName(QString::fromUtf8("fileNewAction"));
  //fileNewAction->setName("fileNewAction");

  //fileNewAction->setIconText(QApplication::translate("CFormMain", "&New Project", 0, QApplication::UnicodeUTF8));

  connect(fileNewAction,    SIGNAL(triggered()), this, SLOT(fileNew()));
  connect(fileWizardAction, SIGNAL(activated()), qApp, SLOT(fileWizard()));
  connect(fileOpenAction,   SIGNAL(activated()), qApp, SLOT(fileOpen()));
  connect(fileSaveAction,   SIGNAL(activated()), this, SLOT(fileSave()));
  connect(fileSaveAsAction, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
  connect(editUndoAction,   SIGNAL(triggered()), this, SLOT(editUndo()));
  connect(editRedoAction,   SIGNAL(triggered()), this, SLOT(editRedo()));
  connect(m_pActionExportDVDAuthorXML, SIGNAL(activated()), qApp, SLOT(slotExportDVDAuthorXML()));
  connect(fileExitAction,   SIGNAL(triggered()), qApp, SLOT(quit()));
  
  connect(m_pActionUndoStack,       SIGNAL ( triggered()), this, SLOT ( slotUndoStack        ( ) ) );
  
  connect (m_pActionAddSubMenu,      SIGNAL ( triggered()), this, SLOT ( slotAddSubMenu       ( ) ) );
  
  connect( m_pActionRenameMenu,	   SIGNAL( triggered() ), this, SLOT( slotRenameCurrentSubMenu   ( ) ) );
  connect( m_pActionCloneMenu,	   SIGNAL( triggered() ), this, SLOT( slotCloneCurrentSubMenu    ( ) ) );
  connect( m_pActionDeleteMenu,	   SIGNAL( triggered() ), this, SLOT( slotRemoveCurrentSubMenu   ( ) ) );
  
  connect( m_pActionAddFrame,      SIGNAL( triggered() ), this, SLOT( slotAddFrameObject  ( ) ) );
  connect( m_pActionAddText,       SIGNAL( triggered() ), this, SLOT( slotAddTextObject   ( ) ) );
  connect( m_pActionAddImage,      SIGNAL( triggered() ), this, SLOT( slotAddImageObject  ( ) ) );
  connect( m_pActionAddMovie,      SIGNAL( triggered() ), this, SLOT( slotAddMovieObject  ( ) ) );
  //connect( m_pActionAddBackground, SIGNAL( triggered() ), this, SLOT( slotAddBackground   ( ) ) );    //ooo
  connect( m_pActionAddBackground, SIGNAL( triggered() ), this, SIGNAL( signalAddBackground   ( ) ) );    //xxx
  
  connect(helpContentsAction, SIGNAL(triggered()), this, SLOT(helpContents()));
  connect(helpGuideAction, SIGNAL(triggered()), this, SLOT(helpGuide()));
  connect(helpAboutAction, SIGNAL(triggered()), this, SLOT(helpAbout()));
  connect(toolsSetupAction, SIGNAL(triggered()), this, SLOT(slotSetup()));
  
  //m_pToolbar             = new QToolBar   ( this );
  m_pToolbar             = addToolBar("main toolbar");
  Global::pSlideDefaults = new SlideDefaults;
  
  m_pActionToolbarNew    = new QAction ( QIcon ( ":/images/new.png"     ), tr("&New Project")   , this );
  m_pActionToolbarWizard = new QAction ( QIcon ( ":/images/wizard.png"  ), tr("&Project Wizard"), this );
  m_pActionToolbarOpen   = new QAction ( QIcon ( ":/images/open.png"    ), tr("&Open Project")  , this );
  m_pActionToolbarSave   = new QAction ( QIcon ( ":/images/save.png"    ), tr("&Save")          , this );
  m_pActionToolbarSaveAs = new QAction ( QIcon ( ":/images/save_as.png" ), tr("&Save As")       , this );
  m_pActionToolbarExit   = new QAction ( QIcon ( ":/images/exit.png"    ), tr("&Exit")          , this );
  
  m_pActionCreateDVD     = new QAction ( QIcon ( ":/images/to_dvd.png"        ), tr("&Create DVD"    )   , this );
  m_pActionAddBackgrnd   = new QAction ( QIcon ( ":/images/add_image.png"     ), tr("Add &Background")   , this );
  m_pActionAddVideo      = new QAction ( QIcon ( ":/images/add_image.png"     ), tr("Add &Video")        , this );
  m_pActionAddAudio      = new QAction ( QIcon ( ":/images/add_audio.png"     ), tr("Add &Audio"     )   , this );
  m_pActionAddSlide      = new QAction ( QIcon ( ":/images/add_slideshow.png" ), tr("Add &Slide"     )   , this );
  m_pActionAddMenuSlide  = new QAction ( QIcon ( ":/images/add_menuslide.png" ), tr("Add &MenuSlide" )   , this );
  
  m_pToolbar->setIconSize(QSize(48, 48));   //xxx

  m_pToolbar->addAction( m_pActionToolbarNew );
  m_pToolbar->addAction( m_pActionToolbarWizard );
  m_pToolbar->addAction( m_pActionToolbarOpen );
  m_pToolbar->addAction( m_pActionToolbarSave );
  m_pToolbar->addAction( m_pActionToolbarSaveAs );
  m_pToolbar->addAction( m_pActionToolbarExit );
  m_pToolbar->addSeparator      ( );
  m_pToolbar->addAction( m_pActionCreateDVD );
  m_pToolbar->addAction( m_pActionAddBackgrnd );
  m_pToolbar->addAction( m_pActionAddVideo );
  m_pToolbar->addAction( m_pActionAddAudio );
  m_pToolbar->addAction( m_pActionAddSlide );
  m_pToolbar->addSeparator      ( );
  m_pToolbar->addAction( m_pActionAddMenuSlide );
  
  connect( m_pActionToolbarNew,    SIGNAL( triggered ( ) ), this, SLOT( fileNew ( ) ) );
  connect( m_pActionToolbarOpen,   SIGNAL( triggered ( ) ), this, SLOT( fileOpen ( ) ) );
  connect( m_pActionToolbarSave,   SIGNAL( triggered ( ) ), this, SLOT( fileSave ( ) ) );
  connect( m_pActionToolbarSaveAs, SIGNAL( triggered ( ) ), this, SLOT( fileSaveAs ( ) ) );
  connect( m_pActionToolbarExit,   SIGNAL( triggered ( ) ), qApp, SLOT( quit ( ) ) );
  
  //connect (m_pTabWidgetMain,         SIGNAL ( currentChanged(QWidget *)), this, SLOT(slotTabChanged(QWidget *)));	//ooo
  //connect (m_pTabWidgetMain,         SIGNAL ( currentChanged(int)), this, SLOT(slotTabChanged(int)));			//xxx
  connect (m_pActionCreateDVD,       SIGNAL ( triggered ( ) ), this, SLOT ( slotCreateDVD        ( ) ) );
  connect (m_pActionFeedback,        SIGNAL ( triggered()), this, SLOT ( slotFeedback         ( ) ) );
  connect( m_pActionAddSlide,	   SIGNAL( triggered() ), this, SLOT( slotEditAddSlideshow  ( ) ) );
  connect( m_pActionAddMenuSlide,  SIGNAL( triggered() ), this, SLOT( slotActivatePlugin    ( ) ) );
  
  connect( m_pActionAddAudio,	 SIGNAL( triggered ( ) ), this, SIGNAL( signalAddSound        ( ) ) );
  connect( m_pActionAddBackgrnd, SIGNAL( triggered ( ) ), this, SIGNAL( signalAddBackground ( ) ) );
  connect( m_pActionAddVideo,    SIGNAL( triggered ( ) ), this, SIGNAL( signalAddVideo ( ) ) );
  
  LoadIni       ( );		//oooo
  
  // Next we start the timer to delete all temp file older Than ...
  if ( Global::iDeleteTempOlderThan > -1 )
    QTimer::singleShot (30000, this, SLOT(slotDeleteTempOlderThan ()));
  
  // Here we create the Structure, and the SourceFile ToolBars.
  m_pSourceToolbar    = new SourceToolBar   ( "SourceToolBar",    this );
  m_pStructureToolbar = new StructureToolBar( m_pSourceToolbar );		//oooo
  //m_pColorToolbar     = new ColorToolBar    ( "ColorToolBar",     this );	//oooo
  //m_pColorToolbar     = new ColorToolBar    ( "ColorToolBar",     this );	//xxx
  
  //QTimer::singleShot ( 200, this, SLOT ( polish ( ) ) );	//xxxx
  
  widget = new QWidget(this);
  
  gridLayout = new QGridLayout(widget); 
  hboxLayout = new QHBoxLayout();
  hboxLayout->setSpacing(6);
  hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
  m_pLabelSize = new QLabel(widget);
  m_pLabelSize->setObjectName(QString::fromUtf8("m_pLabelSize"));
  m_pLabelSize->setMinimumSize(QSize(100, 0));
  m_pLabelSize->setWordWrap(false);
  
  hboxLayout->addWidget(m_pLabelSize);

  m_pProgressSize = new QProgressBar(widget);
  m_pProgressSize->setObjectName(QString::fromUtf8("m_pProgressSize"));
  m_pProgressSize->setMaximumSize(QSize(32767, 20));

  hboxLayout->addWidget(m_pProgressSize);
  
  gridLayout->addLayout(hboxLayout, 2, 0, 1, 1);
  
  m_pTabWidgetMain = new QTabWidget(widget);

  /*tab0 = new QWidget();
  tab0->setObjectName(QString::fromUtf8("tab0"));
  
  //tab0->setLineWidth              ( 0 );
  //tab0->setMargin                 ( 0 );
  //tab0->setScaledContents         ( true );
  tab0->setAutoFillBackground     ( true );
  
  QPixmap backgroundPixmap(":/images/checkered.png");
  QPalette palette;
  palette.setBrush(QPalette::Window, QBrush(backgroundPixmap));	
  tab0->setPalette(palette);
  
  m_pTabWidgetMain->addTab(tab0, QString());*/
  
  VMGMenu *pDVDMenu = new VMGMenu(this);
  /*pDVDMenu->setObjectName(QString::fromUtf8("tab0"));
  m_pTabWidgetMain->addTab(pDVDMenu, QString());*/  //xxxx
  
  TabPage = new QWidget();
  TabPage->setObjectName(QString::fromUtf8("TabPage"));
  m_pTabWidgetMain->addTab(TabPage, QString());
  
  tab = new QWidget();
  tab->setObjectName(QString::fromUtf8("tab"));
  m_pTabWidgetMain->addTab(tab, QString());
  tab1 = new QWidget();
  tab1->setObjectName(QString::fromUtf8("tab1"));
  gridLayout1 = new QGridLayout(tab1);
  gridLayout1->setSpacing(6);
  gridLayout1->setContentsMargins(11, 11, 11, 11);
  gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
  m_pTextEditXml = new QTextEdit(tab1);
  m_pTextEditXml->setObjectName(QString::fromUtf8("m_pTextEditXml"));
  //m_pTextEditXml->setWordWrap(QTextEdit::WidgetWidth);

  gridLayout1->addWidget(m_pTextEditXml, 1, 0, 1, 2);

  textLabel1_2 = new QLabel(tab1);
  textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
  textLabel1_2->setWordWrap(false);

  gridLayout1->addWidget(textLabel1_2, 0, 0, 1, 1);

  m_pComboSource = new QComboBox(tab1);
  m_pComboSource->setObjectName(QString::fromUtf8("m_pComboSource"));

  gridLayout1->addWidget(m_pComboSource, 0, 1, 1, 1);

  
  m_pTabWidgetMain->addTab(tab1, QString());
  
  gridLayout->addWidget(m_pTabWidgetMain, 0, 0, 1, 1);
  
  setCentralWidget(widget);
  
  //m_pLabelSize->setText(QApplication::translate("CFormMain", "0 MB", 0, QApplication::UnicodeUTF8));	//ooo
  m_pLabelSize->setText("0 MB");									//xxx
  //m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(tab0), "Main Menu VMGM");
  //m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(pDVDMenu), "Main Menu VMGM");	//xxx
  //m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(pDVDMenu), QString(VMGM_LABEL));	//xxx
  m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(TabPage), QString("DVD Layout"));
  m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(tab), QString("Subtitles"));
  textLabel1_2->setText("<p align=\"center\">See XML for : </p>");
  m_pTabWidgetMain->setTabText(m_pTabWidgetMain->indexOf(tab1), QString("XML out"));
  
  connect (m_pTabWidgetMain,         SIGNAL ( currentChanged(int)), this, SLOT(slotTabChanged(int)));			//xxx
  connect (m_pComboSource,           SIGNAL ( highlighted(const QString &)), this, SLOT(slotCreateXml(const QString &)));
  
  slotUpdateStructure ( );					//xxx	void DVDMenu::slotUpdateStructure ( )
  m_pVMGMenu = pDVDMenu;					//xxx	void QDVDAuthor::addDVDMenu ( DVDMenu *pDVDMenu, bool bVMGM )
  //m_pVMGMenu->setTabLabel( QString(VMGM_LABEL) );		//xxx
  QTimer::singleShot ( 200, this, SLOT ( polish ( ) ) );	//xxxx
}

void QDVDAuthor::polish ()
{
  //Utils   theUtils;	//ooo
  QString qsLang;
  int     t, iID;

  //CFormMain::polish ( );			//ooo
  //Q3MainWindow::ensurePolished ( );		//xxx
  //initDVDLayoutTab  ( );			//ooo
  Global::pApp->initDVDLayoutTab ( );		//xxx
  //initSubtitlesTab  ( );			//ooo
  Global::pApp->initSubtitlesTab ( );		//xxx
  /*if ( Global::qsLanguage == QString ("en") )
       return;
  // Lastly we want to have the check in the right place ...
  qsLang = theUtils.iso639 ( Global::qsLanguage );
  for ( t=0;t<(int)m_pLanguagePopup->count ();t++) {
    iID = m_pLanguagePopup->idAt ( t );
    if ( qsLang == m_pLanguagePopup->text (  iID  ) )
      m_pLanguagePopup->setItemChecked ( iID,  true );
    else
      m_pLanguagePopup->setItemChecked ( iID, false );
  }*/
}

//void QDVDAuthor::customEvent ( QCustomEvent *pEvent ) //ooo
void QDVDAuthor::customEvent ( QEvent *pEvent ) //xxx-!
//bool QDVDAuthor::event ( QEvent *pEvent ) //xxx-!
//bool QDVDAuthor::event ( QCustomEvent *pEvent ) //xxx
{
  //QEvent::Type QCustomEvent::customEventType = QEvent::None;    //xxx
    
  if ( pEvent->type ( ) == EVENT_RENDER_PROGRESS )  { //ooo
  //if ( pEvent->type ( ) == QCustomEvent::myType_Event_Render_Progress )  {   
    //Render::Client *pClient = (Render::Client *)pEvent->data ( ); //ooo
    Render::Client *pClient = (Render::Client *)pEvent; //xxx
    SourceFileEntry *pEntry =  pClient->sourceFileEntry      ( ); //ooo
    //m_pSourceToolbar->setRenderProgress ( pEntry, pClient->progress ( ) );    //ooo
    m_pSourceToolbar->setRenderProgress ( pEntry, 50.0 );    //xxx
  }
  else if ( pEvent->type ( ) == EVENT_RENDER_EXCEPTION )  {   //ooo
  //else if ( pEvent->type ( ) == QCustomEvent::myType_Event_Render_Exception )  {  //xxx
//    Render::Client *pClient = (Render::Client *)pEvent->data ( );
  }
  else if ( pEvent->type ( ) == EVENT_RENDER_EXIT )  {    //ooo
  //else if ( pEvent->type ( ) == QCustomEvent::myType_Event_Render_Exit )  {   //xxx   
    /*//Render::Client *pClient = (Render::Client *)pEvent->data ( ); //ooo
    Render::Client *pClient = (Render::Client *)pEvent; //xxx
    if ( pClient->sourceFileEntry ( ) )
      eventEntry ( pClient );
    else if ( pClient->buttonTransition ( ) )
      eventTrans ( pClient );*/ //ooo
  }
  //return true;  //xxx
//printf ( "QDVDAuthor::customEvent  : <%X> Event <%d> Client<%p>\n", (unsigned)pthread_self ( ), pEvent->type ( ), pEvent->data ( ) );
} //ooo

void QDVDAuthor::eventEntry ( Render::Client *pClient )
{
  SourceFileEntry *pEntry =  pClient->sourceFileEntry ( );
  Render::Manager::unregisterEntry ( pEntry );
  if ( ! m_pSourceToolbar->sourceFileEntryExists ( pEntry ) )  {
    // What to do and how ?? Search all SourceFileInfo's ???
  }
  else  {
    pEntry->bIsSlideshow    = false;
    SourceFileInfo *pInfo   = pEntry->listFileInfos[0];
    if ( pInfo  &&  pInfo->pSlideshow )
         m_pSourceToolbar->slideshowDone ( pEntry );
    m_pSourceToolbar->setRenderProgress  ( pEntry, -1.0 );

    Cache::Thumbs::Entry *pCache = Global::pThumbsCache->find ( pInfo->qsFileName );
    if ( ! pCache )
      Global::pThumbsCache->append ( pInfo->qsFileName, this );
  }
  // Lasty we check if we have all our slideshows in a row.
  if ( m_bCreateDVDAfterSlideshows )  {
    int t;
    for ( t=0 ;t<sourceFileCount ( ); t++ )  {
      SourceFileEntry *pEntry = sourceFileEntry ( t );
      if ( pEntry->bIsSlideshow )
           return;
    }
    // At this point we are engaged to create the DVD and
    // all Slideshows seem to exist.
    engageCreateDVD ( false );
    slotCreateDVD   ( );
  }
} //ooo

void QDVDAuthor::eventTrans ( Render::Client *pClient )
{
  ButtonTransition *pTrans = pClient->buttonTransition ( );
  if ( ! buttonTransitionExists ( pTrans ) )  {
    // What to do and how ?? Search all SourceFileInfo's ???
  }
  else  {
    Render::Manager::unregisterTrans ( pTrans );
    pTrans->bFinishedRendering = true;
    slotAutosave ( );
//printf ( "%s::%d > Finished rendering ButtonTransition<%s> <%s>\n", __FILE__, __LINE__, pTrans->qsName.ascii ( ), pTrans->qsTransitionVideoFileName.ascii () );
  }
}

bool QDVDAuthor::buttonTransitionExists ( ButtonTransition *pTrans )
{
  if ( ! pTrans )
    return false;

  // Based on the pointer itself ...
  QList<ButtonObject *> buttonList;					//oxx
  QList<DVDMenu      *> menuList = getSubMenus ( );			//oxx
  QList<DVDMenu       *>::iterator itMenu = menuList.begin ( );		//oxx
  QList<ButtonObject *>::iterator it;					//oxx

  DVDMenu *pMenu = getVMGMenu ( );
  do  {
    buttonList = pMenu->getButtons( );
    it = buttonList.begin ( );
    while (  it != buttonList.end ( ) )  {
      if ( (*it++)->getTransition ( ) == pTrans )
        return true;
    }
    pMenu = *itMenu++;
  } while (  itMenu != menuList.end ( ) );
  return false;
}

void QDVDAuthor::showUndoStack ( )
{
  if ( ! m_pUndoStackDialog )
         m_pUndoStackDialog = new DialogUndoStack ( getUndoBuffer ( ) );

  // This function will call the Undo - stack GUI of the current menu
  DVDMenu *pMenu = getCurrentSubMenu ( );
  if   ( ! pMenu )  {
    m_pUndoStackDialog->setUndoBuffer ( NULL );
    return;
  }

  m_pUndoStackDialog->setUndoBuffer ( pMenu->getUndoBuffer ( ) );	//oooo
  m_pUndoStackDialog->show          ( );
  m_pUndoStackDialog->raise         ( );	//oooo
}

void QDVDAuthor::updateUndoStack ( )
{
  if ( ! m_pUndoStackDialog )
    return;

  if ( ! m_pUndoStackDialog->isVisible ( ) )
    return;

  // This function will call the Undo - stack GUI of the current menu
  DVDMenu *pMenu = getCurrentSubMenu ( );
  if   ( ! pMenu )  {
    m_pUndoStackDialog->setUndoBuffer ( NULL );
    return;
  }

  m_pUndoStackDialog->setUndoBuffer ( pMenu->getUndoBuffer ( ) );
}

/*bool QDVDAuthor::tabVisible ( enTabs tab )    //oooo
{
  if ( tab == TabDVDLayout )
    return m_pTabsPopup->isItemChecked ( m_pTabsPopup->idAt ( 0 ) );
  else if ( tab == TabSubtitles )
    return m_pTabsPopup->isItemChecked ( m_pTabsPopup->idAt ( 1 ) );
  else if ( tab == TabDVDAuthorXml )
    return m_pTabsPopup->isItemChecked ( m_pTabsPopup->idAt ( 2 ) );
  return true;	//oooo
}*/

QWidget *QDVDAuthor::getTabWidget ( enTabs tab, int iSubMenu )
{  
  int iNrOfTabs = m_pTabWidgetMain->count ( );

  // The Main Menu is always on tab 1.
  if ( tab == TabMainMenu )
    //return m_pTabWidgetMain->page ( 0 );	//ooo
    return m_pTabWidgetMain->widget ( 0 );	//xxx
  else if ( ( tab == TabSubMenu ) && ( iSubMenu < iNrOfTabs ) )
    //return m_pTabWidgetMain->page ( iSubMenu );	//ooo
    return m_pTabWidgetMain->widget ( iSubMenu );
  else if ( tab == TabDVDLayout )  { 
    if ( m_mainTabBuffer[0] )
      return m_mainTabBuffer[0];
 
    //return m_pTabWidgetMain->page ( iNrOfTabs - 3 );	//ooo
    return m_pTabWidgetMain->widget ( iNrOfTabs - 3 );	//xxx
  }
  else if ( tab == TabSubtitles )  {
    if ( m_mainTabBuffer[1] )
      return m_mainTabBuffer[1];
    //return m_pTabWidgetMain->page ( iNrOfTabs - 2 );	//ooo
    return m_pTabWidgetMain->widget ( iNrOfTabs - 2 );	//xxx
  }
  else if ( tab == TabDVDAuthorXml )  {
    if ( m_mainTabBuffer[2] )
      return m_mainTabBuffer[2];
    //return m_pTabWidgetMain->page ( iNrOfTabs - 1 );	//ooo
    return m_pTabWidgetMain->widget ( iNrOfTabs - 1 );	//xxx
  }
  return NULL;
}

void QDVDAuthor::initSubtitlesTab ( )
{
  QWidget *pWidget = getTabWidget ( TabSubtitles );
  if ( ! pWidget )
    return;
  //Q3GridLayout *pLayout = new Q3GridLayout  ( pWidget );  //ooo
  QGridLayout *pLayout = new QGridLayout  ( pWidget );    //xxx
  /*m_pSubtitlesGui      = new SubtitlesGui ( this, pWidget );
  pLayout->addWidget ( m_pSubtitlesGui, 0, 0 );
  m_pTabWidgetMain->setTabLabel (pWidget, tr("SubPictures (Subtitles)"));
  m_pTabWidgetMain->setCurrentPage ( 0 );*/
}

void QDVDAuthor::initDVDLayoutTab ( )
{
  QWidget *pWidget = getTabWidget ( TabDVDLayout );
  if ( ! pWidget )
    return;

  QGridLayout *pLayout = new QGridLayout  ( pWidget );

  m_pDVDLayoutGui      = new DVDLayout::Gui ( pWidget );

  pLayout->addWidget ( m_pDVDLayoutGui, 0, 0 );

  //m_pTabWidgetMain->setTabLabel    ( pWidget, tr ( "DVD Layout" ) );  //ooo
  m_pTabWidgetMain->setTabText    ( m_pTabWidgetMain->indexOf(pWidget), tr ( "DVD Layout" ) );     //xxx

  //m_pTabWidgetMain->setCurrentPage ( 0 ); //ooo
  m_pTabWidgetMain->setCurrentIndex ( 0 );  //xxx
}

QString QDVDAuthor::getUniqueMenuTitle (QString qsInput, int iNumber, DVDMenu *pDVDMenu)
{
	uint t;
	QString qsTitle, qsTemp;

	if (iNumber == 0)
		qsTitle = qsInput;
	else
		qsTitle = QString ("%1_%2").arg(iNumber).arg(qsInput);

	for (t=0;t<(uint)m_listDVDMenus.count();t++)	{	//ox
		if (m_listDVDMenus[t] == pDVDMenu)
			continue;
		if (qsTitle == m_listDVDMenus[t]->getInterface()->qsMenuName)	{
			return getUniqueMenuTitle (qsInput, iNumber + 1, pDVDMenu);
		}
	}
	return qsTitle;
}

void QDVDAuthor::slotActivatePlugin ( )
{
  slotActivatePlugin ( 0 );
}

void QDVDAuthor::slotActivatePlugin ( int )
{  
  // Test for now hard linked against QDVDAuthor.
  // Later to be linked at runtime through usage of dlopen ()
  //if ( ! m_pPluginInterface ) {   //ooo
    
    m_pPluginInterface = new Plugin::MenuSlideInterface;   //oooo
    //m_pPluginInterface = new Plugin::MenuSlideInterface ( this );  //xxx
    // register callback through slot mechanism ...
    connect ( m_pPluginInterface, SIGNAL ( signalNewMenuResponse ( int, QString, QString  ) ), this, SLOT ( slotPluginCallback ( int, QString, QString ) ) ); //ooo
  //}   //ooo

  m_pPluginInterface->execute ( );    //ooo
  //(Plugin::MenuSlideInterface*) m_pPluginInterface->execute  ( ); //xxx
}

void QDVDAuthor::slotPluginCallback ( int iType, QString qsMenuName, QString qsXMLDVDMenu )
{  
  //printf ( "Create Menu [%d]<%s>\n", iType, qsXMLDVDMenu.ascii ( ) );
  QDomDocument domDoc;
  QDomElement  menuElement;
  QString      qsError;
  DVDMenu     *pDVDMenu;
  int iRow, iCol;
  Plugin::SourceGroup::type enType = (Plugin::SourceGroup::type)iType;
  enType = enType;

  QCursor myCursor ( Qt::WaitCursor );
  setCursor ( myCursor );

  iRow = iCol = 0;
  domDoc.setContent ( qsXMLDVDMenu, true, &qsError, &iRow, &iCol );
  //printf ("\n\n%s\n\n", (const char *)m_qsXMLDVDMenu);
  menuElement = domDoc.documentElement();

  pDVDMenu = new DVDMenu ( this );
  qsMenuName = getUniqueMenuTitle ( qsMenuName, 0 );
  pDVDMenu->readProjectFile ( menuElement, 1.0f );
  pDVDMenu->getInterface    ( )->qsMenuName = qsMenuName;
  pDVDMenu->setTabLabel     ( qsMenuName );
  pDVDMenu->updateDVDMenu   ( );
  addDVDMenu ( pDVDMenu );

  slotAutosave        ( );
  slotUpdateStructure ( );

  //myCursor = QCursor ( QCursor::ArrowCursor );	//ooo
  myCursor = QCursor ( Qt::ArrowCursor );		//xxx
  setCursor ( myCursor );
}

void QDVDAuthor::addDVDMenu ( DVDMenu *pDVDMenu, bool bVMGM )
{
  QString qsTitle;

  // This check ensures we add only unique Tab names.
  qsTitle = getUniqueMenuTitle (pDVDMenu->getInterface()->qsMenuName, 0, pDVDMenu);
  if (qsTitle != pDVDMenu->getInterface()->qsMenuName)	{
    pDVDMenu->getInterface()->qsMenuName = qsTitle;
    //		pDVDMenu->setTabLabel (qsTitle);
  }
  pDVDMenu->setTabLabel (qsTitle);

  if ( bVMGM )
    m_pVMGMenu = pDVDMenu;
  else
    m_listDVDMenus.append ( pDVDMenu );

  //m_pColorToolbar->setDVDMenu   ( pDVDMenu );     //ooo
  pDVDMenu->getMenuPreview()->setImageButton(m_dragNDropContainer.bImageButton);
  connect (this, SIGNAL(signalVisibleRegion(bool)), pDVDMenu, SLOT(slotVisibleRegion (bool)));
  pDVDMenu->slotVisibleRegion(m_bVisibleRegion);	//oooo
}

void QDVDAuthor::addMovieList (QStringList &listMovies, QString *pTitle)
{
	m_pSourceToolbar->addMovieList (listMovies, pTitle);
}

int QDVDAuthor::sourceFileCount()
{
	return (int)m_pSourceToolbar->sourceFileCount();
}

SourceFileEntry *QDVDAuthor::sourceFileEntry(uint iEntryNumber)
{
	return m_pSourceToolbar->sourceFileEntry(iEntryNumber);
}

SourceFileEntry *QDVDAuthor::getSourceEntryByDisplayName(QString &qsDisplayName)
{
	int t;
	for (t=0;t<sourceFileCount();t++)	{
		if (sourceFileEntry(t)->qsDisplayName == qsDisplayName)
			return sourceFileEntry(t);
	}
	return NULL;
}

void QDVDAuthor::editSourceFileInfo ( SourceFileInfo *pInfo )
{
  m_pSourceToolbar->editSourceFileInfo ( pInfo );
}

void QDVDAuthor::updateSourceFileInfo ( SourceFileInfo *pInfo )
{
  m_pSourceToolbar->updateListViewItem ( pInfo );
}

void QDVDAuthor::updateDVDLayout ( SourceFileEntry *pEntry )
{
  if ( m_pDVDLayoutGui )
       m_pDVDLayoutGui->updateEntry ( pEntry );	//oooo
}

void QDVDAuthor::addSound (QStringList &listSoundFiles)
{
	if ( ! checkForNewProject ( ) )
		return;
	m_pSourceToolbar->addSound ( listSoundFiles );
}

void QDVDAuthor::setColor ( uint iWhichColor, Rgba theColor )
{
  //m_pColorToolbar->setColor( iWhichColor, theColor ); //oooo
}

void QDVDAuthor::removeSourceFileEntry (SourceFileEntry *pSourceFileEntry)
{
	// This function is called from an UndoObject (hint: add_sound)
	m_pSourceToolbar->removeEntry(pSourceFileEntry);
}

void QDVDAuthor::appendSourceFileEntry (SourceFileEntry *pSourceFileEntry)
{
  // This function is called from an UndoObject (hint: add_sound) and when importing a dvdauthor.xml file
  m_pSourceToolbar->appendSourceFileEntry ( pSourceFileEntry, true );
}

void QDVDAuthor::removedSourceEntry (SourceFileEntry *pEntry)
{
  // This function is called after a SourceFileEntry has been deleted from the
  // SourceToolBar - class.
  // We should remove all subsequent dependencies (buttons connected with movie files, or background sound files ...)
  uint t;

  if  ( m_pVMGMenu )
        m_pVMGMenu->removeSourceEntry ( pEntry->qsDisplayName );
  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )	//ox
    m_listDVDMenus[t]->removeSourceEntry ( pEntry->qsDisplayName );

  if ( m_pDVDLayoutGui )
       m_pDVDLayoutGui->refresh ( );
}

bool QDVDAuthor::isSourceEntryUsed(QString &qsInquireEntry)
{
	uint t;
	// This function is called when the user wants to delete a SourceFileEntry
	// it returns true if the entry is in use.
	if ( m_pVMGMenu && m_pVMGMenu->isSourceEntryUsed ( qsInquireEntry ) )
		return true;
	// next we step through all sub-menus ...
	for (t=0;t<(uint)m_listDVDMenus.count();t++)	{	//ox
		if (m_listDVDMenus[t]->isSourceEntryUsed (qsInquireEntry))
			return true;
	}
	return false;
}

void QDVDAuthor::replaceSourceDisplayName (QString &qsOriginalDisplayName, QString &qsNewDisplayName)
{
	// This function is called when the user changed the DisplayName of a SourceFileEntry
	// DialogMovie::accept()->SourceToolBar::slotEditMovie->QDVDAuthor::replaceSourceDisplayName()
	uint t;
	if ( m_pVMGMenu )	
	     m_pVMGMenu->replaceSourceDisplayName (qsOriginalDisplayName, qsNewDisplayName);
	for (t=0;t<(uint)m_listDVDMenus.count();t++)		//ox
		m_listDVDMenus[t]->replaceSourceDisplayName (qsOriginalDisplayName, qsNewDisplayName);

	slotUpdateStructure ( );
}

QList<DVDMenu *> &QDVDAuthor::getSubMenus( )
{
	return m_listDVDMenus;
}

MediaInfo *QDVDAuthor::getMediaInfo ()
{
  if (!m_pSourceToolbar)
    return NULL;
  return m_pSourceToolbar->getMediaInfo ();
}

DragNDropContainer *QDVDAuthor::getDragNDropContainer()
{
	return &m_dragNDropContainer;
}

void QDVDAuthor::slotUpdateDVDLayout ( )
{
  if ( m_pDVDLayoutGui ) {
       m_pDVDLayoutGui->refresh ( );	//oooo
  }
}

void QDVDAuthor::slotUpdateStructure ( )
{
  int t;
  StructureItem *pMainItem;

  // Create or return the first item
  pMainItem = m_pStructureToolbar->clean ( true );
  
  if ( m_pVMGMenu ) {
       m_pVMGMenu->createStructure     ( pMainItem );
  }

  for ( t=0; t<(int)m_listDVDMenus.count  ( ); t++ ) {
    m_listDVDMenus[t]->createStructure ( pMainItem );
  }
  
  slotUpdateMenuBar   ( );	//oooo
  slotUpdateDVDLayout ( );
}

void QDVDAuthor::slotUndoStack ()
{
  showUndoStack ( );

//  // This function will call the Undo - stack GUI of the current menu
//  DVDMenu *pMenu = getCurrentSubMenu ( );
//  if ( ! pMenu )
//    return;
//  pMenu->showUndoStack ( );
}

void QDVDAuthor::slotRenameCurrentSubMenu ( )
{
  // Here we remove the current SubMenu ...
  QString qsSubMenuName, qsNewName;
  bool bOkay;
  DVDMenu *pMenu = getCurrentSubMenu ( );
  if ( ! pMenu )
    return;

  if ( pMenu == m_pVMGMenu )  {
    MessageBox::information (NULL, tr ("Can not change VMGM name"),
			     tr ("I can not change the name of the VMGM\n"), QMessageBox::Ok);
    return;
  }
  if ( pMenu )
    qsNewName   = pMenu->getInterface()->qsMenuName;
  //qsSubMenuName = m_pTabWidgetMain->tabLabel ( m_pTabWidgetMain->currentPage ( ) );   //ooo
  qsSubMenuName = m_pTabWidgetMain->tabText ( m_pTabWidgetMain->currentIndex ( ) );     //xxx
  
  //to remove shortcut "&"  //xxx
  if ( qsSubMenuName.indexOf("&") != -1 ) //xxx
    qsSubMenuName.remove(qsSubMenuName.indexOf("&"),1);    //xxx
  
  //qsNewName     = QInputDialog::getText ( tr ( "Sub Menu Name" ), tr ( "Please give the new SubMenu Name" ), QLineEdit::Normal, qsNewName, &bOkay, this );    //ooo
  qsNewName     = QInputDialog::getText ( this, tr ( "Sub Menu Name" ), tr ( "Please give the new SubMenu Name" ), QLineEdit::Normal, qsNewName, &bOkay );      //xxx
  if ( ( ! bOkay ) || ( qsNewName.isEmpty ( ) ) )
    return;

  if ( pMenu && ( qsSubMenuName != qsNewName ) )
    replaceMenuName ( pMenu, qsNewName );
}

void QDVDAuthor::replaceMenuName ( DVDMenu *pMenu, QString &qsNewName )
{
  UndoBuffer *pBuffer = pMenu->getUndoBuffer ( );
  if ( pBuffer )
       pBuffer->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::EDIT_MENU, pMenu ) );

  if ( pMenu )  {
    QString qsSubMenuName = pMenu->getInterface ( )->qsMenuName;
    pMenu->getInterface ( )->qsMenuName = qsNewName;
    //m_pTabWidgetMain->setTabLabel ( m_pTabWidgetMain->currentPage ( ), qsNewName );	//ooo
    m_pTabWidgetMain->setTabText ( m_pTabWidgetMain->currentIndex ( ), qsNewName );	//xxx
    if ( m_pVMGMenu )
         m_pVMGMenu->replaceMenuName ( qsSubMenuName,  qsNewName );
    for ( unsigned int t=0; t<(uint)m_listDVDMenus.count ( ); t++ )	//ox
        m_listDVDMenus[t]->replaceMenuName ( qsSubMenuName, qsNewName );
  }
  slotUpdateStructure ( );
}

void QDVDAuthor::slotCloneCurrentSubMenu ()
{
  // Here we remove the current SubMenu ...
//  bool bVMGM = false;
  DVDMenu *pMenu, *pOrigMenu = getCurrentSubMenu ( );
  if ( ! pOrigMenu )
    return;

  pMenu  = pOrigMenu->clone ( );
  pMenu->getInterface ( )->iTitleset = -1;
  pMenu->getInterface ( )->iMenu     = -1;

  if ( pMenu->getInterface ( )->qsMenuName == QString ( VMGM_LABEL ) ) {
//    bVMGM = true;
    QString qsNewTitle = tr ( "Cloned VMGM" );
    pMenu->getInterface ( )->qsMenuName = qsNewTitle;
    //m_pTabWidgetMain->setTabLabel ( m_pTabWidgetMain->currentPage ( ), qsNewTitle );  //ooo
    m_pTabWidgetMain->setTabText ( m_pTabWidgetMain->currentIndex ( ), qsNewTitle );    //xxx
  }

  addDVDMenu ( pMenu );

  // Adjust buttons in the new menu who pointed to the menu itself, 
  // to point now to the new sub-menu.
  QString qsNewName = pMenu    ->getInterface ( )->qsMenuName;
  QString qsOldName = pOrigMenu->getInterface ( )->qsMenuName;
//  if ( bVMGM )
//       pMenu = (DVDMenu *)m_pVMGMenu;
  pMenu->replaceMenuName ( qsOldName, qsNewName );
  slotUpdateStructure ( );
  updateUndoStack     ( );
}

void QDVDAuthor::slotRemoveCurrentSubMenu ( )
{
  // Here we remove the current SubMenu ...
  QString qsSubMenuName;
  //qsSubMenuName  = m_pTabWidgetMain->tabLabel ( m_pTabWidgetMain->currentPage ( ) );	//ooo
  qsSubMenuName  = m_pTabWidgetMain->tabText ( m_pTabWidgetMain->currentIndex ( ) );	//xxx
  
  //to remove shortcut "&"  //xxx
  if ( qsSubMenuName.indexOf("&") != -1 ) //xxx
    qsSubMenuName.remove(qsSubMenuName.indexOf("&"),1);    //xxx
  
  DVDMenu *pMenu = getCurrentSubMenu ( );
  if ( pMenu == m_pVMGMenu )  {
    MessageBox::information ( NULL, tr ("Can not remove VMGM"),
      tr ("I can not remove the main VMGM\n"), QMessageBox::Ok );
    return;
  }
  int iReturn = MessageBox::warning ( NULL, tr ("Warning, deleting Sub Menu"),
      tr ("Are you sure you want to remove this sub-menu ?\n"
          "All data will be lost permanently."), 
      QMessageBox::Yes|QMessageBox::Default,  QMessageBox::No|QMessageBox::Escape );

  if ( iReturn == QMessageBox::Yes )
    //m_pTabWidgetMain->removePage ( m_pTabWidgetMain->currentPage ( ) );	//ooo
    m_pTabWidgetMain->removeTab ( m_pTabWidgetMain->currentIndex ( ) );		//xxx

  //m_listDVDMenus.remove ( pMenu );	//ooo
  m_listDVDMenus.removeOne ( pMenu );	//xxx

  delete pMenu;
  slotUpdateStructure   ( );
  updateUndoStack       ( );		//oooo
}

void QDVDAuthor::slotDeleteTempOlderThan ()
{
	// This function will check if there is outdated Temp data which ought to be deleted.
	if (Global::iDeleteTempOlderThan < 0)
		return;

	uint t;
	Utils theUtils;
	QStringList listToBeDeleted;
	QStringList listOfProjects = theUtils.getProjectsFromTempPath();
	QString qsProjectTemp;
	QFileInfo fileInfo;
	QDateTime currentTime = QDateTime::currentDateTime();
	QDateTime modificationTime;

	for (t=0;t<(uint)listOfProjects.count();t++)	{	//ox
		qsProjectTemp = Global::qsTempPath + QString ("/") + listOfProjects[t];

		fileInfo.setFile (qsProjectTemp + QString ("/background.jpg"));
		if ( ! fileInfo.exists() )
			fileInfo.setFile (qsProjectTemp + "/" + QString (VMGM_LABEL"/background.jpg"));
		if ( ! fileInfo.exists() )
			continue;
		modificationTime = fileInfo.lastModified();
		if (modificationTime.secsTo (currentTime) > Global::iDeleteTempOlderThan * 60*60*24)
			listToBeDeleted.append (qsProjectTemp);
	}	
	
	// First we check if there was some old data found ...
	if (listToBeDeleted.count () > 0)	{
		// Next we ask the user if he wants to delete the data.
		if (MessageBox::warning (NULL, tr("Deleting temporary data."), 
			tr ( "Attention, I found some temporary data which exceeds the timeframe\n") +
			tr ( "you have set in the SetupDialog (Tools->Setup ...)\n\n") +
			listToBeDeleted.join ("\n") +
			tr ( "\n\nDo you want to delete the temporary data ?"), 
					 QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::Yes)	{
			for (t=0;t<(uint)listToBeDeleted.count ();t++)	//ox
				theUtils.recRmdir (listToBeDeleted[t], "draggedImage");
		}
	}
}

//void QDVDAuthor::slotTabChanged ( QWidget *pWidget )	//ooo
void QDVDAuthor::slotTabChanged ( int pTab )		//xxx
{ 
  QWidget *pWidget = m_pTabWidgetMain->widget(pTab);    //xxx
  
  // Here we  need to take care of two things
  // 1) The Tab has changed to a SubMenu (including vmgm)
  // 2) the tab has changed into the XML tab -=> Create XML output
  if ( ! pWidget )	//ooo
  //if ( ! pTab )		//xxx
    return;

  int t, i, j;
  if ( pWidget == getTabWidget ( TabMainMenu ) )  {	//ooo
  //if ( 0 )  {						//xxx
    DVDMenu *pMenu = getCurrentSubMenu ( );
    if ( pMenu )  {    
      //m_pColorToolbar->setDVDMenu ( pMenu );  //oooo
    }
  }
  // The last tab is the one to dispaly the XML code
  else if ( pWidget == getTabWidget ( TabDVDAuthorXml ) )  {	//ooo
  //else if ( 3 )  {						//xxx
    if ( ! m_pVMGMenu )
      return;
    m_pComboSource->clear ( );
    //m_pComboSource->insertItem ( QString ("") );		//ooo
    m_pComboSource->addItem ( QString ("") );			//xxx
    //m_pComboSource->insertItem ( DVDAUTHOR_XML );		//ooo
    m_pComboSource->addItem ( DVDAUTHOR_XML );			//xxx
    //m_pComboSource->insertItem ( m_pVMGMenu->name ( ) );	//ooo
    m_pComboSource->addItem ( m_pVMGMenu->name ( ) );		//xxx
    for (t=0;t<sourceFileCount();t++) {
      SourceFileEntry *pEntry = sourceFileEntry ( t );
      for (i=0;i< (int) pEntry->listFileInfos.count(); i++) {
	    SourceFileInfo *pInfo = pEntry->listFileInfos[i];
	    if ( pInfo ) {
	      QFileInfo fileInfo ( pInfo->qsFileName );
	      for ( j=0;j<MAX_SUBTITLES; j++ ) {
	        if ( pInfo->arraySubtitles [ j ] ) {
	          QString qsSpumuxFile;
	          //qsSpumuxFile.sprintf ("%s_sub%02d.xml", fileInfo.baseName ( ).ascii ( ), pInfo->arraySubtitles[j]->m_iSubtitleNumber );		//ooo
	          qsSpumuxFile.sprintf ("%s_sub%02d.xml", fileInfo.baseName ( ).toLatin1 ( ).data(), pInfo->arraySubtitles[j]->m_iSubtitleNumber );	//xxx
	          //m_pComboSource->insertItem ( qsSpumuxFile );	//ooo
	          m_pComboSource->addItem ( qsSpumuxFile );		//xxx
	        }
	      }
	    }
      }
    }

    m_pTextEditXml->clear();
    //for (uint t=0;t<(uint)m_listDVDMenus.count();t++)	                //ooo
    for (uint t=0;t<static_cast<uint> ( m_listDVDMenus.count() );t++)	//xxx
      //m_pComboSource->insertItem(m_listDVDMenus[t]->name());		//ooo
      m_pComboSource->addItem(m_listDVDMenus[t]->name());		//xxx
  }
  else if ( pWidget == getTabWidget ( TabSubtitles ) )  {
    // the Subtitles - tab.
    //    if ( m_pSubtitlesGui )
    //         m_pSubtitlesGui->grabKeyboard ( );
  }
  else if ( pWidget == getTabWidget ( TabDVDLayout ) )  {  
    // Need to kick off timer to re-generate the screenshots for the DVDMenus
    if ( m_pDVDLayoutGui )
         m_pDVDLayoutGui->refreshMenus ( );
  }
  // Otherwise it must be a submenu
  /*else  {
    DVDMenu *pMenu = getCurrentSubMenu ( );
    if ( pMenu )
      m_pColorToolbar->setDVDMenu ( pMenu );
  }*/ //oooo
  slotUpdateMenuBar ( );

  if ( m_pUndoStackDialog && m_pUndoStackDialog->isVisible ( ) )
       showUndoStack ( );	//oooo
}

void QDVDAuthor::slotUpdateMenuBar ( )
{
  // Standard is to set undo / redo to false
  DVDMenu *pMenu = getCurrentSubMenu  ( );
  editUndoAction->setEnabled    ( false );
  editRedoAction->setEnabled    ( false );
  m_pActionUndoStack->setEnabled( false );
  /*editCutAction->setEnabled     ( false );
  editCopyAction->setEnabled    ( false );
  editPasteAction->setEnabled   ( false );*/	//oooo

  if ( ! pMenu )
    return;

  /*editCutAction->setEnabled     ( true );
  editCopyAction->setEnabled    ( true );
  editPasteAction->setEnabled   ( true );*/	//oooo
  m_pActionUndoStack->setEnabled( true );

  // Then we check if there is something in the buffer.

  int  iUndoCount = pMenu->getUndoBuffer ( )->count ( );
  if ( iUndoCount > 0 )
    editUndoAction->setEnabled ( true );

  iUndoCount = pMenu->getUndoBuffer ( )->countRedo ( );
  if ( iUndoCount > 0 )
    editRedoAction->setEnabled ( true );
}

void QDVDAuthor::slotFeedback ()
{
  Utils theUtils;
  // First let us  make sure the temp dir exists...
  getTempFile ( QString() );
  // then let us collect some feedback data 
  QString qsFeedbackDir = Global::qsTempPath + QString ("/") + Global::qsProjectName + QString ("/feedback"); 
  QDir feedbackDir ( qsFeedbackDir );
  if ( ! feedbackDir.exists  ( ) )  {
	if ( ! theUtils.recMkdir ( qsFeedbackDir ) )
		return;
  }

  // Here we get the project file.
  slotAutosave ();
  QString qsBashName   = QString ("%1/feedback.sh").arg(Global::qsTempPath);
  QString qsBashScript = Global::qsBashString;
  qsBashScript += QString ( "mkdir %1/qrender" );
  qsBashScript += QString ( "find /tmp/qrender -name \"*.xml\" -exec cp {} %1/qrender/ \\;" );
  qsBashScript += QString ( "find /tmp/qrender -name \"*.txt\" -exec cp {} %1/qrender/ \\;" );
  qsBashScript += QString ("uname -a > %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("procinfo >> %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("mplayer -vo help -ao help >> %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("echo PREFIX_DIRECTORY="PREFIX_DIRECTORY" >> %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("echo SYSTEM_DIRECTORY="SYSTEM_DIRECTORY" >> %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("echo Global::qsSystemPath="+ Global::qsSystemPath +" >> %1/system.txt\n").arg( qsFeedbackDir );
  qsBashScript += QString ("echo \"QDVDAuthorVersionNumber="QDVDAUTHOR_VERSION_NUMBER"\" >> %1/system.txt\n").arg ( qsFeedbackDir );
  qsBashScript += QString ("echo \"Build ("__DATE__")\" >> %1/system.txt\n").arg ( qsFeedbackDir );
  qsBashScript += QString ("cd \"%1/%2/\"\n").arg(Global::qsTempPath).arg(Global::qsProjectName);
  qsBashScript += QString ("cp \"%1/qrender/*.txt\" \"%2\"\n").arg ( Global::qsTempPath ).arg ( qsFeedbackDir );
  qsBashScript += QString ("cp ~/.qdvdauthor/*.ini \"%1\"\n").arg( qsFeedbackDir );
  qsBashScript += QString ("cp ~/.qdvdauthor/autosave.xml \"%1\"\n").arg( qsFeedbackDir );
  qsBashScript += QString ("cp \"%1\" \"%2\" 2> /dev/null\n").arg(Global::qsProjectFileName).arg( qsFeedbackDir );
  qsBashScript += QString ("find . > feedback/filelist_temp\n");
  qsBashScript += QString ("find " + Global::qsSystemPath + "/share/qdvdauthor > feedback/filelist_sys\n");
  qsBashScript += QString ("find $HOME/.qdvdauthor > feedback/filelist_home\n");
  //qsBashScript += QString ("find . -name \"*.xml\" -exec cp {} %1 \\;\n").arg ( qsFeedbackDir );
  //qsBashScript += QString ("find . -name \"*.txt\" -exec cp {} %1 \\;\n").arg ( qsFeedbackDir );
  qsBashScript += QString ("find . -name \"*.xml\" -exec tar -rf feedback/xml_files.tar {} \\;\n");
  qsBashScript += QString ("find . -name \"*.txt\" -exec tar -rf feedback/txt_files.tar {} \\;\n");
  qsBashScript += QString ("tar -czf \"%1/%2/feedback.tar.gz\" feedback/*\n").arg(Global::qsTempPath).arg(Global::qsProjectName);
  qsBashScript += QString ("rm -r feedback\n");
  QFile bashFile (qsBashName);
  if ( bashFile.open( QIODevice::WriteOnly ) ) {
    QTextStream stream( &bashFile );
    stream << qsBashScript;
    bashFile.close();
  }
  else	{
    MessageBox::information (NULL, tr ("Error creating script file"),
        tr ("\nCould not create script file :\n"
	    "%1\n"
	    "Please check access rights etc.").arg(qsBashName), QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  // And here we grant read/write/execute permission.
  //chmod ( qsBashName.utf8 ( ), S_IEXEC | S_IRUSR | S_IWRITE | S_IROTH | S_IWOTH | S_IXOTH );  //ooo
  chmod ( qsBashName.toUtf8 ( ), S_IEXEC | S_IRUSR | S_IWRITE | S_IROTH | S_IWOTH | S_IXOTH );  //xxx

  // And now execute the script ...
  //if ( system (qsBashName) == -1 )    //ooo
  if ( system (qsBashName.toLatin1().data()) == -1 )    //xxx
       return;
  MessageBox::information (NULL, tr ("Collected Feedback Data"),
      //tr ("\nPlease email to: QDVDAuthor@users.sf.net and attach the file :\n"    //ooo
      tr ("\nPlease email to: qtrans@users.sf.net and attach the file :\n"
	  "%1/%2/feedback.tar.gz\n").arg(Global::qsTempPath).arg(Global::qsProjectName), QMessageBox::Ok, QMessageBox::NoButton);
}

void QDVDAuthor::slotEditAddSlideshow ()
{
	m_pSourceToolbar->slotAddSlideshow();
}

void QDVDAuthor::slotCalculateSize ()
{
  uint t, i, megaBytes = 1024 * 1024;
  unsigned long iMaxDVDSize = (unsigned long) (4.3 * 1024 * 1024 * 1024); // ==> 4.3 GBytes

  unsigned long iDVDSize = 0;
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;
  QTime theLength, zeroTime;
  QString qsSize, qsFileName;
  float fPercentage;
  QFileInfo fileInfo;
  Utils theUtils;
  // This function will update the SizeSlider
  // This is the first and very basic version

  // Per menu I take 20 MBytes + 15 MBytes/minute
  // Per video I will calculate 8MBits/second
  QList<DVDMenu *> listMenus = getSubMenus ();

  for (t=0;t<(uint)listMenus.count();t++)	{
    iDVDSize += 20*megaBytes;
    iDVDSize += listMenus[t]->getInterface ()->timeDuration.second() * megaBytes / 60 * 15;
  }

  uint iSourceEntryCount = sourceFileCount ( );
  for ( t=0; t<iSourceEntryCount; t++ )  {
    pEntry = sourceFileEntry ( t );
    for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ )  {	//ox
      pInfo = pEntry->listFileInfos[i];
      qsFileName = pInfo->getFileName ( );
      fileInfo.setFile ( qsFileName );

      if ( pInfo->pTranscodeInterface )  {
        int  iAudioBitrate = pInfo->pTranscodeInterface->iAudioBitrate;
        int  iVideoBitrate = pInfo->pTranscodeInterface->iVideoBitrate;
        long iDuration     = theUtils.getMsFromString ( pInfo->qsLength );

        if ( iDuration == 0 )  {
          if ( fileInfo.exists ( ) )
            iDVDSize += fileInfo.size ( );
          else  {
            fileInfo.setFile ( pInfo->qsFileName );
            iDVDSize += fileInfo.size ( );
          }
        }
        else {
          //iDVDSize += (int)( (float)iDuration / 1000.0f * ( iAudioBitrate + iVideoBitrate ) / 8.0f );	//ooo
	  iDVDSize += static_cast<int> ( static_cast<float> (iDuration) / 1000.0f * ( iAudioBitrate + iVideoBitrate ) / 8.0f );  //xxx
	}
      }
      else  {
        if ( fileInfo.exists ( ) )
          iDVDSize += fileInfo.size ( );
        else {
          fileInfo.setFile ( pInfo->qsFileName );
          iDVDSize += fileInfo.size ( );
        }
      }
      iDVDSize += getAudioSize ( pInfo );
    }
  }

  //fPercentage = (float)( (double)iDVDSize / (double)iMaxDVDSize );						//ooo
  fPercentage = static_cast<float> ( static_cast<double> ( iDVDSize ) / static_cast<double> ( iMaxDVDSize ) );	//xxx

  if ( fPercentage > 1.0 )  {
    //iMaxDVDSize = (int)( 2.0 * iMaxDVDSize ); // switch to 8 GByte Dual layer DVDs				//ooo
    iMaxDVDSize = static_cast<unsigned long> ( 2.0 * iMaxDVDSize ); // switch to 8 GByte Dual layer DVDs	//xxx
    //fPercentage = (float)( (double)iDVDSize / (double)iMaxDVDSize );							//ooo
    fPercentage = static_cast<float> ( static_cast<double> ( iDVDSize ) / static_cast<double> ( iMaxDVDSize ) );	//xxx
  }

  if ( fPercentage > 1.0 )
    fPercentage = 1.0;

  //m_pProgressSize->setProgress ( (int)(fPercentage * 1000) );			//ooo
  m_pProgressSize->setValue ( static_cast<int> ( fPercentage * 100 ) );		//xxx
  // Change color from green to red ...
  //m_pLabelSize->setPaletteBackgroundColor ( QColor ((int)(fPercentage*250.0), (int)((1.0-fPercentage) * 250.0), 0) );				//ooo

  QPalette palette;																//xxx
  palette.setBrush(QPalette::Window, QColor (static_cast<int> (fPercentage*250.0), static_cast<int> ((1.0-fPercentage) * 250.0), 0) );		//xxx
  //palette.setBrush(QPalette::Window, QColor (22, 120, 0) );		//xxx
  m_pLabelSize->setPalette(palette);														//xxx
  m_pLabelSize->setAutoFillBackground(true);				//xxx
  if (iDVDSize > 1024*megaBytes)
    //qsSize.sprintf ("%0.2f GB", (double)iDVDSize /(1024.0*megaBytes));		//ooo
    qsSize.sprintf ("%0.2f GB", static_cast<double> ( iDVDSize ) /(1024.0*megaBytes));	//xxx
  else if (iDVDSize > megaBytes)
    //qsSize.sprintf ("%0.2f MB", (double)iDVDSize / megaBytes);			//ooo
    qsSize.sprintf ("%0.2f MB", static_cast<double> ( iDVDSize ) / megaBytes);		//xxx
  else if (iDVDSize > 1024)
    //qsSize.sprintf ("%0.2f kB", (double)iDVDSize / 1024.0);				//ooo
    qsSize.sprintf ("%0.2f kB", static_cast<double> ( iDVDSize ) / 1024.0);		//xxx
  else 
    //qsSize.sprintf ("%d B", (int)iDVDSize);						//ooo
    qsSize.sprintf ("%d B", static_cast<int> ( iDVDSize ) );				//xxx
  m_pLabelSize->setText (qsSize);
}

unsigned long QDVDAuthor::getAudioSize ( SourceFileInfo *pInfo )
{
  int t;
  Audio *pAudio = NULL;
  unsigned long iSize = 0;
  QFileInfo fileInfo;
  Utils theUtils;

  for ( t=0; t<MAX_AUDIO; t++ )  {
    if ( pInfo->arrayAudio[t] )  {
      pAudio = pInfo->arrayAudio[t];

      if ( pAudio->pTranscodeInterface )  {
        int  iAudioBitrate = pAudio->pTranscodeInterface->iAudioBitrate;
        int  iVideoBitrate = pAudio->pTranscodeInterface->iVideoBitrate;
        long iDuration     = theUtils.getMsFromString ( pInfo->qsLength );
        if ( iDuration == 0 )  {
          fileInfo.setFile ( pAudio->pTranscodeInterface->qsTempFile );
          if ( fileInfo.exists ( ) )
            iSize += fileInfo.size ( );
          else  {
            fileInfo.setFile ( pAudio->qsFileName );
            iSize += fileInfo.size ( );
          }
        }
        else
          iSize += (int)( (float)iDuration/1000.0f * ( iAudioBitrate + iVideoBitrate ) / 8.0f );
      }
      else  {
        fileInfo.setFile ( pAudio->qsFileName );
        iSize += fileInfo.size ( );
      }
    }
  }
  return iSize;
}

void QDVDAuthor::slotAddSubMenu ( )
{
  QString qsNewTabLabel = QString ( "SubMenu %1" ).arg ( m_listDVDMenus.count ( ) + 1 );
  DVDMenu *pNewMenu = new DVDMenu ( this );
  pNewMenu->getInterface ( )->qsMenuName = qsNewTabLabel;
  pNewMenu->getInterface ( )->iTitleset  = -1; //m_listDVDMenus.count() + 2;
  pNewMenu->getInterface ( )->iMenu      = -1; //0;

  pNewMenu->setTabLabel ( qsNewTabLabel );

  addDVDMenu ( pNewMenu );

  slotUpdateStructure ( );
}

DVDMenu *QDVDAuthor::getVMGMenu ()
{
  return m_pVMGMenu;
}

DVDMenu *QDVDAuthor::getCurrentSubMenu ()
{  
  // determines the SubMenu which is currently selected in the main Tab widget
  QString qsSubMenuName;
  if ( ! m_pVMGMenu )
    return NULL;

  //qsSubMenuName = m_pTabWidgetMain->tabLabel ( m_pTabWidgetMain->currentPage ( ) );	//ooo
  qsSubMenuName = m_pTabWidgetMain->tabText ( m_pTabWidgetMain->currentIndex ( ) );	//xxx
  //qsSubMenuName.append ( m_pTabWidgetMain->tabText ( m_pTabWidgetMain->currentIndex ( ) ) );	//xxx
  
  //to remove shortcut "&"  //xxx
  if ( qsSubMenuName.indexOf("&") != -1 ) //xxx
    qsSubMenuName.remove(qsSubMenuName.indexOf("&"),1);    //xxx

  return getSubMenuByName (qsSubMenuName);
}

DVDMenu *QDVDAuthor::getSubMenuByName ( QString qsSubMenuName )
{  
  uint t;
  if ( m_pVMGMenu && m_pVMGMenu->getInterface()->qsMenuName == qsSubMenuName )	{
    return m_pVMGMenu;
  }
  //for (t=0;t<(uint)m_listDVDMenus.count();t++)	{                 //ooo
  for (t=0;t<static_cast<uint> ( m_listDVDMenus.count() );t++)	{ //xxx
    if (m_listDVDMenus[t]->getInterface()->qsMenuName == qsSubMenuName)	{
      return m_listDVDMenus[t];
    }
  }

  return NULL;
}

void QDVDAuthor::slotGenerateMasks()
{
  int iMenuCounter = 0;
  QString qsFileName;
  QString qsHeader (tr("Generating Masks ..."));
  QString qsText (tr("Please be patient, I am generating the masks for the Menus.\n"));

  /*QMessageBox *pTheMessage = new QMessageBox (qsHeader, qsText, QMessageBox::Information, (int)QMessageBox::Ok, (int)QMessageBox::NoButton, (int)QMessageBox::NoButton, NULL, tr("info"), false );
  pTheMessage->show();
  pTheMessage->setText(qsText);*/		//oooo

  DVDMenu *pMenu = m_pVMGMenu;
  //while (iMenuCounter <= (int)m_listDVDMenus.count())	{                   //ooo
  while (iMenuCounter <= static_cast<int> ( m_listDVDMenus.count() ) )	{    //xxx
    qsFileName  = getTempFile(pMenu->name()) + QString("/") + QString(HIGHLIGHTED_NAME);
    pMenu->createMask (qsFileName, HIGHLIGHTED_MASK);
    qsFileName = getTempFile (pMenu->name()) + QString ("/") + QString(SELECTED_NAME);
    pMenu->createMask (qsFileName, SELECTED_MASK);
    qsFileName = getTempFile (pMenu->name()) + QString ("/") + QString(BACKGROUND_NAME);
    pMenu->createMask(qsFileName, BACKGROUND_IMG);
    //if ( ++iMenuCounter < m_listDVDMenus.size ( ) )		//xxx
      pMenu = m_listDVDMenus[iMenuCounter++];		//ooo
      //pMenu = m_listDVDMenus[iMenuCounter - 1];		//xxx
  }
  createMenuXml();		//oooo
  //delete pTheMessage;		//oooo
}

void QDVDAuthor::editUndo ( )
{
  DVDMenu *pCurrentDVDMenu = getCurrentSubMenu ( );
  if (  !  pCurrentDVDMenu )   {
    editUndoAction->setEnabled ( false );
    editRedoAction->setEnabled ( false );
    return;
  }
  DVDMenuUndoObject *pUndoObject = (DVDMenuUndoObject *)pCurrentDVDMenu->getUndoBuffer()->pop();
  if ( pUndoObject )  {
    // We set the re-do option active ...
    editRedoAction->setEnabled ( true );
    pUndoObject->undo ( pCurrentDVDMenu );
  }
  // Check if we are at the end ...
  if ( pCurrentDVDMenu->getUndoBuffer ( )->count ( ) == 0 )
       editUndoAction->setEnabled     ( false );
}

void QDVDAuthor::editRedo ( )
{
  DVDMenu *pCurrentDVDMenu = getCurrentSubMenu ( );
  if (  !  pCurrentDVDMenu )   {
    editUndoAction->setEnabled ( false );
    editRedoAction->setEnabled ( false );
    return;
  }
  DVDMenuUndoObject *pUndoObject = (DVDMenuUndoObject *)pCurrentDVDMenu->getUndoBuffer ( )->popRedo ( );
  if ( pUndoObject )  {
    editUndoAction->setEnabled ( true );
    pUndoObject->redo ( pCurrentDVDMenu );
  }
  else  // The redo returnd nothing, meaning that the stack is empty ...
    editRedoAction->setEnabled ( false );
}

void QDVDAuthor::helpContents()
{

}

void QDVDAuthor::helpGuide ( )
{
  QString qsGuide;
  qsGuide  = QString ("<h3><p align=\"center\">'Q' DVD-Author Quick-Start Guide</p></h3>\n"
		      "How to create a DVD containing a video activated from a menu button:<p>\n"
		      "<ol>\n"
		      "<li>start a new project with <i>File, New Project</i>, accept <i>/tmp</i> "
		      "as a temporary directory, and click on <i>Next</i>, enter a project name, "
		      "and for the path to the DVD use the <i>Browse</i> button and then the "
		      "<i>Create New Folder</i> button,<br><br>\n"
		      "<li>click on the <i>Add Movie</i> button to add a video to the project,<br><br>\n"
		      "<li>click on the <i>Add Background</i> button to load an image file for "
		      "the menu background,<br><br>\n"
		      "<li>(optional) click on the <i>Add Sound</i> button to load a sound file "
		      "for the menu,<br><br>\n"
		      "<li>create menu buttons by using right-click on the workspace background "
		      "image and click on <i>Add Image</i> or <i>Add Text</i>, draw an outline "
		      "rectangle, answer the image or text dialog, and the object will appear,<br><br>\n"
		      "<li>right-click over the image or text to <i>Define as Button</i>, and "
		      "set the <i>Action</i> to <i>jump</i> to the video you have added,<br><br>\n"
		      "<li>click on the <i>DVDAuthor, Create DVD</i> menu option to begin "
		      "exporting, review the suggested commands for suitability, then click on "
		      "<i>OK</i> to begin.\n"
		      "</ol>\n");
  qsGuide += QString ("For more information, go to <a href=\"http://qdvdauthor.sourceforge.net\">http://qdvdauthor.sourceforge.net</a></p>");

  QSize theSize ( 800, 600 );
  MessageBox::html ( this, tr ( "Quick guide" ), qsGuide, theSize );
}

void QDVDAuthor::helpAbout()
{
  QString qsAbout;
  qsAbout  = QString ("<h3><p align=\"center\">'Q' DVD-Author</p></h3>\n");
  qsAbout += QString (          "<p align=\"center\">By<br>"             );
  //qsAbout += QString (             "<u>Varol Okan</u><br>"               );	//ooo
  qsAbout += QString (             "<u>Iven Wold</u><br>"                );	//xxx
  qsAbout += QString (                     "<br>"                        );
  qsAbout += QString (             "Ver: %1 (build:%2)<br>"              ).arg(QDVDAUTHOR_VERSION_NUMBER).arg(__DATE__);
  //qsAbout += QString ("<a href=\"http://qdvdauthor.sourceforge.net\">http://qdvdauthor.sourceforge.net</a><br>");		//ooo
  qsAbout += QString ("<a href=\"https://sourceforge.net/projects/qdvd\">https://sourceforge.net/projects/qdvd</a><br>");				//xxx
  qsAbout += QString ("System Folder at<br>%1/share/qdvdauthor/</p>").arg ( Global::qsSystemPath );
  //uiDialogAbout dialogAbout (this);		//ooo
  DialogAbout dialogAbout (this);		//xxx
  dialogAbout.m_pLabelAbout->setText ( qsAbout );
  dialogAbout.exec();
}

void QDVDAuthor::LoadIni ()
{
  QFileInfo fileInfo;
  QString   qsAbsPath;
  // Load the paths of the tools from the ini-file
  QDVDAuthorInit *pInit = new QDVDAuthorInit (&m_dragNDropContainer);
  m_listHistory          = pInit->getHistory      ( );
  m_listHistoryPix       = pInit->getHistoryPix   ( );
/*  m_iMaxHistoryFiles     = pInit->maxHistoryFiles ( );
  m_iAutosave            = pInit->getAutosave     ( );
  pInit->setSlideshowDefaults ( );
//  Global::qsSystemPath   = pInit->systemPath ( );
  Global::listToolsPaths = pInit->getToolsPaths   ( );
  if (Global::listToolsPaths.count() == 0)	{
    // If no paths are yet in the ini-file (meaning the first time start)
    // Then we call DialogSetup::slotScanSystem, to get there ...
    Utils theUtils;
    Global::listToolsPaths = theUtils.scanSystem ( );
  }
  if ( ! m_listHistory.empty ( ) )  {
    fileInfo.setFile ( m_listHistory.last ( ) );
    //qsAbsPath = fileInfo.absFilePath ( );	//ooo
    qsAbsPath = fileInfo.absoluteFilePath ( );	//xxx
    // setCurrentPath (qsAbsPath);
  }
  // Finally we connect the Menu ...
  buildHistoryEntries ( );
  //setAutosave         ( );	//oooo
*/	//oooo
  /*if ( m_pTabsPopup )  {
       m_pTabsPopup->setItemChecked ( m_pTabsPopup->idAt ( 0 ), pInit->visibleTabs ( 0 ) );
       m_pTabsPopup->setItemChecked ( m_pTabsPopup->idAt ( 1 ), pInit->visibleTabs ( 1 ) );
       m_pTabsPopup->setItemChecked ( m_pTabsPopup->idAt ( 2 ), pInit->visibleTabs ( 2 ) );
       if ( pInit->visibleTabs ( 0 ) == false )
         m_pTabWidgetMain->removePage ( getTabWidget ( TabDVDLayout    ) );
       if ( pInit->visibleTabs ( 1 ) == false )
         m_pTabWidgetMain->removePage ( getTabWidget ( TabSubtitles    ) );
       if ( pInit->visibleTabs ( 2 ) == false )
         m_pTabWidgetMain->removePage ( getTabWidget ( TabDVDAuthorXml ) );
  }*/	//oooo
/*
  MediaCreator::setPreferredEngine ( pInit->preferredEngine ( ) );
  m_bVisibleRegion = pInit->visibleRegion ( );*/	//oooo
  /*toolsVisibleRegionAction->setOn  ( m_bVisibleRegion );
  emit ( signalVisibleRegion ( m_bVisibleRegion ) );
  slotChangeStyle ( pInit->getStyle ( ) );*/	//oooo

  /*if( Global::qsLanguage != "en" )  {
    // Here we install the language the user has chosen ...
    QTranslator *pTrans = new QTranslator( 0 );
    // Under ~/.qdvdauthor/
    QString qsLang = QString ( QString ( Global::qsSystemPath + "/share/qdvdauthor/qdvdauthor_%1.qm" ).arg ( Global::qsLanguage ) );
    if (m_pTranslator)	{
      qApp->removeTranslator ( m_pTranslator );
      delete m_pTranslator;
      m_pTranslator = NULL;	// Just for consistency ...
    }
    pTrans->load( qsLang );
    m_pTranslator = pTrans;
    qApp->installTranslator( m_pTranslator );
    languageChange();
  }*/	//oooo

  delete pInit;
}

void QDVDAuthor::SaveIni ( )
{
  // And then we store the ini - file ...
  int t;
  bool bTabs[3];
  /*bTabs[0] = tabVisible ( TabDVDLayout    );
  bTabs[1] = tabVisible ( TabSubtitles    );
  bTabs[2] = tabVisible ( TabDVDAuthorXml );*/	//oooo

  QDVDAuthorInit *pInit = new QDVDAuthorInit ( Global::listToolsPaths, &m_dragNDropContainer );

  pInit->setHistory       ( m_iMaxHistoryFiles, m_listHistory, m_listHistoryPix );

  pInit->setAutosave      ( m_iAutosave      );

  pInit->setVisibleRegion ( m_bVisibleRegion );
  /*for ( t=0; t<3; t++ )
    pInit->setVisibleTabs ( t, bTabs[t]      );*/		//oooo

  pInit->setStyle         ( m_qsStyle        );

  pInit->saveIniFile      ( );		//oooo

  // after all's said and done, I can dump this object...
  delete pInit;
}

void QDVDAuthor::slotAutosave ( )
{
  if ( ( ! Global::pApp ) || ( ! this ) )
    return;

  //printf ( "%s::%s::%d\n", __FILE__, __FUNCTION__, __LINE__ );
  // This function will create the autosave-project file
  //QString qsAutosaveFile = QDir::homeDirPath( );	//ooo
  QString qsAutosaveFile = QDir::homePath( );		//xxx
  qsAutosaveFile.append  ( AUTOSAVE_FILE_NAME );
  m_bAutosaving = true;
  fileSave ( qsAutosaveFile );
  m_bAutosaving = false;
}

void QDVDAuthor::addHistory (QString qsNewFile)
{
  int iIndex;
  QFileInfo  fileInfo (qsNewFile);
  //QString qsAbsPath = fileInfo.absFilePath();		//ooo
  QString qsAbsPath = fileInfo.absoluteFilePath();	//xxx
  // First we make a screenshot of the current VMGMenu
  updateHistoryPix ();

  // Here we add the file to the history and ensure it is only one time in the history ...
  //iIndex = m_listHistory.findIndex (qsNewFile);		//ooo
  iIndex = m_listHistory.indexOf (qsNewFile);		//xxx

  if (iIndex > -1)	{	// That should keep them in synch ...
    //m_listHistory.remove(qsNewFile);	//ooo
    m_listHistory.removeOne(qsNewFile);	//xxx
    QPixmap *pPixmap = m_listHistoryPix[iIndex];
    //m_listHistoryPix.remove (pPixmap);	//ooo
    m_listHistoryPix.removeOne (pPixmap);	//xxx
    delete pPixmap;
  }
  if((int)m_listHistory.count ( ) > m_iMaxHistoryFiles )	{
    //m_listHistory.remove ( m_listHistory.first ( ) );	//ooo
    m_listHistory.removeOne ( m_listHistory.first ( ) );	//xxx
    QPixmap *pPixmap = m_listHistoryPix.first  ( );
    //m_listHistoryPix.remove ( pPixmap );	//ooo
    m_listHistoryPix.removeOne ( pPixmap );	//xxx
    delete pPixmap;
  }

  m_listHistory.append    ( qsNewFile );
  m_listHistoryPix.append ( new QPixmap ( ) );

  Global::qsCurrentPath = qsAbsPath;
  //buildHistoryEntries ( );	//oooo
  SaveIni ( );			//oooo
}

void QDVDAuthor::buildHistoryEntries ()
{
	int t;
	QAction *pAction = NULL;
	// Here we clear the menu and re-build the entries according to the current values in
	// m_listHistory.
	m_pMenuHistory->clear ();
	for (t=(int)m_listHistory.count()-1; t>=0; t--)	{
		// First let us take care of the pixmap ... 
		if ((!m_listHistoryPix[t]) || (m_listHistoryPix[t]->isNull())	)	{
			//QImage theImage(QImage().fromMimeSource( "error.jpg" ));	//ooo
			QImage theImage(QImage( ":/images/error.jpg" ));		//xxx
			//theImage = theImage.smoothScale(m_dragNDropContainer.sizeThumbnail);	//ooo
			theImage = theImage.scaled(m_dragNDropContainer.sizeThumbnail);		//xxx
			QPixmap *pThePix =  new QPixmap();
			pThePix->convertFromImage (theImage);
			if (m_listHistoryPix[t])	// delete the old pixmap ...
				delete m_listHistoryPix[t];
			m_listHistoryPix[t] = pThePix;
		}
		//pAction = new QAction ( this, QString ("history%1").arg(t) );		//ooo
		pAction = new QAction ( QString ("history%1").arg(t), this );		//xxx
		//pAction->setIconSet (*m_listHistoryPix[t] );		//ooo
		pAction->setIcon (*m_listHistoryPix[t] );		//xxx
		pAction->setText    ( m_listHistory[t] );
		//pAction->setMenuText( m_listHistory[t] );	//oooo
		//pAction->addTo(m_pMenuHistory);	//ooo
		m_pMenuHistory->addAction(pAction);	//xxx
	}
}

void QDVDAuthor::updateHistoryPix ()
{
  int iIndex;
  if ( ! m_pVMGMenu )
    return;

  // If there is no current project then we skip it ...
  if ( Global::qsProjectFileName.isEmpty ( ) )
    return;

  // If the current project is not in the list (Should never happen though).
  //iIndex = m_listHistory.findIndex ( Global::qsProjectFileName );	//ooo
  iIndex = m_listHistory.indexOf ( Global::qsProjectFileName );		//xxx

  if (iIndex < 0) {
    //if ( ( Global::qsProjectFileName.find ( PROJECT_FILENAME ) > -1 ) ||	//ooo
    if ( ( Global::qsProjectFileName.indexOf ( PROJECT_FILENAME ) > -1 ) || 	//xxx
         //( Global::qsProjectFileName.find ( AUTOSAVE_FILE_NAME ) > -1 ) )	//ooo
         ( Global::qsProjectFileName.indexOf ( AUTOSAVE_FILE_NAME ) > -1 ) )	//xxx
      return;

    m_listHistory.append    ( Global::qsProjectFileName );
    m_listHistoryPix.append ( new QPixmap ( ) );
    //iIndex = m_listHistory.findIndex ( Global::qsProjectFileName );	//ooo
    iIndex = m_listHistory.indexOf ( Global::qsProjectFileName );	//xxx
  }

  QString  qsFileName;
  QImage   theImage;
  QPixmap *pCurrentPix = NULL;

  qsFileName = getTempFile ( m_pVMGMenu->name ( ) + QString ("/") + QString ( BACKGROUND_NAME ) );

  m_pVMGMenu->createMask   ( qsFileName, BACKGROUND_IMG, true );	//oooo
  QFileInfo fileInfo ( qsFileName );

  if ( ! fileInfo.exists ( ) )
    return;
  theImage.load ( qsFileName );
  //theImage = theImage.smoothScale ( m_dragNDropContainer.sizeThumbnail, Qt::KeepAspectRatio );			//ooo
  theImage = theImage.scaled ( m_dragNDropContainer.sizeThumbnail, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx

  pCurrentPix = m_listHistoryPix[iIndex];

  if ( ! pCurrentPix )	// delete the old pixmap ...
    pCurrentPix = new QPixmap ( );

  pCurrentPix->convertFromImage ( theImage );
  //buildHistoryEntries();	//oooo
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Here we have the routines which create the DVD structure.
//
///////////////////////////////////////////////////////////////////////////////////////////
void QDVDAuthor::slotCreateDVD ( )
{
  /*if ( m_bCreateDVDAfterSlideshows )  {
    engageCreateDVD  ( false );
    return;
  }*/	//oooo
  // This function is invoked when the user wants to create the menu structure.
  // THus this function does the following
  // create /tmp/spumux.xml
  // create /tmp/dvdauthor.xml
  // invoke spumux -> output to dialog
  // invoke dvdauthor, but use dummy vob files rather then the original (hiuge) ones.
  
  //m_pSubtitlesGui->releaseSourceFileInfo ( );		//oooo
  
  // This command will generate the Project path under the temp directory. This'll ensure the directory is present and writable for the temp data
  createTempDirStructure ( );
  QString qsTempPath = getTempFile ( QString ( ) );

  // The following line is already called from slotGenerateMasks
  //createMenuXml ( );       // create spumux.xml
  createSubtitlesXml   ( );  // create all xml files for those files which have subtitles.	//oooo
  createDvdauthorXml   ( );  // create dvdauthor.xml
  createTransitionsXml ( );	//oooo
  // And then we create the script to generate all those funky files ...
  createDVD ( );
}

void QDVDAuthor::createMenuXml ()
{
  uint t;
  Export::Spumux spumux;
  spumux.createXml   ( m_pVMGMenu );
  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )
    spumux.createXml ( m_listDVDMenus[t] );		//oooo
}

void QDVDAuthor::createTransitionsXml ( )
{
  DVDMenu *pDVDMenu = getVMGMenu ( );
  pDVDMenu->createTransitionsXml ( );

  QList<DVDMenu *> list = getSubMenus ( );		//oxx
  QList<DVDMenu *>::iterator it = list.begin ( );	//oxx
  while ( it != list.end ( ) )  {
    pDVDMenu = *it++;
    pDVDMenu->createTransitionsXml ( );
  }	//oooo
}

void QDVDAuthor::createSubtitlesXml ( )
{
  uint i, t, iCount;
  Export::Spumux   spumux;
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;  
  QFileInfo        fileInfo;
  QString          qsInfo;
  QList<SourceFileInfo *>listAlreadyRendered;		//oxx
  iCount = sourceFileCount ( );

  //  QString qsHTML = "<HTML><BODY BGCOLOR=\"BLACK\" text=\"#ff8000\">";
  //  qsHTML += "<TABLE WIDTH=\"100%\" BGCOLOR=\"#7700ff\" BORDER=\"1\"><TR><TD><FONT COLOR=\"#FFD000\"><B>";

  qsInfo  = "<HTML><BODY>";
  qsInfo += tr ( "The subtitles for the following files have been rendered already.<P>" );
  qsInfo += "<FONT COLOR=\"BLUE\"><B>";
  for (t=0;t<iCount;t++) {
    pEntry = sourceFileEntry ( t );
    for (i=0;i<(uint)pEntry->listFileInfos.count ();i++) {	//ox
      pInfo  = pEntry->listFileInfos[i];
      // the following two calls go in concert ...
      if ( renderSubtitles  ( pInfo ) ) {
	// Already rendered ALL of the subtitles associated with this SourceFileInfo
	// Let's ask the user if he wants to re-render all subtitles.
	fileInfo.setFile ( pInfo->qsFileName );
	qsInfo += fileInfo.fileName ( ) + QString ( "<BR>" );
	listAlreadyRendered.append  ( pInfo );
      }
      else // if the subtitle was rendered here, we have to create the XML file as well
	spumux.createXml ( pInfo );
    }
  }
  qsInfo += "</B></FONT>";

  if ( listAlreadyRendered.count ( ) > 0 ) {
    qsInfo += tr ( "<P>Do you want to re-render all of them ?" );
    qsInfo += "</BODY></HTML>";
    if ( MessageBox::html ( this, tr ( "Subtitles already rendered." ), qsInfo, QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) {
      for ( t=0; t<(uint)listAlreadyRendered.count ( ); t++ ) {	//ox
	pInfo = listAlreadyRendered[t];
	if ( pInfo ) {
	  renderSubtitles  ( pInfo, true );
	  spumux.createXml ( pInfo );
	}
      }
    }
  }
}

// The following function will display the XML code for the specified Submenu (spumux)
// or the dvdauthor.xml - file in the XML - tab.
void QDVDAuthor::slotCreateXml ( const QString &qsWhich )
{
  uint t, i, j;
  bool bFound = false;
  Utils theUtils;
  QString fileSpumux, qsSubIdx;
  Export::Spumux spumux;
  SourceFileEntry *pEntry = NULL;
  SourceFileInfo  *pInfo  = NULL;
  Subtitles  *pSubtitles  = NULL;
  QFileInfo   fileInfo;

  createTempDirStructure  ( );
  if (    qsWhich.isEmpty ( ) )  {
    m_pTextEditXml->clear ( );
    return;
  }
  if (qsWhich == QString ( DVDAUTHOR_XML ) )  {
    createDvdauthorXml   ( );
    fileSpumux = getTempFile ( QString ( DVDAUTHOR_XML ) );
  }
  else if ( m_pVMGMenu && qsWhich == m_pVMGMenu->name ( ) )  {
    spumux.createXml ( m_pVMGMenu );
    fileSpumux = getTempFile ( m_pVMGMenu->name ( ) ) + QString ("/menu.xml");
  }
  else {
    //for (t=0;t<(uint)m_listDVDMenus.count();t++)	{	               //ooo
    for (t=0;t<static_cast<uint> ( m_listDVDMenus.count() );t++)	{	//xxx   
      if (qsWhich == m_listDVDMenus[t]->name())	{
        // Here we create the xml - file in the temp dir.
        spumux.createXml ( m_listDVDMenus[t] );
        fileSpumux = getTempFile(m_listDVDMenus[t]->name()) + QString ("/menu.xml");
        bFound = true;
        break;
      }
    }
    if ( ! bFound ) {
      for ( t=0; t< (uint) sourceFileCount ();t++ ) {
        pEntry = sourceFileEntry ( t );
        if ( ! pEntry )
          continue;

        for ( i=0;i<(uint)pEntry->listFileInfos.count ( ); i++ ) {	//ox
          pInfo = pEntry->listFileInfos [ i ];
          fileInfo.setFile ( pInfo->qsFileName );
          for ( j=0; j<MAX_SUBTITLES; j++ ) {
            pSubtitles = pInfo->arraySubtitles [ j ];
            if ( ! pSubtitles )
              continue;
            // Here we do the translation between what is shown and the actual file name.
            //fileSpumux.sprintf ("%s_sub%02d.xml", fileInfo.baseName ( ).ascii ( ), pSubtitles->m_iSubtitleNumber );		//ooo
	    fileSpumux.sprintf ("%s_sub%02d.xml", fileInfo.baseName ( ).toLatin1 ( ).data(), pSubtitles->m_iSubtitleNumber );	//xxx
            if ( qsWhich == fileSpumux ) {
              // The actual file name looks more like this ...
              qsSubIdx.sprintf ( "/subtitle_%d.xml", pSubtitles->m_iSubtitleNumber );
              //fileSpumux = getTempFile ( fileInfo.baseName ( TRUE ) );	//ooo
	      fileSpumux = getTempFile ( fileInfo.baseName ( ) );		//xxx
              QDir tempDir ( fileSpumux );
              if ( ! tempDir.exists ( ) )
                theUtils.recMkdir   ( fileSpumux );
              spumux.createXml  ( pInfo );
              fileSpumux += qsSubIdx;
              i = pEntry->listFileInfos.count ( );
              break;
            }
          }
        }
      }
    }
  }

  fileInfo.setFile ( fileSpumux );

  if ( ! fileInfo.exists () )
    return;
  // Finally we read in the xml file.
  m_pTextEditXml->clear();
  QFile file( fileSpumux ); // Read the text from a file
  if ( file.open( QIODevice::ReadOnly ) ) {
    QTextStream stream( &file );
    //m_pTextEditXml->setText( stream.read() );		//ooo
    m_pTextEditXml->setText( stream.readAll() );	//xxx
  }		//oooo
}

void QDVDAuthor::createDvdauthorXml ()
{
  Export::DVDAuthor theExporter;
  theExporter.createXml ( );	//oooo
}

bool QDVDAuthor::renderSubtitles ( SourceFileInfo *pInfo, bool bForce )
{
  if ( ( ! pInfo ) || ( pInfo->subtitleCount ( ) < 1 ) )
    return false;

  int        t;
  QString    qsBasePath;
  Utils      theUtils;
  bool       bAlreadyRendered = true;
  Subtitles *pSubtitles, *pAlreadyRendered;
  QList<Subtitles *> list;			//oxx
  QFileInfo  fileInfo   ( pInfo->qsFileName );

  // Okay the subtitles should be named like $TMP/$PROJ/BASE_NAME/sub_<start>.png
  //qsBasePath = theUtils.getTempFile( fileInfo.baseName ( true ) );	//ooo
  qsBasePath = theUtils.getTempFile( fileInfo.baseName ( ) );		//xxx
  QDir tempDir ( qsBasePath );
  if ( ! tempDir.exists ( ) )
    theUtils.recMkdir ( qsBasePath );

  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    pSubtitles = pInfo->arraySubtitles [ t ];
    if ( pSubtitles ) {
      pAlreadyRendered = pSubtitles->render ( qsBasePath, pInfo->qsResolution, pInfo->qsFileName, bForce );
      if ( ! pAlreadyRendered ) {
             bAlreadyRendered = false;
      }
      else
        list.append ( pAlreadyRendered );
    }
  }
  // The logic is thet if any of the subtitles had to be rendered ( did not yet exist ), 
  // then we want to render all subtitles of this file even if it existed.
  if ( ! bAlreadyRendered && list.count ( ) > 0 ) {
    for ( t=0; t<(int)list.count ( ); t++ ) {
      pSubtitles = list [ t ];
      if ( pSubtitles )
           pSubtitles->render ( qsBasePath, pInfo->qsResolution, pInfo->qsFileName, true );
    }
  }
  return bAlreadyRendered;
}

bool QDVDAuthor::checkForNewProject ()
{
  if (Global::qsProjectPath.isEmpty())	{
    newWizard ();
    return false;
  }
  return true;
}

bool QDVDAuthor::checkHealth ()
{
  // This function checks if all is in order
  // o  There is a background image.
  //    - If not only a standard VideoDVD is created without menu.
  // o  There are buttons defined
  //    - Wether through a mask, or manually defined.
  // o  There is enough space on the defined temp - path.
  //    - approximately by adding all sizes together.
  // o  And the temp drive should be writeable.
  // o  And a whole bunch of other sanity checks.
  uint t;
  CDVDMenuInterface *pInterface = m_pVMGMenu ? m_pVMGMenu->getInterface ( ) : NULL;

  // First I want to check if the MenuLength is the same as the length of the 
  // associated Audio files, and the same as the MovieObjects within.
  DVDMenu *pDVDMenu = m_pVMGMenu;
  QString qsWarning, qsTemp;
  for ( t=0; t<=(uint)m_listDVDMenus.count ( ); t++ ) {	//ox
    qsTemp = pDVDMenu->checkDuration ( );
    if ( qsTemp.length ( ) > 1 )
      qsWarning += qsTemp;
    if ( (uint)m_listDVDMenus.count ( ) > t )		//xxx
      pDVDMenu = m_listDVDMenus[t];
  }
  if ( qsWarning.length ( ) > 1 ) {
    QString qsHTML = "<HTML><BODY BGCOLOR=\"BLACK\" text=\"#ff8000\">";
    qsHTML += "<TABLE WIDTH=\"100%\" BGCOLOR=\"#7700ff\" BORDER=\"1\"><TR><TD><FONT COLOR=\"#FFD000\"><B>";
    qsHTML += tr ( "There are issues with some of the DVDMenu length, which do not line up with the included Audio tracks and/or the associated MovieObjects.<BR>" );
    qsHTML += tr ( "This could cause unintended side effects with the affected menus.<P>" );
    qsHTML += tr ( "Below is the list of all DVDMenus and objects.</B></FONT></TD></TR></TABLE>" );
    qsHTML += qsWarning;
    qsHTML += tr ( "&nbsp;<P><CENTER><FONT COLOR=\"RED\"><B>Do you want to fix them first ?</B></FONT></CENTER>" );
    qsHTML += "</BODY></HTML>";
    if ( MessageBox::html ( this, tr ( "Warning, Menu length differences." ), qsHTML, QSize ( 800, 600 ),
		       QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
      return false;
  }

	//oooo
  // primary implementation only checks for Background image.
  if ( Global::qsTempPath.isEmpty ( ) )  {
    MessageBox::warning ( this, tr ("Error, missing temp path."),
			  tr ("You forgot to specify a temp path.\n"
			      "I can not build this DVD without temp path.\n"
			      "Please define the missing temp path first.\n"),
			  QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }
  else  {
    // Check if we can write to the temp path.
    QFile file( Global::qsTempPath + QString ("/test_file") );
    if ( !file.open( QIODevice::WriteOnly ) )	{
      MessageBox::warning ( this, tr ("Error, can't write to temp."),
			    tr ("I can not write to the specified temp directory.\n%1\n"
				"Please verify the temp path and try again.\n").arg(Global::qsTempPath),
			    QMessageBox::Ok, QMessageBox::NoButton);
      return false;
    }
    else  {
      file.close  ( );
      file.remove ( );
    }
  }
  if (Global::qsProjectPath.isEmpty())	{
    if (MessageBox::warning ( this, tr ("Error, missing DVD path."),
			      tr ("You forgot to specify the DVD path.\n"
				  "I can not build this DVD without a DVD path.\n"
				  "Do you want to define the missing DVD path now ?\n"),
			      QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
      slotSetup();
    return false;
  }
  else	{
    // Check if we can write to the projectPath.
    QFile file( Global::qsProjectPath + QString ("/test_file") );
    if ( !file.open( QIODevice::WriteOnly ) )	{
      if (MessageBox::warning ( this, tr ("Error, can't write to project path."),
				tr ("I can not write to the specified Project directory.\n%1\n"
				    "Do you want to change the specified path now ?\n").arg(Global::qsProjectPath),
				QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	slotSetup();
      return false;
    }
    else	{
      file.close();
      file.remove();
    }
  }
  if (pInterface->qsBackgroundFileName.isEmpty())	{
    if (MessageBox::warning ( this, tr ("No background defined."),
			      tr ("You did not define a background image.\n"
				  "Do you want to create a VideoDVD without Menu ?\n"),
			      QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)	{
      // Here we create a Video DVD without a menu
    }
    // otherwise we abort here.
    else
      return false;
  }

  QString qsBadMenus;
  QList<ButtonObject *> listButtons;		//oxx
  if ( m_pVMGMenu )
       listButtons = m_pVMGMenu->getButtons ( );
  if ( ( pInterface->qsHighlightedMaskFileName.isEmpty ( ) ) && 
       ( pInterface->qsSelectedMaskFileName.isEmpty    ( ) ) &&
       ( listButtons.count ( ) == 0) )	{
    if ( MessageBox::warning ( this, tr ("No button masks defined."),
			       tr ("You did not define any buttons.\n"
				   "Do you want to create a VideoDVD without Menu ?\n"),
			       QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)	{
      // Here we create a Video DVD without a menu
    }
    // otherwise we abort here.
    else
      return false;
  }
  if ( m_pVMGMenu && listButtons.count ( ) > 36 )
    qsBadMenus += tr ( "   %1  \n" ).arg ( m_pVMGMenu->getInterface ( )->qsMenuName );

  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ ) {	//ox
    QString qsBadMenus;
    listButtons = m_listDVDMenus[t]->getButtons ( );
    if ( listButtons.count  ( ) > 36 )
      qsBadMenus += QString ( " o   %1  \n" ).arg ( m_listDVDMenus[t]->getInterface ( )->qsMenuName );
  }
  // Finally we see if we found some menus with more then 36 buttons.
  if ( qsBadMenus.length ( ) > 1 ) {
    MessageBox::warning ( this, tr ("Too many buttons for Menu."),
      tr ("The DVD spec does allow a max of 36 buttons per DVDMenu.\n"
	  "Please reduce the number of buttons in the following menus :\n%1").arg ( qsBadMenus ),
	  QMessageBox::Ok, QMessageBox::NoButton );
    return false;
  }

  // Check if buttons are overlapping each other ...
  QString qsOverlappingButtons;
  if ( m_pVMGMenu )
    qsOverlappingButtons = m_pVMGMenu->checkButtonsOverlapping ( );
  for (t=0;t<(uint)m_listDVDMenus.count ();t++)	//ox
    qsOverlappingButtons += m_listDVDMenus[t]->checkButtonsOverlapping ( );

  if ( qsOverlappingButtons.length () > 1 ) {
    qsOverlappingButtons  = tr ( "Found buttons overlapping.<BR>I will correct these but the buttons might not look exactly the way you positioned them.<BR>" ) + qsOverlappingButtons;
    qsOverlappingButtons += tr ( "<BR>Do you want to manually correct the buttons positions ?" );
    if ( MessageBox::html ( this, tr( "Found Buttons overlapping." ), qsOverlappingButtons, QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
      return false;
  }

  if (m_pSourceToolbar->sourceFileCount() < 1)	{
    if (MessageBox::warning ( this, tr ("No movies defined."),
          tr ("You did not select any movies.\n"
	      "Do you want to create a DVD with Menu and no movies ?\n"),
	      QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)	{
      // Here we create a Video DVD without movie files.
    }
    // otherwise we abort here.
    else
      return false;
  }
  // Here we check if there is already a dvd in place where the new one should be
  QDir projectDir (Global::qsProjectPath + QString ("/VIDEO_TS/"));
  //QStringList listFiles = projectDir.entryList ("*");			//ooo
  QStringList listFiles = projectDir.entryList ( QStringList("*") );	//xxx
  if ( listFiles.count ( ) > 2 )  {	// the two standard directories . and ..
    QString stringFiles;
    for  ( t=0; t<(uint)listFiles.count ( ); t++ ) {	//ox
      if ( (listFiles[t] == ".") || (listFiles[t] == "..") )
	continue;
      stringFiles += QString ("%1\n").arg(listFiles[t]);
      if ( t > 15 ) {
	stringFiles += tr ( "... and more files ...\n" );
	break;
      }
    }
    int iReturn = MessageBox::warning ( this, tr ("DVD files found."),
		    tr ("I found DVD files most likely from a previous run.\n%1"
			"Do you want to remove those files first ?\n").arg(stringFiles),
			 QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel );
    if ( iReturn == QMessageBox::Cancel )
      return false;
    else if ( iReturn == QMessageBox::Yes)	{
      // Here we remove all files under Global::qsProjectPath/VIDEO_TS/
      for (t=0;t<(uint)listFiles.count();t++)	{	//ox
	if ( (listFiles[t] == ".") || (listFiles[t] == "..") )
	  continue;
	projectDir.remove (listFiles[t]);
      }
    }
  }

  // The last check will see if we have some slideshows on-going.
  // If this is the case, then we should wait for the termination
  // of all slideshow creation processes / threads.
  /*for ( t=0; t<(uint)sourceFileCount ( ); t++ )  {
    SourceFileEntry *pEntry = sourceFileEntry ( t );
    if ( pEntry->bIsSlideshow )  {
      // Okay I found at least one entry that is not yet done converting to a slideshow
      int iReturn = MessageBox::warning ( this, tr ("Slideshow in progress."),
        tr ("There are some slideshows not finished rendering.\n"
        "Do you want to proceed after all slideshows finished or abort ?\n"),
        QMessageBox::Yes, QMessageBox::Cancel );
      if ( iReturn == QMessageBox::Cancel )
        return false;

      engageCreateDVD ( true );
      return false;
    }
  }*/	//oooo

  // Add ButtonTransition creation check here ...
  if ( m_pVMGMenu )
       listButtons = m_pVMGMenu->getButtons ( );
  ButtonObject *pButton = NULL;
  QList<ButtonObject *>::iterator it = listButtons.begin ( );		//oxx
  while ( it != listButtons.end ( ) )  {
    pButton = *it++;
    if  (  pButton->getTransition ( ) && pButton->getTransition ( )->bFinishedRendering )
      Render::Manager::unregisterTrans ( pButton->getTransition ( ) );
  }

  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )  {	//ox
    listButtons = m_listDVDMenus[t]->getButtons ( );
    it = listButtons.begin ( );
    while ( it != listButtons.end ( ) )  {
      pButton = *it++;
      if  (  pButton->getTransition ( ) && pButton->getTransition ( )->bFinishedRendering )
        Render::Manager::unregisterTrans ( pButton->getTransition ( ) );
    }
  }		//oooo

  return true;
}

void QDVDAuthor::engageCreateDVD ( bool bEngage )
{
  m_bCreateDVDAfterSlideshows = bEngage;
  if ( bEngage )  {
    QCursor myCursor ( Qt::WaitCursor );
    QApplication::setOverrideCursor ( myCursor );
  }
  else
    QApplication::restoreOverrideCursor ( );

  m_pToolbar->setEnabled         ( !bEngage );    //ooo
  //m_pMenuBar->setEnabled         ( !bEngage );    //ooo
  m_pColorToolbar->setEnabled    ( !bEngage );
  m_pSourceToolbar->setEnabled   ( !bEngage );
  m_pStructureToolbar->setEnabled( !bEngage );
  m_pTabWidgetMain->setEnabled   ( !bEngage );

  /*VMGMenu *pMenu = (VMGMenu *)getVMGMenu ( );
  if ( pMenu )  {
    QToolBar *pToolBar = pMenu->getToolBar ( );
    if ( pToolBar )
         pToolBar->setEnabled    ( !bEngage );
  }*/   //oooo
  m_pActionCreateDVD->setEnabled (     true );
}

UndoBuffer *QDVDAuthor::getUndoBuffer ( )
{   
  DVDMenu *pCurrentDVDMenu = getCurrentSubMenu ( );
  if ( pCurrentDVDMenu )
    return pCurrentDVDMenu->getUndoBuffer ( );
  return NULL;
}

QString QDVDAuthor::getTempFile (QString qsOrigFileName)
{
  Utils theUtil;
    return theUtil.getTempFile ( qsOrigFileName );
}

/*******************************************************************************************************
/tmp/ProjectName/+-> dragged Image files.
                 |
                 +->Movie 1/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie 2/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie 3/ --> clean.avi -=> Cleaned movie file or non-existant
                 +->Movie N/ --> clean.avi -=> Cleaned movie file or non-existant
                 |
                 +->Sub Menu 1_menu.mpg                       -=> final movie with sound and buttons.
                 +->Sub Menu 1/ +-> background/ +-> clip.wav
                 |              +-> background/ +-> clean.avi -=> cleaned up movie stream
                 |              +-> background/ +-> clip.avi  -=> Extracted timeframe of input-moviebackground
                 |              +-> background/ +-> menu.m2v  -=> final mpeg2enc menu movie (no sound)
                 |              +-> background/ +-> menu.mp2  -=> sound fileif neccesary
                 |              +-> background/ +-> menu.mpg  -=> final movie with sound
                 |              +-> background/ +-> menu.xml  -=> spumux - file
                 |              +-> background/ +-> -- Extracted images --
                 |              +-> background/ +-> rendered_-- Extracted images --
                 |              |
                 |              +-> Button 1/ +-> clip.wav
                 |              |             +-> clean.avi-=> cleaned up movie stream
                 |              |             +-> clip.avi -=> Extracted timeframe of [movie x]
                 |              |             +-> -- Extracted images -- 
                 |              +-> Button 2/ +-> clip.wav
                 |              |             +-> clip.avi -=> Extracted timeframe of [movie y]
                 |              |             +-> -- Extracted images -- 
                 |              +->MovieObject+-> clip.wav
                 |                            +-> clip.avi -=> Extracted timeframe of [movie z]
                 |                            +-> -- Extracted images -- 
                 |
                 +->Sub Menu 2_menu.mpg
                 +->Sub Menu 2/ +-> background/ +-> clip.wav
                                +-> background/ +-> clip.avi
                                +-> background/ +-> menu.m2v
                                +-> background/ +-> menu.mp2
                                +-> background/ +-> menu.mpg
                                +-> background/ +-> menu.xml
                                +-> background/ +-> -- Extracted images --
                                +-> background/ +-> rendered_-- Extracted images --
                                |
                                +-> Button 1/ +-> clip.wav
                                |             +-> clip.avi -=> Extracted timeframe of [movie a]
                                |             +-> -- Extracted images -- 
                                +-> Button 2/ +-> clip.wav
                                |             +-> clip.avi -=> Extracted timeframe of [movie b]
                                |             +-> -- Extracted images -- 
                                +-> Button 3/ +-> clip.wav
                                              +-> clip.avi -=> Extracted timeframe of [movie c]
                                              +-> -- Extracted images -- 
********************************************************************************/
void QDVDAuthor::createTempDirStructure ()
{  
  Utils theUtils;
  QString qsTempDir = Global::qsTempPath + QString ("/") +  Global::qsProjectName + QString ("/");
  QString qsCurrentDir;
  // we take this to check if the temp drive exists and if not, then we will create one ...
  QDir   tempDir ( qsTempDir );
  if ( ! tempDir.exists  ( ) )  {
    if ( ! theUtils.recMkdir ( qsTempDir ) )
      return;
  }
  // First we create a sub-directory for every SourceFileEntry in the project

  // Then we create a subdirectroy for each menu.
  DVDMenu *pMenu = m_pVMGMenu;
  int iMenuCounter = 0;

  //while (iMenuCounter <= (int)m_listDVDMenus.count())	{                   //ooo
  while (iMenuCounter <= static_cast<int> ( m_listDVDMenus.count() ) )	{    //xxx
    qsCurrentDir = qsTempDir + pMenu->name();
    tempDir.setPath (qsCurrentDir);
    if (!tempDir.exists())
      tempDir.mkdir (qsCurrentDir);
    // Inside each menu we create 
    // background
    qsCurrentDir = qsTempDir + pMenu->name() + QString ("/background");
    tempDir.setPath ( qsCurrentDir );
    if ( ! tempDir.exists ( ) )
      tempDir.mkdir ( qsCurrentDir );

    // one dir for each MovieObject (Note that Buttons / ObjectCollections can contain a MovieObject)
    QList <MovieObject *> listMovieClips = pMenu->getMovieObjects ();		//oxx
    for (uint t=0;t<(uint)listMovieClips.count();t++)	{	//ox
      qsCurrentDir = qsTempDir + pMenu->name() + QString ("/") + listMovieClips[t]->name();
      tempDir.setPath ( qsCurrentDir );
      if ( ! tempDir.exists ( ) )
        tempDir.mkdir  ( qsCurrentDir );
    }	//oooo
    // and finally we increase the menuCounter and get the next menu ...
    //if ( m_listDVDMenus.count ( ) > iMenuCounter++ )		//xxx
      pMenu = m_listDVDMenus[iMenuCounter++];		//ooo
      //pMenu = m_listDVDMenus[iMenuCounter];			//xxx  
  }  // end of loop ...
  // done.
}

void QDVDAuthor::createDVD ( )
{
  int t;
  // Note: All the logic has been put into the config file and is created in QDVDAuthorInit.
  if ( ! checkHealth ( ) )
    return;

  // First let us create the directory under the temp - drive
  QString qsBackgroundFileName;
  QString qsMenuName;
  // Ensure that the temp drive is present, otherwise create it.
  // Error message if not possible.
  QString qsTempPath = getTempFile (QString());
  QStringList stringList;

  int iMenuCounter = 0;
  DVDMenu *pMenu = m_pVMGMenu;

  QDVDAuthorInit *pInit = new QDVDAuthorInit (&m_dragNDropContainer);
  pInit->setHistory    (m_iMaxHistoryFiles, m_listHistory, m_listHistoryPix);
  pInit->setAutosave   (m_iAutosave);
  pInit->setStyle      (m_qsStyle);
  pInit->setToolsPaths (Global::listToolsPaths);
  // Here we go through all Menus (including VMGM) and generate the appropriate commands.
  pInit->appendPreProcessing ();

  for (t=0;t<sourceFileCount ();t++)	{
    SourceFileEntry *pEntry = sourceFileEntry(t);

    pInit->appendConvert (pEntry);
  }

  //while (iMenuCounter <= (int)m_listDVDMenus.count ( ) )  {   //ooo
  while (iMenuCounter <= static_cast<int> ( m_listDVDMenus.count ( ) ) )  {   //xxx 
    qsMenuName = getTempFile (  pMenu->name  ( ) );
      
    qsBackgroundFileName = getTempFile ( pMenu->name ( ) ) + QString ( "/" ) + QString ( BACKGROUND_NAME );
    // If we have a movie background we ought to get the name right ...
    if (!pMenu->getInterface ( )->qsMovieFileName.isEmpty ( ) )
      qsBackgroundFileName = pMenu->getInterface ( )->qsMovieFileName;
    if ( ! pMenu->isEmpty ( ) )
      pInit->appendMenu ( pMenu, qsMenuName, qsBackgroundFileName );	//ooooo
    //if ( ++iMenuCounter <= m_listDVDMenus.size ( ) )	//xxx
      pMenu = m_listDVDMenus[iMenuCounter++];		//ooo
      //pMenu = m_listDVDMenus[iMenuCounter - 1];		//xxx
  }	// end of loop ...

  pInit->appendPostProcessing ( );

  // In case the Dialog does already exist, we want to delete it before we use it again, to free memory.
  if (m_pDialogExecute)
    delete m_pDialogExecute;

  // And finally we call the CommandQueue - Dialog.
  m_pDialogExecute = new DialogExecute(pInit->getList());

  connect (m_pDialogExecute, SIGNAL (signalGenerateMasks())    , this, SLOT ( slotGenerateMasks()     ));
  connect (m_pDialogExecute, SIGNAL (signalRenderMenu(QString)), this, SLOT ( slotRenderMenu(QString) ));

  m_pDialogExecute->setInit(pInit);
  m_pDialogExecute->exec();
  // Please note that the pInit - object is now destroyed in the DialogExecute
}

void QDVDAuthor::slotRenderMenu (QString qsMenuName)
{
	// Called from DialogExecute::slotRenderMenu
	DVDMenu *pMenu = getSubMenuByName ( qsMenuName );
	if ( pMenu )  {
		// We can't just call pMenu->renderMenu() now because 
		// there could be a request already in the FIFO
		// So this call will check for that, and then call pMenu->renderMenu()
		StartMenuRender startMenuRender ( pMenu );
		// Note that the rendering is now done in the main thread OR 
		// we wait for the background thread to terminate
		startMenuRender.renderDVDMenu ( );
//		while (!pMenu->renderingDone ())
//			sleep (1);
	}	//oooo
}

void QDVDAuthor::returnFromRenderMenu ( )
{
	// This function is called when the DVDMenu is done rendering the menu to a movie file.
	// SInce this is a non-blocking operation, we can not handle this in 
	// one function (slotRenderMenu() but we have to have two functions here instead.
	if ( m_pDialogExecute )
		m_pDialogExecute->returnFromRenderMenu ( );
}

void QDVDAuthor::slotSetup()
{
  uint t;
  // Calling the setup dialog.
  DialogSetup *pDialog = new DialogSetup ( this );
  pDialog->setToolsPaths ( Global::listToolsPaths );
  
  if ( pDialog->exec ( ) == QDialog::Rejected )
    return;
  //pDialog->exec();
}

void QDVDAuthor::fileNew ( )
{
  //cleanUp   ( );
  slotCalculateSize ( );
  newWizard ( );
}

void QDVDAuthor::newWizard ( )
{
  //uiNewWizard *pWizard = new uiNewWizard (this);	//ooo
  NewWizard *pWizard = new NewWizard (this);		//xxx
  //pWizard->setVisible(true);
  //pWizard->show();
  //pWizard->exec();
  
  QFileInfo fileInfo (Global::qsProjectFileName);

  //pWizard->m_pEditProjectName->setText(Global::qsProjectName);	//fileInfo.baseName());		//ooo
  pWizard->dvdOutputPath->m_pEditProjectName->setText(Global::qsProjectName);
  //pWizard->m_pEditDVDPath    ->setText(Global::qsProjectPath);					//ooo
  pWizard->dvdOutputPath->m_pEditDVDPath    ->setText(Global::qsProjectPath);
  //pWizard->m_pEditTempPath   ->setText(Global::qsTempPath);						//ooo
  //pWizard->setField("tmpPath", Global::qsTempPath);
  //pWizard->setField("tmpPath", QString("/tmp"));
  //pWizard->tmpPath = Global::qsTempPath;
  pWizard->tempPath->m_pEditTempPath   ->setText(Global::qsTempPath);
  
  //pWizard->exec();
  
  if (pWizard->exec() == QDialog::Rejected)
  	return;

  //Global::qsProjectName = pWizard->m_pEditProjectName->text(); // QString ( PROJECT_FILENAME );	//ooo
  Global::qsProjectName = pWizard->projectName;
  //Global::qsProjectPath = pWizard->m_pEditDVDPath    ->text(); // QString ("");			//ooo
  Global::qsProjectPath = pWizard->dvdPath;
  //Global::qsTempPath    = pWizard->m_pEditTempPath   ->text(); // QString ("./");			//ooo
  Global::qsTempPath    = pWizard->tmpPath;
  addHistory (Global::qsProjectFileName);	//oooo
  delete pWizard;
}

bool QDVDAuthor::cleanUp ( )
{
  // here we close all Menus, unload the SourceFileEntries and re-set all other variables
  uint t;
  int  iReturn;
  // First we should check back if that is what the user wants ...
  if ( m_pVMGMenu )
       m_bModified |= m_pVMGMenu->getInterface ( )->bModified;
  if ( ! m_bModified )  {
    for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )	//ox
      m_bModified |=  m_listDVDMenus[t]->getInterface ( )->bModified;
  }
  if ( m_bModified )  {
    // Now we see if there is something to loose which might be worth saving ...
    iReturn = MessageBox::warning ( this, tr ( "File not saved" ),
      tr ( "The current modifications have not been save to the file. \n"
           "Do you want to save the project now ?"),
      QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel );
    if ( QMessageBox::Yes == iReturn )
         fileSave ( );
    else if ( QMessageBox::Cancel == iReturn )
         return false;
  }

  // Okay let us start with deleting the SubMenus ...
  for ( t=0; t<(uint)m_listDVDMenus.count ( ); t++ )
    delete m_listDVDMenus[t];
  m_listDVDMenus.clear ( );

  // Here we delete the Main VMGM - menu ...
  delete m_pVMGMenu;
  m_pVMGMenu = NULL;
  VMGMenu     *pVMGMenu = new  VMGMenu ( this );  
  //m_pTabWidgetMain->insertTab( 0, pVMGMenu, QString() );	//xxx
  //m_pTabWidgetMain->setCurrentIndex( 0 );			//xxx
  addDVDMenu ( pVMGMenu,  true );
  
  // Next on the list are the SourceFileEntries ...
  m_pSourceToolbar->clear ( );

  // And finally re-set the variables ...
  Global::qsProjectFileName = QString ( PROJECT_FILENAME );
  
  Global::qsCurrentPath = QString ("./");
  if ( m_pUndoStackDialog )
       m_pUndoStackDialog->setUndoBuffer ( NULL );	//oooo

  m_bModified = false;
  return true;
}

void QDVDAuthor::fileOpen ()
{ 
  // First let the user pick a new project to open.
  //QString qsProjectFileName = Q3FileDialog::getOpenFileName(Global::qsCurrentPath, tr("Project Files (*.xml *.XML)"), this, tr("Open Project file dialog"), tr ("Open Project File"));	//ooo
  QString qsProjectFileName = QFileDialog::getOpenFileName(this, tr("Open Project file dialog"), Global::qsCurrentPath, tr("Project Files (*.xml *.XML)"));					//xxx

  // Sanity check .. Did te user choose a new project file ?
  if (qsProjectFileName.isNull ())
    return;
  addHistory (qsProjectFileName);	//oooo
  fileOpen   (qsProjectFileName);
}

void QDVDAuthor::fileOpen ( QString qsProjectFileName )
{
  bool bError = false;
  // This function reads in a project file.
  if (!cleanUp())
    return;
  QFileInfo fileInfo  (qsProjectFileName);
  //QString qsPath = fileInfo.dirPath ( TRUE );	//ooo
  QString qsPath = fileInfo.path ( );		//xxx
  Global::qsCurrentPath = qsPath;

  // Assign the file
  QFile projectFile(qsProjectFileName);
  if (!projectFile.open(QIODevice::ReadWrite))
    return;
  // Try to get the right xml contents ...
  QDomDocument xmlDoc( XML_DOCTYPE );
  if (!xmlDoc.setContent (&projectFile))	{
    // Error handling ...
    projectFile.close();
    int iReturn = MessageBox::warning ( this, tr ("xml project file seems to be defective."),
      tr ("Do you want to try to load another project file ?"),
      QMessageBox::Yes, QMessageBox::No);
    if (iReturn == QMessageBox::Yes)
      fileOpen ();	// Okay, user wants to specify another project file.
    return;
  }

  // Here is the main loop to extract the info ...
  QDomElement docElem = xmlDoc.documentElement();
  QDomNode xmlNode = docElem.firstChild();	
  // First we take care of the QDVDAuthor variables ...
  float fLoadVersion = 1.0f;
  while( !xmlNode.isNull() ) {

    QDomElement searchXMLTree = xmlNode.toElement ( );
    if ( DVDAUTHOR == searchXMLTree.tagName ( ) ) {

      // set the fLoadVersion in this readProjectFile
      bError = ! readProjectFile ( searchXMLTree, fLoadVersion );
    }
    else if ( DVDAUTHOR_ROOT_VMGM == searchXMLTree.tagName ( ) )  {

      m_pVMGMenu->readProjectFile ( searchXMLTree, fLoadVersion );
      m_pVMGMenu->updateDVDMenu   ( false );
    }
    else if ( DVDAUTHOR_ROOT_MENU == searchXMLTree.tagName ( ) )  {
      DVDMenu *pDVDMenu = new DVDMenu(this);
      addDVDMenu ( pDVDMenu );
      pDVDMenu->readProjectFile ( searchXMLTree, fLoadVersion );
      pDVDMenu->updateDVDMenu   ( false );
    }
    else
      printf ("Warning: QDVDAuthor::fileOpen -=> wrong XML Node <%s>\nContinuing ...\n",
          //(const char *)searchXMLTree.tagName());	//ooo
          searchXMLTree.tagName().toLatin1().data());	//xxx

    if (bError)
      break;
    // Go to the next node ...
    xmlNode = xmlNode.nextSibling ( );
  }

  projectFile.close();
  if (bError)	{
    MessageBox::warning ( this, tr ("Error occured reading project file."),
      tr ("An unknown error occured when trying to read the project xml file."),
      QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // Setting of the variables ...
  m_bModified = false;
  Global::qsProjectFileName = qsProjectFileName;
  DVDMenu *pMenu = getCurrentSubMenu ( );

  /*if (pMenu)
    m_pColorToolbar->setDVDMenu ( pMenu );*/	//oooo

  // And finally we update the HistoryPixmap ...
  updateHistoryPix  ( );
  
  // Okay why should we have an outdated copy around ?
  slotAutosave      ( );

  slotCalculateSize ( );

  fileInfo.setFile  ( Global::qsProjectFileName );
  //setCaption ( QString ("'Q' DVD-Author :<%1> %2").arg( Global::qsProjectName ).arg( fileInfo.fileName ()) );		//oooo
  setWindowTitle ( QString ("'Q' DVD-Author :<%1> %2").arg( Global::qsProjectName ).arg( fileInfo.fileName ()) );	//xxx
  //m_pTabWidgetMain->setCurrentPage ( 0 );	//ooo
  m_pTabWidgetMain->setCurrentIndex ( 0 );	//xxx
  if ( m_pDVDLayoutGui )
    QTimer::singleShot ( 1500, m_pDVDLayoutGui, SLOT ( slotPostRead ( ) ) );	//oooo
}

bool QDVDAuthor::readProjectFile ( QDomNode &xmlNode, float &fLoadVersion )
{ 
  QString qsVersionNumber;

  QDomElement theElement = xmlNode.parentNode().toElement();
  QDomAttr attribute;

  attribute = theElement.attributeNode ( THE_PROJECT_NAME );
  if ( ! attribute.isNull ( ) )
    Global::qsProjectName = attribute.value ( );

  attribute = theElement.attributeNode ( DVDAUTHOR_ZONE );
  if ( ! attribute.isNull ( ) )
    Global::iRegionalZone = attribute.value ( ).toInt ( );
  //setRegionalZone ( );	//oooo

  // Here we get the DVDAuthor Nodes ...
  QDomNode xmlDVDAuthor = xmlNode.firstChild ( );
  while ( !xmlDVDAuthor.isNull ( ) )	{
    // Okay, here we retain the stored data from the xml file.
    theElement = xmlDVDAuthor.toElement   ( );
    QString tagName  = theElement.tagName ( );
    QString nodeText = theElement.text    ( );
    // Okay, this is ugly but after all it is the simplest of all xml file structure.
    // No need to get fancy ...
    if (tagName == QDVDAUTHOR_VERSION) {
      bool bOkay = true;
      qsVersionNumber = nodeText;
      fLoadVersion = qsVersionNumber.toFloat ( &bOkay );
      if ( ! bOkay )
        fLoadVersion = 1.0f;
    }
    else if (tagName == DVDAUTHOR_TEMP_PATH)
      Global::qsTempPath = nodeText;
    else if (tagName == DVDAUTHOR_PROJECT_PATH)
      Global::qsProjectPath = nodeText;
    else if (tagName == DVDAUTHOR_CURRENT_PATH)
      Global::qsCurrentPath = nodeText;
    else if (tagName == SOURCE_OBJECT)	{
      SourceFileEntry *pNewEntry = new SourceFileEntry;
      pNewEntry->readProjectFile ( xmlDVDAuthor );
      m_pSourceToolbar->appendSourceFileEntry ( pNewEntry );		//oooo
    }
    else if (tagName == SOURCE_OBJECT_SUBTITLES)	{
      /*if ( m_pSubtitlesGui ) {
	Subtitles *pSubtitles = m_pSubtitlesGui->getSubtitles ( true );
	if ( pSubtitles )
	     pSubtitles->readProjectFile    ( xmlDVDAuthor );
	// And lets create the table for these subtitles ...
	m_pSubtitlesGui->createTableEntries ( );
      }*/	//oooo
    }
    else if ( tagName == DVDAUTHOR_DVDLAYOUT )  {
      if ( m_pDVDLayoutGui )
           m_pDVDLayoutGui->readProjectFile ( xmlDVDAuthor );		//oooo
    }
    else
      printf ("Warning: QDVDAuthor::readProjectFile -=> wrong XML Node <%s>\nContinuing ...\n",
	      //(const char *)tagName);		//ooo
	      tagName.toLatin1().data());	//xxx
    // So lets get the next sibling ... until we hit the end of DVDMenu ...
    xmlDVDAuthor = xmlDVDAuthor.nextSibling();
  }
  return true;
}

bool QDVDAuthor::writeProjectFile ( QDomElement &rootElement )
{ 
	QDomDocument xmlDoc = rootElement.ownerDocument();
	
	if ( ! Global::qsProjectName.isNull ( ) )
		rootElement.setAttribute ( THE_PROJECT_NAME, Global::qsProjectName );

	if ( Global::iRegionalZone != 0 )
		rootElement.setAttribute ( DVDAUTHOR_ZONE, QString ( "%1" ).arg ( Global::iRegionalZone ) );

	QDomElement rootDVDAuthor = xmlDoc.createElement( DVDAUTHOR );
	rootElement.appendChild ( rootDVDAuthor );
	// Okay, the main Node is created, now all DVDAuthor related information belon under it.
	QDomElement tag = xmlDoc.createElement( QDVDAUTHOR_VERSION );
	rootDVDAuthor.appendChild ( tag );
	QDomText text = xmlDoc.createTextNode ( QDVDAUTHOR_VERSION_NUMBER );
	tag.appendChild ( text );

	tag = xmlDoc.createElement   ( DVDAUTHOR_CURRENT_PATH );
	rootDVDAuthor.appendChild    ( tag );
	text = xmlDoc.createTextNode ( Global::qsCurrentPath );
	tag.appendChild ( text );

	tag = xmlDoc.createElement   ( DVDAUTHOR_TEMP_PATH );
	rootDVDAuthor.appendChild    ( tag );
	text = xmlDoc.createTextNode ( Global::qsTempPath );
	tag.appendChild ( text );

	tag = xmlDoc.createElement   ( DVDAUTHOR_PROJECT_PATH );
	rootDVDAuthor.appendChild    ( tag );
	text = xmlDoc.createTextNode ( Global::qsProjectPath );
	tag.appendChild ( text );

/*	if ( m_pSubtitlesGui ) {
	  Subtitles *pSubtitles = m_pSubtitlesGui->getSubtitles ();
	  if ( pSubtitles ) 
	       pSubtitles->writeProjectFile ( rootDVDAuthor );
	}
*/	//oooo
	if ( m_pDVDLayoutGui )
	     m_pDVDLayoutGui->writeProjectFile ( rootDVDAuthor );

	// Here we write the SourceFileEntry - records ...
	int t;
	for (t=0;t<m_pSourceToolbar->sourceFileCount();t++)	
		m_pSourceToolbar->sourceFileEntry(t)->writeProjectFile ( rootDVDAuthor, m_bAutosaving );

	return true;
}

void QDVDAuthor::fileSave ( )
{
  // Here we check if the user is still using the default filename ...
  //if ( Global::qsProjectFileName.find ( PROJECT_FILENAME ) > -1 )	//ooo
  if ( Global::qsProjectFileName.indexOf ( PROJECT_FILENAME ) > -1 )	//xxx
    return fileSaveAs ( );
  // first we save the current project
  fileSave ( Global::qsProjectFileName );
  // Then we save the same info again as autosave, so that in case of a crash we are up to date.
  slotAutosave ();
  // Now we handle the preview - pixmap of the Recent drop down menu ...
  updateHistoryPix();
  m_bModified = false;
}

void QDVDAuthor::fileSave ( QString qsFileName, bool bSaveAs )
{
  m_bWait = true;

  uint t;
  QFileInfo nameInfo ( qsFileName );
  //if ( bSaveAs && ( nameInfo.exists ( ) ) && ( qsFileName.find ( AUTOSAVE_FILE_NAME ) == -1 ) ) {	//ooo
  if ( bSaveAs && ( nameInfo.exists ( ) ) && ( qsFileName.indexOf ( AUTOSAVE_FILE_NAME ) == -1 ) ) {	//xxx
    if ( MessageBox::information ( this, tr ( "File exists." ),
         tr ( "Attention, the file exists already.\nDo you want to overwrite this file ?" ),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No )  {
      m_bWait = false;
      return;
    }
  }

  QFile projectFile ( qsFileName );
  if (  projectFile.open ( QIODevice::WriteOnly ) )  {

    QFileInfo fileInfo ( qsFileName );
    QFileInfo autoInfo ( AUTOSAVE_FILE_NAME );

    if ( fileInfo.fileName ( ) != autoInfo.fileName ( ) )
      //setCaption ( QString ( "'Q' DVD-Author :<%1> %2"  ).arg ( Global::qsProjectName ).arg( fileInfo.fileName ( ) ) );	//oooo
      setWindowTitle ( QString ( "'Q' DVD-Author :<%1> %2"  ).arg ( Global::qsProjectName ).arg( fileInfo.fileName ( ) ) );	//xxx

    QDomDocument xmlDoc( XML_DOCTYPE );
    QDomElement mainElement = xmlDoc.createElement( DVD_PROJECT );
    xmlDoc.appendChild ( mainElement );
    // First we store the data from the QDVDAuthor - class
    writeProjectFile   ( mainElement );

    // next in line is the VMGMenu - class
    if ( m_pVMGMenu )
         m_pVMGMenu->writeProjectFile (mainElement);

    // And lastly we store the infos in all SubMenus ...
    for (t=0;t<(uint)m_listDVDMenus.count();t++)
      m_listDVDMenus[t]->writeProjectFile(mainElement); 
    QString xml = xmlDoc.toString();
    //xml.replace ( ">", "&gt;" );
    //printf ("%s\n", (const char *)xml);
    //
    // Write the file as a stream 
    //
    QTextStream ofs ( &projectFile );
    //ofs.setEncoding( QTextStream::UnicodeUTF8 ); 
    //ofs.setEncoding ( QTextStream::Latin1 );	//oooo
    // ^^^ Dat: not QTextStream::UnicodeUTF8, because xmlDoc.toCString()
    //     already emits proper UTF-8, which we keep intact.

    // header is needed to support special characters 
    //ofs << "<?xml version=\"1.0\" encoding=\""  
    //    << ofs.codec()->name()  
    //    << "\"?>\n"; 
    //ofs << xmlDoc.toString().ascii(); 
    ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    //ofs << xmlDoc.toCString ( );		//ooo
    //ofs << xmlDoc.toByteArray ( );		//xxx
    ofs << xmlDoc.toString().toLatin1();	//xxx

    projectFile.close ( );
  }
  m_bWait = false;
}

void QDVDAuthor::fileSaveAs ( )
{
  QString qsStarting = Global::qsCurrentPath;
  QFileInfo fileInfo ( qsStarting );
  //qsStarting = fileInfo.dirPath ( true );	//ooo
  qsStarting = fileInfo.path ( );		//xxx
  //if ( Global::qsProjectFileName.find ( PROJECT_FILENAME ) > -1 )		//ooo
  if ( Global::qsProjectFileName.indexOf ( PROJECT_FILENAME ) > -1 )		//xxx
    qsStarting = Global::qsCurrentPath + "/" + Global::qsProjectName + ".xml";
  //else if ( Global::qsProjectFileName.find ( AUTOSAVE_FILE_NAME ) == -1 )	//ooo
  else if ( Global::qsProjectFileName.indexOf ( AUTOSAVE_FILE_NAME ) == -1 )	//xxx
    qsStarting = Global::qsProjectFileName;

  //QString qsProjectFileName = QFileDialog::getSaveFileName(qsStarting, tr ("Project Files (*.xml)"), this, tr("Save file dialog"), tr("Save File As ..."));	//oooo
  //QFileDialog ( this, QString("Save file dialog"), qsStarting, QString("Project Files (*.xml)") );								//xxx
  QString qsProjectFileName = QFileDialog::getSaveFileName(this, tr("Save file dialog"), qsStarting, tr("Project Files (*.xml)"));				//xxx

  if ( qsProjectFileName.isNull ( ) )
    return;

  fileInfo.setFile ( qsProjectFileName );
  //QString qsPath = fileInfo.dirPath ( TRUE );		//ooo
  QString qsPath = fileInfo.path ( );			//xxx
  Global::qsCurrentPath = qsPath;

  // Here we check if the user wants to use Unnamed.xml -the default-
  // In this case we change the name slightly, so we have a clue it has bee initialized.
  // And will not ask to SaveAs again.
  if ( fileInfo.fileName ( ) == QString ( PROJECT_FILENAME ) )
       qsProjectFileName  = qsPath + QString ( "/unnamed.xml" );

  Global::qsProjectFileName = qsProjectFileName;
  m_bModified = true;

  fileInfo = QFileInfo ( qsProjectFileName );
  //QString qsExtension = fileInfo.extension ( FALSE );		//ooo
  QString qsExtension = fileInfo.suffix ( );			//xxx
  if (  ( qsExtension.isEmpty ( ) ) ||  // Indication there is no extension ...
        //( qsExtension.lower   ( ) != "xml" ) )	//ooo
        ( qsExtension.toLower   ( ) != "xml" ) )	//xxx
    Global::qsProjectFileName += QString (".xml");

  //  fileSave ( );
  // first we save the current project
  fileSave ( Global::qsProjectFileName, true );
  // Then we save the same info again as autosave, so that in case of a crash we are up to date.
  slotAutosave ( );
  // Now we handle the preview - pixmap of the Recent drop down menu ...
  updateHistoryPix ( );
  m_bModified = false;
}

QString QDVDAuthor::getImageFilter ( )
{
  return QString ( "*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP" );
}

QString QDVDAuthor::getVidFilter ( )
{
  // This is for short vids from the Camera ( used in DialogImages )
  return QString ( "*.avi *.mov *.mpg *.mpeg *.mp4 *.AVI *.MOV *.MPG *.MPEG *.MP4" );
}

QString QDVDAuthor::getMovieFilter ( )
{
  return QString ( "*.mpg *.mpeg *.mpeg2 *.avi *.mwa *.mov *.wma *.mp4 *.m4v *.ogv *.vro *.vob *.m2v *.mpv *.dv *.wmv *.mjpeg *.mve *.asf *.asx *.flv *.MPG *.MPEG *.MPEG2 *.AVI *.MWA *.MOV *.WMA *.MP4 *.M4V *.OGV *.VRO *.VOB *.M2V *.MPV *.DV *.WMV *.MJPEG *.MVE *.ASF *.ASX *.FLV" );
}

QString QDVDAuthor::getSoundFilter ( )
{
  return QString ( "*.wav *.mp3 *.mp2 *.mpa *.mpega  *.ogg *.ac3 *.m2a *.WAV *.MP3 *.MP2 *.MPA *.MPEGA *.OGG *.AC3 *.M2A" );
}
