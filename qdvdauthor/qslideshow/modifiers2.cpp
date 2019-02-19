/***************************************************************************
    modifiers2.cpp
                             -------------------
    Modifiers / ImageManipulator class implementation.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

//#include <q3listview.h>   //ooo
#include <QListWidget>      //xxx

#include "modifiers2.h"
//#include "xml_dvd.h"		//ooo
#include "../xml_dvd.h"		//xxx
//#include "rgba.h"		//ooo
#include "../rgba.h"		//xxx

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Modifiers class
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Modifiers::Modifiers() 
{ 
  fRotate = fShearX = fShearY = fTransparency = 0.0; 
  fZoom   = fScaleX = fScaleY = 1.0; 
  iStartFrame  =  iStopFrame  =  -1;
}

Modifiers::~Modifiers() 
{

}

Modifiers& Modifiers::operator = (Modifiers &other)
{
	fRotate       = other.fRotate;
	fZoom         = other.fZoom;
	fScaleX       = other.fScaleX;
	fScaleY       = other.fScaleY;
	fShearX       = other.fShearX;
	fShearY       = other.fShearY;
	iStartFrame   = other.iStartFrame;
	iStopFrame    = other.iStopFrame;
	fTransparency = other.fTransparency;
	return *this;
};

bool Modifiers::isUnchanged ()
{
    Modifiers initialValues;
    if (( fRotate             != initialValues.fRotate )            ||
	( fZoom               != initialValues.fZoom )              ||
	( fScaleX             != initialValues.fScaleX )            ||
	( fScaleY             != initialValues.fScaleY )            ||
	( fShearX             != initialValues.fShearX )            ||
	( fShearY             != initialValues.fShearY )            ||
	( fTransparency       != initialValues.fTransparency )      ||
	( iStartFrame         != initialValues.iStartFrame )        ||
	( iStopFrame          != initialValues.iStopFrame )          )
	return false;
    return true;
}

bool Modifiers::writeProjectFile ( QDomElement &theElement ) 
{
  QDomDocument xmlDoc = theElement.ownerDocument();
  // Here we set the attributes of the <ImageManipulator> tag
  // Store the ImageManipulator - cariables (if neccesarry) ...
  ImageManipulator initialValues;
 if ( ! isUnchanged () ) {
   QDomElement manipulatorNode = xmlDoc.createElement ( MODIFIERS );
   if (fRotate != initialValues.fRotate)
     manipulatorNode.setAttribute( MODIFIERS_ROTATE, QString ("%1").arg(fRotate) );
   if (fZoom != initialValues.fZoom)
     manipulatorNode.setAttribute( MODIFIERS_ZOOM, QString ("%1").arg(fZoom) );
   if (fScaleX != initialValues.fScaleX)
     manipulatorNode.setAttribute( MODIFIERS_SCALEX, QString ("%1").arg(fScaleX) );
   if (fScaleY != initialValues.fScaleY)
     manipulatorNode.setAttribute( MODIFIERS_SCALEY, QString ("%1").arg(fScaleY) );
   if (fShearX != initialValues.fShearX)
     manipulatorNode.setAttribute( MODIFIERS_SHEARX, QString ("%1").arg(fShearX) );
   if (fShearY != initialValues.fShearY)
     manipulatorNode.setAttribute( MODIFIERS_SHEARY, QString ("%1").arg(fShearY) );
   if (fTransparency != initialValues.fTransparency)
     manipulatorNode.setAttribute( MODIFIERS_TRANSPARENCY, QString ("%1").arg(fTransparency) );
   if (iStartFrame != initialValues.iStartFrame)
     manipulatorNode.setAttribute( MODIFIERS_START_FRAME, QString ("%1").arg(iStartFrame) );
   if (iStopFrame != initialValues.iStopFrame)
     manipulatorNode.setAttribute( MODIFIERS_STOP_FRAME, QString ("%1").arg(iStopFrame) );
   theElement.appendChild( manipulatorNode );
 }

 return true;
}

bool Modifiers::readProjectFile (QDomNode &theNode)
{
  QDomElement manElement = theNode.toElement ();
  QDomAttr attribute;

  attribute = manElement.attributeNode ( MODIFIERS_ROTATE );
  if (!attribute.isNull())
    fRotate = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_ZOOM );
  if (!attribute.isNull())
    fZoom = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_SCALEX );
  if (!attribute.isNull())
    fScaleX = attribute.value().toFloat();
  
  attribute = manElement.attributeNode ( MODIFIERS_SCALEY );
  if (!attribute.isNull())
    fScaleY = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_SHEARX );
  if (!attribute.isNull())
    fShearX = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_SHEARY );
  if (!attribute.isNull())
    fShearY = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_TRANSPARENCY );
  if (!attribute.isNull())
    fTransparency = attribute.value().toFloat();

  attribute = manElement.attributeNode ( MODIFIERS_START_FRAME );
  if (!attribute.isNull())
    iStartFrame = attribute.value().toInt();
  
  attribute = manElement.attributeNode ( MODIFIERS_STOP_FRAME );
  if (!attribute.isNull())
    iStopFrame = attribute.value().toInt();

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageManipulator class
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageManipulator::ImageManipulator ( )
{
	node_name = QString (MANIPULATOR);
	reset ( );
}

void ImageManipulator::reset ( )
{
	backgroundFileName = QString ("");
	bShowBackground = false;
	fKenBurnsX1 = -1.0f;
	fKenBurnsY1 = -1.0f;
	fKenBurnsX2 = -1.0f;
	fKenBurnsY2 = -1.0f;
	fStartX     =  0.0f;
	fStartY     =  0.0f;
	iXRes = 720;
	iYRes = 480;
	iTransformationMode = 0;
	fAspectRatio = 4.0f/3.0f;
	fShearX = 0.0f;
	fShearY = 0.0f;
	fZoom   = 1.0f;
	fScaleX = 1.0f;
	fScaleY = 1.0f;
	fRotate = 0.0f;
	fRed    = 1.0;
	fGreen  = 1.0;
	fBlue   = 1.0;
	fBrightness   = 0.0f;
	fTransparency = 0.0f;	// Opaque in the beginning ...
	iStartFrame   =   -1;
	iStopFrame    =   -1;
}

ImageManipulator::~ImageManipulator()
{
	uint t;
	if (listColorKeys.count())	{
		for (t=0;t<static_cast<uint>(listColorKeys.count());t++)  //oxx
			delete listColorKeys[t];
		listColorKeys.clear();
	}
}

bool ImageManipulator::readProjectFile (QDomNode &theNode)
{
	QDomElement manElement = theNode.toElement ();
	QDomAttr attribute;


	attribute = manElement.attributeNode ( MANIPULATOR_FILE_NAME );
	if (!attribute.isNull())
		backgroundFileName = attribute.value();

	attribute = manElement.attributeNode ( MANIPULATOR_SHOW_BACKGROUND );
	if (!attribute.isNull())
		bShowBackground = (attribute.value() == QString ("yes"));

	attribute = manElement.attributeNode ( MANIPULATOR_STARTX );
	if (!attribute.isNull())
		fStartX = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_STARTY );
	if (!attribute.isNull())
		fStartY = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_KENBURNS_X1 );
	if (!attribute.isNull())
		fKenBurnsX1 = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_KENBURNS_Y1 );
	if (!attribute.isNull())
		fKenBurnsY1 = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_KENBURNS_X2 );
	if (!attribute.isNull())
		fKenBurnsX2 = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_KENBURNS_Y2 );
	if (!attribute.isNull())
		fKenBurnsY2 = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_XRES );
	if (!attribute.isNull())
		iXRes = attribute.value().toInt();

	attribute = manElement.attributeNode ( MANIPULATOR_YRES );
	if (!attribute.isNull())
		iYRes = attribute.value().toInt();

	attribute = manElement.attributeNode ( MANIPULATOR_TRANSFORMATION_MODE );
	if (!attribute.isNull())
		iTransformationMode = attribute.value().toInt();

	attribute = manElement.attributeNode ( MANIPULATOR_ASPECT_RATIO );
	if (!attribute.isNull())
		fAspectRatio = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_RED );
	if (!attribute.isNull())
		fRed = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_GREEN );
	if (!attribute.isNull())
		fGreen = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_BLUE );
	if (!attribute.isNull())
		fBlue = attribute.value().toFloat();

	attribute = manElement.attributeNode ( MANIPULATOR_BRIGHTNESS );
	if (!attribute.isNull())
		fBrightness = attribute.value().toFloat();

	// to get some sanity in backwards compatibility
	if ( fStartX >  1.0 )
	     fStartX =  0.0;
	if ( fStartY >  1.0 )
	     fStartY =  0.0;
	if ( fKenBurnsX1 >  1.0 )
	     fKenBurnsX1 = -1.0;
	if ( fKenBurnsY1 >  1.0 )
	     fKenBurnsY1 = -1.0;
	if ( fKenBurnsX2 >  1.0 )
	     fKenBurnsX2 = -1.0;
	if ( fKenBurnsY2 >  1.0 )
	     fKenBurnsY2 = -1.0;

	for (uint t=0;t<static_cast<uint>(listColorKeys.count());t++)  //oxx
		delete listColorKeys[t];
	listColorKeys.clear ();
	QDomNode    manipulatorNodes = theNode.firstChild();
	QDomElement theElement;
	QString     tagName;
	while ( !manipulatorNodes.isNull () )	{
		// Okay, here we retain the stored data from the xml file.
		theElement = manipulatorNodes.toElement();
		tagName    = theElement.tagName();
		// Okay, this is ugly but after all it is the simplest of all xml file structure.
		// No need to get fancy ...
		if (tagName == MODIFIERS)	{
		  Modifiers::readProjectFile ( manipulatorNodes );
		}
		if (tagName == MANIPULATOR_COLOR_KEY)	{
			Rgba  theColor;
			float fDelta = 0.0f;
			colorKeying *pColorKey;
			attribute = theElement.attributeNode ( MANIPULATOR_COLOR );
			if (!attribute.isNull())
				theColor.fromString ( attribute.value ( ) );

			attribute = theElement.attributeNode ( MANIPULATOR_DELTA );
			if (!attribute.isNull())
				fDelta = attribute.value().toFloat();

			pColorKey = new colorKeying ( theColor.rgb ( ), fDelta);
			listColorKeys.append ( pColorKey );
		}
		manipulatorNodes = manipulatorNodes.nextSibling();
	}
	return true;
}

bool ImageManipulator::isUnchanged ()
{
  ImageManipulator initialValues;
  if (( Modifiers::isUnchanged ())                                ||
      ( backgroundFileName.isEmpty ())                            ||
      ( bShowBackground     != initialValues.bShowBackground)     ||
      ( fStartX             != initialValues.fStartX)             ||
      ( fStartY             != initialValues.fStartY)             ||
      ( fKenBurnsX1         != initialValues.fKenBurnsX1)         ||
      ( fKenBurnsY1         != initialValues.fKenBurnsY1)         ||
      ( fKenBurnsX2         != initialValues.fKenBurnsX2)         ||
      ( fKenBurnsY2         != initialValues.fKenBurnsY2)         ||
      ( iXRes               != initialValues.iXRes)               ||
      ( iYRes               != initialValues.iYRes)               ||
      ( iTransformationMode != initialValues.iTransformationMode) ||
      ( fAspectRatio        != initialValues.fAspectRatio)        ||
      ( fRed                != initialValues.fRed)                ||
      ( fGreen              != initialValues.fGreen)              ||
      ( fBlue               != initialValues.fBlue)               ||
      ( fBrightness         != initialValues.fBrightness)         ||
      ( listColorKeys.count () != 0) )
    return false;
  return true;
}

bool ImageManipulator::writeProjectFile (QDomElement &theElement)
{
  QDomDocument xmlDoc = theElement.ownerDocument();
  // Here we set the attributes of the <ImageManipulator> tag
  // Store the ImageManipulator - cariables (if neccesarry) ...
  ImageManipulator initialValues;
  if ( ! isUnchanged () ) {
    QDomElement manipulatorNode = xmlDoc.createElement ( MANIPULATOR );
    Modifiers::writeProjectFile ( manipulatorNode );
    if (!backgroundFileName.isEmpty())
      manipulatorNode.setAttribute( MANIPULATOR_FILE_NAME, backgroundFileName );
    if (bShowBackground != initialValues.bShowBackground)
      manipulatorNode.setAttribute( MANIPULATOR_SHOW_BACKGROUND, bShowBackground ? QString ("yes") : QString ("no"));
    if (fStartX != initialValues.fStartX)
      manipulatorNode.setAttribute( MANIPULATOR_STARTX, QString ("%1").arg(fStartX) );
    if (fStartY != initialValues.fStartY)
      manipulatorNode.setAttribute( MANIPULATOR_STARTY, QString ("%1").arg(fStartY) );
    if (fKenBurnsX1 != initialValues.fKenBurnsX1)
      manipulatorNode.setAttribute( MANIPULATOR_KENBURNS_X1, QString ("%1").arg(fKenBurnsX1) );
    if (fKenBurnsY1 != initialValues.fKenBurnsY1)
      manipulatorNode.setAttribute( MANIPULATOR_KENBURNS_Y1, QString ("%1").arg(fKenBurnsY1) );
    if (fKenBurnsX2 != initialValues.fKenBurnsX2)
      manipulatorNode.setAttribute( MANIPULATOR_KENBURNS_X2, QString ("%1").arg(fKenBurnsX2) );
    if (fKenBurnsY2 != initialValues.fKenBurnsY2)
      manipulatorNode.setAttribute( MANIPULATOR_KENBURNS_Y2, QString ("%1").arg(fKenBurnsY2) );
    if (iXRes != initialValues.iXRes)
      manipulatorNode.setAttribute( MANIPULATOR_XRES, QString ("%1").arg(iXRes) );
    if (iYRes != initialValues.iYRes)
      manipulatorNode.setAttribute( MANIPULATOR_YRES, QString ("%1").arg(iYRes) );
    if (iTransformationMode != initialValues.iTransformationMode)
      manipulatorNode.setAttribute( MANIPULATOR_TRANSFORMATION_MODE, QString ("%1").arg(iTransformationMode) );
    if (fAspectRatio != initialValues.fAspectRatio)
      manipulatorNode.setAttribute( MANIPULATOR_ASPECT_RATIO, QString ("%1").arg(fAspectRatio) );
    if (fRed != initialValues.fRed)
      manipulatorNode.setAttribute( MANIPULATOR_RED, QString ("%1").arg(fRed) );
    if (fGreen != initialValues.fGreen)
      manipulatorNode.setAttribute( MANIPULATOR_GREEN, QString ("%1").arg(fGreen) );
    if (fBlue != initialValues.fBlue)
      manipulatorNode.setAttribute( MANIPULATOR_BLUE, QString ("%1").arg(fBlue) );
    if (fBrightness != initialValues.fBrightness)
      manipulatorNode.setAttribute( MANIPULATOR_BRIGHTNESS, QString ("%1").arg(fBrightness) );
    for (uint t=0;t<static_cast<uint>(listColorKeys.count ());t++)	{    //oxx
      QDomElement colorKeyNode = xmlDoc.createElement ( MANIPULATOR_COLOR_KEY );
      Rgba        theColor ( listColorKeys[t]->theColor );
      if ( listColorKeys[t]->fDeltaColor != 0.0 )
	colorKeyNode.setAttribute ( MANIPULATOR_DELTA, QString ("%1").arg ( listColorKeys[t]->fDeltaColor ) );
      colorKeyNode.setAttribute   ( MANIPULATOR_COLOR, theColor.toString ( ) );
      manipulatorNode.appendChild ( colorKeyNode );
    }
    theElement.appendChild( manipulatorNode );
  }
  
  return true;
}

//void ImageManipulator::createStructure (Q3ListViewItem *pParentItem)  //ooo
void ImageManipulator::createStructure (QListWidgetItem *pParentItem)   //xxx
{
	//Q3ListViewItem *pModifierItem;   //ooo
    QListWidgetItem *pModifierItem;     //xxx
	QString qsName;

/*	ImageManipulator initialValues;
	// And finally we create the modifiers info ...
	if ( !( 	(fRotate       == initialValues.fRotate)       &&
			(fShearX       == initialValues.fShearX)       &&
			(fShearY       == initialValues.fShearY)       &&
			(fStartX       == initialValues.fStartX)       &&
			(fStartY       == initialValues.fStartY)       &&
			(fKenBurnsX1   == initialValues.fKenBurnsX1)   &&
			(fKenBurnsY1   == initialValues.fKenBurnsY1)   &&
			(fKenBurnsX2   == initialValues.fKenBurnsX2)   &&
			(fKenBurnsY2   == initialValues.fKenBurnsY2)   &&
			(fZoom         == initialValues.fZoom)         &&
			(fScaleX       == initialValues.fScaleX)       &&
			(fScaleY       == initialValues.fScaleY)       &&
			(fBrightness   == initialValues.fBrightness)   &&
			(fTransparency == initialValues.fTransparency) &&
			(fRed          == initialValues.fRed)          &&
			(fGreen        == initialValues.fGreen)        &&
			(fBlue         == initialValues.fBlue)         &&
			(iStartFrame   == initialValues.iStartFrame)   &&
			(iStopFrame    == initialValues.iStopFrame)    &&
			(listColorKeys.count() > 0) ) )	{
		//pModifierItem = new Q3ListViewItem(pParentItem, QString ("Modifiers"), NULL);   //ooo
        pModifierItem = new QListWidgetItem(QString ("Modifiers"), pParentItem);    //xxx
		//pParentItem->setExpandable (TRUE);  //ooo
        pParentItem->setExpandable (true);  //xxx

		if (	(fRed        != initialValues.fRed)   ||
			(fGreen      != initialValues.fGreen) ||
			(fBlue       != initialValues.fBlue) )
			//new Q3ListViewItem(pModifierItem, QString ("Color Adjust"),    //ooo
            new QListWidgetItem(pModifierItem, QString ("Color Adjust"),    //xxx
				QString ("%1, %2, %3").arg(fRed).arg(fGreen).arg(fBlue));
		if (fBrightness  != initialValues.fBrightness)
			//new Q3ListViewItem(pModifierItem, QString ("Brightness"),  //ooo
            new QListWidgetItem(pModifierItem, QString ("Brightness"),  //xxx
				QString ("%1").arg(fBrightness));
		if (fTransparency  != initialValues.fTransparency)
			//new Q3ListViewItem(pModifierItem, QString ("Transparency"),    //ooo
            new QListWidgetItem(pModifierItem, QString ("Transparency"),    //xxx
				QString ("%1").arg(fTransparency));
		if ((fShearX  != initialValues.fShearX) ||
			(fShearY  != initialValues.fShearY)  )
			//new Q3ListViewItem(pModifierItem, QString ("Shear (x, y)"),    //ooo
            new QListWidgetItem(pModifierItem, QString ("Shear (x, y)"),    //xxx
				QString ("%1, %2").arg(fShearX).arg(fShearY));
		if ((iStartFrame  != initialValues.iStartFrame) ||
			(iStopFrame  != initialValues.iStopFrame)  )
			//new Q3ListViewItem(pModifierItem, QString ("VisibleFromTo"),   //ooo
            new QListWidgetItem(pModifierItem, QString ("VisibleFromTo"),   //xxx
				QString ("%1, %2").arg(iStartFrame).arg(iStopFrame));
		if (fZoom  != initialValues.fZoom)
			//new Q3ListViewItem(pModifierItem, QString ("Zoom"),    //ooo
            new QListWidgetItem(pModifierItem, QString ("Zoom"),    //xxx
				QString ("%1").arg(fZoom));
		if (fRotate  != initialValues.fRotate)
			//new Q3ListViewItem(pModifierItem, QString ("Rotate"),  //ooo
            new QListWidgetItem(pModifierItem, QString ("Rotate"),  //xxx
				QString ("%1 deg").arg(fRotate));
		if (listColorKeys.count() > 0)	{
			QString qsColorKeys("|");
			for (uint t=0;t<listColorKeys.count();t++)
				qsColorKeys += QString("0x%1|").arg(listColorKeys[t]->theColor, 16);
			//new Q3ListViewItem (pModifierItem, QString ("ColorKey"), qsColorKeys); //ooo
            new QListWidgetItem (pModifierItem, QString ("ColorKey"), qsColorKeys); //xxx
		}
	}*/    //oooo
}

