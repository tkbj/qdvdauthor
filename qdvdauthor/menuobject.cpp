/***************************************************************************
    menuobject.cpp
                             -------------------
    MenuOject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  This class is the encapsulation of the MenuObject.

  A MenuObject is the base class for any visible object in a DVD menu.
  A MenuObject itself is not visible and not used in itself. Only derived
  classes are used in the DVD Menu.

  Current objects include :
  TextObject, ImageObject, MovieObject, FrameObject, ButtonObject
  ShadowObject, MaskObject, ObjectCollection, and MenuObject
    
****************************************************************************/

#include <QTimer>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>

#include "xml_dvd.h"
#include "menuobject.h"
#include "shadowobject.h"
#include "structuretoolbar.h"
#include "dialogmatrix.h"

MenuObject::Data::Data ( )
{
  m_iDataType = DATA;
}

MenuObject::Data::~Data ( )
{

}

MenuObject::Data &MenuObject::Data::operator = ( Data &theOther )
{
  m_rect        = theOther.m_rect;
  m_qsName      = theOther.m_qsName;
  m_modifiers   = theOther.m_modifiers;
  m_qsAnimation = theOther.m_qsAnimation;
  return *this;
}

bool MenuObject::Data::operator == ( Data &theOther )
{
  bool bReturn = ( ( m_rect        == theOther.m_rect        ) &&
                   ( m_qsName      == theOther.m_qsName      ) &&
                   ( m_modifiers   == theOther.m_modifiers   ) &&
                   ( m_qsAnimation == theOther.m_qsAnimation ) );
  return bReturn;
}

MenuObject::Data *MenuObject::Data::clone ( )
{
  Data *pData = new Data;
  *pData = *this;
  return pData;
}

MenuObject::MenuObject (QWidget *pParent)
  : QObject (pParent)
{
  m_qsObjectType   = QString (MENU_OBJECT);
  m_pParent        = pParent;
  m_bDrawRect      = false;
  m_pContextMenu   = NULL;
  m_pShadow        = NULL;
  m_pOverlay       = NULL;
  m_pStructureItem = NULL;	//ooo
  m_pMatrixDialog  = NULL;	//ooo
  m_objectState    = StateUnknown;
  m_pData          = new Data;  
}

MenuObject::~MenuObject ( )
{
  uint t;
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;
  for ( t=0; t<(uint)m_listAnimationAttributes.count ( ); t++ )
    delete m_listAnimationAttributes[t];	//oooo

  m_listAnimationAttributes.clear ( );	//oooo

  resetStructureItem ( );
  if ( m_pShadow )
    delete m_pShadow;
  m_pShadow = NULL;

  if ( m_pOverlay )
    delete m_pOverlay;
  m_pOverlay = NULL;

  if ( m_pStructureItem )
    delete m_pStructureItem;
  m_pStructureItem = NULL;

  if ( m_pData )
    delete m_pData;
  m_pData = NULL;

  m_pParent = NULL;
}

