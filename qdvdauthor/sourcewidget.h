/****************************************************************************
** vim: ts=4 sw=4 noet
**
** Interface to the GUI.
**
**   Created : Tue Sep 21 19:52:03 2010
**        by : Zsolt Branyiczky
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
****************************************************************************/

#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QWidget>
//#include <QMouseEvent>		//xxx

#include "ui_uisourcewidget.h"

class SourceWidget : public QWidget, public Ui::uiSourceWidget
{
  Q_OBJECT

public:
  SourceWidget(QWidget *pParent = 0);

protected:
  //void mousePressEvent	   	( QMouseEvent * );	//xxx
};
#endif

