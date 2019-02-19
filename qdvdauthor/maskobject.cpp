/***************************************************************************
    maskobject.cpp
                             -------------------
    ShadowObject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class creates a ButtonMask for an associated ImageObject.
    
****************************************************************************/

#include <math.h>
#include <qpainter.h>
//#include <q3popupmenu.h>	/ooo
#include <qcolordialog.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QMouseEvent>

#include "global.h"
#include "xml_dvd.h"
//#include "movieobject.h"	//ooo
#include  "imageobject.h"	//xxx
#include "structuretoolbar.h"

#include "buttonobject.h"
#include "maskobject.h"

MaskObject::MaskObject ( QWidget *pParent )
  : MenuObject ( pParent )
{
  m_pMenuObject   = NULL; 
  m_qsObjectType  = QString ( MASK_OBJECT );
}

MaskObject::MaskObject ( MenuObject *pMenuObject, QWidget *pParent )
  : MenuObject  ( pParent )
{
  m_pMenuObject   = NULL;
  m_qsObjectType  = QString ( MASK_OBJECT );
  setParent ( pMenuObject );
}

MaskObject::~MaskObject ( )
{
}

MaskObject &MaskObject::operator = ( MaskObject &theOther )
{
  setColor ( theOther.color ( ) );
  return *this;
}

bool MaskObject::operator == ( MaskObject &theOther )
{
  return ( color ( ) == theOther.color ( ) );
}

void MaskObject::setParent ( MenuObject *pMenuObject )
{
  // If we pass a ButtonObject we realy are iinterested in the normal(0) - object
  if ( pMenuObject && pMenuObject->objectType ( ) == BUTTON_OBJECT )  {
    ButtonObject *pButton = (ButtonObject *)pMenuObject;
    pMenuObject = pButton->getNormal ( 0 );
  }

  m_pMenuObject = pMenuObject;
}

void MaskObject::drawContents ( QPainter *pPainter )
{ 
  drawMask ( pPainter );
}

void MaskObject::drawMask ( QPainter *pPainter )
{
  if ( ! m_pMenuObject )
    return;

  updateMask ( );

  QRect theRect  = m_pMenuObject->boundingRect ( );
  pPainter->drawImage ( theRect.x ( ), theRect.y ( ), m_imageMask );
}

void MaskObject::updateMask ( )
{
  // since this is only created once per DVD creation process, this is not as time sensitive. 
  // Thus we can simply create the mask every time from scratch.
  createMask ( );
}
/*
void MaskObject::filterColor ( QImage &theImage, QColor theColor )
{
  // At this point we have the frame/text/rect and need to blur this into a semi transparent image
  int x, y;
  QRgb *pLine, colorToFilter, currentPixel, colorTransparent;

  colorTransparent = qRgba ( 0, 0, 0, 0 );
  colorToFilter    = theColor.rgb ( ) | 0xff000000;
  for ( y=0;  y < theImage.height ( ); y++ ) {
    pLine = (QRgb *) theImage.scanLine ( y );
    for ( x=0; x < theImage.width ( ); x++ ) {
      currentPixel = *(pLine + x);
      if ( currentPixel == colorToFilter )
	*(pLine + x) = colorTransparent;
    }
  }
}
*/
void MaskObject::createMask ( )
{
  if ( ! m_pMenuObject )
    return;

  if ( ( m_pMenuObject->objectType ( ) == IMAGE_OBJECT ) ||
       ( m_pMenuObject->objectType ( ) == MOVIE_OBJECT ) ) {
    QImage theImage;
    ImageObject *pImageObject = (ImageObject *)m_pMenuObject;

    // Now that we have the object to draw we should create the QImage - object ...
    theImage = pImageObject->image ( ).copy ( );  // Copy is required otherwise the orig image is changed.
    //theImage.setAlphaBuffer ( true );		//oooo
    //    http://bullwinkle.toonzone.net/rbplan.wav
    // Lastly we should filter out color keys if present (MovieObject, and Imagebject only)

    filterColorKeys ( theImage );
    m_imageMask = theImage;
  }
}

