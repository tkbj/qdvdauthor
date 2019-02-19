/***************************************************************************
    shadowobject.cpp
                             -------------------
    ShadowObject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class
    
****************************************************************************/

#include <math.h>
#include <qpainter.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QPixmap>
#include <QMouseEvent>

#include "global.h"
#include "xml_dvd.h"
#include "menuobject.h"
#include "textobject.h"
#include "imageobject.h"
#include "movieobject.h"
#include "frameobject.h"
#include "kimageeffect.h"
#include "structuretoolbar.h"

#include "buttonobject.h"
#include "selectionobject.h"
#include "shadowobject.h"

#define qMax(a,b) (a>b)?a:b
#define qMin(a,b) (a<b)?a:b

// Exponential blur, Jani Huhtanen, 2006
//
template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha);

template<int aprec,int zprec>
static inline void blurrow( QImage & im, int line, int alpha);

template<int aprec, int zprec>
static inline void blurcol( QImage & im, int col, int alpha);

/*
*  expblur(QImage &img, int radius)
*
*  In-place blur of image 'img' with kernel
*  of approximate radius 'radius'.
*
*  Blurs with two sided exponential impulse
*  response.
*
*  aprec = precision of alpha parameter 
*  in fixed-point format 0.aprec
*
*  zprec = precision of state parameters
*  zR,zG,zB and zA in fp format 8.zprec
*/
template<int aprec,int zprec>
void expblur( QImage &img, int radius )
{
  if(radius<1)
    return;

  /* Calculate the alpha such that 90% of 
     the kernel is within the radius.
     (Kernel extends to infinity) 
  */
  int alpha = (int)((1<<aprec)*(1.0f-expf(-2.3f/(radius+1.f))));

  for(int row=0;row<img.height();row++)
  {
    blurrow<aprec,zprec>(img,row,alpha);
  }

  for(int col=0;col<img.width();col++)
  {
    blurcol<aprec,zprec>(img,col,alpha);
  }
  return;
}

template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha)
{
  int R,G,B,A;
  R = *bptr;
  G = *(bptr+1);
  B = *(bptr+2);
  A = *(bptr+3);

  zR += (alpha * ((R<<zprec)-zR))>>aprec;
  zG += (alpha * ((G<<zprec)-zG))>>aprec;
  zB += (alpha * ((B<<zprec)-zB))>>aprec;
  zA += (alpha * ((A<<zprec)-zA))>>aprec;

  *bptr =     zR>>zprec;
  *(bptr+1) = zG>>zprec;
  *(bptr+2) = zB>>zprec;
  *(bptr+3) = zA>>zprec;
}

template<int aprec,int zprec>
static inline void blurrow( QImage & im, int line, int alpha)
{
  int zR,zG,zB,zA;

  QRgb *ptr = (QRgb *)im.scanLine(line);

  zR = *((unsigned char *)ptr    )<<zprec;
  zG = *((unsigned char *)ptr + 1)<<zprec;
  zB = *((unsigned char *)ptr + 2)<<zprec;
  zA = *((unsigned char *)ptr + 3)<<zprec;

  for(int index=1; index<im.width(); index++)
  {
    blurinner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);
  }
  for(int index=im.width()-2; index>=0; index--)
  {
    blurinner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);
  }
}

template<int aprec, int zprec>
static inline void blurcol( QImage & im, int col, int alpha)
{
  int zR,zG,zB,zA;

  QRgb *ptr = (QRgb *)im.bits();
  ptr+=col;

  zR = *((unsigned char *)ptr    )<<zprec;
  zG = *((unsigned char *)ptr + 1)<<zprec;
  zB = *((unsigned char *)ptr + 2)<<zprec;
  zA = *((unsigned char *)ptr + 3)<<zprec;

  for(int index=im.width(); index<(im.height()-1)*im.width(); index+=im.width())
  {
    blurinner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);
  }

  for(int index=(im.height()-2)*im.width(); index>=0; index-=im.width())
  {
    blurinner<aprec,zprec>((unsigned char *)&ptr[index],zR,zG,zB,zA,alpha);
  }

}

ShadowObject::ShadowObject ( MenuObject *pMenuObject, QWidget *pParent ) 
  : ImageObject  ( pParent ), 
    m_fSunElevation (45.0f)
{
  ButtonObject tempButton;
  m_pMenuObject   = NULL;

  // If we pass a buttonObject we realy are iinterested in the normal(0) - object
  if ( pMenuObject && pMenuObject->objectType ( ) == tempButton.objectType ( ) ) {
    ButtonObject   *pButton = (ButtonObject *)pMenuObject;
    m_pMenuObject = pButton->getNormal ( 0 );
  }

  if ( ! m_pMenuObject )
         m_pMenuObject = pMenuObject;

  m_iDeltaX       =    -1;
  m_iDeltaY       =    -1;
  m_fSunAngle     =  0.0f;
  m_fDistance     =  0.0f;
  m_fTransparency =  0.0f;
  m_iBlurRadius   =     3;
  m_fShearX       =  0.0f;
  m_fShearY       =  0.0f;
  m_fScaleX       =  1.0f;
  m_fScaleY       =  1.0f;
  m_fCutOff       =  0.5f;
  m_fCutOffRadius =  0.3f;
  m_fColorFading  =  0.0f;
  m_enType        = TYPE_SHADOW; //TYPE_MIRROR; // TYPE_SHADOW;
}

ShadowObject::~ShadowObject ( ) 
{
  m_pMenuObject    = NULL;
  resetStructureItem ( );
//  m_pStructureItem = NULL;
}

ShadowObject &ShadowObject::operator = ( ShadowObject &theOther )
{
  setColor        ( theOther.color        ( ) );
  setTransparency ( theOther.transparency ( ) );
  setDistance     ( theOther.distance     ( ) );
  setBlur         ( theOther.blur         ( ) );
  setSunAngle     ( theOther.sunAngle     ( ) );
  setType         ( theOther.type         ( ) );

  setShear        ( theOther.shearX      ( ), theOther.shearY       ( ) );
  setScale        ( theOther.scaleX      ( ), theOther.scaleY       ( ) );
  setCutOff       ( theOther.cutOff      ( ), theOther.cutOffRadius ( ) );
  setColorFading  ( theOther.colorFadeTo ( ), theOther.colorFading  ( ) );

  return *this;
}

bool ShadowObject::operator == ( ShadowObject &theOther )
{
  bool bReturn = true;

  bReturn = ( color ( ) == theOther.color        ( ) ) &&
    ( transparency  ( ) == theOther.transparency ( ) ) &&
    ( distance      ( ) == theOther.distance     ( ) ) &&
    ( blur          ( ) == theOther.blur         ( ) ) &&
    ( sunAngle      ( ) == theOther.sunAngle     ( ) ) &&
    ( type          ( ) == theOther.type         ( ) ) &&
    ( shearX        ( ) == theOther.shearX       ( ) ) &&
    ( shearY        ( ) == theOther.shearY       ( ) ) &&
    ( scaleX        ( ) == theOther.scaleX       ( ) ) &&
    ( scaleY        ( ) == theOther.scaleY       ( ) ) &&
    ( cutOff        ( ) == theOther.cutOff       ( ) ) &&
    ( cutOffRadius  ( ) == theOther.cutOffRadius ( ) ) &&
    ( colorFadeTo   ( ) == theOther.colorFadeTo  ( ) ) &&
    ( colorFading   ( ) == theOther.colorFading  ( ) );

  return bReturn;
}

