/***************************************************************************
    utils.cpp
                             -------------------
    Class Utils implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Here we have some misc functions which are needed by a few classes
   but can not really be assigned to any of those classes.
    
****************************************************************************/

#include <stdlib.h>
//#ifdef QDVD_LINUX	//ooo
#include <unistd.h>
//#endif

//#include <qfileinfo.h>	//ooo
#include <QFileInfo>		//xxx
//#include <qregexp.h>		//ooo
#include <QRegExp>		//xxx
//#include <qimage.h>		//ooo
#include <QImage>		//xxx
//#include <qfile.h>		//ooo
#include <QFile>		//xxx
//#include <qdir.h>		//ooo
#include <QDir>			//xxx
#include <QMessageBox>		//xxx

//#include <qstylefactory.h>	//ooo
#include <QStyleFactory>	//xxx
//#include <qapplication.h>	//ooo
#include <QApplication>		//xxx
//#include <q3urloperator.h>	//ooo
#include <qmetaobject.h>	//ooo
#include <QMetaObject>		//xxx
//#include <qstyle.h>		//ooo
#include <QStyle>		//xxx
//Added by qt3to4:
//#include <Q3TextStream>	//ooo
#include <QTextStream>		//xxx
//#include <Q3ValueList>	//oxx
//#include <Q3PtrList>		//ooo
#include <QPixmap>

#include "global.h"
#include "utils.h"
//#include "messagebox.h"	//ooo

//#include "win32.h"		//ooo

Utils::Utils()
{

}

Utils::~Utils ()
{

}

float Utils::getVersionInfo ( QString &qsFileName )
{
  // Read in the file and parses or the version information
  // Note: This is a single floating point value in the first line of the file
  float fVersion = 1.0f;  // defaults to 1.0

  QRegExp rx  ( "\\d*\\.\\d+"  ); // floating point
  QString qsLine, qsPath;
  QFileInfo fileInfo ( qsFileName );
  //qsPath = fileInfo.dirPath( true );		//ooo
  qsPath = fileInfo.path( );			//xxx
  fileInfo.setFile   ( qsPath );
  if ( ! fileInfo.exists  ( ) )
    return 0.0;

  QFile     file ( qsFileName  );
  if ( file.open ( QIODevice::ReadOnly ) )  {
    //Q3TextStream stream ( &file );	//ooo
    QTextStream stream ( &file );	//xxx
    while ( ! stream.atEnd ( ) )  {
      qsLine= stream.readLine( );
      //if ( rx.search ( qsLine, 0 ) == -1 ) // line must start with a character	//ooo
      if ( rx.indexIn ( qsLine, 0 ) == -1 ) // line must start with a character		//xxx
        continue;
      //fVersion = atof ( qsLine );			//ooo
      fVersion = atof ( qsLine.toLatin1().data() );	//xxx
      break;
    }
    file.close ( );
  }

  return fVersion;
}

/*bool Utils::webToFile ( QString &qsURL, QString &qsLocal )
{
  Q3UrlOperator op; // ( qsServerVersion );
  Q3PtrList<Q3NetworkOperation> list = op.copy ( qsURL, qsLocal );
  // Next we make sure we get the whole file ( less then 1k )
  bool bDone = false;
  int iMaxCounter = 30; // 30 * 100 msec = 3sec
  while ( bDone == false )  {
    bDone = false;
    Q3PtrList<Q3NetworkOperation>::iterator it = list.begin ( );
    while ( it != list.end ( ) )  {
      Q3NetworkOperation *pNO = *it++;
      if ( pNO->state ( ) != Q3NetworkProtocol::StDone )
        break;
      if ( pNO->state ( ) == Q3NetworkProtocol::StFailed )
        return false;
      bDone = true;
    }
    if ( ! bDone )  {
      // Not yet done.
      qApp->processEvents ( );
      iMaxCounter --;
      //usleep ( 100000 ); // sleep for a 100 milli seconds.			//ooo
      SleeperThread8::msleep ( 100 ); // sleep for a 100 milli seconds.		//xxx
      if ( iMaxCounter == 0 )  // exit after max attempts.
           bDone = true;
    }
  }

  return ( iMaxCounter > 0 );
}*/	//ooo

QStringList Utils::localTemplates ( )
{
  // This function will look into the local, and system folder
  // and create a list of templates
  QString qsSubPaths[3], qsBasePath, qsPath, qsTemplate;
  QStringList list;
  QStringList::iterator it2;
  QDir theDir;
  bool bFound;
  //const QFileInfoList *pList;		//ooo
  QFileInfoList pList;			//xxx
  //QFileInfo *pFileInfo;		//ooo
  QFileInfo pFileInfo;			//xxx

  //qsBasePath = QDir::homeDirPath ( ) + "/.qdvdauthor/";	//ooo
  qsBasePath = QDir::homePath ( ) + "/.qdvdauthor/";		//xxx
  qsSubPaths[0] = "static";
  qsSubPaths[1] = "animated";
  qsSubPaths[2] = "transition";

  int t, i;
  for ( t=0; t<2; t++ )  {
    for ( i=0; i<3; i++ )  {
      qsPath = qsBasePath + qsSubPaths[i];
      theDir.setPath    ( qsPath );
      theDir.setFilter  ( QDir::Dirs );
      pList = theDir.entryInfoList ( );
      //if ( ! pList || pList->count ( ) < 1 )		//ooo
      if ( pList.isEmpty ( ) )				//xxx
        continue;

      //QFileInfoListIterator  it ( *pList );		//ooo
      QListIterator<QFileInfo> it ( pList );		//xxx
      //while( (pFileInfo = it.current ( ) ) != 0 )  {	//ooo
      while( it.hasNext( ) )  {				//xxx
        //++it;						//ooo
	pFileInfo = it.next ( );			//xxx
        //qsTemplate = pFileInfo->fileName ( );		//ooo
	qsTemplate = pFileInfo.fileName ( );		//xxx
        if ( ( qsTemplate == "."  ) ||
             ( qsTemplate == ".." ) )
          continue;
        bFound = false;
        it2 = list.begin ( );
        while ( it2 != list.end ( ) )  {
          if ( *it2++ == qsTemplate )  {
            bFound = true;
            break;
          }
        }

        if ( ! bFound )
          list.append ( qsTemplate );
      }
    }
    qsBasePath = Global::qsSystemPath + "/share/qdvdauthor/";
  }
//for (t=0; t<list.size ( ); t++ )
//  printf ( "%s \n", list[t].ascii() );
//printf ( "2 list<%d>\n", list.size() );

  return list;
}

