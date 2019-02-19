/***************************************************************************
    xml_slideshow.h
                             -------------------
    CXmlSlideshow - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is very similar to qslideshow/xml_slideshow.h
   The difference is the missing ImageFilter - object
    
****************************************************************************/

#ifndef XML_SLIDESHOW_H
#define XML_SLIDESHOW_H

// Make sure we have the QT_VERSION defined.
#include <qglobal.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

//#if (QT_VERSION > 0x0301FF)
#ifdef QDVD_RENDER
  #include <QString>
  #include <QStringList>
  #define Q3ValueList QList
  #define QT4
#else
  #include <qstring.h>
  #include <qstringlist.h>
  #define QMatrix QMatrix
#endif


#define SLIDESHOW_DOCTYPE	""
// Here we define all attributes for the structures.
#define SLIDESHOW_TAG		"slideshow"
#define SLIDESHOW_DELAY		"delay"
#define SLIDESHOW_INTRO		"intro"
#define SLIDESHOW_ASPECT	"aspect"
#define SLIDESHOW_FADE_OUT	"fade_out"
#define SLIDESHOW_FILTER_DELAY	"filter_delay"
#define SLIDESHOW_NAME		"name"
#define SLIDESHOW_XRES		"xres"
#define SLIDESHOW_YRES		"yres"
#define SLIDESHOW_BACKGROUND	"background"
#define SLIDESHOW_AUDIO_LENGTH	"audio_length"
#define SLIDESHOW_AUDIO_LIST	"audio_list"
#define SLIDESHOW_FILTER_LIST	"filter_list"
#define SLIDESHOW_KEN_BURNS	"ken_burns"
#define SLIDESHOW_IMG_BKG_IMG	"img_bkg_img"
#define IMG_NODE                "img"
#define VID_NODE                "vid"
#define VID_LEN			"length"
#define IMG_SRC			"src"
#define IMG_TEXT		"text"
#define IMG_WIDTH		"width"
#define IMG_HEIGHT		"height"
#define IMG_MATRIX		"matrix"
#define IMG_ROTATE		"rotate"
#define IMG_DURATION		"duration"
#define EFFECT_NAME		"name"
#define EFFECT_USER_DEF		"user_def"
#define EFFECT_X0		"x0"
#define EFFECT_Y0		"y0"
#define EFFECT_X1		"x1"
#define EFFECT_Y1		"y1"
#define EFFECT_XE0		"xe0"
#define EFFECT_YE0		"ye0"
#define EFFECT_XE1		"xe1"
#define EFFECT_YE1		"ye1"
#define EFFECT_SCROLL		"scroll"
#define FILTER_NAME		"name"
#define FILTER_USER_DEF		"user_def"
#define FILTER_IMG_BKG_IMG	"img_bkg_img"
#define FILTER_DURATION		"duration"
#define FILTER_SUBTITLE		"subtitle"


#define EFFECT_TYPE_KENBURNS    "kenburns"
#define EFFECT_TYPE_CROP        "crop"
#define EFFECT_TYPE_SCROLL      "scroll"
/*
<IMG ...>
	<EFFECT ...></EFFECT>
</IMG>
<FILTER ...></FILTER>
*/
// Container class for pre processing information of images, before usage.

class QMatrix;
class QDomElement;
class QDomDocument;

