/***************************************************************************
    imageline.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Here we start the class ImageLine.
    This class handles the generation, and display of the images.
    
****************************************************************************/

#include <qmatrix.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpainter.h>
//#include <q3popupmenu.h>  //ooo
#include <QMenu>       //xxx
#include <qinputdialog.h>
//Added by qt3to4:
//#include <QCustomEvent>   //ooo
#include <QEvent>           //xxx
#include <QFrame>
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>

#include "imageline.h"
#include "dialogimage.h"		//ooo

static const char* const image0_data[] = {
"21 10 2 1",
". c #000000",
"# c #ffffff",
".....................",
".....................",
".....................",
".####...####...####..",
".####...####...####..",
".####...####...####..",
".####...####...####..",
".....................",
".....................",
"....................."};


//ImageLine::ImageLine(QWidget * parent, const char * name, Qt::WFlags f)   //ooo
ImageLine::ImageLine(QWidget * parent, const char * name, Qt::WindowFlags f)    //xxx
    //: Q3Frame( parent, name, f )  //ooo
    : QFrame( parent )  //xxx
    , m_Image( (const char **) image0_data )
{
	//setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );  //oooo
	setMinimumSize( QSize( 600, 122 ) );
	setMaximumSize( QSize( 32767, 122 ) );
	//setPaletteBackgroundColor( QColor( 0, 0, 0 ) );  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor( 0, 0, 0 ));			//xxx
	this->setPalette(palette);					//xxx
	setFrameShape( QFrame::StyledPanel );
	setFrameShadow( QFrame::Raised );

	//m_pPixmapLabel1 = new QLabel( this, "pixmapLabel1" );    //ooo
    m_pPixmapLabel1 = new QLabel( "pixmapLabel1", this );   //xxx
	m_pPixmapLabel1->setGeometry( QRect( 0, 0, 800, FRAME_HEIGHT ) );
	//m_pPixmapLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pPixmapLabel1->sizePolicy().hasHeightForWidth() ) );    //oooo
	m_pPixmapLabel1->setMaximumSize( QSize( 32767, FRAME_HEIGHT ) );
	m_pPixmapLabel1->setScaledContents         ( true );		//xxx	
	m_pPixmapLabel1->setAutoFillBackground     ( true );		//xxx
	//m_pPixmapLabel1->setPaletteBackgroundPixmap( m_Image );	//ooo
	QPalette palette2;						//xxx
	palette2.setBrush(QPalette::Window, QBrush(m_Image));		//xxx
	m_pPixmapLabel1->setPalette(palette2);				//xxx
	m_pPixmapLabel1->setLineWidth( 0 );
	//m_pPixmapLabel1->setScaledContents( FALSE ); //ooo
    m_pPixmapLabel1->setScaledContents( false );    //xxx

	//m_pImageLabel = new QLabel( this, "imageLabel" );    //ooo
    m_pImageLabel = new QLabel( "imageLabel", this );   //xxx
	m_pImageLabel->setGeometry( QRect( 0, FRAME_HEIGHT+1, 800, height()-2*FRAME_HEIGHT) );
	//m_pImageLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pImageLabel->sizePolicy().hasHeightForWidth() ) );    //oooo
	m_pImageLabel->setMaximumSize( QSize( 32767, 122 - 2*FRAME_HEIGHT ) );
	m_pImageLabel->setLineWidth( 0 );
	//m_pImageLabel->setPaletteBackgroundColor( QColor( 0, 0, 0 ) );   //ooo
	QPalette palette3;	//xxx
	palette3.setBrush(m_pImageLabel->backgroundRole(), QColor( 0, 0, 0 ));	//xxx
	m_pImageLabel->setPalette(palette3);						//xxx
	//m_pImageLabel->setScaledContents( FALSE );   //ooo
    m_pImageLabel->setScaledContents( false );  //xxx

	//m_pPixmapLabel2 = new QLabel( this, "pixmapLabel2" );    //ooo
    m_pPixmapLabel2 = new QLabel( "pixmapLabel2", this );   //xxx
	m_pPixmapLabel2->setGeometry( QRect( 0, 110, 800, FRAME_HEIGHT ) );
	//m_pPixmapLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_pPixmapLabel2->sizePolicy().hasHeightForWidth() ) );    //oooo
	m_pPixmapLabel2->setMaximumSize( QSize( 32767, FRAME_HEIGHT ) );
	m_pPixmapLabel2->setScaledContents         ( true );		//xxx	
	m_pPixmapLabel2->setAutoFillBackground     ( true );		//xxx
	//m_pPixmapLabel2->setPaletteBackgroundPixmap( m_Image );	//ooo
	QPalette palette4;						//xxx
	palette4.setBrush(QPalette::Window, QBrush(m_Image));		//xxx
	m_pPixmapLabel2->setPalette(palette4);				//xxx
	m_pPixmapLabel2->setLineWidth( 0 );
	//m_pPixmapLabel2->setScaledContents( FALSE ); //ooo
    m_pPixmapLabel2->setScaledContents( false );    //xxx
	m_pSlideshow = NULL;
	initMe ();
}

