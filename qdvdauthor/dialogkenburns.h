/****************************************************************************
** DialogAudio
**
**   Created : Tue Jul 13 12:09:08 2010
**        by : Varol Okan using vi
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#ifndef DIALOGKENBURNS_H
#define DIALOGKENBURNS_H

#include <qrect.h>
#include <qpoint.h>
#include <qlabel.h>
#include <qpixmap.h>
//#include <q3listview.h>   //ooo
#include <QTreeWidget>    //xxx
//#include <q3valuelist.h>	//oxx
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>

#include "cachethumbs.h"
#include "xml_slideshow.h"
//#include "uidialogkenburns.h"		//ooo
#include "ui_uidialogkenburns.h"	//xxx

// Forward declaration
class DialogKenBurns;

class PreviewMouseHandler : public QObject
{
public:
           PreviewMouseHandler ( DialogKenBurns * );
  virtual ~PreviewMouseHandler ( );

protected:
  bool eventFilter ( QObject *, QEvent * );

private:
  DialogKenBurns *m_pDialog;
};

//class DialogKenBurns : public uiDialogKenBurns			//ooo
class DialogKenBurns : public QDialog, public Ui::uiDialogKenBurns	//xxx
{
  Q_OBJECT

  //class Item : public Q3ListViewItem  //ooo
  class Item : public QTreeWidgetItem   //xxx
  {
  public:
    //Item ( Cache::Thumbs::Entry *, CXmlSlideshow::img_struct *, float, int, Q3ListView *, Item *, QString );  //ooo
    Item ( Cache::Thumbs::Entry *, CXmlSlideshow::img_struct *, float, int, QTreeWidget *, Item *, QString );   //xxx
    virtual ~Item ( );
    //virtual void paintCell ( QPainter *, const QColorGroup &, int, int, int );    //oooo

    void  update ( );
    QRect getCurrentRect ( double );

    bool  bAlternateColor;
    bool  bRecreateImage;
    float fDefaultDelay;
    QRect rect[2];  // rect[0]=StartRect, rect[1]=EndRect
    QPixmap                    pix;
    QPixmap                    sizedPix; // Scaled image without start / emd frames.
    Cache::Thumbs::Entry      *pCache;
    CXmlSlideshow::img_struct *pXmlImage;
    CXmlSlideshow::vid_struct *pXmlVideo;
  };
  class Undelete
  {
  public:
     Undelete ( uint, Cache::Thumbs::Entry *, CXmlSlideshow::img_struct * );
    ~Undelete ( );

    uint iPosition;
    Cache::Thumbs::Entry      *pCache;
    CXmlSlideshow::img_struct *pXmlImage;
  };

public:
   //DialogKenBurns ( QWidget *p=0, const char *name=0, bool modal=FALSE, Qt::WFlags fl=0 );	//ooo
   DialogKenBurns ( QWidget *pParent=0 );							//xxx
  ~DialogKenBurns ( );
  bool     initMe ( );

  void setSlideshow ( CXmlSlideshow * );
  CXmlSlideshow *getSlideshow ( );

  // These three functions are called from the EventFilter of m_pLabelPreview
  void previewMousePressed  ( QMouseEvent * );
  void previewMouseReleased ( QMouseEvent * );
  void previewMouseMove     ( QMouseEvent * );

private:
  void populate          ( );
  void updateText        ( );
  void updateImage       ( );
  void changeCaption     ( );
  void updateImageDyn    ( );
  void reindexItems      ( );
  void setImage          ( QImage & );
  void drawArrow         ( QPainter *, int, int, int, int );
  uint findCorner        ( QRect &, QPoint & );
  uint findEdge          ( QRect &, QPoint & );
  void moveRect          ( QRect &, QPoint & );
  void moveCorner        ( QRect &, QPoint & );
  void updateKenBurns    ( bool,  bool );
  void createPix         ( Item * );
  void adjustWidthHeight ( Item *, QImage & );
  // Context menu functions
  void deleteCurrentItem (        );
  void unDeleteItem      (        );
  void moveCurrentItem   (   bool );
  void rotate            ( double );

protected slots:
  virtual void accept        ( );
  virtual void reject        ( );
  virtual void slotAnimate   ( );
  virtual void slotAnimation ( );
  virtual void slotDuration  ( );
  virtual void slotRandom    ( );
  virtual void slotHide      ( );
  virtual void slotSlider          (  int );
  virtual void slotToggleKenBurns  ( bool );
  virtual void slotDisplayImage    ( );
  virtual void slotAlternateColors ( );
  //virtual void slotCurrentChanged  ( Q3ListViewItem * );  //ooo
  virtual void slotCurrentChanged  ( QTreeWidgetItem * );   //xxx
  //virtual void slotContextMenu     ( Q3ListViewItem *, const QPoint &, int ); //ooo
  virtual void slotContextMenu     ( QTreeWidgetItem *, const QPoint &, int );  //xxx

private:
  enum { NoCorner=0x0000,
         StartTopLeftCorner=0x0001, StartTopRightCorner=0x0002, StartBottomLeftCorner=0x0004, StartBottomRightCorner=0x0008,
         StopTopLeftCorner =0x0010, StopTopRightCorner =0x0020, StopBottomLeftCorner =0x0040, StopBottomRightCorner =0x0080,
         StartLeftEdge     =0x0100, StartTopEdge       =0x0200, StartRightEdge       =0x0400, StartBottomEdge       =0x0800,
         StopLeftEdge      =0x1000, StopTopEdge        =0x2000, StopRightEdge        =0x4000, StopBottomEdge        =0x8000 };

  int    m_iThumbSize;
  bool   m_bMousePressed;
  bool   m_bFirstQuestion;
  uint   m_iCorner;
  int    m_iAnimation;
  QPoint m_currentMousePos;

  Item          *m_pActiveItem;
  Item          *m_pChangedItem; // While Animation is ongoing.
  //Q3ListViewItem *m_pNextItem;    //ooo
  QTreeWidgetItem *m_pNextItem;     //xxx
  CXmlSlideshow *m_pSlideshow;
  CXmlSlideshow *m_pOrigSlideshow;
  PreviewMouseHandler  *m_pEventFilter;
  QList<QPixmap>   m_listAnimation;	//oxx
  QList<Undelete *>m_listUndelete;	//oxx
};

#endif // DIALOGKENBURNS_H

