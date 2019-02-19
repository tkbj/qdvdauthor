/***************************************************************************
    imagepreview.cpp
                             -------------------
    ImagePreview class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class is the encapsulation of the ImagePreview from the FileSelection
    Dialog.
    It is also used in the ImageDialog.
    
****************************************************************************/

#include <qlabel.h>
#include <qimage.h>
#include <qmatrix.h>
#include <qpixmap.h>
#include <qinputdialog.h>
//#include <q3popupmenu.h>  //ooo
#include <QMenu>    //xxx
#include <qpainter.h>
#include <qcursor.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMessageBox>		//xxx

#include "imagepreview.h"
#include "dialogimage.h"	//ooo
//#include "dialogimage_xxx.h"	//xxx

#ifdef QDVD_SLIDESHOW
#include "modifiers2.h"
#else
#include "../modifiers.h"
#endif

//ImagePreview::ImagePreview (QWidget *pParent, const char * name, Qt::WFlags f )       //ooo
ImagePreview::ImagePreview (QWidget *pParent, const char * name, Qt::WindowFlags f )    //xxx
	//: QLabel (pParent, name, f) // |WRepaintNoErase) //ooo
    : QLabel (pParent) // |WRepaintNoErase) //xxx
{
	//setScaledContents( TRUE );   //ooo
    setScaledContents( true );  //xxx
	m_fStartZoom         = 1.0;
	m_bFastPreview       = false; 
	m_pImage             = NULL;
	m_bActiveCursor      = false;
	m_bActiveZoom        = false;
	m_bNeedToAdjustColor = false;
	m_bShowBackground    = false;
	m_iOriginalWidth     = -1;
	m_iOriginalHeight    = -1;

	if ( pParent )
	  show();
}

ImagePreview::~ImagePreview ()
{

}

void ImagePreview::setResolution (int iXRes, int iYRes)
{
	// Okay for tomorrow ...
	// resolution is the size of the image, whereas
	// aspect ratio is the ratio to be displayed.
	// Thus resize should follow aspect ratio and NOT resolutions.

	// aspect ratio, E.g. letterbox 4:3, widescreen 16:4 etc.
	m_manipulator.iXRes = iXRes;
	m_manipulator.iYRes = iYRes;
	float fAspectRatio = (float)((float) iYRes/(float)iXRes);
	resize (width(), (int)(width () * fAspectRatio));
}

void ImagePreview::setBackground ( QString &fileName, bool bRefresh )
{
  m_backgroundFileName = fileName;
  QPixmap background;
  m_bShowBackground = !(fileName.isEmpty());
  if ( (m_bShowBackground) && (!m_bFastPreview) )	{
    QImage theImage (m_backgroundFileName);
    // Load image and check health of image.
    if (theImage.isNull ())	{
//    background = QPixmap (pixmap()->width(),pixmap()->height());
      background = QPixmap (width(), height());
      background.fill (QColor (0, 0, 0));
    }
    else
      //background.convertFromImage(theImage.smoothScale(width(), height()));   //ooo
      background.convertFromImage(theImage.scaled(width(), height()));  //xxx
// Crashes when no pixmap yet set ...
//    background.convertFromImage(theImage.smoothScale(pixmap()->width(),pixmap()->height()));
    setPixmap (background);
  }
  else	{	// no background image, lets create a black, empty background ...
    //		background = QPixmap (pixmap()->width(),pixmap()->height());
    background = QPixmap (width(), height());
    background.fill (QColor (0, 0, 0));
  }
  //m_background = background.convertToImage(); //ooo
  m_background = background.toImage();  //xxx
  setFixedSize(m_background.width(), m_background.height());
  // Before we go on we should calculate the correct m_iStartX, and m_iStartY
  if ( (m_manipulator.fStartX == 0.0 ) && (m_manipulator.fStartY == 0.0) ) {
    if (m_background.width () > m_image.width ( ) )
      m_manipulator.fStartX = ((m_background.width()-m_image.width())/2.0)/m_background.width();
    if (m_background.height() > m_image.height ( ) )
      m_manipulator.fStartY = ((m_background.height()-m_image.height())/2.0)/m_background.height();
  }
  if (bRefresh)
    rotate (0.0);
}

