/***************************************************************************
    imageobject.cpp
                             -------------------
    ImageObject class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class is the encapsulation of the ImageObject.
    
    AN ImageObject is a visible object in a DVD Menu.
    
****************************************************************************/

#include <QFileInfo>
#include <QPainter>
#include <QMatrix>
#include <QMessageBox>

#include "xml_dvd.h"
#include "imageobject.h"
#include "dialogmatrix.h"
#include "overlayobject.h"
#include "structuretoolbar.h"

// The following three includes required for undo.
#include "global.h"
#include "qdvdauthor.h"
#include "dvdmenuundoobject.h"

ImageObject::ImageData::ImageData ( )
  : MenuObject::Data ( )
{
  m_bHidden   = false;
  m_iDataType = Data::IMAGE;
}

ImageObject::ImageData::~ImageData ( )
{
}

MenuObject::Data &ImageObject::ImageData::operator = ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  MenuObject::Data::operator= ( theOther );
  if ( m_iDataType == theOther.m_iDataType )  {
       ImageData *pOther  = (ImageData *)&theOther;
       m_imageManipulator = pOther->m_imageManipulator;
       m_bHidden          = pOther->m_bHidden;
  }
  if ( theOther.m_iDataType == Data::DATA )  {
       m_imageManipulator   = theOther.m_modifiers;
  }
  return *this;
}

bool ImageObject::ImageData::operator == ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  bool bReturn = ( MenuObject::Data::operator == ( theOther ) );
  if ( ! bReturn )
    return false;

  if ( m_iDataType == theOther.m_iDataType )  {
    ImageData *pOther = (ImageData *)&theOther;
    bReturn  = ( ( m_imageManipulator == pOther->m_imageManipulator ) &&
                 ( m_bHidden          == pOther->m_bHidden          ) );
  }
  return bReturn;
}

MenuObject::Data *ImageObject::ImageData::clone ( )
{ 
  ImageData *pData = new ImageData;
  *pData  = *this;
  return pData;
}

ImageObject::ImageObject ( QWidget *pParent )
  : MenuObject ( pParent )
{
  if ( m_pData )
    delete m_pData;        // part of the base class
  m_pData = new ImageData; // will be deleted in base class
  m_qsObjectType = QString ( IMAGE_OBJECT );
}

ImageObject::~ImageObject ( )
{
}

void ImageObject::setImage ( QImage &theImage )
{
  m_image = theImage;
}

void ImageObject::setFile ( QString fileName )
{
  QString   qsExtractionPath;
  QFileInfo fileInfo ( fileName );
  m_qsFileName = fileInfo.fileName ( );
  setName ( m_qsFileName );
  //qsExtractionPath = fileInfo.baseName ( true );	//ooo
  qsExtractionPath = fileInfo.baseName ( );		//xxx
  setExtractionPath ( qsExtractionPath );
  m_qsFileName = fileName;
}

void ImageObject::setHidden ( bool bHidden )
{
  imageData ( )->m_bHidden = bHidden;
}

void ImageObject::setZoom ( float fZoom )
{
  // For the purpose of this class the Zoom is alreaduy incorporated into m_pixmap ...
  manipulator ( ).fZoom = fZoom;
}

void ImageObject::setManipulator(ImageManipulator &theOther)
{
  manipulator ( ) = theOther;
}

ImageManipulator &ImageObject::manipulator()
{
  ImageData *pData = (ImageData *)data ( );
  return     pData->m_imageManipulator;
}

Modifiers *ImageObject::modifiers()
{
  // Return the base class only ...
  // Note: this is needed to use the same MatrixDialog for text and images.
  return (Modifiers *) &manipulator ( );
}

ImageObject::ImageData *ImageObject::imageData ( )
{
  return (ImageData *)m_pData;
}

QString &ImageObject::fileName()
{
	return m_qsFileName;
}

QImage &ImageObject::image ( )
{
  if ( overlay ( ) )  {
    OverlayObject *pOverlayObject = (OverlayObject *)overlay ( );
    return pOverlayObject->image ( m_image );
  }
  return m_image;
}

float ImageObject::zoom ( )
{
        return manipulator ( ).fZoom;
}

bool ImageObject::hidden ( )
{
        return imageData ( )->m_bHidden;
}

