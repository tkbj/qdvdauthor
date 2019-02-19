/***************************************************************************
    movieobject.cpp
                             -------------------
    MovieObject class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class is the encapsulation of the MovieObject.
    
    The MovieObject is a DVD Menu object which displays a short animation/
    sequence of the movie to be played.
    
****************************************************************************/
#include <stdlib.h>

#include <qfileinfo.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qdir.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QPixmap>

#include "global.h"
#include "xml_dvd.h"
#include "qdvdauthor.h"
#include "dvdmenu.h"
#include "movieobject.h"
#include "dragndropcontainer.h"
#include "structuretoolbar.h"
#include "dialogmovie.h"
#include "messagebox.h"
#include "qplayer/mediacreator.h"
#include "qrender/startmenurender.h"
#include "dvdmenuundoobject.h"


MovieObject::MovieData::MovieData ( )
  : ImageObject::ImageData ( )
{
  m_iDataType    = Data::MOVIE;
  m_bMenuLength  = true; // default to the same length as the menu
  m_enLoop       = Loop;
  m_iMSecPreview = 0;
}

MovieObject::MovieData::~MovieData ( )
{
}

MenuObject::Data &MovieObject::MovieData::operator = ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  ImageObject::ImageData::operator= ( theOther );
  if ( m_iDataType   == theOther.m_iDataType )  {
    MovieData *pOther = (MovieData *)&theOther;
    m_iMSecPreview    = pOther->m_iMSecPreview;
    m_timeOffset      = pOther->m_timeOffset;
    m_timeDuration    = pOther->m_timeDuration;
    m_bMenuLength     = pOther->m_bMenuLength;
    m_enLoop          = pOther->m_enLoop;
    m_bStartAfterDropEvent = pOther->m_bStartAfterDropEvent;
  }

  return *this;
}

bool MovieObject::MovieData::operator == ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  bool bReturn = ( ImageObject::ImageData::operator == ( theOther ) );
  if ( ! bReturn )
    return false;

  if ( m_iDataType  == theOther.m_iDataType )  {
    MovieData *pOther = (MovieData *)&theOther;
    bReturn = ( ( m_iMSecPreview == pOther->m_iMSecPreview ) &&
                ( m_timeOffset   == pOther->m_timeOffset   ) &&
                ( m_timeDuration == pOther->m_timeDuration ) &&
                ( m_bMenuLength  == pOther->m_bMenuLength  ) &&
                ( m_enLoop       == pOther->m_enLoop       ) &&
                ( m_bStartAfterDropEvent == pOther->m_bStartAfterDropEvent ) );
  }
  return bReturn;
}

MenuObject::Data *MovieObject::MovieData::clone ( )
{
  MovieData *pData = new MovieData;
  *pData  = *this;
  return pData;
}

MovieObject::MovieObject ( QWidget *pParent )
  : ImageObject ( pParent )
{
  if ( m_pData )
    delete m_pData;        // part of the base class
  m_pData = new MovieData; // will be deleted in base class

  m_qsObjectType   = QString (MOVIE_OBJECT);
  m_pConvertTimer  = NULL;
  m_pDVDMenu       = NULL;
  m_bExtracted     = false;
  m_iHowNiceShouldIBe = 0;
}

MovieObject::~MovieObject ( )
{
  if ( m_pConvertTimer )  {
       m_pConvertTimer->stop();
       delete m_pConvertTimer;
  }
  // To make sure that there is no background task 
  // which wants to create the preview for this object
  MediaCreator::unregisterFromMediaScanner (this);
  m_pDVDMenu = NULL;
}

MovieObject::MovieData *MovieObject::movieData ( )
{
  return (MovieData *)m_pData;
}

void MovieObject::setOffset ( QTime &timeOffset )
{
  movieData ( )->m_timeOffset = timeOffset;
  createExtractionPath ( );
}

void MovieObject::setDuration ( QTime &timeDuration )
{
  movieData ( )->m_timeDuration = timeDuration;
  createExtractionPath ( );
  if ( dvdMenu ( ) )
    movieData ( )->m_bMenuLength = ( timeDuration == dvdMenu ( )->getInterface ( )->timeDuration );
}

void MovieObject::setMenuLength ( bool bMenuLength )
{
  if ( bMenuLength && dvdMenu ( ) )
       setDuration  ( dvdMenu ( )->getInterface ( )->timeDuration );

  movieData ( )->m_bMenuLength = bMenuLength;
}

void MovieObject::setLoop ( MovieObject::enType enLoop )
{
  movieData ( )->m_enLoop = enLoop;
}

