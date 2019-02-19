/***************************************************************************
    dialogslide.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogslide.ui'

****************************************************************************/

#ifndef CSLIDEDIALOG_H
#define CSLIDEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qstringlist.h>
//#include <q3listbox.h>    //ooo
#include <QListWidget>      //xxx
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

#include "ui_dialogslide.h"	//xxx
//#include "xml_slideshow.h"	//ooo
#include "../xml_slideshow.h"	//xxx
#include "imagepreview.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QButtonGroup;
class QLabel;
class QFrame;
//class Q3ListBox;  //ooo
class QListWidget;  //xxx
//class Q3ListBoxItem;  //ooo
class QListWidgetItem;  //xxx
class QCheckBox;
class QLineEdit;

//class CSlideDialog : public QDialog					//ooo
//class CSlideDialog : public QDialog, public Ui::DialogSlide		//xxx
class CSlideDialog : public QDialog, public Ui::CSlideDialog		//xxx
{
    Q_OBJECT

public:
    //CSlideDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 ); //ooo
    CSlideDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0 );  //xxx
    ~CSlideDialog();

    QPushButton* m_pButtonSelect;
    QGroupBox* buttonGroup1;
    QLabel* m_pLabelNrOfImages;
    QLabel* m_pLabelDuration;
    QFrame* m_pFramePreview;
    QPushButton* m_pButtonImport;
    QPushButton* m_pButtonExport;
    QListWidget* m_pListTable;
    QPushButton* m_pButtonOk;
    QPushButton* m_pButtonCancel;
    QPushButton* m_pButtonHelp;
    QCheckBox* m_pCheckBackground;
    QLabel* textLabel1;
    QCheckBox* m_pCheckFilter;
    QPushButton* m_pButtonBackground;
    QLineEdit* m_pEditDelay;
    QLineEdit* m_pEditFilterDelay;
    QPushButton* m_pButtonDelete;
    QPushButton* m_pButtonFilter;

    QStringList m_slideshowFiles;
    bool m_bModified;
    QString m_qsSlideshowFileName;

    virtual void initMe( void * pSourceToolBar, QString qsTempPath );

public slots:
    virtual void slotSelectImages();
    virtual void slotChooseBackground();
    virtual void slotBackgroundToggled( bool bToggled );
    virtual void slotExport();
    virtual void slotImport();
    virtual void slotOk();
    virtual void slotCancel();
    virtual void slotHelp();
    virtual void slotApplyFilter();
    virtual void slotFilterToggled( bool bToggled );
    //virtual void slotListRightMouseClicked( Q3ListBoxItem *, const QPoint & point );  //ooo
    virtual void slotListRightMouseClicked( QListWidgetItem * );  //xxx
    virtual void slotSelectionChanged();
    virtual void createFilterSlideshow( CXmlSlideshow * pSlideshoow );
    virtual void slotDelete();

protected:
    QGridLayout* CSlideDialogLayout;
    QSpacerItem* spacer6;
    QGridLayout* buttonGroup1Layout;
    QGridLayout* m_pFramePreviewLayout;
    QHBoxLayout* layout6;
    QSpacerItem* spacer4;
    QHBoxLayout* layout9;
    QVBoxLayout* layout7;
    QVBoxLayout* layout8;
    QHBoxLayout* layout15;
    QSpacerItem* spacer5;

protected slots:
    virtual void languageChange();

private:
    QString m_qsTempPath;
    void *m_pSourceToolBar;
    ImagePreview *m_pPreview;
    CXmlSlideshow m_Slideshow;

    void previewImage( int iItem );

private slots:
    virtual void slotUpdateListInfo();

};

#endif // CSLIDEDIALOG_H
