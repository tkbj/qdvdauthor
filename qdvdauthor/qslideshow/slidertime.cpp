/***************************************************************************
    slidertime.cpp
                             -------------------
    FilterLine class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <qslider.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "slidertime.h"

SliderTime::SliderTime (QWidget * parent, const char * name)
	//: QSlider (parent, name) //ooo
    : QSlider (parent)  //xxx
{
}

SliderTime::~SliderTime()
{

}

void SliderTime::mousePressEvent (QMouseEvent *pEvent)
{
	// and finally we'll handle the nominal usage of mousePressEvent
	QSlider::mousePressEvent(pEvent);
	
	// This is the only function I want to differ in that the 
	// user clicks on any part of the slider bar and the cursor 
	// get automagically to the clicked position.
	int iSliderPos = 0;
	// first we get the range
	//int iRange = maxValue()-minValue();  //ooo
    int iRange = maximum()-minimum();
	// Then we get the pixel length of this slider control.
	int iWidth = width();
	float fSliderPos = (float)iRange / iWidth * pEvent->pos().x();
	iSliderPos = (int)fSliderPos;
	//iSliderPos += minValue();    //ooo
    iSliderPos += minimum();    //xxx
	setValue (iSliderPos);
	emit (sliderMoved (iSliderPos));
}


