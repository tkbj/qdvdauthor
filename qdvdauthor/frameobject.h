/***************************************************************************
    frameobject.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   pure virtual class as base for the three button types.
   
****************************************************************************/

#ifndef FRAMEOBJECT_H
#define FRAMEOBJECT_H

//#include <qpixmap.h>
#include "menuobject.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>

class QPixmap;

class FrameObject : public MenuObject
{
	Q_OBJECT
	class FrameData : public MenuObject::Data
	{
	public:
	           FrameData ( );
	  virtual ~FrameData ( );
	  virtual  Data &operator  = ( Data & );
	  virtual  bool  operator == ( Data & );
	  virtual  Data *clone ( );

	  Rgba m_colorFrame;
	  int  m_iFrameWidth;
	  int  m_iFrameStyle;
	  int  m_iFrameJoin;
	};

public:
	// Public functions needed for the abstract base class.
	FrameObject (QWidget *pParent = 0);
	virtual ~FrameObject();
	virtual void drawContents          ( QPainter * );
	virtual void drawContents          ( QPainter *, int, int );
	virtual bool mousePressEvent       ( QMouseEvent * );
	virtual bool mouseReleaseEvent     ( QMouseEvent * );
	virtual bool mouseDoubleClickEvent ( QMouseEvent * );
    //virtual void paintEvent            ( QPaintEvent * );       //xxx

	virtual bool readProjectFile       ( QDomNode      & );
	virtual bool writeProjectFile      ( QDomElement   & );
	virtual bool createContextMenu     ( QPoint );
	virtual StructureItem *createStructure ( StructureItem * );
	virtual MenuObject    *clone       ( QWidget *p1=NULL, MenuObject *p2=NULL );
	virtual void replaceColor          ( Rgba, Rgba );

public:
	// Further public functions not dervied from the base class.
	void setFrameWidth                 ( int  );
	void setFrameStyle                 ( int  );
	void setFrameJoin                  ( int  );
	void setFrameColor                 ( Rgba );

	int  width                         ( );
	Rgba color                         ( );
	int  join                          ( );
	int  style                         ( );

protected slots:
	virtual void slotEdit              ( );
	virtual void slotMatrix            ( );
	virtual void slotAddText           ( );
	virtual void slotAddImage          ( );
	virtual void slotAddMovie          ( );
	virtual void slotDefineAsButton    ( );

protected:
	virtual AnimationAttribute *getSpecificAttributes ( long, QString );

private:
	FrameData *frameData ( );
};

#endif  // FRAMEOBJECT_H