QString Utils::findGuiSU ( )
{
  uint t;
  QString qsReturn;
  QString qsTmpOut = QString ( "%1/%2_out.txt" ).arg ( Global::qsTempPath ).arg ( getlogin ( ) );
  char arraySUs[3][10] = {"kdesu", "xsu", "gnomesu"};
  QFileInfo fileInfo ( qsTmpOut );
  for ( t=0; t<3; t++ )     {
    // the next check is to see if we can find kdesu
    //int iRet = system ((const char *)QString ( "which %1 > %2 2>/dev/null").arg ( arraySUs[t] ).arg ( qsTmpOut ) );		//ooo
    int iRet = system (QString ( "which %1 > %2 2>/dev/null").arg ( arraySUs[t] ).arg ( qsTmpOut ).toLatin1().data() );		//xxx
    iRet = iRet;
    fileInfo.setFile ( qsTmpOut );
    // Okay we cna not find it, so we wont bother asking the user ...
    if ( fileInfo.size ( ) > 4 )  {
      qsReturn = QString ( "%1" ).arg ( arraySUs[t] );
      return qsReturn;
    }
  }
  return qsReturn;
}

void Utils::createStructureIcons ( )
{
// Unknown=0, DVDProject=1, MainMenu=2, SubMenu=3, Intro=4, Extro=5, 
// Background=6, Audio=7, Buttons=8, Button=9, Frame=10, 
// Text=11, Image=12, Video=13, Highlighted=14, Selected=15, 
// Geometry=16, Modifiers=17, Attributes=18
  const char files[20][20] = { "unknown.png", "1.png", "2.png", "submenu.png", "intros.png", "intros.png", "background.png", "audio.png", "buttons.png", "button.png", "frame.png", "text.png", "image.png", "video.png", "highlighted.png", "selected.png", "geometry.png", "modifiers.png", "attributes.png", "unknown.png" };

  QPixmap final ( 32*20, 32 );
  final.fill ( 0xFF0000 );
  QPixmap temp;
  for ( int t=0; t<20; t++ )  {
    temp = QPixmap ( QString ( "/home/varol/structure_icons/" ) + files[t] );
    if ( temp.isNull ( ) )
      continue;
    //copyBlt ( &final, t*32, 0, &temp );	//ooo
  }

  final.save ( "/tmp/structure_icons.png", "PNG", 100 );
}

// Copied over from qrender -> filter.cpp
void Utils::augmentAlpha ( int iDeltaA, QImage &filter, QImage &start, QImage &stop, QImage &result )
{
  int   r, g, b, x, y;
  float fVal;
  QRgb  alpha, pix1, pix2;
  // First we copy the filter to start and end ...
  // Next we run through every pixel to increase / decrease the filters
  for  ( y=0; y<filter.height ( ); y++ )  {
    for ( x=0; x<filter.width ( ); x++ )  {
      alpha = filter.pixel ( x, y );
      pix1  = start.pixel  ( x, y );
      pix2  = stop.pixel   ( x, y );
      fVal = (float)( qRed ( alpha ) - 256 + iDeltaA ) / 255.0f; // represents the alpha value ...
      if ( fVal < 0.0f )
           fVal = 0.0f;
      if ( fVal > 1.0f )
           fVal = 1.0f;

      r = (int)( qRed   ( pix1 ) * ( 1.0f - fVal ) + qRed   ( pix2 ) * fVal );
      g = (int)( qGreen ( pix1 ) * ( 1.0f - fVal ) + qGreen ( pix2 ) * fVal );
      b = (int)( qBlue  ( pix1 ) * ( 1.0f - fVal ) + qBlue  ( pix2 ) * fVal );


      pix1 = qRgb     ( r, g, b );
      result.setPixel ( x, y, pix1 );
    }
  }
}

void Utils::augmentAlpha ( int iDeltaA, int iDeltaB, QImage &filter, QImage &start, QImage &stop, QImage &result, QImage *pBackground )
{
  int   r, g, b, x, y;
  float fVal1, fVal2, fVal3 = 0.0f;
  QRgb alpha, pix1, pix2, pix3;
  // First we copy the filter to start and end ...
  // Next we run through every pixel to increase / decrease the filters
  for  ( y=0; y<filter.height ( ); y++ )  {
    for ( x=0; x<filter.width ( ); x++ )  {
      alpha = filter.pixel ( x, y );
      pix1  = start.pixel  ( x, y );
      pix2  = stop.pixel   ( x, y );
      fVal1 = (float)( qRed ( alpha ) - 256 + iDeltaA ) / 255.0f; // represents the alpha value ...
      if ( fVal1 < 0.0f )
           fVal1 = 0.0f;
      if ( fVal1 > 1.0f )
           fVal1 = 1.0f;
      fVal2 = (float)( qRed ( alpha ) - 256 + iDeltaB ) / 255.0f; // represents the alpha value ...
      if ( fVal2 < 0.0f )
           fVal2 = 0.0f;
      if ( fVal2 > 1.0f )
           fVal2 = 1.0f;

      r = (int)( qRed   ( pix1 ) * ( 1.0f - fVal1 ) + qRed   ( pix2 ) * fVal2 );
      g = (int)( qGreen ( pix1 ) * ( 1.0f - fVal1 ) + qGreen ( pix2 ) * fVal2 );
      b = (int)( qBlue  ( pix1 ) * ( 1.0f - fVal1 ) + qBlue  ( pix2 ) * fVal2 );

      if ( pBackground )  {
        //fVal3 = 1.0f-fVal1 + fVal2;
        fVal3 = fVal2 - fVal1;
        if ( fVal3 < 0.0f )
             fVal3 = -fVal3;
        pix3  = pBackground->pixel ( x, y );
        r += (int)( qRed   ( pix3 ) * fVal3 );
        g += (int)( qGreen ( pix3 ) * fVal3 );
        b += (int)( qBlue  ( pix3 ) * fVal3 );
      }

      pix1 = qRgb     ( r, g, b );
      result.setPixel ( x, y, pix1 );
    }
  }
}

QString Utils::currentStyleName ( )
{
  QStringList list = QStyleFactory::keys();
  list.sort();

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
    QString styleName = *it;
    QStyle *candidate = QStyleFactory::create(styleName);
    Q_ASSERT(candidate);
    //if (candidate->metaObject()->className() == QApplication::style().metaObject()->className()) {	//ooo
    if (candidate->metaObject()->className() == QApplication::style()->metaObject()->className()) {	//xxx
      delete candidate;
      return styleName;
    }
    delete candidate;
  }
  return QString("Default");
}

bool Utils::getWidthHeight ( QString qsDimension, int &iWidth, int &iHeight )
{
  iWidth  = 0;
  iHeight = 0;
  //QStringList listSize = QStringList::split ( "x", qsDimension );	//ooo
  QStringList listSize = qsDimension.split ( "x" );			//xxx
  bool bReturn = false;
  if ( listSize.count ( ) > 1 )  {
    iWidth  = listSize[0].toInt ( );
    iHeight = listSize[1].toInt ( );
    bReturn  = ( ( iWidth > 10 ) && ( iHeight > 10 ) );
  }
  return bReturn;
}


