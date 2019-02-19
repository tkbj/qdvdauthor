/****************************************************************************
** Interface to the GUI.
**
** Created: Tue Dec 14 22:48:00 2004
**      by: Varol Okan using kate editor
**
**
****************************************************************************/

#ifndef DIALOGMPLAYERSETUP_H
#define DIALOGMPLAYERSETUP_H

#include <QThread>	//xxx

#include <QWidget>		//xxx
//#include <QtCore/QVariant>    //ooo
#include <QVariant>             //xxx
//#include <QtGui/QAction>      //ooo
#include <QAction>              //xxx
//#include <QtGui/QApplication> //ooo
#include <QApplication>         //xxx
//#include <QtGui/QButtonGroup> //ooo
#include <QButtonGroup>         //xxx
//#include <QtGui/QComboBox>    //ooo
#include <QComboBox>            //xxx
//#include <QtGui/QDialog>      //ooo
#include <QDialog>              //xxx
//#include <QtGui/QGridLayout>  //ooo
#include <QGridLayout>          //xxx
//#include <QtGui/QHBoxLayout>  //ooo
#include <QHBoxLayout>          //xxx
//#include <QtGui/QHeaderView>  //ooo
#include <QHeaderView>          //xxx
//#include <QtGui/QLabel>       //ooo
#include <QLabel>               //xxx
//#include <QtGui/QLineEdit>    //ooo
#include <QLineEdit>            //xxx
//#include <QtGui/QPushButton>  //ooo
#include <QPushButton>          //xxx
//#include <QtGui/QSpacerItem>  //ooo
#include <QSpacerItem>          //xxx
//#include <QtGui/QVBoxLayout>  //ooo
#include <QVBoxLayout>          //xxx
#include <QProcess>		//xxx

//#include "uidialogmplayersetup.h"	//ooo
//#include "ui_uidialogmplayersetup.h"	//xxx

//class Q3Process;	//ooo
class QProcess;		//xxx
class QStringList;

class SleeperThread1 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

//class DialogMPlayerSetup : public uiDialogMPlayerSetup			//ooo
//class DialogMPlayerSetup : public QDialog, public Ui::uiDialogMPlayerSetup	//xxx
//class DialogMPlayerSetup : public QWidget					//xxx
class DialogMPlayerSetup : public QDialog					//xxx
{
	Q_OBJECT
public:
	         //DialogMPlayerSetup (QWidget *, const char *n=0, Qt::WFlags f=0);	//ooo
		 DialogMPlayerSetup (QWidget *);					//xxx
	virtual ~DialogMPlayerSetup ();

	void initMe ( );		//ooo
	void initMe(QStringList *);
	
public:
    QVBoxLayout *vboxLayout;
    QLabel *textLabel1_4;
    QGridLayout *gridLayout;
    QLabel *textLabel1;
    QLabel *textLabel1_3;
    QLineEdit *m_pEditAudioOptions;
    QLabel *textLabel1_3_2;
    QLabel *textLabel1_2;
    QComboBox *m_pComboVideo;
    QComboBox *m_pComboAudio;
    QLineEdit *m_pEditVideoOptions;
    QSpacerItem *spacer2;
    QHBoxLayout *hboxLayout;
    QPushButton *buttonHelp;
    QSpacerItem *Horizontal_Spacing2;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
	
protected slots:
	// This slot will retrieve the supported video / audio plugins
	virtual void slotProcessVideoPlugins ();
	virtual void slotProcessAudioPlugins ();

private:	// Private member functions
	void killProcesses ();

private:	// Private member variables
	//Q3Process    *m_pVideoProcess;	//ooo
	QProcess    *m_pVideoProcess;		//xxx
	//Q3Process    *m_pAudioProcess;	//ooo
	QProcess    *m_pAudioProcess;		//xxx
	
	QStringList *m_pListPluginOptions;
};

#endif // DIALOGMPLAYERSETUP_H
