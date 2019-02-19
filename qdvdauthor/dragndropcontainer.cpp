/***************************************************************************
    dragndropcontainer.cpp
                             -------------------
    Class DragNDropContainer
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is only a container class to encapsulate the
   variables needed for Drag'n drop support
    
****************************************************************************/

#include "dragndropcontainer.h"

DragNDropContainer::DragNDropContainer()
{
	sizeThumbnail   = QSize (  50,  50 );
	sizeButton      = QSize ( 150, 150 );
	bImageButton    = true;	// Image or MovieButton
	bWithText       = true;
	iTextPosition   = TEXT_POS_BOTTOM;
	timeStartOffset = QTime ( );
	timeDuration    = QTime ( );
	font            = QFont ( );
	colorForeground = Rgba  ( 255, 0, 0 ); // default to red ... ( why not )
	bStartAfterDropEvent = true;
	iHowNiceShouldIBe    =    0;
}

DragNDropContainer::~DragNDropContainer()
{

}