bool MenuObject::readProjectFile  (QDomNode &theNode)
{
	// Here we read in the base class ...
	QDomNode subNode;
	QDomNode xmlMenuObject = theNode.firstChild();
	
	{ // clean up the current object attributes.
	  // if we have a shadowObject, the we should get rid of it.
	  Modifiers emptyModifiers;
	  if ( m_pShadow )
	    delete m_pShadow;
	  m_pShadow = NULL;
	  if ( m_pOverlay )
	    delete m_pOverlay;
	  m_pOverlay = NULL;
	  m_pData->m_qsAnimation = QString ( );
	  *modifiers () = emptyModifiers;
	}

	while ( !xmlMenuObject.isNull () )	{
		// Okay, here we retain the stored data from the xml file.
		QDomAttr attribute;
		QDomElement theElement = xmlMenuObject.toElement();
		QString tagName = theElement.tagName();
		QString nodeText = theElement.text ();
		// Okay, this is ugly but after all it is the simplest of all xml file structure.
		// No need to get fancy ...
		if (tagName == MENU_OBJECT)	{
			m_pData->m_qsName = nodeText;
			QRect theRect;
			int iX, iY, iWidth, iHeight;
			iX      = rect().x();
			iY      = rect().y();
			iWidth  = rect().width();
			iHeight = rect().height();

			attribute = theElement.attributeNode ( MENU_OBJECT_NAME );
			if (!attribute.isNull())
				m_pData->m_qsName = attribute.value();

			attribute = theElement.attributeNode ( MENU_OBJECT_EXTRACTION_PATH );
			if (!attribute.isNull())
				m_qsExtractionPath = attribute.value();

			attribute = theElement.attributeNode ( RECT_X );
			if (!attribute.isNull())
				iX = attribute.value().toInt();

			attribute = theElement.attributeNode ( RECT_Y );
			if (!attribute.isNull())
				iY = attribute.value().toInt();

			attribute = theElement.attributeNode ( RECT_WIDTH );
			if (!attribute.isNull())
				iWidth = attribute.value().toInt();

			attribute = theElement.attributeNode ( RECT_HEIGHT );
			if (!attribute.isNull())
				iHeight = attribute.value().toInt();

			theRect = QRect (iX, iY, iWidth, iHeight);
			setRect(theRect);

			subNode = xmlMenuObject.firstChild ();
			while ( ! subNode.isNull () )	{
				theElement = subNode.toElement();
				tagName = theElement.tagName  ();
				if (tagName == MODIFIERS)	{
					attribute = theElement.attributeNode ( MODIFIERS_ROTATE );
					if (!attribute.isNull())
						modifiers()->fRotate = attribute.value().toFloat();

					attribute = theElement.attributeNode ( MODIFIERS_TRANSPARENCY );
					if (!attribute.isNull())
						modifiers()->fTransparency = attribute.value().toFloat();
					
					attribute = theElement.attributeNode ( MODIFIERS_START_FRAME );
					if (!attribute.isNull())
						modifiers()->iStartFrame = attribute.value().toInt();
					
					attribute = theElement.attributeNode ( MODIFIERS_STOP_FRAME );
					if (!attribute.isNull())
						modifiers()->iStopFrame = attribute.value().toInt();

					attribute = theElement.attributeNode ( MODIFIERS_ZOOM );
					if (!attribute.isNull())
						modifiers()->fZoom = attribute.value().toFloat();

					attribute = theElement.attributeNode ( MODIFIERS_SCALEX);
					if (!attribute.isNull())
						modifiers()->fScaleX = attribute.value().toFloat();

					attribute = theElement.attributeNode ( MODIFIERS_SCALEY );
					if (!attribute.isNull())
						modifiers()->fScaleY = attribute.value().toFloat();

					attribute = theElement.attributeNode ( MODIFIERS_SHEARX );
					if (!attribute.isNull())
						modifiers()->fShearX = attribute.value().toFloat();

					attribute = theElement.attributeNode ( MODIFIERS_SHEARY );
					if (!attribute.isNull())
						modifiers()->fShearY = attribute.value().toFloat();
				}
				//else if (tagName == ANIMATION_OBJECT)	{
				//	m_pData->m_qsAnimation = theElement.text ();
				//}
				subNode = subNode.nextSibling ();
			}
		}
		if ( tagName == ANIMATION_OBJECT )	{
		  m_pData->m_qsAnimation = nodeText;
		}
		else if ( tagName == SHADOW_OBJECT )  {
		  m_pShadow = new ShadowObject ( this );
		  if ( ! m_pShadow->readProjectFile ( xmlMenuObject ) )
		    return false;
		}
/*		else if ( tagName == OVERLAY_OBJECT )  {
		  m_pOverlay = new OverlayObject ( this );
		  if ( ! m_pOverlay->readProjectFile ( xmlMenuObject ) )
		    return false;
		}*/	//oooo
		// So lets get the next sibling ... until we hit hte end of DVDMenu ...
		xmlMenuObject = xmlMenuObject.nextSibling();
	}
	return true;
}

