/***************************************************************************
    printpreview.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Form implementation generated from reading ui file 'smalldialogaudio.ui'

   WARNING! All changes made in this file will be lost!
    
****************************************************************************/

//#include <q3paintdevicemetrics.h> //ooo
#include <QPaintDevice>             //xxx
//#include <qinputdialog.h> //ooo
#include <QInputDialog>     //xxx
//#include <qpushbutton.h>  //ooo
#include <QPushButton>      //xxx
//#include <qlineedit.h>    //ooo
#include <QLineEdit>        //xxx
//#include <qcheckbox.h>    //ooo
#include <QCheckBox>        //xxx
//#include <qcombobox.h>    //ooo
#include <QComboBox>        //xxx
//#include <qpainter.h>     //ooo
#include <QPainter>         //xxx
//#include <qlabel.h>       //ooo
#include <QLabel>           //xxx
//Added by qt3to4:
#include <QPixmap>

#include "global.h"
#include "printpreview.h"

PrintPreview::PrintPreview ( QWidget *pParent, QString qsHeader )
    //: uiPrintPreview ( pParent )	//ooo
    : QDialog ( pParent )		//xxx
{
  setupUi(this);	//ooo
  
  m_printer.setPageSize  ( QPrinter::Letter );
  m_printer.setColorMode ( QPrinter::Color  );
  m_qsHeader  = qsHeader;
  m_offset    = QPoint      ( 0, 110 );
  m_font      = QFont       ( "Sans Serif", 28, QFont::Bold );
  m_fZoom     = 1.0f;
  //Q3PaintDeviceMetrics pdm   ( &m_printer );  //ooo
  //QPrinter pdm   ( &m_printer );              //xxx
  //int iWidth  = pdm.width   ( ); // 810       //ooo
  int iWidth  = m_printer.width   ( ); // 810   //xxx
  //int iHeight = pdm.height  ( ); // 1440      //ooo
  int iHeight = m_printer.height  ( ); // 1440  //xxx
  m_printExtension = QPoint ( iWidth, iHeight );

  m_pPreview->setFixedWidth ( 300 );
  m_pPreview->setFixedHeight( (int)( 300 * (float)iHeight / (float)iWidth ) );

  const char *array[QPrinter::NPageSize] = { "A4", "B5", "Letter", "Legal", "Executive", "A0", "A1", "A2", "A3", "A5", "A6", "A7", "A8", "A9", "B0", "B1", "B10", "B2", "B3", "B4", "B6", "B7", "B8", "B9", "C5E", "Comm10E", "DLE", "Folio", "Ledger", "Tabloid" };

  for ( int t=0; t<QPrinter::NPageSize; t++ )
    //m_pComboLetter->insertItem   ( array[t] );    //ooo
    m_pComboLetter->addItem   ( array[t] );         //xxx
  //m_pComboLetter->setCurrentItem ( 2 ); // default to Letter  //ooo
  m_pComboLetter->setCurrentIndex ( 2 ); // default to Letter   //xxx

  connect ( m_pButtonHeader,   SIGNAL ( clicked ( ) ), this, SLOT ( slotHeader  ( ) ) );
  connect ( m_pButtonSetup,    SIGNAL ( clicked ( ) ), this, SLOT ( slotSetup   ( ) ) );
  connect ( m_pButtonUp,       SIGNAL ( clicked ( ) ), this, SLOT ( slotUp      ( ) ) );
  connect ( m_pButtonDown,     SIGNAL ( clicked ( ) ), this, SLOT ( slotDown    ( ) ) );
  connect ( m_pButtonLeft,     SIGNAL ( clicked ( ) ), this, SLOT ( slotLeft    ( ) ) );
  connect ( m_pButtonRight,    SIGNAL ( clicked ( ) ), this, SLOT ( slotRight   ( ) ) );
  connect ( m_pEditZoom,       SIGNAL ( lostFocus ( ) ), this, SLOT ( slotZoom ( ) ) );
  connect ( m_pCheckLandscape, SIGNAL ( toggled   ( bool ) ), this, SLOT ( slotLandscape   ( bool ) ) );
  connect ( m_pComboLetter,    SIGNAL ( activated (  int ) ), this, SLOT ( slotTypeChanged (  int ) ) );
}

PrintPreview::~PrintPreview ( )
{
}

