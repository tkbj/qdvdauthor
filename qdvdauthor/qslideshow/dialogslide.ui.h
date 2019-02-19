/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
// This file uses a xml style to store all information regarding the slideshow.
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

#include "../xml_slideshow.h"
#include "dialogfilter.h"
#include "dialogcreate.h"
#include "imagepreview.h"

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


