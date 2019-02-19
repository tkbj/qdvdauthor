/***************************************************************************
    startstopcontrol.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

//#include <q3progressbar.h>	//ooo
#include <QProgressBar>		//xxx
#include <qpushbutton.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qimage.h>
#include <qlabel.h>
#include <qstyle.h>
#include <qtimer.h>
#include <qrect.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QGridLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>	//xxx
#include <QStyleOptionComplex>  //xxx

#include "mediacreator.h"
#include "startstopcontrol.h"

////////////////////////////////////////////////////////////////////////////
//
// StartStopControl - class
//
//////////////////////////////////////////////////////////////////////////// 

//StartStopControl::StartStopControl (QWidget* pParent, const char* pName, Qt::WFlags flags)	//ooo
StartStopControl::StartStopControl (QWidget* pParent)						//xxx
	//: uiStartStopControl (pParent, pName, flags)		//ooo
	: QWidget (pParent)					//xxx
{
  setupUi(this);			//xxx
  m_iThumbnailSize = 65;	
  m_fStart = 0.0f; 
  m_fEnd   = 1.2f;
}

StartStopControl::~StartStopControl ()
{
  int t;
  MediaCreator::unregisterFromMediaScanner ( this );
  for (t=0;t<(int)m_listOfScreenshots.count();t++)
    delete m_listOfScreenshots[t];
  m_listOfScreenshots.clear ();
}

void StartStopControl::initMe ( QString qsFileName, int iThumbnailSize, int iLengthMilliSeconds )
{
  initMe  (  qsFileName,  iThumbnailSize );
  createThumbnails ( iLengthMilliSeconds );
}

void StartStopControl::initMe ( QString qsFileName, int iThumbnailSize )
{
  //Q3GridLayout *pLayout = new Q3GridLayout     ( m_pLabelStartStop, 1, 1, 2, 2, "pLayout2");	//ooo
  QGridLayout *pLayout = new QGridLayout     ( m_pLabelStartStop);				//xxx
  m_pStartStopSlider   = new StartStopSlider ( m_pLabelStartStop );
  m_pStartStopSlider->setMinMax (0, 1000);
  connect (m_pStartStopSlider, SIGNAL (signalValuesChanged  ( int, int) ), this, SLOT(slotSliderValues ( int, int ) ) );
  connect (m_pStartStopSlider, SIGNAL (signalStartStopPressed  ( bool ) ), this, SLOT(slotSliderPressed    ( bool ) ) );
  connect (m_pStartStopSlider, SIGNAL (signalStartStopReleased ( bool ) ), this, SLOT(slotSliderReleased   ( bool ) ) );
  pLayout->addWidget ( m_pStartStopSlider, 0,  0 );
  m_pLabelMovie->setFixedHeight ( iThumbnailSize );
  
  if (iThumbnailSize > 1)
    m_iThumbnailSize = iThumbnailSize;
  m_qsFileName = qsFileName;
  
  // Next we calculate the number of Thumbnaisl we need ...
  
  // The available width - 2 times the size of the Handle
  int t;
  int iAvailableWidth = width() - 2*12;
  //int iNrOfImages = (int)((iAvailableWidth) / iThumbnailSize);            //ooo
  int iNrOfImages = static_cast<int>((iAvailableWidth) / iThumbnailSize);   //xxx
  // But we want to ensure to have at least 2 pixels space in between two images
  iAvailableWidth -= 2*iNrOfImages;
  // since this can affect the nrOfImages, we ought to recalculate ...
  //iNrOfImages = (int)((iAvailableWidth) / iThumbnailSize);            //ooo
  iNrOfImages = static_cast<int>((iAvailableWidth) / iThumbnailSize);   //xxx
  
  // Next we are going to create some temp pixmaps as a placeholder until
  // the screenshots arrive.
  
  //QImage backgroundImage = QImage (QImage::fromMimeSource ( "please_wait.jpg" ));	//ooo
  QImage backgroundImage = QImage ( ":/images/please_wait.jpg" );			//xxx
  //backgroundImage = backgroundImage.smoothScale (m_iThumbnailSize, m_iThumbnailSize);	//ooo
  backgroundImage = backgroundImage.scaled (m_iThumbnailSize, m_iThumbnailSize);	//xxx
  for ( t=0; t<iNrOfImages; t++ )
    m_listOfScreenshots.append (new QImage (backgroundImage));    //ooo
    //m_listOfScreenshots.append (backgroundImage);          //xxx
  
  // Next we draw the line with the temp pictures, 
  updateMovieLine ( );
}

void StartStopControl::createThumbnails ( int iLengthMilliSeconds )
{
  // before we go ahead and request the screenshots.
  long iDeltaSeconds = 0;
  int  iNrOfImages   = m_listOfScreenshots.count ( );
  if ( iNrOfImages  != 1 )	// catch possible exception
    //iDeltaSeconds = (long) ( iLengthMilliSeconds / (iNrOfImages) );               //ooo
    iDeltaSeconds = static_cast<long> ( iLengthMilliSeconds / (iNrOfImages) );    //xxx
  for ( int t=0; t<iNrOfImages; t++ )
    MediaCreator::registerWithMediaScanner ( this, m_qsFileName, m_listOfScreenshots[t], iDeltaSeconds * t );
}

void StartStopControl::showSlider (bool bShow)
{
  //m_pLabelStartStop->setShown (bShow);	//ooo
  m_pLabelStartStop->setVisible (bShow);	//xxx
}

void StartStopControl::setProgress (float fProgress)
{
  int iPos, iTotalSteps;
  //iTotalSteps = m_pProgress->totalSteps();	                       //ooo
  //iTotalSteps = m_pProgress->value();		                       //xxx
  iTotalSteps = (m_pProgress->maximum() - m_pProgress->minimum());	//xxx
  //iPos = (int)(iTotalSteps * fProgress);           //ooo
  iPos = static_cast<int>(iTotalSteps * fProgress);   //xxx
  //m_pProgress->setProgress (iPos);		//ooo
  m_pProgress->	setValue (iPos);		//xxx
}

void StartStopControl::setStart (float fStart)
{
  //m_pStartStopSlider->setMinValue ((int)(fStart * 1000));                 //ooo
  m_pStartStopSlider->setMinValue ( static_cast<int> ( fStart * 1000 ) );    //xxx
  m_fStart = fStart;
  updateMovieLine ();
}

void StartStopControl::setEnd (float fEnd)
{
  //m_pStartStopSlider->setMaxValue ((int)(fEnd * 1000));               //ooo
  m_pStartStopSlider->setMaxValue ( static_cast<int> ( fEnd * 1000 ) ); //xxx
  m_fEnd = fEnd;
  updateMovieLine ();
}

void StartStopControl::slotSliderReleased ( bool bStart )
{
  emit ( signalSliderReleased ( bStart ) );
}

void StartStopControl::slotSliderPressed ( bool bStart )
{
  emit ( signalSliderPressed ( bStart ) );
}

void StartStopControl::slotSliderValues (int iStart, int iEnd)
{
  // This function is called when the user uses the sliders
  //m_fStart = (float)iStart / 1000.0f; // [ 1.0 .. 0.0 ]                //ooo
  m_fStart = static_cast<float> ( iStart ) / 1000.0f; // [ 1.0 .. 0.0 ]   //xxx
  //m_fEnd   = (float)iEnd   / 1000.0f; // [ 1.0 .. 0.0 ]                //ooo
  m_fEnd   = static_cast<float> ( iEnd )  / 1000.0f; // [ 1.0 .. 0.0 ]  //xxx
  updateMovieLine ();
  emit (signalSliderValues (m_fStart, m_fEnd));

}

void StartStopControl::updateMovieLine ()
{ 
//	if (m_qsFileName.isEmpty())
//		return;
  // For some strange reasons the Dialog grows in size, which we track here and set straight
  if (width()-30 != m_pixmapMovieLine.width())	// 2*12+3 = 2*slider + 2*border
    m_pixmapMovieLine = QPixmap (width() - 2*(12+3) , m_iThumbnailSize);
  // We have all the neccesary information. The screenshots in m_listOfScreenshots etc ...
  int t, iSpaceInBetween, iStartX, iStartY;
  int iAvailableWidth = m_pixmapMovieLine.width();
  //int iNrOfImages = (int)((iAvailableWidth) / m_iThumbnailSize);           //ooo
  int iNrOfImages = static_cast<int> ((iAvailableWidth) / m_iThumbnailSize);  //xxx
  // But we want to ensure to have at least 2 pixels space in between two images
  iAvailableWidth -= 2*iNrOfImages;
  // since this can affect the nrOfImages, we ought to recalculate ...
  //iNrOfImages = (int)((iAvailableWidth) / m_iThumbnailSize);           //ooo
  iNrOfImages = static_cast<int> ((iAvailableWidth) / m_iThumbnailSize);  //xxx

  //iSpaceInBetween = (int)((m_pixmapMovieLine.width() - iNrOfImages * m_iThumbnailSize) / iNrOfImages);         //ooo
  iSpaceInBetween = static_cast<int>((m_pixmapMovieLine.width() - iNrOfImages * m_iThumbnailSize) / iNrOfImages); //xxx

  // Next we create a vanilla background of the MovieLine.
  QBrush blackBrush (QColor (0, 0, 0));
  QPainter thePainter(&m_pixmapMovieLine);
  thePainter.fillRect (0, 0, m_pixmapMovieLine.width(), m_iThumbnailSize, blackBrush);
  // So we are good to go ...
  QPixmap thePixmap;

  for (t=0;t<(int)m_listOfScreenshots.count();t++)	{
    //thePixmap.convertFromImage (*m_listOfScreenshots[t]);             //ooo
    thePixmap.convertFromImage (*m_listOfScreenshots[t]);             //xxx
    //thePixmap.convertFromImage (QImage ( ":/images/please_wait.jpg" ));  //xxx
    iStartX = iSpaceInBetween * (t+1) + m_iThumbnailSize*t;
    //iStartY = (int)((m_pixmapMovieLine.height() - thePixmap.height()) / 2.0);           //ooo
    iStartY = static_cast<int>((m_pixmapMovieLine.height() - thePixmap.height()) / 2.0);    //xxx
    //bitBlt (&m_pixmapMovieLine, iStartX, iStartY, &thePixmap, 0, 0, thePixmap.width(), thePixmap.height(), Qt::CopyROP);	//ooo
    //bitBlt (&m_pixmapMovieLine, iStartX, iStartY, &thePixmap, 0, 0, thePixmap.width(), thePixmap.height());		//xxx
    //QPainter p( &m_pixmapMovieLine );											//xxx
    //p.drawPixmap( iStartX, iStartY, thePixmap, 0, 0, thePixmap.width(), thePixmap.height() );				//xxx
    thePainter.drawPixmap( iStartX, iStartY, thePixmap, 0, 0, thePixmap.width(), thePixmap.height() );			//xxx
  }
  drawStartStopLines ();
  //m_pLabelMovie->setPixmap (m_pixmapMovieLine);   //ooo
  m_pLabelMovie->setPixmap (m_pixmapMovieLine);     //xxx
  /*m_pLabelMovie->setScaledContents         ( true );			//xxx	
  m_pLabelMovie->setAutoFillBackground     ( true );			//xxx
  //m_pLabelMovie->setPaletteBackgroundPixmap (m_pixmapMovieLine);	//ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(m_pixmapMovieLine));	//xxx
  m_pLabelMovie->setPalette(palette);					//xxx
  */    //xxxx
}