void MovieObject::createExtractionPath ( )
{
  QString   qsExtractionPath, qsOffset;
  QFileInfo fileInfo ( fileName ( ) );
  QTime     zeroTime;
  char      cBuffer[15];

  //qsExtractionPath = fileInfo.baseName ( true );	//ooo
  qsExtractionPath = fileInfo.baseName ( );		//xxx
  qsOffset         = movieData ( )->m_timeOffset.toString ( "hhmmsszzz"  );
  int iDuration    = zeroTime.msecsTo      ( duration ( ) );
  sprintf ( cBuffer, "-%07d", iDuration );
  qsExtractionPath += "-"+qsOffset + cBuffer;
  setExtractionPath  ( qsExtractionPath );
}

QString &MovieObject::extractionPath ( )
{
  // If this is the same length as the Menu then we should mae sure the extractionPath is
  // always up-to-date. Else m_qsExtractionPath holds the most recent string.
  if ( isMenuLength ( ) )
    createExtractionPath ( );
  return ImageObject::extractionPath ( );
}

void MovieObject::setFile ( QString fileName )
{
  QString qsTemp;
  QFileInfo fileInfo ( fileName );
  qsTemp = fileInfo.fileName  ( );
  setName ( qsTemp );
  m_qsFileName = fileName;
  createExtractionPath ( );
}

void MovieObject::setMSecPreview ( long iMSecPreview )
{
	movieData ( )->m_iMSecPreview = iMSecPreview;
}

QTime &MovieObject::offset    ()
{
	return movieData ( )->m_timeOffset;
}

QTime &MovieObject::duration  ( )
{
  if ( movieData ( )->m_bMenuLength && dvdMenu ( ) ) {
    return dvdMenu ( )->getInterface ( )->timeDuration;
  }

  return movieData ( )->m_timeDuration;
}

bool MovieObject::isMenuLength ( )
{
  return movieData ( )->m_bMenuLength;
}

MovieObject::enType MovieObject::loop ( )
{
  return movieData ( )->m_enLoop;
}

long MovieObject::msecPreview  ()
{
	return movieData ( )->m_iMSecPreview;
}

void MovieObject::setMovieSize (QSize &sizeMovie)
{
	m_sizeOriginalMovieSize = sizeMovie;
}

void MovieObject::setExtracted (bool bExtracted)
{
	// This flag is set after the movie images have been extracted by a background thread
	m_bExtracted = bExtracted;
}

void MovieObject::setDVDMenu ( DVDMenu *pDVDMenu, bool bRegisterToRender )
{
  // Here we associate the DVDMenu with the MovieObject.
  // This is needed to get all information for the rendering process.
  DragNDropContainer *pDNDContainer;

  m_pDVDMenu    = pDVDMenu;
  pDNDContainer = pDVDMenu->getDVDAuthor ( )->getDragNDropContainer ( );
  setDragNDropValues ( pDNDContainer );
  setMenuLength   ( true );
  if ( bRegisterToRender )
       registerToRender( );
}

DVDMenu *MovieObject::dvdMenu( )
{
	return m_pDVDMenu;
}

QSize &MovieObject::movieSize ()
{
	return m_sizeOriginalMovieSize;
}

QImage *MovieObject::previewImage ()
{
	return &m_imagePreview;
}

void MovieObject::setPixmap ( QPixmap &thePixmap )
{
  if ( ! thePixmap.isNull ( ) )  {
    //m_image        = thePixmap.convertToImage ( );	//ooo
    m_image        = thePixmap.toImage ( );		//xxx
    //m_imagePreview = thePixmap.convertToImage ( );	//ooo
    m_imagePreview = thePixmap.toImage ( );		//xxx
    return;
  }
  // Here we set the temp image for now but in order to see it in the right size,
  // we have to first stretch it to the movies original extensions 
  // This is because once it is wrought through the manipulation and modifier 
  // we have a zoomed stretched etc.. image.
  //
  // Note that the real imagePreview is generated in a background thread and later on passed into this object
  // in order to have the GUI active.
  QString qsFileName = fileName ();
  //m_imagePreview = QImage (QImage().fromMimeSource("please_wait.jpg"));	//ooo
  m_imagePreview = QImage ( ":/imagres/please_wait.jpg" );			//xxx
  if ( (movieSize().width() > 5) && (movieSize().height() > 5) )
    //m_imagePreview = m_imagePreview.smoothScale (movieSize(), Qt::IgnoreAspectRatio);		//ooo
    m_imagePreview = m_imagePreview.scaled (movieSize(), Qt::IgnoreAspectRatio);		//xxx
  else
    //m_imagePreview = m_imagePreview.smoothScale (rect().size(), Qt::IgnoreAspectRatio);	//ooo
    m_imagePreview = m_imagePreview.scaled (rect().size(), Qt::IgnoreAspectRatio);		//xxx

  updatePixmap (); 
  MediaCreator::registerWithMediaScanner (this, qsFileName, &m_imagePreview, movieData ( )->m_iMSecPreview );
}