void ImageObject::drawContents ( QPainter *painter )
{
  QMatrix theMatrix;
  // Here we calculate the center point of gravity (rotation)
  QPoint centerPos;
  centerPos.setX ( rect ( ).x ( ) + (int)( (float)rect ( ).width  ( ) / 2.0 ) );
  centerPos.setY ( rect ( ).y ( ) + (int)( (float)rect ( ).height ( ) / 2.0 ) );
  // Here we define the cenetered rect.
  QRect theRect  ( (int)-( rect ( ).width ( )/ 2.0 ), (int)-( rect ( ).height ( ) / 2.0 ), rect ( ).width ( ), rect ( ).height ( ) );

  QPen     thePen ( QColor ( 255, 0, 0 ), 2, Qt::DashDotLine );
  painter->setPen ( thePen );

  // From the Qt documentation ...
  // This transformation engine is a three-step pipeline
  // The first step uses the world transformation matrix.
  // The second step uses the window.
  // The third step uses the viewport.

  // First we translate to the appropriate location,
  theMatrix.translate ( (double)centerPos.x ( ), (double)centerPos.y ( ) );

  //. then we apply the other modifications ...
  //theMatrix.scale  ( modifiers ( )->fScaleX, modifiers ( )->fScaleY );
  theMatrix.scale  ( manipulator ( ).fScaleX, manipulator ( ).fScaleY );
  theMatrix.shear  ( manipulator ( ).fShearX, manipulator ( ).fShearY );
  theMatrix.rotate ( manipulator ( ).fRotate );

  // Here we get the rect that encompasses the button for this object.
  m_boundingRect = theMatrix.mapRect ( theRect );

  if ( isActive ( ) )
    drawActiveFrame ( painter );

  painter->setWorldMatrix ( theMatrix );

  // Here we re-set the world matrix and translate the newly drawn ... thing ...
  if ( isActive ( ) )
    painter->drawRect(theRect);

  theMatrix.reset ( );
  painter->setWorldMatrix ( theMatrix );

  // check if we have a complete transparency, in which case we don't even need to bother...
  if ( manipulator ( ).fTransparency == 1.0 )
    return;

  if ( ( ! isActive ( ) ) && ( ! hidden ( ) ) ) {
                if ( m_pShadow )
               m_pShadow->drawContents ( painter );

    int t, x, y;
    QRgb thePixel;
    int iSourceHeight = image ( ).height ( );
    int iSourceWidth  = image ( ).width  ( );
    int iSourceStartX = 0;
    int iSourceStartY = 0;
    int iTargetStartX = painter->viewport ( ).x ( ) + m_boundingRect.x ( );
    int iTargetStartY = painter->viewport ( ).y ( ) + m_boundingRect.y ( );
    // Use the fast method whenever possible
//		if (	( manipulator ( ).fRotate        == 0.0 ) &&
//			( manipulator ( ).fShearX        == 0.0 ) &&
//			( manipulator ( ).fShearY        == 0.0 ) &&
//			( manipulator ( ).fTransparency  == 0.0 ) &&
//			( manipulator ( ).listColorKeys.count ( ) < 1 ) ) {
    if (	( manipulator ( ).fTransparency  == 0.0 ) &&
      ( manipulator ( ).listColorKeys.count ( ) < 1 ) ) {
    if (	( manipulator ( ).fRotate        == 0.0 ) &&
      ( manipulator ( ).fShearX        == 0.0 ) &&
      ( manipulator ( ).fShearY        == 0.0 ) )  {
      QMatrix theMatrix;
//		  theMatrix.scale  ( manipulator ( ).fScaleX, manipulator ( ).fScaleY );
      theMatrix.shear  ( manipulator ( ).fShearX, manipulator ( ).fShearY );
      theMatrix.rotate ( manipulator ( ).fRotate );
      //QImage temp = image ( ).xForm  ( theMatrix );	//ooo
      QImage temp = image ( ).transformed  ( theMatrix );	//xxx
      //painter->drawImage  ( iTargetStartX, iTargetStartY, temp, iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight, Qt::CopyROP );		//ooo
      painter->drawImage  ( iTargetStartX, iTargetStartY, temp, iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight, Qt::AutoColor );		//xxx
    }
    else
      //painter->drawImage ( iTargetStartX, iTargetStartY, image ( ), iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight, Qt::CopyROP );	//ooo
    painter->drawImage ( iTargetStartX, iTargetStartY, image ( ), iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight, Qt::AutoColor );	//xxx
    }
    //iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight, Qt::CopyROP, true);
    // Okay if we can not use bitBlt we have to go through the exact calculations ...
    else	{
      // First we need to create a Pixmap which holds the Target pixels
      QImage theImage = image ( );
      QColor sourceColor, targetColor, theColor;
      QRgb   sourcePixel;
      QImage sourceImg;
      float  fTargetTrans, fSourceTrans;
      fTargetTrans = 1.0 - manipulator ( ).fTransparency;
      fSourceTrans =       manipulator ( ).fTransparency;
      if ( fTargetTrans != 0.0 )	{
        QPixmap sourcePix ( iSourceWidth, iSourceHeight );
        //bitBlt (&sourcePix, 0, 0, painter->device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, Qt::CopyROP, true);	//ooo
        //bitBlt (&sourcePix, 0, 0, painter->device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, Qt::AutoColor);		//xxx
        //QPainter p( &sourcePix );	//xxx
        //p.drawImage( 0, 0, painter->device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight );			//xxx
        // To get Pixel information we need a QImage however ...
        //sourceImg = sourcePix.convertToImage();	//ooo
        sourceImg = sourcePix.toImage();		//xxx
        // Now we can get the original Pixel information ...
//				Note: This ought to be done in the updatePixmap () - function
//					- Also there we leave all info untouched which is (0,0,0)
//					- Also when we move AND when fTransparency != 0.0 then we need to do updatePixmap everytime.
      }
      // Note that this function is actually using the viewport - transformation provided by Qt. The previous bitBlt's
      // did paint directly onto the device, thus we had to manually add the viewport coords. Now we have this info already
      // in the painter and thus we can substract them here.
      iTargetStartX -= painter->viewport().x();
      iTargetStartY -= painter->viewport().y();

      // Next we create the min/max Color keys, to check if we ought to filter the pixel out or not
      QList<QColor *>listColorKeyMin;		//oxx
      QList<QColor *>listColorKeyMax;		//oxx
      QColor *pColorMin, *pColorMax;
      if (manipulator().listColorKeys.count() > 0)	{
          int iDelta, iRed, iGreen, iBlue;
        for (t=0;t<(int)manipulator().listColorKeys.count();t++)	{
          iDelta = (int)(manipulator().listColorKeys[t]->fDeltaColor * 255.0);
          theColor = QColor (manipulator().listColorKeys[t]->theColor);
          //iRed = theColor.Qt::red() - iDelta; iGreen = theColor.Qt::green() - iDelta; iBlue = theColor.Qt::blue() - iDelta;	//ooo
          iRed = theColor.red() - iDelta; iGreen = theColor.green() - iDelta; iBlue = theColor.blue() - iDelta;			//xxx
          iRed = (iRed < 0) ? 0 : iRed; iGreen = (iGreen < 0) ? 0 : iGreen; iBlue = (iBlue < 0) ? 0 : iBlue;
          pColorMin = new QColor (iRed, iGreen, iBlue);
          //iRed = theColor.Qt::red() + iDelta; iGreen = theColor.Qt::green() + iDelta; iBlue = theColor.Qt::blue() + iDelta;	//ooo
          iRed = theColor.red() + iDelta; iGreen = theColor.green() + iDelta; iBlue = theColor.blue() + iDelta;			//xxx
          iRed = (iRed > 255) ? 255 : iRed; iGreen = (iGreen > 255) ? 255 : iGreen; iBlue = (iBlue > 255) ? 255 : iBlue;
          pColorMax = new QColor (iRed, iGreen, iBlue);

          listColorKeyMin.append (pColorMin);
          listColorKeyMax.append (pColorMax);
        }
      }
      for (y=0; y < iSourceHeight; y++)	{
        for (x=0; x < iSourceWidth; x++)	{
          thePixel = theImage.pixel(x+iSourceStartX, y+iSourceStartY);
          thePixel &= 0x00ffffff;			// filter out the transparency part of the color (not used in Qt 3.x)
          if (thePixel != 0x000000)	{	// And check if this is a pixel we need to handle ...
            // If there is a transparency part, or some color we need to filter out, 
            // then  we need to go through this slow loop.
            if ( (fTargetTrans != 0.0) || (manipulator().listColorKeys.count ()) )	{ 
              sourcePixel = sourceImg.pixel(x, y);	// calculate the mix of background and foreground.
              sourceColor = QColor (sourcePixel);
              targetColor = QColor ( thePixel );
              if (filterColorKey ( &targetColor, &listColorKeyMin, &listColorKeyMax ))
                theColor = sourceColor;
              else
                theColor = QColor (
                  //(int)(fSourceTrans * sourceColor.Qt::red()   + (int)(fTargetTrans * targetColor.Qt::red())),	//ooo
                  (int)(fSourceTrans * sourceColor.red()   + (int)(fTargetTrans * targetColor.red())),			//xxx
                  //(int)(fSourceTrans * sourceColor.Qt::green() + (int)(fTargetTrans * targetColor.Qt::green())),	//ooo
                  (int)(fSourceTrans * sourceColor.green() + (int)(fTargetTrans * targetColor.green())),		//xxx
                  //(int)(fSourceTrans * sourceColor.Qt::blue()  + (int)(fTargetTrans * targetColor.Qt::blue()))	//ooo
                  (int)(fSourceTrans * sourceColor.blue()  + (int)(fTargetTrans * targetColor.blue()))			//xxx
                ); 
              painter->setPen ( theColor );
            }
            else	// No transparency is slightly easier ...
              painter->setPen ( QColor ( thePixel ) );
            painter->drawPoint(x+iTargetStartX, y+iTargetStartY);
          }
        }
      }
      if (manipulator().listColorKeys.count() > 0)	{
        for (t=0;t<(int)manipulator().listColorKeys.count();t++)	{
          delete listColorKeyMin[t];
          delete listColorKeyMax[t];
        }
      }
    }
  }
  // Phew ... This funtion has grown over time ...
}