QString Utils::formatChapter (QString qsInput)
{
	char cBuffer[20];
	QString qsReturn ("00:00:00.000");
	if (qsInput == QString ("0"))
		return qsReturn;
	// Here we tidy up the chapter. E.g. 0 gets converted to 00:00:00.000, 0.0.32 to 00:00:32.000 etc
	//QStringList listParts = QStringList::split(":", qsInput);	//ooo
	QStringList listParts = qsInput.split(":");			//xxx
	// SanityCheck ...
	if (listParts.count() < 3)
		return qsReturn;
	int iInput = (int)listParts[2].toFloat();
	sprintf (cBuffer, "%02d:%02d:%02d.%03d", listParts[0].toInt(), listParts[1].toInt(), iInput, (int)((listParts[2].toFloat() - iInput) * 1000));
	qsReturn = QString (cBuffer);
	return qsReturn;
}

QString Utils::longToNorm ( long iInput )
{
	// This function returns a string like 4400k from 4400000
	QString qsReturn;
	float fValue;
	if (iInput > 1000000000)	{
		fValue = (double)iInput / 1000000000.0;
		qsReturn = QString ("%1G").arg(fValue);
	}
	else if (iInput > 1000000)	{
		fValue = (double)iInput / 1000000.0;
		qsReturn = QString ("%1M").arg(fValue);
	}
	else if (iInput > 1000)	{
		fValue = (double)iInput / 1000.0;
		qsReturn = QString ("%1k").arg(fValue);
	}
	else	{
		qsReturn = QString ("%1").arg(iInput);
	}
	return qsReturn;
}

long Utils::normToLong ( QString qsInput )
{
	QString qsValue, arrayDigit (".0123456789");
	uint t, i;
	long iReturn, iMultiplicator = 1;
	// Here we take a string like 4400kbps and return 4400000.
	for (t=0;t<(uint)qsInput.length();t++)	{	//ox
		for (i=0;i<(uint)arrayDigit.length();i++)	{	//ox
			if ( qsInput.at ( t ) == arrayDigit.at ( i ) )	{
				qsValue += qsInput.at ( t );
				i = 10;
				continue;
			}
		}
		//if (qsInput.at ( t ).upper () == "K")	{	//ooo
		if (qsInput.at ( t ).toUpper () == 'K')	{	//xxx
			iMultiplicator = 1000;
			break;
		}
		//if (qsInput.at ( t ).upper () == "M")	{	//ooo
		if (qsInput.at ( t ).toUpper () == 'M')	{	//xxx
			iMultiplicator = 1000000;
			break;
		}
		//if (qsInput.at ( t ).upper () == "G")	{	//ooo
		if (qsInput.at ( t ).toUpper () == 'G')	{	//xxx
			iMultiplicator = 1000000000;
			break;
		}
	}
	iReturn = (long)(qsValue.toFloat() * iMultiplicator);
	return iReturn;
}

QString Utils::getAttribute (QString qsAttrName, QString qsFullString)
{
	QString qsReturn;
	int iStart, iEnd;
	//iStart = qsFullString.find (qsAttrName);	//ooo
	iStart = qsFullString.indexOf (qsAttrName);	//xxx
	if (iStart < 1)
		return qsReturn;
	// increase iStart to skip e.g.[chapters="] and get to the start of the chapters.
	iStart += qsAttrName.length() + 2;
	// Now we find the end ...
	//iEnd = qsFullString.find ("\"", iStart);	//ooo
	iEnd = qsFullString.indexOf ("\"", iStart);	//xxx
	if (iEnd < 1) 
		return qsReturn;
	qsReturn = qsFullString.mid (iStart, iEnd-iStart);
	return qsReturn;
}
/* Some issues ...
QRgb Utils::nearestColor (QRgb *rgbPalette, int iNumColors, QRgb rgbSeeking)
{
  QRgb rgbReturn = rgbPalette[0];
  int t, iRed, iGreen, iBlue, r, g, b;
  unsigned long iSquare, iClosestColor = 0x1ff*0x1ff*3+1;

  r = qRed   (rgbSeeking) - 255;
  g = qGreen (rgbSeeking) - 255;
  b = qBlue  (rgbSeeking) - 255;

  for (t=0;t<iNumColors;t++)	{
    iRed    = qRed   (rgbPalette[t]) - r;
    iGreen  = qGreen (rgbPalette[t]) - g;
    iBlue   = qBlue  (rgbPalette[t]) - b;
    iSquare = iRed*iRed + iGreen*iGreen + iBlue*iBlue;

    // Now let us check if we found a color closer then the previous one.
    if (   iSquare  < iClosestColor   )  {
      rgbReturn     = rgbPalette[t];
      iClosestColor = iSquare;
    }
  }
  return rgbReturn;
}
*/

QRgb Utils::nearestColor (QRgb *rgbPalette, int iNumColors, QRgb rgbSeeking)
{
  QRgb rgbReturn = rgbPalette[0];
  int t, iRed, iGreen, iBlue, r, g, b;
  unsigned long iSquare, iClosestColor = 0xff*0xff*3+1;

  r = qRed   (rgbSeeking);
  g = qGreen (rgbSeeking);
  b = qBlue  (rgbSeeking);

  for (t=0;t<iNumColors;t++)	{
    iRed    = qRed   (rgbPalette[t]) - r;
    iGreen  = qGreen (rgbPalette[t]) - g;
    iBlue   = qBlue  (rgbPalette[t]) - b;
    iSquare = iRed*iRed + iGreen*iGreen + iBlue*iBlue;

    // Now let us check if we found a color closer then the previous one.
    if (   iSquare  < iClosestColor   )  {
      rgbReturn     = rgbPalette[t];
      iClosestColor = iSquare;
    }
  }
  return rgbReturn;
}

/**
 * After a lot of frustration I decided to write this little (slow) routine, to
 * - reduce the used colors to 4 AND
 * - merge all used colors to the lower parts of the palette
 * - reduce the number of colors in the palette.
 * This is needed for the button masks in QDVDAuthor. Max = 4 colors shared between
 * selected and highlighted mask.
 */
