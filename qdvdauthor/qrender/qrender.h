/*****************************************************************************
 ** Class QRender
 **
 ** This class is the main class for the qrender - command line utility.
 ** The goal was to create a utility which is as powerfull as convert
 ** and also capable in reading in mmm - xml - files.
 **
 ** This class depends solely on the Qt - classes to do al the rendering 
 ** (overlapping, sizing, stretching, rotating etc)
 ** 
 ** This class should also write a status.txt file in the dest directory
 ** and automatically recover from any previous crash or unfinished task.
 **
 ** NOTE: one qrender - thread per DVDMenu.
 *****************************************************************************/

#ifndef QRENDER_H
#define QRENDER_H

#define QRENDER_VERSION "0.0.1"

#include <qstring.h>
#include <qcolor.h>
#include <qrect.h>
#include <qsize.h>
#include <qpoint.h>

#include "xml_qrender.h"

class QRenderOptions
{
public:
	QRenderOptions ();

	QString qsFileName;
	bool    bMirror;
	bool    bFlip;
	QColor  colorBackground;
	float   fQuality;
	QRect   rectExtract;
	QString qsMask;
	QPoint  pointOffset;
	bool    bMonochrome;
	float   fBrightness;
	float   fSaturation;
	float   fHue;
	float   fRotate;
	QSize   sizeResize;
	QSize   sizeScale;
	QSize   sizeResample;
	QSize   sizeSample;
	QColor  colorTransparent;
};

class QRender
{
public:
	 QRender (int, char **);
	~QRender ();

protected:
	void parseCommandLine (int, char **);
	void usage            ();
	void readXml          (QString);
	void readStatus       ();
	void transcode        ();
	void extractFrames    ();
	void renderFrames     ();
	void buildMenuMovie   ();
	void generateFile   (QString, QString);
	QString checkForExe ( QString );
private:
	int                          m_iVerbosityLevel;
	QList<QRenderOptions *> m_listInputOptions;
	QRenderOptions              *m_pOutputOptions;
	XmlQRender                   m_xmlQRender;
};

/*****************************************************************************
 ** Command line options :
 **
 ** --dont-continue   QRender will erase all temp files for this menu and 
 **                   start with a fresh directory structure.
 **
 *****************************************************************************/

/*****************************************************************************
 ** XML - file structure
 **
 ** <qrender  [tmpdir="<path>"] [resolution="<WxH>"] [format="pal"|"ntsc"]
 **       [images="ppm"|"png"|"jpg"] [successive="0"|"1"] 
 **       [background="#rrggbb"|"<filename>"]
 **       [frames="<num>"] [output="<filename>"]>
 **    <clip x="<pos>" y="<pos>" vob="<filename>" [startframe="<int>"]
 **      [width="<int>"] [height="<int>"] [mask="<filename>"] />
 **   [<clip ... />]
 ** </qrender>
 ** NOTE due to compatibility reasons the qrender - tag can also be mmm to 
 **      enable reading of mmm - xml files.
 *****************************************************************************/
 
#endif // QRENDER_H
