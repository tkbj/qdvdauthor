/****************************************************************************
** Form interface generated from reading ui file 'smalldialogaudio.ui'
**
**   Created : Mon Jan 29 21:19:48 2007
**        by : Varol Okan, using XEmacs and designer
** Copyright : (c) Varol Okan
** License   : GPL v 2.0
**
****************************************************************************/

#ifndef SMALLDIALOGAUDIO_H
#define SMALLDIALOGAUDIO_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
//#include <Q3GridLayout>   //ooo
#include <QGridLayout>      //xxx
//#include <Q3HBoxLayout>   //ooo
#include <QHBoxLayout>      //xxx
//#include <Q3VBoxLayout>   //ooo
#include <QVBoxLayout>      //xxx
#include <QLabel>
#include "ui_smalldialogaudio.h"	//xxx

//class Q3VBoxLayout;   //ooo
class QVBoxLayout;      //xxx
//class Q3HBoxLayout;   //ooo
class QHBoxLayout;      //xxx
//class Q3GridLayout;   //ooo
class QGridLayout;      //xxx
class QSpacerItem;
class QComboBox;
//class Q3GroupBox;     //ooo
class QGroupBox;        //xxx
class QCheckBox;
class QLabel;
class QPushButton;

//#include "sourcefileentry.h"
class TranscodeInterface;
class SourceFileEntry;
class SourceFileInfo;
class Audio;

class SmallDialogAudio : public QDialog				//ooo
//class SmallDialogAudio : public QDialog, public Ui::SmallDialogAudio	//xxx
{
    Q_OBJECT

public:
     //SmallDialogAudio( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );	//ooo
     SmallDialogAudio( QWidget* parent = 0 );									//xxx
    ~SmallDialogAudio();

    void initMe ( Audio * );
    void initMe ( SourceFileEntry *, bool b=true );
    void setLanguageText ( QString & );
    QString languageText ( );

protected slots:
    virtual void accept                ( );
    virtual void slotHelp              ( );
    virtual void languageChange        ( );
    virtual void slotAudioLanguage     ( );
    virtual void slotTranscoding       ( bool );
    virtual void slotAudioEntryChanged (  int );
    
private:
    void initAudioInfo  ( Audio              *  );
    void initAudioInfo  ( SourceFileInfo     *  );
    void setTranscoding ( Audio              *  );
    void setTranscoding ( SourceFileInfo     *  );
    void setTranscoding ( TranscodeInterface *& );
    void initTranscode  ( TranscodeInterface *  );

private:
    SourceFileEntry *m_pSourceFileEntry;
    SourceFileEntry *m_pOrigSourceFileEntry;
    SourceFileInfo  *m_pPreviousSourceFileInfo;
    Audio           *m_pAudio;

    // Dialog widgets'n stuff
    QLabel* textLabel1;
    QLabel* m_pLabelLanguage;
    QComboBox* m_pComboAudioEntry;
    //Q3GroupBox* m_pGroupAudioFile;    //ooo
    QGroupBox* m_pGroupAudioFile;       //xxx
    QCheckBox* m_pCheckEnable;
    //Q3GroupBox* m_pGroupAudio;        //ooo
    QGroupBox* m_pGroupAudio;           //xxx
    QLabel* textLabel1_6_4_2;
    QLabel* textLabel1_6_4;
    QLabel* textLabel1_6_4_2_2;
    QLabel* textLabel1_2_3_4_2;
    QComboBox* m_pComboAudioOut;
    QComboBox* m_pComboAudioSampleRate;
    QComboBox* m_pComboAudioType;
    QComboBox* m_pComboAudioBitrate;
    QPushButton* m_pButtonLanguage;
    QPushButton* m_pButtonHelp;
    QPushButton* m_pButtonOk;
    QPushButton* m_pButtonCancel;

protected:
    //Q3VBoxLayout* SmallDialogAudioLayout; //ooo
    QVBoxLayout* SmallDialogAudioLayout;    //xxx
    QSpacerItem* spacer5;
    //Q3HBoxLayout* layout9;    //ooo
    QHBoxLayout* layout9;       //xxx
    //Q3GridLayout* m_pGroupAudioFileLayout;    //ooo
    QGridLayout* m_pGroupAudioFileLayout;       //xxx
    //Q3VBoxLayout* layout21;   //ooo
    QVBoxLayout* layout21;      //xxx
    //Q3HBoxLayout* m_pGroupAudioLayout;    //ooo
    QHBoxLayout* m_pGroupAudioLayout;       //xxx
    //Q3VBoxLayout* layout47;   //ooo
    QVBoxLayout* layout47;      //xxx
    //Q3VBoxLayout* layout29;   //ooo
    QVBoxLayout* layout29;      //xxx
    //Q3VBoxLayout* layout31;   //ooo
    QVBoxLayout* layout31;      //xxx
    //Q3VBoxLayout* layout48;   //ooo
    QVBoxLayout* layout48;      //xxx
    //Q3VBoxLayout* layout30;   //ooo
    QVBoxLayout* layout30;      //xxx
    //Q3VBoxLayout* layout32;   //ooo
    QVBoxLayout* layout32;      //xxx
    //Q3GridLayout* layout9_2;  //ooo
    QGridLayout* layout9_2;     //xxx
};

#endif // SMALLDIALOGAUDIO_H
