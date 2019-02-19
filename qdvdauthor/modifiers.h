/***************************************************************************
    modifiers.h
                             -------------------
    Modifiers / ImageManpulator - classes
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef MODIFIERS_H
#define MODIFIERS_H

//#include <qdom.h>		//ooo
#include <QDomNode>		//xxx
//#include <qcolor.h>		//ooo
#include <QColor>		//xxx
//#include <q3valuelist.h>	//oxx

class StructureItem;

class Modifiers
{
public:
	         Modifiers();
	virtual ~Modifiers();
	virtual bool readProjectFile  ( QDomNode    & );
	virtual bool writeProjectFile ( QDomElement & );
	virtual bool isUnchanged ( );
	virtual Modifiers& operator    = ( Modifiers & );
	virtual bool       operator   == ( Modifiers & );
	StructureItem *createStructure ( StructureItem *, StructureItem * );


	/** The rotation in degree [0.0 .. 360.0] */
	double fRotate;
	/** Zoom factor. 1.0 == normal size. */
	double fZoom;
	/** Scaling differs from zoom in ... */
	double fScaleX;
	double fScaleY;
	/** Shear, using the Qt available shearing function. */
	double fShearX;
	double fShearY;
	/** transperency. 0.0 == opaque 1.0 == completely transparent. */
	double fTransparency;
	
	/** Here is the start frameNo when this object should apear in the menu. */
	int    iStartFrame;
	/** Here is the stop frameNo when the object should disapear in the menu. */
	int    iStopFrame;
	/** TODO: Later we can add some type of filter here, to have the object apear/disappear smoothly. */
	
protected:
	// StructureItem to show the Modifiers in the StructureToolbar
	StructureItem *m_pStructureItem;
	friend class MenuObject;
};

class ImageManipulator : public Modifiers
{
public:
	class colorKeying
	{
	public:
		// copy constructor.
		colorKeying (colorKeying *pOrig)	{
			theColor    = pOrig->theColor;
			fDeltaColor = pOrig->fDeltaColor;
		}
		colorKeying ( QRgb clr, float fDelta )  {
			theColor    = clr;
			fDeltaColor = fDelta;
		}

		/** The color to be filtered out */
		QRgb theColor;
		/** This will specify the delta in percent of the MAX value which should be used.
		    0.0 == only the specified color.
		    0.1 == +/-10% delta. E.g. (R=100 / G=200 / B=0) Will result in filtering out any color 
		    which is between (R=75 / G=175 / B=0) and (R=125 / G=225 / B=25)
		 */
		float fDeltaColor;
		bool operator == ( colorKeying & );
	};
		 ImageManipulator ( );
	virtual ~ImageManipulator ( );
	virtual bool readProjectFile  ( QDomNode    & );
	virtual bool writeProjectFile ( QDomElement & );
	virtual StructureItem *createStructure ( StructureItem *, StructureItem *p=NULL );
	virtual ImageManipulator& operator =   ( Modifiers        & );
	virtual ImageManipulator& operator =   ( ImageManipulator & );
	virtual bool              operator ==  ( ImageManipulator & );
	virtual bool isUnchanged ( );
	QString node_name;

	QString backgroundFileName;
	bool	bShowBackground;
	float	fStartX;          // [0 .. 1] Which pixel of the background to start at
	float	fStartY;          // [0 .. 1] type float to be able to adopt different resolutions.
	float	fKenBurnsX1;      // [0 .. 1] Which pixel of the image to start with
	float	fKenBurnsY1;      // [0 .. 1] type float to be able to adopt different resolutions.
	float	fKenBurnsX2;      // [0 .. 1] Which pixel of the image to start with
	float	fKenBurnsY2;      // [0 .. 1] type float to be able to adopt different resolutions.
	int	iTransformationMode;
	float	fRed;         // [0.0 .. 1.0]
	float	fGreen;       // [0.0 .. 1.0]
	float	fBlue;        // [0.0 .. 1.0]
	float	fBrightness;  // [0.0 .. 1.0]
	float	fAspectRatio; // E.g. 4:3 used to resize view
	int	iXRes;        // 720 Currently not used
	int	iYRes;        // 480 currently not used
	QList<colorKeying *> listColorKeys;	//oxx

	friend class ImageObject;
};

#endif // MODIFIERS_H

