/****************************************************************************
** Implementation of class XineInfo
**
** Created: Thu Jun 10 07:53:05 2004
**      by: Varol Okan using the kate editor
**
** This class collects all possible information about
** the current set stream.
**
****************************************************************************/

#include <unistd.h>

#include <qobject.h>
//#include <qimage.h>   //ooo
#include <QImage>       //xxx
#include <qfile.h>
#include <qdir.h>
#include <QMessageBox>	//xxx
#include <QUrl> //xxx

#include "xineinfo.h"

XineInfo::XineInfo  ( void *pVoid )
	: MediaInfo ( pVoid )
{
	m_pXineStream    = NULL;
	m_pAudioDriver   = NULL;
	m_pVideoDriver   = NULL;
	//m_pMediaEngine   = pVoid;
	m_bOwnXineEngine = false;
	if ( ! pVoid )	{
		m_pMediaEngine = (void *)xine_new ( );
		//QString configFile = QDir::homeDirPath ( );		//ooo
		QString configFile = QDir::homePath ( );		//xxx
		configFile.append ( "/.qdvdauthor/xine_config.ini" );
		if ( QFile::exists ( configFile ) )
			//xine_config_load ( (xine_t *)m_pMediaEngine, (const char *)configFile );	//ooo
			xine_config_load ( (xine_t *)m_pMediaEngine, configFile.toLatin1().data() );	//xxx
		xine_init ( (xine_t *)m_pMediaEngine );
		m_bOwnXineEngine = true;
	}
	createXineStream ( );
}

XineInfo::~XineInfo ()
{
//printf ("XineInfo::~XineInfo Stream<%d> engine<%d>\n", (int) m_bOwnXineStream, (int) m_bOwnXineEngine);
	// Cleaning of the xine object.
	if ( m_bOwnXineStream )
		xine_dispose ( m_pXineStream );
	m_pXineStream = NULL;
	if ( ! m_pMediaEngine )
		return;
	if ( m_pAudioDriver )
		xine_close_audio_driver ( (xine_t*)m_pMediaEngine, m_pAudioDriver );
	m_pAudioDriver = NULL;
	if ( m_pVideoDriver )
		xine_close_video_driver ( (xine_t*)m_pMediaEngine, m_pVideoDriver );
	m_pVideoDriver = NULL;
	if ( m_bOwnXineEngine )
		xine_exit ( (xine_t*)m_pMediaEngine );
	m_pMediaEngine = NULL;
}

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)

typedef struct {
  uint32_t  flags;
  uint32_t  functions;
  uint32_t  decorations;
  int32_t   input_mode;
  uint32_t  status;
} MWMHints;
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS 5

void XineInfo::createXineStream ()
{
//printf ("CreateXineEngine::createXineStream (1) \n");
	if (!m_pMediaEngine)
		return;
	/*
	x11_visual_t vis;
	
	int xpos, ypos, width, height;
  double            res_h, res_v;
  char             *vo_driver = "auto";
  char             *ao_driver = "auto";
  char             *mrl = NULL;
  int               i;
  Atom              XA_NO_BORDER;
  MWMHints          mwmhints;
  
  Display *display = XOpenDisplay(NULL);
  Screen  *screen  = (Screen *) XDefaultScreen(display);
  xpos    = 0;
  ypos    = 0;
  width   = 320;
  height  = 200;

  // some initalization for the X11 Window we will be showing video in 
  XLockDisplay(display);

  Window window = XCreateSimpleWindow(display, XDefaultRootWindow(display),
                                  xpos, ypos, width, height, 1, 0, 0);
  
  XSelectInput(display, window, INPUT_MOTION);

  XA_NO_BORDER         = XInternAtom(display, "_MOTIF_WM_HINTS", False);
  mwmhints.flags       = MWM_HINTS_DECORATIONS;
  mwmhints.decorations = 0;
  XChangeProperty(display, window,
                  XA_NO_BORDER, XA_NO_BORDER, 32, PropModeReplace, (unsigned char *) &mwmhints,
                  PROP_MWM_HINTS_ELEMENTS);
  
  XMapRaised(display, window);
  
  res_h = 200; //(DisplayWidth (display, screen) * 1000 / DisplayWidthMM (display, screen));
  res_v = 200; //(DisplayHeight(display, screen) * 1000 / DisplayHeightMM(display, screen));
  XSync(display, False);
  XUnlockDisplay(display);

	// filling in the xine visual struct
	vis.display           = display;
	vis.screen            = 0; //(Screen *)screen;
	vis.d                 = window;
	vis.dest_size_cb      = NULL;
	vis.frame_output_cb   = NULL;
	vis.user_data         = NULL;
	double pixel_aspect   = 1.0; //res_v / res_h;	
*/
	// opening xine output ports
	m_pVideoDriver = xine_open_video_driver((xine_t*)m_pMediaEngine, "none", XINE_VISUAL_TYPE_NONE, NULL);
	//m_pVideoDriver = xine_open_video_driver((xine_t*)m_pMediaEngine, "none", XINE_VISUAL_TYPE_X11, &vis);
	m_pAudioDriver = xine_open_audio_driver((xine_t*)m_pMediaEngine, "none", NULL);

	// open a xine stream connected to these ports
	m_pXineStream = xine_stream_new((xine_t *)m_pMediaEngine, m_pAudioDriver, m_pVideoDriver);

	m_bOwnXineStream = true;
}