void StartStopControl::timerEvent (QTimerEvent *pTimerEvent)
{
  // This function is called when the MdediaEngine has renedered one image.
  // Because of threading issues we ought to use the timer here and not a 
  // simple function call from the background task.
  int t;
  if (pTimerEvent->timerId () == MEDIASCANNER_EVENT + 2)	{ // ExecuteJob::TYPE_IMAGE
    for (t=0;t<(int)m_listOfScreenshots.count();t++)	{
      if (m_listOfScreenshots[t]->width() > m_iThumbnailSize)
        //*m_listOfScreenshots[t] = m_listOfScreenshots[t]->smoothScale (m_iThumbnailSize, m_iThumbnailSize, Qt::KeepAspectRatio);	//ooo
        *m_listOfScreenshots[t] = m_listOfScreenshots[t]->scaled (m_iThumbnailSize, m_iThumbnailSize, Qt::KeepAspectRatio);		//xxx
    }
    updateMovieLine  ();
  }
}

void StartStopControl::drawStartStopLines ()
{
  // This function will mark the start and end of the movie as adjusted through the sliders.
  QColor colorGrey (200, 200, 200);
  QBrush theBrush ( colorGrey, Qt::Dense6Pattern );
  int x1, x2, iWidth, iHeight;

//printf ("StartStopControl::drawStartStopLines <%.4f> <%.4f> w<%d>\n", m_fStart, m_fEnd, m_pixmapMovieLine.width());
  QPainter thePainter (&m_pixmapMovieLine);
  iHeight = m_pixmapMovieLine.height();
  iWidth = m_pixmapMovieLine.width();
  //x1 = (int)(iWidth * m_fStart);          //ooo
  x1 = static_cast<int>(iWidth * m_fStart); //xxx
  //x2 = (int)(iWidth * m_fEnd);            //ooo
  x2 = static_cast<int>(iWidth * m_fEnd);    //xxx
  thePainter.setPen ( Qt::red );
  thePainter.setBrush (theBrush );

  thePainter.drawRect (0, 0, x1, iHeight);
  thePainter.drawRect (x2, 0, iWidth, iHeight);
}

