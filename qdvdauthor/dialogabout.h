/****************************************************************************
** vim: ts=4 sw=4 noet
**
** Interface to the GUI.
**
**   Created : Tue Sep 21 14:46:17 2010
**        by : Zsolt Branyiczky
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
****************************************************************************/

#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

#include "ui_uidialogabout.h"

class DialogAbout : public QDialog, public Ui::uiDialogAbout
{
	Q_OBJECT

public:
	DialogAbout(QWidget *pParent = 0);
};
#endif