void ImageObject::drawContents (QPainter *pPainter, int iRenderFrameNumber, int)
{
  // Next is to set the AnimationAttributes
  bool bRefresh = false;
  for (uint t=0; t<(uint)m_listAnimationAttributes.count ( ); t++)
    bRefresh |= m_listAnimationAttributes[t]->setValue ( iRenderFrameNumber );

  //  printf ( "[%d] : Start<%d> Stop<%d>\n", iRenderFrameNumber, modifiers ( )->iStartFrame, modifiers ( )->iStopFrame );
  // If start stop have been set then we check the range
  if ( ( ( iRenderFrameNumber < modifiers ( )->iStartFrame ) && ( modifiers ( )->iStartFrame != -1 ) ) || 
       ( ( iRenderFrameNumber > modifiers ( )->iStopFrame  ) && ( modifiers ( )->iStopFrame  != -1 ) ) ) 
    return;

  if ( overlay ( ) )
       overlay ( )->drawContents ( pPainter, iRenderFrameNumber, 0 );
  if ( shadow  ( ) )
       shadow  ( )->drawContents ( pPainter, iRenderFrameNumber, 0 );

  // And then we ought to refresh the pixmap ...
  if ( bRefresh ) {
    updatePixmap ( );
    updateShadow ( );
  }
  // before we can go ahead and draw the contents with the newly set attributes.
  drawContents (pPainter);
}

// This function checks if pColor is within the min/max of one of the color keys.
bool ImageObject::filterColorKey ( QColor *pColor, QList<QColor *> *pListColorMin, QList<QColor *>*pListColorMax )	//oxx
{
  int iRed, iGreen, iBlue, t, iColorKeyCount;
  iColorKeyCount = manipulator().listColorKeys.count ();
  if ( ( iColorKeyCount < 1 ) || ( ! pListColorMin ) || ( ! pListColorMax ) )
    return false;

  // If image is rotated then pitch balck ought to be removed ...
  if ( ( ( modifiers ( )->fShearX != 0.0 ) || 
	 ( modifiers ( )->fShearX != 0.0 ) || 
	 ( modifiers ( )->fRotate != 0.0 ) ) && 
       ( *pColor == QColor ( 0, 0, 0 ) ) )
    return true;

  iRed = pColor->red(); iGreen = pColor->green(); iBlue = pColor->blue ();
  for (t=0;t<iColorKeyCount;t++)	{
    if ( (iRed   >= (*pListColorMin)[t]->red  () && iRed   <= (*pListColorMax)[t]->red  ()) && 
	 (iGreen >= (*pListColorMin)[t]->green() && iGreen <= (*pListColorMax)[t]->green()) &&
	 (iBlue  >= (*pListColorMin)[t]->blue () && iBlue  <= (*pListColorMax)[t]->blue ()) )
      return true;
  }
  return false;
}

