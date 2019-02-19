
#include <qpushbutton.h>
//#include <q3iconview.h>   //ooo
#include <QListWidget>      //xxx
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtimer.h>

#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QMouseEvent>
#include <QMessageBox>		//xxx

#include "global.h"
#include "previewer.h"
#include "qdvdauthor.h"

#include "qplayer/mediacreator.h"

namespace Input
{

//Previewer::Previewer ( QWidget *pParent, Q3IconView *pPreview )   //ooo
Previewer::Previewer ( QWidget *pParent, QListWidget *pPreview )    //xxx
  //: uiPreviewer ( pParent )		//ooo
  : QWidget ( pParent )			//xxx
{ 
  setupUi(this);	//xxx
  m_pCurrentItem    = NULL;
  m_pIconView       = pPreview;

  //pParent->layout  ( )->add ( this );     //oooo
  //pParent->layout  ( )->addWidget ( this ); //xxx

  m_pTimerAutoPlay  = new QTimer ( this );
  m_bImageItem      = true;
  m_bPaused         = false;
  m_iCurrentPix     = 0;
  m_pMediaInterface = NULL;
  
  // Next we take care of the buttons
  //m_pButtonPlay->setPixmap ( QPixmap ( ).fromMimeSource ( "player_play.png" ) );	//ooo
  m_pButtonPlay->setIcon ( QPixmap ( ":/images/player_play.png" ) );			//xxx
  //m_pButtonStop->setPixmap ( QPixmap ( ).fromMimeSource ( "player_stop.png" ) );	//ooo
  m_pButtonStop->setIcon ( QPixmap ( ":/images/player_stop.png" ) );			//xxx
  //m_pButtonPrev->setPixmap ( QPixmap ( ).fromMimeSource ( "player_prev.png" ) );	//ooo
  m_pButtonPrev->setIcon ( QPixmap ( ":/images/player_prev.png" ) );			//ooo
  //m_pButtonNext->setPixmap ( QPixmap ( ).fromMimeSource ( "player_next.png" ) );	//ooo
  m_pButtonNext->setIcon ( QPixmap ( ":/images/player_next.png" ) );			//xxx
  
  connect ( m_pTimerAutoPlay, SIGNAL ( timeout ( ) ), this, SLOT ( slotAutoPlay ( ) ) );
  connect ( m_pButtonPlay,    SIGNAL ( clicked ( ) ), this, SLOT ( slotPlay ( ) ) );
  connect ( m_pButtonStop,    SIGNAL ( clicked ( ) ), this, SLOT ( slotStop ( ) ) );
  connect ( m_pButtonPrev,    SIGNAL ( clicked ( ) ), this, SLOT ( slotPrev ( ) ) );
  connect ( m_pButtonNext,    SIGNAL ( clicked ( ) ), this, SLOT ( slotNext ( ) ) );
  
  m_pButtonStop->setEnabled ( false );
  m_pFramePreviewM->hide    ( );
  
  // Next we create the movie bitmap :
  int iBMHeight = 22;
  QPainter thePainter;
  QPixmap  theBackground ( iBMHeight, iBMHeight );
  theBackground.fill     ( QColor ( 0, 0, 0 ) );
  thePainter.begin       ( &theBackground );
  thePainter.setPen      ( QColor ( 255, 255, 255 ) );
  thePainter.setBrush    ( QColor ( 255, 255, 255 ) );
  thePainter.drawRect    (  6,  6, 10, 10 );
  thePainter.end         ( );
  
  m_pLabelTop->setScaledContents         ( true );			//xxx	
  m_pLabelTop->setAutoFillBackground     ( true );			//xxx
  //m_pLabelTop   ->setPaletteBackgroundPixmap ( theBackground );	//ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(theBackground));		//xxx
  m_pLabelTop->setPalette(palette);					//xxx
  
  m_pLabelBottom->setScaledContents         ( true );			//xxx
  m_pLabelBottom->setAutoFillBackground     ( true );			//xxx
  //m_pLabelBottom->setPaletteBackgroundPixmap ( theBackground );	//ooo
  QPalette palette2;							//xxx
  palette2.setBrush(QPalette::Window, QBrush(theBackground));		//xxx
  m_pLabelBottom->setPalette(palette2);					//xxx
  hide ( );
}

Previewer::~Previewer ( )
{
  m_pTimerAutoPlay->stop ( );
  delete m_pTimerAutoPlay;
  m_pTimerAutoPlay = NULL;

  QList<QPixmap *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end ( ) )
    delete *it++;
  m_listOfThumbs.clear ( );

