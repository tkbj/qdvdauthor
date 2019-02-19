/***************************************************************************
    dvdlayoutgui.cpp
                             -------------------
    Class DVDLayout::Gui
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This is a sub-project to 'Q' DVD-Author to display the current DVD Layout
    
****************************************************************************/

#include <qtimer.h>
#include <qlayout.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qlineedit.h>
//#include <q3popupmenu.h>  //ooo
#include <qpushbutton.h>
//Added by qt3to4:
//#include <Q3HBoxLayout>   //ooo
#include <QHBoxLayout>      //xxx
#include <QKeyEvent>
//#include <Q3ValueList>	//oxx
#include <QLabel>
//#include <Q3GridLayout>   //ooo
#include <QGridLayout>      //xxx
//#include <Q3Frame>        //ooo
#include <QFrame>           //xxx
//#include <q3mimefactory.h>    //ooo
#include <QMouseEvent>
#include <QEvent>
#include <QMessageBox>

#include "global.h"
#include "dvdmenu.h"
#include "xml_dvd.h"
#include "qdvdauthor.h"
#include "messagebox.h"
#include "cachethumbs.h"  //ooo
#include "dvdlayoutgui.h"
#include "dvdlayoutarea.h"
#include "sourcefileentry.h"

namespace DVDLayout
{

const int Object::BUTTON_SIZE  =  17;
const int Object::INITIAL_SIZE = 200;
const int Gui::REFRESH_MENU    = 123;

//Header::Header ( QWidget *pParent, Object *pObject, const char *pName, Qt::WFlags flags )     //ooo
Header::Header ( QWidget *pParent, Object *pObject, const char *pName, Qt::WindowFlags flags )  //xxx
  //: QLabel ( pParent, pName, flags )  //ooo
  : QLabel ( pParent )                  //xxx
{
  m_bMousePressed = false;
  m_pObject       = pObject;
}

Header::~Header ( )
{

}

void Header::displayText ( bool bDisplayText )
{
  if ( bDisplayText )
    QLabel::setText ( m_qsText );
  else
    QLabel::setText ( trUtf8 ( "" ) );
}

void Header::setText ( const QString &string )
{
  m_qsText = string;
  QLabel::setText ( m_qsText );
}


// these are normal mouse events event
void Header::mousePressEvent ( QMouseEvent *pMouseEvent )
{
  m_bMousePressed = true;
  m_pointInitial  = pMouseEvent->pos ( );
  parentWidget ( )->parentWidget ( )->raise ( );
  m_pObject->setActiveObject ( m_pObject );
}

void Header::mouseReleaseEvent( QMouseEvent * )
{
  m_bMousePressed = false;
  // This signal is sent to The FunctionPluginPainter::slotRedrawAll.
  // From there through signalRedrawAll to the ConstructionArea
  m_pObject->setActiveObject ( NULL );
  emit ( signalRedrawAll ( ) );
  // Here goes the code to Update the UndoBufferObject in the ConstructionArea.
}

void Header::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
  if ( m_bMousePressed )  {
    QWidget   *pParent = parentWidget ( )->parentWidget ( );
    QPoint parentPoint = pParent->mapToParent ( pMouseEvent->pos ( ) - m_pointInitial );
    pParent->move ( parentPoint );
//  QPoint parentPoint = parentWidget()->mapToParent (pMouseEvent->pos()-m_pointInitial);
//  parentWidget()->move(parentPoint);
    m_pObject->moveActiveObject ( );
  }
}

void Header::mouseDoubleClickEvent ( QMouseEvent * )
{
  printf ("MouseDoubleClicked ...\n");
}


