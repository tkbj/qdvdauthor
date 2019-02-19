/***************************************************************************
    modifiers.cpp
                             -------------------
    Modifiers / ImageManipulator class implementation.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QMessageBox>

#include "structuretoolbar.h"
#include "modifiers.h"
#include "xml_dvd.h"
#include "rgba.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Modifiers class
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Modifiers::Modifiers ( )
{
  fRotate = fShearX = fShearY = fTransparency = 0.0; 
  fZoom   = fScaleX = fScaleY = 1.0; 
  iStartFrame       =  iStopFrame  =  -1;
  m_pStructureItem  = NULL;
}

Modifiers::~Modifiers ( )
{
  // Since the Modifier is part of a MenuObject AND the MenuObject deletes its own StructureItem,
  // This StructureItem is already destroyed.
  //  if ( m_pStructureItem )
  //    delete m_pStructureItem;
  //  m_pStructureItem = NULL;
}

Modifiers& Modifiers::operator = ( Modifiers &other )
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

bool Modifiers::operator == ( Modifiers &other )
{
  bool bReturn = ( ( fRotate       == other.fRotate       ) &&
                   ( fZoom         == other.fZoom         ) &&
                   ( fScaleX       == other.fScaleX       ) &&
                   ( fScaleY       == other.fScaleY       ) &&
                   ( fShearX       == other.fShearX       ) &&
                   ( fShearY       == other.fShearY       ) &&
                   ( iStartFrame   == other.iStartFrame   ) &&
                   ( iStopFrame    == other.iStopFrame    ) &&
                   ( fTransparency == other.fTransparency ) );
  return bReturn;
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

StructureItem *Modifiers::createStructure ( StructureItem *pParentItem, StructureItem *pPreviousItem )
{
  if ( ! pParentItem )  {
    m_pStructureItem = NULL;
    return NULL;
  }

  // And finally we create the modifiers info ...
  if ( !( ( fRotate == 0.0 ) && ( fShearX == 0.0 ) && ( fShearY == 0.0 ) &&
          ( fZoom   == 1.0 ) && ( fScaleX == 1.0 ) && ( fScaleY == 1.0 ) ) )  {
    // First we get the translatable strings :
    StructureItem *pTemp;
    MenuObject *pMenuObject = pParentItem->getMenuObject ( );
    QString qsModifiers = QObject::tr ( "Modifiers"    );
    QString qsShear     = QObject::tr ( "Shear (x, y)" );
    QString qsZoom      = QObject::tr ( "Zoom"         );
    QString qsRotate    = QObject::tr ( "Rotate"       );

    if ( ! m_pStructureItem )  {
      m_pStructureItem = new StructureItem ( pMenuObject, StructureItem::Modifiers, pParentItem, pPreviousItem, qsModifiers, NULL );
      m_pStructureItem->setText ( 0, qsModifiers );	//xxx
      //m_pStructureItem->setExpandable ( TRUE );	//ooo
      m_pStructureItem->setExpanded ( true );		//xxx
    }

    // Comming here we are guaranteed to have a StructureItem - object.
    QString qsArgs = QObject::tr ( "%1 deg" ).arg ( fRotate );
    pTemp  = m_pStructureItem->setSibling ( pMenuObject, NULL, qsRotate, qsArgs );

    qsArgs = QString ( "%1 <%2x%3>" ).arg ( fZoom ).arg ( fScaleX ).arg ( fScaleY );
    pTemp  = m_pStructureItem->setSibling ( pMenuObject, pTemp, qsZoom,  qsArgs );

    qsArgs = QString ( "%1, %2" ).arg ( fShearX ).arg ( fShearY );
    pTemp  = m_pStructureItem->setSibling ( pMenuObject, pTemp, qsShear, qsArgs );
  }
  else if ( m_pStructureItem ) {
    delete  m_pStructureItem;
    m_pStructureItem = NULL;
  }

  return m_pStructureItem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ImageManipulator class
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageManipulator::colorKeying::operator == ( ImageManipulator::colorKeying &theOther )
{
  return ( ( theColor == theOther.theColor ) && ( fDeltaColor == theOther.fDeltaColor ) );
}


ImageManipulator::ImageManipulator ( )
  : Modifiers ( )
{
	node_name = QString ( MANIPULATOR );

	backgroundFileName = QString ( "" );
	bShowBackground = false;
	fKenBurnsX1     = -1.0f;
	fKenBurnsY1     = -1.0f;
	fKenBurnsX2     = -1.0f;
	fKenBurnsY2     = -1.0f;
	fStartX         =  0.0f;
	fStartY         =  0.0f;
	iXRes           =   720;
	iYRes           =   480;
	iTransformationMode = 0;
	fAspectRatio   =   4.0f/3.0f;
	fShearX        =   0.0f;
	fShearY        =   0.0f;
	fZoom          =   1.0f;
	fScaleX        =   1.0f;
	fScaleY        =   1.0f;
	fRotate        =   0.0f;
	fRed           =   1.0f;
	fGreen         =   1.0f;
	fBlue          =   1.0f;
	fBrightness    =   0.0f;
	fTransparency  =   0.0f;	// Opaque in the beginning ...
	iStartFrame    =     -1;
	iStopFrame     =     -1;
}

ImageManipulator::~ImageManipulator()
{
	uint t;
	if (listColorKeys.count())	{
		for (t=0;t<(uint)listColorKeys.count();t++)
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

	for (uint t=0;t<(uint)listColorKeys.count();t++)
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

bool ImageManipulator::operator == ( ImageManipulator &other )
{
  Modifiers &IAm  = (Modifiers &)*this;
  Modifiers &cool = (Modifiers &)other;
  bool bReturn = ( IAm == cool );
  if ( ! bReturn )
    return false;

  bReturn = ( ( backgroundFileName    == other.backgroundFileName  ) &&
              ( bShowBackground       == other.bShowBackground     ) &&
              ( fStartX               == other.fStartX             ) &&
              ( fStartY               == other.fStartY             ) &&
              ( fKenBurnsX1           == other.fKenBurnsX1         ) &&
              ( fKenBurnsY1           == other.fKenBurnsY1         ) &&
              ( fKenBurnsX2           == other.fKenBurnsX2         ) &&
              ( fKenBurnsY2           == other.fKenBurnsY2         ) &&
              ( iXRes                 == other.iXRes               ) &&
              ( iYRes                 == other.iYRes               ) &&
              ( iTransformationMode   == other.iTransformationMode ) &&
              ( fAspectRatio          == other.fAspectRatio        ) &&
              ( fRed                  == other.fRed                ) &&
              ( fGreen                == other.fGreen              ) &&
              ( fBlue                 == other.fBlue               ) &&
              ( fBrightness           == other.fBrightness         ) &&
              ( listColorKeys.count() == other.listColorKeys.count() ) );
  if ( ! bReturn )
    return false;

  QList<colorKeying *>::iterator it1 =       listColorKeys.begin ( );		//oxx
  QList<colorKeying *>::iterator it2 = other.listColorKeys.begin ( );		//oxx

  while ( it1 != listColorKeys.end ( ) )  {
    if  ( ! ( *(*it1++) == *(*it2++) ) )
      return false;
  }

  return bReturn;
};

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
    for (uint t=0;t<(uint)listColorKeys.count ();t++)	{
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

StructureItem *ImageManipulator::createStructure ( StructureItem *pParentItem, StructureItem *pPreviousItem )
{
  QString qsName;
  if ( ! pParentItem )  {
    m_pStructureItem = NULL;
    return NULL;
  }

  ImageManipulator initialValues;
  // And finally we create the modifiers info ...
  if ( !( ( fRotate       == initialValues.fRotate)       &&
	  ( fShearX       == initialValues.fShearX)       &&
	  ( fShearY       == initialValues.fShearY)       &&
	  ( fStartX       == initialValues.fStartX)       &&
	  ( fStartY       == initialValues.fStartY)       &&
	  ( fKenBurnsX1   == initialValues.fKenBurnsX1)   &&
	  ( fKenBurnsY1   == initialValues.fKenBurnsY1)   &&
	  ( fKenBurnsX2   == initialValues.fKenBurnsX2)   &&
	  ( fKenBurnsY2   == initialValues.fKenBurnsY2)   &&
	  ( fZoom         == initialValues.fZoom)         &&
	  ( fScaleX       == initialValues.fScaleX)       &&
	  ( fScaleY       == initialValues.fScaleY)       &&
	  ( fBrightness   == initialValues.fBrightness)   &&
	  ( fTransparency == initialValues.fTransparency) &&
	  ( fRed          == initialValues.fRed)          &&
	  ( fGreen        == initialValues.fGreen)        &&
	  ( fBlue         == initialValues.fBlue)         &&
	  ( iStartFrame   == initialValues.iStartFrame)   &&
	  ( iStopFrame    == initialValues.iStopFrame)    &&
	  ( listColorKeys.count() > 0) ) )  {
 
    StructureItem *pTemp=NULL;
    if ( ! m_pStructureItem ) { 
      MenuObject *pMenuObject = pParentItem->getMenuObject ( );
      m_pStructureItem = new StructureItem ( pMenuObject, StructureItem::Modifiers, pParentItem, pPreviousItem, QObject::tr ( "Modifiers" ), NULL );
      //m_pStructureItem->setText ( 0, QObject::tr ( "Modifiers" ) );	//xxx
      //pParentItem->setExpandable ( TRUE );	//ooo
      pParentItem->setExpanded ( false );	//xxx
    }

    //pTemp   = (StructureItem *)m_pStructureItem->firstChild ( );	//ooo
    pTemp   = (StructureItem *)m_pStructureItem->child ( 0 );
 
    while  ( pTemp ) {
      delete pTemp;

      //pTemp = (StructureItem *)m_pStructureItem->firstChild ( );	//ooo
      pTemp = (StructureItem *)m_pStructureItem->child ( 0 );		//xxx
    }
	//oooo  

    if ( ( fRed   != initialValues.fRed   ) ||
	 ( fGreen != initialValues.fGreen ) ||
	 ( fBlue  != initialValues.fBlue  ) )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("Color Adjust"),  QString ("%1, %2, %3").arg(fRed).arg(fGreen).arg(fBlue));

    if ( fBrightness    != initialValues.fBrightness )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("Brightness"),    QString ("%1").arg(fBrightness));

    if ( fTransparency  != initialValues.fTransparency )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("Transparency"),  QString ("%1").arg(fTransparency));
  
    if ( ( fShearX      != initialValues.fShearX ) || ( fShearY  != initialValues.fShearY)  )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("Shear (x, y)"),  QString ("%1, %2").arg(fShearX).arg(fShearY));

    if ( ( iStartFrame  != initialValues.iStartFrame ) || ( iStopFrame  != initialValues.iStopFrame) )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("VisibleFromTo"), QString ("%1, %2").arg(iStartFrame).arg(iStopFrame));

    if ( fZoom  != initialValues.fZoom )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ( "Zoom" ), QString ("%1").arg ( fZoom ) );
  
    if ( fRotate  != initialValues.fRotate )
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ("Rotate"), QString ("%1 deg").arg ( fRotate ) );

    if ( listColorKeys.count ( ) > 0 )  {
      QString qsColorKeys ( "|" );
      for ( uint t=0; t<(uint)listColorKeys.count ( ); t++ )
	qsColorKeys += QString ( "0x%1|" ).arg ( listColorKeys[t]->theColor, 16 );
      new StructureItem ( m_pStructureItem, StructureItem::Attributes, QObject::tr ( "ColorKey" ), qsColorKeys );
    }
  }
  return m_pStructureItem;
}

ImageManipulator &ImageManipulator::operator = ( Modifiers &theOther )
{
  fRotate       = theOther.fRotate;
  fZoom         = theOther.fZoom;
  fScaleX       = theOther.fScaleX;
  fScaleY       = theOther.fScaleY;
  fShearX       = theOther.fShearX;
  fShearY       = theOther.fShearY;
  iStartFrame   = theOther.iStartFrame;
  iStopFrame    = theOther.iStopFrame;
  fTransparency = theOther.fTransparency;
  return *this;
};

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
    for (t=0;t<(uint)listColorKeys.count();t++)
      delete listColorKeys[t];
    listColorKeys.clear();
  }
  for (t=0;t<(uint)theOther.listColorKeys.count();t++)
    listColorKeys.append(new colorKeying (theOther.listColorKeys[t]));

  return *this;
}


