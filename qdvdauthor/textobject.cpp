/***************************************************************************
    textobject.cpp
                             -------------------
    TextObject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QPen>
#include <QPainter>
#include <QMenu>
#include <QMouseEvent>

#include <QMessageBox>

#include "global.h"
#include "xml_dvd.h"
#include "dialogtextfont.h"
#include "dialogmatrix.h"
#include "menuobject.h"
#include "textobject.h"	// derived from menuobject
#include "structuretoolbar.h"
#include "qdvdauthor.h"
#include "dvdmenuundoobject.h"

TextObject::TextData::TextData ( )
  : MenuObject::Data ( )
{
  m_iDataType       = Data::TEXT;
  m_colorBackground = Rgba ( TRANSPARENT_COLOR ); // indicate the transparent color.
  m_iStyleStrategy  = QFont::PreferAntialias;
  m_bAllowWordBreak = true;
}

TextObject::TextData::~TextData ( )
{
}

MenuObject::Data &TextObject::TextData::operator = ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  MenuObject::Data::operator= ( theOther );
  if ( m_iDataType == theOther.m_iDataType )  {
       TextData *pOther  = (TextData *)&theOther;
       m_qsText          = pOther->m_qsText;
       m_Font            = pOther->m_Font;
       m_ptTextAlign     = pOther->m_ptTextAlign;
       m_colorForeground = pOther->m_colorForeground;
       m_colorBackground = pOther->m_colorBackground;
       m_iStyleStrategy  = pOther->m_iStyleStrategy;
       // don't need ot be stored. Only set after DialogTextFont
       m_bAllowWordBreak = pOther->m_bAllowWordBreak;
  }

  return *this;
}

bool TextObject::TextData::operator == ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  bool bReturn = ( MenuObject::Data::operator == ( theOther ) );
  if ( ! bReturn )
    return false;

  if ( m_iDataType  == theOther.m_iDataType )  {
    TextData *pOther = (TextData *)&theOther;
    bReturn = ( ( m_qsText          == pOther->m_qsText          ) &&
                ( m_Font            == pOther->m_Font            ) &&
                ( m_ptTextAlign     == pOther->m_ptTextAlign     ) &&
                ( m_colorForeground == pOther->m_colorForeground ) &&
                ( m_colorBackground == pOther->m_colorBackground ) &&
                ( m_iStyleStrategy  == pOther->m_iStyleStrategy  ) &&
                ( m_bAllowWordBreak == pOther->m_bAllowWordBreak ) );
  }
  return bReturn;
}

MenuObject::Data *TextObject::TextData::clone ( )
{
  TextData *pData = new TextData;
  *pData  = *this;
  return pData;
}

TextObject::TextObject ( QWidget *pParent )
  : MenuObject ( pParent )
{
   if ( m_pData )
    delete m_pData;        // part of the base class			//oooo
  m_pData = new TextData; // will be deleted in base class

  m_qsObjectType    = QString ( TEXT_OBJECT );
  m_pContextMenu    = NULL; 	//oooo

}

TextObject::~TextObject ( )
{
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL; 
}

TextObject::TextData *TextObject::textData ( )
{
  return (TextData *)m_pData;
}

void TextObject::setFont ( QFont &theFont )
{
  textData ( )->m_Font = theFont;
}

QFont &TextObject::getFont ()
{
  return textData ( )->m_Font;
}

void TextObject::setWordBreak ( bool bBreak )
{
  textData ( )->m_bAllowWordBreak = bBreak;
}

void TextObject::setText (QString &theText)
{
  textData ( )->m_qsText = theText;
  // Here we seal off after 20 char and remove special characters from the Object Name.
  QString qsTemp = theText;
  qsTemp = qsTemp.remove  ( '\n' );
  qsTemp = qsTemp.remove  ( '\t' );
  if ( qsTemp.length ( )  > 20 )
    qsTemp = qsTemp.left  ( 20 );
  setName ( qsTemp );
}

QString &TextObject::getText ()
{
	return textData ( )->m_qsText;
}

void TextObject::setTextAlign (QPoint &newAlign)
{
	textData ( )->m_ptTextAlign = newAlign;
}

QPoint &TextObject::getTextAlign ()
{
	return textData ( )->m_ptTextAlign;
}

void TextObject::setStyleStrategy (int iStyleStrategy)
{
	textData ( )->m_iStyleStrategy = iStyleStrategy;
	textData ( )->m_Font.setStyleStrategy ((QFont::StyleStrategy)iStyleStrategy);
}

int TextObject::getStyleStrategy ()
{
  return (int)textData ( )->m_iStyleStrategy;
}

void TextObject::setForegroundColor ( const Rgba &theColor)
{
  textData ( )->m_colorForeground = theColor;
}

void TextObject::setBackgroundColor ( const Rgba &theColor)
{
  textData ( )->m_colorBackground = theColor;
}

Rgba &TextObject::getForegroundColor ()
{
  return textData ( )->m_colorForeground;
}

Rgba &TextObject::getBackgroundColor ()
{
  return textData ( )->m_colorBackground;
}

void TextObject::drawSemiTransparent (QPainter *pPainter)
{  
  //printf ( "%s::%s : %d  \n", __FILE__, __FUNCTION__, __LINE__ );
  if ( m_pShadow && ! m_bDrawRect )
       m_pShadow->drawContents ( pPainter );

  QMatrix theMatrix;
  QRect dummyRect, theRect;
  int iFlags, x, y;
  int iSourceHeight, iSourceWidth, iTargetStartX, iTargetStartY;
  Rgba sourceColor, targetColor, theColor;
  QRgb sourcePixel, targetPixel;
  QImage sourceImg, targetImg;
  float fTargetTrans, fSourceTrans;

  // init variables ...
  fTargetTrans = 1.0 - modifiers()->fTransparency;
  fSourceTrans = modifiers()->fTransparency;

  QPen thePen ( textData ( )->m_colorForeground.color ( ), 2, Qt::DashDotLine );
  // Here we define the cenetered rect.
  //theRect = QRect((int)-(rect().width()/ 2.0), (int)-(rect().height()/2.0), rect().width(), rect().height()); //ooo
  theRect = QRect(static_cast<int>(-(rect().width()/ 2.0)), static_cast<int>(-(rect().height()/2.0)), rect().width(), rect().height()); //xxx

  // First we translate to the center of the rect.
  //theMatrix.translate ( (double)rect().x() + (int)(rect().width () / 2.0),                        //ooo
  theMatrix.translate ( static_cast<double>(rect().x()) + static_cast<int>(rect().width () / 2.0),  //xxx
		        //(double)rect().y() + (int)(rect().height() / 2.0) );                          //ooo
                        static_cast<double>(rect().y()) + static_cast<int>(rect().height() / 2.0) );    //xxx
  //. then we apply the other modifications ...
  theMatrix.scale  (modifiers()->fScaleX, modifiers()->fScaleY);
  theMatrix.rotate (modifiers()->fRotate);
  theMatrix.shear  (modifiers()->fShearX, modifiers()->fShearY);
  // Here we get the rect that encompasses the button for this object.
  m_boundingRect = theMatrix.mapRect(theRect);
  // And for the source we need to set the right matrix.
  theMatrix.reset  ();
  //theMatrix.translate ((double)m_boundingRect.width()/2.0, (double)m_boundingRect.height()/2.0);                          //ooo
  theMatrix.translate (static_cast<double>(m_boundingRect.width())/2.0, static_cast<double>(m_boundingRect.height())/2.0);  //xxx
  theMatrix.scale  (modifiers()->fScaleX, modifiers()->fScaleY);
  theMatrix.rotate (modifiers()->fRotate);
  theMatrix.shear  (modifiers()->fShearX, modifiers()->fShearY);

  QPixmap thePixmap = QPixmap ( m_boundingRect.size ( ) );
  if (thePixmap.isNull())
    return;

  thePixmap.fill ( QColor ( TRANSPARENT_COLOR ) );    //ooo
  QPainter pmPainter (&thePixmap);
  //  then we actually set the world matrix 
  pmPainter.setWorldMatrix(theMatrix);

  pmPainter.setPen   ( Qt::NoPen );
  pmPainter.setBrush ( QBrush    ( textData ( )->m_colorBackground.color ( ) ) ); //ooo
  pmPainter.drawRect ( theRect   );   //ooo

  textData ( )->m_Font.setStyleStrategy ((QFont::StyleStrategy)textData ( )->m_iStyleStrategy);
  pmPainter.setFont(textData ( )->m_Font);

  // Okay, I abused a QPoint class to hold some other infos ... so what ???
  //iFlags = Qt::DontClip|Qt::WordBreak|textData ( )->m_ptTextAlign.x()|textData ( )->m_ptTextAlign.y();
  iFlags = Qt::TextDontClip|textData ( )->m_ptTextAlign.x ( )|textData ( )->m_ptTextAlign.y ( );
  if ( textData ( )->m_bAllowWordBreak )
    iFlags |= Qt::TextWordWrap;

  //pmPainter.setPen(Qt::blue);   //xxx
  pmPainter.setPen(textData ( )->m_colorForeground.color ( ));   //xxx
  //pmPainter.setFont(QFont("Arial", 30));    //xxx
  //pmPainter.drawText(theRect, iFlags, textData ( )->m_qsText, -1, &dummyRect);	//ooo
  pmPainter.drawText(theRect, iFlags, textData ( )->m_qsText, &dummyRect);		//xxx
  //pmPainter.drawText( 0, 0, 100, 50, Qt::AlignCenter, "Qt");  //xxx
  //p.drawText( 0, 0, iSourceWidth, iSourceHeight, Qt::AlignCenter, "Qt");  //xxx
  //pmPainter.drawText( 0, 0, iSourceWidth, iSourceHeight, Qt::AlignCenter, "Qt");  //xxx
  pmPainter.end();  //xxx
  
  // At this point we should have the text on a memory Bitmap object, and now 
  // we should copy this over, bit for bit. 
  iSourceHeight = thePixmap.height(); 
  iSourceWidth  = thePixmap.width ();
  iTargetStartX = pPainter->viewport().x() + m_boundingRect.x();
  iTargetStartY = pPainter->viewport().y() + m_boundingRect.y();

  // First we need to create a Pixmap which holds the Target pixels
  QPaintDevice *pPDPainter = pPainter->device();
  if (pPDPainter == 0)
    return;

  //sourceImg = thePixmap.convertToImage();	//ooo
  sourceImg = thePixmap.toImage();		//xxx

  QPixmap *pPixmap = new QPixmap(iSourceWidth, iSourceHeight);
  if ( ! pPixmap )
    return;

  //bitBlt (pPixmap, 0, 0, pPainter->device(), 							//oooo
//	  //iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, Qt::CopyROP, true);	//oooo
  //        iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, Qt::AutoColor);		//xxx
    //QRect devRect(0, 0, pPainter->device()->width(), pPainter->device()->height());                     //xxx
  //QImage image(48, 48, QImage::Format_RGB32);   //xxx
  //QImage image(iSourceWidth, iSourceHeight, QImage::Format_RGB32);   //xxx
  //image.fill(QColor(0,0,0));    //xxx
  //image.load(pPainter->device());    //xxx
  //image(pPainter->widget());    //xxx
  QPainter p( pPixmap );											//xxx
    //p.setPen(Qt::blue); //xxx
    //p.setFont(QFont("Arial", 30));  //xxx
    //p.drawText( 0, 0, iSourceWidth, iSourceHeight, Qt::AlignCenter, "Qt");  //xxx
    //p.drawRect( 0, 0, iSourceWidth, iSourceHeight );	                                //xxx
    //p.begin(pPainter->device());    //xxx
    //pmPainter.begin(pPainter->device());    //xxx
    //p.fillRect(0, 0, iSourceWidth, iSourceHeight, pPainter->background());  //xxx
    //p.drawImage(0,0, (QImage) (*((QImage*)pPainter->device())); //xxx
    //p.drawImage(0,0, (QImage) (*((QImage*)pPainter->device()))); //xxx
    //p.drawImage( 0, 0, pPainter->device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );		//xxx
    //p.drawImage( 0, 0, image );		//xxx
    //p.end();    //xxx

  //QPainter p( pPixmap );  
    //pmPainter.drawImage( 0, 0, image );		//xxx
    //pmPainter.drawText(theRect, iFlags, textData ( )->m_qsText, &dummyRect);		//xxx
    //pPainter->background();		//xxx
    //p.drawPixmap( iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, the_Pixmap );		//xxx
    //p.drawPixmap( 0, 0, iSourceWidth, iSourceHeight, the_Pixmap );		//xxx
    p.drawPixmap( 0, 0, the_Pixmap, iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );		//xxx
    //p.drawImage( 0, 0, pPainter->background(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );	//xxx
    //p.drawRect( iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );	                                //xxx
    //p.drawText( iTargetStartX, iTargetStartY, "Qt5" );	                                //xxx

    p.end();    //xxx
    //pmPainter.end();    //xxx
    //QPainter p;									//xxx
  //p.begin(pPainter->device());							//xxx
    //p.drawRect(iTargetStartX, iTargetStartY, iSourceWidth-10, iSourceHeight-10 );	//xxx
  //p.setViewport(iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );	//xxx
    //QPen myPen(Qt::black, 2, Qt::SolidLine);                                      //xxx
    //myPen.setColor(Qt::red);                                                      //xxx
    //p.drawPoint(iTargetStartX+10,iTargetStartY+10);                                                         //xxx
  //p.end();                                                                      //xxx
    //QPainter p(pPainter->device());   //xxx
    //p.drawLine(1,1,100,100);    //xxx
    //p.drawRect( iTargetStartX+5, iTargetStartY+5, iSourceWidth-5, iSourceHeight-5 );  //xxx
    //p.drawRect(1,1,100,100);    //xxx
    //QImage image(pPainter->device()); //xxx
    //image.copy(iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );     //xxx
  // To get Pixel information we need a QImage however ...
  //targetImg = pPixmap->convertToImage();	//ooo
  targetImg = pPixmap->toImage();		//xxx
  
  delete pPixmap;

  iTargetStartX -= pPainter->viewport().x();
  iTargetStartY -= pPainter->viewport().y();
  for (y=0; y < iSourceHeight; y++)	{
    for (x=0; x < iSourceWidth; x++)	{
      sourcePixel = sourceImg.pixel(x, y );	// calculate the mix of background and foreground.
      sourcePixel &= 0x00ffffff;// filter out the transparency part of the color
      if (sourcePixel != 0x00FFFFFF)	{	// And check if this is a pixel we 
	targetPixel = targetImg.pixel(x, y);
	targetPixel &= 0x00ffffff;	// filter out the transparency part of the color
	targetColor = Rgba ( targetPixel );
	sourceColor = Rgba ( sourcePixel );
	theColor    = Rgba ( (int)(fSourceTrans * sourceColor.red()   + (int)(fTargetTrans * targetColor.red())),                    //ooo
    //theColor    = Rgba ( static_cast<int>(fSourceTrans * sourceColor.red()   + /*static_cast<int>*/(fTargetTrans * targetColor.red())), //xxx
			     (int)(fSourceTrans * sourceColor.green() + (int)(fTargetTrans * targetColor.green())),                        //ooo
                 //static_cast<int>(fSourceTrans * sourceColor.green() + /*static_cast<int>*/(fTargetTrans * targetColor.green())),   //xxx
			     (int)(fSourceTrans * sourceColor.blue()  + (int)(fTargetTrans * targetColor.blue()))                          //ooo
                 //static_cast<int>(fSourceTrans * sourceColor.blue()  + /*static_cast<int>*/(fTargetTrans * targetColor.blue()))     //xxx
			     );
	pPainter->setPen ( theColor.color ( ) );
	pPainter->drawPoint(x+iTargetStartX, y+iTargetStartY);
      }
    }
  }
}

