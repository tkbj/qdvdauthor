/***************************************************************************
    importsrt.h
                             -------------------
    Class Import::Srt
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class handles the import filter for srt (subtitle) files
    
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

#ifndef __IMPORT_SRT_H__
#define __IMPORT_SRT_H__

#include "sourcefileentry.h"
//Added by qt3to4:
//#include <Q3TextStream>   //ooo
#include <QTextStream>      //xxx
//#include <Q3ValueList>	//oxx

namespace Import
{

class BaseSubtitle
{
  public:
             BaseSubtitle ( );
    virtual ~BaseSubtitle ( );

    virtual bool readFile ( QString & ) = 0;
    virtual QList<Subtitles::entry *> takeSubtitles ( ) = 0;		//oxx
    virtual QList<Subtitles::entry *> &getSubtitles ( ) = 0;		//oxx
    QString getFileName ( );

  protected:
    QList<Subtitles::entry *> m_listOfEntries;		//oxx
    QString m_qsFileName;
};

class Srt : public BaseSubtitle
{
public:

public:
           Srt ( );
  virtual ~Srt ( );

  virtual bool readFile ( QString & );
  virtual QList<Subtitles::entry *> takeSubtitles ( );		//oxx
  virtual QList<Subtitles::entry *> &getSubtitles ( );		//oxx

  bool    import      ( );
  //QString readEntry   ( Q3TextStream *, Subtitles::entry * ); //ooo
  QString readEntry   ( QTextStream *, Subtitles::entry * );    //xxx

private:
  bool    isTimeStamp  ( QString &       );
  QString getTimeRange ( QString &, bool );
  long    getTimeFromString ( QString &  );
};

}

#endif // __IMPORT_SRT_H__