ImageManipulator& ImageManipulator::operator = (ImageManipulator &theOther)
{
  uint t;
  backgroundFileName  = theOther.backgroundFileName;
  bShowBackground     = theOther.bShowBackground;
  fStartX             = theOther.fStartX;
  fStartY             = theOther.fStartY;
  fKenBurnsX1         = theOther.fKenBurnsX1;
  fKenBurnsY1         = theOther.fKenBurnsY1;
  fKenBurnsX2         = theOther.fKenBurnsX2;
  fKenBurnsY2         = theOther.fKenBurnsY2;
  iXRes               = theOther.iXRes;
  iYRes               = theOther.iYRes;
  iTransformationMode = theOther.iTransformationMode;
  fAspectRatio        = theOther.fAspectRatio;
  fShearX             = theOther.fShearX;
  fShearY             = theOther.fShearY;
  iStartFrame         = theOther.iStartFrame;
  iStopFrame          = theOther.iStopFrame;
  fZoom               = theOther.fZoom;
  fScaleX             = theOther.fScaleX;
  fScaleY             = theOther.fScaleY;
  fRotate             = theOther.fRotate;
  fRed                = theOther.fRed;
  fGreen              = theOther.fGreen;
  fBlue               = theOther.fBlue;
  fBrightness         = theOther.fBrightness;
  fTransparency       = theOther.fTransparency;
  if (listColorKeys.count())	{
    for (t=0;t<listColorKeys.count();t++)
      delete listColorKeys[t];
    listColorKeys.clear();
  }
  for (t=0;t<theOther.listColorKeys.count();t++)
    listColorKeys.append(new colorKeying (theOther.listColorKeys[t]));

  return *this;
}


