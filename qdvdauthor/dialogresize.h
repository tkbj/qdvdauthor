/***************************************************************************
    dialogresize.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef DIALOGRESIZE_H
#define DIALOGRESIZE_H

#include <QDialog>

#include "ui_dialogresize.h"

class DialogResize : public QDialog, public Ui::CResizeDialog
{
	Q_OBJECT

public:
	DialogResize(QWidget *pParent = 0);

	QSize m_posOffset;
	int m_iStretchType;
	int m_iFormat;
	QString m_qsFileName;

	virtual void setImage( QImage * pImage );
	void setFilename( QString & qsFileName );
	void setResizeAlgo( QStringList & algoList );

public slots:
	virtual void slotManualResize( const QString & );
	virtual void slotResize( );	//xxx
	virtual void slotResize( int iState );
	virtual void slotResizeAlogrithm( int iState );
	virtual void slotStretchType( int iState );
	virtual void slotPreview();
	virtual void preview( int iWidgetWidth, int iWidgetHeight, int iResizeAlgorithm );
	virtual void slotOk( void );
	virtual void slotHelp();
	virtual void setSize( int iWidth, int iHeight );

private:
	QImage *m_pImage;

	virtual int getResizeAlgorithm();
};

#endif

