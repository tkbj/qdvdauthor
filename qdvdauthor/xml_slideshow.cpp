/***************************************************************************
    xml_slideshow.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file is shared between
   QRender    ( Qt 4.x ) and
   QSlideshow ( Qt 3.x )
   
   It would not be wise to re-write the whole file or
   to have two copies of essentially the same code in
   a project. This way changes in one art of the app
   will automagically be available in the other part.
    
****************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "xml_slideshow.h"

#define QDVD_RENDER 1	//xxx

//#if (QT_VERSION > 0x0301FF)
#ifdef QDVD_RENDER
  #include <QtXml/QDomDocument>
  #include <QFile>
  #include <QColor>
  #include <QMatrix>
  //#include <Q3ListView>	//ooo
  #include <QListView>		//xxx
  //#include <Q3FileDialog>	//ooo
  #include <QFileDialog>	//xxx
  #include <QMessageBox>
#else
  #include <qdom.h>
  #include <qfile.h>
  #include <qcolor.h>
  #include <qmatrix.h>
  #include <q3listview.h>
  #include <q3filedialog.h>
  #include <qmessagebox.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QList>		//xxx

  #include "xml_dvd.h"
  #include "qslideshow/modifiers2.h"		//ooo
  //#include "modifiers.h"			//xxx

  #ifndef toAscii
    #define toAscii ascii
  #endif
#endif


CXmlSlideshow::time_object::time_object ( )
{
	node_name	= QString ( "" );
	fDuration	= -1.0f;	// Define the duration in seconds
	fStartTime	= -1.0f;
	fEndTime	= -1.0f;
	pTransition	= NULL;
	pModifier 	= NULL;
}

CXmlSlideshow::time_object::~time_object ( )
{
  if ( pTransition )
    delete pTransition;
  pTransition = NULL;
}

CXmlSlideshow::effect_struct::effect_struct ()
{
	node_name = QString ("effect");
	name= "";  // either crop/kenburns/scroll
	bUserDefined = false;
	x0  = 0;
	y0  = 0;
	x1  = 0;
	y1  = 0;
	xe0 = 0;
	ye0 = 0;
	xe1 = 0;
	ye1 = 0;
	scroll = QString (""); // either left or right
}

bool CXmlSlideshow::effect_struct::isDefault ( )
{
  return ( ( 0 ==  x0 ) && ( 0 ==  y0 ) && ( 0 ==  x1 ) && ( 0 ==  y1 ) &&
           ( 0 == xe0 ) && ( 0 == ye0 ) && ( 0 == xe1 ) && ( 0 == ye1 ) );
}

void CXmlSlideshow::effect_struct::randomKenBurns ( float fStrength, int iWidth, int iHeight, bool bForce )
{
  int iCorner = -1;

  if ( bForce || isDefault ( ) ) {
    name = EFFECT_TYPE_KENBURNS;
    // Randomly create start stop etc ...
    // RandomRange :
    if ( ( fStrength > 0.99999 ) || ( fStrength < 0.00001 ) )
           fStrength = 0.5;

    // RandomRange is delta between cut-out image and full image corner. E.g. 50
    int iRandomXRange = (int)( fStrength / 2.0 * iWidth  );
    int iRandomYRange = (int)( fStrength / 2.0 * iHeight );

    // Also we want to limit the randomness to 0.5 - 1.0
    float fMinScale = 0.1f;
    float fMaxScale = 1.0f - fMinScale;
    float fScaleX   = ( (float)rand ( ) / RAND_MAX ) * fMaxScale + fMinScale;
    float fScaleY   = ( (float)rand ( ) / RAND_MAX ) * fMaxScale + fMinScale;

    // randomly determine in which corner to start
    float fRand = (float)rand ( );
    iCorner = (int)( fRand / RAND_MAX * 4.0 );
    if ( iCorner == 0 )  { // Starting Upper left corner
      x0  = 0;
      y0  = 0;
      x1  = iWidth  - (int)( fScaleX * iRandomXRange );
      y1  = iHeight - (int)( fScaleY * iRandomYRange );

      xe0 = iWidth  - x1;
      ye0 = iHeight - y1;
      xe1 = iWidth;
      ye1 = iHeight;
    }
    else if ( iCorner == 1 )  { // Starting Upper right corner
      x0  = (int)( fScaleX * iRandomXRange );
      y0  = 0;
      x1  = iWidth;
      y1  = iHeight - (int)( fScaleY * iRandomYRange );

      xe0 = 0;
      ye0 = iHeight - y1;
      xe1 = iWidth - x0;
      ye1 = iHeight;
    }
    else if ( iCorner == 2 )  { // Starting Lower right corner
      x0  = (int)( fScaleX * iRandomXRange );
      y0  = (int)( fScaleY * iRandomYRange );
      x1  = iWidth;
      y1  = iHeight;

      xe0 = 0;
      ye0 = 0;
      xe1 = iWidth  - x0;
      ye1 = iHeight - y0;
    }
    else { // Starting Lower left corner
      x0  = 0;
      y0  = (int)( fScaleY * iRandomYRange );
      x1  = iWidth  - (int)( fScaleX * iRandomXRange );;
      y1  = iHeight;

      xe0 = iWidth  - x1;
      ye0 = 0;
      xe1 = iWidth;
      ye1 = iHeight - y0;
    }
  }
}

bool CXmlSlideshow::effect_struct::readXml (QDomElement *pDocElem)
{
	if (pDocElem->tagName() != node_name)
		return false;
	// So lets get first the attributes for this node.
	QDomAttr a = pDocElem->attributeNode ( EFFECT_NAME );
	name = a.value();
	bUserDefined = false;
	a = pDocElem->attributeNode ( EFFECT_USER_DEF );
	bUserDefined = ( a.value ( ) == "true" ) ? true : false;
	a = pDocElem->attributeNode ( EFFECT_X0 );
	x0 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_Y0 );
	y0 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_X1 );
	x1 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_Y1 );
	y1 = a.value().toInt();

	a = pDocElem->attributeNode ( EFFECT_XE0 );
	xe0 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_YE0 );
	ye0 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_XE1 );
	xe1 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_YE1 );
	ye1 = a.value().toInt();
	a = pDocElem->attributeNode ( EFFECT_SCROLL );
	scroll = a.value();
	return true;
}

bool CXmlSlideshow::effect_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
//	debug_out ("CXmlSlideshow::effect_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);
	QDomElement effectNode = pDocument->createElement( node_name );	// <effect_start> or <effect_stop>

	if ( ! name.isEmpty ( ) )
		effectNode.setAttribute ( EFFECT_NAME, name );
	else if ( isDefault ( ) ) // Check if we have anything to store.
		return true;

	if ( bUserDefined )
		effectNode.setAttribute ( EFFECT_USER_DEF, "true" );
	if ( x0  > 1 )
		effectNode.setAttribute ( EFFECT_X0,  QString ( "%1" ).arg (  x0 ) );
	if ( y0  > 1 )
		effectNode.setAttribute ( EFFECT_Y0,  QString ( "%1" ).arg (  y0 ) );
	if ( x1  > 1 )
		effectNode.setAttribute ( EFFECT_X1,  QString ( "%1" ).arg (  x1 ) );
	if ( y1  > 1 )
		effectNode.setAttribute ( EFFECT_Y1,  QString ( "%1" ).arg (  y1 ) );
	if ( xe0 > 1 )
		effectNode.setAttribute ( EFFECT_XE0, QString ( "%1" ).arg ( xe0 ) );
	if ( ye0 > 1 )
		effectNode.setAttribute ( EFFECT_YE0, QString ( "%1" ).arg ( ye0 ) );
	if ( xe1 > 1 )
		effectNode.setAttribute ( EFFECT_XE1, QString ( "%1" ).arg ( xe1 ) );
	if ( ye1 > 1 )
		effectNode.setAttribute ( EFFECT_YE1, QString ( "%1" ).arg ( ye1 ) );
	if ( ! scroll.isEmpty ( ) )
		effectNode.setAttribute ( EFFECT_SCROLL, scroll );

	pNodeElement->appendChild ( effectNode );
	return true;
}

CXmlSlideshow::effect_struct& CXmlSlideshow::effect_struct::operator =(CXmlSlideshow::effect_struct &theOther)
{
	name = theOther.name;
	bUserDefined = theOther.bUserDefined;
	x0  = theOther.x0;
	y0  = theOther.y0;
	x1  = theOther.x1;
	y1  = theOther.y1;
	xe0 = theOther.xe0;
	ye0 = theOther.ye0;
	xe1 = theOther.xe1;
	ye1 = theOther.ye1;
	scroll = theOther.scroll;
	return *this;
}
bool CXmlSlideshow::effect_struct::operator == ( CXmlSlideshow::effect_struct &theOther )
{
	return ( ( name   == theOther.name   ) &&
		 ( bUserDefined == theOther.bUserDefined ) &&
		 ( x0     == theOther.x0     ) &&
		 ( y0     == theOther.y0     ) &&
		 ( x1     == theOther.x1     ) &&
		 ( y1     == theOther.y1     ) &&
		 ( xe0    == theOther.xe0    ) &&
		 ( ye0    == theOther.ye0    ) &&
		 ( xe1    == theOther.xe1    ) &&
		 ( ye1    == theOther.ye1    ) &&
		 ( scroll == theOther.scroll ) );
}

CXmlSlideshow::img_struct::img_struct ()
	: time_object ()
{
	node_name = QString ( IMG_NODE );
	src       = QString (   "" );
	text      = QString (   "" );
	width     =     0;
	height    =     0;
	pMatrix   =  NULL;
	rotate    =  0.0f;
	effect[0].node_name = effect[0].node_name + "_start";
	effect[1].node_name = effect[1].node_name + "_end";
}

CXmlSlideshow::img_struct::~img_struct ()
{
  if ( pMatrix )
    delete pMatrix;
  pMatrix = NULL;
#ifndef QDVD_RENDER
	if (pModifier)	{
		ImageManipulator *pManipulator = (ImageManipulator *)pModifier;
		delete pManipulator;
		pModifier = NULL;
	}
#endif
}

bool CXmlSlideshow::img_struct::readXml ( QDomElement *pDocElem )
{
	if (pDocElem->tagName() != node_name)
		return false;
	// So lets get first the attributes for this node.
	QDomAttr a = pDocElem->attributeNode ( IMG_SRC );
	src = a.value();
	a = pDocElem->attributeNode ( IMG_TEXT );
	text = a.value();
	a = pDocElem->attributeNode ( IMG_WIDTH );
	width = a.value().toInt();
	a = pDocElem->attributeNode ( IMG_HEIGHT );
	height = a.value().toInt();
	a = pDocElem->attributeNode ( IMG_ROTATE );
	rotate = a.value().toFloat();
	a = pDocElem->attributeNode ( IMG_DURATION );
	fDuration = a.value().toFloat();
	a = pDocElem->attributeNode ( IMG_MATRIX );
	if ( pMatrix )
		delete pMatrix;
	pMatrix = NULL;

	if ( ! a.value ( ).isEmpty ( ) )  {
#ifdef QDVD_RENDER
		QStringList list = a.value ( ).split ( "," );
		qreal fValues[10]; //m11, m12, m21, m22, dx, dy;
#else
		QStringList list = QStringList::split ( ",", a.value ( ) );
		double fValues[10]; //m11, m12, m21, m22, dx, dy;
#endif
		for ( int t=0; t<(int)list.count ( ); t++ )
			fValues[t] = list[t].toDouble ( );
		pMatrix = new QMatrix ( fValues[0], fValues[1], fValues[2], fValues[3], fValues[4], fValues[5] );
	}
	bool bReturn = false;
	QDomNode xmlNode = pDocElem->firstChild ( );
	filter_struct    tmpFilter;
	if ( pTransition )
		delete pTransition;
	pTransition = NULL;

	while ( !xmlNode.isNull ( ) ) {
		QDomElement searchTree = xmlNode.toElement();
		if ( searchTree.tagName ( ) == tmpFilter.node_name )  {
			pTransition = new filter_struct;
			bReturn = pTransition->readXml ( &searchTree );
		}
		if ( searchTree.tagName ( ) == effect[0].node_name )
			bReturn = effect[0].readXml ( &searchTree  );
		if ( searchTree.tagName ( ) == effect[1].node_name )
			bReturn = effect[1].readXml ( &searchTree  );
#ifndef QDVD_RENDER
		if ( searchTree.tagName ( ) == QString ( MANIPULATOR ) )	{
			ImageManipulator *pImageManipulator = new ImageManipulator;
			pModifier = (void *)pImageManipulator;
			bReturn = pImageManipulator->readProjectFile( searchTree );
		}
#endif
		// If there has been a problem then return false.
		if ( ! bReturn )
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling ( );
	}
	return true;
}

bool CXmlSlideshow::img_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
//	debug_out ("CXmlSlideshow::img_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);
	QDomElement imgNode = pDocument->createElement( node_name );	// <img>

	if (!src.isEmpty())
		imgNode.setAttribute( IMG_SRC, src );
	if (!text.isEmpty())
		imgNode.setAttribute( IMG_TEXT, text );
	if (width > 1)
		imgNode.setAttribute( IMG_WIDTH, QString ("%1").arg(width) );
	if (height > 1)
		imgNode.setAttribute( IMG_HEIGHT, QString ("%1").arg(height) );
	if (rotate != 0.0)
		imgNode.setAttribute( IMG_ROTATE, QString ("%1").arg(rotate) );
	if (fDuration > -1.0f)
		imgNode.setAttribute( IMG_DURATION, QString ("%1").arg(fDuration) );
//	if (pModifier)
//		((ImageManipulator *)pModifier)->writeProjectFile( imgNode );
	if ( pMatrix )  {
		QString qsMatrix = QString ( "%1,%2,%3,%4,%5,%6" )
		.arg ( pMatrix->m11 ( ) ).arg ( pMatrix->m12 ( ) )
		.arg ( pMatrix->m21 ( ) ).arg ( pMatrix->m22 ( ) )
		.arg ( pMatrix->dx  ( ) ).arg ( pMatrix->dy  ( ) );
		imgNode.setAttribute( IMG_MATRIX, qsMatrix );
	}

	pNodeElement->appendChild ( imgNode );
	if ( pTransition )
		pTransition->writeXml ( pDocument, &imgNode );

	effect[0].writeXml            ( pDocument, &imgNode );
	return effect[1].writeXml     ( pDocument, &imgNode );
}

CXmlSlideshow::time_object& CXmlSlideshow::img_struct::operator =(CXmlSlideshow::time_object &theOther)
{
	CXmlSlideshow::img_struct *pTheOther = (CXmlSlideshow::img_struct *)&theOther;
#ifndef QDVD_RENDER
	ImageManipulator *pMan, *pOtherMan;
	pMan = (ImageManipulator *)pModifier;
	// check if we already have a modifier ...
	if ( pMan )
		delete pMan;
	pMan = NULL;
	if (pTheOther->pModifier)	{
		pOtherMan = (ImageManipulator *)pTheOther->pModifier;
		//create a modifier, and copy over ...
		pMan = new ImageManipulator;
		*pMan = *pOtherMan;
		pModifier = pMan;
	}
	if ( pTransition )
		delete pTransition;
	pTransition = NULL;
	if ( pTheOther->pTransition )  {
		pTransition = new filter_struct;
		*pTransition = *pTheOther->pTransition;
	}
#endif
	if ( pMatrix )
		delete pMatrix;
	pMatrix = NULL;
	if ( pTheOther->pMatrix )
		pMatrix = new QMatrix ( *pTheOther->pMatrix );

	src = pTheOther->src;
	text = pTheOther->text;
	width = pTheOther->width;
	height = pTheOther->height;
	rotate  = pTheOther->rotate;
	effect[0]= pTheOther->effect[0];
	effect[1] = pTheOther->effect[1];
	fEndTime   = pTheOther->fEndTime;
	fDuration   = pTheOther->fDuration;
	fStartTime   = pTheOther->fStartTime;
	return *this;
}

bool CXmlSlideshow::img_struct::operator == ( CXmlSlideshow::time_object &theOther )
{
	CXmlSlideshow::img_struct *pTheOther = (CXmlSlideshow::img_struct *)&theOther;
	bool bReturn = ( 
		 ( src == pTheOther->src                  ) &&
		 ( text == pTheOther->text                ) &&
		 ( width == pTheOther->width              ) &&
		 ( height == pTheOther->height            ) &&
		 ( rotate  == pTheOther->rotate           ) &&
		 ( effect[0]== pTheOther->effect[0]       ) &&
		 ( effect[1] == pTheOther->effect[1]      ) &&
		 ( fEndTime   == pTheOther->fEndTime     ) &&
		 ( fDuration   == pTheOther->fDuration   ) &&
		 ( fStartTime   == pTheOther->fStartTime ) );

	if ( ! bReturn )
		return false;

	if ( ( pMatrix == NULL ) && ( pTheOther->pMatrix == NULL ) )
		return true;

	if ( pMatrix && pTheOther->pMatrix )  {
	// return ( *pMatrix == *pTheOther->pMatrix ); Does not work ...
		bReturn = (
		  fabs ( pMatrix->m11 ( ) - pTheOther->pMatrix->m11 ( ) ) < 0.000001 &&
		  fabs ( pMatrix->m12 ( ) - pTheOther->pMatrix->m12 ( ) ) < 0.000001 &&
		  fabs ( pMatrix->m21 ( ) - pTheOther->pMatrix->m21 ( ) ) < 0.000001 &&
		  fabs ( pMatrix->m22 ( ) - pTheOther->pMatrix->m22 ( ) ) < 0.000001 &&
		  fabs ( pMatrix->dx  ( ) - pTheOther->pMatrix->dx  ( ) ) < 0.000001 &&
		  fabs ( pMatrix->dy  ( ) - pTheOther->pMatrix->dy  ( ) ) < 0.000001 );
/*
printf ( "%s = %d > %f:%f=%d, %f:%f=%d, %f:%f=%d, %f:%f=%d, %f:%f=%d, %f:%f=%d \n", src.ascii(), bReturn, 
pMatrix->m11 ( ), pTheOther->pMatrix->m11 ( ), fabs( pMatrix->m11 ( ) - pTheOther->pMatrix->m11 ( ) ) < 0.0001,
pMatrix->m12 ( ), pTheOther->pMatrix->m12 ( ), fabs( pMatrix->m12 ( ) - pTheOther->pMatrix->m12 ( ) ) < 0.0001,
pMatrix->m21 ( ), pTheOther->pMatrix->m21 ( ), fabs( pMatrix->m21 ( ) - pTheOther->pMatrix->m21 ( ) ) < 0.0001,
pMatrix->m22 ( ), pTheOther->pMatrix->m22 ( ), fabs( pMatrix->m22 ( ) - pTheOther->pMatrix->m22 ( ) ) < 0.0001,
pMatrix->dx  ( ), pTheOther->pMatrix->dx  ( ), fabs( pMatrix->dx  ( ) - pTheOther->pMatrix->dx  ( ) ) < 0.0001,
pMatrix->dy  ( ), pTheOther->pMatrix->dy  ( ), fabs( pMatrix->dy  ( ) - pTheOther->pMatrix->dy  ( ) ) < 0.0001 );
*/
		return bReturn;
	}
	return false;
}

