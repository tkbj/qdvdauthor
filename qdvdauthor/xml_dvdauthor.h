/***************************************************************************
    xml_dvdauthor.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file contains the import filter for
   dvdauthor-xml files, and spumux-xml files.
    
****************************************************************************/

#ifndef XML_DVDAUTHOR_H
#define XML_DVDAUTHOR_H

#include <QString>
#include <QDomDocument>
#include <QDomElement>

#define DVD_DOCTYPE              ""
// Here we define all attributes for the structures.
#define DVDAUTHOR_DEST           "dest"
#define DVDAUTHOR_JUMPPAD        "jumppad"
#define MENUS_LANG               "lang"
#define VIDEO_FORMAT             "format"
#define VIDEO_ASPECT             "aspect"
#define VIDEO_RESOLUTION         "resolution"
#define VIDEO_CAPTION            "caption"
#define VIDEO_WIDESCREEN         "widescreen"
#define AUDIO_FORMAT             "format"
#define AUDIO_CHANNELS           "channels"
#define AUDIO_QUANT              "quant"
#define AUDIO_DOLBY              "dolby"
#define AUDIO_LANG               "lang"
#define SUBPICTURE_LANG          "lang"
#define PGC_ENTRY                "entry"
#define PGC_PALETTE              "palette"
#define PGC_PAUSE                "pause"
#define BUTTON_NAME              "name"
#define VOB_FILE                 "file"
#define VOB_CHAPTERS             "chapters"
#define VOB_PAUSE                "pause"
#define CELL_START               "start"
#define CELL_END                 "end"
#define CELL_CHAPTER             "chapter"
#define CELL_PROGRAM             "program"
#define CELL_PAUSE               "pause"
// Here are the identifiers for spumux
#define SPU_START                "start"
#define SPU_END                  "end"
#define SPU_IMAGE                "image"
#define SPU_HIGHLIGHT            "highlight"
#define SPU_SELECT               "select"
#define SPU_TRANSPARENT          "transparent"
#define SPU_FORCE                "force"
#define SPU_AUTOOUTLINE          "autooutline"
#define SPU_OUTLINEWIDTH         "outlinewidth"
#define SPU_AUTOORDER            "autoorder"
#define SPU_XOFFSET              "xoffset"
#define SPU_YOFFSET              "yoffset"
#define TEXTSUB_FILENAME         "filename"
#define TEXTSUB_CHARACTERSET     "characterset"
#define TEXTSUB_FONT             "font"
#define TEXTSUB_TRANSPARENT      "transparent"
#define TEXTSUB_FONTSIZE         "fontsize"
#define TEXTSUB_HORIZ_ALIGNMENT  "horizontal-alignment"
#define TEXTSUB_VERT_ALIGNMENT   "vertical-alignment"
#define TEXTSUB_LEFT_MARGIN      "left-margin"
#define TEXTSUB_RIGHT_MARGIN     "right-margin"
#define TEXTSUB_BOTTOM_MARGIN    "bottom-margin"
#define TEXTSUB_TOP_MARGIN       "top-margin"
#define TEXTSUB_SUBTITLE_FPS     "subtitle-fps"
#define TEXTSUB_MOVIE_FPS        "movie-fps"
#define TEXTSUB_MOVIE_WIDTH      "movie-width"
#define TEXTSUB_MOVIE_HEIGHT     "movie-height"
#define BUTTON_LABEL             "name"
#define BUTTON_X0                "x0"
#define BUTTON_Y0                "y0"
#define BUTTON_X1                "x1"
#define BUTTON_Y1                "y1"
#define BUTTON_UP                "up"
#define BUTTON_DOWN              "down"
#define BUTTON_LEFT              "left"
#define BUTTON_RIGHT             "right"
#define ACTION_LABEL             "name"
#define ACTION_LABEL_OLD         "label"