void XineInfo::setXineStream (xine_stream_t *pXineStream)
{
	initMe ();
	m_pXineStream = pXineStream;
}

void XineInfo::frameOutputCb ( void * /*pUserData*/, int /*iVideoWidth*/, int /*iVideoHeight*/, double /*fVideoAspect*/, 
							   int * /*piDestX*/, int * /*piDestY*/, int * /*piDestWidth*/, int * /*piDestHeight*/, double * /*pfDestAspect*/, int * /*piWinX*/, int * /*piWinY*/)
{
}

void XineInfo::queryValues ()
{
  int iReturn, iPosStream, iPosTime, iLengthTime;

  // Sanity check ...
  //if ( ! xine_open ( m_pXineStream, m_qsFileName.ascii ( ) ) ||		//ooo
  if ( ! xine_open ( m_pXineStream, m_qsFileName.toLatin1().data() ) ||	//xxx
       ! xine_play ( m_pXineStream, 0, 0 ) ) {
    printf ( "Unable to open mrl\n" );
    return;
  }	//oooo

  // this is the main function, to query all values
  if ( ! m_pXineStream )
    return;

  m_bHasAudio = (bool)xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_HAS_AUDIO);
  m_bHasVideo = (bool)xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_HAS_VIDEO);

  m_bAudioHandled = (bool)xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_HANDLED);
  m_bVideoHandled = (bool)xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_HANDLED);

  //F.J.Cruz 18-12-04
  m_bIsSeekeable = (bool)xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_SEEKABLE);

  // Here we check if the status is Ok (I.e. if audio avail and we can play the audio, then it is ok (same with video)
  if ( ( (m_bHasAudio) && (m_bAudioHandled) ) || ( (m_bHasVideo) && (m_bVideoHandled) ) )
    m_qsStatus = QObject::tr ("Ok");
  else
    m_qsStatus = QObject::tr ("Not Ok");

  iReturn      = 0;
  int iCounter = 0;
  while ( ( iReturn == 0 ) && ( iCounter++ < 3 ) )  {
    usleep ( 1000 );
    // Note: Xine is prone to returning wrong length information ( 2008 - 12 - 21 )
    iReturn = xine_get_pos_length (m_pXineStream, &iPosStream, &iPosTime, &iLengthTime );
  }
  if (iReturn == 1)	{	// success
    m_iLength  = iLengthTime;
    m_qsLength = msToTimeString ( iLengthTime );
  }
  else
    m_qsLength = QString ("0:00:00");

  if (m_bHasVideo)	{
    m_iResolutionWidth  = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_WIDTH);
    m_iResolutionHeight = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_HEIGHT);
    m_qsResolution = QString ("%1x%2").arg(m_iResolutionWidth).arg(m_iResolutionHeight);
    iReturn = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_FRAME_DURATION);
    m_fFPS = 90000.0 / (float)iReturn;
    m_qsFPS = QString ("%1").arg(m_fFPS);		// E.g. 25

    m_qsVideoFormat = getVideoFormat (m_iResolutionWidth, m_iResolutionHeight);

