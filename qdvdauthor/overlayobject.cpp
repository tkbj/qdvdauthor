/***************************************************************************
    overlayobject.cpp
                             -------------------
    ShadowObject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class
    
****************************************************************************/

#include <qimage.h>
#include <qpainter.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "xml_dvd.h"
#include "structuretoolbar.h"
#include "buttonobject.h"
#include "selectionobject.h"
#include "overlayobject.h"


OverlayObject::OverlayObject ( MenuObject *pMenuObject, QWidget *pParent ) 
  : ImageObject  ( pParent )
{
  m_pMenuObject = NULL;
  m_bFastDraw   = false;
  m_fScaleX     = 1.0f;
  m_fScaleY     = 1.0f;
  m_fShearX     = 0.0f;
  m_fShearY     = 0.0f;
  m_fRotate     = 0.0f;
  m_bIsMask     = true;
  m_color       = Rgba ( 0xFF000000 );

  // If we pass a buttonObject we realy are interested in the normal(0) - object
  ButtonObject tempButton;
  if ( pMenuObject && pMenuObject->objectType ( ) == tempButton.objectType ( ) ) {
    ButtonObject   *pButton = (ButtonObject *)pMenuObject;
    m_pMenuObject = pButton->getNormal ( 0 );
  }

  if ( ! m_pMenuObject )
         m_pMenuObject = pMenuObject;
}

OverlayObject::~OverlayObject ( )
{
  m_pMenuObject    = NULL;
  resetStructureItem ( );
//  m_pStructureItem = NULL;
}

OverlayObject &OverlayObject::operator = ( OverlayObject &theOther )
{
  setColor     ( theOther.color     ( ) );
  setOffset    ( theOther.offset    ( ) );
  setRotate    ( theOther.rotate    ( ) );
  setIsMask    ( theOther.isMask    ( ) );
  setScale     ( theOther.scaleX    ( ), theOther.scaleY ( ) );
  setShear     ( theOther.shearX    ( ), theOther.shearY ( ) );
  setAnimation ( theOther.animation ( ) );
  setOverlay   ( theOther.fileName  ( ) );
  return *this;
}

bool OverlayObject::operator == ( OverlayObject &theOther )
{
  bool bReturn = true;
  bReturn = ( ( color     ( ) == theOther.color     ( ) ) &&
              ( offset    ( ) == theOther.offset    ( ) ) &&
              ( scaleX    ( ) == theOther.scaleX    ( ) ) &&
              ( scaleY    ( ) == theOther.scaleX    ( ) ) &&
              ( shearX    ( ) == theOther.shearX    ( ) ) &&
              ( shearY    ( ) == theOther.shearY    ( ) ) &&
              ( rotate    ( ) == theOther.rotate    ( ) ) &&
              ( isMask    ( ) == theOther.isMask    ( ) ) &&
              ( fileName  ( ) == theOther.fileName  ( ) ) &&
              ( animation ( ) == theOther.animation ( ) ) );

  return bReturn;
}

MenuObject *OverlayObject::clone ( QWidget *pParentWidget, MenuObject *pMenuObject )
{
  if ( ! pParentWidget )
         pParentWidget = MenuObject::parent ( );
  OverlayObject *pNewObject = new OverlayObject ( pMenuObject, pParentWidget );
  *pNewObject = *this;

  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect      (  rect      ( ) );
  pNewObject->setModifiers ( *modifiers ( ) );

  return pNewObject;
}

