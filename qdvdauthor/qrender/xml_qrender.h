/*****************************************************************************
 ** Class XmlQRender
 **
 ** This class reads / writes the xml file for the QRender executable
 ** The format was taken from the MakeMovingMenus - project 
 ** (http://jensge.org/video/mmm/)
 **
 ** NOTE: The actual implementation was done independand as I were missing
 **       some dependencies to make mmm work on my box.
 *****************************************************************************/


#ifndef XMLQRENDER
#define XMLQRENDER

#define NODE_CLIP          "clip"
#define NODE_MMM           "mmm" 
#define NODE_QRENDER       "qrender" 

#define QRENDER_TMPDIR     "tmpdir"
#define QRENDER_RESOLUTION "resolution"
#define QRENDER_FORMAT     "format"
#define QRENDER_IMAGES     "images"
#define QRENDER_SUCCESSIVE "successive"
#define QRENDER_BACKGROUND "background" // Background image of background movie
#define QRENDER_FRAMES     "frames"     // Number of frames to extract.
#define QRENDER_STARTFRAME "startframe" // uint (offset to extrack from background)
#define QRENDER_LENGTH     "length"     // float in seconds
#define QRENDER_OFFSET     "offset"     // float in seconds (offset to extrack from background)
#define QRENDER_OUTPUT     "output"

#define CLIP_X             "x"
#define CLIP_Y             "y"
#define CLIP_WIDTH         "width"
#define CLIP_HEIGHT        "height"
#define CLIP_VOB           "vob"
#define CLIP_FRAMES        "frames"
#define CLIP_STARTFRAME    "startframe"
#define CLIP_LENGTH        "length"
#define CLIP_OFFSET        "offset"
#define CLIP_MASK          "mask"


#include <QList>
//#include <qdom.h>     //ooo
#include <QDomNode>		//xxx

class XmlQRender
{
	class clip_struct {
		public:
		clip_struct ();
		bool readXml   (QDomElement *);
		bool writeXml  (QDomElement *);
		QString node_name;
		
		int     x;
		int     y;
		QString vob;	// filename
		uint    startframe;
		uint    frames;
		float   offset;
		float   length;
		int     width;
		int     height;
		QString mask;
	};
	class mmm_struct {
		public:
		mmm_struct ();
		bool readXml   (QDomElement *);
		bool writeXml  (QDomElement *);
		QString node_name;

		QString tmpdir;
		QString resolution;
		QString format;
		QString images;
		bool    successive;
		QString background;	// "#rrggbb"|"<filename>"
		uint    startframe;
		uint    frames;
		float   offset;
		float   length;
		QString output;
		QList<clip_struct *> listOfClips;
	};
	class qrender_struct {
		public:
		qrender_struct ();
		bool readXml   (QDomElement *);
		bool writeXml  (QDomElement *);
		QString node_name;

		mmm_struct mmm;
	};
public:
	 XmlQRender    ();
	~XmlQRender    ();

	void clear     ();
	void addStatus (QString);

	qrender_struct &qrender ();

	bool readXml   (QString);
	bool writeXml  (QString);

	// comparison operator ...
	bool operator == (XmlQRender &);

protected:
	qrender_struct m_qrender;
	QString        m_qsStatusFile;
	QString        m_qsXmlFile;
};


/*****************************************************************************
 ** XML - file structure
 **
 ** <qrender  [tmpdir="<path>"] [resolution="<WxH>"] [format="pal"|"ntsc"]
 **       [images="ppm"|"png"|"jpg"] [successive="0"|"1"] 
 **       [background="#rrggbb"|"<filename>"] 
 **       [startframe="<num>"]|[offset="<sec>"] [frames="<num>"]|[length="<sec>"] 
 **       [output="<filename>"]>
 **    <clip x="<pos>" y="<pos>" vob="<filename>"
 **       [startframe="<num>"]|[offset="<sec>"] [frames="<num>"]|[length="<sec>"] 
 **      [width="<int>"] [height="<int>"] [mask="<filename>"] />
 **   [<clip ... />]
 ** </qrender>
 ** NOTE due to compatibility reasons the qrender - tag can also be mmm to 
 **      enable reading of mmm - xml files.
 *****************************************************************************/

#endif // XMLQRENDER

