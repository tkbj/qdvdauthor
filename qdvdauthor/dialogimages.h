/***************************************************************************
    dialogimages.h
                             -------------------
    DialogImages
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This dialog offers an easy way to create slideshows from a selection
    of images.

    This of course is helpfull if you have 100+ files
    
****************************************************************************/

#ifndef __DIALOGIMAGES_H__
#define __DIALOGIMAGES_H__

#include "dialogfiles.h"
//Added by qt3to4:
#include <QPixmap>
//#include <Q3ValueList>	//oxx
#include <QTimerEvent>
//#include <Q3PopupMenu>    //ooo
#include <QMenu>            //xxx
// Separate from global namspace because nested classes 
// are not supported by moc.

class QMatrix;
//class Q3PopupMenu;    //ooo
class QMenu;            //xxx
//class Q3IconViewItem; //ooo
class QTreeWidgetItem;  //xxx

class CXmlSlideshow;

namespace Input
{

class ImageScan;

class DialogImages : public DialogFiles
{
  Q_OBJECT

  class ImageEntry  {
  public:
     ImageEntry ( );
    ~ImageEntry ( );

    // Note: Can I remove most vars here and use the ones from the cache object ?
    QString   qsFileName; // name of the Image
    QDateTime    created; // Either file date or from exif info ( to be used as a subtitle ? )
    //Q3IconViewItem *pItem; // If the item exists  //ooo
    QListWidgetItem *pItem; // If the item exists   //xxx
    QString  qsDimension; // The width x height
    QString  qsImageInfo; // All info returned by jhead

    QString qsComment; // User provided commant ( to be used as subtitle )
    uint  iDurationMs; // Duration the image is visible in Ms
    // Placeholder for a filter 
    QPixmap thePixmap;  // Only set if required to be set ...
    Cache::Thumbs::ImageEntry *pCache;
  };
  class Worker : public QThread  {
  public:
    Worker ( DialogImages * );
    virtual ~Worker  ( );
    bool hasExited   ( );
  protected:
    virtual void run ( );

    DialogImages *pParent;
    bool bExited;
  };
public:
  //DialogImages ( QWidget *, bool b=true, const char *n=0, Qt::WFlags f=0 );	//ooo
  DialogImages ( QWidget *, bool b=true );					//xxx
  virtual ~DialogImages ( );

  QList<CXmlSlideshow *> &getSlideshows ( );		//oxx
  void initMe ( SourceFileEntry *p=NULL );

protected slots:
  virtual void slotEditGroup      ( );
  virtual void slotCheckWorker    ( );
  //virtual void slotPreviewClicked ( int, Q3IconViewItem *, const QPoint & );  //ooo
  virtual void slotPreviewClicked ( int, QTreeWidgetItem *, const QPoint & );   //xxx
  //virtual void slotPreview        ( Q3IconViewItem * );   //ooo
  virtual void slotPreview        ( QListWidgetItem * );    //xxx

  virtual void slotPlay       ( );   // overloaded from DIalogFiles. Calls slotView ( )
  virtual void slotView       ( );
  virtual void slotRemove     ( );
  virtual void slotRotate90   ( );
  virtual void slotRotate180  ( );
  virtual void slotRotate270  ( );
  virtual void slotRotateFree ( );

protected:
  void rotate  (  double );
  void clearSlideshows ( );
  void clearWorker     ( );

  bool rescueImageAttributes ( void *, CXmlSlideshow * );
  void populateFromGroup     ( CXmlSlideshow *, GroupView * );

  ImageScan *takeNextScanObject   ( ImageScan *p=NULL );
  virtual QString getDirFilter    ( );
  virtual void setIconText        ( Thumbs        * );
  //virtual void setIconText        ( Q3IconViewItem * );   //ooo
  virtual void setIconText        ( QListWidgetItem * );    //xxx
  //virtual void newThumbnailRequest( Q3IconViewItem * );   //ooo
  virtual void newThumbnailRequest( QListWidgetItem * );    //xxx
  virtual void showBasicMenu      ( const QPoint &  );
  //virtual void showContextMenu    ( const QPoint &, Q3IconViewItem * );   //ooo
  virtual void showContextMenu    ( const QPoint &, QTreeWidgetItem * );    //xxx
  virtual void playFromGroup      ( Cache::Thumbs::Entry * );
  virtual void initGroups         ( );
  virtual GroupView *addGroup     ( SourceFileEntry * );
  virtual void timerEvent         ( QTimerEvent * );
  virtual void accept             ( );

private:
  QList<CXmlSlideshow *> m_listOfSlideshows;		//oxx
  QList<ImageScan *>     m_listOfImageScansToDo;	//oxx
  QList<ImageScan *>     m_listOfImageScansDone;	//oxx
  QList<Worker *>        m_listOfWorker;		//oxx

  uint m_iMaxThreads;

  friend class ImageScan;
  friend class Worker;
};

class ImageScan
{
public:
  //ImageScan ( DialogImages *, Q3IconViewItem * ); //ooo
  ImageScan ( DialogImages *, QListWidgetItem * );  //xxx
  virtual ~ImageScan ( );

  bool hasExited ( bool b=false );
  void createPix ( );

private:
  bool bExited;
  DialogImages *pDialog;

public:
  DialogImages::ImageEntry *pEntry;
};

}; // end namespace Input

#endif // __DIALOGIMAGES_H__