////////////////////////////////////////////////////////////////////////////
//
// StartStopSlider::SliderHandle - sub-class
//
////////////////////////////////////////////////////////////////////////////

SliderHandle::SliderHandle (QWidget *pParent, const char *pName)
	//: QPushButton (pParent, pName)	//ooo
	: QPushButton (pParent)			//xxx
{
  setFixedWidth  ( 12 );
  //setPaletteBackgroundColor (QColor (255, 100, 10));		//ooo
  QPalette palette;						//xxx
  palette.setBrush(QPalette::Window, QColor (255, 100, 10) );	//xxx
  setPalette(palette);						//xxx
  m_bPressed = false;
  m_iMaxX = 100;
  m_iMinX = 0;
}

SliderHandle::~SliderHandle ()
{

}

void SliderHandle::setMinMax (int iMinX, int iMaxX)
{
  m_iMinX = iMinX;
  m_iMaxX = iMaxX;
}

int SliderHandle::getMin ()
{
  return m_iMinX;
}

int SliderHandle::getMax ()
{
  return m_iMaxX;
}

void SliderHandle::mouseMoveEvent (QMouseEvent *pMouseEvent)
{
  if ( m_bPressed )	{
    QPoint thePos = mapTo ((QWidget *)parent(), pMouseEvent->pos());
    int iNewPos = thePos.x()-6;
    // Note that 2 equals the frame width
    if (iNewPos < m_iMinX+2)
      iNewPos = m_iMinX+2;
    if (iNewPos > m_iMaxX-12)
      iNewPos = m_iMaxX - 12;
    move (iNewPos, 2);
    // Note that the middlePosition is half the with more (I.e. 6)
    emit (SliderHandle::signalNewValue(iNewPos+6));
  }
  QPushButton::mouseMoveEvent (pMouseEvent);
}