CXmlSlideshow::vid_struct::vid_struct ( )
	: img_struct ( )
{
	node_name = QString ( VID_NODE );
	length = 0L;
}

CXmlSlideshow::vid_struct::~vid_struct ( )
{
}

bool CXmlSlideshow::vid_struct::readXml (QDomElement *pDocElem)
{
	if ( pDocElem->tagName ( ) != node_name )
		return false;
	// So lets get first the attributes for this node.
	QDomAttr a = pDocElem->attributeNode ( VID_LEN );
	length = a.value ( ).toUInt ( );
	return img_struct::readXml  ( pDocElem );
}

bool CXmlSlideshow::vid_struct::writeXml ( QDomDocument *pDocument, QDomElement *pNodeElement )
{
//	debug_out ("CXmlSlideshow::img_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);
	QDomElement imgNode = pDocument->createElement( node_name );  // <img>

	if ( ! src.isEmpty  ( ) )
		imgNode.setAttribute( IMG_SRC, src );
	if ( ! text.isEmpty ( ) )
		imgNode.setAttribute( IMG_TEXT, text );
	if ( length > 0 )
		imgNode.setAttribute( VID_LEN, QString ("%1").arg ( length ) );
	if ( width > 1 )
		imgNode.setAttribute( IMG_WIDTH, QString ("%1").arg ( width ) );
	if ( height > 1 )
		imgNode.setAttribute( IMG_HEIGHT, QString ("%1").arg(height) );
	if ( rotate != 0.0 )
		imgNode.setAttribute( IMG_ROTATE, QString ("%1").arg(rotate) );
	if ( fDuration > -1.0f )
		imgNode.setAttribute( IMG_DURATION, QString ("%1").arg(fDuration) );
	if ( pMatrix )  {
		QString qsMatrix = QString ( "%1,%2,%3,%4,%5,%6" )
		.arg ( pMatrix->m11 ( ) ).arg ( pMatrix->m12 ( ) )
		.arg ( pMatrix->m21 ( ) ).arg ( pMatrix->m22 ( ) )
		.arg ( pMatrix->dx  ( ) ).arg ( pMatrix->dy  ( ) );
		imgNode.setAttribute( IMG_MATRIX, qsMatrix );
	}

	pNodeElement->appendChild ( imgNode );
	if ( pTransition )
		pTransition->writeXml ( pDocument, &imgNode );
	effect[0].writeXml            ( pDocument, &imgNode );
	return effect[1].writeXml     ( pDocument, &imgNode );
}

