/***************************************************************************
    dvdlayoutgui.h
                             -------------------
    Class QSubtitles
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class displays the DVD Layout - tab of the main QDVDAuthor
   application
   
****************************************************************************
   
   The DVD Layout must handle
   o DVDMenuObjects
   o MovieObjects
   o FirstObject ( FPC )
   o PreObjects / PostObjects
    
****************************************************************************/

#ifndef DVDLAYOUTGUI_H
#define DVDLAYOUTGUI_H

#include <qlabel.h>
#include <qimage.h>
//Added by qt3to4:
//#include <Q3GridLayout>   //ooo
#include <QGridLayout>      //xxx
#include <QEvent>
//#include <Q3HBoxLayout>   //ooo
#include <QHBoxLayout>      //xxx
#include <QMouseEvent>
//#include <Q3ValueList>	//oxx
//#include <Q3Frame>        //ooo
#include <QFrame>           //xxx
#include <QKeyEvent>

//#include "uidvdlayout.h"	//ooo
#include "ui_uidvdlayout.h"	//xxx

class DVDMenu;
class SourceFileEntry;
class QDomNode;
class QDomElement;

namespace DVDLayout
{

// Forward declaration of classes.
class Object;
class Area;

// Header used for the MenuObject, and MovieObject
class Header : public QLabel
{
  Q_OBJECT

public:
  //Header ( QWidget *, Object *, const char *p=0, Qt::WFlags f=0 );        //ooo
  Header ( QWidget *, Object *, const char *p=0, Qt::WindowFlags f=0 );     //xxx
  virtual ~Header ( );

  void displayText ( bool );
  void setText     ( const QString & );

signals:
  void signalRedrawAll ( );

protected:
  virtual void mouseMoveEvent        ( QMouseEvent * );
  virtual void mousePressEvent       ( QMouseEvent * );
  virtual void mouseReleaseEvent     ( QMouseEvent * );
  virtual void mouseDoubleClickEvent ( QMouseEvent * );

private:
  Object *m_pObject;
  QString m_qsText;
  QPoint  m_pointInitial;
  bool    m_bMousePressed;
};

// Base class or Objects in the DVD Layout tab
//class Object : public Q3Frame //ooo
class Object : public QFrame    //xxx
{
  Q_OBJECT

public:
  static const int BUTTON_SIZE;
  static const int INITIAL_SIZE;

public:
  typedef enum { TYPE_UNKNOWN=0, TYPE_MENU, TYPE_MOVIE } enType;

  //Object ( QWidget *, const char *p=0, Qt::WFlags f=0 );      //ooo
  Object ( QWidget *, const char *p=0, Qt::WindowFlags f=0 );   //xxx
  virtual ~Object ( );

  void    setThumbnail  ( ); // Scales depending on the zoomed factor
  void    setZoom (   float  );
  void    setSize ( int, int );
  QSize   size    ( );

  virtual enType  type          ( );
  virtual void    refresh       ( ) = 0;
  virtual void   *getObject     ( ) = 0; // DVDMenu, or SourceFileEntry
  virtual QString getObjectName ( ) = 0;
  virtual void    createThumb   ( ) = 0; // Creates from DVDMenu

  void  setActiveObject   ( Object * );
  void  moveActiveObject  ( );

signals:
  void signalRedrawAll ( );

protected:
  virtual void mouseMoveEvent    ( QMouseEvent * );
  virtual void mousePressEvent   ( QMouseEvent * );
  virtual void mouseReleaseEvent ( QMouseEvent * );
  virtual void keyPressEvent     ( QKeyEvent   * );
  virtual void enterEvent        ( QEvent      * );
  virtual void leaveEvent        ( QEvent      * );

protected slots:
  virtual void slotInfo      ( );
  virtual void slotRedrawAll ( );

// Private functions
private:
  void resizeHeader  ( );

// Private member variables
protected:
  Area  *m_pArea;
  QImage m_thumbnail;
  QSize  m_gridSize;
  QSize  m_objectSize;
  QPoint m_ptPosition;
  double m_fZoom;
  bool   m_bGridActivated;