void MovieObject::slotSetThumbnail ( long iMSecPreview )
{
  movieData ( )->m_iMSecPreview = iMSecPreview;
  MediaCreator::registerWithMediaScanner (this, m_qsFileName, &m_imagePreview, movieData ( )->m_iMSecPreview );
}

void MovieObject::timerEvent(QTimerEvent *pTimerEvent)
{
  // Called after the background thread has generated the preview ...
  QSize theSize;
  switch ( pTimerEvent->timerId ( ) )	{
  case MEDIASCANNER_EVENT + 2: // ExecuteJob::TYPE_IMAGE
    float fZoom = ((float)rect ( ).width ( ) / m_imagePreview.width ( ) );
    theSize = m_imagePreview.size ( );
    setZoom      ( fZoom );
    setMovieSize ( theSize );
    updatePixmap ( );
    updateShadow ( );
    if (  shadow ( ) ) // need a refresh to do the shadow correctly.
      emit   ( signalUpdatePixmap ( ) );
    break;
  }
		//oooo
}

void MovieObject::updatePixmap()
{
	// This function will take the input image and pass it through the whole modifications pipeline,
	// so we get a nice and clean rotated / colored image again.

	// In order to speed up the whole procedure, we set the matrix first to scale the image,
	// then we handle the color functions on the smaller image and lastly we apply the rotation,shearing...
	int iRed, iGreen, iBlue;
	int x, y;
	bool bLighter = true;
	float fAdjust = 0.0;
	QColor theColor;
	QRgb thePix;	// QRgb defined as :  0xAARRGGBB;
	QMatrix theMatrix;
	// Here we calculate the center point of gravity (rotation)
	QPoint centerPos;

	if (fileName().isEmpty())
		return;

	QImage theImage = m_imagePreview;
	if (theImage.isNull())
		return;
	//theImage = theImage.smoothScale ( rect ().width(), rect().height(), Qt::IgnoreAspectRatio);	//ooo
	theImage = theImage.scaled ( rect ().width(), rect().height(), Qt::IgnoreAspectRatio);		//xxx

	//centerPos.setX (rect().x() + (int)((float)rect().width () / 2.0));                          //ooo
        centerPos.setX (rect().x() + static_cast<int>(static_cast<float>(rect().width ()) / 2.0));     //xxx
	//centerPos.setY (rect().y() + (int)((float)rect().height() / 2.0));                           //ooo
        centerPos.setY (rect().y() + static_cast<int>(static_cast<float>(rect().height()) / 2.0));      //xxx
	// First we translate to the appropriate location,
	//theMatrix.translate ((double)centerPos.x(), (double)centerPos.y());                          //ooo
        theMatrix.translate (static_cast<double>(centerPos.x()), static_cast<double>(centerPos.y()));   //xxx
	// then we apply the other modifications ...
	theMatrix.scale  (modifiers()->fScaleX, modifiers()->fScaleY);
	// Here we create the scaled image ...
	//theImage = theImage.xForm ( theMatrix );	//ooo
	theImage = theImage.transformed ( theMatrix );	//xxx

	// create empty image.
	//QImage imageColored = QImage (theImage.width(), theImage.height(), theImage.depth());		//ooo
	QImage imageColored = QImage (theImage.width(), theImage.height(), QImage::Format_ARGB32);	//xxx
	if (manipulator().fBrightness > 0.0)	{
		bLighter = true;
		fAdjust = 100 + manipulator().fBrightness * 300;
	}
	else	{
		bLighter = false;
		fAdjust = 100 + manipulator().fBrightness * -300;
	}
	for (y=0;y<theImage.height();y++)	{
		for (x=0;x<theImage.width();x++)	{
			thePix = theImage.pixel(x, y);
			iRed = (int)((float)qRed(thePix) * manipulator().fRed);
			iBlue = (int)((float)qBlue(thePix) * manipulator().fBlue);
			iGreen = (int)((float)qGreen(thePix) * manipulator().fGreen);

			theColor = QColor (iRed, iGreen, iBlue);
			if (bLighter)
				theColor = theColor.light ((int)fAdjust);
			else
				theColor = theColor.dark ((int)fAdjust);
			// This test is to mask out the transparency color (0, 0, 0)
			if (theColor == QColor (0,0,0))
				theColor = QColor (0,0,1);
			imageColored.setPixel (x, y, theColor.rgb());
		}
	}
	// Next we re-set the matrix ...
	theMatrix.reset ();
	// and plug in the remaining operations ...
	theMatrix.rotate (manipulator().fRotate);
	theMatrix.shear  (manipulator().fShearX, manipulator().fShearY);
	//m_image = imageColored.xForm ( theMatrix );		//ooo
	m_image = imageColored.transformed ( theMatrix );	//xxx
	emit    ( signalUpdatePixmap ( ) );
}