void ImageObject::updatePixmap()
{  
  // This function will take the input image and pass it through the whole modifications pipeline,
  // so we get a nice and clean rotated / colored image again.

  // In order to speed up the whole procedure, we set the matrix first to scale the image,
  // then we handle the color functions on the smaller image and lastly we apply the rotation,shearing...
  int    iRed, iGreen, iBlue, iAlpha;
  int    x, y;
  int    iLighter = 0;
  float  fAdjust  = 0.0;
  QColor theColor;
  QRgb   thePix;	// QRgb defined as :  0xAARRGGBB;
  Rgba   rgba;
  QMatrix theMatrix;
  // Here we calculate the center point of gravity (rotation)
  QPoint centerPos;
  
  if ( fileName ( ).isEmpty ( ) )
    return;

  QImage theImage ( fileName ( ) );

  if ( theImage.isNull ( ) )
    return;

  // the kenBurns is taken care of here ...
  if ( ( manipulator().fKenBurnsX1 > -1.0 ) || ( manipulator().fKenBurnsY1 > -1.0 ) ) {
    float fKen[4];
    int   iKen[4];
    // if X2 and Y2 from manipulator is -1 then we go all the way to the max ext of the image
    fKen[0] = manipulator ( ).fKenBurnsX1;
    fKen[1] = manipulator ( ).fKenBurnsY1;
    fKen[2] = manipulator ( ).fKenBurnsX2;
    fKen[3] = manipulator ( ).fKenBurnsY2;
    iKen[0] = 0;
    iKen[1] = 0;
    iKen[2] = theImage.width  ( );
    iKen[3] = theImage.height ( );
  
    if ( fKen[0] > fKen[2] ) {
         fKen[0] = fKen[2];
               fKen[2] = manipulator ( ).fKenBurnsX1;
    }
    if ( fKen[1] > fKen[3] ) {
           fKen[1] = fKen[3];
               fKen[3] = manipulator ( ).fKenBurnsY1;
    }
    if ( fKen[2] < 0.0 )
               fKen[2] = 1.0;
    if ( fKen[3] < 0.0 )
               fKen[3] = 1.0;
    //iKen[0] = (int)(   fKen[0] * iKen[2] );                       //ooo
    iKen[0] = static_cast<int> (   fKen[0] * iKen[2] );             //xxx
    //iKen[1] = (int)(   fKen[1] * iKen[3] );                       //ooo
    iKen[1] = static_cast<int> (   fKen[1] * iKen[3] );             //xxx
    //iKen[2] = (int)( ( fKen[2] - fKen[0] ) * iKen[2] );           //ooo
    iKen[2] = static_cast<int>( ( fKen[2] - fKen[0] ) * iKen[2] );  //xxx
    //iKen[3] = (int)( ( fKen[3] - fKen[1] ) * iKen[3] );           //ooo
    iKen[3] = static_cast<int> ( ( fKen[3] - fKen[1] ) * iKen[3] ); //xxx
    if ( iKen[0] > theImage.width ( ) )
         iKen[0] = 0;
    if ( iKen[1] > theImage.height  ( ) )
         iKen[1] = 0;
    if ( iKen[0] + iKen[2] > theImage.width  ( ) )
         iKen[2] = theImage.width  ( ) - iKen[0];
    if ( iKen[1] + iKen[3] > theImage.height ( ) )
         iKen[3] = theImage.height ( ) - iKen[1];
  
    theImage = theImage.copy ( iKen[0], iKen[1], iKen[2], iKen[3] );
  }

  //theImage = theImage.smoothScale ( rect ().width(), rect().height(), Qt::IgnoreAspectRatio);	//ooo
  theImage = theImage.scaled ( rect ().width(), rect().height(), Qt::IgnoreAspectRatio);		//xxx

  //centerPos.setX (rect().x() + (int)((float)rect().width () / 2.0));                                  //ooo
  centerPos.setX (rect().x() + static_cast<int> (static_cast<float> ( rect().width () / 2.0) ) );       //xxx
  //centerPos.setY (rect().y() + (int)((float)rect().height() / 2.0));                                  //ooo
  centerPos.setY (rect().y() + static_cast<int> (static_cast <float> ( rect().height() / 2.0) ) );      //xxx

  // First we translate to the appropriate location,
  //theMatrix.translate ((double)centerPos.x(), (double)centerPos.y());                                 //ooo
  theMatrix.translate (static_cast<double> ( centerPos.x() ), static_cast<double> ( centerPos.y() ) );  //xxx
  // then we apply the other modifications ...
  theMatrix.scale  ( manipulator ( ).fScaleX, manipulator ( ).fScaleY );

  // Here we create the scaled image ...
  //theImage = theImage.xForm ( theMatrix );    	//ooo
  theImage = theImage.transformed ( theMatrix );	//xxxx
  //m_image = theImage.transformed ( theMatrix );	        //xxx
  // create empty image.
  //QImage imageColored = QImage (theImage.width(), theImage.height(), theImage.depth());		//oooo
  QImage imageColored = QImage (theImage.width(), theImage.height(), QImage::Format_RGB32);	//xxx
  //imageColored.setAlphaBuffer ( true );		//oooo
  if ( ( manipulator ( ).fBrightness != 0.0 ) ||
       ( manipulator ( ).fRed        != 1.0 ) ||
       ( manipulator ( ).fGreen      != 1.0 ) ||
       ( manipulator ( ).fBlue       != 1.0 ) ) {
    if ( manipulator ( ).fBrightness > 0.0 )	{
      iLighter = 1;
      fAdjust = 100 + manipulator ( ).fBrightness * 300;
    }
    else	{
      iLighter = -1;
      fAdjust  = 100 + manipulator ( ).fBrightness * -300;
    }
    for (y=0;y<theImage.height();y++)	{
      for (x=0;x<theImage.width();x++)	{
        thePix = theImage.pixel ( x, y );
        //iRed   = (int)((float)qRed  (thePix) * manipulator ( ).fRed   );                              //ooo
        iRed   = static_cast<int> ( static_cast<float> ( qRed  (thePix) * manipulator ( ).fRed )  );    //xxx
        //iBlue  = (int)((float)qBlue (thePix) * manipulator ( ).fBlue  );                              //ooo
        iBlue  = static_cast<int> ( static_cast<float> ( qBlue (thePix) * manipulator ( ).fBlue ) );    //ooo
        //iGreen = (int)((float)qGreen(thePix) * manipulator ( ).fGreen );                              //ooo
        iGreen = static_cast<int> ( static_cast<float> ( qGreen(thePix) * manipulator ( ).fGreen ) );   //xxx
        iAlpha = qAlpha ( thePix );

        theColor =  QColor ( iRed, iGreen, iBlue );
        if ( iLighter != 0 ) {
    if ( iLighter == 1 )
      //theColor = theColor.light ( (int)fAdjust );                 //ooo
      theColor = theColor.light ( static_cast<int> ( fAdjust ) );   //xxx
    else 
      //theColor = theColor.dark  ( (int)fAdjust );                 //ooo
      theColor = theColor.dark  ( static_cast<int> ( fAdjust ) );   //xxx
        }
        // This test is to mask out the transparency color (0, 0, 0)
        if ( theColor == QColor ( 0, 0, 0 ) )
    rgba      = QColor ( 0, 0, 1 );
        else {
    rgba.setRgb   ( theColor.rgb ( ) );
    rgba.setAlpha ( iAlpha );
        }
        imageColored.setPixel ( x, y, rgba.rgb ( ) );
      }
    }
  }
  else
    imageColored = theImage;
  // Next we re-set the matrix ...
  theMatrix.reset ();
  // and plug in the remaining operations ...
  theMatrix.shear  ( manipulator ( ).fShearX, manipulator ( ).fShearY );

  theMatrix.rotate ( manipulator ( ).fRotate );
  //m_image = imageColored.xForm   ( theMatrix );		//ooo
  m_image = imageColored.transformed   ( theMatrix );	//xxx

  emit ( signalUpdatePixmap ( ) );
}

