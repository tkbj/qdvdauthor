/***************************************************************************
    soundline.cpp
                             -------------------
    FilterLine class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QMenu>
#include <qsplitter.h>
#include <qfileinfo.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>

#include "soundline.h"

//SoundLine::SoundLine (QWidget * parent, const char * name, Qt::WFlags f ) //ooo
SoundLine::SoundLine (QWidget * parent, const char * name, Qt::WindowFlags f )  //xxx
	: FilterLine (parent, name, f)
{
	m_pSplitter = NULL;
	m_pSlideshow = NULL;
}

SoundLine::~SoundLine ()
{

}

/*
void SoundLine::rangeChanged (DialogFilter::range_struct *pNewRange, CXmlSlideshow *pSlideshow)
{
	uint t;
	QLabel *pLabel;
	float fWidthX;
	QValueList<int> listSizes;
	// Here we build the sound line
	if (m_pSplitter)
		delete m_pSplitter;
	m_pSplitter = new QSplitter (this, "splitter_this");
	m_pSplitter->setOrientation( QSplitter::Horizontal );

	if (pNewRange)
		resize (pNewRange->iXExtension, height());

	QString qsFileName;
	QFileInfo fileInfo;
	for (t=0;t<pSlideshow->audio_list.count();t++)	{
		pLabel = new QLabel (m_pSplitter);
		pLabel->setPaletteForegroundColor (QColor (255, 0 ,0));
		pLabel->setPaletteBackgroundColor (QColor (20, 20, 20));
		pLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)1, 0, 0 ) );
		pLabel->setFixedHeight (LINE_HEIGHT-2*LINE_WIDTH);
		fileInfo.setFile (pSlideshow->audio_list[t]);
		qsFileName = fileInfo.fileName ();
		pLabel->setText (QString ("<p align=\"center\">%1</p>").arg(qsFileName));
		// +0.5 to round to the next integer var (simulation of math.h's round function)
		fWidthX = (float) (t+1)/ (float)(pSlideshow->audio_list.count()+1) * width() +0.5;
		listSizes.append ((int)fWidthX);
	}
	m_pFrameLayout->addWidget( m_pSplitter, 0, 0 );
	m_pSplitter->show();
}
*/

void SoundLine::rangeChanged (DialogFilter::range_struct *pNewRange, CXmlSlideshow *pSlideshow)
{
	uint t;
	SoundPart *pLabel;
	m_pSlideshow = pSlideshow;
	// Here we build the sound line
	if (m_pSplitter)
		delete m_pSplitter;
	//m_pSplitter = new QSplitter (this, "splitter_this"); //ooo
    m_pSplitter = new QSplitter (this); //xxx
	m_pSplitter->setOrientation( Qt::Horizontal );

	if (pNewRange)
		resize (pNewRange->iXExtension, height());

	QString qsFileName;
	QFileInfo fileInfo;
	for (t=0;t<(uint)pSlideshow->audio_list.count();t++)	{	//ox
		fileInfo.setFile (pSlideshow->audio_list[t]);
		qsFileName = fileInfo.fileName ();
		pLabel = new SoundPart (qsFileName, m_pSplitter);
		connect (pLabel, SIGNAL(signalDeleteMe(QString)), this, SLOT(slotDeleteSound(QString)));
	}
	m_pFrameLayout->addWidget( m_pSplitter, 0, 0 );
	m_pSplitter->show();
}

void SoundLine::slotDeleteSound(QString qsSound)
{
	// Here we have to find the entry in the list first and then remove it from m_pSlideshow->audio_list ...
	uint t;
	QFileInfo fileInfo;
	if (!m_pSlideshow)
		return;

	for (t=0; t<(uint)m_pSlideshow->audio_list.count();t++)	{	//ox
		fileInfo.setFile(m_pSlideshow->audio_list[t]);
		if (QString ("<p align=\"center\">") + fileInfo.fileName() + QString ("</p>") == qsSound)	{
			//m_pSlideshow->audio_list.remove (m_pSlideshow->audio_list[t]); //ooo
            m_pSlideshow->audio_list.removeOne (m_pSlideshow->audio_list[t]);   //xxx
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
//
// Class FilterPart.
//
// This class encapsulates the part of the FilterLine,
// which actually contains a filter. The rest are images and of no interest here.
//
///////////////////////////////////////////////////////////////////////////////////
//SoundPart::SoundPart (QString fileName, QWidget *parent, const char *name, Qt::WFlags f)  //ooo
SoundPart::SoundPart (QString fileName, QWidget *parent, const char *name, Qt::WindowFlags f)   //xxx
	//: QLabel (parent, name, f)   //ooo
    : QLabel (parent)   //xxx
{
	//setPaletteForegroundColor (QColor (255, 0 ,0));  //ooo
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QColor (255, 0 ,0));		//xxx
    this->setPalette(palette);							//xxx
	//setPaletteBackgroundColor (QColor (20, 20, 20)); //ooo
    QPalette palette2;								//xxx
    palette2.setBrush(QPalette::Base, QBrush(QColor(20, 20, 20)));	//xxx
    this->setPalette(palette2);						//xxx
	//setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)2, (QSizePolicy::SizeType)1, 0, 0 ) );    //ooo
	setFixedHeight (LINE_HEIGHT-2*LINE_WIDTH);
	setText (QString ("<p align=\"center\">%1</p>").arg(fileName));
}
SoundPart::~SoundPart ()
{

}

void SoundPart::enterEvent 	(QEvent *)
{
	//setPaletteBackgroundColor (QColor (50, 50, 50)); //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor(50, 50, 50)));	//xxx
    this->setPalette(palette);						//xxx
}

void SoundPart::leaveEvent		(QEvent *)
{
	//setPaletteBackgroundColor (QColor (20, 20, 20)); //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor(20, 20, 20)));	//xxx
    this->setPalette(palette);						//xxx
}

void SoundPart::mousePressEvent	(QMouseEvent *pEvent)
{
	//setPaletteBackgroundColor (QColor (255, 0, 0));  //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor(255, 0, 0)));	//xxx
    this->setPalette(palette);						//xxx
	QMenu *pMenu = new QMenu (this);
	/*pMenu->insertItem ( tr ("&Edit ..."), this, SLOT(slotEdit()));
	pMenu->insertItem ( tr ("&Delete"),   this, SLOT(slotDelete()));
	pMenu->exec(mapToGlobal(pEvent->pos()));*/ //oooo
}

void SoundPart::mouseReleaseEvent	(QMouseEvent *)
{
	//setPaletteBackgroundColor (QColor (170, 170, 255));  //ooo
    QPalette palette;								//xxx
    palette.setBrush(QPalette::Base, QBrush(QColor(170, 170, 255)));	//xxx
    this->setPalette(palette);						//xxx
}

void SoundPart::slotEdit     ()
{
	printf ("Edit something ... \n");
}

void SoundPart::slotDelete   ()
{
	emit (signalDeleteMe(text()));
	delete this;
}