void MovieObject::slotEdit ()
{
	if (m_pConvertTimer)
		m_pConvertTimer->stop();

	ImageObject::slotEdit ();
	// And finally we re-start the timer (or start a new time to extract the propper images.
	// Note: This does not need to be started here if it has been converted already since
	//       the edit dialog does not change the extraction range (unlike slotProperties)
	if (m_pConvertTimer)
		registerToRender ();
}

void MovieObject::slotProperties ()
{
  // Okay the user does want to make some changes ... so let us stop the timer ...
  if ( m_pConvertTimer )
       m_pConvertTimer->stop ( );

  DialogMovie movieDialog (NULL);
  connect ( &movieDialog, SIGNAL ( signalSetThumbnail ( long ) ), this, SLOT ( slotSetThumbnail ( long ) ) );
  movieDialog.initMe ( this ); 
  if ( movieDialog.exec ( ) == QDialog::Accepted )  {

    DVDMenuUndoObject *pUndo = new DVDMenuUndoObject ( this, MenuObject::StateUnknown );
    int iX, iY, iWidth, iHeight;
    iX      = movieDialog.m_pEditX->text ( ).toInt ( );
    iY      = movieDialog.m_pEditY->text ( ).toInt ( );
    iWidth  = movieDialog.m_pEditWidth ->text ( ).toInt ( );
    iHeight = movieDialog.m_pEditHeight->text ( ).toInt ( );
    QRect rect ( iX, iY, iWidth, iHeight );
    setRect    ( rect );
    // Next we hande the start/stop time.
    QString qsFormat ("hh:mm:ss.zzz");
    QTime startingTime, endingTime, duration;
    startingTime = QTime::fromString ( movieDialog.m_pEditStarting2->text ( ) );
    endingTime   = QTime::fromString ( movieDialog.m_pEditEnding2  ->text ( ) );
    if ( startingTime.isValid ( ) )  {
      setOffset ( startingTime );
      if (endingTime.isValid ( ) )  {
        int iMSecs = startingTime.msecsTo ( endingTime );
        duration   = duration.addMSecs    (     iMSecs );
        setDuration ( duration );
      }
    }
    setMenuLength ( movieDialog.m_pCheckSameAsMenuLength->isChecked  ( ) );
    setLoop ( Loop );
    if ( movieDialog.m_pButtonLoop->text ( ) == "Stretch" )
         setLoop ( Stretch );
    if ( movieDialog.m_pButtonLoop->text ( ) == "Stop" )
         setLoop ( Stop );

    emit ( signalUpdatePixmap ( ) );
    updatePixmap ( );
    if ( pUndo->hasChanged ( ) )  {
      UndoBuffer *pBuffer = Global::pApp->getUndoBuffer ( );
      if ( pBuffer )
           pBuffer->push ( pUndo );
    }
    else
      delete pUndo;
  }
  disconnect ( &movieDialog, SIGNAL ( signalSetThumbnail ( long ) ), this, SLOT ( slotSetThumbnail ( long ) ) );

  // And finally we re-start the timer (or start a new time to extract the propper images.
  // Note: this can be done bacause I will check a status file first before extracting the frames, 
  //       thus avoiding unneccesary extracting of the images in the movie.
  registerToRender ( );
}

void MovieObject::registerToRender ()
{
	// We start processing this dropped MovieObject 
	// Note that there is a delay of 30 seconds, to give the user time to make some adjustments.
	if (!m_pConvertTimer)	{
		m_pConvertTimer = new QTimer (this);
		connect( m_pConvertTimer, SIGNAL(timeout()), this, SLOT(slotStartConverting()) );
	}
	//m_pConvertTimer->start( 30000, TRUE ); // 30 seconds single-shot timer	//ooo
	m_pConvertTimer->start( 30000 ); // 30 seconds single-shot timer		//xxx
}

void MovieObject::slotStartConverting ()
{
  startConverting ( true );
}

