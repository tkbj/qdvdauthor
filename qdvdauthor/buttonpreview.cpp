/***************************************************************************
    buttonpreview.cpp
                             -------------------
    ButtonPreview class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   This class is the encapsulation of the ButtonPreview from the FileSelection
   Dialog.
    
****************************************************************************/

#include <QPainter>
#include <QCursor>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

#include "undobuffer.h"
#include "buttonpreview.h"
#include "dvdmenuundoobject.h"
#include "qimagehelper.h"
#include "dvdmenu.h"
#include "xml_dvd.h"    //xxx
 
//ButtonPreview::ButtonPreview(QWidget * parent, const char * name, Qt::WindowFlags f)	//ooo
ButtonPreview::ButtonPreview(QWidget * parent)						//xxx
  //: QLabel (parent, name, f)	//ooo
  : QLabel (parent), 		//xxx
    m_pUndoBuffer   ( NULL ),
    m_pActiveObject ( NULL )
{
  // Here we have the undo buffer ...
  m_pUndoBuffer        = new UndoBuffer ( MAX_UNDO_DEPTH );
  m_iCursorStyle       = MousePointerCursor;
  m_pActiveObject      = NULL;
  m_bMouseEventDone    = false;
  m_bDrawVisibleRegion = false;	// This is set in the derived calss MenuPreview - only ... (No need for the buttonPreview class to play with)
}

ButtonPreview::~ButtonPreview()
{
  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;

 uint t;
 for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )
   delete m_listMenuObjects.at ( t );

 m_listMenuObjects.clear ( );  
}

ButtonPreview &ButtonPreview::operator= ( ButtonPreview &theOther )
{
  // Unclonable parameters ...
  //    UndoBuffer *m_pUndoBuffer;
  //  MenuObject *m_pActiveObject;
  // tells the process to skip further handling of the MouseEvents since they were already taken care of.
  //  bool        m_bMouseEventDone;

  int t;
  MenuObject *pObject;
  for ( t=0; t<(int)m_listMenuObjects.count ( ); t++ )
    delete m_listMenuObjects.at ( t );
  m_listMenuObjects.clear ( );

  for ( t=0; t<(int)theOther.m_listMenuObjects.count ( ); t++ ) {
    pObject = theOther.m_listMenuObjects.at ( t );
    if ( ! pObject )
      continue;
    pObject = pObject->clone ( this );
    // At this point we cloned a child for theOther - parent, so we need to 
    // re-parent the clone to the new owner 
    //    pObject->reParent ( this );
    addMenuObject ( pObject );
  }

  m_rectViewport       = theOther.m_rectViewport;     // This is the viewport set in drawContents()
  m_backgroundPixmap   = theOther.m_backgroundPixmap;
  m_bDrawVisibleRegion = theOther.m_bDrawVisibleRegion;
  m_iCursorStyle       = theOther.m_iCursorStyle;

  return *this;
}

bool ButtonPreview::removeObject ( MenuObject *pTheObject )
{
  if ( m_pActiveObject == pTheObject )
       m_pActiveObject  = NULL;

  //bool bReturn = m_listMenuObjects.remove ( pTheObject );	//ooo
  bool bReturn = m_listMenuObjects.removeOne ( pTheObject );	//xxx

  update ( );
  return bReturn;
}

void ButtonPreview::addMenuObject ( MenuObject *pNewObject )
{
  // Adds the menuObject to the list of MenuObjects.
  m_listMenuObjects.append ( pNewObject );
  update ( );
}

void ButtonPreview::slotDeleteObject ( MenuObject *pTheObject )
{
  // object not found. Maybe it is in a ButtonObject ...
  if ( m_pActiveObject == pTheObject )
       m_pActiveObject  = NULL;

  //if ( m_listMenuObjects.remove ( pTheObject ) == false )	//ooo
  if ( m_listMenuObjects.removeOne ( pTheObject ) == false )	//xxx
    return;

  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::DELETED_OBJECT, pTheObject, this ) );	//oooo
