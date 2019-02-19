/***************************************************************************
    uidialogcreate.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'uidialogcreate.ui'

****************************************************************************/

#ifndef UIDIALOGCREATE_H
#define UIDIALOGCREATE_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QProgressBar;

class uiDialogCreate : public QDialog
{
    Q_OBJECT

public:
    //uiDialogCreate( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );   //ooo
    uiDialogCreate( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0 );    //xxx
    ~uiDialogCreate();

    QPushButton* buttonHelp;
    QPushButton* m_pButtonBack;
    QPushButton* buttonCancel;
    QTabWidget* m_pTabWidget;
    QWidget* tab;
    QPushButton* m_pButtonGenerate;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabel1_2;
    QLabel* textLabel1_3;
    QLineEdit* m_pEditTempDir;
    QLineEdit* m_pEditExecutableDir;
    QLineEdit* m_pEditOutputFileName;
    QLabel* textLabel1_4;
    QLineEdit* m_pEditSlideshowTitle;
    QComboBox* m_pComboFormat;
    QWidget* tab_2;
    QLabel* textLabel4;
    QLabel* textLabel6;
    QLineEdit* m_pEditImageNumber;
    QLabel* textLabel5;
    QLineEdit* m_pEditTotalNumber;
    QLineEdit* m_pEditCurrentFileName;
    QTextEdit* m_pTextOutput;
    QLabel* m_pTextStatus;
    QProgressBar* m_pProgress;

public slots:
    virtual void slotBack();

protected:
    QGridLayout* uiDialogCreateLayout;
    QHBoxLayout* layout9;
    QSpacerItem* Horizontal_Spacing2;
    QSpacerItem* Horizontal_Spacing2_2;
    QGridLayout* tabLayout;
    QSpacerItem* spacer5_2;
    QHBoxLayout* layout14;
    QVBoxLayout* layout11;
    QVBoxLayout* layout12;
    QHBoxLayout* layout10;
    QVBoxLayout* tabLayout_2;
    QSpacerItem* spacer6;
    QHBoxLayout* layout11_2;
    QVBoxLayout* layout8;
    QVBoxLayout* layout10_2;
    QHBoxLayout* layout9_2;
    QSpacerItem* spacer4;

protected slots:
    virtual void languageChange();

};

#endif // UIDIALOGCREATE_H