void PrintPreview::setPixmap ( QPixmap &pix )
{
  //m_original   = pix.convertToImage ( );  //ooo
  m_original   = pix.toImage ( );           //xxx
  //m_screenshot = m_original.smoothScale ( m_pPreview->width ( ), m_pPreview->height ( ), Qt::KeepAspectRatio );   //ooo
  m_screenshot = m_original.scaled ( m_pPreview->width ( ), m_pPreview->height ( ), Qt::KeepAspectRatio );          //xxx
  createBackground ( );
  refresh ( );
}

void PrintPreview::refresh ( )
{
  float  fScale = scale ( );
  QPoint offset ( (int)( fScale * m_offset.x ( ) ), (int)( fScale * m_offset.y ( ) ) );

  int iZoomWidth  = (int)( m_fZoom * m_pPreview->width  ( ) );
  int iZoomHeight = (int)( m_fZoom * m_pPreview->height ( ) ) - (int)( 110 * fScale );

  //m_screenshot = m_original.smoothScale ( iZoomWidth, iZoomHeight, Qt::KeepAspectRatio ); //ooo
  m_screenshot = m_original.scaled ( iZoomWidth, iZoomHeight, Qt::KeepAspectRatio );        //xxx

  // Create the large Pixmap
  QPixmap    preview    ( m_background         );
  QPainter   thePainter ( &preview             );
  thePainter.drawImage  ( offset, m_screenshot );

  m_pPreview->setPixmap ( preview );
}

void PrintPreview::createBackground ( )
{
  // This function will create a scaled down version of the background, including the text.
  // The refresh function is then using the smaller version without the need to scale
  // ths improving performance.
  int iWidth  = m_printExtension.x ( );
  int iHeight = m_printExtension.y ( );
  int iNewHeight = (int)( m_pPreview->width ( ) * iHeight / iWidth );
  m_pPreview->setFixedHeight ( iNewHeight );

  // Create the large Pixmap
  QPixmap preview ( iWidth, iHeight );
  preview.fill    ( QColor ( 255, 255, 255 ) );

  QPainter thePainter   ( &preview );
  thePainter.setFont    ( m_font );
  thePainter.drawText   ( 0, 0, m_printExtension.x ( ), 100, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, m_qsHeader );
  // Finally after we rendered the preview 1:1 we can scale it down
  // to fit the preview
  //QImage  image = preview.convertToImage ( ); //ooo
  QImage  image = preview.toImage ( );          //xxx
  //image = image.smoothScale ( m_pPreview->width ( ), iNewHeight, Qt::KeepAspectRatio );   //ooo
  image = image.scaled ( m_pPreview->width ( ), iNewHeight, Qt::KeepAspectRatio );          //xxx

  m_background.convertFromImage ( image );
}

void PrintPreview::slotHeader ( )
{
  bool bOkay = false;
  //QString qsNewHeader = QInputDialog::getText ( tr ( "Header" ), tr ( "New Header" ), QLineEdit::Normal, m_qsHeader, &bOkay, this );  //ooo
  QString qsNewHeader = QInputDialog::getText ( this, tr ( "Header" ), tr ( "New Header" ), QLineEdit::Normal, m_qsHeader, &bOkay );    //xxx

  if ( bOkay )
    m_qsHeader = qsNewHeader;
  createBackground ( );
  refresh ( );
}

void PrintPreview::slotTypeChanged ( int iNewType )
{
  m_printer.setPageSize ( (QPrinter::PageSize)iNewType );

  //Q3PaintDeviceMetrics pdm   ( &m_printer );      //ooo
  //QPaintDevice pdm   ( &m_printer );              //xxx
  //int iWidth  = pdm.width   ( ); // 810           //ooo
  int iWidth  = m_printer.width   ( ); // 810       //xxx
  //int iHeight = pdm.height  ( ); // 1440          //ooo
  int iHeight = m_printer.height  ( ); // 1440      //xxx
  m_printExtension = QPoint ( iWidth, iHeight );

  createBackground ( );
  refresh ( );

}

void PrintPreview::slotLandscape ( bool bLandscape )
{
  m_printer.setOrientation ( bLandscape ? QPrinter::Landscape : QPrinter::Portrait );

  //Q3PaintDeviceMetrics pdm   ( &m_printer );  //ooo
  //int iWidth  = pdm.width   ( ); // 810       //ooo
  int iWidth  = m_printer.width   ( ); // 810   //xxx
  //int iHeight = pdm.height  ( ); // 1440      //ooo
  int iHeight = m_printer.height  ( ); // 1440  //xxx
  m_printExtension = QPoint ( iWidth, iHeight );

  createBackground ( );
  refresh ( );
  checkSizePos ( 0, 0 );
}