//  delete pTheObject;

  update ( );
}

uint ButtonPreview::clearObjectList  ( )
{
  uint iListSize = m_listMenuObjects.count ( );
  // Only clear, do not delete the objects.
  m_listMenuObjects.clear ( );
  return iListSize;
}

MenuObject *ButtonPreview::childContains (QPoint &pos)
{
  // Here we check if the mouse click appeared withtin one of the MenuObjects,
  // in which case the object will take over ...
  int t;
  QPoint theCorrectedPos = pos;
  if (m_rectViewport.width() > 10)	{
    theCorrectedPos.setX(pos.x()-m_rectViewport.x());
    theCorrectedPos.setY(pos.y()-m_rectViewport.y());
  }

  for (t=m_listMenuObjects.count()-1;t>=0;t--)	{
    if (m_listMenuObjects.at(t)->contains (theCorrectedPos))	{
      return m_listMenuObjects.at(t);
    }
  }
  return NULL;
}

MenuObject *ButtonPreview::menuObject (uint iNr)
{
	if (iNr < (uint)m_listMenuObjects.count())
		return m_listMenuObjects.at(iNr);
	else 
		return NULL;
}

uint ButtonPreview::objectCount()
{
  return m_listMenuObjects.count ( );
}

UndoBuffer *ButtonPreview::undoBuffer()
{
	return m_pUndoBuffer;
}

void ButtonPreview::toTop ( MenuObject *pObject )
{
  // This function pushes the currently selected child to the top of the stack as to draw it last (on the top).
  //m_listMenuObjects.remove (pObject);		//ooo
  m_listMenuObjects.removeOne (pObject);	//xxx
  m_listMenuObjects.append (pObject);
}

void ButtonPreview::setObjectCursor ( MenuObject::enState theState )
{
  QCursor theCursor;
  switch ( theState ) {
  case MenuObject::StateScaleTLXY: // >< - cross
  case MenuObject::StateScaleBRXY: // >< - cross
    setMouseCursor ( MouseLZoomCursor );
    break;
  case MenuObject::StateScaleTRXY: // >< - cross
  case MenuObject::StateScaleBLXY: // >< - cross
    setMouseCursor ( MouseRZoomCursor );
    break;
  case MenuObject::StateScaleLeftX: // Horizontal
  case MenuObject::StateScaleRightX: // Horizontal
    setMouseCursor ( MouseXResizeCursor );
    break;
  case MenuObject::StateScaleTopY: // vertical
  case MenuObject::StateScaleBottomY: // vertical
    setMouseCursor ( MouseYResizeCursor );
    break;
  case MenuObject::StateMoveObject: // moving
    setMouseCursor ( MouseMoveCursor    );
    break;
  case MenuObject::StateRotate: // 
    setMouseCursor ( MouseRotateCursor );
    break;

  default:
    setMouseCursor ( MousePointerCursor );
    break;
  }
}

void ButtonPreview::setMouseCursor ( enPointerShape iPointerShape )
{ 
  QCursor theCursor;
  //theCursor.setShape ( iPointerShape );			//ooo
  //theCursor.setShape ( Qt::CrossCursor );			//xxx
  theCursor.setShape ( (enum Qt::CursorShape)iPointerShape );	//xxx
  setCursor ( theCursor );
  m_iCursorStyle = iPointerShape;
}

void ButtonPreview::slotAnimateObject ( QString &qsTempAnim, int iType, QString qsFontName )
{
  emit ( signalAnimateObject ( qsTempAnim, iType, qsFontName ) );
}

