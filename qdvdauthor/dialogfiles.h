/***************************************************************************
    dialogfiles.h
                             -------------------
    DialogFiles
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This ialog offers an easy way to select and sort source files ( videos )
    into SourceFileEntries.

    This of course is helpfull if you have 100+ files

****************************************************************************/

#ifndef __DIALOGFILES_H__
#define __DIALOGFILES_H__


//#include <q3listview.h>       //ooo
#include <QListWidget>            //xxx
//#include <qmenudata.h>        //ooo
#include <QMenu>                //xxx
//#include <q3listbox.h>        //ooo
//#include <QListView>            //xxx
//#include <q3process.h>        //ooo
#include <QProcess>             //xxx
#include <QMutex>               //xxx
#include <qpixmap.h>
#include <qmap.h>

#include "qplayer/mediascanner.h"
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	     //oxx
#include <QDragEnterEvent>
#include <QDropEvent>
//#include <Q3PopupMenu>        //ooo
#include "sourcefileentry.h"
//#include "uidialogfiles.h"	//ooo
#include "ui_uidialogfiles.h"	//xxx
#include "cachethumbs.h"

//class Q3IconViewItem;         //ooo
class QListWidgetItem;          //xxx
//class Q3IconDragItem;         //ooo
class QPushButton;
//class Q3ScrollView;           //ooo
class QScrollArea;              //xxx
class QToolBox;
class QTimer;

class SourceToolBar;
class CXmlSlideshow;
class Previewer;

// Separate from global namspace because nested classes 
// are not supported by moc.
namespace Input
{

class DialogFiles; // forward declaration

//class GroupView : public Q3ListView   //ooo
class GroupView : public QTreeWidget    //xxx
{
  Q_OBJECT
public:
  // This is the size for all thumbs in the list controls
  static const int m_iSize;

  //class Item : public Q3ListViewItem  {   //ooo
  class Item : public QTreeWidgetItem  {    //xxx
    public:
    //Item ( Q3ListView *, QString, bool ); //ooo
    Item ( QTreeWidget *, QString, bool );  //xxx
    //Item ( Q3ListView *, Item *, QString, bool ); //ooo
    Item ( QTreeWidget *, Item *, QString, bool );  //xxx
    virtual ~Item ( );

    //virtual void paintCell ( QPainter   *, const QColorGroup &, int, int, int );  //ooo
    //virtual int  width     ( const QFontMetrics &, const Q3ListView *, int ) const;   //ooo
    //virtual int  width     ( const QFontMetrics &, const QListWidget *, int ) const;    //xxx

    SourceFileInfo       *pSourceFileInfo;
    Cache::Thumbs::Entry *pCache;
    bool bAlternateColor;
  };

  //GroupView ( QWidget *, Q3IconView *, DialogFiles *, SourceFileEntry *p=NULL );  //ooo
  GroupView ( QWidget *, QListWidget *, DialogFiles *, SourceFileEntry *p=NULL );     //xxx
  //GroupView ( QWidget *, Q3IconView *, DialogFiles *, CXmlSlideshow   * );        //ooo
  GroupView ( QWidget *, QListWidget *, DialogFiles *, CXmlSlideshow   * );           //xxx
  GroupView ( QWidget *, SourceFileEntry * );
  virtual ~GroupView  ( );

  void setSlideshow ( CXmlSlideshow * );

  CXmlSlideshow   *slideshow       ( ); // could be NULL
  SourceFileEntry *sourceFileEntry ( ); // could be NULL

  void append ( void * );

  QString groupName     ( );
  void    setGroupName  ( QString & );
protected: // member functions
  // Overloaded function
  //virtual Q3DragObject *dragObject( );    //oooo
  //virtual void dropEvent         ( QDropEvent      * );   //ooo
  virtual void dragEnterEvent    ( QDragEnterEvent * );
  virtual void timerEvent        ( QTimerEvent     * );

protected slots:
  //virtual void slotAlternateColors ( );   //ooo

private: // member variables
  //Previewer       *m_pFramePreview;	//xxxx -> *.ui
  //Q3IconView       *m_pPreview;       //ooo
  QListWidget       *m_pPreview;      //xxx
  DialogFiles     *m_pDialog;
  SourceFileEntry *m_pSourceFileEntry;
  CXmlSlideshow   *m_pSlideshow;
};

//class ScanDVDate : public Q3Process   //ooo
class ScanDVDate : public QProcess      //xxx
{
  Q_OBJECT
public:
  // Scanning the input video for DVDates
  //ScanDVDate ( DialogFiles *, QString, Q3IconViewItem * );    //ooo
  ScanDVDate ( DialogFiles *, QString, QListWidgetItem * );     //xxx
  virtual ~ScanDVDate ( );

