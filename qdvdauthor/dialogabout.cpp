/****************************************************************************
** vim: ts=4 sw=4 noet
**
** DialogAbout class
**
**   Created : Tue Sep 21 14:46:17 2010
**        by : Zsolt Branyiczky
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
****************************************************************************/

#include <QtGui>

#include "dialogabout.h"

DialogAbout::DialogAbout(QWidget *pParent)
	: QDialog(pParent)
{
	setupUi(this);
}