void ButtonPreview::slotMoveOnStack(MenuObject *pObject, int iHowMany)
{
	// This function pushes the currently selected child on the stack by iHowMany steps
	if ( ! pObject ) 
		return;

	//int iIndex = m_listMenuObjects.find (pObject);   //ooo
    int iIndex = m_listMenuObjects.indexOf (pObject);   //xxx
	iIndex += iHowMany;
	if (iIndex < 0)
		iIndex = 0;
	if (iIndex >(int)m_listMenuObjects.count ()-1)
		iIndex = m_listMenuObjects.count()-1;

	//m_listMenuObjects.remove (pObject);  //ooo
    m_listMenuObjects.removeOne (pObject);  //xxx

	m_listMenuObjects.insert (iIndex, pObject);
	updatePixmap ( );
}

void ButtonPreview::mousePressEvent ( QMouseEvent *pEvent )
{ 
  // Here we check if the mouse click appeared withtin one of the MenuObjects,
  // in which case the Object will take over the mouse handling ...
  QPoint thePos  = pEvent->pos ( );
  MenuObject *pObject = childContains ( thePos );
  if ( pObject )  {
    //MenuObject::enState currentState = pObject->setObjectState ( thePos, pEvent->state ( ) );		//oooo
    MenuObject::enState currentState = pObject->setObjectState ( thePos, pEvent->button ( ) );		//xxx
    setObjectCursor ( currentState );
    m_bMouseEventDone = pObject->mousePressEvent ( pEvent );
 
    // Here we protect the action toTop for creation of Buttons, which delete the returned pObject
    if ( ! m_bMouseEventDone )  {
      toTop ( pObject );
      undoBuffer ( )->push ( new DVDMenuUndoObject ( pObject, currentState ) );	//oooo
    }
  }
}

void ButtonPreview::mouseReleaseEvent (QMouseEvent *pEvent)
{  
  // Here we check if the mouse click appeared withtin one of the MenuObjects,
  // in which case the object will take over ...
  //  QPoint thePos = pEvent->pos();
  //  MenuObject *pObject = childContains(thePos);
  //  if (pObject != NULL)	{
  //    pObject->setObjectState ( MenuObject::StateInactive );
  //    m_bMouseEventDone = pObject->mouseReleaseEvent(pEvent);
  //    // here we get the last object on the undoBuffer
  //    DVDMenuUndoObject *pUndo = (DVDMenuUndoObject *) undoBuffer()->last();
  //    if (pUndo && !pUndo->hasMoved())	{
  //      undoBuffer()->removeLast();
  //      delete pUndo;
  //    }
  //  }
  //for ( uint t=0; t<(uint)m_listMenuObjects.count( );t++ )  {                 //ooo
  for ( uint t=0; t<static_cast<uint> ( m_listMenuObjects.count( ) );t++ )  {   //xxx
    if ( m_listMenuObjects.at  ( t )->isActive ( ) )  {
      m_bMouseEventDone = m_listMenuObjects.at ( t )->mouseReleaseEvent ( pEvent );
      // here we get the last object on the undoBuffer
      DVDMenuUndoObject *pUndo = (DVDMenuUndoObject *) undoBuffer ( )->last ( );
      if ( pUndo && ! pUndo->hasMoved ( ) )  {
        undoBuffer  ( )->removeLast   ( );
        delete pUndo;
      }	//oooo
    }
  }
  unsetCursor  ( );
}

