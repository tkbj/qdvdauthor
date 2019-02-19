/***************************************************************************
    filterline.cpp
                             -------------------
    FilterLine class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

//#include <q3frame.h>  //ooo
#include <QFrame>       //xxx
//#include <q3valuelist.h>	//oxx
#include <qlabel.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qfileinfo.h>
#include <QMenu>
//Added by qt3to4:
#include <QGridLayout>
#include <QEvent>
#include <QMouseEvent>

#include "filterline.h"
#include "dialogfilter.h"
#include "../xml_slideshow.h"

//FilterLine::FilterLine (QWidget * parent, const char * name, Qt::WFlags f )   //ooo
FilterLine::FilterLine (QWidget * parent, const char * name, Qt::WindowFlags f )    //xxx
	//: Q3Frame (parent, name, f)  //ooo
    : QFrame (parent)   //xxx
{
	// Here are the prepared drop down menus
	//setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );  //oooo
	setMinimumSize( QSize( 600, 30 ) );
	setLineWidth(LINE_WIDTH);
	setFrameShape( QFrame::WinPanel );
	setFrameShadow( QFrame::Sunken );
	
	//m_pFrameLayout = new Q3GridLayout(this, 1, 1, LINE_WIDTH, LINE_WIDTH); //, 4, 6, "frameLayout"); //ooo
    m_pFrameLayout = new QGridLayout(this); //, 4, 6, "frameLayout");   //xxx
	m_pSplitter = NULL;
}

FilterLine::~FilterLine ()
{

}

void FilterLine::initMe ()
{
	m_pSplitter = NULL;
}

void FilterLine::rangeChanged (DialogFilter::range_struct *pNewRange, CXmlSlideshow *pSlideshow)
{
	uint t;
	QLabel *pLabel;	
	float fTotalTime, fWidthX;
	QList<int> listSizes;	//oxx
	CXmlSlideshow::time_object *pTimeObject;
	// Here we build the filter lline
	if (m_pSplitter)
		delete m_pSplitter;
	//m_pSplitter = new QSplitter (this, "splitter_this"); //ooo
    m_pSplitter = new QSplitter (this); //xxx
	m_pSplitter->setOrientation( Qt::Horizontal );

	resize (pNewRange->iXExtension, height());
	// The QLabel objects are destroyed together with the m_pSplitter !!!
//	for (t=0;t<m_listLabels.count();t++)
//		delete m_listLabels[t];
	m_listLabels.clear ();
	
	fTotalTime = pNewRange->fEndTime -  pNewRange->fStartTime;
	QString qsFileName;
	QFileInfo fileInfo;
	for (t=pNewRange->iStartObject;t<pNewRange->iEndObject;t++)	{
		pTimeObject = pSlideshow->getTimeObject(t);
		if (pTimeObject->node_name == "img")	{
			pLabel = new QLabel (m_pSplitter);
			//pLabel->setPaletteForegroundColor (QColor (255, 0 ,0));    //ooo
	        QPalette palette;	//xxx
	        palette.setBrush(pLabel->backgroundRole(), QColor(255, 0 ,0));	//xxx
	        pLabel->setPalette(palette);						//xxx
			//pLabel->setPaletteBackgroundColor (QColor (20, 20, 20));   //ooo
	        QPalette palette2;	//xxx
	        palette2.setBrush(QPalette::Base, QColor(20, 20, 20));			//xxx
	        pLabel->setPalette(palette2);					//xxx
			//pLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)1, 0, 0 ) );  //oooo
			pLabel->setFixedHeight (LINE_HEIGHT-2*LINE_WIDTH);
			fileInfo.setFile (((CXmlSlideshow::img_struct *)(pTimeObject))->src);
			qsFileName = fileInfo.fileName ();
			pLabel->setText (QString ("<p align=\"center\">%1</p>").arg(qsFileName));
		}
		else
			pLabel = (QLabel *)new FilterPart ((CXmlSlideshow::filter_struct *)pTimeObject, m_pSplitter);
		m_listLabels.append(pLabel);
		// +0.5 to round to the next integer var (simulation of math.h's round function)
		fWidthX = (pTimeObject->fEndTime - pTimeObject->fStartTime) / fTotalTime * width() +0.5;
		listSizes.append ((int)fWidthX);
	}
	m_pSplitter->setSizes (listSizes);
	m_pFrameLayout->addWidget( m_pSplitter, 0, 0 );
	m_pSplitter->show();
}


///////////////////////////////////////////////////////////////////////////////////
//
// Class FilterPart.
//
// This class encapsulates the part of the FilterLine, 
// which actually contains a filter. The rest are images and of no interest here.
//
///////////////////////////////////////////////////////////////////////////////////
//FilterPart::FilterPart (CXmlSlideshow::filter_struct *pFilter, QWidget * parent, const char * name, Qt::WFlags f) //ooo
FilterPart::FilterPart (CXmlSlideshow::filter_struct *pFilter, QWidget * parent, const char * name, Qt::WindowFlags f)  //xxx
	//: QLabel (parent, name, f)   //ooo
    : QLabel (parent)   //xxx
{
	if (pFilter->name.isEmpty())
		setText( QString( "<p align=\"center\">crossfade</p>" ) );
	else
		setText( tr( "<p align=\"center\">%1</p>" ).arg(pFilter->name) );
	//setPaletteBackgroundColor (QColor (170, 170, 255));  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor(170, 170, 255));			//xxx
	this->setPalette(palette);					//xxx
	setFixedHeight(LINE_HEIGHT-2*LINE_WIDTH);
	//setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );  //oooo
	m_qsName = pFilter->name;
}

FilterPart::~FilterPart ()
{

}

QString &FilterPart::getName ()
{
	return m_qsName;
}

void FilterPart::mousePressEvent (QMouseEvent *)
{
	//setPaletteBackgroundColor (QColor (255, 0, 0));  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor(255, 0, 0));			//xxx
	this->setPalette(palette);					//xxx
	QMenu *pMenu = new QMenu (this);
	/*pMenu->insertItem ( tr ("f&adein"), this, SLOT(slotFadein()));
	pMenu->insertItem ( tr ("&crossfade"), this, SLOT(slotCrossfade()));
	pMenu->insertItem ( tr ("&fadeout"), this, SLOT(slotFadeout()));
	pMenu->insertSeparator ();
	pMenu->insertItem ( tr ("Insert Image"), this, SLOT(slotInsertImage()));
	pMenu->insertItem ( tr ("Insert Filter"), this, SLOT(slotInsertFilter()));	
	pMenu->insertSeparator ();
	pMenu->insertItem ( tr ("&Edit"), this, SLOT(slotEdit()));
	pMenu->exec(mapToGlobal(QPoint(0,0)));*/   //oooo
}

