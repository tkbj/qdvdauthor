/***************************************************************************
    dialogslide.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Form implementation generated from reading ui file 'dialogslide.ui'
    
****************************************************************************/

#include "dialogslide.h"

#include <qvariant.h>
#include <qpushbutton.h>
//#include <q3buttongroup.h>    //ooo
#include <QButtonGroup>         //xxx
#include <qlabel.h>
//#include <q3frame.h>  //ooo
#include <QFrame>       //xxx
//#include <q3listbox.h>    //ooo
#include <QListWidget>      //xxx
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>  //ooo
#include <QWhatsThis>       //xxx
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <qfileinfo.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
#include <qmessagebox.h>
#include <QMenu>
#include <QToolBar>
//Added by qt3to4:
//#include <q3mimefactory.h>    //ooo
//#include <Q3ValueList>	//ooo
#include <QList>		//xxx
#include <QPixmap>
#include <QMessageBox>		//xxx

//#include "../dialogslide.ui.h"	//ooo
//#include "dialogslide.ui.h"		//xxx
#include "../xml_slideshow.h"
#include "dialogfilter.h"
#include "dialogcreate.h"
#include "imagepreview.h"
/*
 *  Constructs a CSlideDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
//CSlideDialog::CSlideDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )    //ooo
CSlideDialog::CSlideDialog( QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl ) //xxx
    //: QDialog( parent, name, modal, fl )  //ooo
    : QDialog( parent ) //xxx
{
    //setupUi(this);	//xxx
  
    if ( !name )
	//setName( "CSlideDialog" );   //ooo
    setWindowTitle( "CSlideDialog" );   //xxx
    //CSlideDialogLayout = new Q3GridLayout( this, 1, 1, 11, 6, "CSlideDialogLayout");  //ooo
    CSlideDialogLayout = new QGridLayout( this );    //xxx
    
    //m_pButtonSelect = new QPushButton( this, "m_pButtonSelect" ); //ooo
    m_pButtonSelect = new QPushButton( "m_pButtonSelect", this );   //xxx
    m_pButtonSelect->setMaximumSize( QSize( 250, 32767 ) );

    //CSlideDialogLayout->addWidget( m_pButtonSelect, 0, 0 );   //ooo
    CSlideDialogLayout->addWidget( m_pButtonSelect, 0, 0, 1, 2 );   //xxx

    //buttonGroup1 = new Q3ButtonGroup( this, "buttonGroup1" ); //ooo
    buttonGroup1 = new QGroupBox( this );   //xxx
    buttonGroup1->setMaximumSize( QSize( 250, 32767 ) );
    //buttonGroup1->setColumnLayout(0, Qt::Vertical );  //ooo
    //buttonGroup1->layout()->setSpacing( 6 );  //ooo
    //buttonGroup1->layout()->setMargin( 11 );  //ooo
    
    //buttonGroup1Layout = new Q3GridLayout( buttonGroup1->layout() );  //ooo
    buttonGroup1Layout = new QGridLayout( buttonGroup1 ); //xxx
    buttonGroup1Layout->setAlignment( Qt::AlignTop );
    
    //m_pLabelNrOfImages = new QLabel( buttonGroup1, "m_pLabelNrOfImages" );    //ooo
    m_pLabelNrOfImages = new QLabel( buttonGroup1 );    //xxx

    buttonGroup1Layout->addWidget( m_pLabelNrOfImages, 0, 0 );

    //m_pLabelDuration = new QLabel( buttonGroup1, "m_pLabelDuration" );    //ooo
    m_pLabelDuration = new QLabel( buttonGroup1 );  //xxx

    buttonGroup1Layout->addWidget( m_pLabelDuration, 1, 0 );

    //CSlideDialogLayout->addWidget( buttonGroup1, 1, 0 );  //ooo
    CSlideDialogLayout->addWidget( buttonGroup1, 1, 0, 1, 2 );//xxx

    //m_pFramePreview = new Q3Frame( this, "m_pFramePreview" ); //ooo
    m_pFramePreview = new QFrame( this );   //xxx
    m_pFramePreview->setMinimumSize( QSize( 0, 150 ) );
    m_pFramePreview->setMaximumSize( QSize( 250, 32767 ) );
    m_pFramePreview->setFrameShape( QFrame::StyledPanel );
    m_pFramePreview->setFrameShadow( QFrame::Raised );
    //m_pFramePreview->setMargin( 0 );  //ooo
    //m_pFramePreviewLayout = new Q3GridLayout( m_pFramePreview, 1, 1, 2, 2, "m_pFramePreviewLayout");  //ooo
    m_pFramePreviewLayout = new QGridLayout( m_pFramePreview );     //xxx

    //CSlideDialogLayout->addWidget( m_pFramePreview, 2, 0 );   //ooo
    CSlideDialogLayout->addWidget( m_pFramePreview, 2, 0, 2, 2 );   //xxx
    spacer6 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    CSlideDialogLayout->addItem( spacer6, 3, 0 );

    //m_pButtonImport = new QPushButton( this, "m_pButtonImport" ); //ooo
    m_pButtonImport = new QPushButton( "m_pButtonImport", this );    //xxx
    m_pButtonImport->setMaximumSize( QSize( 250, 32767 ) );

    //CSlideDialogLayout->addWidget( m_pButtonImport, 5, 0 );   //ooo
    CSlideDialogLayout->addWidget( m_pButtonImport, 5, 0, 1, 2 );   //xxx

    //m_pButtonExport = new QPushButton( this, "m_pButtonExport" ); //ooo
    m_pButtonExport = new QPushButton( "m_pButtonExport", this );   //xxx
    m_pButtonExport->setMaximumSize( QSize( 250, 32767 ) );

    //CSlideDialogLayout->addWidget( m_pButtonExport, 4, 0 );   //ooo
    CSlideDialogLayout->addWidget( m_pButtonExport, 4, 0, 1, 2 );   //xxx

    //m_pListTable = new Q3ListBox( this, "m_pListTable" ); //ooo
    m_pListTable = new QListWidget( this ); //xxx
    //m_pListTable->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)7, 0, 0, m_pListTable->sizePolicy().hasHeightForWidth() ) );   //oooo
    //m_pListTable->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum, QSizePolicy::DefaultType ) );   //xxx
    //m_pListTable->sizePolicy().setHeightForWidth(false); //xxx

    //CSlideDialogLayout->addMultiCellWidget( m_pListTable, 0, 6, 1, 1 );   //ooo
    CSlideDialogLayout->addWidget( m_pListTable, 0, 2, 7, 1 );  //xxx

    //layout6 = new Q3HBoxLayout( 0, 0, 6, "layout6");  //ooo
    layout6 = new QHBoxLayout( );   //xxx

    //m_pButtonOk = new QPushButton( this, "m_pButtonOk" ); //ooo
    m_pButtonOk = new QPushButton( "m_pButtonOk", this );   //xxx
    //m_pButtonOk->setEnabled( FALSE ); //ooo
    m_pButtonOk->setEnabled( false );   //xxx
    layout6->addWidget( m_pButtonOk );

    //m_pButtonCancel = new QPushButton( this, "m_pButtonCancel" ); //ooo
    m_pButtonCancel = new QPushButton( "m_pButtonCancel", this );   //xxx
    layout6->addWidget( m_pButtonCancel );
    spacer4 = new QSpacerItem( 10, 21, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout6->addItem( spacer4 );

    //m_pButtonHelp = new QPushButton( this, "m_pButtonHelp" ); //ooo
    m_pButtonHelp = new QPushButton( "m_pButtonHelp", this );   //xxx
    layout6->addWidget( m_pButtonHelp );

    //CSlideDialogLayout->addLayout( layout6, 7, 0 );   //ooo
    CSlideDialogLayout->addLayout( layout6, 7, 0, 1, 2 );   //xxx

    //layout9 = new Q3HBoxLayout( 0, 0, 6, "layout9");  //ooo
    layout9 = new QHBoxLayout( );   //xxx

    //layout7 = new Q3VBoxLayout( 0, 0, 6, "layout7");  //ooo
    layout7 = new QVBoxLayout( );   //xxx

    //m_pCheckBackground = new QCheckBox( this, "m_pCheckBackground" ); //ooo
    m_pCheckBackground = new QCheckBox( this ); //xxx
    layout7->addWidget( m_pCheckBackground );

    //textLabel1 = new QLabel( this, "textLabel1" );    //ooo
    textLabel1 = new QLabel( this );    //xxx
    //textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );   //oooo
    layout7->addWidget( textLabel1 );

    //m_pCheckFilter = new QCheckBox( this, "m_pCheckFilter" ); //ooo
    m_pCheckFilter = new QCheckBox( this ); //xxx
    //m_pCheckFilter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pCheckFilter->sizePolicy().hasHeightForWidth() ) );   //oooo
    //m_pCheckFilter->setMaximumSize( QSize( 150, 32767 ) ); //xxx
    //m_pCheckFilter->setChecked( TRUE );   //ooo
    m_pCheckFilter->setChecked( true ); //xxx
    layout7->addWidget( m_pCheckFilter );
    layout9->addLayout( layout7 );

    //layout8 = new Q3VBoxLayout( 0, 0, 6, "layout8");  //ooo
    layout8 = new QVBoxLayout( );   //xxx

    //m_pButtonBackground = new QPushButton( this, "m_pButtonBackground" ); //ooo
    m_pButtonBackground = new QPushButton( "m_pButtonBackground", this );   //xxx
    //m_pButtonBackground->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, m_pButtonBackground->sizePolicy().hasHeightForWidth() ) ); //oooo
    //m_pButtonBackground->setMaximumSize( QSize( 250, 32767 ) );   //ooo
    m_pButtonBackground->setMaximumSize( QSize( 50, 32767 ) );   //xxx
    layout8->addWidget( m_pButtonBackground );

    //m_pEditDelay = new QLineEdit( this, "m_pEditDelay" ); //ooo
    m_pEditDelay = new QLineEdit( this );   //xxx
    //m_pEditDelay->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pEditDelay->sizePolicy().hasHeightForWidth() ) );   //oooo
    m_pEditDelay->setMaximumSize( QSize( 50, 32767 ) );   //ooo
    layout8->addWidget( m_pEditDelay );

    //m_pEditFilterDelay = new QLineEdit( this, "m_pEditFilterDelay" ); //ooo
    m_pEditFilterDelay = new QLineEdit( this ); //xxx
    //m_pEditFilterDelay->setEnabled( TRUE );   //ooo
    m_pEditFilterDelay->setEnabled( true ); //xxx
    //m_pEditFilterDelay->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pEditFilterDelay->sizePolicy().hasHeightForWidth() ) );   //oooo
    m_pEditFilterDelay->setMaximumSize( QSize( 50, 32767 ) ); //ooo
    layout8->addWidget( m_pEditFilterDelay );
    layout9->addLayout( layout8 );

    //CSlideDialogLayout->addLayout( layout9, 6, 0 );   //ooo
    CSlideDialogLayout->addLayout( layout9, 6, 0, 1, 2 );    //xxx

    //layout15 = new Q3HBoxLayout( 0, 0, 6, "layout15");    //ooo
    layout15 = new QHBoxLayout( );  //xxx

    //m_pButtonDelete = new QPushButton( this, "m_pButtonDelete" ); //ooo
    m_pButtonDelete = new QPushButton( "m_pButtonDelete", this );   //xxx
    //m_pButtonDelete->setEnabled( FALSE ); //ooo
    m_pButtonDelete->setEnabled( false );   //xxx
    layout15->addWidget( m_pButtonDelete );
    spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout15->addItem( spacer5 );

    //m_pButtonFilter = new QPushButton( this, "m_pButtonFilter" ); //ooo
    m_pButtonFilter = new QPushButton( "m_pButtonFilter", this );   //xxx
    layout15->addWidget( m_pButtonFilter );

    //CSlideDialogLayout->addLayout( layout15, 7, 1 );  //ooo
    CSlideDialogLayout->addLayout( layout15, 7, 2, 1, 1 );  //xxx
    languageChange();
    resize( QSize(506, 565).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );			//ooo
    setAttribute(Qt::WA_WState_Polished, false);	//xxx

    // signals and slots connections
    connect( m_pButtonBackground, SIGNAL( clicked() ), this, SLOT( slotChooseBackground() ) );
    connect( m_pButtonCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    connect( m_pButtonExport, SIGNAL( clicked() ), this, SLOT( slotExport() ) );
    connect( m_pButtonHelp, SIGNAL( clicked() ), this, SLOT( slotHelp() ) );
    connect( m_pButtonImport, SIGNAL( clicked() ), this, SLOT( slotImport() ) );
    connect( m_pButtonOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( m_pButtonSelect, SIGNAL( clicked() ), this, SLOT( slotSelectImages() ) );
    //connect( m_pListTable, SIGNAL( selectionChanged() ), this, SLOT( slotSelectionChanged() ) );  //ooo
    connect( m_pListTable, SIGNAL( itemSelectionChanged() ), this, SLOT( slotSelectionChanged() ) );    //xxx
    connect( m_pButtonFilter, SIGNAL( clicked() ), this, SLOT( slotApplyFilter() ) );
    connect( m_pCheckFilter, SIGNAL( toggled(bool) ), this, SLOT( slotFilterToggled(bool) ) );
    //connect( m_pListTable, SIGNAL( contextMenuRequested(Q3ListBoxItem*,const QPoint&) ), this, SLOT( slotListRightMouseClicked(Q3ListBoxItem*,const QPoint&) ) ); //ooo
    connect( m_pListTable, SIGNAL( itemClicked(QListWidgetItem *) ), this, SLOT( slotListRightMouseClicked(QListWidgetItem*) ) ); //xxx
    connect( m_pCheckBackground, SIGNAL( toggled(bool) ), this, SLOT( slotBackgroundToggled(bool) ) );
    connect( m_pEditDelay, SIGNAL( textChanged(const QString&) ), this, SLOT( slotUpdateListInfo() ) );
    connect( m_pEditFilterDelay, SIGNAL( textChanged(const QString&) ), this, SLOT( slotUpdateListInfo() ) );
    connect( m_pButtonDelete, SIGNAL( clicked() ), this, SLOT( slotDelete() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CSlideDialog::~CSlideDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CSlideDialog::languageChange()
{
    //setCaption( tr( "Slideshow dialog box." ) );  //ooo
    setWindowTitle( tr( "Slideshow dialog box." ) );    //xxx
    m_pButtonSelect->setText( tr( "Add Imges ..." ) );
    buttonGroup1->setTitle( QString::null );
    m_pLabelNrOfImages->setText( tr( "<p align=\"center\">0 Images</p>" ) );
    m_pLabelDuration->setText( tr( "<p align=\"center\">Duration = 00:00:00.000</p>" ) );
    m_pButtonImport->setText( tr( "Import Slideshow ..." ) );
    m_pButtonExport->setText( tr( "Export Slideshow ..." ) );
    m_pButtonOk->setText( tr( "Ok" ) );
    m_pButtonCancel->setText( tr( "Cancel" ) );
    m_pButtonHelp->setText( tr( "Help" ) );
    m_pCheckBackground->setText( tr( "Use Background" ) );
    textLabel1->setText( tr( "General delay in seconds" ) );
    m_pCheckFilter->setText( tr( "Include filter" ) );
    m_pButtonBackground->setText( tr( "..." ) );
    m_pEditDelay->setText( tr( "5" ) );
    m_pEditFilterDelay->setText( tr( "3" ) );
    m_pButtonDelete->setText( tr( "Delete" ) );
    m_pButtonFilter->setText( tr( "Continue >>>" ) );
}

void CSlideDialog::slotSelectImages()
{
  QString qsFilter ("*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP");
  //QStringList slideshowFiles = Q3FileDialog::getOpenFileNames(tr("Images for Slideshow (")+qsFilter+QString(")"), "./",   //ooo
							     //this, tr("open files dialog Select one or more files to open" )); //ooo
  QStringList slideshowFiles = QFileDialog::getOpenFileNames ( NULL, QObject::tr("open files dialog Select one or more files to open"), "./", QObject::tr("Images for Slideshow (")+qsFilter+QString(")") ); 
  if (slideshowFiles.isEmpty())
    return;
  // Here we check if there are already images in the structure ...
  if (m_Slideshow.count() > 0)	{
    // Ask if slideshow files should be appended or replace current selection.
    if (QMessageBox::warning (NULL, tr ("List not empty!"),
			      tr ("Do you want do add the images to the existing list ?\n"),
			      QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)	{
      m_Slideshow.clear      ( );  // The full entries
      m_slideshowFiles.clear ( );  // the short fileNames (without directories)
      m_pListTable->clear    ( );  // the listTable widget.
      m_pPreview->resetImage ( );
    }
  }
  
  QString fileName;
  QFileInfo fileInfo;
  for (uint t=0;t<(uint)slideshowFiles.count();t++)	{	//ox
    CXmlSlideshow::img_struct *pImg = m_Slideshow.addImg();
    pImg->src=slideshowFiles[t];
//    if (m_pCheckBackground->isChecked () )  {
//	ImageManipulator *pImgMan   = new ImageManipulator;
//	pImgMan->bShowBackground    = m_pCheckBackground->isChecked ();
//	pImgMan->backgroundFileName = m_Slideshow.background;
//	pImg   ->pModifier          = pImgMan;
//    }
    // And here we add the filenames to the table.
    fileInfo = QFileInfo (slideshowFiles[t]);
    fileName = fileInfo.fileName ();
    //m_pListTable->insertItem (fileName);  //ooo
    m_pListTable->addItem (fileName);       //xxx
  }
  m_slideshowFiles += slideshowFiles;
  // And finally, if there are files selected, then enable the Ok - button.
  if (m_slideshowFiles.count())	{
    m_pButtonOk->setEnabled(true);
    m_pButtonFilter->setEnabled(true);
    m_pButtonExport->setEnabled(true);
  }

  //m_pListTable->setSelected (m_pListTable->numRows()-1, TRUE);    //oooo
  //m_pListTable->setSelected (m_pListTable->count()-1, TRUE);    //xxx
  //previewImage(m_pListTable->numRows()-1);    //ooo
  previewImage(m_pListTable->count()-1);    //xxx
  m_bModified = true;
  slotUpdateListInfo ();
}

void CSlideDialog::slotChooseBackground()
{
	QString qsFilter ("*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP");
	//QString qsBackground = Q3FileDialog::getOpenFileName("./", tr("Image Files (")+qsFilter+QString(")"), this,  //xxx
	//tr("Select background image"), tr("Select the background image."));  //xxx
    QString qsBackground = QFileDialog::getOpenFileName(NULL, tr("Select the background image."), "./", tr("Image Files (")+qsFilter+QString(")")); //xxx
	if (qsBackground.isNull())
		return;
	m_Slideshow.background  = qsBackground;
	m_pPreview->setBackground (qsBackground);
}

void CSlideDialog::slotBackgroundToggled( bool bToggled )
{
	m_pButtonBackground->setEnabled(bToggled);
	if (bToggled && m_Slideshow.background.isEmpty())
		slotChooseBackground();

	m_pPreview->toggleBackground (bToggled);
}

void CSlideDialog::slotExport()
{
	// And finally, if there are files selected, then enable the Ok - button.
	if (!m_slideshowFiles.count())	{
		QMessageBox::warning (NULL, tr ("No files in list !"),
			tr (	"Warning, there are no files in the current list.\n"
				"Do you really want to exit ?"
			), QMessageBox::Ok ,  QMessageBox::Cancel);
		return;
	}
	//QString qsExportSlideshow = Q3FileDialog::getSaveFileName("./", tr("Slideshow Files (*.slide)"), this, tr("Slideshow selection Dialog"), tr("Specify slideshow name.")); //ooo
	QString qsExportSlideshow = QFileDialog::getSaveFileName(NULL, tr("Slideshow selection Dialog"), "./", tr("Slideshow Files (*.slide)"));  //xxx
	if (qsExportSlideshow.isNull())
		return;
	//previewImage(m_pListTable->currentItem());   //ooo
    previewImage(m_pListTable->currentRow());      //xxx
	QFileInfo fileInfo (qsExportSlideshow);
	//if (fileInfo.extension(FALSE).isEmpty())	// Indication there is no extension ... //ooo
    if (fileInfo.suffix().isEmpty())	// Indication there is no extension ...    //xxx
		qsExportSlideshow += QString (".slide");
	m_Slideshow.delay = m_pEditDelay->text().toInt();
	m_qsSlideshowFileName=qsExportSlideshow;

	// Here we handle the check if background should be used ...
	QString qsTemp = m_Slideshow.background;
	if ( ! m_pCheckBackground->isChecked () )
		m_Slideshow.background = QString ("");
	if (m_pCheckFilter->isChecked())	{
		CXmlSlideshow theSlideshow;
		createFilterSlideshow ( &theSlideshow );	// Here we insert additionally somefilters and save it.
		theSlideshow.writeXml ( qsExportSlideshow );
	}
	else
		m_Slideshow.writeXml (qsExportSlideshow);
	// And we re-set the background information ...
	m_Slideshow.background = qsTemp;
	m_bModified = false;
}

void CSlideDialog::slotImport()
{
	//QString qsImportSlideshow = Q3FileDialog::getOpenFileName("./", tr("Slideshow Files (*.slide)"), this, tr("Slideshow selection Dialog"), tr("Select a slideshow to add."));  //ooo
    QString qsImportSlideshow = QFileDialog::getOpenFileName(NULL, tr("Slideshow selection Dialog"), "./", tr("Slideshow Files (*.slide)"));   //xxx
	if (qsImportSlideshow.isNull())
		return;
	if (m_Slideshow.count() > 0)	{
		// Ask if slideshow files should be appended or replace current selection.
		if (QMessageBox::warning (NULL, tr ("List not empty!"),
			tr ("Do you want do add the images to the existing list ?\n"),
				QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)	{
					m_Slideshow.clear      ( );	// The full entries
					m_slideshowFiles.clear ( );	// the short fileNames (without directories)
					m_pListTable->clear    ( );	// the listTable widget.
					m_pPreview->resetImage ( );
			}
	}
//	if (m_qsSlideshowFileName.isEmpty())
//		m_qsSlideshowFileName = qsImportSlideshow;
	CXmlSlideshow importedSlideshow;
	QString fileName;
	QFileInfo fileInfo;
	importedSlideshow.readXml (qsImportSlideshow);
	m_Slideshow.background = importedSlideshow.background;
	m_Slideshow.delay = importedSlideshow.delay;
	m_Slideshow.audio_list = importedSlideshow.audio_list;
	m_pEditDelay->setText (QString ("%1").arg(importedSlideshow.delay));
	CXmlSlideshow::time_object *pObject, *pNewObject;
	CXmlSlideshow::img_struct *pImg;

	// Let us use the imported background file, only if it is not null, and the file exists.
	if (!importedSlideshow.background.isEmpty())	{
		QFileInfo fileInfo(importedSlideshow.background);
		if (fileInfo.exists ())	{
			m_Slideshow.background = importedSlideshow.background;
			m_pCheckBackground->setChecked (true);
			m_pButtonBackground->setEnabled(true);
		}
	}
	// And here we add the imported images to the current image list.
	m_slideshowFiles.clear ();
	m_pListTable->clear ();
	if (importedSlideshow.countImg () > 0 )	{
		for (uint t=0;t<importedSlideshow.count();t++)	{
			pObject = importedSlideshow.getTimeObject (t);
			if (pObject->node_name == "img")	{
				pImg = m_Slideshow.addImg();
				pNewObject = (CXmlSlideshow::time_object *) pImg;
				*pImg = *pObject;
				m_slideshowFiles.append (pImg->src);
				// Last but nor least we also add those images to the QListBox ...
				fileInfo = QFileInfo (pImg->src);
				fileName = fileInfo.fileName ();
				//m_pListTable->insertItem (fileName);  //ooo
                m_pListTable->addItem (fileName);   //xxx
			}
			else	{
				pNewObject = (CXmlSlideshow::time_object *)m_Slideshow.addFilter();
				*pNewObject = *pObject;
			}
		}
	}
	if (m_slideshowFiles.count())	{
		m_pButtonOk->setEnabled (true);
		m_pButtonFilter->setEnabled(true);
		m_pButtonExport->setEnabled(true);
	}
	if (importedSlideshow.countFilter () > 0)	{
		m_pCheckFilter->setChecked (false);
		m_pEditFilterDelay->setEnabled (false);
	}
	m_bModified = true;
	m_pPreview->setBackground (m_Slideshow.background);
	m_pLabelNrOfImages->setText (tr ("%1 Images").arg(m_slideshowFiles.count()));

	slotUpdateListInfo ();
}

void CSlideDialog::slotOk() 
{
	if (!m_slideshowFiles.count())	{
		if (QMessageBox::warning (NULL, tr ("No files in list !"),
			tr ("Warning, there are no files in the current list.\n"
				"Do you really want to exit ?"
			), QMessageBox::Yes ,  QMessageBox::No) == QMessageBox::No)
				return;
	}
	if (m_bModified)
		m_qsSlideshowFileName= m_qsTempPath + QString ("/slideshow.xml");

	CXmlSlideshow execSlideshow;
	//previewImage (m_pListTable->currentItem());  //ooo
    previewImage (m_pListTable->currentRow());  //xxx
	m_Slideshow.delay = m_pEditDelay->text().toInt();
	if ( m_pCheckFilter->isChecked ( ) )
		createFilterSlideshow  ( &execSlideshow );	// Here we insert additionally somefilters and save it.
	else
		execSlideshow = m_Slideshow;

	if (!m_pCheckBackground->isChecked ())
		execSlideshow.background = QString ("");
	//hide ();	//ooo
	DialogCreate *pDialog = new DialogCreate (this);
	pDialog->setXmlSlideshow (&execSlideshow, m_qsTempPath);

	// If this is called from DVDAuthor then we should connect to the application.
	if (m_pSourceToolBar)	{	// Note that this OK Button only leads to the next dialog
	  //Q3ToolBar *pSourceToolbar = (Q3ToolBar *)m_pSourceToolBar; //ooo
        QToolBar *pSourceToolbar = (QToolBar *)m_pSourceToolBar;    //xxx
		connect ((QObject *)pDialog,     SIGNAL (signalSetSlideshowEntry(QString)), 
			pSourceToolbar, SLOT   (slotAddSlideshow       (QString)));
		connect ((QObject *)pDialog,     SIGNAL (signalSlideshowDone    (QString, QString)), 
			 pSourceToolbar, SLOT   (slotSlideshowDone      (QString, QString)));
	}

//	int iRet = pDialog->exec();
//	if (iRet ==  QDialog::Accepted)
//		accept ();
	pDialog->show();
}

void CSlideDialog::slotCancel()
{
	reject ();
}

void CSlideDialog::slotHelp()
{
  QMessageBox::information (NULL, tr ("Info :"),
    tr ("[Use Background]  Allows you to set a background image. You can simply use \n"
	"   a one colored image to improve the Look of your slideshow.\n"
	"   Other nice backgrounds are landscapes, skies, ocean etc.\n\n"
	"[General delay in seconds] Is she time the images will be displayed.\n\n"
	"[Include Filter]  This will add filter between images (crossfading).\n"
	"   You can specify the time in seconds for the filter to run through.\n"
	"   Note: You can fine tune the settings afterwards when you press [Continue >>>].\n\n"
	"[Continue >>>]  This will open the filter dialog to modify the global settings \n"
	"   on a per image/filter basis.\n"
	), QMessageBox::Ok ,  Qt::NoButton);
}

void CSlideDialog::slotApplyFilter()
{
	// The user clicked the [Continue >>>] button
	DialogFilter *pDialog = new DialogFilter (this);

	m_Slideshow.delay = m_pEditDelay->text().toInt();
	if ( m_pCheckFilter->isChecked ( ) )	{
		CXmlSlideshow theSlideshow;
		m_Slideshow.filter_delay = m_pEditFilterDelay->text().toInt();
		createFilterSlideshow ( &theSlideshow );
		m_Slideshow = theSlideshow;
	}
	else
		m_Slideshow.filter_delay = 0;

	m_pPreview->resetImage ( );
	pDialog->setImages ( &m_Slideshow );
	pDialog->show ( );
}

void CSlideDialog::slotFilterToggled ( bool bToggled )
{
	// In case we have already loaded a slideshow with some filters, we don't want to destroy any information.
	if ((m_Slideshow.countFilter() > 0)	&& (bToggled) ){
//		m_pEditFilterDelay->setEnabled(false);
		if (QMessageBox::warning (NULL, tr ("Already filter defined !"),
			tr ("This slideshow has already filter defined.\n"
				"If you check this box it will overwrite the existing filter.\n"
				"Do you really want to create new filters ?"
			), QMessageBox::Yes ,  QMessageBox::No) == QMessageBox::No)	{
				m_pCheckFilter->setChecked (false);
				m_pEditFilterDelay->setEnabled(false);
				return;
			}
	}

	m_pEditFilterDelay->setEnabled(bToggled);
	slotUpdateListInfo ();
}

//void CSlideDialog::slotListRightMouseClicked( Q3ListBoxItem *, const QPoint &point)   //ooo
void CSlideDialog::slotListRightMouseClicked( QListWidgetItem *pItem)   //xxx
{
    
    
    QPoint globalPos;   //xxx
    //globalPos.setX(pItem->listWidget()->visualItemRect(pItem).x());  //xxx
    //globalPos.setY(pItem->listWidget()->visualItemRect(pItem).y());  //xxx
    globalPos.setX(this->pos().x()+pItem->listWidget()->pos().x());  //xxx
    globalPos.setY(this->pos().y()+pItem->listWidget()->pos().y());  //xxx
    
	if (m_pListTable->count () < 1)
		return;
    
    QAction *rotate90Action = new QAction("Rotate 90", this); //xxx
    QAction *rotate180Action = new QAction("Rotate 180", this); //xxx
    QAction *rotate270Action = new QAction("Rotate 270", this); //xxx
    QAction *rotateFree = new QAction("Rotate Free ...", this); //xxx
    QAction *editImageAction = new QAction("Edit Image ...", this); //xxx
    
	QMenu *pMenu = new QMenu (this);
	/*pMenu->insertItem ( tr ("Rotate 90") , m_pPreview, SLOT(slotRotate90()));
	pMenu->insertItem ( tr ("Rotate 180"), m_pPreview, SLOT(slotRotate180()));
	pMenu->insertItem ( tr ("Rotate 270"), m_pPreview, SLOT(slotRotate270()));
	pMenu->insertItem ( tr ("Rotate Free ..."), m_pPreview, SLOT(slotRotateFree()));
	pMenu->insertSeparator ();
	pMenu->insertItem ( tr ("Edit Image ..."), m_pPreview, SLOT(slotEditImage()));
	pMenu->exec(point);*/  //oooo
	pMenu->addAction ( rotate90Action );   //xxx
    pMenu->addAction ( rotate180Action );   //xxx
    pMenu->addAction ( rotate270Action );   //xxx
    pMenu->addAction ( rotateFree );   //xxx
    pMenu->addSeparator (); //xxx
    pMenu->addAction ( editImageAction );   //xxx
    
    connect(rotate90Action,    SIGNAL(triggered()), m_pPreview, SLOT(slotRotate90()));  //xxx
    connect(rotate180Action,   SIGNAL(triggered()), m_pPreview, SLOT(slotRotate180()));  //xxx
    connect(rotate270Action,   SIGNAL(triggered()), m_pPreview, SLOT(slotRotate270()));  //xxx
    connect(rotateFree,        SIGNAL(triggered()), m_pPreview, SLOT(slotRotateFree()));  //xxx
    connect(editImageAction,   SIGNAL(triggered()), m_pPreview, SLOT(slotEditImage()));  //xxx
    
    pMenu->exec(globalPos);  //xxx
}

