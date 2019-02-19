/***************************************************************************
    uidialogcreate.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Form implementation generated from reading ui file 'uidialogcreate.ui'
    
****************************************************************************/

#include "uidialogcreate.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
//#include <q3textedit.h>   //ooo
#include <QTextEdit>        //xxx
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>  //ooo
#include <QWhatsThis>       //xxx
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

/*
 *  Constructs a uiDialogCreate as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
//uiDialogCreate::uiDialogCreate( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )    //ooo
uiDialogCreate::uiDialogCreate( QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl ) //xxx
    //: QDialog( parent, name, modal, fl )  //ooo
    : QDialog( parent ) //xxx
{
    if ( !name )
	//setName( "uiDialogCreate" ); //ooo
    setWindowTitle( "uiDialogCreate" ); //xxx
    //setPaletteBackgroundColor( QColor( 170, 170, 255 ) ); //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor( 170, 170, 255 )));	//xxx
    this->setPalette(palette);						//xxx
    //setSizeGripEnabled( TRUE );   //ooo
    setSizeGripEnabled( true ); //xxx
    //uiDialogCreateLayout = new Q3GridLayout( this, 1, 1, 11, 6, "uiDialogCreateLayout");  //ooo
    uiDialogCreateLayout = new QGridLayout( this );     //xxx

    //layout9 = new Q3HBoxLayout( 0, 0, 6, "layout9");  //ooo
    layout9 = new QHBoxLayout( );  //xxx

    //buttonHelp = new QPushButton( this, "buttonHelp" );   //ooo
    buttonHelp = new QPushButton( "buttonHelp", this ); //xxx
    //buttonHelp->setAutoDefault( TRUE );   //ooo
    buttonHelp->setAutoDefault( true ); //xxx
    layout9->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 70, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( Horizontal_Spacing2 );

    //m_pButtonBack = new QPushButton( this, "m_pButtonBack" ); //ooo
    m_pButtonBack = new QPushButton( "m_pButtonBack", this );   //xxx
    //m_pButtonBack->setAutoDefault( TRUE );    //ooo
    m_pButtonBack->setAutoDefault( true );  //xxx
    layout9->addWidget( m_pButtonBack );
    Horizontal_Spacing2_2 = new QSpacerItem( 70, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( Horizontal_Spacing2_2 );

    //buttonCancel = new QPushButton( this, "buttonCancel" );   //ooo
    buttonCancel = new QPushButton( "buttonCancel", this ); //xxx
    //buttonCancel->setAutoDefault( TRUE ); //ooo
    buttonCancel->setAutoDefault( true );   //xxx
    layout9->addWidget( buttonCancel );

    uiDialogCreateLayout->addLayout( layout9, 1, 0 );

    //m_pTabWidget = new QTabWidget( this, "m_pTabWidget" );    //ooo
    m_pTabWidget = new QTabWidget( this );  //xxx
    //m_pTabWidget->setPaletteBackgroundColor( QColor( 170, 170, 255 ) );
    QPalette palette2;								//xxx
    palette2.setBrush(QPalette::Base, QBrush(QColor( 170, 170, 255 )));	//xxx
    m_pTabWidget->setPalette(palette2);						//xxx

    //tab = new QWidget( m_pTabWidget, "tab" ); //ooo
    tab = new QWidget( m_pTabWidget );  //xxx
    //tabLayout = new Q3GridLayout( tab, 1, 1, 11, 6, "tabLayout"); //ooo
    tabLayout = new QGridLayout( tab ); //xxx

    //m_pButtonGenerate = new QPushButton( tab, "m_pButtonGenerate" );  //ooo
    m_pButtonGenerate = new QPushButton( "m_pButtonGenerate", tab );    //xxx

    tabLayout->addWidget( m_pButtonGenerate, 2, 0 );

    //layout14 = new Q3HBoxLayout( 0, 0, 6, "layout14");    //ooo
    layout14 = new QHBoxLayout( );     //xxx

    //layout11 = new Q3VBoxLayout( 0, 0, 6, "layout11");    //ooo
    layout11 = new QVBoxLayout( );     //xxx

    //textLabel1 = new QLabel( tab, "textLabel1" ); //ooo
    textLabel1 = new QLabel( "textLabel1", tab );   //xxx
    //textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );   //ooo
    layout11->addWidget( textLabel1 );

    //textLabel2 = new QLabel( tab, "textLabel2" ); //ooo
    textLabel2 = new QLabel( "textLabel2", tab );   //xxx
    //textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );   //ooo
    textLabel2->setMinimumSize( QSize( 200, 0 ) );
    layout11->addWidget( textLabel2 );

    //textLabel3 = new QLabel( tab, "textLabel3" ); //ooo
    textLabel3 = new QLabel( "textLabel3", tab );   //xxx
    //textLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, textLabel3->sizePolicy().hasHeightForWidth() ) );   //ooo
    layout11->addWidget( textLabel3 );

    //textLabel1_2 = new QLabel( tab, "textLabel1_2" ); //ooo
    textLabel1_2 = new QLabel( "textLabel1_2", tab );   //xxx
    //textLabel1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, textLabel1_2->sizePolicy().hasHeightForWidth() ) );   //ooo
    layout11->addWidget( textLabel1_2 );

    //textLabel1_3 = new QLabel( tab, "textLabel1_3" ); //ooo
    textLabel1_3 = new QLabel( "textLabel1_3", tab );   //xxx
    //textLabel1_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, textLabel1_3->sizePolicy().hasHeightForWidth() ) );   //ooo
    layout11->addWidget( textLabel1_3 );
    layout14->addLayout( layout11 );

    //layout12 = new Q3VBoxLayout( 0, 0, 6, "layout12");    //ooo
    layout12 = new QVBoxLayout( );  //xxx

    //m_pEditTempDir = new QLineEdit( tab, "m_pEditTempDir" );  //ooo
    m_pEditTempDir = new QLineEdit( "m_pEditTempDir", tab );    //xxx
    layout12->addWidget( m_pEditTempDir );

    //m_pEditExecutableDir = new QLineEdit( tab, "m_pEditExecutableDir" );  //ooo
    m_pEditExecutableDir = new QLineEdit( "m_pEditExecutableDir", tab );    //xxx
    layout12->addWidget( m_pEditExecutableDir );

    //layout10 = new Q3HBoxLayout( 0, 0, 0, "layout10");    //ooo
    layout10 = new QHBoxLayout( );  //xxx

    //m_pEditOutputFileName = new QLineEdit( tab, "m_pEditOutputFileName" );    //ooo
    m_pEditOutputFileName = new QLineEdit( "m_pEditOutputFileName", tab );  //xxx
    layout10->addWidget( m_pEditOutputFileName );

    //textLabel1_4 = new QLabel( tab, "textLabel1_4" ); //ooo
    textLabel1_4 = new QLabel( "textLabel1_4", tab );   //xxx
    //textLabel1_4->setPaletteBackgroundColor( QColor( 191, 191, 191 ) );
    QPalette palette3;								//xxx
    palette3.setBrush(QPalette::Base, QBrush(QColor( 191, 191, 191 )));	//xxx
    textLabel1_4->setPalette(palette3);						//xxx
    layout10->addWidget( textLabel1_4 );
    layout12->addLayout( layout10 );

    //m_pEditSlideshowTitle = new QLineEdit( tab, "m_pEditSlideshowTitle" );    //ooo
    m_pEditSlideshowTitle = new QLineEdit( "m_pEditSlideshowTitle", tab );  //xxx
    layout12->addWidget( m_pEditSlideshowTitle );

    //m_pComboFormat = new QComboBox( FALSE, tab, "m_pComboFormat" );   //ooo
    m_pComboFormat = new QComboBox( tab );  //xxx
    layout12->addWidget( m_pComboFormat );
    layout14->addLayout( layout12 );

    tabLayout->addLayout( layout14, 0, 0 );
    spacer5_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer5_2, 1, 0 );
    //m_pTabWidget->insertTab( tab, QString::fromLatin1("") );  //ooo
    m_pTabWidget->addTab( tab, QString::fromLatin1("") );   //xxx

    //tab_2 = new QWidget( m_pTabWidget, "tab_2" ); //ooo
    tab_2 = new QWidget( m_pTabWidget );    //xxx
    //tabLayout_2 = new Q3VBoxLayout( tab_2, 11, 6, "tabLayout_2"); //ooo
    tabLayout_2 = new QVBoxLayout( tab_2);  //xxx

    //layout11_2 = new Q3HBoxLayout( 0, 0, 6, "layout11_2");    //ooo
    layout11_2 = new QHBoxLayout( );    //xxx

    //layout8 = new Q3VBoxLayout( 0, 0, 6, "layout8");  //ooo
    layout8 = new QVBoxLayout( );   //xxx

    //textLabel4 = new QLabel( tab_2, "textLabel4" );   //ooo
    textLabel4 = new QLabel( "textLabel4", tab_2 ); //xxx
    layout8->addWidget( textLabel4 );

    //textLabel6 = new QLabel( tab_2, "textLabel6" );   //ooo
    textLabel6 = new QLabel( "textLabel6", tab_2 ); //xxx
    textLabel6->setMinimumSize( QSize( 130, 0 ) );
    textLabel6->setTextFormat( Qt::RichText );
    //textLabel6->setAlignment( int( Qt::AlignVCenter ) );  //ooo
    textLabel6->setAlignment( Qt::AlignVCenter );   //xxx
    layout8->addWidget( textLabel6 );
    layout11_2->addLayout( layout8 );

    //layout10_2 = new Q3VBoxLayout( 0, 0, 6, "layout10_2");    //ooo
    layout10_2 = new QVBoxLayout( );    //xxx

    //layout9_2 = new Q3HBoxLayout( 0, 0, 6, "layout9_2");  //ooo
    layout9_2 = new QHBoxLayout( );     //xxx

    //m_pEditImageNumber = new QLineEdit( tab_2, "m_pEditImageNumber" );    //ooo
    m_pEditImageNumber = new QLineEdit( "m_pEditImageNumber", tab_2 );  //xxx
    //m_pEditImageNumber->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, m_pEditImageNumber->sizePolicy().hasHeightForWidth() ) );   //ooo
    m_pEditImageNumber->setMaximumSize( QSize( 70, 32767 ) );
    //m_pEditImageNumber->setPaletteBackgroundColor( QColor( 170, 170, 255 ) ); //xxx
    QPalette palette4;								//xxx
    palette4.setBrush(QPalette::Base, QBrush(QColor( 170, 170, 255 )));	//xxx
    m_pEditImageNumber->setPalette(palette4);						//xxx
    //m_pEditImageNumber->setReadOnly( TRUE );  //ooo
    m_pEditImageNumber->setReadOnly( true );    //xxx
    layout9_2->addWidget( m_pEditImageNumber );

    //textLabel5 = new QLabel( tab_2, "textLabel5" );   //ooo
    textLabel5 = new QLabel( "textLabel5", tab_2 ); //xxx
    layout9_2->addWidget( textLabel5 );

    //m_pEditTotalNumber = new QLineEdit( tab_2, "m_pEditTotalNumber" );    //ooo
    m_pEditTotalNumber = new QLineEdit( "m_pEditTotalNumber", tab_2 );  //xxx
    //m_pEditTotalNumber->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, m_pEditTotalNumber->sizePolicy().hasHeightForWidth() ) );   //ooo
    m_pEditTotalNumber->setMaximumSize( QSize( 70, 32767 ) );
    //m_pEditTotalNumber->setPaletteBackgroundColor( QColor( 170, 170, 255 ) ); //ooo
    QPalette palette5;								//xxx
    palette5.setBrush(QPalette::Base, QBrush(QColor( 170, 170, 255 )));	//xxx
    m_pEditTotalNumber->setPalette(palette5);						//xxx
    //m_pEditTotalNumber->setReadOnly( TRUE );  //ooo
    m_pEditTotalNumber->setReadOnly( true );
    layout9_2->addWidget( m_pEditTotalNumber );
    spacer4 = new QSpacerItem( 70, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9_2->addItem( spacer4 );
    layout10_2->addLayout( layout9_2 );

    //m_pEditCurrentFileName = new QLineEdit( tab_2, "m_pEditCurrentFileName" );    //ooo
    m_pEditCurrentFileName = new QLineEdit( "m_pEditCurrentFileName", tab_2 );  //xxx
    //m_pEditCurrentFileName->setPaletteBackgroundColor( QColor( 170, 170, 255 ) ); //ooo
    QPalette palette6;								//xxx
    palette6.setBrush(QPalette::Base, QBrush(QColor( 170, 170, 255 )));	//xxx
    m_pEditCurrentFileName->setPalette(palette6);						//xxx
    //m_pEditCurrentFileName->setReadOnly( TRUE );  //ooo
    m_pEditCurrentFileName->setReadOnly( true );    //xxx
    layout10_2->addWidget( m_pEditCurrentFileName );
    layout11_2->addLayout( layout10_2 );
    tabLayout_2->addLayout( layout11_2 );

    //m_pTextOutput = new Q3TextEdit( tab_2, "m_pTextOutput" ); //ooo
    m_pTextOutput = new QTextEdit( "m_pTextOutput", tab_2 );    //xxx
    //m_pTextOutput->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 4, m_pTextOutput->sizePolicy().hasHeightForWidth() ) ); //ooo
    //m_pTextOutput->setPaletteForegroundColor( QColor( 24, 189, 19 ) );		//ooo
    QPalette palette7;								//xxx
    palette7.setBrush(m_pTextOutput->backgroundRole(), QColor ( 24, 189, 19 ));		//xxx
    m_pTextOutput->setPalette(palette7);							//xxx
    //m_pTextOutput->setPaletteBackgroundColor( QColor( 0, 0, 0 ) );
    QPalette palette8;								//xxx
    palette8.setBrush(QPalette::Base, QBrush(QColor( 0, 0, 0 )));	//xxx
    m_pTextOutput->setPalette(palette8);						//xxx
    //m_pTextOutput->setTextFormat( Qt::LogText );  //oooo
    m_pTextOutput->setReadOnly(true);   //xxx
    tabLayout_2->addWidget( m_pTextOutput );
    spacer6 = new QSpacerItem( 20, 5, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer6 );

    //m_pTextStatus = new QLabel( tab_2, "m_pTextStatus" ); //ooo
    m_pTextStatus = new QLabel( "m_pTextStatus", tab_2 );   //xxx
    tabLayout_2->addWidget( m_pTextStatus );

    //m_pProgress = new Q3ProgressBar( tab_2, "m_pProgress" );  //ooo
    m_pProgress = new QProgressBar( tab_2 );    //xxx
    tabLayout_2->addWidget( m_pProgress );
    //m_pTabWidget->insertTab( tab_2, QString::fromLatin1("") );    //ooo
    m_pTabWidget->addTab( tab_2, QString::fromLatin1("") ); //xxx

    uiDialogCreateLayout->addWidget( m_pTabWidget, 0, 0 );
    languageChange();
    resize( QSize(442, 378).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );			//ooo
    setAttribute(Qt::WA_WState_Polished, false);	//xxx

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_pButtonBack, SIGNAL( clicked() ), this, SLOT( slotBack() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
uiDialogCreate::~uiDialogCreate()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void uiDialogCreate::languageChange()
{
    //setCaption( tr( "Create Dialog" ) );  //ooo
    setWindowTitle( tr( "Create Dialog" ) );    //xxx
    buttonHelp->setText( tr( "&Help" ) );
    //buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );   //ooo
    m_pButtonBack->setText( tr( "&Back" ) );
    //m_pButtonBack->setAccel( QKeySequence( tr( "Alt+B" ) ) ); //ooo
    buttonCancel->setText( tr( "&Cancel" ) );
    //buttonCancel->setAccel( QKeySequence( QString::null ) );  //ooo
    m_pButtonGenerate->setText( tr( "Generate Slideshow" ) );
    textLabel1->setText( tr( "<p align=\"center\">Temp Dir :</p>" ) );
    textLabel2->setText( tr( "<p align=\"center\">dvd-slideshow dir :</p>" ) );
    textLabel3->setText( tr( "<p align=\"center\">Output File Name :</p>" ) );
    textLabel1_2->setText( tr( "<p align=\"center\">Slideshow Title :</p>" ) );
    textLabel1_3->setText( tr( "<p align=\"center\">Format :</p>" ) );
    m_pEditTempDir->setText( tr( "/tmp" ) );
    m_pEditExecutableDir->setText( tr( "/usr/bin" ) );
    m_pEditOutputFileName->setText( tr( "slideshow" ) );
    textLabel1_4->setText( tr( ".vob" ) );
    m_pEditSlideshowTitle->setText( tr( "My Slideshow" ) );
    m_pComboFormat->clear();
    //m_pComboFormat->insertItem( tr( "PAL" ) );    //ooo
    m_pComboFormat->addItem( tr( "PAL" ) ); //xxx
    //m_pComboFormat->insertItem( tr( "NTSC" ) );   //ooo
    m_pComboFormat->addItem( tr( "NTSC" ) );    //xxx
    //m_pTabWidget->changeTab( tab, tr( "Info" ) ); //ooo
    m_pTabWidget->setTabText( m_pTabWidget->indexOf(tab), tr( "Info" ) ); //xxx
    textLabel4->setText( tr( "<p align=\"center\">Step</p>" ) );
    textLabel6->setText( tr( "<p align=\"center\">Name</p>" ) );
    m_pEditImageNumber->setText( tr( "0" ) );
    textLabel5->setText( tr( "<p align=\"center\">of</p>" ) );
    m_pEditTotalNumber->setText( tr( "0" ) );
    m_pTextStatus->setText( tr( "<p align=\"center\">Processing Images</p>" ) );
    //m_pTabWidget->changeTab( tab_2, tr( "Progress" ) );   //xxx
    m_pTabWidget->setTabText( m_pTabWidget->indexOf(tab_2),tr( "Progress" ) ); //xxx
}

void uiDialogCreate::slotBack()
{
    qWarning( "uiDialogCreate::slotBack(): Not implemented yet" );
}

