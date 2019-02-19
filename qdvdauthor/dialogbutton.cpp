/***************************************************************************
    dialogbutton.cpp
                             -------------------
    DialogButton class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   This class handles the ButtonDialog.
   With this dialog you are able to set all properties of the button,
   including the different states (normal, highlighted, and selected).
   
   Once the Button object has been created, it is displayed in the MenuPreview
   window and handles the Highlight state, when the mouse is over it, and the
   selected state when the mouse is 'left-clicked' on it.
    
****************************************************************************/

#include <QColorDialog>
#include <QFileInfo>
#include <QTimer>
#include <QPixmap>
#include <QMessageBox>

#include "xml_dvd.h"
#include "dvdmenu.h"
#include "qdvdauthor.h"
#include "dialogbutton.h"
#include "menupreview.h"
#include "menuobject.h"
#include "maskobject.h"
#include "frameobject.h"
#include "textobject.h"
#include "messagebox.h"
#include "dialogselect.h"
#include "buttonobject.h"
#include "imageobject.h"
#include "buttonpreview.h"
#include "sourcefileentry.h"
#include "dialogbuttontrans.h"
#include "dvdmenuundoobject.h" //undobuffer.h"

#define BUTTON_BASIC    "<<< Basic"
#define BUTTON_ADVANCED "Advanced >>>"
#define MANUAL_CHANGE   255, 180, 60

//DialogButton::DialogButton ( QWidget * parent, const char * name, Qt::WFlags f )	//ooo
DialogButton::DialogButton ( QWidget *pParent )						//xxx
	//: uiDialogButton   ( parent, name, f )					//ooo
	: QDialog ( pParent )								//xxx
{
  setupUi(this);
  // Standard is set to false. This indicates that we create a button as
  // opposed to modify an existing one.
  m_bButtonCreation     = false;
  m_bAutomatic          = false;
  m_pMenuPreview        = NULL;
  m_pHighlightedMask[0] = NULL; // Frame
  m_pHighlightedMask[1] = NULL; // Mask
  m_pSelectedMask[0]    = NULL; // Frame
  m_pSelectedMask[1]    = NULL; // Mask
  m_iColorIdx[0]        = 0;
  m_iColorIdx[1]        = 0;
}

DialogButton::~DialogButton()
{
}