void MovieObject::startConverting ( bool bAllowDialogs )
{
	QString qsCommand, qsFullPath, qsMenuName, qsFormat;
	QFileInfo fileInfo;
	QTime zeroTime;
	QDir theTempDir;
	qsFormat = QString ("hh:mm:ss.zzz");

	// here we kill the timer as we no longer need it.
	if (m_pConvertTimer)	{
		m_pConvertTimer->stop();
		delete m_pConvertTimer;
		m_pConvertTimer = NULL;
	}
	// first we check if we have valid data.
	if ( duration () == zeroTime )  {
	  if ( bAllowDialogs )
	    MessageBox::warning(NULL, tr("No time left."), tr("Warning I found a length of 0 for MovieButton <%1>.\nFrom Offset %2 with a duration of %3\n\nPlease adjust Offset or Duration for this Object.").arg(fileName()).arg(offset().toString (qsFormat)).arg(duration().toString(qsFormat)), QMessageBox::Ok, QMessageBox::NoButton);
	  return;
	}
	// Next we create the directory neccesary ...
	// Note that for the movies we follow the scheme :ith 
	// /temp-Path/ProjectName/MenuName/MovieName/0000001.jpg
	///////////////////////////////////////////////////////
	qsMenuName = QString ( "NoMenuError" );
	if ( m_pDVDMenu )
		qsMenuName = m_pDVDMenu->getInterface()->qsMenuName;

	// Make sure we have the latest and greates path.
	createExtractionPath ( );

	qsFullPath = QFile::encodeName ( QString ("%1/%2/%3/%4").arg ( Global::qsTempPath ).arg ( Global::qsProjectName ).arg ( qsMenuName ).arg ( extractionPath ( ) ) );
	qsCommand  = QString ( "mkdir -p '%1'" ).arg ( qsFullPath );
	//if ( system ((const char *)qsCommand) == -1 )		//ooo
	if ( system (qsCommand.toLatin1().data()) == -1 )	//xxx
		return;
	// and now we check that the path has been created succesfully
	//theTempDir.setPath ( QFile::decodeName ( qsFullPath.latin1 ( ) ) );		//ooo
	theTempDir.setPath ( QFile::decodeName ( qsFullPath.toLatin1 ( ).data() ) );	//xxx
	if ( ! theTempDir.exists ( ) )  {
	  if ( bAllowDialogs )
	    MessageBox::warning(NULL, tr("Could not create Path"), tr("Could not create Path \n%1").arg(qsFullPath), QMessageBox::Ok, QMessageBox::NoButton);
	  return;
	}

	// At this point we can pretty much be assured we have the writing permissions necessary.
	// Next is to run a background thread with a nice priority to extract all images for the 
	// time span required.
	// Note: that the final rendering of the Menu is done when the Whole DVD is rendered.
	StartMenuRender::registerToExtract (this);
}

void MovieObject::setDragNDropValues (DragNDropContainer *pDNDContainer)
{
 setOffset   ( pDNDContainer->timeStartOffset   );
 setDuration ( pDNDContainer->timeDuration      );
 setNiceness ( pDNDContainer->iHowNiceShouldIBe );
 setStartAfterDropEvent ( pDNDContainer->bStartAfterDropEvent );
}

void MovieObject::setStartAfterDropEvent(bool bStartAfterDropEvent)
{
	movieData ( )->m_bStartAfterDropEvent = bStartAfterDropEvent;
}

void MovieObject::setNiceness  (int iVeryNice)
{
	m_iHowNiceShouldIBe = iVeryNice;
}