//Object::Object ( QWidget *pParent, const char *pName, Qt::WFlags flags )      //ooo
Object::Object ( QWidget *pParent, const char *pName, Qt::WindowFlags flags )   //xxx
  //: Q3Frame ( pParent, pName, flags ) //ooo
  : QFrame ( pParent )                 //xxx
{
  // Okay, to set the style of the PluginFunctionPainter ...
  m_objectSize     = QSize  ( INITIAL_SIZE, INITIAL_SIZE );
  m_ptPosition     = QPoint ( 0, 0 );
  m_fZoom          = 1.00f;
  m_bGridActivated = false;
  m_gridSize       = QSize ( 10, 10 );
  m_pArea          = (Area *)pParent;

  //setAcceptDrops  ( TRUE );   //ooo
  setAcceptDrops  ( true );     //xxx
  //setFrameShape   ( Q3Frame::NoFrame );   //ooo
  setFrameShape   ( QFrame::NoFrame );      //xxx
  setLineWidth    ( 2 );
  setMidLineWidth ( 1 );
  //m_pMainLayout = new Q3HBoxLayout   ( this, 0, 0, "m_pMainLayout" ); //ooo
  m_pMainLayout = new QHBoxLayout   ( this );                            //xxx

  //m_pInnerFrame = new Q3Frame        ( this, "m_pInnerFrame" );   //ooo
  m_pInnerFrame = new QFrame        ( this );                       //xxx
  //m_pInnerFrame->setFrameShape      ( Q3Frame::StyledPanel ); //ooo
  m_pInnerFrame->setFrameShape      ( QFrame::StyledPanel );    //xxx
  //m_pInnerFrame->setFrameShadow     ( Q3Frame::Raised );      //ooo
  m_pInnerFrame->setFrameShadow     ( QFrame::Raised );         //xxx
  m_pInnerFrame->setLineWidth       ( 2 );
  m_pInnerFrame->setMidLineWidth    ( 1 );
  m_pMainLayout->addWidget          ( m_pInnerFrame );

  //m_pFrameLayout  = new Q3GridLayout ( m_pInnerFrame, 1, 1, 3, 2, "m_pFrameLayout" );     //ooo
  m_pFrameLayout  = new QGridLayout ( m_pInnerFrame );                                       //xxx
  //m_pHeaderLayout = new Q3HBoxLayout ( 0, 0, 0,       "m_pHeaderLayout"  );   //ooo
  m_pHeaderLayout = new QHBoxLayout (   );                                      //xxx

  m_pHeader = new Header            ( m_pInnerFrame, this, "m_pLabelHeader" );
  m_pHeader->setMaximumSize         ( QSize  (   32767, BUTTON_SIZE ) );
  //m_pHeader->setPaletteBackgroundColor       ( QColor ( 255, 170, 0 ) );  //ooo
  QPalette palette;								                     //xxx
  palette.setBrush(QPalette::Base, QBrush(QColor ( 255, 170, 0 )));	//xxx
  m_pHeader->setPalette(palette);						             //xxx
  QFont labelHeader_font            (  m_pHeader->font() );
  labelHeader_font.setFamily        ( "Helvetica" );
  labelHeader_font.setPointSize     ( 6 );
  m_pHeader->setFont                ( labelHeader_font );
  //m_pHeader->setAlignment           ( int( Qt::AlignCenter ) );   //ooo
  m_pHeader->setAlignment           ( Qt::AlignCenter );  //xxx
  m_pHeader->setScaledContents      ( true );
  QSizePolicy sizePol = m_pHeader->sizePolicy ( );
  //sizePol.setHorData                ( QSizePolicy::Ignored ); //oooo
  m_pHeader->setSizePolicy          ( sizePol );
  m_pHeaderLayout->addWidget        ( m_pHeader );
  // Connect the header -> this -> ConstructionArea ...
  QObject::connect ( m_pHeader, SIGNAL ( signalRedrawAll ( ) ), this, SLOT ( slotRedrawAll ( ) ) );

  //m_pButtonInfo = new QPushButton   ( m_pInnerFrame,   "m_pButtonInfo" ); //ooo
  m_pButtonInfo = new QPushButton   ( m_pInnerFrame );                      //xxx
  m_pButtonInfo->setMaximumSize     ( QSize ( BUTTON_SIZE, BUTTON_SIZE ) );
  //m_pButtonInfo->setPaletteBackgroundColor  ( QColor  (  255, 255, 127 ) );   //ooo
  QPalette palette2;								//xxx
  palette2.setBrush(QPalette::Base, QBrush(QColor  (  255, 255, 127 )));	//xxx
  m_pButtonInfo->setPalette(palette2);						//xxx
  QFont buttonInfo_font             (  m_pButtonInfo->font ( ) );
  buttonInfo_font.setFamily         ( "Helvetica" );
  buttonInfo_font.setPointSize      ( 6 );
  m_pButtonInfo->setFont            ( buttonInfo_font );
  m_pButtonInfo->setText            ( trUtf8  ( "i" ) );
  m_pHeaderLayout->addWidget        ( m_pButtonInfo );
  m_pFrameLayout->addLayout         ( m_pHeaderLayout, 0, 0 );

  //m_pInsideWidget = new QLabel      ( m_pInnerFrame, "m_pInsideWidget" ); //ooo
  m_pInsideWidget = new QLabel      ( m_pInnerFrame );                      //xxx
  //m_pInsideWidget->setPaletteBackgroundColor ( QColor ( 170, 255, 255 ) );    //ooo
  QPalette palette3;								//xxx
  palette3.setBrush(QPalette::Base, QBrush(QColor ( 170, 255, 255 )));	//xxx
  m_pInsideWidget->setPalette(palette3);						//xxx
  //m_pInsideWidget->setAlignment     ( int ( Qt::AlignCenter ) );  //ooo
  m_pInsideWidget->setAlignment     ( Qt::AlignCenter );            //xxx

  m_pFrameLayout->addWidget         ( m_pInsideWidget, 1, 0 );

  // Now for the header buttonshttps://login.vzw.com/cdsso/public/controller?action=logout
  connect ( m_pButtonInfo, SIGNAL ( clicked ( ) ), this,    SLOT ( slotInfo      ( ) ) );
  connect ( this,  SIGNAL ( signalRedrawAll ( ) ), pParent, SLOT ( slotRedrawAll ( ) ) );
}

Object::~Object ( )
{

}

void Object::slotInfo ( )
{

}

void Object::slotRedrawAll ( )
{
  // Call DVDLayout::Area::slotRefreshAll ()
  emit ( signalRedrawAll ( ) );
}

void Object::setZoom ( float fZoom )
{
  if ( fZoom == m_fZoom )
    return;

  QPoint thePos = pos ( );
  float  fDelta = 1.0f + ( fZoom - m_fZoom );

  m_fZoom = fZoom;
  float fHeaderHeight = BUTTON_SIZE * m_fZoom + 8.0f; // give some extra to account fo the layout
  setFixedSize ( (int)( m_objectSize.width ( ) * m_fZoom + 6.0f ), (int)( m_objectSize.height ( ) * m_fZoom + fHeaderHeight ) );
  resizeHeader ( );

  thePos.setX ( (int)( thePos.x ( ) * fDelta ) );
  thePos.setY ( (int)( thePos.y ( ) * fDelta ) );
  move ( thePos );
}

void Object::setSize ( int iWidth, int iHeight )
{
  // This will set the size of the object without the header.
  m_objectSize = QSize ( iWidth, iHeight );
}

