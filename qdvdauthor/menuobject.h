/***************************************************************************
    menuobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  This is the base class for all that is to appear onto the MenuPreview.
  Exception are background image, and the button masks (if the user
  specifies them rather then create the button manually)

  The MenuPreview class is the QLabel derived class where all buttons, text
  etc is to be plaeced for a DVD-Menu.

  The MenuObject is the base class for TextObject, ImageObject, MovieObject,
  FrameObject, and ButtonObject.

  The ButtonObject in turn is a container class which can hold a
  TextObject, ImageObject, MovieObject, AND a FrameObject.
  Please refer to the buttonobject.h - file for more detail on the
  ButtonObject.
    
****************************************************************************/

#ifndef MENUOBJECT_H
#define MENUOBJECT_H

#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QMouseEvent>

#include "rgba.h"
#include "modifiers.h"
#include "animationattribute.h"

class StructureItem;
class DialogMatrix;

#define HANDLE_WIDTH  9
#define HANDLE_HEIGHT 9

class MenuObject : public QObject
{
  Q_OBJECT
  friend class DVDMenuUndoObject;
//protected:    //ooo
public:         //xxx
  class Data
  {
  public:
    enum { DATA=0, IMAGE, TEXT, FRAME, MOVIE, BUTTON };
             Data ( );
    virtual ~Data ( );
    virtual  Data &operator  = ( Data & );
    virtual  bool  operator == ( Data & );
    virtual  Data *clone ( );

    int            m_iDataType; // is this for MenuObjects or ImageObjects or ...
    QString        m_qsName;
    QRect          m_rect;
    Modifiers      m_modifiers;
    QString        m_qsAnimation; // script or dataset holds also the datafilename
  };
public:
  enum enState {
    StateUnknown = 0,
    StateInactive,     // No object is currently active
    StateMoveObject,   // the object is being moved
    StateScaleTLXY,    // the object is being scaled equally TopLeft
    StateScaleTRXY,    // the object is being scaled equally TopRight
    StateScaleBLXY,    // the object is being scaled equally BottomLeft
    StateScaleBRXY,    // the object is being scaled equally BottmRight
    StateScaleLeftX,   // the object is only scaled in X direction
    StateScaleRightX,  // the object is only scaled in X direction
    StateScaleTopY,    // the object is only scaled in Y direction
    StateScaleBottomY, // the object is only scaled in Y direction
    StateRotate
  };
  
  MenuObject (QWidget *pParent);
  virtual ~MenuObject();
  
  virtual void drawActiveFrame      ( QPainter    * );
  virtual void drawContents         ( QPainter    * ) = 0;
  virtual void drawContents         ( QPainter    *, int, int) = 0;
  //virtual void drawContents         ( QPainter    *, QPixmap *) = 0;    //xxx
  virtual bool mouseMoveEvent       ( QMouseEvent * );
  virtual bool mousePressEvent      ( QMouseEvent * ) = 0;
  virtual bool mouseReleaseEvent    ( QMouseEvent * ) = 0;
  
  virtual bool readProjectFile      ( QDomNode     & );
  virtual bool writeProjectFile     ( QDomElement  & );
  virtual bool contains             ( QPoint  & );
  virtual bool createContextMenu    ( QPoint    ) = 0;
  virtual StructureItem *createStructure ( StructureItem * ) = 0;
  virtual MenuObject *clone         ( QWidget *p1=NULL, MenuObject *p2=NULL );
  virtual bool isActive             ( );
  virtual void setActive            ( bool );
  virtual void replaceColor         ( Rgba, Rgba ) { };
  virtual void updateShadow         ( );
  virtual void setShadow            ( MenuObject * );
  virtual void updateOverlay        ( );
  virtual void setOverlay           ( MenuObject * );
  virtual Data *data                ( );
  virtual void  setData             ( Data * );
  
  virtual void setModifiers   ( Modifiers &     );
  virtual void setRect        ( QRect &         );
  virtual QRect &rect         ( );
  virtual QRect &boundingRect ( );
  void      update            (                 );
  void      setName           ( QString &       );
  void      setExtractionPath ( QString &       );
  void      setAnimation      ( QString &       );
  void      setParent         ( QWidget *       );
  void      setPosition       ( QPoint  &       );
  void      setCurrentMousePos( QPoint  &       );
  void      animationDialog   ( QString &, int  );
  void      loadAnimationData ( QString &, long );
  //enState   setObjectState    ( QPoint  &, Qt::ButtonState );		//oooo
  enState   setObjectState    ( QPoint  &, Qt::MouseButton );		//xxx
  void      setObjectState    ( enState         );
  QPixmap  the_Pixmap; //xxx
  
  virtual Modifiers *modifiers( );
  virtual MenuObject *shadow  ( );
  virtual MenuObject *overlay ( );
  virtual void resetStructureItem ( );
  virtual QString &extractionPath ( );  // Used to store a unique path name for this object to be extracted into (movieObject)
  QString   &objectType       ( );
  QString   &name             ( );
  QString   &animation        ( );
  QPoint     position         ( );
  QWidget   *parent           ( );
  QPoint    &currentMousePos  ( );
  
protected:
  // This function will return Object specific attributes for Animation purposes
  virtual AnimationAttribute *getSpecificAttributes (long, QString) = 0;
  
public slots:
  virtual void slotDelete       ( );
  virtual void slotEmitDelete   ( );
  virtual void slotAddShadow    ( );
  virtual void slotAddOverlay   ( );
  virtual void slotDeleteMtxDlg ( );

signals:
  void signalMoveOnStack      ( MenuObject *, int );
  void signalDefineAsButton   ( MenuObject * );
  void signalDeleteMe         ( MenuObject * );  // used to delete the object
  void signalUndoMe           ( MenuObject * );  // used for object to be put onto the UndoStack
  void signalShadowMe         ( MenuObject * );
  void signalOverlayMe        ( MenuObject * );
  void signalAnimateMe        ( QString &, int, QString );
  void signalUpdateStructure  ( );
  void signalUpdatePixmap     ( );
  // Only used by Text, Frame, Button and only received if in ButtonDialog.
  void signalMaskColorChanged ( Rgba, Rgba );
  
protected:
  Data          *m_pData;
  DialogMatrix  *m_pMatrixDialog;
  QMenu         *m_pContextMenu;
  QString        m_qsObjectType;
  QString        m_qsExtractionPath;
  QRect          m_boundingRect;
  QWidget       *m_pParent;
  bool           m_bDrawRect; // isActive, draw ActiveFrame (resize, rotate etc)
  MenuObject    *m_pShadow;
  MenuObject    *m_pOverlay;
  StructureItem *m_pStructureItem;
  QPoint         m_currentMousePos;
  enState        m_objectState;
  QList<AnimationAttribute *> m_listAnimationAttributes;
};

#endif  // MENUOBJECT_H
