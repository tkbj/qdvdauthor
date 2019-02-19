/***************************************************************************
    exportdvdauthor.h
                             -------------------
    Implementation of class Export::DVDAuthor
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class collects all possible information about
   the current set stream.
    
****************************************************************************/

#ifndef __EXPORT_DVDAUTHOR_H__
#define __EXPORT_DVDAUTHOR_H__

#include <QMap>
//#include <q3valuelist.h>	//ooo

class SourceFileEntry;
class CXmlDVDAuthor;
class ButtonObject;
class CXmlSpumux;
class Subtitles;
class VMGMenu;
class DVDMenu;


namespace Export
{

class DVDAuthor;

class JumpTable
{
public:
	JumpTable ( );

	// The jumpIndex is the number assigned to g3
	// the menu designated to handle the jumpTable 
	// will use this index to jump to the associated 
	// title / chapter.
	bool            bAlreadyPresent; // the optimization step filters out double entries
	int             iJumpIndex;
	int             iTitle;
	int             iMenu;
	int             iChapter;
	QString         qsAction;
	DVDMenu        *pMenu;
	ButtonObject   *pButton;
};

class ButtonTransition
{
  // For ButtonTransitions from a Menu to another Menu we'll create a new Titleset which only holds
  // transition videos, followed by the post command to jump to the actual menu.
public:
  class Source {
   public:
    Source ( );
    ButtonObject *pSourceButton;
    QString       qsChapter;    // Only vald for Transitions to Video files
    DVDMenu      *pSourceMenu;
    QString       qsTransitionFileName;
    int           iChapter;    // Only valid for video sources.
    int           iTitle;      // Only valid for video sources.
  };
 public:
  ButtonTransition ( SourceFileInfo *, DVDMenu * );
 ~ButtonTransition ( );
  void addSource   ( ButtonObject *, DVDMenu *, QString );

  int     calcTitleIdx ( ButtonObject * );
  Source *findButton   ( ButtonObject * );
  Source *findChapter  ( QString );
  QString fileName     (   int   );

  SourceFileInfo     *  pTargetInfo;
  DVDMenu            *  pTargetMenu;
  int                   iJumpIndex; // The Jump to the actual menu after we played the transition video
  //Q3ValueList<Source  *> listSource;		//ooo
  QList<Source  *> listSource;			//xxx
};

class ExportTitleset
{
public:
	 ExportTitleset              ( DVDAuthor       * );
	~ExportTitleset              (                   );
	int  findTitle               ( SourceFileInfo  * );
	int  findTitle               ( SourceFileEntry * );
	int  findTitle               ( ButtonTransition*, ButtonObject *p=NULL );
	int  findMenu                ( DVDMenu *         );
	JumpTable *findJumpTable     ( DVDMenu *         );
	int  getTotalChapters        ( SourceFileInfo  * );
	int  getChapters             ( SourceFileInfo  *, QString );
	bool checkForPrePostCommands ( SourceFileEntry *, bool b=false );

	int                           iTitlesetNumber;
	bool                          bSetByUser;
	QString                       qsFormat;
	DVDAuthor                  *  pExportDVDAuthor;
	QList<DVDMenu         *> listMenus;              // all menus which fit the format.		//oxx
	QList<JumpTable       *> listJumpTable;          // the jump table				//oxx
	QList<SourceFileEntry *> listSourceFileEntries;							//oxx
	QList<ButtonTransition*> listButtonTransitions;							//oxx
};

class PlayMultiple
{
 public:
  PlayMultiple ( SourceFileInfo *, SourceFileInfo * );
  QString getPostCommand  ( );
  void    addPlayMultiple ( QString & );

  SourceFileInfo *pSourceInfo;
  SourceFileInfo *pTargetInfo; // required to determine checkForPrePostCommands
  QString         qsPost;
  bool            bLoop;
};


class DVDAuthor
{
public:
   DVDAuthor  ( );
  ~DVDAuthor  ( );