void ImagePreview::setBackground (QPixmap &pix, bool bRefresh)
{
	// Here we set the background.
	if (!pix.isNull())	{
		//m_background = pix.convertToImage();    //ooo
        m_background = pix.toImage();   //xxx
		setPixmap (pix);
	}
	else	{	// no background image, lets create a black, empty background ...
		QPixmap background;
		background = QPixmap (pixmap()->width(),pixmap()->height());
		background.fill (QColor (0, 0, 0));
		//m_background = background.convertToImage(); //ooo
        m_background = background.toImage();    //xxx
		setPixmap (background);
	}
	setFixedSize(m_background.width(), m_background.height());
	if (bRefresh)
		rotate (0.0);
}

void ImagePreview::setImage (QPixmap &theImage, ImageManipulator *pManipulator)
{
	//m_image = theImage.convertToImage(); //ooo
    m_image = theImage.toImage();   //xxx
	if ( pManipulator ) {  // This is called when we want to render the 
	  // image (rotate, zoom etc) before we deliver it to dvd-slideshow.
	  m_pImage = NULL; // To make sure ...
	  m_manipulator = *pManipulator;  
	  adjustColor ();
	  refreshPreview ();
	  return;
	}
	rotate (0.0, false);
}

void ImagePreview::resetImage ( )
{
  m_pImage = NULL;
}

void ImagePreview::setImage (CXmlSlideshow::img_struct *pImg)
{
  // before we change the image, we should connect the current 
  // ImageManipulator to the current image struct.
  getModifier ( );	//ooo
  m_pImage = pImg;
  setModifier ( );	// get the additional infos for this image
  preloadImage( );
  adjustColor ( );
  centerImage ( );
  
  if ( ! pImg->src.isEmpty ( ) )
    rotate ( 0.0, false );
  
}

CXmlSlideshow::img_struct *ImagePreview::getImage()
{
	return m_pImage;
}

void ImagePreview::mousePressEvent (QMouseEvent *pEvent)
{
	if ( ( pEvent->button  ( ) == Qt::MidButton  ) || // If the midle button was pressed
	     ( ( pEvent->button  ( ) == Qt::RightButton ) && ( m_bActiveCursor ) ) )  { // or first the left and second the right button ...
		m_currentMousePos = pEvent->pos ( );
		m_bActiveCursor   = false;
		m_bActiveZoom     = true;
		setFastPreview    ( true );
		return;
	}
	else if ( pEvent->button ( ) == Qt::LeftButton )  {
		m_currentMousePos = pEvent->pos  ( );
		m_bActiveCursor   = true;
		setFastPreview    ( true );
		return;
	}
	if (m_pImage == NULL)
		return;
	QMenu *pMenu = new QMenu (this);
	/*pMenu->insertItem ( tr ("Rotate 90") , this, SLOT(slotRotate90()));
	pMenu->insertItem ( tr ("Rotate 180"), this, SLOT(slotRotate180()));
	pMenu->insertItem ( tr ("Rotate 270"), this, SLOT(slotRotate270()));
	pMenu->insertItem ( tr ("Rotate Free ..."), this, SLOT(slotRotateFree()));
	pMenu->insertSeparator ();
	pMenu->insertItem ( tr ("Edit Image..."), this, SLOT(slotEditImage()));
	pMenu->exec(pEvent->globalPos());*/    //oooo
}

void ImagePreview::mouseReleaseEvent (QMouseEvent *)
{
	m_bActiveCursor = false;
	m_bActiveZoom   = false;
	setFastPreview  ( false );
	refreshPreview  ( );
}

void ImagePreview::mouseMoveEvent (QMouseEvent *pEvent)
{ 
	static bool bActive = false;
	if ( bActive )
		return;
	// this one we move the object regardless of the cursor beeing in or out.
	bActive = true;
	if ( m_bActiveZoom )	{
		m_manipulator.fZoom -= (float)( pEvent->pos ( ).y ( ) - m_currentMousePos.y ( ) ) / height ( );
		m_currentMousePos = pEvent->pos ( );
		refreshPreview ( );
	}
	else if (m_bActiveCursor)	{
		m_manipulator.fStartX += (float)(pEvent->pos().x() - m_currentMousePos.x())/width ();
		m_manipulator.fStartY += (float)(pEvent->pos().y() - m_currentMousePos.y())/height();
		m_currentMousePos = pEvent->pos ( );
		refreshPreview ( );
	}
	bActive = false;
}