void ShadowObject::blurImage ( QImage &theImage, int iBlur )
{
  expblur <16, 8>( theImage, iBlur );
}

void ShadowObject::drawContents ( QPainter *pPainter )
{ 
  if ( m_pMenuObject && m_pMenuObject->objectType ( ) == QString ( SELECTION_OBJECT ) ) {
    SelectionObject *pSelection = (SelectionObject *)m_pMenuObject;
    pSelection->drawShadows ( pPainter );
  }
  else if ( m_enType == TYPE_SHADOW )
    drawShadow ( pPainter );
  else if ( m_enType == TYPE_MIRROR )
    drawMirror ( pPainter );
  else if ( m_enType == TYPE_REFLECTION )
    drawReflection ( pPainter );
}

void ShadowObject::drawShadow ( QPainter *pPainter )
{
  if ( ( m_fDistance == 0.0 ) || ( m_fTransparency >= 1.0 ) )
    return;

  if ( m_imageShadow.isNull ( ) || m_imageBuffer.isNull ( ) )
    updateShadow ( true );

  if ( ! m_pMenuObject )
    return;

  QRect objectRect = m_pMenuObject->rect ( );

  QMatrix theMatrix;
  //ImageManipulator *pManipulator = ;
  Modifiers *pModifiers = m_pMenuObject->modifiers ( );
  // Here we calculate the center point of gravity (rotation)
  QPoint centerPos;
  //centerPos.setX ( objectRect.x ( ) + (int) ( (float) objectRect.width  ( ) / 2.0 ) );    //ooo
  centerPos.setX ( objectRect.x ( ) + static_cast<int> ( static_cast<float> ( objectRect.width  ( ) ) / 2.0 ) );  //xxx
  //centerPos.setY ( objectRect.y ( ) + (int) ( (float) objectRect.height ( ) / 2.0 ) );    //ooo
  centerPos.setY ( objectRect.y ( ) + static_cast<int> ( static_cast<float> ( objectRect.height ( ) ) / 2.0 ) );    //xxx
  // Here we define the cenetered rect.
  //QRect theRect ((int)-( objectRect.width ( ) / 2.0), (int)-( objectRect.height ( ) / 2.0 ), objectRect.width ( ), objectRect.height ( ) );   //ooo
  QRect theRect (static_cast<int> (-( objectRect.width ( ) / 2.0)), static_cast<int> (-( objectRect.height ( ) / 2.0 )), objectRect.width ( ), objectRect.height ( ) ); //xxx

  // From the Qt documentation ...
  // This transformation engine is a three-step pipeline
  // The first step uses the world transformation matrix.
  // The second step uses the window.
  // The third step uses the viewport.

  // First we translate to the appropriate location,
  //theMatrix.translate ( (double)centerPos.x ( ), (double)centerPos.y ( ) );   //ooo
  theMatrix.translate ( static_cast<double>(centerPos.x ( )), static_cast<double>(centerPos.y ( )) );   //xxx

  // then we apply the other modifications ...
  theMatrix.scale  ( pModifiers->fScaleX, pModifiers->fScaleY );
  theMatrix.shear  ( pModifiers->fShearX, pModifiers->fShearY );
  theMatrix.rotate ( pModifiers->fRotate );

  pPainter->setWorldMatrix ( theMatrix );
  pPainter->drawImage ( theRect.x ( ) + deltaX ( ), theRect.y ( ) + deltaY ( ), m_imageShadow );  //ooo
  //pPainter->drawImage ( theRect.x ( ) + deltaX ( ), theRect.y ( ) + deltaY ( ), m_imageShadow, 0, 0, 100, 50 );    //xxx

  theMatrix.reset ( );
  pPainter->setWorldMatrix ( theMatrix );
}

void ShadowObject::drawMirror ( QPainter *pPainter )
{
  if ( m_fTransparency >= 1.0 )
    return;

  if ( m_imageShadow.isNull ( ) || m_imageBuffer.isNull ( ) )
    updateShadow ( true );

  if ( ! m_pMenuObject )
    return;

  int iWidth   = m_pMenuObject->rect ( ).width  ( );
  int iHeight  = m_pMenuObject->rect ( ).height ( );
  int iX       = m_pMenuObject->rect ( ).x ( );
  int iY       = m_pMenuObject->rect ( ).y ( );
  enMType    mType = mirrorType      ( );
  QMatrix   theMatrix;
  Modifiers *pModifiers = m_pMenuObject->modifiers ( );

  int iDeltaX = (int)( iWidth  / 2.0 * ( 1.0 - pModifiers->fScaleX ) );
  int iDeltaY = (int)( iHeight / 2.0 * ( 1.0 - pModifiers->fScaleY ) );
  iX += iDeltaX;
  iY += iDeltaY;

  if ( mType == MIRROR_TOP )
    iY += - iHeight - (int)( m_iDeltaY * pModifiers->fScaleY ) + (int)( 2.0 * iDeltaY );
  else if ( mType == MIRROR_LEFT )
    iX += - iWidth  - (int)( m_iDeltaX * pModifiers->fScaleX ) + (int)( 2.0 * iDeltaX );
  else if ( mType == MIRROR_BOTTOM )
    iY +=   iHeight + (int)( m_iDeltaY * pModifiers->fScaleY ) - (int)( 2.0 * iDeltaY );
  else // if ( mType == MIRROR_RIGHT )
    iX +=   iWidth  + (int)( m_iDeltaX * pModifiers->fScaleX ) - (int)( 2.0 * iDeltaX );

  // First we translate to the appropriate location,
  theMatrix.translate ( (double)iX, (double)iY );

  // then we apply the other modifications ...
  theMatrix.scale  ( pModifiers->fScaleX, pModifiers->fScaleY );
  theMatrix.shear  ( pModifiers->fShearX, pModifiers->fShearY );
  theMatrix.rotate ( pModifiers->fRotate );

  pPainter->setWorldMatrix ( theMatrix );
  //printf ( "%s::%s::%d <%d - %d>\n", __FILE__, __FUNCTION__, __LINE__, m_iDeltaX, m_iDeltaY  );
  pPainter->drawImage ( 0, 0, m_imageShadow );	

  theMatrix.reset ( );
  pPainter->setWorldMatrix ( theMatrix );
}

void ShadowObject::drawReflection ( QPainter * )
{

}