bool ImageObject::readProjectFile  (QDomNode &theNode)
{
  QDomElement theElement = theNode.toElement();
  // The video node has only attributes and nothing else.
  QDomAttr attribute = theElement.attributeNode ( IMAGE_OBJECT_FILE_NAME );
  if ( ! attribute.isNull ( ) )
    setFile ( attribute.value ( ) );

  setHidden ( false );
  attribute = theElement.attributeNode ( IMAGE_OBJECT_HIDDEN );
  if ( ! attribute.isNull ( ) && attribute.value ( ) == "true" )
    setHidden  ( true );

  QDomNode  manNode = theNode.firstChild ( );
  while ( ! manNode.isNull ( ) )  {
    QDomElement manElement = manNode.toElement();
    if (manElement.tagName() == MANIPULATOR)
      manipulator().readProjectFile (manNode);
    else if (manElement.tagName() == MENU_OBJECT)
      MenuObject::readProjectFile( theNode );

    manNode = manNode.nextSibling();
  }
  this->updatePixmap ();
  return true;
}

bool ImageObject::writeProjectFile (QDomElement &theElement)
{
  QDomDocument xmlDoc = theElement.ownerDocument();
  QDomElement imageNode = xmlDoc.createElement( IMAGE_OBJECT );	// <FrameObject>
  // Here we set the attributes of the <dvdauthor> tag
  if (!m_qsFileName.isNull())
    imageNode.setAttribute ( IMAGE_OBJECT_FILE_NAME, m_qsFileName );
  if ( hidden ( ) )
    imageNode.setAttribute ( IMAGE_OBJECT_HIDDEN, "true" );

  manipulator().writeProjectFile (imageNode);

  theElement.appendChild( imageNode );
  // And here we write the base class ...
  return MenuObject::writeProjectFile( imageNode );
}

void ImageObject::resetStructureItem ( )
{
  if ( m_pShadow )
       m_pShadow->resetStructureItem ( );

  if ( m_pOverlay )
       m_pOverlay->resetStructureItem ( );

  if ( m_pStructureItem )
    delete m_pStructureItem;
  m_pStructureItem = NULL;
  manipulator ( ).m_pStructureItem = NULL;
}

StructureItem *ImageObject::createStructure ( StructureItem *pParentItem )
{
  StructureItem *pTemp=NULL;
  QFileInfo fileInfo ( m_qsFileName );
  QString qsName;

  if ( ! pParentItem )  {
    if ( m_pOverlay  )
      pTemp = m_pOverlay->createStructure ( NULL );
    if ( m_pShadow )
      pTemp = m_pShadow->createStructure  ( NULL );
    manipulator ( ).createStructure ( NULL );
    m_pStructureItem = NULL;
    return NULL;
  }

  qsName = tr( "Image Object" );

  if ( ! m_pStructureItem ) { 
    m_pStructureItem = new StructureItem ( this, StructureItem::Image, pParentItem, pParentItem->lastChild ( ), qsName, fileInfo.fileName ( ) );
    //m_pStructureItem->setText ( 0, qsName );			//xxx
    //m_pStructureItem->setText ( 1, fileInfo.fileName ( ) );	//xxx
    //m_pStructureItem->setExpandable      ( TRUE );		//ooo
    m_pStructureItem->setExpanded      ( false );		//xxx
  }
 
  //  pTemp = (StructureItem *)m_pStructureItem->firstChild ( );
  //  if ( pTemp ) printf ( "ImageObject : <%s>\n", pTemp->text ( 0 ).ascii() );
  //  else         printf ( "ImageObject : No First for <%s>.\n", fileInfo.fileName().ascii() );

  pTemp = m_pStructureItem->setSibling ( this, NULL, tr ( "Geometry" ), QString ( "%1, %2, %3, %4" ).
       	         arg ( rect().x()).arg(rect().y()).arg(rect().width()).arg(rect().height()) );

  if ( m_pOverlay )
    pTemp = m_pOverlay->createStructure ( m_pStructureItem );

  if ( m_pShadow )
    pTemp = m_pShadow->createStructure ( m_pStructureItem );

  manipulator ( ).createStructure ( m_pStructureItem, pTemp );	//oooo

  return m_pStructureItem;
}