/*    iReturn = xine_get_param (m_pXineStream, XINE_PARAM_VO_ASPECT_RATIO);
    if (iReturn == XINE_VO_ASPECT_AUTO)
      m_qsRatio = QString ("auto");
    else if (iReturn == XINE_VO_ASPECT_SQUARE)
      m_qsRatio = QString ("1:1");
    else if (iReturn == XINE_VO_ASPECT_4_3)
      m_qsRatio = QString ("4:3");
    else if (iReturn == XINE_VO_ASPECT_ANAMORPHIC)
      m_qsRatio = QString ("16:9");
    else if (iReturn ==  XINE_VO_ASPECT_DVB)
      m_qsRatio = QString ("2.11:1");
    else
      m_qsRatio = QString ("<%1>").arg(iReturn);
*/

    iReturn = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_RATIO);
    m_qsRatio.sprintf ( "%4.2f:1", (double)iReturn/10000.0);

/*    if (iReturn == 1)
      m_qsRatio = QString ("1:1");
    else if (iReturn == (int)(16.0/9.0*10000))
      m_qsRatio = QString ("16:9");
    else if (iReturn == (int)(4.0/3.0*10000))
      m_qsRatio = QString ("4:3");
    else if (iReturn == (int)(2.11*10000))
      m_qsRatio = QString ("2.11:1");
    else if (iReturn == (int)(21.0/9.0*10000))
      m_qsRatio = QString ("21:9");
    else if (iReturn == (int)(29.0/9.0*10000))
      m_qsRatio = QString ("29:9");
    else
      m_qsRatio = QString ("%1:1").arg((float)iReturn/10000);
*/

    iReturn = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_FOURCC);
    if (iReturn)
      m_qsVideoCodec = QString(QString ("<%1 = %2>").arg(iReturn).arg(get_fourcc_string (iReturn)));
    else
      m_qsVideoCodec = QString(xine_get_meta_info (m_pXineStream, XINE_META_INFO_VIDEOCODEC));

    m_iVideoBPS = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_VIDEO_BITRATE);
    if (m_iVideoBPS < 1)	{ 
      // Here we construct the video bitrate in case it is set to zero by xine.
      m_iVideoBPS = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_BITRATE);
      m_iAudioBPS = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_BITRATE);
      if ((m_iVideoBPS > 0) && (m_iAudioBPS > 0) )
        m_iVideoBPS -= m_iAudioBPS;
    }
    if (m_iVideoBPS < 1)
      m_qsVideoBPS = QString ("? bps");
    if (m_iVideoBPS < 1000)
      m_qsVideoBPS = QString ("%1 bps").arg(m_iVideoBPS);
    else if (m_iVideoBPS < 1000000)
      m_qsVideoBPS = QString ("%1kbps").arg((float)m_iVideoBPS/1000);
    else
      m_qsVideoBPS = QString ("%1Mbps").arg((float)m_iVideoBPS/1000000);
  }
  if (m_bHasAudio)	{
    m_iBits = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_BITS);
    iReturn = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_FOURCC);
    if (iReturn)
      m_qsAudioCodec = QString(QString ("<%1 = %2>").arg(iReturn).arg(get_fourcc_string (iReturn)));
    else
      m_qsAudioCodec = QString(xine_get_meta_info (m_pXineStream, XINE_META_INFO_AUDIOCODEC));

    iReturn = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_CHANNELS);
    if (iReturn == 0)
      m_qsAudioFormat = QString ("");
    else if (iReturn == 1)
      m_qsAudioFormat = QObject::tr ("mono");
    else if (iReturn == 2)
      m_qsAudioFormat = QObject::tr ("stereo");
    else if (iReturn == 4)
      m_qsAudioFormat = QObject::tr ("quad");
    else
      m_qsAudioFormat = QObject::tr ("%1 channels").arg(iReturn);

    m_iSample = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_SAMPLERATE);
    if (m_iSample < 1000)
      m_qsSample = QString ("%1 Hz").arg(m_iSample);
    else if (m_iSample < 1000000)
      m_qsSample = QString ("%1kHz").arg(m_iSample/1000);
    else
      m_qsSample = QString ("%1MHz").arg(m_iSample/1000000);

    m_iAudioBPS = xine_get_stream_info (m_pXineStream, XINE_STREAM_INFO_AUDIO_BITRATE);
    if (m_iAudioBPS < 1000)
      m_qsAudioBPS = QString ("%1 bps").arg(m_iAudioBPS);
    else if (m_iAudioBPS < 1000000)
      m_qsAudioBPS = QString ("%1kbps").arg((float)m_iAudioBPS/1000);
    else
      m_qsAudioBPS = QString ("%1Mbps").arg((float)m_iAudioBPS/1000000);
  }
