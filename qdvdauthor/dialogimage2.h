/***************************************************************************
    dialogimage2.h
                             -------------------
                             
    DialogImage2 - class                         
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    After all the extensions to the MenuObjects (Shadows, groups etc)
    the original DialogImage - dialog was no longer sufficient and
    became unmaintanable.
    
    This class recreates the exact same user interface and handles
    all user interaction and displaying internally.
    
 ***************************************************************************/

#ifndef DIALOGIMAGE2_H
#define DIALOGIMAGE2_H

#include <qlabel.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>

//#include "uidialogimage.h"	//ooo
#include "ui_uidialogimage.h"	//xxx

//class Q3ScrollView;   //ooo
class QScrollArea;     //xxx
class ImageObject;
class ImageManipulator;

//class DialogImage2 : public uiDialogImage			//ooo
class DialogImage2 : public QDialog, Ui::uiDialogImage		//xxx
{
    Q_OBJECT

    class ImagePreview : public QLabel
    {
      //      Q_OBJECT
    public:
      //ImagePreview ( ImageObject *, DialogImage2 *, QWidget *p=NULL, const char *n=NULL, Qt::WFlags f=0 );    //ooo
      ImagePreview ( ImageObject *, DialogImage2 *, QWidget *p=NULL );                                          //xxx
      virtual ~ImagePreview ( );

    protected:
      void mouseReleaseEvent     ( QMouseEvent * );
      void mouseMoveEvent        ( QMouseEvent * );
      void mouseDoubleClickEvent ( QMouseEvent * );
      void mousePressEvent       ( QMouseEvent * );

    private:
      ImageObject  *m_pImageObject;
      DialogImage2 *m_pParent;
    };

public:
     //DialogImage2 ( QWidget *p=NULL, const char *n=NULL, bool m=FALSE, Qt::WFlags f=0 );	//ooo
     DialogImage2 ( QWidget *pParent=NULL );							//xxx
    ~DialogImage2 ( );

    void initMe ( ImageObject *, QPixmap * );
    void createActiveBackground (   bool   );

private:
    void redrawObject ( );

protected slots:
    virtual void slotOk                       (      );
    virtual void reject                       (      );
    virtual void slotCenterScrollView         (      );
    virtual void slotSliderReleased           (      );
    virtual void slotSliderPressed            (      );
    virtual void slotSliderReleased2          (      );
    virtual void slotRotate90                 (      );
    virtual void slotRotate180                (      );
    virtual void slotRotate270                (      );
    virtual void slotButtonProperties         (      );
    virtual void slotButtonShadow             (      );
    virtual void slotShearXValueChanged       ( int  );
    virtual void slotShearYValueChanged       ( int  );
    virtual void slotRotateValueChanged       ( int  );
    virtual void slotTransparencyValueChanged ( int  );
    virtual void slotZoomValueChanged         ( int  );
    virtual void slotBrightnessValueChanged   ( int  );
    virtual void slotRedValueChanged          ( int  );
    virtual void slotGreenValueChanged        ( int  );
    virtual void slotBlueValueChanged         ( int  );
    virtual void slotComboActivated           ( int  );
    virtual void slotValueChanged             ( const QString & );

private:
    QLabel      *m_pImagePreview;
    //Q3ScrollView *m_pScrollView;  //ooo
    QScrollArea *m_pScrollView;     //xxx
    ImageObject *m_pImageObject;
    ImageObject *m_pOrigImageObject;
    QPixmap      m_backgroundBuffer;      
    QPixmap      m_activeBackgroundBuffer; 
    double       m_fOrigScaleX; // ImageObjects are scaled using fScaleX, fScaleY
    double       m_fOrigScaleY; // m_fZoom simply multiplies their current values
};

#endif // DIALOGIMAGE2_H