bool MovieObject::readProjectFile  (QDomNode &theNode)
{
	QDomElement theElement = theNode.toElement();
	// The video node has only attributes and nothing else.
	QDomAttr attribute =  theElement.attributeNode ( MOVIE_OBJECT_MSEC_PREVIEW );
	if ( !attribute.isNull ( ) )
	  setMSecPreview ( attribute.value ( ).toLong ( ) );
	attribute = theElement.attributeNode ( MOVIE_OBJECT_FILE_NAME );
	if (!attribute.isNull())	{
		setFile ( attribute.value() );
		QPixmap nullPixmap;
		setPixmap (nullPixmap);
	}
	attribute =  theElement.attributeNode ( MOVIE_OBJECT_SIZE );
	if (!attribute.isNull())	{
		QString qsAttribute = attribute.value();
		QSize theSize;
		// In the form of WIDTHxHEIGHT
		//int iPos = qsAttribute.find (QString("x"));	//ooo
		int iPos = qsAttribute.indexOf (QString("x"));	//xxx
		if (iPos > -1)	{
			theSize.setWidth (qsAttribute.left (iPos).toInt());
			theSize.setHeight(qsAttribute.right(qsAttribute.length()-iPos-1).toInt());
		}
		setMovieSize (theSize);
	}
	attribute =  theElement.attributeNode ( MOVIE_OBJECT_OFFSET );
	if (!attribute.isNull())	{
		QTime timeOffset = QTime::fromString (attribute.value());
		setOffset (timeOffset);
	}
	attribute =  theElement.attributeNode ( MOVIE_OBJECT_DURATION );
	if (!attribute.isNull())	{
		QTime timeDuration = QTime::fromString (attribute.value());
		setDuration (timeDuration);
	}
	attribute =  theElement.attributeNode ( MOVIE_OBJECT_MENU_LENGTH );
	if ( ! attribute.isNull ( ) )
	  setMenuLength ( ( attribute.value ( ) == "true" ) );
	else
	  setMenuLength ( true );
	attribute =  theElement.attributeNode ( MOVIE_OBJECT_LOOP );
	setLoop ( Loop );
	if ( ! attribute.isNull ( ) )  {
	  if ( attribute.value  ( ) == "Stretch" )
	    setLoop ( Stretch );
	  else  if  ( attribute.value ( ) == "Stop" )
	    setLoop ( Stop );
	}

	QDomNode manNode = theNode.firstChild();
	while (!manNode.isNull())	{
		QDomElement manElement = manNode.toElement();
		if (manElement.tagName() == MANIPULATOR)
			manipulator().readProjectFile ( manElement );
		else if (manElement.tagName() == MENU_OBJECT)
 			MenuObject::readProjectFile( theNode );
		manNode = manNode.nextSibling();
	}
	// Here we create the Pixmap in the right scale
	QPixmap nullPixmap; 
	//QImage theImage = QImage().fromMimeSource("please_wait.jpg");		//ooo
	QImage theImage = QImage ( ":/images/please_wait.jpg" );		//xxx
	//theImage = theImage.smoothScale(rect().width(), rect().height(), Qt::IgnoreAspectRatio);	//ooo
	theImage = theImage.scaled(rect().width(), rect().height(), Qt::IgnoreAspectRatio);		//xxx
	nullPixmap.convertFromImage(theImage);
	setPixmap (nullPixmap);
	return true;
}

bool MovieObject::writeProjectFile (QDomElement &theElement)
{
	QString qsFormat ("hh:mm:ss.zzz");
	QDomDocument xmlDoc = theElement.ownerDocument();
	QDomElement movieNode = xmlDoc.createElement( MOVIE_OBJECT );	// <MovieObject>
	// Here we set the attributes of the <dvdauthor> tag
	QString qsType = "Loop";
	if ( movieData ( )->m_enLoop == Stretch )
	  qsType = "Stretch";
	if ( movieData ( )->m_enLoop == Stop )
	  qsType = "Stop";
	if (!m_qsFileName.isNull())
		movieNode.setAttribute( MOVIE_OBJECT_FILE_NAME, m_qsFileName );
	movieNode.setAttribute( MOVIE_OBJECT_OFFSET,   movieData ( )->m_timeOffset.toString   (qsFormat) );
	movieNode.setAttribute( MOVIE_OBJECT_DURATION, movieData ( )->m_timeDuration.toString (qsFormat) );
	movieNode.setAttribute( MOVIE_OBJECT_MENU_LENGTH,  movieData ( )->m_bMenuLength ? "true" : "false" );
	movieNode.setAttribute( MOVIE_OBJECT_LOOP,         qsType );
	movieNode.setAttribute( MOVIE_OBJECT_MSEC_PREVIEW, QString ( "%1" ).arg ( movieData ( )->m_iMSecPreview ) );
	movieNode.setAttribute( MOVIE_OBJECT_SIZE, QString("%1x%2").arg(movieSize().width()).arg(movieSize().height()) );

	// Store the ImageManipulator - variables (if neccesarry) ...
	manipulator().writeProjectFile (movieNode);

	theElement.appendChild( movieNode );
	// And here we write the base class ...
	return MenuObject::writeProjectFile( movieNode );
}

bool MovieObject::createContextMenu ( QPoint globalPos )
{
  return ImageObject::createContextMenu ( globalPos );
}