CXmlSlideshow::time_object& CXmlSlideshow::vid_struct::operator =(CXmlSlideshow::time_object &theOther)
{

	CXmlSlideshow::vid_struct *pTheOther = (CXmlSlideshow::vid_struct *)&theOther;
//	CXmlSlideshow::img_struct *pMe  = (CXmlSlideshow::img_struct *)this;
//	CXmlSlideshow::img_struct *pHim = (CXmlSlideshow::img_struct *)&theOther;
//	*pMe   = *pHim;
#ifndef QDVD_RENDER
	ImageManipulator *pMan, *pOtherMan;
	pMan = (ImageManipulator *)pModifier;
	// check if we already have a modifier ...
	if ( pMan )
		delete pMan;
	pMan = NULL;
	if (pTheOther->pModifier)	{
		pOtherMan = (ImageManipulator *)pTheOther->pModifier;
		//create a modifier, and copy over ...
		pMan = new ImageManipulator;
		*pMan = *pOtherMan;
		pModifier = pMan;
	}
	if ( pTransition )
		delete pTransition;
	pTransition = NULL;
	if ( pTheOther->pTransition )  {
		pTransition = new filter_struct;
		*pTransition = *pTheOther->pTransition;
	}
#endif
	if ( pMatrix )
		delete pMatrix;
	pMatrix = NULL;
	if ( pTheOther->pMatrix )
		pMatrix = new QMatrix ( *pTheOther->pMatrix );

	src = pTheOther->src;
	text = pTheOther->text;
	width = pTheOther->width;
	height = pTheOther->height;
	rotate  = pTheOther->rotate;
	effect[0]= pTheOther->effect[0];
	effect[1] = pTheOther->effect[1];
	fEndTime   = pTheOther->fEndTime;
	fDuration   = pTheOther->fDuration;
	fStartTime   = pTheOther->fStartTime;

	length = pTheOther->length;

	return *this;
}
bool CXmlSlideshow::vid_struct::operator == ( CXmlSlideshow::time_object &theOther )
{
	CXmlSlideshow::vid_struct *pTheOther = (CXmlSlideshow::vid_struct *)&theOther;
	if ( length != pTheOther->length )
	  return false;

	bool bReturn = ( 
		 ( src == pTheOther->src                 ) &&
		 ( text == pTheOther->text               ) &&
		 ( width == pTheOther->width             ) &&
		 ( height == pTheOther->height           ) &&
		 ( rotate  == pTheOther->rotate          ) &&
		 ( effect[0]== pTheOther->effect[0]      ) &&
		 ( effect[1] == pTheOther->effect[1]     ) &&
		 ( fEndTime   == pTheOther->fEndTime     ) &&
		 ( fDuration   == pTheOther->fDuration   ) &&
		 ( fStartTime   == pTheOther->fStartTime ) );

	if ( ! bReturn )
		return false;

	if ( ( pMatrix == NULL ) && ( pTheOther->pMatrix == NULL ) )
		return true;

	if ( pMatrix && pTheOther->pMatrix )
		return ( *pMatrix == *pTheOther->pMatrix );

	return false;
}

