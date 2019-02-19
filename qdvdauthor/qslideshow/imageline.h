/***************************************************************************
    imageline.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef IMAGELINE_H
#define IMAGELINE_H

//#include <q3frame.h>  //ooo
#include <QFrame>       //xxx
//Added by qt3to4:
#include <QPixmap>
//#include <QCustomEvent>   //ooo
#include <QEvent>           //xxx
#include <QMouseEvent>
#include <QLabel>

#include "dialogfilter.h"
#include "../xml_slideshow.h"

class QLabel;

// This class holds the lines of images in the top of this dialog.
class ImageLine : public QFrame
{
	Q_OBJECT
	// Here we define the private class for the thread
	class CreateThumbs : public QThread
	{
	public:
		 CreateThumbs ();
		~CreateThumbs ();
		
		// This function sets the neccesary values.
		void initThumbs (ImageLine *pImageLine);
		// this is the main function which executes the new thread.
		// Start it by using thread->start ();
		virtual void run ();
		void sync ();
	private:
		ImageLine *m_pImageLine;
	};
	
public:
	 //ImageLine (QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 ); //ooo
    ImageLine (QWidget * parent = 0, const char * name = 0, Qt::WindowFlags f = 0 );    //xxx
	~ImageLine () {};

	void initImages(CXmlSlideshow *);	//QString &xmlFileName);
	void setCurrentRange(DialogFilter::range_struct *pRange);
	void createThumbs(bool bCallThread = true);
	void createFrames();

public slots:
	virtual void slotRefreshPixmap ();
	virtual void slotRotate90  ();
	virtual void slotRotate180 ();
	virtual void slotRotate270 ();
	virtual void slotRotateFree();
	virtual void slotEditImage ();
	/**
	 * F.J.Cruz
	 * 18/10/04
	 **/
	virtual void slotInsertSubtittle ();
	// callback from the ImageDialog when done
	virtual void slotImageDialogDone (void *);

signals:
	void signalProgress (int);
	void signalRefreshPixmap ();

protected:
	//virtual void customEvent  (QCustomEvent* );  //ooo
    virtual void customEvent  (QEvent* );   //xxx
	virtual void mousePressEvent (QMouseEvent *);
	virtual void mouseReleaseEvent  (QMouseEvent *);
	virtual void mouseDoubleClickEvent (QMouseEvent *);
private:	// Private functions
	void initMe();
	void rotate (float fRot, bool bClear=false);
	void refreshPixmap ();
	void setTotalXExtension (uint iTotalXExtension);
	int  getImageByPos (int iMouseX);

private:	// private variables.
	QLabel		*m_pPixmapLabel1;
	QLabel		*m_pPixmapLabel2;
	QLabel		*m_pImageLabel;
	QPixmap 	 m_Image;	// holds the upper/lower frame.
	CreateThumbs	 m_createThumbs;
	CXmlSlideshow	*m_pSlideshow;			// double of the one in DialogFileter.
	DialogFilter::range_struct *m_pCurrentRange;	// Is set from DialogFilter.
	bool			 m_bSemaphore;
	bool 			 m_bCreatingThumbnails;
	uint m_iHotObject;	// the object which is currently pressed on (must be of type image_struct)
};


#endif // IMAGELINE_H