void ImagePreview::mouseDoubleClickEvent (QMouseEvent *)
{
	if (m_pImage == NULL)
		return;
	slotEditImage();
}

void ImagePreview::resizeEvent (QResizeEvent *p)
{
	// Here we make sure the aspect ratio is taken care of.
	QLabel::resizeEvent ( p );
}

void ImagePreview::rotate (float fRot, bool bClear)
{
	// sanity check ...
	if (!m_pImage)
		return;
	if (!pixmap())
		setPixmap(QPixmap (width(), height()));
	m_pImage->rotate += fRot;
	if (m_pImage->rotate > 360.0)
		m_pImage->rotate -= 360.0;
	if (bClear)
		m_pImage->rotate = fRot;

	if (m_imageColored.isNull())
		adjustColor();

	refreshPreview();
}

void ImagePreview::drawContents(QPainter *pPainter)
{
	//QLabel::drawContents (pPainter);	//ooo
	if (m_bFastPreview)	{
		QMatrix matrix;
		QRect startRect, boundingRect;
		float fZoom = m_manipulator.fZoom * m_fStartZoom;
		// Next we move the Image but keep the moving point centered.
		matrix.translate ((int)(m_manipulator.fStartX * pPainter->window().width ())+(int)((float)m_image.width ()/2.0*fZoom), 
				  (int)(m_manipulator.fStartY * pPainter->window().height())+(int)((float)m_image.height()/2.0*fZoom) );

		matrix.scale (fZoom, fZoom);
		matrix.rotate(m_pImage->rotate);
		matrix.shear( m_manipulator.fShearX, m_manipulator.fShearY);
		// Fill in the rectangles to be drawn ...
		startRect = QRect((int)-(m_image.width()/ 2.0), (int)-(m_image.height()/2.0), m_image.width(), m_image.height());
		boundingRect = matrix.mapRect(startRect);

		QPen thePen (QColor (255,0,0), 2, Qt::DashDotLine);
		pPainter->setPen(thePen);
 
		// Okay everything is set up for some drawing action ...
		pPainter->drawRect(boundingRect);
		pPainter->setWorldMatrix (matrix);
		pPainter->drawRect(startRect);
	} 
}

