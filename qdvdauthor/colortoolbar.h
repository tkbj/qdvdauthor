/****************************************************************************
** Interface to the GUI.
**
**   Created : Tue 08 May 12:09:08 2004
**        by : Varol Okan using kate editor
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** This class will create a toolbar, which holds only one widget.
** The StructureWidget is displaying the current structure of the DVD.
**
****************************************************************************/

#ifndef COLORTOOLBAR_H
#define COLORTOOLBAR_H

#include <QToolBar>
#include <QDockWidget>
#include <QSize>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFrame>

#include "rgba.h"

class QDockWidget;
//class QListView;	//ooo

class DVDMenu;

class QColorFrame : public QFrame
{
  Q_OBJECT
public:
  QColorFrame ( int, Rgba, QWidget *p=0, const char *n=0, Qt::WindowFlags f=0 );	//ooo
  //QColorFrame ( QWidget *p=0, Qt::WindowFlags f=0 );	//xxx
  virtual ~QColorFrame ( );

  Rgba getColor   ( );
  void setColor   ( Rgba );

signals:
  void signalRightButtonClicked (int);

protected:
  // Overloaded member function ...
  virtual void mouseReleaseEvent (QMouseEvent *);
  virtual void paintEvent        (QPaintEvent *);

private:
  int  m_iWhichColor;
  Rgba m_color;
};


//class ColorToolBar : public QToolBar
class ColorToolBar : public QDockWidget
{
  Q_OBJECT

public:
  ColorToolBar ( const char *, QMainWindow * );	//ooo
  //ColorToolBar ( const char *, QDockWidget * );
  virtual ~ColorToolBar ( );

  Rgba getColor   ( uint );
  void setColor   ( uint, Rgba );
  void setDVDMenu ( DVDMenu  * );

protected slots:
  virtual void slotColorClicked ( int );


private:
	QColorFrame *m_colorFrames[4];
	DVDMenu     *m_pCurrentDVDMenu;
};

#endif // COLORTOOLBAR_H


 