void CSlideDialog::slotSelectionChanged()
{
  //previewImage ( m_pListTable->currentItem ( ) ); //ooo
  previewImage ( m_pListTable->currentRow ( ) );    //xxx
  
  uint t;
  for ( t=0;t<(uint)m_pListTable->count ( ); t++ ) {	//ox
    //if ( m_pListTable->isSelected ( t ) ) {   //ooo
    if ( m_pListTable->currentRow ( ) == t ) {  
      //m_pButtonDelete->setEnabled ( TRUE );   //ooo
      m_pButtonDelete->setEnabled ( true );     //xxx
      return;
    }
  }
}

void CSlideDialog::slotUpdateListInfo()
{
	uint iHours, iMins, iSecs, iTotalTime, iDelay;
	QString qsTimeString, qsStartString;

	iTotalTime = m_pEditDelay->text().toInt() * m_Slideshow.countImg();
	iDelay = m_pEditFilterDelay->text().toInt();
	// Here we check if the user wants transitions, and if so if the m_Slideshow provides a list or
	// if this filter list has yet to be generated.
	if (m_pCheckFilter->isChecked())	{
		if (m_Slideshow.countFilter() < 1)
			iTotalTime += iDelay * (m_Slideshow.countImg() + 2);
		else
			iTotalTime += iDelay * (m_Slideshow.countFilter());
	}
	iHours  = (int)((double)iTotalTime / 3600.0);
	iMins   = (int)((double)iTotalTime  / 60.0) - 60*iHours;
	iSecs   = (int)((double)iTotalTime) - 60*iMins - 3600*iHours;
	// Format  00:00:00.000
	m_pLabelNrOfImages->setText(tr ("<p align=\"center\">%1 Images</P>").arg(m_Slideshow.countImg()));
//	qsTimeString.sprintf  ("<p align=\"center\">%1 = %02d:%02d:%02d.000</p>", iHours, iMins, iSecs);
	qsStartString = QString ("<p align=\"center\">" + tr("Duration"));
	qsTimeString.sprintf (" = %02d:%02d:%02d.000</p>", iHours, iMins, iSecs);
	m_pLabelDuration->setText(qsStartString + qsTimeString);
}