bool MaskObject::filterColorKeys ( QImage &theShadow )
{
  ImageObject tempImage, *pImageObject;
  /*MovieObject tempMovie;
  // Only Image, or Movie objects of interest for color keying.
  if ( ( m_pMenuObject->objectType ( ) != tempImage.objectType ( ) ) &&
       ( m_pMenuObject->objectType ( ) != tempMovie.objectType ( ) ) )
    return false;*/	//oooo

  // Note MovieObject is derived off ImageObject.
  pImageObject = (ImageObject *)m_pMenuObject;
  // Check if Color Keying is enabled.
  int   x, y;
  QRgb *pMaskLine;
  Rgba  color ( m_color );
  unsigned char  alpha = m_color.alpha ( );
  float fAlpha = alpha / 255.0;

  if ( pImageObject->manipulator( ).listColorKeys.count ( ) < 1 ) {
    for   ( y=0; y < theShadow.height ( ); y++ )  {
      pMaskLine = (QRgb *)theShadow.scanLine  ( y );
      for ( x=0; x < theShadow.width  ( ); x++ )  {
//        *( pMaskLine + x ) = m_color.rgb ( );
        alpha = ( *( pMaskLine + x ) & 0xFF000000 ) >> 24;
        color.setAlpha ( (unsigned char) ( alpha * fAlpha ) );
        *( pMaskLine + x ) = color.rgb ( );
      }
    }
    return true;
  }

  // Next we create the min/max Color keys, to check if we ought to filter the pixel out or not
  ImageManipulator *pManipulator = &pImageObject->manipulator ( );
  QImage theImage   = pImageObject->image ( );
  QList<QColor *>listColorKeyMin;	//oxx
  QList<QColor *>listColorKeyMax;	//oxx
  QColor *pColorMin, *pColorMax, theColor, targetColor;
  QRgb thePixel, *pLine, *pShadowLine;

  int t, iX, iY, iDelta, iRed, iGreen, iBlue, iColorKeyCount;
  iColorKeyCount = pManipulator->listColorKeys.count();
  for (t=0;t<iColorKeyCount;t++)	{
    iDelta = (int)(pManipulator->listColorKeys[t]->fDeltaColor * 255.0);
    theColor = QColor (pManipulator->listColorKeys[t]->theColor);
    iRed = theColor.red() - iDelta; iGreen = theColor.green() - iDelta; iBlue = theColor.blue() - iDelta;
    iRed = (iRed < 0) ? 0 : iRed; iGreen = (iGreen < 0) ? 0 : iGreen; iBlue = (iBlue < 0) ? 0 : iBlue;
    pColorMin = new QColor (iRed, iGreen, iBlue);
    iRed = theColor.red() + iDelta; iGreen = theColor.green() + iDelta; iBlue = theColor.blue() + iDelta;
    iRed = (iRed > 255) ? 255 : iRed; iGreen = (iGreen > 255) ? 255 : iGreen; iBlue = (iBlue > 255) ? 255 : iBlue;
    pColorMax = new QColor (iRed, iGreen, iBlue);

    listColorKeyMin.append (pColorMin);
    listColorKeyMax.append (pColorMax);
  }

  for (y=0; y < theShadow.height ( ); y++)	{
    iY = y;
    if ( iY >= theImage.height ( ) )
         iY  = theImage.height ( )-1;
    pLine = (QRgb *)theImage.scanLine  ( iY );
    iY = y;
    if ( ( iY >= 0 ) && ( iY < theShadow.height ( ) ) ) {
      pShadowLine = (QRgb *)theShadow.scanLine  ( iY  );
      for (x=0; x < theShadow.width ( ); x++)	{
	iX = x;
	if ( ( iX >= 0) && ( iX < theShadow.width ( ) ) ) {
	  if ( x >= theImage.width ( ) )
	    continue;
	  else
	    thePixel = *(pLine + x);

	  if ( thePixel != 0x00000000 )  {	// And check if this is a pixel we need to handle ...
	    targetColor = QColor ( thePixel );
	    if ( filterColorKey  ( &targetColor, iColorKeyCount, &listColorKeyMin, &listColorKeyMax ) )
	      *( pShadowLine + iX ) = qRgba ( 0, 0, 0, 0 );
	    else
	      *( pShadowLine + iX ) = m_color.rgb ( ); // | 0x80000000;
	  }
	}
      }
    }
  }
  for ( t=0; t<iColorKeyCount; t++ )  {
    delete listColorKeyMin[t];
    delete listColorKeyMax[t];
  }

  return true;
}