  bool exportXml ( );
  bool createXml ( );
  bool createXml ( QString & );

private:
  ExportTitleset *findTitleset ( SourceFileEntry * );
  ExportTitleset *findTitleset ( SourceFileInfo * );
  ExportTitleset *findTitleset ( ButtonObject * ); // currently unused ...
  ExportTitleset *findTitleset ( DVDMenu * );
  ExportTitleset *findTitleset ( int );
  JumpTable      *findJumpTableEntry ( ButtonObject * );
  ButtonTransition *findTransition   ( void *, bool b=true );
  PlayMultiple   *findMultiple ( SourceFileInfo *, bool );
  bool optimizeJumpTable       ( ExportTitleset *, JumpTable * );
  int  getHighestTitlesetNumber( );
  int  getFirstEmptyTitleset   ( );
  void createVMGMTransitions   ( );
  bool populateListOfTitlesets ( );
  bool stowMenusAway           ( );
  bool createJumpTable         ( );
  bool createNoMenuDVD         ( QString & );
  bool createDVDAuthorXml      ( QString & );
  void createPlayMultiple      ( ButtonObject   *, DVDMenu *, uint );
  void createJumpToMenu        ( SourceFileInfo *, DVDMenu *, uint );
  QString createFormatString   ( SourceFileInfo *, bool  b = false );
  QString createFormatString   ( DVDMenu * );
  bool cleanUpSourceFileEntries( QList<SourceFileEntry *>   & );	//oxx
  void cleanSourceFileEntris   ( );

  ExportTitleset *addToTitleset(SourceFileEntry *, int, bool b=false      );
  bool addToTitleset( CXmlDVDAuthor::titleset_struct *, ButtonTransition* );
  bool addToTitleset( CXmlDVDAuthor::titleset_struct *, SourceFileEntry * );
  bool addToTitleset( CXmlDVDAuthor::titles_struct *,   SourceFileEntry * );
  bool addToTitleset( CXmlDVDAuthor::pgc_struct *,      SourceFileEntry * );
  bool addToTitleset( CXmlDVDAuthor::pgc_struct *,      SourceFileInfo  * );
  void addTransition( CXmlDVDAuthor::pgc_struct *,      SourceFileInfo  * );
  bool addToMenu    ( CXmlDVDAuthor::titleset_struct *, ExportTitleset  * );
  bool addToMenu    ( CXmlDVDAuthor::menus_struct *,    ExportTitleset  * );
  bool addToMenu    ( CXmlDVDAuthor::pgc_struct *,      ExportTitleset  * );
  bool addToMenu    ( CXmlDVDAuthor::titleset_struct *, DVDMenu         * );
  bool addToMenu    ( CXmlDVDAuthor::menus_struct *,    DVDMenu         * );
  bool addToMenu    ( CXmlDVDAuthor::pgc_struct *,      DVDMenu         * );
  bool addToVideo   ( CXmlDVDAuthor::video_struct *,    QString&,QString& );
  bool addToVideo   ( CXmlDVDAuthor::video_struct *,    int,int,QString & );
  bool addToVideo   ( CXmlDVDAuthor::video_struct *,    ButtonTransition* );
  bool addToAudio   ( CXmlDVDAuthor::menus_struct *,    DVDMenu         * );

  CXmlDVDAuthor::pgc_struct *getPgc ( CXmlDVDAuthor::menus_struct *, DVDMenu * );
  QString getPreAction         ( ButtonObject     * );
  DVDMenu *getMenuFromButton   ( ButtonObject     * );
  DVDMenu *getMenuFromEntry    ( SourceFileEntry  * );
  QString jumpToSubMenu        ( SourceFileEntry  * );
  QString adjustChapterOffset  ( QStringList &, int );
  QString getFromFormat        ( QString &,     int );
  void getButtonNumberFromName ( QList<ButtonObject *> &, QString & );		//oxx

  bool m_bOutOfSpecDialog;
  bool m_bMenuZeroDialog;
  bool m_bInsertComments;
  QMap<void *, ButtonTransition *> m_mapTransitionFiles;
  QList<DVDMenu *>            m_listOfAllMenus;		//oxx
  QList<PlayMultiple *>       m_listPlayMultiple;	//oxx
  QList<ExportTitleset *>     m_listTitlesets;		//oxx
  QList<SourceFileEntry *>    m_listNewEntries;		//oxx
  friend class ExportTitleset;
};

class Spumux
{
public:
   Spumux ( );
  ~Spumux ( );

  bool createXml      ( );
  bool createXml      ( DVDMenu * );
  bool createXml      ( SourceFileInfo * );
  bool createXml      ( SourceFileInfo *, Subtitles * );
  bool createTextXml  ( SourceFileInfo *, Subtitles * );
  bool createImageXml ( SourceFileInfo *, Subtitles * );

protected:
  bool correctOverlappingButtons ( QList<QRect> &, int, int );		//oxx

};

}

#endif // __EXPORT_DVDAUTHOR_H__
