/***************************************************************************
    buttonpreview.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   This class generates the command line for dvdauthor.
   Current Version of DVDAUTHOR used is 0.67
   You can find dvdauthor under http://dvdauthor.sourceforge.net/
    
****************************************************************************/

#ifndef BUTTONPREVIEW_H
#define BUTTONPREVIEW_H 

#include <QLabel>
#include <QPixmap>
#include <QList>
#include <QRect>

#include "menuobject.h"

class UndoBuffer;
class CDVDMenuInterface;

class ButtonPreview : public QLabel
{
  Q_OBJECT

public:
  enum enPointerShape {                     MousePointerCursor=Qt::ArrowCursor,
    MouseCrossCursor  =Qt::CrossCursor,     MouseMoveCursor   =Qt::SizeAllCursor,
    MouseXResizeCursor=Qt::SizeHorCursor,   MouseYResizeCursor=Qt::SizeVerCursor,
    MouseZoomCursor   =Qt::SizeFDiagCursor, MouseRotateCursor =Qt::PointingHandCursor,
    MouseLZoomCursor  =Qt::SizeFDiagCursor, MouseRZoomCursor  =Qt::SizeBDiagCursor,
  };
  
   //ButtonPreview (QWidget * parent, const char * name = 0, Qt::WindowFlags f = 0 );	//ooo
   ButtonPreview (QWidget * parent );							//xxx
  ~ButtonPreview ();
  
  ButtonPreview & operator =    ( ButtonPreview & );
  void         toTop            ( MenuObject * );
  UndoBuffer  *undoBuffer       ( );
  MenuObject  *childContains    ( QPoint & );
  MenuObject  *menuObject       ( uint );
  uint         objectCount      ( );
  void         updatePixmap     ( );
  bool         removeObject     ( MenuObject * );
  void         addMenuObject    ( MenuObject * );
  uint         clearObjectList  ( );
  QPixmap     &renderMenu       ( CDVDMenuInterface *, long, long );
  void fitDVDMenuBackground     ( QPixmap  *, int, int, QSize, int, int );
  void         setMouseCursor   ( enPointerShape );
  
  virtual void drawBackground   ( QPixmap &, MenuObject *p=NULL );
  virtual void setBackground    ( QPixmap & );
  void paint();     //xxx
  void paint2();     //xxx
  
public slots:
  virtual void slotDeleteObject ( MenuObject * );
  //virtual void setPixmap        ( const QPixmap & );  //ooo
  virtual void set_Pixmap        ( const QPixmap & );    //xxx
  virtual void slotMoveOnStack  ( MenuObject *, int );
  virtual void slotAnimateObject( QString &, int, QString );
  
signals:
  void signalRenderProgress     ( float );
  void signalAnimateObject      ( QString &, int, QString );

protected:
  virtual void mousePressEvent	     ( QMouseEvent * );
  virtual void mouseReleaseEvent     ( QMouseEvent * );
  virtual void setObjectCursor       ( MenuObject::enState );
  virtual void drawContents          ( QPainter    * );
  virtual void paintEvent            ( QPaintEvent * );       //xxx
  
private:
  void drawVisibleRegion             ( QPainter * );
  
protected:
  // Here we have the undo buffer ...
  UndoBuffer *m_pUndoBuffer;
  
  //Q3PtrList<MenuObject>   m_listMenuObjects;	//ooo
  QList<MenuObject*>   m_listMenuObjects;	//xxx
  QRect       m_rectViewport;  // This is the viewport set in drawContents()
  QPixmap     m_backgroundPixmap;
  MenuObject *m_pActiveObject;
  // tells the process to skip further handling of the MouseEvents since they were already taken care of.
  bool        m_bMouseEventDone;
  bool        m_bDrawVisibleRegion;
  int         m_iCursorStyle;
  //void paintEvent        (QPaintEvent *);       //xxx
  
private:    //xxx
   QPixmap m_doubleBuffer; 
};

#endif // BUTTONPREVIEW_H