void ShadowObject::drawContents ( QPainter *pPainter, int iRenderFrameNumber, int )
{
  // Next is to set the AnimationAttributes
  for ( uint t=0; t<(uint)m_listAnimationAttributes.count ( ) ;t++ )
    m_listAnimationAttributes[t]->setValue ( iRenderFrameNumber );

  // And then we ought to refresh the pixmap ...
  if ( m_listAnimationAttributes.count () > 0 )
    updateShadow ( true );

  drawContents ( pPainter );
}

bool ShadowObject::mousePressEvent ( QMouseEvent   * )
{
  return false;
}

bool ShadowObject::mouseReleaseEvent ( QMouseEvent * )
{
  return false;
}

bool ShadowObject::mouseDoubleClickEvent ( QMouseEvent * )
{
  return false;
}

StructureItem *ShadowObject::createStructure ( StructureItem *pParentItem )
{
  if ( ! pParentItem )  {
    m_pStructureItem = NULL;
    return NULL;
  }

  StructureItem *pShadowItem, *pTemp=NULL;
  enType theType = type ( );
  QString qsItem = tr   ( "Shadow Object" );
  if ( theType  == TYPE_MIRROR )
       qsItem    = tr   ( "Mirror Object" );
  QString qsName = qsItem;
  QString qsInfo (   tr ( "Shadow dist=%1 blur=%2 angle=%3 transparency=%4").
                    arg ( distance ( ) ).arg ( blur         ( ) ).
                    arg ( sunAngle ( ) ).arg ( transparency ( ) ) );

  pShadowItem = m_pStructureItem;
  if ( ! pShadowItem ) {
    pShadowItem = new StructureItem ( this, StructureItem::Attributes, pParentItem, pParentItem->lastChild ( ), qsName, qsInfo );
    //pShadowItem->setExpandable ( TRUE );  //ooo
    pShadowItem->setExpanded ( true );      //xxx
    m_pStructureItem = pShadowItem;
  }
  else {
    //pShadowItem->setText ( qsName, qsInfo );  //ooo
    pShadowItem->setText ( 0, qsName );         //xxx
    pShadowItem->setText ( 1, qsInfo );         //xxx
  }

  QString qsArgs;

  pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Sun Angle"    ), QString ( "%1" ).arg ( sunAngle ( ) ) );
  pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Distance"     ), QString ( "%1" ).arg ( distance ( ) ) );
  pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Blur Radius"  ), QString ( "%1" ).arg ( blur     ( ) ) );
  pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Transparency" ), QString ( "%1" ).arg ( transparency ( ) ) );

  if ( theType == TYPE_MIRROR ) {
    qsArgs = QString ( "%1, Color(%2, %3, %4)").arg ( sunAngle ( ) ).
                      arg ( m_colorFadeTo.red   ( ) ).
                      arg ( m_colorFadeTo.green ( ) ).
                      arg ( m_colorFadeTo.blue  ( ) );
    pTemp  = pShadowItem->setSibling ( (MenuObject *)this, pTemp, tr ( "Fading" ), qsArgs );

    qsArgs = QString ( "%1, Radius %2" ).arg ( cutOff ( ) ).arg ( cutOffRadius ( ) );
    pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Cut Off" ), qsArgs );

    qsArgs = QString ( "X<%1> Y<%2>" ).arg ( scaleX ( ) ).arg ( scaleY ( ) );
    pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Scale" ), qsArgs );

    qsArgs = QString ( "X<%1> Y<%2>" ).arg ( shearX ( ) ).arg ( shearY ( ) );
    pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Shear" ), qsArgs );
  }
  else {
    if ( pShadowItem->childCount ( ) == 8 ) {
      // previously TYPE_MIRROR. Need to delete the last three child items
      delete pShadowItem->lastChild ( );
      delete pShadowItem->lastChild ( );
      delete pShadowItem->lastChild ( );
    }
    qsArgs = tr ("color(%1, %2, %3)").
                      arg ( m_color.red   ( ) ).
                      arg ( m_color.green ( ) ).
                      arg ( m_color.blue  ( ) );
    pTemp  = pShadowItem->setSibling ( this, pTemp, tr ( "Shadow Color" ), qsArgs );
  }

  return pShadowItem;
}

bool ShadowObject::createContextMenu ( QPoint   )
{
  return false;
}

AnimationAttribute *ShadowObject::getSpecificAttributes (long, QString)
{
  return NULL;
}

// Overloaded virtual function
void ShadowObject::updateShadow ( )
{
  updateShadow ( true );
}

void ShadowObject::updateShadow ( bool bRecreate )
{
  if ( ! m_pMenuObject )
    return;

  float fSunAngle = m_fSunAngle / 180.0 * PI;

  if ( type ( ) == TYPE_SHADOW ) {
    m_iDeltaX = (int) ( sin ( 45.0 / 180.0 * PI ) * m_fDistance * cos ( fSunAngle ) );
    m_iDeltaY = (int) ( sin ( 45.0 / 180.0 * PI ) * m_fDistance * sin ( fSunAngle ) );
  }
  else {
    m_iDeltaX = 0;
    m_iDeltaY = 0;
    if ( m_fSunAngle > 315.0 )
      m_iDeltaY = (int) -m_fDistance;
    else if ( m_fSunAngle > 225.0 )
      m_iDeltaX = (int) -m_fDistance;
    else if ( m_fSunAngle > 135.0 )
      m_iDeltaY = (int)  m_fDistance;
    else if ( m_fSunAngle > 45.0 )
      m_iDeltaX = (int)  m_fDistance;
    else 
      m_iDeltaY = (int) -m_fDistance;
  }
  //  printf ( "%s::%s::%d <%d - %d>\n", __FILE__, __FUNCTION__, __LINE__, m_iDeltaX, m_iDeltaY  );
  // Next we create the image
  int         iFrameWidth = 0;
  QString     emptyAnimation;
  Modifiers   emptyModifier;
  MenuObject      *pMenuObject = NULL;
  SelectionObject *pSelection  = NULL;
  QColor      backgroundColor (   1,   1,   1 );
  QColor      foregroundColor ( 255, 255, 255 );

  if ( m_pMenuObject->objectType ( ) == TEXT_OBJECT )  {
    pMenuObject = m_pMenuObject->clone ( );
    if ( ! pMenuObject )
      return;
    pMenuObject->setShadow ( NULL );
    // Remember that the modifiers will be part of the Imagebject
    pMenuObject->setModifiers ( emptyModifier  );
    pMenuObject->setAnimation ( emptyAnimation );
    if ( m_enType != TYPE_MIRROR ) {
      TextObject *pText = (TextObject *)pMenuObject;
      pText->setStyleStrategy   ( QFont::NoAntialias ); // shadows should not be aliased, can not filter out the aliased ranges.
      pText->setForegroundColor ( foregroundColor );
      pText->setBackgroundColor ( backgroundColor );
    }
  }
  else if ( m_pMenuObject->objectType  ( ) == FRAME_OBJECT )  {  //tempFrame.objectType ( ) ) {
     pMenuObject = m_pMenuObject->clone ( );
    if ( ! pMenuObject )
      return;
    pMenuObject->setShadow ( NULL );
    // Remember that the modifiers will be part of the Imagebject
    pMenuObject->setModifiers ( emptyModifier  );
    pMenuObject->setAnimation ( emptyAnimation );
    FrameObject *pFrame = (FrameObject *)pMenuObject;
    iFrameWidth = pFrame->width ( );
    m_iDeltaX = m_iDeltaX - (int)( iFrameWidth / 2.0 );
    m_iDeltaY = m_iDeltaY - (int)( iFrameWidth / 2.0 );
    if ( m_enType != TYPE_MIRROR ) 
      pFrame->setFrameColor ( foregroundColor );
  }
  else if ( m_pMenuObject->objectType ( ) == SELECTION_OBJECT ) {
    pSelection = (SelectionObject *)m_pMenuObject;
    pSelection->updateShadows ( this, bRecreate );
  }
  else {
    // ImageObject, MoveObject
    // Otherwise we simply want to draw a rect in the right color / size
    if ( ( TYPE_MIRROR == m_enType && bRecreate ) ||
         ( TYPE_SHADOW == m_enType ) )  {
      pMenuObject = m_pMenuObject->clone ( );
      if ( ! pMenuObject )
        return;

      ImageObject *pImage = (ImageObject *)pMenuObject;
      pMenuObject->setShadow ( NULL ); 
      // Remember that the modifiers will be part of the Imagebject
      pMenuObject->setModifiers ( emptyModifier  );
      pMenuObject->setAnimation ( emptyAnimation );
      pImage->updatePixmap ( );
    }
  }

  if ( ! pSelection ) { // selectio objects already called this fnc for all seelcted objects inside.
    if ( m_enType == TYPE_SHADOW )
      createShadow ( pMenuObject, iFrameWidth );
    else if ( m_enType == TYPE_REFLECTION )
      createShadow ( pMenuObject, iFrameWidth );
    else if ( m_enType == TYPE_MIRROR )
      createMirror ( pMenuObject, iFrameWidth, bRecreate );

    //drawRedRect ( m_imageShadow );
    delete pMenuObject;
  }
}