QPixmap &ButtonPreview::renderMenu (CDVDMenuInterface *pInterface, long iRenderFrameNumber, long iTotalFramesToRender)
{  
	if (m_backgroundPixmap.isNull())
		return m_backgroundPixmap;
	if (!pInterface)
		return m_backgroundPixmap;

	uint        t, iCurrentBackgroundFile;
	QString     qsFileName, qsFullPath, qsMenuName;
	MenuObject *pMenuObject;
	QFileInfo   fileInfo;
	QFile       theFile;
	QDir        theDir;
	// ////////////////////////////////////////////////////////////////////////////////////////////
	// This function renders the frame number iRenderFrameNumber and spits the generated file out as :
	// /TMP_PATH/PROJECT_NAME/MENU_NAME/background/rendered_$(iRenderFrameNumber).jpg
	//
	// This function requires that the extracted frames of the background are located under 
	// /TMP_PATH/PROJECT_NAME/MENU_NAME/background/$(iRenderFrameNumber).jpg
	// And also that all movieObjects (imageObjects = movieButton with 1 frame) frames are found under :
	// /TMP_PATH/PROJECT_NAME/MENU_NAME/$(MovieFileName)/$(iRenderFrameNumber).jpg
	// /////////////////////////////////////////////////////////////////////////////////////////////
	qsMenuName = pInterface->qsMenuName;
	qsFullPath = QString ("%1/%2/%3/background/").arg(Global::qsTempPath) .arg(Global::qsProjectName).arg(qsMenuName);

	// Okay here we get the Directory list of the extracted image files from the directory
	theDir.setPath(qsFullPath);
	//QStringList listFiles = theDir.entryList ("0*.jpg");					//ooo
	QStringList listFiles = theDir.entryList ( QStringList( QString( "0*.jpg" ) ) );	//xxx
	//QStringList listFiles = theDir.entryList ("rendered_*.png");
	if (listFiles.count () == 0)	{
		qsFullPath = QString ("%1/%2/").arg(Global::qsTempPath).arg(Global::qsProjectName);
		theDir.setPath (qsFullPath);
		// Okay nothing extracted, let us assume we have a backgournd image and NOT a movie.
		qsFileName = QString ("%1/%2/%3/"BACKGROUND_NAME).arg(Global::qsTempPath).arg(Global::qsProjectName).arg(qsMenuName);
		// We should however check if the file exists ...
		fileInfo.setFile(qsFileName);
		if (!fileInfo.exists())
			return m_backgroundPixmap;
		// Okay the file exists so let us add it to the empty list ...
		listFiles.append (QString ("%1/"BACKGROUND_NAME).arg(qsMenuName));
	}

	// And here we get the index of the current background image.
	// Now please note that we might have completely different number of images for any 
	// of the media files (background / movie-clips etc.)

	// Version 1 will stretch the found files to fit exactly to the required framesToRender
	iCurrentBackgroundFile = (int)((float)listFiles.count() * (float)iRenderFrameNumber/(float)iTotalFramesToRender);

	// second alternative : render each frame 1:1 and at the end stay at the last frame
	if (0)	{
		iCurrentBackgroundFile = iRenderFrameNumber-1;
		if (iCurrentBackgroundFile > theDir.count())
			iCurrentBackgroundFile = theDir.count();
	}

	// Here we determine the fileName ...
    //qsFileName = QString ("%1/%2").arg(theDir.absPath()).arg(listFiles[iCurrentBackgroundFile]);		//ooo
    //qsFileName = QString ("%1/%2").arg(theDir.absolutePath()).arg(listFiles[iCurrentBackgroundFile]);	//xxx
	if ( listFiles.count () > 0 )  //xxx
	  //qsFileName = QString ("%1/%2").arg(theDir.absPath()).arg(listFiles[iCurrentBackgroundFile]);		//ooo
	  qsFileName = QString ("%1/%2").arg(theDir.absolutePath()).arg(listFiles[iCurrentBackgroundFile]);	//xxx
    else    //xxx
      qsFileName = QString ("%1/").arg(theDir.absolutePath());	//xxx

	//theFile.setName (qsFileName);		//ooo
	theFile.setFileName (qsFileName);	//xxx

	if (!theFile.exists())	{
		//error ...
		return m_backgroundPixmap;
	}

	QPixmap thePixmap(qsFileName);
	// This step is necessary to get the attributes for the backround right if the image / movie does
	// not fit the requested Menu size.
	if (	( (m_backgroundPixmap.width() > 30) || (m_backgroundPixmap.height() > 30) ) && 
		( (thePixmap.width() != m_backgroundPixmap.width()) || (thePixmap.height() != m_backgroundPixmap.height()) ) )
			fitDVDMenuBackground (&thePixmap, 0, pInterface->iStretchType, pInterface->posOffset, pInterface->iWidth, pInterface->iHeight);
//			thePixmap.resize (m_backgroundPixmap.width(), m_backgroundPixmap.height());

	QPainter thePainter(&thePixmap);
	// And here we draw the objects ...
	for (t=0;t<(uint)m_listMenuObjects.count();t++)	{
	  pMenuObject = m_listMenuObjects.at(t);
	  // Filter out if StopFrame is > 0(property is used) AND the current frame is within the limits
	  if ( ( pMenuObject->modifiers ( )->iStopFrame < 1 )  ||
	       ( ( iRenderFrameNumber > pMenuObject->modifiers ( )->iStartFrame ) &&
		 ( iRenderFrameNumber < pMenuObject->modifiers ( )->iStopFrame  ) ) )
	    pMenuObject->drawContents ( &thePainter, iRenderFrameNumber, iTotalFramesToRender );
      }

	float fProgress = (float)iRenderFrameNumber / iTotalFramesToRender;
 
	emit (signalRenderProgress (fProgress));

	m_backgroundPixmap = thePixmap;
 
	return m_backgroundPixmap;
}

