/***************************************************************************
    objectcollection.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   pure virtual class as base for the three button types.

****************************************************************************/

#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

#include "menuobject.h"

class ObjectCollection : public MenuObject
{
  Q_OBJECT
  class Collection
  {
  public:
     Collection ( MenuObject *, bool b=true );
    ~Collection ( );
    MenuObject *pObject;
    bool bDeleteMe;
  };

public:
  // Public functions needed for the abstract base class.
  ObjectCollection (QWidget *pParent = 0);
  virtual ~ObjectCollection();
  virtual void drawContents          ( QPainter *   );
  virtual void drawContents          ( QPainter *, int, int);
  virtual bool mousePressEvent       ( QMouseEvent * );
  virtual bool mouseReleaseEvent     ( QMouseEvent * );
  
  virtual bool createContextMenu     ( QPoint          );
  virtual StructureItem *createStructure ( StructureItem * );
  
public:
  int         getCount               ( );
  MenuObject *getObject              ( int );
  
  void append                        ( MenuObject *, bool b=true );
  void setObjects                    ( QList<MenuObject *> &, bool b=true );	//oxx
  bool contains                      ( MenuObject * );
  void clear                         ( );
  
protected:
  virtual AnimationAttribute *getSpecificAttributes (long, QString);
  QList<Collection *> m_listObjectCollection;
};

#endif	// OBJECTCOLLECTION_H