void ShadowObject::createMirror ( MenuObject *pMenuObject, int iFrameWidth, bool bRecreate )
{
  if ( ! pMenuObject )
    return;
  // Now that we have the object to draw we should create the QImage - object ...
  QMatrix theMatrix;
  theMatrix.scale  ( m_fScaleX, m_fScaleY );
  theMatrix.shear  ( m_fShearX, m_fShearY );

  if ( bRecreate ) { // in case we have changed the CutOff parms we need to recreate all
    QImage theImage;
    QColor backgroundColor (   1,   1,   1 );
    QRect r = m_pMenuObject->rect ( );
    {
      //int iOffset1 = (int) ( iFrameWidth/2.0 + m_iBlurRadius + 10 );
      //int iOffset2 = (int) ( iFrameWidth + 2.0*m_iBlurRadius + 10 ); 
      int iOffset1 = (int) ( iFrameWidth/2.0 );
      int iOffset2 = (int) ( iFrameWidth ); 
      QRect    theRect     ( iOffset1, iOffset1, r.width ( ), r.height ( ) );
      QPixmap  thePix      ( r.width ( ) + iOffset2, r.height ( ) + iOffset2 );
      QPainter thePainter  ( &thePix );

      thePainter.fillRect       ( thePix.rect ( ), backgroundColor );
      pMenuObject->setRect      ( theRect );
      pMenuObject->drawContents ( &thePainter );
      // At this point we should have the image 
      //theImage = thePix.convertToImage ( );   //ooo
      theImage = thePix.toImage ( );            //xxx
      filterColor ( theImage, backgroundColor );
    }

    // Lastly we should filter out color keys if present (MovieObject, and Imagebject only)
    filterColorKeys ( theImage );

    enMType mType = mirrorType ( );
    // Okay flip the image aongside the Y - Axis
    if ( ( mType == MIRROR_TOP ) || ( mType == MIRROR_BOTTOM ) )
      //theImage = theImage.mirror ( false, true );     //ooo
      theImage = theImage.mirrored ( false, true );     //xxx
    else
      //theImage = theImage.mirror ( true, false );     //ooo
      theImage = theImage.mirrored ( true, false );     //xxx

    //theImage.setAlphaBuffer ( true );         //ooo
    theImage.createAlphaMask ( Qt::AutoColor ); //xxx
    cutOffImage ( theImage, mType  );
    m_imageBuffer = theImage.copy( );
  }
  // If cut off has not changed, then we can use the buffered image.
  //m_imageShadow = m_imageBuffer.xForm ( theMatrix );      //ooo
  m_imageShadow = m_imageBuffer.transformed ( theMatrix );  //xxx
  applyColorFading ( );
  blurShadow ( );
  applyTransparency ( );
}

void ShadowObject::filterColor ( QImage &theImage, QColor theColor )
{
  // At this point we have the frame/text/rect and need to blur this into a semi transparent image
  int x, y;
  QRgb *pLine, colorToFilter, currentPixel, colorTransparent;

  colorTransparent = qRgba ( 0, 0, 0, 0 );
  colorToFilter    = theColor.rgb ( ) | 0xff000000;
  for ( y=0;  y < theImage.height ( ); y++ ) {
    pLine = (QRgb *) theImage.scanLine ( y );
    if ( ! pLine )
      continue;
    for ( x=0; x < theImage.width ( ); x++ ) {
      currentPixel = *(pLine + x);
      if ( currentPixel == colorToFilter )
        *(pLine + x) = colorTransparent;
    }
  }
}

void ShadowObject::applyTransparency ( ) 
{
  if ( ( m_fTransparency <= 0.0 ) || ( m_fTransparency >= 1.0 ) )
    return;
  // At this point we have the frame/text/rect and need to blur this into a semi transparent image
  int x, y;
  QRgb *pLine, currentPixel, newPixel;
  unsigned long lAlpha;

  for ( y=0;  y < m_imageShadow.height ( ); y++ ) {
    pLine = (QRgb *) m_imageShadow.scanLine ( y );
    if ( ! pLine )
      continue;
    for ( x=0; x < m_imageShadow.width ( ); x++ ) {
      currentPixel = *(pLine + x);
      lAlpha   = (unsigned long ) ( ( currentPixel & 0xff000000 ) * ( 1.0 - m_fTransparency ) );
      lAlpha   = lAlpha & 0xff000000;
      newPixel = ( currentPixel & 0x00ffffff ) | lAlpha;
      *(pLine + x) = newPixel;
    }
  }
}