void ImagePreview::refreshPreview ()
{ 
	if (m_imageColored.isNull())
		return;
	if (m_bFastPreview)	{
		update();
		return;
	}
	if (m_background.isNull())
		return;
	// If the image is completely transparent we can simply exit here ... N'est pas ???
	if (m_manipulator.fTransparency == 1.0)
		return;

	int iTargetStartX, iTargetStartY, iSourceStartX, iSourceStartY, iSourceWidth, iSourceHeight;
	QRect startRect, boundingRect;
	float fZoom = m_manipulator.fZoom * m_fStartZoom;
	QMatrix matrix;
	QPixmap background;
	QImage theImage;

	/*if (m_manipulator.iTransformationMode == 1)
		matrix.setTransformationMode(QMatrix::Areas);*/		//ooo
	matrix.translate ((int)(m_manipulator.fStartX * m_background.width ())+(int)((float)m_image.width ()/2.0*fZoom), 
			  (int)(m_manipulator.fStartY * m_background.height())+(int)((float)m_image.height()/2.0*fZoom) );
//	matrix.translate (m_manipulator.iStartX, m_manipulator.iStartY);
	matrix.scale (fZoom, fZoom);
	if ( m_pImage )
	  matrix.rotate ( m_pImage->rotate );
	else
	  matrix.rotate ( m_manipulator.fRotate );
	matrix.shear (m_manipulator.fShearX, m_manipulator.fShearY);

	// First we obtain a background to write to ...
	if ( m_bShowBackground )
		background.convertFromImage(m_background);
	else	{
		background = QPixmap (m_background.width(), m_background.height());
		background.fill (QColor (0, 0, 0));
	}
	// Now we twist and twitch the actual interesting image ...
	// Note: we only need a QImage because we need to get access to the pixel, which we can't with a Pixmap.
	//theImage = m_imageColored.xForm ( matrix );  //ooo
	theImage = m_imageColored.transformed ( matrix );  //xxx

	// We need the bounding Rect for copiing purposes, so first create the original rectangle
	startRect = QRect((int)-(m_image.width()/ 2.0), (int)-(m_image.height()/2.0), m_image.width(), m_image.height());
	// and then put it through the matrix transformation ...
	boundingRect = matrix.mapRect(startRect);

	int x, y;
	QRgb thePixel;
	QPainter thePainter (&background);
	iSourceHeight = theImage.height();
	iSourceWidth  = theImage.width ();
	iSourceStartX = 0;
	iSourceStartY = 0;
	iTargetStartX = boundingRect.x() + thePainter.viewport().x();
	iTargetStartY = boundingRect.y() + thePainter.viewport().y();
	
// Transparency handling ... next ...
	QColor sourceColor, targetColor, theColor;
	QRgb sourcePixel;
	QImage sourceImg;
	float fTargetTrans = 1.0 - m_manipulator.fTransparency;
	float fSourceTrans = m_manipulator.fTransparency;

	if (m_manipulator.fTransparency != 0.0)	{
		// First we generate a QImage object of the size needed from the background Image ...
		QPixmap sourcePix (iSourceWidth, iSourceHeight);
		//bitBlt (&sourcePix, 0, 0, thePainter.device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, Qt::CopyROP, true);		//oooo
		//bitBlt (&sourcePix, 0, 0, thePainter.device(), iTargetStartX, iTargetStartY, iSourceWidth, iSourceHeight, true);			//xxx
        //QPainter p( this );           //xxx
        //p.drawPixmap(0, 0, temp );  //xxx
		// To get Pixe information we need a QImage however ...
		//sourceImg = sourcePix.convertToImage(); //ooo
        sourceImg = sourcePix.toImage();    //xxx
	}

	// Okay if the rotation is any angle, then we get black background even with background image enabled.
	// Here we copy only those pixel over, which are part of the rotated image.
	for (y=0; y < iSourceHeight; y++)	{
		for (x=0; x < iSourceWidth; x++)	{
			thePixel = theImage.pixel(x+iSourceStartX, y+iSourceStartY);
			thePixel &= 0x00ffffff;	// filter out the transparency part
			if (thePixel != 0x000000)	{
				if (m_manipulator.fTransparency != 0.0)	{
					sourcePixel = sourceImg.pixel(x, y);
					sourceColor = QColor (sourcePixel);
					targetColor = QColor ( thePixel );
					theColor = QColor (
						(int)(fSourceTrans * sourceColor.red  () + (int)(fTargetTrans * targetColor.red  ())),
						(int)(fSourceTrans * sourceColor.green() + (int)(fTargetTrans * targetColor.green())),
						(int)(fSourceTrans * sourceColor.blue () + (int)(fTargetTrans * targetColor.blue ()))
					);
					thePainter.setPen ( theColor );
				}
				else
					thePainter.setPen ( QColor ( thePixel ) );
				thePainter.drawPoint(x+iTargetStartX, y+iTargetStartY);
			}
		}
	}
	setPixmap (background);
}