void ImageLine::initImages (CXmlSlideshow *pSlideshow) //QString &xmlFileName)
{
	// This function only reads in the m_slideShow - file.
	m_pSlideshow = pSlideshow;
//	m_pSlideshow->readXml(xmlFileName);
}

void ImageLine::mousePressEvent (QMouseEvent *pEvent)
{
	// check wether we are still producing thumbnails and the user was unpatient.
	if (m_bCreatingThumbnails)
		return;

	int iImage;
	// First we check if the user clicked with the left mouse button
	// we don't handle this.
	if (pEvent->button () == Qt::LeftButton)
		return;
	// Here we get the ObjectNumber of the Image the user clicked on (or -1 if not an image).
	iImage = getImageByPos (pEvent->x());
	if (iImage < 0)
		return;
	// m_iHotObject marks the ImageObject for the callbacks of the following drop down menu.
	m_iHotObject = iImage;
	QMenu *pMenu = new QMenu (this);
/*	pMenu->insertItem ( tr ("Rotate 90") , this, SLOT(slotRotate90()));
	pMenu->insertItem ( tr ("Rotate 180"), this, SLOT(slotRotate180()));
	pMenu->insertItem ( tr ("Rotate 270"), this, SLOT(slotRotate270()));
	pMenu->insertItem ( tr ("Rotate Free ..."), this, SLOT(slotRotateFree()));
	pMenu->insertSeparator ();
	pMenu->insertItem ( tr ("Edit Image ..."), this, SLOT(slotEditImage()));*/ //oooo
	/**
	 * F.J. Cruz - neke
	 *18/10/04
	 **/
	/*pMenu->insertSeparator();
	pMenu->insertItem( tr ("Add Subtittle..."), this, SLOT(slotInsertSubtittle()));
	
	pMenu->exec(pEvent->globalPos());*/    //oooo
}

void ImageLine::mouseDoubleClickEvent (QMouseEvent *pEvent)
{
	// check wether we are still producing thumbnails and the user was unpatient.
	if (m_bCreatingThumbnails)
		return;

	// Here we have to extract the information from the mouse position ...
	int iImage = getImageByPos (pEvent->x());
	if (iImage < 0)
		return;
	m_iHotObject = iImage;
	slotEditImage();
}

int ImageLine::getImageByPos (int iMouseX)
{
	uint t = 0;
	uint iTotalX = 0;
	// get the object of the current cursor position.
	while ((iTotalX < (uint)iMouseX) && (t < (uint)m_pCurrentRange->listX.count()))	{	//ox
		iTotalX += m_pCurrentRange->listX[t];
		t++;
	}
	// adjustment to the global image number ...
	t +=  m_pCurrentRange->iStartObject - 1;
	if (m_pSlideshow->getTimeObject(t)->node_name != "img")
		return -1;
	return t;
}
/**
 * F.J.Cruz
 * 18/10/04
 **/
