/***************************************************************************
    qdvdauthorinit.h
                             -------------------
    QDVDAuthorInit - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef QDVDAUTHORINIT_H
#define QDVDAUTHORINIT_H

// This is the version number for the Init-file
#define INIT_VERSION  3.0

#define AUTOSAVE_FILE_NAME "/.qdvdauthor/autosave.xml"
#define INIT_DIR_NAME  "/.qdvdauthor/"
#define INIT_FILE_NAME "qdvdauthor.ini"

#define INIT_DOCTYPE           ""
#define TAG_INIT               "QDVDAuthorInitFile"
#define TAG_PREFERRED_ENGINE   "PreferredEngine"
#define TAG_EXTERNAL_PLAYER    "ExternalPlayer"
#define TAG_TEMPLATE_AUTHOR    "TemplateAuthor"
#define TAG_TEMPLATE_CONTACT   "TemplateContact"
#define TAG_COLOR              "Color"
#define TAG_PATH               "Path"
#define TAG_HISTORY            "History"
#define TAG_EXECUTE            "ExecuteInterface"
#define TAG_CONVERT            "Convert"
#define TAG_SUBTITLES          "Subtitles"
#define TAG_SLIDESHOW          "Slideshow"
#define TAG_PAL                "Pal"
#define TAG_NTSC               "Ntsc"
#define TAG_DVDAUTHOR          "Dvdauthor"
#define TAG_BURNING            "Burning"
#define TAG_BLOCK              "Block"
#define TAG_SUBBLOCK           "SubBlock"
#define TAG_COMMAND            "Command"
#define TAG_COMMENT            "Comment"
#define TAG_DRAGNDROP          "DragNDrop"
#define TAG_SLIDE_DEFAULTS     "SlideDefaults"
#define TAG_FONT               "Font"

#define ATTRIB_DND_THUMBNAIL   "thumbnail_size"
#define ATTRIB_DND_BUTTON      "button_size"
#define ATTRIB_DND_BUTTON_TYPE "button_type"
#define ATTRIB_DND_WITH_TEXT   "with_text"
#define ATTRIB_DND_TEXT_POS    "text_pos"
#define ATTRIB_DND_START       "start_offset"
#define ATTRIB_DND_DURATION    "duration"
#define ATTRIB_DND_COLOR       "color"
#define ATTRIB_DND_START_WHEN  "start_when"
#define ATTRIB_DND_NICE        "nice"

#define ATTRIB_VERSION           "version"
#define ATTRIB_PNGRENDER         "pngrender"
#define ATTRIB_STYLE             "style"
#define ATTRIB_LANGUAGE          "language"
#define ATTRIB_FIRST_START       "first_start"
#define ATTRIB_AUTOSAVE          "autosave"
#define ATTRIB_ACTIVE            "active"
#define ATTRIB_MAX_HISTORY       "max_history"
#define ATTRIB_MAX_THREADS       "max_threads"
#define ATTRIB_DELETE_OLDER_THAN "delete_older_than"
#define ATTRIB_VISIBLE_REGION    "visible_region"
#define ATTRIB_TABS              "tabs"
#define ATTRIB_NUMBER            "number"
#define ATTRIB_HIDDEN            "hidden"
#define ATTRIB_EDITABLE          "editable"
#define ATTRIB_CANMODIFY         "canmodify"
#define ATTRIB_NAME              "name"
#define ATTRIB_COLOR             "color"
#define ATTRIB_DEFAULT           "default"
#define ATTRIB_FOREACH           "foreachinput"
#define ATTRIB_BLOCKNAME         "blockname"

#define BLOCK_MOVIE_CONVERT    "MOVIE_CONVERT"
#define BLOCK_AUDIO_CONVERT    "AUDIO_CONVERT"
#define BLOCK_SUBTITLES        "SUBTITLES"
#define BLOCK_MOVIE_BACKGROUND "MOVIE_BACKGROUND"
#define BLOCK_IMAGE_BACKGROUND "IMAGE_BACKGROUND"
#define BLOCK_EMPTY_SOUND      "EMPTY_SOUND"
#define BLOCK_MPLEX            "MPLEX"
#define BLOCK_SPUMUX           "SPUMUX"
#define BLOCK_OTHERS           "OTHERS"
#define SUB_BLOCK_VIDEOTRANS   "Videotrans"
#define SUB_BLOCK_TRANSCODE    "Transcode"
#define SUB_BLOCK_MENCODER     "MEncoder"
#define SUB_BLOCK_FFMPEG       "FFMpeg"

#include "dvdmenu.h"
#include "dialogexecute.h"	// get the definition of class ExecuteInterface
#include "keyword.h"
#include "utils.h"

class TranscodeInterface;
class DragNDropContainer;
class SourceFileEntry;
class SlideDefaults;

class QDVDAuthorInit
{
//private:	//ooo
public:  	//xxx
  class MenuBlock {
    public:
      class SubBlock;
      class Block
      {
        public:
          Block ( )
          { iDefaultCommand = 0; iBlockID = -1; iActive = -1; bHidden = false; bEditable = true; };
        ~Block ( )
          { for (uint t=0;t<(uint)listSubBlocks.count();t++) delete listSubBlocks[t]; };	//ox
        QString     qsBlockTag;
        int         iActive;
        int         iBlockID;
        int         iDefaultCommand;

        bool        bHidden;
        bool        bEditable;
        QStringList listComments;
        QStringList listCommands;
        QList<SubBlock *>listSubBlocks;
      };
      class SubBlock
      {
        public:
         SubBlock () { iNumber=0;  iForEachInput = -1; };
        ~SubBlock () { for (uint t=0;t<(uint)listBlocks.count();t++) delete listBlocks[t];};	//ox
        QString     qsName;	// Name. E.g. Transcode
        int         iNumber;	// running number starting from 0. In case we need to sort it.
        int         iForEachInput;   // this command is to be repeated for each input file (E.g. +AUDIO_LIST+)

        QList<Block *>listBlocks;
      };
      MenuBlock ();

      QString qsMenuTag;      // Holds the ID to indicate which MenuBlock it is (E.g. TAG_CONVERT).
      bool    bValid;         // 
      bool    bHidden;        // Should this MenuBlock be hidden initially ?
      QString qsBlockName;    // The name of the MenuBlock (E.g. Main Menu VMGM (widthxheight))
      QColor  colorBackground;
      int     iKeyset;        // wich keyset to use.

      Block   blockMovieBackground;
      Block   blockImageBackground;
      Block   blockEmptySound;
      Block   blockMplex;
      Block   blockSpumux;
      Block   blockOthers;
};

public:
  QDVDAuthorInit ();
  QDVDAuthorInit (DragNDropContainer *);
  QDVDAuthorInit (QList<Utils::toolsPaths *>&, DragNDropContainer *);
  ~QDVDAuthorInit ();
  
  QString preferredEngine ( );
  bool    visibleRegion   ( );
  void setToolsPaths ( QList<Utils::toolsPaths *>& );
  QList <Utils::toolsPaths *> &getToolsPaths     ( );
  QList <ExecuteInterface *>  &getList           ( );
  QStringList                       getHistory        ( );
  QList<QPixmap *>             getHistoryPix     ( );
  int                               maxHistoryFiles   ( );
  int                               getAutosave       ( );
  void setStyle                 ( QString );
  void setAutosave              ( int  );
  void setVisibleRegion         ( bool );
  void setVisibleTabs           ( int, bool );
  void setHistory               ( int, QStringList, QList<QPixmap *> );
  void appendMenu               ( DVDMenu *, QString, QString );
  void appendButtonTransitions  ( DVDMenu * );
  void appendPreProcessing      ( );
  void appendConvert            ( SourceFileEntry * );
  void appendConvertAudioTracks ( SourceFileEntry * );
  void appendPostProcessing     ( );
  bool readIniFile              ( );
  bool saveIniFile              ( );
  void setSlideshowDefaults     ( );
  void setDefault               ( int, uint);
  QDVDAuthorInit::MenuBlock::Block *getBlock ( int, int *pi=NULL );
  Keyword *getKeyword           ( );
  
private:	// private functions.
  bool    initMe ();
  void    setMenuAttributes  ( DVDMenu *, QString );
  void    setAudioAttributes ( SourceFileInfo   *, TranscodeInterface *, QString *p=NULL, int i=0 );
  void    addMenuAudioBlock  ( ExecuteInterface *, TranscodeInterface *, SourceFileEntry *, MenuBlock::Block * );
  void    addMenuBlock  ( ExecuteInterface *, MenuBlock::Block *, Keyword::enKeyBase e=Keyword::Keys );
  void    addSubBlock   ( ExecuteInterface *, MenuBlock::Block *, Keyword::enKeyBase e=Keyword::Keys ); //ooo
  void    addBlock      ( ExecuteInterface *, MenuBlock::Block *, Keyword::enKeyBase e=Keyword::Keys );
  bool    readBlock     ( MenuBlock::Block *,    QDomElement * );
  bool    readSubBlock  ( MenuBlock::SubBlock *, QDomElement * );
  bool    readMenuBlock ( MenuBlock *, QDomElement * );
  bool    saveMenuBlock ( MenuBlock *, QDomElement * );
  bool    saveBlock     ( MenuBlock::Block *, QDomElement * );
  bool    saveSubBlock  ( MenuBlock::SubBlock *, QDomElement * );
  int     printBlock    ( MenuBlock::Block *, int i=0 );    //ooo
  void    resetInitFile ();
  void    mergeToolsPaths (QList<Utils::toolsPaths *>);
  void    handleMultipleSubtitles ( SourceFileInfo *, QString & );
  
  void printCommandList ( char * );
  
private:  // private variables
  QString              m_qsPreferredEngine;
  bool                 m_bActivateTabs[3]; // DVDLayout/Subtitle/XMLOut
  bool                 m_bVisibleRegion;
  bool                 m_bProtectToolsObjects;
  bool                 m_bFirstApplicationStart;
  int                  m_iCurrentBlockID;
  QString              m_qsTemplateAuthor;
  QString              m_qsTemplateContact;
  QString              m_qsTempPath;
  QString              m_qsStyle;
  QString              m_qsProjectPath;
  QString              m_qsDvdauthorFile;
  QList<Rgba *>   m_arrayColors;
  int                  m_iColorCounter;
  MenuBlock            m_convertMenuBlock;
  MenuBlock            m_subtitlesMenuBlock;
  MenuBlock            m_slideshowMenuBlock;
  MenuBlock            m_palMenuBlock;
  MenuBlock            m_ntscMenuBlock;
  MenuBlock            m_dvdauthorMenuBlock;
  MenuBlock            m_burningMenuBlock;
  QStringList          m_listHistory;	   // History Project files ...
  QList<QPixmap *>m_listHistoryPix;	   // History Project file Icons ...
  bool                 m_bOwnerOfHistoryPix; // indicates if we have ownership and thus should delete the objects at exit ...
  int                  m_iMaxHistoryFiles;
  int                  m_iAutosave;
  QStringList          m_soundList;
  QStringList          m_listAudioFiles; // to prevent double transcoding of audio files.
  Keyword              m_keyword;
  // Pre processing steps
  ExecuteInterface     m_convertInterface;   // Also incl Subtitles.
  ExecuteInterface     m_slideshowInterface; // Remaining slideshows and Button transitions
  SlideDefaults       *m_pSlideDefaults;	//ooo
  
  // Post processing steps
  ExecuteInterface m_dvdauthorInterface;
  ExecuteInterface m_burnInterface;
  
  // The full list of commands to execute ...
  QList <ExecuteInterface *>  m_listExecute;
  QList <ExecuteInterface *>  m_listToBeDeleted;
  QList <Utils::toolsPaths *> m_listToolsPaths;
  DragNDropContainer              *m_pDragNDropContainer;
  bool                             m_bOwnDragNDropContainer;

};

#endif // QDVDAUTHORINIT_H
