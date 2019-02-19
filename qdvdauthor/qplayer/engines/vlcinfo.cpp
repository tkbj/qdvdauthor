/****************************************************************************
** Implementation of class VLCInfo
**
** Created: Thu Nov 23 07:53:05 2004
**      by: Varol Okan using the kate editor
**
** This class collects all possible information about
** the current set File.
**
****************************************************************************/

#include <stdio.h>	//xxx

#include <qimage.h>

#include "vlcinfo.h"

VLCInfo::VLCInfo ()
{
}

VLCInfo::~VLCInfo ()
{
}

VLCInfo::VLCInfo (void *pVoid)
{
}

void VLCInfo::queryValues ()
{
	printf ("VLCInfo::queryValues\n");
}

QImage VLCInfo::getScreenshot( long )
{
	return QImage ();
}

