/***************************************************************************
    sourcetoolbar.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will create a toolbar, which displays all currently selected
   sources for the DVD, such as all Videos, and all Audio files.

   Besides displaying these it will also do some background checks.
   For this it will spawn a new thread, which will analyse the file
   and see if it needs to get converted for a propper usage.

   tcscan is your friend.
 
   The result will be color coded.
   o	Gray, not yet determined
   o	any shades of green -> In DVD format.
   o	any shades of blue  -> will be converted before creating DVD
   o	any shades of red   -> Can not automatically handle this format.
    
****************************************************************************/

#ifndef SOURCETOOLBAR_H
#define SOURCETOOLBAR_H

#include <QDockWidget>

// The following two defines specify which column the file name is to be found. 
#define MOVIE_FILE_NAME_INDEX 7
#define AUDIO_FILE_NAME_INDEX 7

#include <QMutex>
#include <QThread>
#include <QTimerEvent>
#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
//#include <QMouseEvent>		//xxx

#include "sourcewidget.h"
#include "sourcefileentry.h"	//ooo

class MediaInfo;
//class SourceFileInfo		//xxx
class ListViewItemMedia;

class SleeperThread7 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

class SourceToolBar : public QDockWidget
{
   Q_OBJECT

public:
   SourceToolBar     ( const char *, QMainWindow * );
   ~SourceToolBar     ( );

   void addMovie      ( SourceFileEntry * );
   void addMovieList  ( QStringList &, QString *p=NULL );
   void addSound      ( QStringList & );
   void clear         ( );
   
   int  sourceFileCount                       ( );
   int  getFreeSourceSlot                     ( );
   SourceFileEntry *sourceFileEntry           ( uint );
   SourceFileEntry *sourceFileEntry           ( CXmlSlideshow *);
   SourceFileEntry *entryFromSlideshow        ( CXmlSlideshow *);
   SourceFileEntry *getCurrentSourceFileEntry ( );
   SourceFileInfo  *getCurrentSourceFileInfo  ( );
   void slideshowDone                         ( SourceFileEntry * );
   bool sourceFileEntryExists                 ( SourceFileEntry * );
   void editSourceFileInfo                    ( SourceFileInfo  * );
   bool updateListViewItem                    ( SourceFileInfo  * );
   bool checkRightAudioFormat                 ( SourceFileInfo  * );
   bool checkRightFileFormat                  ( SourceFileInfo  * );
   void createContextMenu                     ( SourceFileEntry *, QPoint & );
   void appendSourceFileEntry                 ( SourceFileEntry *, bool b=false );
   bool setRenderProgress                     ( SourceFileEntry *, float );
   void removeEntry                           ( SourceFileEntry *, bool b=true  );
   void removeEntry                           ( QString &, bool  b=true  );
   //QListView   *getStructureListView          ( );	//ooo
   QTreeWidget   *getStructureListView          ( );	//xxx
   MediaInfo *getMediaInfo                    ( );
   
protected:
  virtual void timerEvent       ( QTimerEvent     * );
  ListViewItemMedia *getItem    ( SourceFileEntry * );
   
private:
  void  refreshToolBarEntries   ( );
  void *getMediaEngine          ( );
  void  updateEntryHeader       ( SourceFileEntry * );
  void  updateSlideshowItem     ( SourceFileEntry * );
  void  setAudioTranscode       ( SourceFileInfo  * );
   
public slots:
  virtual void slotDoubleClickedOnVideoList ( QTreeWidgetItem *, int );
  virtual void slotRightClickedOnVideoList ( QTreeWidgetItem *, int );
  virtual void slotAddVideo                ( );		//xxx
  virtual void slotClickedOnVideoList      ( QTreeWidgetItem *, int );
  virtual void slotPlayMovie       ( );
  virtual void slotEditMovie       ( );
  virtual void slotRerenderSlide   ( );
  virtual void slotEditAudio       ( );
  virtual void slotDeleteSource    ( );
  virtual void slotTranscodeAll    ( );
  virtual void slotAddMovieOld     ( );
  virtual void slotAddMovie        ( );
  virtual void slotAddSound        ( );
  virtual void slotAddSlideshow    ( );
  virtual void slotSetThumbnail    ( long );
   
private:
   SourceWidget      *m_pSourceWidget;
   QMutex             m_mutexThumbnails;
   MediaInfo         *m_pMediaInfo;
   //Q3ListView         *m_pListViewAllSources;		//ooo
   QTreeWidget         *m_pListViewAllSources;		//xxx
   QList<SourceFileEntry *> m_listSourceFileEntries;
   SourceFileEntry   *m_pContexMenuEntry;
   SourceFileInfo    *m_pContexMenuInfo;
   
   QListWidget *customerList;	//xxx

protected:
  //void mousePressEvent	   	( QMouseEvent * );	//xxx
};

#endif // SOURCETOOLBAR_H