bool ImageObject::mousePressEvent (QMouseEvent *pEvent)
{
  m_currentMousePos = pEvent->pos ( );
  if ( pEvent->button ( ) == Qt::RightButton )  {
    QPoint globalPos = pEvent->globalPos ( );
    return createContextMenu ( globalPos );
  }
  else {
    m_bDrawRect = true;
  }

  return false;
}

bool ImageObject::mouseReleaseEvent ( QMouseEvent * )
{  
  UndoBuffer *pUndo = Global::pApp->getUndoBuffer ( );
  pUndo->removeUnchanged ( );

  m_bDrawRect = false;
  updatePixmap ( );
  emit ( signalUpdatePixmap    ( ) );
  emit ( signalUpdateStructure ( ) );
  return false;
}

bool ImageObject::mouseDoubleClickEvent (QMouseEvent *)
{
  // Back to the MenuPreview - class, where I can handle this request ...
  emit ( signalModifyMe ( this ) );
  return false;
}

bool ImageObject::createContextMenu ( QPoint globalPos )
{
  /*QString qsShadow, qsOverlay;
  QPoint globalPos2 = globalPos;
  Q3PopupMenu *pStackMenu = new Q3PopupMenu(m_pParent);
  pStackMenu->insertItem ( tr ("Cut") , this, SLOT ( slotCut  ( ) ) );
  pStackMenu->insertItem ( tr ("Copy"), this, SLOT ( slotCopy ( ) ) );
  pStackMenu->insertSeparator ( );
  pStackMenu->insertItem ( tr ("To Front")  , this, SLOT ( slotToFront ( ) ) );
  pStackMenu->insertItem ( tr ("To Back")   , this, SLOT ( slotToBack  ( ) ) );
  globalPos.setY ( globalPos.y ( ) - 25 );
  globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint().width()); // -100);
  pStackMenu->popup ( globalPos, 1 );

  qsShadow   = tr ( "Add Shadow ..."  );
  if ( shadow ( ) )
    qsShadow = tr ( "Edit Shadow ..." );

  qsOverlay   = tr ( "Add Mask ..."  );
  if ( overlay ( ) )
    qsOverlay = tr ( "Edit Mask ..." );


  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = new Q3PopupMenu ( m_pParent );
  m_pContextMenu->setCheckable ( true );
  m_pContextMenu->insertItem ( tr ("Edit ..." )      , this, SLOT ( slotEdit      ( ) ) );
  m_pContextMenu->insertItem ( tr ("Properties ...") , this, SLOT ( slotProperties( ) ) );
  m_pContextMenu->insertItem ( tr ("Matrix ...")     , this, SLOT ( slotMatrix    ( ) ) );
  m_pContextMenu->insertItem ( tr ("Delete")         , this, SLOT ( slotDelete    ( ) ) );
  m_pContextMenu->insertSeparator ( );
  m_pContextMenu->insertItem ( tr ("Rotate 90")      , this, SLOT ( slotRotate90  ( ) ) );
  m_pContextMenu->insertItem ( tr ("Rotate 180")     , this, SLOT ( slotRotate180 ( ) ) );
  m_pContextMenu->insertItem ( tr ("Rotate 270")     , this, SLOT ( slotRotate270 ( ) ) );
  m_pContextMenu->insertSeparator ( );
  m_pContextMenu->insertItem ( qsShadow              , this, SLOT ( slotAddShadow ( ) ) );
  if ( shadow ( ) )
    m_pContextMenu->insertItem ( tr ("Delete Shadow"), this, SLOT ( slotDeleteShadow ( ) ) );

  m_pContextMenu->insertItem ( qsOverlay             , this, SLOT ( slotAddOverlay   ( ) ) );
  if ( overlay ( ) )
    m_pContextMenu->insertItem ( tr ("Delete Mask"),   this, SLOT ( slotDeleteOverlay( ) ) );

  //	m_pContextMenu->insertItem ( tr ("Add Frame")      , this, SLOT ( slotAddFrame  ( ) ) );
  //	m_pContextMenu->insertItem ( tr ("Add Text")       , this, SLOT ( slotAddText   ( ) ) );
  m_pContextMenu->insertSeparator ( );
  m_pContextMenu->insertItem ( tr ("Define as Button"), this, SLOT( slotDefineAsButton ( ) ) );
  // Blocking call.
  m_pContextMenu->exec(globalPos2, 6);
  delete pStackMenu;
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;
  return true;*/	//oooo
  
  QPoint globalPos2 = globalPos;
  
  if ( m_pContextMenu )
    delete m_pContextMenu;
  
  QAction *editNewAction =           new QAction("Edit ...", this);
  QAction *matrixNewAction =         new QAction("Matrix ...", this);
  QAction *deleteNewAction =         new QAction("Delete", this);
  QAction *rotate90NewAction =       new QAction("Rotate 90", this);
  QAction *rotate180NewAction =      new QAction("Rotate 180", this);
  QAction *rotate270NewAction =      new QAction("Rotate 270", this);
  QAction *defineAsButtonNewAction = new QAction("Define as Button", this);
  
  m_pContextMenu = new QMenu (m_pParent);
  m_pContextMenu->addAction(editNewAction);
  m_pContextMenu->addAction(matrixNewAction);
  m_pContextMenu->addAction(deleteNewAction);
  m_pContextMenu->addSeparator ( );
  m_pContextMenu->addAction(rotate90NewAction);
  m_pContextMenu->addAction(rotate180NewAction);
  m_pContextMenu->addAction(rotate270NewAction);
  m_pContextMenu->addSeparator ( );
  m_pContextMenu->addAction(defineAsButtonNewAction);
  
  connect(editNewAction,           SIGNAL(triggered()), this, SLOT( slotEdit            ( ) ) );
  connect(matrixNewAction,         SIGNAL(triggered()), this, SLOT( slotMatrix          ( ) ) );
  connect(deleteNewAction,         SIGNAL(triggered()), this, SLOT( slotDelete          ( ) ) );
  connect(rotate90NewAction,       SIGNAL(triggered()), this, SLOT( slotRotate90        ( ) ) );
  connect(rotate180NewAction,      SIGNAL(triggered()), this, SLOT( slotRotate180       ( ) ) );
  connect(rotate270NewAction,      SIGNAL(triggered()), this, SLOT( slotRotate270       ( ) ) );
  connect(defineAsButtonNewAction, SIGNAL(triggered()), this, SLOT( slotDefineAsButton  ( ) ) );
  
  m_pContextMenu->exec( globalPos2, deleteNewAction );
  
  if (m_pContextMenu)
    delete m_pContextMenu;
  m_pContextMenu = NULL;
  
  return true;
}

