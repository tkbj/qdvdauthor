/***************************************************************************
    exportsrt.h
                             -------------------
    Class Export::Srt
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class handles the export filter for srt (subtitle) files
   
   2
   00:00:27,480 --> 00:00:31,480
   Une production SAMULI TORSSONEN
   (adaptée de STAR TREK & BABYLON 5)
   
   3
   00:00:42,400 --> 00:00:45,880
   Un film de TIMO VUORENSOLA
   SOUS-TITRAGE & TRADUCTION : Fred CORNEC
   
   Lines 3 onward are the text of the subtitle. New lines are indicated by new lines (i.e. there's no "\n" code). The only formatting accepted are the following:
   
   <b>text</b>: put text in boldface
   <i>text</i>: put text in italics
   <u>text</u>: underline text
   <font color="#00ff00">text</font>: apply green color formatting to the text (you can use the font tag only to change color)
    
****************************************************************************/

#ifndef __EXPORT_SRT_H__
#define __EXPORT_SRT_H__

#include "sourcefileentry.h"

namespace Export
{

class BaseSubtitle
{
public:
           BaseSubtitle  ( );
  virtual ~BaseSubtitle  ( );
  virtual bool exportSub ( Subtitles * ) = 0;
  virtual bool writeFile ( Subtitles *, QString & ) = 0;
};

class Srt : public BaseSubtitle
{
public:
           Srt ( );
  virtual ~Srt ( );

  virtual bool exportSub ( Subtitles * );
  virtual bool writeFile ( Subtitles *, QString & );
};

}

#endif // __EXPORT_SRT_H__