QSize Object::size ( )
{
  QSize theSize;
  theSize.setWidth  ( (int)( m_objectSize.width  ( ) * m_fZoom ) );
  theSize.setHeight ( (int)( m_objectSize.height ( ) * m_fZoom ) );
  return theSize;
}

Object::enType Object::type ( )
{
  return TYPE_UNKNOWN;
}

void Object::resizeHeader ( )
{
  float fButtonSize = BUTTON_SIZE * m_fZoom;
  m_pHeader->setMaximumSize  ( QSize ( 32767, (int)fButtonSize ) );
  m_pButtonInfo->setFixedSize( QSize ( (int)fButtonSize, (int)fButtonSize ) );
  if ( m_fZoom < 0.5f )    {
    m_pHeader->displayText ( false );
    m_pButtonInfo->setText ( trUtf8 ( "" ) );
  }
  else    {
    m_pHeader->displayText ( true );
    m_pButtonInfo->setText ( trUtf8 ( "i" ) );
  }
}

void Object::setThumbnail ( )
{  
  if ( m_thumbnail.isNull ( ) )
    return;

  QImage  theImage;
  QPixmap thePix;
  QSize   theSize = size ( );

  //theImage = m_thumbnail.smoothScale ( theSize, Qt::KeepAspectRatio );				//ooo
  theImage = m_thumbnail.scaled ( theSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );		//xxx
  thePix.convertFromImage    ( theImage );
  m_pInsideWidget->setPixmap (   thePix );
}

void Object::keyPressEvent ( QKeyEvent *pKey )
{
  if ( pKey->key ( ) == Qt::Key_Left )  {
    move ( pos ( ).x ( ) - 1, pos ( ).y ( ) );
    slotRedrawAll   ( );
  }
  if ( pKey->key ( ) == Qt::Key_Right )  {
    move ( pos ( ).x ( ) + 1, pos ( ).y ( ) );
    slotRedrawAll   ( );
  }
  if ( pKey->key ( ) == Qt::Key_Up )  {
    move ( pos ( ).x ( ), pos ( ).y ( ) - 1 );
    slotRedrawAll   ( );
  }
  if ( pKey->key ( ) == Qt::Key_Down )  {
    move ( pos ( ).x ( ), pos ( ).y ( ) + 1 );
    slotRedrawAll   ( );
  }
}

void Object::enterEvent ( QEvent * )
{
  grabKeyboard ( );
}

void Object::leaveEvent ( QEvent * )
{
  releaseKeyboard ( );
}

void Object::mousePressEvent ( QMouseEvent *pMouseEvent )
{
  if ( pMouseEvent->button ( ) == Qt::LeftButton )  {
    m_bMousePressed = true;
    m_pointInitial  = pMouseEvent->pos ( );
    raise ( );
    setActiveObject ( this );
  }
  else  { // Right click, Sow context menu
    if ( type ( ) == TYPE_MOVIE )  {
      SourceFileEntry *pEntry = (SourceFileEntry *)getObject ( );
      QPoint  pos    = pMouseEvent->globalPos (     );
      //Global::pApp->createContextMenu ( pEntry, pos );    //oooo
    }
    else if ( type ( ) == TYPE_MENU )  {
      QPoint      pos   = pMouseEvent->globalPos ( );
      DVDMenu    *pMenu = (DVDMenu *)getObject   ( );
      /*Q3PopupMenu popuMenu ( NULL );
      popuMenu.insertItem ( tr ( "Proprties ..." ), pMenu, SLOT ( slotEditTimeline ( ) ) );
      popuMenu.exec ( pos, 2 );*/   //oooo
    }
  }
}

void Object::mouseReleaseEvent( QMouseEvent * )
{
  m_bMousePressed = false;
  setActiveObject ( NULL );
  slotRedrawAll   ( );
}

void Object::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
  if ( m_bMousePressed )  {
    QPoint parentPoint = mapToParent ( pMouseEvent->pos ( ) - m_pointInitial );
    move ( parentPoint );
    moveActiveObject ( );
  }
}

void Object::setActiveObject ( Object *pObject )
{
  if ( m_pArea )
       m_pArea->setActiveObject ( pObject );
}

void Object::moveActiveObject ( )
{
  if ( m_pArea )
       m_pArea->moveActiveObject ( );
}

// Main container to depict a movie
MovieObject::MovieObject ( QWidget *pParent, SourceFileEntry *pEntry )
  : Object ( pParent )
{
  // MovieObjects have a slight green tint header
  //m_pHeader->setPaletteBackgroundColor     ( QColor (  0, 170,  0 ) );    //ooo
  QPalette palette;								//xxx
  palette.setBrush(QPalette::Base, QBrush(QColor (  0, 170,  0 )));	//xxx
  m_pHeader->setPalette(palette);						//xxx
  //m_pButtonInfo->setPaletteBackgroundColor ( QColor ( 60, 255, 90 ) );    //ooo
  QPalette palette2;								//xxx
  palette2.setBrush(QPalette::Base, QBrush(QColor ( 60, 255, 90 )));	//xxx
  m_pButtonInfo->setPalette(palette2);						//xxx
  m_pSourceFileEntry = pEntry;
  m_pHeader->setText ( getObjectName ( ) );
  createThumb ();
  refresh    (  );
  setZoom  ( 1.0f );
}

MovieObject::~MovieObject ( )
{
}

Object::enType MovieObject::type ( )
{
  return TYPE_MOVIE;
}

void MovieObject::refresh ( )
{
  setThumbnail ( );
}

void *MovieObject::getObject ( )
{
  return m_pSourceFileEntry;
}