CXmlSlideshow::filter_struct::filter_struct ( )
	: time_object ( )
{
	bUserDefined = false;
	node_name    = QString ( "filter" );
	name         = QString ( "" );	// the type of filter, e.g. crossfade/fadein/fadeout
	subtitle     = QString ( "" );
	bImgBackgroundImg = false;
}

bool CXmlSlideshow::filter_struct::readXml (QDomElement *pDocElem)
{
	if (pDocElem->tagName() != node_name)
		return false;
	// So lets get first the attributes for this node.
	QDomAttr a = pDocElem->attributeNode ( FILTER_NAME );
	name = a.value();
	a = pDocElem->attributeNode ( FILTER_IMG_BKG_IMG );
	bImgBackgroundImg = ( a.value ( )  ==  "true" ) ? true : false;
	a = pDocElem->attributeNode ( FILTER_USER_DEF );
	bUserDefined      = ( a.value ( )  ==  "true" ) ? true : false;
	a = pDocElem->attributeNode ( FILTER_DURATION );
	fDuration = a.value().toFloat();
	a = pDocElem->attributeNode ( FILTER_SUBTITLE );
	subtitle = a.value();
	return true;
}

bool CXmlSlideshow::filter_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
//	debug_out ("CXmlSlideshow::effect_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);
	QDomElement filterNode = pDocument->createElement( node_name );	// <img>

	if (!name.isNull())
		filterNode.setAttribute( FILTER_NAME, name );
	if ( bImgBackgroundImg )
		filterNode.setAttribute( FILTER_IMG_BKG_IMG, "true" );
	if ( bUserDefined )
		filterNode.setAttribute( FILTER_USER_DEF, "true" );
	if (fDuration > 0.0f)
		filterNode.setAttribute( FILTER_DURATION, QString ("%1").arg(fDuration) );
	if (!subtitle.isEmpty())
		filterNode.setAttribute( FILTER_SUBTITLE, subtitle );
	pNodeElement->appendChild( filterNode );
	return true;
}