/*
uint t;
int iAudioArray[] = {XINE_STREAM_INFO_HAS_AUDIO, XINE_STREAM_INFO_IGNORE_AUDIO, XINE_STREAM_INFO_BITRATE, XINE_STREAM_INFO_AUDIO_CHANNELS, XINE_STREAM_INFO_AUDIO_BITS, XINE_STREAM_INFO_AUDIO_SAMPLERATE, XINE_STREAM_INFO_AUDIO_BITRATE, XINE_STREAM_INFO_AUDIO_FOURCC, XINE_STREAM_INFO_AUDIO_HANDLED, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL, XINE_STREAM_INFO_AUDIO_MODE};

int iVideoArray[] = {XINE_STREAM_INFO_VIDEO_WIDTH, XINE_STREAM_INFO_VIDEO_HEIGHT, XINE_STREAM_INFO_VIDEO_RATIO, XINE_STREAM_INFO_VIDEO_CHANNELS, XINE_STREAM_INFO_VIDEO_STREAMS, XINE_STREAM_INFO_VIDEO_BITRATE, XINE_STREAM_INFO_VIDEO_FOURCC, XINE_STREAM_INFO_VIDEO_HANDLED, XINE_STREAM_INFO_FRAME_DURATION, XINE_STREAM_INFO_HAS_CHAPTERS, XINE_STREAM_INFO_HAS_VIDEO, XINE_STREAM_INFO_IGNORE_VIDEO, XINE_STREAM_INFO_VIDEO_HAS_STILL, XINE_STREAM_INFO_SKIPPED_FRAMES, XINE_STREAM_INFO_DISCARDED_FRAMES};

printf ("Audio - Video properties ...\n");
for (t=0;t<11;t++)
	printf ("<%d = %d> ", iAudioArray[t], xine_get_stream_info (m_pXineStream, iAudioArray[t]));
printf ("\n\n");
for (t=0;t<15;t++)
	printf ("<%d = %d> ", iVideoArray[t], xine_get_stream_info (m_pXineStream, iVideoArray[t]));
printf ("\n\n");
*/
  xine_close(m_pXineStream);
}

// Function borrowed from kaffeine ...
QString XineInfo::msToTimeString(unsigned long int iMSec)
{
  int hours;
  int min;
  int sec;
  int my_msec=iMSec;
  QString tmp;
  QString t;

    // the extra 500 will ensure that we are rounding correctly.
    iMSec = (iMSec+500)/1000;  //sec
    hours = iMSec/3600;
    my_msec -= hours*3600*1000;
    t = t.setNum(hours);
    t.append(":");

    iMSec = iMSec - (hours*3600);
    min = iMSec / 60;
    my_msec -= min*60*1000;
    tmp = tmp.setNum(min);
    //tmp = tmp.rightJustify(2, '0');	//ooo
    tmp = tmp.rightJustified(2, '0');	//xxx
    t.append(tmp);
    t.append(":");

    sec = iMSec - (min*60);
    my_msec -= sec*1000;
    
    tmp = tmp.setNum(sec);
    //tmp = tmp.rightJustify(2, '0');	//ooo
    tmp = tmp.rightJustified(2, '0');	//xxx
    t.append(tmp);

   return t;
}

char * XineInfo::get_fourcc_string (int f)
{	// from gtk-xine.c rev 1.2
	static char fcc[5];
	memset(&fcc, 0, sizeof(fcc));
	
	/* should we take care about endinaness ? */
	fcc[0] = f      | 0xFFFFFF00;
	fcc[1] = f>>8   | 0xFFFFFF00;
	fcc[2] = f>>16  | 0xFFFFFF00;
	fcc[3] = f>>24  | 0xFFFFFF00;
	fcc[4] = 0;

	if (f <= 0xFFFF)
		snprintf (fcc, 5, "0x%x", f);
		
	if  ( (fcc[0] == 'm') && (fcc[1] == 's') )
	{
//		if ((fcc[2] = 0x0) && (fcc[3] == 0x55)) Original
		if ((fcc[2] == 0x0) && (fcc[3] == 0x55))
			*(uint32_t *)fcc = 0x33706d2e; /* Force to '.mp3' */
	}
	return (char *)&fcc[0];
}

xine_stream_t *XineInfo::getXineStream ()
{
	return m_pXineStream;
}