////////////////////////////////////////////////////////////////////////
//
// Helper functions ...
//
////////////////////////////////////////////////////////////////////////
void CSlideDialog::initMe(void *pSourceToolBar, QString qsTempPath)
{
	// Store the pointer for the signalSlideshowDone locally.
	m_pSourceToolBar = pSourceToolBar;
	m_qsTempPath = qsTempPath;
	m_pPreview = new ImagePreview( m_pFramePreview );
	if ( ! m_pPreview )
		return;

	m_pPreview->setMinimumSize( QSize( 150, 150 ) );
	m_pPreview->setMaximumSize( QSize( 250, 32767 ) );
	
	m_pPreview->setScaledContents         ( true );		//xxx
	m_pPreview->setAutoFillBackground     ( true );		//xxx

	m_pFramePreviewLayout->addWidget( m_pPreview, 0, 0 );
	m_bModified = false;
	m_pButtonBackground->setEnabled(false);
	m_pButtonFilter->setEnabled(false);
	m_pButtonExport->setEnabled(false);
	//m_pPreview->setPaletteBackgroundPixmap ( qPixmapFromMimeSource (":/images/checkered.png") );		//ooo
	//m_pPreview->setPaletteBackgroundPixmap ( QPixmap (":/images/checkered.png") );			//xxx
	QPalette palette;									//xxx
	//palette.setBrush(QPalette::Window, QBrush(":/images/checkered.png"));			//xxx
	palette.setBrush(QPalette::Window, QBrush( QPixmap (":/images/checkered.png") ) );	//xxx
	//palette.setColor (QPalette :: Background, QColor (192,253,123));			//xxx
	m_pPreview->setPalette(palette);							//xxx
}