void ShadowObject::applyColorFading ( ) 
{
  if ( ( m_fColorFading <= 0.0 ) || ( m_fColorFading > 1.0 ) )
    return;

  // using a static lookup table, which should be way faster then calculating 
  static QRgb  lastColor  =  0;
  static float lastFading = -1.0;
  static unsigned char lookup[3][256];
  int x, y;
  QRgb *pLine, currentPixel, newPixel;
  volatile int iRed, iGreen, iBlue;

  if ( ( m_colorFadeTo.rgb ( ) != lastColor ) || ( m_fColorFading != lastFading ) ) {
    iRed   = m_colorFadeTo.red   ( );
    iGreen = m_colorFadeTo.green ( );
    iBlue  = m_colorFadeTo.blue  ( );

    for ( int t=0;t<256;t++ ) { // red
      lookup[0][t] = (unsigned char)( t - (int)( ( t - iRed   ) * m_fColorFading ) );
      lookup[1][t] = (unsigned char)( t - (int)( ( t - iGreen ) * m_fColorFading ) );
      lookup[2][t] = (unsigned char)( t - (int)( ( t - iBlue  ) * m_fColorFading ) );
    }
    lastColor  = m_colorFadeTo.rgb ( );
    lastFading = m_fColorFading;
  }

  // What we have now is a greyscale image. The values actually represent the 
  // transparency of the final shadow.
  // Thus we will create an image that has only one color and only modulate the 
  // transparency.
  for ( y=0;  y < m_imageShadow.height ( ); y++ ) {
    pLine = (QRgb *) m_imageShadow.scanLine ( y );
    if ( ! pLine )
      continue;
    for ( x=0; x < m_imageShadow.width ( ); x++ ) {
      currentPixel = *(pLine + x);
      iRed   = lookup [0][ qRed   ( currentPixel ) ];
      iGreen = lookup [1][ qGreen ( currentPixel ) ];
      iBlue  = lookup [2][ qBlue  ( currentPixel ) ];
      newPixel = ( currentPixel & 0xff000000 ) | ( iRed << 16 ) | ( iGreen << 8 ) | iBlue;  
      *(pLine + x) = newPixel;
    }
  }
}

void ShadowObject::createShadow ( MenuObject *pMenuObject, int iFrameWidth )
{
  if ( ! pMenuObject )
    return;
  QColor backgroundColor (   1,   1,   1 );
  QRect r = m_pMenuObject->rect ( );
  bool bImageOrText = true;
  // Now that we have the object to draw we should create the QImage - object ...
  QImage theImage;
  {
    int iOffset1 = (int) ( iFrameWidth/2.0 + m_iBlurRadius + 10 );
    int iOffset2 = (int) ( iFrameWidth + 2.0*m_iBlurRadius + 10 );
    QRect    theRect     ( iOffset1, iOffset1, r.width ( ), r.height ( ) );
    if ( ( pMenuObject->objectType ( ) == IMAGE_OBJECT ) ||
         ( pMenuObject->objectType ( ) == MOVIE_OBJECT ) )  {
      ImageObject *pImageObject = (ImageObject *)pMenuObject;
      QImage img = pImageObject->image ( ).copy ( );
      //img = img.convertDepth ( 32 );                      //ooo
      img = img.convertToFormat ( QImage::Format_RGB32 );   //xxx
      //theImage = QImage ( img.width ( ) + iOffset1, img.height ( ) + iOffset2, 32 ); //theRect.size ( ), 32 );    //ooo
      theImage = QImage ( img.width ( ) + iOffset1, img.height ( ) + iOffset2, QImage::Format_RGB32 );              //xxx
      //theImage.setAlphaBuffer ( true );           //ooo
      theImage.createAlphaMask ( Qt::AutoColor );   //xxx
      QRgb  *pBits = (QRgb *)theImage.bits ( );
      //int    iSize = (int)( theImage.numBytes ( ) / 4.0 );    //ooo
      int    iSize = (int)( theImage.byteCount ( ) / 4.0 );     //xxx
      for ( int t=0; t<iSize; t++ )
        pBits[t] = 0x00FFFFFF;

      //bitBlt ( &theImage, iOffset1, iOffset2, &img ); //ooo
      QPainter p( &theImage );                          //xxx
      p.drawImage( iOffset1, iOffset2, img );           //xxx
    }
    else  {  // Text, or Frame Objects
      QPixmap  thePix      ( r.width ( ) + iOffset2, r.height ( ) + iOffset2 );
      QPainter thePainter  ( &thePix );

      thePainter.fillRect  ( thePix.rect ( ), backgroundColor );
      pMenuObject->setRect ( theRect );
      pMenuObject->drawContents ( &thePainter );
      // At this point we should have thePix as we want/need with a shadowee backdrop
      //theImage = thePix.convertToImage ( );   //ooo
      theImage = thePix.toImage ( );            //xxx
      //theImage.setAlphaBuffer ( true );           //ooo
      theImage.createAlphaMask ( Qt::AutoColor );   //xxx
      bImageOrText = false;
    }
  }

  // Lastly we should filter out color keys if present (MovieObject, and Imagebject only)
  filterColorKeys ( theImage );

  // At this point we have the frame/text/rect and need to blur this into a semi transparent image
  int x, y;
  QRgb *pLine, currentPixel, newPixel;
  int iRed, iGreen, iBlue, iAlpha;
  m_imageShadow = theImage;

  // What we have now is a greyscale image. The values actually represent the 
  // transparency of the final shadow.
  // Thus we will create an image that has only one color and only modulate the 
  // transparency.
  //iRed   = m_color.Qt::red   ( );	//ooo
  iRed   = m_color.red   ( );		//xxx
  //iGreen = m_color.Qt::green ( );	//ooo
  iGreen = m_color.green ( );		//xxx
  //iBlue  = m_color.Qt::blue  ( );	//ooo
  iBlue  = m_color.blue  ( );		//xxx
  if ( bImageOrText )    {  // For performance reasons I check only once and have the loop in here twice
    for ( y=0;  y < m_imageShadow.height ( ); y++ ) {
      pLine = (QRgb *) m_imageShadow.scanLine ( y );
      if ( ! pLine )
        continue;
      for ( x=0; x < m_imageShadow.width ( ); x++ ) {
        currentPixel = *(pLine + x);
        iAlpha = (int)( (1.0 - m_fTransparency) * qAlpha ( currentPixel ) );
        newPixel = qRgba ( iRed, iGreen, iBlue, iAlpha );
        *(pLine + x) = newPixel;
      }
    }
  }
  else  {
    for ( y=0;  y < m_imageShadow.height ( ); y++ ) {
      pLine = (QRgb *) m_imageShadow.scanLine ( y );
      if ( ! pLine )
        continue;
      for ( x=0; x < m_imageShadow.width ( ); x++ ) {
        currentPixel = *(pLine + x);
        iAlpha = (int)( (1.0 - m_fTransparency) * qRed ( currentPixel ) );
        newPixel = qRgba ( iRed, iGreen, iBlue, iAlpha );
        *(pLine + x) = newPixel;
      }
    }
  }

  blurShadow ( );
}