void Utils::reduceColors ( QImage &theImage, int iNumColors, QRgb *pPalette )
{
  bool bFound;
  uchar *pLine;
  unsigned long iCounter = 0;
  int  x, y, t, iIndex, iImageColors;
  //QRgb theNearestColor, theColor, *pImageColors;	//ooo
  QRgb theNearestColor, theColor;			//xxx
  QVector<QRgb> pImageColors;				//xxx

  // First we have to ensure that we do have a color indexed image.
  //theImage = theImage.convertDepthWithPalette (8, (QRgb *)pPalette, iNumColors, Qt::ThresholdDither);		//ooo
  theImage = theImage.convertToFormat (QImage::Format_Indexed8, Qt::ThresholdDither);		//xxx

  // This will create a image which might have more then the requested colors.
  // So we replace all colors with the nearest color of the four.
  pImageColors = theImage.colorTable ();
  //iImageColors = theImage.numColors  ();	//ooo
  iImageColors = theImage.colorCount  ();	//xxx
  for (t=0;(int)t<iImageColors; t++) {
    theNearestColor = nearestColor (pPalette, iNumColors, pImageColors[t]);
    theImage.setColor (t, theNearestColor );
  }

  // Here we are going to shift all colors down to the first iNumColors places
  for (y=0;y<theImage.height();y++) {
    pLine = theImage.scanLine ( y );
    for (x=0;x<theImage.width();x++) {
      iIndex = pLine[x];
      theColor = theImage.color ( iIndex );
      pLine[x] = 0;
      bFound = false;
      for ( t=0;t<iNumColors;t++ ) {
	// Okay we found the new color (and t = the new index)
	if ( pPalette[t] == theColor ) {
	  pLine[x] = t;
	  bFound = true;
	  t = iNumColors; // exit the inner most loop.
	}
      }
      if ( ! bFound ) // too many output in case something goes wrong 
	printf ( "Could not find color %X count <%ld>\n", (unsigned)theColor, ++iCounter );
    }
  }

//  printf ( "numColors=%d width=<%d> height=<%d>\n", iImageColors, theImage.width(), theImage.height());
//  for (t=0;t<iImageColors;t++) {
//    printf ( "Color[%d] = %X", t, theImage.color (t));
//    if ( t<iNumColors )
//      printf ( " - Palette[%d] = %X", t, pPalette[t]);
//    printf ("\n");
//  }
//  printf ("\n\n" );
  // Then we should cut down the number of colors used ...
  //theImage.setNumColors ( iNumColors );	//ooo
  theImage.setColorCount ( iNumColors );	//xxx

  // and then set the right colors ...
  for (t=0;t<iNumColors;t++)
    theImage.setColor ( t, pPalette [t] );
}

long Utils::countColors ( QImage &theImage )
{
  bool bFound;
  int  x, y, z, iIndex;

  if ( theImage.depth () == 8 )  {
    uchar *pLine;
    //Q3ValueList<int> listFoundColors;		//ooo
    QList<int> listFoundColors;			//xxx
    // Here we are going to shift all colors down to the first iNumColors places
    for (y=0;y<theImage.height();y++) {
      pLine = theImage.scanLine ( y );
      for (x=0;x<theImage.width();x++) {
        bFound=false;
        iIndex = pLine[x];
        for (z=0;z<(int)listFoundColors.count ();z++)  {
          if ( iIndex == listFoundColors[z] )  {
            z=listFoundColors.count ( );
            bFound=true;
          }
        }
        if ( !bFound )
          listFoundColors.append ( iIndex );
      }
    }
    return listFoundColors.count ();
  }
  else  { // 32 bits
//static bool bFirst=true;

    QRgb *pLine, rgb;
    QList<QRgb> listFoundColors;		//oxx
    for (y=0;y<theImage.height();y++) {
      pLine = (QRgb *)theImage.scanLine ( y );
      for (x=0;x<theImage.width();x++) {
        bFound=false;
        rgb = pLine[x];
        for (z=0;z<(int)listFoundColors.count ();z++)  {
          if ( rgb == listFoundColors[z] )  {
            z=listFoundColors.count ( );
            bFound=true;
          }
        }
        if ( !bFound )
          listFoundColors.append ( rgb );
      }
    }
//if ( bFirst )  {
//  qHeapSort ( listFoundColors );
//  for (z=0;z<(int)listFoundColors.count ();z++)  {
//    printf ( "[%d]=<%X>\n", z, listFoundColors[z] );
//  }
//  bFirst = false;
//}
    return listFoundColors.count ();
  }
  return 0L;
}

QString Utils::getVideoFormat (int iWidth, int iHeight)
{
  QString qsFormat = ( "custom" );
  int iFormat = getFormat ( iWidth, iHeight );
  if ( iFormat != FORMAT_NONE )  {
    if ( iFormat > FORMAT_NTSC4 )
      qsFormat = "pal";
    else
      qsFormat = "ntsc";
  }
  return qsFormat;
}

int Utils::getFormat ( QString &qsResolution )
{
  int iWidth  = getWHFromResolution ( qsResolution, true  );
  int iHeight = getWHFromResolution ( qsResolution, false );

  return getFormat ( iWidth, iHeight );
}

int Utils::getFormat (int iWidth, int iHeight)
{
	int iFormat = FORMAT_NONE;
	if ((iWidth == 720) && (iHeight == 480) )
		iFormat = FORMAT_NTSC1;
	else if  ((iWidth == 704) && (iHeight == 480) )
		iFormat = FORMAT_NTSC2;
	else if  ((iWidth == 352) && (iHeight == 480) )
		iFormat = FORMAT_NTSC3;
	else if  ((iWidth == 352) && (iHeight == 240) )
		iFormat = FORMAT_NTSC4;
	else if  ((iWidth == 720) && (iHeight == 576) )
		iFormat = FORMAT_PAL1;
	else if ((iWidth == 704) && (iHeight == 576) )
		iFormat = FORMAT_PAL2;
	else if  ((iWidth == 352) && (iHeight == 576) )
		iFormat = FORMAT_PAL3;
	else if  ((iWidth == 352) && (iHeight == 288) )
		iFormat = FORMAT_PAL4;
	
	return iFormat;
}

bool Utils::getResolution ( int iFormat, int &iWidth, int &iHeight )
{
  bool bFound = true;
  iWidth  = 720;
  iHeight = 480;
  switch ( iFormat )  {
  case FORMAT_NTSC1:
  break;
  case FORMAT_NTSC2:
    iWidth = 704;
  break;
  case FORMAT_NTSC3:
    iWidth = 352;
  break;
  case FORMAT_NTSC4:
    iWidth  = 352;
    iHeight = 240;
  break;
  case FORMAT_PAL1:
    iHeight = 576;
  break;
  case FORMAT_PAL2:
    iWidth  = 704;
    iHeight = 576;
  break;
  case FORMAT_PAL3:
    iWidth  = 352;
    iHeight = 576;
  break;
  case FORMAT_PAL4:
    iWidth  = 352;
    iHeight = 288;
  break;

  default:
    bFound = false;
  }
  return bFound;
}