void ButtonPreview::fitDVDMenuBackground (QPixmap *pPixmap, int iResizeAlgorithm, int iStretchType, QSize posOffset, int iWidth, int iHeight)
{
  // Okay to tackle the background image generation
  QImageHelper theImage;
  //((QImage&)theImage) = *pPixmap;		//ooo
  ((QImage&)theImage) = pPixmap->toImage();	//xxx
  // set orig Pixmap to zero to save memory ...
  *pPixmap = QPixmap();
  // Next we handle the resizing pending the checked RadioButton algo ...
  if (iStretchType > CDVDMenuInterface::StretchToFit)	{
    // we create a tempImage 
    QImage tempImage;
    int iDeltaX, iDeltaY, iResizeAlgorithm;
    iDeltaX = iDeltaY = iResizeAlgorithm = 0;

    if (iStretchType == CDVDMenuInterface::StretchCutOff)	{	// Cut Off
      //tempImage = theImage.smoothScale (iWidth, iHeight, Qt::KeepAspectRatioByExpanding);	//ooo
      tempImage = theImage.scaled (iWidth, iHeight, Qt::KeepAspectRatioByExpanding);		//xxx
      iDeltaX = posOffset.width ();
      iDeltaY = posOffset.height();
    }
    else	{	// m_DVDMenuInterface::StretchBlackBorder
      //tempImage = theImage.smoothScale (iWidth, iHeight, Qt::KeepAspectRatio);	//ooo
      tempImage = theImage.scaled (iWidth, iHeight, Qt::KeepAspectRatio);		//xxx
      // Note: The tempImage does not need to have the same extensions due to the ScaleMin flag.
      iDeltaX = iWidth - tempImage.width();
      if (iDeltaX > 0)
        iDeltaX = (int)((float)iDeltaX / 2.0);
      iDeltaY = iHeight - tempImage.height();
      if (iDeltaY > 0)
        iDeltaY = (int)((float)iDeltaY / 2.0);
    }

    theImage.fill (0);  //Qt::black);
    theImage.resize (iWidth, iHeight, iResizeAlgorithm);
    //bitBlt (&theImage, iDeltaX, iDeltaY, &tempImage, 0, 0, tempImage.width(), tempImage.height(), Qt::CopyROP);	//oooo
    //bitBlt (&theImage, iDeltaX, iDeltaY, &tempImage, 0, 0, tempImage.width(), tempImage.height());		//xxx
    QPainter p( &theImage );
    p.drawImage( iDeltaX, iDeltaY, tempImage, 0, 0, tempImage.width(), tempImage.height() );			//xxx
  }
  else	// StretchToFit
    theImage.resize (iWidth, iHeight, iResizeAlgorithm);

  *pPixmap = QPixmap (iWidth, iHeight);

  // and here we set the final pixmap.	
  pPixmap->convertFromImage (theImage);
}

