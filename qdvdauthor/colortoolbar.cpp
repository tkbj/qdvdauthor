/****************************************************************************
** StructureToolBar - class
**
**   Created : Tue May 08 12:09:08 2004
**        by : Varol Okan using kate editor
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** This class will create a toolbar, which holds only one widget.
** The StructureWidget is displaying the current structure of the DVD.
**
****************************************************************************/
//#include <QDockWidget>		//xxx
#include <QMainWindow>	//ooo
#include <QMessageBox>
#include <QColorDialog>
#include <QToolTip>
#include <QPainter>
//Added by qt3to4:
#include <QMouseEvent>
#include <QFrame>
#include <QPaintEvent>

#include "global.h"
#include "colortoolbar.h"
#include "dvdmenu.h"

QColorFrame::QColorFrame ( int iWhichColor, Rgba theColor, QWidget *pParent, const char *pName, Qt::WindowFlags f )	//ooo
//QColorFrame::QColorFrame ( QWidget *pParent, Qt::WindowFlags f )	//xxx
  //: QFrame ( pParent, pName, f )	//ooo
  : QFrame ( pParent, f )
{
  m_color       = theColor;
  m_iWhichColor = iWhichColor;
  //setPaletteBackgroundColor ( theColor.color ( ) );			//ooo
  QPalette palette;							//xxx
  palette.setBrush(this->backgroundRole(), theColor.color ( ));		//xxx
  this->setPalette(palette);						//xxx
  setFixedSize  ( QSize( 45, 45 ) );
  setFrameShape ( QFrame::StyledPanel );
  setFrameShadow( QFrame::Raised );
  //  setText       ( "1.33" );
  setLineWidth  ( 3 );
}

QColorFrame::~QColorFrame ( )
{
}

void QColorFrame::paintEvent ( QPaintEvent *pPaint )
{
  // first we call the main function
  QWidget::paintEvent ( pPaint );
  
  // and here we do  our little do ...
  if ( ( m_color.alpha ( ) == 0 ) || ( m_iWhichColor == 0 ) ) {
    //m_pFrameBackgroundColor->paletteForegroundColor())	{
    QPainter thePainter ( this );
    thePainter.setPen   ( QPen ( QColor (  0, 0, 0 ), 2 ) );
    thePainter.drawLine ( 6, 6, width ( )-6, height ( )-6 );
    thePainter.drawLine ( 6, height ( )-6, width ( )-6, 6 );
  }
}

void QColorFrame::mouseReleaseEvent (QMouseEvent *pMouseEvent)
{
	QFrame::mouseReleaseEvent(pMouseEvent);
//	if (pMouseEvent->button() == RightButton)
	emit (signalRightButtonClicked (m_iWhichColor));
}

Rgba QColorFrame::getColor()
{
  return m_color;
}

void QColorFrame::setColor ( Rgba newColor )
{
  m_color = newColor;
  //setPaletteBackgroundColor( newColor.color ( ) );			//ooo
  QPalette palette;							//xxx
  palette.setBrush(this->backgroundRole(), newColor.color ( ));		//xxx
  this->setPalette(palette);						//xxx
}


ColorToolBar::ColorToolBar ( const char *pName, QMainWindow *pMainWindow )	//ooo
//ColorToolBar::ColorToolBar ( const char *pName, QDockWidget *pMainWindow )
  //: QToolBar ( pName, pMainWindow, pMainWindow, false, pName )	//ooo
  : QDockWidget ( pName, pMainWindow )	//xxx
{
  m_pCurrentDVDMenu = NULL;
  //setResizeEnabled ( false );		//ooo
  //setMovingEnabled ( true  );		//ooo
  //pMainWindow->addDockWindow ( this, Qt::DockTop );	//ooo
  pMainWindow->addDockWidget ( Qt::TopDockWidgetArea, this );	    //xxx
  
  // Next we include the 4 colors (MAX_MASK_COLORS) for the Button masks into the main toolbar.
  m_colorFrames[0] = new QColorFrame ( 0, Rgba ( TRANSPARENT_COLOR       ), this );
  m_colorFrames[1] = new QColorFrame ( 1, Rgba ( START_HIGHLIGHTED_COLOR ), this );
  m_colorFrames[2] = new QColorFrame ( 2, Rgba ( START_SELECTED_COLOR    ), this );
  m_colorFrames[3] = new QColorFrame ( 3, Rgba ( START_FRAME_COLOR       ), this );
  
  QAction *m_frameNew    = new QAction ( m_colorFrames[0] );    //xxx
  this->addAction(m_frameNew);  //xxx
  
  //QToolTip::add( m_colorFrames[0], "This color is always the transparent color for the DVDMenu\nNote: this is regardless of the alpha channel." );	//ooo
  //QToolTip::showText( m_colorFrames[0], "This color is always the transparent color for the DVDMenu\nNote: this is regardless of the alpha channel." );	//xxx
  //QToolTip::add( m_colorFrames[1], "Change DVD mask colors\nFor button masks." );	//ooo
  //QToolTip::showText( QString("Change DVD mask colors\nFor button masks."), m_colorFrames[1] );	//xxx
  //QToolTip::add( m_colorFrames[2], "Change DVD mask colors\nFor button masks." );	//ooo
  //QToolTip::add( m_colorFrames[3], "Change DVD mask colors\nFor button masks." );	//ooo
  
  connect ( m_colorFrames[0], SIGNAL ( signalRightButtonClicked (int) ), this, SLOT ( slotColorClicked (int) ) );
  connect ( m_colorFrames[1], SIGNAL ( signalRightButtonClicked (int) ), this, SLOT ( slotColorClicked (int) ) );
  connect ( m_colorFrames[2], SIGNAL ( signalRightButtonClicked (int) ), this, SLOT ( slotColorClicked (int) ) );
  connect ( m_colorFrames[3], SIGNAL ( signalRightButtonClicked (int) ), this, SLOT ( slotColorClicked (int) ) );
}