  // To handle mouse click/move
  QPoint  m_pointInitial;
  bool    m_bMousePressed;

  // These are the widget that make out the PluginFunction appearance
  Header      *m_pHeader;
  QPushButton *m_pButtonInfo;
  QLabel      *m_pInsideWidget;
  //Q3Frame      *m_pInnerFrame;    //ooo
  QFrame      *m_pInnerFrame;       //xxx
  //Q3HBoxLayout *m_pMainLayout;    //ooo
  QHBoxLayout *m_pMainLayout;       //xxx

  //Q3GridLayout *m_pFrameLayout;   //ooo
  QGridLayout *m_pFrameLayout;      //xxx
  //Q3HBoxLayout *m_pHeaderLayout;  //ooo
  QHBoxLayout *m_pHeaderLayout;     //xxx
};

// Main container to depict a movie
class MovieObject : public Object
{
  Q_OBJECT

public:
  MovieObject ( QWidget *, SourceFileEntry * );
  virtual ~MovieObject ( );

  virtual enType  type          ( );
  virtual void    refresh       ( );
  virtual void   *getObject     ( ); // DVDMenu, or SourceFileEntry
  virtual QString getObjectName ( );
  virtual void    createThumb   ( ); // Creates from SourceFileEntry

  QImage drawMovieLines( QImage & );

private:
  SourceFileEntry *m_pSourceFileEntry;
};

// Main container to depict a DVD Menu
class MenuObject : public Object
{
  Q_OBJECT

public:
  MenuObject ( QWidget *, DVDMenu * );
  virtual ~MenuObject ( );

  virtual enType  type          ( );
  virtual void    refresh       ( );
  virtual void   *getObject     ( ); // DVDMenu, or SourceFileEntry
  virtual QString getObjectName ( );

  virtual void createThumb  ( ); // Creates from DVDMenu

private:
public:
  DVDMenu *m_pDVDMenu;
};

//class Gui : public uiDVDLayout		//ooo
class Gui : public QWidget, Ui::uiDVDLayout	//xxx
{
  Q_OBJECT

  static const int REFRESH_MENU;

public:
   //Gui ( QWidget *, const char *p=0, Qt::WFlags f=0 );	//ooo
   Gui ( QWidget *pParent );					//xxx
  ~Gui ( );

  void refresh ( );
  void clear   ( );

  void updateEntry  ( SourceFileEntry * );
  void refreshMenus ( );

  bool readProjectFile  ( QDomNode    & );
  bool writeProjectFile ( QDomElement & );

protected:
  Object *findObject  ( void *, bool );
  Object *findObject  ( void * );
  Object *checkEntry  ( SourceFileEntry * );
  void    createEntry ( SourceFileEntry * );
  Object *checkMenu   ( DVDMenu * );
  void    createMenu  ( DVDMenu * );

protected slots:
  virtual void slotRecreateThumb ( );
  virtual void slotPostRead      ( );
  virtual void slotGrid          ( );
  virtual void slotPrinter       ( );
  virtual void slotRefresh       ( );
  virtual void slotAdjustZoom    ( );
  virtual void slotZoomChanged   ( );
  virtual void slotZoomChanged   ( int );
  virtual void slotSliderMoved   ( int );
  virtual void slotRouterChanged ( int );

// Private functions
private:
  void   refresh    ( bool  );
  void   setZoom    ( float );
  QPoint initialPos ( bool  );

private:
  Area *m_pArea;
  int   m_iPreviousSliderValue;
  QList<Object *> m_listOfObjects;		//oxx
  QList<Object *> m_listOfMenus;		//oxx
  QList<Object *> m_listOfVideos;		//oxx

  QList<Object *> m_listRecreateThumbs;		//oxx
};

}; // End of namespace DVDLayout

#endif // DVDLAYOUTGUI_H