void ImagePreview::adjustColor()
{
	// m_image is the input,
	// m_imageColored is the output ...
	unsigned char iRed, iGreen, iBlue;
	int x, y;
	bool bLighter = true;
	float fAdjust;
	QColor theColor;
	QRgb thePix;	// QRgb defined as :  0xAARRGGBB;
	// deep copy of the image.
	if ( m_image.isNull  ( ) )
		preloadImage ( );
	// create empty image.
	//m_imageColored = QImage (m_image.width(), m_image.height(), m_image.depth());    //ooo
    m_imageColored = QImage (m_image.width(), m_image.height(), QImage::Format_RGB32);  //xxx

	if (m_manipulator.fBrightness > 0.0)	{
		bLighter = true;
		fAdjust = 100 + m_manipulator.fBrightness * 300;
	}
	else	{
		bLighter = false;
		fAdjust = 100 + m_manipulator.fBrightness * -300;
	}
	for ( y=0; y<m_image.height ( ); y++ )	{
		for ( x=0; x<m_image.width ( ); x++ )	{
			thePix = m_image.pixel ( x, y );
			iRed   = (unsigned char)( (float)qRed  (thePix) * m_manipulator.fRed   );
			iBlue  = (unsigned char)( (float)qBlue (thePix) * m_manipulator.fBlue  );
			iGreen = (unsigned char)( (float)qGreen(thePix) * m_manipulator.fGreen );

			theColor = QColor ( iRed, iGreen, iBlue );
			if (bLighter)
				theColor = theColor.light ( (int)fAdjust );
			else
				theColor = theColor.dark  ( (int)fAdjust );
			// This test is to mask out the transparency color ( 0, 0, 0 )
			if ( theColor == QColor ( 0, 0, 0 ) )
				theColor = QColor  ( 0, 0, 1 );
			m_imageColored.setPixel ( x, y, theColor.rgb ( ) );
		}
	}
	m_bNeedToAdjustColor = false;
}

void ImagePreview::centerImage ( )
{
	if ( m_image.isNull ( ) )
		return;

	if ( ( m_manipulator.fStartX != 0.0f ) || ( m_manipulator.fStartY != 0.0f ) )
		return;

	m_manipulator.fStartX = ((width () - m_image.width ()) / 2.0 )/width ();
	m_manipulator.fStartY = ((height() - m_image.height()) / 2.0 )/height();
}

void ImagePreview::preloadImage ( )
{
	QFileInfo fileInfo ( m_pImage->src ); 
	if ( ( m_pImage->src.isEmpty ( ) ) || 
	     ( ! fileInfo.exists     ( ) ) )  {
		//m_image = QImage::fromMimeSource ("error.jpg");	//ooo
		m_image = QImage (":/images/error.jpg");		//xxx
	}
	else
		m_image = QImage  ( m_pImage->src );
	m_iOriginalWidth  = m_image.width  ( );
	m_iOriginalHeight = m_image.height ( );
//	int iWidth  = m_image.width ();
	// adjust colors by scaling down to a manageable level.
	if (m_bFastPreview)
		//m_image = m_image.scale((int)(width()/4.0), (int)(height()/4.0), Qt::KeepAspectRatio);	//ooo
		m_image = m_image.scaled((int)(width()/4.0), (int)(height()/4.0), Qt::KeepAspectRatio);		//xxx
	else
		//m_image = m_image.smoothScale(width(), height(), Qt::KeepAspectRatio);			//ooo
		m_image = m_image.scaled(width(), height(), Qt::KeepAspectRatio);				//xxx
        
	// And here we create the right m_coloredImage;
//	m_manipulator.fZoom = (float)((float) m_image.width () / iWidth);
//	m_fStartZoom = (float)((float) m_image.width () / iWidth);
//	adjustColor ();
}

void ImagePreview::slotRotate90 ()
{
	rotate(90.0);
}

void ImagePreview::slotRotate180 ()
{
	rotate (180.0);
}

void ImagePreview::slotRotate270 ()
{
	rotate (270.0);
}

void ImagePreview::slotRotateFree ()
{
	bool bOk = 0;
	//double fRotate = QInputDialog::getDouble (tr("Please type the rotation angle."), tr ("Rotate : "), 0.0, -360.0, 360.0, 1, &bOk); //ooo
    double fRotate = QInputDialog::getDouble (this, tr("Please type the rotation angle."), tr ("Rotate : "), 0.0, -360.0, 360.0, 1, &bOk);  //xxx
	if (!bOk)
		return;
	rotate (fRotate, true);
}
void ImagePreview::slotEditImage ()
{
	QString qsEmptyString;
	DialogImage *pDialog = new DialogImage (this);
	if (m_bShowBackground)
		pDialog->initMe(m_pImage, m_backgroundFileName);
	else
		pDialog->initMe(m_pImage, qsEmptyString);

	connect (pDialog, SIGNAL (signalDone(void *)), this, SLOT(slotImageDialogDone (void *)));  //oooo
}

