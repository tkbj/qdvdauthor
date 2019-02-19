/***************************************************************************
    textobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   pure virtual class as base for the three button types. 
    
****************************************************************************/

#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "menuobject.h"

class TextObject : public MenuObject
{
  Q_OBJECT
  class TextData : public MenuObject::Data
  {
  public:
             TextData ( );
    virtual ~TextData ( );
    virtual  Data &operator  = ( Data & );
    virtual  bool  operator == ( Data & );
    virtual  Data *clone ( );

    QString  m_qsText;
    QFont    m_Font;
    QPoint   m_ptTextAlign;
    Rgba     m_colorForeground;
    Rgba     m_colorBackground;
    int      m_iStyleStrategy;
    bool     m_bAllowWordBreak; // don't need ot be stored. Only set after DialogTextFont
  };
  
public:
  TextObject ( QWidget *p=0 );
  virtual ~TextObject ( );
  virtual void drawContents          ( QPainter * );
  virtual void drawContents          ( QPainter *, int , int );
  virtual bool mousePressEvent       ( QMouseEvent * );
  virtual bool mouseReleaseEvent     ( QMouseEvent * );
  virtual bool mouseDoubleClickEvent ( QMouseEvent * );
  
  virtual bool readProjectFile       ( QDomNode      & );
  virtual bool writeProjectFile      ( QDomElement   & );
  virtual bool createContextMenu     ( QPoint );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual MenuObject    *clone       ( QWidget *p1=NULL, MenuObject *p2=NULL );

  virtual void setFont               ( QFont   & );
  virtual void setText               ( QString & );
  virtual void setTextAlign          ( QPoint  & );
  virtual void setWordBreak          ( bool );
  virtual void setStyleStrategy      ( int  );
  virtual void setForegroundColor    ( const Rgba & );
  virtual void setBackgroundColor    ( const Rgba & );
  virtual QFont   &getFont           ( );
  virtual QString &getText           ( );
  virtual QPoint  &getTextAlign      ( );
  virtual int      getStyleStrategy  ( );
  virtual Rgba    &getForegroundColor( );
  virtual Rgba    &getBackgroundColor( );
  
private:
  void drawSemiTransparent( QPainter * );
  TextData *textData      ( );
  
protected:
  virtual AnimationAttribute *getSpecificAttributes ( long, QString );
  
public slots:
  virtual void slotEdit              ( );
  virtual void slotMatrix            ( );
  virtual void slotDefineAsButton    ( );
};

#endif	// TEXTOBJECT_H