CXmlSlideshow::time_object& CXmlSlideshow::filter_struct::operator =(CXmlSlideshow::time_object &theOther)
{
	CXmlSlideshow::filter_struct *pTheOther = (CXmlSlideshow::filter_struct *)&theOther;
	name   = pTheOther->name;
	subtitle= pTheOther->subtitle;
	fEndTime = pTheOther->fEndTime;
	fDuration = pTheOther->fDuration;
	fStartTime = pTheOther->fStartTime;
	bUserDefined = pTheOther->bUserDefined;
	bImgBackgroundImg= pTheOther->bImgBackgroundImg;

	return *this;
}
bool CXmlSlideshow::filter_struct::operator == ( CXmlSlideshow::time_object &theOther )
{
	CXmlSlideshow::filter_struct *pTheOther = (CXmlSlideshow::filter_struct *)&theOther;
	return ( ( name   == pTheOther->name               ) &&
		 ( subtitle== pTheOther->subtitle          ) &&
		 ( fEndTime == pTheOther->fEndTime         ) &&
		 ( fDuration == pTheOther->fDuration       ) &&
		 ( fStartTime == pTheOther->fStartTime     ) &&
		 ( bUserDefined == pTheOther->bUserDefined ) &&
		 ( bImgBackgroundImg== pTheOther->bImgBackgroundImg ) );
}

CXmlSlideshow::CXmlSlideshow ( )
{
	node_name     = QString ( SLIDESHOW_TAG );
	delay         = -1.0;
	filter_delay  = -1.0;
	xres          =  720;
	yres          =  480; // NTSC for now
	aspect        =    0; // 4:3
	intro_page    = true;
	fade_out      = true;
	id            = NULL;
	total_audio_length = 0;
	bImgBackgroundImg  = false;
	bKenBurns          = false;
}

CXmlSlideshow::~CXmlSlideshow ()
{
	clear ( );
}

uint CXmlSlideshow::vidsLength ( )
{
	uint iTotalPlayTime = 0;
	vid_struct *pVid = NULL;
	//Q3ValueList<vid_struct *>::iterator it = m_listVids.begin ( );	//ooo
	QList<vid_struct *>::iterator it = m_listVids.begin ( );		//xxx
	while ( it != m_listVids.end ( ) )  {
		pVid = *it++;
		if ( ! pVid )
			continue;
		if ( pVid->length == 0 )
			iTotalPlayTime += (uint)delay;
		else
			iTotalPlayTime += (uint)( (double)pVid->length / 1000.0 );
	}
	return iTotalPlayTime;
}

uint CXmlSlideshow::audioLength ( )
{
	return total_audio_length;
}

uint CXmlSlideshow::slideLength ( )
{
	uint iTotalPlayTime = 0;
	iTotalPlayTime += (int)( count      ( ) * filter_delay );
	iTotalPlayTime += (int)( countImg   ( ) * delay );
	iTotalPlayTime += (int)( vidsLength ( ) );
	iTotalPlayTime += (int)( filter_delay );

	return iTotalPlayTime;
}

void CXmlSlideshow::clear ( )
{
	clearObjects ( );
	delay        = -1.0f;
	filter_delay = -1.0f;
	xres         = 720;
	yres         = 480;  // NTSC for now
	aspect       =   0;  // 4:3
	intro_page   = true;
	fade_out     = true;
	id           = NULL;
	background   = QString ( "" );
	audio_list.clear ( );
	validFilterNames.clear ( );
}

void CXmlSlideshow::clearObjects ( )
{
	// Lets clean up a little bit ...
	int t=0;
	// The timObjectArray holds Img, and Filter objects, thus no need to delete these again.
	time_object *pTimeObject;
	for ( t=0; t<(int)m_listTimeObjects.count(); t++ )	{
		pTimeObject = m_listTimeObjects[t];
		delete pTimeObject;
	}

	m_listTimeObjects.clear ( );
	m_listFilter.clear      ( );
	m_listImages.clear      ( );
	m_listVids.clear        ( );
	audio_list.clear        ( );
	validFilterNames.clear  ( );
	total_audio_length = 0;
}

bool CXmlSlideshow::kenBurns ( )
{
	return bKenBurns;
}

bool CXmlSlideshow::imgBkgImg ( )
{
	return bImgBackgroundImg;
	bool bReturn = false;
	if ( count ( ) > 0 )  {
		filter_struct *pFilter = (filter_struct *)getTimeObject ( 0 )->pTransition;
		if ( pFilter )
			return pFilter->bImgBackgroundImg;
	}
	return bReturn;
}

void CXmlSlideshow::setKenBurns ( bool bSet )
{
  if ( bSet && ! bKenBurns )  {
    uint iImgCount = countImg ( );
    for ( uint t=0; t<iImgCount; t++ )  {
      CXmlSlideshow::img_struct *pImg = getImg ( t );
      if ( ! pImg )
        continue;
      // don't force it if the img already has KenBurns
      pImg->effect[0].randomKenBurns ( 0.3, pImg->width, pImg->height, false );
    }
  }
  bKenBurns = bSet;
}

void CXmlSlideshow::setImgBkgImg ( bool bImgBkgImg )
{
  // For now we stay with the global var.
  bImgBackgroundImg = bImgBkgImg;
  if ( count ( ) > 0 )  {
    filter_struct *pFilter = (filter_struct *)getTimeObject ( 0 )->pTransition;
    if ( pFilter )
         pFilter->bImgBackgroundImg = bImgBkgImg;
  }
}

CXmlSlideshow &CXmlSlideshow::operator = (CXmlSlideshow &theOther)
{
	uint t;
	time_object *pTimeObject, *pNewObject;

	clear ();
	delay          = theOther.delay;
	filter_delay   = theOther.filter_delay;
	xres           = theOther.xres;
	yres           = theOther.yres;
	intro_page     = theOther.intro_page;
	aspect         = theOther.aspect;
	fade_out       = theOther.fade_out;
	id             = theOther.id;
	background     = theOther.background;
	slideshow_name = theOther.slideshow_name;
	audio_list     = theOther.audio_list;
	bKenBurns      = theOther.bKenBurns;
	validFilterNames   = theOther.validFilterNames;
	bImgBackgroundImg  = theOther.bImgBackgroundImg;
	total_audio_length = theOther.total_audio_length;
	// Here we copy the objects over ..
	for (t=0;t<theOther.count();t++)	{
		pTimeObject = theOther.getTimeObject(t);
		if (pTimeObject->node_name == IMG_NODE )
			pNewObject = addImg    ( );  //new img_struct;
		else if (pTimeObject->node_name == VID_NODE )
			pNewObject = addVid    ( );  //new img_struct;
		else
			pNewObject = addFilter ( );  //new filter_struct;
		*pNewObject = *pTimeObject;
	}
	return *this;
}