class CXmlDVDAuthor
{
private:
  class base_struct {
  public: 
    base_struct ()
      {node_name=QString  ("unknown");};
    virtual ~base_struct  ( )       { };
    virtual bool readXml  ( QDomElement * ) = 0;
    virtual bool writeXml ( QDomDocument *, QDomElement * ) = 0;
    QString node_name;	// Name of this node ...
    QString comment;    // add "<!-- comment // -->"
  };
public:
        class video_struct : public base_struct	{
                public:
                video_struct ()
		  {node_name=QString("video");};
		virtual ~video_struct () {};
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString format;		// [ntsc|pal]
		QString aspect;		// [4:3|16:9]
		QString resolution;	// [XxY]
		QString caption;	// [field1|field2]
		QString widescreen;	// [nopanscan|noletterbox]
        };
        class audio_struct : public base_struct	{
		public:
		audio_struct ()
		{node_name=QString("audio"); channels=0;};
		virtual  ~audio_struct () {};
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString format;		// [mp2|ac3|dts|pcm]
		uint	channels;	// number of channels
		QString quant;		// [16bps|2-bps|24bps|drc]
		QString dolby;		// [surround]
		QString lang;		// language
	};
        class subpicture_struct : public base_struct  {
		public:
		subpicture_struct ()
		{node_name=QString("subpicture");};
		virtual ~subpicture_struct () {};
		virtual bool readXml (QDomElement  * );
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString lang;		// language
	};
        class fpc_struct : public base_struct  {
		public:
		fpc_struct ()
		{node_name=QString ("fpc");};
		virtual ~fpc_struct  ( ) { };
		virtual bool readXml ( QDomElement  * );
		virtual bool writeXml( QDomDocument *, QDomElement *);
		QString value; // commands to be executed once at startup
	};
        class pre_struct : public base_struct  {
		public:
		pre_struct ()
		{node_name=QString("pre");};
		virtual ~pre_struct () {};
		virtual bool readXml(QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString value;		// commands to be executed before.
	};
        class cell_struct : public base_struct  {
		public:
		cell_struct ()
		{node_name=QString("cell");};
		virtual ~cell_struct () {};
		virtual bool readXml(QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString start;		// "0:00:00.000"
		QString end;		// "-1"
		QString chapter;	// [1|ON|YES]
		QString program;	// "1"
		QString pause;		// [seconds|inf]
	};
        class vob_struct : public base_struct {
		public:
		vob_struct ()
		{node_name=QString("vob");};
		virtual ~vob_struct () {};
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		virtual bool operator ==( vob_struct & );
		QString file;		// file.mpg
		QString chapters;	// chapters - list
		QString pause;		// [seconds|inf]
		cell_struct cell;
	};
        class button_struct : public base_struct  {
		public:
		button_struct ()
		{node_name=QString("button");};
		virtual ~button_struct () {};
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString value;		// commands to be executed.
		QString name;		// button name
	};
        class post_struct : public base_struct  {
		public:
		post_struct ()
		{node_name=QString("post");};
		virtual ~post_struct () {};
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString value;		// comands to be executed after ...
	};
	// Okay, the following structs will generate the structure of the xml - file.
	class pgc_struct : public base_struct {
		public:
		         pgc_struct ();
		virtual ~pgc_struct ();
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		button_struct *addButton();
		vob_struct *addVob();
		// attributes
		QString entry;		// [title|root|subtitle|audio|angle|ptt]
		QString palette;	// [yuv-file|rgb-file]
		QString pause;		// [seconds|inf]
		// nodes
		pre_struct	pre;
		vob_struct	**ppArrayVob;
		button_struct	**ppArrayButton;
		post_struct	post;
	};
	class menus_struct : public base_struct {
		public:
		         menus_struct ();
		virtual ~menus_struct ();
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		pgc_struct *addPgc ();

		QString 	lang;		// language-code
		video_struct	video;
		audio_struct	audio[8];
		subpicture_struct subpicture[32];
		pgc_struct	**ppArrayPgc;
	};
	class titles_struct : public base_struct {
		public:
		         titles_struct ();
		virtual ~titles_struct ();
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		pgc_struct *addPgc ();
		int pgcCount ( );

