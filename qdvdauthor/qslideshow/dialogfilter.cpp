/***************************************************************************
    dialogfilter.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Form implementation generated from reading ui file 'dialogfilter.ui'
    
****************************************************************************/

/*
TODO:
	o	implement the filter frame and functionality
	o	Button to view the generated dvd-slideshow input file.
	o	Button to view the generated xml-file.
	o	Add Subtitles (just like sound, or filter)
	o	Add button <Add Subtitle> and offer different image file informations (all, date only, name, name and date etc.)
	o	implement sound frame (I.e. display the sound waves).
	o	implement the add - button functionality.
	o	implement <Add Soundtrack> functionality.

Improve the Slideshow filter Dialog:
        - use slot to update the pixmap, and/or semaphores to protect against crashes.
        - After <ok> ask if task should run in background already converting imgs to mpeg.
        - If done display message box.
        - Use ProgressBar UI with or without text output of command line for the background task.
        - display total time as well as time range of the current slot.
        - manually sync Y pos of <add> buttons to the SoundFrame / FilterFrame height.
        - allow images to be moved/edited(rotated)/removed/inserted in the filter GUI.
        - import a dvd-slideshow file (done in the slideshow selector GUI).
*/
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
//#include <q3scrollview.h> //ooo
#include <QScrollArea>      //xxx
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qcombobox.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QVBoxLayout>

#include "dialogfilter.h"
#include "filterline.h"
#include "soundline.h"
#include "imageline.h"

