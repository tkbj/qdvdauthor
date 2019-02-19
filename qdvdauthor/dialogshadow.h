/***************************************************************************
    dialogshadow.h
                             -------------------
    DialogShadow - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef DIALOGSHADOW_H
#define DIALOGSHADOW_H

//#include "uidialogshadow.h"	//ooo
#include "ui_uidialogshadow.h"	//xxx
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>

class ShadowObject;
//class Q3ScrollView;   //ooo
class QScrollArea;      //xxx
class ImageObject;
class MenuObject;
class QPixmap;
class QLabel;

//class DialogShadow : public uiDialogShadow				//ooo
class DialogShadow : public QDialog, public Ui::uiDialogShadow		//xxx
{
  Q_OBJECT
public:
   //DialogShadow ( QWidget *p=0, const char *n=0, bool m=false, Qt::WFlags fl=0 );	//ooo
   DialogShadow ( QWidget *pParent=0 );							//xxx
  ~DialogShadow ( );

  void initMe ( MenuObject *, QPixmap * );

protected:
  void drawShadow   ( );
  void refresh      ( );
  int getDialogType ( );

protected slots:
  virtual void accept               ( );
  virtual void reject               ( );
  virtual void slotCenterScrollView ( );
  virtual void slotShadowColor      ( );
  virtual void slotTransparency ( int );
  virtual void slotDistance     ( int );
  virtual void slotSunAngle     ( int );
  virtual void slotStyle        ( int );
  virtual void slotBlur         ( int );
  virtual void slotShearX       ( int );
  virtual void slotShearY       ( int );
  virtual void slotScaleX       ( int );
  virtual void slotScaleY       ( int );
  virtual void slotCutOff       ( int );
  virtual void slotCutOffRadius ( int );
  virtual void slotColorFade    ( int );
  virtual void slotTransparency     ( );
  virtual void slotCutOff           ( );
  virtual void slotCutOffRadius     ( );
  virtual void slotDistance ( const QString & );

private:
  void keepStaticVariables ( );
  void setStaticVariables  ( );
  void displayAdvanced     ( bool );

private:
  QPixmap      *m_pBackground;
  ShadowObject *m_pShadow;
  ShadowObject *m_pOrigShadow; // stores the original shadow in case we click cancel.
  MenuObject   *m_pMenuObject;
  QLabel       *m_pPreview;
  //Q3ScrollView  *m_pScrollView;   //ooo
  QScrollArea  *m_pScrollView;      //xxx
  //QPalette palette;                 //xxx

  // some playfield varialbles for the animation of the advanced part.
  int m_iAdvancedWidth; // stores the width of the m_pFrameMirror for the animation effect

  bool m_bStoredActiveState; // the object is to be drawn in normal state.

  // static vars to keep the current values for the shadow
  // this way the user does not always have to start from square one.
  static int    m_keepBlurRadius;
  static float  m_keepTransparency;
  static float  m_keepDistance;
  static float  m_keepSunAngle;
  static QColor m_keepColor;

  static float  m_keepShearX;
  static float  m_keepShearY;
  static float  m_keepScaleX;
  static float  m_keepScaleY;
  static float  m_keepCutOff;
  static float  m_keepCutOffRadius;
  static float  m_keepColorFade;
  static QColor m_keepTargetColor;
};

#endif // DIALOGSHADOW_H
