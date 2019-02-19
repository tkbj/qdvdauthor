/***************************************************************************
    dialogcreate.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
    Here we start the class DialogCreate.
    This class handles the generation, and display of the images.
    
****************************************************************************/

#include <stdlib.h>

#include <qtabwidget.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qlineedit.h>
//#include <q3textedit.h>   //ooo
#include <QTextEdit>        //xxx
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
//#include <q3process.h>    //ooo
#include <QProcess>         //xxx
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qlabel.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qimage.h>
#include <qpainter.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QTextStream>
#include <QPixmap>

#include "imagepreview.h"
#include "dialogcreate.h"
//#include "xml_slideshow.h"		//ooo
#include "../xml_slideshow.h"		//xxx

#include "../CONFIG.h"

//DialogCreate::DialogCreate(QWidget * parent, const char * name, Qt::WFlags f)		//ooo
DialogCreate::DialogCreate(QWidget * parent, const char * name, Qt::WindowFlags f)	//xxx
	//: uiDialogCreate (parent, name, f)		//ooo
	: QDialog(parent)				//xxx
{
	setupUi(this);		//xxx
  
	m_iProgressCounter = 0;
	//m_pProgress->setTotalSteps (100);    //oooo
	m_pMainDialog = parent;
//	m_pTextOutput->setMaxLogLines (50);
	connect (m_pButtonGenerate, SIGNAL(clicked()), this, SLOT(slotGenerate()));
	findDVDSlideshow();
}

DialogCreate::~DialogCreate()
{

}

void DialogCreate::setXmlSlideshow (CXmlSlideshow *pSlideshow, QString qsTempPath)
{
	m_qsTempPath = qsTempPath;
	if (!qsTempPath.isEmpty())
		m_pEditTempDir->setText(qsTempPath);

	if (!pSlideshow)
		return;
	// copy the original CXmlSlideshow over. This way we can
	// play around with the names etc.
	m_Slideshow = *pSlideshow;
	verifySlideshowExists (false);
}

