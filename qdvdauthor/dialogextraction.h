/***************************************************************************
    dialogextraction.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef DIALOGEXTRACTION_H
#define DIALOGEXTRACTION_H

#include <QDialog>

#include "ui_uidialogextraction.h"

class DialogExtraction : public QDialog, public Ui::uiDialogExtraction
{
	Q_OBJECT

public:
	DialogExtraction(QWidget *pParent = 0);
	~DialogExtraction();

    virtual void setMenuName( QString qsMenuName );
    virtual void setObjectName( QString qsObjectName );
    virtual void setTime( QString qsTime );

public slots:
    virtual void slotAddASecond();

protected:
    int m_iSeconds;

protected slots:
    virtual void languageChange();

private:
    void init();
};
#endif