void TextObject::drawContents ( QPainter *painter )
{
  QMatrix theMatrix;
  // If we have a complete transparent text we should not draw at all.
  if ( modifiers ( )->fTransparency == 1.0 )
    return;
  else if ( (modifiers ( )->fTransparency != 0.0 ) && ( ! m_bDrawRect ) )
    return drawSemiTransparent ( painter );	//oooo

  if ( m_pShadow && ! m_bDrawRect )
    m_pShadow->drawContents ( painter );
  // Here we calculate the center point of gravity (rotation)
  QPoint centerPos;
  //centerPos.setX (rect().x() + (int)(rect().width () / 2.0));             //ooo
  centerPos.setX (rect().x() + static_cast<int>(rect().width () / 2.0));    //xxx
  //centerPos.setY (rect().y() + (int)(rect().height() / 2.0));             //ooo
  centerPos.setY (rect().y() + static_cast<int>(rect().height() / 2.0));    //xxx
  // Here we define the cenetered rect.
  QRect theRect ((int)-(rect().width()/ 2.0), (int)-(rect().height()/2.0), rect().width(), rect().height());

  // From the Qt documentation ...
  // This transformation engine is a three-step pipeline
  // The first step uses the world transformation matrix.
  // The second step uses the window.
  // The third step uses the viewport.

  // First we translate to the appropriate location,
  //theMatrix.translate ( (double)centerPos.x ( ), (double)centerPos.y ( ) );                           //ooo
  theMatrix.translate ( static_cast<double>(centerPos.x ( )), static_cast<double>(centerPos.y ( )) );   //xxx

  //. then we apply the other modifications ...
  theMatrix.scale  ( modifiers ( )->fScaleX, modifiers ( )->fScaleY );
  theMatrix.rotate ( modifiers ( )->fRotate);
  theMatrix.shear  ( modifiers ( )->fShearX, modifiers ( )->fShearY );

  QPen thePen ( textData ( )->m_colorForeground.color ( ), 2, Qt::DashDotLine );
  painter->setPen ( thePen );

  // Here we get the rect that encompasses the button for this object.
  m_boundingRect = theMatrix.mapRect ( theRect );
  if ( m_bDrawRect )
    drawActiveFrame ( painter );

  //  then we actually set the world matrix 
  painter->setWorldMatrix ( theMatrix );

  // and continue to draw the contents ...
  if ( textData ( )->m_colorBackground.alpha ( ) > 0 )  {
    // No frame wanted, we take care of that ourself if requested ...
    painter->setPen   ( Qt::NoPen );
    painter->setBrush ( QBrush ( textData ( )->m_colorBackground.color ( ) ) );
    painter->drawRect ( theRect );
    // and re-set the PEN.
    painter->setPen   ( thePen );
  }
  else
    painter->setBrush ( Qt::NoBrush );

  textData ( )->m_Font.setStyleStrategy ( (QFont::StyleStrategy)textData ( )->m_iStyleStrategy );
  painter->setFont ( textData ( )->m_Font );
  QRect boundingRect;
  bool  bUpdateAgain = false;
  // Okay, I abused a QPoint class to hold some other infos ... so what ???
  int iFlags = Qt::TextDontClip|textData ( )->m_ptTextAlign.x ( )|textData ( )->m_ptTextAlign.y ( );
  if ( textData ( )->m_bAllowWordBreak )
    iFlags |= Qt::TextWordWrap;

  if ( m_bDrawRect )
    painter->drawRect ( theRect );

  //painter->drawText ( theRect, iFlags, textData ( )->m_qsText, -1, &boundingRect );	//ooo
  painter->drawText ( theRect, iFlags, textData ( )->m_qsText, &boundingRect );		//xxx
  
  // Here we expand the rectangle if needs be, and update again, to re-draw.
  if ( rect ( ).width ( ) < boundingRect.width ( ) )  {
    rect ( ).setWidth ( boundingRect.width ( ) );
    bUpdateAgain = true;
  }
  if ( rect ( ).height ( ) < boundingRect.height ( ) )  {
    rect ( ).setHeight ( boundingRect.height ( ) );
    bUpdateAgain = true;
  }

  // Here we re-set the world matrix and translate the newly drawn ... thing ...
  theMatrix.reset ( );
  painter->setWorldMatrix ( theMatrix );

  if ( bUpdateAgain ) {
    m_pParent->update ( );
    updateShadow      ( );
  }
}