		QString		titles;
		video_struct	video;
		audio_struct	audio[8];
		subpicture_struct   subpicture[32];
		pgc_struct	  **ppArrayPgc;
	};
	class vmgm_struct : public base_struct {
		public:
		         vmgm_struct ();
		virtual ~vmgm_struct ();
		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		menus_struct  *addMenus();
		
		fpc_struct   fpc;
		menus_struct **ppArrayMenus;
	};
	class titleset_struct : public base_struct {
		public:
		         titleset_struct     ();
		virtual ~titleset_struct     ();
		menus_struct  *addMenus      ();
		titles_struct *addTitles     ();

		virtual bool readXml (QDomElement  *);
		virtual bool writeXml(QDomDocument *, QDomElement *);

		menus_struct	**ppArrayMenus;
		titles_struct	**ppArrayTitles;
	};
	class dvdauthor_struct : public base_struct {
		public:
                          dvdauthor_struct   ( );
		 virtual ~dvdauthor_struct   ( );
		// public functions ...
		virtual bool readXml ( QDomElement  * );
		virtual bool writeXml( QDomDocument *, QDomElement *p=NULL );
		titleset_struct *addTitleset ( );
		// dvdauthor attributes
		QString     dest;    // output dir
		QString     jumppad; // [1|on|yes]
		vmgm_struct vmgm;
		titleset_struct **ppArrayTitleset;
	};
public:
	 CXmlDVDAuthor ( );
	~CXmlDVDAuthor ( );