bool CXmlSlideshow::operator == ( CXmlSlideshow &theOther )
{
	uint t;
	time_object *p1, *p2;

	bool bReturn = (
		( delay            == theOther.delay            ) &&
		( filter_delay     == theOther.filter_delay     ) &&
		( xres             == theOther.xres             ) &&
		( yres             == theOther.yres             ) &&
		( intro_page       == theOther.intro_page       ) &&
		( aspect           == theOther.aspect           ) &&
		( fade_out         == theOther.fade_out         ) &&
		( background       == theOther.background       ) &&
		( slideshow_name   == theOther.slideshow_name   ) &&
		( imgBkgImg ( )    == theOther.imgBkgImg ( )    ) &&
		( validFilterNames == theOther.validFilterNames ) &&
		( audio_list       == theOther.audio_list       )  );
/*
printf ( "%s:%d > XmlSlideshow<%s> bRet<%s> cnt<%d vs %d> ", __FILE__, __LINE__, slideshow_name.ascii(), bReturn ? "true" : "false", count(),theOther.count() );
for ( t=0; t<count ( ); t++ )  {
	p1 = getTimeObject ( t );
	p2 = theOther.getTimeObject ( t );
        img_struct *pImg = NULL;
        if ( p1->node_name == IMG_NODE )  
           pImg = (img_struct *)p1;
        if ( p1->node_name == VID_NODE )
           pImg = (img_struct *)p1;
printf ( "\n[%d]%s,%s=<%s> <%s>", t, p1->node_name.ascii(), p2->node_name.ascii(), ( *p1 == *p2 ) ? "true" : "false", pImg ? pImg->src.ascii() : "NotImageObj" );
}
printf ("\n" );
*/
	if ( ! bReturn )
		return false;

	if ( count ( )  != theOther.count ( ) )
		return false;

	// Here we copy the objects over ..
	for ( t=0; t<count ( ); t++ )  {
		p1 = getTimeObject ( t );
		p2 = theOther.getTimeObject ( t );
		if ( ! ( *p1 == *p2 ) )
			return false;
	}
	// Went through all this hassle. Deserves a true 'true' :)
	return true;
}

bool CXmlSlideshow::readXml ()
{
	// Here we read in a xml - file and create the neccesary underlying structure.
	//
	// For now we are going to ask for the file name here and handle the QDom...
	// Later on this is done a level further up and only QDomNode * is sent.
	//
	//////////////////////////////////////////////////////////////////////////////////
//#if (QT_VERSION > 0x0301FF)
#ifdef QDVD_RENDER
	//QString fileName = Q3FileDialog::getOpenFileName ( NULL, QString("./"), QObject::tr ("Slideshow files ( *.slide)"));		//ooo
	QString fileName = QFileDialog::getOpenFileName ( NULL, QString("./"), QObject::tr ("Slideshow files ( *.slide)"));		//xxx
#else
	QString fileName = Q3FileDialog::getOpenFileName ( QString("./"), QObject::tr ("Slideshow files ( *.slide)"));
#endif
	return readXml (fileName);
}

bool CXmlSlideshow::readXml ( QString &fileName, bool bMessageBox )
{
	// Here we read in a xml - file and create the neccesary underlying structure.
	//
	//////////////////////////////////////////////////////////////////////////////////
	// Assign the file
	QFile projectFile ( fileName );
	QString qsErrorMsg;
	int iErrorLine;
	int iErrorCol;

#ifdef QDVD_RENDER
	if ( ! projectFile.open ( QIODevice::ReadWrite ) )
		return false;
#else
	if ( ! projectFile.open ( QIODevice::ReadWrite ) )
		return false;
#endif
	QDomDocument xmlDoc ( SLIDESHOW_DOCTYPE );
	if ( ! xmlDoc.setContent ( &projectFile, true, &qsErrorMsg, &iErrorLine, &iErrorCol ) )  {
		// Error handling ...
		projectFile.close ( );
		QString qsHeader = QObject::tr ( "Slideshow file seems to be defective." );
		QString qsBody   = QObject::tr ( "There is a problem at line %1 at column %2\nwith your slideshow file %3\nError string = %4\n\nDo you want to try to load another slideshow file ?" ) 
			.arg ( iErrorLine ).arg ( iErrorCol). arg ( fileName ).arg ( qsErrorMsg);
		if ( bMessageBox )  {
			int iReturn = QMessageBox::warning ( NULL, qsHeader, qsBody, QMessageBox::Yes, QMessageBox::No );
			if (iReturn == QMessageBox::Yes)
				return readXml (); // Okay, user wants to specify another project file.
		}
		else  {
			//printf ( "Slideshow file <%s> seems to be defective\n", (const char *)fileName.toAscii ( ) );		//ooo
			printf ( "Slideshow file <%s> seems to be defective\n", fileName.toLatin1().data() );			//xxx
			//printf ( "Error row<%d> col<%d> . ErrorString<%s>\n", iErrorLine, iErrorCol, (const char *)qsErrorMsg.toAscii ( ) );		//ooo
			printf ( "Error row<%d> col<%d> . ErrorString<%s>\n", iErrorLine, iErrorCol, qsErrorMsg.toLatin1().data() );			//xxx
			return false;
		}
	}

	QDomElement docElem = xmlDoc.documentElement ( );
	bool bReturn = readXml ( &docElem );
	projectFile.close ( );
	return bReturn;
}