void TextObject::drawContents (QPainter *pPainter, int iRenderFrameNumber, int)
{
/*  // Next is to set the AnimationAttributes
  for (uint t=0;t<m_listAnimationAttributes.count();t++)
    m_listAnimationAttributes[t]->setValue (iRenderFrameNumber);

  // If start stop have been set then we check the range
  if ( ( ( iRenderFrameNumber < modifiers ( )->iStartFrame ) && ( modifiers ( )->iStartFrame != -1 ) ) ||
       ( ( iRenderFrameNumber > modifiers ( )->iStopFrame  ) && ( modifiers ( )->iStopFrame  != -1 ) ) ) 
    return;

  drawContents (pPainter);*/	//oooo
}

bool TextObject::readProjectFile  (QDomNode &theNode)
{
  QDomElement theElement = theNode.toElement();
  QDomAttr attribute;

  attribute = theElement.attributeNode ( TEXT_OBJECT_TEXT );
  if (!attribute.isNull())
    textData ( )->m_qsText = attribute.value();

  attribute = theElement.attributeNode ( TEXT_OBJECT_TEXT_ALIGN );
  if (!attribute.isNull())	{
    // QString "x,y" - we abused a QPoint object for the Align info.
    //QStringList list = QStringList::split (",", attribute.value());	//ooo
    QStringList list = attribute.value().split (",");			//xxx
    QPoint textAlign;
    if (list.count() == 2)
      textAlign = QPoint(list[0].toInt(), list[1].toInt());
    setTextAlign (textAlign);
  }

  attribute = theElement.attributeNode ( TEXT_OBJECT_STYLE_STRATEGY );
  if (!attribute.isNull())
    textData ( )->m_iStyleStrategy = attribute.value().toInt();
  
  attribute = theElement.attributeNode ( TEXT_OBJECT_FONT);
  if (!attribute.isNull())	{
    QFont theFont;
    if (theFont.fromString (attribute.value()))
      setFont (theFont);
  }
  
  attribute = theElement.attributeNode ( TEXT_OBJECT_BACKGROUND_COLOR );
  if (!attribute.isNull())	{
    Rgba backgroundColor;
    backgroundColor.fromString ( attribute.value ( ) );
    setBackgroundColor ( backgroundColor );
  }
  
  attribute = theElement.attributeNode ( TEXT_OBJECT_FOREGROUND_COLOR );
  if (!attribute.isNull())	{
    Rgba foregroundColor;
    foregroundColor.fromString ( attribute.value ( ) );
    setForegroundColor ( foregroundColor );
  }
  
  MenuObject::readProjectFile( theNode );
  return true;
}

