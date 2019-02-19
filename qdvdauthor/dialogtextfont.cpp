/***************************************************************************
    dialogtextfont.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QFont>
#include <QStringList>
#include <QFontDatabase>
#include <QTextEdit>
//#include <q3listbox.h>	//ooo
#include <QComboBox>
#include <QColorDialog>
#include <QPainter>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QList>
#include <QPixmap>
#include <QPalette>		//xxx
#include <QMessageBox>

#include "global.h"
#include "dialogtextfont.h"
//#include "dialoganimation.h"	//ooo

#include "textobject.h"
#include "buttonpreview.h"

// Here we initialize the static member variables ...
QFont   DialogTextFont::m_previousFont       = QFont   ( );
QString DialogTextFont::m_previousText       = QString ( );
QPoint  DialogTextFont::m_previousTextAlign  = QPoint  ( Qt::AlignHCenter, Qt::AlignVCenter );
Rgba    DialogTextFont::m_previousBackground = Rgba    ( TRANSPARENT_COLOR, 0xff );
Rgba    DialogTextFont::m_previousForeground = Rgba    ( 20, 20, 255, 255 );

DialogTextFont::DialogTextFont ( QWidget *pParent, TextObject *pTextObject )
  //: uiDialogTextFont ( pParent )	//ooo
  : QDialog ( pParent )			//xxx
{
  setupUi( this );	//xxx
  initMe ( );

  if ( pTextObject ) {
    setTextAlign       ( pTextObject->getTextAlign       ( ) );
    setBackgroundColor ( pTextObject->getBackgroundColor ( ) );
    setForegroundColor ( pTextObject->getForegroundColor ( ) );
    setFont            ( pTextObject->getFont   ( ) );
    setText            ( pTextObject->getText   ( ) );
    setTransparency    ( pTextObject->modifiers ( )->fTransparency );	//oooo
    setAnimation       ( pTextObject->animation ( ) );		//oooo
    setRect            ( pTextObject->rect      ( ) );

    ButtonPreview *pPreview = (ButtonPreview *)pParent;
    QPixmap thePix, pix;
    pPreview->drawBackground   (     thePix, pTextObject );
    //pix.resize ( thePix.width  ( ) - pTextObject->rect ( ).left ( ),	//ooo
    pix.scaled ( thePix.width  ( ) - pTextObject->rect ( ).left ( ), 	//xxx
		 thePix.height ( ) - pTextObject->rect ( ).top  ( ) );
    //copyBlt  ( &pix, 0, 0, &thePix,  pTextObject->rect ( ).left ( ),  pTextObject->rect ( ).top ( ) );	//oooo
    pix = thePix.copy(0, 0, pTextObject->rect ( ).left ( ),  pTextObject->rect ( ).top ( ) );			//xxx
    //pix.drawPixmap(0, 0, &thePix, 0, 0, pTextObject->rect ( ).left ( ),  pTextObject->rect ( ).top ( ) );	//xxx

    //m_pTextEdit->setPaletteBackgroundPixmap ( pix ); //thePix );	//ooo
    QPalette palette;
    palette.setBrush(m_pTextEdit->backgroundRole(), QBrush(pix));
    m_pTextEdit->setPalette(palette);
    //m_pTextEdit->setResizePolicy ( QScrollView::Manual );
    //m_pTextEdit->resizeContents  ( thePix.width ( ), thePix.height ( ) );
    //m_pTextEdit->scrollBy ( -pTextObject->rect   ( ).left ( ), -pTextObject->rect ( ).top ( ) );
  }

  if ( pParent )
    connect ( this, SIGNAL ( signalAnimate ( QString &, int, QString ) ), pParent, SLOT ( slotAnimateObject ( QString &, int, QString ) ) );

  //const QPixmap *pPix = pParent->paletteBackgroundPixmap ( );
  //if ( pPix )
  //     pPix->save ( "/tmp/pix.png", "PNG", 100 );
}

DialogTextFont::~DialogTextFont ( )
{

}

void DialogTextFont::setFont ( const QFont &newFont )
{
  m_qfFont = newFont;
  // Here we set the fields ...
  // Underline / strikeout / pointSize / fontFamily / script / fontStyle
  int t, iIndex;
  for (iIndex=0;iIndex<(int)m_pListFonts->count();iIndex++)	{
    //if (m_pListFonts->text(iIndex) == newFont.family())	{		//ooo
    if (m_pListFonts->item(iIndex)->text() == newFont.family())	{	//xxx
      //m_pListFonts->setCurrentItem(iIndex);				//ooo
      m_pListFonts->setCurrentItem(m_pListFonts->item(iIndex));	//xxx
      break;
    }
  }
  updateFont ( 0 );
  // Next we try to set the style of the font.
  iIndex = 0;
  QFontDatabase database;
  QString qsStyle = database.styleString (newFont);
  for (t=0;t<m_pComboFontStyle->count();t++)	{
    //if (m_pComboFontStyle->text(t) == qsStyle)	{	//ooo
    if (m_pComboFontStyle->itemText(t) == qsStyle)	{	//xxx
      iIndex = t;
      break;
    }
  }
  //m_pComboFontStyle->setCurrentItem(iIndex);	//ooo
  m_pComboFontStyle->setCurrentIndex(iIndex);	//xxx
  updateFont ( 1 );
  // Here we get the font size which comes closest to the newFont pointSize (or the same).
  int iAvailFontSize, iFontSize = newFont.pointSize();
  for (iIndex=0;iIndex<m_pComboFontSizes->count();iIndex++)	{
    //iAvailFontSize = m_pComboFontSizes->text(iIndex).toInt();	//ooo
    iAvailFontSize = m_pComboFontSizes->itemText(iIndex).toInt();	//xxx
    if (iAvailFontSize >= iFontSize)
      break;
  }
  //m_pComboFontSizes->setCurrentItem(iIndex);	//ooo
  m_pComboFontSizes->setCurrentIndex(iIndex);	//xxx

  if (newFont.underline())
    m_pButtonUnderline->setChecked(true);
  if (newFont.strikeOut())
    m_pButtonStrikeout->setChecked(true);

  m_pTextEdit->setFont(newFont);
}

QFont &DialogTextFont::getFont ()
{
  QFontDatabase database;

  //QString qsFamily = m_pListFonts->text(m_pListFonts->currentItem());			//ooo
  QString qsFamily = m_pListFonts->currentItem()->text();					//xxx
  //QString qsStyle  = m_pComboFontStyle->text(m_pComboFontStyle->currentItem());		//ooo
  QString qsStyle  = m_pComboFontStyle->itemText(m_pComboFontStyle->currentIndex());	//xxx
  int iPointSize   = m_pComboFontSizes->currentText().toInt();
  m_qfFont =  database.font(qsFamily, qsStyle, iPointSize );
  m_qfFont.setUnderline (m_pButtonUnderline->isChecked());
  m_qfFont.setStrikeOut (m_pButtonStrikeout->isChecked());

  return m_qfFont;
}

void  DialogTextFont::setText (QString &newText)
{
	m_pTextEdit->setText(newText);
}

QString DialogTextFont::getText ()
{
  //return m_pTextEdit->text();		//ooo
  return m_pTextEdit->toPlainText();	//xxx
}

void  DialogTextFont::setAnimation (QString &qsAnimation)
{
  m_qsAnimation = qsAnimation;
}

QString &DialogTextFont::getAnimation ()
{
  return m_qsAnimation;
}

void  DialogTextFont::setTextAlign (QPoint &newAlign)
{
  // First we re-set the buttons ...
  m_pButtonLeft->   setChecked(false);
  m_pButtonRight->  setChecked(false);
  m_pButtonCenter-> setChecked(false);
  m_pButtonJustify->setChecked(false);

  if (newAlign.x() == Qt::AlignHCenter)
    m_pButtonCenter->setChecked(true);
  else if (newAlign.x() == Qt::AlignHCenter)
    m_pButtonCenter->setChecked(true);
  else if (newAlign.x() == Qt::AlignRight)
    m_pButtonRight->setChecked(true);
  else if (newAlign.x() == Qt::AlignJustify)
    m_pButtonJustify->setChecked(true);

  if (newAlign.y() == Qt::AlignVCenter)
    m_pButtonVCenter->setChecked(true);
  else
    m_pButtonVCenter->setChecked(false);
  updateFont (2);
}

QPoint &DialogTextFont::getTextAlign ()
{
  static QPoint thePoint;
  thePoint.setX(0);
  thePoint.setY(0);
  if (m_pButtonLeft->isChecked())
    thePoint.setX(Qt::AlignLeft);
  else if (m_pButtonCenter->isChecked())
    thePoint.setX(Qt::AlignHCenter);
//  else if (m_pButtonCenter->isOn())
//    thePoint.setX(Qt::AlignHCenter);
  else if (m_pButtonRight->isChecked())
    thePoint.setX(Qt::AlignRight);
  else if (m_pButtonJustify->isChecked())
    thePoint.setX(Qt::AlignJustify);

  if (m_pButtonVCenter->isChecked())
    thePoint.setY(Qt::AlignVCenter);
  else
    thePoint.setY(Qt::AlignTop);
  return thePoint;
}

bool DialogTextFont::getFit()
{
  return m_pButtonFit->isChecked();
}

void DialogTextFont::setRect (QRect &rect)
{
  m_pEditX->setText(QString("%1").arg(rect.x()));
  m_pEditY->setText(QString("%1").arg(rect.y()));
  m_pEditWidth->setText(QString("%1").arg(rect.width()));
  m_pEditHeight->setText(QString("%1").arg(rect.height()));
}

QRect &DialogTextFont::getRect ()
{
  static QRect theRect;
  theRect = QRect (m_pEditX->text().toInt(), m_pEditY->text().toInt(), m_pEditWidth->text().toInt(), m_pEditHeight->text().toInt());
  return theRect;
}

void DialogTextFont::setTransparency (float fNewTransparency)
{ 
  if (fNewTransparency > 1.0f)
    fNewTransparency = 1.0f;
  if (fNewTransparency < 0.0f)
    fNewTransparency = 0.0f;

  m_fTransparency = fNewTransparency;
  m_pEditTransparency->setText (QString("%1").arg(fNewTransparency));
  //m_pSliderTransparency->setValue ((int)(1000.0*fNewTransparency));		//ooo
  m_pSliderTransparency->setValue (static_cast<int>(1000.0*fNewTransparency));	//xxx
}

float DialogTextFont::getTransparency ()
{
  return m_fTransparency;
}

void DialogTextFont::updateButtons ()
{
	//if (m_pButtonLeft->isOn())	//ooo
	if (m_pButtonLeft->isChecked()) {	//xxx
		//QPixmap pixmap = QPixmap(":/images/text_cleft.png");				//xxx
		//m_pButtonLeft->setIcon (QIcon ( pixmap.scaled ( m_pButtonLeft->width(), m_pButtonLeft->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));		//xxx
		//m_pButtonLeft->setIcon (QIcon ( pixmap.scaled ( 22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation)));		//xxx
		//m_pButtonLeft->setPixmap (QPixmap ().fromMimeSource("text_cleft.png"));	//ooo
		//m_pButtonLeft->setPixmap (QPixmap (":/images/text_cleft.png"));			//xxx
		//m_pButtonLeft->setIcon (QIcon (QPixmap (":/images/text_cleft.png")));		//xxx
		m_pButtonLeft->setIcon (QIcon (":/images/text_cleft.png"));			//xxx
	} else
		//m_pButtonLeft->setPixmap (QPixmap ().fromMimeSource("text_left.png"));	//ooo
		//m_pButtonLeft->setPixmap (QPixmap (":/images/text_left.png"));			//xxx
		m_pButtonLeft->setIcon (QIcon (QPixmap (":/images/text_left.png")));		//xxx
		//m_pButtonLeft->setIcon (QIcon (":/images/text_left.png"));			//xxx

	//if (m_pButtonCenter->isOn())	//ooo
	if (m_pButtonCenter->isChecked())	//xxx
		//m_pButtonCenter->setPixmap (QPixmap ().fromMimeSource("text_ccenter.png"));	//ooo
		//m_pButtonCenter->setPixmap (QPixmap (":/images/text_ccenter.png"));		//xxx
		m_pButtonCenter->setIcon (QIcon (":/images/text_ccenter.png"));			//xxx
	else
		//m_pButtonCenter->setPixmap (QPixmap ().fromMimeSource("text_center.png"));	//ooo
		//m_pButtonCenter->setPixmap (QPixmap (":/images/text_center.png"));		//xxx
		m_pButtonCenter->setIcon (QIcon (":/images/text_center.png"));			//xxx

	//if (m_pButtonRight->isOn())	//ooo
	if (m_pButtonRight->isChecked())	//xxx
		//m_pButtonRight->setPixmap (QPixmap ().fromMimeSource("text_cright.png"));	//ooo
		//m_pButtonRight->setPixmap (QPixmap (":/images/text_cright.png"));		//xxx
		m_pButtonRight->setIcon (QIcon (":/images/text_cright.png"));			//xxx
	else
		//m_pButtonRight->setPixmap (QPixmap ().fromMimeSource("text_right.png"));	//ooo
		//m_pButtonRight->setPixmap (QPixmap (":/images/text_right.png"));		//xxx
		m_pButtonRight->setIcon (QIcon (":/images/text_right.png"));			//xxx

	//if (m_pButtonJustify->isOn())	//ooo
	if (m_pButtonJustify->isChecked())	//xxx
		//m_pButtonJustify->setPixmap (QPixmap ().fromMimeSource("text_cjustify.png"));	//ooo
		//m_pButtonJustify->setPixmap (QPixmap (":/images/text_cjustify.png"));		//xxx
		m_pButtonJustify->setIcon (QIcon (":/images/text_cjustify.png"));		//xxx
	else
		//m_pButtonJustify->setPixmap (QPixmap ().fromMimeSource("text_justify.png"));	//ooo
		//m_pButtonJustify->setPixmap (QPixmap (":/images/text_justify.png"));		//xxx
		m_pButtonJustify->setIcon (QIcon (":/images/text_justify.png"));		//xxx

	//if (m_pButtonVCenter->isOn())	//ooo
	if (m_pButtonVCenter->isChecked())	//xxx
		//m_pButtonVCenter->setPixmap (QPixmap ().fromMimeSource("text_cvcenter.png"));	//ooo
		//m_pButtonVCenter->setPixmap (QPixmap (":/images/text_cvcenter.png"));		//xxx
		m_pButtonVCenter->setIcon (QIcon (":/images/text_cvcenter.png"));		//xxx
	else
		//m_pButtonVCenter->setPixmap (QPixmap ().fromMimeSource("text_vcenter.png"));	//ooo
		//m_pButtonVCenter->setPixmap (QPixmap (":/images/text_vcenter.png"));		//xxx
		m_pButtonVCenter->setIcon (QIcon (":/images/text_vcenter.png"));		//xxx

	//if (m_pButtonFit->isOn())	//ooo
	if (m_pButtonFit->isChecked())	//xxx
		//m_pButtonFit->setPixmap (QPixmap ().fromMimeSource("text_cfit.png"));		//ooo
		//m_pButtonFit->setPixmap (QPixmap (":/images/text_cfit.png"));			//xxx
		m_pButtonFit->setIcon (QIcon (":/images/text_cfit.png"));			//xxx
	else
		//m_pButtonFit->setPixmap (QPixmap ().fromMimeSource("text_fit.png"));		//ooo
		//m_pButtonFit->setPixmap (QPixmap (":/images/text_fit.png"));			//xxx
		m_pButtonFit->setIcon (QIcon (":/images/text_fit.png"));			//xxx

	//if (m_pButtonUnderline->isOn())	//ooo
	if (m_pButtonUnderline->isChecked())	//xxx
		//m_pButtonUnderline->setPixmap (QPixmap ().fromMimeSource("text_cunderline.png"));	//ooo
		//m_pButtonUnderline->setPixmap (QPixmap (":/images/text_cunderline.png"));		//xxx
		m_pButtonUnderline->setIcon (QIcon (":/images/text_cunderline.png"));			//xxx
	else
		//m_pButtonUnderline->setPixmap (QPixmap ().fromMimeSource("text_underline.png"));	//ooo
		//m_pButtonUnderline->setPixmap (QPixmap (":/images/text_underline.png"));		//xxx
		m_pButtonUnderline->setIcon (QIcon (":/images/text_underline.png"));			//xxx

	//if (m_pButtonStrikeout->isOn())	//ooo
	if (m_pButtonStrikeout->isChecked())	//xxx
		//m_pButtonStrikeout->setPixmap (QPixmap ().fromMimeSource("text_cstrikeout.png"));	//ooo
		//m_pButtonStrikeout->setPixmap (QPixmap (":/images/text_cstrikeout.png"));		//xxx
		m_pButtonStrikeout->setIcon (QIcon (":/images/text_cstrikeout.png"));			//xxx
	else
		//m_pButtonStrikeout->setPixmap (QPixmap ().fromMimeSource("text_strikeout.png"));	//ooo
		//m_pButtonStrikeout->setPixmap (QPixmap (":/images/text_strikeout.png"));		//xxx
		m_pButtonStrikeout->setIcon (QIcon (":/images/text_strikeout.png"));			//xxx
}

void DialogTextFont::initMe()
{
  // Here we set all available options ...
  m_iSelectedFontSize = 12;
  QFontDatabase database;
  m_pSliderTransparency->setMaximum(1000);	//xxx
  setTransparency (0.0);
  QStringList listFonts = database.families();
  //m_pListFonts->insertStringList(listFonts);	//ooo
  m_pListFonts->addItems(listFonts);		//xxx
  // Next we search for a good starting font ...
  uint t, iIndex;
  QString fontName;
  for (iIndex=0;iIndex<(uint)m_pListFonts->count();iIndex++)	{
    //fontName = m_pListFonts->text(iIndex);	//ooo
    fontName = m_pListFonts->item(iIndex)->text();	//xxx
    if (fontName == QString ("Arial"))
      break;
    else if (fontName == QString ("Courier"))
      break;
    else if (fontName == QString ("Times New Roman"))
      break;
    else if (fontName == QString ("Times"))
      break;
    else if (fontName == QString ("Nimbus Sans L"))
      break;
    else if (fontName == QString ("Sans Serif"))
      break;
  }
  if ( iIndex < (uint) listFonts.count ( ) )
    //fontName = m_pListFonts->text(iIndex);	//ooo
    fontName = m_pListFonts->item(iIndex)->text();	//xxx
  else
    //fontName = m_pListFonts->text( 0 );		//ooo
    fontName = m_pListFonts->item( 0 )->text();	//xxx

  QStringList listStyles = database.styles (fontName);
  //m_pComboFontStyle->insertStringList (listStyles);	//ooo
  m_pComboFontStyle->addItems (listStyles);		//xxx
  //QString styleName = m_pComboFontStyle->text(0);	//ooo
  QString styleName = m_pComboFontStyle->itemText(0);	//xxx
  //m_pComboFontStyle->setCurrentItem(0);			//ooo
  m_pComboFontStyle->setCurrentIndex(0);			//xxx

  // Here we get the sizes of the font.
  QList<int> listSizes = database.pointSizes(fontName);	//oxx
  if (listSizes.isEmpty())	{
    listSizes = database.smoothSizes(fontName, styleName);
    if (listSizes.isEmpty())	{
      listSizes = database.standardSizes();
    }
  }
  // So lets say we can set the fontFamily at least ...
  //m_pListFonts->setCurrentItem(iIndex);				//ooo
  m_pListFonts->setCurrentItem(m_pListFonts->item(iIndex));	//xxx
  // And here we take care of the FontSize ...
  iIndex = 0;
  for (t=0;t<(uint)listSizes.count();t++)	{
    //m_pComboFontSizes->insertItem (QString ("%1").arg(listSizes[t]));	//ooo
    m_pComboFontSizes->addItem (QString ("%1").arg(listSizes[t]));		//xxx
    if (listSizes[t] < 11)	// Lets get at least a nice big readable size
      iIndex = t;
  }
  //m_pComboFontSizes->setCurrentItem (iIndex);	//ooo
  m_pComboFontSizes->setCurrentIndex (iIndex);	//xxx

  QString qsScripts("Unicode Latin Greek Cyrillic Armenian Georgian Runic Ogham SpacingModifiers CombiningMarks"
    "Hebrew Arabic Syriac Thaana"
    "Devanagari Bengali Gurmukhi Gujarati Oriya Tamil Telugu Kannada Malayalam Sinhala Thai Lao Tibetan Myanmar Khmer"
    "Han Hiragana Katakana Hangul Bopomofo Yi"
    "Ethiopic Cherokee CanadianAboriginal Mongolian"
    "CurrencySymbols LetterlikeSymbols NumberForms MathematicalOperators TechnicalSymbols GeometricSymbols MiscellaneousSymbols EnclosedAndSquare Braille");
  //QStringList listScripts = QStringList::split (QString (" "), qsScripts);	//ooo
  QStringList listScripts = qsScripts.split (QString (" "));			//xxx
  //m_pComboScript->insertStringList(listScripts);	//ooo
  m_pComboScript->addItems(listScripts);			//xxx
  //m_pComboScript->setCurrentItem(1);			//ooo
  m_pComboScript->setCurrentIndex(1);			//xxx

  //Finally we set the font of the TextEdit window ...
  setFont            ( m_previousFont       );
  setText            ( m_previousText       );
  setTextAlign       ( m_previousTextAlign  );
  setBackgroundColor ( m_previousBackground );
  setForegroundColor ( m_previousForeground );

  QPainter thePainter ( m_pFrameBackgroundColor );
  thePainter.drawLine ( 0, 0, m_pFrameBackgroundColor->width ( ), m_pFrameFontColor->height ( ) );

  //m_pTextEdit->setTextFormat (PlainText);	//ooo
  //m_pTextEdit->setTextFormat (Qt::PlainText);	//xxxx

  // Finally we set up the callback routines ...
  //connect (m_pListFonts, SIGNAL(highlighted (int)), this, SLOT(slotFontFamilyChanged(int)));		//ooo
  connect (m_pListFonts, SIGNAL(itemSelectionChanged ( )), this, SLOT(slotFontFamilyChanged ( )));	//xxx
  connect (m_pComboFontStyle, SIGNAL(activated (int)), this, SLOT(slotFontStyleChanged(int)));
  connect (m_pComboFontSizes, SIGNAL(activated (int)), this, SLOT(slotFontSizeChanged(int)));
  //	connect (m_pComboFontSizes, SIGNAL(textChanged ( const QString & )), this, SLOT(slotFontSizeChanged( const QString & )));
  connect (m_pComboScript, SIGNAL(activated (int)), this, SLOT(slotScriptChanged(int)));

  connect (m_pButtonLeft, SIGNAL(toggled (bool)), this, SLOT(slotLeft(bool)));
  connect (m_pButtonCenter, SIGNAL(toggled (bool)), this, SLOT(slotCenter(bool)));
  connect (m_pButtonRight, SIGNAL(toggled (bool)), this, SLOT(slotRight(bool)));
  connect (m_pButtonJustify, SIGNAL(toggled (bool)), this, SLOT(slotJustify(bool)));
  connect (m_pButtonVCenter, SIGNAL(toggled (bool)), this, SLOT(slotVCenter(bool)));
  connect (m_pButtonFit, SIGNAL(toggled (bool)), this, SLOT(slotFit(bool)));
  connect (m_pButtonStrikeout, SIGNAL(toggled (bool)), this, SLOT(slotStrikeout(bool)));
  connect (m_pButtonUnderline, SIGNAL(toggled (bool)), this, SLOT(slotUnderline(bool)));

  connect (m_pButtonAnimation,  SIGNAL(clicked()), this, SLOT(slotAnimation()));
  connect (m_pButtonFontColor,  SIGNAL(clicked()), this, SLOT(slotForegroundColor()));
  connect (m_pButtonBackgroundColor, SIGNAL(clicked()), this, SLOT(slotBackgroundColor()));
  connect (m_pSliderTransparency, SIGNAL(valueChanged (int)), this, SLOT(slotTransparency(int)));

  connect (buttonOk, SIGNAL(clicked()), this, SLOT(slotOkay()));
  connect (buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));	//xxx
  updateButtons();
}

void DialogTextFont::resizeEvent ( QResizeEvent *pEvent )
{
  //uiDialogTextFont::resizeEvent ( pEvent );	//ooo
  QDialog::resizeEvent ( pEvent );		//xxx

  // Here we handle the bad expansion of TextEdit. Keep it in line ...
  // NOte, when changing font size the TextEdit would resize the complete frame ... 
  QSize size  = m_pGroupText->size ( );
  int iWidth  = size.width  ( ) -20;
  int iHeight = size.height ( ) -30;
  if ( ( iWidth < 1 ) || ( iHeight < 1 ) )
    return;
  size.setWidth  ( iWidth );
  size.setHeight ( iHeight );
  m_pTextEdit->setFixedSize ( size );
}

void DialogTextFont::paintEvent (QPaintEvent *pPaint)
{
  // first we call the main function
  QWidget::paintEvent(pPaint);
  
  // and here we do  our little do ...
  if ( m_backgroundColor.alpha ( ) == 0 )  {
    //m_pFrameBackgroundColor->paletteForegroundColor())	{
    QPainter thePainter ( m_pFrameBackgroundColor );
    thePainter.setPen   ( QPen ( QColor ( 0, 0, 0 ), 2 ) );
    thePainter.drawLine ( 6, 6, m_pFrameBackgroundColor->width ( )-6, m_pFrameFontColor->height ( )-6 );
    thePainter.drawLine ( 6, m_pFrameFontColor->height ( )-6, m_pFrameBackgroundColor->width ( )-6, 6 );
  }
  if ( m_foregroundColor.alpha ( ) == 0 )  {
    //m_pFrameBackgroundColor->paletteForegroundColor())	{
    QPainter thePainter ( m_pFrameBackgroundColor );
    thePainter.setPen   ( QPen ( QColor ( 0, 0, 0 ), 2 ) );
    thePainter.drawLine ( 6, 6, m_pFrameBackgroundColor->width ( )-6, m_pFrameFontColor->height ( )-6 );
    thePainter.drawLine ( 6, m_pFrameFontColor->height ( )-6, m_pFrameBackgroundColor->width ( )-6, 6 );
  }
}

void DialogTextFont::slotForegroundColor ()
{
  bool bOkay;
  QRgb newColor = QColorDialog::getRgba ( m_foregroundColor.rgb ( ), &bOkay, this );
  if ( bOkay ) {
    Rgba theColor      ( newColor );
    setForegroundColor ( theColor );
  }
  updateFont (2);	//xxx
}

void DialogTextFont::slotBackgroundColor ()
{
  bool bOkay;
  Rgba transparentColor = Rgba ( TRANSPARENT_COLOR, 0 );
  QRgb newColor         = QColorDialog::getRgba ( transparentColor.rgb ( ), &bOkay, this );
  if ( bOkay ) {
    Rgba theColor      ( newColor );
    setBackgroundColor ( theColor );
  }
  updateFont (2);	//xxx
}

void DialogTextFont::slotTransparency (int iNewValue)
{
  // Called from the slider
  if (iNewValue < 5)
    iNewValue = 0;
  if (iNewValue > 995)
    iNewValue = 1000;

  // [0.0 .. 1.0]=[0..999]=> 1.0=fully transparent.
  //m_fTransparency = (float)((float)iNewValue/1000.0);				//ooo
  m_fTransparency = static_cast<float>(static_cast<float>(iNewValue)/1000.0);	//xxx
  m_pEditTransparency->setText (QString("%1").arg(m_fTransparency, 0, 'f', 2));
}

//void DialogTextFont::slotFontFamilyChanged(int)	//ooo
void DialogTextFont::slotFontFamilyChanged( )		//xxx
{
  updateFont(0);
}

void DialogTextFont::slotFontStyleChanged(int)
{
  updateFont(1);
}

void DialogTextFont::slotFontSizeChanged ( int )
{
  QSize size = m_pTextEdit->size ( );
  m_pTextEdit->setFixedSize ( size );
  updateFont(2);
  m_iSelectedFontSize = m_pComboFontSizes->currentText ( ).toInt ( );
  if ( m_iSelectedFontSize < 4 )
       m_iSelectedFontSize = 12;
}

void DialogTextFont::slotFontSizeChanged( const QString & )
{
  m_iSelectedFontSize = m_pComboFontSizes->currentText ( ).toInt ( );
  if ( m_iSelectedFontSize < 4 )
       m_iSelectedFontSize = 12;
  //  printf ( "       DialogTextFone::slotFontSizeChanged  :  SelectedFontSize<%d> vs <%s>\n", m_iSelectedFontSize, string.ascii() );
  updateFont ( 2 );
}

void DialogTextFont::slotScriptChanged(int)
{
  updateFont(0);
}

void DialogTextFont::slotLeft (bool)
{
  if (m_pButtonLeft->isChecked())	{
    m_pButtonCenter->setChecked(false);
    m_pButtonRight->setChecked(false);
    m_pButtonJustify->setChecked(false);
  }
  updateFont(2);
}

void DialogTextFont::slotCenter (bool)
{
  if (m_pButtonCenter->isChecked())	{
    m_pButtonLeft->setChecked(false);
    m_pButtonRight->setChecked(false);
    m_pButtonJustify->setChecked(false);
  }
  updateFont(2);
}

void DialogTextFont::slotRight (bool)
{
  if (m_pButtonRight->isChecked())	{
    m_pButtonCenter->setChecked(false);
    m_pButtonLeft->setChecked(false);
    m_pButtonJustify->setChecked(false);
  }
  updateFont(2);
}

void DialogTextFont::slotJustify (bool)
{
  if (m_pButtonJustify->isChecked())	{
    m_pButtonCenter->setChecked(false);
    m_pButtonRight->setChecked(false);
    m_pButtonLeft->setChecked(false);
  }
  updateFont(2);
}

void DialogTextFont::slotVCenter (bool)
{
  updateFont(2);
}

void DialogTextFont::slotFit (bool bFitForIt)
{
  // here we enable - disable the coordinates if the user wants to
  // fit the box to the text extension.
  m_pEditWidth-> setEnabled(!bFitForIt);
  m_pEditHeight->setEnabled(!bFitForIt);
  updateFont(2);
}

void DialogTextFont::slotUnderline (bool)
{
  updateFont(2);
}

void DialogTextFont::slotStrikeout (bool)
{
  updateFont(2);
}

void DialogTextFont::slotAnimation ()
{
  //emit ( signalAnimate ( m_qsAnimation, DialogAnimation::TypeText, m_qfFont.toString ( ) ) );		//oooo
}

void  DialogTextFont::updateFont(uint iSwitch)
{
  uint t;
  int iIndex;
  // First see what styles are avail ...
  QFontDatabase database;
  //QString fontName  = m_pListFonts->text(m_pListFonts->currentItem());			//ooo
  QString fontName  = m_pListFonts->currentItem()->text();				//xxx
  //QString styleName = m_pComboFontStyle->text(m_pComboFontStyle->currentItem());	//ooo
  QString styleName = m_pComboFontStyle->itemText(m_pComboFontStyle->currentIndex());	//xxx
  //	int iFontSize     = m_pComboFontSizes->currentText ().toInt();
  int iFontSize = m_iSelectedFontSize;
  switch (iSwitch)	{
  case 0:	{
    // The fontName has changed, so we need to get the styles avail for this new font.
    QStringList listStyles = database.styles (fontName);
    m_pComboFontStyle->clear();
    //m_pComboFontStyle->insertStringList (listStyles);	//ooo
    m_pComboFontStyle->addItems (listStyles);		//xxx
    iIndex = findString(listStyles, styleName);
    if (iIndex == -1)
      iIndex = 0;
    //styleName = m_pComboFontStyle->text(iIndex);		//ooo
    styleName = m_pComboFontStyle->itemText(iIndex);	//xxx
    //m_pComboFontStyle->setCurrentItem(iIndex);		//ooo
    m_pComboFontStyle->setCurrentIndex(iIndex);		//xxx
  }
  case 1:
    // The style has changed
    m_pComboFontSizes->clear();
    QList<int> listSizes = database.pointSizes(fontName);	//oxx
    if (listSizes.isEmpty())	{
      listSizes = database.smoothSizes(fontName, styleName);
      if (listSizes.isEmpty())	{
        listSizes = database.standardSizes();
      }
    }
    iIndex = 0;
    for (t=0;t<(uint)listSizes.count();t++)	{
      //m_pComboFontSizes->insertItem (QString ("%1").arg(listSizes[t]));	//ooo
      m_pComboFontSizes->addItem (QString ("%1").arg(listSizes[t]));		//xxx
      if (listSizes[t] == iFontSize)
        iIndex = t;
    }
    //m_pComboFontSizes->setCurrentItem (iIndex);	//ooo
    m_pComboFontSizes->setCurrentIndex (iIndex);
  }
  //QString qsText = m_pTextEdit->text();		//xxx
  QString qsText = m_pTextEdit->toPlainText();	//xxx
  //int iAlignement;	//ooo
  Qt::Alignment iAlignement;	//xxx
  iAlignement = Qt::AlignLeft;
  if (m_pButtonCenter->isChecked())
    iAlignement =  Qt::AlignHCenter;
  else if (m_pButtonRight->isChecked())
    iAlignement = Qt::AlignRight;
  else if (m_pButtonJustify->isChecked())
    iAlignement = Qt::AlignJustify;

  m_pTextEdit->setText(qsText);
  // and last we update the TextEdit accordingly
  if (m_pButtonVCenter->isChecked())
    iAlignement |= Qt::AlignVCenter;
  else
    iAlignement |= Qt::AlignTop;

  m_pTextEdit->setAlignment ( iAlignement );			//ooo
  //m_pTextEdit->setAlignment ( (Qt::Alignment) iAlignement );	//xxx
  //m_pTextEdit->setAlignment ( Qt::AlignRight );			//xxx
  updateButtons ();
  m_pTextEdit->setFont(getFont());
};

int DialogTextFont::findString (QStringList &list, QString string)
{
  for (uint t=0;t<(uint)list.count();t++)	{
    if (list[t] == string)
      return t;
  }
  return -1;
}

Rgba &DialogTextFont::getBackgroundColor ()
{
  return m_backgroundColor;
}

Rgba &DialogTextFont::getForegroundColor ()
{
  return m_foregroundColor;
}

void DialogTextFont::setBackgroundColor ( Rgba &newColor )
{
  //m_pFrameBackgroundColor->setPaletteBackgroundColor ( newColor.color ( ) );		//ooo
  //m_pFrameBackgroundColor->setBackgroundRole ( newColor.color ( ) );			//xxx
  QPalette palette;									//xxx
  //palette.setBrush(m_pFrameBackgroundColor->backgroundRole(), newColor.color ( ));	//xxxx
  palette.setColor(m_pFrameBackgroundColor->backgroundRole(), newColor.color ( ));	//xxx
  m_pFrameBackgroundColor->setPalette(palette);						//xxx
  update ( );
  //m_pTextEdit->setPaletteBackgroundColor             ( newColor.color ( ) );	//ooo
  m_pTextEdit->setTextBackgroundColor             ( QColor ( newColor.color ( ) ) );	//xxx
  m_backgroundColor = newColor;
}

void DialogTextFont::setForegroundColor ( Rgba &newColor )
{
  //m_pFrameFontColor->setPaletteBackgroundColor ( newColor.color ( ) );		//ooo
  QPalette palette;									//xxx
  palette.setBrush(m_pFrameFontColor->backgroundRole(), newColor.color ( ));		//xxx
  m_pFrameFontColor->setPalette(palette);						//xxx
  //m_pTextEdit->setPaletteForegroundColor       ( newColor.color ( ) );		//ooo
  //m_pTextEdit->setTextColor       ( QColor ( newColor.color ( ) ) );			//xxxx
  m_pTextEdit->setTextColor       ( newColor.color ( ) );			//xxx
  //QPalette palette2;                                                        //xxx
  //palette2.setColor(m_pTextEdit->foregroundRole(), newColor.color ( ));     //xxx
  //m_pTextEdit->setPalette(palette2);                                        //xxx
  m_foregroundColor = newColor;
}

void DialogTextFont::slotOkay()
{
  // here we store the last settings in the static members so the next time we open this
  // dialog we will have the right settings ...
  m_previousFont = getFont ();
  m_previousText = getText ();
  m_previousTextAlign  = getTextAlign ();
  m_previousForeground = getForegroundColor ();
  m_previousBackground = getBackgroundColor ();

  accept ();
}


/* scripts ...
QFont::Latin - consists of most alphabets based on the original Latin alphabet.
QFont::Greek - covers ancient and modern Greek and Coptic.
QFont::Cyrillic - covers the Slavic and non-Slavic languages using cyrillic alphabets. 
QFont::Armenian - contains the Armenian alphabet used with the Armenian language. 
QFont::Georgian - covers at least the language Georgian. 
QFont::Runic - covers the known constituents of the Runic alphabets used by the early and medieval societies in the Germanic, Scandinavian, and Anglo-Saxon areas. 
QFont::Ogham - is an alphabetical script used to write a very early form of Irish. 
QFont::SpacingModifiers - are small signs indicating modifications to the preceeding letter. 
QFont::CombiningMarks - consist of diacritical marks not specific to a particular alphabet, diacritical marks used in combination with mathematical and technical symbols, and glyph encodings applied to multiple letterforms. 

Middle Eastern scripts (right to left): 

QFont::Hebrew - is used for writing Hebrew, Yiddish, and some other languages. 
QFont::Arabic - covers the Arabic language as well as Persian, Urdu, Kurdish and some others. 
QFont::Syriac - is used to write the active liturgical languages and dialects of several Middle Eastern and Southeast Indian communities. 
QFont::Thaana - is used to write the Maledivian Dhivehi language. 

South and Southeast Asian scripts (left to right with few historical exceptions): 

QFont::Devanagari - covers classical Sanskrit and modern Hindi as well as several other languages. 
QFont::Bengali - is a relative to Devanagari employed to write the Bengali language used in West Bengal/India and Bangladesh as well as several minority languages. 
QFont::Gurmukhi - is another Devanagari relative used to write Punjabi. 
QFont::Gujarati - is closely related to Devanagari and used to write the Gujarati language of the Gujarat state in India. 
QFont::Oriya - is used to write the Oriya language of Orissa state/India. 
QFont::Tamil - is used to write the Tamil language of Tamil Nadu state/India, Sri Lanka, Singapore and parts of Malaysia as well as some minority languages. 
QFont::Telugu - is used to write the Telugu language of Andhra Pradesh state/India and some minority languages. 
QFont::Kannada - is another South Indian script used to write the Kannada language of Karnataka state/India and some minority languages. 
QFont::Malayalam - is used to write the Malayalam language of Kerala state/India. 
QFont::Sinhala - is used for Sri Lanka's majority language Sinhala and is also employed to write Pali, Sanskrit, and Tamil. 
QFont::Thai - is used to write Thai and other Southeast Asian languages. 
QFont::Lao - is a language and script quite similar to Thai. 
QFont::Tibetan - is the script used to write Tibetan in several countries like Tibet, the bordering Indian regions and Nepal. It is also used in the Buddist philosophy and liturgy of the Mongolian cultural area.
QFont::Myanmar - is mainly used to write the Burmese language of Myanmar (former Burma). 
QFont::Khmer - is the official language of Kampuchea. 

East Asian scripts (traditionally top-down, right to left, modern often horizontal left to right): 

QFont::Han - consists of the CJK (Chinese, Japanese, Korean) idiographic characters.
QFont::Hiragana - is a cursive syllabary used to indicate phonetics and pronounciation of Japanese words. 
QFont::Katakana - is a non-cursive syllabic script used to write Japanese words with visual emphasis and non-Japanese words in a phonetical manner. 
QFont::Hangul - is a Korean script consisting of alphabetic components. 
QFont::Bopomofo - is a phonetic alphabet for Chinese (mainly Mandarin). 
QFont::Yi - (also called Cuan or Wei) is a syllabary used to write the Yi language of Southwestern China, Myanmar, Laos, and Vietnam. 

Additional scripts that do not fit well into the script categories above: 

QFont::Ethiopic - is a syllabary used by several Central East African languages. 
QFont::Cherokee - is a left-to-right syllabic script used to write the Cherokee language. 
QFont::CanadianAboriginal - consists of the syllabics used by some Canadian aboriginal societies. 
QFont::Mongolian - is the traditional (and recently reintroduced) script used to write Mongolian. 

Symbols: 

QFont::CurrencySymbols - contains currency symbols not encoded in other scripts. 
QFont::LetterlikeSymbols - consists of symbols derived from ordinary letters of an alphabetical script. 
QFont::NumberForms - are provided for compatibility with other existing character sets. 
QFont::MathematicalOperators - consists of encodings for operators, relations and other symbols like arrows used in a mathematical context. 
QFont::TechnicalSymbols - contains representations for control codes, the space symbol, APL symbols and other symbols mainly used in the context of electronic data processing. 
QFont::GeometricSymbols - covers block elements and geometric shapes. 
QFont::MiscellaneousSymbols - consists of a heterogeneous collection of symbols that do not fit any other Unicode character block, e.g. Dingbats. 
QFont::EnclosedAndSquare - is provided for compatibility with some East Asian standards. 
QFont::Braille - is an international writing system used by blind people. This script encodes the 256 eight-dot patterns with the 64 six-dot patterns as a subset. 

QFont::Unicode - includes all the above scripts.
*/