void ImageLine::slotInsertSubtittle()
{
	CXmlSlideshow::time_object *pTimeObject = m_pSlideshow->getTimeObject(m_iHotObject);
	if (pTimeObject->node_name != "img")
		return;
	CXmlSlideshow::filter_struct *pImage = (CXmlSlideshow::filter_struct *)pTimeObject;
	//pImage->subtitle = QInputDialog::getText (tr("Please type the subtitle."), tr ("Subtitle : "), QLineEdit::Normal, tr("My Subtitle"));    //ooo
    pImage->subtitle = QInputDialog::getText (this, tr("Please type the subtitle."), tr ("Subtitle : "), QLineEdit::Normal, tr("My Subtitle")); //xxx
}

void ImageLine::rotate (float fRot, bool bClear)
{
	CXmlSlideshow::time_object *pTimeObject = m_pSlideshow->getTimeObject (m_iHotObject);
	if (pTimeObject->node_name != "img")
		return;
	// Okay, we are safe to cast the pointer to the right struct
	CXmlSlideshow::img_struct *pImage = (CXmlSlideshow::img_struct *)pTimeObject;

	pImage->rotate += fRot;
	if (pImage->rotate > 360.0)
		pImage->rotate -= 360.0;
	if (bClear)
		pImage->rotate = fRot;
	// Here we load the image (again)
	QImage myImage (pImage->src);
	QPixmap theThumb;
	// The we rotate the image
	if( !myImage.isNull() )	{
		QMatrix m;
		m.rotate( pImage->rotate );
		//myImage = myImage.xForm( m );   //ooo
        myImage = myImage.transformed( m ); //xxx
	}

	// and finallly we scale it back down again.
	float fAspectRatio = (float) m_pSlideshow->xres / (float) m_pSlideshow->yres; // For now only NTSC
	float fXExtension = fAspectRatio * (height() - m_pPixmapLabel1->height() - m_pPixmapLabel2->height());
	float fTotalTime = m_pCurrentRange->fEndTime -  m_pCurrentRange->fStartTime;

	//theThumb.convertFromImage(myImage.smoothScale((int)fXExtension, m_pImageLabel->height()-6, Qt::KeepAspectRatio));    //ooo
    theThumb.convertFromImage(myImage.scaled((int)fXExtension, m_pImageLabel->height()-6, Qt::KeepAspectRatio));    //xxx
//printf ("theThumb<%d><%d> \n", theThumb.width(), theThumb.height());

	float fMiddleX = (pImage->fEndTime - pImage->fStartTime) / 2.0 + pImage->fStartTime - m_pCurrentRange->fStartTime;
	fMiddleX *= m_pCurrentRange->iXExtension / fTotalTime;
	// Here we clear the back to a black background ...
	QPainter thePainter(&m_pCurrentRange->pixmapRangeThumbs);
	QBrush theBrush (QColor(0,0,0));

	float fDelay = pTimeObject->fEndTime - pTimeObject->fStartTime;
	float fStartX = (pTimeObject->fStartTime - m_pCurrentRange->fStartTime) / fTotalTime  * (float) m_pCurrentRange->iXExtension;
	float fWidthX = fDelay / fTotalTime  * (float) m_pCurrentRange->iXExtension + 0.5;

	thePainter.fillRect((int)fStartX+1, 1, (int)fWidthX-2, m_pImageLabel->height()-6, theBrush);


	//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//ooo
	//		&theThumb, 0, 0, myImage.width(), myImage.height(), Qt::CopyROP, true );			//ooo
	//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//xxx
	//		&theThumb, 0, 0, myImage.width(), myImage.height()/*, Qt::CopyROP, true*/ );			//xxx
    QPainter p( &m_pCurrentRange->pixmapRangeThumbs );
    p.drawPixmap((int)fMiddleX-(int)((double) theThumb.width()/2.0), 1, theThumb, 0, 0, myImage.width(), myImage.height() );  //xxx
	refreshPixmap();
}

void ImageLine::slotRotate90 ()
{
	rotate(90.0);
}

void ImageLine::slotRotate180 ()
{
	rotate (180.0);
}

void ImageLine::slotRotate270 ()
{
	rotate (270.0);
}

