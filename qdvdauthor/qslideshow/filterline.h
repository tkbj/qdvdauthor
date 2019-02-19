/***************************************************************************
    filterline.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef FILTERLINE_H
#define FILTERLINE_H

//#include <q3frame.h>  //ooo
#include <QFrame>       //xxx
#include <qlabel.h>
//#include <q3valuelist.h>	//oxx
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QGridLayout>

#include "dialogfilter.h"

#define LINE_WIDTH 2

class QGridLayout;
class QSplitter;
class CXmlSlideshow;

class FilterPart : public QLabel 
{
	Q_OBJECT
public:
	//FilterPart (CXmlSlideshow::filter_struct *pFilter, QWidget * parent, const char * name = 0, Qt::WFlags f = 0 );  //ooo
    FilterPart (CXmlSlideshow::filter_struct *pFilter, QWidget * parent, const char * name = 0, Qt::WindowFlags f = 0 );    //xxx
	virtual ~FilterPart ();

	QString &getName ();
protected:
	virtual void enterEvent 	(QEvent *);
	virtual void leaveEvent		(QEvent *);
	virtual void mousePressEvent	(QMouseEvent *);
	virtual void mouseReleaseEvent	(QMouseEvent *);

protected slots:
	virtual void slotFadein       ();
	virtual void slotCrossfade    ();
	virtual void slotFadeout      ();
	virtual void slotEdit         ();
	virtual void slotInsertImage  ();
	virtual void slotInsertFilter ();
private:
	QString m_qsName;
};

class FilterLine : public QFrame
{
	Q_OBJECT
public:
	 //FilterLine (QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );    //ooo
    FilterLine (QWidget * parent = 0, const char * name = 0, Qt::WindowFlags f = 0 );   //xxx
	virtual ~FilterLine ();

	virtual void initMe ();
	virtual void rangeChanged (DialogFilter::range_struct *pNewRange, CXmlSlideshow *pSlideshow);

protected:
	// private member variables.
	QGridLayout*	m_pFrameLayout;
	QSplitter*	m_pSplitter;
	QList<QLabel *>	m_listLabels;	//oxx
};

#endif // FILTERLINE_H