  bool hasExited ( );

protected slots:
  virtual void slotExited ( );

private:
  bool bExited;
  QString        qsFileName;
  //Q3IconViewItem *pItem;      //ooo
  QListWidgetItem *pItem;       //xxx
  DialogFiles   *pDialog;
};

//class DialogFiles : public uiDialogFiles			//ooo
class DialogFiles : public QDialog, public Ui::uiDialogFiles	//xxx
{
  Q_OBJECT
  //class DirItem : public Q3ListViewItem  {    //ooo
  class DirItem : public QTreeWidgetItem  {     //xxx
  public:
    //DirItem ( Q3ListView *, QString );    //ooo
    DirItem ( QTreeWidget *, QString );     //xxx
    DirItem ( DirItem *,   QString );
    virtual ~DirItem ( ) { };

    virtual void setOpen ( bool );
    QString  dir ( );
    DirItem *findSubDir ( QString );

  private:
    DirItem *pParent;
    QString  qsPathName;
    bool     bReadable;
  };
  class Manual  {
    // This class stores manual assignments of subtitles / audio tracks to video source files.
    // Instead of keeping this info around forever in the cache or project file, we limit it
    // to a working session of QDVDAuthor
  public:
    class Change  {
    public:
       Change ( );
      ~Change ( );

       void clearAudioList    ( );
       void clearSubtitleList ( );
       void copyAudioList     ( QList<Audio *>         & );	//oxx
       void copySubtitleList  ( QList<SubtitleEntry *> & );	//oxx
       QList<Audio *>         copyAudioList     ( );		//oxx
       QList<SubtitleEntry *> copySubtitleList  ( );		//oxx

       QString qsFileName;
       unsigned long long iHashValue;
       QList<Audio *> audioList;		//oxx
       QList<SubtitleEntry *> subtitleList;	//oxx
    };

     Manual ( );
    ~Manual ( );

     QString qsPath;  // The change is for a video file in this path
     unsigned long long iHashValue; // instead of searching path names, use a map of hash values.
     QMap<unsigned long long, Change *> mapOfChange;
  };

protected:
  // When the image is rendered the returned data is temp stored in this struct
  // to be executed in timerEvent.
  class WorkWaiting  {
  public:
     //WorkWaiting ( Q3IconViewItem *, QImage *, bool );    //ooo
     WorkWaiting ( QListWidgetItem *, QImage *, bool );     //xxx
    ~WorkWaiting ( );

    //Q3IconViewItem *pItem;        //ooo
    QListWidgetItem *pItem;         //xxx
    QImage        *pImage;
    QString        qsIconText;
    bool           bMarkIcon;
  };
  class ThumbnailRequest : public ExecuteJob {
  public:
    ThumbnailRequest ( DialogFiles * );
    //ThumbnailRequest ( DialogFiles *, Q3IconViewItem *, QString, uint );  //ooo
    ThumbnailRequest ( DialogFiles *, QListWidgetItem *, QString, uint );   //xxx
    virtual ~ThumbnailRequest ( );

    virtual bool response ( );

    //Q3IconViewItem *pOwnerItem;       //ooo
    QListWidgetItem *pOwnerItem;        //xxx
    DialogFiles   *pParent;
    uint           iNumberOfThumbs;
  };
  class MultiThumbRequest : public ThumbnailRequest  {
  public:
    //MultiThumbRequest ( DialogFiles *, Q3IconViewItem *, QString, QImage *, uint );   //ooo
    MultiThumbRequest ( DialogFiles *, QListWidgetItem *, QString, QImage *, uint );    //xxx
    virtual ~MultiThumbRequest ( );
    virtual bool response ( );

    MultiThumbRequest *clone ( );
    void handleResponse ( bool, QString );
    void createThumbs   ( );

    QString qsTitle;
    long iMovieLength;
    long iDelta;
    uint iSize;
    uint iCurrentThumb;
    QImage **arrayOfThumbs;
  };
  class Thumbs {
  public:
     Thumbs ( );
    ~Thumbs ( );

    bool findTracks    ( );
    void clearAudio    ( );
    void clearSubtitles( );
    bool audioTrackEnabled    ( QString & );
    bool subtitleTrackEnabled ( QString & );
    void toggleAudioTrack     ( Audio          *  );
    void toggleSubtitleTrack  ( SubtitleEntry  *  );
    bool getManualChange      ( Manual::Change *& );
    bool getManualChange      ( Manual *&, Manual::Change *&, unsigned long long &, unsigned long long & );

