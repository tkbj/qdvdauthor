/***************************************************************************
    dialogimage.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Form implementation generated from reading ui file 'dialogimage.ui'
    
****************************************************************************/

#include "dialogimage.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qslider.h>
//#include <q3frame.h>  //ooo
#include <QFrame>       //xxx
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>  //ooo
#include <QWhatsThis>       //xxx
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

#include <qlayout.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
//#include <q3scrollview.h> //ooo
#include <QScrollArea>      //xxx
#include <qdesktopwidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QMessageBox>		//xxx

//#include "../dialogimage.ui.h"	//ooo
//#include "dialogimage.ui.h"		//xxx

#include "imagepreview.h"
//#include "xml_slideshow.h"		//ooo
#include "../xml_slideshow.h"		//xxx


/*
 *  Constructs a DialogImage as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
//DialogImage::DialogImage( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )  //ooo
DialogImage::DialogImage( QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl )   //xxx
    //: QDialog( parent, name, modal, fl )  //ooo
    : QDialog( parent ) //xxx
{
  
    setupUi(this);	//xxx
  
/*    if ( !name )
	//setName( "DialogImage" );    //ooo
    setWindowTitle( "DialogImage" ); //xxx
    //setPaletteBackgroundColor( QColor( 96, 136, 238 ) );  //ooo
    QPalette palette;							//xxx
    palette.setBrush(this->backgroundRole(), QColor( 96, 136, 238 ));		//xxx
    this->setPalette(palette);						//xxx
    //setSizeGripEnabled( TRUE );   //ooo
    setSizeGripEnabled( true );     //xxx
    //DialogImageLayout = new Q3GridLayout( this, 1, 1, 11, 6, "DialogImageLayout");    //ooo
    DialogImageLayout = new QGridLayout( this );   //xxx

    //layout5 = new Q3VBoxLayout( 0, 0, 6, "layout5");  //ooo
    layout5 = new QVBoxLayout( );  //xxx

    //textLabel5 = new QLabel( this, "textLabel5" );    //ooo
    textLabel5 = new QLabel( this );    //xxx
    textLabel5->setFrameShape( QLabel::NoFrame );
    textLabel5->setFrameShadow( QLabel::Plain );
    layout5->addWidget( textLabel5 );

    //m_pSliderRotate = new QSlider( this, "m_pSliderRotate" ); //ooo
    m_pSliderRotate = new QSlider( this );  //xxx
    m_pSliderRotate->setOrientation( Qt::Horizontal );
    layout5->addWidget( m_pSliderRotate );

    DialogImageLayout->addLayout( layout5, 0, 1 );

    //m_pFrameImage = new Q3Frame( this, "m_pFrameImage" ); //ooo
    m_pFrameImage = new QFrame( this ); //xxx
    m_pFrameImage->setFrameShape( QFrame::StyledPanel );
    m_pFrameImage->setFrameShadow( QFrame::Raised );

    //DialogImageLayout->addMultiCellWidget( m_pFrameImage, 1, 3, 1, 1 );   //ooo
    DialogImageLayout->addWidget( m_pFrameImage, 1, 3, 1, 1 );  //xxx

    //layout4 = new Q3HBoxLayout( 0, 0, 6, "layout4");  //ooo
    layout4 = new QHBoxLayout( ); //xxx

    //textLabel4 = new QLabel( this, "textLabel4" );    //ooo
    textLabel4 = new QLabel( this );    //xxx
    layout4->addWidget( textLabel4 );

    //m_pSliderShearY = new QSlider( this, "m_pSliderShearY" ); //ooo
    m_pSliderShearY = new QSlider( this );  //xxx
    m_pSliderShearY->setOrientation( Qt::Vertical );
    layout4->addWidget( m_pSliderShearY );

    //DialogImageLayout->addMultiCellLayout( layout4, 1, 3, 0, 0 ); //ooo
    DialogImageLayout->addLayout( layout4, 1, 3, 0, 0 );       //xxx

    //line1 = new Q3Frame( this, "line1" ); //ooo
    line1 = new QFrame( this );    //xxx
    //line1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, line1->sizePolicy().hasHeightForWidth() ) ); //oooo
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    //DialogImageLayout->addMultiCellWidget( line1, 5, 5, 0, 5 );   //ooo
    DialogImageLayout->addWidget( line1, 5, 5, 0, 5 );  //xxx

    //frame5 = new Q3Frame( this, "frame5" );   //ooo
    frame5 = new QFrame( this );    //xxx
    frame5->setFrameShape( QFrame::StyledPanel );
    frame5->setFrameShadow( QFrame::Raised );
    //frame5Layout = new Q3GridLayout( frame5, 1, 1, 11, 6, "frame5Layout");    //ooo
    frame5Layout = new QGridLayout( frame5 );   //xxx

    //m_pComboTransformMode = new QComboBox( FALSE, frame5, "m_pComboTransformMode" );  //ooo
    m_pComboTransformMode = new QComboBox( frame5 );    //xxx

    frame5Layout->addWidget( m_pComboTransformMode, 1, 0 );

    //textLabel1 = new QLabel( frame5, "textLabel1" );  //ooo
    textLabel1 = new QLabel( frame5 );  //xxx

    frame5Layout->addWidget( textLabel1, 0, 0 );

    //DialogImageLayout->addMultiCellWidget( frame5, 3, 4, 3, 5 );  //ooo
    DialogImageLayout->addWidget( frame5, 3, 4, 3, 5 ); //xxx

    //layout3 = new Q3HBoxLayout( 0, 0, 6, "layout3");  //ooo
    layout3 = new QHBoxLayout( );  //xxx

    //m_pSliderZoom = new QSlider( this, "m_pSliderZoom" ); //ooo
    m_pSliderZoom = new QSlider( this );    //xxx
    m_pSliderZoom->setOrientation( Qt::Vertical );
    layout3->addWidget( m_pSliderZoom );

    //textLabel2 = new QLabel( this, "textLabel2" );    //ooo
    textLabel2 = new QLabel( this );    //xxx
    layout3->addWidget( textLabel2 );

    //DialogImageLayout->addMultiCellLayout( layout3, 1, 3, 2, 2 ); //ooo
    DialogImageLayout->addLayout( layout3, 1, 3, 2, 2 );    //xxx

    //frame6 = new Q3Frame( this, "frame6" );   //ooo
    frame6 = new QFrame( this );   //xxx
    frame6->setFrameShape( QFrame::StyledPanel );
    frame6->setFrameShadow( QFrame::Raised );
    //frame6Layout = new Q3GridLayout( frame6, 1, 1, 11, 6, "frame6Layout");    //ooo
    frame6Layout = new QGridLayout( frame6 );   //xxx

    //m_pButtonBackground = new QPushButton( frame6, "m_pButtonBackground" );   //ooo
    m_pButtonBackground = new QPushButton( frame6 );    //xxx

    frame6Layout->addWidget( m_pButtonBackground, 1, 0 );

    //m_pCheckBackground = new QCheckBox( frame6, "m_pCheckBackground" );   //ooo
    m_pCheckBackground = new QCheckBox( frame6 );   //xxx

    frame6Layout->addWidget( m_pCheckBackground, 0, 0 );

    //DialogImageLayout->addMultiCellWidget( frame6, 0, 1, 3, 5 );  //ooo
    DialogImageLayout->addWidget( frame6, 0, 1, 3, 5 ); //xxx

    //layout6 = new Q3VBoxLayout( 0, 0, 6, "layout6");  //ooo
    layout6 = new QVBoxLayout( );   //xxx

    //m_pSliderShearX = new QSlider( this, "m_pSliderShearX" ); //ooo
    m_pSliderShearX = new QSlider( this );  //xxx
    m_pSliderShearX->setOrientation( Qt::Horizontal );
    layout6->addWidget( m_pSliderShearX );

    //textLabel3 = new QLabel( this, "textLabel3" );    //ooo
    textLabel3 = new QLabel( this );    //xxx
    layout6->addWidget( textLabel3 );

    DialogImageLayout->addLayout( layout6, 4, 1 );

    //layout7 = new Q3HBoxLayout( 0, 0, 6, "layout7");  //ooo
    layout7 = new QHBoxLayout( );   //xxx

    //buttonHelp = new QPushButton( this, "buttonHelp" );   //ooo
    buttonHelp = new QPushButton( this );   //xxx
    //buttonHelp->setAutoDefault( TRUE );   //ooo
    buttonHelp->setAutoDefault( true ); //xxx
    layout7->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 181, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( Horizontal_Spacing2 );

    //m_pLabelStatus = new QLabel( this, "m_pLabelStatus" );    //ooo
    m_pLabelStatus = new QLabel( this );    //xxx
    layout7->addWidget( m_pLabelStatus );

    //m_pEditStatus = new QLineEdit( this, "m_pEditStatus" );   //ooo
    m_pEditStatus = new QLineEdit( this );  //xxx
    //m_pEditStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, m_pEditStatus->sizePolicy().hasHeightForWidth() ) ); //oooo
    layout7->addWidget( m_pEditStatus );
    Horizontal_Spacing2_2 = new QSpacerItem( 181, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( Horizontal_Spacing2_2 );

    //buttonOk = new QPushButton( this, "buttonOk" );   //ooo
    buttonOk = new QPushButton( "buttonOk", this ); //xxx
    //buttonOk->setAutoDefault( TRUE ); //ooo
    buttonOk->setAutoDefault( true );   //xxx
    //buttonOk->setDefault( TRUE ); //ooo
    buttonOk->setDefault( true );   //xxx
    layout7->addWidget( buttonOk );

    //buttonCancel = new QPushButton( this, "buttonCancel" );   //ooo
    buttonCancel = new QPushButton( "buttonCancel", this ); //xxx
    //buttonCancel->setAutoDefault( TRUE ); //ooo
    buttonCancel->setAutoDefault( true );   //xxx
    layout7->addWidget( buttonCancel );

    //DialogImageLayout->addMultiCellLayout( layout7, 6, 6, 0, 5 ); //ooo
    DialogImageLayout->addLayout( layout7, 6, 6, 0, 5 );    //xxx

    //frame4 = new Q3Frame( this, "frame4" );   //ooo
    frame4 = new QFrame( ); //xxx
    frame4->setFrameShape( QFrame::StyledPanel );
    frame4->setFrameShadow( QFrame::Raised );
    //frame4Layout = new Q3GridLayout( frame4, 1, 1, 11, 6, "frame4Layout");    //ooo
    frame4Layout = new QGridLayout( frame4 );   //xxx

    //textLabel7 = new QLabel( frame4, "textLabel7" );  //ooo
    textLabel7 = new QLabel( frame4 );  //xxx

    frame4Layout->addWidget( textLabel7, 0, 1 );

    //textLabel8 = new QLabel( frame4, "textLabel8" );  //ooo
    textLabel8 = new QLabel( frame4 );  //xxx

    frame4Layout->addWidget( textLabel8, 0, 2 );

    //textLabel6 = new QLabel( frame4, "textLabel6" );  //ooo
    textLabel6 = new QLabel( frame4 );  //xxx

    frame4Layout->addWidget( textLabel6, 0, 0 );

    //m_pSliderRed = new QSlider( frame4, "m_pSliderRed" ); //ooo
    m_pSliderRed = new QSlider( frame4 );   //xxx
    m_pSliderRed->setOrientation( Qt::Vertical );

    frame4Layout->addWidget( m_pSliderRed, 1, 0 );

    //m_pSliderGreen = new QSlider( frame4, "m_pSliderGreen" ); //ooo
    m_pSliderGreen = new QSlider( frame4 ); //xxx
    m_pSliderGreen->setOrientation( Qt::Vertical );

    frame4Layout->addWidget( m_pSliderGreen, 1, 1 );

    //m_pSliderBlue = new QSlider( frame4, "m_pSliderBlue" );   //ooo
    m_pSliderBlue = new QSlider( frame4 );  //xxx
    m_pSliderBlue->setOrientation( Qt::Vertical );

    frame4Layout->addWidget( m_pSliderBlue, 1, 2 );

    DialogImageLayout->addWidget( frame4, 2, 4 );

    //frame4_3 = new Q3Frame( this, "frame4_3" );   //ooo
    frame4_3 = new QFrame( this ); //xxx
    frame4_3->setFrameShape( QFrame::StyledPanel );
    frame4_3->setFrameShadow( QFrame::Raised );
    //frame4_3Layout = new Q3GridLayout( frame4_3, 1, 1, 11, 6, "frame4_3Layout");  //ooo
    frame4_3Layout = new QGridLayout( frame4_3);    //xxx

    //m_pSliderBrightness = new QSlider( frame4_3, "m_pSliderBrightness" ); //ooo
    m_pSliderBrightness = new QSlider( frame4_3 );  //xxx
    m_pSliderBrightness->setOrientation( Qt::Vertical );

    frame4_3Layout->addWidget( m_pSliderBrightness, 0, 0 );

    //textLabel6_3 = new QLabel( frame4_3, "textLabel6_3" );    //ooo
    textLabel6_3 = new QLabel( frame4_3 );  //xxx

    frame4_3Layout->addWidget( textLabel6_3, 0, 1 );

    DialogImageLayout->addWidget( frame4_3, 2, 5 );

    //frame4_3_2 = new Q3Frame( this, "frame4_3_2" );   //ooo
    frame4_3_2 = new QFrame( this );   //xxx
    frame4_3_2->setFrameShape( QFrame::StyledPanel );
    frame4_3_2->setFrameShadow( QFrame::Raised );
    //frame4_3_2Layout = new Q3HBoxLayout( frame4_3_2, 11, 6, "frame4_3_2Layout");  //ooo
    frame4_3_2Layout = new QHBoxLayout( frame4_3_2 );   //xxx

    //textLabel6_3_2 = new QLabel( frame4_3_2, "textLabel6_3_2" );  //ooo
    textLabel6_3_2 = new QLabel( frame4_3_2 );  //xxx
    frame4_3_2Layout->addWidget( textLabel6_3_2 );

    //m_pSliderTransparency = new QSlider( frame4_3_2, "m_pSliderTransparency" );   //ooo
    m_pSliderTransparency = new QSlider( frame4_3_2 );  //xxx
    m_pSliderTransparency->setOrientation( Qt::Vertical );
    frame4_3_2Layout->addWidget( m_pSliderTransparency );

    DialogImageLayout->addWidget( frame4_3_2, 2, 3 );
    languageChange();
    resize( QSize(834, 569).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );			//ooo
    setAttribute(Qt::WA_WState_Polished, false);	//xxx
*/  //ooo
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_pSliderShearY, SIGNAL( valueChanged(int) ), this, SLOT( slotShearYValueChanged(int) ) );
    connect( m_pSliderShearX, SIGNAL( valueChanged(int) ), this, SLOT( slotShearXValueChanged(int) ) );
    connect( m_pSliderRotate, SIGNAL( valueChanged(int) ), this, SLOT( slotRotateValueChanged(int) ) );
    connect( m_pSliderZoom, SIGNAL( valueChanged(int) ), this, SLOT( slotZoomValueChanged(int) ) );
    connect( m_pSliderRed, SIGNAL( valueChanged(int) ), this, SLOT( slotRedValueChanged(int) ) );
    connect( m_pSliderGreen, SIGNAL( valueChanged(int) ), this, SLOT( slotGreenValueChanged(int) ) );
    connect( m_pSliderBlue, SIGNAL( valueChanged(int) ), this, SLOT( slotBlueValueChanged(int) ) );
    connect( m_pSliderBrightness, SIGNAL( valueChanged(int) ), this, SLOT( slotBrightnessValueChanged(int) ) );
    connect( m_pButtonBackground, SIGNAL( clicked() ), this, SLOT( slotButtonBackground() ) );
    connect( m_pSliderRotate, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderShearX, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderShearY, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderZoom, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderRotate, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderShearX, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderShearY, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderZoom, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pEditStatus, SIGNAL( textChanged(const QString&) ), this, SLOT( slotValueChanged(const QString&) ) );
    connect( m_pCheckBackground, SIGNAL( toggled(bool) ), this, SLOT( slotBackgroundToggled(bool) ) );
    connect( m_pSliderRed, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderGreen, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderBlue, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderRed, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderGreen, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderBlue, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderBrightness, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderBrightness, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderTransparency, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
    connect( m_pSliderTransparency, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );
    connect( m_pSliderTransparency, SIGNAL( valueChanged(int) ), this, SLOT( slotTransparencyValueChanged(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogImage::~DialogImage()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogImage::languageChange()
{
/*    //setCaption( tr( "Image Dialog" ) );   //ooo
    setWindowTitle( tr( "Image Dialog" ) );
    textLabel5->setText( tr( "<p align=\"center\">Rotate</p>" ) );
    textLabel4->setText( tr( "S\n"
"h\n"
"e\n"
"a\n"
"r\n"
"\n"
"Y" ) );
    m_pComboTransformMode->clear();
    //m_pComboTransformMode->insertItem( tr( "Point" ) );   //ooo
    m_pComboTransformMode->addItem( tr( "Point" ) );    //xxx
    //m_pComboTransformMode->insertItem( tr( "Aerea" ) );   //ooo
    m_pComboTransformMode->addItem( tr( "Aerea" ) );    //xxx
    textLabel1->setText( tr( "<p align=\"center\">Transform Mode </p>" ) );
    textLabel2->setText( tr( " Z\n"
" o\n"
" o\n"
"m" ) );
    m_pButtonBackground->setText( tr( "Background ..." ) );
    m_pCheckBackground->setText( tr( "Use Background" ) );
    textLabel3->setText( tr( "<p align=\"center\">S h e a r   X</p>" ) );
    buttonHelp->setText( tr( "&Help" ) );
    //buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );   //ooo
    m_pLabelStatus->setText( tr( "Zoom" ) );
    buttonOk->setText( tr( "&OK" ) );
    //buttonOk->setAccel( QKeySequence( QString::null ) );  //ooo
    buttonCancel->setText( tr( "&Cancel" ) );
    //buttonCancel->setAccel( QKeySequence( QString::null ) );  //ooo
    textLabel7->setText( tr( "G" ) );
    textLabel8->setText( tr( "B" ) );
    textLabel6->setText( tr( "R" ) );
    textLabel6_3->setText( tr( "B\n"
"r\n"
"i\n"
"g\n"
"h\n"
"t\n"
"n\n"
"e\n"
"s\n"
"s\n"
"" ) );
    textLabel6_3_2->setText( tr( "T\n"
"r\n"
"a\n"
"n\n"
"s\n"
"p\n"
"a\n"
"r\n"
"e\n"
"c\n"
"y" ) );*/  //ooo
}

void DialogImage::initMe(void *pImg, QString &backgroundFileName)
{
	CXmlSlideshow::img_struct *pImage = (CXmlSlideshow::img_struct *)pImg;
    
	m_pSliderRotate->setRange      (-3600, 3600 );
	m_pSliderShearX->setRange      (-1000, 1000 );
	m_pSliderShearY->setRange      (-1000, 1000 );
	m_pSliderZoom->setRange        (-1000, 1000 );
	m_pSliderRed->setRange         (    0, 1000 );
	m_pSliderGreen->setRange       (    0, 1000 );
	m_pSliderBlue->setRange        (    0, 1000 );
	m_pSliderTransparency->setRange(    0, 1000 );
	m_pSliderBrightness->setRange  (-1000, 1000 );

	m_pSliderRotate->setValue       ( 0 );
	m_pSliderShearX->setValue       ( 0 );
	m_pSliderShearY->setValue       ( 0 );
	m_pSliderZoom->setValue         ( 0 );
	m_pSliderRed->setValue          ( 0 );
	m_pSliderGreen->setValue        ( 0 );
	m_pSliderBlue->setValue         ( 0 );
	m_pSliderTransparency->setValue ( 0 );
	m_pSliderBrightness->setValue   ( 0 );

	m_pLabelStatus->setText (QString (""));
	//Q3GridLayout *pLayout = new Q3GridLayout (m_pFrameImage, 1, 1, 2, 2, "m_pFrameImageLayout"); //ooo
    QGridLayout *pLayout = new QGridLayout (m_pFrameImage); //xxx

	// Here we get the available screen resolution (all screens)
	// We want to keep the old style for resolutions above 1024x768 
	// But for lower res we need to use a scrollview to be able to
	// use QDVDAuthor.
	int iScreenWidth, iScreenHeight, iScreens, t;
	//Q3ScrollView *m_pScrollView = NULL;  //ooo
    QScrollArea *m_pScrollView = NULL;  //xxx
	QDesktopWidget desktop;
	iScreenWidth = iScreenHeight = 0;
	iScreens = desktop.numScreens();
	for (t=0;t<iScreens;t++)	{
		iScreenWidth  += desktop.screenGeometry(t).width();
		iScreenHeight += desktop.screenGeometry(t).height();
	}
	//uint iFlags = 0;		//ooo
//#if (QT_VERSION > 0x0301FF)		//ooo
//	iFlags = Qt::WNoAutoErase;
//#else
//	iFlags = Qt::WPaintClever;
//#endif
	
	Qt::WindowFlags iFlags = 0;	//xxx
	
	if ( (iScreenHeight <= 768) || (iScreenWidth <= 1024) )	{
		// Here we embed the MenuPreview into a ScrollView object ...
		//m_pScrollView = new Q3ScrollView( m_pFrameImage, "m_pScrollView", iFlags);  //ooo
        m_pScrollView = new QScrollArea( m_pFrameImage);    //xxx
		m_pImagePreview = new ImagePreview( m_pScrollView, "m_pPreview", iFlags);
		//m_pScrollView->addChild (m_pImagePreview);  //ooo
        m_pScrollView->setWidget (m_pImagePreview);   //xxx
		m_pImagePreview->resize(720, 480);
		pLayout->addWidget( m_pScrollView, 0, 0 );
	}
	else	{
		m_pImagePreview = new ImagePreview( m_pFrameImage, "m_pPreview", iFlags);
		pLayout->addWidget( m_pImagePreview, 0, 0 );
	}
	show ();

	// Here we init the vars ...
	bool bBackgroundToggleOn = false;
	m_pImagePreview->setBackground (backgroundFileName);
	m_pImagePreview->setImage( pImage );
//	m_pImagePreview->setBackground (backgroundFileName, true);
	m_pLabelStatus ->setText ( tr     ("Zoom"));
	m_pEditStatus  ->setText ( QString ("1.0"));

	if (!backgroundFileName.isEmpty())
		bBackgroundToggleOn = true;

	if (m_pImagePreview->getImage())	{
		ImageManipulator *pManipulator = (ImageManipulator *)m_pImagePreview->getImage()->pModifier;
		if (pManipulator)	{
			m_pSliderRotate->setValue	( (int)(m_pImagePreview->getImage()->rotate * 10) );
			m_pSliderShearX->setValue	( (int)(pManipulator->fShearX * 1000.0) );
			m_pSliderShearY->setValue	( (int)(pManipulator->fShearY * 1000.0) );
			m_pSliderZoom->setValue		( (int)(pManipulator->fZoom * 1000.0) - 1000);
			m_pSliderRed->setValue		( (int)(1000 - pManipulator->fRed * 1000.0) );
			m_pSliderGreen->setValue	( (int)(1000 - pManipulator->fGreen * 1000.0) );
			m_pSliderBlue->setValue		( (int)(1000 - pManipulator->fBlue * 1000.0) );
			m_pSliderTransparency->setValue	( (int)(pManipulator->fTransparency * 1000.0) );
			m_pSliderBrightness->setValue	( (int)( - pManipulator->fBrightness * 1000.0) );
//			bBackgroundToggleOn = pManipulator->bShowBackground;
			// Get the one stored in the structure rather then the global backgroundFileName
//			if ( ! pManipulator->backgroundFileName.isNull() );
//				m_pImagePreview->setBackground (pManipulator->backgroundFileName);
		}
	}
	if (bBackgroundToggleOn)	{
		m_pButtonBackground->setEnabled(true);
		m_pCheckBackground->setChecked (true);
	}
	else	{
		m_pButtonBackground->setEnabled(false);
		m_pCheckBackground->setChecked (false);
	}
}

void DialogImage::slotShearXValueChanged(int iValue)
{
	float fShearX = (float)iValue / 1000.0;
	m_pImagePreview->setShearX (fShearX);
	m_pLabelStatus->setText(tr ("ShearX"));
	m_pEditStatus->setText (QString ("%1").arg(fShearX));
}

void DialogImage::slotShearYValueChanged(int iValue)
{
	float fShearY = (float)iValue / 1000.0;
	m_pImagePreview->setShearY (fShearY);
	m_pLabelStatus->setText(tr ("ShearY"));
	m_pEditStatus->setText (QString ("%1").arg(fShearY));
}

void DialogImage::slotRotateValueChanged(int iValue)
{
	float fRotation = (float) iValue / 10.0;
	m_pImagePreview->setRotation (fRotation);
	m_pLabelStatus->setText(tr ("Rotate"));
	m_pEditStatus->setText (QString ("%1").arg(fRotation));
}
void DialogImage::slotTransparencyValueChanged(int iValue)
{
	float fTransparency = (float) iValue / 1000.0;
	m_pImagePreview->setTransparency (fTransparency);
	m_pLabelStatus->setText(tr ("Tarnsparency"));
	m_pEditStatus->setText (QString ("%1").arg(fTransparency));
}
void DialogImage::slotZoomValueChanged(int iValue)
{
	float fZoom = (float)(1000.0+iValue) / 1000.0;
	m_pImagePreview->setZoom (fZoom);
	m_pLabelStatus->setText(tr ("Zoom"));
	m_pEditStatus->setText (QString ("%1").arg(fZoom));
}
void DialogImage::slotBrightnessValueChanged(int iValue)
{
//	float fBrightness = (float)(1000.0+iValue) / 1000.0 - 1.0;
	float fBrightness = (float)iValue / 1000.0;
	m_pImagePreview->setBrightness (-fBrightness);
	m_pLabelStatus->setText(tr ("Brightness"));
	m_pEditStatus->setText (QString ("%1").arg(fBrightness));
}
void DialogImage::slotRedValueChanged(int iValue)
{
	float fRed = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setRed (fRed);
	m_pLabelStatus->setText(tr ("Red"));
	m_pEditStatus->setText (QString ("%1").arg(fRed));
}
void DialogImage::slotGreenValueChanged(int iValue)
{
	float fGreen = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setGreen (fGreen);
	m_pLabelStatus->setText(tr ("Green"));
	m_pEditStatus->setText (QString ("%1").arg(fGreen));
}
void DialogImage::slotBlueValueChanged(int iValue)
{
	float fBlue = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setBlue (fBlue);
	m_pLabelStatus->setText(tr ("Blue"));
	m_pEditStatus->setText (QString ("%1").arg(fBlue));
}

void DialogImage::slotSliderReleased()
{
	m_pImagePreview->setFastPreview(false);
}

void DialogImage::slotSliderPressed()
{
	m_pImagePreview->setFastPreview(true);
}

void DialogImage::slotValueChanged( const QString &newText )
{
	// The user typed in a new value in the status. Thus we need to check which value was changed and do the do ...
	float fNewValue = newText.toFloat();
	if (m_pLabelStatus->text() == tr("ShearX"))
		m_pSliderShearX->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("ShearY"))
		m_pSliderShearY->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Rotate"))
		m_pSliderRotate->setValue((int)(fNewValue * 10.0));
	else if (m_pLabelStatus->text() == tr("Zoom"))
		m_pSliderZoom->setValue((int)(fNewValue * 1000.0)-1000);
	else if (m_pLabelStatus->text() == tr("Transparency"))
		m_pSliderTransparency->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Brightness"))
		m_pSliderBrightness->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Red"))
		m_pSliderRed->setValue((int)(1000 - fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Green"))
		m_pSliderGreen->setValue((int)(1000 - fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Blue"))
		m_pSliderBlue->setValue((int)(1000 - fNewValue * 1000.0)); 
}

void DialogImage::slotButtonBackground ()
{
	QString qsFilter ("*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP");
	//QString qsBackground = Q3FileDialog::getOpenFileName("./", tr("Image Files (")+qsFilter+QString(")"), this,  //ooo
	//tr("Select background image"), tr("Select the background image."));  //ooo
    QStringList qsBackground = QFileDialog::getOpenFileNames ( NULL, QObject::tr("Select the background image."), "./", QObject::tr("Image Files (")+qsFilter+QString(")") );  //xxx
	//if (qsBackground.isNull())   //ooo
    if (qsBackground[0].isNull())   //xxx
		return;
	//m_pImagePreview->setBackground (qsBackground);   //ooo
    m_pImagePreview->setBackground (qsBackground[0]);  //xxx
}

void DialogImage::slotBackgroundToggled( bool bToggled )
{
	m_pButtonBackground->setEnabled(bToggled);
	m_pImagePreview->toggleBackground(bToggled);
}

void DialogImage::slotComboActivated(int iNewMode)
{
	m_pImagePreview->setTransformationMode(iNewMode);
}

void DialogImage::slotOk()
{
	// This'll set the ImageModifier - structure and tack it onto m_pImage.
	m_pImagePreview->getModifier();
	// This will call either ImagePreview::slotImageDialogDone, or ImageLine::slotImageDialogDone
	// Note : need to call setModifier within this object then for the changes to take effect.
	emit (signalDone (this));
	accept ();
}

ImagePreview *DialogImage::getPreview()
{
	return m_pImagePreview;
}