bool TextObject::writeProjectFile (QDomElement &theElement)
{
	QDomDocument xmlDoc = theElement.ownerDocument();
	QDomElement textNode = xmlDoc.createElement( TEXT_OBJECT );	// <FrameObject>
	
	TextObject initialValues;
	// Here we set the attributes of the <dvdauthor> tag
	if (!getText().isNull())
		textNode.setAttribute( TEXT_OBJECT_TEXT, textData ( )->m_qsText );
	if (getTextAlign() != initialValues.getTextAlign())
		textNode.setAttribute( TEXT_OBJECT_TEXT_ALIGN, QString("%1,%2").arg(getTextAlign().x()).arg(getTextAlign().y()) );
	if (getStyleStrategy() != initialValues.getStyleStrategy())
	        textNode.setAttribute( TEXT_OBJECT_STYLE_STRATEGY, QString("%1").arg(getStyleStrategy()) );
	if (getFont() != initialValues.getFont())
		textNode.setAttribute( TEXT_OBJECT_FONT, getFont().toString() );
	if (getBackgroundColor() != initialValues.getBackgroundColor())
		textNode.setAttribute( TEXT_OBJECT_BACKGROUND_COLOR, getBackgroundColor().toString ( ) );
	if (getForegroundColor() != initialValues.getForegroundColor())
		textNode.setAttribute( TEXT_OBJECT_FOREGROUND_COLOR, getForegroundColor().toString ( ) );
	
	theElement.appendChild( textNode );
	// And here we write the base class ...
	return MenuObject::writeProjectFile( textNode );
}

