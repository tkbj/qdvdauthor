/***************************************************************************
    dialogsetup.cpp
                             -------------------
    Form implementation generated from reading ui file 'uidialogsetup.ui'
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Here we handle the DialogSetup, which is called from 
   Tools->Setup

   This dialog also handles the scaning of the system for all neccesary tools
   -> The neccesary tools are defined in global.h
    
****************************************************************************/

#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QTreeWidgetItem>
#include <QTimer>

#include "global.h"
#include "qdvdauthorinit.h"
#include "dialogman.h"
#include "dialogsetup.h"
#include "messagebox.h"
#include "render_client.h"
#include "qplayer/mediacreator.h"

#define COLOR_ME_RED    255,  50,  50
#define COLOR_ME_ORANGE 255, 150,  50
#define COLOR_ME_WHITE  255, 250, 250

DialogSetup::DialogSetup ( QWidget *pParent )
  : QDialog ( pParent )
{
  setupUi(this);
  
  m_bModifiedToolsPaths = false;
  m_pScrollFrame        = NULL;
  m_pScrollView         = NULL;
  m_pDNDContainer       = NULL;
  m_pTimer              = NULL;
  m_iMaxHistoryFiles    = 10;
  m_qsStyle             = QString ("Default");
  
  uint t = 0;
  QStringList listEngines = MediaCreator::getAvailableEngines ();
  for (t=0;t<(uint)listEngines.count();t++)
    //comboPreferredEngine->insertItem (listEngines[t]);	//ooo
    comboPreferredEngine->addItem (listEngines[t]);		//xxx
  comboPreferredEngine->setCurrentText (MediaCreator::preferredEngine());
  m_pTimeEditDuration->setTime ( QTime ( 0, 0, 1 ) );
  
  initExternalPlayerList ();
  m_pSpinMaxThreads->setValue (Global::iMaxRunningThreads);
  m_pCheckDeleteOlder->setChecked (true);
  m_pEditOlderThan->setText (QString ("%1").arg(Global::iDeleteTempOlderThan));
  if ( ! Global::bPNGRender )
    //m_pComboPNGRender->setCurrentItem ( 1 );	//ooo
    m_pComboPNGRender->setCurrentIndex ( 1 );	//xxx
    
  if (Global::iDeleteTempOlderThan < 0)	{
    m_pCheckDeleteOlder->setChecked (false);
    m_pEditOlderThan->setText (QString ("%1").arg(-Global::iDeleteTempOlderThan));
    m_pEditOlderThan->setEnabled (false);
  }
  
  initSlideshowTab ( );
  
  //connect (m_pButtonAdminTemp  , SIGNAL (clicked()), this, SLOT(slotAdminTemp()));
  connect (m_pButtonScan       , SIGNAL (clicked()), this, SLOT(slotScanSystem()));
  connect (m_pButtonBrowseProjectPath, SIGNAL (clicked()), this, SLOT(slotBrowseProjectPath()));
  //connect (tabWidget, SIGNAL (currentChanged (QWidget*)), this, SLOT(slotTabChanged(QWidget *)));
  connect (comboPreferredEngine, SIGNAL (activated(const QString &)), this, SLOT (slotPreferredEngineChanged(const QString &)));

  //connect (m_pSpinAutosave     , SIGNAL (valueChanged(int)), this, SLOT(slotAutosaveChanged(int)));
  //connect (m_pSpinMaxHistory   , SIGNAL (valueChanged(int)), this, SLOT(slotMaxHistoryChanged(int)));
  //connect (m_pButtonReset      , SIGNAL (clicked()), this, SLOT(slotResetHistory()));
  connect (m_pButtonBrowseTemp , SIGNAL (clicked()), this, SLOT(slotBrowseTemp()));
  //connect (m_pButtonSetupEngine, SIGNAL (clicked()), this, SLOT(slotSetupEngine()));
  //connect (m_pButtonEngineInfo , SIGNAL (clicked()), this, SLOT(slotEngineInfo()));
  //connect (m_pButtonFont       , SIGNAL (clicked()), this, SLOT(slotFont ()));
  //connect (m_pButtonColor      , SIGNAL (clicked()), this, SLOT(slotColor()));

  // Drag'n Drop tab-paged
  /*connect (m_pRadioImageButton, SIGNAL (stateChanged(int)), this, SLOT (slotImageButton(int)));
  connect (m_pRadioMovieButton, SIGNAL (stateChanged(int)), this, SLOT (slotMovieButton(int)));
  connect (m_pCheckWithText   , SIGNAL (toggled(bool))    , this, SLOT (slotWithText   (bool)));
  connect (m_pCheckDeleteOlder, SIGNAL (toggled(bool))    , this, SLOT (slotDeleteOlder(bool)));
  */	//oooo
  
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(accept()));		//xxx
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));		//xxx
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));	//xxx
}