  if ( m_pMediaInterface )
    delete m_pMediaInterface;
  m_pMediaInterface = NULL;
}

//void Previewer::setItem ( Cache::Thumbs::Entry *pCache, Q3IconViewItem *pItem )   //ooo
void Previewer::setItem ( Cache::Thumbs::Entry *pCache, QListWidgetItem *pItem )    //xxx
{
  if ( ! pCache )
    return;

  QImage   largeImg;
  QPixmap  largePix;
  QWidget *pParent = parentWidget  (    );
  QSize    size    = pParent->size (    );
  size.setWidth  ( size.width  ( ) - 30 );
  size.setHeight ( size.height ( ) - 30 );

  m_qsFileName   = pCache->qsFileName; // Only used in slotPlay, IF m_pCUrrentItem == NULL
  m_pCurrentItem = pItem;              // Could be NULL if called from a VirtualFolder / GroupView
  m_pMatrix      = pCache->getMatrix ( );

  setButtonState  ( );
  m_bImageItem = isImage ( pCache->qsFileName );

  if ( ! pCache->arrayOfThumbs )
         pCache->loadImages  ( );

  // If we were currently watching a vid, we should hide the MediInterface.
  if ( m_bImageItem && m_pMediaInterface && m_pMediaInterface->getWidget ( )->isVisible ( ) )  {
    stop ( );
  }
  setMovieFrame ( );

  if ( pCache->getMatrix ( ) )  {
    //largeImg = pCache->arrayOfThumbs[0]->xForm  ( *pCache->getMatrix  ( ) );      //ooo
    largeImg = pCache->arrayOfThumbs[0]->transformed  ( *pCache->getMatrix  ( ) );  //xxx
    //largeImg = largeImg.smoothScale ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio ); //ooo
    largeImg = largeImg.scaled ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio );        //xxx
  }
  else
    //largeImg = pCache->arrayOfThumbs[0]->smoothScale ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio );    //ooo
    largeImg = pCache->arrayOfThumbs[0]->scaled ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio );           //xxx

  //m_pButtonNext->setEnabled ( ( pItem != m_pIconView->lastItem  ( ) ) );      //oooo
  //m_pButtonNext->setEnabled ( ( pItem != m_pIconView->topLevelItem  ( 0 ) ) );  //xxx
  //m_pButtonPrev->setEnabled ( ( pItem != m_pIconView->firstItem ( ) ) );      //ooo
  //m_pButtonPrev->setEnabled ( ( pItem != m_pIconView->item ( 0 ) ) );   //xxx

  //if ( ( pItem == m_pIconView->lastItem  ( ) ) || ( pItem == m_pIconView->lastItem  ( ) ) )   //ooo
  if ( ( pItem == m_pIconView->item  ( m_pIconView->count ( ) ) ) ) //xxx
    m_pTimerAutoPlay->stop ( );   //oooo

  largePix.convertFromImage ( largeImg );
  m_pPreview ->setPixmap    ( largePix );
  if ( m_pIconView->isVisible ( ) )  {
       m_pIconView->hide      ( );

    move   ( 2, 2 );
    resize ( size.width ( ) - 4, size.height ( ) - 4 );
    show   ( );
    raise  ( );
  }

  QTimer::singleShot  ( 5, this, SLOT ( slotShowImageInfo ( ) ) );
}

//void Previewer::setItem ( Q3IconViewItem *pItem ) //ooo
void Previewer::setItem ( QListWidgetItem *pItem )  //xxx
{
  if ( ! pItem )
    return;

  //QString qsFileName  = pItem->key ( );   //oooo
  QString qsFileName  = pItem->text ( );  //xxx
  m_bImageItem = isImage  ( qsFileName );

  Cache::Thumbs::Entry *pCache = Global::pThumbsCache->find ( qsFileName, m_bImageItem );
  setItem ( pCache, pItem );
}

bool Previewer::isImage ( QString &qsFileName )
{
  QFileInfo fileInfo ( qsFileName );
  QString qsVids = Global::pApp->getImageFilter ( );
  //if ( qsVids.find ( fileInfo.extension ( FALSE ) ) > -1 )    //ooo
  if ( qsVids.indexOf ( fileInfo.suffix ( ) ) > -1 )            //xxx
    return true;
  return false;
}

