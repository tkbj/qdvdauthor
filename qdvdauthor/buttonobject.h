/***************************************************************************
    buttonobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0

   The ButtonObject is a MenuObject derived class, where the user can store
   all associated information (objects).

   A Button can consist of a Background (color), an Image or Movie, a Frame,
   and a Text - object.
   All these object can belon to only one button, stacked above each other.
   For this reason the ButtonObject is a container class for those Object.

   ButtonObject is also derived from the same base-class as the object it
   contains, simply to make the right handling of mouse events etc easier
   to handle. It also makes a lot of sense designing this structure because
   it makes the MenuPreview class and the MenuObject derived classes so much
   more flexible.



               [Text]
        /--------|------------/
       /         |           / <- [Frame]
      /  /-------V----------/--/
     /  /    Click Me      /  / 
    /  /                  /  /
   /---------------------/  /
     /                     / <- [Image or Movie]
    /---------------------/       [Background]


****************************************************************************/

#ifndef BUTTONOBJECT_H
#define BUTTONOBJECT_H

// The button state
#define STATE_NORMAL      0
#define STATE_SELECTED    1
#define STATE_HIGHLIGHTED 2

// Which way to go next ...
#define NEXT_BUTTON_UP    0
#define NEXT_BUTTON_DOWN  1
#define NEXT_BUTTON_RIGHT 2
#define NEXT_BUTTON_LEFT  3

#include <QMouseEvent>

#include "menuobject.h"

class SourceFileInfo;
class CXmlSlideshow;
class MenuPreview;
class DVDMenu;

class SourceFileEntry;

class ButtonTransition  {
  // Container to define the transition
  // from a menu to another menu
  // or
  // from a menu to a movie
  // or
  // from the end of a movie to a menu
public:
   ButtonTransition ( );
  ~ButtonTransition ( );
  
  ButtonTransition &operator  = ( const ButtonTransition & );
  bool              operator == ( const ButtonTransition & );
  
  CXmlSlideshow *createSlideshow ( );
  
  void initMe           ( );
  bool readProjectFile  ( QDomNode    & );
  bool writeProjectFile ( QDomElement & );
  
  bool    bUserFile;
  float   fDuration; // ( in seconds )
  QString qsName;    // <MenuName>/<ButtonName>
  QString qsTransitionVideoFileName;  // temp file name ( under $TEMP/<MenuName>/<ButtonName>.vob )
  int     iFormat;
  int     iTransitionType;     // Either cross_fade or alpha_fade.
  QString qsTransitionName;    // The full path to the alpha-transition file
  bool    bFinishedRendering;  // flags if rendering has finished.
  
};

class ButtonObject : public MenuObject
{
  Q_OBJECT

  class ButtonData : public MenuObject::Data
  {
  public:
             ButtonData ( );
    virtual ~ButtonData ( );
    virtual  Data &operator  = ( Data & );	//ooo
    virtual  bool  operator == ( Data & );	//ooo
    virtual  Data *clone ( );			//ooo
    //void reParentChildren             ( );	//ooo

    QString m_qsAction;     // the action to take ...
    QString m_qsPreAction;  // ';' separated commands before the jump - action.
    QString m_qsUp;
    QString m_qsDown;
    QString m_qsRight;
    QString m_qsLeft;
    ButtonTransition *m_pTransition;
    QString m_qsSourceDisplayName;
    // The following lists hold the information for the different button states.
    QList<MenuObject *> m_listNormalState;      // Image or Movie, Frame, Text, Background	//oxx
    QList<MenuObject *> m_listSelectedState;    // Frame, Text, Background			//oxx
    QList<MenuObject *> m_listHighlightedState; // Frame, Text, Background			//oxx

    QStringList              m_listPlayMultiple;     // If the user selected multiple files to be played
    bool                     m_bLoopMultiple;        // Loop after playing all files in the Multiple list.
  };
public:
  ButtonObject (QWidget *pParent = 0); //, const char * name = 0, WFlags f = 0 );
  virtual ~ButtonObject();
  virtual void drawContents         ( QPainter *          );
  virtual void drawContents         ( QPainter *, uint    );
  virtual void drawContents         ( QPainter *, int, int);
  virtual bool mousePressEvent      ( QMouseEvent *       );
  virtual bool mouseReleaseEvent    ( QMouseEvent *       );
  virtual bool mouseDoubleClickEvent( QMouseEvent *       );
  virtual MenuObject *clone         ( QWidget *p1=NULL, MenuObject *p2=NULL );
  virtual ButtonObject &operator =  ( ButtonObject &      );
  virtual void resetStructureItem   ( );
  virtual bool readProjectFile      ( QDomNode    &  );
  virtual bool writeProjectFile     ( QDomElement &  );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual bool createContextMenu    ( QPoint         );
  void setPlayMultipleList          ( QStringList &  );
  void setLoopMultiple              ( bool );
  void setTransition                ( ButtonTransition * );
  void reParentChildren             ( );
  
  void appendNormal                 ( MenuObject  *  );
  void appendSelected               ( MenuObject  *  );
  void appendHighlighted            ( MenuObject  *  );
  
  MenuObject *getNormal             ( uint );
  MenuObject *getSelected           ( uint );
  MenuObject *getHighlighted        ( uint );
  
  MenuObject *removeNormal          ( uint );
  
  bool removeNormal                 ( MenuObject * );
  bool removeSelected               ( MenuObject * );
  bool removeHighlighted            ( MenuObject * );
  
  uint getNormalCount               ( );
  uint getSelectedCount             ( );
  uint getHighlightedCount          ( );
  
  Rgba getMaskColor                 (       bool, int i=0 );
  bool setMaskColor                 ( Rgba, bool, int i=0 );
  QStringList &getPlayMultipleList  ( );
  bool         getLoopMultiple      ( );
  ButtonTransition *getTransition   ( );
  
  long getOffset                    ( );
  int  chapter                      ( bool );
  void setAction                    ( QString & );
  void setPreAction                 ( QString & );
  void setNext                      ( uint, QString & );
  QString &next                     ( uint );
  QString &action                   ( );
  QString &preAction                ( );
  
  DVDMenu         *dvdMenu          ( );
  SourceFileInfo  *sourceFileInfo   ( );
  SourceFileEntry *sourceFileEntry  ( );
  void setSourceFileEntry           ( SourceFileEntry *);
  
  void deleteMasks                  ( );
  bool readObjects                  ( QDomNode &, MenuPreview *);
  QString &sourceDisplayName        ( );
  
  void replaceColor                 ( Rgba, Rgba );
  
protected:
  void clear    ( );
  void reParent ( MenuObject * );
  ButtonData *buttonData ( );
  virtual AnimationAttribute *getSpecificAttributes ( long, QString );
  
signals:
  void signalUnbuttonMe             ( ButtonObject * );
  void signalCreateButtonDialog     ( ButtonObject * );  
  
public slots:
  virtual void slotEdit             ( );
  virtual void slotUnbutton         ( );
  virtual void slotEmitUnbuttonMe   ( );
  virtual void slotToText           ( );
   
protected:
  int     m_iButtonState;
  bool    m_bMoveable;
  SourceFileEntry  *m_pSourceFileEntry;
};

#endif // BUTTONOBJECT_H