bool OverlayObject::writeProjectFile ( QDomElement &theElement )
{
  // If this is a Mask then we do no need to store anything ...
  QDomDocument xmlDoc      = theElement.ownerDocument ( );
  QDomElement  overlayNode = xmlDoc.createElement ( OVERLAY_OBJECT );	// <ShadowObject>

  overlayNode.setAttribute ( OVERLAY_OBJECT_OFFSET, QString ( "%1,%2" ).arg ( offset ( ).x ( ) ).arg ( offset ( ).y ( ) ) );
  overlayNode.setAttribute ( OVERLAY_OBJECT_COLOR, m_color.toString ( ) );
  overlayNode.setAttribute ( OVERLAY_OBJECT_FILE_NAME,  m_qsFileName );
  overlayNode.setAttribute ( OVERLAY_OBJECT_IS_MASK,    m_bIsMask ? "true" : "false" );

  if ( m_fShearX != 0.0f )
    overlayNode.setAttribute ( OVERLAY_OBJECT_SHEAR_X, m_fShearX );

  if ( m_fShearY != 0.0f )
    overlayNode.setAttribute ( OVERLAY_OBJECT_SHEAR_Y, m_fShearY );

  if ( m_fScaleX != 1.0f )
    overlayNode.setAttribute ( OVERLAY_OBJECT_SCALE_X, m_fScaleX );

  if ( m_fScaleY != 1.0f )
    overlayNode.setAttribute ( OVERLAY_OBJECT_SCALE_Y, m_fScaleY );

  if ( m_fRotate != 0.0f )
    overlayNode.setAttribute ( OVERLAY_OBJECT_ROTATE,  m_fRotate );

  if ( ! animation ( ).isEmpty()) {
    QDomElement animationNode = xmlDoc.createElement( ANIMATION_OBJECT );   // <AnimationObject>
    QDomText text = xmlDoc.createTextNode ( animation ( ) );
    animationNode.appendChild ( text );
    overlayNode.appendChild   ( animationNode );
  }

  theElement.appendChild  ( overlayNode );
  return true;
}

bool OverlayObject::readProjectFile ( QDomNode &theNode )
{
  QDomElement theElement = theNode.toElement ( );
  QDomAttr attribute;
  m_pData->m_qsAnimation = QString ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_OFFSET );
  if ( ! attribute.isNull ( ) )  {
    QPoint theOffset;
    //QStringList list = QStringList::split ( ",", attribute.value ( ) );	//ooo
    QStringList list = attribute.value ( ).split ( "," );			//xxx
    if ( list.count ( ) == 2 )
         theOffset = QPoint ( list[0].toInt ( ), list[1].toInt ( ) );
    setOffset ( theOffset );
  }

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_COLOR );
  if ( ! attribute.isNull ( ) )
    m_color.fromString ( attribute.value ( ) );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_SHEAR_X );
  if ( ! attribute.isNull ( ) )
    m_fShearX = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_SHEAR_Y );
  if ( ! attribute.isNull ( ) )
    m_fShearY = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_SCALE_X );
  m_fScaleX = 1.0f;
  if ( ! attribute.isNull ( ) )
    m_fScaleX = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_SCALE_Y );
  m_fScaleY = 1.0f;
  if ( ! attribute.isNull ( ) )
    m_fScaleY = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_ROTATE );
  m_fRotate = 0.0f;
  if ( ! attribute.isNull ( ) )
    m_fRotate = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( OVERLAY_OBJECT_FILE_NAME );
  if ( ! attribute.isNull ( ) )
    setOverlay ( attribute.value ( ) );

  m_bIsMask = true;
  attribute = theElement.attributeNode ( OVERLAY_OBJECT_IS_MASK );
  if ( ! attribute.isNull ( ) )
    m_bIsMask = ( attribute.value ( ) == "true" ) ? true : false;

  QString tagName;
  QDomNode subNode = theElement.firstChild ( );
  while ( ! subNode.isNull ( ) )   {
    theElement = subNode.toElement ( );
    tagName = theElement.tagName   ( );
    if ( tagName == ANIMATION_OBJECT )      {
      m_pData->m_qsAnimation = theElement.text ( );
    }
    // So lets get the next sibling ... until we hit hte end of DVDMenu ...
    subNode = subNode.nextSibling ( );
  }

  return true;
}

void OverlayObject::setColor ( Rgba &color )
{
  m_color = color;
}

Rgba &OverlayObject::color ( )
{
  return m_color;
}

void OverlayObject::setScale ( float fScaleX, float fScaleY )
{
  m_fScaleX = fScaleX;
  m_fScaleY = fScaleY;
}

float OverlayObject::scaleX ( )
{
  return m_fScaleX;
}

float OverlayObject::scaleY ( )
{
  return m_fScaleY;
}

void OverlayObject::setShear ( float fShearX, float fShearY )
{
  m_fShearX = fShearX;
  m_fShearY = fShearY;
}

