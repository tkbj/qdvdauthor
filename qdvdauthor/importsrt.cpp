/***************************************************************************
    importsrt.cpp
                             -------------------
    ButtonObject class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This file holds the implementation of the import filter for the srt 
    subtitles file.
    
****************************************************************************/

#include <qfile.h>
#include <qobject.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
#include <QTime>		//xxx
//Added by qt3to4:
//#include <Q3TextStream>   //ooo
#include <QTextStream>      //xxx
//#include <Q3ValueList>	//oxx

#include "global.h"
#include "importsrt.h"

namespace Import
{

BaseSubtitle::BaseSubtitle ( )
{
}

BaseSubtitle::~BaseSubtitle ( )
{
  //for (uint t=0;t<m_listOfEntries.count(); t++)   //ooo
  for (uint t=0;t<static_cast<uint> ( m_listOfEntries.count() ); t++)   //ooo
    delete m_listOfEntries[t];
  m_listOfEntries.clear ();
}

QString BaseSubtitle::getFileName ( )
{
  return m_qsFileName;
}

Srt::Srt ()
{
}

Srt::~Srt ()
{
}

bool Srt::import ( )
{
  //QString qsImportFile = Q3FileDialog::getOpenFileName ( Global::qsCurrentPath, QObject::tr("Subtitle files (*.srt *.SRT)"), NULL, QObject::tr("Open Subtitles File") );  //ooo
  QStringList qsImportFile = QFileDialog::getOpenFileNames ( NULL, QObject::tr("Open Subtitles File"), Global::qsCurrentPath, QObject::tr("Subtitle files (*.srt *.SRT)") );    //xxx

  // Sanity check .. Did te user choose a new project file ?
  //if (qsImportFile.isNull ()) //ooo
  if (qsImportFile.isEmpty ())  //xxx
    return false;

  //return readFile ( qsImportFile );   //ooo
  return readFile ( qsImportFile[0] );  //xxx
}

bool Srt::readFile ( QString &fileName )
{
  if ( fileName.isNull ( ) )
    return false;

  // Assign the file
  Subtitles::entry *pEntry;
  QString line, lineIndex, lineTimestamp, lineText;
  QFile projectFile ( fileName );
  if (!projectFile.open ( QIODevice::ReadOnly ) )
    return false;

  //Q3TextStream srtStream( &projectFile ); //ooo
  QTextStream srtStream( &projectFile );    //xxx

  lineIndex = srtStream.readLine();
  while ( !srtStream.atEnd() ) {
    // first we are looking for the beginning of an entry which ist the index number
    if ( ( lineIndex.toInt ( ) > 0 ) && ( ! isTimeStamp ( lineIndex ) ) ) {
      pEntry = new Subtitles::entry;
      pEntry->iIndex = lineIndex.toInt ( );
      lineIndex = readEntry ( &srtStream,  pEntry );
      m_listOfEntries.append ( pEntry );
    }
    else
      lineIndex = srtStream.readLine ( );
  }

  m_qsFileName = fileName;
  projectFile.close();
  return true;
}

//QString Srt::readEntry ( Q3TextStream *pStream, Subtitles::entry *pEntry )    //ooo
QString Srt::readEntry ( QTextStream *pStream, Subtitles::entry *pEntry )       //xxx
{
  int t;
  bool bTrailing = true;
  QString line, lineIndex, lineTimestamp, lineText, lineTemp;
  QStringList listStrings;
  // Found the first line ... get the timestamp
  lineTimestamp = pStream->readLine ();
  while ( ! isTimeStamp ( lineTimestamp ) )
    lineTimestamp = pStream->readLine ();
  lineText = QString ();
  //  lineText = pStream->readLine () + "\n";
  line = pStream->readLine ();

  while ( ( line.toInt  ( ) == 0 ) && ( ! isTimeStamp ( line ) ) && 
          ( ! pStream->atEnd ( ) ) )  {
    // while ( line.find ( '-' ) <= 2 && line.find ('-') > -1 )  {
    listStrings.append ( line );
    line = pStream->readLine ();
  }

  // Here we remove the trailing empty lines ...
  for (t=listStrings.count ()-1;t>=0;t--) {
    if ( ( listStrings[t].length () < 2 ) && bTrailing )
      continue;
    // once we get here, we found a non empty line at the end.
    // So from now on everything is valid.
    bTrailing  = false;
    if ( listStrings[t].length ( ) > 1 )  {
      // Taking care of the empty lines at the end ...
      if ( lineTemp.length ( ) > 1 )
         lineText = lineTemp + "\n" + listStrings[t] + "\n" + lineText;
      else
         lineText = listStrings[t] + "\n" + lineText;
      lineTemp = "";
    }
    else
      lineTemp = listStrings[t] + "\n" + lineTemp;
//    lineText = listStrings[t] + "&lt;br&gt;" + lineText;
  }

  pEntry->qsTimeStart = getTimeRange ( lineTimestamp, true  );
  pEntry->qsTimeStart.replace ( ',', '.' );
  pEntry->iTimeStart  = getTimeFromString ( pEntry->qsTimeStart );
  pEntry->qsTimeStop  = getTimeRange ( lineTimestamp, false );
  pEntry->qsTimeStop.replace ( ',', '.' );
  pEntry->iTimeStop   = getTimeFromString ( pEntry->qsTimeStop );
  pEntry->qsText      = lineText;

  return line;
}

bool Srt::isTimeStamp ( QString &line )
{
  line = line;

  //if ( ( line.find ("-->") > -1 ) && ( line.contains (':') >= 3) )	//ooo
  if ( ( line.indexOf ("-->") > -1 ) && ( line.count (':') >= 3) )	//xxx
    return true;
  return false;
}

QString Srt::getTimeRange ( QString &line, bool bStartTime )
{
  QString qsReturn;
  //int iPos = line.find ( "-->" );     //ooo
  int iPos = line.indexOf ( "-->" );    //xxx

  if ( iPos < 1 )
    return qsReturn;
  if ( bStartTime )
    //qsReturn = line.left ( iPos ).stripWhiteSpace ( );    //ooo
    qsReturn = line.left ( iPos ).simplified ( );           //xxx
  else
    //qsReturn = line.right ( line.length() - iPos -3 ).stripWhiteSpace ( );    //ooo
    qsReturn = line.right ( line.length() - iPos -3 ).simplified ( );           //xxx

  return qsReturn;
}

long Srt::getTimeFromString ( QString &qsTime )
{
  long iMSeconds = -1;
  QTime theTime;
  iMSeconds = (long)theTime.msecsTo ( QTime::fromString ( qsTime ) );

  return iMSeconds;
}

QList<Subtitles::entry *> Srt::takeSubtitles ( )	//oxx
{
  // and then we pass them on to the caller.
  QList<Subtitles::entry *>list = m_listOfEntries;	//oxx
  m_listOfEntries.clear ( );
  return list;
}

QList<Subtitles::entry *> &Srt::getSubtitles ( )	//oxx
{
  return m_listOfEntries;
}

}; // End of namespace Import
