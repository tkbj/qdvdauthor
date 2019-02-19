/***************************************************************************
    imageobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  pure virtual class as base for the three button types.
    
****************************************************************************/

#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include "menuobject.h"

class ImageObject : public MenuObject
{
  Q_OBJECT

protected:
  class ImageData : public MenuObject::Data
  {
  public:
             ImageData ( );
    virtual ~ImageData ( );
    virtual  Data &operator  = ( Data & );
    virtual  bool  operator == ( Data & );
    virtual  Data *clone ( );
    
    bool             m_bHidden;
    ImageManipulator m_imageManipulator;
  };
public:
  // Public functions needed for the abstract base class.
  ImageObject (QWidget *pParent = 0);
  virtual ~ImageObject();
  virtual void drawContents          ( QPainter    *  );
  virtual void drawContents          ( QPainter    *, int, int);
  virtual bool mousePressEvent       ( QMouseEvent *  );
  virtual bool mouseReleaseEvent     ( QMouseEvent *  );
  virtual bool mouseMoveEvent        ( QMouseEvent *  );
  virtual bool mouseDoubleClickEvent ( QMouseEvent *  );
  
  virtual bool readProjectFile       ( QDomNode &     );
  virtual bool writeProjectFile      ( QDomElement &  );
  virtual bool createContextMenu     ( QPoint         );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual MenuObject    *clone       ( QWidget *p1=NULL, MenuObject *p2=NULL );
  
public:
  // Further public functions not dervied from the base class.
  virtual void setImage          ( QImage  & );
  virtual void updatePixmap      ( );
  virtual void setManipulator    ( ImageManipulator & );
  virtual void setFile           ( QString );
  void setHidden                 ( bool  );
  void setZoom                   ( float );
  QImage  &image                 ( );
  QString &fileName              ( );
  float    zoom                  ( );
  bool     hidden                ( );
  virtual Modifiers *modifiers   ( );
  ImageManipulator &manipulator  ( );
  virtual void resetStructureItem( );
  
signals:
  void signalModifyMe ( MenuObject * );
  
protected:
  virtual AnimationAttribute *getSpecificAttributes (long, QString);
  bool filterColorKey ( QColor *, QList<QColor *>*, QList<QColor *>*);
  void rotate (float);
  ImageData *imageData ( );
  
public slots:
  virtual void slotProperties     ( );
  
protected slots:
  virtual void slotEdit           ( );
  virtual void slotRotate90       ( );
  virtual void slotRotate180      ( );
  virtual void slotRotate270      ( );
  virtual void slotMatrix         ( );
  virtual void slotDefineAsButton ( );
  
protected:
  QString m_qsFileName;
  QImage  m_image;

};

#endif	// IMAGEOBJECT_H