QImage XineInfo::getScreenshot ( long iMSecOffset )
{
  //if ( ! xine_open ( m_pXineStream, (const char *)m_qsFileName ) )	//ooo
  if ( ! xine_open ( m_pXineStream, m_qsFileName.toLatin1().data() ) )	//xxx
    return getScreenshot ( m_qsFileName );

  xine_play ( m_pXineStream, 0, iMSecOffset );
  xine_stop ( m_pXineStream );

  QImage theScreenshot = getScreenshot ( m_qsFileName );  //ooo
  //QImage *theScreenshot;    //xxx
  //theScreenshot = new QImage (getScreenshot ( m_qsFileName ));  //xxx
  //QImage theScreenshot(getScreenshot ( m_qsFileName ));     //xxx

  xine_close ( m_pXineStream );

  return theScreenshot;   //ooo
  //return *theScreenshot;    //xxx
}

QImage XineInfo::getScreenshot(QString)
{
	uchar *pRgbPile = NULL;
	int iWidth, iHeight;
	double fScaleFactor;

	GetScreenshot (pRgbPile, iWidth, iHeight, fScaleFactor);
	
	if (!pRgbPile)
		return QImage();
	//QImage screenShot(pRgbPile, iWidth, iHeight, 32, 0, 0, QImage::IgnoreEndian);	//ooo
	//QImage screenShot(pRgbPile, iWidth, iHeight, QImage::Format_ARGB32_Premultiplied, 0, 0);	//xxx
    QImage screenShot(pRgbPile, iWidth, iHeight, QImage::Format_RGB32, 0, 0);	//xxx
    //QImage screenShot(pRgbPile, iWidth, iHeight, QImage::Format_ARGB32);	//xxx
    //screenShot.setColorCount(32);   //xxx
    
    //QImage image = screenShot;  //xxx
    
	// not neccesary since this woul scale to the window extend which we don't want.
//	if (fScaleFactor >= 1.0)
//		iWidth = (int)((double) iWidth * fScaleFactor);
//	else
//		iHeight = (int) ((double) iHeight / fScaleFactor);
    
	// need this step to copy the data over and beeing able
	// to delete the temp data (pRgbPile) without the application crashing.
	//screenShot = screenShot.smoothScale(iWidth, iHeight);		//ooo
	//screenShot = screenShot.scaled(iWidth, iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);		//xxx
    screenShot = screenShot.copy(0, 0, iWidth, iHeight);		//xxx
    //screenShot = screenShot.copy();		//xxx
    //screenShot = screenShot.scaled(150, 150);		//xxx
    //QImage screenShot_ = screenShot;		//xxx
    
	delete []pRgbPile;	
	return screenShot;	//ooo
    //return screenShot_;	//xxx
}

void XineInfo::GetScreenshot(uchar*& rgb32BitData, int& videoWidth, int& videoHeight, double& scaleFactor)
{    
  uint8_t   *yuv = NULL, *y = NULL, *u = NULL, *v =NULL;
  
  int        width, height, ratio, format;
  double     desired_ratio, image_ratio;

  if (!xine_get_current_frame (m_pXineStream, &width, &height, &ratio, &format, NULL))
    return;

  yuv = new uint8_t[((width+8) * (height+1) * 2)];
  if (yuv == NULL)
    {
      printf ("Not enough memory to make screenshot!\n");
      return;
    }  

  xine_get_current_frame (m_pXineStream, &width, &height, &ratio, &format, yuv);
  
  videoWidth = width;
  videoHeight = height;

 /*
  * convert to yv12 if necessary
  */

  switch (format) {
  case XINE_IMGFMT_YUY2:
    {
      uint8_t *yuy2 = yuv;

      yuv = new uint8_t[(width * height * 2)];
      if (yuv == NULL)
      {
//        errorOut("Not enough memory to make screenshot!");
        return;
      }  
      y = yuv;
      u = yuv + width * height;
      v = yuv + width * height * 5 / 4;

      yuy2Toyv12 (y, u, v, yuy2, width, height);

      delete [] yuy2;
    }
    break;
  case XINE_IMGFMT_YV12:
    y = yuv;
    u = yuv + width * height;
    v = yuv + width * height * 5 / 4;

    break;
  default:
    {
      printf ("Screenshot: Format %s not supported!\n", (char*)&format);
      delete [] yuv;
      return;
    }  
  }

 /*
  * convert to rgb
  */

  rgb32BitData = yv12ToRgb (y, u, v, width, height);


  image_ratio = (double) width / (double) height; //ooo
  //image_ratio = static_cast<double> ( width ) / static_cast<double> ( height );   //xxx

  switch (ratio) {
  case XINE_VO_ASPECT_ANAMORPHIC:  // * anamorphic  * /
//  case XINE_VO_ASPECT_PAN_SCAN:  // * depreciated * /
    desired_ratio = 16.0 /9.0;
    break;
  case XINE_VO_ASPECT_DVB:         // * 2.11:1 * /
    desired_ratio = 2.11/1.0;
    break;
  case XINE_VO_ASPECT_SQUARE:        // * square pels * /
//  case XINE_VO_ASPECT_DONT_TOUCH:  // * depreciated * /
    desired_ratio = image_ratio;
    break; 
  default:
//    printf ("Screenshot: Unknown aspect ratio: %d - using 4:3", ratio);
  case XINE_VO_ASPECT_4_3:         // * 4:3   * /
    desired_ratio = 4.0 / 3.0;
    break;
  }

  scaleFactor = desired_ratio / image_ratio;

  delete [] yuv;
}

