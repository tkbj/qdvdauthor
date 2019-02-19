/***************************************************************************
    dialogframe.cpp
                             -------------------
    DialogFrame class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QRadioButton>
#include <QColorDialog>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QSpinBox>
#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QPen>
#include <QPalette> //xxx
#include <QMessageBox>
//Added by qt3to4:
#include <QPaintEvent>

#include "global.h"
#include "dialogframe.h"

//DialogFrame::DialogFrame(QWidget *parent, const char *name, Qt::WFlags f )	//ooo
DialogFrame::DialogFrame(QWidget *pParent)					//xxx
	//: uiDialogFrame (parent, name, f )	//ooo
	: QDialog(pParent)			//xxx
{  
  setupUi(this);	//xxx
  m_pOrigFrame  = NULL;
  m_iFrameWidth = 4;
  m_iFrameStyle = Qt::SolidLine;
  m_iFrameJoin  = Qt::MiterJoin;
  m_colorFrame  = Rgba ( START_FRAME_COLOR );
}

DialogFrame::~DialogFrame()
{
}

void DialogFrame::initMe( int iStyle, int iWidth, int iJoin, Rgba rgb )
{
  m_pFrameSelect->hide    ( );
  m_pGroupGeometry->hide  ( );
  m_pGroupAnimation->hide ( );
  //m_pGroupGeometry->setEnabled ( FALSE );	//ooo
  m_pGroupGeometry->setEnabled ( false );	//xxx

  //m_pFrameColor->setPaletteBackgroundColor ( rgb.color ( ) );	//ooo
  QPalette palette;						                           //xxx
  //palette.setBrush(QPalette::Base, QBrush(rgb.color ( )));	   //xxx
  //palette.setColor(m_pFrameColor->backgroundRole(), rgb.color ( ));	//xxx
  palette.setColor(QPalette::Window, rgb.color ( ));	//xxx
  m_pFrameColor->setAutoFillBackground(true);                       //xxx
  m_pFrameColor->setPalette(palette);				               //xxx
  m_colorFrame  = rgb;
  m_iFrameWidth = iWidth;
  m_iFrameStyle = iStyle;
  m_iFrameJoin  = iJoin;

  initMe ( );
}

void DialogFrame::initMe ( FrameObject *pFrame )
{
  // Here we continue with the standard initialization.
  //m_pFrameColor->setPaletteBackgroundColor ( pFrame->color ( ).color ( ) );	//ooo
  QPalette palette;								//xxx
  //palette.setBrush(QPalette::Base, QBrush(pFrame->color ( ).color ( )));	//xxx
  //palette.setColor(m_pFrameColor->backgroundRole(), pFrame->color ( ).color ( ));	//xxx
  palette.setColor(QPalette::Window, pFrame->color ( ).color ( ));	//xxx
  //palette.setBrush(m_pFrameColor->backgroundRole(), QColor( 0, 0, 0));	//xxx
  //palette.setBrush(QPalette::Window, pFrame->color ( ).color ( ));	//xxx
  m_pFrameColor->setAutoFillBackground(true);                       //xxx
  m_pFrameColor->setPalette(palette);						//xxx
  m_colorFrame  = pFrame->color ( );
  m_iFrameWidth = pFrame->width ( );
  m_qsAnimation = pFrame->animation ( );
  m_pOrigFrame  = pFrame;

  m_pEditX->setText     (QString ("%1").arg(pFrame->rect().x()));
  m_pEditY->setText     (QString ("%1").arg(pFrame->rect().y()));
  m_pEditWidth->setText (QString ("%1").arg(pFrame->rect().width()));
  m_pEditHeight->setText(QString ("%1").arg(pFrame->rect().height()));

  initMe ( );
}

void DialogFrame::initMe ( )
{ 
  m_pComboStyle->addItem ( QString ("SolidLine"      ) );
  m_pComboStyle->addItem ( QString ("DashLine"       ) );
  m_pComboStyle->addItem ( QString ("DashDotLine"    ) );
  m_pComboStyle->addItem ( QString ("DashDotDotLine" ) );
	
  m_pComboJoin->addItem  ( QString ("MiterJoin") );
  m_pComboJoin->addItem  ( QString ("BevelJoin") );
  m_pComboJoin->addItem  ( QString ("RoundJoin") );
  
  connect ( m_pButtonAnimation,SIGNAL ( clicked ( ) ), this, SLOT ( slotAnimation ( ) ) );
  connect ( m_pButtonColor,    SIGNAL ( clicked ( ) ), this, SLOT ( slotColor     ( ) ) );
  connect ( m_pButtonImage,    SIGNAL ( clicked ( ) ), this, SLOT ( slotImage     ( ) ) );
  connect ( m_pButtonPicture,  SIGNAL ( clicked ( ) ), this, SLOT ( slotPicture   ( ) ) );
  connect ( m_pRadioImage,     SIGNAL ( toggled(bool)),this, SLOT ( slotImage     ( bool ) ) );
  connect ( m_pRadioPicture,   SIGNAL ( toggled(bool)),this, SLOT ( slotPicture   ( bool ) ) );
  connect ( m_pSpinBoxWidth,   SIGNAL ( valueChanged(int)), this, SLOT(slotWidth  ( int  ) ) );
  connect ( m_pComboStyle,     SIGNAL ( highlighted (int)), this, SLOT(slotStyle  ( int  ) ) );
  connect ( m_pComboJoin,      SIGNAL ( highlighted (int)), this, SLOT(slotJoin   ( int  ) ) );
}

void DialogFrame::polish ( )
{
  //uiDialogFrame::polish ( );	//oooo
  //QDialog::polish ( );		//xxx
  //resize ( 250, 250 );
  //drawPreview ( );

  QTimer::singleShot ( 50, this, SLOT ( slotRefresh ( ) ) );
}

void DialogFrame::slotRefresh ( )
{
  //  resize ( 250, 250 );
  //drawPreview ( );    //ooo
  repaint();            //xxx  
}

void DialogFrame::slotColor ()
{  
  bool bOkay;
//  Rgba transparentColor = Rgba ( TRANSPARENT_COLOR );
//  QRgb rgbColor = QColorDialog::getRgba ( transparentColor.rgb ( ), &bOkay, this );
  QRgb rgbColor = QColorDialog::getRgba ( m_colorFrame.rgb ( ), &bOkay, this );
  if ( ! bOkay )
    return;

  Rgba newColor ( rgbColor );
  //m_pFrameColor->setPaletteBackgroundColor ( newColor.color ( ) );	//ooo
  QPalette palette;							//xxx
  //palette.setBrush(QPalette::Base, QBrush(newColor.color ( )));		//xxx
  //palette.setColor(m_pFrameColor->backgroundRole(), newColor.color ( ));		//xxx
  palette.setColor(QPalette::Window, newColor.color ( ));		//xxx
  m_pFrameColor->setPalette(palette);					//xxx
  m_colorFrame = newColor;
  //drawPreview ( );    //ooo
  repaint();            //xxx
}

void DialogFrame::slotImage()
{

}

void DialogFrame::slotPicture()
{

}

void DialogFrame::slotImage(bool)
{

}

void DialogFrame::slotPicture (bool)
{

}

void DialogFrame::slotAnimation ( )
{
  if ( m_pOrigFrame )
       m_pOrigFrame->animationDialog ( m_qsAnimation, DialogAnimation::TypeFrame );
}

QString &DialogFrame::animation ( )
{
  return m_qsAnimation;
}

void DialogFrame::slotWidth(int iNewValue)
{
	m_iFrameWidth = iNewValue;
	//drawPreview();   //ooo
    repaint();          //xxx
}

void DialogFrame::slotJoin(int iNewValue)
{
	switch (iNewValue)	{
	case 1:
		m_iFrameJoin = Qt::BevelJoin;
	break;
	case 2:
		m_iFrameJoin = Qt::RoundJoin;
	break;
	default:
		m_iFrameJoin = Qt::MiterJoin;
	}
	//drawPreview();   //ooo
	repaint();         //xxx
}

void DialogFrame::slotStyle(int iNewValue)
{
	switch (iNewValue)	{
	case 1:
		m_iFrameStyle = Qt::DashLine;
	break;
	case 2:
		m_iFrameStyle = Qt::DashDotLine;
	break;
	case 3:
		m_iFrameStyle = Qt::DashDotDotLine;
	break;
	default:
		m_iFrameStyle = Qt::SolidLine;
	}
	//drawPreview();   //ooo
	repaint();         //xxx
}

void DialogFrame::drawPreview()
{  
  //m_pPreview => QFrame
  QPainter thePainter (m_pPreview);
  QPen thePen ( color ( ).color ( ), frameWidth(), (Qt::PenStyle)style(), Qt::FlatCap, (Qt::PenJoinStyle)join());
  thePainter.setPen (thePen);
  thePainter.setBrush(Qt::NoBrush);

  //m_pPreview->repaint();  //oooo
  //m_pPreview->update();
  //m_pPreview->update();   //xxx

  // Note: the drawing range is 32 pixels
  QRect theRect (16, 16, m_pPreview->width()-32, m_pPreview->height()-32);
  thePainter.drawRect (theRect);
}

void DialogFrame::paintEvent (QPaintEvent *)
{
  // uiDialogFrame::paintEvent(pPainter);
  drawPreview ();
}

Rgba DialogFrame::color()
{
  return m_colorFrame;
}

int DialogFrame::style()
{
  return m_iFrameStyle;
}

int DialogFrame::join()
{
  return m_iFrameJoin;
}

int DialogFrame::frameWidth()
{
  return m_iFrameWidth;
}

