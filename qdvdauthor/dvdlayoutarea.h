/***************************************************************************
    dvdlayoutarea.h
                             -------------------
    Class DVDLayout::Area
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class handles the display area for the DVD Layout tab.
   
****************************************************************************   
   
   The DVD Layout must handle
   o DVDMenuObjects
   o MovieObjects
   o FirstObject ( FPC )
   o PreObjects / PostObjects
    
****************************************************************************/

#ifndef DVDLAYOUTAREA_H
#define DVDLAYOUTAREA_H

//#include <q3iconview.h>   //ooo
#include <QListView>       //xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QPixmap>
#include <QMouseEvent>

class QDomElement;
class QDomNode;

namespace DVDLayout
{

// Forward declaration of classes.
class Object;
class LinearRouter;
class SimplePainter;

// Finally we can create the main DVD Layout Area object
//class Area : public Q3IconView    //ooo
class Area : public QListView       //xxx
{
  Q_OBJECT

  public:
  typedef enum { SIDE_UNKNOWN=0, SIDE_BOTTOM, SIDE_LEFT, SIDE_RIGHT } enSide;
  class PieceOfLine  {
    /// PieceOfLine is a line stipple which defines the path
    /// from this connection to the target connection.
  public:
    PieceOfLine ( QPoint &, QPoint & );
    QPoint m_startPos;
    QPoint m_endPos;
  };

  private:
  class Container {
    public:
    /// Sub class to hold information about the connections
    class Connection  {
      /// Properties is a wrapper class to hold connection specific
      /// properties, from This Connecion, to the target connection.
      public:
      Connection  ( Container  *, Container  * );
      Connection  ( Connection & );
     ~Connection  ( );

      enSide     side   ( int );
      int        number ( int );
      Container *target ( );
      Container *source ( );
      void clearLines   ( );
      void setLines     ( QList<PieceOfLine *> );		//oxx
      QList<PieceOfLine *> &lines ( );				//oxx

    //protected:
      QRgb        m_color;
      enSide      m_enSide[2];  // 0 == local side, 1 == Target's side
      int         m_iConNr[2];  // 0 == local side, 1 == Target's side
      Container  *m_pTarget;    // Going OUT, stored in m_listTargets
      Container  *m_pSource;    // Coming IN, stored in m_listSources
      QList<PieceOfLine *> m_listLineStipples;		//oxx
    };
  public:
     Container     ( Object * );
    ~Container     ( );
    Object *object ( );

    bool readProjectFile  ( QDomNode   & );
    bool writeProjectFile ( QDomElement& );

    void drawConnections  ( QPainter   &, Object *p=NULL );
    void drawConnection   ( QPainter   &, Connection   * );
    int  getTargetIdx     ( Connection * );
    int  getSourceIdx     ( Connection * );

    QList<Container   *> &container   ( ); // temp storage access	//oxx
    QList<Connection  *> &targets     ( );				//oxx
    QList<Connection  *> &sources     ( );				//oxx
    QList<Connection  *>  side        ( enSide );			//oxx
    QList<Connection  *>  copy        ( );				//oxx

    QPoint      getPos    ( enSide, int  );
    void        addSource ( Connection * );
    Connection *addTarget ( Container  * );
    Connection *getTarget ( Container  * );
    void     clearTargets ( );
    void     clearSources ( );
    void clearConnections ( );

  protected:
    Object                *  m_pObject;       // Not owned, don't delete
    QList<Container  *> m_listContainer; // temp storage (not owned, don't delete)		//oxx
    QList<Connection *> m_listTargets;   // Going to other Container (owned objects)		//oxx
    QList<Connection *> m_listSources;   // Comming from other Connections (not owned)		//oxx
  };
  class ReadObject {
  public:
    ReadObject ( ) { };
    bool readProjectFile  ( QDomNode    & );
    bool writeProjectFile ( QDomElement & );

    // Temp storage of data from readProjectFile
    QString m_qsObjectName;
    QRect   m_rect;
  };

public:
  //Area ( QWidget *, const char *p=0, Qt::WFlags f=0 );        //ooo
  Area ( QWidget *, const char *p=0, Qt::WindowFlags f=0 );     //xxx
  virtual ~Area ( );

  void  refresh  ( );
  void  setGrid  ( bool,  int x=0, int y=0 );
  bool  getGrid  ( int &, int & );
  void  setZoom  ( float );
  float getZoom  ( );
  void  print    ( );
  void  drawConnections     ( );
  void  setRouterType       ( int );
  int   routerType          ( );
  void  createConnections   ( QList<Object *> & );	//oxx
  void  createConnections   ( );
  void  drawContainer       ( Object      * );
  void  deleteContainer     ( Object      * );
  void  createConnections   ( Container   * );
  bool  readProjectFile     ( QDomNode    & );
  bool  writeProjectFile    ( QDomElement & );
  LinearRouter  *getRouter  ( );
  SimplePainter *getPainter ( );

  // Handle moving Objects around.
  void  setActiveObject   ( Object      * );
  void  moveActiveObject  ( );

protected:
  QPoint m_prevPos;
  bool   m_bMouseDown;
  virtual void contentsMouseMoveEvent    ( QMouseEvent * );
  virtual void contentsMousePressEvent   ( QMouseEvent * );
  virtual void contentsMouseReleaseEvent ( QMouseEvent * );

protected slots:
  virtual void slotRedrawAll ( );

private:
  void moveAll        ( int,    int );
  void drawGrid       ( QPainter  & );
  void drawContainer  ( Container * );
  void drawContainer  ( QPainter  &, Object *p=NULL );
  void clearPixmap    ( );
  void clearContainer ( );
  void cleanListOfContainer ( );

  void setReadParams    ( Container * );
  void findMovieTargets ( Container * );
  void findMenuTargets  ( Container * );
  Container *findContainerFromAction ( QString & );
  Container *findContainerFromObject ( Object  * );
  Container *findContainerFromPtr    ( void    * );

private:
  Container *m_pActiveContainer;
  QList<Container::Connection *> m_listActiveLines;	//oxx
  LinearRouter  *m_pRouter;
  SimplePainter *m_pPainter;
  int            m_iRouterType; // needed for writeProjectFile
  bool           m_bGrid;
  float          m_fZoom;
  QSize          m_sizeGrid;
  QPixmap        m_pixmap;

  QList<Container  *> m_listOfContainer;	//oxx
  QList<ReadObject *> m_listOfReadObjects;	//oxx

  friend class NicePainter;
  friend class SimplePainter;
  friend class LinearRouter;
  friend class OthogonalRouter;
};

}; // End of namespace DVDLayout

#endif // DVDLAYOUTAREA_H
