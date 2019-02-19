/***************************************************************************
    importssa.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This file holds the implementation of the import filter for the
    saa and ass subtitles file.

    saa = SubStationAlpha Version 4
    ass = AdvancedSubStation Verion 4+

    http://en.wikipedia.org/wiki/SubStation_Alpha
    
****************************************************************************/

#include <qfile.h>
#include <qregexp.h>
#include <qobject.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
//Added by qt3to4:
//#include <Q3TextStream>   //ooo
#include <QTextStream>      //xxx
//#include <Q3ValueList>	//oxx

#include "global.h"
#include "importssa.h"

namespace Import
{

Ssa::Script::Script ( )
{
  m_iPlayResX      = 480;
  m_iPlayResY      = 720;
  m_fTimerMultiply = 100.0f;
}

Ssa::Style::Style ( )
{
  qsStyleName        = QString ( "Default" );
  qsFontName         = QString ( "Courier" );
  iFontSize          = 24;
  clrFillingColor    = Rgba ( 0x00FF0000 );
  clrSecondaryColour = Rgba ( 0x0000FF00 );
  clrOutline         = Rgba ( 0x00FF8010 );
  clrShadow          = Rgba ( 0xFF000000 );
  iAlignement        = 2;
  bBold              = false;
  bItalic            = false;
  bUnderline         = false;
  bStrikeOut         = false;
  iSpacing           = 0;
  iAngle             = 0;
  iBorderStyle       = 1;
  iOutline           = 3;
  iShadow            = 3;
  iMarginL           = 0;
  iMarginR           = 0;
  iMarginV           = 0;
  iEncoding          = 128;
  fScaleX            = 100.0f;
  fScaleY            = 100.0f;
  iAlphaLevel        = 0;
}

Ssa::Event::Event ( )
{
  iMarked      = 1;
  iLayer       = 0;
  qsStartTime  = QString ( "0:00:00.00" );
  qsEndTime    = QString ( "0:00:00.00" );
  pStyle       = NULL;
  qsName       = QString ( "Default" );
  iMarginL     = 0000;
  iMarginR     = 0000;
  iMarginV     = 0000;
  qsEffect     = QString ( "" );
  qsText       = QString ( "" );
}

Ssa::Ssa ( )
{
}

Ssa::~Ssa ( )
{
  clearEntries ( );

  QList<Style *>::iterator it = m_listOfStyles.begin ( );	//oxx
  while ( it != m_listOfStyles.end ( ) )
    delete *it++;
  m_listOfStyles.clear ( );

  QList<Event *>::iterator it2 = m_listOfEvents.begin ( );	//oxx
  while ( it2 != m_listOfEvents.end ( ) )
    delete *it2++;
  m_listOfEvents.clear ( );
}

bool Ssa::import ()
{
  //QString qsImportFile = Q3FileDialog::getOpenFileName ( Global::qsCurrentPath, QObject::tr("Subtitle files (*.ssa *.ass *.SSA *.ASS)" ), NULL, QObject::tr ( "Open Subtitles File" ) );  //ooo
  QStringList qsImportFile = QFileDialog::getOpenFileNames ( NULL, QObject::tr ( "Open Subtitles File" ), Global::qsCurrentPath, QObject::tr("Subtitle files (*.ssa *.ass *.SSA *.ASS)" ) );    //xxx

  // Sanity check .. Did te user choose a new project file ?
  //if ( qsImportFile.isNull ( ) )  //ooo
  if ( qsImportFile.isEmpty ( ) )   //xxx
    return false;

  //return readFile ( qsImportFile );   //ooo
  return readFile ( qsImportFile[0] );  //xxx
}

bool Ssa::readFile ( QString &fileName )
{
  if ( fileName.isNull ( ) )
    return false;

  // Assign the file
  QString line, temp;
  bool bExitLoop = false;
  QRegExp rx ( "\\[script info\\]" );

  QFile  projectFile ( fileName );
  if ( ! projectFile.open ( QIODevice::ReadOnly ) )
    return false;

  //Q3TextStream ssaStream ( &projectFile );    //ooo
  QTextStream ssaStream ( &projectFile );       //xxx
  while ( (  !  ssaStream.atEnd ( ) ) && ( ! bExitLoop ) ) {
    line = ssaStream.readLine   ( );
    //temp = line.stripWhiteSpace ( );  //ooo
    temp = line.simplified ( );         //xxx
    // Filter out commenting lines
    if  ( temp[0] == ';' )
      continue;
    //temp = line.lower ( );    //ooo
    temp = line.toLower ( );    //xxx
    // Find the header
    //if ( temp.find ( rx ) == -1 )     //ooo
    if ( temp.indexOf ( rx ) == -1 )    //xxx
      continue;

    // If we get here, we have found the [Script Info] - header
    bExitLoop = readScriptInfo ( ssaStream );
  }

  m_qsFileName = fileName;
  projectFile.close ( );
  return true;
}

//bool Ssa::readScriptInfo ( Q3TextStream &ssaStream )  //ooo
bool Ssa::readScriptInfo ( QTextStream &ssaStream )     //xxx
{
  QStringList list;
  QString line, temp;
  QRegExp rxSSA ( "\\[v4 styles\\]" );
  QRegExp rxASS ( "\\[v4\\+ styles\\]" );

  while ( ! ssaStream.atEnd ( ) )  {
    line = ssaStream.readLine ( );
    //temp = line.stripWhiteSpace ( );  //ooo
    temp = line.simplified ( );         //xxx
    // Filter out commenting lines
    if  ( temp[0] == ';' )
      continue;

    //list = QStringList::split ( ":", line );  //ooo
    list = line.split ( ":" );                  //xxx
    if ( list.count ( ) == 2 )  {
      // Here we extract the variables for the Script class.
      //if ( list[0].find ( "PlayResX" ) > -1 )     //ooo
      if ( list[0].indexOf ( "PlayResX" ) > -1 )    //xxx
        m_script.m_iPlayResX = list[1].toInt ( );
      //else if ( list[0].find ( "PlayResY" ) > -1 )    //ooo
      else if ( list[0].indexOf ( "PlayResY" ) > -1 )   //xxx
        m_script.m_iPlayResY = list[1].toInt ( );
      //else if ( list[0].find ( "Timer" ) > -1 )  {    //ooo
      else if ( list[0].indexOf ( "Timer" ) > -1 )  {   //xxx
        // Convert 100,0000 to 100.0f
        list[1].replace ( ",", "." );
        m_script.m_fTimerMultiply = list[1].toFloat ( );
      }
    }
    else  {
      //temp = line.lower ( );  //ooo
      temp = line.toLower ( );  //xxx
      // Find the header
      //if ( temp.find ( rxSSA ) != -1 )    //ooo
      if ( temp.indexOf ( rxSSA ) != -1 )   //xxx
        return readSSAStyles ( ssaStream );
      //else if ( temp.find( rxASS ) != -1 )    //ooo
      else if ( temp.indexOf( rxASS ) != -1 )   //xxx
        return readASSStyles ( ssaStream );
    }
  }

  return false;
}

// [V4 Styles]
//bool Ssa::readSSAStyles ( Q3TextStream &ssaStream )   //ooo
bool Ssa::readSSAStyles ( QTextStream &ssaStream )      //xxx
{
// [V4 Styles]
// Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, TertiaryColour, BackColour, Bold, Italic, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, AlphaLevel, Encoding
// Style: DefaultVCD, Arial,28,11861244,11861244,11861244,-2147483640,-1,0,1,1,2,2,30,30,30,0,0

  // V4+ is a superset of V4, so we might as well just call the V4+ routine.
  return readASSStyles ( ssaStream );
}

// [V4+ Styles]
//bool Ssa::readASSStyles ( Q3TextStream &ssaStream )   //ooo
bool Ssa::readASSStyles ( QTextStream &ssaStream )      //xxx
{
// [V4+ Styles]
// Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding
// Style: Default,Bitstream Vera Sans,26,&H00FFFFFF,&H0000FFFF,&H4B000000,&H4B404040,0,0,0,0,100,100,0,0,1,0,2,2,20,20,40,0

  QString line, temp;
  QStringList list, format, style;
  QRegExp rxEvents ( "\\[events\\]" );
  Style *pStyle = NULL;
  int t;

  while ( ! ssaStream.atEnd ( ) )  {
    line = ssaStream.readLine ( );
    //temp = line.stripWhiteSpace ( );  //ooo
    temp = line.simplified ( );         //xxx
    // Filter out commenting lines
    if  ( temp[0] == ';' )
      continue;

    //temp = line.lower ( );    //ooo
    temp = line.toLower ( );    //xxx
    // Find the header
    //if ( temp.find ( rxEvents ) !=  -1 )  //ooo
    if ( temp.indexOf ( rxEvents ) !=  -1 ) //xxx
      return readEvents ( ssaStream );

    //list = QStringList::split ( ":", line );  //ooo
    list = line.split ( ":" );                  //xxx
    if ( list.count ( ) < 2 )
      continue;

    if ( list[0] == "Format" )  {
      //format = QStringList::split ( ",", list[1].remove ( " " ) );    //ooo
      format = list[1].remove ( " " ).split ( "," );                    //xxx
      continue;
    }

    //style = QStringList::split ( ",", list[1], true );    //ooo
    style = list[1].split ( "," );                          //xxx
    if ( style.count ( ) != format.count ( ) )
      continue;

    pStyle = new Style;
    for ( t=0; t<(int)format.count ( ); t++ )  {
      if ( format[t] == "Name" )
        pStyle->qsStyleName= style[t];
      else if ( format[t] ==        "Fontname" )
        pStyle->qsFontName = style[t];
      else if ( format[t] ==        "Fontsize" )
        pStyle->iFontSize  = style[t].toInt  ( );
      else if ( format[t] ==   "PrimaryColour" )
        pStyle->clrFillingColor    = toRgba ( style[t] );
      else if ( format[t] == "SecondaryColour" )
        pStyle->clrSecondaryColour = toRgba ( style[t] );
      else if ( format[t] ==   "OutlineColour" )
        pStyle->clrOutline = toRgba ( style[t] );
      else if ( format[t] ==      "BackColour" )
        pStyle->clrShadow  = toRgba ( style[t] );
      else if ( format[t] ==      "Bold" )
        pStyle->bBold      = ( style[t].toInt ( ) == -1 );
      else if ( format[t] ==    "Italic" )
        pStyle->bItalic    = ( style[t].toInt ( ) == -1 );
      else if ( format[t] == "Underline" )
        pStyle->bUnderline = ( style[t].toInt ( ) == -1 );
      else if ( format[t] == "StrikeOut" )
        pStyle->bStrikeOut = ( style[t].toInt ( ) == -1 );
      else if ( format[t] ==    "ScaleX" )
        pStyle->fScaleX    = style[t].toFloat ( );
      else if ( format[t] ==    "ScaleY" )
        pStyle->fScaleY    = style[t].toFloat ( );
      else if ( format[t] ==   "Spacing" )
        pStyle->iSpacing   = style[t].toInt ( );
      else if ( format[t] ==     "Angle" )
        pStyle->iAngle     = style[t].toInt ( );
      else if ( format[t] ==     "BorderStyle" )
        pStyle->iBorderStyle = style[t].toInt ( );
      else if ( format[t] ==     "Outline" )
        pStyle->iOutline   = style[t].toInt ( );
      else if ( format[t] ==     "Shadow" )
        pStyle->iShadow = style[t].toInt ( );
      else if ( format[t] ==     "Alignment" )
        pStyle->iAlignement= style[t].toInt ( );
      else if ( format[t] ==     "MarginL" )
        pStyle->iMarginL   = style[t].toInt ( );
      else if ( format[t] ==     "MarginR" )
        pStyle->iMarginR   = style[t].toInt ( );
      else if ( format[t] ==     "MarginV" )
        pStyle->iMarginV   = style[t].toInt ( );
      else if ( format[t] ==     "Encoding" )
        pStyle->iEncoding  = style[t].toInt ( );
    } // end for loop
    m_listOfStyles.append ( pStyle );

//printf ( "%s::%d > Style<%s> Font<%s>\n", __FILE__, __LINE__, pStyle->qsStyleName.ascii(), pStyle->qsFontName.ascii() );
  }

  return false;
}

//bool Ssa::readEvents ( Q3TextStream &ssaStream )  //ooo
bool Ssa::readEvents ( QTextStream &ssaStream )     //xxx
{
// [Events]
// Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text
// Dialogue: 0,0:00:30.52,0:00:32.96,*Default,,0000,0000,0000,,I first went to\NRussia in 1938

  QList<Style *>::iterator it;		//oxx
  QStringList list, format, event;
  Event *pEvent = NULL;
  Style *pStyle = NULL;
  QString line, temp, qsStyle;
  QRegExp rx;
  int t;

  while ( ! ssaStream.atEnd ( ) )  {
    line = ssaStream.readLine ( );
    //temp = line.stripWhiteSpace ( );  //ooo
    temp = line.simplified ( );         //xxx
    // Filter out commenting lines
    if  ( temp[0] == ';' )
      continue;

    //list = QStringList::split ( ":", line );  //ooo
    list = line.split ( ":" );                  //xxx
    if ( list.count ( ) < 2 )
      continue;

    if ( list[0] == "Format" )  {
      //format = QStringList::split ( ",", list[1].remove ( " " ) );    //ooo
      format = list[1].remove ( " " ).split ( "," );                    //xxx
      continue;
    }
    else if ( list[0] == "Dialogue" )  {
      line = line.right  ( line.length ( ) - 9 );
      //event = QStringList::split   ( ",", line, true );   //ooo
      event = line.split   ( "," );                         //xxx
      if ( event.count ( ) != format.count ( ) )
        continue;

      pEvent = new Event;
      for ( t=0; t<(int)format.count ( ); t++ )  {
        if ( format[t] == "Layer" )
          pEvent->iLayer = event[t].toInt ( );
        else if ( format[t] == "Marked" )
          pEvent->iMarked  = event[t].toInt ( );
        else if ( format[t] == "Start" )
          pEvent->qsStartTime  = event[t];
        else if ( format[t] == "End" )
          pEvent->qsEndTime  = event[t];
        else if ( format[t] == "Style" )
          qsStyle = event[t];
        else if ( format[t] == "Name" )
          pEvent->qsName = event[t];
        else if ( format[t] == "MarginL" )
          pEvent->iMarginL = event[t].toInt ( );
        else if ( format[t] == "MarginR" )
          pEvent->iMarginR = event[t].toInt ( );
        else if ( format[t] == "MarginV" )
          pEvent->iMarginV = event[t].toInt ( );
        else if ( format[t] == "Effect" )
          pEvent->qsEffect = event[t];
        else if ( format[t] == "Text" )
          pEvent->qsText = event[t];
      }
      // finally we try to find the style
      pEvent->pStyle = m_listOfStyles[0];
      it = m_listOfStyles.begin ( );
      while ( it != m_listOfStyles.end ( ) )  {
        pStyle = *it++;
        if ( qsStyle == pStyle->qsStyleName )  {
             pEvent->pStyle = pStyle;
             break;
        }
      }

      m_listOfEvents.append ( pEvent );

//printf ( "%s::%d > start<%s> end<%s> text<%s>\n", __FILE__, __LINE__, pEvent->qsStartTime.ascii(), pEvent->qsEndTime.ascii(), pEvent->qsText.ascii() );
    }
  }

  return true;
}

Rgba Ssa::toRgba ( QString &qsClr )
{
  QString qsColor = qsClr;
  Rgba ret;
  // Colors are stored as &H<AABBGGRR>.
  QString aa, rr, gg, bb;
  qsColor.remove  ( "&" );
  qsColor.remove  ( "H" );
  if ( qsColor.length ( ) > 6 )
    aa = qsColor.left ( 2 );
  rr =  qsColor.right ( 2 );
  gg = qsColor.mid ( qsColor.length ( ) - 4, 2 );
  bb = qsColor.mid ( qsColor.length ( ) - 6, 2 );

  qsColor = aa + rr + gg + bb;
  ret.fromString ( qsColor );

  return ret;
}

void Ssa::clearEntries ( )
{
  QList<Subtitles::entry *>::iterator it = m_listOfEntries.begin ( );	//oxx
  while ( it != m_listOfEntries.end ( ) )
    delete *it++;
  m_listOfEntries.clear ( );
}

void Ssa::toSubtitles ( )
{
  // This function will convert the SSA/ASS subtitles to a list of entries which I can use in QDVDAuthor
  clearEntries ( );

  QString qsText;
  Utils   theUtils;
  QRegExp rxFontSize  ( "{\\\fs\\d+}" );
  QRegExp rxFontSize2 (  "\\\fs\\d+"  );
  QRegExp rxFontName  ( "{\\\fn\\w+}" );
  QRegExp rxFontName2 (  "\\\fn\\w+"  );
  rxFontSize.setMinimal  ( true );
  rxFontSize2.setMinimal ( true );
  rxFontName.setMinimal  ( true );
  rxFontName2.setMinimal ( true );

  Event  *pEvent = NULL;
  int     iPos, iIndex = 0;
  long    iStartTime, iEndTime;
  Subtitles::entry *pEntry = NULL;
  QList<Event *>::iterator it = m_listOfEvents.begin ( );	//oxx
  while ( it != m_listOfEvents.end ( ) )  {
    pEvent = *it++;

    pEntry = new Subtitles::entry;
//  pEntry->rect = ???
    pEntry->iIndex      = iIndex++;
    iStartTime = theUtils.getMsFromString ( pEvent->qsStartTime );
    iEndTime   = theUtils.getMsFromString ( pEvent->qsEndTime   );
    // Now adjust if required.
    if ( m_script.m_fTimerMultiply != 100.0f )  {
      iStartTime = (int)( iStartTime * m_script.m_fTimerMultiply / 100.0f );
      iEndTime   = (int)( iEndTime   * m_script.m_fTimerMultiply / 100.0f );
    }

    pEntry->iTimeStart  = iStartTime;
    pEntry->iTimeStop   = iEndTime;
    pEntry->qsTimeStart = theUtils.getStringFromMs ( iStartTime ); //pEvent->qsStartTime;
    pEntry->qsTimeStop  = theUtils.getStringFromMs ( iEndTime   ); //pEvent->qsEndTime;

    qsText = pEvent->qsText;
    qsText.remove  ( "{" );
    qsText.remove  ( "}" );
    qsText.replace ( "\\N",  "<br>" );
    qsText.replace ( "\\n",  "<br>" );
    qsText.replace ( "\\i1",  "<i>" );
    qsText.replace ( "\\i0", "</i>" );
    qsText.replace ( "\\u1",  "<u>" );
    qsText.replace ( "\\u0", "</u>" );
    qsText.replace ( "\\s1",  "<s>" );
    qsText.replace ( "\\s0", "</s>" );

    iPos = 0;
    //while ( ( iPos = rxFontSize.search ( qsText, iPos ) ) != -1 ) {   //ooo
    while ( ( iPos = rxFontSize.indexIn ( qsText, iPos ) ) != -1 ) {    //xxx
//    if ( rxFontSize.search ( ) > -1 )  {
      QString qsFontSize = rxFontSize.cap ( 1 );
      qsFontSize = QString ( "<font size=\"%1\">" ).arg ( qsFontSize );
      qsText.replace ( rxFontSize, qsFontSize );
    }
    iPos = 0;
    //while ( ( iPos = rxFontSize2.search ( qsText, iPos ) ) != -1 ) {  //ooo
    while ( ( iPos = rxFontSize2.indexIn ( qsText, iPos ) ) != -1 ) {   //xxx
//    if ( rxFontSize2.search ( ) > -1 )  { // font size without curly brackets
      QString qsFontSize = rxFontSize2.cap ( 1 );
      qsFontSize = QString ( "<font size=\"%1\">" ).arg ( qsFontSize );
      qsText.replace ( rxFontSize2, qsFontSize );
    }
    iPos = 0;
    //while ( ( iPos = rxFontName.search ( qsText, iPos ) ) != -1 ) {   //ooo
    while ( ( iPos = rxFontName.indexIn ( qsText, iPos ) ) != -1 ) {    //xxx
//    if ( rxFontName.search ( ) > -1 )  {
      QString qsFontName = rxFontName.cap ( 1 );
      qsFontName = QString ( "<font face=\"%1\">" ).arg ( qsFontName );
      qsText.replace ( rxFontName, qsFontName );
    }
    iPos = 0;
    //while ( ( iPos = rxFontName2.search ( qsText, iPos ) ) != -1 ) {  //ooo
    while ( ( iPos = rxFontName2.indexIn ( qsText, iPos ) ) != -1 ) {   //xxx
//    if ( rxFontName2.search ( ) > -1 )  { // Font name without curly brackets
      QString qsFontName = rxFontName.cap ( 1 );
      qsFontName = QString ( "<font face=\"%1\">" ).arg ( qsFontName );
      qsText.replace ( rxFontName, qsFontName );
    }

    qsText.remove  ( "{" );
    qsText.remove  ( "}" );
    if ( qsText.length ( ) > 0 )
      pEntry->qsText    = qsText;
    else
      pEntry->qsText    = pEvent->qsText;

//printf ( "%s::%d > start<%s> stop<%s> text<%s>\n", __FILE__, __LINE__, pEntry->qsTimeStart.ascii(), pEntry->qsTimeStop.ascii(), pEntry->qsText.ascii() );
    m_listOfEntries.append ( pEntry );
  }

//printf ( "%s::%d > entries<%d> events<%d> styles<%d>\n", __FILE__, __LINE__, (int)m_listOfEntries.count ( ), (int)m_listOfEvents.count ( ), (int)m_listOfStyles.count ( ) );
}

QList<Subtitles::entry *> Ssa::takeSubtitles ( )	//oxx
{
  // First we create the Subtitles
  toSubtitles ( );

  // and then we pass them on to the caller.
  QList<Subtitles::entry *>list = m_listOfEntries;	//oxx
  m_listOfEntries.clear ( );
  return list;
}

QList<Subtitles::entry *> &Ssa::getSubtitles ( )	//oxx
{
  // First we create the Subtitles
  toSubtitles ( );
  return m_listOfEntries;
}

}; // End of namespace Import