void ShadowObject::blurShadow ( )
{
  if ( m_iBlurRadius > 0 ) {
    expblur <16, 8>( m_imageShadow, m_iBlurRadius );
    // the resulting image is a bit off around the corners, thus we should fade the shadow out
    int x, y;
    QRgb *pLine, currentPixel, newPixel;
    int iRed, iGreen, iBlue, iAlpha, iWidth, iHeight, iPhaseZone;
    float fPhaseOut;

    // What we have now is a greyscale image. The values actually represent the 
    // transparency of the final shadow.
    // Thus we will create an image that has only one color and only modulate the 
    // transparency.
    iWidth  = m_imageShadow.width   ( );
    iHeight = m_imageShadow.height  ( );
    iPhaseZone = 5;
    for ( y=0;  y < m_imageShadow.height ( ); y++ ) {
      pLine = (QRgb *) m_imageShadow.scanLine ( y );
      if ( ! pLine )
        continue;
      for ( x=0; x < m_imageShadow.width ( ); x++ ) {
	currentPixel = *(pLine + x);
	iRed   = qRed   ( currentPixel );
	iGreen = qGreen ( currentPixel );
	iBlue  = qBlue  ( currentPixel );
	fPhaseOut = 1.0;
	if ( x < iPhaseZone )
	  fPhaseOut = x / iPhaseZone;
	if ( x > iWidth - iPhaseZone )
	  fPhaseOut = ( iWidth - x ) / iPhaseZone;
	if ( y < iPhaseZone )
	  fPhaseOut = y / iPhaseZone;
	if ( y > iHeight - iPhaseZone )
	  fPhaseOut = ( iHeight - y ) / iPhaseZone;
	if ( fPhaseOut < 1.0 ) {
	  iAlpha = (int)( fPhaseOut * qAlpha ( currentPixel ) );
	
	  // Draw a red frame around the Shadow object so we know where we are ...
	  //if ( ( x == 0 ) || ( x == iWidth - 1) || ( y == 0 ) || ( y == iHeight - 1 ) )
	  //  newPixel = qRgba ( 255, 0, 0, 255 );
	  //else
	  newPixel = qRgba ( iRed, iGreen, iBlue, iAlpha );
	  *(pLine + x) = newPixel;
	}
      }
    }
  }
}