DialogSetup::~DialogSetup()
{
  
}

void DialogSetup::initExternalPlayerList ()
{
  // Okay, now we should try to look for the following players ...
  uint t;
  char arrayPlayers[][14] = {"qplayer", "vlc", "xine", "mplayer", "kaffeine", "kmplayer", "kplayer", "ogle", "helix", "totem", "sinek", "okle", "xfmedia", "gtkplayer"};
  Utils theUtils;
  QString qsPlayer;
  for (t=0;t<14;t++)	{
    qsPlayer = theUtils.checkForExe (arrayPlayers[t]);
    if (!qsPlayer.isEmpty())
      //m_pComboExternalPlayer->insertItem (qsPlayer);	//ooo
      m_pComboExternalPlayer->addItem (qsPlayer);		//xxx
  }
  m_pComboExternalPlayer->setCurrentText (Global::qsExternalPlayer);
}

void DialogSetup::initSlideshowTab ( )
{
  m_slideDefaults = *Global::pSlideDefaults;

  m_pSpinImageDelay->setValue  ( (int)m_slideDefaults.imageDelay  ( ) );
  m_pSpinFilterDelay->setValue ( (int)m_slideDefaults.filterDelay ( ) );

  m_pRadioImgImg->setChecked   ( ! (bool)m_slideDefaults.transitionType ( ) );
  m_pRadioImgBkgImg->setChecked(   (bool)m_slideDefaults.transitionType ( ) );

  //m_pComboFormat->setCurrentItem ( m_slideDefaults.formatType ( ) );  //ooo
  m_pComboFormat->setCurrentIndex ( m_slideDefaults.formatType ( ) );   //xxx
  m_pCheckIntroPage->setChecked  ( m_slideDefaults.introSlide ( ) );
  m_pCheckKenBurns->setChecked   ( m_slideDefaults.kenBurns   ( ) );

  //m_pListBackground->insertStringList ( m_slideDefaults.backgroundList ( ) ); //ooo
  m_pListBackground->insertItems ( 0, m_slideDefaults.backgroundList ( ) );   //xxx
  //m_pListAudio->insertStringList      ( m_slideDefaults.audioList      ( ) ); //ooo
  m_pListAudio->insertItems      ( 0, m_slideDefaults.audioList      ( ) ); //xxx

  initTransitions ( );

  connect ( m_pButtonAddImage,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAddBackgroundImg ( ) ) );
  connect ( m_pButtonColorBackround, SIGNAL ( clicked ( ) ), this, SLOT ( slotAddBackgroundClr ( ) ) );
  connect ( m_pButtonDelBackground,  SIGNAL ( clicked ( ) ), this, SLOT ( slotDelBackground    ( ) ) );
  connect ( m_pButtonAddAudio,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAddAudio         ( ) ) );
  connect ( m_pButtonDelAudio,       SIGNAL ( clicked ( ) ), this, SLOT ( slotDelAudio         ( ) ) );
  connect ( m_pButtonSelectTrans,    SIGNAL ( clicked ( ) ), this, SLOT ( slotSelectTrans      ( ) ) );
  connect ( m_pButtonAllTrans,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAllTrans         ( ) ) );
  connect ( m_pButtonClearTrans,     SIGNAL ( clicked ( ) ), this, SLOT ( slotClearTrans       ( ) ) );
  connect ( m_pButtonAnim,           SIGNAL ( clicked ( ) ), this, SLOT ( slotAnimTrans        ( ) ) );
  //connect ( m_pListViewTransitions,  SIGNAL ( selectionChanged ( Q3ListViewItem * ) ), this, SLOT ( slotSelectionChanged ( Q3ListViewItem * ) ) );    //ooo
  connect ( m_pListViewTransitions,  SIGNAL ( itemChanged ( QTreeWidgetItem *, int ) ), this, SLOT ( slotSelectionChanged ( QTreeWidgetItem *, int ) ) );   //xxx
}

