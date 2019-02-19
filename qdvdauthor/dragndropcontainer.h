/***************************************************************************
    dragndropcontainer.h
                             -------------------
    Class DragNDropContainer
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is only a container class to encapsulate the
   variables needed for Drag'n drop support

****************************************************************************/

#ifndef DRAGNDROPCONTAINER_H
#define DRAGNDROPCONTAINER_H

#include <QDateTime>
#include <QSize>
#include <QFont>

#include "rgba.h"

class DragNDropContainer
{
public:
	enum enTextPos {TEXT_POS_UNDEF=0, TEXT_POS_BOTTOM, TEXT_POS_TOP, TEXT_POS_LEFT, TEXT_POS_RIGHT};

	 DragNDropContainer();
	~DragNDropContainer();

public:
	// Here are the member variables. Note that they are all publicly available.
	QSize  sizeThumbnail;
	QSize  sizeButton;
	bool   bStartAfterDropEvent;
	bool   bImageButton;	// Image or MovieButton
	bool   bWithText;
	int    iHowNiceShouldIBe; // priority of background thread.
	int    iTextPosition;
	QTime  timeStartOffset;
	QTime  timeDuration;
	QFont  font;
	Rgba   colorForeground;
};

#endif// DRAGNDROPCONTAINER_H
