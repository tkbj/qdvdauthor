/***************************************************************************
    printpreview.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Form interface generated from reading ui file 'smalldialogaudio.ui'
   
****************************************************************************/

#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

//#include <qprinter.h> //ooo
#include <QPrinter>     //xxx
//#include <qpixmap.h>  //ooo
#include <QPixmap>      //ooo
//#include <qimage.h>   //ooo
#include <QImage>       //xxx
#include <QPrintDialog>

//#include "uiprintpreview.h"		//ooo
#include "ui_uiprintpreview.h"		//xxx

//class PrintPreview : public uiPrintPreview				//ooo
class PrintPreview : public QDialog, public Ui::uiPrintPreview		//xxx
{
    Q_OBJECT

public:
//  enum types { Letter, A4, B5, Legal, Executive, A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1, B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E, DLE, Folio, Ledger, Tabloid, TypesCount };

           //PrintPreview ( QWidget *, QString );			//ooo
	   PrintPreview ( QWidget *pParent, QString qsHeader );		//xxx
  virtual ~PrintPreview ( );

  void setPixmap  ( QPixmap & );
  void refresh    ( );

protected:
  void createBackground ( );
  void checkSizePos     ( int, int );
  float          scale  ( );
  virtual void   accept ( );

private slots:
  virtual void slotZoom      ( );
  virtual void slotSetup     ( );
  virtual void slotHeader    ( );
  virtual void slotUp        ( );
  virtual void slotDown      ( );
  virtual void slotLeft      ( );
  virtual void slotRight     ( );
  virtual void slotLandscape ( bool );
  virtual void slotTypeChanged( int );

private:
  QPrinter m_printer;
  QPoint   m_offset;
  QPoint   m_printExtension;
  QFont    m_font;
  QString  m_qsHeader;
  float    m_fZoom;

  QPixmap m_background;
  QImage  m_screenshot;  // scaled to m_pPreview->width () == 300 fixed width
  QImage  m_original;
};

#endif // PRINTPREVIEW_H
