/****************************************************************************
** SourceFileEntry - class
**
** Created: Thu Jun 10 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class will use the xineEngine to obtain information about the 
** stream currently set by this engine.
**
****************************************************************************/

#ifndef VLCINFO_H
#define VLCINFO_H

#include <qimage.h>

#include "../mediainfo.h"

class VLCInfo : public MediaInfo
{
public:
	VLCInfo ();
	VLCInfo (void *);
	virtual ~VLCInfo ();

	// Mandatory function getScreenshot ()
	virtual QImage getScreenshot ( long );
protected:
	// Mandatory function queryValues ()
	virtual void queryValues ();

private:
	// Private member variables
};

#endif	// VLCINFO_H