bool TextObject::createContextMenu (QPoint globalPos)
{
	/*QPoint globalPos2 = globalPos;
	Q3PopupMenu *pStackMenu = new Q3PopupMenu(m_pParent);
	pStackMenu->insertItem ( tr ("Cut") , this, SLOT ( slotCut  ( ) ) );
	pStackMenu->insertItem ( tr ("Copy"), this, SLOT ( slotCopy ( ) ) );
	pStackMenu->insertSeparator ( );
	pStackMenu->insertItem ( tr ("To Front")  , this, SLOT(slotToFront()));
	pStackMenu->insertItem ( tr ("To Back")   , this, SLOT(slotToBack()));
	globalPos.setY ( globalPos.y ( ) - 25 );
	globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint().width()); // -100);
	pStackMenu->popup(globalPos, 1);

	if (m_pContextMenu)
		delete m_pContextMenu;
	m_pContextMenu = new Q3PopupMenu(m_pParent);
	m_pContextMenu->insertItem ( tr ("Edit ...")  , this,  SLOT ( slotEdit   ( ) ) );
	m_pContextMenu->insertItem ( tr ("Matrix ..."), this,  SLOT ( slotMatrix ( ) ) );
	m_pContextMenu->insertItem ( tr ("Delete")    , this,  SLOT ( slotDelete ( ) ) );
	m_pContextMenu->insertSeparator();
	if ( shadow ( ) )  {
	  m_pContextMenu->insertItem ( tr ( "Edit Shadow ..." ), this, SLOT ( slotAddShadow    ( ) ) );
	  m_pContextMenu->insertItem ( tr ( "Delete Shadow" ),   this, SLOT ( slotDeleteShadow ( ) ) );
	}
	else
	  m_pContextMenu->insertItem ( tr ( "Add Shadow ..." ),  this, SLOT ( slotAddShadow ( ) ) );

	m_pContextMenu->insertSeparator ( );
	m_pContextMenu->insertItem ( tr ( "Define as Button" ),  this, SLOT ( slotDefineAsButton ( ) ) );
	// Blocking call
	m_pContextMenu->exec(globalPos2, 3);
	// Here we mark that the user called a menu item thus we don't want the
	// base classes to continue with the mouse event
	delete pStackMenu;
	// Note the menu might have been already deleted in slotToFront/slotToBack
	if (m_pContextMenu)
		delete m_pContextMenu;
	m_pContextMenu = NULL;
	return true;*/		//oooo

  //QPoint globalPos2 = globalPos;
  //globalPos.setY ( globalPos.y ( ) - 25 );
  //globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint().width()); // -100);
  
  QPoint globalPos2 = globalPos;
  
  QMenu *pStackMenu = new QMenu(m_pParent);
  
  QAction *cutNewAction = new QAction("&Cut", this);
  QAction *copyNewAction = new QAction("&Copy", this);
  QAction *toFontNewAction = new QAction("&To Front", this);
  QAction *toBackNewAction = new QAction("&To Back", this);
  
  pStackMenu->addAction(cutNewAction);
  pStackMenu->addAction(copyNewAction);
  pStackMenu->addSeparator ( );
  pStackMenu->addAction(toFontNewAction);
  pStackMenu->addAction(toBackNewAction);
  
  globalPos.setY ( globalPos.y ( ) - 25 );
  globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint().width()); // -100);
  
  //pStackMenu->exec( globalPos );
  pStackMenu->popup(globalPos, cutNewAction);

  if (m_pContextMenu)
    delete m_pContextMenu;

  QAction *editNewAction = new QAction("&Edit ...", this);
  QAction *matrixNewAction = new QAction("&Matrix ...", this);
  QAction *deleteNewAction = new QAction("&Delete", this);
  QAction *editShadowNewAction = new QAction("&Edit Shadow ...", this);
  QAction *deleteShadowNewAction = new QAction("&Delete Shadow", this);
  QAction *addShadowNewAction = new QAction("&Add Shadow ...", this);
  QAction *defineAsButtonNewAction = new QAction("&Define as Button", this);
  
  //QMenu *pMenu = new QMenu (this);
  //pMenu->addAction(defineAsButtonNewAction);
  m_pContextMenu = new QMenu (m_pParent);
  m_pContextMenu->addAction(editNewAction);
  m_pContextMenu->addAction(matrixNewAction);
  m_pContextMenu->addAction(deleteNewAction);
  m_pContextMenu->addSeparator();
  if ( shadow ( ) )  {
    m_pContextMenu->addAction(editShadowNewAction);
    m_pContextMenu->addAction(deleteShadowNewAction);
  }
  else
    m_pContextMenu->addAction(addShadowNewAction);
  
  m_pContextMenu->addSeparator();
  m_pContextMenu->addAction(defineAsButtonNewAction);
  
  connect(editNewAction,           SIGNAL(triggered()), this, SLOT( slotEdit            ( ) ) );
  connect(matrixNewAction,         SIGNAL(triggered()), this, SLOT( slotMatrix          ( ) ) );
  connect(deleteNewAction,         SIGNAL(triggered()), this, SLOT( slotDelete          ( ) ) );
  connect(defineAsButtonNewAction, SIGNAL(triggered()), this, SLOT( slotDefineAsButton  ( ) ) );
  
  //pMenu->exec( globalPos );
  m_pContextMenu->exec( globalPos2, deleteNewAction );
  // Here we mark that the user called a menu item thus we don't want the
  // base classes to continue with the mouse event
  delete pStackMenu;
  //delete pMenu;
  // Note the menu might have been already deleted in slotToFront/slotToBack
  if (m_pContextMenu)
    delete m_pContextMenu;
  m_pContextMenu = NULL;
  return true;

}

