/***************************************************************************
    dialogmatrix.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef DIALOGMATRIX_H
#define DIALOGMATRIX_H

#include <QDialog>

#include "ui_dialogmatrix.h"

class DialogMatrix : public QDialog, public Ui::DialogMatrix
{
	Q_OBJECT

public:
	DialogMatrix(QWidget *pParent = 0);
    ~DialogMatrix();

    virtual void initMe( MenuObject * pParent );

public slots:
    virtual void slotClose();
    virtual void slotZoom( int iPos );
    virtual void slotRotate( int iPos );
    virtual void slotShearX( int iPos );
    virtual void slotShearY( int iPos );
    virtual void slotSliderPressed();
    virtual void slotSliderReleased();
    virtual void slotDeleteObject( MenuObject * );
    virtual void slotZoomChanged();
    virtual void slotRotateChanged();
    virtual void slotShearXChanged();
    virtual void slotShearYChanged();

protected:
    MenuObject *m_pParent;

protected slots:
    virtual void languageChange();
};
#endif