bool ShadowObject::filterColorKeys ( QImage &theShadow )
{
  ImageObject *pImageObject = NULL;
  // Only Image, or Movie objects of interest for color keying.
  if ( ( m_pMenuObject->objectType ( ) != IMAGE_OBJECT ) &&
       ( m_pMenuObject->objectType ( ) != MOVIE_OBJECT ) )
    return false;

  // Note MovieObject is derived off ImageObject.
  pImageObject = (ImageObject *)m_pMenuObject;
  // Check if Color Keying is enabled.
  if ( pImageObject->manipulator( ).listColorKeys.count ( ) < 1 )
    return false;

  // Next we create the min/max Color keys, to check if we ought to filter the pixel out or not
  ImageManipulator *pManipulator = &pImageObject->manipulator ( );
  QImage theImage   = pImageObject->image ( );
  QList<QColor *>listColorKeyMin;		//oxx
  QList<QColor *>listColorKeyMax;		//oxx
  QColor *pColorMin, *pColorMax, theColor, targetColor;
  QRgb thePixel, *pLine, *pShadowLine;

  int t, x, y, iX, iY, iDelta, iRed, iGreen, iBlue, iColorKeyCount;
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
    if ( ! pLine )
      continue;
    iY = y + 10; 
    if ( ( iY >= 0 ) && ( iY < theShadow.height ( ) ) ) {
      pShadowLine = (QRgb *)theShadow.scanLine  ( iY  );
      for (x=0; x < theShadow.width ( ); x++)	{
	iX = x + 10;
	if ( ( iX >= 0) && ( iX < theShadow.width ( ) ) ) {
	  if ( x >= theImage.width ( ) )
	    thePixel = *(pLine + theImage.width ( ) );
	  else
	    thePixel = *(pLine + x);
	  thePixel &= 0x00ffffff;		// filter out the transparency part of the color (not used in Qt 3.x)
	  if (thePixel != 0x000000)	{	// And check if this is a pixel we need to handle ...
	    targetColor = QColor ( thePixel );
	    if ( filterColorKey ( &targetColor, iColorKeyCount, &listColorKeyMin, &listColorKeyMax ) )
	      *(pShadowLine + iX ) = qRgba ( 0, 0, 0, 0 );
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

// This function checks if pColor is within the min/max of one of the color keys.
// Same as ImageObject::filterColorKey.
bool ShadowObject::filterColorKey ( QColor *pColor, int iColorKeyCount, QList<QColor *> *pListColorMin, QList<QColor *>*pListColorMax )		//oxx
{
  int iRed, iGreen, iBlue, t;

  if ( ( iColorKeyCount < 1 ) || ( ! pListColorMin ) || ( ! pListColorMax ) )
    return false;
  //iRed = pColor->Qt::red(); iGreen = pColor->Qt::green(); iBlue = pColor->Qt::blue ();	//ooo
  iRed = pColor->red(); iGreen = pColor->green(); iBlue = pColor->blue ();			//xxx
  for (t=0;t<iColorKeyCount;t++)	{
    //if (	(iRed   >= (*pListColorMin)[t]->Qt::red  () && iRed   <= (*pListColorMax)[t]->Qt::red  ()) &&		//ooo
    if (	(iRed   >= (*pListColorMin)[t]->red  () && iRed   <= (*pListColorMax)[t]->red  ()) &&			//xxx
		//(iGreen >= (*pListColorMin)[t]->Qt::green() && iGreen <= (*pListColorMax)[t]->Qt::green()) &&		//ooo
		(iGreen >= (*pListColorMin)[t]->green() && iGreen <= (*pListColorMax)[t]->green()) &&			//xxx
		//(iBlue  >= (*pListColorMin)[t]->Qt::blue () && iBlue  <= (*pListColorMax)[t]->Qt::blue ()) )		//ooo
                (iBlue  >= (*pListColorMin)[t]->blue () && iBlue  <= (*pListColorMax)[t]->blue ()) )			//xxx
      return true;
  }
  return false;
}

void ShadowObject::cutOffImage ( QImage &theImage, enMType theType )
{
  int   x, y, iStart, iStop, iStartCutOff, iStopCutOff, iTotalSteps, iCounter;
  unsigned char iTransparency;
  float fValue, fHalfCutRadius, fCutOff;
  QRgb *pLine;

  fCutOff = m_fCutOff;
  if ( ( theType == MIRROR_LEFT ) || ( theType == MIRROR_TOP ) )
    fCutOff = 1.0 - m_fCutOff;

  fHalfCutRadius = m_fCutOffRadius / 2.0;
  if ( ( theType == MIRROR_TOP ) || ( theType == MIRROR_BOTTOM ) ) {
    iStartCutOff = (int) ( ( fCutOff - fHalfCutRadius ) * theImage.height ( ) );
    if ( iStartCutOff < 0 )
         iStartCutOff = 0;
    iStopCutOff  = (int) ( ( fCutOff + fHalfCutRadius ) * theImage.height ( ) );
    if ( iStopCutOff > theImage.height ( ) )
         iStopCutOff = theImage.height ( );

    if ( theType == MIRROR_TOP ) {
      iStart = 0;
      iStop  = iStartCutOff;
    }
    else { // CUT_BOTTOM;
      iStart = iStopCutOff;
      iStop  = theImage.height ( );
    }

    for ( y=iStart; y<iStop; y++ ) {
      pLine = (QRgb *)theImage.scanLine ( y );
	  if ( ! pLine )
		  continue;
      for ( x=0;x<theImage.width ( ); x++ )
	pLine[x] = pLine[x] & 0x00FFFFFF;
    }

    // Next is to make the transition ( cutOfRadius )
    iTotalSteps = iStopCutOff - iStartCutOff;
    iCounter = 0;
    for ( y=iStartCutOff; y<iStopCutOff;y++ ) {
      pLine = (QRgb *)theImage.scanLine ( y );
      // calc transparency ...
      fValue = (float)iCounter++ / (float)iTotalSteps;
      if ( theType == MIRROR_BOTTOM )
      	   fValue  =  1.0 - fValue;
      iTransparency = (unsigned char)( 0xFF * fValue );
      //printf ( "%s::%s::%d cntr=%d tot=%d fVal=%f %d>\n", __FILE__, __FUNCTION__, __LINE__, iCounter, iTotalSteps, fValue, iTransparency );
      for ( x=0;x<theImage.width ( ); x++ )
	pLine[x] = pLine[x] & ( ( iTransparency << 24 )| 0xFFFFFF );
    }
  }

  if ( ( theType == MIRROR_LEFT ) || ( theType == MIRROR_RIGHT ) ) {
    iStartCutOff = (int) ( ( fCutOff - fHalfCutRadius ) * theImage.width ( ) );
    if ( iStartCutOff < 0 )
         iStartCutOff = 0;
    iStopCutOff  = (int) ( ( fCutOff + fHalfCutRadius ) * theImage.width ( ) );
    if ( iStopCutOff > theImage.width ( ) )
         iStopCutOff = theImage.width ( );

    if ( theType == MIRROR_LEFT ) {
      iStart = 0;
      iStop  = iStartCutOff;
    }
    else { // CUT_RIGHT;
      iStart = iStopCutOff;
      iStop  = theImage.width ( );
    }

    for ( y=0; y<theImage.height ( ); y++ ) {
      pLine = (QRgb *)theImage.scanLine ( y );
	  if ( ! pLine )
		  continue;
      for ( x=iStart; x<iStop; x++ )
	pLine[x] = pLine[x] & 0x00FFFFFF;
    }

    // Next is to make the transition ( cutOfRadius )
    iTotalSteps = iStopCutOff - iStartCutOff;
    for ( y=0; y<theImage.height ( );y++ ) {
      pLine = (QRgb *)theImage.scanLine ( y );
      iCounter = 0;
      // calc transparency ...
      for ( x=iStartCutOff; x<iStopCutOff; x++ ) {
	fValue = (float)iCounter++ / (float)iTotalSteps;
	if ( theType == MIRROR_RIGHT )
	     fValue  =  1.0 - fValue;
	iTransparency = (unsigned char)( 0xFF * fValue );
	pLine[x] = pLine[x] & ( ( iTransparency << 24 )| 0xFFFFFF );
      }
    }
  }

  //  drawRedRect ( theImage );
}

void ShadowObject::drawRedRect ( QImage &theImage )
{
  QRgb *pLine;
  int x, y;
  // Draw red rect ...
  pLine = (QRgb *)theImage.scanLine ( 0 );
  if ( ! pLine ) 
	  return;
  for ( x=0;x<theImage.width ( ); x++ )
    pLine[x] = 0xFFFF0000;
  pLine = (QRgb *)theImage.scanLine ( theImage.height ( )-1 );
  if ( ! pLine )
	  return;
  for ( x=0;x<theImage.width ( ); x++ )
    pLine[x] = 0xFFFF0000;
  for ( y=0; y<theImage.height ( );y++ ) {
    pLine = (QRgb *)theImage.scanLine ( y );
	if ( ! pLine )
		continue;
    pLine[0] = 0xFFFF0000;
    pLine[theImage.width ( )-1] = 0xFFFF0000;
  }
}

bool ShadowObject::writeProjectFile ( QDomElement &theElement ) 
{
  // If this is a Mask then we do no need to store anything ...
  QDomDocument xmlDoc     = theElement.ownerDocument ( );
  QDomElement  shadowNode = xmlDoc.createElement( SHADOW_OBJECT );	// <ShadowObject>
 
  if ( m_enType != TYPE_SHADOW )
    shadowNode.setAttribute( SHADOW_OBJECT_TYPE, (int)m_enType );

  if ( m_color.isValid ( ) )
    shadowNode.setAttribute( SHADOW_OBJECT_COLOR, m_color.name ( ) );

  if ( m_fTransparency != 0.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_TRANSPARENCY, m_fTransparency );

  if ( m_fDistance != -1.0 )
    shadowNode.setAttribute( SHADOW_OBJECT_DISTANCE, m_fDistance );

  if (m_iBlurRadius != 3 )
    shadowNode.setAttribute( SHADOW_OBJECT_BLUR_RADIUS, m_iBlurRadius );

  if (m_fSunAngle != 0.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_SUN_ANGLE,  m_fSunAngle );
  
  if (m_fShearX != 0.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_SHEAR_X,  m_fShearX );
  
  if (m_fShearY != 0.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_SHEAR_Y,  m_fShearY );
  
  if (m_fScaleX != 1.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_SCALE_X,  m_fScaleX );
  
  if (m_fScaleY != 1.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_SCALE_Y,  m_fScaleY );

  if (m_fCutOff != 0.5f )
    shadowNode.setAttribute( SHADOW_OBJECT_CUT_OFF,  m_fCutOff );
  
  if (m_fCutOffRadius != 0.3f )
    shadowNode.setAttribute( SHADOW_OBJECT_CUT_OFF_RADIUS,  m_fCutOffRadius );
  
  if (m_fColorFading != 0.0f )
    shadowNode.setAttribute( SHADOW_OBJECT_COLOR_FADING,  m_fColorFading );
  
  if ( m_colorFadeTo.isValid ( ) )
    shadowNode.setAttribute( SHADOW_OBJECT_COLOR_FADE_TO, m_colorFadeTo.name ( ) );

  theElement.appendChild( shadowNode );

 return true;
}

bool ShadowObject::readProjectFile ( QDomNode &theNode )
{
  QDomElement theElement = theNode.toElement();
  QDomAttr attribute;

  attribute = theElement.attributeNode ( SHADOW_OBJECT_TYPE );
  if ( ! attribute.isNull ( ) )
    m_enType = (enType)attribute.value ( ).toInt ( );
  
  attribute = theElement.attributeNode ( SHADOW_OBJECT_COLOR );
  if ( ! attribute.isNull ( ) )
    m_color = QColor ( attribute.value ( ) );
  
  attribute = theElement.attributeNode ( SHADOW_OBJECT_TRANSPARENCY );
  if ( ! attribute.isNull ( ) )
    m_fTransparency = attribute.value ( ).toFloat( );
  
  attribute = theElement.attributeNode ( SHADOW_OBJECT_DISTANCE );
  if ( ! attribute.isNull ( ) )
    m_fDistance = attribute.value ( ).toFloat ( );
  
  attribute = theElement.attributeNode ( SHADOW_OBJECT_BLUR_RADIUS );
  if ( ! attribute.isNull ( ) )
    m_iBlurRadius = attribute.value ( ).toInt ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_SUN_ANGLE );
  if ( ! attribute.isNull ( ) )
    m_fSunAngle = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_SHEAR_X );
  if ( ! attribute.isNull ( ) )
    m_fShearX = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_SHEAR_Y );
  if ( ! attribute.isNull ( ) )
    m_fShearY = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_SCALE_X );
  m_fScaleX = 1.0f;
  if ( ! attribute.isNull ( ) )
    m_fScaleX = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_SCALE_Y );
  m_fScaleY = 1.0f;
  if ( ! attribute.isNull ( ) )
    m_fScaleY = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_CUT_OFF );
  m_fCutOff = 0.5f;
  if ( ! attribute.isNull ( ) )
    m_fCutOff = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_CUT_OFF_RADIUS );
  m_fCutOffRadius = 0.3f;
  if ( ! attribute.isNull ( ) )
    m_fCutOffRadius = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_COLOR_FADING );
  if ( ! attribute.isNull ( ) )
    m_fColorFading = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( SHADOW_OBJECT_COLOR_FADE_TO );
  if ( ! attribute.isNull ( ) )
    m_colorFadeTo = QColor ( attribute.value ( ) );

  return true;
}

