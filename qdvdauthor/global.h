/***************************************************************************
    global.h
                             -------------------
    Global variables                         
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class collects all possible information about
   the current set stream.
    
****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

//#include <qstring.h>			//ooo
#include <QString>			//xxx
//#include <qeventloop.h>		//ooo
#include <QEventLoop>			//xxx
#include <QList>
// Include the logging facilities ...
#include "utils.h"			//ooo
#include "log.h"

// Forward declartion of the Thumbs cache class
class SlideDefaults;
namespace Cache {
   class Thumbs;
};

#define DEBUG_INFO 1

// Here we define all external tools neccesary
// Format = executable, Display Name, Explanation of function
#define EXTERNAL_TOOLS \
		{"bash",          "Linux",          "Mandatory !\n\nYou need bash in order to execute the generated scripts."}, \
		{"dvdauthor",     "DVDAuthor",      "Mandatory !\n\nThis is the main application needed to create a Movie DVD.\nhttp://dvdauthor.sf.net"}, \
		{"spumux",        "Spumux",         "Mandatory !\n\nThis program is part of the dvdauthor package.\nhttp://dvdauthor.sf.net"}, \
		{"mplex",         "MPlex",          "Mandatory !\n\nThis program is part of the dvdauthor package.\nhttp://dvdauthor.sf.net"}, \
		{"qrender",       "QRender",        "Mandatory !\n\nThis program is part of QDVDAuthor package.\nhttp://qdvdauthor.sf.net"}, \
		{"dv2sub",        "DV to Sub",      "Optional. Important.\n\nThis program will generate subtitles from the meta information in a DV stream.\nhttp://dv2sub.sf.net"}, \
		{"arecord",       "ARecord",        "Optional.\n\nALSA sound utility. There are other utilities you can use instead."}, \
		{"movie-to-dvd",  "VideoTrans",     "Optional. Important.\n\nYou can also use MEncoder, or transcode.\nThis utility uses MPlayer to convert movies into the correct format (See also man pages).\nhttp://videotrans.sf.net"}, \
		{"transcode",     "Transcode",      "Optional. Important.\n\nYou can also use MEncoder.\nThis utility converts movies into the correct format (See also man pages).\nhttp://www.transcoding.org"}, \
		{"mencoder",      "MEncoder",       "Optional.\n\nThis utility is part of the mplayer utilities. It can be used to convert  video into DVD confoming mpeg2.\nThis will be implemented in one of the next versions of 'Q' DVD-Author.\nhttp://www.mplayerhq.hu"}, \
		{"ffmpeg",        "FFMpeg",         "Optional.\n\nThis utility is part of the mplayer utilities. It can be used to convert  video into DVD confoming mpeg2.\nThis will be implemented in one of the next versions of 'Q' DVD-Author.\nhttp://ffmpeg.mplayerhq.hu"}, \
		{"png2yuv",       "PNG To YUV",     "Optional. Important.\n\nThis utility is used to create the mpeg2 video stream for Movie Menus.\nNote: without this tool you can not create MovieMenus.\nhttp://netpbm.sf.net"}, \
		{"oggdec",        "Ogg Dec",        "Optional.\n\nThis utility will help to convert OGG audio files into a DVD conform format.\nThis will be implemented in the next release of 'Q' DVD-Author.\nogg to mp2 > oggdec test.ogg -o - | mp2enc -o test2.mp2"}, \
		{"mp2enc",        "mp2 Enc",        "Optional !\n\nYou can also use twolame instead.\nThis utility converts a audio file into DVD conforming mp2 format."}, \
		{"lame",          "Lame",           "Optional.\n\nNot used in the standard settings ... So why is this tool here ?\nmp3 to mp2 > lame --decode blues.mp3 - | mp2enc -o blues.mp2"}, \
		{"mplayer",       "MPlayer",        "Optional.\n\nThis program / utility can be used to replace the xine engine.\nThis will be implemented in one of the next versions of 'Q' DVD-Author.\nmp3 to mp2 > mplayer mymusicfile.mp3 -vo null -ao pcm -aofile menu_audio.pcm ; mp2enc -r 48000 -o menu_audio.mpa < menu_audio.pcm\nhttp://www.mplayerhq.hu"}, \
		{"jpegtopnm",     "JPG To PNM",     "Optional.\n\nYou can also use jpeg2yuv instead.\nThis utility is used to convert a background image into a mpeg2 movie."}, \
		{"ppmtoy4m",      "PPM To Y4M",     "Optional.\n\nYou can also use mpeg2enc instead.\nThis utility i sused to convert a background image into a mpeg2 movie."}, \
		{"mpeg2enc",      "MPEG2 Enc",      "Mandatory.\n\nThis utility is used to convert a background image into a mpeg2 movie.\nhttp://mjpeg.sf.net"}, \
		{"jpeg2yuv",      "JPG To YUV",     "Optional.\n\nYou can also use jpegtopnm.\nThis utility is used to convert a background image into a mpeg2 movie."}, \
		{"pcm2aiff",      "PCM To AIFF",    "Optional.\n\nNot used in the standard settings ... So why is this tool here ?"}, \
		{"twolame",       "TwoLame",        "Optional.\n\n"}, \
		{"mctoolame",     "MC TooLame",     "Optional.\n\n"}, \
		{"dd",            "dd",             "Optional.\n\n"}, \
		{"dvd-slideshow", "DVD-Slideshow",  "Optional. Important.\n\nThere is no substiutute for this utility.\nDVD-Slideshow is a bash script which generates the slideshow using the sox, convert, pngtopnm, mpeg2enc, and mp2enc utilities.\nhttp://dvd-slideshow.sf.net"}, \
		{"sox",           "SOund eXchange", "Optional. Important.\n\nThis tool is used by dvd-slideshow to generate the background sound for the slideshow.\nhttp://sox.sf.net"}, \
		{"pngtopnm",      "PNG To PNM",     "Optional. Important.\n\nThis utility is used with dvd-slideshow to create the mpeg2 video stream.\nhttp://netpbm.sf.net"}, \
		{"convert",       "Convert",        "Optional. Important.\n\nThis utility is used with dvd-slideshow to create the mpeg2 video stream.\nhttp://imagemagick.sf.net."}, \
		{"growisofs",     "Grow ISO FS",    "Optional.\n\nThis utility is used to burn the video DVD onto the DVD."}, \
		{"dvd+rw-format", "DVD+RW Format",  "Optional.\n\nThis utility is used to burn the video DVD onto a RW+DVD."}, \
		{"mkisofs",       "Make ISO FS",    "Optional.\n\nThis utility is used to create a iso image of the final DVD (on the harddrive) to be burned with your favourite burning program."}, \
		{"dvdisaster",    "DVDisaster",     "Optional.\n\nThis utility can add ECC information to a DVD-iso.\nYou can find it at : http://www.dvdisaster.net"}, \
		{"dvdrecord",     "DVD Record",     "Optional.\n\nRecords the final DVD structure onto the DVD media.\nYou can find it at : http://www.arklinux.org/projects/dvdrtools"}, \
		{"wodim",         "Wodim",          "Optional.\n\nRecords the final DVD structure onto the DVD media.\nYou can find it at : http://www.cdrkit.org"}

#define STRING_SEPARATOR "+-+"

// The initial project file name for a new project. Need to get changed manually
#define PROJECT_FILENAME "Unnamed.xml"
#define PROJECT_NAME     "Unnamed"

// Here we define the dpeth of the undo buffer ... maybe later we can adjust this value in the setup dialog ...
#define MAX_UNDO_DEPTH 250

// Here are the defines for the different video modes
#define FORMAT_NONE		0
#define FORMAT_NTSC1		1
#define FORMAT_NTSC2		2
#define FORMAT_NTSC3		3
#define FORMAT_NTSC4		4
#define FORMAT_PAL1		5
#define FORMAT_PAL2		6
#define FORMAT_PAL3		7
#define FORMAT_PAL4		8


#define BACKGROUND_IMG		0
#define SELECTED_MASK		1
#define HIGHLIGHTED_MASK	2

#define BACKGROUND_NAME		"background.jpg"
#define SELECTED_NAME		"selected.png"
#define HIGHLIGHTED_NAME	"highlighted.png"

// The max colors a mask (button) can have.
#define MAX_MASK_COLORS 4

// Here we define the color which is to be interpreted as transparent (only used as a type of flag)
#define TRANSPARENT_COLOR        254, 255, 254

// These are the standard colors for the SELECTED, and HIGHLIGHTED layer.
// Note: max of 4 colors are allowed, so colorspace is sparse.
#define START_HIGHLIGHTED_COLOR  250,150,  50
#define START_SELECTED_COLOR     255, 20,  20

// These are the initial colors for the subtitles.
// Note: the third color ill be interpolated between BG and FG color.
//       the fourth color if transparent
//#define START_SUBTITLEFG_COLOR   233, 240, 24
#define START_SUBTITLEFG_COLOR   255, 200,  0
#define START_SUBTITLEBG_COLOR     0,   0,  0

// This color is applied to the background, thus it can be different unless the user 
// decides to put it onto a Selected / highlighted layer.
#define START_FRAME_COLOR        255, 255, 20

// These defines specify the color to use in the SourceFileToolbar
//#define COLOR_MOVIE_FILES 110, 160, 210
#define COLOR_MOVIE_FILES_OK  140, 200, 250
#define COLOR_MOVIE_FILES_BAD 200,  30,  45
#define COLOR_SOUND_FILES_OK   80,  80, 220
#define COLOR_SOUND_FILES_BAD 200,  30,  45
#define COLOR_SLIDE_FILES_OK  210, 130, 170
#define COLOR_SLIDE_FILES_BAD 225, 140, 180
#define COLOR_MOVIE_TRANSCODE 170,   0, 255
#define COLOR_AUDIO_TRANSCODE 150,  20, 235

// PI is always good to have  3.141592653589793238462643383279502884197169399375105820974944592
#ifndef PI
#define PI 3.1415926536
#endif

class MenuObject;
class QDVDAuthor;

class Global
{
public:
	 Global ();
	~Global ();
	static Log    *pLogObject;
	static QDVDAuthor *pApp;
	static QString qsBashString;
	static QString qsProjectFileName;
	static QString qsProjectName;
	static QString qsTempPath;
	static QString qsSystemPath;
	static QString qsProjectPath;
	static QString qsCurrentPath;
	static QString qsExternalPlayer;
	static QString qsLanguage;
	static QString qsDefaultLanguage; // en
	static QString qsDefaultFormat;   // PAL or NTSC
	static uint    iRegionalZone;
	static uint    iMaxRunningThreads;
	static int     iDeleteTempOlderThan;
	static int     iChildThreadPriority;
	static bool    bPNGRender;
	static QEventLoop     eventLoop;
	static MenuObject    *pCopyObject;
	static SlideDefaults *pSlideDefaults;
	// Here we store the path to the different tools required.
	static QList<Utils::toolsPaths *> listToolsPaths;	//ooo
	static Cache::Thumbs *pThumbsCache;
};

#endif // GLOBAL_H