float OverlayObject::shearX ( )
{
  return m_fShearX;
}

float OverlayObject::shearY ( )
{
  return m_fShearY;
}

void OverlayObject::setOffset ( int iX, int iY )
{
  m_offset = QPoint ( iX, iY );
}

void OverlayObject::setOffset ( QPoint &pos )
{
  m_offset = pos;
}

QPoint &OverlayObject::offset ( )
{
  return m_offset;
}

void OverlayObject::setRotate ( float fRotate )
{
  m_fRotate = fRotate;
}

float OverlayObject::rotate ( )
{
  return m_fRotate;
}

void OverlayObject::setIsMask ( bool bIsMask )
{
  m_bIsMask = bIsMask;
}

bool OverlayObject::isMask ( )
{
  return m_bIsMask;
}

bool OverlayObject::setOverlay ( QString qsFileName )
{
  m_qsFileName   = qsFileName;
  m_imageScaled  = QImage ( );
  m_imageOverlay = QImage ( qsFileName );
  return ( ! m_imageOverlay.isNull ( ) );
}

void OverlayObject::setFastDraw ( bool bFastDraw )
{
  m_bFastDraw   = bFastDraw;
  m_imageScaled = QImage ( );
}

bool OverlayObject::fastDraw ( )
{
  return m_bFastDraw;
}