void PrintPreview::slotZoom ( )
{
  // Limit zoom to between 100% and 10% of the paper width
  bool bOkay = false;
  QString qsZoom = m_pEditZoom->text ( );
  qsZoom.remove ( "%" );
  float fZoom = qsZoom.toFloat ( &bOkay );
  if ( bOkay )
       m_fZoom = fZoom / 100.0f;

  if ( m_fZoom < 0.15f )
       m_fZoom = 0.15f;
  if ( m_fZoom > 1.00f )
       m_fZoom = 1.00f;

  qsZoom = QString ( "%1%" ).arg ( m_fZoom * 100.0f );
  m_pEditZoom->setText ( qsZoom );

  refresh ( );
  checkSizePos ( 0, 0 );
}

void PrintPreview::slotSetup ( )
{
  //m_printer.setup  ( this );  //oooo
  QPrintDialog printDialog(&m_printer, this);       //xxx
  if (printDialog.exec() == QDialog::Accepted) {    //xxx
    // print ...
  }  

  //m_pComboLetter->setCurrentItem ( (int)m_printer.pageSize ( ) ); //ooo
  m_pComboLetter->setCurrentIndex ( (int)m_printer.pageSize ( ) );  //xxx
  m_pCheckLandscape->setChecked  ( ( m_printer.orientation ( ) == QPrinter::Landscape ) );
}

void PrintPreview::slotUp ( )
{
  return checkSizePos ( 0, -10 );
}

void PrintPreview::slotDown ( )
{
  return checkSizePos ( 0, 10 );
}

void PrintPreview::slotLeft ( )
{
  return checkSizePos ( -10, 0 );
}

void PrintPreview::slotRight ( )
{
  return checkSizePos ( 10, 0 );
}

float PrintPreview::scale ( )
{
  // Calculates the scale of the preview vs the Printer available resolution
  int    iWidth  = m_printExtension.x ( );
  float  fScale  = (float)m_pPreview->width  ( ) / iWidth;
  int    iHeight = m_printExtension.y ( );
  float  fScaleY = (float)m_pPreview->height ( ) / iHeight;

  if ( fScale < fScaleY )
       fScale = fScaleY;

  return fScale;
}

void PrintPreview::checkSizePos ( int iDeltaX, int iDeltaY )
{
  float fScale= scale ( );
  int iX = m_offset.x ( ) + iDeltaX;
  int iY = m_offset.y ( ) + iDeltaY;
  int iScreenshotX = (int)( m_screenshot.width  ( ) );
  int iScreenshotY = (int)( m_screenshot.height ( ) ); // - 110  fScale;

  if ( iX * fScale > m_pPreview->width  ( ) - iScreenshotX )
       iX = (int)( ( m_pPreview->width  ( ) - iScreenshotX ) / fScale );

  if ( iY * fScale > m_pPreview->height ( ) - iScreenshotY )
       iY = (int)( ( m_pPreview->height ( ) - iScreenshotY ) / fScale );

  if ( iX < 0 )
       iX = 0;
  if ( iY < 110 )
       iY = 110;

  m_offset.setX ( iX );
  m_offset.setY ( iY );

  refresh ( );
}

void PrintPreview::accept ( )
{
  QPainter thePainter;
  if ( ! thePainter.begin ( &m_printer ) )
    return;

  int iZoomedX = (int)( m_fZoom * m_printExtension.x ( ) );
  int iZoomedY = (int)( m_fZoom * m_printExtension.y ( ) ) - 110;
  //QImage image = m_original.smoothScale ( iZoomedX, iZoomedY, Qt::KeepAspectRatio );  //ooo
  QImage image = m_original.scaled ( iZoomedX, iZoomedY, Qt::KeepAspectRatio );         //xxx

  thePainter.setFont   ( m_font );
//  thePainter.drawText  ( 0, 0, m_printExtension.x ( ), m_offset.y ( ), Qt::AlignHCenter | Qt::AlignVCenter | Qt::WordBreak, m_qsHeader );
  thePainter.drawText  ( 0, 0, m_printExtension.x ( ), 100, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, m_qsHeader );
  thePainter.drawImage ( m_offset.x ( ), m_offset.y ( ), image );

  //uiPrintPreview::accept ( );		//ooo
  QDialog::accept ( );			//xxx
}