void DialogSetup::storeSlideshowDefaults ( )
{
  m_slideDefaults.setImageDelay     ( (float)m_pSpinImageDelay->value   ( ) );
  m_slideDefaults.setFilterDelay    ( (float)m_pSpinFilterDelay->value  ( ) );
  m_slideDefaults.setTransitionType ( (int)m_pRadioImgBkgImg->isChecked ( ) );
  //m_slideDefaults.setFormatType     ( m_pComboFormat->currentItem       ( ) );    //ooo
  m_slideDefaults.setFormatType     ( m_pComboFormat->currentIndex       ( ) );      //xxx
  m_slideDefaults.setIntroSlide     ( m_pCheckIntroPage->isChecked      ( ) );
  m_slideDefaults.setKenBurns       ( m_pCheckKenBurns->isChecked       ( ) );

  QStringList list;
  //Q3ListBoxItem *pItem = m_pListBackground->firstItem ( );    //ooo
  QListWidgetItem *pItem = m_pListBackground->item ( 0 );    //xxx
  while ( pItem )  {
    list.append ( pItem->text ( ) );
    //pItem = pItem->next ( );  //ooo
    pItem = pItem->listWidget ( )->item ( pItem->listWidget ( )->row ( pItem ) + 1 );  //xxx
  }
  m_slideDefaults.setBackgroundList ( list );
  list.clear ( );

  //pItem = m_pListAudio->firstItem ( );    //ooo
  pItem = m_pListAudio->item ( 0 ); //xxx
  while ( pItem )  {
    list.append ( pItem->text ( ) );
    //pItem = pItem->next ( );  //ooo
    pItem = pItem->listWidget ( )->item ( pItem->listWidget ( )->row ( pItem ) + 1 );  //xxx
  }
  m_slideDefaults.setAudioList ( list );
  list.clear ( );

  //Q3CheckListItem *pLVItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( );   //ooo
  QTreeWidgetItem *pLVItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );     //xxx
  while ( pLVItem )  {
    //if  ( pLVItem->isOn ( ) ) //ooo
    if  ( pLVItem->isSelected ( ) ) //xxx
      list.append (  pLVItem->text ( 0 ) );
    //pLVItem = (Q3CheckListItem *)pLVItem->nextSibling  ( );   //ooo
    pLVItem = (QTreeWidgetItem *)pLVItem->parent()->child(pLVItem->parent()->indexOfChild(pLVItem)+1);     //xxx
  }
  m_slideDefaults.setTransitionList ( list );

  *Global::pSlideDefaults = m_slideDefaults;
}

void DialogSetup::initTransitions ( )
{
  // Here we add the system transitions plus 
  // load the available transitions from the system folder

  // Check if we need to create a new object ...
  if ( Render::Manager::m_pSelf == NULL )
       Render::Manager::m_pSelf = new Render::Manager;

  int t;
  for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
    m_arrayPix[t] = new QPixmap;
  }
  m_iCurrentPix = 0;
  m_pTimer      = NULL;
  m_bAnimated   = false;

  // Disable sorting.
  //m_pListViewTransitions->setSorting ( -1 );  //ooo
  m_pListViewTransitions->setSortingEnabled ( false );  //xxx

  // We need to also init the selected transitions for a Slideshow
  QString qsFilter;
  QStringList::iterator itFilter;
  //Q3CheckListItem    *pItem = NULL;   //ooo
  QTreeWidgetItem    *pItem = NULL;     //xxx
  QStringList         list = Render::Manager::m_pSelf->getFilterNames ( );
  QStringList::iterator it = list.end ( );
  bool bChecked;

  if ( list.count ( ) < 5 )  {
    Utils theUtils;
    QString qsQRender = theUtils.getToolPath ( "qrender");
    if ( qsQRender == "qrender" )  // qrender path not found
      qsQRender = tr ( "Could not find 'qrender'. Please make sure that 'qrender is in $PATH and executable.\n\n" );
    else
      qsQRender = "";
    QString qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
    QString qsText = qsQRender;
    qsText += tr ( "Please check if you have all transitions installed in " );
    qsText += tr ( "\n%1\n\nYou can donload them from:\nhttp://qdvdauthor.sourceforge.net/data/alpha_trans.tar.bz2\n\n" ).arg ( qsTransitionPath );
    qsText += tr ( "To install do the following:\n" );

    qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/" );
    qsText += tr ( "cd /tmp; wget http://qdvdauthor.sourceforge.net/data/alpha_trans.tar.bz2\n" );
    qsText += tr ( "cd %1; sudo tar -xjf /tmp/alpha_trans.tar.bz2\n\n" ).arg ( qsTransitionPath );

    QSize theSize    ( 600, 300 );
    MessageBox::html ( this, tr ( "Warning" ), qsText, theSize );
  }

  QPixmap pix;
  //QImage  img ( QImage::fromMimeSource ( "please_wait.jpg" ) );			//ooo
  QImage  img ( QImage ( ":/images/please_wait.jpg" ) );				//xxx
  //img  =  img.smoothScale ( 24, 24 );							//ooo
  img  =  img.scaled ( 24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  pix.convertFromImage  ( img );
  do  {
    qsFilter = *(--it);
    //pItem = new Q3CheckListItem ( m_pListViewTransitions, qsFilter, Q3CheckListItem::CheckBoxController );    //ooo
    pItem = new QTreeWidgetItem ( m_pListViewTransitions, QStringList( qsFilter ) );  //xxx
    //pItem->setPixmap ( 0, pix );  //ooo
    pItem->setIcon ( 0, pix );  //xxx
    bChecked = false;
    if ( m_slideDefaults.transitionList ( ).count ( ) > 0 )  {
      itFilter =   m_slideDefaults.transitionList ( ).begin ( );
      while (  itFilter   != m_slideDefaults.transitionList ( ).end ( ) )  {
        if  ( *itFilter++ == *it )  {
          bChecked = true;
          break;
        }
      }
    }
    else
      bChecked  =  true;
    //pItem->setOn ( bChecked );    //ooo
    pItem->setSelected ( bChecked ); //xxx
  } while ( it != list.begin ( ) );

  QTimer::singleShot ( 1000, this, SLOT ( slotCreateListViewPix ( ) ) );
}

