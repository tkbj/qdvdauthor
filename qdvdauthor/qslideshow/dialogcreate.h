/***************************************************************************
    dialogcreate.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogfilter.ui'

****************************************************************************/

#ifndef DIALOGCREATE_H
#define DIALOGCREATE_H

#include <QDialog>	//xxx

//#include "uidialogcreate.h"		//ooo
#include "ui_uidialogcreate.h"		//xxx
//#include "xml_slideshow.h"		//ooo
#include "../xml_slideshow.h"		//xxx

//class Q3Process;  //ooo
class QProcess; //xxx

//class DialogCreate : public uiDialogCreate				//ooo
class DialogCreate : public QDialog, public Ui::uiDialogCreate		//xxx
{
	Q_OBJECT
public:
	 //DialogCreate (QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );		//ooo
	 DialogCreate (QWidget * parent = 0, const char * name = 0, Qt::WindowFlags f = 0 );	//xxx
	~DialogCreate ();

	void findDVDSlideshow();

	void setXmlSlideshow (CXmlSlideshow *, QString);
	QString & getOutputFileName ();
	void setOutputFileName (QString &);

signals:
	void signalSetSlideshowEntry(QString);
	void signalSlideshowDone    (QString, QString);
	
private:
	void    renderImage ( CXmlSlideshow::img_struct *, QString, bool );
	void    verifySlideshowExists  ( bool b=true );
	QString getUniqueTempFile      ( QString );
	bool    sanityCheck            ();
	void    preprocessSlideshow    ();
	void    createDVDSlideshowFile ();
	void    executeDVDSlideshow    ();

public slots:
	virtual void slotGenerate      ();
	virtual void slotBack          ();
	// callbacks for the QProcess ...
	virtual void slotReadFromStdout();
	virtual void slotReadFromStderr();
	virtual void slotProcessExited ();


private:
	QString m_qsOutputFileName;
	QString m_qsTempPath;
	CXmlSlideshow	m_Slideshow;	// This is a copy of the orig to hold the
		// names of the preprocessed images rather then the original ones.
	QProcess *m_pNewProcess;
	uint m_iProgressCounter;
	QWidget *m_pMainDialog;
};

#endif // DIALOGCREATE