void SliderHandle::mousePressEvent (QMouseEvent *pMouseEvent)
{
  m_bPressed = true;
  QPushButton::mousePressEvent (pMouseEvent);
  emit ( signalSliderPressed ( ) );
}

void SliderHandle::mouseReleaseEvent (QMouseEvent *pMouseEvent)
{
  m_bPressed = false;
  QPushButton::mouseReleaseEvent (pMouseEvent);
  emit ( signalSliderReleased ( ) );
}

/////////////////////////////////////////////////////////////////////////////
//
// StartStopSlider - class
//
/////////////////////////////////////////////////////////////////////////////

StartStopSlider::StartStopSlider (QWidget *pParent, const char *pName )
	//: QSlider (pParent, pName)	//ooo
	: QSlider (pParent)		//xxx
{
  setOrientation (  Qt::Horizontal );
  m_pHandleStart = new SliderHandle ( pParent , "ButtonStart");
  m_pHandleEnd   = new SliderHandle ( pParent , "ButtonEnd");
  m_pHandleStart->setFixedHeight ( height() - 12);
  m_pHandleEnd  ->setFixedHeight ( height() - 12);
  m_pLastClicked = m_pHandleStart;
  m_pRepeatTimer = NULL;

  m_iMin = 0;
  m_iMax = 100;

  connect ( m_pHandleStart, SIGNAL ( signalSliderPressed ( ) ), this, SLOT ( slotStartPressed  ( ) ) );
  connect ( m_pHandleEnd  , SIGNAL ( signalSliderPressed ( ) ), this, SLOT ( slotEndPressed    ( ) ) );
  connect ( m_pHandleStart, SIGNAL ( signalSliderReleased( ) ), this, SLOT ( slotStartReleased ( ) ) );
  connect ( m_pHandleEnd  , SIGNAL ( signalSliderReleased( ) ), this, SLOT ( slotEndReleased   ( ) ) );
  connect ( m_pHandleStart, SIGNAL ( signalNewValue  ( int ) ), this, SLOT ( slotNewStartValue ( int ) ) );
  connect ( m_pHandleEnd  , SIGNAL ( signalNewValue  ( int ) ), this, SLOT ( slotNewEndValue   ( int ) ) );
}