// <...>
// <MenuObject Name="the Name" X="" Y="" Width="" Height="">
//		<Modifiers Rotate="" Zoom="" ScaleX="" ScaleY="" ShearX="" ShearY=""></Modifiers>
// </MenuObject>
// </...>
bool MenuObject::writeProjectFile (QDomElement &theElement)
{
	// First check if this node holds any information at all ...
//printf ("MenuObject::writeProjectFile <%s><%s>\n",(const char *)theElement.tagName(), (const char *)name());
	QDomDocument xmlDoc = theElement.ownerDocument();
	QDomElement objectNode = xmlDoc.createElement( MENU_OBJECT );	// <MenuObject>
	// Here we set the attributes of the <dvdauthor> tag
	if ( ! name ( ).isNull ( ) )
		objectNode.setAttribute( MENU_OBJECT_NAME, name ( ) );
	if ( ! extractionPath  ( ).isNull ( ) )
		objectNode.setAttribute( MENU_OBJECT_EXTRACTION_PATH, extractionPath() );
	objectNode.setAttribute( RECT_X, rect ( ).x ( ) );
	objectNode.setAttribute( RECT_Y, rect ( ).y ( ) );
	objectNode.setAttribute( RECT_WIDTH,   rect ( ).width  ( ) );
	objectNode.setAttribute( RECT_HEIGHT,  rect ( ).height ( ) );
	// And now proceed to writing the Modifiers ... If we need that is ...
	Modifiers initialValues;
	if (	(modifiers()->fRotate != initialValues.fRotate) || (modifiers()->fShearX != initialValues.fShearX)  ||
		(modifiers()->fShearY != initialValues.fShearX) || (modifiers()->fZoom != initialValues.fZoom)      ||
		(modifiers()->fScaleX != initialValues.fScaleX) || (modifiers()->fScaleY != initialValues.fScaleY)  ||
		(modifiers()->iStartFrame   != initialValues.iStartFrame) || (modifiers()->iStopFrame != initialValues.iStopFrame)  ||
		(modifiers()->fTransparency != initialValues.fTransparency) )	{

		QDomElement modifiersNode = xmlDoc.createElement ( MODIFIERS );
		if (modifiers()->fRotate != initialValues.fRotate)
			modifiersNode.setAttribute( MODIFIERS_ROTATE, QString ("%1").arg(modifiers()->fRotate) );
		if (modifiers()->fShearX != initialValues.fShearX)
			modifiersNode.setAttribute( MODIFIERS_SHEARX, QString ("%1").arg(modifiers()->fShearX) );
		if (modifiers()->fShearY != initialValues.fShearY)
			modifiersNode.setAttribute( MODIFIERS_SHEARY, QString ("%1").arg(modifiers()->fShearY) );
		if (modifiers()->fZoom != initialValues.fZoom)
			modifiersNode.setAttribute( MODIFIERS_ZOOM  , QString ("%1").arg(modifiers()->fZoom) );
		if (modifiers()->fScaleX != initialValues.fScaleX)
			modifiersNode.setAttribute( MODIFIERS_SCALEX, QString ("%1").arg(modifiers()->fScaleX) );
		if (modifiers()->fScaleY != initialValues.fScaleY)
			modifiersNode.setAttribute( MODIFIERS_SCALEY, QString ("%1").arg(modifiers()->fScaleY) );
		if (modifiers()->iStartFrame != initialValues.iStartFrame)
			modifiersNode.setAttribute( MODIFIERS_START_FRAME, QString ("%1").arg(modifiers()->iStartFrame) );
		if (modifiers()->iStopFrame != initialValues.iStopFrame)
			modifiersNode.setAttribute( MODIFIERS_STOP_FRAME, QString ("%1").arg(modifiers()->iStopFrame) );
		if (modifiers()->fTransparency != initialValues.fTransparency)
			modifiersNode.setAttribute( MODIFIERS_TRANSPARENCY, QString ("%1").arg(modifiers()->fTransparency) );
		objectNode.appendChild( modifiersNode );
	}
	if ( ! m_pData->m_qsAnimation.isEmpty())	{
		QDomElement animationNode = xmlDoc.createElement( ANIMATION_OBJECT );	// <AnimationObject>
		QDomText text = xmlDoc.createTextNode ( m_pData->m_qsAnimation );
		animationNode.appendChild ( text );
		objectNode.appendChild ( animationNode );
	}
	if ( m_pShadow ) {
	  if ( ! m_pShadow->writeProjectFile ( theElement ) )
	    return false;
	}
	if ( m_pOverlay )  {
	  if ( ! m_pOverlay->writeProjectFile ( theElement ) )
	    return false;
	}
	theElement.appendChild( objectNode );
	return true;
}

void MenuObject::update ( )
{
  QPainter thePainter ( parent ( ) );
  QRect theRect = boundingRect ( );
  //theRect.addCoords ( -5, -5, 5, 5 );		//ooo
  theRect.adjust ( -5, -5, 5, 5 );		//xxx
  //parent()->repaint ( theRect, FALSE );	//ooo
  parent()->repaint ( theRect );		//xxx
  drawContents ( &thePainter );
}