void DialogSetup::slotPreferredEngineChanged(const QString &qsNewText)
{
	MediaCreator::setPreferredEngine (qsNewText);
}

void DialogSetup::slotBrowseProjectPath()
{
   // This function will browse for the directory where the DVD is to be build.
   //QString qsPathName = Q3FileDialog::getExistingDirectory ( m_pEditProjectPath->text() );	//ooo
   QString qsPathName = QFileDialog::getExistingDirectory ( this, tr("Find Directory"), m_pEditProjectPath->text(), QFileDialog::ShowDirsOnly );
   if  ( (qsPathName.isEmpty()) || (qsPathName.isNull()) )
      return;
   m_pEditProjectPath->setText(qsPathName);
}

void DialogSetup::setToolsPaths(QList<Utils::toolsPaths *> &listToolsPaths)
{
  uint t, i, iIndex;
  bool bFoundLostTool;
  QList<uint> listOfTheLostTools;
  QList<QString> listOfTheFoundTools;
  Utils theUtils;
  // Here we create the ScrollView ... Note this is the only function which creates these widgets.
  QFrame *pTempFrame = m_pScrollFrame;
  // The first check is to see if the list has any values ...
  QCursor myCursor (Qt::WaitCursor);
  setCursor (myCursor);
  if (listToolsPaths.count() == 0)
    return slotScanSystem();	// If not, then slotScanSystem will call this function here.

  // If the scrollview does exist then we can delete it now and re-create it.
  // Note: This will also delete all objects inside the ScrollView.
  if (!m_pScrollView)	{
    //m_pScrollView = new Q3ScrollView (Widget3, "SetupScrollView");	//ooo
    m_pScrollView = new QScrollArea ( Widget3 );				//xxx
    m_pScrollLayout->addWidget ( m_pScrollView );
    //m_pScrollView->setVScrollBarMode ( Q3ScrollView::AlwaysOn );		//oooo
    //m_pScrollView->setVScrollBarMode ( QScrollView::AlwaysOn );		//xxx
    m_pScrollView->setWidgetResizable( true );				//xxx
  }
  // And this Frame will hold all neccesarywidgets ...
  //m_pScrollFrame = new Q3Frame( m_pScrollView->viewport (), "m_pScrollFrame" );	//ooo
  //m_pScrollFrame = new QFrame( m_pScrollView->widget () );				//xxx
  m_pScrollFrame = new QFrame( m_pScrollView->viewport () );				//xxx
  //m_pScrollFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_pScrollFrame->sizePolicy().hasHeightForWidth() ) );	//ooo
  //m_pScrollFrame->setPaletteBackgroundColor( QColor( 177, 211, 238 ) );		//ooo
  m_pScrollFrame->setFrameShape( QFrame::StyledPanel );
  m_pScrollFrame->setFrameShadow( QFrame::Sunken );
  m_pScrollFrame->setMinimumWidth ( m_pLabelHeader->width() - 20 );	//ooo

  //Q3HBoxLayout *pHLayoutMain = new Q3HBoxLayout ( m_pScrollFrame ); // 0, 0, 6, "HLayout" );	//ooo
  QHBoxLayout *pHLayoutMain = new QHBoxLayout ( m_pScrollFrame );					//xxx
  //Q3VBoxLayout *pVLayout1 = new Q3VBoxLayout ( NULL, 4, 6, "VLayout1" );			//ooo
  QVBoxLayout *pVLayout1 = new QVBoxLayout (  );							//xxx
  //Q3VBoxLayout *pVLayout2 = new Q3VBoxLayout ( NULL, 4, 6, "VLayout2" );			//ooo
  QVBoxLayout *pVLayout2 = new QVBoxLayout (  );							//xxx

  m_listButtons.clear();
  m_listManButtons.clear();
  // Now we build in some complexity ...
  // In order to have all tools top first which were NOT found, we have a dry - run first in which we create the list 
  // of order to display the tools plus found / not-found flags for background coloring.
  // Also we create a global translation list to get from the original order to the new order.
  m_listIndex.clear ();
  for (t=0;t<(uint)listToolsPaths.count();t++)	{
    if ( !listToolsPaths[t]->bPresent )	{
      listOfTheLostTools.append (t);
      listOfTheFoundTools.append ( listToolsPaths[t]->qsExecutableName);
    }
    else
      listOfTheFoundTools.append ( listToolsPaths[t]->qsFullPath );
  }
  // Now we create the index-list. First all tools not found ...
  m_listIndex = listOfTheLostTools;
  for (t=0;t<(uint)listToolsPaths.count();t++)	{
    bFoundLostTool = false;
    // Here we add the remainder ...
    for (i=0;i<(uint)listOfTheLostTools.count();i++)	{
      if (t == listOfTheLostTools[i])	{
        bFoundLostTool = true;
        // And here we exit this inner loops since we found what we were looking for.
        i = listOfTheLostTools.count();
      }
    }
    if (!bFoundLostTool)
      m_listIndex.append(t);
  }

  m_listEditToolsPaths.clear();
  m_listTools.clear();
  // At this point I have the information I need to create the GUI elements.
  for (t=0;t<(uint)listToolsPaths.count();t++)	{
    iIndex = m_listIndex[t];
    //QLabel *pToolLabel = new QLabel( m_pScrollFrame, QString("pToolLabel%1").arg(iIndex) );	//ooo
    QLabel *pToolLabel = new QLabel( QString("pToolLabel%1").arg(iIndex), m_pScrollFrame );		//xxx
    pVLayout1->addWidget( pToolLabel );
    pToolLabel->setText (theUtils.getToolsDisplayName(listToolsPaths[iIndex]->qsExecutableName));

    QHBoxLayout *pHLayout = new QHBoxLayout(  );

    //QLineEdit *pEditToolsPath = new QLineEdit( m_pScrollFrame, QString("pEditToolsPath%1").arg(iIndex) );		//ooo
    QLineEdit *pEditToolsPath = new QLineEdit( QString("pEditToolsPath%1").arg(iIndex), m_pScrollFrame );		//xxx
    m_listEditToolsPaths.append (pEditToolsPath);
    pHLayout->addWidget( pEditToolsPath );
    if (t<(uint)listOfTheLostTools.count())	{
      if (theUtils.isMandatory(iIndex)) {
        //pEditToolsPath->setBackgroundColor(QColor (COLOR_ME_RED));			//ooo
        //pEditToolsPath->setBackgroundRole(QPalette::Window);				//xxx
        QPalette palette;								//xxx
        palette.setBrush(pEditToolsPath->backgroundRole(), QColor (COLOR_ME_RED));	//xxx
	pEditToolsPath->setPalette(palette);						//xxx
      } else {
        //pEditToolsPath->setBackgroundColor(QColor (COLOR_ME_ORANGE));	//ooo
        //pEditToolsPath->setBackgroundRole(QPalette::Window);		//xxx
        QPalette palette;								//xxx
        palette.setBrush(pEditToolsPath->backgroundRole(), QColor (COLOR_ME_ORANGE));	//xxx
	pEditToolsPath->setPalette(palette);						//xxx
      }
    }
//		pEditToolsPath->setText( toolsArray[iIndex].pExecutableName );
    pEditToolsPath->setText( listOfTheFoundTools[iIndex] );
    connect (pEditToolsPath, SIGNAL(textChanged(const QString &)), this, SLOT(slotModifiedToolsPath(const QString &)));
    m_listTools.append ( listToolsPaths[iIndex]->qsExecutableName );

    //QPushButton *pButtonInfo = new QPushButton( m_pScrollFrame, QString("m_pButtonBrowseProjectPath%1").arg(iIndex) );	//ooo
    QPushButton *pButtonInfo = new QPushButton( QString("m_pButtonBrowseProjectPath%1").arg(iIndex), m_pScrollFrame );	//xxx
    pButtonInfo->setText (tr("Info"));
    connect (pButtonInfo, SIGNAL(clicked()), this, SLOT(slotButtonInfo()));
    // Now we add teh button to the buttonList. This is needed to later on 
    // decide which button has been pressed (QObject::sender() does return the Object pointer)
    m_listButtons.append (pButtonInfo);


    //QPushButton *pButtonMan = new QPushButton( m_pScrollFrame, QString("m_pButtonBrowseProjectPath%1").arg(iIndex) );	//ooo
    QPushButton *pButtonMan = new QPushButton( QString("m_pButtonBrowseProjectPath%1").arg(iIndex), m_pScrollFrame );	//xxx
    if (t<(uint)listOfTheLostTools.count())
      pButtonMan->setText (tr("Search ..."));
    else 
      pButtonMan->setText (tr("man"));
    connect (pButtonMan, SIGNAL(clicked()), this, SLOT(slotButtonMan()));
    // Now we add teh button to the buttonList. This is needed to later on 
    // decide which button has been pressed (QObject::sender() does return the Object pointer)
    m_listManButtons.append (pButtonMan);

    pHLayout->addWidget(  pButtonMan );
    pHLayout->addWidget( pButtonInfo );
    // Finally we add our creation to the main layout
    pVLayout2->addLayout( pHLayout );
  }
  pHLayoutMain->addLayout ( pVLayout1 );
  pHLayoutMain->addLayout ( pVLayout2 );

  // Okay we waited until after scanning the system to deleet the previous frame (f any)
  if (pTempFrame)	{
    //m_pScrollView->removeChild (pTempFrame);	//ooo
    m_pScrollView->takeWidget ( );		//xxx
    delete pTempFrame;
  }	//oooo

  // And now let us happily activate the new Frame ...
  //m_pScrollView->addChild ( m_pScrollFrame );	//ooo
  m_pScrollView->setWidget ( m_pScrollFrame );	//xxx
  m_pScrollView->show();
  m_pScrollFrame->show();
  //myCursor = QCursor(QCursor::ArrowCursor);	//ooo
  myCursor = QCursor(Qt::ArrowCursor);		//xxx

  // Here we create the objects for m_listToolsPaths of type <Utils::toolsPaths *>
  for (t=0;t<(uint)m_listToolsPaths.count();t++)
    delete m_listToolsPaths[t];
  m_listToolsPaths.clear ();
  m_listToolsPaths = getToolsPathsList();

  setCursor (myCursor);
}