StructureItem *TextObject::createStructure ( StructureItem *pParentItem )
{
  StructureItem *pTemp=NULL;

  if ( ! pParentItem )  { 
    if ( m_pOverlay  )
      pTemp = m_pOverlay->createStructure ( NULL );
    if ( m_pShadow )
      pTemp = m_pShadow->createStructure  ( NULL );
    //modifiers ( )->createStructure  ( NULL, NULL );	//oooo
    m_pStructureItem = NULL;
    return NULL;
  }

  QString qsName = tr ( "Text Object" );
  if ( ! m_pStructureItem ) { 
    m_pStructureItem = new StructureItem ( this, StructureItem::Text, pParentItem, qsName, textData ( )->m_qsText );
    //m_pStructureItem->setText ( 0, qsName );				//xxx
    //m_pStructureItem->setText ( 1, textData ( )->m_qsText );		//xxx
    //m_pStructureItem->setExpandable      ( TRUE );	//ooo
    m_pStructureItem->setExpanded      ( false );	//xxx
  }
  else {
    //m_pStructureItem->setText ( qsName, textData ( )->m_qsText );	//ooo
    m_pStructureItem->setText ( 0, qsName );				//xxx
    m_pStructureItem->setText ( 1, textData ( )->m_qsText );		//xxx
  }
  
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ( "Geometry" ), QString ( "%1, %2, %3, %4" ).
       	         arg ( rect().x()).arg(rect().y()).arg(rect().width()).arg(rect().height()) );

  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Foreground"),
                //tr ("color(%1, %2, %3)").	//ooo
		QString ("color(%1, %2, %3)").	//xxx
		arg ( textData ( )->m_colorForeground.red   ( ) ).
		arg ( textData ( )->m_colorForeground.green ( ) ).
		arg ( textData ( )->m_colorForeground.blue  ( ) ) );

  if ( textData ( )->m_colorBackground == Rgba ( TRANSPARENT_COLOR ) ) {

    pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Background"), tr ("Transparent") );
  } else {
 
    pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Background"),
			tr  ( "color(%1, %2, %3)").
			arg ( textData ( )->m_colorBackground.red   ( ) ).
			arg ( textData ( )->m_colorBackground.green ( ) ).
			arg ( textData ( )->m_colorBackground.blue  ( ) ) );
  }
  
  if ( m_pShadow )
    pTemp = m_pShadow->createStructure ( m_pStructureItem );

  modifiers ( )->createStructure ( m_pStructureItem, pTemp );
	//oooo

  return m_pStructureItem;
}