void ImagePreview::slotImageDialogDone (void *)
{
	// The dialog might have changed the image, we should get it from there.
//	DialogImage *pDialog = (DialogImage *) pDlg;
//	setPixmap (pDialog->getPreview()->getPixmap (iWidth, iHeight));
//printf ("pModifier=<%x><%x>\n", pDialog->getPreview()->getImage()->pModifier, m_pImage->pModifier);
	setModifier ();
	adjustColor();
	rotate (0.0);
}

void ImagePreview::setStartX (int iStartX)
{
  m_manipulator.fStartX = (float)iStartX/width();
  refreshPreview();
}
void ImagePreview::setStartY (int iStartY)
{
  m_manipulator.fStartY = (float)iStartY/height();
  refreshPreview();
}
void ImagePreview::setShearX (float fShearX)
{
	m_manipulator.fShearX = fShearX;
	refreshPreview();
}
void ImagePreview::setShearY (float fShearY)
{
	m_manipulator.fShearY = fShearY;
	refreshPreview();
}
void ImagePreview::setAspectRatio (float fAspectRatio)
{
	m_manipulator.fAspectRatio = fAspectRatio;
	resize (width(), (int)(width () * fAspectRatio));
	refreshPreview();
}

void ImagePreview::setZoom (float fZoom)
{
	m_manipulator.fZoom = fZoom;
	refreshPreview();
}

void ImagePreview::setStartZoom (float fZoom, bool bAdjust)
{
  // m_image is scaled to be 
  if ( bAdjust && m_iOriginalWidth > -1 ) // coming from ImageDialog, we ought to adjust it to original image size ratio
    m_fStartZoom = fZoom * (double) m_iOriginalHeight / (double) height ( );
  //    m_fStartZoom = fZoom * (double) m_iOriginalWidth / (double) width ( );
  else
    m_fStartZoom = fZoom;
  //printf ( "ImagePreview::setStartZoom fZoom<%f> bAdjust<%s> startZoom<%f>\n", fZoom, bAdjust ? "true" : "false", m_fStartZoom );
  refreshPreview();
}

void ImagePreview::setRed (float fRed)
{
	m_manipulator.fRed = fRed;
	m_bNeedToAdjustColor = true;
}
void ImagePreview::setGreen (float fGreen)
{
	m_manipulator.fGreen = fGreen;
	m_bNeedToAdjustColor = true;
}
void ImagePreview::setBlue (float fBlue)
{
	m_manipulator.fBlue = fBlue;
	m_bNeedToAdjustColor = true;
}
void ImagePreview::setBrightness (float fBrightness)
{
	m_manipulator.fBrightness = fBrightness;
	m_bNeedToAdjustColor = true;
}
void ImagePreview::setTransparency (float fTransparency)
{
	m_manipulator.fTransparency = fTransparency;
	m_bNeedToAdjustColor = true;
}

void ImagePreview::setRotation (float fRotation)
{
	m_pImage->rotate = fRotation;
	refreshPreview();
}

void ImagePreview::setTransformationMode (int iTransformationMode)
{
	m_manipulator.iTransformationMode = iTransformationMode;
	refreshPreview();
}

void ImagePreview::toggleBackground (bool bToggle)
{
	m_bShowBackground = bToggle;
	refreshPreview();
}

void ImagePreview::setFastPreview (bool bFast)
{
	m_bFastPreview = bFast;
//	preloadImage ();
	QCursor myCursor (Qt::WaitCursor);
	setCursor(myCursor);
	if (m_bNeedToAdjustColor)
		adjustColor();
	refreshPreview();
	//myCursor = QCursor(QCursor::ArrowCursor);	//ooo
	myCursor = QCursor(Qt::ArrowCursor);		//xxx
	setCursor(myCursor);
	// If the user releases the slider bar then we have two big images, of which we only need the colored one ...
//	if (!bFast)
//		m_image = QImage ();
}

