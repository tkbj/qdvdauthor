/***************************************************************************
    listviewitemmedia.h
                             -------------------
    ListViewItemMedia class to display the Video sources name, and thumbnail
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  This class re-implements the QListViewItem class and adds 
   1) a thumbnail preview of the video(s)
   2) a tree-node with a list of all included SourceFileInfo's
    
****************************************************************************/

#ifndef LISTVIEWITEMMEDIA_H
#define LISTVIEWITEMMEDIA_H

#include <QTreeWidgetItem>
#include <QTimer>   //xxx

#include "cachethumbs.h"

class ListViewItemMedia;
class SourceFileEntry;
class SourceFileInfo;
class CXmlSlideshow;
//class QTimer; //ooo

class AnimateMini : public QObject
{
  Q_OBJECT
  // This class is coupled with the CacheThumbs class
  // and will animate the thumbnail in the VideoList
public:
           AnimateMini ( ListViewItemMedia * );
  virtual ~AnimateMini ( );
  
  void initMe ( Cache::Thumbs::Entry * );
  void initMe ( CXmlSlideshow * );
  void refreshButtonState ( );
  void setProgress  ( float );
  void reset  ( );
  void clear  ( );
  void start  ( );
  void stop   ( );
  void info   ( );
  
protected slots:
  virtual void slotAnimate ( );
 
private:
  QTimer   *m_pTimerThumbing;
  QPixmap **m_arrayPix;
  QPixmap   m_pixmapOrig;
  int       m_iNumberOfPix;
  int       m_iCurrentPix;
  float     m_fProgress; // If a slideshow is in the makings.
  ListViewItemMedia *m_pItem;
};

class ListViewItemMedia : public QTreeWidgetItem	//xxx
{
  enum enButtonState {StateUndef = 0, StateStop, StatePlay, StateBack};
public:
  //ListViewItemMedia ( Q3ListView     *, SourceFileEntry *, const QColor & );	//ooo
  ListViewItemMedia ( QTreeWidget     *, SourceFileEntry *, const QColor & );	//xxx
  //ListViewItemMedia ( Q3ListViewItem *, SourceFileEntry *, const QColor & );	//ooo
  ListViewItemMedia ( QTreeWidgetItem *, SourceFileEntry *, const QColor & );	//xxx
  //ListViewItemMedia ( Q3ListView     *, Q3ListViewItem   *, SourceFileEntry *, const QColor & );	//ooo
  ListViewItemMedia ( QTreeWidget     *, QTreeWidgetItem   *, SourceFileEntry *, const QColor & );	//xxx
  //ListViewItemMedia ( Q3ListViewItem *, SourceFileInfo  *, const QColor &, QString, QSize );		//ooo
  ListViewItemMedia ( QTreeWidgetItem *, SourceFileInfo  *, const QColor &, QString, QSize );		//xxx
  virtual ~ListViewItemMedia       ( );
  
  SourceFileEntry *sourceFileEntry ( );
  SourceFileInfo  *sourceFileInfo  ( );
  QColor backgroundColor ( );
  void   replaceInfo     ( SourceFileInfo * );
  void   updateItem      ( );
  void   setProgress     ( float );
  void renderProgress    ( QPixmap *, float );  //xxx
  void attachButtons ( QImage *, QPixmap * );   //xxx
  void drawButtonState   ( QPixmap * );         //xxx
  
private:
  void initMe ( SourceFileEntry *, const QColor & );
  //void attachButtons ( QImage *, QPixmap * ); //ooo
  QPixmap &attachButtons ( QImage &, QSize );
  void miniButton        ( int,  QString & );
  void miniButton        ( int,  CXmlSlideshow * );
  //void renderProgress    ( QPixmap *, float );    //ooo
  //void drawButtonState   ( QPixmap * );           //ooo
  
private:
  uint m_iButtonState;
  uint m_iPreviousButtonState;
  AnimateMini *m_pAnimateMini;
  QPixmap     *m_pPixmap; // Used if we want to display progress in a Thumbnail
  
  QColor m_backgroundColor;
  QSize  m_sizeThumbnails;
  // Only one of the two possible data is valid
  SourceFileEntry *m_pSourceFileEntry;
  SourceFileInfo  *m_pSourceFileInfo;

};

#endif // LISTVIEWITEMMEDIA_H