    //Q3IconViewItem *pOwnerItem;       //ooo
    QListWidgetItem *pOwnerItem;        //xxx
    QPixmap      **arrayOfPix; // these pixmaps are the scaled versions of pEntry->arrayOfThumbs
    uint           iNumberOfPix;
    uint           iCurrentPix;

    // we need a pointer to the corresponding cache object in here
    Cache::Thumbs::Entry *pCache;
    SourceFileInfo *pSourceFileInfo;          // only set if the item is created through a drop from a GroupView.
    QList<Audio *>         audioList;    // the list of associated Audio tracks			//oxx
    QList<SubtitleEntry *> subtitleList; // the list of associated subtitle tracks		//oxx
  };

public:
  //class VFSelectItem : public Q3ListBoxText  { //QListBoxItem {   //ooo
  //class VFSelectItem : public QTreeWidgetItem  { //QListBoxItem {   //xxx
  class VFSelectItem : public QListWidgetItem  { //QListBoxItem {   //xxx
  public:
    //VFSelectItem ( bool, Q3ListBox *, const QString &t=QString::null );   //ooo
    //VFSelectItem ( bool, QTreeWidget *, const QString &t=QString::null );   //xxx
    VFSelectItem ( bool, QListWidget *, const QString &t=QString::null );   //xxx
    virtual void paint ( QPainter * );
    //virtual int height ( const Q3ListBox * ) const;   //ooo
    //virtual int height ( const QTreeWidget * ) const;   //xxx
    virtual int height ( const QListWidget * ) const;   //xxx

    bool bAlternate;
  };

public: // public strctures ...
  //DialogFiles ( QWidget *, bool b=false, const char *n=0, Qt::WFlags f=0 );	//ooo
  DialogFiles ( QWidget *, bool bIsImageDialog=false );				//xxx
  virtual ~DialogFiles ( );

  static void clearMapOfChange ( );

  void initMe        ( );
  //void initWork      ( Q3IconViewItem *, QImage *, bool );    //ooo
  void initWork      ( QListWidgetItem *, QImage *, bool );     //xxx
  bool initScanDV    ( Thumbs * );
  void addProgress   ( int );
  uint thumbnailSize ( );
  static bool isMyself ( DialogFiles * );

  //SourceFileInfo *findSourceFileInfo ( Q3IconViewItem * );    //ooo
  SourceFileInfo *findSourceFileInfo ( QListWidgetItem * );     //xxx
  QList<SourceFileEntry *> &getGroups ( );		//oxx

protected:
  virtual void initGroups( );
  void createScrollView  ( );
  void copyEntry         ( SourceFileEntry *, SourceFileEntry * );
  GroupView *addGroup    ( QString &, QStringList & );
  void addSourceToGroup  ( GroupView * );
  QString uniqueGroupName( QString );
  bool findDesktopDir    ( QString );
  void initVirtualFolders( );
  void initMapOfChange   ( );
  void initDirectories   ( );
  void createIconStars   ( );
  bool isValid           ( Thumbs * );
  //bool isValid           ( Q3IconViewItem * );    //ooo
  bool isValid           ( QListWidgetItem * );     //xxx
  //bool removeThumb       ( Q3IconViewItem * );    //ooo
  bool removeThumb       ( QListWidgetItem * );     //xxx
  //Thumbs *findThumb      ( Q3IconViewItem * );    //ooo
  Thumbs *findThumb      ( QListWidgetItem * );     //xxx
  //void addPlayButton     ( Q3IconViewItem *, QPixmap * ); //ooo
  void addPlayButton     ( QListWidgetItem *, QPixmap * );  //xxx
  void recreateThumbs    ( Thumbs  * );
  void createPix         ( Thumbs  *, uint );
  void createPix         ( QPixmap *, QImage *, float f=0.0f, int i=-1 );
  //void markIcon          ( Q3IconViewItem *, int );       //ooo
  void markIcon          ( QListWidgetItem *, int );        //xxx

  void setTreeDir        ( QString );
  void setQuickDir       (     int );
  void setCurrentDir     ( QString );
  QString currentDir     ( );