QString MovieObject::getObjectName ( )
{
  QString qsDummy;
  if ( ! m_pSourceFileEntry )
    return qsDummy;
  return m_pSourceFileEntry->qsDisplayName;
}

void MovieObject::createThumb ( )
{
  if ( ! m_pSourceFileEntry )
    return;
  if( m_pSourceFileEntry->listFileInfos.size ( ) < 1 )
    return;

  SourceFileInfo *pInfo = NULL;
  pInfo = m_pSourceFileEntry->listFileInfos[0];
  if ( ! pInfo )
    return;

  QImage theImage;
  if ( pInfo->pPreview && ! pInfo->pPreview->isNull ( ) )
    theImage = *pInfo->pPreview;
  else  {
    Cache::Thumbs::Entry *pCache = Global::pThumbsCache->find ( pInfo->qsFileName );
    if ( pCache && ! pCache->arrayOfThumbs )
         pCache->loadImages  ( );

    if ( pCache && pCache->arrayOfThumbs && pCache->arrayOfThumbs[0] )
         theImage = *pCache->arrayOfThumbs[0];
  }

  if ( theImage.isNull ( ) )
    return;

  // We only need half the size so we won't have to re-scale the full image all the time.
  //theImage = theImage.smoothScale ( INITIAL_SIZE, INITIAL_SIZE, Qt::KeepAspectRatio );			//ooo
  theImage = theImage.scaled ( INITIAL_SIZE, INITIAL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
  // Next we draw the movieLines.
  m_thumbnail = drawMovieLines ( theImage );
  setSize  ( m_thumbnail.width ( ), m_thumbnail.height ( ) );
}

QImage MovieObject::drawMovieLines ( QImage &theImage )
{
  const  int iHoleSize = 14;
  int t, iX, iHeight, iWidth, iCount;
  iWidth  = theImage.width ( );
  iHeight = theImage.height ( );
  iCount  = ( iWidth / iHoleSize ) + 1; // the number of holes to draw

  QColor  colorGrey ( 200, 200, 200 ), colorBlack ( 0, 0, 0 );
  QBrush  theBrush  ( colorGrey ); //, QBrush::Dense6Pattern );
  QPixmap pix ( iWidth, iHeight + 2 * iHoleSize );
  pix.fill ( colorBlack );

  QPainter thePainter   ( &pix );
  thePainter.setBrush   ( theBrush );
  thePainter.drawImage  ( 0, iHoleSize, theImage );
  for ( t=0; t<iCount; t++ )  {
    iX = t * iHoleSize + 3;
    thePainter.drawRect ( iX,                   2, iHoleSize-4, iHoleSize-4 );
    thePainter.drawRect ( iX, iHeight+iHoleSize+2, iHoleSize-4, iHoleSize-4 );
  }

  //QImage finalImage = pix.convertToImage ( ); //ooo
  QImage finalImage = pix.toImage ( );          //xxx
  return finalImage;
}

// Main container to depict a DVD Menu
MenuObject::MenuObject ( QWidget *pParent, DVDMenu *pDVDMenu )
  : Object ( pParent )
{
  m_pDVDMenu = pDVDMenu;
  m_pHeader->setText ( getObjectName ( ) );
  createThumb ();
  refresh    (  );
  setZoom  ( 1.0f );
}

MenuObject::~MenuObject ( )
{
}

Object::enType MenuObject::type ( )
{
  return TYPE_MENU;
}

void MenuObject::createThumb ( )
{
  QString qsFileName;
  QImage  theImage;
  Utils   theUtil;

  qsFileName = theUtil.getTempFile ( m_pDVDMenu->name ( ) + QString ("/") + QString ( BACKGROUND_NAME ) );
  m_pDVDMenu->createMask ( qsFileName, BACKGROUND_IMG, true );
  QFileInfo fileInfo ( qsFileName );

  if ( ! fileInfo.exists ( ) )
    return;

  theImage.load  ( qsFileName );
  if ( theImage.isNull ( ) )
    return;

  // We only need half the size so we won't have to re-scale the full image all the time.
  //theImage = theImage.smoothScale ( INITIAL_SIZE, INITIAL_SIZE, Qt::KeepAspectRatio );			//ooo
  theImage = theImage.scaled ( INITIAL_SIZE, INITIAL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
  setSize ( theImage.width ( ), theImage.height ( ) );

  m_thumbnail = theImage;
}

void MenuObject::refresh ( )
{
  setThumbnail ( );
}

void *MenuObject::getObject ( )
{
  return (void *)m_pDVDMenu;
}

QString MenuObject::getObjectName ( )
{
  QString qsDummy;
  if ( ! m_pDVDMenu )
    return qsDummy;
  return m_pDVDMenu->getInterface ( )->qsMenuName;
}

// This class handles the controls, such as Zoom slider, menu items etc.
// It is also the document that keeps the DVDLayout objects
//Gui::Gui ( QWidget *pParent, const char *pName, Qt::WFlags flags)	//ooo
Gui::Gui ( QWidget *pParent )						//xxx
  //: uiDVDLayout  ( pParent, pName, flags )	//ooo
  : QWidget ( pParent )				//xxx
{
  setupUi(this);	//xxx
  //Q3HBoxLayout *m_pMainLayout = new Q3HBoxLayout ( m_pFramePreview, 2, 2, "m_pMainLayout");   //ooo
  QHBoxLayout *m_pMainLayout = new QHBoxLayout ( m_pFramePreview );                             //xxx
  m_pArea      = new  Area ( m_pFramePreview, "DVDLayout::Area" );
  m_pMainLayout->addWidget ( m_pArea );

  //QPixmap pix  = qPixmapFromMimeSource ( "printer.png" );	//ooo
  QPixmap pix ( ":/images/printer.png" );			//xxx
  //QIcon ButtonIcon(pix);    //xxx
  //QIcon ButtonIcon( ":/images/printer.png" );    //xxx
  //m_pButtonPrinter->setPixmap ( pix );    //oooo
  m_pButtonPrinter->setIcon ( pix );    //xxx
  m_pButtonPrinter->setIconSize(pix.rect().size());    //xxx
  //m_pButtonPrinter->setIcon ( ButtonIcon );    //xxx
  //m_pButtonPrinter->setIconSize(pix.rect().size());    //xxx
  //m_pButtonPrinter->setFixedSize(pix.rect().size());   //xxx
  m_iPreviousSliderValue = -2000;

  connect ( m_pButtonPrinter, SIGNAL ( clicked       (     ) ), this, SLOT ( slotPrinter      (     ) ) );
  connect ( m_pEditZoom,      SIGNAL ( lostFocus     (     ) ), this, SLOT ( slotZoomChanged  (     ) ) );
  connect ( m_pSliderZoom,    SIGNAL ( valueChanged  ( int ) ), this, SLOT ( slotZoomChanged  ( int ) ) );
  connect ( m_pSliderZoom,    SIGNAL ( sliderMoved   ( int ) ), this, SLOT ( slotSliderMoved  ( int ) ) );
  connect ( m_pSliderZoom,    SIGNAL ( sliderReleased(     ) ), this, SLOT ( slotZoomChanged  (     ) ) );
  connect ( m_pComboRouter,   SIGNAL ( activated     ( int ) ), this, SLOT ( slotRouterChanged( int ) ) );
}

Gui::~Gui ( )
{
  clear ( );
}

void Gui::clear ( )
{
  Object *pObject = NULL;
  QList<Object *>::iterator it = m_listOfObjects.begin ( );	//oxx
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    m_pArea->deleteContainer ( pObject );
    delete pObject;
  }
  m_listOfObjects.clear ( );
  m_listOfVideos.clear  ( );
  m_listOfMenus.clear   ( );
}

bool Gui::readProjectFile ( QDomNode &xmlNode )
{
  return m_pArea->readProjectFile ( xmlNode );
}

bool Gui::writeProjectFile ( QDomElement &rootElement )
{
  // Store : Zoom, Object Position, PanPos
  return m_pArea->writeProjectFile ( rootElement );
}

void Gui::slotPostRead ( )
{
  // this function is called after readProjectFile went through all structures
  // and created all required objects ( DVDMenus, SourceFileEntries )
  float fAreaZoom = m_pArea->getZoom ( );
  int iZoom;
  QString qsZoom;

  if ( fAreaZoom < 0.1f )
       fAreaZoom = 0.1f;

  // Set the Zoom text
  qsZoom.sprintf ( "%0.2f%%", fAreaZoom * 100.0f );
  m_pEditZoom->setText ( qsZoom );

  // Set the slider pos.
  if  ( fAreaZoom < 1.0f )
    iZoom = (int)( ( ( fAreaZoom * 100.0f -  10.0f ) * 1000.0f /  90.0f ) - 1000.0f ); // range [ 0 .. -1000 ]
  else
    iZoom = (int)  ( ( fAreaZoom * 100.0f - 100.0f ) * 1000.0f / 900.0f ); // range [ 0 .. +1000 ]

  m_pSliderZoom->setValue ( iZoom );
  //m_pComboRouter->setCurrentItem ( m_pArea->routerType ( ) ); //ooo
  m_pComboRouter->setCurrentIndex ( m_pArea->routerType ( ) );  //xxx
  refresh ( false );
}

void Gui::slotRecreateThumb ( )
{
  QList<Object *>::iterator it = m_listRecreateThumbs.begin ( );	//oxx
  while ( it != m_listRecreateThumbs.end ( ) )  {
    Object *pObject = *it++;
    //m_listRecreateThumbs.remove ( pObject );  //ooo
    m_listRecreateThumbs.removeOne ( pObject ); //xxx
    if ( m_listOfMenus.contains ( pObject ) > 0 )  {
      pObject->createThumb  ( );
      pObject->setThumbnail ( );
      QTimer::singleShot ( 100, this, SLOT ( slotRecreateThumb ( ) ) );
      return;
    }
  }
}

void Gui::refreshMenus ( )
{  
  // Called after switching main tab to DVDLayout
  m_listRecreateThumbs.clear ( );

  QList<Object *>::iterator it = m_listOfMenus.begin ( );	//oxx
  while ( it != m_listOfMenus.end (   )   )
      m_listRecreateThumbs.append ( *it++ );
  QTimer::singleShot ( 100, this, SLOT ( slotRefresh ( ) ) );
  QTimer::singleShot ( 200, this, SLOT ( slotRecreateThumb ( ) ) );
}

void Gui::slotRefresh ( )
{
  refresh (      );
  setZoom ( m_pArea->getZoom ( ) );
  refresh ( true );
}

void Gui::refresh ( )
{
  // Main function, to non-destructively refresh the DVDLayout structure
  Object *pObject = NULL;
  QList<Object *> listTemp = m_listOfVideos;		//oxx
  QList<Object *>::iterator itTemp;			//oxx

  // First we make sure all sources are taken care of.
  SourceFileEntry *pEntry = NULL;
  uint t, iEntries = Global::pApp->sourceFileCount ( );
  for ( t=0; t<iEntries; t++ )  {   
    pEntry = Global::pApp->sourceFileEntry  ( t ); 
    if ( ( ! pEntry ) || ( pEntry->bSoundSource ) )
      continue;

    pObject = checkEntry ( pEntry );
 
    if ( pObject )
      //listTemp.remove ( pObject );    //ooo
      listTemp.removeOne ( pObject );   //xxx
  }

  // All objects not found can be deleted.
  itTemp = listTemp.begin ( );
  while ( itTemp != listTemp.end ( ) )   {
    pObject = *itTemp++;
    //m_listOfObjects.remove   ( pObject );     //ooo
    m_listOfObjects.removeOne   ( pObject );    //xxx
    //m_listOfVideos.remove    ( pObject );     //ooo
    m_listOfVideos.removeOne    ( pObject );    //xxx
    m_pArea->deleteContainer ( pObject );
    delete pObject;
  }

  // First we check if the VMGMenu is already handled
  listTemp = m_listOfMenus;
  pObject  = checkMenu ( Global::pApp->getVMGMenu ( ) );
  if ( pObject )
    //listTemp.remove ( pObject );      //ooo
    listTemp.removeOne ( pObject );     //xxx

  // Next we do the same for all sub menus.
  DVDMenu  *pDVDMenu = NULL;
  QList<DVDMenu *> list = Global::pApp->getSubMenus ( );	//oxx
  QList<DVDMenu *>::iterator it = list.begin ( );		//oxx

  while ( it != list.end ( ) )  {
    pDVDMenu = *it++;
    pObject = checkMenu ( pDVDMenu );
    if ( pObject )
      //listTemp.remove   ( pObject  );     //ooo
      listTemp.removeOne   ( pObject  );    //xxx
  }

  // All objects not found can be deleted.
  itTemp = listTemp.begin ( );
  while ( itTemp != listTemp.end ( ) )  {
    pObject = *itTemp++;
    //m_listOfObjects.remove   ( pObject );     //ooo
    m_listOfObjects.removeOne   ( pObject );    //xxx
    //m_listOfMenus.remove     ( pObject );     //ooo
    m_listOfMenus.removeOne     ( pObject );    //xxx
    m_pArea->deleteContainer ( pObject );
    delete pObject;
  }
}

void Gui::refresh ( bool /* bAll */ )
{
  Object *pObject = NULL;
  QList<Object *>::iterator it = m_listOfObjects.begin ( );	//oxx
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    pObject->refresh ( );
  }

  // After we refreshed all Menus / Movies we had better refresh the Lines too.
  //m_pArea->createConnections ( m_listOfObjects ); //oooo-bug
  m_pArea->drawConnections   ( );			//ooo
  m_pArea->update ( ); //drawConnections   ( );		//xxx
}