void CSlideDialog::previewImage(int iItem)
{
  CXmlSlideshow::img_struct *pImg = NULL;
  pImg = m_Slideshow.getImg (iItem);
  if (pImg == NULL)
    return;
  
  if ( m_pCheckBackground->isChecked() ) {
    m_pPreview->setBackground(m_Slideshow.background, false);	// false = no refresh, done int setImage
  } else {
    QPixmap background ( m_pPreview->width ( ), m_pPreview->height ( ) );
    background.fill ( QColor ( 0, 0, 0 ) );    
    m_pPreview->setBackground(background, false); // false = no refresh, done int setImage
  }
  
  m_pPreview->setImage (pImg);
}

void CSlideDialog::createFilterSlideshow ( CXmlSlideshow *pSlideshow )
{
	pSlideshow->clear ( );
	CXmlSlideshow::filter_struct	*pFilter;
	CXmlSlideshow::time_object	*pOrigImage;
	CXmlSlideshow::time_object	*pNewImage;
	pSlideshow->filter_delay   = m_pEditFilterDelay->text().toInt();
	pSlideshow->slideshow_name = m_Slideshow.slideshow_name;
	pSlideshow->background     = m_Slideshow.background;
	pSlideshow->delay          = m_Slideshow.delay;
	pSlideshow->audio_list     = m_Slideshow.audio_list;
	pSlideshow->xres           = m_Slideshow.xres;
	pSlideshow->yres           = m_Slideshow.yres;
	
	pFilter = pSlideshow->addFilter  ( );
	pFilter->name = QString ( "fadein" );
	uint t, iImgCount;
	iImgCount = m_Slideshow.countImg ( );
	for ( t=0; t<iImgCount; t++ )	{
		pNewImage  = (CXmlSlideshow::time_object *)pSlideshow->addImg  ( );
		pOrigImage = (CXmlSlideshow::time_object *)m_Slideshow.getImg ( t );
		*pNewImage = *pOrigImage;
		if (t < iImgCount-1)	{
			// The last filter ought to be a fadeout filter.
			pFilter = pSlideshow->addFilter ( );
			pFilter->name = QString ("crossfade");
		}
	}
	pFilter = pSlideshow->addFilter ( );
	pFilter->name = QString ( "fadeout" );
	
	// And finally we write the structure out to the HD.
	//	theSlideshow.writeXml (m_qsSlideshowFileName);
}