StartStopSlider::~StartStopSlider ()
{
  delete m_pHandleStart;
  delete m_pHandleEnd;
  if (m_pRepeatTimer)
    delete m_pRepeatTimer;
}

// Public function
void StartStopSlider::setProgress (float, QString &)
{

}

void StartStopSlider::setMinMax (int iMin, int iMax)
{
  m_iMin = iMin;
  m_iMax = iMax;
}

void StartStopSlider::setMin (int iMin)
{
  m_iMin = iMin;
}

void StartStopSlider::setMax (int iMax)
{
  m_iMax = iMax;
}

int  StartStopSlider::getMin ()
{
  return m_iMin;
}

int  StartStopSlider::getMax ()
{
  return m_iMax;
}

void StartStopSlider::setMinValue (int iMin)
{
  if (iMin < m_iMin)
    iMin = m_iMin;
  if (iMin > m_iMax)
    iMin = m_iMax;

  // Next we convert from the user's range to the propper widget's range.
  int iDeltaRange  = m_iMax - m_iMin;
  int iDeltaWidget = width () - 24;
  //int iNewMin = (int)((float)iDeltaWidget / (float)iDeltaRange * iMin);                                    //ooo
  int iNewMin = static_cast<int>(static_cast<float>(iDeltaWidget) / static_cast<float>(iDeltaRange * iMin));   //xxx

  m_pHandleStart->move (iNewMin, 2);
}

