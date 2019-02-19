/****************************************************************************
** vim: ts=4 sw=4 noet
**
** SourceWidget class
**
**   Created : Tue Sep 21 19:52:03 2010
**        by : Zsolt Branyiczky
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
****************************************************************************/

#include <QtGui>
#include <QMessageBox>

#include "sourcewidget.h"

SourceWidget::SourceWidget(QWidget *pParent)
	: QWidget(pParent)
{
	setupUi(this);
}

/*void SourceWidget::mousePressEvent (QMouseEvent *pEvent)	//xxx
{
  //createContextMenu ( pEvent->globalPos  ( ) );
  //m_pPixmapMenu->createContextMenu( pEvent->globalPos  ( ) );
}*/
