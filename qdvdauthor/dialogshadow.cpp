/***************************************************************************
    dialogshadow.cpp
                             -------------------
    DialogShadow
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <qdial.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qtimer.h>
#include <qslider.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qcombobox.h>
//#include <q3groupbox.h>   //ooo
#include <QGroupBox>        //xxx
//#include <q3scrollview.h> //ooo
#include <QScrollArea>      //xxx
#include <qpushbutton.h>
#include <qcolordialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QPixmap>


#include "global.h" // for PI
#include "shadowobject.h"
#include "menuobject.h"
#include "dialogshadow.h"
#include "messagebox.h"

// Init static member variables
int    DialogShadow::m_keepBlurRadius    =    0;
float  DialogShadow::m_keepTransparency  =  0.0f;
float  DialogShadow::m_keepDistance      = 10.0f;
float  DialogShadow::m_keepSunAngle      = 46.0f;
QColor DialogShadow::m_keepColor         = QColor ( 0, 0, 0 );

// Statics for the Mirror variant of a shadow
float  DialogShadow::m_keepShearX        =  0.0f;
float  DialogShadow::m_keepShearY        =  0.0f;
float  DialogShadow::m_keepScaleX        =  1.0f;
float  DialogShadow::m_keepScaleY        =  1.0f;
float  DialogShadow::m_keepCutOff        =  0.5f;
float  DialogShadow::m_keepCutOffRadius  =  0.3f;
float  DialogShadow::m_keepColorFade     =  0.0f;
QColor DialogShadow::m_keepTargetColor   = QColor ( 0, 0, 0 );


//DialogShadow::DialogShadow ( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )	//ooo
DialogShadow::DialogShadow ( QWidget* pParent )							//xxx
  //: uiDialogShadow   ( parent, name, modal, fl )	//ooo
  : QDialog ( pParent )					//xxx
{
  setupUi(this);	//xxx
  
  m_pBackground    = NULL;
  m_pShadow        = NULL;
  m_pOrigShadow    = NULL;
  m_pMenuObject    = NULL;
  m_pPreview       = NULL;
  m_pScrollView    = NULL;
  m_iAdvancedWidth = 100;
  m_bStoredActiveState = false;

  //m_pDialSunAngle->setMinValue       (     0 );   //ooo
  m_pDialSunAngle->setMinimum          (     0 );   //xxx
  //m_pDialSunAngle->setMaxValue       ( 36000 );   //ooo
  m_pDialSunAngle->setMaximum          ( 36000 );   //xxx
  //m_pSliderBlur->setMinValue         (     2 );   //ooo
  m_pSliderBlur->setMinimum            (     2 );   //xxx
  //m_pSliderBlur->setMaxValue         (    50 );   //ooo
  m_pSliderBlur->setMaximum            (    50 );   //xxx
  //m_pSliderDistance->setMinValue     (  -300 );   //ooo
  m_pSliderDistance->setMinimum        (  -300 );   //xxx
  //m_pSliderDistance->setMaxValue     (   300 );   //ooo
  m_pSliderDistance->setMaximum     (   300 );      //xxx
  m_pSliderDistance->setValue        (     0 );
  //m_pSliderTransparency->setMinValue (     0 );   //ooo
  m_pSliderTransparency->setMinimum    (     0 );   //xxx
  //m_pSliderTransparency->setMaxValue ( 10000 );   //ooo
  m_pSliderTransparency->setMaximum ( 10000 );      //xxx
  m_pDialSunAngle->setValue          ( 31500 );

  //m_pSliderShearX->setMaxValue       (  1000 );   //ooo
  m_pSliderShearX->setMaximum          (  1000 );   //xxx
  //m_pSliderShearX->setMinValue       ( -1000 );   //ooo
  m_pSliderShearX->setMinimum          ( -1000 );   //xxx
  m_pSliderShearX->setValue          (     0 );
  //m_pSliderShearY->setMaxValue       (  1000 );   //ooo
  m_pSliderShearY->setMaximum          (  1000 );   //xxx
  //m_pSliderShearY->setMinValue       ( -1000 );   //ooo
  m_pSliderShearY->setMinimum          ( -1000 );   //xxx
  m_pSliderShearY->setValue          (     0 );
  //m_pSliderScaleX->setMaxValue       (  1000 );   //ooo
  m_pSliderScaleX->setMaximum          (  1000 );   //xxx
  //m_pSliderScaleY->setMaxValue       (  1000 );   //ooo
  m_pSliderScaleY->setMaximum          (  1000 );   //xxx
  //m_pSliderCutOff->setMaxValue       (  1000 );   //ooo
  m_pSliderCutOff->setMaximum          (  1000 );   //xxx
  //m_pSliderCutOffRadius->setMaxValue (  1000 );   //ooo
  m_pSliderCutOffRadius->setMaximum    (  1000 );   //xxx
  //m_pSliderColorFade->setMaxValue    (  1000 );   //ooo
  m_pSliderColorFade->setMaximum       (  1000 );   //xxx

  //m_pSliderBlur->setTickmarks            ( QSlider::TicksBelow ); //ooo
  m_pSliderBlur->setTickPosition         ( QSlider::TicksBelow );   //xxx
  //m_pSliderDistance->setTickmarks        ( QSlider::TicksBelow ); //ooo
  m_pSliderDistance->setTickPosition     ( QSlider::TicksBelow );   //xxx
  //m_pSliderTransparency->setTickmarks    ( QSlider::TicksRight ); //ooo
  m_pSliderTransparency->setTickPosition ( QSlider::TicksRight );   //xxx
  //m_pSliderTransparency->setLineStep     (  100 );    //ooo
  m_pSliderTransparency->setPageStep     ( 1000 );
  m_pSliderDistance->setTickInterval     (   50 );
  m_pSliderTransparency->setTickInterval ( 1000 );

  //Q3GridLayout *pLayout;  //ooo
  QGridLayout *pLayout;     //xxx
  //uint iFlags = 0;	//ooo

//#if (QT_VERSION > 0x0301FF)		//ooo
//	iFlags = Qt::WNoAutoErase;
//#else
//	iFlags = Qt::WPaintClever;
//#endif
	
  //Qt::WindowFlags iFlags = Qt::WNoAutoErase;	//xxx

  //pLayout       = new Q3GridLayout ( m_pLabelPreview, 1, 1, 2, 2, "pFramePreviewLayout"); //ooo
  pLayout       = new QGridLayout ( m_pLabelPreview);                                       //xxx
  //m_pScrollView = new Q3ScrollView ( m_pLabelPreview, "m_pScrollView", iFlags );          //ooo
  m_pScrollView = new QScrollArea ( m_pLabelPreview );                                      //xxx
  //m_pPreview    = new QLabel      ( m_pScrollView,   "pPreview",      iFlags );           //ooo
  m_pPreview    = new QLabel      ( m_pScrollView );                                        //xxx
  //m_pScrollView->addChild ( m_pPreview ); //ooo
  m_pScrollView->setWidget ( m_pPreview );  //xxx
  m_pPreview->resize      (  720, 480  );
  pLayout->addWidget      ( m_pScrollView, 0, 0 );

  m_pEditDistance->setText   ( "25" );
  m_pSliderDistance->setValue ( 25 );
  //m_pLabelColor->setPaletteBackgroundColor ( QColor ( 0, 0, 0 ) );        //ooo
  QPalette palette;							                                //xxx
  //palette.setColor(m_pLabelColor->backgroundRole(), QColor ( 0, 0, 0 ));    //xxx
  palette.setColor(QPalette::Window, QColor ( 0, 0, 0 ));    //xxx
  //palette.setColor(QPalette::Button, QColor ( 0, 0, 0 ));    //xxx
  //palette.setBrush(QPalette::Base, QBrush(QColor ( "#BACBE9" ) ) );	//xxx
  //palette.setBrush(QPalette::Window, QBrush(QColor ( "#aaaaff" ) ) );	//xxx
  m_pLabelColor->setAutoFillBackground(true);   //xxx
  m_pLabelColor->setPalette(palette);						                //xxx
  //this->setPalette(palette);   //xxx
  //this->m_pLabelColor->setPalette(palette);   //xxx
  //m_pGroupBoxColor->setPalette(palette);    //xxx
}

DialogShadow::~DialogShadow ( )
{
  
   if ( m_pMenuObject ) 
        m_pMenuObject->setActive ( m_bStoredActiveState );

  if ( m_pShadow )
    delete m_pShadow;
  if ( m_pOrigShadow )
    delete m_pOrigShadow;
  m_pShadow     = NULL;
  m_pOrigShadow = NULL;
}

void DialogShadow::initMe ( MenuObject *pMenuObject, QPixmap *pBackground )
{
  m_pBackground =  pBackground;
  if ( pMenuObject ) {
     m_pMenuObject = pMenuObject;
     m_bStoredActiveState = m_pMenuObject->isActive ( );
     m_pMenuObject->setActive ( false );

     m_pShadow = new ShadowObject ( m_pMenuObject );
     if ( pMenuObject->shadow ( ) ) {
       ShadowObject    *pCurrentShadow = ( ShadowObject *) pMenuObject->shadow ( );
       m_pOrigShadow =  new ShadowObject ( m_pMenuObject );
      *m_pOrigShadow = *pCurrentShadow;
      *m_pShadow     = *pCurrentShadow;
     }
     else
       setStaticVariables ( );
     int iBlur         =        m_pShadow->blur         ( );
     int iDelta        =      ( m_pShadow->sunAngle     ( ) <    90.0 ) ? 27000 : -9000;
     int iSunAngle     = (int)( m_pShadow->sunAngle     ( ) *   100.0 ) + iDelta;
     int iTransparency = (int)( m_pShadow->transparency ( ) * 10000.0 );
     if ( m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR )
       iSunAngle -= 9000;

     m_pMenuObject->setShadow        ( NULL );
     m_pEditDistance->setText        ( QString ( "%1" ).arg ( (int) m_pShadow->distance ( ) ) );
     m_pSliderDistance->setValue     ( (int)m_pShadow->distance ( ) );
     m_pSliderTransparency->setValue ( iTransparency );
     m_pDialSunAngle->setValue       (   iSunAngle   );
     m_pSliderBlur->setValue         (     iBlur     );

     //m_pLabelColor->setPaletteBackgroundColor ( m_pShadow->color ( ) );       //ooo
     QPalette palette;							                                //xxx
     //palette.setColor(m_pLabelColor->backgroundRole(), m_pShadow->color ( ));	//xxx
     palette.setColor(QPalette::Window, m_pShadow->color ( ));	//xxx
     //palette.setBrush(QPalette::Base, QBrush(QColor ( "#BACBE9" ) ) );	//xxx
     //palette.setBrush(QPalette::Window, m_pShadow->color ( ) );	//xxx
     //m_pLabelColor->setBackgroundRole(QPalette::Base);    //xxx
     m_pLabelColor->setPalette(palette);					                    //xxx
     
     m_pSliderDistance->setPageStep  ( 1 );

     m_pSliderShearX->setValue       ( (int) ( m_pShadow->shearX ( ) * 1000.0 ) );
     m_pSliderShearY->setValue       ( (int) ( m_pShadow->shearY ( ) * 1000.0 ) );
     m_pSliderScaleX->setValue       ( (int) ( m_pShadow->scaleX ( ) * 1000.0 ) );
     m_pSliderScaleY->setValue       ( (int) ( m_pShadow->scaleY ( ) * 1000.0 ) );
     m_pSliderCutOff->setValue       ( (int) ( m_pShadow->cutOff ( ) * 1000.0 ) );
     m_pSliderCutOffRadius->setValue ( (int) ( m_pShadow->cutOffRadius ( ) * 1000.0 ) );
     m_pSliderColorFade->setValue    ( (int) ( m_pShadow->colorFading  ( ) * 1000.0 ) );

     m_pEditShearX->setText ( QString ( "%1" ).arg ( m_pShadow->shearX ( ) ) );
     m_pEditShearY->setText ( QString ( "%1" ).arg ( m_pShadow->shearY ( ) ) );
     m_pEditScaleX->setText ( QString ( "%1" ).arg ( m_pShadow->scaleX ( ) ) );
     m_pEditScaleY->setText ( QString ( "%1" ).arg ( m_pShadow->scaleY ( ) ) );
     m_pEditCutOff->setText ( QString ( "%1" ).arg ( m_pShadow->cutOff ( ) ) );
     m_pEditCutOffRadius->setText ( QString ( "%1" ).arg ( m_pShadow->cutOffRadius ( ) ) );

     { // this hack is to prevent three times refresh if not needed.
       ShadowObject *pTemp = m_pShadow;
       m_pShadow = NULL;
       slotBlur         (     iBlur     );
       slotSunAngle     (   iSunAngle   );
       slotTransparency ( iTransparency );
       m_pShadow = pTemp;
     }
  }

  refresh ( );

  connect ( m_pButtonOkay,         SIGNAL ( clicked ( ) ), this, SLOT ( accept ( ) ) );
  connect ( m_pButtonCancel,       SIGNAL ( clicked ( ) ), this, SLOT ( reject ( ) ) );
  connect ( m_pButtonColor,        SIGNAL ( clicked ( ) ), this, SLOT ( slotShadowColor ( ) ) );

  connect ( m_pSliderDistance,     SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotDistance     ( int ) ) );
  connect ( m_pSliderTransparency, SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotTransparency ( int ) ) );
  connect ( m_pSliderTransparency, SIGNAL ( sliderReleased ( ) ),   this, SLOT ( slotTransparency ( ) ) );
  connect ( m_pDialSunAngle,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotSunAngle     ( int ) ) );
  connect ( m_pSliderBlur,         SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotBlur         ( int ) ) );
  connect ( m_pComboStyle,         SIGNAL ( activated    ( int ) ), this, SLOT ( slotStyle        ( int ) ) );

  connect ( m_pSliderColorFade,    SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotColorFade    ( int ) ) );
  connect ( m_pSliderShearX,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotShearX       ( int ) ) );
  connect ( m_pSliderShearY,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotShearY       ( int ) ) );
  connect ( m_pSliderScaleX,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotScaleX       ( int ) ) );
  connect ( m_pSliderScaleY,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotScaleY       ( int ) ) );
  connect ( m_pSliderCutOff,       SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotCutOff       ( int ) ) );
  connect ( m_pSliderCutOffRadius, SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotCutOffRadius ( int ) ) );
  connect ( m_pSliderCutOff,       SIGNAL ( sliderReleased ( ) ),   this, SLOT ( slotCutOff       ( ) ) );
  connect ( m_pSliderCutOffRadius, SIGNAL ( sliderReleased ( ) ),   this, SLOT ( slotCutOffRadius ( ) ) );
  connect ( m_pEditDistance,       SIGNAL ( textChanged ( const QString & ) ), this, SLOT ( slotDistance ( const QString & ) ) );

  QTimer::singleShot ( 0, this, SLOT (slotCenterScrollView ( ) ) );
}

void DialogShadow::accept ( )
{
  keepStaticVariables ( );

  float fTransparency   = m_pSliderTransparency->value ( ) / 10000.0;
  float fCutOff         = m_pEditCutOff->text ( ).toFloat ( );
  float fCutOffRadius   = m_pEditCutOffRadius->text ( ).toFloat ( );

  // If the user made the shadow invisible, then simply remove it ...
  if ( ( fTransparency >= 1.0 ) || 
       ( ( fCutOff == 0 ) && ( fCutOffRadius == 0.0 ) ) ){
    if ( MessageBox::information ( NULL, tr ( "Shadow deleted." ), tr ( "Shadow deleted." ), QMessageBox::Ok, QMessageBox::Cancel ) == QMessageBox::Cancel )
      return;
    m_pMenuObject->setShadow ( NULL );
    delete m_pShadow;
  }
  else
    m_pMenuObject->setShadow ( m_pShadow );
  m_pShadow = NULL;
  
  QDialog::accept ( );
}

void DialogShadow::reject ( )
{
  if ( m_pOrigShadow )
    m_pMenuObject->setShadow ( m_pOrigShadow );
  m_pOrigShadow = NULL;

  QDialog::reject ( );
}

void DialogShadow::keepStaticVariables ( )
{
  if ( ! m_pShadow )
    return;

  m_keepTransparency = m_pShadow->transparency ( );
  m_keepDistance     = m_pShadow->distance     ( );
  m_keepSunAngle     = m_pShadow->sunAngle     ( );
  m_keepColor        = m_pShadow->color        ( );
  m_keepBlurRadius   = m_pShadow->blur         ( );

  m_keepShearX       = m_pShadow->shearX       ( );
  m_keepShearY       = m_pShadow->shearY       ( );
  m_keepScaleX       = m_pShadow->scaleX       ( );
  m_keepScaleY       = m_pShadow->scaleY       ( );
  m_keepCutOff       = m_pShadow->cutOff       ( );
  m_keepCutOffRadius = m_pShadow->cutOffRadius ( );
  m_keepColorFade    = m_pShadow->colorFading  ( );
  m_keepTargetColor  = m_pShadow->colorFadeTo  ( );
}

void DialogShadow::setStaticVariables ( )
{
  if ( ! m_pShadow )
    return;
  
  m_pShadow->setTransparency ( m_keepTransparency );
  m_pShadow->setDistance     ( m_keepDistance     );
  m_pShadow->setSunAngle     ( m_keepSunAngle     );
  m_pShadow->setColor        ( m_keepColor        );
  m_pShadow->setBlur         ( m_keepBlurRadius   );

  m_pShadow->setShear        ( m_keepShearX,      m_keepShearY       );
  m_pShadow->setScale        ( m_keepScaleX,      m_keepScaleY       );
  m_pShadow->setCutOff       ( m_keepCutOff,      m_keepCutOffRadius );
  m_pShadow->setColorFading  ( m_keepTargetColor, m_keepColorFade    );
}

// Called from the init timer ( oneShot ... )
void DialogShadow::slotCenterScrollView ( )
{
  drawShadow ( );

  int iScrollToX, iScrollToY;
  iScrollToX = m_pMenuObject->rect ( ).x ( ) - 15;
  if ( iScrollToX < 1 )
       iScrollToX = 1;
  iScrollToY = m_pMenuObject->rect ( ).y ( ) - 15;
  if ( iScrollToY < 1 )
       iScrollToY = 1;
  //m_pScrollView->setContentsPos ( iScrollToX, iScrollToY );   //ooo
  m_pScrollView->move ( iScrollToX, iScrollToY );               //xxx

  m_iAdvancedWidth = m_pFrameMirror->width ( );

  if ( m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR ) {
    //m_pComboStyle ->setCurrentItem ( 1 ); //ooo
    m_pComboStyle ->setCurrentIndex ( 1 );  //xxx
    m_pGroupBoxColor->setTitle ( "Target Color" );
  }
  else {
    displayAdvanced ( false );
    m_pSliderColorFade->hide ( );
  }
}

void DialogShadow::displayAdvanced ( bool bDisplayOrHide )
{
  QRect r = QRect ( geometry ( ) );
  if ( ( ! bDisplayOrHide ) && ( m_pFrameMirror->isVisible ( ) ) ) {
    // Type = Shadow
    m_pFrameMirror->hide ( );
    m_pSliderColorFade->hide ( );
    m_pGroupBoxColor->setTitle ( tr ( "Shadow Color" ) );
    setGeometry ( r.x ( ) + m_iAdvancedWidth, r.y ( ), r.width ( ) - m_iAdvancedWidth, r.height ( ) );
  }
  else if ( ( bDisplayOrHide ) && ( ! m_pFrameMirror->isVisible ( ) ) ) {
    m_pFrameMirror->show ( );
    m_pSliderColorFade->show ( );
    m_pGroupBoxColor->setTitle ( tr ( "Target Color" ) );
    setGeometry ( r.x ( ) - m_iAdvancedWidth, r.y ( ), r.width ( ) + m_iAdvancedWidth, r.height ( ) );
  }
}

void DialogShadow::slotShadowColor ( )
{ 
  QColor newColor = QColorDialog::getColor ( m_pShadow->color ( ), this );    //ooo
  //QColor newColor = QColorDialog::getColor ( QColor(45,60,230), this );         //xxx

  //m_pLabelColor->setPaletteBackgroundColor ( newColor );      //ooo
  QPalette palette;							                    //xxx
  //palette.setColor(m_pLabelColor->backgroundRole(), newColor);	//xxx
  palette.setColor(QPalette::Window, newColor);	//xxx
  //palette.setBrush(m_pLabelColor->backgroundRole(), QBrush(newColor));	//xxx
  //palette.setBrush(m_pLabelColor->backgroundRole(), newColor);	//xxx
  //palette.setBrush(QPalette::Base, newColor);	//xxx
  m_pLabelColor->setPalette(palette);					        //xxx
  
  m_pShadow->setColor        ( newColor );  //xxxx

  if ( m_pShadow && m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR ) {
    float fColorFading = m_pShadow->colorFading ( );
    m_pShadow->setColorFading ( newColor, fColorFading );
  }
  refresh ( );
}

void DialogShadow::slotColorFade ( int iValue )
{
  if ( m_pShadow ) {
    //float  fColorFade = (float)iValue / 1000.0;   //ooo
    float  fColorFade = static_cast<float>(iValue) / 1000.0;    //xxx
    QColor theColor   = m_pShadow->colorFadeTo ( );
    m_pShadow->setColorFading ( theColor, fColorFade );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

int DialogShadow::getDialogType ( )
{
  if ( m_pShadow )
    return m_pShadow->type ( );
  return -1;
}

// called from m_pSliderTransparency
void DialogShadow::slotTransparency ( int iTransparency ) // [ 0 .. 10000 ]
{
  QString qsText;
  float   fValue = iTransparency / 100.0;

  qsText.sprintf ( "<B><CENTER>%0.2f%%</CENTER></B>", fValue );
  m_pLabelTransparency->setText ( qsText );
  if ( m_pShadow && m_pShadow->type ( ) == ShadowObject::TYPE_SHADOW )
    refresh ( );
}

void DialogShadow::slotTransparency ( )
{
  if (  m_pShadow && m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR )
    refresh ( );
}

void DialogShadow::slotDistance ( const QString &qsDistance )
{
  bool bOkay = false;
  int iDistance = qsDistance.toInt ( &bOkay );
  if ( bOkay )
    m_pSliderDistance->setValue ( iDistance );
}

// called from m_pSliderDistance
void DialogShadow::slotDistance ( int iDistance ) // [ 0 .. 100 ]
{
  QString qsText;
  int iDist = iDistance;
//  if ( ( iDistance < 3 ) && ( iDistance > -3 ) )
//    iDist = 0;

  qsText.sprintf      ( "%d", iDist );
  m_pEditDistance->setText ( qsText );

  if ( m_pShadow ) {
    m_pShadow->setDistance  ((float) iDistance );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

// called from m_pSliderBlur
void DialogShadow::slotBlur ( int iBlur ) // [ 0 .. 50 ]
{
  QString qsText;
  if ( iBlur < 3 )  
       iBlur = 0;
  //qsText.sprintf ( tr ("<B><CENTER>Blur radius [%d]</CENTER></B>" ), iBlur );     //oooo
  qsText.sprintf ( "<B><CENTER>Blur radius [%d]</CENTER></B>", iBlur );     //xxx
  m_pLabelBlur->setText ( qsText );
  
  //  printf ( "%s::%s::%d shadow<%p>\n", __FILE__, __FUNCTION__, __LINE__, m_pShadow );
  if (  m_pShadow && m_pShadow->type ( ) == ShadowObject::TYPE_SHADOW )
    refresh ( );
  else if ( m_pShadow ) {
    m_pShadow->setBlur ( iBlur );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

void DialogShadow::slotStyle ( int iStyle )
{
  if ( ! m_pShadow )
    return;
  if ( iStyle == 1 ) { // Mirror
    m_pShadow->setType ( ShadowObject::TYPE_MIRROR );
    displayAdvanced ( true );
  }
  else if ( iStyle == 2 ) { // Reflection
    m_pShadow->setType ( ShadowObject::TYPE_REFLECTION );
    displayAdvanced ( false );
  }
  else { // Shadow
    m_pShadow->setType ( ShadowObject::TYPE_SHADOW );
    slotSunAngle ( m_pDialSunAngle->value ( ) );
    displayAdvanced ( false );
  }
  refresh ( );
}

// called from m_pDialSunAngle
void DialogShadow::slotSunAngle ( int iAngle )
{
  // The angle is between 0 and 360.00
  float fAngle = (float) ( iAngle - 27000 ) / 100.0;
  QString strPos = tr ( "RIGHT" );
  if  ( fAngle  <   0.0 )
        fAngle += 360.0;

  if ( m_pShadow &&  m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR ) {
    if ( ( fAngle > 225.0 ) && ( fAngle < 315.0 ) )
      strPos = tr ( "TOP" );
    else if ( ( fAngle > 135.0 ) && ( fAngle <= 225.0 ) )
      strPos = tr ( "LEFT" );
    else if ( ( fAngle > 45.0 ) && ( fAngle <= 135.0 ) )
      strPos = tr ( "BOT" );
  }
  
  QString qsText;
  if ( m_pShadow &&  m_pShadow->type ( ) == ShadowObject::TYPE_SHADOW )
    qsText.sprintf ( "<p align=\"center\"><b>SunAngle [%0.2f%%]</b></p>", fAngle );
  else
    //qsText.sprintf ( "<p align=\"center\"><b>Position [%s]</b></p>", strPos.ascii ( ) );          //ooo
    qsText.sprintf ( "<p align=\"center\"><b>Position [%s]</b></p>", strPos.toLatin1().data ( ) );  //xxx
  m_pLabelSunAngle->setText ( qsText );

  refresh ( );
}

void DialogShadow::slotShearX ( int iValue )
{
  if ( ( iValue > -10 ) && ( iValue < 10 ) )
    iValue = 0;
  if ( m_pShadow ) {
    float fShearX, fShearY;
    fShearX = (float)iValue / 500.0;
    fShearY = m_pShadow->shearY ( );
    m_pShadow->setShear ( fShearX, fShearY );
    m_pEditShearX->setText  ( tr ( "%1").arg ( fShearX ) );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

void DialogShadow::slotShearY ( int iValue )
{
  if ( ( iValue > -10 ) && ( iValue < 10 ) )
    iValue = 0;
  if ( m_pShadow ) {
    float fShearX, fShearY;
    fShearX = m_pShadow->shearX ( );
    fShearY = (float)iValue / 500.0;
    m_pShadow->setShear ( fShearX, fShearY );
    m_pEditShearY->setText  ( tr ( "%1").arg ( fShearY ) );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

void DialogShadow::slotScaleX ( int iValue )
{
  if ( ( iValue > -10 ) && ( iValue < 10 ) )
    iValue = 0;
  if ( m_pShadow ) {
    float fScaleX, fScaleY;
    fScaleX = (float)iValue / 1000.0;
    fScaleY = m_pShadow->scaleY ( );
    m_pShadow->setScale ( fScaleX, fScaleY );
    m_pEditScaleX->setText  ( tr ( "%1").arg ( fScaleX ) );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

void DialogShadow::slotScaleY ( int iValue )
{
  if ( ( iValue > -10 ) && ( iValue < 10 ) )
    iValue = 0;
  if ( m_pShadow ) {
    float fScaleX, fScaleY;
    fScaleX = m_pShadow->scaleX ( );
    //fScaleY = (float)iValue / 1000.0; //ooo
    fScaleY = static_cast<float>(iValue) / 1000.0;  //xxx
    m_pShadow->setScale ( fScaleX, fScaleY );
    m_pEditScaleY->setText  ( tr ( "%1").arg ( fScaleY ) );
    m_pShadow->updateShadow ( false );
    drawShadow ( );
  }
}

void DialogShadow::slotCutOff ( int iValue )
{
  //float fCutOff = (float)iValue / 1000.0; //ooo
  float fCutOff = static_cast<float>(iValue) / 1000.0;      //xxx
  m_pEditCutOff->setText  ( tr ( "%1").arg ( fCutOff ) );
}

void DialogShadow::slotCutOff ( )
{
  int iValue = m_pSliderCutOff->value ( );
  if ( m_pShadow ) {
    float fCutOff, fCutOffRadius;
    //fCutOff       = (float)iValue / 1000.0;   //ooo
    fCutOff       = static_cast<float>(iValue) / 1000.0;    //xxx
    fCutOffRadius = m_pShadow->cutOffRadius ( );
    m_pShadow->setCutOff ( fCutOff, fCutOffRadius );
    m_pEditCutOff->setText  ( tr ( "%1").arg ( fCutOff ) );
    m_pShadow->updateShadow ( true );
    drawShadow ( );
  }
}

void DialogShadow::slotCutOffRadius ( int iValue )
{
  float fCutOffRadius = (float)iValue / 1000.0;
  m_pEditCutOffRadius->setText  ( tr ( "%1").arg ( fCutOffRadius ) );
}

void DialogShadow::slotCutOffRadius ( )
{
  int iValue = m_pSliderCutOffRadius->value ( );
  if ( m_pShadow ) {
    float fCutOff, fCutOffRadius;
    fCutOff       = m_pShadow->cutOff ( );
    //fCutOffRadius = (float)iValue / 1000.0;   //ooo
    fCutOffRadius = static_cast<float>(iValue) / 1000.0;    //xxx
    m_pShadow->setCutOff ( fCutOff, fCutOffRadius );
    m_pEditCutOffRadius->setText  ( tr ( "%1").arg ( fCutOffRadius ) );
    m_pShadow->updateShadow ( true );
    drawShadow ( );
  }
}

void DialogShadow::refresh ( )
{
  if ( ( ! m_pMenuObject ) || ( ! m_pShadow ) )
    return;

  //QColor backgroundColor =   m_pLabelColor->paletteBackgroundColor ( );           //ooo
  //QPalette palette;	//xxx
  //QColor backgroundColor =   palette.color ( m_pLabelColor->backgroundRole ( ) );   //xxx
  //QColor backgroundColor =   QColor ( m_pLabelColor->backgroundRole ( ) );   //xxx
  QColor backgroundColor =   QColor ( m_pShadow->color ( ) );   //xxx
  //QColor backgroundColor =   QColor(45,60,230);   //xxx

  float  fTransparency   =   m_pSliderTransparency->value ( ) / 10000.0;
  int    iBlurRadius     =   m_pSliderBlur->value   ( );
  float  fDistance       =   m_pEditDistance->text  ( ).toFloat ( );
  float  fSunAngle       = ( m_pDialSunAngle->value ( ) - 27000 ) / 100.0;
  if ( m_pShadow &&  m_pShadow->type ( ) == ShadowObject::TYPE_MIRROR )
    fSunAngle += 90.0;

  if ( fSunAngle < 0.0 )
       fSunAngle += 360.0;
  else if ( fSunAngle > 360.0 )
       fSunAngle -= 360.0;

  m_pShadow->setTransparency ( fTransparency );
  m_pShadow->setColor        ( backgroundColor );
  m_pShadow->setBlur         ( iBlurRadius );
  m_pShadow->setDistance     ( fDistance );
  m_pShadow->setSunAngle     ( fSunAngle );

  m_pShadow->updateShadow ( true );
  drawShadow ( );
}

void DialogShadow::drawShadow ( )
{
  QPixmap  thePixmap = *m_pBackground;
  QPainter thePainter ( &thePixmap );

  m_pShadow    ->drawContents ( &thePainter );
  m_pMenuObject->drawContents ( &thePainter );

  m_pPreview->setScaledContents         ( true );			//xxx	
  m_pPreview->setAutoFillBackground     ( true );			//xxx
  //m_pPreview->setPaletteBackgroundPixmap  ( thePixmap );		//ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(thePixmap));		//xxx
  //palette.setBrush(m_pPreview->backgroundRole(), QBrush(thePixmap));		//xxx
  m_pPreview->setPalette(palette);					//xxx
  m_pPreview->resize ( thePixmap.width ( ), thePixmap.height ( ) );
}