QString Utils::iso639 (QString qsCode, bool bReverse, int iWhich )
{
	// This function will return the Country name of the found two-letter - code
	// according to the iso639 standard (ftp://dkuug.dk/i18n/ISO_639)
	char arrayCountry[278][36] = {"aa", "Afar", "ab", "Abkhazian", "af", "Afrikaans", 
	"am", "Amharic", "ar", "Arabic", "as", "Assamese", "ay", "Aymara", "az", "Azerbaijani", 
	"ba", "Bashkir", "be", "Byelorussian", "bg", "Bulgarian", "bh", "Bihari", "bi", "Bislama", 
	"bn", "Bengali; Bangla", "bo", "Tibetan", "br", "Breton", "ca", "Catalan", "co", "Corsican", 
	"cs", "Czech", "cy", "Welsh", "da", "Danish", "de", "German", "dz", "Bhutani", "el", "Greek", 
	"en", "English", "eo", "Esperanto", "es", "Spanish", "et", "Estonian", "eu", "Basque", 
	"fa", "Persian", "fi", "Finnish", "fj", "Fiji", "fo", "Faroese", "fr", "French", "fy", "Frisian", 
	"ga", "Irish", "gd", "Scots Gaelic", "gl", "Galician", "gn", "Guarani", "gu", "Gujarati", 
	"ha", "Hausa", "he", "Hebrew (formerly iw)", "hi", "Hindi", "hr", "Croatian", "hu", "Hungarian", 
	"hy", "Armenian", "ia", "Interlingua", "id", "Indonesian (formerly in)", "ie", "Interlingue", 
	"ik", "Inupiak", "is", "Icelandic", "it", "Italian", "iu", "Inuktitut", "ja", "Japanese", 
	"jw", "Javanese", "ka", "Georgian", "kk", "Kazakh", "kl", "Greenlandic", "km", "Cambodian", 
	"kn", "Kannada", "ko", "Korean", "ks", "Kashmiri", "ku", "Kurdish", "ky", "Kirghiz", 
	"la", "Latin", "ln", "Lingala", "lo", "Laothian", "lt", "Lithuanian", "lv", "Latvian, Lettish", 
	"mg", "Malagasy", "mi", "Maori", "mk", "Macedonian", "ml", "Malayalam", "mn", "Mongolian", 
	"mo", "Moldavian", "mr", "Marathi", "ms", "Malay", "mt", "Maltese", "my", "Burmese", 
	"na", "Nauru", "ne", "Nepali", "nl", "Dutch", "no", "Norwegian", "oc", "Occitan", 
	"om", "(Afan) Oromo", "or", "Oriya", "pa", "Punjabi", "pl", "Polish", "ps", "Pashto, Pushto", 
	"pt", "Portuguese", "qu", "Quechua", "rm", "Rhaeto-Romance", "rn", "Kirundi", "ro", "Romanian", 
	"ru", "Russian", "rw", "Kinyarwanda", "sa", "Sanskrit", "sd", "Sindhi", "sg", "Sangho", 
	"sh", "Serbo-Croatian", "si", "Sinhalese", "sk", "Slovak", "sl", "Slovenian", "sm", "Samoan", 
	"sn", "Shona", "so", "Somali", "sq", "Albanian", "sr", "Serbian", "ss", "Siswati", "st", "Sesotho", 
	"su", "Sundanese", "sv", "Swedish", "sw", "Swahili", "ta", "Tamil", "te", "Telugu", "tg", "Tajik", 
	"th", "Thai", "ti", "Tigrinya", "tk", "Turkmen", "tl", "Tagalog", "tn", "Setswana", "to", "Tonga", 
	"tr", "Turkish", "ts", "Tsonga", "tt", "Tatar", "tw", "Twi", "ug", "Uighur", "uk", "Ukrainian", 
	"ur", "Urdu", "uz", "Uzbek", "vi", "Vietnamese", "vo", "Volapuk", "wo", "Wolof", "xh", "Xhosa", 
	"yi", "Yiddish (formerly ji)", "yo", "Yoruba", "za", "Zhuang", "zh", "Chinese", "zu", "Zulu"};
	uint t, iCount;
	int iReverse;
	QString qsReturn;
	iCount = (int)(sizeof (arrayCountry) / (sizeof (arrayCountry[0]) * 2)); 
	//iCount = (int)(sizeof (arrayCountry) / (sizeof (char) * 36 * 2));
	// Determines forward/backward looking up.
	iReverse = 0;
	if (bReverse)
		iReverse = -1;
	if ( iWhich > -1 ) {
	  if ( iWhich < (int)iCount )
	    qsReturn = QString ((char *)arrayCountry[iWhich*2+1 + iReverse]);
	  return qsReturn;
	}
	// else ...
	for (t=0;t<iCount;t++)	{
		if (qsCode == arrayCountry[t*2 - iReverse])	{
			qsReturn = QString ((char *)arrayCountry[t*2+1 + iReverse]);
			return qsReturn;
		}
	}
	return qsCode;
}

QString Utils::checkForExe( QString qsExe )
{
	QString qsExePath;
	QFileInfo fileInfo(QString ("%1/%2_out.txt").arg(Global::qsTempPath).arg(getlogin()));
	// the next check is to see if we can find 
	//if ( system ((const char *)QString ("which %1 > %2/%3_out.txt 2>/dev/null").arg(qsExe).arg(Global::qsTempPath).arg(getlogin())) == -1)	//ooo
	if ( system (QString ("which %1 > %2/%3_out.txt 2>/dev/null").arg(qsExe).arg(Global::qsTempPath).arg(getlogin()).toLatin1().data()) == -1)	//xxx
		return QString ();
	fileInfo.setFile(QString("%1/%2_out.txt").arg(Global::qsTempPath).arg(getlogin()));
	// Okay we can not find it, so we wont bother asking the user ...
	if (fileInfo.size() > 4)	{
	  QFile theFile   (QString ("%1/%2_out.txt").arg(Global::qsTempPath).arg(getlogin()));
		//theFile.open    (QIODevice::ReadOnly);	//ooo
		//theFile.readLine(qsExePath, 4096);		//ooo
		if ( theFile.open ( QIODevice::ReadOnly ) )  {	//xxx
			QTextStream in(&theFile);
			qsExePath = in.readLine();
			if (!qsExePath.isEmpty())	{
				qsExePath.remove("\n");
			}
		}
		theFile.close();
		//if (!qsExePath.isEmpty())	{	//ooo
		//	qsExePath.remove("\n");
		//	return qsExePath;
		//}
	}
	//return QString();	//ooo
	return qsExePath;	//xxx
}

QString Utils::getToolsDisplayName (QString qsExecutableName)
{
	int t = getIndexFromToolName (qsExecutableName);
	// All external Tools are defined in global.h
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };

	if (t >= 0)
		return QString (toolsArray[t].pDisplayName);
	return qsExecutableName;
}

int Utils::getIndexFromToolName (QString qsExecutableName)
{
	uint t, iNrOfTools;
	// All external Tools are defined in global.h
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	iNrOfTools = sizeof ( toolsArray ) / ( sizeof ( structTools ) );
	for (t=0;t<iNrOfTools;t++)
		if (qsExecutableName == QString (toolsArray[t].pExecutableName))
			return t;
	return -1;
}

QString Utils::getToolByIndex (uint iIndex)
{
	uint iNrOfTools;
	// All external Tools are defined in global.h
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	iNrOfTools = sizeof ( toolsArray ) / ( sizeof ( structTools ) );
	if (iIndex > iNrOfTools-1)
		return QString();

	return QString (toolsArray[iIndex].pExecutableName);
}