ColorToolBar::~ColorToolBar ()
{

}

void ColorToolBar::slotColorClicked ( int iColor )
{
  while ( iColor > MAX_MASK_COLORS-1 )
    iColor -= MAX_MASK_COLORS;

  QColorDialog::setCustomColor ( 0, m_colorFrames[0]->getColor ( ).rgb ( ) );
  QColorDialog::setCustomColor ( 1, m_colorFrames[1]->getColor ( ).rgb ( ) );
  QColorDialog::setCustomColor ( 2, m_colorFrames[2]->getColor ( ).rgb ( ) );
  QColorDialog::setCustomColor ( 3, m_colorFrames[3]->getColor ( ).rgb ( ) );

  bool bOkay = false;
  QRgb rgba;
  if ( iColor > 0 ) {
    rgba = QColorDialog::getRgba ( m_colorFrames[iColor]->getColor ( ).rgb ( ), &bOkay );
    if ( ! bOkay ) 
      return;
  }
  else {
    QColor transparentColor = QColorDialog::getColor ( m_colorFrames[0]->getColor ( ).color ( ) );
    if ( ! transparentColor.isValid ( ) )   return;
    rgba = transparentColor.rgb     ( ) | 0x00000000;
  }

  m_colorFrames[iColor]->setColor ( rgba );  
  // And here we set the color in the Menu accordingly
  if ( m_pCurrentDVDMenu )
       m_pCurrentDVDMenu->modifyColor( (uint)iColor, rgba );
}

Rgba ColorToolBar::getColor ( uint iWhichColor )
{
  return m_colorFrames[iWhichColor]->getColor ( );
}

void ColorToolBar::setColor(uint iWhichColor, Rgba theColor )
{
  if ( iWhichColor < MAX_MASK_COLORS )
    m_colorFrames[iWhichColor]->setColor ( theColor );
}

void ColorToolBar::setDVDMenu ( DVDMenu *pMenu )
{
  uint t;
  CDVDMenuInterface *pInterface = NULL;
  if (pMenu)	{
    pInterface = pMenu->getInterface ( );
    for  ( t=0; t<MAX_MASK_COLORS; t++ )
      setColor ( t, pInterface->pgcColors[t] );
  }
  m_pCurrentDVDMenu = pMenu;
}

/*
void ColorToolBar::modifyColor(QColor theColor, uint iWhichColor, void *pMenu)
{
	// This function changes the colors according to the following rules :
	// tHERE ARE 4 colors available :
	// -	1 = Transparent color
	// -	2 = Highlighted color
	// -	3 = Selected color
	// -	4 = -=> FREE <=-
	
	// 1)	The first color change is associated to the 4'th color
	// 2)	The second color change will affect all Buttons (Selected or Highlighted)
	//		a)	If only one ButtonObject in menu, then don't care
	//		b)	Else ask user to confirm
	//			- inquire which color ...
	//			- Go through all ButtonObjects of this menu and change this color
	//		c)	Change ColorToolbar ...
	static bool bFirstModification = true;
	if (bFirstModification)	{
		m_colorFrames[MAX_MASK_COLORS]->setColor(theColor);
		bFirstModification = false;
	}

	// The pointer pMenu is also abused to indicate if we should force the color change 
	// This is done when we have only one ButtonObject currently ...
	if (pMenu)	{
		QString qsWhichColor ("Highlighted");
		if (iWhichColor == SELECTED_MASK)
			qsWhichColor = QString ("Selected");

		if (QMessageBox::information ( this, tr("Change all colors ?"), tr("This modification will change will affect all %1 colors.\n"
			"Do you want to proceed ?").arg(qsWhichColor), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No)
			return;
	}

	m_colorFrames[iWhichColor]->setColor(theColor);
	emit (signalChangeMaskColor (theColor, iWhichColor));
}
*/
 
