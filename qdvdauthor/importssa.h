/***************************************************************************
    importssa.h
                             -------------------
    Class Import::Ssa
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class handles the import filter for srt (subtitle) files

****************************************************************************/

#ifndef __IMPORT_SSA_H__
#define __IMPORT_SSA_H__

#include "importsrt.h"

#include "sourcefileentry.h"
//Added by qt3to4:
//#include <Q3TextStream>   //ooo
#include <QTextStream>      //xxx
//#include <Q3ValueList>	//oxx

namespace Import
{

class Ssa : public BaseSubtitle
{
  // Here are the infromation from the [Script Info] section
  class Script  {
  public:
    Script ( );
    int   m_iPlayResX; // E.g. 480
    int   m_iPlayResY; // E.g. 720
    float m_fTimerMultiply; // E.g. 100.0. All times are multiplied by this percentage to adjust timing
  };

  // Variables from [V4 Styles] or [V4+ Styles]
  // Note colors are stored in AABBGGRR in th ssa file.
  class Style {
  public:
    Style ( );
    QString qsStyleName;       // Name
    QString qsFontName;        // Fontname
    int     iFontSize;          // Fontsize
    Rgba    clrFillingColor;    // PrimaryColour
    Rgba    clrSecondaryColour; // SecondaryColour ( unused )
    Rgba    clrOutline;         // TertiaryColour or OutlineColour
    Rgba    clrShadow;          // BackColour
    int     iAlignement;        // Alignement ( 1=Left, 2=Centered, 3=Right )
    bool    bBold;              // Bold       ( 0=false, -1=true )
    bool    bItalic;            // Italic     ( 0=false, -1=true )
    bool    bUnderline;         // Underline  ( 0=false, -1=true )
    bool    bStrikeOut;         // StrikeOut  ( 0=false, -1=true )
    int     iSpacing;           // Spacing in pixels between chracters  ( not used, always 0 )
    int     iAngle;             // Angle      ( not used, always 0 )
    int     iBorderStyle;       // BorderStyle 1=Ouline+DropShadow 3=OpaqueBox ( not used, always 1 )
    int     iOutline;           // Outline ( if BorderStyle=1, then this is the width of the outline )
    int     iShadow;            // Shadow  ( if BorderStyle=1, then distance from shadow to Subtitle )

    int     iMarginL;           // MarginL ( minimum margin to video limits )
    int     iMarginR;           // MarginR ( minimum margin to video limits )
    int     iMarginV;           // MarginV ( unused )
    int     iEncoding;          // Encoding   ( character set # )
    float   fScaleX;            // ScaleX width  of font in percent ( ignored )
    float   fScaleY;            // ScaleY height of font in percent ( ignored )
    int     iAlphaLevel;        // AlphaLevel ( 0=null, 255=total ) unused, alpha is in the color defs
  };

  class Event  {
    // Dialogue: this is the only event to be displayed
  public:
    Event ( );
    int     iMarked;     // Marked ( unused. 0=Line not shown, 1= Line is shown )
    int     iLayer;      // Layer  ( unused, always 0 )
    QString qsStartTime; // Start ( E.g. 0:01:21.47 )
    QString qsEndTime;   // End   ( E.g. 0:01:22.63 )
    Style  *pStyle;      // Style ( pointer to Style object [From StyleName] )
    QString qsName;      // Name. mostly used to know who spoke the text in the movie
    int     iMarginL;    // MarginL E.g. 0000 ( ignored )
    int     iMarginR;    // MarginL E.g. 0000 ( ignored )
    int     iMarginV;    // MarginL E.g. 0000 ( ignored )
    QString qsEffect;    // Effect ( unused )
    QString qsText;      // Text Subtitle text Ignore everything inside '{', and '}'
    // Special chars :
    // \N, and \n    : Carriage Return
    // \i1 txt \i0   : Marks text italic in between
    // \u1 txt \u0   : Marks text underlined in between
    // \s1 txt \s0   : Marks text strikeout in between
    // \fn fontName  : Sets font ( E.g. \fnCourier )
    // \fs fontSize  : Sets Font Size ( E.g. \fs16 )
    // \r<style>     : Cancels all previous style overrides
    // Many more not handled here ( http://moodub.free.fr/video/ass-specs.doc )
  };

public:
           Ssa ( );
  virtual ~Ssa ( );

  virtual bool readFile ( QString & );
  virtual QList<Subtitles::entry *> takeSubtitles ( );		//oxx
  virtual QList<Subtitles::entry *> &getSubtitles ( );		//oxx

  bool    import      ( );
  //QString readEntry   ( Q3TextStream *, Subtitles::entry * ); //ooo
  QString readEntry   ( QTextStream *, Subtitles::entry * );    //xxx
  void    toSubtitles ( );

private: // Private member functions
  //bool    readScriptInfo ( Q3TextStream & );  //ooo
  bool    readScriptInfo ( QTextStream & );     //xxx
  //bool    readSSAStyles  ( Q3TextStream & );  //ooo
  bool    readSSAStyles  ( QTextStream & );     //xxx
  //bool    readASSStyles  ( Q3TextStream & );  //ooo
  bool    readASSStyles  ( QTextStream & );     //xxx
  //bool    readEvents     ( Q3TextStream & );  //ooo
  bool    readEvents     ( QTextStream & );     //xxx
  void    clearEntries   ( );
  Rgba    toRgba ( QString & );


private: // Private member variables
  Script  m_script;
  QList<Style *> m_listOfStyles;	//oxx
  QList<Event *> m_listOfEvents;	//oxx	
};

}

#endif // __IMPORT_SSA_H__