QString Utils::getToolPath ( QString qsToolName )
{
  QString qsReturn;
  Utils::toolsPaths *pToolPath;
  uint t;
  for ( t=0; t<(uint)Global::listToolsPaths.count ( ); t++ ) {	//ox
    pToolPath = Global::listToolsPaths[ t ];
    if ( pToolPath && ( pToolPath->qsExecutableName == qsToolName ) )
      return pToolPath->qsFullPath;
  }
  return qsReturn;
}

QList<Utils::toolsPaths *>Utils::scanSystem()			//oxx
{
	static QList<Utils::toolsPaths *>listToolsPaths;	//oxx
	Utils::toolsPaths *pEntry;

	uint t, iNrOfTools;
	// All external Tools are defined in global.h
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	iNrOfTools = sizeof ( toolsArray ) / ( sizeof ( structTools ) );

	// Note at this point if there were entries in thelist they ought to be deleted already.
	listToolsPaths.clear();
	for (t=0;t<iNrOfTools;t++)	{
		pEntry = new Utils::toolsPaths;
		pEntry->qsExecutableName = QString( toolsArray[t].pExecutableName );
		pEntry->qsFullPath  = checkForExe ( toolsArray[t].pExecutableName );
		if ( pEntry->qsFullPath.isEmpty() )	{
			 pEntry->qsFullPath = pEntry->qsExecutableName;
			 pEntry->bPresent = false;
		}
		else
			pEntry->bPresent = true;
		listToolsPaths.append(pEntry);
	}
	// Please remember unlike the norm to create/destroy objects in the same class I pass the entries in this list on to another class.
	return listToolsPaths;
}