bool ImageObject::mouseMoveEvent (QMouseEvent *pEvent)
{
  // Slightly different behaviour for images.
  // if we stretch X and/or Y separately we'll mess rotation up
  // this is why we have to prevent this. 
  // So the stretch will be reset and ShearX will be used instead ScaleX if rotated.
  int iPos = 0;

  switch ( m_objectState ) {
  case MenuObject::StateScaleLeftX: 
  case MenuObject::StateScaleRightX: {
    iPos = pEvent->pos().x() - m_currentMousePos.x();
    //double fZoom   = (double)( iPos / 100.0 );            //ooo
    double fZoom   = static_cast<double> ( iPos / 100.0 );  //xxx
    if (fZoom > 1.0)
      fZoom = fZoom * 2.0 - 1.0;

    if ( m_objectState == MenuObject::StateScaleLeftX )
      fZoom *= -1;
    if ( modifiers()->fRotate == 0.0 )
      modifiers( )->fScaleX *= ( 1.0 + fZoom );
    else
      modifiers( )->fShearX -= fZoom;
    update   ( );
  }
  break;
  case MenuObject::StateScaleTopY:
  case MenuObject::StateScaleBottomY: {
    iPos = pEvent->pos().y() - m_currentMousePos.y();
    //double fZoom   = (double)( iPos / 100.0 );            //ooo
    double fZoom   = static_cast<double> ( iPos / 100.0 );  //xxx
    if (fZoom > 1.0)
      fZoom = fZoom * 2.0 - 1.0;

    if ( m_objectState == MenuObject::StateScaleTopY )
      fZoom *= -1;
    if ( modifiers()->fRotate == 0.0 )
      modifiers( )->fScaleY *= ( 1.0 + fZoom );
    else
      modifiers( )->fShearY += fZoom;
    update   ( );
  }
  break;
  case MenuObject::StateRotate:
    modifiers()->fScaleX = 1.0;
    modifiers()->fScaleY = 1.0;    
  default:
    return MenuObject::mouseMoveEvent ( pEvent );
  }
  m_currentMousePos = pEvent->pos();
  return false;
}

void ImageObject::slotEdit()
{
	mouseDoubleClickEvent(NULL);
}

void ImageObject::slotProperties ( )
{
/*  ImageManipulator *pMan= &manipulator();

  DialogGeometry *pGeometry = new DialogGeometry ( );
  pGeometry->initMe ( rect ( ), pMan );
  connect ( pGeometry->m_pButtonAnimation, SIGNAL ( clicked ( ) ), this, SLOT ( slotAnimateMe ( ) ) );

  if ( pGeometry->exec ( ) == QDialog::Accepted )  {
    DVDMenuUndoObject *pUndo = new DVDMenuUndoObject ( this, MenuObject::StateUnknown );
    int iX, iY, iWidth, iHeight;
    iX      = pGeometry->m_pEditX->text().toInt();
    iY      = pGeometry->m_pEditY->text().toInt();
    iWidth  = pGeometry->m_pEditWidth ->text().toInt();
    iHeight = pGeometry->m_pEditHeight->text().toInt();
    QRect rect ( iX, iY, iWidth, iHeight );

    ImageManipulator::colorKeying *pColorKey;
    ListViewColorItem *pItem;
    uint t;
    float  fDelta;
    Rgba   theColor;

    // First we should remove all entries in the color Key
    for ( t=0; t<pMan->listColorKeys.count ( ); t++ )
      delete pMan->listColorKeys[t];
    pMan->listColorKeys.clear ( );
    // And then we add whats currently in the widget.
    if ( pGeometry->m_pListViewCK->childCount ( ) )	{
      pItem = (ListViewColorItem *)pGeometry->m_pListViewCK->firstChild ( );
      while (pItem)	{
        theColor.fromString ( pItem->text ( 0 ) );	// The Color name is stored in the first text
        fDelta = pItem->text ( 1 ).toFloat ( );		// The delta value is stored in the second column.
        pColorKey = new ImageManipulator::colorKeying ( theColor.rgb ( ), fDelta );
        pMan->listColorKeys.append ( pColorKey );
        pItem = (ListViewColorItem *)pItem->itemBelow ();
      }
    }
    setRect ( rect );
    updatePixmap ( );
    updateShadow ( );
    emit ( signalUpdatePixmap ( ) );
    if   ( pUndo->hasChanged  ( ) )  {
      UndoBuffer *pBuffer = Global::pApp->getUndoBuffer ( );
      if ( pBuffer )
           pBuffer->push ( pUndo );
    }
    else
      delete pUndo;
  }
  delete pGeometry;*/	//oooo
}