void ImagePreview::setModifier ()
{
	// This function is coping all neccesary information from the ImageManipulator
	// class into this object.
	if (!m_pImage)
		return;
	// Check if there are modifiers at all
	if (!m_pImage->pModifier) {
	  ImageManipulator initialManipulator;
	  m_manipulator  = initialManipulator;
	  // removed bc the QSlideshows filter Dlg returned rotate but did not set modifiers.
//	  m_pImage->rotate = 0.0;
	  return;
	}
	ImageManipulator *pManipulator = (ImageManipulator *)m_pImage->pModifier;
	// Okay, there were some changes, so we should take care of them.
	m_manipulator    = *pManipulator;
	m_pImage->rotate =  pManipulator->fRotate;

	// only if there is a background file name specified,
	// otherwise keep the global background image (if any)
	if (!pManipulator->backgroundFileName.isEmpty())	{
		m_backgroundFileName  = pManipulator->backgroundFileName;
		m_bShowBackground     = pManipulator->bShowBackground;
	}
}

void ImagePreview::getModifier ()
{
	// This function is coping all neccesary information into the ImageManipulator
	// class and attaches them to img_struct (time_object) of this image.
	if (!m_pImage)
		return;
	ImageManipulator *pManipulator;
	pManipulator = (ImageManipulator *)m_pImage->pModifier;
	if (pManipulator)
		delete pManipulator;
	m_pImage->pModifier = NULL;

	// We don't need such a modifier if nothing has changed ...
	if ( m_manipulator.isUnchanged () )
	    return;
	
	// Okay, there were some changes, so we should take care of them.
	pManipulator = new ImageManipulator;
	*pManipulator = m_manipulator;
	pManipulator->fZoom = m_manipulator.fZoom * m_fStartZoom;
	pManipulator->fRotate = m_pImage->rotate;
	m_pImage->pModifier = pManipulator;
}

QPixmap &ImagePreview::getPixmap (int iWidth, int iHeight)
{
	// Here we return the whole Image including background and ImageObject.
	// first the easy way ... if teh size is the same (or 0 is defined).
	if	( ( (iWidth == 0) && (iHeight == 0) )	||
		  ( (iWidth == pixmap()->width()) && (iHeight == pixmap()->height() ) )
		)	{
		m_pixmap = *pixmap();
	}
	else	{
		//QImage theImage = pixmap ()->convertToImage();  //ooo
        QImage theImage = pixmap ()->toImage(); //xxx
		//theImage = theImage.smoothScale (iWidth, iHeight);  //ooo
        theImage = theImage.scaled (iWidth, iHeight);   //xxx
		m_pixmap.convertFromImage(theImage);
	}
	return m_pixmap;
}

QPixmap &ImagePreview::getObject()
{
	float fZoom = m_manipulator.fZoom * m_fStartZoom;
	QMatrix matrix;
	QImage theImage;

	if (m_manipulator.iTransformationMode == 1)
		//matrix.setTransformationMode(QMatrix::Areas);		//ooo
	matrix.translate ((int)(m_manipulator.fStartX*width())+(int)((float)m_image.width()/2.0), (int)(m_manipulator.fStartY*height())+(int)((float)m_image.height()/2.0));
	matrix.scale (fZoom, fZoom);
	matrix.rotate(m_pImage->rotate);
	matrix.shear (m_manipulator.fShearX, m_manipulator.fShearY);

	// Now we twist and twitch the actual interesting image ...
	// Note: we only need a QImage because we need to get access to the pixel, which we can't with a Pixmap.
	//theImage = m_imageColored.xForm ( matrix );  //ooo
    theImage = m_imageColored.transformed ( matrix );   //xxx
	m_pixmap.convertFromImage(theImage); 
	return m_pixmap;
}

QRect ImagePreview::getRect()
{
	QRect theRect;
	float fZoom = m_manipulator.fZoom * m_fStartZoom;
	theRect = QRect((int)(m_manipulator.fStartX*width()), (int)(m_manipulator.fStartY*height()),
		(int)(fZoom*m_image.width()), (int)(fZoom*m_image.height()));
	return theRect;
}

 