void Previewer::setMovieFrame ( )
{
  if ( m_bImageItem )  {
    if ( m_pLabelTop->isVisible ( ) )
         m_pPreview->resize( m_pPreview->width ( ), m_pPreview->height ( ) - 44 );
    m_pLabelTop->hide    ( );
    m_pLabelBottom->hide ( );
  }
  else  {
    if ( !m_pLabelTop->isVisible ( ) )
          m_pPreview->resize( m_pPreview->width ( ), m_pPreview->height ( ) + 44 );
    m_pLabelTop->show    ( );
    m_pLabelBottom->show ( );
  }
}

void Previewer::setButtonState ( )
{
  if ( ! m_pCurrentItem )
    return;
  // Enables / disables buttons based on the item being first last, or neither.
}

void Previewer::mouseReleaseEvent ( QMouseEvent *pEvent )
{
  if ( pEvent->pos ( ).x ( ) > width ( ) - 180 )
    return;

  clear ( );
}

void Previewer::clear ( )
{
  stop ( );
  hide ( );
  m_pIconView->show ( );
/*  if ( m_pCurrentItem )
    m_pIconView->ensureItemVisible ( m_pCurrentItem );*/    //oooo
}

void Previewer::slotLoadWholeImage ( )
{
  if ( ! m_pCurrentItem )
    return;

  QWidget *pParent  =  parentWidget  (    );
  QSize    size     =  pParent->size (    );
  size.setWidth    ( size.width  ( ) - 30 );
  size.setHeight   ( size.height ( ) - 30 );
  //QImage  theImage ( m_pCurrentItem->key( ) );    //oooo
  QImage  theImage ( m_pCurrentItem->text( ) );   //xxx
  QPixmap thePixmap;

  if ( theImage.isNull ( ) )
    return;

  if ( m_pMatrix )  {
    //theImage = theImage.xForm ( *m_pMatrix );     //ooo
    theImage = theImage.transformed ( *m_pMatrix ); //xxx
    //theImage = theImage.smoothScale ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio ); //ooo
    theImage = theImage.scaled ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio );        //xxx
  }
  else
    //theImage = theImage.smoothScale ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio ); //ooo
    theImage = theImage.scaled ( size.width( ) - 180, size.height( ), Qt::KeepAspectRatio );        //xxx
//  theImage = theImage.smoothScale ( size, QImage::ScaleMin );

  thePixmap.convertFromImage ( theImage  );
  m_pPreview ->setPixmap     ( thePixmap );

  m_pIconView->clearSelection ( );
  m_pCurrentItem->setSelected ( true );
}