void MenuObject::resetStructureItem ( )
{
  if ( m_pShadow )
       m_pShadow->resetStructureItem  ( );
  if ( m_pOverlay )
       m_pOverlay->resetStructureItem ( );

  if ( m_pStructureItem )
    delete m_pStructureItem;
  m_pStructureItem = NULL;
  modifiers ( )->m_pStructureItem = NULL;
}

QString &MenuObject::objectType()
{
	return m_qsObjectType;
}

QString &MenuObject::name()
{
	return m_pData->m_qsName;
}

QString &MenuObject::extractionPath()
{
	return m_qsExtractionPath;
}

void MenuObject::animationDialog ( QString &qsTempAnim, int iType )
{
  emit ( signalAnimateMe ( qsTempAnim, iType, QString ( ) ) );
}

QString &MenuObject::animation()
{
  return m_pData->m_qsAnimation;
}

Modifiers *MenuObject::modifiers ( )
{
  if ( ! m_pData )
         m_pData = new Data;
  return &m_pData->m_modifiers;
}

void MenuObject::setModifiers ( Modifiers &theModifiers )
{
  *modifiers ( ) = theModifiers;
}

QPoint &MenuObject::currentMousePos ( )
{
  return m_currentMousePos;
}

// Need to take m_pData->m_rectPreview into accoutn ...
bool MenuObject::contains ( QPoint &pos )
{
  if ( ! boundingRect( ).contains ( pos ) )
    return false;
  return true;
}

bool MenuObject::mouseMoveEvent ( QMouseEvent *pEvent )
{
  // This will move, scale rotatae or share the Object ...
  QRect theRect = rect();
  int iX, iY, iWidth, iHeight;

  int iPos = pEvent->pos().x() - m_currentMousePos.x();
  switch ( m_objectState ) {
  case MenuObject::StateScaleTLXY:
  case MenuObject::StateScaleTRXY:
  case MenuObject::StateScaleBLXY:
  case MenuObject::StateScaleBRXY: {
    //double fZoom   = (double)( iPos / 100.0 );                //ooo
    double fZoom   = static_cast<double> ( iPos / 100.0 );      //xxx
    if (fZoom > 1.0)
      fZoom = fZoom * 2.0 - 1.0;

    if ( ( m_objectState == MenuObject::StateScaleTLXY ) ||
         ( m_objectState == MenuObject::StateScaleBLXY )  )
      fZoom *= -1;
    modifiers( )->fScaleX *= ( 1.0 + fZoom );
    modifiers( )->fScaleY *= ( 1.0 + fZoom );
    update   ( );
  }
  break;
  case MenuObject::StateScaleLeftX: 
  case MenuObject::StateScaleRightX: {
    //double fZoom   = (double)( iPos / 100.0 );                //ooo
    double fZoom   = static_cast<double> ( iPos / 100.0 );      //xxx
    if (fZoom > 1.0)
      fZoom = fZoom * 2.0 - 1.0;

    if ( m_objectState == MenuObject::StateScaleLeftX )
      fZoom *= -1;

    modifiers( )->fScaleX *= ( 1.0 + fZoom );
    update   ( );
  }
  break;
  case MenuObject::StateScaleTopY:
  case MenuObject::StateScaleBottomY: {
    iPos = pEvent->pos().y() - m_currentMousePos.y();
    //double fZoom   = (double)( iPos / 100.0 );            //ooo
    double fZoom   = static_cast<double> ( iPos / 100.0 );  //xxx
    if ( fZoom > 1.0 )
         fZoom = fZoom * 2.0 - 1.0;

    if ( m_objectState == MenuObject::StateScaleTopY )
         fZoom *= -1;
    modifiers( )->fScaleY *= (1.0 + fZoom);
    update   ( );
  }
  break;
  case MenuObject::StateRotate: {
    //double fRotate = (double)iPos;                    //ooo
    double fRotate = static_cast<double> ( iPos );      //xxx
    modifiers( )->fRotate += fRotate;
    update   ( );
  }
  break;
  case MenuObject::StateMoveObject:
  default:
    iX = rect().x() - (m_currentMousePos.x() - pEvent->pos().x());
    iY = rect().y() - (m_currentMousePos.y() - pEvent->pos().y());
    iWidth  = rect( ).width  ( );
    iHeight = rect( ).height ( );
    theRect.setX  ( iX );
    theRect.setY  ( iY );
    theRect.setWidth  ( iWidth  );
    theRect.setHeight ( iHeight );

    setRect ( theRect );
    break;
  }
  m_currentMousePos = pEvent->pos ( );

  //  char arrayState[][30] = { "StateUnknown", "StateInactive", "StateMoveObject", "StateScaleTLXY", "StateScaleTRXY", "StateScaleBLXY", "StateScaleBRXY", "StateScaleLeftX", "StateScaleRightX", "StateScaleTopY", "StateScaleBottomY", "StateRotate" };
  //  printf ( "MenuObject::mouseMoveEvent objState<%s>\n", arrayState[m_objectState] );
  return false;
}