void ButtonPreview::paint() //xxx
{
  ButtonPreview::repaint();
  //QLabel::update();
}

void ButtonPreview::paint2() //xxx
{
  QPixmap thePixmap;
  thePixmap = QPixmap ( m_backgroundPixmap );
  QPainter thePainter ( &thePixmap  );
  //drawVisibleRegion   ( &thePainter );
  QPainter *painter;
  painter = &thePainter;
  
  //QLabel::drawContents ( painter );		//ooo
  // We use the viewport to draw the Object in the Center for the ButtonDialog.
  // The Viewport is not used in the MenuPreview for the main window, thus drawing the button
  // on the right location ...
  if ( m_rectViewport.width ( ) > 10 )
       painter->setViewport ( m_rectViewport );
  // In case the user has not yet defined a background Image, we want to show the Objects anyways ...
  // Note: we draw usually all objects into the pixmap, which we have set as a background see updatePixmap()
  if ( m_backgroundPixmap.isNull ( ) )  {
    uint t;
    //erase ( 5, 5, width ( ) - 10, height ( ) - 10 );		//ooo
    //eraseRect ( 5, 5, width ( ) - 10, height ( ) - 10 );	//xxx
    drawVisibleRegion ( painter );
    for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {
      m_listMenuObjects.at ( t )->drawContents ( painter );
    }
    return;
  }

  // This will draw the currentObject on top of the 'copy' we already painted into the pixmap.
  if ( m_pActiveObject )
       m_pActiveObject->drawContents ( painter );
}

void ButtonPreview::paintEvent( QPaintEvent* event )    //xxxx
{
  QLabel::paintEvent(event);
  
  //QLabel::paintEvent( event );  
  //QPainter painter(this);
  //drawContents( &painter ); // ... additional drawing  
  
  //QPainter thePainter;
  //thePainter.begin      ( this );
  //QPixmap doubleBuffer;
  //doubleBuffer = QPixmap ( m_doubleBuffer );
  //thePainter.begin  ( &doubleBuffer );
  //drawContents( &thePainter );
  
  QPixmap thePixmap;
  thePixmap = QPixmap ( m_backgroundPixmap );

  QPainter thePainter ( &thePixmap  );
  ButtonPreview::drawContents( &thePainter );
}

void ButtonPreview::drawContents ( QPainter *painter )
{ 
  //QLabel::drawContents ( painter );		//ooo
  // We use the viewport to draw the Object in the Center for the ButtonDialog.
  // The Viewport is not used in the MenuPreview for the main window, thus drawing the button
  // on the right location ...
  if ( m_rectViewport.width ( ) > 10 )
       painter->setViewport ( m_rectViewport );
  // In case the user has not yet defined a background Image, we want to show the Objects anyways ...
  // Note: we draw usually all objects into the pixmap, which we have set as a background see updatePixmap()
  if ( m_backgroundPixmap.isNull ( ) )  {
    uint t;
    //erase ( 5, 5, width ( ) - 10, height ( ) - 10 );		//ooo
    //eraseRect ( 5, 5, width ( ) - 10, height ( ) - 10 );	//xxx
    drawVisibleRegion ( painter );
    for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {
      m_listMenuObjects.at ( t )->drawContents ( painter );
    }
    return;
  }

  // This will draw the currentObject on top of the 'copy' we already painted into the pixmap.
  if ( m_pActiveObject )
       m_pActiveObject->drawContents ( painter );
}