void Previewer::slotShowImageInfo ( )
{
  if ( ! m_pCurrentItem )
    return;

  QString qsText, qsFileName;
  jhead::JHead *pPlugin = Global::pThumbsCache->plugin ( );
  jhead::JHead::Info *pInfo = NULL;

  //qsFileName = m_pCurrentItem->key ( );   //oooo
  qsFileName = m_pCurrentItem->text ( );  //xxx
  QFileInfo fileInfo ( qsFileName );

  qsText  = tr ( "<font size=\"-4\"><table width=\"100%\">" );
  if ( m_bImageItem )
    qsText += tr ( "<tr><td colspan=\"2\" bgcolor=\"#ffee44\"><b>%1</b></td></tr>" ).arg ( fileInfo.fileName ( ) );
  else
    qsText += tr ( "<tr><td colspan=\"2\" bgcolor=\"#6622ff\"><b>%1</b></td></tr>" ).arg ( fileInfo.fileName ( ) );

  if ( pPlugin && m_bImageItem )  {
    pInfo = pPlugin->getFileInfo ( qsFileName );
    if ( pInfo )  {
      QString qsOrientation = "Upside Down";

//      qsText += tr ( "<tr><td colspan=\"2\" bgcolor=\"#ff0000\"><b></b></td></tr>" );
      qsText += tr ( "<tr><td>Make :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->qsCamera );
      qsText += tr ( "<tr><td>Model :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->qsModel );
      qsText += tr ( "<tr><td>Date :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->imageDate.toString ( "yyyy/MM/dd hh:mm:ss" ) );
//      qsText += tr ( "<tr><td>Orient.:</td><td><b>%1</b></td></tr>" ).arg ( qsOrientation );
      qsText += tr ( "<tr><td>Res. :</td><td><b>%1 x %2</b></td></tr>" ).arg ( pInfo->iWidth ).arg ( pInfo->iHeight );
      if ( ! pInfo->bIsColor )
        qsText += tr ( "<tr><td>Color/bw :</td><td><b>Black and White</b></td></tr>" );
      qsText += tr ( "<tr><td>Flash :</td><td><b>%1</b></td></tr>" ).arg ( ( pInfo->iFlashFlags & 1 ) ? "Yes" : "No");
      qsText += tr ( "<tr><td>Focal :</td><td><b>%1mm</b></td></tr>" ).arg ( pInfo->fFocalLength, 0, 'g', 2 );
      qsText += tr ( "<tr><td>CCDWidth :</td><td><b>%1mm</b></td></tr>" ).arg ( pInfo->fCCDWidth, 0, 'g', 2 );
      qsText += tr ( "<tr><td>Exposure :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fExposureTime );
      qsText += tr ( "<tr><td>Focal :</td><td><b>f/%1</b></td></tr>" ).arg ( pInfo->fAperture );
      if ( pInfo->fDigitalZoom > 1.0f )
        qsText += tr ( "<tr><td>Digi Zoom :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fDigitalZoom );
      if ( pInfo->iISOEquiv )
        qsText += tr ( "<tr><td>ISO Equiv :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->iISOEquiv );
      if ( pInfo->fExposureBias > 0.0 )
        qsText += tr ( "<tr><td>Expos Bias :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fExposureBias );
      qsText += tr ( "<tr><td>Whitebal :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->bWhitebalance ? "Manual" : "Auto" );

//    qsText += tr ( "<tr><td>Focal :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fFocalLength );
//    qsText += tr ( "<tr><td>Focal :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fFocalLength );
//    qsText += tr ( "<tr><td>Focal :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fFocalLength );
//    qsText += tr ( "<tr><td>Focal :</td><td><b>%1</b></td></tr>" ).arg ( pInfo->fFocalLength );

//    GPSInfo  *pGPSInfo;
//    int       iLightsource;
//    int       iOrientation;
//    int       iFlashFlags;
//    int       iExposureProg;
//    int       iDistanceRange;

      if ( ! pInfo->qsComment.isEmpty ( ) ) 
        qsText += tr ( "<tr><td colspan=\"2\" bgcolor=\"#33ff33\"><b>%1</b></td></tr>" ).arg ( pInfo->qsComment );
      qsText += tr ( "</table></font>" );
      delete pInfo;
    }
  }
  if ( ! pInfo )  {
    qsText += tr ( "<tr><td>Size :</td><td><b>%1</b></td></tr>"    ).arg ( fileInfo.size     ( ) );
    qsText += tr ( "<tr><td>Created :</td><td><b>%1</b></td></tr>" ).arg ( fileInfo.created  ( ).toString ( "yyyy/MM/dd hh:mm:ss" ) );
    if ( ! pPlugin )
      qsText += tr ( "</table><p><b>Could not find libjhead plugin</b></font>" );
  }
  m_pLabelInfo->setText ( qsText );

  if ( m_bImageItem )
   QTimer::singleShot ( 5, this, SLOT ( slotLoadWholeImage ( ) ) );
}

void Previewer::slotAutoPlay ( )
{
  if ( ! m_pCurrentItem )
    return;

  //setItem ( m_pCurrentItem->nextItem ( ) );                               //ooo
  setItem ( m_pCurrentItem->listWidget ( )->item ( m_pCurrentItem->listWidget ( )->row ( m_pCurrentItem ) + 1 ) ); //xxx
}

void Previewer::slotFirst ( )
{
  if ( ! m_pCurrentItem )
    return;

  //setItem ( m_pIconView->firstItem ( ) );     //ooo
  setItem ( m_pIconView->item ( 0 ) );  //xxx
}

void Previewer::slotLast ( )
{
  if ( ! m_pCurrentItem )
    return;

  //setItem ( m_pIconView->lastItem ( ) );  //oooo
}

void Previewer::slotNext ( )
{
  if ( ! m_pCurrentItem )
    return;

  slotStop ( );
  //setItem  ( m_pCurrentItem->nextItem ( ) );                              //ooo
  setItem ( m_pCurrentItem->listWidget ( )->item ( m_pCurrentItem->listWidget ( )->row ( m_pCurrentItem ) + 1 ) ); //xxx
}

void Previewer::slotPrev ( )
{
  if ( ! m_pCurrentItem )
    return;

  slotStop ( );
  //setItem  ( m_pCurrentItem->prevItem ( ) );                              //ooo
  setItem ( m_pCurrentItem->listWidget ( )->item ( m_pCurrentItem->listWidget ( )->row ( m_pCurrentItem ) - 1  ) ); //xxx
}

void Previewer::slotPlay ( )
{       
  if ( m_bPaused )  {
    //m_pButtonPlay->setPixmap  ( QPixmap ( ).fromMimeSource ( "player_play.png" ) );	//ooo
    m_pButtonPlay->setIcon  ( QPixmap ( ":/images/player_play.png" ) );         		//xxx
    if ( m_bImageItem )
      m_pTimerAutoPlay->stop   ( );
    else {
      if ( m_pTimerAutoPlay->isActive ( ) )  { // We started the slideshow but now we have a movieItem as active
           m_pTimerAutoPlay->stop ( );
           m_pButtonStop->setEnabled ( false );
           //m_pButtonPlay->setPixmap  ( QPixmap ( ).fromMimeSource ( "player_play.png" ) );	//ooo
	       m_pButtonPlay->setIcon  ( QPixmap ( ":/images/player_play.png" ) );             		//xxx
           m_bPaused = false;
           return;
      }
      if ( m_pMediaInterface )
           m_pMediaInterface->pause ( );
    }
  }
  else  {
    if ( m_bImageItem )  {
      if ( m_pMediaInterface && m_pMediaInterface->getWidget ( )->isVisible ( ) )
           slotStop ( );

      m_pTimerAutoPlay->start ( 3000 ); // Set timer to a nicely paced 1.5 seconds.
    }
    else   {
      m_pFramePreview->hide  ( );
      m_pFramePreviewM->show ( );
      if ( ! m_pMediaInterface )  {
        m_pMediaInterface = MediaCreator::createPreferredWidget ( m_pFramePreviewM, "MediaWidget", NULL );
        QLayout *pLayout  = m_pFramePreviewM->layout ( );
        //pLayout->add      ( m_pMediaInterface->getWidget ( ) );       //ooo
        pLayout->addWidget      ( m_pMediaInterface->getWidget ( ) );   //xxx
        m_pMediaInterface->initMediaEngine ( );
      }
      if ( m_pMediaInterface )  {
           //QString qsFileName = ( m_pCurrentItem ) ? m_pCurrentItem->key ( ) : m_qsFileName;  //oooo
           QString qsFileName = ( m_pCurrentItem ) ? m_pCurrentItem->text ( ) : m_qsFileName; //xxx
           m_pMediaInterface->getWidget ( ) ->show ( );
           m_pMediaInterface->playMRL   ( qsFileName );
      }
      QTimer::singleShot ( 10, this, SLOT ( slotResizeM ( ) ) );
    }
    //m_pButtonPlay->setPixmap  ( QPixmap ( ).fromMimeSource ( "player_pause.png" ) );	//ooo
    m_pButtonPlay->setIcon  ( QPixmap ( ":/images/player_pause.png" ) );        		//xxx
    m_pButtonStop->setEnabled ( true );
  }

  m_bPaused = ! m_bPaused;
}

void Previewer::slotResizeM ( )
{
//  m_pPreviewM->resize ( m_pFramePreviewM->width ( ) - 4,  m_pFramePreviewM->height ( ) - 4 );
}

void Previewer::slotStop ( )
{
  if ( m_bImageItem )  {
    m_pTimerAutoPlay->stop ( );
  }
  if ( m_pMediaInterface )  {
    m_pMediaInterface->stop ( );
    m_pMediaInterface->getWidget ( )->hide ( );
  }
  m_pButtonStop->setEnabled ( false );
  //m_pButtonPlay->setPixmap  ( QPixmap ( ).fromMimeSource ( "player_play.png" ) );	//ooo
  m_pButtonPlay->setIcon  ( QPixmap ( ":/images/player_play.png" ) );   			//xxx   
  m_pFramePreviewM->hide ( );
  m_pFramePreview->show  ( );
  m_bPaused = false;
}

void Previewer::stop ( )
{
  if ( m_bImageItem )  {
    if ( m_pTimerAutoPlay->isActive ( ) )
         m_pTimerAutoPlay->stop ( );
  }
  if ( m_pMediaInterface && m_pMediaInterface->getWidget ( )->isVisible ( ) )  {
    if ( m_pMediaInterface->isPlaying ( ) )
         m_pMediaInterface->stop ( );
    m_pMediaInterface->getWidget ( )->hide ( );
    m_pFramePreviewM->hide ( );
    m_pFramePreview->show  ( );
  }
  m_pButtonStop->setEnabled ( false );
  //m_pButtonPlay->setPixmap  ( QPixmap ( ).fromMimeSource ( "player_play.png" ) );	//ooo
  m_pButtonPlay->setIcon  ( QPixmap ( ":/images/player_play.png" ) );   			//xxx
  m_bPaused = false;
}

}; // End of namespace Input