bool TextObject::mousePressEvent ( QMouseEvent *pEvent )
{
  m_currentMousePos  = pEvent->pos ( );
  if ( pEvent->button ( ) == Qt::RightButton )  {
    QPoint globalPos = pEvent->globalPos   ( );
    return createContextMenu ( globalPos     );
  }

  m_bDrawRect = true;
  return false;
}

bool TextObject::mouseReleaseEvent ( QMouseEvent * )
{
  UndoBuffer *pUndo = Global::pApp->getUndoBuffer ( );
  pUndo->removeUnchanged ( );		//oooo

  m_bDrawRect = false;
  emit ( signalUpdatePixmap    ( ) );
  emit ( signalUpdateStructure ( ) );
  return false;
}

bool TextObject::mouseDoubleClickEvent ( QMouseEvent * )
{
  QFont theFont;
  DialogTextFont fontDialog  ( m_pParent, this );

  if ( fontDialog.exec ( ) == QDialog::Rejected )
    return false;

  DVDMenuUndoObject *pUndo = new DVDMenuUndoObject ( this, MenuObject::StateUnknown );

  setFont ( fontDialog.getFont ( ) );
  QString theString = fontDialog.getText ( );
  setText ( theString );
  setAnimation ( fontDialog.getAnimation ( ) );
  if ( fontDialog.getBackgroundColor ( ) != getBackgroundColor ( ) )  {
    // This emit signal is only caught when in ButtonDialog and the Selected, or Highlighted mask
    // was changed (I.e. This Object is in one of those two frames).
    // Otherwise the signal is ignored.
    emit ( signalMaskColorChanged ( fontDialog.getBackgroundColor ( ), getBackgroundColor ( ) ) );
    // P.s. replaceColor (QColor, QColor) of all ButtonObjects will be called, inclusive this object)
  }
  setBackgroundColor ( fontDialog.getBackgroundColor ( ) );
  if ( fontDialog.getForegroundColor ( ) != getForegroundColor ( ) )  {
    // This emit signal is only caught when in ButtonDialog and the Selected, or Highlighted mask 
    // was changed (I.e. This Object is in one of those two frames).
    // Otherwise the signal is ignored.
    emit ( signalMaskColorChanged ( fontDialog.getForegroundColor ( ), getForegroundColor ( ) ) );
    // P.s. replaceColor (QColor, QColor) of all ButtonObjects will be called, inclusive this object)
  }
  setForegroundColor ( fontDialog.getForegroundColor ( ) );
  modifiers ( )->fTransparency = fontDialog.getTransparency ( );
  setTextAlign ( fontDialog.getTextAlign ( ) );
  if ( fontDialog.getFit ( ) )  {
    rect ( ).setWidth  ( 1 );
    rect ( ).setHeight ( 1 );
    setWordBreak   ( false );
  }
  else
    setRect ( fontDialog.getRect ( ) );

  if ( m_pParent )
       m_pParent->update ( );

  QRect orig = rect ( );
  updateShadow      ( );
  emit ( signalUpdatePixmap    ( ) );

  if ( rect ( ) != orig ) // if the rect has changed ...
    emit ( signalUpdatePixmap  ( ) );

  emit ( signalUpdateStructure ( ) );

  setWordBreak ( true );

  if ( pUndo->hasChanged ( ) )  {
    UndoBuffer *pBuffer = Global::pApp->getUndoBuffer ( );
    if ( pBuffer )
         pBuffer->push ( pUndo );
  }
  else
    delete pUndo;

  return false;
}