StructureItem *MovieObject::createStructure ( StructureItem *pParentItem )
{
  StructureItem *pTemp=NULL;
  QFileInfo fileInfo ( m_qsFileName );
  QString qsName;

  if ( ! pParentItem )  {
    if ( m_pOverlay  )
      pTemp = m_pOverlay->createStructure ( NULL );
    if ( m_pShadow )
      pTemp = m_pShadow->createStructure ( NULL );
    manipulator ( ).createStructure ( NULL );
    m_pStructureItem = NULL;
    return NULL;
  }

  qsName = QString ( "Movie Object" );

  if ( ! m_pStructureItem ) {
    m_pStructureItem = new StructureItem ( this, StructureItem::Video, pParentItem, pParentItem->lastChild ( ), qsName, fileInfo.fileName ( ) );
    //m_pStructureItem->setExpandable      ( TRUE );	//ooo
    m_pStructureItem->setExpanded      ( true );	//xxx
  }

  ImageObject initialValues;
  // And finally we create the modifiers info ...
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Geometry"),
                                         QString ( "%1, %2, %3, %4" ).arg ( rect ( ).x ( ) ).arg ( rect ( ).y ( ) ).
                                         arg ( rect ( ).width ( ) ).arg( rect ( ).height ( ) ) );

  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Duration"), duration ( ).toString ( ) );
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Offset"), movieData ( )->m_timeOffset.toString   ( ) );

  if ( m_pOverlay )
    pTemp = m_pOverlay->createStructure ( m_pStructureItem );

  if ( m_pShadow )
    pTemp = m_pShadow->createStructure ( m_pStructureItem );

  manipulator ( ).createStructure ( m_pStructureItem );

  return m_pStructureItem;
}

MenuObject *MovieObject::clone ( QWidget *pParent, MenuObject * )
{
  if ( !pParent )
    pParent = MenuObject::parent ( );
  MovieObject *pNewObject = new MovieObject( pParent );
  pNewObject->setImage       ( image       ( ) );
  pNewObject->setFile        ( fileName    ( ) );
  pNewObject->setZoom        ( zoom        ( ) );
  pNewObject->setMSecPreview ( msecPreview ( ) );
  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect       ( rect         ( ) );
  pNewObject->setManipulator( manipulator  ( ) );
  pNewObject->setHidden      ( hidden      ( ) );
//	pNewObject->setModifiers  (*modifiers());
  pNewObject->setMovieSize  ( movieSize    ( ) );
  pNewObject->setOffset     ( offset       ( ) );
  pNewObject->setDuration   ( duration     ( ) );
  pNewObject->setMenuLength ( isMenuLength ( ) );
  pNewObject->setNiceness   ( m_iHowNiceShouldIBe);
  //	pNewObject->setMenuName   (m_qsBelongsToMenuName);
  pNewObject->setDVDMenu    ( m_pDVDMenu, false );
  pNewObject->setStartAfterDropEvent ( movieData ( )->m_bStartAfterDropEvent );
  //	QImage  m_imagePreview;	// used to create a preview in a background task.
  //	QTimer *m_pConvertTimer;

  MenuObject *pShadow = shadow ( );
  if ( pShadow )
    pShadow = pShadow->clone ( pParent, pNewObject );
  pNewObject->setShadow      ( pShadow );

  MenuObject *pOverlay = overlay ( );
  if ( pOverlay )
       pOverlay = pOverlay->clone ( pParent, pNewObject );
  pNewObject->setOverlay ( pOverlay );

  return pNewObject;
}

void MovieObject::loadPixmap(QString qsFileName)
{
	// This function will take the input image and pass it through the whole modifications pipeline,
	// so we get a nice and clean rotated / colored image again.

	// In order to speed up the whole procedure, we set the matrix first to scale the image,
	// then we handle the color functions on the smaller image and lastly we apply the rotation,shearing...
	int iRed, iGreen, iBlue;
	int x, y;
	bool bLighter = true;
	float fAdjust = 0.0;
	QColor theColor;
	QRgb thePix;	// QRgb defined as :  0xAARRGGBB;
	QMatrix theMatrix;
	// Here we calculate the center point of gravity (rotation)
	QPoint centerPos;

	if (qsFileName.isEmpty())
		return;

	QImage theImage(qsFileName);
	if (theImage.isNull())
		return;
	float fZoom = ((float)rect().width() / theImage.width());
	if (theImage.width() == 0)
		fZoom = 1.0;
	setZoom (fZoom);
	centerPos.setX (rect().x() + (int)((float)rect().width () / 2.0));
	centerPos.setY (rect().y() + (int)((float)rect().height() / 2.0));
	// First we translate to the appropriate location,
	theMatrix.translate ((double)centerPos.x(), (double)centerPos.y());
	// then we apply the other modifications ...
	theMatrix.scale  (manipulator().fZoom, manipulator().fZoom);
	// Here we create the scaled image ...
	//theImage = theImage.xForm ( theMatrix );	//ooo
	theImage = theImage.transformed ( theMatrix );	//xxx

	// create empty image.
	//QImage imageColored = QImage (theImage.width(), theImage.height(), theImage.depth());		//ooo
	QImage imageColored = QImage (theImage.width(), theImage.height(), QImage::Format_ARGB32);	//xxx
	if (manipulator().fBrightness > 0.0)	{
		bLighter = true;
		fAdjust = 100 + manipulator().fBrightness * 300;
	}
	else	{
		bLighter = false;
		fAdjust = 100 + manipulator().fBrightness * -300;
	}
	for (y=0;y<theImage.height();y++)	{
		for (x=0;x<theImage.width();x++)	{
			thePix = theImage.pixel(x, y);
			iRed = (int)((float)qRed(thePix) * manipulator().fRed);
			iBlue = (int)((float)qBlue(thePix) * manipulator().fBlue);
			iGreen = (int)((float)qGreen(thePix) * manipulator().fGreen);

			theColor = QColor (iRed, iGreen, iBlue);
			if (bLighter)
				theColor = theColor.light ((int)fAdjust);
			else
				theColor = theColor.dark ((int)fAdjust);
			// This test is to mask out the transparency color (0, 0, 0)
			if (theColor == QColor (0,0,0))
				theColor = QColor (0,0,1);
			imageColored.setPixel (x, y, theColor.rgb());
		}
	}
	// Next we re-set the matrix ...
	theMatrix.reset ();
	// and plug in the remaining operations ...
	theMatrix.rotate (manipulator().fRotate);
	theMatrix.shear  (manipulator().fShearX, manipulator().fShearY);
	//m_image = imageColored.xForm( theMatrix );		//ooo
	m_image = imageColored.transformed( theMatrix );	//xxx
}