Object *Gui::checkEntry ( SourceFileEntry *pEntry )
{
  Object *pEntryObject = findObject ( pEntry, false );
  if  ( ! pEntryObject )  { // DVDMenu is not yet displayed in the DVDLayout tab.
     createEntry ( pEntry );
     return NULL;
  }
  return pEntryObject;
}

Object *Gui::checkMenu ( DVDMenu *pDVDMenu )
{
  Object *pMenuObject = findObject ( pDVDMenu, true );
  if  ( ! pMenuObject )  { // DVDMenu is not yet displayed in the DVDLayout tab.
     createMenu ( pDVDMenu );
     return NULL;
  }
  return pMenuObject;
}

Object *Gui::findObject ( void *pRealObject, bool bDVDMenu )
{
  Object *pObject = NULL;
  QList<Object *>::iterator it;		//oxx
  if ( bDVDMenu )  {
    it = m_listOfMenus.begin ( );
    while ( it != m_listOfMenus.end ( ) )  {
      pObject = *it++;
      if ( pObject->getObject ( ) == pRealObject )
        return pObject;
    }
  }
  else  {
    it = m_listOfVideos.begin ( );
    while ( it != m_listOfVideos.end ( ) )  {
      pObject = *it++;
      if ( pObject->getObject ( ) == pRealObject )
        return pObject;
    }
  }

  return NULL;
}

