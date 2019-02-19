/***************************************************************************
    dialogimage2.cpp
                             -------------------
                             
    DialogImage2 - class implementation
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class handles the DialogImage - dialog for QDVDAutor.
    
    Note that QSlideshow has its own class, GUI to handle the same interface
    
****************************************************************************/

#include <qdesktopwidget.h>
#include <qpushbutton.h>
//#include <q3scrollview.h> //ooo
#include <QScrollArea>      //xxx
//#include <q3popupmenu.h>  //ooo
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qtimer.h>
#include <qlabel.h>
//Added by qt3to4:
//#include <Q3GridLayout>   //ooo
#include <QGridLayout>   //xxx
#include <QMouseEvent>

#include "modifiers.h"
//#include "dialogshadow.h" //ooo
#include "dialogimage2.h"
#include "menuobject.h"
#include "imageobject.h"
#include "movieobject.h"

//DialogImage2::ImagePreview::ImagePreview ( ImageObject *pImageObject, DialogImage2 *pDialog, QWidget *pParent, const char *pName, Qt::WFlags theFlags )   //ooo
DialogImage2::ImagePreview::ImagePreview ( ImageObject *pImageObject, DialogImage2 *pDialog, QWidget *pParent )                                             //xxx
  //: QLabel ( pParent, pName, theFlags )   //ooo
  : QLabel ( pParent )                      //xxx
{
  m_pParent      = pDialog;
  m_pImageObject = pImageObject;
}

DialogImage2::ImagePreview::~ImagePreview ( )
{
}

void DialogImage2::ImagePreview::mousePressEvent ( QMouseEvent *pEvent )
{
  if ( pEvent->button ( ) == Qt::LeftButton )    {
    m_pParent->createActiveBackground ( true );
    m_pImageObject->mousePressEvent ( pEvent );
    return;
  }
    
/*  QString qsShadow;
  qsShadow = tr ( "Add Shadow ..." );
  if ( m_pImageObject->shadow ( ) )
     qsShadow = tr ( "Edit Shadow ..." );

  Q3PopupMenu *pMenu = new Q3PopupMenu (this);
  pMenu->insertItem ( tr ("Properties ..."),  m_pParent, SLOT ( slotButtonProperties ( ) ) );
  pMenu->insertItem ( qsShadow,               m_pParent, SLOT ( slotButtonShadow     ( ) ) );
  pMenu->insertSeparator ();
  pMenu->insertItem ( tr ("Rotate 90") ,      m_pParent, SLOT ( slotRotate90   ( ) ) );
  pMenu->insertItem ( tr ("Rotate 180"),      m_pParent, SLOT ( slotRotate180  ( ) ) );
  pMenu->insertItem ( tr ("Rotate 270"),      m_pParent, SLOT ( slotRotate270  ( ) ) );
  pMenu->exec ( pEvent->globalPos ( ) );
  */    //ooo
}

void DialogImage2::ImagePreview::mouseReleaseEvent (QMouseEvent * )
{
  m_pImageObject->setActive ( false );
  m_pParent->createActiveBackground ( false );
  //m_pImageObject->mouseReleaseEvent ( pEvent );
  m_pParent->redrawObject ( );
}

void DialogImage2::ImagePreview::mouseMoveEvent ( QMouseEvent *pEvent )
{
  static bool bActive = false;
  if ( bActive )
    return;
  
  bActive = true;
  m_pImageObject->mouseMoveEvent ( pEvent );
  m_pParent->redrawObject ( );
  bActive = false;
}

void DialogImage2::ImagePreview::mouseDoubleClickEvent (QMouseEvent *)
{
  m_pParent->slotButtonProperties ( );
}

