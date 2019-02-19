/***************************************************************************
    soundline.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef SOUNDLINE_H
#define SOUNDLINE_H

#include "filterline.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QLabel>

class SoundPart : public QLabel
{
	Q_OBJECT
public:
	//SoundPart (QString, QWidget * parent, const char * name = 0, Qt::WFlags f = 0 ); //ooo
    SoundPart (QString, QWidget * parent, const char * name = 0, Qt::WindowFlags f = 0 );   //xxx
	virtual ~SoundPart ();

signals:
	void signalDeleteMe (QString);

protected:
	virtual void enterEvent 	(QEvent *);
	virtual void leaveEvent		(QEvent *);
	virtual void mousePressEvent	(QMouseEvent *);
	virtual void mouseReleaseEvent	(QMouseEvent *);

protected slots:
	virtual void slotEdit     ();
	virtual void slotDelete   ();
};


class SoundLine : public FilterLine
{
	Q_OBJECT
public:
			 //SoundLine (QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );   //ooo
             SoundLine (QWidget * parent = 0, const char * name = 0, Qt::WindowFlags f = 0 );   //xxx
	virtual ~SoundLine ();

	virtual void rangeChanged (DialogFilter::range_struct *pNewRange, CXmlSlideshow *pSlideshow);

protected slots:
	void slotDeleteSound (QString);

private:
	CXmlSlideshow *m_pSlideshow;
};

#endif // SOUNDLINE_H