void DialogSetup::accept ( )
{
/*  if ( m_pTimeEditDuration->time ( ).isNull ( ) && m_pRadioMovieButton->isChecked ( ) ) {
    QString qsError = tr ( "The Duration for MovieObjects is se to 0.\nPlease set the duration first." );
    MessageBox::warning ( NULL, tr ( "Error: Duration is 0." ), qsError, QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  if ( m_pDNDContainer )  {
    // Here we set the values for the Drag'n Drop, so we can later on store them in the init - file.
    if ( (m_pEditThumbnailWidth->text().toInt() > 0) && (m_pEditThumbnailHeight->text().toInt() > 0) )
      m_pDNDContainer->sizeThumbnail = QSize (m_pEditThumbnailWidth->text().toInt(), m_pEditThumbnailHeight->text().toInt());
    if ( (m_pEditButtonWidth->text().toInt() > 0) && (m_pEditButtonHeight->text().toInt() > 0) )
      m_pDNDContainer->sizeButton = QSize (m_pEditButtonWidth->text().toInt(), m_pEditButtonHeight->text().toInt());

    m_pDNDContainer->bImageButton         = m_pRadioImageButton   ->isChecked();
    m_pDNDContainer->bWithText            = m_pCheckWithText      ->isChecked();
    m_pDNDContainer->iTextPosition        = m_pComboTextPosition  ->currentItem()+1;// Note that the enum type starts with TEXT_POS_UNDEF
    m_pDNDContainer->iHowNiceShouldIBe    = m_pSpinPriority->value();
    m_pDNDContainer->font                 = m_font;
    m_pDNDContainer->colorForeground      = m_colorForeground;
    m_pDNDContainer->timeStartOffset      = m_pTimeEditStartOffset->time();
    m_pDNDContainer->timeDuration         = m_pTimeEditDuration   ->time();
    m_pDNDContainer->bStartAfterDropEvent = m_pCheckStartAfterDropEvent->isChecked();
  }*/		//oooo
  QString qsLanguage = Global::qsLanguage;
  storeSlideshowDefaults ( );		//oooo
  // And then we store the ini - file ...
  QDVDAuthorInit *pInit = new QDVDAuthorInit ( m_listToolsPaths, m_pDNDContainer );
  // init of those vars after readInitFile (in constructor)
  Global::qsLanguage           = qsLanguage;
  //Global::iMaxRunningThreads   = m_pSpinMaxThreads->value ( );	//oooo
  Global::qsExternalPlayer     = m_pComboExternalPlayer->currentText ( );
  Global::iDeleteTempOlderThan =-m_pEditOlderThan->text().toInt ( );
  //Global::bPNGRender           =(m_pComboPNGRender->currentItem ( ) == 0 );	//ooo
  Global::bPNGRender           =(m_pComboPNGRender->currentIndex ( ) == 0 );	//xxx
  if (m_pCheckDeleteOlder->isChecked ( ) )
    Global::iDeleteTempOlderThan   = m_pEditOlderThan->text().toInt();

  pInit->setHistory  ( m_iMaxHistoryFiles, m_listHistory, m_listHistoryPix );
  pInit->setAutosave ( m_iAutosave );
  pInit->setStyle    ( m_qsStyle );

  pInit->saveIniFile ( );	//oooo

  // after all's said and done, I can dump this object...
  delete pInit;
  
  // Okay if the user changes the Project name we might want to warn him that
  // all temp data is to be lost ...
  if ( m_pEditProjectName->text ( ) != Global::qsProjectName )  {
    // Okay next we should check if there is some data in the prviouse temp directory...
    QString qsTempDir = Global::qsTempPath + QString ( "/" ) + Global::qsProjectName;
    QDir tempDir ( qsTempDir );
    if ( tempDir.count ( ) > 2 )  {
      QString qsWarning = tr("Warning, there seem to be some temp data in %1/%2.\n\n").arg ( Global::qsTempPath).arg ( Global::qsProjectName );
      qsWarning += tr("[Yes] Move all temp data to %1/%2\n").arg ( Global::qsTempPath).arg ( m_pEditProjectName->text ( ) );
      qsWarning += tr("[Ok]  Leave everything as is,\n");
      qsWarning += tr("[No]  Delete all temp data under %1/%2\n").arg (Global::qsTempPath).arg(Global::qsProjectName );
      int iResult = MessageBox::warning ( NULL, "Warning: Temp data", qsWarning, QMessageBox::Yes, QMessageBox::Ok, QMessageBox::No );
      if (iResult == QMessageBox::No)	{
        QString qsCommand = QString ( "rm -R %1/* ").arg ( qsTempDir );
        //if ( system ( qsCommand ) == -1 )			//ooo
	if ( system ( qsCommand.toLatin1().data() ) == -1 )	//xxx
             return;
      }
      else if ( iResult == QMessageBox::Yes )
        tempDir.rename (qsTempDir, QString ("%1/%2").arg(Global::qsTempPath).arg(m_pEditProjectName->text()));
    }
  }

  //uiDialogSetup::accept ( );	//ooo
  QDialog::accept ( );		//xxx
}