Object *Gui::findObject ( void *pRealObject )
{
  Object *pObject = NULL;
  QList<Object *>::iterator it = m_listOfObjects.begin ( );	//oxx
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    if ( pObject->getObject ( ) == pRealObject )
      return pObject;
  }

  return NULL;
}

void Gui::updateEntry ( SourceFileEntry *pEntry )
{
  if ( ! pEntry )
    return;

  Object *pObject = findObject ( pEntry );
  if ( !  pObject )
    return;

  pObject->createThumb  ( );
  pObject->setThumbnail ( );
  pObject->setZoom ( m_pArea->getZoom ( ) + 0.00001 );
}

void Gui::createEntry ( SourceFileEntry *pEntry )
{
  if ( ( ! pEntry ) || ( pEntry->bSoundSource ) )
    return;

  QPoint pos = initialPos ( false );
  MovieObject *pMovie = NULL;
  pMovie = new MovieObject ( m_pArea, pEntry );
  pMovie->move ( pos.x ( ), pos.y ( ) );
  pMovie->show ( );

  // Finally we can add it to the propper lists
  m_listOfObjects.append ( pMovie );
  m_listOfVideos.append  ( pMovie );

  QTimer::singleShot ( 10, this, SLOT ( slotAdjustZoom ( ) ) );
}

