/***************************************************************************
    mediainfo.h
                             -------------------
    MediaInfo - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class will use the Media Engine (Xine, MPlayer, or VLC), to obtain 
    information about the stream currently set by this engine.

****************************************************************************/

#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <qthread.h>
#include <qstring.h>

class MediaInfo
{
public:
	MediaInfo (void *p=NULL); // Constructor which takes a parent engine to speed up initializing time and save memory
	virtual ~MediaInfo ();    // Destructor

	void   *getMediaEngine  ();
	void    setMediaEngine  (void *);
	void    setFileName     (QString, bool b=true );
	QString getFileName     (bool b=true);

	unsigned long int getLength ();
	QString getLengthString ();
	QString getRatio        ();
	QString getFormat       (bool);	// true=Video
	QString getCodec        (bool);	// true=Video
	uint    getSize         ();
	QString getSizeString   ();
	int     getResolution   (bool);
	QString getResolutionString ();
	float   getFPS          ();
	QString getStatus       ();
	int     getSample       ();
	QString getSampleString ();
	int     getBPS          (bool);
	QString getBPSString    (bool);
	int     getBits         ();

	bool hasAudio ( );
	bool hasVideo ( );
	//F.J.Cruz
	bool isSeekeable  ( );
	
	bool audioHandled ( );
	bool videoHandled ( );

	virtual QImage getScreenshot ( long ) = 0;

	// Non mandatory overloaded functions
	virtual QString getExtensions (bool b=true);

	// Some housekeeping stuff
	// Required for MPlayerInfo. MainThread must use EventLoop, worker thread must not have an eventLoop.
	bool useEventLoop    ( );
	void setUseEventLoop ( bool );

protected:
	void initMe ();
	virtual void queryValues () = 0;
	QString getVideoFormat (int, int);
	void test_print ();
//	char * get_fourcc_string (int);

protected:
	void   *m_pMediaEngine;
	bool    m_bHaveValues;
	bool    m_bUseEventLoop;

	bool    m_bHasAudio;
	bool    m_bHasVideo;
	//F.J.Cruz 18-12-04
	bool	m_bIsSeekeable;
	bool    m_bAudioHandled;
	bool    m_bVideoHandled;
	QString m_qsAudioCodec;
	QString m_qsVideoCodec;
	QString m_qsFileName;	// E.g. video.mpg
	uint    m_iSize;
	QString m_qsSize;	// E.g. 2.2M
	int     m_iResolutionWidth;
	int     m_iResolutionHeight;
	QString m_qsResolution;	// E.g. 720x576
	float   m_fFPS;
	QString m_qsFPS;		// E.g. 25
	QString m_qsRatio;	// E.g. 16:9
	unsigned long int m_iLength;
	QString m_qsLength;	// E.g. 01:22:23
	QString m_qsVideoFormat;	// E.g. 'PAL'
	QString m_qsAudioFormat;	// E.g. 'Stereo'
	QString m_qsStatus;	// E.g. Ok	-=> to say this file we can autmatically convert to a DVD video
	// For Audio Sources :
	int     m_iBits;
	int     m_iSample;
	QString m_qsSample;	// E.g. 48kHz
	int     m_iAudioBPS;	// E.g. 224000
	QString m_qsAudioBPS;	// E.g. 224kbps
	int     m_iVideoBPS;	// E.g. 978000
	QString m_qsVideoBPS;	// E.g. p78kbps
};


#endif	// MEDIAINFO_H

