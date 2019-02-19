/***************************************************************************
    imagepreview.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMouseEvent>

//#include "xml_slideshow.h"		//ooo
#include "../xml_slideshow.h"		//xxx

#define QDVD_SLIDESHOW 1	//xxx

#ifdef QDVD_SLIDESHOW
#include "modifiers2.h"
#else
#include "../modifiers.h"
#endif


class ImagePreview : public QLabel
{
	Q_OBJECT
public:
	 //ImagePreview (QWidget * parent, const char * name = 0, Qt::WFlags f = 0 );      //ooo
     ImagePreview (QWidget * parent, const char * name = 0, Qt::WindowFlags f = 0 );    //xxx
	~ImagePreview ();

	void setImage			(CXmlSlideshow::img_struct *pImg);
	void setImage			(QPixmap &, ImageManipulator *);
	void setBackground		(QString &fileName, bool bRefresh=true );
	void setBackground		(QPixmap &pix, bool bRefresh=true);
	void setResolution		(int iXres, int iYRes);
	void setAspectRatio		(float fAspectRatio);
	void toggleBackground		(bool bToggle);
	void setFastPreview		(bool bToggle);
	void setTransformationMode	(int);

	void resetImage         ();
	void refreshPreview	();

	void setStartX		(int);
	void setStartY		(int);
	void setShearX		(float);
	void setShearY		(float);
	void setZoom		(float);
	void setStartZoom	(float, bool b=false);
	void setRed 		(float);
	void setGreen		(float);
	void setBlue		(float);
	void setBrightness	(float);
	void setTransparency	(float);
	void setRotation	(float);
	void setModifier 	();
	void getModifier 	();
	
	CXmlSlideshow::img_struct *	getImage ();	// returns trh img_struct - object
	QPixmap &getPixmap(int iWidth=0, int iHeight=0);// retrurns a QPixmap of the specified size
	QPixmap &getObject();	// returns only the rotated and colored Object, not the background
	QRect    getRect();	// returns the rect size of the Image. Needed in case the image was rescaled.

public slots:	// for testing only ...
	virtual void slotEditImage();
	virtual void slotImageDialogDone (void *pDlg);

protected slots:
	virtual void slotRotate90  ();
	virtual void slotRotate180 ();
	virtual void slotRotate270 ();
	virtual void slotRotateFree();

protected:
	virtual void resizeEvent		(QResizeEvent *);
	virtual void mousePressEvent		(QMouseEvent *);
	virtual void mouseReleaseEvent		(QMouseEvent *);
	virtual void mouseMoveEvent		(QMouseEvent *);
	virtual void mouseDoubleClickEvent	(QMouseEvent *);
	virtual void drawContents (QPainter *);

private:
	void rotate (float fRot, bool bClear=false);
	void centerImage  ();
	void preloadImage ();	// sets the m_image object	(used for input to m_imageColored)
	void adjustColor  ();	// sets the m_imageColored object. (used for the matrix)

private:
	CXmlSlideshow::img_struct *m_pImage;
	QString m_backgroundFileName;
	QImage  m_background;	// Holds the unscaled background image or black if not avail.
	QImage	m_image;		// This holds the image in the right size (bFast or not)
	QImage	m_imageColored;	// This is the image after coloration and before rotation, stretch shear etc.
	QPixmap	m_pixmap;		// only used to return the generated pixmap.
	// for faster handling the image is scaled to width()/height()
	// In order to scal correctly we need to keep the original width/height of the image
	int     m_iOriginalWidth;
	int     m_iOriginalHeight;
	QPoint	m_currentMousePos;
	bool	m_bActiveCursor;
	bool	m_bActiveZoom;
	bool	m_bShowBackground;
	bool	m_bFastPreview;
	ImageManipulator m_manipulator;

	float	m_fStartZoom;	// This zoom factor is the original zoom I.e. the one the image was using when opening this GUI.
	bool	m_bNeedToAdjustColor;
};

#endif // IMAGEPREVIEW_H