  void setVirtualFolder  ( int );
  void toSourceGroup     ( int ); //oooo
  void toVirtualFolder   ( int );
  void registerRequest   ( MultiThumbRequest * );
  void addClone          ( MultiThumbRequest * );
  void append            ( Thumbs  * );
  void clearPreview      ( );
  void updateTitlebar    ( );
  void resizeIcons       ( int );
  Thumbs *createFromCache( Cache::Thumbs::Entry * );
  QPixmap &createMenuHeader   ( QString,  QSize   );
  void currentSourceToPreview ( );
  void allSourcesToPreview    ( );
  void stopThumbing           ( );
  //void play                ( Cache::Thumbs::Entry *, Q3IconViewItem *p=NULL );    //ooo
  void play                ( Cache::Thumbs::Entry *, QListWidgetItem *p=NULL );     //xxx
  void lock                ( );
  void unlock              ( );
  bool lockDialog          ( );
  void unlockDialog        ( );
  bool isImageDialog       ( );
  bool isImageDialog       ( QString );
  bool sortIcons           ( DialogFiles::Thumbs *, DialogFiles::Thumbs *, int );
  void applyManualChange   ( SourceFileEntry  * );
  void mergeAudioTracks    ( QList<Audio *> &, SourceFileInfo * );		//oxx
  void mergeSubtitleTracks ( QList<SubtitleEntry *> &, SourceFileInfo * );	//oxx

  //QList<Q3ListViewItem *> getSelectedVirtual ( );				//oxx
  QList<QTreeWidgetItem *> getSelectedVirtual ( );				//xxx

  // Virtual functions
  virtual void timerEvent          ( QTimerEvent   * );
  virtual void load                ( QStringList   & );
  virtual void setIconText         ( Thumbs        * );
  //virtual void setIconText         ( Q3IconViewItem * );      //ooo
  virtual void setIconText         ( QListWidgetItem * );       //xxx
  //virtual void newThumbnailRequest ( Q3IconViewItem * );      //ooo
  virtual void newThumbnailRequest ( QListWidgetItem * );       //xxx
  virtual void showBasicMenu       ( const QPoint &  );
  //virtual void showContextMenu     ( const QPoint &, Q3IconViewItem * );  //ooo
  virtual void showContextMenu     ( QListWidgetItem * );   //xxx
  //virtual void contextMenuEvent     ( QContextMenuEvent * );   //xxx
  virtual GroupView *addGroup      ( SourceFileEntry * );
  virtual QString getDirFilter     ( );

protected slots:
  //virtual void slotToVirtualFolder       ( Q3ListBoxItem  * );    //ooo
  virtual void slotToVirtualFolder       ( QTreeWidgetItem  * );    //xxx
  //virtual void slotToSourceGroup         ( Q3ListBoxItem  * );    //ooo
  //virtual void slotToSourceGroup         ( QTreeWidgetItem  * );    //xxx
  virtual void slotToSourceGroup         ( QListWidgetItem  * );    //xxx
  //virtual void slotAllToVirtualFolder    ( Q3ListBoxItem  * );    //ooo
  virtual void slotAllToVirtualFolder    ( QTreeWidgetItem  * );    //xxx
  //virtual void slotAllToSourceGroup      ( Q3ListBoxItem  * );    //ooo
  //virtual void slotAllToSourceGroup      ( QTreeWidgetItem  * );    //xxx
  virtual void slotAllToSourceGroup      ( QListWidgetItem  * );    //xxx
  //virtual void slotSourceToVirtualFolder ( Q3ListBoxItem  * );    //ooo
  virtual void slotSourceToVirtualFolder ( QTreeWidgetItem  * );    //xxx
  //virtual void slotSourceToSourceGroup   ( Q3ListBoxItem  * );    //ooo
  //virtual void slotSourceToSourceGroup   ( QTreeWidgetItem  * );    //xxx
  virtual void slotSourceToSourceGroup   ( QListWidgetItem  * );    //xxx
  //virtual void slotDirectoryChanged      ( Q3ListViewItem * );    //ooo
  virtual void slotDirectoryChanged      ( QTreeWidgetItem *, int );     //xxx
  //virtual void slotGroupContextMenu      ( Q3ListViewItem *, const QPoint &, int );   //ooo
  virtual void slotGroupContextMenu      ( QListWidgetItem * );    //xxx
  //virtual void slotPreviewClicked        ( int, Q3IconViewItem *, const QPoint & );   //ooo
  virtual void slotPreviewClicked        ( QListWidgetItem *, int );    //xxx
  //virtual void slotPreviewPressed        (      Q3IconViewItem *, const QPoint & );   //ooo
  virtual void slotPreviewPressed        ( QListWidgetItem * );    //xxx
  //virtual void slotDroppedOnPreview      ( QDropEvent *, const QList<Q3IconDragItem> & );	//oooo
  virtual void slotMultiThumbResponse    ( );
  virtual void slotVirtualChanged        ( );
  virtual void slotIconSelectionChanged  ( );
  virtual void slotIconSelectionChanged  ( QTreeWidgetItem *, int );    //xxx
  virtual void slotAutoOpen              ( );
  virtual void slotQuickDir              ( );
  virtual void slotScanDir               ( );
  virtual void slotThumbing              ( );
  virtual void slotSelectByStars         ( );
  virtual void slotSizeReleased          ( );
  virtual void slotSizeChanged           ( int );
  virtual void slotAudioSubtitle         ( int );
  virtual void slotSortBy                ( int );
  virtual void slotRegisterRequests      ( );
  virtual void slotAddGroup              ( );
  virtual void slotEditGroup             ( );
  virtual void slotDeleteGroup           ( );
  virtual void slotAddVirtual            ( );
  virtual void slotEditVirtual           ( );
  virtual void slotCloneVirtual          ( );
  virtual void slotDeleteVirtual         ( );
  virtual void slotDeleteVFDialog        ( );
  virtual void slotPlay                  ( );
  virtual void accept                    ( ); //ooo
  //virtual void slotAccept                ( );   //xxx
  virtual void reject                    ( );
  virtual void slotResizeScrollView      ( );
  virtual void slotAutoGroup             ( );
  virtual void slotReload                ( );
  virtual void slotRefresh               ( );
  virtual void slotSetProgress           ( );
  virtual void slotAttrib                ( bool );
  //virtual void slotShowVirtual           ( Q3ListViewItem *, const QPoint &, int );   //ooo
  virtual void slotShowVirtual           ( QTreeWidgetItem *, const QPoint &, int );    //xxx
  //virtual void slotVirtualContextMenu    ( Q3ListViewItem *, const QPoint &, int );   //ooo
  virtual void slotVirtualContextMenu    ( QTreeWidgetItem *, const QPoint &, int );    //xxx

protected:
  static bool m_bStars;
  static bool m_bName;
  static bool m_bDate;
  static bool m_bLength;
  static int  m_iThumbnailSize;
  static QMap<unsigned long long, Manual *> m_mapOfManualChange; // keep manual changes around for the duration of ths session
  bool        m_bTooSmallForText;    // less then 50x50 which is too small to display text
  bool        m_bExitScannFunction;  // For the ImageDialog to exit slotCheckWorker

