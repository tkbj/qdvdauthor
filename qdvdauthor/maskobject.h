/***************************************************************************
    maskobject.h
                             -------------------
    Shadow object
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class will create a buttonmask for the associated MenuObject.
    Thus far masks were only Rectangles. This class will create a mask
    according to the associated imageObject ( similar to a ShadowObject)
    
****************************************************************************/

#ifndef MASKOBJECT_H
#define MASKOBJECT_H

#include <qimage.h>
//Added by qt3to4:
#include <QMouseEvent>
//#include <Q3ValueList>	//oxx

#include "menuobject.h"

class MaskObject : public MenuObject
{
  Q_OBJECT

  friend class SelectionObject;

public:
  MaskObject ( QWidget *p=NULL);
  MaskObject ( MenuObject *, QWidget *p=NULL );
  virtual ~MaskObject ( );

  virtual void drawContents         ( QPainter      * );
  virtual void drawMask             ( QPainter      * );

  virtual bool readProjectFile      ( QDomNode      & );
  virtual bool writeProjectFile     ( QDomElement   & );
  virtual bool createContextMenu    ( QPoint          );
  virtual void replaceColor         ( Rgba,      Rgba );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual MenuObject    *clone      ( QWidget *p=NULL,  MenuObject *p2=NULL );

  virtual void   updateMask          ( );
  virtual QRect &boundingRect        ( );

  MaskObject &operator =          ( MaskObject & );
  bool operator       ==          ( MaskObject & );

  bool filterColorKeys  ( QImage      & );
  void setParent        ( MenuObject * );

  void setColor         ( Rgba  );
  Rgba    &color        ( );

protected slots:
  virtual void slotChangeColor ( );

private:
  bool filterColorKey ( QColor *, int, QList<QColor *> *, QList<QColor *>* );	//oxx
  void createMask     ( );
  //void filterColor    ( QImage &, QColor );

  virtual void drawContents   ( QPainter*, int, int ) { };
  virtual bool mousePressEvent      ( QMouseEvent * );
  virtual bool mouseReleaseEvent    ( QMouseEvent * ) { return false; };
  virtual bool mouseDoubleClickEvent( QMouseEvent * ) { return false; };
  virtual AnimationAttribute *getSpecificAttributes(long int, QString){ return NULL; };

  MenuObject  *m_pMenuObject;  // the object which has the mask ( Currently only IMageObject )
  QImage       m_imageMask;    // the mask is stored as a QImage - object ( 2 colors )

  // Mask attributes
  Rgba  m_color;
};

#endif // MASKOBJECT_H