void ImageLine::slotRotateFree ()
{
	bool bOk = 0;
	//double fRotate = QInputDialog::getDouble (tr("Please type the rotation angle."), tr ("Rotate : "), 0.0, -360.0, 360.0, 1, &bOk); //ooo
    double fRotate = QInputDialog::getDouble (this, tr("Please type the rotation angle."), tr ("Rotate : "), 0.0, -360.0, 360.0, 1, &bOk);  //xxx
	if (!bOk)
		return;
	rotate (fRotate, true);
}

void ImageLine::slotEditImage ()
{
	DialogImage *pDialog = new DialogImage (this);	//ooo
	pDialog->initMe (m_pSlideshow->getTimeObject(m_iHotObject), m_pSlideshow->background);
	pDialog->show();
	connect (pDialog, SIGNAL (signalDone(void *)), this, SLOT(slotImageDialogDone (void *)));
}

void ImageLine::slotImageDialogDone (void *pDlg)
{
	DialogImage *pDialog = (DialogImage *) pDlg;		//ooo
	CXmlSlideshow::img_struct *pImage = pDialog->getPreview()->getImage();
	QPixmap theThumb;
	// and finallly we scale it back down again.
	float fAspectRatio = (float) m_pSlideshow->xres / (float) m_pSlideshow->yres; // For now only NTSC
	float fXExtension = fAspectRatio * (height() - m_pPixmapLabel1->height() - m_pPixmapLabel2->height());
	float fTotalTime = m_pCurrentRange->fEndTime -  m_pCurrentRange->fStartTime;

	theThumb = pDialog->getPreview()->getPixmap ((int)fXExtension, m_pImageLabel->height()-6);

	float fMiddleX = (pImage->fEndTime - pImage->fStartTime) / 2.0 + pImage->fStartTime - m_pCurrentRange->fStartTime;
	fMiddleX *= m_pCurrentRange->iXExtension / fTotalTime;
	// Here we clear the back to a black background ...
	QPainter thePainter(&m_pCurrentRange->pixmapRangeThumbs);
	QBrush theBrush (QColor(0,0,0));

	float fDelay = pImage->fEndTime - pImage->fStartTime;
	float fStartX = (pImage->fStartTime - m_pCurrentRange->fStartTime) / fTotalTime  * (float) m_pCurrentRange->iXExtension;
	float fWidthX = fDelay / fTotalTime  * (float) m_pCurrentRange->iXExtension + 0.5;

	thePainter.fillRect((int)fStartX+1, 1, (int)fWidthX-2, m_pImageLabel->height()-6, theBrush);

	//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//ooo
	//		&theThumb, 0, 0, theThumb.width(), theThumb.height(), Qt::CopyROP, true );			//ooo
	//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//xxx
	//		&theThumb, 0, 0, theThumb.width(), theThumb.height() );						//xxx
    QPainter p( &m_pCurrentRange->pixmapRangeThumbs );           //xxx
    p.drawPixmap((int)fMiddleX-(int)((double) theThumb.width()/2.0), 1, theThumb, 0, 0, theThumb.width(), theThumb.height() );  //xxx
	refreshPixmap();
}

void ImageLine::mouseReleaseEvent (QMouseEvent *)
{
	//setPaletteBackgroundColor (QColor (170, 170, 255));  //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor(170, 170, 255)));	//xxx
    this->setPalette(palette);						//xxx
}

void ImageLine::setCurrentRange (DialogFilter::range_struct *pRange)
{
	if (!pRange)
		return;
	m_pCurrentRange = pRange;
	setTotalXExtension (pRange->iXExtension);
	if (pRange->bAlreadyGenerated)
		refreshPixmap();
	else
		createThumbs ();

}

void ImageLine::setTotalXExtension (uint iTotalXExtension)
{
	resize (iTotalXExtension, width());
	m_pPixmapLabel1->resize(iTotalXExtension, m_pPixmapLabel1->height());
	m_pPixmapLabel2->resize(iTotalXExtension, m_pPixmapLabel2->height());

	m_pImageLabel->resize (iTotalXExtension, m_pImageLabel->height());
}