void DialogSetup::slotBrowseTemp ()
{
   // This function is called when the user clickes the Browse button next 
   // to the TempPath
   // This function will browse for the directory where the DVD is to be build.
   //QString qsPathName = Q3FileDialog::getExistingDirectory ( m_pEditTempPath->text() );	//ooo
   QString qsPathName = QFileDialog::getExistingDirectory ( this, tr("Find Directory"), m_pEditTempPath->text(), QFileDialog::ShowDirsOnly );  
   if  ( (qsPathName.isEmpty()) || (qsPathName.isNull()) )
      return;
   m_pEditTempPath->setText(qsPathName);
}

void DialogSetup::slotScanSystem()
{
  uint t, x;
  Utils theUtils;
  QList<Utils::toolsPaths *>listToolsPaths;
  // Here we generate the list to cte the GUI widgets.
  listToolsPaths = theUtils.scanSystem();
  // Here we check all tools which were not be found if the user manually gave a path ...
  for (t=0;t<(uint)listToolsPaths.count();t++)	{
    // Okay we are only interested in the tolls not found by scanSystem.
    if (!listToolsPaths[t]->bPresent)	{
      for (x=0;x<(uint)m_listTools.count();x++)	{
        if (m_listTools[x] == listToolsPaths[t]->qsExecutableName)	{
          // and since m_listTools and m_listEditToolsPaths are synched ...
          QFileInfo fileInfo(m_listEditToolsPaths[x]->text());
          if (fileInfo.exists())	{
            listToolsPaths[t]->qsFullPath = m_listEditToolsPaths[x]->text();
            listToolsPaths[t]->bPresent = true;
          }
        }
      }
    }
  }

  // Next we can craete the GUI elements, and ...
  setToolsPaths (listToolsPaths);

  // then delete the created list-entries of the list.
  for (t=0;t<(uint)listToolsPaths.count();t++)
    delete listToolsPaths[t];
  m_bModifiedToolsPaths = true;
}

