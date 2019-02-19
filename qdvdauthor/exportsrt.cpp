/***************************************************************************
    exportsrt.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file holds the implementation of the export filter for the srt
   subtitles file.
    
****************************************************************************/

#include <QFileDialog>
#include <QTextStream>

#include "global.h"
#include "sourcefileentry.h"
#include "exportsrt.h"

namespace Export
{

BaseSubtitle::BaseSubtitle  ( )
{
}

BaseSubtitle::~BaseSubtitle  ( )
{
}

Srt::Srt ()
{
}

Srt::~Srt ()
{
}

bool Srt::exportSub ( Subtitles *pSubtitles )
{
  //QString qsExportFile = Q3FileDialog::getSaveFileName ( Global::qsCurrentPath, QObject::tr("Subtitle files (*.srt *.SRT)"), NULL, QObject::tr("Save Subtitles File") );	//ooo
  QString qsExportFile = QFileDialog::getSaveFileName ( NULL, QObject::tr("Save Subtitles File"), Global::qsCurrentPath, QObject::tr("Subtitle files (*.srt *.SRT)") );		//xxx

  // Sanity check .. Did te user choose a new project file ?
  if (qsExportFile.isNull ())
    return true;

  return writeFile ( pSubtitles, qsExportFile );
}

bool Srt::writeFile ( Subtitles *pSubtitles, QString &fileName )
{
  uint t;
  // Assign the file
  QString qsTimeStart, qsTimeStop;
  Subtitles::entry *pEntry;
  QFile projectFile ( fileName );

  if ( ! projectFile.open ( QIODevice::WriteOnly ) )
    return false;

  QTextStream srtStream( &projectFile );
  for ( t=0;t<pSubtitles->m_listOfSubtitles.count ();t++) {
    pEntry = pSubtitles->m_listOfSubtitles[t];
	qsTimeStart = pEntry->qsTimeStart;
	qsTimeStart.replace ( ".", "," );
	qsTimeStop  = pEntry->qsTimeStop;
	qsTimeStop.replace  ( ".", "," ); 
	srtStream << pEntry->iIndex << "\n";
    srtStream << qsTimeStart << " --> " << qsTimeStop << "\n";
    srtStream << pEntry->qsText << "\n\n";
  }

  projectFile.flush ( );
  projectFile.close ( );

  return true;
}

}; // End of namespace Export