void StartStopSlider::setMaxValue (int iMax)
{
  if (iMax < m_iMin)
    iMax = m_iMin;
  if (iMax > m_iMax)
    iMax = m_iMax;

  // Next we convert from the user's range to the propper widget's range.
  int iDeltaRange  = m_iMax - m_iMin;
  int iDeltaWidget = width () - 24;
  //int iNewMax = (int)((float)iDeltaWidget / (float)iDeltaRange * iMax);                                    //ooo
  int iNewMax = static_cast<int>(static_cast<float>(iDeltaWidget) / static_cast<float>(iDeltaRange * iMax));  //xxx

  m_pHandleEnd->move (iNewMax + 12, 2);
}

// Overloaded member functions
void StartStopSlider::paintEvent( QPaintEvent * )
{
  // Here we paint the SliderGroove only ...
  QPainter thePainter( this );
  QStyle::State flags = QStyle::State_None;
  if (isEnabled())
   flags |= QStyle::State_Enabled;
  if (hasFocus())
    flags |= QStyle::State_HasFocus;

  //QStyle::SCFlags sub = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
  QStyle::SubControls sub = QStyle::SC_SliderGroove;
  /*if ( tickmarks() != QSlider::NoTicks )
    sub |= QStyle::SC_SliderTickmarks;*/	//oooo

  QStyleOptionComplex option;							//xxx
  option.initFrom(this);								//xxx
  option.state = flags;								//xxx
  option.subControls = sub;							//xxx
  option.activeSubControls = QStyle::SC_SliderHandle;				//xxx
  style()->drawComplexControl(QStyle::CC_Slider, &option, &thePainter, this); 	//xxx

  //style().drawComplexControl(QStyle::CC_Slider, &thePainter, this, rect(),  colorGroup(), flags, sub, QStyle::SC_SliderHandle);	//ooo
}
	
void StartStopSlider::resizeEvent (QResizeEvent *pEvent)
{
  QSlider::resizeEvent (pEvent);
  m_pHandleStart->setMinMax (m_pHandleStart->getMin(), width()-10);
  m_pHandleEnd  ->setMinMax (m_pHandleEnd  ->getMin(), width()+2);
  // If the resize is too much ...
  if (m_pHandleEnd->x() > width()-10)
    m_pHandleEnd->move (width()-10, 2);
  if (m_pHandleStart->x() > width()-22)
    m_pHandleStart->move (width()-22, 2);
}

void StartStopSlider::mousePressEvent (QMouseEvent *pEvent)
{
  QSlider::mousePressEvent (pEvent);
  moveTowards (pEvent->pos().x());
  if (!m_pRepeatTimer)
    m_pRepeatTimer = new QTimer (this);
  connect( m_pRepeatTimer, SIGNAL(timeout()), SLOT(timeoutRepeat()) );
  //m_pRepeatTimer->start(100, FALSE);	//ooo
  m_pRepeatTimer->start(100);		//xxx
}

void StartStopSlider::mouseReleaseEvent (QMouseEvent *pEvent)
{
  QSlider::mouseReleaseEvent (pEvent);
  if (m_pRepeatTimer)
    delete m_pRepeatTimer;
  m_pRepeatTimer = NULL;
}

void StartStopSlider::timeoutRepeat ()
{
  moveTowards (-1);
}

void StartStopSlider::moveTowards (int iMoveTowards)
{
  static int iPos = iMoveTowards;
  // Here we advance the last touched handle by 
  if (!m_pLastClicked)
    return;
  if (iMoveTowards >= 0)
    iPos = iMoveTowards;

  int iDeltaX = QSlider::pageStep (); // lineStep ();
  int iNewPos = m_pLastClicked->x ();
  if (iPos > m_pLastClicked->x())	{
    iNewPos += iDeltaX;
    if (iNewPos > m_pLastClicked->getMax ())
      iNewPos = m_pLastClicked->getMax ()-12;
  }
  else	{
    iNewPos -= iDeltaX;
    if (iNewPos < m_pLastClicked->getMin ())
      iNewPos = m_pLastClicked->getMin ()+2;
  }
  m_pLastClicked->move (iNewPos, 2);
  // set MinMax and emit signalNewValues ...
  if (m_pLastClicked == m_pHandleStart)
    slotNewStartValue (iNewPos+6);
  else
    slotNewEndValue   (iNewPos+6);
}

