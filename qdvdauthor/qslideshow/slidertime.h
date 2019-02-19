/***************************************************************************
    slidertime.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef SLIDERTIME_H
#define SLIDERTIME_H

#include <qslider.h>
//Added by qt3to4:
#include <QMouseEvent>

class SliderTime : public QSlider 
{
	Q_OBJECT
public:
	SliderTime (QWidget * parent, const char * name = 0);
	virtual ~SliderTime();

protected:
	virtual void mousePressEvent	(QMouseEvent *);
};

#endif // SLIDERTIME_H