	bool readXml   ( );
	bool readXml   ( QString &fileName );
	bool writeXml  ( );
	bool writeXml  ( QString &fileName );
	dvdauthor_struct *getDVDAuthor   ( );

private:
	// Private functions

//private:
public:
  dvdauthor_struct m_dvdauthor;  // This struct holds the whole structure
                                 // of the xml file for dvdauthor.
};

/////////////////////////////////////////////////////////
//
// Following is the structure of dvdauthor-xml files.
//
/////////////////////////////////////////////////////////
//<dvdauthor [dest="output-dir"] [jumppad="1|on|yes"] >
//   <vmgm>
//     <menus [lang="language-code"] >
//         <video [format="ntsc|pal"] [aspect="4:3|16:9"]
//                [resolution="XxY"] [caption="field1|field2"]
//                [widescreen="nopanscan|noletterbox"] />
//         <audio [format="mp2|ac3|dts|pcm"] [channels="numchannels"]
//                [quant="16bps|20bps|24bps|drc"] [dolby="surround"]
//                [lang="language"] />
//         [<audio ... />]
//         <subpicture lang="language" />
//         <pgc [entry="title"] [palette="yuvfile|rgbfile"]
//              [pause="seconds|inf"]>
//            <pre> commands; </pre>
//            <vob file="file.mpg" [chapters="chapter-list"]
//                 [pause="seconds|inf"] />
//            [<vob ... />]
//            <button [name="buttonname"]> commands; </button>
//            [<button ... />]
//            <post> commands; </post>
//          </pgc>
//         [<pgc ... />]
//      </menus>
//   </vmgm>
//   <titleset>
//      <menus>
//         [<video ... />]
//         [<audio ... />]
//         <pgc [entry="root|subtitle|audio|angle|ptt"]
//              [palette="yuvfile|rgbfile"] [pause="seconds|inf"]>
//            [...]
//         </pgc>
//         [<pgc ... />]
//      </menus>
//      <titles>
//         [<video ... />]
//         [<audio ... />]
//         <pgc [palette="yuvfile|rgbfile"] [pause="seconds|inf"]>
//            [...]
//         </pgc>
//         [<pgc ... />]
//      </titles>
//   </titleset>
//   [<titleset ... />]
//</dvdauthor>
/////////////////////////////////////////////////////////
class CXmlSpumux
{
private:
  class base_struct {
  public:
    base_struct ()
    { node_name=QString("unknown");};
    virtual ~base_struct () {};
    virtual bool readXml  (QDomElement *) = 0;
    virtual bool writeXml (QDomDocument *, QDomElement *) = 0;
    QString node_name;
  };
public:
	class button_struct : public base_struct {
		public:
                         button_struct ();
		virtual ~button_struct () {};
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString label; // Button text
		int	x0;    // coordinates.
		int	y0;    //
		int	x1;    //
		int	y1;    //
		QString up;    // Label for up-button
		QString down;  // Label for down-button
		QString left;  // Label for ???
		QString right; // Label for ???
	};
	class action_struct : public base_struct {
		public:
		action_struct ()
		{node_name = QString ("action");}
		virtual ~action_struct () {};
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString label;	// the label for this action
	};
	class textsub_struct : public base_struct { 
		public:
                textsub_struct ();
		virtual ~textsub_struct ();
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		QString filename;
		QString characterset;
		int     fontsize;
		QString horizontal_alignment;
		QString vertical_alignment;
		int     left_margin;
		int     right_margin;
		int     bottom_margin;
		int     top_margin;
		QString font;
		float	subtitle_fps;
		float	movie_fps;
		int     movie_width;
		int     movie_height;
		QString transparent;
	};
	// And here we build the file structure of teh xml file for spumux
	class spu_struct : public base_struct {
		public:
		         spu_struct ();
		virtual ~spu_struct ();
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		button_struct *addButton ();
		action_struct *addAction ();
		QString start;			// [start-time] E.g. 00:00:00.0
		QString end;			// [end-time]
		QString image;			// picture.png
		QString highlight;		// picture.png
		QString select;			// picture.png
		QString transparent;	// [color-code] E.g. 0xRRGGBB
		QString force;			// [yes/no]
		QString autooutline;	// [infer]
		int	outlinewidth;
		QString autoorder;		// [rows|columns]
		int	xoffset;			//
		int	yoffset;			//
		button_struct	**ppArrayButton;
		action_struct	**ppArrayAction;
	};
	class	stream_struct : public base_struct {
		public:
                         stream_struct ();
		virtual ~stream_struct ();
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *);
		spu_struct	 *addSpu     ();
		spu_struct	**ppArraySpu;
		textsub_struct	 *addTextsub ();
		textsub_struct	**ppArrayTextsub;
	};
	class subpictures_struct : public base_struct {
		public:
		subpictures_struct ()
		  {node_name = QString ("subpictures");}
		virtual ~subpictures_struct () {};
		virtual bool readXml (QDomElement *);
		virtual bool writeXml(QDomDocument *, QDomElement *p=NULL);
		stream_struct	stream;
	};
public:
	 CXmlSpumux ();
	~CXmlSpumux ();

	bool readXml  ();
	bool readXml  (QString &fileName);
	bool writeXml ();
	bool writeXml (QString &fileName);

public:
	subpictures_struct	m_subpictures;	// This struct holds the full xml file for spumux.
};


/////////////////////////////////////////////////////////
//
// Following is the structure of spumux-xml files.
//
/////////////////////////////////////////////////////////
//<subpictures>
//   <stream>
//      <spu start="start-time" [ end="end-time" ] [ image="picture.png" ]
//           [ highlight="picture.png" ] [ select="picture.png" ]
//           [ transparent="color-code" ] [ force="yes" ]
//           [ autooutline="infer" [ outlinewidth="width" ]
//             [ autoorder="rows" | autoorder="columns" ] ]
//           [ xoffset="x-coord" yoffset="y-coord" ] >
//         <button [ label="name" ] [ x0="x0" ] [ y0="y0" ] [ x1="x1" ]
//                 [ y1="y1" ] [ up="label" ] [ down="label" ]
//                 [ left="label" ] [ right="label" ] />
//         <action [ label="name" ] />
//      </spu>
//   </stream>
//</subpictures>
/////////////////////////////////////////////////////////

#endif // XML_DVDAUTHOR_H