void Gui::createMenu ( DVDMenu *pDVDMenu )
{
  if ( ! pDVDMenu )
    return;

  QPoint pos = initialPos ( true );

  MenuObject *pMenu = NULL;
  pMenu = new MenuObject ( m_pArea, pDVDMenu );
  pMenu->move ( pos.x ( ),     pos.y ( ) );
  pMenu->setFixedSize ( pMenu->size  ( ) );
  pMenu->show ( );

  // Finally we can add it to the propper lists
  m_listOfObjects.append ( pMenu );
  m_listOfMenus.append   ( pMenu );

  QTimer::singleShot ( 10, this, SLOT ( slotAdjustZoom ( ) ) );
}

QPoint Gui::initialPos ( bool bMenu )
{
  // We need to put some brains in here to offer a nice looking initial display.
  const int iMARGIN   = 20; // Objects distance from border and each other.
  const int iMAX_COLS =  5; // Max objects in one row before we add a new row

//  slotAdjustZoom ( );
  float fZoom = m_pArea->getZoom ( );
  float fAdjustedWidth = Object::INITIAL_SIZE * fZoom;
  float fAdjustedPos   = iMARGIN * fZoom;
  float fAdjustedSize  = fAdjustedWidth + fAdjustedPos;
  float fMaxWidth      = iMAX_COLS * ( fAdjustedWidth + fAdjustedPos );

  QPoint newPos  ( (int)fAdjustedPos, (int)fAdjustedPos );
  QRect  objRect ( (int)fAdjustedPos, (int)fAdjustedPos, (int)fAdjustedWidth, (int)fAdjustedWidth );
  //////////////////////////////////////////////////////////////////////////////////////
  // Since there is no perfect logic for all cases lets follow this approach:         //
  // o Menus on the top and videos on the bottom.                                     //
  // o Add videos MARGIN below the lowest Menu.                                       //
  // o Add a max of  5 objects in a row, and adjust scale to display all in one view  //
  // o If a New Menu will create a new row of menus ( E.g. Menu # 11 ),               //
  //   then shift all videos one row lower.                                           //
  // o When adding new Object, make sure we won't overlay existing Object             //
  //////////////////////////////////////////////////////////////////////////////////////
  int iMaxMenuY  = 0;
  int iMaxVideoY = 0;
  int iMinVideoY = 100000000;
  // To do the right thing we need to get the MaxVideoY
  QList<Object *>::iterator it = m_listOfVideos.begin ( );	//oxx
  while ( it != m_listOfVideos.end ( ) )  {
    MovieObject *pVideo =(MovieObject *)*it++;
    if ( iMaxVideoY < pVideo->pos ( ).y ( ) + pVideo->height ( ) )
         iMaxVideoY = pVideo->pos ( ).y ( ) + pVideo->height ( );
    if ( iMinVideoY > pVideo->pos ( ).y ( ) )
         iMinVideoY = pVideo->pos ( ).y ( );
  }
  it = m_listOfMenus.begin ( );
  while ( it != m_listOfMenus.end ( ) )  {
    MenuObject *pMenu= (MenuObject *)*it++;
    if ( iMaxMenuY   < pMenu->pos ( ).y ( ) + pMenu->height ( ) )
         iMaxMenuY   = pMenu->pos ( ).y ( ) + pMenu->height ( );
  }

  if ( bMenu )  {
    MenuObject *pMenu;
    QRect rect;
    it = m_listOfMenus.begin ( );
    while ( it != m_listOfMenus.end ( ) )  {
      pMenu = (MenuObject *)*it++;
      rect  = QRect ( pMenu->pos ( ).x ( ), pMenu->pos ( ).y ( ), pMenu->width ( ), pMenu->height ( ) );
      if ( objRect.intersects ( rect ) )  {
        // First we make sure we don't exceed the max width
        if ( rect.left ( ) + fAdjustedSize > fMaxWidth )
          objRect = QRect ( (int)fAdjustedPos, (int)( fAdjustedSize ) + objRect.top ( ), (int)fAdjustedWidth, (int)fAdjustedWidth );
        else
          objRect = QRect ( rect.right ( ) + (int)( fAdjustedPos ), objRect.top ( ), objRect.width ( ), objRect.height ( ) );

        // retry withthe new pos.
        it = m_listOfMenus.begin ( );
        continue;
      }
    }
    newPos = QPoint ( objRect.left ( ), objRect.top ( ) );
    // At this point we ensured that we do not interfere with any menu
    // lets check if we started a new row ...
    if ( iMaxMenuY-iMARGIN < objRect.bottom ( ) )  {
      // Let us go through all videos and move them down one row.
      MovieObject *pVideo;
      QRect rect;
      if (   ( iMinVideoY < iMaxMenuY ) ||
           ( ( iMinVideoY > iMaxMenuY ) && ( iMinVideoY < iMaxMenuY + fAdjustedPos * 3 ) ) )  {
        // Now we can go through all videos and shift them down below the Menus.
        int  iDeltaY = (int)( fAdjustedPos * 3 ) + iMaxMenuY - iMinVideoY;
        it = m_listOfVideos.begin ( );
        while ( it != m_listOfVideos.end ( ) )  {
          pVideo = (MovieObject *)*it++;
          pVideo->move ( pVideo->pos ( ).x ( ), pVideo->pos ( ).y ( ) + iDeltaY );
        }
        return newPos;
      }
    }
  }
  else {  // Get the initial pos for a video Object
    QRect   rect;
    Object *pObject;
    if ( m_listOfVideos.size ( ) < 1 )
      iMinVideoY = iMaxMenuY + (int)( fAdjustedSize );
    objRect = QRect ( (int)fAdjustedPos, (int)iMinVideoY, (int)fAdjustedWidth, (int)fAdjustedWidth );
    it = m_listOfObjects.begin ( );
    while ( it != m_listOfObjects.end ( ) )  {
      pObject   = *it++;
      rect = QRect ( pObject->pos ( ).x ( ), pObject->pos ( ).y ( ), pObject->width ( ), pObject->height ( ) );
      if ( objRect.intersects ( rect ) )  {
        // First we make sure we don't exceed the max width
        if ( objRect.left ( ) + fAdjustedSize > fMaxWidth )
          objRect = QRect ( (int)fAdjustedPos, (int)( fAdjustedSize ) + objRect.top ( ), (int)fAdjustedWidth, (int)fAdjustedWidth );
        else
          objRect = QRect ( objRect.left ( ) + (int)( fAdjustedSize ), objRect.top ( ), objRect.width ( ), objRect.height ( ) );
        it = m_listOfObjects.begin ( );
        continue;
      }
    }
    newPos = QPoint ( objRect.left ( ), objRect.top ( ) );
  }
  return newPos;
}

