/***************************************************************************
    dialogfilter.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef DIALOGFILTER_H
#define DIALOGFILTER_H

#define FRAME_HEIGHT   9
#define LINE_HEIGHT    30
#define TICK_FACTOR    100
#define MAX_PROGRESS   1000
#define MAX_XEXTENSION 15000

#include <qpixmap.h>
#include <qdialog.h>
#include <qthread.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
//#include <Q3ValueList>	//oxx
#include <QFrame>

#include "slidertime.h"
#include "../xml_slideshow.h"	//xxx

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
//class Q3ScrollView;   //ooo
class QScrollArea;      //xxx
class QLabel;
class QLineEdit;
class QPushButton;
class QFrame;
class QComboBox;
class QProgressBar;

//class SliderTime;
class ImageLine;
class FilterLine;
class SoundLine;

class DialogFilter : public QDialog
{
    Q_OBJECT

public:
	// a range is the range of images you can see in one line.
	class range_struct {
 		public:
		range_struct()	{
			iXExtension = iStartImg = iEndImg  = 0;
			iStartObject = iEndObject = 0;
			fStartTime = fEndTime = 0.0f;
			bAlreadyGenerated = false;
		};
		uint  iStartImg;	// Image index (not filter !!!)
		uint  iEndImg;		// Image index (not filter !!!)
		uint  iStartObject;	// the index of the TimeObject (Img + Filter)
		uint  iEndObject;	// the index of the TimeObject (Img + Filter)
		float fStartTime;	// start time of this range.
		float fEndTime;
		uint  iXExtension;	// XExtension of the range.
		bool  bAlreadyGenerated;
		QList<int> listX;		//oxx
		QPixmap pixmapRangeThumbs;
		QPixmap tmpPixmap;
	};
	// Constructor / destructor.
	 //DialogFilter( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );   //ooo
     DialogFilter( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0 ); //xxx
	~DialogFilter();

	// This function will load a slideshow - xml file and display the images/filter marks sound tracks accordingly
	void setImages(CXmlSlideshow *); //QString &xmlFileName);
	range_struct *getRange(int iWhichRange);

public slots:
	virtual void slotSetProgress (int);
	virtual void slotAddSound ();
	virtual void slotAddSoundTrack ();

private:
	// Private member functions.
	void initImages (QString &xmlFileName);		// inits m_listRange, m_slideShow

protected slots:
	virtual void languageChange    ();
	virtual void slotSliderMoved   (int);
	virtual void slotRangeChanged  (int);


private:
	SliderTime* m_pSliderTime;

	QPushButton* m_pButtonAddSoundTrack;
	QPushButton* buttonHelp;
	QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QFrame* m_pFrame;
	QFrame* m_pScrollFrame;
	QScrollArea* m_pScrollView;
	QProgressBar* m_pProgress;
	QFrame* frameRange;
	QComboBox* m_pComboRange;
	QLabel* m_pEditTotal;
	QLabel* textLabel3;
	QLineEdit* m_pEditTime;
	QPushButton* m_pButtonAddFilter;
	QLabel* textLabel1;
	QPushButton* m_pButtonAddSound;
	QLabel* textLabel2;
	QGridLayout* DialogFilterLayout;
	QHBoxLayout* layout6;
	QGridLayout* m_pFrameLayout;
	QGridLayout* m_pScrollViewLayout;
	QGridLayout* frameRangeLayout;
	QVBoxLayout* layout60;
	QVBoxLayout* layout48;
	QVBoxLayout* layout13;
	QHBoxLayout* layout10;
	QHBoxLayout* layout43;

	FilterLine* m_pFrameFilter;
	SoundLine* m_pFrameSound;

	QString			 m_qsLastDirectory;
	int 			 m_iDeltaPadding;	// the difference between ScrollView, and ScrollFrame.
	ImageLine		*m_pImageLine;		// is the object to draw / generate the thumbnails.
	CXmlSlideshow	*m_pSlideshow;		// The orig rests in the DialogSlide class
	QList<DialogFilter::range_struct *>	m_listRange;	// stores some information to the range.	//oxx
};

#endif // DIALOGFILTER_H
