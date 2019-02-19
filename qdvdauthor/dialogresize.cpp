/***************************************************************************
    dialogresize.cpp
                             -------------------
    DialogResize class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QtGui>
#include <QCheckBox>
#include <QMessageBox>	//xxx

#include "global.h"
#include "qimagehelper.h"
#include "dialogresize.h"

DialogResize::DialogResize(QWidget *pParent)
	: QDialog(pParent)
{
  setupUi(this);
  
  connect(m_pRadio1, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio2, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio3, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio4, SIGNAL(clicked()), this, SLOT(slotResize()));
  
  connect(m_pRadio5, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio6, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio7, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadio8, SIGNAL(clicked()), this, SLOT(slotResize()));
  
  connect(m_pRadioStretch1, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadioStretch2, SIGNAL(clicked()), this, SLOT(slotResize()));
  connect(m_pRadioStretch3, SIGNAL(clicked()), this, SLOT(slotResize()));
  
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(slotHelp()));
  //connect(m_pButtonPreview, SIGNAL(clicked()), this, SLOT(preview()));
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(slotOk(void)));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

}

void DialogResize::slotManualResize(const QString &)
{
}

void DialogResize::slotResize ( )	//xxx
{
  if (m_pRadio1->isChecked())
    slotResize(0);
  else if (m_pRadio2->isChecked())
    slotResize(1);
  else if (m_pRadio3->isChecked())
     slotResize(2);
  else if (m_pRadio4->isChecked())
    slotResize(3);
  else if (m_pRadio5->isChecked())
    slotResize(4);
  else if (m_pRadio6->isChecked())
    slotResize(5);
  else if (m_pRadio7->isChecked())
    slotResize(6);
  else if (m_pRadio8->isChecked())
    slotResize(7);
  else if (m_pRadioStretch1->isChecked())
    slotResize(8);
  else if (m_pRadioStretch1->isChecked())
    slotResize(9);
  else if (m_pRadioStretch1->isChecked())
    slotResize(10);
      
}

void DialogResize::slotResize ( int iState )
{
  // Here we filter out the second RadioButton, which gets Unset ...
  //if (iState != QCheckBox::On)		//ooo
  //if (iState != QCheckBox::isChecked())		//xxx
    //return;			//ooo
  int iWidth = 720;
  int iHeight = 480;
  int iFormat = FORMAT_NONE;
  // ButtonGroup : 
  if (m_pRadio1->isChecked())	{
    iWidth	= 720;
    iHeight	= 576;
    iFormat = FORMAT_PAL1;
  }
  else if (m_pRadio2->isChecked()){
    iWidth	= 704;
    iHeight	= 576;
    iFormat = FORMAT_PAL2;
  }
  else if (m_pRadio3->isChecked()){
    iWidth	= 352;
    iHeight	= 576;
    iFormat = FORMAT_PAL3;
  }
  else if (m_pRadio4->isChecked()){
    iWidth	= 352;
    iHeight	= 288;
    iFormat = FORMAT_PAL4;
  }
  else if (m_pRadio5->isChecked()){
    iWidth	= 720;
    iHeight	= 480;
    iFormat = FORMAT_NTSC1;
  }
  else if (m_pRadio6->isChecked()){
    iWidth	= 704;
    iHeight	= 480;
    iFormat = FORMAT_NTSC2;
  }
  else if (m_pRadio7->isChecked()){
    iWidth	= 352;
    iHeight	= 480;
    iFormat = FORMAT_NTSC3;
  }
  else if (m_pRadio8->isChecked()){
    iWidth	= 352;
    iHeight	= 240;
    iFormat = FORMAT_NTSC4;
  }
  m_pEditWidth->setText(QString("%1").arg(iWidth));
  m_pEditHeight->setText(QString("%1").arg(iHeight));
  m_iFormat = iFormat;
  slotPreview();
}


void DialogResize::slotResizeAlogrithm( int iState )
{
  //if (iState != QCheckBox::On)			//ooo
  //if (iState != comboResizeAlgorithm->isChecked())	//xxx
  //  return;						//ooo

}

void DialogResize::slotStretchType( int iState )
{
  //if (iState == QCheckBox::On)		//ooo
  //if (iState == groupBox_2->isChecked())	//xxx
  //  slotPreview ();

  bool bEnable = false;
  //if (m_pRadioStretch3->isOn())	//ooo
  if (m_pRadioStretch3->isChecked())	//xxx
    bEnable = true;
  m_pEditOffsetX->setEnabled(bEnable);
  m_pEditOffsetY->setEnabled(bEnable);
}

void DialogResize::slotPreview()
{
  QCursor myCursor (Qt::WaitCursor);
  setCursor(myCursor);

  int iWidth, iHeight;
  bool  bOk = true;

  iWidth = m_pEditWidth->text().toInt(&bOk);
  if (!bOk)
    iWidth = 720;
  iHeight = m_pEditHeight->text().toInt(&bOk);
  if (!bOk)
    iHeight = 480;

  // And since we are using a preview, we can shrink it by factor 2.
  iWidth  = (int) (iWidth  / 2.0);
  iHeight = (int) (iHeight / 2.0);
  m_iStretchType = 0;
  //if (m_pRadioStretch2->isOn())		//ooo
  if (m_pRadioStretch2->isChecked())	//xxx
    m_iStretchType = 1;
  //else if (m_pRadioStretch3->isOn())	//ooo
  else if (m_pRadioStretch3->isChecked())	//xxx
    m_iStretchType = 2;

  preview (iWidth, iHeight, getResizeAlgorithm());

  myCursor = QCursor(Qt::ArrowCursor);
  setCursor(myCursor);
}

int DialogResize::getResizeAlgorithm ()
{
  int iResizeAlgorithm = RESIZE_QT;
  // Here we see which algol the user wants to use ...
  //comboResizeAlgorithm->currentItem();		//ooo
  comboResizeAlgorithm->currentText();		//xxx
  // Please note that the QImageHelper class can figure out the algol itself since
  // the QStringList has been delivered in a known order.
  return iResizeAlgorithm;
}

void DialogResize::preview (int iWidgetWidth, int iWidgetHeight, int iResizeAlgorithm)
{
  QImageHelper theImage;
    bool bOk;
    int iDeltaX, iDeltaY, iOffsetX, iOffsetY;

  iOffsetX = m_pEditOffsetX->text().toInt(&bOk);
  if (!bOk)
    iOffsetX = 0;
  iOffsetY = m_pEditOffsetY->text().toInt(&bOk);
  if (!bOk)
    iOffsetY = 0;

  if (!m_qsFileName.isEmpty())
    theImage = QImageHelper(m_qsFileName, 0);
  else if (m_pImage)
    theImage = QImageHelper((const QImageHelper &)*m_pImage);
  else
    return;

  QPixmap thePreview;
  // Next we handle the resizing pending the checked RadioButton algo ...
  if (m_iStretchType > 0)	{ 
    QImage tempImage;
    if (m_iStretchType == 2)	{	// Cut Off 
      //tempImage = theImage.smoothScale (iWidgetWidth, iWidgetHeight, Qt::KeepAspectRatioByExpanding);	//ooo
      tempImage = theImage.scaled (iWidgetWidth, iWidgetHeight, Qt::KeepAspectRatioByExpanding);		//xxx
      iDeltaX = iOffsetX;
      iDeltaY = iOffsetY;
      m_posOffset = QSize (iOffsetX, iOffsetY);
    }
    else	{	// m_iStretchType == 1 -=> Black Borders
      //tempImage = theImage.smoothScale (iWidgetWidth, iWidgetHeight, Qt::KeepAspectRatio);	//ooo
      tempImage = theImage.scaled (iWidgetWidth, iWidgetHeight, Qt::KeepAspectRatio);		//xxx
      iDeltaX = iWidgetWidth - tempImage.width();
      if (iDeltaX > 0)
        iDeltaX = (int)((float)iDeltaX / 2.0);
      iDeltaY = iWidgetHeight - tempImage.height();
      if (iDeltaY > 0)
        iDeltaY = (int)((float)iDeltaY / 2.0);
    }

    theImage.fill (0);  //Qt::black);
    theImage.resize (iWidgetWidth, iWidgetHeight, iResizeAlgorithm);
    //bitBlt (&theImage, iDeltaX, iDeltaY, &tempImage, 0, 0, tempImage.width(), tempImage.height());	//ooo
    //void bitBlt( QPixmap& dst, int x, int y, const QPixmap& src )	//xxx
    //QPainter p( &dst );						//xxx
    //p.drawPixmap( x, y, src );					//xxx
    QPainter p( &theImage );
    //p.drawPixmap( QRect(0,0,tempImage.width(), tempImage.height()), &tempImage, QRect(0,0,iDeltaX, iDeltaY) );
    //p.drawPixmap( iDeltaX, iDeltaY, &tempImage, 0, 0, tempImage.width(), tempImage.height());
    //p.drawPixmap( QRect(0, 0, iDeltaX, iDeltaY), QPixmap(tempImage), QRect(0, 0, tempImage.width(), tempImage.height()) );
    //p.drawImage ( 0, 0, tempImage.width(), tempImage.height(), &tempImage, iDeltaX, iDeltaY, -1, -1 );
    //p.drawImage( QRect(0, 0, iDeltaX, iDeltaY), tempImage, QRect(0, 0, tempImage.width(), tempImage.height()) );
    p.drawImage( iDeltaX, iDeltaY, tempImage, 0, 0, tempImage.width(), tempImage.height() );
  }
  else	// StretchToFit
    theImage.resize (iWidgetWidth, iWidgetHeight, iResizeAlgorithm);

  thePreview = QPixmap (iWidgetWidth, iWidgetHeight);

  thePreview.convertFromImage (theImage);
  m_pPixmapPreview->setFixedSize(iWidgetWidth, iWidgetHeight);

  m_pPixmapPreview->setPixmap ( (const QPixmap &) thePreview);
}

void DialogResize::slotOk(void)
{
  int iWidth, iHeight;
  bool  bOk = true;
  Utils theUtils;
  iWidth = m_pEditWidth->text().toInt(&bOk);
  if (!bOk)	{
    iWidth = 720;
    m_pEditWidth ->setText(QString ("%1").arg(iWidth));
  }
  iHeight = m_pEditHeight->text().toInt(&bOk);
  if (!bOk)	{
    iHeight = 480;
    m_pEditHeight->setText(QString ("%1").arg(iHeight));
  }
  m_iStretchType = 0;
  if (m_pRadioStretch2->isChecked())
    m_iStretchType = 1;
  else if (m_pRadioStretch3->isChecked())
    m_iStretchType = 2;

  m_iFormat = theUtils.getFormat ( iWidth, iHeight );
  preview(iWidth, iHeight, getResizeAlgorithm());
  accept();
}

void DialogResize::slotHelp()
{

}

// This function is called when there is no file associated to the image
// I.e. a Drag'n Drop operation ...
void DialogResize::setImage ( QImage *pImage )
{
  // first we set the avail resize algos ...
  QImageHelper temp;
  comboResizeAlgorithm->clear();
  //comboResizeAlgorithm->insertStringList (temp.getAvailableResizeAlgorithms());		//ooo
  comboResizeAlgorithm->insertItems ( 0, temp.getAvailableResizeAlgorithms());		//xxx
  m_qsFileName = QString();
  m_pImage = pImage;
  slotPreview();
}

void DialogResize::setSize( int iWidth, int iHeight )
{
  QString qsText;
  qsText = QString ( "%1" ).arg ( iWidth );
  m_pEditWidth->setText  ( qsText );
  qsText = QString ( "%1" ).arg ( iHeight );
  m_pEditHeight->setText ( qsText );
  }

void DialogResize::setFilename( QString &qsFileName )
{
  // first we set the avail resize algos ...
  QImageHelper temp;
  comboResizeAlgorithm->clear();
  //comboResizeAlgorithm->insertStringList (temp.getAvailableResizeAlgorithms());		//ooo
  comboResizeAlgorithm->insertItems ( 0, temp.getAvailableResizeAlgorithms());		//xxx
  m_qsFileName = qsFileName;
  m_pImage = NULL;
  m_pEditOffsetX->setText (QString("%1").arg(m_posOffset.width ()));
  m_pEditOffsetY->setText (QString("%1").arg(m_posOffset.height()));
  if (m_iStretchType == 0)
    m_pRadioStretch1->setChecked (true);
  else if  (m_iStretchType == 1)
    m_pRadioStretch2->setChecked (true);
  else if  (m_iStretchType == 2)
    m_pRadioStretch3->setChecked (true);
  slotPreview();
}

void DialogResize::setResizeAlgo( QStringList &algoList )
{
  //comboResizeAlgorithm->insertStringList (algoList);	//ooo
  comboResizeAlgorithm->insertItems ( 0, algoList);	//xxx
}

