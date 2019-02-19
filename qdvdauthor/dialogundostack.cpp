/***************************************************************************
    dialogundostack.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QPushButton>

#include "dialogundostack.h"
#include "dvdmenuundoobject.h"

bool DialogUndoStack::Item::m_bStartAdjust = true;

//DialogUndoStack::Item::Item ( Q3ListView *pListView, int iCounter, bool bAlt, QString qsType, QString qsName )	//ooo
DialogUndoStack::Item::Item ( QTreeWidget *pListView, int iCounter, bool bAlt, QString qsType, QString qsName )		//xxx
  //: Q3ListViewItem ( pListView, qsType, "", qsName )	//ooo
  : QTreeWidgetItem ( pListView )			//xxx
{
  /*m_bAlternateColor = bAlt;
  m_iCounter        = iCounter;
  if ( iCounter != 0 )
    setExpandable ( true );*/	//oooo
}

//DialogUndoStack::Item::Item ( Q3ListViewItem *pItem, int iCounter, bool bAlt, QString qsType, QString qsName )	//ooo
DialogUndoStack::Item::Item ( QTreeWidgetItem *pItem, int iCounter, bool bAlt, QString qsType, QString qsName )		//xxx
  //: Q3ListViewItem ( pItem, qsType, QString ( "%1" ).arg ( iCounter ), qsName )	//ooo
  : QTreeWidgetItem ( pItem )								//xxx
{
  /*m_bAlternateColor = bAlt;
  m_iCounter        = iCounter;*/	//oooo
}

DialogUndoStack::Item::~Item ( )
{
}

//DialogUndoStack::DialogUndoStack ( UndoBuffer *pUndoBuffer, QWidget* parent, const char* name, bool, Qt::WFlags fl)		//ooo
DialogUndoStack::DialogUndoStack ( UndoBuffer *pUndoBuffer, QWidget* parent, const char* name, bool, Qt::WindowFlags fl)	//xxx
    //: QDialog ( parent, name, FALSE, fl )	//ooo
    : QDialog ( parent )			//xxx
{
  m_pUndoBuffer = pUndoBuffer;
  m_pBaseItem   = NULL;
  m_iCounter    = 0;
  if ( !name )
    //setName ( "DialogUndoStack" );		//ooo
    setObjectName ( "DialogUndoStack" );	//xxx
  //setSizeGripEnabled ( TRUE );	//ooo
  setSizeGripEnabled ( true );		//xxx
  //DialogUndoStackLayout = new Q3GridLayout( this, 1, 1, 11, 6, "DialogUndoStackLayout");	//ooo
  DialogUndoStackLayout = new QGridLayout( this );						//xxx

  //Layout1 = new Q3HBoxLayout( 0, 0, 6, "Layout1");	//ooo
  Layout1 = new QHBoxLayout( this ); 			//xxx

  //buttonHelp = new QPushButton( this, "buttonHelp" );	//ooo
  buttonHelp = new QPushButton( "buttonHelp", this );	//xxx
  //buttonHelp->setAutoDefault( TRUE );			//ooo
  buttonHelp->setAutoDefault( true );			//xxx
  Layout1->addWidget( buttonHelp );
  Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( Horizontal_Spacing2 );

  //buttonOk = new QPushButton( this, "buttonOk" );	//ooo
  buttonOk = new QPushButton( "buttonOk", this );	//xxx
  //buttonOk->setAutoDefault( TRUE );			//ooo
  buttonOk->setAutoDefault( true );			//xxx
  //buttonOk->setDefault( TRUE );			//ooo
  buttonOk->setDefault( true );				//xxx
  Layout1->addWidget( buttonOk );

  //buttonCancel = new QPushButton( this, "buttonCancel" );	//ooo
  buttonCancel = new QPushButton( "buttonCancel", this );	//xxx
  //buttonCancel->setAutoDefault( TRUE );	//ooo
  buttonCancel->setAutoDefault( true );		//xxx
  Layout1->addWidget( buttonCancel );

  DialogUndoStackLayout->addLayout( Layout1, 1, 0 );

  //m_pListViewUndo = new Q3ListView ( this, "m_pListViewUndo" );	//ooo
  m_pListViewUndo = new QListView ( this );				//xxx
  /*m_pListViewUndo->setSorting ( -1 );
  m_pListViewUndo->addColumn  ( tr ( "Type"   ) );
  m_pListViewUndo->addColumn  ( tr ( "Nr"     ) );
  m_pListViewUndo->addColumn  ( tr ( "Object" ) );
  m_pListViewUndo->setRootIsDecorated  ( TRUE );*/	//ooo

  DialogUndoStackLayout->addWidget( m_pListViewUndo, 0, 0 );
  languageChange();
  resize( QSize(200, 350).expandedTo(minimumSizeHint()) );
  //clearWState ( Qt::WState_Polished );		//ooo
  setAttribute(Qt::WA_WState_Polished, false);		//xxx
//    clearWState ( Qt::WStyle_StaysOnTop );
    // signals and slots connections
  //connect ( m_pListViewUndo, SIGNAL ( clicked ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotClickedOnList ( Q3ListViewItem *, const QPoint &, int ) ) );	//oooo
  connect ( buttonOk,     SIGNAL ( clicked ( ) ), this, SLOT ( accept ( ) ) );
  connect ( buttonCancel, SIGNAL ( clicked ( ) ), this, SLOT ( reject ( ) ) );
}

DialogUndoStack::~DialogUndoStack ( )
{
    // no need to delete child widgets, Qt does it all for us
}

void DialogUndoStack::languageChange ( )
{
  //setCaption             ( tr ( "Undo Stack" ) );		//ooo
  buttonCancel->setText  ( tr ( "&Cancel"    ) );
  buttonHelp->setText    ( tr ( "&Help"      ) );
  buttonOk->setText      ( tr ( "&OK"        ) );
  //buttonHelp->setAccel   ( QKeySequence ( tr ( "F1" ) ) );	//ooo
  //buttonOk->setAccel     ( QKeySequence ( QString::null ) );	//ooo
  //buttonCancel->setAccel ( QKeySequence ( QString::null ) );	//ooo
  //m_pListViewUndo->clear ( );					//ooo
}

void DialogUndoStack::setUndoBuffer ( UndoBuffer *pUndoBuffer )
{
  m_pUndoBuffer  = pUndoBuffer;
  //buildUndoTree ( );		//oooo
}