void ImageObject::slotRotate90()
{
	rotate (90.0);
}

void ImageObject::slotRotate180()
{
	rotate (180.0);
}

void ImageObject::slotRotate270()
{
	rotate (270.0);
}

void ImageObject::rotate(float fRot)
{
  QMatrix theMatrix;
  theMatrix.rotate ( fRot );
  float fRotate = manipulator ( ).fRotate + fRot;
  if ( fRotate  > 180.0 )
       fRotate -= 360.0;
  manipulator ( ).fRotate = fRotate;
  //m_image  = m_image.xForm    ( theMatrix );      //ooo
  m_image  = m_image.transformed    ( theMatrix );  //xxx

  emit   ( signalUpdatePixmap ( ) );
  parent ( )->update ( );
}

void ImageObject::slotMatrix()
{
  if ( m_pMatrixDialog )
    return;

  m_pMatrixDialog = new DialogMatrix(MenuObject::parent());
  m_pMatrixDialog->initMe ( this );
  connect ( this, SIGNAL ( signalDeleteMe ( MenuObject * ) ), m_pMatrixDialog, SLOT ( slotDeleteObject ( MenuObject * ) ) );
  m_pMatrixDialog->show   ( );
}

void ImageObject::slotDefineAsButton ( )
{
  emit ( signalDefineAsButton ( this ) );
}

MenuObject *ImageObject::clone ( QWidget *pParent, MenuObject * )
{
  if ( !pParent )
    pParent = MenuObject::parent  ( );
  ImageObject *pNewObject = new ImageObject ( pParent );

  pNewObject->setImage ( image    ( ) );
  pNewObject->setFile  ( fileName ( ) );
  pNewObject->setZoom  ( zoom     ( ) );
  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect        ( rect        ( ) );
  pNewObject->setManipulator ( manipulator ( ) );
  pNewObject->setAnimation   ( animation   ( ) );
  pNewObject->setHidden      ( hidden      ( ) );

  MenuObject *pShadow = shadow  ( );
  if ( pShadow )
       pShadow = pShadow->clone ( pParent, pNewObject );
  pNewObject->setShadow ( pShadow );

  MenuObject *pOverlay = overlay ( );
  if ( pOverlay )
       pOverlay = pOverlay->clone ( pParent, pNewObject );
  pNewObject->setOverlay ( pOverlay );

  return pNewObject;
}

// Called from xyz, when creating the AnimationAttribute objects, which hold the list of values for the attribute to be set.
// Note: only those AnimationAttribute objects are created which are modified during the animation (Menu - video).
AnimationAttribute *ImageObject::getSpecificAttributes (long iMaxNumberOfFrames, QString qsProperty)
{
  AnimationAttribute *pAnimAttr = NULL;

  if (qsProperty == "zoom")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fZoom, &manipulator ( ).fZoom);
  else if (qsProperty == "rotate")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fRotate, &manipulator ( ).fRotate);
  else if (qsProperty == "shear.x")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fShearX, &manipulator ( ).fShearX);
  else if (qsProperty == "shear.y")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fShearY, &manipulator ( ).fShearY);
  else if (qsProperty == "scale.x")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fScaleX, &manipulator ( ).fScaleX);
  else if (qsProperty == "scale.y")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fScaleY, &manipulator ( ).fScaleY);
  else if (qsProperty == "transparency")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fTransparency, &manipulator ( ).fTransparency);
  else if (qsProperty == "visibleFromTo.startFrame") {
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).iStartFrame, &manipulator ( ).iStartFrame);
    pAnimAttr->setNeedRefresh ( false );
  }
  else if (qsProperty == "visibleFromTo.stopFrame") {
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).iStopFrame, &manipulator ( ).iStopFrame);
    pAnimAttr->setNeedRefresh ( false );
  }
  //	else if (qsProperty == "backgroundFileName")
  //		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).backgroundFileName, &manipulator ( ).backgroundFileName);
  //	else if (qsProperty == "showBackground")
  //		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).bShowBackground, &manipulator ( ).bShowBackground);
  else if (qsProperty == "kenBurns.x1")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fKenBurnsX1, &manipulator ( ).fKenBurnsX1);
  else if (qsProperty == "kenBurns.y1")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fKenBurnsY1, &manipulator ( ).fKenBurnsY1);
  else if (qsProperty == "kenBurns.x2")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fKenBurnsX2, &manipulator ( ).fKenBurnsX2);
  else if (qsProperty == "kenBurns.y2")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fKenBurnsY2, &manipulator ( ).fKenBurnsY2);
  //	else if (qsProperty == "res.x")
  //		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).iXRes, &manipulator ( ).iXRes);
  //	else if (qsProperty == "res.y")
  //		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).iYRes, &manipulator ( ).iYRes);
  //	else if (qsProperty == "aspectRatio")
  //		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fAspectRatio, &manipulator ( ).fAspectRatio);
  else if (qsProperty == "blendColor.red")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fRed, &manipulator ( ).fRed);
  else if (qsProperty == "blendColor.green")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fGreen, &manipulator ( ).fGreen);
  else if (qsProperty == "blendColor.blue")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fBlue, &manipulator ( ).fBlue);
  else if (qsProperty == "brightness")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, manipulator ( ).fBrightness, &manipulator ( ).fBrightness);
  else if (qsProperty == "fileName")
    pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, m_qsFileName, &m_qsFileName);

  return pAnimAttr;
}