void DialogCreate::verifySlideshowExists ( bool bMessage )
{
	QDir::setCurrent ( m_pEditTempDir->text() );
	QString qsFileName = QString ("%1/%2.vob").arg(m_pEditTempDir->text()).arg(m_pEditOutputFileName->text()); 
	// here we check if a slidesho with the same name already exists
	QFileInfo fileInfo ( qsFileName );
	if (fileInfo.exists ()) {
		if ( bMessage )
			if (QMessageBox::warning ( NULL, tr ("Warning, slideshow exists already."), tr("Slideshow file exists : \n%1/%2.vob\n\nDo you want to change the output file ?").arg (m_pEditTempDir->text()).arg (m_pEditOutputFileName->text()), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
				return;
		qsFileName = getUniqueTempFile ( m_pEditOutputFileName->text () );
		m_qsOutputFileName = qsFileName;
		fileInfo = QFileInfo ( qsFileName );
		m_pEditOutputFileName->setText ( fileInfo.baseName () );
	}
}

QString DialogCreate::getUniqueTempFile (QString qsOrigFileName)
{
	uint t = 0;
	QString qsUniqueFileName, qsFileName;
	QFileInfo fileInfo (qsOrigFileName);
	QString qsOrigBaseName   = fileInfo.baseName ();
	//QString qsOrigExtension  = fileInfo.extension(); //ooo
    QString qsOrigExtension  = fileInfo.suffix();      //xxx

	//qsFileName.sprintf ("%s%03d.%s", (const char *)qsOrigBaseName, t++, (const char *)qsOrigExtension);  //ooo
    qsFileName.sprintf ("%s%03d.%s", qsOrigBaseName.toLatin1().data(), t++, qsOrigExtension.toLatin1().data()); //xxx
	qsUniqueFileName = qsFileName;
	// The same as above but we ensure the file does not yet exist.
	fileInfo.setFile(qsUniqueFileName);
	while (fileInfo.exists())	{
		//qsFileName.sprintf ("%s%03d.%s", (const char *)qsOrigBaseName, t++, (const char *)qsOrigExtension); //ooo
        qsFileName.sprintf ("%s%03d.%s", qsOrigBaseName.toLatin1().data(), t++, qsOrigExtension.toLatin1().data()); //xxx
		qsUniqueFileName = qsFileName;
		fileInfo.setFile(qsUniqueFileName);
	}
	return qsUniqueFileName;
}

QString & DialogCreate::getOutputFileName ()
{
	return m_qsOutputFileName;
}

void DialogCreate::setOutputFileName (QString &qsOutputFileName)
{
	m_qsOutputFileName = qsOutputFileName;
	m_pEditOutputFileName->setText(qsOutputFileName);
	verifySlideshowExists ( );
}

void DialogCreate::slotGenerate ()
{
	m_iProgressCounter = 0;
	QFileInfo fileInfo;
	m_pMainDialog->show();	// bug in Qt. When main dialog is hidden it will close the whole application when pressing OK of a QMessageBox.
	if (!sanityCheck())	{
		m_pMainDialog->hide();	// bug in Qt. When main dialog is hidden it will close the whole application when pressing OK of a QMessageBox.
		return;	// try again
	}
	m_pMainDialog->hide();	// bug in Qt. When main dialog is hidden it will close the whole application when pressing OK of a QMessageBox.
	m_Slideshow.slideshow_name = m_pEditSlideshowTitle->text();
//	verifySlideshowExists ( );
	emit ( signalSetSlideshowEntry ( m_Slideshow.slideshow_name ) );
	// change the tab to show log / progress.
	//m_pTabWidget->setCurrentPage ( 1 );  //ooo
    m_pTabWidget->setCurrentIndex ( 1 );   //xxx
	preprocessSlideshow    ( );
	createDVDSlideshowFile ( );
	m_pEditTotalNumber->setText ( QString("%1").arg ( (int)( ( m_Slideshow.count ( ) / 2.0 ) + 3.5 ) ) );
	m_pEditImageNumber->setText ( "0" );
	fileInfo.setFile ( m_Slideshow.getImg ( 0 )->src );
	m_pEditCurrentFileName->setText ( fileInfo.fileName ( ) );
	m_pButtonGenerate->setEnabled   ( false );
	// and finally we execute dvd-slideshow.
	executeDVDSlideshow ( );
}

bool DialogCreate::sanityCheck()
{
	// first we check if we can write to the temp directory specified ...
	QString qsTestFile = m_pEditTempDir->text() + QString("/writeCheck.txt");
	QFile theFile ( qsTestFile );
	if ( theFile.open ( QIODevice::WriteOnly ) ) {
	  //Q3TextStream theStream( &theFile );    //ooo
      QTextStream theStream( &theFile );       //xxx
	  theStream << "Test" << "\n";
	  theFile.close ();
	}

	QFileInfo fileInfo ( qsTestFile );
	if ( ! fileInfo.exists ( ) )	{
		QMessageBox::about (NULL, tr ("Could not write"),
		tr ("Attention, I could not write to the temp directory.\n\n"
			"Please make sure the name is correct and you have read/write\n"
			"permission to this directory.\n"));
		//m_pEditTempDir->setPaletteBackgroundColor (QColor(255,0,0));    //ooo
        QPalette palette;								//xxx
        palette.setBrush(QPalette::Base, QBrush(QColor(255,0,0)));	//xxx
        m_pEditTempDir->setPalette(palette);						//xxx
			return false;
	}
	//m_pEditTempDir->setPaletteBackgroundColor (QColor(255,255,255));
    QPalette palette2;								//xxx
    palette2.setBrush(QPalette::Base, QBrush(QColor(255,255,255)));	//xxx
    m_pEditTempDir->setPalette(palette2);						//xxx
	// second check if the dvd-slideshow is on the noted place.
	// The standard libraries are not in the ld.so.conf file ?!?!!?
	QString qsDVDSlideshow = m_pEditExecutableDir->text();
	//if (qsDVDSlideshow.find (QString ("dvd-slideshow"), 0, FALSE) == -1) //ooo
    if (qsDVDSlideshow.indexOf (QString ("dvd-slideshow"), 0, Qt::CaseInsensitive) == -1)   //xxx
		qsDVDSlideshow += "/dvd-slideshow";
	fileInfo = QFileInfo (qsDVDSlideshow);
	if (!fileInfo.exists())	{
		QMessageBox::about (NULL, tr ("dvd-slideshow not found"),
		tr ("Attention, I could not find the dvd-slideshow bash script.\n\n"
			"Please make sure you gave the correct location.\n"));
		//m_pEditExecutableDir->setPaletteBackgroundColor (QColor(255,0,0));  //ooo
        QPalette palette3;								//xxx
        palette3.setBrush(QPalette::Base, QBrush(QColor(255,0,0)));	//xxx
        m_pEditExecutableDir->setPalette(palette3);						//xxx
		return false;
	}
	if (!fileInfo.isExecutable())	{
		QMessageBox::about (NULL, tr ("Can not execute dvd-slideshow"),
		tr ("Attention, I found dvd-slideshow but it is not executable.\n\n"
			"Please change the file properties.\n"));
		//m_pEditExecutableDir->setPaletteBackgroundColor (QColor(255,0,0));  //ooo
        QPalette palette4;								//xxx
        palette4.setBrush(QPalette::Base, QBrush(QColor(255,0,0)));	//xxx
        m_pEditExecutableDir->setPalette(palette4);						//xxx
		return false;
	}
	//m_pEditExecutableDir->setPaletteBackgroundColor (QColor(255,255,255));   //ooo
    QPalette palette5;								//xxx
    palette5.setBrush(QPalette::Base, QBrush(QColor(255,255,255)));	//xxx
    m_pEditExecutableDir->setPalette(palette5);						//xxx
	if (m_pEditOutputFileName->text().isEmpty())	{
		QMessageBox::about (NULL, tr ("No output file name"),
		tr ("Attention, I could not find a output file name.\n\n"
			"Please specify a Output file.\n"));
		//m_pEditOutputFileName->setPaletteBackgroundColor (QColor(255,0,0)); //ooo
        QPalette palette6;								//xxx
        palette6.setBrush(QPalette::Base, QBrush(QColor(255,0,0)));	//xxx
        m_pEditOutputFileName->setPalette(palette6);						//xxx
		return false;
	}
	fileInfo.setFile(m_pEditOutputFileName->text());
	if (fileInfo.exists())	{
		if (QMessageBox::information (NULL, tr ("Output file exists already"),
		tr ("Attention, I the specified Output file exists already.\n\n"
			"Are you sure you want to overwrite this file ?\n"),
			QMessageBox::Yes ,  QMessageBox::No) == QMessageBox::No) {
		//m_pEditOutputFileName->setPaletteBackgroundColor (QColor(255,0,0)); //ooo
        QPalette palette7;								//xxx
        palette7.setBrush(QPalette::Base, QBrush(QColor(255,0,0)));	//xxx
        m_pEditOutputFileName->setPalette(palette7);						//xxx
        }
					return false;
	}
	//m_pEditOutputFileName->setPaletteBackgroundColor (QColor(255,255,255));  //xxx
    QPalette palette8;								//xxx
    palette8.setBrush(QPalette::Base, QBrush(QColor(255,255,255)));	//xxx
    m_pEditOutputFileName->setPalette(palette8);						//xxx
	return true;
}

void DialogCreate::findDVDSlideshow()
{
	// this function tries to locate the dvd-slideshow bash script.
	QFileInfo fileInfo;
	fileInfo = QFileInfo ( QString (PREFIX_DIRECTORY"/share/qdvdauthor/dvd-slideshow") );
	if (fileInfo.exists())	{
		m_pEditExecutableDir->setText(PREFIX_DIRECTORY"/share/qdvdauthor");
		return;
	}
	//fileInfo = QFileInfo (QDir::homeDirPath() + QString ("/.qdvdauthor/dvd-slideshow")); //ooo
	fileInfo = QFileInfo (QDir::homePath() + QString ("/.qdvdauthor/dvd-slideshow"));  //xxx
	if (fileInfo.exists())	{
		//m_pEditExecutableDir->setText(QDir::homeDirPath() + QString ("/.qdvdauthor/dvd-slideshow"));    //ooo
        m_pEditExecutableDir->setText(QDir::homePath() + QString ("/.qdvdauthor/dvd-slideshow"));   //xxx
		return;
	}
	//fileInfo = QFileInfo (QDir::currentDirPath() + QString ("/dvd-slideshow"));  //ooo
	fileInfo = QFileInfo (QDir::currentPath() + QString ("/dvd-slideshow"));   //xxx
	if (fileInfo.exists())	{
		//m_pEditExecutableDir->setText(QDir::currentDirPath());  //ooo
        m_pEditExecutableDir->setText(QDir::currentPath()); //xxx
		return;
	}
	fileInfo = QFileInfo (QString ("/bin/dvd-slideshow"));
	if (fileInfo.exists())	{
		m_pEditExecutableDir->setText("/bin");
		return;
	}
	fileInfo = QFileInfo (QString ("/usr/bin/dvd-slideshow"));
	if (fileInfo.exists())	{
		m_pEditExecutableDir->setText("/usr/bin");
		return;
	}
	fileInfo = QFileInfo (QString ("/usr/local/bin/dvd-slideshow"));
	if (fileInfo.exists())	{
		m_pEditExecutableDir->setText("/usr/local/bin");
		return;
	}
	//fileInfo = QFileInfo (QDir::homeDirPath() + QString ("/bin/dvd-slideshow")); //ooo
	fileInfo = QFileInfo (QDir::homePath() + QString ("/bin/dvd-slideshow"));  //xxx
	if (fileInfo.exists())	{
		//m_pEditExecutableDir->setText(QDir::homeDirPath() + QString("/bin"));   //ooo
        m_pEditExecutableDir->setText(QDir::homePath() + QString("/bin"));  //xxx
		return;
	}
	// Here we go through all directories in the PATH variable.
	QString qsPath = QString(getenv("PATH"));
	//QStringList list = QStringList::split( ":", qsPath );    //ooo
    QStringList list = qsPath.split( ":" ); //xxx
	for (uint t=0; t<(uint)list.count();t++)	{	//ox
		fileInfo = QFileInfo (list[t] + QString ("/dvd-slideshow"));
		if (fileInfo.exists())	{
			m_pEditExecutableDir->setText(list[t]);
			return;
		}
	}
	m_pEditExecutableDir->setText(QString("not found"));
}

// How to get a CXmlSlideshow - structure ...
void DialogCreate::preprocessSlideshow()
{
  uint t, iImgCount, iCount, iPreprocessingImgCount = 0;
  bool bPal = false;
  QFileInfo fileInfo;
  // this function pre-processes the images.
  // The user might have chosen to rotate/shear/change color,
  // change brightness, add background etc.

  // m_Slideshow is the temp CXmlSlideshow struct,
  // to hold the names of the pre-processed images,
  // rather then the original file names.
  m_pTextStatus->setText (tr("Preprocessing images"));
  // This function creates the dvd-slideshow file and puts it into the temp directory.
  iImgCount = m_Slideshow.countImg(); 
  //if (m_pComboFormat->currentItem() == 0)// Generate PAL ???  //ooo
  if (m_pComboFormat->currentIndex() == 0)// Generate PAL ???   //xxx
    bPal = true;

  // To have a better progress indicator, count only the images that do need a preprocessing
  for (t=0;t<iImgCount;t++)	{
    if ( m_Slideshow.getImg (t)->pModifier )
      iPreprocessingImgCount ++;
  }
  iCount = 0;
  for (t=0;t<iImgCount;t++)	{
    renderImage ( m_Slideshow.getImg (t), m_Slideshow.background, bPal ); 
    if ( m_Slideshow.getImg (t)->pModifier ) {
      fileInfo.setFile ( m_Slideshow.getImg (t)->src);
      //m_pProgress->setProgress ((int)((float)iCount++/iPreprocessingImgCount * 100.0));   //ooo
      m_pProgress->setValue ((int)((float)iCount++/iPreprocessingImgCount * 100.0));    //xxx
      m_pTextStatus->setText (tr("Preprocessing %1").arg(fileInfo.fileName()));
      //qApp->processEvents ( 100 ); // for 100ms refresh GUI etc.				//ooo
      qApp->processEvents ( QEventLoop::AllEvents, 100 ); // for 100ms refresh GUI etc.		//xxx
    }
  }
  //m_pProgress->setProgress (0);   //ooo
  m_pProgress->setValue (0);    //xxx
  //m_pTextOutput->setPaletteBackgroundPixmap ( NULL ); //oooo
  //m_pTextOutput->setPaletteBackgroundColor ( QColor ( 0, 0, 0 ) );    //ooo
  QPalette palette;							                        //xxx
  palette.setBrush(m_pTextOutput->backgroundRole(), QColor ( 0, 0, 0 ));	//xxx
  m_pTextOutput->setPalette(palette);						         //xxx
  m_pTextOutput->setText (" ");
  m_pTextOutput->setMaximumWidth  ( 30000 );
  m_pTextOutput->setMaximumHeight ( 30000 );
}

void DialogCreate::renderImage ( CXmlSlideshow::img_struct *pImage, QString qsBackground, bool bPal )
{
  // check if we need to do any pre processing for this image ...
  if ( ! pImage->pModifier )
    return;
  QFileInfo fileInfo ( pImage->src );
  QString qsTempName = QString ("/%1/%2.png").arg(m_pEditTempDir->text()).arg (fileInfo.baseName());

  // Okay, we should have all neccesary information for this pre processing in
  // the ImageManipulator.
  ImageManipulator *pManipulator = (ImageManipulator *)pImage->pModifier;
  int iWidth, iHeight;
  QPixmap theBackground, theImage;
  iWidth = 720;
  if ( bPal ) 
    iHeight = 576;
  else
    iHeight = 480;

  if ( ! qsBackground.isEmpty () ) {
    QImage tempImage (qsBackground);
    //tempImage = tempImage.smoothScale ( iWidth, iHeight, Qt::IgnoreAspectRatio ); //ooo
    tempImage = tempImage.scaled ( iWidth, iHeight, Qt::IgnoreAspectRatio );    //xxx
    theBackground.convertFromImage ( tempImage );
  }
  else {
    theBackground = QPixmap ( iWidth, iHeight );
    theBackground.fill  ( QColor ( 0,0,0 ) );
  }

  {  /// between brakets to free the tempImage - memory when we no longer need it.
    QImage tempImage ( pImage->src );
    //tempImage = tempImage.smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio );   //ooo
    tempImage = tempImage.scaled ( iWidth, iHeight, Qt::KeepAspectRatio );  //xxx
    theImage.convertFromImage ( tempImage );
  }

  // Okay, let us create the image ...
  ImagePreview imagePreview ( NULL );
  imagePreview.setBackground ( theBackground, false );
  imagePreview.setImage ( theImage, pManipulator );
  theBackground = imagePreview.getPixmap ();

  // And finally we save the result.
  theBackground.save ( qsTempName, "PNG" );
  // Next to the last step here is to display a small preview ...
  QMatrix matrix;
  float fZoom = ((float)m_pTextOutput->height ()/(float)theBackground.height());
  matrix.scale (fZoom, fZoom);
  //theBackground = theBackground.xForm ( matrix ); //ooo
  theBackground = theBackground.transformed ( matrix ); //xxx

  m_pTextOutput->setFixedSize (theBackground.width(), theBackground.height());
  //m_pTextOutput->setScaledContents         ( true );			//xxx	
  m_pTextOutput->setAutoFillBackground     ( true );			//xxx
  //m_pTextOutput->setPaletteBackgroundPixmap ( theBackground );	//ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(theBackground));		//xxx
  m_pTextOutput->setPalette(palette);					//xxx
  m_pTextOutput->setText (" ");

  // Finally we set the the image name to the temp fileName
  pImage->src = qsTempName;
  pImage->rotate = 0.0;
}

void DialogCreate::createDVDSlideshowFile()
{
	uint iTitleDelay = 10;
	uint t;
	int iTemp;

	CXmlSlideshow::time_object *pTimeObject;
	CXmlSlideshow::img_struct *pImg;
	CXmlSlideshow::filter_struct *pFilter;
	QString qsTitle = m_pEditSlideshowTitle->text ( );
	if ( qsTitle.length ( ) < 1 )
		qsTitle = "slideshow";
	QString qsSlideshowFile = m_pEditTempDir->text() + QString ("/") + qsTitle + QString (".in");
	QFile theFile (qsSlideshowFile);
	if ( ! theFile.open (QIODevice::WriteOnly) )
	  return;
	QTextStream theStream( &theFile );
	int iDelay = m_Slideshow.delay;
	int iFilterDelay = m_Slideshow.filter_delay;
	// Here we create the title ...
	if ( m_pEditSlideshowTitle->text ( ).length ( ) > 0 )
		theStream << "title:" << iTitleDelay << ":" << m_pEditSlideshowTitle->text() << "\n";
	for (t=0;t<m_Slideshow.count();t++)	{
		pTimeObject = m_Slideshow.getTimeObject(t);
		if (pTimeObject->node_name == "img")	{
			pImg = (CXmlSlideshow::img_struct *)pTimeObject;
			iTemp = (int)(pImg->fDuration+0.5);
			if (iTemp < 0.5)
				iTemp = iDelay;
			theStream << pImg->src << ":" << iTemp << ":" << pImg->text << "\n";
		}
		else	{
			pFilter = (CXmlSlideshow::filter_struct *)pTimeObject;
			iTemp = (pFilter->fDuration < 0.5) ? iFilterDelay : (int)(pFilter->fDuration+0.5);
			theStream << pFilter->name << ":" << iTemp << ":" << pFilter->subtitle << "\n";
		}
	}
	theFile.close ();
}
/* example file ...
title:10:Preparing for Our Journey
~scott/images/Picture_013.jpg:5:Me loading the car
fadeout:3
background:2:
fadein:2
~scott/images/Picture_069.jpg:3:Us getting ready
crossfade:1
~scott/images/Picture_073.jpg:5:John and friends
fadeout:3
*/
void DialogCreate::executeDVDSlideshow()
{
	m_pTextStatus->setText (tr("Processing images"));
	// Here we create a new thread to handle them_pEditSlideshowTitle generation in the background.
	// This Thread shoud be completely independand of the parent thread.
	QString qsDVDSlideshow = m_pEditExecutableDir->text();
	// add the name of the executable only if needed.
	//if (qsDVDSlideshow.find ("dvd-slideshow", 0, FALSE) == -1)   //ooo
    if (qsDVDSlideshow.indexOf ("dvd-slideshow", 0, Qt::CaseInsensitive) == -1) //xxx
		qsDVDSlideshow += QString ("/dvd-slideshow");
	QString qsCommand;
    qsCommand.append ( qsDVDSlideshow );	        // the executable itself    //xxx
    
	QStringList listArgs;
	//listArgs.append ( qsDVDSlideshow );	        // the executable itself //ooo
    //listArgs.append ( "-nocleanup" );	        // Leave temporary files in the temporary directory. Useful for debugging. //xxx

	//if (m_pComboFormat->currentItem() == 0)         // Generate PAL ???  //ooo
    if (m_pComboFormat->currentIndex() == 0)         // Generate PAL ???    //xxx
		listArgs.append(QString ("-p"));
	listArgs.append(QString ("-o"));
	listArgs.append(m_pEditTempDir->text());        // the output dir name
	listArgs.append(QString ("-n"));
	listArgs.append(m_pEditOutputFileName->text()); // The output FileName
	listArgs.append(QString ("-f"));
	// the generated inputFileName
	QString qsTitle = m_pEditSlideshowTitle->text ( );
	if ( qsTitle.length ( ) < 1 )
		qsTitle = "slideshow";
	QString qsSlideshowFile = m_pEditTempDir->text() + QString ("/") + qsTitle + QString (".in");    //ooo
    //QString qsSlideshowFile = m_pEditTempDir->text() + QString ("/") + QString ("\"") + qsTitle + QString (".in") + QString ("\"");   //xxx

	listArgs.append ( qsSlideshowFile );
	if (m_Slideshow.audio_list.count() > 0)	{
		for (uint t=0;t<(uint)m_Slideshow.audio_list.count();t++)	{	//ox
			listArgs.append(QString ("-a"));
			listArgs.append(m_Slideshow.audio_list[t]);
		}
	}
	if ( ! m_Slideshow.background.isEmpty  ( ) )	{
		listArgs.append ( QString ( "-b" ) );
		listArgs.append ( m_Slideshow.background );	// the background image (if any)
	}
	if ( m_Slideshow.delay > 0 )  {
		listArgs.append ( QString ( "-t" ) );
		listArgs.append ( QString ( "%1" ).arg ( m_Slideshow.delay ) );
	}

	m_pNewProcess  = new QProcess ( this );
	//connect( m_pNewProcess, SIGNAL( readyReadStdout ( ) ), this, SLOT ( slotReadFromStdout ( ) ) );  //ooo
    connect( m_pNewProcess, SIGNAL( readyReadStandardOutput ( ) ), this, SLOT ( slotReadFromStdout ( ) ) ); //xxx
	//connect( m_pNewProcess, SIGNAL( readyReadStderr ( ) ), this, SLOT ( slotReadFromStderr ( ) ) );  //ooo
    connect( m_pNewProcess, SIGNAL( readyReadStandardError ( ) ), this, SLOT ( slotReadFromStderr ( ) ) );  //xxx
	//connect( m_pNewProcess, SIGNAL( processExited   ( ) ), this, SLOT ( slotProcessExited  ( ) ) );  //ooo
    connect( m_pNewProcess, SIGNAL( finished  ( int, QProcess::ExitStatus) ), this, SLOT ( slotProcessExited  ( ) ) );  //xxx

	//m_pNewProcess->setArguments ( listArgs );    //ooo
	//m_pNewProcess->start ( );    //ooo
    m_pNewProcess->start ( qsCommand, listArgs );  //xxx
//printf ("Command<%s>\n", listArgs.join ( " " ).ascii ( ) );   //ooo
printf ("Command<%s>\n", listArgs.join ( " " ).toLatin1 ( ).data ( ) ); //xxx
}
/* dvd-slideshow - output
[dvd-slideshow]########################################
[dvd-slideshow] 0/8 title
[dvd-slideshow] Making title slide:
[dvd-slideshow]         Test Show
[dvd-slideshow]         My Slideshow
[dvd-slideshow]########################################
[dvd-slideshow] 1/8 fadein
[dvd-slideshow] frame=0071 94%
*/

void DialogCreate::slotReadFromStdout()
{  
	static bool bFoundSeparator = false;
	QString qsText;
	QFileInfo fileInfo;
	//QString stdOut = m_pNewProcess->readStdout();    //ooo
    QString stdOut = m_pNewProcess->readAllStandardOutput ();   //xxx
	// First we should give some feedback to the user ...
	if (stdOut[0] == '#')
	  return;
	qsText = stdOut;
  
	qsText.truncate (qsText.length()-1);
	m_pTextOutput->append( qsText );
	if (bFoundSeparator)	{
		// Here we search for the right information int the stdOut
		//QStringList listLines    = QStringList::split ( QString ("\n"), stdOut );   //ooo
        QStringList listLines    = stdOut.split ( QString ("\n") ); //xxx
		// Okay do the computing ...
		//QStringList listParts    = QStringList::split ( QString (" "), listLines[0] );  //ooo
        QStringList listParts    = listLines[0].split ( QString (" ") );    //xxx
		//QStringList listNumbers  = QStringList::split ( QString ("/"), listParts[1] );  //ooo
        QStringList listNumbers  = listParts[1].split ( QString ("/") );    //xxx
		if (listNumbers.count() == 1)	{	// this is at the end like "[dvd-slideshow] Joining each mpeg..."
			m_pEditCurrentFileName->setText(listLines[0].right (listLines[0].length() - 16));
		}
		else	{
			fileInfo.setFile (listParts[2]);
			m_pEditCurrentFileName->setText(fileInfo.fileName());
		}
		int iProgress = (int)((float)(m_iProgressCounter++ +1)/((m_Slideshow.count ( )/2.0) + 2 ) * 100.0);
		//m_pProgress->setProgress ( iProgress ); //ooo
        m_pProgress->setValue ( iProgress );    //xxx
		m_pEditImageNumber->setText(QString ("%1").arg(m_iProgressCounter));
		//m_pEditImageNumber->setText(QString ("%1").arg(listNumbers[0].toInt()+1));
		// finally we unmark the separator ...
		bFoundSeparator = false;
	}
	// Here we check if we found the separator ...
	//if (((stdOut.find ("[dvd-slideshow]########################################")) != -1) && (m_iProgressCounter < m_Slideshow.count()+2) )  //ooo
	if (((stdOut.indexOf ("[dvd-slideshow]########################################")) != -1) && (m_iProgressCounter < m_Slideshow.count()+2) ) //xxx
		bFoundSeparator = true;
}

void DialogCreate::slotReadFromStderr()
{
}

void DialogCreate::slotProcessExited()
{
  m_pButtonGenerate->setEnabled(true);
  //QString stdErr = m_pNewProcess->readStderr();   //ooo
  QString stdErr = m_pNewProcess->readAllStandardError();   //xxx
  //QString stdOut = m_pNewProcess->readStdout();   //ooo
  QString stdOut = m_pNewProcess->readAllStandardOutput();  //xxx
  if (!stdOut.isEmpty())
    //printf ("Exit Out <%s>\n", (const char *)stdOut); //ooo
    printf ("Exit Out <%s>\n", stdOut.toLatin1().data());  //xxx
  if (!stdErr.isEmpty())
    //printf ("Exit Err <%s>\n", (const char *)stdErr); //ooo
    printf ("Exit Err <%s>\n", stdErr.toLatin1().data());  //xxx
  m_iProgressCounter = 0;
  //m_pProgress->setProgress (100); //ooo
  m_pProgress->setValue (100);  //xxx
  m_pTextStatus->setText ("Done.");
  m_pEditCurrentFileName->setText("Done.");
	
  // Here we check if the mpeg file has ben generated (see if file exists)
  QString qsOutputFile = m_qsTempPath + QString ("/") + m_pEditOutputFileName->text() + QString (".vob");
  QFileInfo fileInfo ( qsOutputFile );
  if (fileInfo.exists())
    emit (signalSlideshowDone(m_Slideshow.slideshow_name, qsOutputFile));
}

void DialogCreate::slotBack ( )
{
  m_pButtonGenerate->setEnabled ( true );
  m_pMainDialog->show ( );
  reject ( );	// This will only close this dialog and open the main dialog.
 		// accept() would close the whole SlideshowDialog.
}






