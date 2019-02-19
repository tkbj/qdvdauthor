/***************************************************************************
    qimagehelper.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class adds some image manipulation routines for QImage objects.
   Please note, that you need FreeImage(Plus) installed in order to have
   some usefull functionality.
   
   However I created this class so it would compile also without this
   library only missing the functionality and thus falling back onto
   the standard functions (E.g. resize becomes the standard RESIZE_QT)
    
****************************************************************************/

#ifndef QIMAGEHELPER_H
#define QIMAGEHELPER_H

//#include <qobject.h>
//#include <qimage.h>	//ooo
#include <QImage>	//xxx

// This define is used to indicate that the image(s) are resized using the standard Qt functionality.
#define QT_RESIZE_ALGO "Normal Qt"
#define RESIZE_QT			-1

class QImageHelper : public QImage
{
public:
	QImageHelper  ();
	QImageHelper  (const QString & fileName, const char * format = 0 );
	virtual ~QImageHelper ();

	QStringList &getAvailableResizeAlgorithms();
	uint countColors();	//QImage &image);
	QRgb getMaskColor();

	bool resize (int, int, int);
	bool dither (int, QColor *p=NULL);
};

#endif // QIMAGEHELPER_H
