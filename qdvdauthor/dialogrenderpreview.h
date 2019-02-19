/***************************************************************************
    dialogrenderpreview.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef DIALOGRENDERPREVIEW_H
#define DIALOGRENDERPREVIEW_H

#include <QDialog>

#include "ui_uidialogrenderpreview.h"

class DialogRenderPreview : public QDialog, public Ui::uiDialogRenderPreview
{
	Q_OBJECT

public:
	DialogRenderPreview(QWidget *pParent = 0);

	void setProgressData (QPixmap *pPixmap, long iCurrentFrameNumber, long iTotalFramesToRender);
};
#endif