void TextObject::slotEdit ( )
{
  mouseDoubleClickEvent ( NULL );
}

void TextObject::slotMatrix ( )
{
  if ( m_pMatrixDialog )
    return;

  m_pMatrixDialog = new DialogMatrix ( MenuObject::parent ( ) );
  m_pMatrixDialog->initMe ( this );
  m_pMatrixDialog->show   ( );
}

void TextObject::slotDefineAsButton ()
{
  // We want to convert this TextObject into a ButtonObject.
  // Thus the class TextObject is no longer the logical place for doing this, now is it ?
  // Okay we call the MenuPreview and let this handle the task.
  emit (signalDefineAsButton(this));
}

MenuObject *TextObject::clone ( QWidget *pParent, MenuObject * )
{
  if ( ! pParent )
    pParent = MenuObject::parent ( );

  TextObject *pNewObject = new TextObject ( pParent );
  pNewObject->setFont ( getFont ( ) );
  pNewObject->setText ( getText ( ) );
  pNewObject->setTextAlign ( getTextAlign ( ) );
  pNewObject->setForegroundColor ( getForegroundColor ( ) );
  pNewObject->setBackgroundColor ( getBackgroundColor ( ) );
  pNewObject->setStyleStrategy   ( (QFont::StyleStrategy)textData ( )->m_iStyleStrategy );

  // The following two are not really needed, since we get those 
  // informations solely from the Normal State - objects ...
  pNewObject->setRect ( rect ( ) );
  pNewObject->setModifiers ( *modifiers ( ) );

  MenuObject *pShadow = shadow  ( );
  if ( pShadow )
       pShadow = pShadow->clone ( pParent, pNewObject );
  pNewObject->setShadow ( pShadow );

  return pNewObject;
}

AnimationAttribute *TextObject::getSpecificAttributes (long iMaxNumberOfFrames, QString qsProperty)
{
  AnimationAttribute *pAnimAttr = NULL;
  static int m_iTestInt = 0;

  if (qsProperty == "text")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_qsText, &textData ( )->m_qsText);
  else if (qsProperty == "font")	{
    FunctionCallback<QFont> *callMeBack = new FunctionCallback<QFont>(textData ( )->m_Font, &QFont::fromString);
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_Font.toString(), callMeBack);
  }
  else if (qsProperty == "textAlign")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_iTestInt, &m_iTestInt);
  else if (qsProperty == "foregroundColor.red")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorForeground, TYPE_COLOR_RED );
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.Qt::red(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.red(), callMeBack);		//xxx
  }
  else if (qsProperty == "foregroundColor.green")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorForeground, TYPE_COLOR_GREEN);
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.Qt::green(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.green(), callMeBack);		//xxx
  }
  else if (qsProperty == "foregroundColor.blue")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorForeground, TYPE_COLOR_BLUE);
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.Qt::blue(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.blue(), callMeBack);		//xxx
  }
  else if (qsProperty == "foregroundColor.alpha")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorForeground, TYPE_COLOR_ALPHA);
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorForeground.alpha(), callMeBack);
  }
  else if (qsProperty == "backgroundColor.red")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorBackground, TYPE_COLOR_RED);
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.Qt::red(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.red(), callMeBack);		//xxx
  }
  else if (qsProperty == "backgroundColor.green")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorBackground, TYPE_COLOR_GREEN);
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.Qt::green(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.green(), callMeBack);		//xxx
  }
  else if (qsProperty == "backgroundColor.blue")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorBackground, TYPE_COLOR_BLUE);
    //pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.Qt::blue(), callMeBack);	//ooo
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.blue(), callMeBack);		//xxx
  }
  else if (qsProperty == "backgroundColor.alpha")	{
    FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(textData ( )->m_colorBackground, TYPE_COLOR_ALPHA);
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, textData ( )->m_colorBackground.alpha(), callMeBack);
  }
  return pAnimAttr;
}
