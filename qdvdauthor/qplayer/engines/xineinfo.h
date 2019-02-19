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

#ifndef XINEINFO_H
#define XINEINFO_H

#include <qthread.h>
//#include <qstring.h>  //ooo
#include <QString>      //xxx
//#include <QImage>       //xxx

#include <xine.h>
#include "../mediainfo.h"

class XineInfo : public MediaInfo
{
public:
	XineInfo (void *p=NULL);
	virtual ~XineInfo ();

	virtual QImage getScreenshot ( long );

protected:
	// Mandatory function queryValues ()
	virtual void queryValues ();

private:
	xine_stream_t *getXineStream ();
	void setXineStream (xine_stream_t *);

	QImage getScreenshot(QString);
	void GetScreenshot(uchar*&, int&, int&, double&);

	QString msToTimeString(unsigned long int);
	void   yuy2Toyv12 (uint8_t *, uint8_t *, uint8_t *, uint8_t *, int, int);
	uchar *yv12ToRgb (uint8_t *, uint8_t *, uint8_t *, int, int);
	char  *get_fourcc_string (int);
	static void frameOutputCb ( void *pUserData, int  iVideoWidth, int iVideoHeight, double fVideoAspect, 
				    int  *piDestX,   int *piDestY, int *piDestWidth, int *piDestHeight, double *pfDestAspect, 
				    int  *piWinX,    int *piWinY );

	void createXineStream ();

private:
	xine_stream_t *m_pXineStream;
	xine_audio_port_t *m_pAudioDriver;
	xine_video_port_t *m_pVideoDriver;
	bool           m_bOwnXineEngine;
	bool           m_bOwnXineStream;
	bool           m_bHaveValues;
};


#endif	// XINEINFO_H

