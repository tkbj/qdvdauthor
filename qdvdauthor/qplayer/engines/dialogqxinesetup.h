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

#ifndef DIALOGQXINESETUP_H
#define DIALOGQXINESETUP_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QPushButton>

//#include "ui_dialogqxinesetup.h"	//ooo

//class DialogQXineSetup : public QDialog, public Ui::DialogQXineSetup		//ooo
class DialogQXineSetup : public QDialog						//xxx
{
	Q_OBJECT
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QComboBox *m_pComboAudio;
    QLabel *textLabel1_2;
    QComboBox *m_pComboVideo;
    QLabel *textLabel1;
    QSpacerItem *spacer2;
    QHBoxLayout *hboxLayout;
    QPushButton *buttonHelp;
    QSpacerItem *Horizontal_Spacing2;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
    
public:
	DialogQXineSetup(QWidget *pParent = 0);

    virtual void initMe();
    virtual void setVideoPlugins( QStringList & listVideoPlugins, QString & qsCurrentVideo );
    virtual QString getAudioPlugin();
    virtual QString getVideoPlugin();

public slots:
    virtual void setAudioPlugins( QStringList & listAudioPlugins, QString & qsCurrentAudio );

protected slots:
    virtual void languageChange();
};
#endif