//DialogImage2::DialogImage2 ( QWidget *pParent, const char *pName, bool bModal, Qt::WFlags flags )	//ooo
DialogImage2::DialogImage2 ( QWidget *pParent )								//xxx
    //: uiDialogImage ( pParent, pName, bModal, flags )		//ooo
    : QDialog ( pParent )					//xxx
{
  setupUi(this);		//xxx
  m_pImagePreview    = NULL;
  m_pImageObject     = NULL;
  m_pOrigImageObject = NULL;
  m_pScrollView      = NULL;
  m_fOrigScaleX      = 1.0;   // ImageObjects are scaled using fScaleX, fScaleY
  m_fOrigScaleY      = 1.0;   // m_fZoom simply multiplies their current values

  m_pSliderRotate->setRange       ( -3600, 3600 );
  m_pSliderShearX->setRange       ( -1000, 1000 );
  m_pSliderShearY->setRange       ( -1000, 1000 );
  m_pSliderZoom->setRange         ( -1000, 1000 );
  m_pSliderBrightness->setRange   ( -1000, 1000 );
  m_pSliderRed->setRange          (     0, 1000 );
  m_pSliderGreen->setRange        (     0, 1000 );
  m_pSliderBlue->setRange         (     0, 1000 );
  m_pSliderTransparency->setRange (     0, 1000 );

  m_pSliderRotate->setValue       ( 0 );
  m_pSliderShearX->setValue       ( 0 );
  m_pSliderShearY->setValue       ( 0 );
  m_pSliderZoom->setValue         ( 0 );
  m_pSliderRed->setValue          ( 0 );
  m_pSliderGreen->setValue        ( 0 );
  m_pSliderBlue->setValue         ( 0 );
  m_pSliderTransparency->setValue ( 0 );
  m_pSliderBrightness->setValue   ( 0 );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( m_pSliderShearY, SIGNAL( valueChanged(int) ),     this, SLOT( slotShearYValueChanged(int) ) );
  connect( m_pSliderShearX, SIGNAL( valueChanged(int) ),     this, SLOT( slotShearXValueChanged(int) ) );
  connect( m_pSliderRotate, SIGNAL( valueChanged(int) ),     this, SLOT( slotRotateValueChanged(int) ) );
  connect( m_pSliderZoom, SIGNAL( valueChanged(int) ),       this, SLOT( slotZoomValueChanged(int) ) );
  connect( m_pSliderRed, SIGNAL( valueChanged(int) ),        this, SLOT( slotRedValueChanged(int) ) );
  connect( m_pSliderGreen, SIGNAL( valueChanged(int) ),      this, SLOT( slotGreenValueChanged(int) ) );
  connect( m_pSliderBlue, SIGNAL( valueChanged(int) ),       this, SLOT( slotBlueValueChanged(int) ) );
  connect( m_pSliderBrightness, SIGNAL( valueChanged(int) ), this, SLOT( slotBrightnessValueChanged(int) ) );
  connect( m_pButtonProperties, SIGNAL( clicked() ),         this, SLOT( slotButtonProperties() ) );
  connect( m_pButtonShadow, SIGNAL( clicked() ),             this, SLOT( slotButtonShadow() ) );
  connect( m_pSliderRotate, SIGNAL( sliderReleased() ),      this, SLOT( slotSliderReleased() ) );
  connect( m_pSliderShearX, SIGNAL( sliderReleased() ),      this, SLOT( slotSliderReleased() ) );
  connect( m_pSliderShearY, SIGNAL( sliderReleased() ),      this, SLOT( slotSliderReleased() ) );
  connect( m_pSliderZoom, SIGNAL( sliderReleased() ),        this, SLOT( slotSliderReleased() ) );
  connect( m_pSliderRotate, SIGNAL( sliderPressed() ),       this, SLOT( slotSliderPressed() ) );
  connect( m_pSliderShearX, SIGNAL( sliderPressed() ),       this, SLOT( slotSliderPressed() ) );
  connect( m_pSliderShearY, SIGNAL( sliderPressed() ),       this, SLOT( slotSliderPressed() ) );
  connect( m_pSliderZoom, SIGNAL( sliderPressed() ),         this, SLOT( slotSliderPressed() ) );
  connect( m_pSliderTransparency, SIGNAL( valueChanged(int)),this, SLOT( slotTransparencyValueChanged(int) ) );
  connect( m_pEditStatus, SIGNAL( textChanged(const QString&) ), this, SLOT( slotValueChanged(const QString&) ) );
  //  connect( m_pSliderRed, SIGNAL( sliderPressed() ),          this, SLOT( slotSliderPressed2() ) );
  //  connect( m_pSliderGreen, SIGNAL( sliderPressed() ),        this, SLOT( slotSliderPressed2() ) );
  //  connect( m_pSliderBlue, SIGNAL( sliderPressed() ),         this, SLOT( slotSliderPressed2() ) );
  //  connect( m_pSliderBrightness, SIGNAL( sliderPressed() ),   this, SLOT( slotSliderPressed2() ) );
  //  connect( m_pSliderTransparency, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed2() ) );
  connect( m_pSliderRed, SIGNAL( sliderReleased() ),         this, SLOT( slotSliderReleased2() ) );
  connect( m_pSliderGreen, SIGNAL( sliderReleased() ),       this, SLOT( slotSliderReleased2() ) );
  connect( m_pSliderBlue, SIGNAL( sliderReleased() ),        this, SLOT( slotSliderReleased2() ) );
  connect( m_pSliderBrightness, SIGNAL( sliderReleased() ),  this, SLOT( slotSliderReleased2() ) );
  connect( m_pSliderTransparency, SIGNAL( sliderReleased() ),this, SLOT( slotSliderReleased2() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogImage2::~DialogImage2 ( )
{
  if ( m_pImageObject )
    delete m_pImageObject;
}

void DialogImage2::initMe ( ImageObject *pImageObject, QPixmap *pBackground )
{
  // Here we get the available screen resolution (all screens)
  // We want to keep the old style for resolutions above 1024x768 
  // But for lower res we need to use a scrollview to be able to
  // use QDVDAuthor.
  int iScreenWidth, iScreenHeight, iScreens, t;
  QDesktopWidget desktop;
  //Q3GridLayout   *pLayout = new Q3GridLayout (m_pFrameImage, 1, 1, 2, 2, "m_pFrameImageLayout");  //ooo
  QGridLayout   *pLayout = new QGridLayout (m_pFrameImage);                                       //xxx
  iScreenWidth = iScreenHeight = 0;
  iScreens = desktop.numScreens();
  for (t=0;t<iScreens;t++)	{
    iScreenWidth  += desktop.screenGeometry(t).width();
    iScreenHeight += desktop.screenGeometry(t).height();
  }
  //uint iFlags = 0;		//ooo
//#if (QT_VERSION > 0x0301FF)	//ooo
//  iFlags = Qt::WNoAutoErase;
//#else
//  iFlags = Qt::WPaintClever;
//#endif
  
  //Qt::WindowFlags iFlags = Qt::WNoAutoErase;	//xxx

  m_pOrigImageObject = pImageObject;
  m_pImageObject     = (ImageObject *) pImageObject->clone ( );
  ImageManipulator &theManipulator = m_pImageObject->manipulator ( );
  m_fOrigScaleX = theManipulator.fScaleX;
  m_fOrigScaleY = theManipulator.fScaleY;

  if ( (iScreenHeight <= 768) || (iScreenWidth <= 1024) )	{
    // Here we embed the MenuPreview into a ScrollView object ...
    //m_pScrollView   = new Q3ScrollView  ( m_pFrameImage, "m_pScrollView",   iFlags ); //ooo
    m_pScrollView   = new QScrollArea  ( m_pFrameImage );                              //xxx
    //m_pImagePreview = new ImagePreview ( m_pImageObject, this, m_pScrollView, "m_pImagePreview", iFlags );    //ooo
    m_pImagePreview = new ImagePreview ( m_pImageObject, this, m_pScrollView );                                 //xxx
    //m_pScrollView->addChild            ( m_pImagePreview );   //ooo
    m_pScrollView->setWidget            ( m_pImagePreview );    //xxx
    m_pImagePreview->resize            ( 720, 480 );
    pLayout->addWidget                 ( m_pScrollView, 0, 0 );
  }
  else	{
    //m_pImagePreview = new ImagePreview ( m_pImageObject, this, m_pFrameImage, "m_pPreview", iFlags ); //ooo
    m_pImagePreview = new ImagePreview ( m_pImageObject, this, m_pFrameImage );                         //xxx
    pLayout->addWidget                 ( m_pImagePreview, 0, 0 );
  }
  
  if ( pBackground && ( pBackground->width () > 10 ) && ( pBackground->height () > 10 ) ) {
    m_backgroundBuffer = *pBackground;
    m_pImagePreview->setScaledContents         ( true );		//xxx	
    m_pImagePreview->setAutoFillBackground     ( true );		//xxx
    //m_pImagePreview->setPaletteBackgroundPixmap ( *pBackground );	//ooo
    QPalette palette;							//xxx
    palette.setBrush(QPalette::Window, QBrush(*pBackground));		//xxx
    m_pImagePreview->setPalette(palette);				//xxx
    m_pImagePreview->setFixedWidth  ( pBackground->width  ( ) );
    m_pImagePreview->setFixedHeight ( pBackground->height ( ) );
  }
  m_pLabelStatus ->setText ( tr("Zoom") );
  m_pEditStatus  ->setText ( "1.0" );

  if ( ! pImageObject->shadow ( ) )
    m_pButtonShadow->setText ( tr ( "Add Shadow ..." ) );

  m_pSliderRotate->setValue       ( (int)( theManipulator.fRotate       *   10.0 ) );
  m_pSliderShearX->setValue       ( (int)( theManipulator.fShearX       * 1000.0 ) );
  m_pSliderShearY->setValue       ( (int)( theManipulator.fShearY       * 1000.0 ) );
  //m_pSliderZoom->setValue         ( (int)( theManipulator.fZoom         * 1000.0 ) - 1000 );
  //m_pSliderZoom->setValue         ( (int)( theManipulator.fScaleX       * 1000.0 ) - 1000 );
  m_pSliderZoom->setValue         (  0 ); // zoom always starts at 0 [-1000 .. +1000]
  m_pSliderRed->setValue          ( (int)( 1000 - theManipulator.fRed   * 1000.0 ) );
  m_pSliderGreen->setValue        ( (int)( 1000 - theManipulator.fGreen * 1000.0 ) );
  m_pSliderBlue->setValue         ( (int)( 1000 - theManipulator.fBlue  * 1000.0 ) );
  m_pSliderTransparency->setValue ( (int)( theManipulator.fTransparency * 1000.0 ) );
  m_pSliderBrightness->setValue   ( (int)( - theManipulator.fBrightness * 1000.0 ) );

  // and finally we draw the active object ...
  redrawObject ( );

  QTimer::singleShot ( 0, this, SLOT (slotCenterScrollView ( ) ) );
}

void DialogImage2::slotOk ( )
{
  MenuObject *pShadow = NULL;
  MenuObject *pCurrentShadow = m_pImageObject->shadow ( );
  m_pOrigImageObject->setManipulator ( m_pImageObject->manipulator ( ) );

  if ( pCurrentShadow )
    pShadow = pCurrentShadow->clone ( NULL, m_pOrigImageObject );
  m_pOrigImageObject->setRect       ( m_pImageObject->rect ( ) );
  m_pOrigImageObject->setShadow     ( pShadow );
  m_pOrigImageObject->setAnimation  ( m_pImageObject->animation ( ) );
  m_pOrigImageObject->updatePixmap  ( );
  
  //uiDialogImage::accept ( );	//ooo
  QDialog::accept ( );		//xxx
}

void DialogImage2::reject ( )
{
  //uiDialogImage::reject ( );	//ooo
  QDialog::reject ( );		//xxx
}

void DialogImage2::slotRotate90 ( )
{
  m_pImageObject->manipulator ( ).fRotate += 90.0;
  m_pImageObject->updatePixmap (       );
  redrawObject ( );
}

void DialogImage2::slotRotate180 ( )
{
  m_pImageObject->manipulator ( ).fRotate += 180.0;
  m_pImageObject->updatePixmap (       );
  redrawObject ( );
}

void DialogImage2::slotRotate270 ( )
{
  m_pImageObject->manipulator ( ).fRotate += 270.0;
  m_pImageObject->updatePixmap (       );
  redrawObject ( );
}

void DialogImage2::slotCenterScrollView ( )
{
  if ( m_pScrollView ) {
    int iScrollToX, iScrollToY;
    iScrollToX = m_pImageObject->rect ( ).x ( ) - 15;
    if ( iScrollToX < 1 )
      iScrollToX = 1;
    iScrollToY = m_pImageObject->rect ( ).y ( ) - 15;
    if ( iScrollToY < 1 )
      iScrollToY = 1;
    //m_pScrollView->setContentsPos ( iScrollToX, iScrollToY ); //ooo
    //m_pScrollView->scrollContentsBy ( iScrollToX, iScrollToY ); //xxx
    m_pScrollView->move ( iScrollToX, iScrollToY );             //xxx
  }
}

void DialogImage2::redrawObject ( )
{
  if ( m_pImageObject ) {
    QPixmap  tempPixmap;
    if ( m_pImageObject->isActive ( ) )
      tempPixmap = m_activeBackgroundBuffer;
    else
      tempPixmap = m_backgroundBuffer;

    QPainter thePainter ( &tempPixmap );
    m_pImageObject->drawContents ( &thePainter );
    m_pImagePreview->setScaledContents         ( true );		//xxx	
    m_pImagePreview->setAutoFillBackground     ( true );		//xxx
    //m_pImagePreview->setPaletteBackgroundPixmap ( tempPixmap );	//ooo
    QPalette palette;							//xxx
    palette.setBrush(QPalette::Window, QBrush(tempPixmap));		//xxx
    m_pImagePreview->setPalette(palette);				//xxx
  }
}

void DialogImage2::slotShearXValueChanged( int iValue )
{
  float fShearX = (float)iValue / 1000.0;
  m_pLabelStatus->setText(tr ("ShearX"));
  m_pEditStatus->setText (QString ("%1").arg(fShearX));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fShearX = fShearX;
  redrawObject ( );
}

void DialogImage2::slotShearYValueChanged( int iValue )
{
  float fShearY = (float)iValue / 1000.0;
  m_pLabelStatus->setText(tr ("ShearY"));
  m_pEditStatus->setText (QString ("%1").arg(fShearY));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fShearY = fShearY;
  redrawObject ( );
}

void DialogImage2::slotRotateValueChanged( int iValue )
{
  float fRotation = (float) iValue / 10.0;
  m_pLabelStatus->setText(tr ("Rotate"));
  m_pEditStatus->setText (QString ("%1").arg(fRotation));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fRotate = fRotation;
  redrawObject ( );
}

void DialogImage2::slotTransparencyValueChanged( int iValue )
{
  float fTransparency = (float) iValue / 1000.0;
  m_pLabelStatus->setText(tr ("Tarnsparency"));
  m_pEditStatus->setText (QString ("%1").arg(fTransparency));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fTransparency = fTransparency;
  redrawObject ( );
}

void DialogImage2::slotZoomValueChanged( int iValue )
{
  if ( iValue > 0 )
       iValue *= 3;

  float fZoom = (float)(1000.0+iValue) / 1000.0;
  m_pLabelStatus->setText(tr ("Zoom"));
  m_pEditStatus->setText (QString ("%1").arg(fZoom));
  if ( m_pImageObject ) {
    //m_pImageObject->manipulator ( ).fZoom = fZoom;
    m_pImageObject->manipulator ( ).fScaleX = m_fOrigScaleX * fZoom;
    m_pImageObject->manipulator ( ).fScaleY = m_fOrigScaleY * fZoom;
  }
  redrawObject ( );
}

void DialogImage2::slotBrightnessValueChanged( int iValue )
{
  //float fBrightness = (float)iValue / 1000.0;                 //ooo
  float fBrightness = static_cast<float> ( iValue ) / 1000.0;   //xxx
  m_pLabelStatus->setText(tr ("Brightness"));
  m_pEditStatus->setText (QString ("%1").arg(fBrightness));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fBrightness = -fBrightness;
  redrawObject ( );
}

void DialogImage2::slotRedValueChanged( int iValue )
{
  //float fRed = 1.0 - (float)iValue / 1000.0;                  //ooo
  float fRed = 1.0 - static_cast<float> ( iValue ) / 1000.0;  //xxx
  m_pLabelStatus->setText(tr ("Red"));
  m_pEditStatus->setText (QString ("%1").arg(fRed));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fRed = fRed;
  redrawObject ( );
}

void DialogImage2::slotGreenValueChanged( int iValue )
{
  //float fGreen = 1.0 - (float)iValue / 1000.0;                    //ooo
  float fGreen = 1.0 - static_cast<float> ( iValue ) / 1000.0;      //xxx
  m_pLabelStatus->setText(tr ("Green"));
  m_pEditStatus->setText (QString ("%1").arg(fGreen));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fGreen = fGreen;
  redrawObject ( );
}

void DialogImage2::slotBlueValueChanged( int iValue )
{
  //float fBlue = 1.0 - (float)iValue / 1000.0;                 //ooo
  float fBlue = 1.0 - static_cast<float> ( iValue ) / 1000.0;   //xxx
  m_pLabelStatus->setText(tr ("Blue"));
  m_pEditStatus->setText (QString ("%1").arg(fBlue));
  if ( m_pImageObject )
       m_pImageObject->manipulator ( ).fBlue = fBlue;
  redrawObject ( );
}

void DialogImage2::slotComboActivated ( int iNewMode )
{
  if ( m_pImageObject )  
       m_pImageObject->manipulator ( ).iTransformationMode = iNewMode;
  redrawObject ( );
}

void DialogImage2::slotSliderReleased()
{
  if ( m_pImageObject ) {
       m_pImageObject->setActive    ( false );
       createActiveBackground       ( false );
       m_pImageObject->updatePixmap (       );
       redrawObject ( );
  }
}

void DialogImage2::slotSliderPressed()
{
  if ( m_pImageObject ) {
    createActiveBackground    ( true );
    m_pImageObject->setActive ( true );
  }
}

void DialogImage2::slotSliderReleased2 ( )
{
  if ( m_pImageObject ) {
       m_pImageObject->updatePixmap (       );
       createActiveBackground       ( false );
       redrawObject ( );
  }
}

void DialogImage2::createActiveBackground ( bool bCreate )
{
  if ( ( m_pImageObject ) && ( bCreate ) ) {
    m_activeBackgroundBuffer = m_backgroundBuffer;
    QPainter thePainter ( &m_activeBackgroundBuffer );
    m_pImageObject->drawContents ( &thePainter );
  }
  else
    m_activeBackgroundBuffer = QPixmap ( );    
}

void DialogImage2::slotValueChanged( const QString & newText )
{
  // The user typed in a new value in the status. Thus we need to check which value was changed and do the do ...
  float fNewValue = newText.toFloat();
  if (m_pLabelStatus->text() == tr("ShearX"))
    //m_pSliderShearX->setValue((int)(fNewValue * 1000.0));             //ooo
    m_pSliderShearX->setValue(static_cast<int>(fNewValue * 1000.0));    //xxx
  else if (m_pLabelStatus->text() == tr("ShearY"))
    //m_pSliderShearY->setValue((int)(fNewValue * 1000.0));             //ooo
    m_pSliderShearY->setValue(static_cast<int>(fNewValue * 1000.0));    //xxx
  else if (m_pLabelStatus->text() == tr("Rotate"))
    //m_pSliderRotate->setValue((int)(fNewValue * 10.0));               //ooo
    m_pSliderRotate->setValue(static_cast<int>(fNewValue * 10.0));      //xxx
  else if (m_pLabelStatus->text() == tr("Zoom")) {
    if ( fNewValue > 1.0 )
      fNewValue = (fNewValue - 1.0) / 3.0 + 1.0;
    //m_pSliderZoom->setValue((int)(fNewValue * 1000.0)-1000);          //ooo
    m_pSliderZoom->setValue(static_cast<int>(fNewValue * 1000.0)-1000); //xxx
  }
  else if (m_pLabelStatus->text() == tr("Transparency"))
    //m_pSliderTransparency->setValue((int)(fNewValue * 1000.0));               //ooo
    m_pSliderTransparency->setValue(static_cast<int>(fNewValue * 1000.0));      //xxx
  else if (m_pLabelStatus->text() == tr("Brightness"))
    //m_pSliderBrightness->setValue((int)(fNewValue * 1000.0));                 //ooo
    m_pSliderBrightness->setValue(static_cast<int>(fNewValue * 1000.0));        //xxx
  else if (m_pLabelStatus->text() == tr("Red"))
    //m_pSliderRed->setValue((int)(1000 - fNewValue * 1000.0));                 //ooo
    m_pSliderRed->setValue(static_cast<int>(1000 - fNewValue * 1000.0));        //xxx
  else if (m_pLabelStatus->text() == tr("Green"))
    //m_pSliderGreen->setValue((int)(1000 - fNewValue * 1000.0));               //ooo
    m_pSliderGreen->setValue(static_cast<int>(1000 - fNewValue * 1000.0));      //xxx
  else if (m_pLabelStatus->text() == tr("Blue"))
    //m_pSliderBlue->setValue((int)(1000 - fNewValue * 1000.0));                //ooo
    m_pSliderBlue->setValue(static_cast<int>(1000 - fNewValue * 1000.0));       //xxx

  redrawObject ( );
}

void DialogImage2::slotButtonProperties ( )
{
  if ( m_pImageObject )
       m_pImageObject->slotProperties ( );

  redrawObject ( );
}

void DialogImage2::slotButtonShadow ( )
{
/*  DialogShadow dialog ( this );
  dialog.initMe       ( m_pImageObject, &m_backgroundBuffer );
  if ( dialog.exec    ( ) == QDialog::Rejected )
    return;

  if ( m_pImageObject->shadow ( ) )
    m_pButtonShadow->setText ( tr ( "Edit Shadow ..." ) );
  else
    m_pButtonShadow->setText ( tr ( "Add Shadow ..." ) );

  redrawObject ( );*/   //oooo
}


