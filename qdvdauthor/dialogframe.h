/***************************************************************************
    dialogframe.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef DIALOGFRAME_H
#define DIALOGFRAME_H

#include <QFrame>
//Added by qt3to4:
#include <QPaintEvent>

// Okay this is stupid but I could not create a template class from a QFrame class.
// I needed this in order to have the Preview of the Line around the inner box
// updated correctly. If I am using QFrame I can not guarantee a correct update
// hence I create a derived class but this I could no incorporate into the .ui File
// So I use the created source, modify it sligthly and be hapy with it.
// Note: I am leaving the uidialogframe.ui file in the source tree but I removed it from the
// project file for qdvdauthor.
//
// here is the derived QFrame class which I simply substitute in the uidialogframe - class.

/*
// forward declaration
class DialogFrame;
class FramePreview : public QFrame
{
public:
	 FramePreview(QWidget *p=0, const char *n=0, WFlags f=0);
	~FramePreview();
	virtual void setDialog(DialogFrame *);
	void paintEvent(QPaintEvent *pPaintE);
protected:
	DialogFrame *m_pDialogFrame;
};
*/
#include "rgba.h"
//#include "uidialogframe.h"	//ooo
#include "ui_uidialogframe.h"	//xxx
#include "frameobject.h"
#include "dialoganimation.h"

class QPaintEvent;
class QPainter;

//class DialogFrame : public uiDialogFrame			//ooo
class DialogFrame : public QDialog, public Ui::uiDialogFrame	//xxx
{
	Q_OBJECT
public:
	 //DialogFrame ( QWidget *, const char *n=0, Qt::WFlags f=0 );	//ooo
	 DialogFrame ( QWidget *pParent = 0 );				//xxx
	~DialogFrame ( );

	void initMe  ( );
	void initMe  ( FrameObject * );
	void initMe  ( int, int, int, Rgba );

	int      frameWidth ( );
	int      style      ( );
	int      join       ( );
	Rgba     color      ( );
	QString &animation  ( );

protected:
	virtual void paintEvent  (QPaintEvent *);
	void   drawPreview  ( );
	virtual void polish ( );

protected slots:
	virtual void slotColor     ( );
	virtual void slotImage     ( );
	virtual void slotPicture   ( );
	virtual void slotAnimation ( );
	virtual void slotImage     ( bool );
	virtual void slotPicture   ( bool );
	virtual void slotWidth     ( int  );
	virtual void slotStyle     ( int  );
	virtual void slotJoin      ( int  );
	virtual void slotRefresh   ( );

private:
	FrameObject *m_pOrigFrame;
	Rgba    m_colorFrame;
	int     m_iFrameWidth;
	int     m_iFrameStyle;
	int     m_iFrameJoin;
	QString m_qsAnimation;
};

#endif // DIALOGFRAME_H
