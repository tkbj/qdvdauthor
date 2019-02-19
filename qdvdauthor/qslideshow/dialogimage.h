/***************************************************************************
    dialogimage.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'dialogimage.ui'

****************************************************************************/

#ifndef DIALOGIMAGE_H
#define DIALOGIMAGE_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include "imagepreview.h"
#include "ui_dialogimage.h"	//xxx

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QSlider;
class QFrame;
class QComboBox;
class QPushButton;
class QCheckBox;
class QLineEdit;

//class DialogImage : public QDialog					//ooo
class DialogImage : public QDialog, public Ui::DialogImage		//xxx
{
    Q_OBJECT

public:
    //DialogImage( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );  //ooo
    DialogImage( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0 );   //xxx
    //~DialogImage();   //ooo
    virtual ~DialogImage(); //xxx

    /*QLabel* textLabel5;
    QSlider* m_pSliderRotate;
    QFrame* m_pFrameImage;
    QLabel* textLabel4;
    QSlider* m_pSliderShearY;
    QFrame* line1;
    QFrame* frame5;
    QComboBox* m_pComboTransformMode;
    QLabel* textLabel1;
    QSlider* m_pSliderZoom;
    QLabel* textLabel2;
    QFrame* frame6;
    QPushButton* m_pButtonBackground;
    QCheckBox* m_pCheckBackground;
    QSlider* m_pSliderShearX;
    QLabel* textLabel3;
    QPushButton* buttonHelp;
    QLabel* m_pLabelStatus;
    QLineEdit* m_pEditStatus;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QFrame* frame4;
    QLabel* textLabel7;
    QLabel* textLabel8;
    QLabel* textLabel6;
    QSlider* m_pSliderRed;
    QSlider* m_pSliderGreen;
    QSlider* m_pSliderBlue;
    QFrame* frame4_3;
    QSlider* m_pSliderBrightness;
    QLabel* textLabel6_3;
    QFrame* frame4_3_2;
    QLabel* textLabel6_3_2;
    QSlider* m_pSliderTransparency;*/   //ooo

    virtual void initMe( void * pImg, QString & backgroundFileName );
    virtual ImagePreview * getPreview();

public slots:
    virtual void slotShearXValueChanged( int iValue );
    virtual void slotShearYValueChanged( int iValue );
    virtual void slotRotateValueChanged( int iValue );
    virtual void slotTransparencyValueChanged( int iValue );
    virtual void slotZoomValueChanged( int iValue );
    virtual void slotBrightnessValueChanged( int iValue );
    virtual void slotRedValueChanged( int iValue );
    virtual void slotGreenValueChanged( int iValue );
    virtual void slotBlueValueChanged( int iValue );
    virtual void slotSliderReleased();
    virtual void slotSliderPressed();
    virtual void slotValueChanged( const QString & newText );
    virtual void slotButtonBackground();
    virtual void slotBackgroundToggled( bool bToggled );
    virtual void slotComboActivated( int iNewMode );
    virtual void slotOk();

signals:
    void signalDone(void *);

protected:
    QGridLayout* DialogImageLayout;
    QVBoxLayout* layout5;
    QHBoxLayout* layout4;
    QGridLayout* frame5Layout;
    QHBoxLayout* layout3;
    QGridLayout* frame6Layout;
    QVBoxLayout* layout6;
    QHBoxLayout* layout7;
    QSpacerItem* Horizontal_Spacing2;
    QSpacerItem* Horizontal_Spacing2_2;
    QGridLayout* frame4Layout;
    QGridLayout* frame4_3Layout;
    QHBoxLayout* frame4_3_2Layout;

protected slots:
    virtual void languageChange();

private:
    ImagePreview *m_pImagePreview;

};

#endif // DIALOGIMAGE_H