void Gui::slotPrinter ( )
{
  m_pArea->print ( );
}

void Gui::slotGrid ( )
{
//  MessageBox::information ( this, tr ( "Under construction" ), tr ( "Not yet implemented" ) );
//  m_pArea->refresh ( );
  refresh ( true );
}

void Gui::slotRouterChanged ( int iWhich )
{
  m_pArea->setRouterType ( iWhich );
}

void Gui::slotZoomChanged ( )
{
  // Called when the slider is released or when the user changed the value manually
  QString qsValue = m_pEditZoom->text ( );
  qsValue = qsValue.remove ( '%' );
  float  fValue   = qsValue.toFloat ( );
  if ( ( fValue < 10.0f ) || ( fValue > 1000.0f ) ) {
    fValue = 100.0;
  }

  qsValue.sprintf ( "%0.2f%%", fValue );
  m_pEditZoom->setText ( qsValue );
  setZoom ( fValue / 100.0f );
  refresh ( false );
}

void Gui::slotSliderMoved ( int iValue )
{
  // Dragging the slider calls both slotSliderMoved(int) plus slotZoomChanged(int) ( with same value )
  // Stepping through left mouse click on the slider only calls slotZoomChanged(int)
  m_iPreviousSliderValue = iValue;
}

void Gui::slotZoomChanged ( int iValue )
{
  // iValue => [ -1000 .. +1000 ] == [ 0.1 .. 10.0 ]
  QString qsZoom;
  float fZoom = 100.0f;
  if  ( iValue < 0 )
    fZoom = 10.0f + ( 1000.0f + iValue ) * 90.0f / 1000.0f;
  else
    fZoom = 100.0f + iValue / 1000.0f * 900.0f;
  qsZoom.sprintf ( "%0.2f%%", fZoom );

  m_pEditZoom->setText ( qsZoom );
  setZoom ( fZoom / 100.0f );

  // Execute only if we advance by left clicking on the QSlider object.
  if ( m_iPreviousSliderValue == -2000 ) //!= iValue )
    refresh ( false );

  m_iPreviousSliderValue = -2000;
}

void Gui::setZoom ( float fZoom )
{
  // This function will update the zoom of all objects.
  Object *pObject = NULL;
  m_pArea->setZoom ( fZoom );
  QList<Object *>::iterator it = m_listOfObjects.begin ( );	//oxx
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    pObject->setZoom ( fZoom );
  }
}

void Gui::slotAdjustZoom ( )
{
  // This function will adjust the zoom such that all objects are visible on the screen.
  Object *pObject = NULL;
  int iMaxPosX = 0;
  int iMaxPosY = 0;
  QList<Object *>::iterator it = m_listOfObjects.begin ( );	//oxx
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    if ( iMaxPosX < pObject->pos ( ).x ( ) + pObject->width  ( ) )
         iMaxPosX = pObject->pos ( ).x ( ) + pObject->width  ( ) ;
    if ( iMaxPosY < pObject->pos ( ).y ( ) + pObject->height ( )  )
         iMaxPosY = pObject->pos ( ).y ( ) + pObject->height ( ) ;
  }

  float fAreaZoom = m_pArea->getZoom ( );
  iMaxPosX += (int)( 20.0 * fAreaZoom );
  iMaxPosY += (int)( 20.0 * fAreaZoom );

  if ( ( iMaxPosX > m_pArea->width ( ) ) || ( iMaxPosY > m_pArea->height ( ) ) )  {
    // Okay here we need to adjust the Zoom factor
    QString qsZoom;
    float    fZoom[2];
    int      iZoom;

    fZoom[0] = (float)m_pArea->width  ( ) / iMaxPosX;
    fZoom[1] = (float)m_pArea->height ( ) / iMaxPosY;
    if ( fZoom[0] < fZoom[1] )
       fAreaZoom = fAreaZoom * fZoom[0];
    else
       fAreaZoom = fAreaZoom * fZoom[1];

    if ( fAreaZoom < 0.1f )
         fAreaZoom = 0.1f;
 
    // Set the Zoom text
    qsZoom.sprintf ( "%0.2f%%", fAreaZoom * 100.0f );
    m_pEditZoom->setText ( qsZoom );

    // Set the slider pos.
    if  ( fAreaZoom < 1.0f )
      iZoom = (int)( ( ( fAreaZoom - 1.0f ) * 1000.0f ) / 0.9f ); // range [ 0 .. -1000 ]
    else
      iZoom = (int)( ( fAreaZoom - 1.0f ) * 1000.0f / 900.0f );
   
    m_pSliderZoom->setValue ( iZoom );    //ooo

    setZoom ( fAreaZoom );
 
    refresh (   false   );
  }
}

}; // End namespace DVDLayout