QImage &OverlayObject::image ( QImage &image )
{
  m_image = image.copy   ( );
  //m_image.setAlphaBuffer ( true );	//oooo

  if ( m_imageOverlay.isNull ( ) )
    return m_image;

  int iWidth        = image.width   ( );
  int iHeight       = image.height  ( );
  int iScaledWidth  = (int)( iWidth  * scaleX ( ) );
  int iScaledHeight = (int)( iHeight * scaleY ( ) );
  int iOffsetX = offset ( ) .x ( );
  int iOffsetY = offset ( ) .y ( );
  int iMaxX    = iScaledWidth  - 1;
  int iMaxY    = iScaledHeight - 1;
  int iMaskX, iMaskY;

  if ( ( m_imageScaled.width ( ) != iScaledWidth ) || ( m_imageScaled.height ( ) != iScaledHeight ) )  {
    if ( fastDraw ( ) )
         //m_imageScaled = m_imageOverlay.scale ( iScaledWidth, iScaledHeight );	//ooo
         m_imageScaled = m_imageOverlay.scaled ( iScaledWidth, iScaledHeight );		//xxx
    else
         //m_imageScaled = m_imageOverlay.smoothScale ( iScaledWidth, iScaledHeight );							//ooo
         m_imageScaled = m_imageOverlay.scaled ( iScaledWidth, iScaledHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  }
  if ( ( rotate ( ) != 0.0f ) || ( shearX ( ) != 1.0f) || ( shearY ( ) != 1.0f ) ) {
    QMatrix theMatrix;
    theMatrix.rotate ( rotate ( ) );
    theMatrix.shear  ( shearX ( ), shearY ( ) );
    //m_imageScaled = m_imageScaled.xForm  ( theMatrix );	//ooo
    m_imageScaled = m_imageScaled.transformed  ( theMatrix );	//xxx
    iScaledWidth  = m_imageScaled.width  ( );
    iScaledHeight = m_imageScaled.height ( );
    iMaxX         = iScaledWidth  - 1;
    iMaxY         = iScaledHeight - 1;
  }
/*
  if ( fastDraw ( ) )  {
    if ( m_image.size ( ) != image.size ( ) )
         m_image = QImage (  image.size ( ), 32 );
    QRgb  *pBits = (QRgb *)m_image.bits ( );
    int    iSize = m_image.numBytes ( ) / 4.0;
    for ( int t=0; t<iSize; t++ )
      pBits[t] = 0x00FFFFFF;
    bitBlt ( &m_image, offset ( ).x ( ), offset ( ).y ( ), &m_imageScaled );
    return m_image;
  }
*/

  // We handle the overlay ourselfes ...
  int x, y;
  QRgb dest, src, *pDestLine, *pSrcLine;
  for ( y=0; y<iHeight; y++ )  {
    iMaskY = y - iOffsetY;
    pDestLine = (QRgb *)m_image.scanLine ( y );
    if ( ( iMaskY  < 0 ) || ( iMaskY > iMaxY ) ) {
      // All rows here are transparent
      for ( x=0; x<iWidth; x++ )
        pDestLine[x] = 0x00FFFFFF;
    }
    else  {
      pSrcLine = (QRgb *)m_imageScaled.scanLine ( iMaskY );
      for (   x=0; x<iWidth; x++ )  {
        dest   = pDestLine[x];
        iMaskX = x - iOffsetX;
        if ( ( iMaskX < 0 ) || ( iMaskX > iMaxX ) )  {
          pDestLine[x] = 0x00FFFFFF;
        }
        else  {
          src   = pSrcLine [iMaskX];
          if ( m_bIsMask )  {
            // First we'll copy the transparency over
            if ( ( ( src & 0x00808080 ) == 0x00808080 ) || ( src == 0x00000000 ) )
              pDestLine[x] = ( dest & 0x00FFFFFF ) | ( src & 0xFF000000 );
            else
              pDestLine[x] = color ( ).rgb ( ); // Set the the frame's color
          }
          else {
            if ( ( ( src & 0x00FFFFFF ) == 0x00808080 ) || ( src == 0x00000000 ) )
              pDestLine[x] = ( dest & 0x00FFFFFF ) | ( src & 0xFF000000 );
            else
              pDestLine[x] = src; // Set the the frame's color
          }
        }
      }
    }
  }

  return m_image;
}

void OverlayObject::drawContents ( QPainter * )
{
}

void OverlayObject::drawContents ( QPainter *, int iRenderFrameNumber, int )
{
  if ( ! m_pMenuObject )
    return;

  // Next is to set the AnimationAttributes
  for ( uint t=0; t<(uint)m_listAnimationAttributes.count ( ) ;t++ )
    m_listAnimationAttributes[t]->setValue ( iRenderFrameNumber );

  // And then we ought to refresh the pixmap ...
  if ( m_listAnimationAttributes.count ( ) > 0 )
       m_imageScaled = QImage ( );
}

bool OverlayObject::mousePressEvent ( QMouseEvent   * )
{
  return false;
}

bool OverlayObject::mouseReleaseEvent ( QMouseEvent * )
{
  return false;
}

bool OverlayObject::mouseDoubleClickEvent ( QMouseEvent * )
{
  return false;
}

StructureItem *OverlayObject::createStructure ( StructureItem *pParentItem )
{
  if ( ! pParentItem )  {
    m_pStructureItem = NULL;
    return NULL;
  }

  StructureItem *pOverlayItem, *pTemp=NULL;
  QString qsItem = tr ( "Mask Object" );
  QString qsInfo ( tr ( "Something") );
  QFileInfo fileInfo  ( fileName ( ) );
  qsInfo = fileInfo.baseName ( );

  pOverlayItem = m_pStructureItem;
  if ( ! pOverlayItem ) {
    pOverlayItem = new StructureItem ( this, StructureItem::Modifiers, pParentItem, pParentItem->lastChild ( ), qsItem, qsInfo );
    //pOverlayItem->setExpandable ( TRUE );	//ooo
    pOverlayItem->setExpanded ( true );		//xxx
    m_pStructureItem = pOverlayItem;
  }
  else {
    //pOverlayItem->setText ( qsItem, qsInfo );		//ooo
    pOverlayItem->setText ( 0, qsItem );		//xxx
    pOverlayItem->setText ( 1, qsInfo );		//xxx
  }

  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Offset"), QString ( "%1, %2" ).arg ( offset ( ).x ( ) ).arg ( offset ( ).y ( ) ) );
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Rotate"), QString ( "%1" ).arg ( rotate ( ) ) );
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Scale"),  QString ( "%1, %2" ).arg ( scaleX ( ) ).arg ( scaleY ( ) ) );
  pTemp = m_pStructureItem->setSibling ( this, pTemp, tr ("Shear"),  QString ( "%1, %2" ).arg ( shearX ( ) ).arg ( shearY ( ) ) );

  qsInfo = tr ( "RGBA (%1, %2, %3, %4)" ).
    arg ( color ( ).red   ( ) ).
    arg ( color ( ).green ( ) ).
    arg ( color ( ).blue  ( ) ).
    arg ( color ( ).alpha ( ) );

  pTemp  = m_pStructureItem->setSibling ( this, pTemp, tr ( "Color" ), qsInfo );

  return pOverlayItem;
}

bool OverlayObject::createContextMenu ( QPoint   )
{
  return false;
}

AnimationAttribute *OverlayObject::getSpecificAttributes ( long iMaxNumberOfFrames, QString qsProperty)
{
  FunctionCallback<QPoint> *callMeBack1 = NULL;
  FunctionCallback<Rgba>   *callMeBack2 = NULL;
  AnimationAttribute       *pAnimAttr  = NULL;

//FunctionCallback<QRect> *callMeBack = new FunctionCallback<QRect>(m_rect, &QRect::setWidth);
//pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_rect.width(), callMeBack);
//pAnimAttr->setNeedRefresh ( false );

  if ( qsProperty == "offset.x" )  {
    callMeBack1 = new FunctionCallback<QPoint> ( m_offset, &QPoint::setX ); //TYPE_POSITION_X );
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_offset.x(), callMeBack1 );
  }
  else if ( qsProperty == "offset.y" )  {
    callMeBack1 = new FunctionCallback<QPoint> ( m_offset, &QPoint::setY ); //TYPE_POSITION_Y );
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_offset.y(), callMeBack1 );
  }
  else if ( qsProperty == "color.red" )  {
    callMeBack2 = new FunctionCallback<Rgba> ( m_color, TYPE_COLOR_RED );
    //pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.Qt::red(), callMeBack2 );	//ooo
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.red(), callMeBack2 );		//xxx
  }
  else if ( qsProperty == "color.green" )   {
    callMeBack2 = new FunctionCallback<Rgba> ( m_color, TYPE_COLOR_GREEN );
    //pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.Qt::green(), callMeBack2 );	//ooo
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.green(), callMeBack2 );	//xxx
  }
  else if ( qsProperty ==  "color.blue" )   {
    callMeBack2 = new FunctionCallback<Rgba> ( m_color, TYPE_COLOR_BLUE );
    //pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.Qt::blue(), callMeBack2 );	//ooo
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.blue(), callMeBack2 );	//xxx
  }
  else if ( qsProperty == "color.alpha" )   {
    callMeBack2 = new FunctionCallback<Rgba> ( m_color, TYPE_COLOR_ALPHA );
    pAnimAttr  = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_color.alpha(), callMeBack2 );
  }
  else if ( qsProperty == "rotate" )
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_fRotate, &m_fRotate );
  else if (qsProperty == "shear.x")
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_fShearX, &m_fShearX );
  else if (qsProperty == "shear.y")
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_fShearY, &m_fShearY );
  else if (qsProperty == "scale.x")
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_fScaleX, &m_fScaleX );
  else if (qsProperty == "scale.y")
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_fScaleY, &m_fScaleY );
  else if ( qsProperty == "fileName" )
    pAnimAttr = new AnimationAttribute ( iMaxNumberOfFrames, qsProperty, m_qsFileName, &m_qsFileName );

  return pAnimAttr;
}