void ImageLine::initMe()
{
	m_pCurrentRange = NULL;
	m_bSemaphore = false;
	m_bCreatingThumbnails = false;
	connect (this, SIGNAL(signalRefreshPixmap ()), this, SLOT(slotRefreshPixmap ()));
}

////////////////////////////////////////////////////////////////////////////
//
// Here we handle the new thread ...
//
////////////////////////////////////////////////////////////////////////////
ImageLine::CreateThumbs::CreateThumbs ()
{

}

ImageLine::CreateThumbs::~CreateThumbs ()
{

}

void ImageLine::CreateThumbs::initThumbs (ImageLine *pImageLine)
{
	m_pImageLine = pImageLine;
}

void ImageLine::CreateThumbs::run ()
{
	m_pImageLine->createThumbs (false);
}

void ImageLine::CreateThumbs::sync ()
{
	msleep (100);
}

#include <qapplication.h>
int giProgress;

void ImageLine::createFrames()
{
 	// If we have these thumbnails already, then set the bitmap and return;
	setTotalXExtension(m_pCurrentRange->iXExtension);
	if (m_pCurrentRange->bAlreadyGenerated)	{
		m_pImageLabel->setPixmap (m_pCurrentRange->pixmapRangeThumbs);
		return;
	}
	uint t;

	float fDelay, fGlobalDelay;
	// And here we resize the background image and create the thumb nails ...
	m_pCurrentRange->pixmapRangeThumbs = QPixmap (m_pCurrentRange->iXExtension, m_pImageLabel->height());
	CXmlSlideshow::time_object *pTimeObject;
	fGlobalDelay = (float)m_pSlideshow->delay;
	QImage theImage;
	QPixmap theThumb;
	// The X-expension per image depends on the aspect ratio.

	QPainter thePainter(&m_pCurrentRange->pixmapRangeThumbs);
	thePainter.setPen(QColor(255,0 ,0));
	thePainter.scale (1.0, 1.0);
	float fTotalTime = m_pCurrentRange->fEndTime -  m_pCurrentRange->fStartTime;

	// Because this can take quite some time, we will draw the outlines, and the filter first, and display them.
	QBrush blackBrush (QColor (0, 0, 0));
	QBrush theBrush (QColor (128, 128, 128), Qt::Dense6Pattern);
	float fStartX, fWidthX;
	m_pCurrentRange->listX.clear ();

	thePainter.fillRect (0, 0, m_pCurrentRange->pixmapRangeThumbs.width(),  m_pCurrentRange->pixmapRangeThumbs.height(), blackBrush);
	for (t=m_pCurrentRange->iStartObject;t<m_pCurrentRange->iEndObject;t++)	{
		pTimeObject = m_pSlideshow->getTimeObject (t);
		if (pTimeObject != NULL)	{
			// Here we draw the frame around the image and draw a filled rect for Filters.
			fDelay = pTimeObject->fDuration;
			if (fDelay <= 0.0f)
				fDelay = fGlobalDelay;

			fDelay = pTimeObject->fEndTime - pTimeObject->fStartTime;
			fStartX = (pTimeObject->fStartTime - m_pCurrentRange->fStartTime) / fTotalTime  * (float) m_pCurrentRange->iXExtension;
			fWidthX = fDelay                  / fTotalTime  * (float) m_pCurrentRange->iXExtension + 0.5;

			if (pTimeObject->node_name == "img")
				thePainter.drawRect((int)fStartX, 0, (int)fWidthX, m_pImageLabel->height()-4);
			else
				thePainter.fillRect((int)fStartX, 0, (int)fWidthX, m_pImageLabel->height()-4, theBrush);
			m_pCurrentRange->listX.append ((int)(fWidthX));
		}
	}
	refreshPixmap ();
}

