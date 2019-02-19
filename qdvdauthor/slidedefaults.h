/***************************************************************************
    slidedefaults.h
                             -------------------
    Class SlideDefaults
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class stores the globally set parameters for th default values
   when creating a slideshow.
    
****************************************************************************/

#ifndef __SLIDE_DEFAULTS_H__
#define __SLIDE_DEFAULTS_H__

#define TAG_BACKGROUND       "BackgroundList"
#define TAG_AUDIO            "AudioList"
#define TAG_TRANSITION       "TransitionList"

#define ATTRIB_IMAGE_DELAY   "ImageDelay"
#define ATTRIB_FILTER_DELAY  "FilterDelay"
#define ATTRIB_FORMAT        "Format"
#define ATTRIB_TRANSITION    "Transition"
#define ATTRIB_INTRO         "Intro"
#define ATTRIB_KENBURNS      "KenBurns"

#include <qstringlist.h>

class QDomElement;

class CXmlSlideshow;

class SlideDefaults
{
public:
   SlideDefaults ( );
  ~SlideDefaults ( );

  SlideDefaults &operator = ( const SlideDefaults & );

  void readIniFile ( QDomElement & );
  bool saveIniFile ( QDomElement & );

  void initSlideshow ( CXmlSlideshow * );
  float imageDelay            ( );
  void  setImageDelay         ( float );
  float filterDelay           ( );
  void  setFilterDelay        ( float );
  int   formatType            ( );
  void  setFormatType         ( int );
  int   transitionType        ( );
  void  setTransitionType     ( int );
  bool  introSlide            ( );
  void  setIntroSlide         ( bool );
  bool  kenBurns              ( );
  void  setKenBurns           ( bool );
  QStringList &backgroundList ( );
  QStringList &audioList      ( );
  QStringList &transitionList ( );
  void setBackgroundList      ( QStringList & );
  void setAudioList           ( QStringList & );
  void setTransitionList      ( QStringList & );
  void addAudio               ( QString & );
  void addTransition          ( QString & );
  void addBackground          ( QString & );

protected:
  void initMe ( );

private:
  float       m_fImageDelay;
  float       m_fFilterDelay;
  int         m_iFormatType;       // PAL or NTSC
  int         m_iTransitionType;   // ImgImg or ImgBckgImg
  bool        m_bCreateIntroSlide;
  bool        m_bKenBurns;
  QStringList m_listBackground;    // Images, directories or colors
  QStringList m_listAudio;         // Audio files or directories ( what about transcoding ? )
  QStringList m_listTransitions;
};

#endif // __SLIDE_DEFAULTS_H__