// Same as ImageObject::filterColorKey.
bool MaskObject::filterColorKey ( QColor *pColor, int iColorKeyCount, QList<QColor *> *pListColorMin, QList<QColor *>*pListColorMax )	//oxx
{
  int iRed, iGreen, iBlue, t;

  if ( ( iColorKeyCount < 1 ) || ( ! pListColorMin ) || ( ! pListColorMax ) )
    return false;

  // If image is rotated then pitch black ought to be removed ...
  if ( ( ( m_pMenuObject->modifiers ( )->fShearX != 0.0 ) || 
	 ( m_pMenuObject->modifiers ( )->fShearX != 0.0 ) || 
	 ( m_pMenuObject->modifiers ( )->fRotate != 0.0 ) ) && 
       ( *pColor == QColor ( 0, 0, 0 ) ) )
    return true;

  //iRed = pColor->Qt::red(); iGreen = pColor->Qt::green(); iBlue = pColor->Qt::blue ();	//ooo
  iRed = pColor->red(); iGreen = pColor->green(); iBlue = pColor->blue ();			//xxx
  for (t=0;t<iColorKeyCount;t++)	{
    //if ( (iRed   >= (*pListColorMin)[t]->Qt::red  () && iRed   <= (*pListColorMax)[t]->Qt::red  ()) &&	//ooo
    if ( (iRed   >= (*pListColorMin)[t]->red  () && iRed   <= (*pListColorMax)[t]->red  ()) &&			//xxx
	 //(iGreen >= (*pListColorMin)[t]->Qt::green() && iGreen <= (*pListColorMax)[t]->Qt::green()) &&	//ooo
         (iGreen >= (*pListColorMin)[t]->green() && iGreen <= (*pListColorMax)[t]->green()) &&			//xxx
	 //(iBlue  >= (*pListColorMin)[t]->Qt::blue () && iBlue  <= (*pListColorMax)[t]->Qt::blue ()) )		//ooo
	 (iBlue  >= (*pListColorMin)[t]->blue () && iBlue  <= (*pListColorMax)[t]->blue ()) )			//xxx
      return true;
  }
  return false;
}

bool MaskObject::writeProjectFile ( QDomElement &theElement ) 
{
  QDomDocument xmlDoc     = theElement.ownerDocument  ( );
  QDomElement  shadowNode = xmlDoc.createElement ( MASK_OBJECT );  // <ShadowObject>
  shadowNode.setAttribute ( MASK_OBJECT_COLOR, m_color.toString ( ) );
  theElement.appendChild  ( shadowNode );  

  return true;
}

bool MaskObject::readProjectFile ( QDomNode &theNode )
{
  QDomElement theElement = theNode.toElement();
  QDomAttr attribute;

  attribute = theElement.attributeNode ( MASK_OBJECT_COLOR );
  if ( ! attribute.isNull ( ) )
    m_color.fromString ( attribute.value ( ) );

  return true;
}

void MaskObject::setColor ( Rgba color )
{
  m_color = color;
}

Rgba &MaskObject::color ( )
{
  return m_color;
}

void MaskObject::replaceColor ( Rgba theColor, Rgba oldColor )
{
  if ( m_color == oldColor ) {
       m_color  = theColor;
       updateMask ( );
  }
}

StructureItem *MaskObject::createStructure ( StructureItem *pParentItem )
{
  if ( ! pParentItem )  {
    m_pStructureItem = NULL;
    return NULL;
  }

  if ( ! m_pStructureItem ) {
    m_pStructureItem = new StructureItem ( this, StructureItem::Unknown, pParentItem, "Mask Object" );
    //m_pStructureItem->setExpandable ( FALSE );	//ooo
    m_pStructureItem->setExpanded ( false );		//xxx
  }
  return m_pStructureItem;
}

QRect &MaskObject::boundingRect()
{
  if ( m_pMenuObject )
    return m_pMenuObject->boundingRect ( );
  return m_boundingRect;
}

bool MaskObject::mousePressEvent ( QMouseEvent *pEvent )
{
  m_currentMousePos = pEvent->pos();
  if ( pEvent->button ( ) == Qt::RightButton )  {
    QPoint globalPos = pEvent->globalPos ( );
    return createContextMenu ( globalPos );
  }
  else
    m_bDrawRect = true;
  return false;
}

bool MaskObject::createContextMenu ( QPoint )
{
  //  QPopupMenu *pStackMenu = new QPopupMenu ( m_pParent );
  //  pStackMenu->insertItem ( tr ("Change Color ...") , this, SLOT ( slotChangeColor ( ) ) );
  //  pStackMenu->exec ( globalPos, 1 );

  //  delete pStackMenu;
  return true;
}

void MaskObject::slotChangeColor ( )
{
  // Never called, now handled inside the ButtonDialog
  bool bOkay = false;
  QRgb rgba = QColorDialog::getRgba ( color ( ).rgb ( ), &bOkay );
  if ( ! bOkay ) 
    return;

  if ( rgba == color ( ).rgb ( ) )
    return; // nothing has changed

  Rgba newColor ( rgba );

  emit ( signalMaskColorChanged ( newColor, color ( ) ) );
  setColor ( newColor );
  emit ( signalUpdatePixmap ( ) );
}

MenuObject *MaskObject::clone ( QWidget *pParentWidget, MenuObject *pParent )
{
  if ( ! pParentWidget )
    pParentWidget =  MenuObject::parent ( );
  MaskObject *pNewObject = new MaskObject( pParent, pParentWidget );
  pNewObject->setColor     ( color      ( ) );

  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect      (  rect      ( ) );
  pNewObject->setModifiers ( *modifiers ( ) );

  return pNewObject;
}