/************************************************************
 *   Helpers to convert yuy and yv12 frames to rgb          *
 *   code from gxine modified for 32bit output              *
 *   Copyright (C) 2000-2003 the xine project               *
 ************************************************************/

void XineInfo::yuy2Toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input,
        int width, int height)
{

  int    i, j, w2;

  w2 = width / 2;

  for (i = 0; i < height; i += 2) {
    for (j = 0; j < w2; j++) {
      /*
       * packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
       */
      *(y++) = *(input++);
      *(u++) = *(input++);
      *(y++) = *(input++);
      *(v++) = *(input++);
    }

    /*
     * down sampling
     */

    for (j = 0; j < w2; j++) {
      /*
       * skip every second line for U and V
       */
      *(y++) = *(input++);
      input++;
      *(y++) = *(input++);
      input++;
    }
  }
}

/*
 *   Create rgb data from yv12
 */

#define clip_8_bit(val)              \
{                                    \
   if (val < 0)                      \
      val = 0;                       \
   else                              \
      if (val > 255) val = 255;      \
}

uchar *XineInfo::yv12ToRgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v,
           int width, int height)
{
  int     i, j;

  int     y, u, v;
  int     r, g, b;

  int     sub_i_uv;
  int     sub_j_uv;

  int     uv_width, uv_height;

  uchar *rgb;

  uv_width  = width / 2;
  uv_height = height / 2;

  rgb = new uchar[(width * height * 4)]; //qt needs a 32bit align
  if (!rgb)
  {
//    kdError(555) << "Not enough memory!" << endl;
    return NULL;
  }

  for (i = 0; i < height; ++i) {
    /*
     * calculate u & v rows
     */
    sub_i_uv = ((i * uv_height) / height);

    for (j = 0; j < width; ++j) {
      /*
       * calculate u & v columns
       */
      sub_j_uv = ((j * uv_width) / width);

      /***************************************************
       *
       *  Colour conversion from
       *    http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
       *
       *  Thanks to Billy Biggs <vektor@dumbterm.net>
       *  for the pointer and the following conversion.
       *
       *   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
       *   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
       *   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
       *
       *  Where in xine the above values are represented as
       *
       *   Y' == image->y
       *   Cb == image->u
       *   Cr == image->v
       *
       ***************************************************/

      y = src_y[(i * width) + j] - 16;
      u = src_u[(sub_i_uv * uv_width) + sub_j_uv] - 128;
      v = src_v[(sub_i_uv * uv_width) + sub_j_uv] - 128;

      //r = (int)((1.1644 * (double)y) + (1.5960 * (double)v)); //ooo
      r = static_cast<int>((1.1644 * static_cast<double>(y)) + (1.5960 * static_cast<double>(v)));  //xxx
      //g = (int)((1.1644 * (double)y) - (0.3918 * (double)u) - (0.8130 * (double)v));  //ooo
      g = static_cast<int>((1.1644 * static_cast<double>(y)) - (0.3918 * static_cast<double>(u)) - (0.8130 * static_cast<double>(v)));   //xxx
      //b = (int)((1.1644 * (double)y) + (2.0172 * (double)u)); //ooo
      b = static_cast<int>((1.1644 * static_cast<double>(y)) + (2.0172 * static_cast<double>(u)));  //xxx
      
      clip_8_bit (r);
      clip_8_bit (g);
      clip_8_bit (b);

      rgb[(i * width + j) * 4 + 0] = b;
      rgb[(i * width + j) * 4 + 1] = g;
      rgb[(i * width + j) * 4 + 2] = r;
      rgb[(i * width + j) * 4 + 3] = 0;

    }
  }  
  return rgb;
}


