/***************************************************************************
    keyword.h
                             -------------------
    Class Keyword
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will replace a given token in a string with a value
    
****************************************************************************/

#ifndef KEYWORD_H
#define KEYWORD_H

#include <qstringlist.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

class Keyword
{
public:
  enum enKeyBase { Keys, TransKeys, SubKeys, BaseLast };

  enum enTranscodeKeys {
    TransDVDAuthorXML=0, TransProjectPath,    TransTempPath,    TransInputName , 
    TransBaseName,       TransFileExt,        TransAudioFormat, TransAudioType, 
    TransAudioBitrate,   TransSampleRate,     TransStartingAt,  TransEndingAt,
    TransDuration,       TransWidth,          TransHeight,      TransVideoFormat,
    TransVideoType,      TransVideoBitrate,   TransFrameRate,   TransRatio,
    TransAudioList,      TransForEachCounter, TransTrackName,   TransLast
  };
  enum enKeys {
    KeyDVDAuthorXML=0,     KeyProjectPath,   KeyTempPath,      KeyWidth, 
    KeyHeight,             KeyFormat,        KeyMenuName,      KeyAspectRatio,
    KeyStartPos,           KeyDuration,      KeyNrOfFrames,    KeyBackgroundFileName, 
    KeyBackgroundFullName, KeyButtonName,    KeySoundList,     KeyLast
  };
  enum enSubtitlesKeys {
    SubProjectPath=0,  SubTempPath, SubInputFileName, SubXmlFile, SubBaseName,
    SubFileExt, SubSubtitleNumber, SubMultipleSubtitlePipe, SubLast
  };

  class Entry {
  public:
     Entry  ( ) { iIndex = 0; };
    ~Entry  ( ) { };
    QString qsKey;
    QString qsValue;
    int     iIndex;
  };

   Keyword ();
  ~Keyword ();

  void        printf                           ( int );
  int         count                            ( int );
  void        clear                            ( int );
  QString     value                       ( int, int );
  QString     key                         ( int, int );
  bool        setValue           ( int, int, QString );
  QString     resolvString            ( int, QString );
  QString     resolvConvert       ( QString, QString );
  QStringList getValues                        ( int );
  QStringList getKeys                          ( int );
  void        setKeywords ( int, QList<QString> );	//oxx

private:
  QList<Entry *> m_listOfPair    [ BaseLast ];		//oxx
};

#endif // KEYWORD_H