float ShadowObject::transparency ( )
{
  return m_fTransparency;
}

int ShadowObject::blur ( )
{
  return m_iBlurRadius;
}

float ShadowObject::distance ( )
{
  return m_fDistance;
}

float ShadowObject::sunAngle ( )
{
  return m_fSunAngle;
}

void ShadowObject::setTransparency ( float fTransparency )
{
  m_fTransparency = fTransparency;
}

void ShadowObject::setType ( enType theType )
{
  m_enType = theType;
}

ShadowObject::enMType ShadowObject::mirrorType ( )
{
  // The sun angle is actually determining the MIRROR_TYPE
  if ( m_fSunAngle > 315.0 )
    return MIRROR_TOP;
  else if ( m_fSunAngle > 225.0 )
    return MIRROR_LEFT;
  else if ( m_fSunAngle > 135.0 )
    return MIRROR_BOTTOM;
  else if ( m_fSunAngle > 45.0 )
    return MIRROR_RIGHT;

  return MIRROR_TOP;
}

ShadowObject::enType ShadowObject::type ( )
{
  return m_enType;
}

void ShadowObject::setColor ( QColor color )
{
  m_color = color;
}

QColor &ShadowObject::color ( )
{
  return m_color;
}

void ShadowObject::setBlur ( int iBlurRadius )
{
  if ( iBlurRadius < 3 )
    iBlurRadius = 0;
  m_iBlurRadius = iBlurRadius;
}

void ShadowObject::setDistance ( float fDistance )
{
  if ( fDistance != m_fDistance ) {
    
    if ( m_pMenuObject ) {
      float fSunAngle = m_fSunAngle / 180.0 * PI;
      m_iDeltaX = (int) ( sin ( 45.0 / 180.0 * PI ) * fDistance * cos ( fSunAngle ) );
      m_iDeltaY = (int) ( sin ( 45.0 / 180.0 * PI ) * fDistance * sin ( fSunAngle ) );
    }
  }

  m_fDistance = fDistance;
}

void ShadowObject::setSunAngle ( float fSunAngle )
{
  if ( fSunAngle != m_fSunAngle ) {
    if ( m_pMenuObject ) {
      float fSunAngle = m_fSunAngle / 180.0 * PI;
      m_iDeltaX = (int) ( sin ( 45.0 / 180.0 * PI ) * m_fDistance * cos ( fSunAngle ) );
      m_iDeltaY = (int) ( sin ( 45.0 / 180.0 * PI ) * m_fDistance * sin ( fSunAngle ) );
    }
  }
  m_fSunAngle = fSunAngle;
}

int ShadowObject::deltaX ( )
{
  return ( m_iDeltaX - m_iBlurRadius - 10 );
}

int ShadowObject::deltaY ( )
{
  return ( m_iDeltaY - m_iBlurRadius - 10 );
}

float ShadowObject::shearX ( )
{
  return m_fShearX;
}

float ShadowObject::shearY ( )
{
  return m_fShearY;
}

void ShadowObject::setShear ( float fShearX, float fShearY )
{
  m_fShearX = fShearX;
  m_fShearY = fShearY;
}

float ShadowObject::scaleX ( )
{
  return m_fScaleX;
}

float ShadowObject::scaleY ( )
{
  return m_fScaleY;
}

void ShadowObject::setScale ( float fScaleX, float fScaleY )
{
  m_fScaleX = fScaleX;
  m_fScaleY = fScaleY;
}

float ShadowObject::cutOff ( )
{
  return m_fCutOff;
}

float ShadowObject::cutOffRadius ( )
{
  return m_fCutOffRadius;
}

void ShadowObject::setCutOff ( float fCutOff, float fCutOffRadius )
{
  m_fCutOff = fCutOff;
  m_fCutOffRadius = fCutOffRadius;
}

float ShadowObject::colorFading ( )
{
  return m_fColorFading;
}

QColor &ShadowObject::colorFadeTo ( )
{
  return m_colorFadeTo;
}

void ShadowObject::setColorFading ( QColor theColor, float fFading )
{
  m_colorFadeTo = theColor;
  m_fColorFading = fFading;
}

MenuObject *ShadowObject::clone ( QWidget *pParentWidget, MenuObject *pMenuObject )
{
  if ( ! pParentWidget )
         pParentWidget =  MenuObject::parent ( );
  ShadowObject *pNewObject = new ShadowObject( pMenuObject, pParentWidget );
  pNewObject->setType         ( type         ( ) );
  pNewObject->setTransparency ( transparency ( ) );
  pNewObject->setBlur         ( blur         ( ) );
  pNewObject->setDistance     ( distance     ( ) );
  pNewObject->setSunAngle     ( sunAngle     ( ) );
  pNewObject->setColor        ( color        ( ) );
  pNewObject->setShear        ( shearX       ( ), shearY       ( ) );
  pNewObject->setCutOff       ( cutOff       ( ), cutOffRadius ( ) );
  pNewObject->setColorFading  ( colorFadeTo  ( ), colorFading  ( ) );

  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect      (  rect      ( ) );
  pNewObject->setModifiers ( *modifiers ( ) );

  return pNewObject;
}