/**
 *  Constructs a DialogFilter as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
//DialogFilter::DialogFilter( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )    //ooo
DialogFilter::DialogFilter( QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl ) //xxx
    //: QDialog( parent, name, modal, fl )  //ooo
    : QDialog( parent ) //xxx
{
	m_qsLastDirectory = QString ("./");
	m_iDeltaPadding = 23;
	if ( !name )
		//setName( "DialogFilter" );  //ooo
        setWindowTitle( "DialogFilter" );   //xxx
	//setPaletteBackgroundColor( QColor( 170, 85, 127 ) ); //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor( 170, 85, 127 ));			//xxx
	this->setPalette(palette);					//xxx
	//setSizeGripEnabled( TRUE );  //ooo
    setSizeGripEnabled( true ); //xxx
	//DialogFilterLayout = new Q3GridLayout( this, 1, 1, 11, 6, "DialogFilterLayout"); //ooo
    DialogFilterLayout = new QGridLayout( this );   //xxx

	// The progress bar.
	//m_pProgress = new Q3ProgressBar( this, "m_pProgress" );  //ooo
    m_pProgress = new QProgressBar( this ); //xxx
	//m_pProgress->setTotalSteps (MAX_PROGRESS);   //ooo
    m_pProgress->setValue (MAX_PROGRESS);   //xxx
	DialogFilterLayout->addWidget( m_pProgress, 1, 0 );
    
	//layout6 = new Q3HBoxLayout( 0, 0, 6, "layout6"); //ooo
    layout6 = new QHBoxLayout( );   //xxx

	//m_pButtonAddSoundTrack = new QPushButton( this, "m_pButtonAddSoundTrack" );  //ooo
    m_pButtonAddSoundTrack = new QPushButton( "m_pButtonAddSoundTrack", this ); //xxx
	layout6->addWidget( m_pButtonAddSoundTrack );
	QSpacerItem* spacer = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout6->addItem( spacer );

	//buttonHelp = new QPushButton( this, "buttonHelp" );  //ooo
    buttonHelp = new QPushButton( "buttonHelp", this ); //xxx
	//buttonHelp->setAutoDefault( TRUE );  //ooo
    buttonHelp->setAutoDefault( true ); //xxx
	layout6->addWidget( buttonHelp );
	QSpacerItem* spacer_2 = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout6->addItem( spacer_2 );

	//buttonOk = new QPushButton( this, "buttonOk" );  //ooo
    buttonOk = new QPushButton( "buttonOk", this ); //xxx
	//buttonOk->setAutoDefault( TRUE );    //ooo
    buttonOk->setAutoDefault( true );   //xxx
	//buttonOk->setDefault( TRUE );    //ooo
    buttonOk->setDefault( true );   //xxx
	layout6->addWidget( buttonOk );

//	buttonCancel = new QPushButton( this, "buttonCancel" );
//	buttonCancel->setAutoDefault( TRUE );
//	layout6->addWidget( buttonCancel );

	DialogFilterLayout->addLayout( layout6, 2, 0 );  //ooo
    //DialogFilterLayout->addLayout( layout6 );   //xxx
    
	//m_pFrame = new Q3Frame( this, "m_pFrame" );  //ooo
    m_pFrame = new QFrame( this );  //xxx
	m_pFrame->setMinimumSize( QSize( 600, 300 ) );
	//m_pFrame->setPaletteBackgroundColor( QColor( 170, 170, 255 ) );  //ooo
	QPalette palette2;	//xxx
	palette2.setBrush(QPalette::Base, QColor( 170, 170, 255 ));			//xxx
	m_pFrame->setPalette(palette2);					//xxx
	m_pFrame->setFrameShape( QFrame::StyledPanel );
	m_pFrame->setFrameShadow( QFrame::Raised );
	//m_pFrameLayout = new Q3GridLayout( m_pFrame, 1, 1, 11, 6, "m_pFrameLayout"); //ooo
    m_pFrameLayout = new QGridLayout( m_pFrame );  //xxx
	QSpacerItem* spacer_3 = new QSpacerItem( 20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
	m_pFrameLayout->addItem( spacer_3, 0, 0 );

	//m_pScrollView = new Q3ScrollView( m_pFrame, "m_pScrollView" );   //ooo
    m_pScrollView = new QScrollArea( m_pFrame );    //xxx
	m_pScrollView->setFrameShape( QFrame::StyledPanel );
	m_pScrollView->setFrameShadow( QFrame::Raised );
	//m_pScrollView->viewport()->setPaletteBackgroundColor( QColor( 170, 170, 255 ) ); //oooo

	//m_pScrollView->resizeContents(800, 250); //oooo
	//m_pScrollView->enableClipper(TRUE);  //oooo

	//m_pScrollFrame = new Q3Frame( m_pScrollView, "m_pScrollFrame" ); //ooo
    m_pScrollFrame = new QFrame( m_pScrollView );   //xxx
	m_pScrollFrame->resize (800, 265);

		// Add the one and only child to the scroll view, the csroll frame.
	//m_pScrollView->addChild(m_pScrollFrame); //ooo
    m_pScrollView->setWidget(m_pScrollFrame);   //xxx
	//m_pScrollView->moveChild(m_pScrollFrame, 0, 0);  //oooo

	//m_pScrollViewLayout = new Q3GridLayout( m_pScrollFrame, 1, 1, 11, 6, "m_pScrollViewLayout"); //ooo
    m_pScrollViewLayout = new QGridLayout( m_pScrollFrame );    //xxx

	// Here we create the image frames ...
	//layout60 = new Q3VBoxLayout( 0, 0, 6, "layout60");   //ooo
    layout60 = new QVBoxLayout( );  //xxx

	m_pImageLine = new ImageLine (m_pScrollFrame, "m_pImageLine");

	connect (m_pImageLine, SIGNAL(signalProgress(int)), this, SLOT(slotSetProgress (int)));

	layout60->addWidget( m_pImageLine );

	m_pSliderTime = new SliderTime( m_pScrollFrame, "m_pSliderTime" );
	m_pSliderTime->setMinimumSize( QSize( 600, 25 ) );
	//m_pSliderTime->setMouseTracking( TRUE ); //ooo
    m_pSliderTime->setMouseTracking( true );    //xxx
	m_pSliderTime->setOrientation( Qt::Horizontal );
	//m_pSliderTime->setTickmarks( QSlider::TicksRight );  //ooo
    m_pSliderTime->setTickPosition( QSlider::TicksRight ); //xxx

	layout60->addWidget( m_pSliderTime );
	m_pScrollViewLayout->addLayout( layout60, 0, 0 );    //ooo
    //m_pScrollViewLayout->addLayout( layout60 ); //xxx

	//layout48 = new Q3VBoxLayout( 0, 0, 6, "layout48");   //ooo
    layout48 = new QVBoxLayout( );  //xxx

	m_pFrameFilter = new FilterLine( m_pScrollFrame, "m_pFrameFilter" );
	layout48->addWidget( m_pFrameFilter );

//	m_pFrameSound = new QFrame( m_pScrollFrame, "m_pFrameSound" );
//	m_pFrameSound->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, m_pFrameSound->sizePolicy().hasHeightForWidth() ) );
//	m_pFrameSound->setMinimumSize( QSize( 600, LINE_HEIGHT ) );
//	m_pFrameSound->setLineWidth(3);
//	m_pFrameSound->setFrameShape( QFrame::WinPanel );
//	m_pFrameSound->setFrameShadow( QFrame::Sunken );

	m_pFrameSound = new SoundLine( m_pScrollFrame, "m_pFrameSound" );
	layout48->addWidget( m_pFrameSound );

	m_pScrollViewLayout->addLayout( layout48, 2, 0 );
	QSpacerItem* spacer_4 = new QSpacerItem( 20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
	m_pScrollViewLayout->addItem( spacer_4, 1, 0 );

	m_pFrameLayout->addWidget( m_pScrollView, 0, 1 );

	//layout13 = new Q3VBoxLayout( 0, 0, 6, "layout13");   //ooo
    layout13 = new QVBoxLayout( );  //xxx

	//frameRange = new Q3Frame( m_pFrame, "frameRange" );  //ooo
    frameRange = new QFrame( m_pFrame );    //xxx
	frameRange->setFrameShape( QFrame::StyledPanel );
	frameRange->setFrameShadow( QFrame::Raised );
	//frameRangeLayout = new Q3GridLayout( frameRange, 1, 1, 11, 6, "frameRangeLayout");   //ooo
    frameRangeLayout = new QGridLayout( frameRange );   //xxx
	//m_pComboRange = new QComboBox( FALSE, frameRange, "m_pComboRange" ); //ooo
    m_pComboRange = new QComboBox( frameRange );    //xxx
	frameRangeLayout->addWidget( m_pComboRange, 1, 0 );
	//m_pEditTotal = new QLabel( frameRange, "m_pEditTotal" ); //ooo
    m_pEditTotal = new QLabel( "m_pEditTotal", frameRange );    //xxx
	frameRangeLayout->addWidget( m_pEditTotal, 0, 0 );
	layout13->addWidget( frameRange );

	QSpacerItem* spacer_5 = new QSpacerItem( 20, 30, QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout13->addItem( spacer_5 );

	//textLabel3 = new QLabel( m_pFrame, "textLabel3" );   //ooo
    textLabel3 = new QLabel( "textLabel3", m_pFrame );  //xxx
	//textLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel3->sizePolicy().hasHeightForWidth() ) );  //oooo
	textLabel3->setMaximumSize( QSize( 130, 32767 ) );
	layout13->addWidget( textLabel3 );

	//m_pEditTime = new QLineEdit( m_pFrame, "m_pEditTime" );  //ooo
    m_pEditTime = new QLineEdit( "m_pEditTime", m_pFrame ); //xxx
	//m_pEditTime->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pEditTime->sizePolicy().hasHeightForWidth() ) );    //oooo
	m_pEditTime->setMaximumSize( QSize( 140, 32767 ) );
	//m_pEditTime->setPaletteForegroundColor( QColor( 255, 0, 0 ) );   //ooo
	QPalette palette3;	//xxx
	palette3.setBrush(QPalette::Base, QColor( 255, 0, 0 ));	//xxx
    m_pEditTime->setPalette(palette3);					//xxx
	//m_pEditTime->setPaletteBackgroundColor( QColor( 170, 170, 255 ) );   //ooo
	QPalette palette4;	//xxx
	palette4.setBrush(QPalette::Base, QColor( 170, 170, 255 ));			//xxx
	m_pEditTime->setPalette(palette4);					//xxx
	QFont m_pEditTime_font(  m_pEditTime->font() );
	m_pEditTime_font.setPointSize( 10 );
	//m_pEditTime_font.setBold( TRUE );    //ooo
    m_pEditTime_font.setBold( true );   //xxx
	m_pEditTime->setFont( m_pEditTime_font );
	//m_pEditTime->setReadOnly( TRUE );    //ooo
    m_pEditTime->setReadOnly( true );   //xxx
	layout13->addWidget( m_pEditTime );

	QSpacerItem* spacer7_2 = new QSpacerItem( 20, 15, QSizePolicy::Fixed, QSizePolicy::Fixed );
	layout13->addItem( spacer7_2 );

	//layout10 = new Q3HBoxLayout( 0, 0, 6, "layout10");    //ooo
    layout10 = new QHBoxLayout( );  //xxx

	//m_pButtonAddFilter = new QPushButton( m_pFrame, "m_pButtonAddFilter" );  //ooo
    m_pButtonAddFilter = new QPushButton( "m_pButtonAddFilter", m_pFrame );   //xxx
	//m_pButtonAddFilter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pButtonAddFilter->sizePolicy().hasHeightForWidth() ) );  //oooo
	layout10->addWidget( m_pButtonAddFilter );

	//textLabel1 = new QLabel( m_pFrame, "textLabel1" );   //ooo
    textLabel1 = new QLabel( "textLabel1", m_pFrame );  //xxx
	//textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );  //oooo
	layout10->addWidget( textLabel1 );
	layout13->addLayout( layout10 );

	//layout43 = new Q3HBoxLayout( 0, 0, 6, "layout43");   //ooo
    layout43 = new QHBoxLayout( );   //xxx

	//m_pButtonAddSound = new QPushButton( m_pFrame, "m_pButtonAddSound" );    //ooo
    m_pButtonAddSound = new QPushButton( "m_pButtonAddSound", m_pFrame );   //xxx
	//m_pButtonAddSound->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pButtonAddSound->sizePolicy().hasHeightForWidth() ) );    //oooo
	layout43->addWidget( m_pButtonAddSound );

	//textLabel2 = new QLabel( m_pFrame, "textLabel2" );   //ooo
    textLabel2 = new QLabel( "textLabel2", m_pFrame );  //xxx
	//textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );  //oooo
	layout43->addWidget( textLabel2 );
	layout13->addLayout( layout43 );

	QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout13->addItem( spacer_6 );
	m_pFrameLayout->addLayout( layout13, 0, 0 );

	DialogFilterLayout->addWidget( m_pFrame, 0, 0 );
    
	languageChange();
	resize( QSize(900, 400).expandedTo(minimumSizeHint()) );

	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
//	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( m_pButtonAddSound, SIGNAL( clicked() ), this, SLOT( slotAddSound() ) );
	connect( m_pButtonAddSoundTrack, SIGNAL( clicked() ), this, SLOT( slotAddSoundTrack() ) );

	connect( m_pSliderTime, SIGNAL(sliderMoved(int)), this, SLOT( slotSliderMoved (int) ) );
	connect( m_pComboRange, SIGNAL( activated(int) ), this, SLOT( slotRangeChanged(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogFilter::~DialogFilter()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogFilter::languageChange()
{
    //setCaption( tr( "Filter Dialog" ) );  //ooo
    setWindowTitle( tr( "Filter Dialog" ) );    //xxx
    m_pComboRange->clear();
    m_pEditTotal->setText( tr( "<p align=\"center\">Total: 229</p>" ) );
    m_pButtonAddSoundTrack->setText( tr( "Add Sound track" ) );
    buttonHelp->setText( tr( "&Help" ) );
    //buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );   //ooo
    buttonOk->setText( tr( "&OK" ) );
    //buttonOk->setAccel( QKeySequence( QString::null ) );  //ooo
//    buttonCancel->setText( tr( "&Cancel" ) );
//    buttonCancel->setAccel( QKeySequence( QString::null ) );
    textLabel3->setText( tr( "<p align=\"center\">Timeline</p>" ) );
    m_pEditTime->setText( tr( "00:00:00.000" ) );
    m_pButtonAddFilter->setText( tr( "Add" ) );
    textLabel1->setText( tr( "Filter" ) );
    m_pButtonAddSound->setText( tr( "Add" ) );
    textLabel2->setText( tr( "Sound" ) );
}

void DialogFilter::setImages(CXmlSlideshow *pSlideshow)
{
	// Here we open the xmlSlideshow file and
	// o  set the images accordingly in the m_pImageLine - frame
	// o  set the image filter accordingly in the	m_pFrameFilter
	// o  set the sound accordingly in the 		m_pFrameSound
	////////////////////////////////////////////////////////////////
	QString qsFileName = QString ("/tmp/slideshow.slide");
	m_pSlideshow = pSlideshow;
	initImages(qsFileName);

	uint iTotalXExtension = m_listRange[0]->iXExtension;

	float fTotalTime = m_listRange[0]->fEndTime - m_listRange[0]->fStartTime;
	//m_pScrollView->resizeContents(iTotalXExtension+m_iDeltaPadding, 250);    //oooo
	m_pScrollFrame->resize (iTotalXExtension+m_iDeltaPadding, 265);
	m_pSliderTime->setTickInterval( 10 );
	//m_pSliderTime->setMaxValue( (int) fTotalTime * TICK_FACTOR );    //oooo
	resize( QSize(900, 420).expandedTo(minimumSizeHint()) );
}

void DialogFilter::initImages (QString &)	//xmlFileName)
{
	// Here we open the xmlSlideshow file and
	// o  calculate the X-extension neccesary
	// o  create thumbnails and display them in the main QFrame widget.
	////////////////////////////////////////////////////////////////
	int t, iCount;
	float fTemp, fGlobalDelay, fMinSec = 100000.0f;
	CXmlSlideshow::time_object *pTimeObject = NULL;
	CXmlSlideshow::img_struct *pImg = NULL;
//	m_pSlideshow->readXml(xmlFileName);
	m_pEditTotal->setText( tr( "<p align=\"center\">Total: %1</p>" ).arg(m_pSlideshow->countImg()+1) );

	// And check also the global delay if given.
	fGlobalDelay = (float)m_pSlideshow->delay;
	// If no global delay is specified then we give it 5 seconds.
	if (fGlobalDelay <= 0.0f)
		fGlobalDelay = 5.0;
	fMinSec = fGlobalDelay;
	// Here we get the shortest image duration
	iCount = m_pSlideshow->countImg();
	for (t=0;t<iCount;t++)	{
		pImg = m_pSlideshow->getImg(t);
		if (pImg != NULL)	{
			fTemp = pImg->fDuration;
			if ( (fMinSec > fTemp) && (fTemp > 0.0f) )
				fMinSec = fTemp;
		}
	}
	// The X-expension per image depends on the aspect ratio.
	float fAspectRatio = (float) m_pSlideshow->xres / (float) m_pSlideshow->yres;		// For now only NTSC
	float fXPerSecond = fAspectRatio * m_pImageLine->height() / fMinSec;

	// The second step is to fill in the Range structs.
	int iTotalTime;
	float fRangeCurrentDeltaTime;	// This var holds the time if the current time of the
									// timeline (start of timeline is always 0 see fRangeStartTime)
	float fRangeStartTime;			// This var hold the start time of this range in seconds.
	float fCurrentTime;				// This var is used to generate the fStartTime, and fEndTime of a time_object
	int iCounter, iImgCount, iNrOfObjects;
	// Here we create the first range structure to fill in
	range_struct *pRange = new range_struct;
	m_listRange.append(pRange);
	pRange->iStartImg = 0;
	pRange->iStartObject = 0;
	pRange->fStartTime = 0.0f;
	iNrOfObjects = m_pSlideshow->count();
	iTotalTime = 0;
	fRangeCurrentDeltaTime = 0.0f;
	fRangeStartTime = 0.0f;
	iCounter = 0;
	iImgCount = 0;
	fCurrentTime = 0.0;
	for (t=0;t<iNrOfObjects;t++)	{
		pTimeObject = m_pSlideshow->getTimeObject (t);
		// Here we check if we exceeded the max X extension
		if (pTimeObject->fDuration <= 0.0f)	{
			if (pTimeObject->node_name == "img")
				pTimeObject->fDuration = fGlobalDelay;
			else
				pTimeObject->fDuration = m_pSlideshow->filter_delay;
		}
		pTimeObject->fStartTime = fCurrentTime;
		fCurrentTime += pTimeObject->fDuration;
		pTimeObject->fEndTime = fCurrentTime;

		fTemp = (fRangeCurrentDeltaTime + pTimeObject->fDuration) * fXPerSecond;
		if (fTemp > MAX_XEXTENSION)	{
			// First lets terminate the current range :
			fRangeStartTime += fRangeCurrentDeltaTime;
			pRange->iEndImg = iImgCount;
			pRange->iEndObject  = t;
			pRange->fEndTime = fRangeStartTime;
			pRange->iXExtension = (int) (fXPerSecond * fRangeCurrentDeltaTime);
			// And here we add a new item to te comboBox.
			//m_pComboRange->insertItem( QString( "%1 .. %2" ).arg(pRange->iStartImg+1).arg(pRange->iEndImg+1) );    //ooo
            m_pComboRange->addItem( QString( "%1 .. %2" ).arg(pRange->iStartImg+1).arg(pRange->iEndImg+1) );    //xxx
			// Okay so we should create a new range
			pRange = new range_struct;
			m_listRange.append(pRange);
			pRange->iStartImg = iImgCount;
			pRange->iStartObject  = t;
			pRange->fStartTime = fRangeStartTime;	// same as the end time of the previous range.
			fRangeCurrentDeltaTime = 0.0f;
		}
		// Here we increae the image counter.
		if (pTimeObject->node_name == "img")
			iImgCount ++;
		fRangeCurrentDeltaTime += pTimeObject->fDuration;
	}
	// And here we terminate the l;ast range ..
	fRangeStartTime += fRangeCurrentDeltaTime;
	pRange->iEndImg = iImgCount;
	pRange->iEndObject  = iNrOfObjects;
	pRange->fEndTime = fRangeStartTime;
	pRange->iXExtension = (int) (fXPerSecond * fRangeCurrentDeltaTime);
	//m_pComboRange->insertItem( QString( "%1 .. %2" ).arg(pRange->iStartImg+1).arg(pRange->iEndImg+1) );  //ooo
    m_pComboRange->addItem( QString( "%1 .. %2" ).arg(pRange->iStartImg+1).arg(pRange->iEndImg+1) );    //xxx

	// and here we load the xml file also for the ImageLine object
	m_pImageLine->initImages(m_pSlideshow); //xmlFileName);
	// nd then we set the range.
	slotRangeChanged (0);
}

void DialogFilter::slotSetProgress(int iProgress)
{
	//m_pProgress->setProgress (iProgress);    //ooo
    m_pProgress->setValue (iProgress);  //xxx
}

void DialogFilter::slotSliderMoved(int iNewPosition)
{
	uint iHours, iMins, iSecs, iFracs, iNewPos;
		//range_struct *pRange = m_listRange[m_pComboRange->currentItem()];   //ooo
        range_struct *pRange = m_listRange[m_pComboRange->currentIndex()];  //xxx

	// Eliminate the tick factor
	iNewPos = (int)((iNewPosition/TICK_FACTOR) + pRange->fStartTime);
	iHours  = (int)((double)iNewPos / 3600.0);
	iMins   = (int)((double)iNewPos  / 60.0) - 3600*iHours;
	iSecs   = (int)((double)iNewPos) - 60*iMins - 3600*iHours;
	iFracs  = (iNewPosition%TICK_FACTOR) * 10;
	// Format  00:00:00.000
	QString qsTimeString;
	m_pEditTime->setText(qsTimeString.sprintf ("%02d:%02d:%02d.%03d", iHours, iMins, iSecs, iFracs));
}

void DialogFilter::slotRangeChanged (int iNewRange)
{
	range_struct *pRange = getRange(iNewRange);
	// basic error checking.
	if (pRange == NULL)
		return;
	m_pImageLine->setCurrentRange(pRange);
	m_pFrameFilter->resize (pRange->iXExtension, m_pFrameFilter->height());
	m_pFrameSound->resize (pRange->iXExtension, m_pFrameSound->height());
	//m_pScrollView->resizeContents (pRange->iXExtension+m_iDeltaPadding, m_pScrollView->height());    //ooo
    m_pScrollView->resize (pRange->iXExtension+m_iDeltaPadding, m_pScrollView->height());   //xxx
	m_pScrollFrame->resize (pRange->iXExtension+m_iDeltaPadding, m_pScrollFrame->height());

	float fTotalTime = pRange->fEndTime - pRange->fStartTime;
	m_pSliderTime->resize (pRange->iXExtension, m_pSliderTime->height());
	//m_pSliderTime->setMaxValue( (int) fTotalTime * TICK_FACTOR );    //ooo
    m_pSliderTime->setMaximum( (int) fTotalTime * TICK_FACTOR );    //xxx

	// runtime condition.
//	sleep (1);
	m_pFrameFilter->rangeChanged (pRange, m_pSlideshow);
	m_pFrameSound ->rangeChanged (pRange, m_pSlideshow);
	return;
}

DialogFilter::range_struct *DialogFilter::getRange(int iWhichRange)
{
	// basic error checking.
	if (iWhichRange > (int)m_listRange.count())
		return NULL;
	range_struct *pRange = m_listRange[iWhichRange];
	return pRange;
}

void DialogFilter::slotAddSound ()
{
	QString qsFilter ("*.wav *.mp3 *.mp2 *.mpa *.mpega *.ogg *.WAV *.MP3 *.MP2 *.MPA *.MPEGA *.OGG");
	// Adds another sound file to the current SoundTrack.
	//QStringList fileNames = Q3FileDialog::getOpenFileNames (QString ("Sound Files (")+qsFilter+QString(")"), m_qsLastDirectory); //ooo
    QStringList fileNames = QFileDialog::getOpenFileNames (this, QString ("Sound Files (")+qsFilter+QString(")"), m_qsLastDirectory); //xxx
	if (fileNames.count () < 1)
		return;
	if (m_pSlideshow)
		m_pSlideshow->audio_list += fileNames;
	QFileInfo fileInfo (fileNames[0]);
	m_qsLastDirectory = fileInfo.filePath();
	m_pFrameSound->rangeChanged(NULL, m_pSlideshow);
}

void DialogFilter::slotAddSoundTrack ()
{
	// Adds another track to the DVD movie (english/german etc.)
}

