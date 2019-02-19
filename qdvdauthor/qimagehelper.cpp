/***************************************************************************
    qimagehelper.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file handles some different graphics libraries.
   I generated it this way because I could not decide which
   library to go with. So I decided to leave it up to the user.
   
   For ImageMagick speaks the ful functionality (w/o fft)
   - but it is huge
   
   For FreeImage(Plus) speacks the compact llibrary with most of all I need
   - but it crashed on me all the time. (origin Windows)
   
   CxImage image has the most liberal license, is compact and has fft transform
   - but it is missing all those nice rescaling algos (origin Windows)
   
   ImgLib ...
   
   IMPORTANT only one library can be defined at any one time. The application
   will adjust dynamically to the different capabilities.
    
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <qimage.h>


#include "global.h"
#include "kimageeffect.h"
//#include "quantize.h"
#include "qimagehelper.h"

QImageHelper::QImageHelper()
	: QImage ()
{

}

QImageHelper::QImageHelper (const QString &fileName, const char *format)
	: QImage (fileName, format)
{

}

QImageHelper::~QImageHelper ()
{ 

}

QStringList &QImageHelper::getAvailableResizeAlgorithms()
{
	static QStringList listAlgorithm;
	listAlgorithm.clear ();
	listAlgorithm << QT_RESIZE_ALGO;
	return listAlgorithm;
}

bool QImageHelper::resize (int iWidth, int iHeight, int /* iResizeAlgorithm */)
{
	//((QImage &)*this) = smoothScale(iWidth, iHeight);	//ooo
	((QImage &)*this) = scaled(iWidth, iHeight);		//xxx
	return true;
}

bool QImageHelper::dither (int iMaxColors, QColor *pPalette)
{
	// Here we use the KImageEffects - class to do the dithering ...
	if (!pPalette)	{
		// FIXME:
		// For lack of better knowledge we create a greyscale image here ...
		// See also $DVDAUTHOR_HOME/test/test_dither/quantize.h
		int t;
		QColor *pPal = new QColor [iMaxColors];
		for (t=0;t<iMaxColors;t++)	{
			float fGreyLevel =  (float)t/(float)iMaxColors * 255;
			pPal[t] = QColor ((int)fGreyLevel, (int)fGreyLevel, (int)fGreyLevel);
		}
		// And here we dither the image down to the required colors.
		(QImage &)*this = KImageEffect::dither ((QImage &)*this, pPal, iMaxColors);
		delete []pPal;
		return true;
	}
	(QImage &)*this = KImageEffect::dither ((QImage &)*this, pPalette, iMaxColors);
//	(QImage &)*this = KImageEffect::flatten((QImage &)*this, (const QColor &)pPalette[0], (const QColor &)pPalette[1], iMaxColors);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Following are the algos which I developed.
// Not nice but working ...
//
///////////////////////////////////////////////////////////////////////////////
uint QImageHelper::countColors() //QImage &image)
{
	// Okay, let us count the actual used colors ...
	// Now remember that the images are okay in size for this stupidly
	// simple algol to work.
	uint x, y, c, iCounter=0;
	// for each possible color we keep a space in our hearts.
	bool *pArray = new bool [0x1000000];
	QRgb thePixel;
	if (!pArray)	{
		// Alternate algo to find out fast if > 4 colors
		QRgb rgbArray[MAX_MASK_COLORS+1];
		bool bFound = false;
		for (y=0;y<(uint)height();y++)	{
			for (x=0;x<(uint)width();x++)	{
				thePixel = pixel(x, y);
				// Filter out the alpha channel
				thePixel &= 0x00ffffff;
				for (c=0;c<iCounter;c++)	{
					if (rgbArray[c] == thePixel)
						bFound = true;
				}
				if (!bFound)
					rgbArray[iCounter++] = thePixel;
				bFound = false;
				if (iCounter > MAX_MASK_COLORS)
					return MAX_MASK_COLORS+1;
			}
		}
		return iCounter;
	}
	memset (pArray, 0, 0x1000000);
	for (y=0;y<(uint)height();y++)	{
		for (x=0;x<(uint)width();x++)	{
			thePixel = pixel(x, y);
			// Filter out the alpha channel
			thePixel &= 0x00ffffff;
			if (!pArray[(uint)thePixel])	{
				iCounter ++;
				pArray[(uint)thePixel] = 1;
			}
		}
	}
	delete []pArray;
	return iCounter;
}


QRgb QImageHelper::getMaskColor()
{
	QRgb theMaskColor = 0x00000000;
	// Okay, let us count the actual used colors ...
	// Now remember that the images are okay in size for this stupidly
	// simple algol to work.
	uint t, x, y, iFoundColor=0;
	bool bFound = false;
	// for each possible color we keep a space in our hearts.
	long int colorOccurence[MAX_MASK_COLORS];
	QRgb usedColors[MAX_MASK_COLORS+1];
	QRgb thePixel;

	for (t=0;t<MAX_MASK_COLORS;t++)
		colorOccurence[t] = 0;

	for (y=0;y<(uint)height();y++)	{
		for (x=0;x<(uint)width();x++)	{
			thePixel = pixel(x, y);
			bFound = false;
			for (t=0;t<iFoundColor;t++)	{
				if (thePixel == usedColors[t])	{
					bFound = true;
					colorOccurence[t] ++;
					continue;
				}
			}
			if (bFound == false)	{
				usedColors[iFoundColor] = thePixel;
				colorOccurence[iFoundColor] ++;
				iFoundColor ++;
				if (iFoundColor > MAX_MASK_COLORS)
					return 0;
			}
		}
	}
	// Okay we should have  no more then 4 colors here to check.
	// set the first color as the mask first and then check against the rest
	theMaskColor=usedColors[0];
	iFoundColor = colorOccurence[0];
	for (t=1;t<MAX_MASK_COLORS;t++)	{
		if ((uint)colorOccurence[t] > iFoundColor)	{
			iFoundColor = t;
			theMaskColor = usedColors[t];
		}
	}
	return theMaskColor;
}