void ButtonPreview::drawVisibleRegion(QPainter *pPainter)
{
	// Take the visible region at 7% of the total extension (2 * 3.5%)...
	if (m_bDrawVisibleRegion)	{
		uint iXOffset, iYOffset;
		QPen thePen (QColor (255, 30,30), 2, Qt::DashDotLine);
		iXOffset = (int)(3.5/100.0*pPainter->viewport().width());
		iYOffset = (int)(3.5/100.0*pPainter->viewport().height());
		pPainter->setPen(thePen);
		QRect visibleRect (iXOffset, iYOffset, pPainter->viewport().width() - iXOffset*2, pPainter->viewport().height() - iYOffset*2);
		pPainter->drawRect (visibleRect);
// According to http://en.wikipedia.org/wiki/Overscan
// NTSC=720x480  4:3 crop at 660x450 scale to 600x450
// PAL =720x576  4:3 crop at 658x540 scale to 720x540
// NTSC=720x480 16:9 crop at 660x450 scale to 800x450
// PAL =720x576 16:9 crop at 658x540 scale to 960x540
//
// This rect crops at NTSC : 670x448 and  PAL 670x536
// Close enough for an approximation.
	}
}

//void ButtonPreview::setPixmap ( const QPixmap &pixmap )   //ooo
void ButtonPreview::set_Pixmap ( const QPixmap &pixmap )    //xxx
{
  // This is an overloaded function from QLabel
  // We are going to handle the double buffer here ...
  m_backgroundPixmap = pixmap;

  updatePixmap ( );
}

void ButtonPreview::updatePixmap ( )
{
	if ( m_backgroundPixmap.isNull ( ) )
		return;
	// Set the waiting cursor ...
	QCursor myCursor ( Qt::WaitCursor );
	setCursor ( myCursor );

	// This is the function, which updates the background Pixmap
	QPixmap thePixmap;

	drawBackground ( thePixmap );

	// This will check if we load the transparent background (extensions are much smaller)
	if ( (thePixmap.width ( )  < 100 ) || ( thePixmap.height ( ) < 100 ) )
		setFixedSize  ( 720, 480 );
	else	// Otherwise we set the original image size.
		setFixedSize  ( thePixmap.width ( ), thePixmap.height ( ) );

	setBackground ( thePixmap );
	
	// re-set the cursor ...
	//myCursor = QCursor ( QCursor::ArrowCursor );		//ooo
	myCursor = QCursor ( Qt::ArrowCursor );			//xxx
	setCursor ( myCursor );
}

void ButtonPreview::setBackground ( QPixmap &thePixmap )
{
  //if ( parentWidget ( )->parentWidget ( )->name ( ) == QString ( "usingScrollView" ) )		//ooo
  if ( parentWidget ( )->parentWidget ( )->accessibleName ( ) == QString ( "usingScrollView" ) ) {	//xxx
    //setPaletteBackgroundPixmap ( thePixmap );		//oooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Window, QBrush(thePixmap));				//xxx
    this->setPalette(palette);					//xxx
  } else {
    //QLabel::setPixmap ( thePixmap );  //oooo
    this->setPixmap ( thePixmap );  //  //xxx
  }
}

void ButtonPreview::drawBackground ( QPixmap &thePixmap, MenuObject *pOmitMe )
{
  uint t;
  MenuObject *pObject;
  thePixmap = QPixmap ( m_backgroundPixmap );
                                                                                                                             
  QPainter thePainter ( &thePixmap  );
  // First lets imprint the visibleRegionR - rect (if neccesarry).
  drawVisibleRegion   ( &thePainter );
  
  // And here we draw the objects ...
  for  ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ ) {
    pObject = m_listMenuObjects.at ( t );
    if ( ! pObject )
      continue;
    if ( pOmitMe && ( pOmitMe == pObject ) )
      continue;

    if (pObject->objectType ( ) == TEXT_OBJECT)  //xxx
       pObject->the_Pixmap = thePixmap;
    
    pObject->drawContents ( &thePainter );    //ooo
  }
}

/*void ButtonPreview::paintEvent (QPaintEvent *event)   //xxx
{
    
}*/