void MenuObject::setObjectState ( enState newState )
{
  m_objectState = newState;
}

// the state of the mouse operation.
// This function checks wether the mouse was pressed above a hot spot 
// Resize / rotate etc. or not.
//MenuObject::enState MenuObject::setObjectState ( QPoint &pos, Qt::ButtonState theState )	//oooo
MenuObject::enState MenuObject::setObjectState ( QPoint &pos, Qt::MouseButton theState )	//xxx
{
  m_objectState = StateMoveObject;
  int iHeight2, iWidth2, iX, iY;
  QRect theRect = m_boundingRect;

  iX = pos.x( );
  iY = pos.y( );
  //iWidth2  = theRect.x( ) + (int)((float) theRect.width  ( )  / 2.0 );                            //ooo
  iWidth2  = theRect.x( ) + static_cast<int> ( static_cast<float> ( theRect.width  ( ) ) / 2.0 );   //xxx
  //iHeight2 = theRect.y( ) + (int)((float) theRect.height ( )  / 2.0 );                            //ooo
  iHeight2 = theRect.y( ) + static_cast<int> ( static_cast<float> ( theRect.height ( ) ) / 2.0 );   //xxx
  if ( iX < theRect.x() + HANDLE_WIDTH ) { // left side
    if ( iY < theRect.y() + HANDLE_HEIGHT )
      m_objectState = StateScaleTLXY; // upper left corner
    else if ( iY > theRect.y( ) + theRect.height ( ) - HANDLE_HEIGHT )
      m_objectState = StateScaleBLXY; // lower left corner
    else if ( ( iY > iHeight2 - HANDLE_HEIGHT ) && 
	      ( iY < iHeight2 + HANDLE_HEIGHT )  )
      m_objectState = StateScaleLeftX;  // left side center handle
  }
  else if ( iX > theRect.x( ) + theRect.width () - HANDLE_WIDTH ) { // right side
    if ( iY < theRect.y( ) + HANDLE_HEIGHT )
      m_objectState = StateScaleTRXY; // upper right corner
    else if ( iY > theRect.y( ) + theRect.height ( ) - HANDLE_HEIGHT )
      m_objectState = StateScaleBRXY; // lower right corner
    else if ( ( iY > iHeight2 - HANDLE_HEIGHT ) && 
	      ( iY < iHeight2 + HANDLE_HEIGHT )  )
      m_objectState = StateScaleRightX;  // right side center handle
  }
  else if ( iY < theRect.y() + HANDLE_HEIGHT ) {  // upper handle in the center
    if ( ( iX > iWidth2 - HANDLE_WIDTH ) && ( iX < iWidth2 + HANDLE_WIDTH ) )
      m_objectState = StateScaleTopY;
  }
  else if ( iY > theRect.y() + theRect.height() - HANDLE_HEIGHT ) { // lower handle in the center
    if ( ( iX > iWidth2 - HANDLE_WIDTH ) && ( iX < iWidth2 + HANDLE_WIDTH ) )
      m_objectState = StateScaleBottomY;
  }
  //  else if ( ( iX > iWidth2  - HANDLE_WIDTH  ) && ( iX < iWidth2  + HANDLE_WIDTH  ) &&
  //	    ( iY > iHeight2 - HANDLE_HEIGHT ) && ( iY < iHeight2 + HANDLE_HEIGHT ) ) {
  else if ( theState == Qt::ControlModifier ) {
    m_objectState = StateRotate;
  }
  return m_objectState;
}

