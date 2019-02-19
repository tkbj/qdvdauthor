/***************************************************************************
    selectionobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   SelectionObject is being used in MenuPreview for multiple selections 
    
****************************************************************************/

#ifndef SELECTIONOBJECT_H
#define SELECTIONOBJECT_H

#include "objectcollection.h"

class ShadowObject;

class SelectionObject : public ObjectCollection
{
  Q_OBJECT

public:
  // Public functions needed for the abstract base class.
  SelectionObject         ( QWidget *pParent=0 );
  virtual ~SelectionObject                   ( );
  
  void resetObjectStates                     ( );
  bool mouseOver             ( const QPoint  & );
  void setMoveState                     ( bool );
  void setCurrentMousePos           ( QPoint & );
  virtual bool mousePressEvent ( QMouseEvent * );
  virtual bool createContextMenu      ( QPoint );
  
  virtual void setActive        ( bool );
  virtual void drawContents     ( QPainter   * );
  virtual void drawSelected     ( QPainter   * );
  
  void clearShadows   ( );
  void drawShadows    ( QPainter * );
  void updateShadows  ( ShadowObject *, bool );
  
private:
  QList<ShadowObject *> m_listOfShadows;	//oxx
};

#endif	// SELECTIONOBJECT_H