void DialogSetup::slotButtonMan ()
{
	// Here we handle the [Search ...] [man] button clicks.
	uint t;

	//QButton *pObjectUnderMouse = NULL;		//ooo
	QAbstractButton *pObjectUnderMouse = NULL;	//xxx
	for (t=0;t<(uint)m_listManButtons.count();t++)		//ox
	{
		// check if this button has the focus ...
		//if ( (m_listManButtons[t]->hasMouse ()) || (m_listManButtons[t]->hasFocus()) )	{	//ooo
		if ( (m_listManButtons[t]->hasMouseTracking ()) || (m_listManButtons[t]->hasFocus()) )	{	//xxx
			pObjectUnderMouse = m_listManButtons[t];
			break;
		}
	}
    if (pObjectUnderMouse != NULL)	{
		Utils theUtils;
		// Okay lets find out what this button stands for ...
		if (pObjectUnderMouse->text () == QString ("man"))
			displayMan (theUtils.getToolByIndex (m_listIndex[t]));
		else
			searchTool (theUtils.getToolByIndex (m_listIndex[t]));
	}
}

void DialogSetup::displayMan (QString qsTool)
{
	QStringList listTool;
	listTool.append(qsTool);
	
	QCursor myCursor (Qt::WaitCursor);
	setCursor (myCursor);
	// Create a modeless dialog, so we can work with DialogExecute and the DialogMan at the same time.
	DialogMan *pManDialog = new DialogMan(listTool, this);
	pManDialog->show();
	//myCursor = QCursor(QCursor::ArrowCursor);	//ooo
	myCursor = QCursor(Qt::ArrowCursor);		//xxx
	setCursor (myCursor);
}