  QTimer     *m_pTimerAutoOpen;
  QTimer     *m_pTimerScanDir;
  QTimer     *m_pTimerThumbing;
  //Q3ListBox   *m_pVirtualFolderDialog;    //ooo
  //QTreeWidget   *m_pVirtualFolderDialog;    //xxx
  QListWidget   *m_pVirtualFolderDialog;    //xxx
  Previewer  *m_pPreviewer;
  //Q3PopupMenu *m_pContextMenu;    //ooo
  QMenu *m_pContextMenu;            //xxx

  QString   m_qsProjectDir;
  QString   m_qsDocumentsDir;
  QString   m_qsDesktopDir;
  QString   m_qsHomeDir;
  QString   m_qsRootDir;
  QString   m_qsTempDir;
  QString   m_qsCurrentDir;
  QPushButton *m_pActiveQuickButton;

  QPixmap   m_pixMenuStars[6];
  QPixmap   m_pixIconStars[7];

  bool      m_bCanClose;
  bool      m_bCanChangeIcons;
  bool      m_bIsImageDialog;   // Dialog can either be Image or Video dialog.
  uint      m_iProgress;
  uint      m_iMultiThumbNumber;
  bool      m_bSortAscending;
  int       m_iSortType;
  uint      m_iVidCounter;

  QMutex    m_mutex;
  QMutex    m_mutexDialog;
  QMutex    m_mutexRequest;

  static DialogFiles *m_pIAmAlive;

  QList<ScanDVDate  *>       m_listOfDVScansToDo;	//oxx
  QList<WorkWaiting *>       m_listOfWorkToDo;		//oxx
  QList<MultiThumbRequest *> m_listOfRequests;		//oxx

  Thumbs *m_pActiveThumbs;
  QList<Thumbs *>            m_listOfThumbs;		//oxx
  QList<SourceFileEntry *>   m_listOfGroups;		//oxx
  QList<MultiThumbRequest *> m_listOfClones;		//oxx

  friend class GroupView;

  // Create the ScrollView or the Groups
  //Q3ScrollView *m_pScrollView;    //ooo
  QScrollArea *m_pScrollView;       //xxx
  QToolBox    *m_pToolbox;
};

}; // end namespace Input

#endif // __DIALOGFILES_H__