void ImageLine::createThumbs(bool bCallThread)
{
giProgress = 0;
// error checking ...
	if (m_pCurrentRange == NULL)
		return;
	// The m_createThumbs class is derived from QThread and only calls back this function again to generate the threads
	// However without the bCallThread flag set, thus actually creating the thumbnails.
	// This way we have the full thumbnail creation in one neat algorithm rather then spread out through a coule
	// of functions.
	if (bCallThread)	{
		createFrames ();
		m_createThumbs.initThumbs (this);
		// If the thread is still running we skip this one ...
		//if (!m_createThumbs.running ()) //ooo
        if (!m_createThumbs.isRunning ())   //xxx
			m_createThumbs.start ();
		return;
	}
	// Okay and here we create the thumbnails.
	// check wether we are still producing thumbnails and the user was unpatient.
	if (m_bCreatingThumbnails)
		return;
	m_bCreatingThumbnails = true;
//	sleep (1);	// lets have a short delay.
//	m_createThumbs.sync ();
	uint t;

	float fDelay, fGlobalDelay;
	float fProgress;
	// And here we resize the background image and create the thumb nails ...
	CXmlSlideshow::img_struct *pImg;
	fGlobalDelay = (float)m_pSlideshow->delay;
	QImage theImage;
	QPixmap theThumb;
	// The X-expension per image depends on the aspect ratio.
	float fAspectRatio = (float) m_pSlideshow->xres / (float) m_pSlideshow->yres; // For now only NTSC
	float fXExtension = fAspectRatio * (height() - m_pPixmapLabel1->height() - m_pPixmapLabel2->height());

	uint iNumberOfImages = m_pCurrentRange->iEndImg - m_pCurrentRange->iStartImg;
	float fTotalTime = m_pCurrentRange->fEndTime -  m_pCurrentRange->fStartTime;

	// Here we create the thumb nails but display the result every 10 generated images ...
	float fMiddleX;
	for (t=m_pCurrentRange->iStartImg;t<m_pCurrentRange->iEndImg;t++)	{
		fProgress = ((float)(t-m_pCurrentRange->iStartImg) / (float)(iNumberOfImages-1))*MAX_PROGRESS;

//		emit (signalProgress ( (int)fProgress));
giProgress = (int)fProgress;
//QApplication::postEvent (this, new QCustomEvent(QEvent::User+2)); //ooo
QApplication::postEvent (this, new QEvent((QEvent::Type)(QEvent::User+2))); //xxx

		pImg = m_pSlideshow->getImg (t);
		if (pImg  != NULL)	{
			// Here we draw the frame around the image
			fDelay = pImg->fDuration;
			if (fDelay <= 0.0f)
				fDelay = fGlobalDelay;

			theImage.load (pImg->src);
			if( !theImage.isNull() )	{
				QMatrix m;
				m.rotate( pImg->rotate );
				//theImage = theImage.xForm( m );   //ooo
                theImage = theImage.transformed( m );   //xxx
			}

			//theThumb.convertFromImage(theImage.smoothScale((int)fXExtension, m_pImageLabel->height()-6, Qt::KeepAspectRatio)); //ooo
			theThumb.fromImage(theImage.scaled((int)fXExtension, m_pImageLabel->height()-6, Qt::KeepAspectRatio));   //xxx

			fMiddleX = (pImg->fEndTime - pImg->fStartTime) / 2.0 + pImg->fStartTime - m_pCurrentRange->fStartTime;
			fMiddleX *= m_pCurrentRange->iXExtension / fTotalTime;
			//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//ooo
			//		&theThumb, 0, 0, theThumb.width(), theThumb.height(), Qt::CopyROP, true );			//ooo
			//bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,		//xxx
			//		&theThumb, 0, 0, theThumb.width(), theThumb.height()/*, Qt::CopyROP, true*/ );			//xxx
            QPainter p( &m_pCurrentRange->pixmapRangeThumbs );           //xxx
            p.drawPixmap((int)fMiddleX-(int)((double) theThumb.width()/2.0), 1, theThumb, 0, 0, theThumb.width(), theThumb.height() );  //xxx
			if (t%10 == 5)
				refreshPixmap ();
		}
	}
	m_pCurrentRange->bAlreadyGenerated = true;
	refreshPixmap ();
	m_bCreatingThumbnails = false;
}
#include <qtimer.h>
#ifdef QDVD_LINUX 
#include <unistd.h>	// neede for sleep() only.
#endif
// The following two functions will refresh the pixmap in the main thread (ImageLine object)
void ImageLine::refreshPixmap()
{
/*
	m_pCurrentRange->tmpPixmap = m_pCurrentRange->pixmapRangeThumbs;
	m_bSemaphore = true;
	QApplication::postEvent (this, new QCustomEvent(QEvent::User+1));
	m_createThumbs.sync ();
//	while (m_bSemaphore)	{
//printf ("Go to sleep\n");
//		m_createThumbs.sync ();
//	}
return;
*/
	m_pCurrentRange->tmpPixmap = m_pCurrentRange->pixmapRangeThumbs;
	// this function is called from the createThumbs background thread and waits
	// with further execution until the main thread has finished updating the pixmap.
	m_bSemaphore = true;
	emit (signalRefreshPixmap ());
//	m_createThumbs.sync ();
//	QTimer::singleShot(1, this, SLOT(slotRefreshPixmap ()));
//	while (m_bSemaphore)	{
//printf ("Go to sleep\n");
//		m_createThumbs.sync ();
//	}
}