void FilterPart::mouseReleaseEvent (QMouseEvent *)
{
	//setPaletteBackgroundColor (QColor (170, 170, 255));  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor(170, 170, 255));			//xxx
	this->setPalette(palette);					//xxx
}

void FilterPart::enterEvent (QEvent *)
{
	//setPaletteBackgroundColor (QColor (190, 190, 230));  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor(190, 190, 230));			//xxx
	this->setPalette(palette);					//xxx
}

void FilterPart::leaveEvent (QEvent *)
{
	//setPaletteBackgroundColor (QColor (170, 170, 255));  //ooo
	QPalette palette;	//xxx
	palette.setBrush(QPalette::Base, QColor(170, 170, 255));			//xxx
	this->setPalette(palette);					//xxx
}

void FilterPart::slotFadein ()
{
	setText( tr( "<p align=\"center\">fadein</p>" ) );
	m_qsName = QString ("fadein");
}

void FilterPart::slotCrossfade ()
{
	setText( tr( "<p align=\"center\">crossfade</p>" ) );
	m_qsName = QString ("crossfade");
}

void FilterPart::slotFadeout ()
{
	setText( tr( "<p align=\"center\">fadeout</p>" ) );
	m_qsName = QString ("fadeout");
}

void FilterPart::slotEdit ()
{
	printf ("Edit something ... \n");
}

void FilterPart::slotInsertImage ()
{
	printf ("Insert something ... \n");
}

void FilterPart::slotInsertFilter ()
{
	printf ("Edit something ... \n");
}