bool Utils::isMandatory(uint iIndex)
{
	// All external Tools are defined in global.h
	struct structTools { 
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	//if (QString (toolsArray[iIndex].pDescription).find ("Mandatory") > -1)	//ooo
	if (QString (toolsArray[iIndex].pDescription).indexOf ("Mandatory") > -1)	//xxx
		return true;
	return false;
}

QString Utils::getTempFile (QString qsOrigFileName)
{
	QString qsTempDir = Global::qsTempPath + QString ("/") +  Global::qsProjectName + QString ("/");
	// we take this to check if the temp drive exists and if not, then we will create one ...
	QDir tempDir  ( qsTempDir );
	if ( ! tempDir.exists ( ) )  {
		 if ( ! recMkdir  ( qsTempDir ) )
			 return Global::qsTempPath + QString ("/");
	}
	// Here we append the file name to the temp path ...
	if ( ! qsOrigFileName.isEmpty ( ) )	{
		QFileInfo fileInfo  = QFileInfo ( qsOrigFileName );
		qsTempDir = QString ( qsTempDir + fileInfo.fileName ( ) );
	}
	return qsTempDir;
}

// creates a path under the temp dir ...
QString Utils::getTempPath ( QString qsInputPath )
{
  QString qsPath = getTempFile ( qsInputPath );
  QDir tempDir ( qsPath );
  if ( ! tempDir.exists ( ) )  {
    if ( ! recMkdir ( qsPath ) )
      return Global::qsTempPath + QString ("/");
  }
  return qsPath;
}

QString Utils::getUniqueTempFile (QString qsOrigFileName)
{
	uint t = 0;
	QString qsUniqueFileName, qsFileName;
	QFileInfo fileInfo (qsOrigFileName);
	QString qsOrigBaseName   = fileInfo.baseName ();
	//QString qsOrigExtension  = fileInfo.extension();	//ooo
	QString qsOrigExtension  = fileInfo.suffix();		//xxx

	//qsFileName.sprintf ("%s%03d.%s", (const char *)qsOrigBaseName, t++, (const char *)qsOrigExtension);		//ooo
	qsFileName.sprintf ("%s%03d.%s", qsOrigBaseName.toLatin1().data(), t++, qsOrigExtension.toLatin1().data());	//xxx
	qsUniqueFileName = getTempFile(qsFileName);
	// The same as above but we ensure the file does not yet exist.
	fileInfo.setFile(qsUniqueFileName);
	while (fileInfo.exists())	{
		//qsFileName.sprintf ("%s%03d.%s", (const char *)qsOrigBaseName, t++, (const char *)qsOrigExtension);	//ooo
	  qsFileName.sprintf ("%s%03d.%s", qsOrigBaseName.toLatin1().data(), t++, qsOrigExtension.toLatin1().data());	//xxx
		qsUniqueFileName = getTempFile(qsFileName);
		fileInfo.setFile(qsUniqueFileName);
	}
	return qsUniqueFileName;
}

QStringList Utils::getProjectsFromTempPath()
{
	// This function looks for "Main Menu VMGM/background.jpg"
	// under the temp path
	uint t;
	QStringList listOfProjects;
	QString qsTemp;
	QDir mainMenuDir, tempDir (Global::qsTempPath);
	QFile backgroundFile, menuBackgroundFile;
	
	// But we aree only interested in sub dirs ...
	tempDir.setFilter (QDir::Dirs);
	for (t=0;t<tempDir.count();t++)	{
		qsTemp = tempDir[t];
		if ( ( qsTemp == "." ) || ( qsTemp == ".." ) )
			continue;
		// three criterias
		//backgroundFile.setName ( tempDir.absPath ( ) + QString ( "/%1/Main Menu VMGM/background.jpg" ).arg ( qsTemp ) );				//ooo
		backgroundFile.setFileName ( tempDir.absoluteFilePath (Global::qsTempPath) + QString ( "/%1/Main Menu VMGM/background.jpg" ).arg ( qsTemp ) );	//xxx
		//menuBackgroundFile.setName ( tempDir.absPath ( ) + QString ("/%1/background.jpg" ).arg ( qsTemp ) );						//ooo
		menuBackgroundFile.setFileName ( tempDir.absoluteFilePath (Global::qsTempPath) + QString ("/%1/background.jpg" ).arg ( qsTemp ) );		//xxx
		//mainMenuDir.setPath ( tempDir.absPath ( ) + QString ( "/%1/Main Menu VMGM" ).arg ( qsTemp ) );						//ooo
		mainMenuDir.setPath ( tempDir.absoluteFilePath (Global::qsTempPath) + QString ( "/%1/Main Menu VMGM" ).arg ( qsTemp ) );			//xxx
		if ( backgroundFile.exists ( ) )
			listOfProjects.append ( qsTemp );
		else if (menuBackgroundFile.exists ( ) )
			listOfProjects.append ( qsTemp );
		else if (mainMenuDir.exists ( ) )
			listOfProjects.append ( qsTemp );
	}
	return listOfProjects;
}

QImage  Utils::convertStringToImage (QString &qsImage)
{
	uint t;
	bool bOk;
	QImage theImage;
	//QByteArray baData(qsImage.length());					//ooo
	QByteArray baData(qsImage.toLatin1().data(), qsImage.length());		//xxx

	// Here we convert the hex values to char ...
	for (t=0;t<(uint)qsImage.length();t++)	//ox
		//baData.at ( t ) = qsImage.mid (t*2, 2).toUInt(&bOk, 16);	//ooo
		baData[ t ] = qsImage.mid (t*2, 2).toUInt(&bOk, 16);		//xxx
	
	//QDataStream stream( baData, QIODevice::ReadOnly );	//ooo
	QDataStream stream( &baData, QIODevice::ReadOnly );	//xxx
	stream >> theImage;

	return theImage;
}

QString Utils::convertImageToString (QImage &theImage)
{
	uint t;
	QString qsTemp, qsResult;
	QByteArray baData;
	//QDataStream stream( baData, QIODevice::WriteOnly );	//ooo
	QDataStream stream( &baData, QIODevice::WriteOnly );	//xxx
	stream << (const QImage &)theImage;
	for (t=0;t<(uint)baData.count();t++)	{	//ox
		qsTemp.sprintf ("%02X", (uchar)baData.at ( t ) );
		qsResult += qsTemp;
	}
//printf ("Utils::convertImageToString QByteArray<\n%s\n>\n", (const char *)qsResult);
	return  qsResult;
}

bool Utils::recRmdir( const QString &dirName, const char *pExcludeFiles ) const
{
	// Recursively removes directroy and all sub-directories.
	// Attention this can be devestating !
	QString dirN = dirName;

	if(QDir::isRelativePath(dirN)) // if not absolute path, make it absolute
		dirN = QDir::current().path() + dirN; // FIXME: I'm not really sure that this works
	QDir dir(dirN);
	//QStringList list = dir.entryList(QDir::All); // make list of entries in directory		//ooo
	QStringList list = dir.entryList(QDir::AllEntries); // make list of entries in directory	//xxx
	unsigned int i, lstCnt = list.count();
	QFileInfo fileInfo;
	QString curItem, lstAt;
	bool bRemove;
	for(i = 0; i < lstCnt; i++){ // loop through all items of list
		//lstAt = *list.at(i);		//ooo
		lstAt = list.at(i);		//xxx

		if(!(lstAt == ".") && !(lstAt == "..")){
			curItem = dirN + "/" + lstAt;
			fileInfo.setFile(curItem);
			if(fileInfo.isDir())       // is directory
				recRmdir(curItem); // call recRmdir() recursively for deleting subdirectory
			else	{                  // is file
				bRemove = true;
				if (pExcludeFiles)	{		// If we want to keep files
					//if (lstAt.find (pExcludeFiles) == 0)	// which begin with the string *pExcludeFiles	//ooo
					if (lstAt.indexOf (pExcludeFiles) == 0)	// which begin with the string *pExcludeFiles	//xxx
						bRemove = false;	// Then we flag so ...
				}
				if (bRemove)
					QFile::remove(curItem);		// ok, delete file
			}
		}
	}
	dir.cdUp();
	return dir.rmdir(dirN); // delete empty dir and return if (now empty) dir-removing was successfull
}

bool Utils::recMkdir ( const QString &qsDir )
{
  QDir theDir ( qsDir );
  if ( theDir.exists ( ) )
    return true;

  int t;
  QString qsFullPath, qsPartialDir;
  //qsFullPath = theDir.absPath ( );			//ooo
  qsFullPath = theDir.absoluteFilePath ( qsDir );	//xxx
  //QStringList dirList = QStringList::split ( "/", qsFullPath );	//ooo
  QStringList dirList = qsFullPath.split ( "/" );			//xxx

  for ( t=0; t<(int)dirList.count ( ); t++ )  {
    qsPartialDir += "/" + dirList[t];
    theDir.setPath ( qsPartialDir );
    if ( theDir.exists ( ) )
      continue;
    if ( ! theDir.mkdir ( qsPartialDir ) )
      return false;
  }

  // Sanity check if the dir exists ...
  theDir.setPath ( qsFullPath );
  if ( ! theDir.exists ( ) )  {
    //MessageBox::warning ( NULL, QObject::tr ("Could not create temp directory"),	//ooo
    QMessageBox::warning ( NULL, QObject::tr ("Could not create temp directory"),	//xxx
    QObject::tr ("Failed to create temp directory\n%1\nPlease make sure you have the read/write access.").arg  ( qsFullPath ),
    QMessageBox::Ok, QMessageBox::NoButton);
    return false;
  }
  return true;
}

long Utils::getMsFromString ( QString qsInput )
{ 
  long iMilliSeconds;
  uint iHours, iMinutes, iSeconds, iStepper;
  float fSeconds=0.0;
  iStepper=0;
  // 00:00:00.000
  iHours = iMinutes = iSeconds = iMilliSeconds = 0;
  //QStringList listParts = QStringList::split (":", qsInput);	//ooo
  QStringList listParts = qsInput.split (":");			//xxx
  if (listParts.count() > 3)
    return 0;
  if (listParts.count() > 2)
    iHours = listParts[iStepper++].toInt();
  if (listParts.count() > 1)
    iMinutes = listParts[iStepper++].toInt();
  if (listParts.count() > 0)
    fSeconds = listParts[iStepper++].toFloat();
  iMilliSeconds = (uint)(1000*fSeconds) + 60*1000*iMinutes + 60*60*1000*iHours;
// 	uint iHours, iMinutes, iSeconds, iMilliSeconds;
// 	QTime length;
// 	
// 	if (qsInput.length() < 8)
// 	{
// 		qsInput.prepend("0");
// 	}
// 	qWarning(qsInput);
// 	
// 	length= QTime::fromString(qsInput);
// 	iHours = 
	
  return iMilliSeconds;
}

QString Utils::getStringFromMs ( long iInput )
{
	uint iHours, iMinutes, iSeconds;
        double fMilliSeconds=0.0;
        iHours = iMinutes = iSeconds = 0;
        QString qsReturn ("00:00:00.000");
        fMilliSeconds = (double)iInput;
        if (fMilliSeconds >= 3600000.0) // 60*60*1000
                iHours = (int)(fMilliSeconds/3600000.0);
        fMilliSeconds -= iHours * 3600000;
        if (fMilliSeconds >= 60000.0)   // 60*1000
                iMinutes = (int)(fMilliSeconds/60000.0);
        if (iMinutes > 60)
                iMinutes = 0;
        fMilliSeconds -= iMinutes * 60000;
        if (fMilliSeconds >= 1000.0)
                iSeconds = (int)(fMilliSeconds/1000.0);
        if (iSeconds > 60)
                iSeconds = 0;
        fMilliSeconds -= iSeconds * 1000;
        if (fMilliSeconds > 1000)
                fMilliSeconds = 0;

        qsReturn.sprintf ("%02d:%02d:%02d.%03d", iHours, iMinutes, iSeconds, (int)fMilliSeconds);
        return qsReturn;

	/**
	 * F.J.Cruz - 17/12/04
	 */
	 
// 	QTime tmpTime;
// 	QTime length;
// 	length = tmpTime.addMSecs(iInput);
// 	if (length.isNull())
// 	{
// 		qWarning("'length' is null ??");
// 	}
// 	qsReturn = length.toString("hh:mm:ss.zzz");
// 	
// 	qWarning(qsReturn);

// 	qsReturn.sprintf ("%02d:%02d:%02d.%03d", iHours, iMinutes, iSeconds, (int)fMilliSeconds);
// 	return qsReturn;
}

int Utils::getWHFromResolution ( QString qsResolution, bool bWidth )
{
  int iXPos;
  QString qsWidth, qsHeight;

  //iXPos = qsResolution.find ("x");	//ooo
  iXPos = qsResolution.indexOf ("x");	//xxx
  if ( bWidth ) {
    qsWidth  = qsResolution.left (iXPos);
    return qsWidth.toInt ();
  }
  qsHeight = qsResolution.right(iXPos);
  return qsHeight.toInt ();
}

bool Utils::isMpeg2 ( QString qsFileName )
{
  unsigned char Mpeg2Header[4] = { 0x00, 0x00, 0x01, 0xba };
  int t, iByte;
  unsigned char c;
  //char *ch;				//xxx
  char ch;				//xxx
  QFile theFile ( qsFileName );

  if ( ! theFile.exists () )
    return false;

  if ( ! theFile.open ( QIODevice::ReadOnly ) )
    return false;

  for (t=0;t<4;t++) {
    //iByte = theFile.getch ();			//ooo
    theFile.getChar (&ch);			//xxx
    //iByte = theFile.pos();			//xxx
    //QByteArray line = theFile.readLine();	//xxx

    //iByte = line.at(t);	//xxx
    iByte = (int) ch;		//xxx

    if ( iByte == -1 ) {
      theFile.close ();
      return false;
    }
    c = (unsigned char) iByte;

    if ( Mpeg2Header[t] != c )
      return false;
  }
  theFile.close ( );

  // Okay getting here means the first 4 bytes are mpeg2 header compliant ... Greeeeeat !!!
  return true;
}

bool Utils::hasVobus ( QString qsFileName )
{
  // VOB is a special subset of the Mpeg2 standard. I stole this test version from DVDStyler's MPEG.cpp - file.
  // A great thank you goes to Alex Thuering for his great effort to advance DVDStyler. 
  // But Ya'll know 'Q'DVD-Author is much better ;o) 

  /*
    Navigation packets are PES packets with a stream id 0xbf, i.e.
    private stream 2.  It's made up of PCI, Presentation Control Information
    and DSI, Data Search Information.

    details: www.mpucoder.com
   */

  unsigned char PCI_PACK[] = { 0x00, 0x00, 0x01, 0xBF, 0x03, 0xD4 };
  unsigned char DSI_PACK[] = { 0x00, 0x00, 0x01, 0xBF, 0x03, 0xFA };

  QFile theFile ( qsFileName );
  if ( ! theFile.open ( QIODevice::ReadOnly ) )
    return false;

  // skip zeroes at start of file
  int zeroCnt = 0;
  char c;

  //while ( (c = (char)theFile.getch ( ) ) == 0)	//ooo
  while ( ( theFile.getChar ( &c ) ) && ( c == 0x00 ) )	//xxx
      zeroCnt++;

  //if ( zeroCnt < 2 || c != 1 )	//ooo
  if ( zeroCnt < 2 || c != 0x01 )	//xxx
    return false;

  const long MAX_LENGTH = 16*1024; // only looking at 1st 16KB - enough to contain nav packs
  unsigned char *buffer = new unsigned char[MAX_LENGTH];

  buffer[0] = buffer[1] = 0; buffer[2] = 1;
  //int iRead = theFile.readBlock ( (char *)(buffer+3), MAX_LENGTH-3);	//ooo
  int iRead = theFile.read ( (char *)(buffer+3), MAX_LENGTH-3);		//xxx

  bool bValidPCI = false;
  bool bValidDSI = false;

  for ( int i=0; i<iRead-(int)sizeof(PCI_PACK); i++ )  {
    int j;
    for ( j=0; j<(int)sizeof(PCI_PACK); j++ )  {
      if ( buffer[i+j] != PCI_PACK[j] )
        break;
      else if ( j == sizeof(PCI_PACK) - 1 )
        bValidPCI  = true;
    }
    for ( j=0; j<(int)sizeof(DSI_PACK); j++ )  {
      if ( buffer[i+j] != DSI_PACK[j] )
        break;
      else if (j == sizeof(DSI_PACK)-1)
        bValidDSI = true;
    }
    if ( bValidPCI && bValidDSI )  {
      delete []buffer;
      return true;
    }
  }
  delete []buffer;
  return false;
}

bool Utils::checkCommand ( const char *pCommand )
{
  // executes a command and checks if there was a result
  char tmpName[20] =  { "/tmp/out_XXXXXX" };
  int iFD = mkstemp ( (char *)&tmpName );
  iFD = iFD; // Silence the compiler warnings.
  QString qsTmpFile = QString ( "%1"  ).arg ( tmpName  );
  QString qsCommand = QString ( "%1 > %2 " ).arg ( pCommand ).arg ( qsTmpFile );

  //int iRet = system ( qsCommand.ascii ( ) ); iRet = iRet;		//ooo
  int iRet = system ( qsCommand.toLatin1().data ( ) ); iRet = iRet;	//xxx
  QFile theFile ( qsTmpFile );
  if ( theFile.exists ( ) ) {
    if ( theFile.size ( ) > 2 )
      return true;
    theFile.remove ( );
  }
  return false;
}

QString Utils::getDefaultParameter ( enDefaultParameter enWhich )
{
  QString qsReturn;
  switch ( enWhich ) {
    //For mjpegtools versions smaller than 1.8.0, use '-S 420_mpeg2' instead of '-S 420mpeg2' in your 'ppmtoy4m' line
  case ppmtoy4mChroma: {
    qsReturn = QString ( "420mpeg2" );
    if ( checkCommand ( "ppmtoy4m -h 2>&1 | grep 420_mpeg2" ) )
         qsReturn = QString ( "420_mpeg2" ); // must be mjpegtools version smaller then 1.8.0 !!!
  }
  break;
  case twolameSampleRate:
  default: {
    qsReturn = QString ( "48000" ); // E.g. toolame 0.2m
    // execute [toolame -h 2>&1 | grep sfrq | grep "rate in Hz" > /tmp/test ] and check if result is empty.
    if ( checkCommand ( "twolame -h 2>&1 | grep sfrq | grep \"rate in kHz\"" ) )
       qsReturn = QString ( "48" ); // E.g.  toolame 0.2i
  }
  break;
  };
  return qsReturn;
}