void CSlideDialog::slotDelete()
{
  uint t, i;
  QString   qsTemp;
  QFileInfo fileInfo;
  //Q3ValueList<Q3ListBoxItem *> list;		//ooo
  QList<QListWidgetItem *> list;			//xxx
  // Make sure we leave at least one image ...
  if ( m_pListTable->count ( ) < 2 )
    return;

  for ( t=0;t<m_pListTable->count ( ); t++ ) {
    //if ( m_pListTable->isSelected ( t ) ) {   //ooo
    if ( m_pListTable->currentRow ( ) == t) {  //xxx
      list.append(m_pListTable->item( t ) );
    }
  }
  for ( t=0; t<(uint)list.count ( ); t++ ) {		//ooo
    qsTemp = list[t]->text ( );
    for ( i=0; i<m_Slideshow.countImg ( ); i++ ) {
      CXmlSlideshow::img_struct *pImg = m_Slideshow.getImg ( i );
      if ( pImg ) { 
	fileInfo.setFile ( pImg->src );
	if ( fileInfo.fileName ( ) == qsTemp ) {
	  // delete entry from QStringLIst and m_Slideshow ...
	  //m_slideshowFiles.remove ( pImg->src ); //ooo
      m_slideshowFiles.removeOne ( pImg->src ); //xxx
	  m_pPreview->resetImage ( );
	  m_Slideshow.delImg ( pImg );
	  break; // exit inner loop
	}
      }
    }
    // delete entry from ListBox ...
    delete list[t];
  }
}