void MenuObject::drawActiveFrame ( QPainter *pPainter )
{
  QBrush theBrush ( Qt::yellow );
  QRect theRect = m_boundingRect;
  int iHeight2, iWidth2, iRight, iBottom;

  //iWidth2  = theRect.x( ) + (int)((float) theRect.width  ( )  / 2.0 );                            //ooo
  iWidth2  = theRect.x( ) + static_cast<int> ( static_cast<float> ( theRect.width  ( ) ) / 2.0 );  //xxx
  //iHeight2 = theRect.y( ) + (int)((float) theRect.height ( )  / 2.0 );                            //ooo
  iHeight2 = theRect.y( ) + static_cast<int> ( static_cast <float> ( theRect.height ( ) ) / 2.0 );  //xxx
  iRight   = theRect.x( ) + theRect.width  ( ) - HANDLE_WIDTH;
  iBottom  = theRect.y( ) + theRect.height ( ) - HANDLE_HEIGHT;

  pPainter->drawRect ( theRect ); // the full rect
  // top left
  pPainter->fillRect ( theRect.x(), theRect.y(), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );
  // top right
  pPainter->fillRect ( iRight, theRect.y(), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );
  // top center
  //pPainter->fillRect ( iWidth2 - (int)(HANDLE_WIDTH / 2.0), theRect.y(), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );             //ooo
  pPainter->fillRect ( iWidth2 - static_cast<int> (HANDLE_WIDTH / 2.0), theRect.y(), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );   //xxx

  // bottom left
  pPainter->fillRect ( theRect.x(), iBottom, HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );
  // bottom right
  pPainter->fillRect ( iRight, iBottom, HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );
  // bottom center
  //pPainter->fillRect ( iWidth2 - (int)(HANDLE_WIDTH / 2.0), iBottom, HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );             //ooo
  pPainter->fillRect ( iWidth2 - static_cast<int> (HANDLE_WIDTH / 2.0), iBottom, HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );   //xxx

  // left center
  //pPainter->fillRect ( theRect.x(), iHeight2 - (int)(HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );   //ooo
  pPainter->fillRect ( theRect.x(), iHeight2 - static_cast<int> (HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush ); //xxx
  // right center
  //pPainter->fillRect ( iRight, iHeight2 - (int)(HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );            //ooo
  pPainter->fillRect ( iRight, iHeight2 - static_cast<int> (HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );  //xxx

  // center
  //  pPainter->fillRect ( iWidth2 - (int)(HANDLE_WIDTH / 2.0), iHeight2 - (int)(HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT, theBrush );
  //pPainter->drawEllipse ( iWidth2 - (int)(HANDLE_WIDTH / 2.0), iHeight2 - (int)(HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT );              //ooo
  pPainter->drawEllipse ( iWidth2 - static_cast<int> (HANDLE_WIDTH / 2.0), iHeight2 - static_cast<int> (HANDLE_HEIGHT / 2.0), HANDLE_WIDTH, HANDLE_HEIGHT );    //xxx
}

// called from ButtonPreview::releaseMouseEvent, to reset active object.
bool MenuObject::isActive ()
{
  return m_bDrawRect;
}

void MenuObject::setActive ( bool bActive )
{
  m_bDrawRect = bActive;
}

MenuObject *MenuObject::shadow ( )
{
  return m_pShadow;
}

void MenuObject::setShadow ( MenuObject *pShadow )
{
  if ( m_pShadow )
    delete m_pShadow;

  m_pShadow = pShadow;
}

void MenuObject::updateShadow ( )
{
  if ( m_pShadow )
       m_pShadow->updateShadow ( );
}

MenuObject *MenuObject::overlay ( )
{
  return m_pOverlay;
}

void MenuObject::setOverlay ( MenuObject *pOverlay )
{
  if ( m_pOverlay )
    delete m_pOverlay;

  m_pOverlay = pOverlay;
}

void MenuObject::updateOverlay ( )
{
  if ( m_pOverlay )
       m_pOverlay->updateOverlay ( );
}

QRect &MenuObject::rect ( )
{
  return m_pData->m_rect;
}

MenuObject::Data *MenuObject::data ( )
{
  return m_pData;
}

void MenuObject::setData ( MenuObject::Data *pData )
{
  if ( m_pData && pData )
      *m_pData = *pData;
  if ( m_pMatrixDialog  )
       m_pMatrixDialog->initMe ( this );	//oooo
}

QPoint MenuObject::position ( )
{
  // returns the centered position ...
  QPoint thePosition;
  thePosition.setX ( m_pData->m_rect.x ( ) + (int) ( (float)m_pData->m_rect.width  ( ) / 2.0 ) );
  thePosition.setY ( m_pData->m_rect.y ( ) + (int) ( (float)m_pData->m_rect.height ( ) / 2.0 ) );
  return thePosition;
}

QRect &MenuObject::boundingRect()
{
  return m_boundingRect;
}

void MenuObject::setRect ( QRect &rect )
{
  m_pData->m_rect = rect;
  // The bounding rect is set by drawContents.
  m_boundingRect = rect;
}

void MenuObject::setPosition ( QPoint &position )
{
  int iWidth  = m_pData->m_rect.width  ( );
  int iHeight = m_pData->m_rect.height ( );
  int iX =  position.x () - (int)( (float) iWidth  / 2.0 );
  int iY =  position.y () - (int)( (float) iHeight / 2.0 );
  m_pData->m_rect = QRect ( iX, iY, iWidth, iHeight );

  iWidth  = m_boundingRect.width  ( );
  iHeight = m_boundingRect.height ( );
  iX =  position.x () - (int)( (float) iWidth  / 2.0 );
  iY =  position.y () - (int)( (float) iHeight / 2.0 );
  m_boundingRect = QRect ( iX, iY, iWidth, iHeight );
}


void MenuObject::setCurrentMousePos ( QPoint &pos )
{
  m_currentMousePos = pos;
}

void MenuObject::setParent ( QWidget *pParent )
{  
  m_pParent = pParent;
}

QWidget *MenuObject::parent ( )
{
  return m_pParent;
}

MenuObject *MenuObject::clone( QWidget *, MenuObject * )
{
  return NULL;
}

void MenuObject::setName ( QString &qsName )
{
  m_pData->m_qsName = qsName;
}

void MenuObject::setExtractionPath (QString &qsPath)
{
  m_qsExtractionPath = qsPath;
}

void MenuObject::setAnimation (QString &qsAnimation)
{
  m_pData->m_qsAnimation = qsAnimation;
}

void MenuObject::slotDelete ()
{
  // get a slight delay to allow the ContextMenu to be destroyed ...
  QTimer::singleShot ( 10, this, SLOT ( slotEmitDelete ( ) ) );
}

void MenuObject::slotEmitDelete ( )
{
  emit ( signalDeleteMe ( this ) );
}

void MenuObject::slotAddShadow ( )
{
  // Send request back to be handled in MenuPreview ..
  emit ( signalShadowMe ( this ) );
}

void MenuObject::slotAddOverlay ( )
{
  // Send request back to be handled in MenuPreview ..
  emit ( signalOverlayMe ( this ) );
}

void MenuObject::loadAnimationData (QString &qsFileName, long iMaxNumberOfFrames)
{
  // Called from MenuPreview::createAnimationData
  // Loads in the pure data from a file.
  // Note: changes are done on a per frame basis
  // If no value for a prop at a frame is defined, 
  // then the last value is the winner.
  ///////////////////////////////////////////////

  // Note, these values will change the true props 
  // of the object.
  // Once rendering is done the original values are re-set.
  QFile file (qsFileName);
  file.open ( QIODevice::ReadOnly );
  QTextStream stream( &file );
  QString line;
  uint t;
  long iFrameNumber;
  int  iPos, iPos2;
  bool bFound;
  QString qsProperty, qsValue;

  for (t=0;t<(uint)m_listAnimationAttributes.count();t++)	{
    delete m_listAnimationAttributes[t];
  }
  m_listAnimationAttributes.clear();
  AnimationAttribute *pAnimAttr = NULL;

  while ( !stream.atEnd() ) {
    line = stream.readLine(); // line of text excluding '\n'
    //iPos = line.find ("::");		//ooo
    iPos = line.indexOf ("::");		//xxx
    //iPos2 = line.find ("=");		//ooo
    iPos2 = line.indexOf ("=");		//xxx
    if ( (iPos == -1) || (iPos2 == -1) )
      continue;
    qsProperty   = QString ("");
    qsValue      = QString ("");
    bFound       = false;
    iFrameNumber = line.left ( iPos ).toInt ( &bFound );
    // Note: Frame position 0 is used to store the original values.
    if ( !bFound || ( iFrameNumber < 0 ) || (iFrameNumber > iMaxNumberOfFrames) )
      continue;
    bFound = false;
    qsProperty = line.mid (iPos+2, iPos2-iPos-2);

    qsValue = line.right (line.length() - iPos2-1);
    if (qsValue == "SolidLine")
      qsValue = QString ("%1").arg(Qt::SolidLine);
    else if (qsValue == "DashLine")
      qsValue = QString ("%1").arg(Qt::DashLine);
    else if (qsValue == "DashDotLine")
      qsValue = QString ("%1").arg(Qt::DashDotLine);
    else if (qsValue == "DashDotDotLine")
      qsValue = QString ("%1").arg(Qt::DashDotDotLine);
    else if (qsValue == "MiterJoin")
      qsValue = QString ("%1").arg(Qt::MiterJoin);
    else if (qsValue == "BevelJoin")
      qsValue = QString ("%1").arg(Qt::BevelJoin);
    else if (qsValue == "RoundJoin")
      qsValue = QString ("%1").arg(Qt::RoundJoin);
    //printf ("MenuObject::loadAnimationData - load <%d> <%s>=<%s>\n", iFrameNumber, (const char *)qsProperty, (const char *)qsValue);

    // This loop will check if this property is already taken care of and we have a array of values waiting.
    for (t=0;t<(uint)m_listAnimationAttributes.count ();t++)	{
      pAnimAttr = m_listAnimationAttributes[t];
      if (pAnimAttr->name() == qsProperty)	{
	pAnimAttr->append (iFrameNumber, qsValue);
	bFound = true;
	break;
      }
    }
    if ( ! bFound )	{
      pAnimAttr = NULL;
      // If not a standard property, then branch in the object and get the specific attribute 
      // (I.e. text for TextObject etc. )
      pAnimAttr = getSpecificAttributes (iMaxNumberOfFrames, qsProperty);
      if ( ! pAnimAttr )	{
	if (qsProperty == QString("pos.x"))	{
	  FunctionCallback<MenuObject> *callMeBack = new FunctionCallback<MenuObject>(*this, &MenuObject::setPosition, &MenuObject::position, TYPE_POSITION_X );
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, position().x(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == QString("pos.y"))	{
	  FunctionCallback<MenuObject> *callMeBack = new FunctionCallback<MenuObject>(*this, &MenuObject::setPosition, &MenuObject::position, TYPE_POSITION_Y );
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, position().y(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == QString("rect.x1"))	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setX);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.x(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == QString("rect.x2"))	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setRight);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.right(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "rect.y1")	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setY);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.y(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "rect.y2")	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setBottom);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.bottom(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "rect.width")	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setWidth);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.width(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "rect.height")	{
	  FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_pData->m_rect, &QRect::setHeight);
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_pData->m_rect.height(), callMeBack);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "visibleFromTo.startFrame") {
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->iStartFrame, &modifiers ( )->iStartFrame);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "visibleFromTo.stopFrame") {
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->iStopFrame, &modifiers ( )->iStopFrame);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "transparency")
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fTransparency, &modifiers ( )->fTransparency);
	else if (qsProperty == "zoom")
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fZoom, &modifiers ( )->fZoom);
	else if (qsProperty == "rotate")
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fRotate, &modifiers ( )->fRotate);
	else if (qsProperty == "shear.x")
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fShearX, &modifiers ( )->fShearX);
	else if (qsProperty == "shear.y")
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fShearY, &modifiers ( )->fShearY);
	else if (qsProperty == "scale.x") {
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fScaleX, &modifiers ( )->fScaleX);
	  pAnimAttr->setNeedRefresh ( false );
	}
	else if (qsProperty == "scale.y") {
	  pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, modifiers ( )->fScaleY, &modifiers ( )->fScaleY);
	  pAnimAttr->setNeedRefresh ( false );
	}
      }
      if (pAnimAttr)
	m_listAnimationAttributes.append ( pAnimAttr );
    }
    if ( ( pAnimAttr ) && ( ! bFound ) )
      pAnimAttr->append (iFrameNumber, qsValue);
  }
  file.close();
}

void MenuObject::slotDeleteMtxDlg ( )
{
  if ( m_pMatrixDialog )
       delete m_pMatrixDialog;
  m_pMatrixDialog = NULL;
}