bool CXmlSlideshow::readXml ( QDomElement *pElement )
{
	// And now read in all remaining nodes and handle them accordingly.
	img_struct    tempImg;     // temp to get the node_name.
	vid_struct    tempVid;     // temp to get the node_name.
	filter_struct tempFilter;  // temp to get the node_name.
	time_object   *pTimeObject = NULL;
	bool bReturn = false;
	intro_page   = true;
	aspect       = 0;
	fade_out     = true;
	QString audioList, filterList;

	QDomNode xmlNode = pElement->firstChild ( );
	QDomAttr a = pElement->attributeNode ( SLIDESHOW_DELAY );
	delay = a.value ( ).toFloat ( );
	a = pElement->attributeNode ( SLIDESHOW_INTRO );
	if ( !  a.value ( ).isEmpty ( ) )
		intro_page = a.value( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_ASPECT );
	if ( !  a.value ( ).isEmpty ( ) )
		aspect = a.value( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_FADE_OUT );
	if ( !  a.value ( ).isEmpty ( ) )
		fade_out = a.value  ( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_FILTER_DELAY );
	filter_delay    = a.value ( ).toFloat ( );
	a = pElement->attributeNode ( SLIDESHOW_BACKGROUND );
	background      = a.value ( );
	a = pElement->attributeNode ( SLIDESHOW_NAME );
	slideshow_name  = a.value ( );
	a = pElement->attributeNode ( SLIDESHOW_XRES );
	xres  = a.value ( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_YRES );
	yres  = a.value ( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_AUDIO_LENGTH );
	total_audio_length= a.value ( ).toInt ( );
	a = pElement->attributeNode ( SLIDESHOW_IMG_BKG_IMG  );
	bImgBackgroundImg = ( a.value ( ) == "true" ) ? true : false;
	a = pElement->attributeNode ( SLIDESHOW_KEN_BURNS );
	bKenBurns  = ( a.value ( ) == "true" ) ? true : false;
	audioList  = pElement->attributeNode ( SLIDESHOW_AUDIO_LIST  ).value ( );
	filterList = pElement->attributeNode ( SLIDESHOW_FILTER_LIST ).value ( );
#ifdef QDVD_RENDER
	audio_list       = audioList.split  ( "+-+" );
	validFilterNames = filterList.split ( "+-+" );
#else
	audio_list       = QStringList::split ( "+-+", audioList  );
	validFilterNames = QStringList::split ( "+-+", filterList );
#endif
	if (xres < 1)
		xres = 720;
	if (yres < 1)
		yres = 480;

	if ( delay <= 0.0f )
		delay = 5.0f;
	if (filter_delay <= 0.0f )
		filter_delay = 3.0f;
	float fCurrentTime = 0.0f;
	while( ! xmlNode.isNull ( ) ) {
		QDomElement searchTree = xmlNode.toElement ( );
		pTimeObject = NULL;
		if ( searchTree.tagName ( ) == tempImg.node_name )
			pTimeObject = (time_object *)addImg ( );
		if ( searchTree.tagName ( ) == tempVid.node_name )
			pTimeObject = (time_object *)addVid ( );
		else if ( searchTree.tagName ( ) == tempFilter.node_name )
			pTimeObject = (time_object *)addFilter ( );
		if ( pTimeObject )
			bReturn = pTimeObject->readXml ( &searchTree );
		else
			return false;
		// If there has been a problem then return false.
		if ( ! bReturn )
			return false;

		// And here we calculate some extra values ...
		if (pTimeObject->fDuration <= 0.0f) {
			if (searchTree.tagName() == tempImg.node_name)
				pTimeObject->fDuration = delay;
			else
				pTimeObject->fDuration = filter_delay;
		}
		pTimeObject->fStartTime = fCurrentTime;
		fCurrentTime += pTimeObject->fDuration;
		pTimeObject->fEndTime = fCurrentTime;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling ( );
	}
	return true;
}

bool CXmlSlideshow::writeXml ()
{
	// Here we write to a xml - file and create the neccesary underlying structure.
	//
	// For now we are going to ask for the file name here and handle the QDom...
	// Later on this is done a level further up and only QDomNode * is sent.
	//
	//////////////////////////////////////////////////////////////////////////////////
#ifdef QDVD_RENDER
	//QString fileName = Q3FileDialog::getSaveFileName ( NULL, QString("./"), QObject::tr ("Slideshow files ( *.slide)"));		//ooo
	QString fileName = QFileDialog::getSaveFileName ( NULL, QString("./"), QObject::tr ("Slideshow files ( *.slide)"));		//xxx
#else
	QString fileName = Q3FileDialog::getSaveFileName ( QString("./"), QObject::tr ("Slideshow files ( *.slide)"));
#endif
	if (fileName.isNull())
		return false;

	return writeXml(fileName);
}

bool CXmlSlideshow::writeXml (QString &fileName)    //ooooo
{
//	debug_out ("CXmlSlideshow::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);
	// Assign the file
	QFile projectFile ( fileName );
#ifdef QDVD_RENDER
	if ( ! projectFile.open ( QIODevice::WriteOnly ) )
		return false;
#else
	if ( ! projectFile.open ( QIODevice::WriteOnly ) )
		return false;
#endif
	QDomDocument xmlDoc ( SLIDESHOW_DOCTYPE );	// <""> for now.
	QDomElement  slideshowNode = xmlDoc.createElement ( node_name );  // <slideshow>
	bool bReturn = writeXml ( &slideshowNode );
	QString xml  = xmlDoc.toString  ( );
//	printf ("%s\n", (const char *)xml);
#ifdef QDVD_RENDER
	projectFile.write ( xml.toUtf8 ( ), (qint64)xml.toUtf8 ( ).length ( ) );
#else
	projectFile.writeBlock ( xml.utf8 ( ), (Q_INT64)xml.utf8 ( ).length ( ) );
#endif
	projectFile.close ( );
	return bReturn;
}

bool CXmlSlideshow::writeXml ( QDomElement *pElement )
{
	int t;
	QDomDocument xmlDoc = pElement->ownerDocument();

	if ( delay > 0.0f )
		pElement->setAttribute( SLIDESHOW_DELAY,  QString ( "%1" ).arg ( delay ) );
	if ( filter_delay > 0.0f )
		pElement->setAttribute( SLIDESHOW_FILTER_DELAY,  QString ( "%1" ).arg ( filter_delay ) );
	if ( ! background.isNull ( ) )
		pElement->setAttribute( SLIDESHOW_BACKGROUND, background );
	if ( ! slideshow_name.isNull  ( ) )
		pElement->setAttribute( SLIDESHOW_NAME, slideshow_name );
	if ( xres > 0 )
		pElement->setAttribute( SLIDESHOW_XRES,  QString ("%1").arg ( xres ) );
	if ( yres > 0 )
		pElement->setAttribute( SLIDESHOW_YRES,  QString ("%1").arg ( yres ) );
	if ( total_audio_length > 0 )
		pElement->setAttribute( SLIDESHOW_AUDIO_LENGTH,  QString ("%1").arg ( total_audio_length ) );
	if ( bKenBurns )
		pElement->setAttribute( SLIDESHOW_KEN_BURNS, "true" );
	if ( bImgBackgroundImg )
		pElement->setAttribute( SLIDESHOW_IMG_BKG_IMG, "true" );
	if ( ! intro_page )
		pElement->setAttribute( SLIDESHOW_INTRO, QString ("%1").arg ( (int)intro_page ) );
	if ( aspect )
		pElement->setAttribute( SLIDESHOW_ASPECT, QString ("%1").arg ( aspect ) );
	if ( ! fade_out )
		pElement->setAttribute( SLIDESHOW_FADE_OUT, QString ("%1").arg ( (int)fade_out ) );
	if ( audio_list.count ( ) > 0 )
		pElement->setAttribute( SLIDESHOW_AUDIO_LIST, audio_list.join (QString ("+-+")) );
	if ( validFilterNames.count ( ) > 0 )
		pElement->setAttribute( SLIDESHOW_FILTER_LIST, validFilterNames.join (QString ("+-+")) );
	// And now proceed to writing the rest of the file.
	xmlDoc.appendChild ( *pElement );

	for ( t=0; t<(int)m_listTimeObjects.count ( ); t++ )
		m_listTimeObjects[t]->writeXml ( &xmlDoc, pElement );
	return true;
}