//void ImageLine::customEvent (QCustomEvent* pEvent) {  //ooo
void ImageLine::customEvent (QEvent* pEvent) {  //xxx
	if (pEvent)	{
		if (pEvent->type() == QEvent::User+1) {
			m_bSemaphore = true;
//			m_pImageLabel->setPixmap (m_pCurrentRange->pixmapRangeThumbs);
			m_pImageLabel->setPixmap (m_pCurrentRange->tmpPixmap);
			m_bSemaphore = false;
		}
		if (pEvent->type() == QEvent::User+2) {
			emit (signalProgress(giProgress));
		}
	}
}

void ImageLine::slotRefreshPixmap ()
{
	// This function is called when the main thread is not busy.
	// the semaphore is set(again) and after the refresh unset.
	// The unset semaphore will tell the createThumb thread to
	// continue creating thumbnails.
	m_bSemaphore = true;
	m_pImageLabel->setPixmap (m_pCurrentRange->tmpPixmap);
//	m_pImageLabel->setPixmap (m_pCurrentRange->pixmapRangeThumbs);
	m_bSemaphore = false;
}

/* createThumbs - old
//#include <unistd.h>
#include <qapplication.h>
int giProgress;
void ImageLine::createThumbs(bool bCallThread)
{
giProgress = 0;
	// The m_createThumbs class is derived from QThread and only calls back this function again to generate the threads
	// However without the bCallThread flag set, thus actually creating the thumbnails.
	// This way we have the full thumbnail creation in one neat algorithm rather then spread out through a coule
	// of functions.
	if (bCallThread)	{
		m_createThumbs.initThumbs (this);
		// If the thread is still running we skip this one ...
		if (!m_createThumbs.running ())
			m_createThumbs.start ();
		return;
	}
 // error checking ...
	if (m_pCurrentRange == NULL)
		return;
	// If we have these thumbnails already, then set the bitmap and return;
	setTotalXExtension(m_pCurrentRange->iXExtension);
	if (m_pCurrentRange->bAlreadyGenerated)	{
		m_pImageLabel->setPixmap (m_pCurrentRange->pixmapRangeThumbs);
		return;
	}
	// Okay and here we create the thumbnails.
//	m_bCreatingThumbnails = false;
	// check wether we are still producing thumbnails and the user was unpatient.
	if (m_bCreatingThumbnails)
		return;
	m_bCreatingThumbnails = true;
//	sleep (1);	// lets have a short delay.
	m_createThumbs.sync ();
	uint t;

	float fDelay, fGlobalDelay;
	float fProgress;
	// And here we resize the background image and create the thumb nails ...
	m_pCurrentRange->pixmapRangeThumbs = QPixmap (m_pCurrentRange->iXExtension, m_pImageLabel->height());
	CXmlSlideshow::img_struct *pImg;
	CXmlSlideshow::time_object *pTimeObject;
	fGlobalDelay = (float)m_pSlideshow->delay;
	QImage theImage;
	QPixmap theThumb;
	// The X-expension per image depends on the aspect ratio.
	float fAspectRatio = (float) m_pSlideshow->xres / (float) m_pSlideshow->yres; // For now only NTSC
	float fXExtension = fAspectRatio * (height() - m_pPixmapLabel1->height() - m_pPixmapLabel2->height());

	QPainter thePainter(&m_pCurrentRange->pixmapRangeThumbs);
	thePainter.setPen(QColor(255,0 ,0));
	thePainter.scale (1.0, 1.0);
	uint iNumberOfImages = m_pCurrentRange->iEndImg - m_pCurrentRange->iStartImg;
	float fTotalTime = m_pCurrentRange->fEndTime -  m_pCurrentRange->fStartTime;

	// Because this can take quite some time, we will draw the outlines, and the filter first, and display them.
	QBrush theBrush (QColor (128, 128, 128), Qt::Dense6Pattern);
	float fStartX, fWidthX;
	m_pCurrentRange->listX.clear ();

	for (t=m_pCurrentRange->iStartObject;t<m_pCurrentRange->iEndObject;t++)	{
		pTimeObject = m_pSlideshow->getTimeObject (t);
		if (pTimeObject != NULL)	{
			// Here we draw the frame around the image and draw a filled rect for Filters.
			fDelay = pTimeObject->fDuration;
			if (fDelay <= 0.0f)
				fDelay = fGlobalDelay;

			fDelay = pTimeObject->fEndTime - pTimeObject->fStartTime;
			fStartX = (pTimeObject->fStartTime - m_pCurrentRange->fStartTime) / fTotalTime  * (float) m_pCurrentRange->iXExtension;
			fWidthX = fDelay                  / fTotalTime  * (float) m_pCurrentRange->iXExtension + 0.5;

			if (pTimeObject->node_name == "img")
				thePainter.drawRect((int)fStartX, 0, (int)fWidthX, m_pImageLabel->height()-4);
			else
				thePainter.fillRect((int)fStartX, 0, (int)fWidthX, m_pImageLabel->height()-4, theBrush);
			m_pCurrentRange->listX.append ((int)(fWidthX));
		}
	}
	refreshPixmap ();
//m_bCreatingThumbnails = false;
//return;
	// Here we create the thumb nails but display the result every 10 generated images ...
	float fMiddleX;
	for (t=m_pCurrentRange->iStartImg;t<m_pCurrentRange->iEndImg;t++)	{
		fProgress = ((float)(t-m_pCurrentRange->iStartImg) / (float)(iNumberOfImages-1))*MAX_PROGRESS;

//		emit (signalProgress ( (int)fProgress));
giProgress = (int)fProgress;
QApplication::postEvent (this, new QCustomEvent(QEvent::User+2));

		pImg = m_pSlideshow->getImg (t);
		if (pImg  != NULL)	{
			// Here we draw the frame around the image
			fDelay = pImg->fDuration;
			if (fDelay <= 0.0f)
				fDelay = fGlobalDelay;

			theImage.load (pImg->src);
			if( !theImage.isNull() )	{
				QWMatrix m;
				m.rotate( pImg->rotate );
				theImage = theImage.xForm( m );
			}

			theThumb.convertFromImage(theImage.smoothScale((int)fXExtension, m_pImageLabel->height()-6, QImage::ScaleMin));

			fMiddleX = (pImg->fEndTime - pImg->fStartTime) / 2.0 + pImg->fStartTime - m_pCurrentRange->fStartTime;
			fMiddleX *= m_pCurrentRange->iXExtension / fTotalTime;
			bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,
					&theThumb, 0, 0, theThumb.width(), theThumb.height(), Qt::CopyROP, true );
//			bitBlt (&m_pCurrentRange->pixmapRangeThumbs, (int)fMiddleX-(int)((double) theThumb.width()/2.0), 1,
//					&theThumb, 0, 0, theImage.width(), theImage.height(), Qt::CopyROP, true );

			if (t%10 == 5)
				refreshPixmap ();
		}
	}
	m_pCurrentRange->bAlreadyGenerated = true;
	refreshPixmap ();
	m_bCreatingThumbnails = false;
}
*/

