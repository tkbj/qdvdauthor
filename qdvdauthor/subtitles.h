/***************************************************************************
    subtitles.h
                             -------------------
    Subtitles - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class encapsulates the attributes or the Subtitle generation.
    
****************************************************************************/

#ifndef __SUBTITLES_H__
#define __SUBTITLES_H__

#include <qstringlist.h>
#include <qfont.h>
#include <qrect.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#include "rgba.h"

class QDomNode;
class QDomElement;

class SourceFileInfo;

// MAX_SUBTITLE_COLORS 4 does not work as ImageMagick counts 5 in some cases using spumux.
#define MAX_SUBTITLE_COLORS  3
#define MAX_SUBTITLES       32

class SubtitleEntry
{
public:
  SubtitleEntry ( );
  SubtitleEntry &operator = ( SubtitleEntry & );
  QString m_qsIso639;        // Language code    ( E.g. "en" )
  QString m_qsFileName;      // Source file name ( if exists )
  int     m_iSubtitleNumber; // Subtitle track    [ 0 .. 31 ]
};

class Subtitles : public SubtitleEntry
{
public:
  enum enState {
    STATE_MANUAL = 0, // User imported or manually entered this subtitle
    STATE_EXTRACTING, // Subtitle is being extracted from meta info in background task
    STATE_META_INFO   // Subtitle was created using videos Meta information
  };

  class entry {
  public:
    entry ();
    entry &operator = ( entry & );

    int     iIndex;
    QString qsTimeStart;// 00:00:00.000
    QString qsTimeStop; // 00:00:00.000
    long    iTimeStart; // in ms
    long    iTimeStop;  // in ms
    QString qsText;
    QRect   rect;       // Note not stored as it is created when drawing the text.
  };

   Subtitles ( int, int );
  ~Subtitles ( );

  Subtitles & operator =    ( Subtitles   & );
  bool    readProjectFile   ( QDomNode    & );
  bool    writeProjectFile  ( QDomElement & );
  long    getTimeFromString ( QString     & );
  QString getStringFromTime ( long          );
  QString getXMLFileName    (               );

  Subtitles *render         ( QString, QString, QString, bool b=false );
  bool alreadyRendered      ( QString );


  // static fnc to register for background subtitle extraction ...
  static void generateSubtitles ( QObject *, SourceFileInfo *, QString );

private: // private member functions.
  QRect   getSubtitlesRect ( QString & );
  void    cutHorizontal    ( QImage  &, QRgb );

public:
  enState              m_subtitleState;
  bool                 m_bTextSubtitles;
  QString              m_qsTextFont;
  int                  m_iTextFontSize;
  bool                 m_bFit;
  int                  m_iOutline;
  QFont                m_font;
  QRect                m_rect;
  QSize                m_videoSize;
  QPoint               m_alignment; // x=1==Center / x=2==Left    / x=3==Right / x=4==Justify
                                    // y=1==Bottom / y=2==VCenter / y=3==top   / y=4==Bottom
  QList <entry *> m_listOfSubtitles;			//oxx
  // this is the file that is going to be created.
  QString              m_qsTempFile;
  Rgba                 m_subColors [ MAX_SUBTITLE_COLORS ];
  QString              m_qsXmlFile; // holds the full path of the xml file.
};

#endif // __SUBTITLES_H__