void DialogButton::initMe ( ButtonObject *pButton, MenuPreview *pView )
{
  //QPixmap image  = qPixmapFromMimeSource (QString ("checkered.png"));	//ooo
  QPixmap image (":/images/checkered.png");				//xxx
  m_pMenuPreview = pView;

  // Here we hide the advanced features and wait for the user to ask for them.
  m_pGroupGoto->hide           ( );
  m_pEditAction->hide          ( );
  m_pGroupGeometry->hide       ( );
  m_pButtonHelpAction->hide    ( );
  m_pGroupSystemRegister->hide ( );
  m_pButtonAdvanced->setText   ( BUTTON_ADVANCED );

  //m_pFramePreviewNormal = new ButtonPreview( tab, "m_pFramePreviewNormal" );	//ooo
  m_pFramePreviewNormal = new ButtonPreview( tab );				//xxx
  m_pFramePreviewNormal->setAccessibleName( "m_pFramePreviewNormal" );		//xxx
  m_pFramePreviewNormal->setScaledContents         ( true );			//xxx
  m_pFramePreviewNormal->setAutoFillBackground     ( true );			//xxx
  //m_pFramePreviewNormal->setPaletteBackgroundPixmap( image );			//ooo
  QPalette palette;								//xxx
  palette.setBrush(QPalette::Window, QBrush(image));				//xxx
  m_pFramePreviewNormal->setPalette(palette);					//xxx
  //m_pFramePreviewNormal->resize(400,400);						//xxx
  /*m_pFramePreviewNormal->setFrameShape  ( QFrame::StyledPanel );
  //m_pFramePreviewNormal->setFrameShadow ( Q3Frame::Raised );			//ooo
  m_pFramePreviewNormal->setFrameShadow ( QFrame::Raised );			//xxx
  QSizePolicy sizePolicy = m_pFramePreviewNormal->sizePolicy ( );
  m_pFramePreviewNormal->setSizePolicy  ( sizePolicy );
  m_pFramePreviewNormal->setLineWidth   ( 2 );
  //tabLayout->addWidget( m_pFramePreviewNormal );				//ooo*/
 
  //tab->layout()->addWidget( m_pFramePreviewNormal );				//xxx
 
  //m_pFrameHighlighted = new ButtonPreview( tab_2, "m_pFrameHighlighted" );	//ooo
  m_pFrameHighlighted = new ButtonPreview( tab_2 );				//xxx
  //m_pFrameHighlighted->setPaletteBackgroundPixmap( image );			//ooo
  //QPalette palette;								//xxx
  palette.setBrush(QPalette::Window, QBrush(image));				//xxx
  m_pFrameHighlighted->setPalette(palette);					//xxx
  m_pFrameHighlighted->setFrameShape  ( QFrame::StyledPanel );
  m_pFrameHighlighted->setFrameShadow ( QFrame::Raised );
  //m_pFrameHighlighted->setSizePolicy  ( sizePolicy );				//ooo
  m_pFrameHighlighted->setLineWidth   ( 2 );
  //tabLayout_2->addWidget( m_pFrameHighlighted );	//ooo

  //tab_2->layout()->addWidget( m_pFrameHighlighted );	//xxx

  //m_pFrameSelected = new ButtonPreview( TabPage_2, "m_pFrameSelected" );	//ooo
  m_pFrameSelected = new ButtonPreview( TabPage_2 );				//xxx
  //m_pFrameSelected->setPaletteBackgroundPixmap( image );			//ooo
  //QPalette palette;								//xxx
  palette.setBrush(QPalette::Window, QBrush(image));				//xxx
  m_pFrameSelected->setPalette(palette);					//xxx
  m_pFrameSelected->setFrameShape  ( QFrame::StyledPanel );
  m_pFrameSelected->setFrameShadow ( QFrame::Raised );
  //m_pFrameSelected->setSizePolicy  ( sizePolicy );				//ooo
  m_pFrameSelected->setLineWidth   ( 2 );
  //TabPageLayout_2->addWidget( m_pFrameSelected );	//ooo

  //TabPage_2->layout()->addWidget( m_pFrameSelected );	//xxx

  m_pButtonObject = pButton;
  // Note the cloned Button is where all the action takes place. Unless the User clicks on Ok 
  // Nothing in the original ButtonObject is modified.
  m_pClonedButton = (ButtonObject *)pButton->clone ( );
 
  // Okay for the cloned Button but we need to reParent the MenuObjects contained within,
  // otherwise the effect will shoot back ...
  uint t=0;
  MenuObject *pObject;
  for (t=0;t<m_pClonedButton->getNormalCount();t++)	{
    pObject = m_pClonedButton->getNormal(t);
    pObject->setParent(m_pFramePreviewNormal);
  }
  for (t=0;t<m_pClonedButton->getHighlightedCount();t++)	{
    pObject = m_pClonedButton->getHighlighted(t);
    pObject->setParent(m_pFrameHighlighted);
    // In case the user changes the highlighted color of the objects in this dialog ...
    connect (pObject, SIGNAL(signalMaskColorChanged(Rgba, Rgba)), pView, SIGNAL(signalMaskColorChanged(Rgba, Rgba)));
  }
  for (t=0;t<m_pClonedButton->getSelectedCount();t++)	{
    pObject = m_pClonedButton->getSelected(t);
    pObject->setParent(m_pFrameSelected);
    // In case the user changes the selected color of the objects in this dialog ...
    connect (pObject, SIGNAL(signalMaskColorChanged(Rgba, Rgba)), pView, SIGNAL(signalMaskColorChanged(Rgba, Rgba)));
  }
  
  m_pFramePreviewNormal->addMenuObject ( m_pClonedButton->getNormal      ( 0 ) );
  m_pFrameHighlighted  ->addMenuObject ( m_pClonedButton->getHighlighted ( 0 ) );
  m_pFrameSelected     ->addMenuObject ( m_pClonedButton->getSelected    ( 0 ) );
  
  MenuObject *pMenuObject = m_pClonedButton->getHighlighted ( 0 );
  bool bChecked = false;
  if ( pMenuObject ) 
    bChecked = ( ( pMenuObject->objectType ( ) == MASK_OBJECT ) || 
           ( pMenuObject->objectType ( ) == TEXT_OBJECT ) ); // Text Objects are kind o a mask
  m_pRadioHighlightedMask ->setChecked (  bChecked );
  m_pRadioHighlightedFrame->setChecked ( !bChecked );
  m_pHighlightedMask[bChecked] = pMenuObject;
  
  pMenuObject = m_pClonedButton->getSelected ( 0 );
  bChecked    = false;
  if ( pMenuObject )
    bChecked  = ( ( pMenuObject->objectType ( ) == MASK_OBJECT ) ||
      ( pMenuObject->objectType ( ) == TEXT_OBJECT ) ); // Text Objects are kind o a mask
  m_pRadioSelectedMask ->setChecked (  bChecked ); 
  m_pRadioSelectedFrame->setChecked ( !bChecked ); 
  m_pSelectedMask[bChecked] = pMenuObject;
  
  bool bEnable = false;
  pMenuObject  = m_pClonedButton->getNormal ( 0 );
  if ( pMenuObject->objectType ( ) == FRAME_OBJECT ) {
    m_pGroupHighlighted->setEnabled ( false );
    m_pGroupSelected   ->setEnabled ( false );
  }
  else if ( ( pMenuObject->objectType ( ) == IMAGE_OBJECT ) ||
            ( pMenuObject->objectType ( ) == MOVIE_OBJECT ) )  {
    bEnable = true;
    ImageObject *pImage = (ImageObject *)pMenuObject;
    m_pCheckHide->setChecked ( pImage->hidden ( ) );
  }
  if ( ! bEnable )
    m_pCheckHide->hide ( );
  
  // Now we proceed to initialize the GUI objects (LineEdits, ComboBoxes etc.)
  initNext       ( );
  //initMultiple   ( ); called from initTransition ( )
  initTransition ( );
  //initSystem     ( pView, pButton ); // must be initialized last (timer)

  // Here we set the text fields for the bounding rectangle ...
  m_pEditX     ->setText ( QString ("%1").arg( pButton->getNormal(0)->boundingRect().x ( ) ) );
  m_pEditY     ->setText ( QString ("%1").arg( pButton->getNormal(0)->boundingRect().y ( ) ) );
  m_pEditWidth ->setText ( QString ("%1").arg( pButton->getNormal(0)->boundingRect().width  ( ) ) );
  m_pEditHeight->setText ( QString ("%1").arg( pButton->getNormal(0)->boundingRect().height ( ) ) );	
  m_pEditName  ->setText ( pButton->name ( ) );
  
  // And then we should also init the possible Actions to take ...
  initColors  ( );	//ooo
  initActions ( );
  
  // Last we connect all signal's slots neccesary.

  connect (m_pButtonPlayMultiple,SIGNAL( clicked ( ) ), this, SLOT(slotMultiple   ( ) ) );
  connect (m_pButtonAdvanced,   SIGNAL ( clicked ( ) ), this, SLOT(slotAdvanced   ( ) ) );
  connect (m_pButtonHelpAction, SIGNAL ( clicked ( ) ), this, SLOT(slotHelpAction ( ) ) );
  connect (m_pButtonHelpGoto,   SIGNAL ( clicked ( ) ), this, SLOT(slotHelpGoto   ( ) ) );
  connect (m_pButtonHelp,       SIGNAL ( clicked ( ) ), this, SLOT(slotHelp       ( ) ) );
  connect (m_pButtonHelpSystem, SIGNAL ( clicked ( ) ), this, SLOT(slotHelpSystem ( ) ) );
  

  
  // Next are the slots related to the color buttons in the Highlighted / Selected - tab.
  connect (m_pButtonHColor1,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonHColor2,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonHColor3,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonHColor4,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  
  connect (m_pButtonSColor1,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonSColor2,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonSColor3,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonSColor4,    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );
  connect (m_pButtonTransition, SIGNAL ( clicked (      ) ), this, SLOT ( slotTransition  (   ) ) );
  
  connect (m_pButtonOkay,       SIGNAL ( clicked ( ) ), this, SLOT ( slotOkay   ( ) ) );
  connect (m_pButtonCancel,     SIGNAL ( clicked ( ) ), this, SLOT ( slotCancel ( ) ) );
  QTimer::singleShot ( 10, this, SLOT  ( slotAdjustSize ( ) ) );
}

void DialogButton::slotAdjustSize ( )
{  
  resize  ( width ( ), 530 );
  initSystem ( m_pMenuPreview, m_pButtonObject );
}

void DialogButton::initNext()
{ 
	// Interesting handling here, 
	// first we should check if the choice exists in the ComboBox and if so set the index, OR
	// if it does not exist, we should set it as the currentText.
//	m_pButtonObject->setNext (BUTTON_UP,    m_pComboUp->currentText());
//	m_pButtonObject->setNext (BUTTON_DOWN,  m_pComboDown->currentText());
//	m_pButtonObject->setNext (BUTTON_LEFT,  m_pComboLeft->currentText());
//	m_pButtonObject->setNext (BUTTON_RIGHT, m_pComboRight->currentText());

	// First we build the available choices ...
	QComboBox *comboArray[4];
	int intArray[4];
	comboArray[0] = m_pComboUp;	intArray[0] = NEXT_BUTTON_UP;
	comboArray[1] = m_pComboDown;	intArray[1] = NEXT_BUTTON_DOWN;
	comboArray[2] = m_pComboRight;	intArray[2] = NEXT_BUTTON_RIGHT;
	comboArray[3] = m_pComboLeft;	intArray[3] = NEXT_BUTTON_LEFT;

	uint t, i;
	for (t=0;t<4;t++)	{
		comboArray[t]->clear();
		// The main menu (bien sur)
//		comboArray[t]->insertItem(QString("vmgm"));
		//comboArray[t]->insertItem(QString("-- default --"));		//ooo
		comboArray[t]->addItem(QString("-- default --"));		//xxx

		// Next we should include all sub-menus ...
		//for (i=0;i<(uint)m_listSubMenus.count();i++)                    //ooo
                for (i=0;i<static_cast<uint> ( m_listSubMenus.count() );i++)    //xxx
			//comboArray[t]->insertItem(m_listSubMenus[i]);		//ooo
			comboArray[t]->addItem(m_listSubMenus[i]);		//xxx

		// and also all available Buttons for the current menu
		//for (i=0;i<(uint)m_listMenuButtons.count();i++)                 //ooo
                for (i=0;i<static_cast<uint> ( m_listMenuButtons.count() );i++) //xxx
			//comboArray[t]->insertItem(m_listMenuButtons[i]);	//ooo
			comboArray[t]->addItem(m_listMenuButtons[i]);		//xxx

		// After we initialized the combo boxes, we set the current text (from the buttonObject
		if (!m_pClonedButton->next(intArray[t]).isEmpty())
			comboArray[t]->setCurrentText(m_pClonedButton->next(intArray[t]));	//oooo
	}
}

void DialogButton::initMultiple ( )
{
  if ( ! m_pClonedButton )
    return;

  QString qsText;
  QStringList list = m_pClonedButton->getPlayMultipleList ( );
  if (   m_pClonedButton->getTransition ( ) ) {
    if ( m_pClonedButton->getTransition ( )->bUserFile )
      qsText = tr ( "User Transition" );
    else
      qsText = tr ( "Transition, %1 Sec" ).arg ( m_pClonedButton->getTransition ( )->fDuration );
  }
  if ( list.count ( ) > 0 ) {
    if ( qsText.length ( ) > 0 )
         qsText += "<br>";
    QString qsLoopMenu;
    int iTargetCount = list.count ( );
    QString qsLast   = list.last  ( );
    //QStringList lastList = QStringList::split ( STRING_SEPARATOR, qsLast );	//ooo
    QStringList lastList = qsLast.split ( STRING_SEPARATOR );			//xxx
    qsLoopMenu += m_pClonedButton->getLoopMultiple ( ) ? tr (  ", Loop"   ) : "";
    if ( lastList.count ( ) == 2 )  { // Last target is to a Menu
      qsLoopMenu += tr ( ", to Menu" );
      iTargetCount --;
    }
    qsText += tr ( "Play %1" ).arg ( iTargetCount ) + qsLoopMenu;
    m_pLabelMultiple->setText ( qsText );
  }
  m_pLabelMultiple->setText ( "<FONT COLOR=\"RED\"><B>" + qsText + "</B></FONT>" );

/*
  QStringList list = m_pClonedButton->getPlayMultipleList ( );
  if ( list.count ( ) > 0 ) {
    int iTargetCount = list.count ( );
    QString qsText;
    QString qsLast = list.last ( );
    QStringList lastList = QStringList::split ( STRING_SEPARATOR, qsLast );
    qsText += m_pClonedButton->getLoopMultiple ( ) ? tr (  ", Loop"   ) : "";
    if ( lastList.count ( ) == 2 )  { // Last target is to a Menu
      qsText += tr ( ", to Menu" );
      iTargetCount --;
    }
    qsText += "</B></FONT>";
    qsText  = "<FONT COLOR=\"RED\"><B>" + tr ( "Play %1" ).arg ( iTargetCount ) + qsText; 
    m_pLabelMultiple->setText ( qsText );
  }
  else
    m_pLabelMultiple->setText ( "" );
*/
}

void DialogButton::initTransition ( )
{
  // Simply sets the text of the transition Button
  // and re-write the Multiple label.
  initMultiple ( );
  if ( m_pClonedButton->getTransition ( ) )
    m_pButtonTransition->setText ( tr ( "Remove Transition" ) );
  else
    m_pButtonTransition->setText ( tr ( "Add Transition" ) );
}

void DialogButton::initColors ( )
{
  // Here we set the background color of the [Color] buttons in the Highlighted / Selected tab
  // and set the check of the current used color of said Mask.
  uint t, i;
  DVDMenu *pDVDMenu = Global::pApp->getCurrentSubMenu ( );

  if ( ! pDVDMenu )
    return;

  CDVDMenuInterface *pInterface = pDVDMenu->getInterface ( );
  QPushButton *arrayButtons[8] = { m_pButtonHColor1, m_pButtonHColor2, m_pButtonHColor3, m_pButtonHColor4, 
                                   m_pButtonSColor1, m_pButtonSColor2, m_pButtonSColor3, m_pButtonSColor4 };
  QRadioButton *arrayChecks[8] = { m_pRadioHColor1,  m_pRadioHColor2,  m_pRadioHColor3,  m_pRadioHColor4, 
                                   m_pRadioSColor1,  m_pRadioSColor2,  m_pRadioSColor3,  m_pRadioSColor4  };

  Rgba clrHighlighted = m_pClonedButton->getMaskColor ( true  );
  Rgba clrSelected    = m_pClonedButton->getMaskColor ( false );

  for ( t=0; t<8; t++ ) {
    i = t%4;
    if ( t < MAX_MASK_COLORS ) {
      m_pgcColorsOrig[i] = pInterface->pgcColors[i]; // Store so we can compare.
      m_pgcColors    [i] = pInterface->pgcColors[i]; // colors we can modify.
    }
    //arrayButtons[t]->setPaletteBackgroundColor ( pInterface->pgcColors[i].color ( ) );  //ooo
    QPalette palette;							                        //xxx
    palette.setBrush(arrayButtons[t]->backgroundRole(), pInterface->pgcColors[i].color ( ));	//xxx
    arrayButtons[t]->setPalette(palette);						         //xxx
    if ( ( t < 4 ) && ( clrHighlighted == pInterface->pgcColors[i] ) ) {
      arrayChecks[t]->setChecked ( true );
      m_iColorIdx[1] = t;
    }
    if ( ( t > 3 ) && ( clrSelected    == pInterface->pgcColors[i] ) ) {
      arrayChecks[t]->setChecked ( true );    
      m_iColorIdx[0] = t;
    }
  }
}

void DialogButton::initActions()
{
  // Action1 can only be one of these four ...
   // Note: an empty string indicates that the user manually changed the action along the way.
  //m_pComboAction1->insertItem(QString ("jump"));	//ooo
  m_pComboAction1->addItem(QString ("jump"));		//xxx
  //m_pComboAction1->insertItem(QString ("call"));	//ooo
  m_pComboAction1->addItem(QString ("call"));		//xxx
  //m_pComboAction1->insertItem(QString ("resume"));	//ooo
  m_pComboAction1->addItem(QString ("resume"));		//xxx
  // The slots will build the rest of the fest ...
  slotAction1(0);
}

void DialogButton::initActionState ()
{
  // Here we initialize the state of the Action Combo boxes according to ButtonObject->action()
  QString qsAction1, qsAction2, qsAction3, qsAction4, qsButtonAction;
  // It could be 2 different possibilities : a) only one file in this entry b) multiple files in this entry
  // a) action1 = Jump / action2 = file / action3 = chapter
  // b) action1 = Jump / action2 = entry / action3 = file / action4 = chapter
  QStringList listActions;

  //  qsButtonAction = m_pButtonObject->action();
  qsButtonAction = m_pClonedButton->action ( );
  if (qsButtonAction.isEmpty())
    return;

  //listActions = QStringList::split (STRING_SEPARATOR, qsButtonAction);	//ooo
  listActions = qsButtonAction.split (STRING_SEPARATOR );			//xxx
  qsAction1 = listActions[0];

  // At this point we have the correct mix of Actions ... Let's set them ...
  if ( qsAction1 == QString ( "resume" ) ) {
    m_pComboAction1->setCurrentText ( qsAction1 );
    return;
  }

  if (listActions.count() == 1)	{
    if (m_pComboAction1->count() <= 3)
      //m_pComboAction1->insertItem (QString ());	//ooo
      m_pComboAction1->addItem (QString ());		//xxx
    //m_pComboAction1->setCurrentItem (3);		//ooo
      m_pComboAction1->setCurrentIndex (3);		//xxx
    return;
  }
  qsAction2 = listActions[1];
  qsAction3 = listActions[2];
  if (listActions.count() > 3)
    qsAction4 = listActions[3];

  if (!qsAction2.isEmpty())
    m_pComboSourceEntry->setCurrentText(qsAction2);

  // Okay here we generate the information in the Combo boxes
  //slotSourceEntry(m_pComboSourceEntry->currentItem());	//ooo
  slotSourceEntry(m_pComboSourceEntry->currentIndex());		//xxx

  // and finally we set the value found in the file ...
  if (listActions.count() == 3)	{
    if ( ( ! qsAction3.isEmpty ( ) ) && ( m_pComboChapters->count ( ) > 0 ) )
      m_pComboChapters->setCurrentText ( qsAction3 );

    slotChapters(0);
  }
  else if  ( listActions.count ( ) > 3 ) {
    if ( ( ! qsAction3.isEmpty ( ) ) &&  ( m_pComboSourceFile->count ( ) > 0 ) )
      m_pComboSourceFile->setCurrentText(qsAction3);
    slotSourceFile ( 0 );
    if ( ( ! qsAction4.isEmpty ( ) ) && ( m_pComboChapters2->count ( ) > 0 ) ) {
      m_pComboChapters2->setCurrentText ( qsAction4 );
      m_pComboChapters->setCurrentText  ( qsAction4 );
    }
    slotChapters2(0);
  }
}	//oooo

void DialogButton::initButtons ( ButtonObject *pButton )
{
  DVDMenu *pDVDMenu = NULL;
  if ( pButton )  {
    pDVDMenu = getSelectedDVDMenu ( ); //pButton->dvdMenu ( );
    if ( ! pDVDMenu )  // The button does not point to a DVDMenu
           pDVDMenu = Global::pApp->getCurrentSubMenu ( );
  }
  initButtons ( pDVDMenu );
}

void DialogButton::initButtons ( DVDMenu *pDVDMenu )
{
  // Note: pDVDMenu is either the menu the button is defined in OR
  //       it is the menu the button points to.
  ButtonObject *pButton = NULL;
  m_pComboButton->clear ( );
  //m_pComboButton->insertItem ( "Button -Def-" );	//ooo
  m_pComboButton->addItem ( "Button -Def-" );		//xxx

  if ( pDVDMenu )  {
    QList<ButtonObject *> list = pDVDMenu->getButtons ( );	//oxx
    QList<ButtonObject *>::iterator it  =  list.begin ( );	//oxx
    while ( it != list.end ( ) )  {
      pButton = *it++;
      //m_pComboButton->insertItem ( pButton->name ( ) );	//ooo
      m_pComboButton->addItem ( pButton->name ( ) );		//xxx
    }
  }
//  slotSystem ( 0 );
}

void DialogButton::initSystem ( MenuPreview *pMenuPreview, ButtonObject *pButton )
{  
  uint t, i, iItem;
  bool bOkay, bFound;
  MenuObject *pMenuObject = NULL;
  QString     qsTemp, qsPreAction;

  if ( pButton )
    qsPreAction = pButton->preAction ( );

  initButtons ( pButton );

  // Next we initialize the combo boxes.
  QRegExp rx;
  rx.setMinimal ( true );
  rx.setPattern ( "audio=.*;" );
  //if ( rx.search ( qsPreAction ) > - 1 ) {	//ooo
  if ( rx.indexIn ( qsPreAction ) > - 1 ) {	//xxx
    qsTemp = rx.cap ( 0 );
    qsTemp.remove ( "audio=" ); qsTemp.remove ( ";" );
    iItem = qsTemp.toInt ( &bOkay );
    if ( bOkay ) 
      //m_pComboAudio->setCurrentItem ( ++iItem );	//ooo
      m_pComboAudio->setCurrentIndex ( ++iItem );	//xxx
  }
  rx.setPattern ( "subtitle=.*;" );
  //if ( rx.search ( qsPreAction ) > - 1 ) {	//ooo
  if ( rx.indexIn ( qsPreAction ) > - 1 ) {	//xxx
    qsTemp = rx.cap ( 0 );
    qsTemp.remove ( "subtitle=" ); qsTemp.remove ( ";" );
    iItem = qsTemp.toInt ( &bOkay );
    if ( bOkay ) {
      iItem -= 64;
      //m_pComboSubtitles->setCurrentItem ( iItem+1 );	//ooo
      m_pComboSubtitles->setCurrentIndex ( iItem+1 );	//xxx
    }
  }
  rx.setPattern ( "angle=.*;" );
  //if ( rx.search ( qsPreAction ) > - 1 ) {	//ooo
  if ( rx.indexIn ( qsPreAction ) > - 1 ) {	//xxx
    qsTemp = rx.cap ( 0 );
    qsTemp.remove ( "angle=" ); qsTemp.remove ( ";" );
    iItem = qsTemp.toInt ( &bOkay );
    if ( bOkay ) 
      //m_pComboAngle->setCurrentItem ( ++iItem );	//ooo
      m_pComboAngle->setCurrentIndex ( ++iItem );	//xxx
  }
  rx.setPattern ( "region=.*;" );
  //if ( rx.search ( qsPreAction ) > - 1 ) {	//ooo
  if ( rx.indexIn ( qsPreAction ) > - 1 ) {	//xxx
    qsTemp = rx.cap ( 0 );
    qsTemp.remove ( "region=" ); qsTemp.remove ( ";" );
    iItem = qsTemp.toInt ( &bOkay );
    if ( bOkay ) 
      //m_pComboRegion->setCurrentItem ( ++iItem );	//ooo
      m_pComboRegion->setCurrentIndex ( ++iItem );	//xxx
  }

  MenuPreview *pButtonsMenuPreview = pMenuPreview;
  qsPreAction.replace ( "\"", "&quot;" ); // this to make sure we catch either way " and &quot; ...
  rx.setPattern ( "button=&quot;.*&quot;" );
  //if ( rx.search ( qsPreAction ) > -1 )  {	//ooo
  if ( rx.indexIn ( qsPreAction ) > -1 )  {	//xxx
    if ( pButton->dvdMenu ( ) ) // Button to another menu should check there 
      pMenuPreview = pButton->dvdMenu ( )->getMenuPreview ( );
    bFound = false;
    for ( i=0; i<2 && !bFound; i++ )  {
      if ( pMenuPreview )  {
        QString qsButtonName = rx.cap ( 0 );
        qsButtonName.remove ( "button=" );
        qsButtonName.remove ( "&quot;" );
        for ( t=0; t<pMenuPreview->objectCount ( ); t++ )  {
          pMenuObject = pMenuPreview->menuObject ( t );
          if ( pMenuObject->name ( ) == qsButtonName ) {
            //if ( (uint)m_pComboButton->count      ( )  > t + 1 )              //ooo
            if ( static_cast<uint> ( m_pComboButton->count      ( ) ) > t + 1 ) //xxx 
                       //m_pComboButton->setCurrentItem  ( t + 1 );	//ooo
	               m_pComboButton->setCurrentIndex  ( t + 1 );	//xxx
            bFound = true;
            break; // exit for - loop
          }
        }
      }
      pMenuPreview = pButtonsMenuPreview;
    }
  }
  qsPreAction.replace ( "&quot;", "\"" ); // this to make sure we catch either way " and &quot; ...

  m_pEditSystem->setText ( qsPreAction );
}

void DialogButton::slotHelpSystem ()
{
  QSize theSize ( 1000, 600 );
  MessageBox::help ( this, "System Register", "system_register.html", theSize );
}

void DialogButton::setSourceFiles(QList<SourceFileEntry *>listSourceFileEntries)	//oxx
{
	uint t;
	m_listSourceFileEntries.clear();
	//for (t=0;t<(uint)listSourceFileEntries.count();t++)	{	               //ooo
        for (t=0;t<static_cast<uint> ( listSourceFileEntries.count() );t++)	{	//xxx
		if (!listSourceFileEntries[t]->bSoundSource)
			m_listSourceFileEntries.append(listSourceFileEntries[t]);
	}
	slotAction1    ( 0 );
	initActionState ( );	//oooo
}

void DialogButton::setSubMenus (QStringList listSubMenus)
{
	m_listSubMenus = listSubMenus;
	slotAction1    ( 0 );
	initActionState ( );
}

void DialogButton::setMenuButtons (QStringList listMenuButtons)
{
  m_listMenuButtons = listMenuButtons;
  initNext();
}

DVDMenu *DialogButton::getSelectedDVDMenu ( )
{
  DVDMenu *pMenu     = NULL;
  QString qsMenuName = m_pComboSourceEntry->currentText ( );
  if ( qsMenuName == "vmgm" )
       qsMenuName  = VMGM_LABEL;

  if ( qsMenuName.length ( ) > 0 )
       pMenu = Global::pApp->getSubMenuByName( qsMenuName );

  return pMenu;
}

SourceFileEntry *DialogButton::getSelectedSourceEntry()
{
	uint t;

	// Since we have also menus in the combo box we need to get the iCurrentEntry, which is the correct index 
	// to the m_listSourceFileEntries - list.
	//for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	{                  //ooo
        for (t=0;t<static_cast<uint> ( m_listSourceFileEntries.count() );t++)	{ //xxx
		if (m_listSourceFileEntries[t]->qsDisplayName == m_pComboSourceEntry->currentText())
			return m_listSourceFileEntries[t];
	}
	return NULL;
}

SourceFileInfo *DialogButton::getSelectedSourceFile()
{
	uint t;
	QFileInfo fileInfo;
	SourceFileEntry *pEntry = getSelectedSourceEntry();
	// sanity check ...
	if (pEntry)	{
		// First we should see which sourceFile is selected ...
		//for (t=0;t<(uint)pEntry->listFileInfos.count();t++)	{	          //ooo
                for (t=0;t<static_cast<uint> ( pEntry->listFileInfos.count() );t++)	{   //xxx
			fileInfo.setFile (pEntry->listFileInfos[t]->qsFileName);
			if (fileInfo.fileName() == m_pComboSourceFile->currentText())	{
				return pEntry->listFileInfos[t];
			}
		}
	}
	return NULL;
}

void DialogButton::slotAction1 ( int )
{
	// Okay, here we prepare the available actions ...
	// if (Action1->text() == jump , then we generate a movie list for Action2, and Action3 has chapters according to the Movie.
	// if (Action1->text() == call , then we generate only a menu list for Action2.
	// if (Action1->text() == resume , then we keep Action2 and Action3 empty.

	// The following is allowed :
	// jump vmgm menu               -> jumps to default menu.
	// jump titleset 2 menu         -> default menu of titleset 2
	// jump vmgm menu 3             -> jumps to menu 3.
	// jump titleset 2 menu 3       -> jumps to menu 3 of titleset 2
	// jump vmgm menu 3 entry 4     -> jumps to menu denoted as the entry 4 for the vmgm.
	// jump titleset 2 menu entry 3 -> jumps to menu denoted as the entry 3 of titleset 2
	//
	// call - resume work together but only from a title t a menu, all other forms are illegal.
	// this is used to allow to return to the point in the title from where the call originated.
	//
	// jump vmgm title 2 [chapter 3] -> jumps to the title or chapter in a title
	//                                  All titles on the disc are accessible in the vmgm domain
	// jump chater 2, jump program 1, cell 4 -> jumps to a chapter, program, or cell in the current title.
	//                                  You cna use this to create looping menus (jump cell 1;)

	uint t;
	m_pComboSourceEntry->clear();
	m_pComboSourceFile->clear();
	if (m_pComboAction1->currentText() == QString ("jump"))	{
		// We can always jump to the main menu ...
		//m_pComboSourceEntry->insertItem("vmgm");	//ooo
		m_pComboSourceEntry->addItem("vmgm");		//xxx
		//for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	{	                //ooo
                for (t=0;t<static_cast<uint> ( m_listSourceFileEntries.count() );t++)	{	//xxx
			if (!m_listSourceFileEntries[t]->bSoundSource)
				//m_pComboSourceEntry->insertItem(m_listSourceFileEntries[t]->qsDisplayName);	//ooo
				m_pComboSourceEntry->addItem(m_listSourceFileEntries[t]->qsDisplayName);	//xxx
		}
		//for (t=0;t<(uint)m_listSubMenus.count();t++)	               //ooo
		for (t=0;t<static_cast<uint> ( m_listSubMenus.count() );t++)	//xxx
			//m_pComboSourceEntry->insertItem(m_listSubMenus[t]);	//ooo
			m_pComboSourceEntry->addItem(m_listSubMenus[t]);	//xxx
	}
	else if (m_pComboAction1->currentText() == QString ("call"))	{
//		for (t=0;t<m_listSourceFiles.count();t++)
//			m_pComboAction2->insertItem(m_listSubMenus[t]);
		//m_pComboSourceEntry->insertItem("vmgm");	//ooo
		m_pComboSourceEntry->addItem("vmgm");		//xxx
		//for (t=0;t<(uint)m_listSubMenus.count();t++)	               //ooo
                for (t=0;t<static_cast<uint> ( m_listSubMenus.count() );t++)	//xxx
			//m_pComboSourceEntry->insertItem ( m_listSubMenus[t] );	//ooo
			m_pComboSourceEntry->addItem ( m_listSubMenus[t] );		//xxx
	}

	slotSourceEntry ( 0 );
}

void DialogButton::slotSourceEntry(int)
{
  uint t;
  bool bHideStatus = true;
  QFileInfo fileInfo;
  SourceFileEntry *pEntry;

  m_pComboChapters->clear();
  // Since we have also menus in the combo box we need to get the iCurrentEntry, which is the correct index
  // to the m_listSourceFileEntries - list.
  pEntry = getSelectedSourceEntry ( );
  if (pEntry)	{
    if (pEntry->listFileInfos.count() > 1)	{
      // Okay we found a movie SourceEntry with multiple source files.
      // So we should dislay m_pComboSourceFile, and m_pComboChapters
      // and we also ought to hide m_pComboChapter
      bHideStatus = false;
      if (m_pComboAction1->currentText() == QString ("jump"))	{
	m_pComboSourceFile->clear ();
	//for (t=0;t<(uint)pEntry->listFileInfos.count();t++)	{                    //ooo
        for (t=0;t<static_cast<uint> ( pEntry->listFileInfos.count() );t++)	{   //xxx
	  fileInfo.setFile(pEntry->listFileInfos[t]->qsFileName);
	  //m_pComboSourceFile->insertItem(fileInfo.fileName());	//ooo
	  m_pComboSourceFile->addItem(fileInfo.fileName());		//xxx
	}
	// Here we connect the button object with the SourceFileEntry from the list.
	m_pClonedButton->setSourceFileEntry(pEntry);
      }
      // Lets generate the text for m_pEditAction
      slotSourceFile(0);
    }
    else	{
      // Here we add all chapters possible ...
      if ( m_pComboAction1->currentText() == QString ("jump"))	{
	//for ( t=0; t<(uint)pEntry->listFileInfos[0]->listChapters.count(); t++ )                 //ooo
        for ( t=0; t<static_cast<uint> ( pEntry->listFileInfos[0]->listChapters.count() ); t++ )    //xxx
	  //m_pComboChapters->insertItem ( pEntry->listFileInfos[0]->listChapters[t] );		//ooo
	  m_pComboChapters->addItem ( pEntry->listFileInfos[0]->listChapters[t] );		//xxx
	if  ( pEntry->listFileInfos[0]->listChapters.count() < 1 ) 
	  //m_pComboChapters->insertItem ( QString ( "00:00:00.000" ) );	//ooo
	  m_pComboChapters->addItem ( QString ( "00:00:00.000" ) );		//xxx
	// Here we connect the button object with the SourceFileEntry from the list.
	m_pClonedButton->setSourceFileEntry(pEntry);
      }
      // Lets generate the text for m_pEditAction
      slotChapters(0);
    }
  }
  else
    slotChapters(0);
  // Finally we show or hide the appropriate combo boxes ...
  if (bHideStatus)	{
    m_pComboSourceFile->hide ( );
    m_pComboChapters2->hide  ( );
    m_pComboChapters->show   ( );
  }
  else	{	// The default is to show only one line of ComboBoxes ...
    m_pComboSourceFile->show ( );
    m_pComboChapters2->show  ( );
    m_pComboChapters->hide   ( );
  }

  initButtons ( m_pButtonObject );
}

void DialogButton::slotSourceFile(int)
{
	uint t;
	m_pComboChapters2->clear();
	SourceFileEntry *pEntry = getSelectedSourceEntry ();
	if (pEntry)	{
		// Note: Later on I want to hide the Chapters for Movies which are called from within the VMGM - Main Menu.
		// This is becaus eyou can not jump to chapters, only titles from the VMGM.
		SourceFileInfo *pInfo = getSelectedSourceFile ();
		if (!pInfo)
			pInfo = pEntry->listFileInfos[0];
		// Here we add all chapters possible ...
		if (m_pComboAction1->currentText() == QString ("jump"))	{
			//for (t=0;t<(uint)pInfo->listChapters.count();t++)                  //ooo
                        for (t=0;t<static_cast<uint> ( pInfo->listChapters.count() );t++)   //xxx
				//m_pComboChapters2->insertItem(pInfo->listChapters[t]);	//ooo
				m_pComboChapters2->addItem(pInfo->listChapters[t]);		//xxx
			if (pInfo->listChapters.count() < 1)
				//m_pComboChapters2->insertItem(QString ("00:00:00.000"));	//ooo
				m_pComboChapters2->addItem(QString ("00:00:00.000"));		//xxx
		}
	}
	// Okay does currently nothig but ...
	slotChapters2 ( 0 );
}

void DialogButton::slotChapters(int)
{
	uint t;
	int  iChapterCounter = 1;
	SourceFileEntry *pEntry = getSelectedSourceEntry ();
	// Okay this here is generating the text for m_pEditAction.
	QString qsAction1 = m_pComboAction1->currentText();
	QString qsAction2 = m_pComboSourceEntry->currentText();
	QString qsAction3 = m_pComboChapters->currentText();
	// Here we get the Chapter number of chapter (e.g. "00:00:01.200")
	if ( (!qsAction3.isEmpty()) && (pEntry) )	{
		//for (t=0;t<(uint)pEntry->listFileInfos[0]->listChapters.count();t++)	{                  //ooo
                for (t=0;t<static_cast<uint> ( pEntry->listFileInfos[0]->listChapters.count() );t++)	{  //xxx
			if (pEntry->listFileInfos[0]->listChapters[t] == qsAction3)	{
				iChapterCounter = t + 1;
				break;
			}
		}
		qsAction3 = QString (" chapter %1").arg(iChapterCounter);
	}
	else
		qsAction3 = QString (" menu 1");
	
	QString qsAction = qsAction2;
	// Here we check that the changes are not manually made by the user ...
	if (!qsAction1.isEmpty())	{
		qsAction = QString (" ")  + qsAction1 + QString (" ") + qsAction2 + qsAction3 + QString (" ; ");
		m_bAutomatic = true;
	}
	m_pEditAction->setText(qsAction);
	m_bAutomatic = false;
}

void DialogButton::slotChapters2(int)
{
	uint t;
	int  iChapterCounter = 1;
	QFileInfo fileInfo;
	SourceFileEntry *pEntry = getSelectedSourceEntry ( );
	SourceFileInfo  *pInfo  = NULL;
	// Okay this here is generating the text for m_pEditAction.
	QString qsAction1 = m_pComboAction1->currentText();
	QString qsAction2 = m_pComboSourceFile->currentText();
	QString qsAction3 = m_pComboChapters2->currentText();
	// Here we get the Chapter number of chapter (e.g. "00:00:01.200")
	if ( (!qsAction3.isEmpty()) && (pEntry) )	{
		// First we should see which sourceFile is selected ...
		pInfo = getSelectedSourceFile ();
		if (!pInfo)
			pInfo = pEntry->listFileInfos[0];
		//for (t=0;t<(uint)pInfo->listChapters.count();t++)	{	                //ooo
                for (t=0;t<static_cast<uint> ( pInfo->listChapters.count() );t++)	{	//xxx
			if (pInfo->listChapters[t] == qsAction3)	{
				iChapterCounter = t + 1;
				break;
			}
		}
		qsAction3 = QString (" chapter %1").arg(iChapterCounter);
	}
	else
		qsAction3 = QString (" menu 1");
	QString qsAction = QString (" ")  + qsAction1 + QString (" ") + qsAction2 + qsAction3 + QString (" ; ");
	m_bAutomatic = true;
	m_pEditAction->setText(qsAction);
	m_bAutomatic = false;
}

void DialogButton::slotHelpAction()
{
	MessageBox::warning(this, tr("Action - tag"), tr("If you make manual changes here they will be kept.\n"
		"If this is the case then the background color will change to orange.\n"
		"You can reverse these changes by using the drop down boxes above.\n"
		"This is an advanced feature and should be used with care.\n\n"
		"Please note that the syntax in the XML file is different to the one displayed here."), 
		QMessageBox::Ok, QMessageBox::NoButton);
}

void DialogButton::slotHelpGoto()
{
	MessageBox::warning(this, tr("Goto - attributes"), 
		tr("Here you can specify the button to goto when the user pushes\n"
		"the up/down/right/left buttons on the remote control.\n\n"
		"Note: dvdauthor will choose a logical default value for each of those cases."), 
		QMessageBox::Ok, QMessageBox::NoButton);
}

void DialogButton::slotHelp()
{
	MessageBox::warning(this, "ButtonDialog", "Cool you got a button !!!\n", 
		QMessageBox::Ok, QMessageBox::NoButton);
}

void DialogButton::slotTransition ( )
{
  // Add a transition from the menu to the Movie or the next menu.
  // every button can max hold 1 transition
  // Required parameters:
  if ( m_pClonedButton->getTransition (   )  )
       m_pClonedButton->setTransition ( NULL );
  else  {
    DialogButtonTrans dialog ( m_pClonedButton->getTransition ( ) );
    if ( dialog.exec ( ) == QDialog::Rejected )
      return;

    m_pClonedButton->setTransition ( dialog.getTransition ( ) );
  }
  initTransition ( ); //oooo
}

void DialogButton::slotMultiple ( )
{
  // Open the selection dialog of all available objects.
  if ( ! m_pClonedButton )
    return;

  DialogSelect theDialog  ( this );
  if ( ! theDialog.initMe ( m_pClonedButton->getPlayMultipleList ( ), m_pClonedButton->getLoopMultiple ( ) ) )
    return;  // No videos in the current project
  // has the same format as the m_qsAction ... 
  // jump+-+<SourceFileEntry>+-+<SourceFileInfo>+-+<Chapter>
  //  DialogPlayMultiple theDialog ( this );
  //  theDialog.initMe ( ... something ... );
  if ( theDialog.exec ( ) == QDialog::Accepted ) {
    QStringList list = theDialog.getSelected ( );
    if ( list.count ( ) > 0 ) {
      QString     qsAction   = list[0];
      //QStringList actionList = QStringList::split ( STRING_SEPARATOR, qsAction ); //ooo
      QStringList actionList = qsAction.split ( STRING_SEPARATOR ); //xxx
      for (uint t=0; t<(uint)m_listSourceFileEntries.count ( ); t++ )  {	//ox
	if ( m_listSourceFileEntries[t]->qsDisplayName == list[1] ) {
	  if ( m_listSourceFileEntries[t]->listFileInfos.count ( ) < 2 )
	    qsAction = actionList[0] + STRING_SEPARATOR + actionList[1] + STRING_SEPARATOR + actionList[3];
	}
      }
      m_pClonedButton->setAction ( qsAction );
      m_pClonedButton->setLoopMultiple ( theDialog.getLoop ( ) );
      if ( ! theDialog.getLoop ( ) ) {  // loop can not have a finalDestination.
	QString strFinalMenu;
	QString strFinal = theDialog.getFinalDestination ( );
	if ( strFinal != "-- None --" ) {
	  strFinalMenu = "call"STRING_SEPARATOR + strFinal + STRING_SEPARATOR;
	  list.append  ( strFinalMenu );
	}
      }
      initActionState ( );
    }
    m_pClonedButton->setPlayMultipleList( list );
    initMultiple ( );
  }   //oooo
  // Note buttonAction == first PlayMultiple
}

void DialogButton::slotAdvanced()
{
  QString m_pButtonAdvancedText = m_pButtonAdvanced->text ( );  //xxx
  
  //to remove shortcut "&"  //xxx
  if ( m_pButtonAdvancedText.indexOf("&") != -1 ) //xxx
    m_pButtonAdvancedText.remove(m_pButtonAdvancedText.indexOf("&"),1);    //xxx
    
  //if ( m_pButtonAdvanced->text ( ) == QString ( BUTTON_ADVANCED ) )  {    //ooo
  if ( m_pButtonAdvancedText == QString ( BUTTON_ADVANCED ) )  {            //xxx
       m_pGroupGeometry->show       ( );
       m_pGroupGoto->show           ( );
       m_pEditAction->show          ( );
       m_pButtonHelpAction->show    ( );
       m_pGroupSystemRegister->show ( );
       m_pButtonAdvanced->setText   ( BUTTON_BASIC );
  }
  else {
       m_pGroupGeometry->hide       ( );
       m_pGroupGoto->hide           ( );
       m_pEditAction->hide          ( );
       m_pButtonHelpAction->hide    ( );
       m_pGroupSystemRegister->hide ( );
       m_pButtonAdvanced->setText   ( BUTTON_ADVANCED );
  }
}

void DialogButton::setButtonCreation(bool bButtonCreation)
{
  m_bButtonCreation = bButtonCreation;
}

void DialogButton::slotChangeColor ( )
{ 
  QPushButton *arrayButtons[8] = { m_pButtonHColor1, m_pButtonHColor2, m_pButtonHColor3, m_pButtonHColor4, 
                                   m_pButtonSColor1, m_pButtonSColor2, m_pButtonSColor3, m_pButtonSColor4 };
  QRadioButton *arrayChecks[8] = { m_pRadioHColor1,  m_pRadioHColor2,  m_pRadioHColor3,  m_pRadioHColor4, 
                                   m_pRadioSColor1,  m_pRadioSColor2,  m_pRadioSColor3,  m_pRadioSColor4  };
  uint t, iIdx;
  QPushButton *pButton = NULL;
  for ( t=0; t<8; t++ ) {    
    //connect (arrayButtons[t],    SIGNAL ( clicked (      ) ), this, SLOT ( slotChangeColor (   ) ) );  
      
    //if ( arrayButtons[t]->hasMouse ( ) ) {    //ooo
    if ( arrayButtons[t]->isChecked ( ) ) {       //xxx  
      pButton = arrayButtons[t];
      arrayButtons[t]->setChecked(false);       //xxx
      break;
    }
  }

  if ( ! pButton )
    return;

  iIdx = t%4;

  MenuObject **pMasks      = m_pSelectedMask;
//  MenuObject **pOtherMasks = m_pHighlightedMask;
  bool bHighlighted   = ( t  ==  iIdx );
  Rgba oldColor = m_pgcColors[iIdx];
  QRgb rgba;
  if ( iIdx == 0 ) {
    QColor transparentColor = QColorDialog::getColor ( oldColor.color ( ) );
    if ( ! transparentColor.isValid ( ) )   return;
    rgba = transparentColor.rgb     ( ) | 0x00000000;
  }
  else {
    bool bOkay    = false;
    rgba = QColorDialog::getRgba ( oldColor.rgb ( ), &bOkay );
    if ( ! bOkay ) 
      return;
  }

  if ( rgba == oldColor.rgb ( ) )
    return; // nothing has changed

  Rgba newColor ( rgba );
  m_pgcColors[iIdx] = newColor;
  if ( bHighlighted ) {
    m_iColorIdx[1] = iIdx;
    pMasks      = m_pHighlightedMask;
//    pOtherMasks = m_pSelectedMask;
  }
  else 
    m_iColorIdx[0] = iIdx;

  // Set both colors in Highlighted and Selected tab.
  //arrayButtons[iIdx]->setPaletteBackgroundColor ( newColor.color ( ) );   //ooo
  QPalette palette;							          //xxx
  palette.setBrush(arrayButtons[iIdx]->backgroundRole(), newColor.color ( ));	//xxx
  arrayButtons[iIdx]->setPalette(palette);					//xxx
  arrayChecks [  t ]->setChecked ( true ); // only set the check of the current mask color.
  iIdx += 4;
  //arrayButtons[iIdx]->setPaletteBackgroundColor ( newColor.color ( ) );   //ooo
  //QPalette palette_2;							          //xxx
  palette.setBrush(arrayButtons[iIdx]->backgroundRole(), newColor.color ( ));	//xxx
  arrayButtons[iIdx]->setPalette(palette);					//xxx
  // set the current mask to the new color
  setObjectColor ( pMasks[0], newColor );
  setObjectColor ( pMasks[1], newColor );

  // and finally we should see if the other mask is using the same color we just modified.
  if ( m_pClonedButton->getMaskColor ( ! bHighlighted ) == oldColor ) {
    pMasks = m_pHighlightedMask;
    if ( bHighlighted )
      pMasks = m_pSelectedMask;
    setObjectColor ( pMasks[0], newColor );
    setObjectColor ( pMasks[1], newColor );
  }
}

bool DialogButton::setObjectColor ( MenuObject *pObject, Rgba &newColor )
{
  if ( ! pObject )
    return false;

  if ( pObject->objectType ( ) == MASK_OBJECT ) {
    MaskObject  *pMask = (MaskObject *)pObject;
    pMask->setColor ( newColor );
  }
  else if ( pObject->objectType ( ) == FRAME_OBJECT ) {
    FrameObject *pFrame = (FrameObject *)pObject;
    pFrame->setFrameColor ( newColor );
  }
  else if ( pObject->objectType ( ) == TEXT_OBJECT ) {
    TextObject  *pText = (TextObject *)pObject;
    pText->setForegroundColor ( newColor );
  }
  else
    return false;
  return true;
}

void DialogButton::storeColorsInMenu ( )
{
  uint t, i;
  Rgba newColor;
  DVDMenu *pDVDMenu = Global::pApp->getCurrentSubMenu ( );
  if ( pDVDMenu ) {
    QList <ButtonObject *> listButtons = pDVDMenu->getButtons();	//oxx

    for  ( t=0 ;t<MAX_MASK_COLORS; t++ ) {
      if ( m_pgcColors[t] != m_pgcColorsOrig[t] ) {
	// Found a new color
	newColor = m_pgcColors[t];
	for  ( i=0; i<MAX_MASK_COLORS; i++ ) {
	  if ( t == i )
	    continue;
	  // Here we make sure that each of the 4 colors is unique.
	  if ( m_pgcColorsOrig[i] == newColor )	{
	    if ( newColor.red ( ) > 0 )
	      newColor.setRgb ( newColor.red ( ) - 1, newColor.green ( ), newColor.blue ( ), newColor.alpha ( ) );
	    else
	      newColor.setRgb ( newColor.red ( ) + 1, newColor.green ( ), newColor.blue ( ), newColor.alpha ( ) );
	  }
	}
	
	//for ( i=0; i<(uint)listButtons.count ( ); i++ )		        //ooo
	for ( i=0; i<static_cast<uint> ( listButtons.count ( ) ); i++ )		//xxx
	  listButtons[i]->replaceColor ( newColor, m_pgcColorsOrig[t] );
	// Next we set the color in the DVDMenuInterface (The most important one)
	pDVDMenu->getInterface ( )->pgcColors[t] = newColor;
	// And finally we set the color in the m_pColorToolbar of DVDAuthor
	Global::pApp->setColor ( t, newColor );
      }
    }
  }
}

void DialogButton::slotCancel ( )
{
  // This function handles only the case of the cancelation of the Button creation.
  // I.e. the user just created the Button and decides to cancel out.
  // In case the User cancels the ButtonDialog when the ButtonObject already 
  // existed before, nothing ought to be done here.
  if ( m_bButtonCreation )  {
    m_pButtonObject->deleteMasks  ( );
   
    m_pButtonObject->slotUnbutton ( );  
  }

  // Free memory ...
  //m_pClonedButton->removeSelected    ( (uint)0 );			//ooo
  m_pClonedButton->removeSelected    ( static_cast<uint> ( 0 ) );	//xxx
  //m_pClonedButton->removeHighlighted ( (uint)0 );			//ooo
  m_pClonedButton->removeHighlighted ( static_cast<uint> ( 0 ) );	//xxx

  for  ( uint t=0; t<2; t++ ) { 
    if ( m_pHighlightedMask[t] )
      delete m_pHighlightedMask[t];
    if ( m_pSelectedMask[t] )
      delete m_pSelectedMask[t];
  }

  //delete m_pClonedButton;	//oooo

  reject ( );
}

void DialogButton::slotOkay ( )
{
  // Here we take care of the information we obtained ...
  QString qsPreAction = m_pEditSystem->text        ( );
  QString qsAction    = m_pEditAction->text        ( );
  QString qsName      = m_pEditName->text          ( );
  QString qsUp        = m_pComboUp->currentText    ( );
  QString qsDown      = m_pComboDown->currentText  ( );
  QString qsRight     = m_pComboRight->currentText ( );
  QString qsLeft      = m_pComboLeft->currentText  ( );
 
  // if the user has manually changed the action, then we do not want to modify it at all.
  // Power to the user ...
  QPalette palette;
  //if ( m_pEditAction->paletteBackgroundColor ( ) != QColor ( MANUAL_CHANGE ) )  {	//ooo
  if ( palette.color (m_pEditAction->backgroundRole ( ) ) != QColor ( MANUAL_CHANGE ) )  {
    // Next let us take care of the action. Remember we showed the user something like :
    // jump my_movie1.mpg chapter 3
    // this needs to convert to something like ...
    // jump titleset 2 chapter 3
    SourceFileEntry *pEntry = getSelectedSourceEntry   ( );
    if ( pEntry && ( pEntry->listFileInfos.count ( ) > 1 ) )
      qsAction = m_pComboAction1->currentText ( ) + QString (STRING_SEPARATOR) +
    m_pComboSourceEntry->currentText( ) + QString (STRING_SEPARATOR) +
    m_pComboSourceFile->currentText ( ) + QString (STRING_SEPARATOR) +
    m_pComboChapters2->currentText  ( );
    else
      qsAction = m_pComboAction1->currentText ( ) + QString (STRING_SEPARATOR) +
    m_pComboSourceEntry->currentText ( ) + QString (STRING_SEPARATOR) +
    m_pComboChapters->currentText    ( );
  }

  uint t, iIdx;
  //storeColorsInMenu ( );	//oooo

  // set the Highlighted / Selected masks ...
  iIdx = ( m_pRadioHighlightedFrame->isChecked ( ) ) ? 0 : 1;
  if ( m_pHighlightedMask[iIdx] )  {
    //m_pClonedButton->removeHighlighted ( (uint)0 );   //ooo
    m_pClonedButton->removeHighlighted ( static_cast<uint> ( 0 ) );  
    m_pClonedButton->appendHighlighted ( m_pHighlightedMask[iIdx] );
    m_pHighlightedMask[iIdx] = NULL;
  }
  iIdx = ( m_pRadioSelectedFrame->isChecked ( ) ) ? 0 : 1;
  if ( m_pSelectedMask[iIdx] )  {
    //m_pClonedButton->removeSelected ( (uint)0 );                  //ooo
    m_pClonedButton->removeSelected ( static_cast<uint> ( 0 ) );    //xxx
    m_pClonedButton->appendSelected ( m_pSelectedMask[iIdx] );
    m_pSelectedMask[iIdx] = NULL;
  }

  for ( t=0; t<2; t++ ) {
    if ( m_pHighlightedMask[t] )
      delete m_pHighlightedMask[t];
    if ( m_pSelectedMask[t] )
      delete m_pSelectedMask[t];
  }

  m_pClonedButton->setPreAction ( qsPreAction );

  m_pClonedButton->setAction    ( qsAction    );

  m_pClonedButton->setName      ( qsName      );
  //  m_pClonedButton->setLoopMultiple( m_pCheckLoop->isChecked  ( ) );
  m_pClonedButton->setNext      ( NEXT_BUTTON_UP,    qsUp    );
  m_pClonedButton->setNext      ( NEXT_BUTTON_DOWN,  qsDown  );
  m_pClonedButton->setNext      ( NEXT_BUTTON_LEFT,  qsLeft  );
  m_pClonedButton->setNext      ( NEXT_BUTTON_RIGHT, qsRight );

  // Here we associate the new SourceFileEntry to the Cloned Button ...
  m_pClonedButton->setSourceFileEntry(NULL);
  //for (t=0;t<(uint)m_listSourceFileEntries.count();t++)		         //ooo
  for (t=0;t<static_cast<uint> ( m_listSourceFileEntries.count() );t++)		//xxx
    if (m_listSourceFileEntries[t]->qsDisplayName == m_pComboSourceEntry->currentText())
      m_pClonedButton->setSourceFileEntry(m_listSourceFileEntries[t]);

  // We should only touch the rect if the user has indeed changed something ...	
  QRect rect (m_pEditX->text().toInt(), m_pEditY->text().toInt(), m_pEditWidth->text().toInt(), m_pEditHeight->text().toInt());
  if ( rect != m_pButtonObject->getNormal ( 0 )->boundingRect ( ) )  {
    m_pClonedButton->setRect ( rect );
    for ( t=0; t<m_pButtonObject->getNormalCount ( );    t++ )
      m_pButtonObject->getNormal      ( t )->setRect ( rect  );
//    for ( t=0; t<m_pClonedButton->getNormalCount ( );    t++ )
//      m_pClonedButton->getNormal      ( t )->setRect ( rect  );
    for ( t=0; t<m_pClonedButton->getHighlightedCount( );t++ )
      m_pClonedButton->getHighlighted ( t )->setRect ( rect  );
    for ( t=0; t<m_pClonedButton->getSelectedCount   ( );t++ )
      m_pClonedButton->getSelected    ( t )->setRect ( rect  );
  }

  MenuObject *pObject = NULL;
  // We want to preserve the original object to keep Undo functionality intact.
  QList<MenuObject *> list;		//oxx
  for ( t=0; t<m_pButtonObject->getNormalCount ( ); t++ )  {
    pObject =  m_pButtonObject->getNormal ( t );
    list.append ( pObject );
  }
 
  for ( t=0; t<m_pButtonObject->getNormalCount ( ); t++ )
    //m_pButtonObject->removeNormal ( (uint)0 );                    //ooo
    m_pButtonObject->removeNormal ( static_cast<uint> ( 0 ) );      //xxx

  // And last but not least we equalize the original with the cloned brother ...
  *m_pButtonObject = *m_pClonedButton;	//oooo

//  pObject = m_pButtonObject->getNormal ( 0 );
//  if ( ( list.count ( ) > 0 ) && ( list[0]->objectType ( ) == pObject->objectType ( ) ) )  {
//    if ( ( pObject->objectType ( ) == IMAGE_OBJECT ) ||
//         ( pObject->objectType ( ) == MOVIE_OBJECT ) )  {
      // The only object parameter that can be changed here.
//      ImageObject *pImageObject1 = (ImageObject *)pObject;
//      ImageObject *pImageObject2 = (ImageObject *)list[0];

//      pImageObject2->setHidden ( m_pCheckHide->isOn ( ) );
//    }
//  }

  if ( list.count ( ) > 0 )  {
    pObject = list[0];
 
    if ( ( pObject->objectType ( ) == IMAGE_OBJECT ) ||
         ( pObject->objectType ( ) == MOVIE_OBJECT ) )  {
     
      // The only object parameter that can be changed here.
      ImageObject *pImageObject = (ImageObject *)pObject;
      //pImageObject->setHidden ( m_pCheckHide->isOn ( ) );	//ooo
      pImageObject->setHidden ( m_pCheckHide->isChecked ( ) );	//xxx
    }
  }	//oooo

  for ( t=0; t<m_pButtonObject->getNormalCount ( ); t++ )  {
    pObject =  m_pButtonObject->getNormal ( t );
    m_pClonedButton->removeNormal   ( pObject );
    pObject->disconnect ( );
    m_pMenuPreview->connectStdSlots ( pObject, true );
  }

  for (t=0; t<m_pButtonObject->getHighlightedCount ( ); t++ )  {
    pObject = m_pButtonObject->getHighlighted( t );
    m_pClonedButton->removeHighlighted ( pObject );
    pObject->disconnect ( );
    m_pMenuPreview->connectStdSlots    ( pObject, true );
  }
 
  for (t=0; t<m_pButtonObject->getSelectedCount ( ); t++ )  {
    pObject = m_pButtonObject->getSelected ( t );
    m_pClonedButton->removeSelected  ( pObject );
    pObject->disconnect ( );
    m_pMenuPreview->connectStdSlots  ( pObject, true );
  }
  delete m_pClonedButton;

  // Then delete the newly created objects
  for ( t=0;  t<m_pButtonObject->getNormalCount  ( ); t++ )
    delete m_pButtonObject->removeNormal ( (uint)0 );
  // and copy the originals over
  //for ( t=0; t<(uint)list.count ( );  t++ )	                  //ooo
  for ( t=0; t<static_cast<uint> ( list.count ( ) );  t++ )	//xxx
    m_pButtonObject->appendNormal   ( list[t] );
  m_pButtonObject->reParentChildren ( );
	//oooo
//  VAROL
//  pObject = m_pButtonObject->getNormal ( 0 );
//    if ( ( pObject->objectType ( ) == IMAGE_OBJECT ) ||
//         ( pObject->objectType ( ) == MOVIE_OBJECT ) )  {
      // The only object parameter that can be changed here.
//      ImageObject *pImageObject1 = (ImageObject *)pObject;
//    }


  m_pFramePreviewNormal->clearObjectList ( );
  m_pFrameHighlighted->clearObjectList   ( );		//oooo
  m_pFrameSelected->clearObjectList      ( );		//oooo

  emit   ( signalUpdateStructure ( ) );
  emit   ( signalUpdatePixmap    ( ) );

  ButtonTransition *pTransition = m_pButtonObject->getTransition ( );
  if ( pTransition )  { // Initialize the transitions name, and video file name
    pTransition->bFinishedRendering = false;
    DVDMenu *pDVDMenu     = Global::pApp->getCurrentSubMenu ( );
    if ( pDVDMenu )  {
      Utils theUtils;
      QString qsTempPath   = theUtils.getTempFile ( "" );
      CDVDMenuInterface    * pInterface = pDVDMenu->getInterface ( );
      QString qsName       = pInterface->qsMenuName + "/" + m_pButtonObject->name ( );
      pTransition->iFormat = pInterface->iFormat;
      pTransition->qsName  = qsName; // E.g. "Main VMGM/Button 1"
      if ( ! pTransition->bUserFile )  {
        theUtils.recMkdir   ( qsTempPath + qsName );
        qsName += "/transition.vob";
        pTransition->qsTransitionVideoFileName = qsTempPath + qsName; // E.g. "/tmp/TestProjec/Main VMGM/Button 1/transition.vob"
      }
    }
  }

  if ( m_bButtonCreation )  {
    // This button was just created. Let us add an UndoObject
    DVDMenu *pMenu = Global::pApp->getCurrentSubMenu ( );
    if ( pMenu )
         pMenu->getUndoBuffer ( )->push  ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_BUTTON, m_pButtonObject, m_pButtonObject->getNormal ( 0 ) ) );
  }	//oooo

  accept ( );
}