void DialogSetup::searchTool (QString qsTool)
{
	uint t;
	int  i;
	//QString qsFullPath = Q3FileDialog::getOpenFileName (QString ("./"), tr ("executable ( %1 );;All Files ( * )").arg(qsTool));				//ooo
  QString qsFullPath = QFileDialog::getOpenFileName (this, tr("DialogMan"), QString ("./"), tr ("executable ( %1 );;All Files ( * )").arg(qsTool));	//xxx
  if (qsFullPath.isEmpty())
    return;
  // Okay here we set the path
  Utils theUtils;
  i = theUtils.getIndexFromToolName (qsTool);
  if (i < 0)
    return;
  for (t=0;t<(uint)m_listIndex.count();t++)	//ox
    if ((int)m_listIndex[t] == i)
      break;

  m_listEditToolsPaths[t]->setText (qsFullPath);
  //m_listEditToolsPaths[t]->setBackgroundColor (QColor (COLOR_ME_WHITE) );		//ooo
  QPalette palette;									//xxx
  palette.setBrush(m_listEditToolsPaths[t]->backgroundRole(), QColor (COLOR_ME_WHITE));	//xxx
  m_listEditToolsPaths[t]->setPalette(palette);						//xxx
  m_listManButtons[t]->setText("man");
  //m_pScrollFrame->resize (m_pScrollView->visibleWidth(), m_pScrollFrame->height());	//ooo
  m_pScrollFrame->resize (m_pScrollView->width(), m_pScrollFrame->height());		//xxx
//	slotTabChanged(NULL);
}

void DialogSetup::slotButtonInfo ( )
{
	// Here we get the QObject * under the mouse pointer and check if it is in one of the buttons.
	// I prefer this approach over the QObject::sender() - function since this is a non-object oriented approach.
	// All external Tools are defined in global.h
	uint t;
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
 	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	//QObject *pObjectUnderMouse = sender();
	//QButton *pObjectUnderMouse = NULL;		//ooo
	QAbstractButton *pObjectUnderMouse = NULL;	//xxx
	for (t=0;t<(uint)m_listButtons.count();t++)	//ox
	{
		// check if this button has the focus ...
		//if ( (m_listButtons[t]->hasMouse ()) || (m_listButtons[t]->hasFocus()) )	{		//ooo
		if ( (m_listButtons[t]->hasMouseTracking ()) || (m_listButtons[t]->hasFocus()) )	{	//xxx
			pObjectUnderMouse = m_listButtons[t];
			break;
		}
	}
	if (!pObjectUnderMouse)	{
		MessageBox::warning(this, "Warning", "Say what ???", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	MessageBox::warning(this, QString( "Info tool : %1").arg(toolsArray[m_listIndex[t]].pExecutableName), toolsArray[m_listIndex[t]].pDescription, QMessageBox::Ok, QMessageBox::NoButton);
//	pToolLabel->setText (toolsArray[t].pDisplayName);
}

QList<Utils::toolsPaths *> DialogSetup::getToolsPathsList ()
{
	// Here we actually create the toolsPaths list before we return it ...
	
	// Warning, m_listTools has to have the same count as m_listIndex !!!
	uint t, iIndex;
	Utils::toolsPaths *pEntry;
	QList<Utils::toolsPaths *> listToolsPaths;	//oxx
	// then we clear the list ...
//	for (t=0;t<m_listToolsPaths.count();t++)
//		delete m_listToolsPaths[t];
//	m_listToolsPaths.clear ();
	// I want to use the m_listIndex values to recreate the original order ...
	for (t=0;t<(uint)m_listTools.count();t++)	// should be same count as m_listEditToolsPaths ...	//ox
		listToolsPaths.append(new Utils::toolsPaths);
	// Okay and here we go through the current values ...
	for (t=0;t<(uint)listToolsPaths.count();t++)	{	//ox
		iIndex = m_listIndex[t];
		pEntry = listToolsPaths[iIndex];
		pEntry->qsExecutableName = m_listTools[iIndex];
		pEntry->qsFullPath = m_listEditToolsPaths[iIndex]->text();
		/*if (m_listEditToolsPaths[iIndex]->backgroundColor() == QColor (COLOR_ME_WHITE))
			pEntry->bPresent = true;
		else
			pEntry->bPresent = false;*/	//oooo
	}

	QString qsPreferredEngine = comboPreferredEngine->currentText();
	//MediaCreator::setPreferredEngine (qsPreferredEngine);		//oooo
	// and then we can return ..
	return listToolsPaths;
}