uint CXmlSlideshow::count ()
{
	return m_listTimeObjects.count();
}

int CXmlSlideshow::findImg ( CXmlSlideshow::img_struct *pImg )
{
	uint t;
	for (t=0;t<count();t++)	{
		if (m_listTimeObjects[t] == (time_object *)pImg)
			return t;
	}
	return -1;
}

uint CXmlSlideshow::countImg ( )
{
  return m_listImages.count ( );
}

uint CXmlSlideshow::countVid ( )
{
  return m_listVids.count ( );
}

uint CXmlSlideshow::countFilter ()
{
  return m_listFilter.count ( );
}

CXmlSlideshow::time_object *CXmlSlideshow::getTimeObject(uint iObjectNo)
{
	if (iObjectNo < (uint)m_listTimeObjects.count())
		return m_listTimeObjects[iObjectNo];
	else
		return NULL;
}

CXmlSlideshow::img_struct *CXmlSlideshow::getImg ( uint iImgNo )
{
	if ( iImgNo < countImg ( ) )
		return m_listImages[iImgNo];
	else
		return NULL;
}

CXmlSlideshow::vid_struct *CXmlSlideshow::getVid ( uint iVidNo )
{
	if ( iVidNo < countVid ( ) )
		return m_listVids[iVidNo];
	else
		return NULL;
}

CXmlSlideshow::filter_struct *CXmlSlideshow::getFilter ( uint iFilterNo )
{
	if ( iFilterNo < countFilter ( ) )
		return m_listFilter[iFilterNo];
	else
		return NULL;
}

CXmlSlideshow::img_struct *CXmlSlideshow::addImg()
{
  img_struct *pImg = new img_struct;
  m_listTimeObjects.append ( pImg );
  m_listImages.append ( pImg );
  return pImg;
}

bool CXmlSlideshow::insertImg ( uint iPos, CXmlSlideshow::img_struct *pImg )
{
  if ( iPos > (uint)m_listTimeObjects.count ( ) ) 
    return false;

  bool bRet = false;
  int  iCount = 0, iImagePos = 0;
  time_object *pTimeObject = NULL;
  QList<time_object *>::iterator it = m_listTimeObjects.begin ( );	//oxx
  while ( it != m_listTimeObjects.end ( ) )  {
    iCount++; // Always increase
    pTimeObject = *it++;
    if ( pTimeObject->node_name == IMG_NODE )
      iImagePos ++;
    if ( (uint)iCount == iPos )  {
      bRet = true;
      m_listTimeObjects.insert ( it, pImg );
      break;
    }
  }
  if ( ! bRet ) // Not yet found, then we should push it to the end ...
    m_listTimeObjects.push_back ( pImg );

  QList<img_struct *>::iterator it2 = m_listImages.begin ( );		//oxx
  while ( it2 != m_listImages.end ( ) || iImagePos-- >= 0 )  {
    it2++;
  }
  if ( it2 != m_listImages.end ( ) )
    m_listImages.insert ( it2, pImg );
  else
    m_listImages.push_back ( pImg );

  // Always true at this point
  return true;
}

bool CXmlSlideshow::insertVid ( uint iPos, CXmlSlideshow::vid_struct *pVid )
{
  if ( iPos > (uint)m_listTimeObjects.count ( ) )
    return false;

  bool bRet = false;
  int  iCount = 0, iVideoPos = 0;
  time_object *pTimeObject = NULL;
  QList<time_object *>::iterator it = m_listTimeObjects.begin ( );	//oxx
  while ( it != m_listTimeObjects.end ( ) )  {
    iCount++; // Always increase
    pTimeObject = *it++;
    if ( pTimeObject->node_name == VID_NODE )
      iVideoPos ++;
    if ( (uint)iCount == iPos )  {
      bRet = true;
      m_listTimeObjects.insert ( it, pVid );
      break;
    }
  }
  if ( ! bRet ) // Not yet found, then we should push it to the end ...
    m_listTimeObjects.push_back ( pVid );

  QList<vid_struct *>::iterator it2 = m_listVids.begin ( );		//oxx
  while ( it2 != m_listVids.end ( ) || iVideoPos-- >= 0 )  {
    it2++;
  }
  if ( it2 != m_listVids.end ( ) )
    m_listVids.insert ( it2, pVid );
  else
    m_listVids.push_back ( pVid );

  // Always true at this point
  return true;
}

void CXmlSlideshow::delImg ( CXmlSlideshow::img_struct *pImg )
{
  if ( ! pImg )
    return;
#ifdef QDVD_RENDER
  m_listTimeObjects.removeAll ( pImg );
  m_listImages.removeAll      ( pImg );
#else
  m_listTimeObjects.remove    ( pImg );
  m_listImages.remove         ( pImg );
#endif
  delete pImg;
}

CXmlSlideshow::vid_struct *CXmlSlideshow::addVid ( )
{
  vid_struct *pVid = new vid_struct;
  m_listTimeObjects.append ( pVid );
  m_listVids.append        ( pVid );
  return pVid;
}

void CXmlSlideshow::delVid ( CXmlSlideshow::vid_struct *pVid )
{
  if ( ! pVid )
    return;
#ifdef QDVD_RENDER
  m_listTimeObjects.removeAll ( pVid );
  m_listVids.removeAll        ( pVid );
#else
  m_listTimeObjects.remove    ( pVid );
  m_listVids.remove           ( pVid );
#endif
  delete pVid;
}

CXmlSlideshow::filter_struct *CXmlSlideshow::addFilter()
{
  filter_struct *pFilter = new filter_struct;
  m_listTimeObjects.append ( pFilter );
  m_listFilter.append      ( pFilter );
  return pFilter;
}