class CXmlSlideshow
{
public:
	class filter_struct;
	class time_object	{
		public:
		time_object ( );
		virtual ~time_object ( );
		virtual bool readXml ( QDomElement  * ) = 0;
		virtual bool writeXml( QDomDocument *, QDomElement * ) = 0;
		virtual time_object& operator  = ( time_object & ) { return *this; };
		virtual bool         operator == ( time_object & ) = 0;
		QString node_name;
		float fDuration;  // in seconds
		float fStartTime;
		float fEndTime;
		// The modifier is a internal structure to process this item before
		// continuing to the command line tools.
		// (E.g. img can have a ImageManipulator stored here)
		void *pModifier;
		filter_struct *pTransition;
	};
	class effect_struct	{
		public:
		effect_struct ( );
		virtual ~effect_struct ( ) { };
		virtual bool readXml  ( QDomElement  * );
		virtual bool writeXml ( QDomDocument *,QDomElement * );
		virtual effect_struct& operator  = ( effect_struct & );
		virtual bool           operator == ( effect_struct & );
		void randomKenBurns   ( float, int, int, bool b=false );
		bool isDefault        ( );
		QString node_name;  // effect
		QString name;       // either crop/kenburns/scroll
		bool bUserDefined;
		int x0;  // KenBurns Start upper left  corner
		int y0;  // KenBurns Start upper left  corner
		int x1;  // KenBurns Start lower right corner
		int y1;  // KenBurns Start lower right corner
		int xe0; // KenBurns Stop  upper left  corner
		int ye0; // KenBurns Stop  upper left  corner
		int xe1; // KenBurns Stop  upper right corner
		int ye1; // KenBurns Stop  upper right corner
		QString scroll;	// either left or right
	};
	class img_struct : public time_object
	{
		public:
		img_struct ( );
		virtual ~img_struct ( );
		virtual bool readXml  ( QDomElement  * );
		virtual bool writeXml ( QDomDocument *, QDomElement * );
		virtual time_object& operator  = ( time_object & );
		virtual bool         operator == ( time_object & );
		virtual time_object& operator  = ( img_struct  &obj ) { return operator =((time_object &)obj); };
		virtual bool         operator == ( img_struct  &obj ) { return operator==((time_object &)obj); };
		QString src;	// the location of this image.
		QString text;	// the text of this image.
		int     width;
		int     height;
		float   rotate;
		QMatrix *pMatrix;
		effect_struct effect[2]; // start / end
	};
	class vid_struct : public img_struct
	{
		public:
		vid_struct ( );
		virtual ~vid_struct ( );
		virtual bool readXml  ( QDomElement  * );
		virtual bool writeXml ( QDomDocument *, QDomElement * );
		virtual time_object& operator  = ( time_object & );
		virtual bool         operator == ( time_object & );
		virtual time_object& operator  = ( vid_struct  &obj ) { return operator =((time_object &)obj); };
		virtual bool         operator == ( vid_struct  &obj ) { return operator==((time_object &)obj); };
		unsigned long length; // in MSeconds
	};
	class filter_struct : public time_object
	{
		public:
		filter_struct ( );
		virtual bool readXml  ( QDomElement  * );
		virtual bool writeXml ( QDomDocument *, QDomElement * );
		virtual time_object& operator  = ( time_object & );
		virtual bool         operator == ( time_object & );
		virtual time_object& operator  = ( filter_struct  &obj ) { return operator =((time_object &)obj); };
		virtual bool         operator == ( filter_struct  &obj ) { return operator==((time_object &)obj); };
		QString name;  // the type of filter, e.g. crossfade/fadein/fadeout
		QString subtitle;
		bool bUserDefined;
		bool bImgBackgroundImg;
	};
public:
	 CXmlSlideshow ( );
	~CXmlSlideshow ( );

	bool readXml   ( );
	bool readXml   ( QString     &, bool b=true );
	bool readXml   ( QDomElement * );
	bool writeXml  ( );
	bool writeXml  ( QString     & );
	bool writeXml  ( QDomElement * );
	// copy the object over ...
	CXmlSlideshow &operator =  ( CXmlSlideshow & );
	bool           operator == ( CXmlSlideshow & );

	CXmlSlideshow::time_object   *getTimeObject ( uint );
	CXmlSlideshow::img_struct    *addImg        ( );
	CXmlSlideshow::img_struct    *getImg        ( uint );
	CXmlSlideshow::vid_struct    *addVid        ( );
	CXmlSlideshow::vid_struct    *getVid        ( uint );
	CXmlSlideshow::filter_struct *addFilter     ( );
	CXmlSlideshow::filter_struct *getFilter     ( uint );

	bool insertImg ( uint, img_struct * );
	bool insertVid ( uint, vid_struct * ); 
	void delImg    ( img_struct * );
	void delVid    ( vid_struct * );
	int  findImg   ( img_struct * );

	void clear       ( );
	uint count       ( );
	uint countImg    ( );
	uint countVid    ( );
	uint countFilter ( );
	void clearObjects( );

	uint vidsLength  ( ); // total play time of all vids in seconds
	uint slideLength ( ); // total play time
	uint audioLength ( ); // total play time of all audio files in seconds

	bool imgBkgImg   ( );
	void setImgBkgImg( bool );
	bool kenBurns    ( );
	void setKenBurns ( bool );

public:
	QString     node_name;
	QString     background;
	QStringList audio_list;
	QStringList validFilterNames;
	QString     slideshow_name;
	float delay;             // in seconds
	float filter_delay;      // in seconds
	int   xres;              // Determines the resolution as well as PAL vs NTSC
	int   yres;
	int   aspect;            // Currently only 0=4:3 or 1=16:9
	bool  intro_page;
	bool  fade_out;
	uint  total_audio_length; // in seconds.
	bool  bImgBackgroundImg;
	bool  bKenBurns;
	// Note: filter and img are on the same level, to get a continous structure as stored in the file we need to check the iPosition of those structures ...
	QList<time_object   *> m_listTimeObjects;		//oxx
	QList<filter_struct *> m_listFilter;			//oxx
	QList<img_struct    *> m_listImages;			//oxx
	QList<vid_struct    *> m_listVids;			//oxx
	void *id; // used to ID where the Slideshow object belongs to
};

/////////////////////////////////////////////////////////
//
// Following is the structure of spumux-xml files.
//
/////////////////////////////////////////////////////////
//<slideshow delay="sec" background="background.jpg">
// <img src="img_name" width="width" height="height />
// <img ... >
//</slideshow>
/////////////////////////////////////////////////////////

#endif // XML_SLIDESHOW_H