void StartStopSlider::keyPressEvent (QKeyEvent *pEvent)
{
  QSlider::keyPressEvent (pEvent);
	
}

void StartStopSlider::show ()
{
  QSlider::show();
  m_pHandleStart->setMinMax (m_pHandleStart->getMin(), width()-10);
  m_pHandleEnd  ->setMinMax (m_pHandleEnd  ->getMin(), width()+2);
  m_pHandleStart->move (2, 2);
  m_pHandleEnd  ->move (width()-10, 2);
}

// Misc functions
void StartStopSlider::slotStartPressed ()
{
  // this function 'marks' the last touched Handle
  m_pLastClicked = m_pHandleStart;
  emit ( signalStartStopPressed ( true ) );
}

void StartStopSlider::slotEndPressed ()
{
  // this function 'marks' the last touched Handle
  m_pLastClicked = m_pHandleEnd;
  emit ( signalStartStopPressed ( false ) );
}

void StartStopSlider::slotStartReleased ()
{
  emit ( signalStartStopReleased ( true ) );
}

void StartStopSlider::slotEndReleased ()
{
  emit ( signalStartStopReleased ( false ) );
}

void StartStopSlider::slotNewStartValue (int iNewValue)
{
  // Called from the Start Handle

  // Okay we have the new Position of the start slider,
  // We want to make this the min for the end slider.
  m_pHandleEnd->setMinMax (iNewValue+4, m_pHandleEnd->getMax());

  // Note, we subtract the 12 width from m_pHandleStart since this is returning
  // only the values between the two sliders inside.
  int iDeltaRange  = m_iMax - m_iMin;
  int iDeltaWidget = width () - 24;
  //int iNewMin = (int)((float)iDeltaRange / (float)iDeltaWidget * iNewValue) - 11;                                          //ooo
  int iNewMin = static_cast<int>(static_cast<float>(iDeltaRange) / static_cast<float>(iDeltaWidget) * iNewValue) - 11;       //xxx
  //int iNewMax = (int)((float)iDeltaRange / (float)iDeltaWidget * m_pHandleEnd->x()-20);                                    //ooo
  int iNewMax = static_cast<int>(static_cast<float>(iDeltaRange) / static_cast<float>(iDeltaWidget) * m_pHandleEnd->x()-20); //xxx
  emit (signalValuesChanged (iNewMin, iNewMax));
}

void StartStopSlider::slotNewEndValue (int iNewValue)
{
  // Called from the stop handle
  // Here we have the new value of the end slider 
  // and we want to make this vbalue the max for the start handle
  m_pHandleStart->setMinMax (m_pHandleStart->getMin (), iNewValue-6);

  // Note, we subtract 12 here bc we want to return the range between the two
  // sliders inside which is [0 .. end-handle.width()]
  int iDeltaRange  = m_iMax - m_iMin;
  int iDeltaWidget = width () - 24;
  //int iNewMin = (int)((float)iDeltaRange / (float)iDeltaWidget * m_pHandleStart->x());                                     //ooo
  int iNewMin = static_cast<int>(static_cast<float>(iDeltaRange) / static_cast<float>(iDeltaWidget) * m_pHandleStart->x());   //xxx
  //int iNewMax = (int)((float)iDeltaRange / (float)iDeltaWidget * (iNewValue - 12)) - 11;                                   //ooo
  int iNewMax = static_cast<int>(static_cast<float>(iDeltaRange) / static_cast<float>(iDeltaWidget) * (iNewValue - 12)) - 11; //xxx
  emit (signalValuesChanged (iNewMin, iNewMax));
}