void MovieObject::drawContents (QPainter *pPainter, int iRenderFrameNumber, int iTotalFramesToRender)
{   
  if ( m_pOverlay )  {
       m_pOverlay->drawContents ( pPainter, iRenderFrameNumber, iTotalFramesToRender );
       return;
  }

  QString qsFullPath, qsFileName, qsMenuName;
  int iCurrentClipFile = 0;

  // Okay first step is get the fileName of the frame
  qsMenuName = dvdMenu ( )->getInterface ( )->qsMenuName;
  qsFullPath = QString ( "%1/%2/%3/%4" ).arg ( Global::qsTempPath ).arg ( Global::qsProjectName ) .arg ( qsMenuName ).arg ( extractionPath ( ) );

  QDir theDir ( qsFullPath );
  //theDir.setNameFilter ( "0*.jpg" );				//ooo
  theDir.setNameFilters ( QStringList(QString("0*.jpg")) );	//xxx
  int iDirCount = theDir.count  ( );

//	// Next is to set the AnimationAttributes
//	for (t=0;t<m_listAnimationAttributes.count();t++)
//		m_listAnimationAttributes[t]->setValue (iRenderFrameNumber);

  // This one we use as a loop the video
  if ( movieData ( )->m_enLoop == Loop )
    iCurrentClipFile = ( iDirCount > 0 ) ? iRenderFrameNumber%iDirCount : 0;
  else if ( movieData ( )->m_enLoop == Stretch ) // Stretch : We have the full list of the frames, now we pick the appropriate frame ...
    iCurrentClipFile = (int)((float)theDir.count() * (float)iRenderFrameNumber/(float)iTotalFramesToRender);
  else if ( movieData ( )->m_enLoop == Stop )
    iCurrentClipFile = ( iRenderFrameNumber > iDirCount ) ? iDirCount-1 : iRenderFrameNumber;

  //qsFileName = QString ("%1/%2").arg(qsFullPath).arg(theDir[iCurrentClipFile]);   //ooo
  
  if ( iDirCount > 0 )  //xxx
    qsFileName = QString ("%1/%2").arg(qsFullPath).arg(theDir[iCurrentClipFile]);   //ooo
  else  //xxx  
    qsFileName = QString ("%1/").arg(qsFullPath); //xxx

  //printf ("MovieObject::drawContents <%s> <%s> <%d> <%d>\n", (const char *)qsFileName, (const char *)qsFileName, theDir.count(), iCurrentClipFile);
  loadPixmap (qsFileName);
  // if we have an animation, then the shadow is updated in the imageobject ...
  if ( m_listAnimationAttributes.count ( ) < 1 )
    updateShadow ( );

  // then we call
  ImageObject::drawContents ( pPainter, iRenderFrameNumber, iTotalFramesToRender );
}

AnimationAttribute *MovieObject::getSpecificAttributes (long iMaxNumberOfFrames, QString qsProperty)
{
  AnimationAttribute *pAnimAttr = NULL;
  static int m_iTestInt;
  // first check the base class ...
  pAnimAttr = ImageObject::getSpecificAttributes( iMaxNumberOfFrames, qsProperty);
  if ( ! pAnimAttr )	{
    if (qsProperty == "frameNo")
      pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_iTestInt, &m_iTestInt);
  }

  return pAnimAttr;
}