/*
#include <qdir.h>
#include <qfileinfo.h>
#include "global.h"
void OverlayObject::createMaskFrames ( )
{
  // Here we take the input of the two current images for the Masks
  int t;
  QDir        theDir;
  QString     qsFileName, qsPath, qsDest;
  QFileInfo   fileInfo;
  QStringList listFiles;
  QStringList::iterator it;
  QImage      src1, dst;

  const char array [9][20] = { "black_gold.png", "black.png", "black_wood.png", "gold.png",
    "ornated.png", "simple.png", "white_gold.png", "white_wood.png", "wood.png" };

  qsPath = "/tmp/pf/";
  qsDest = qsPath + "dest/";
  for ( t=0; t<9; t++ )  {
    qsFileName = qsPath + array[t];
printf ( "fileName<%s>\n", qsFileName.ascii ( ) );

    src1 = QImage ( qsFileName );
    src1 = src1.convertDepth ( 32 );
    src1.setAlphaBuffer ( true );

    dst = QImage ( src1.size ( ), 32 );
    dst.setAlphaBuffer ( true );
    dst.fill ( 0xFF808080 );

    int iWidth  = src1.width  ( );
    int iHeight = src1.height ( );

    // We copy the transparency value over
    int x, y;
    QRgb src, *pDestLine, *pSrcLine;
    for ( y=0; y<iHeight; y++ )  {
      pSrcLine  = (QRgb *)src1.scanLine ( y );
      pDestLine = (QRgb *)dst.scanLine  ( y );
      for ( x=0; x<iWidth; x++ )  {
        src  = pSrcLine [x];
        // ensure we do not have the magic color in the image at all
        if ( ( src  & 0x00FFFFFF ) == 0x00808080 )
               src  = src + 1; //0xFFFF0000; //src | 0x000F0FFF; // 0xFFFF0000;
        if ( ( src  & 0x00FFFFFF ) == 0x00000000 )
               src  = src + 1; //0xFF00FF00;
        pDestLine[x]= src;
      }
    } // end for y loop
    qsFileName = qsDest + array[t];
    dst.save ( qsFileName, "PNG", 100 );
  }
}

void OverlayObject::createMaskImages ( )
{
  // Here we take the input of the two current images for the Masks
  QDir        theDir;
  QString     qsPath[3], qsFileName, qsFName;
  QFileInfo   fileInfo;
  QStringList listFiles;
  QStringList::iterator it;
  QImage   src1, src2, dst;

  qsPath[0] = QString ("/tmp/masks/src1");   // mask
  qsPath[1] = QString ("/tmp/masks/src2"); // black outlines
  qsPath[2] = QString ("/tmp/masks/dst");
  if ( theDir.exists ( qsPath[0] ) )  {
    theDir.cd ( qsPath[0] );
    listFiles = theDir.entryList ( QDir::Files, QDir::Name );
    it = listFiles.begin ( );
    while ( it != listFiles.end ( ) )  {
      qsFileName = qsPath[0] + "/" + *it++;
      fileInfo.setFile ( qsFileName );
      if ( ! fileInfo.exists ( ) )
        continue;
      src1 = QImage ( qsFileName );
      src1 = src1.convertDepth ( 32 );
      src1.setAlphaBuffer ( true );
      if ( src1.isNull ( ) )
        continue;
      qsFName = fileInfo.fileName ( );
      qsFileName = qsPath[1] + "/2t_" + fileInfo.fileName ( );
      fileInfo.setFile ( qsFileName );
      if ( ! fileInfo.exists ( ) )
        continue;
      src2 = QImage ( qsFileName );
      src2 = src2.convertDepth ( 32 );
      src2.setAlphaBuffer ( true );
      if ( src2.isNull ( ) )
        continue;
      dst = QImage ( src1.size ( ), 32 );
      dst.setAlphaBuffer ( true );
      dst.fill ( 0xFF808080 );

      if ( src2.size ( ) != src1.size ( ) )
        src2.smoothScale (  src1.size ( ) );

      int iWidth  = src1.width  ( );
      int iHeight = src1.height ( );

      // We copy the transparency value over
      int x, y;
      QRgb dest, src, *pDestLine, *pSrcLine;
      for ( y=0; y<iHeight; y++ )  {
        pSrcLine  = (QRgb *)src1.scanLine ( y );
        pDestLine = (QRgb *)dst.scanLine  ( y );
        for ( x=0; x<iWidth; x++ )  {
          src  = pSrcLine [x];
          dest = pDestLine[x];
          pDestLine[x] = ( dest & 0x00FFFFFF ) | ( src & 0xFF000000 );
          // ensure the outline is completely black
          if ( ( pDestLine[x] & 0x00FFFFFF ) != 0x00808080 )
            pDestLine[x] = pDestLine[x] & 0xFF000000;
        }
      } // end for y loop
      // then we can simply copy over the second source file
      bitBlt ( &dst, 0, 0, &src2,  0, 0, iWidth, iHeight, Qt::CopyROP );
      dst.save ( qsPath[2] + "/" + qsFName, "PNG", 100 );
    }
  }
}
*/

