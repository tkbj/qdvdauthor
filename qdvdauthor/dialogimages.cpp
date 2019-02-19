/***************************************************************************
    dialogimages.cpp
                             -------------------
    DialogImages
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <stdlib.h>

#include <qtimer.h>
#include <qregexp.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qmatrix.h>
#include <qcheckbox.h>
#include <qcombobox.h>
//#include <q3iconview.h>   //ooo
#include <QListView>        //xxx
#include <qtabwidget.h>
//#include <q3popupmenu.h>  //ooo
#include <QMenu>            //xxx
#include <qpushbutton.h>
#include <qradiobutton.h>
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qapplication.h>
#include <qinputdialog.h>

#include <qtoolbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	//oxx
#include <QPixmap>
#include <QFileInfo>    //xxx

#include "utils.h"
#include "global.h"
#include "previewer.h"
#include "qdvdauthor.h"
#include "messagebox.h"
#include "dialogimages.h"
#include "xml_slideshow.h"
#include "slidedefaults.h"
#include "dialogslideshow.h"  //ooo
#include "qplayer/mediacreator.h" // for MEDIASCANNER_EVENT - id


#define MAX_THREADS          8
// The following IDs are signals to DialogImages::timerEvent
#define EVENT_UPDATE_ICONS  200
#define EVENT_DELETE_WORKER 201


// TODO:
//
// o  AutoGroup needs enhancement.
//   - Group by Directory
//   - Group by Timeiff
//   - Group by NrOfImages
//   - Group by StarRating
//

/*
#ifndef DEBUG_INFO
#define debug_cout printf
#else
void debug_cout(const char *, ...){};
#endif
*/

namespace Input
{

/****************************************************************
 **
 ** Here is the implemenation of the DialogImages - class
 ** Basically te same dialog but tailored to images / Slideshows
 ** for easy usage from within the OneClickDVD - intr=erface.
 **
 ****************************************************************/
//DialogImages::DialogImages ( QWidget *pParent, bool bIsImageDialog, const char *pName, Qt::WFlags f )		//ooo
DialogImages::DialogImages ( QWidget *pParent, bool bIsImageDialog )						//xxx
  //: DialogFiles ( pParent, bIsImageDialog, pName, f )		//ooo
  : DialogFiles ( pParent, bIsImageDialog )			//xxx
{
}

DialogImages::~DialogImages ( )
{
  m_pIAmAlive =  NULL;
  lockDialog      ( );
  clearWorker     ( );
  clearSlideshows ( );
  unlockDialog    ( );
}

void DialogImages::initMe ( SourceFileEntry *pEntry )
{
  if ( pEntry )
    m_listOfGroups.append ( pEntry );

  DialogFiles::initMe ( );
  m_pContextMenu = NULL;
  m_pPreviewer   = new Previewer ( m_pFramePreview, m_pPreview );
  m_pButtonPlay->setText ( tr ( "&View ..." ) );
  m_pButtonReload->hide  ( );  // no real purpose for this button in here ...

  m_pButtonEdit->setText ( "Edit" );
  //QWidget *pGroupTab = m_pTabWidget->page ( 2 );  //ooo
  QWidget *pGroupTab = m_pTabWidget->widget ( 2 );  //xxx
  if ( pGroupTab )
    //m_pTabWidget->setTabLabel ( pGroupTab, QString ( "Slideshows" ) );    //ooo
    m_pTabWidget->setTabText ( m_pTabWidget->indexOf ( pGroupTab ), QString ( "Slideshows" ) ); //xxx

  m_iMaxThreads = 2; // default to 2 worker threads
  if ( getenv ( "MAX_IMG_RENDER_THREADS" ) )  {
    uint iEnv = QString ( getenv ( "MAX_IMG_RENDER_THREADS" ) ).toUInt ( );
    if ( ( iEnv > 0 ) && ( iEnv <= MAX_THREADS ) )
      m_iMaxThreads = iEnv;
  }
  //connect ( m_pPreview, SIGNAL ( doubleClicked ( Q3IconViewItem * ) ), this, SLOT ( slotPreview ( Q3IconViewItem * ) ) );                 //ooo
  connect ( m_pPreview, SIGNAL ( itemDoubleClicked ( QListWidgetItem * ) ), this, SLOT ( slotPreview ( QListWidgetItem * ) ) );   //xxx
}

void DialogImages::slotEditGroup ( )
{
  int        iIdx       = m_pToolbox->currentIndex        ( );
  //QString    qsLabel    = m_pToolbox->itemLabel         ( iIdx ); //ooo
  QString    qsLabel    = m_pToolbox->itemText         ( iIdx );    //xxx
  //GroupView *pGroupView = (GroupView *)m_pToolbox->item ( iIdx ); //ooo
  GroupView *pGroupView = (GroupView *)m_pToolbox->widget ( iIdx ); //xxx
  if ( ! pGroupView )
    return;

  CXmlSlideshow *pSlideshow = pGroupView->slideshow ( );
  if ( ! pSlideshow )  {
    pSlideshow = new CXmlSlideshow;
    pSlideshow->slideshow_name = qsLabel;
    Global::pSlideDefaults->initSlideshow ( pSlideshow );
//    populateFromGroup ( pSlideshow, pGroupView );
    pGroupView->setSlideshow ( pSlideshow );
  }

  populateFromGroup ( pSlideshow, pGroupView );
  DialogSlideshow theDialog ( this );
  theDialog.initMe    ( pSlideshow );

  if ( theDialog.exec ( ) == QDialog::Accepted )  {
    pSlideshow->slideshow_name = theDialog.m_pEditName->text  ( );
    //m_pToolbox->setItemLabel ( iIdx, pSlideshow->slideshow_name );    //ooo
    m_pToolbox->setItemText ( iIdx, pSlideshow->slideshow_name );       //xxx
    pSlideshow->background       = theDialog.background ( );
    pSlideshow->delay            = theDialog.m_pSpinImageDelay->value    ( );
    pSlideshow->filter_delay     = theDialog.m_pSpinFilterDelay->value   ( );
    pSlideshow->intro_page       = theDialog.m_pCheckIntroPage->isChecked( );
    pSlideshow->audio_list       = theDialog.audioList  ( );
    pSlideshow->validFilterNames = theDialog.filterList ( );
    //int iFormat = theDialog.m_pComboFormat->currentItem ( );  //ooo
    int iFormat = theDialog.m_pComboFormat->currentIndex ( );   //xxx
    pSlideshow->yres             = ( ( iFormat == 0 ) || ( iFormat == 2 ) ) ? 480 : 576;
    pSlideshow->xres             = 720;
    pSlideshow->aspect           = ( ( iFormat == 0 ) || ( iFormat == 1 ) ) ? 0 : 1;  // 4:3 or 16:9
    pSlideshow->setImgBkgImg ( theDialog.m_pRadioImgBkgImg->isChecked ( ) );
    pSlideshow->setKenBurns  ( theDialog.m_pCheckKenBurns ->isChecked ( ) );
  }
}

void DialogImages::playFromGroup ( Cache::Thumbs::Entry *pCache )
{
  // Here we add or modify a comment to an image.
  //QStringList emptyList;
  if ( ! pCache )
    return;

  QFileInfo fileInfo ( pCache->qsFileName );
  QString qsHeader = tr ( "Comment for this %1" ).arg ( fileInfo.fileName ( ) );
  QString qsLabel  = tr ( "Comment : " );
  QString qsComment = pCache->qsComment;

  bool bOkay;
  //qsComment  = QInputDialog::getText ( qsHeader, qsLabel, QLineEdit::Normal, qsComment, &bOkay ); //ooo
  qsComment  = QInputDialog::getText ( this, qsHeader, qsLabel, QLineEdit::Normal, qsComment, &bOkay );
  if ( ! bOkay )
    return;

  pCache->qsComment = qsComment;
  if ( ! qsComment.isEmpty ( ) )
    pCache->bStoreData = true;
//  else
//    pCache->bStoreData = false;

  Global::pThumbsCache->saveImageDB ( );
}

void DialogImages::initGroups ( )
{
  if ( m_listOfGroups.size ( ) < 1 )  {
    DialogFiles::initGroups    ( );
    return;
  }
  // We are coming from the context menu of a slideshow and do only want to handle this single one
  m_pButtonNew->hide    ( );
  m_pButtonDelete->hide ( );
  connect ( m_pButtonEdit,   SIGNAL ( clicked ( ) ), this, SLOT ( slotEditGroup   ( ) ) );
  
  SourceFileEntry *pEntry = m_listOfGroups[0];
  m_listOfGroups.clear ( );
  addGroup ( pEntry );
}

GroupView *DialogImages::addGroup ( SourceFileEntry *pEntry )
{  
  // DialogImages is only interested in Slideshow Entries.
  if ( ! pEntry || pEntry->listFileInfos.count ( ) != 1 )
    return NULL;

  SourceFileInfo *pInfo = pEntry->listFileInfos[0];
  if ( ! pInfo->pSlideshow )
    return NULL;

  Cache::Thumbs::Entry       *pCache;
  CXmlSlideshow::img_struct  *pXMLImg;
  CXmlSlideshow::time_object *pXMLObj;
  CXmlSlideshow    *pSlideshow = new CXmlSlideshow;
  *pSlideshow    = *pInfo->pSlideshow;
  pSlideshow->id = (void *)pEntry;

  GroupView       *pGroupView = new GroupView ( m_pToolbox, m_pPreview, this, pSlideshow );
  GroupView::Item *pItem      = NULL;

  QFileInfo  fileInfo;
  QPixmap    thePixmap, errorPixmap;
  //QImage     theImage ( QImage ( ).fromMimeSource ( "error.jpg" ) );		//ooo
  QImage     theImage ( QImage ( ":/images/error.jpg" ) );			//xxx
  //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize ); //ooo
  theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize );        //xxx
  errorPixmap.convertFromImage ( theImage );

  int t, iCount;
  bool bAlternate = true;

  iCount = pSlideshow->count ( );
//  for ( t=0; t<iCount; t++ )  {
  for ( t=iCount-1; t>=0; t-- )  {
    pXMLObj = pSlideshow->getTimeObject ( t );
    if ( ! pXMLObj )
      continue;
    if ( ( pXMLObj->node_name != "vid" ) && ( pXMLObj->node_name != "img" ) )
      continue;

    pXMLImg = (CXmlSlideshow::img_struct *)pXMLObj;
    pCache  = Global::pThumbsCache->find ( pXMLImg->src, isImageDialog ( pXMLImg->src ) );
    if ( ! pCache )
           pCache = Global::pThumbsCache->append ( pXMLImg->src, (QImage *)NULL );    //ooooo

    fileInfo.setFile ( pXMLImg->src );
    bAlternate = ! bAlternate;
    pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), bAlternate );
    pItem->pSourceFileInfo = pInfo;
    if ( pCache )  {
      pItem->pCache = pCache;
      if ( ! pCache->arrayOfThumbs )
             pCache->loadImages  ( );
      if ( pCache->arrayOfThumbs && pCache->arrayOfThumbs[0] )  {
          if ( pCache->getMatrix ( ) )  {
               //theImage = pCache->arrayOfThumbs[0]->xForm ( *pCache->getMatrix ( ) );     //ooo
            theImage = pCache->arrayOfThumbs[0]->transformed ( *pCache->getMatrix ( ) );    //xxx
               //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );   //ooo
               theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );          //xxx
          }
          else if ( ( pCache->fRotate != 0.0 ) && ( pCache->bIsVideo ) )  {
               QMatrix matrix;
               matrix.rotate ( pCache->fRotate );
               //theImage = pCache->arrayOfThumbs[0]->xForm ( matrix );     //ooo
               theImage = pCache->arrayOfThumbs[0]->transformed ( matrix ); //xxx
               //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );   //ooo
               theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );          //xxx
          }
          else
            //theImage = pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio ); //ooo
            theImage = pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );        //xxx

        thePixmap.convertFromImage ( theImage );
        //pItem->setPixmap ( 0, thePixmap );    //ooo
        pItem->setIcon ( 0, thePixmap );        //xxx
      }
      else
        //pItem->setPixmap ( 0, errorPixmap );  //ooo
        pItem->setIcon ( 0, errorPixmap );      //xxx
    }
    else
      //pItem->setPixmap ( 0, errorPixmap );    //ooo
      pItem->setIcon ( 0, errorPixmap );        //xxx
  }
  Global::pThumbsCache->saveImageDB ( );

  //connect  ( pGroupView, SIGNAL ( contextMenuRequested ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotGroupContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );  //ooo
  connect  ( pGroupView, SIGNAL ( itemActivated ( QListWidgetItem *item ) ), this, SLOT ( slotGroupContextMenu ( QListWidgetItem *item ) ) );   //xxx

  // And finally we can add the listBox to the ToolBox
  //m_pToolbox->addItem ( pGroupView, uniqueGroupName ( pEntry->qsDisplayName ) );
  m_pToolbox->addItem ( pGroupView, uniqueGroupName ( pSlideshow->slideshow_name ) );
  return pGroupView;
}

QList<CXmlSlideshow *> &DialogImages::getSlideshows ( )		//oxx
{
  return m_listOfSlideshows;
}

QString DialogImages::getDirFilter ( )
{
//  return Global::pApp->getImageFilter ( );
  QString qsImageFilter = Global::pApp->getImageFilter ( );
  QString qsVidFilter   = Global::pApp->getVidFilter   ( );
  return qsImageFilter + " " + qsVidFilter;
}

void DialogImages::slotCheckWorker ( )
{  
  // called from timerEvent in the main thread.
  uint iCount = 0;
  QPixmap    thePixmap;
  ImageScan  *pScan;
  ImageEntry *pEntry;
  QList<ImageScan *> tempList;		//oxx

  // to decrease threading issues, we quickly remove the used Work objects from the member list.
  lock ( );
  QList<ImageScan *>::iterator it = m_listOfImageScansDone.begin ( );		//oxx
  while ( it != m_listOfImageScansDone.end ( ) )  {
    pScan = *it++;
    if ( pScan && pScan->hasExited (   )   )  {
      //m_listOfImageScansDone.remove( pScan );     //ooo
      m_listOfImageScansDone.removeOne( pScan );    //xxx
      tempList.append ( pScan );
    }
  }

//printf ( "  ---slotCheckWorker <%d> and <%d> vs <%d>\n", m_listOfImageScansDone.count ( ), m_listOfImageScansToDo.count ( ), tempList.count ( ) );
  unlock ( );

  it = tempList.begin ( );
  while ( it != tempList.end ( ) )  {
    pScan  = *it++;
    if ( ( ! pScan ) || ( ! pScan->pEntry ) )
      continue;

    if ( m_bExitScannFunction )
      break; // User clicked on a different directory.

    pEntry = pScan->pEntry;
//    if ( pEntry->pItem && ( ! isValid ( pEntry->pItem ) ) )
//      continue;
//    pScan->createPix ( ); // Moved to the main GUI thread to avoid locking X

 
    //Q3IconViewItem *pValid = m_pPreview->firstItem ( );   //ooo
    QListWidgetItem *pValid = m_pPreview->item ( 0 );    //xxx
    while ( pValid ) {
      if  ( pEntry->pItem == pValid ) {    
        pScan->createPix ( ); // Moved to the main GUI thread to avoid locking X    //oooooo    
        setIconText ( pValid );
        //pValid->setPixmap ( pEntry->thePixmap );  //ooo
        pValid->setIcon ( pEntry->thePixmap );   //xxx

        Thumbs *pThumb          = new Thumbs;
        pThumb->pOwnerItem      = pValid;
        pThumb->arrayOfPix      = new QPixmap *[1];
        pThumb->arrayOfPix[0]   = new QPixmap ( thePixmap );
        pThumb->iNumberOfPix    = 1;
        pThumb->iCurrentPix     = 0;
        pThumb->pCache          = (Cache::Thumbs::Entry *)pEntry->pCache;
        pThumb->pSourceFileInfo = NULL;
        m_listOfThumbs.append ( pThumb );

        break;
      }   
      //pValid = pValid->nextItem ( );  //ooo
      pValid = pValid->listWidget ( )->item ( pValid->listWidget ( )->row ( pValid ) + 1 );    //xxx
    }

    delete pScan;
    //m_pProgressBar->setProgress ( m_iProgress ++ );   //ooo
    m_pProgressBar->setValue ( m_iProgress ++ );        //xxx
    if ( iCount ++ > 10 )  {
      //qApp->processEvents ( 100 );				//ooo
      qApp->processEvents ( QEventLoop::AllEvents, 100 );	//xxx
      iCount = 0;
    }
  }
  m_bExitScannFunction = false;

  /*if ( (int)m_iProgress == m_pProgressBar->totalSteps ( )-1 )
    m_pPreview->arrangeItemsInGrid ( TRUE );*/  //oooo
  //qApp->processEvents ( 100 );				//ooo
  qApp->processEvents ( QEventLoop::AllEvents, 100 );		//xxx

  lock ( );
  if ( m_listOfImageScansToDo.count ( ) + m_listOfImageScansDone.count ( ) > 0 )  {
    QTimer::singleShot ( 100, this,  SLOT ( slotCheckWorker ( ) ) );
    // sanity check. Sometimes the worker get eliminated before the whole work s done.
    if ( ( m_listOfWorker.count  ( ) == 0 ) && ( m_listOfImageScansToDo.count ( ) > 0 ) )
           m_listOfWorker.append ( new  Worker ( this ) );
  }
  else 
    //m_pProgressBar->setProgress ( 0 );    //ooo
    m_pProgressBar->setValue ( 0 );         //xxx
  unlock ( );
}

//void DialogImages::newThumbnailRequest ( Q3IconViewItem *pItem )  //ooo
void DialogImages::newThumbnailRequest ( QListWidgetItem *pItem )   //xxx
{ 
  //if ( ! isImageDialog ( pItem->key ( ) ) )   //ooo
  if ( ! isImageDialog ( pItem->text ( ) ) )  //xxx
    return DialogFiles::newThumbnailRequest ( pItem );

  ImageScan *pImageScan = new ImageScan ( this,pItem );
  lock   ( );
    m_listOfImageScansToDo.append       ( pImageScan );
  unlock ( );

  m_bExitScannFunction = false;
  //if ( m_listOfWorker.count  ( ) < m_iMaxThreads )  { //ooo
  if ( static_cast<uint> ( m_listOfWorker.count  ( ) ) < m_iMaxThreads )  {  //xxx   
       m_listOfWorker.append ( new Worker ( this ) );
  
       QTimer::singleShot ( 50, this, SLOT( slotCheckWorker ( ) ) );  //oooo
  }
}

DialogImages::Worker::Worker ( DialogImages *p )
{
  pParent = p;
  bExited = false;
  // And start working ...
  start ( QThread::LowestPriority ); // or maybe QThread::LowPriority 
}

DialogImages::Worker::~Worker ( )
{
  bExited = true;
  //if ( running ( ) )  //ooo
  if ( isRunning ( ) )  //xxx
     terminate ( );
  wait ( 100 );
  pParent = NULL;
}

bool DialogImages::Worker::hasExited ( )
{
  return bExited;
}

void DialogImages::Worker::run ( )
{
  ImageScan *pScan = pParent->takeNextScanObject ( );
  while ( pScan )  {
//    pScan->createPix ( ); // locnig up X if done in background thread
    pScan->hasExited ( true ); // mark for deletion
    if ( ! pParent )
      return;
    pScan = pParent->takeNextScanObject ( pScan );
    QApplication::postEvent ( pParent, new QTimerEvent ( EVENT_UPDATE_ICONS ) );
  }

  bExited = true;
  // Signal to delete this Worker as he finished working ...
  QApplication::postEvent ( pParent, new QTimerEvent ( EVENT_DELETE_WORKER ) );
}

ImageScan *DialogImages::takeNextScanObject ( ImageScan *pOldScan )
{
  ImageScan *pScan = NULL;
  if ( ! m_pIAmAlive )
    return NULL;

  lock   ( );
  if ( m_listOfImageScansToDo.count   ( ) > 0 )  {
       pScan = m_listOfImageScansToDo.first ( );
       //m_listOfImageScansToDo.remove  ( pScan );  //ooo
       m_listOfImageScansToDo.removeOne  ( pScan ); //xxx
  }
  // And finally put the previous scan on the list to be deleted ...
  if ( pOldScan && m_pIAmAlive )
    m_listOfImageScansDone.append ( pOldScan );
  unlock ( );

  return pScan;
}

//ImageScan::ImageScan ( DialogImages *pDlg, Q3IconViewItem *pItm ) //, Cache::Thumbs::ImageEntry *pFromCache ) //ooo
ImageScan::ImageScan ( DialogImages *pDlg, QListWidgetItem *pItm ) //, Cache::Thumbs::ImageEntry *pFromCache )  //xxx
{
  pEntry = new DialogImages::ImageEntry;
  pEntry->pItem      = pItm;
  //pEntry->qsFileName = pItm->key ( ); //ooo
  pEntry->qsFileName = pItm->text ( );    //xxx
  pDialog = pDlg;
  bExited = false;
}

ImageScan::~ImageScan ( )
{
  if ( pEntry )
    delete pEntry;
  pEntry  = NULL;
  pDialog = NULL;
}

bool ImageScan::hasExited ( bool bForce )
{
  if ( bForce )
       bExited = true;
  return bExited;
}

void ImageScan::createPix ( )
{  
  if ( ! pEntry )
    return;

  // Finally we get the dimensions and some more infos.
  if ( ! pEntry->pCache )  {
   
    pEntry->pCache = (Cache::Thumbs::ImageEntry *)Global::pThumbsCache->find ( pEntry->qsFileName, true );
  
    if ( pEntry->pCache ) {   
      bExited = true;
    } else  {
      QImage *pImage = NULL;
      pEntry->pCache = (Cache::Thumbs::ImageEntry *)Global::pThumbsCache->append ( pEntry->qsFileName, pImage );   //ooooo
    }
  }

  if ( pEntry && pEntry->pCache )  {   
    if ( ! pEntry->pCache->arrayOfThumbs )
           pEntry->pCache->loadImages  ( );

    if ( ! pDialog )
      return;

    if ( ! pDialog->lockDialog ( ) )
      return;

    if ( pEntry->pCache->getMatrix ( ) )  {
      //QImage tempImage = pEntry->pCache->arrayOfThumbs[0]->xForm ( *pEntry->pCache->getMatrix ( ) );  //ooo
      QImage tempImage = pEntry->pCache->arrayOfThumbs[0]->transformed ( *pEntry->pCache->getMatrix ( ) );  //xxx
      pDialog->createPix ( &pEntry->thePixmap,  &tempImage,  0.0f,  pEntry->pCache->iStarRating );
    }
    else if ( ( pEntry->pCache->fRotate != 0.0 ) && ( pEntry->pCache->bIsVideo ) )  {
      QMatrix matrix;
      matrix.rotate     ( pEntry->pCache->fRotate );
      //QImage tempImage  = pEntry->pCache->arrayOfThumbs[0]->xForm (  matrix );    //ooo
      QImage tempImage  = pEntry->pCache->arrayOfThumbs[0]->transformed (  matrix );    //xxx
      pDialog->createPix(&pEntry->thePixmap,   &tempImage,  0.0f,  pEntry->pCache->iStarRating );
    }
    else
      pDialog->createPix ( &pEntry->thePixmap, pEntry->pCache->arrayOfThumbs[0], 0.0f, pEntry->pCache->iStarRating );

    pDialog->unlockDialog ( );
  }
}

DialogImages::ImageEntry::ImageEntry ( )
{
  created     = QDateTime::currentDateTime ( );
  qsDimension = "width x height";
  iDurationMs = 0;
  pItem       = NULL;
  pCache      = NULL;
}

DialogImages::ImageEntry::~ImageEntry ( )
{
}

//void DialogImages::setIconText ( Q3IconViewItem *pItem )  //ooo
void DialogImages::setIconText ( QListWidgetItem *pItem )   //xxx
{
  if ( ! pItem )
    return;

  if ( m_bTooSmallForText )  {
    //pItem->setText ( QString::null );     //ooo
    pItem->setText ( QString::null );    //xxx
    return;
  }

  QString  qsText, qsFileName;
  //qsFileName = pItem->key ( );    //ooo
  qsFileName = pItem->text ( );     //xxx

  Cache::Thumbs::ImageEntry *pEntry;
  pEntry = (Cache::Thumbs::ImageEntry *)Global::pThumbsCache->find ( qsFileName, true );
  if ( ! pEntry )
    return;

  QFileInfo fileInfo ( qsFileName );
  QString qsDimension = pEntry->qsDimension;
  if ( ! pEntry->arrayOfThumbs )
    qsDimension = "Loading Preview"; // maybe better to use "width x height"; ???

  if ( m_bName )
    qsText = fileInfo.fileName ( );

  if ( m_bDate )
    qsText += ( m_bName ? tr ( "\nD=" ) : tr ( "D=" ) ) + pEntry->dateCreated.toString ( "yy/MM/dd" );
  if ( m_bStars )
    qsText += ( ( m_bName || m_bDate ) ? tr ( "\nT=" ) : tr ( "T=" ) ) + pEntry->dateCreated.toString ( "hh:mm:ss" );
  if ( m_bLength )
    qsText += ( m_bName || m_bDate || m_bStars ) ? tr( "\nG=%1" ).arg ( qsDimension ) : tr ( "G=%1" ).arg ( qsDimension );

  //pItem->setText ( qsText );  //ooo
  pItem->setText ( qsText ); //xxx
}

void DialogImages::setIconText ( DialogFiles::Thumbs *pThumbs )
{  
  if ( ! pThumbs )
    return;

  if ( ! isValid ( pThumbs->pOwnerItem ) )
    return;

  if ( m_bTooSmallForText )  {
    //pThumbs->pOwnerItem->setText ( QString::null );   //ooo
    pThumbs->pOwnerItem->setText ( QString::null );  //xxx
    return;
  }

  QString   qsText, qsFileName;
  //qsFileName = pThumbs->pOwnerItem->key ( );  //ooo
  qsFileName = pThumbs->pOwnerItem->text ( ); //xxx
  if ( ! isImageDialog ( qsFileName ) )
    return DialogFiles::setIconText ( pThumbs );

  QFileInfo fileInfo ( qsFileName );
  Cache::Thumbs::ImageEntry *pImageEntry = (Cache::Thumbs::ImageEntry *)pThumbs->pCache;

  if ( m_bName )
    qsText = fileInfo.fileName ( );

  if ( m_bDate )
    qsText += ( m_bName ? tr ( "\nD=" ) : tr ( "D=" ) ) + pImageEntry->dateCreated.toString ( "yy/MM/dd" );
  if ( m_bStars )
    qsText += ( ( m_bName || m_bDate ) ? tr ( "\nT=" ) : tr ( "T=" ) ) + pImageEntry->dateCreated.toString ( "hh:mm:ss" );

  if ( m_bLength )
    qsText += ( ( m_bName || m_bDate || m_bStars ) ? tr ( "\nG=%1" ).arg ( pImageEntry->qsDimension ) : tr ( "G=%1" ).arg ( pImageEntry->qsDimension ) );

  //pThumbs->pOwnerItem->setText ( qsText );    //ooo
  pThumbs->pOwnerItem->setText ( qsText );   //xxx
}

void DialogImages::timerEvent ( QTimerEvent *pEvent )
{  
  // Called from the worker threads after a thumbnail has been generated.
  if ( pEvent->timerId ( ) == EVENT_UPDATE_ICONS ) {
       slotCheckWorker ( );
  } else if ( pEvent->timerId ( ) == EVENT_DELETE_WORKER )  {
    Worker *pWorker = NULL;
    QList<Worker *> tempList;						//oxx
    QList<Worker *>::iterator it = m_listOfWorker.begin ( );		//oxx
    lock  ( );
    while ( it != m_listOfWorker.end ( ) && m_pIAmAlive )  {
      pWorker = *it++;
      if  ( pWorker->hasExited ( ) )  {
        tempList.append ( pWorker );
        //m_listOfWorker.remove ( pWorker );    //ooo
        m_listOfWorker.removeOne ( pWorker );   //xxx
      }
    }
    unlock ( );

    // delete the threads
    it = tempList.begin ( );
    while ( it != tempList.end ( ) )  {
      pWorker = *it++;
      if ( pWorker->wait ( 10 ) ) 
        delete pWorker;
    }
  }
  else
    DialogFiles::timerEvent ( pEvent );
}

//void DialogImages::slotPreviewClicked ( int iButton, QIconViewItem *pItem, const QPoint & )
//void DialogImages::slotPreviewClicked ( int, Q3IconViewItem *, const QPoint & )   //ooo
void DialogImages::slotPreviewClicked ( int, QTreeWidgetItem *, const QPoint & )    //xxx
{
/*
  if ( ! pItem )
    return; // User clicked on empty space in QIconView ( m_pPreview )

  if ( iButton == Qt::RightButton )
    return;

  pItem->setSelected ( false );
  if ( m_pTimerThumbing->isActive ( ) )
       stopThumbing  ( );
  else {
    uint t;
    Thumbs *pThumb  = NULL;
    for ( t=0; t<m_listOfThumbs.count ( ); t++ ) {
      pThumb  =  m_listOfThumbs[t];
      if ( pThumb->pOwnerItem == pItem ) {
	// If we click the second time on a item we should stop the animation
	if ( m_pActiveThumbs == pThumb )
	  break;
	
	m_pActiveThumbs = pThumb;
	m_pTimerThumbing->start ( 1500 ); // Set timer to a nicely paced 1.5 seconds.
	return;
      }
    }
  }
*/
  m_pActiveThumbs = NULL;
}

void DialogImages::showBasicMenu ( const QPoint &pos )
{
  int t, iID, iIDs[7];
  //Q3PopupMenu *pMenu   = new Q3PopupMenu ( this );    //ooo
  QMenu *pMenu   = new QMenu ( this );                  //xxx
  //pMenu->setCheckable ( TRUE );   //ooo

  //maybe submenu ...
  //iIDs[0] = pMenu->insertItem ( tr ( "All to &Virtual Folder ..." ), 1 );         //ooo
  QAction *allToVirtualFolder_Act = new QAction ( tr ( "All to &Virtual Folder ..." ), this ); //xxx
  //iIDs[1] = pMenu->insertItem ( tr ( "All to &Source Group ..." ),   2 );                    //ooo
  QAction *allToSourceGroup_Act = new QAction ( tr ( "All to &Source Group ..." ), this ); //xxx
  //pMenu->insertSeparator ( ); //ooo
  pMenu->addSeparator ( );      //xxx
  //iIDs[2] = pMenu->insertItem ( tr ( "&Auto Group" ),                3 ); //ooo
  QAction *autoGroup_Act = new QAction ( tr ( "&Auto Group" ), this ); //xxx
  //iIDs[3] = pMenu->insertItem ( tr ( "&Refresh" ),                   4 ); //ooo
  QAction *refresh_Act = new QAction ( tr ( "&Refresh" ), this ); //xxx
  //iIDs[4] = pMenu->insertItem ( tr ( "&Clear" ),                     5 ); //ooo
  QAction *clear_Act = new QAction ( tr ( "&Clear" ), this ); //xxx
  
  pMenu->addAction ( allToVirtualFolder_Act );         //xxx
  pMenu->addAction ( allToSourceGroup_Act );         //xxx
  pMenu->addAction ( autoGroup_Act );         //xxx
  pMenu->addAction ( refresh_Act );         //xxx
  pMenu->addAction ( clear_Act );         //xxx

  /*if ( m_pPreview->count ( ) < 1 ) {
    for ( t=0; t<5; t++ )
      pMenu->setItemEnabled ( iIDs[t], false );      
  }*/   //oooo

  //iID = pMenu->exec ( pos );  //ooo
  pMenu->exec ( pos );          //xxx

  /*if ( iID < 1 ) { // Nothing selected ...
    delete pMenu;
    return;
  }

  stopThumbing ( );
  if ( iID ==  1 ) // All to Virtual Folder
    toVirtualFolder( 2 );
  else if ( iID  ==  2 )  // All to Source Group
    toSourceGroup  ( 2 );
  else if ( iID  ==  3 )  // Auto Group
    slotAutoGroup  (   );
  else if ( iID  ==  4 )  // Refresh
    slotRefresh    (   );
  else if ( iID  ==  5 )  // Clear
    clearPreview   (   );*/ //oooo

  delete pMenu;
}

//void DialogImages::showContextMenu ( const QPoint &pos, Q3IconViewItem * )    //ooo
void DialogImages::showContextMenu ( const QPoint &pos, QTreeWidgetItem * )     //xxx
{
  int t, iID, iIDs[10], iStarRating = -2;
  bool bVidsSelected = false;
  Thumbs *pThumbs    = NULL;
  QPoint globalPos   = pos, globalPos2 = pos;

  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );  //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );   //xxx
  //QList<Q3IconViewItem *>listIcons;			//oxx
  QList<QListWidgetItem *>listIcons;			//xxx
  //QList<Q3IconViewItem *>::iterator it2;		//oxx
  QList<QTreeWidgetItem *>::iterator it2;		//xxx
  QList<Thumbs *>listSelected;				//oxx
  QList<Thumbs *>::iterator it;				//oxx

  while ( pIcon ) {
    if  ( pIcon->isSelected ( )  )
      listIcons.append   ( pIcon );
    //pIcon = pIcon->nextItem ( );  //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 );   //xxx
  }
  if ( listIcons.count ( ) < 1 )
    return;
  if ( m_pContextMenu )
    delete m_pContextMenu;
  //m_pContextMenu = new Q3PopupMenu ( this );  //ooo
  m_pContextMenu = new QMenu ( this );          //xxx
  //m_pContextMenu->setCheckable    ( TRUE );   //ooo

  // Here we see which StarRating should be checked.
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      listSelected.append ( pThumbs );
      if ( iStarRating == -2 )
           iStarRating  = (int)pThumbs->pCache->iStarRating;
      if ( iStarRating != (int)pThumbs->pCache->iStarRating )
           iStarRating  = -1;
      if (!isImageDialog ( pThumbs->pCache->qsFileName ) )
           bVidsSelected= true;
    }
  }

  // We first create the menu on the right, non blocking.
  //Q3PopupMenu *pStackMenu = new Q3PopupMenu (  this  );   //ooo
  QMenu *pStackMenu = new QMenu (  this  );                 //xxx
  //pStackMenu->insertItem ( tr ( "R&emove" ), this, SLOT ( slotRemove ( ) ) );   //ooo
  QAction *remove_Act = new QAction ( tr ( "R&emove" ), this );     //xxx
  //iID     = pStackMenu->insertItem ( tr ( "&View ..." ),    this, SLOT ( slotView       ( ) ) );  //ooo
  QAction *view_Act = new QAction ( tr ( "&View ..." ), this );     //xxx
  //pStackMenu->insertSeparator ( );    //ooo
  //iIDs[1] = pStackMenu->insertItem ( tr ( "Rotate &90"  ),  this, SLOT ( slotRotate90   ( ) ) );  //ooo
  QAction *rotate90_Act = new QAction ( tr ( "Rotate &90" ), this );     //xxx
  //iIDs[2] = pStackMenu->insertItem ( tr ( "Rotate &180" ),  this, SLOT ( slotRotate180  ( ) ) );  //ooo
  QAction *rotate180_Act = new QAction ( tr ( "Rotate &180" ), this );     //xxx
  //iIDs[3] = pStackMenu->insertItem ( tr ( "Rotate &270" ),  this, SLOT ( slotRotate270  ( ) ) );  //ooo
  QAction *rotate270_Act = new QAction ( tr ( "Rotate &270" ), this );     //xxx
  //iIDs[4] = pStackMenu->insertItem ( tr ( "Rotate &Free" ), this, SLOT ( slotRotateFree ( ) ) );  //ooo
  QAction *rotateFree_Act = new QAction ( tr ( "Rotate &Free" ), this );     //xxx
  
  pStackMenu-> addAction ( remove_Act );    //xxx
  pStackMenu-> addAction ( view_Act );    //xxx
  pStackMenu-> addAction ( view_Act );    //xxx
  pStackMenu->addSeparator ( );         //xxx
  pStackMenu-> addAction ( rotate90_Act );    //xxx
  pStackMenu-> addAction ( rotate180_Act );    //xxx
  pStackMenu-> addAction ( rotate270_Act );    //xxx
  pStackMenu-> addAction ( rotateFree_Act );    //xxx
  /*if (  listIcons.count  ( ) > 1 )
    pStackMenu->setItemEnabled ( iID, false );*/    //oooo

//  pStackMenu->setItemEnabled ( iIDs[1], false );
//  pStackMenu->setItemEnabled ( iIDs[2], false );
//  pStackMenu->setItemEnabled ( iIDs[3], false );
  /*if ( bVidsSelected )
    pStackMenu->setItemEnabled ( iIDs[4], false );*/    //oooo

  //globalPos.setY    ( globalPos.y ( ) - 25 );
  /*globalPos.setX    ( globalPos.x ( ) - pStackMenu->sizeHint ( ).width ( ) - 4 );
  pStackMenu->popup ( globalPos, 3 );*/ //oooo

  // Next we will create the right menu ( To source, To VFolder, and StarRating

  //iIDs[8] = m_pContextMenu->insertItem ( tr ( "to &Source Group ..."   ), 8 );    //ooo
  QAction *toSourceGroup_Act = new QAction ( tr ( "to &Source Group ..." ), this );     //xxx
  //iIDs[9] = m_pContextMenu->insertItem ( tr ( "to &Virtual Folder ..." ), 9 );    //ooo
  QAction *toVirtualFolder_Act = new QAction ( tr ( "to &Virtual Folder ..." ), this );     //xxx
//  if ( listIcons.count ( ) == listSelected.count ( ) ) {
    //m_pContextMenu->insertSeparator ( );  //ooo
    //iIDs[7] = m_pContextMenu->insertItem ( tr ( "No Stars" ),                1 );     //ooo
    QAction *noStars_Act = new QAction ( tr ( "No Stars" ), this );     //xxx
    /*for ( t=0; t<6; t++ )
      iIDs[t] = m_pContextMenu->insertItem( m_pixMenuStars[t], t+2 );*/ //oooo
//  }

   m_pContextMenu-> addAction ( toSourceGroup_Act );    //xxx
   m_pContextMenu-> addAction ( toVirtualFolder_Act );    //xxx
   m_pContextMenu->addSeparator ( );       //xxx
   m_pContextMenu-> addAction ( noStars_Act );    //xxx

  /*if ( iStarRating > -1 )
    m_pContextMenu->setItemChecked ( iStarRating + 1, TRUE );*/ //oooo

  // last we check if we do have VirtualFolders ...
  /*if ( m_pListViewVirtual->childCount ( ) == 0 )
       m_pContextMenu->setItemEnabled ( iID,  FALSE );

  iID = m_pContextMenu->exec ( globalPos2, 3 );
  if ( iID > -1 )  {
    // A point from the ContextMenu was chosen
    if ( ( iID > 0 ) && ( iID < 8 ) ) {  // StarRating
      if ( ! lockDialog ( ) )
        return;

      it = listSelected.begin ( );
      while ( it != listSelected.end ( ) ) {
        pThumbs = *it++;
        pThumbs->pCache->iStarRating = iID - 1;
        recreateThumbs  ( pThumbs );
      }
      if ( bVidsSelected )
        Global::pThumbsCache->saveCacheDB ( );
      else
        Global::pThumbsCache->saveImageDB ( );
      unlockDialog ( );
    }
    else if  (  iID ==  8 )
      toSourceGroup   ( 3 );
    else if  (  iID ==  9 )
      toVirtualFolder ( 3 );
  }*/   //oooo
  delete pStackMenu;
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;
}

//void DialogImages::slotPreview ( Q3IconViewItem * )   //ooo
void DialogImages::slotPreview ( QListWidgetItem * )    //xxx
{
  // Mouse double click
  slotView ( );
}

void DialogImages::slotPlay ( )
{
  slotView ( );
}

void DialogImages::slotView ( )
{
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;

  Thumbs *pThumbs;
  lock ( );
  QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end  ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      if ( m_pActiveThumbs == pThumbs )
           m_pTimerThumbing->stop   ( );

      m_pPreviewer->setItem ( pThumbs->pCache, pThumbs->pOwnerItem );

      break; // only one selected item possible. So let us leave the while loop.
    }
  }
  unlock ( );
}

void DialogImages::slotRemove ( )
{
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;

  Thumbs *pThumbs;
  lock ( );
  QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      //m_listOfThumbs.remove  ( pThumbs );     //ooo
      m_listOfThumbs.removeOne  ( pThumbs );    //xxx
      if ( m_pActiveThumbs == pThumbs )
           m_pTimerThumbing->stop   ( );
      delete pThumbs->pOwnerItem;
      delete pThumbs;
    }
  }
  unlock ( );
}

void DialogImages::slotRotate90 ( )
{
  rotate ( 90.0 );
}

void DialogImages::slotRotate180 ( )
{
  rotate ( 180.0 );
}

void DialogImages::slotRotate270 ( )
{
  rotate ( 270.0 );
}

void DialogImages::slotRotateFree ( )
{
  bool   bOkay;
  double fRotate = -1.0;
  QString qsHeader = tr ( "Please enter rotation angle." );
  QString qsLabel  = tr ( "Angle (deg): " );

  // Next we try to see if all of the selected items have the same rotational angle
  Thumbs *pThumbs;
  QList<Thumbs *>listTemp;			//oxx
  QList<Thumbs *>::iterator it;			//oxx
  Cache::Thumbs::ImageEntry *pCache;

  lock ( );
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) )  {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) )
         listTemp.append ( pThumbs );
  }
  unlock ( );

  it = listTemp.begin ( );
  while ( it != listTemp.end ( ) ) {
    pThumbs = *it++;
    pCache  = (Cache::Thumbs::ImageEntry *)pThumbs->pCache;
    // And finally if we modified the thumb we should save it.
    if ( pCache )  {
      if ( fRotate == -1.0 )
           fRotate = pCache->fRotate;
      else if ( fRotate != pCache->fRotate )  {
        fRotate = 0.0; // no match between items
        break;
      }
    }
  }

  //fRotate = QInputDialog::getDouble ( qsHeader, qsLabel, fRotate, -360.0, 360.0, 2, &bOkay ); //ooo
  fRotate = QInputDialog::getDouble ( this, qsHeader, qsLabel, fRotate, -360.0, 360.0, 2, &bOkay ); //xxx
  if ( bOkay )
    rotate ( fRotate );
}

void DialogImages::rotate ( double fRotate )
{
  // First we can get rid of the context menu 
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;

  // rotate thumb[0] and Pix[0]
  bool bSaveCache = false;
  bool bIsImage   = false;
  Thumbs *pThumbs;
  QList<Thumbs *>listTemp;			//oxx
  QList<Thumbs *>::iterator it;			//oxx
  Cache::Thumbs::Entry *pCache = NULL;

  // Going through the list of images and rotate them according to the iOrientation value
  lock ( );
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) )  {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) )
         listTemp.append ( pThumbs );
  }
  unlock ( );

  if ( ! lockDialog ( ) )
    return;

  it = listTemp.begin ( );
  while ( it != listTemp.end ( ) ) {
    pThumbs = *it++;
//    pCache  = (Cache::Thumbs::ImageEntry *)pThumbs->pCache;
    pCache  = (Cache::Thumbs::Entry *)pThumbs->pCache;
    bIsImage = isImageDialog ( pCache->qsFileName );

    // And finally if we modified the thumb we should save it.
    if ( pCache->arrayOfThumbs )  {
         pCache->bStoreData = true;
         pCache->fRotate   += (float)fRotate;
         while ( pCache->fRotate >=  360.0 )
                 pCache->fRotate -=  360.0;
         while ( pCache->fRotate <= -360.0 )
                 pCache->fRotate +=  360.0;
         if ( bIsImage )
            ((Cache::Thumbs::ImageEntry *)pCache)->initMatrix ( );
         bSaveCache = true;
    }
    recreateThumbs ( pThumbs );
  }
  if ( bSaveCache )
       Global::pThumbsCache->saveImageDB ( );

  unlockDialog ( );
}

void DialogImages::clearSlideshows ( )
{
  lock ( );
  QList<CXmlSlideshow *>::iterator it = m_listOfSlideshows.begin ( );		//oxx
  while ( it != m_listOfSlideshows.end ( ) )
    delete *it++;
  m_listOfSlideshows.clear ( );
  unlock ( );
}

void DialogImages::clearWorker ( )
{
  lock   ( );
  QList<Worker *>::iterator it = m_listOfWorker.begin ( );	//oxx
  while ( it != m_listOfWorker.end ( ) )
    delete *it++;
  m_listOfWorker.clear ( );

  QList<ImageScan *>::iterator it2 = m_listOfImageScansToDo.begin ( );		//oxx
  while ( it2 != m_listOfImageScansToDo.end ( ) )
    delete *it2++;
  m_listOfImageScansToDo.clear ( );
  unlock ( );
}

//void DialogImages::rescueImageAttributes ( CXmlSlideshow::img_struct *pXmlImg, CXmlSlideshow *pTemp )
bool DialogImages::rescueImageAttributes (  void *pVoidImg, CXmlSlideshow *pSlideshow )
{
  CXmlSlideshow::img_struct *p, *pXmlImg = (CXmlSlideshow::img_struct *)pVoidImg;
  uint t, iCount;
  iCount = pSlideshow->countImg( );
  for ( t=0; t<iCount; t++ )   {
    p = pSlideshow->getImg ( t );
    if ( p && pXmlImg->src == p->src )  {
       pXmlImg->fDuration  =  p->fDuration;
       pXmlImg->fStartTime =  p->fStartTime;
       pXmlImg->fEndTime   =  p->fEndTime;
       pXmlImg->width      =  p->width;
       pXmlImg->height     =  p->height;
//     pXmlImg->rotate     =  p->rotate;
       pXmlImg->effect[0]  =  p->effect[0];
       pXmlImg->effect[1]  =  p->effect[1];
       return true;
    }
  }
  return false;
}

void DialogImages::accept ( )
{
  // Here we build the SourceFileEntries from the information in m_pToolbox
  int t;
  GroupView       *pGroupView = NULL;
  GroupView::Item *pGroupItem = NULL;
  CXmlSlideshow   *pSlideshow = NULL;

  if ( ! m_bCanClose ) {
    if ( MessageBox::warning ( this, tr ( "Can't close." ), tr ( "Can not close dialog while waiting for MediaScanner to finish.\nDo you want to force Quit ?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
      return;
  }

  clearSlideshows ( );

  // Check if the default group has not been filled with anything but the Preview holds images, 
  // then we should move them into the group. The user probably assumed that the Preview is a Slideshow group.
  if ( m_pToolbox->count ( ) == 1 )  {
    //pGroupView = (GroupView *)m_pToolbox->item ( 0 ); //ooo
    pGroupView = (GroupView *)m_pToolbox->widget ( 0 ); //xxx
    //if ( pGroupView && pGroupView->childCount ( ) == 0 )  {   //oooo
    //if ( pGroupView && pGroupView->topLevelItem ( 0 )->childCount ( ) == 0 )  {   //xxx
    if ( pGroupView && pGroupView->topLevelItemCount ( ) == 0 )  {  //xxx
      int iRes = MessageBox::question ( this, tr ( "No Files in Group." ), 
            //tr ( "There were no files added to the group \"%1\".\nDo you want to add all files in the preview to this group ?" ).arg ( m_pToolbox->itemLabel ( 0 ) ), //ooo
            tr ( "There were no files added to the group \"%1\".\nDo you want to add all files in the preview to this group ?" ).arg ( m_pToolbox->itemText ( 0 ) ),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel );
      if ( iRes == QMessageBox::Yes )  {
        QFileInfo fileInfo;
        Thumbs *pThumbs;
	QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
        while ( it != m_listOfThumbs.end ( ) ) {
          pThumbs = *it++;
          fileInfo.setFile ( pThumbs->pCache->qsFileName );
          GroupView::Item *pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), true );
          pItem->pCache  =  pThumbs->pCache;
        }
      }
      else if ( iRes == QMessageBox::Cancel )
        return;
    }
  }

  for ( t=0; t<m_pToolbox->count ( ); t++ ) {
    pSlideshow = NULL;
    //pGroupView = (GroupView *)m_pToolbox->item ( t ); //ooo
    pGroupView = (GroupView *)m_pToolbox->widget ( t ); //xxx
    if ( pGroupView ) {
      pSlideshow = pGroupView->slideshow ( );   
      //pGroupItem = (GroupView::Item *)pGroupView->firstChild ( ); //oooo
      pGroupItem = (GroupView::Item *)pGroupView->topLevelItem ( 0 );      //xxx
      //pGroupItem = (GroupView::Item *)pGroupView;      //xxx   
      if ( pGroupItem ) {       
        if ( ! pSlideshow )  {         
          pSlideshow = new CXmlSlideshow;
          Global::pSlideDefaults->initSlideshow ( pSlideshow );
        }
     
        //pSlideshow->slideshow_name = m_pToolbox->itemLabel ( t ); //ooo
        pSlideshow->slideshow_name = m_pToolbox->itemText ( t );    //xxx
        // We need to take the slideshow object away from the GroupView object
        pGroupView->setSlideshow ( NULL );
      }
   
      // We need to clear the objects and re-add them based on what is in the GroupView.
      // For this we create a temp copy to preserve the other image attributes
      if ( pSlideshow )  {
        populateFromGroup ( pSlideshow, pGroupView );
        m_listOfSlideshows.append ( pSlideshow );
      }
    }
  }
  Global::pThumbsCache->saveImageDB ( );
  Global::pThumbsCache->saveCacheDB ( );
  QDialog::accept ( );
}

void DialogImages::populateFromGroup ( CXmlSlideshow *pSlideshow, GroupView *pGroupView )
{ 
  if ( ! pSlideshow || ! pGroupView )
    return;

  Utils theUtils;
  Cache::Thumbs::Entry      *pVidCache = NULL;
  Cache::Thumbs::ImageEntry *pImgCache = NULL;
  CXmlSlideshow::img_struct *pXmlImg   = NULL;
  CXmlSlideshow::vid_struct *pXmlVid   = NULL;
  CXmlSlideshow tempSlideshow;
  bool bRescued = false;

  //GroupView::Item *pGroupItem  = (GroupView::Item *)pGroupView->firstChild ( );   //oooo
  GroupView::Item *pGroupItem  = (GroupView::Item *)pGroupView->topLevelItem ( 0 ); //xxx
  tempSlideshow = *pSlideshow;
  pSlideshow->clearObjects ( );
  pSlideshow->audio_list       = tempSlideshow.audio_list;
  pSlideshow->validFilterNames = tempSlideshow.validFilterNames;
  while ( pGroupItem &&   pGroupItem->pCache )  {  
    if  ( isImageDialog ( pGroupItem->pCache->qsFileName ) )  {
      pXmlImg       = pSlideshow->addImg ( );
      pXmlImg->src  = pGroupItem->pCache->qsFileName;
      pXmlImg->text = pGroupItem->text ( 0 );
      bRescued  = rescueImageAttributes ( pXmlImg, &tempSlideshow );
      pImgCache = (Cache::Thumbs::ImageEntry *)Global::pThumbsCache->find ( pXmlImg->src, true );
      if ( pImgCache )  {
        if ( pImgCache->getMatrix ( ) )  {
          pXmlImg->rotate  = pImgCache->fRotate;
          pXmlImg->pMatrix = new QMatrix ( *pImgCache->getMatrix ( ) );
        }
        else if ( ( pImgCache->fRotate != 0.0 ) && ( pImgCache->bIsVideo ) )  {
          QMatrix matrix;
          matrix.rotate    ( pImgCache->fRotate );
          pXmlImg->rotate  = pImgCache->fRotate;
          pXmlImg->pMatrix = new QMatrix ( matrix );
        }
        theUtils.getWidthHeight ( pImgCache->qsDimension, pXmlImg->width, pXmlImg->height );
      }
      if ( ! bRescued && pSlideshow->kenBurns ( ) )  {
        // Did not find the image in the original Slideshow So we can add KenBurns effects
        pXmlImg->effect[0].randomKenBurns ( 0.5, pXmlImg->width, pXmlImg->height, false );
      }
    }
    else  {
      pXmlVid         = pSlideshow->addVid ( );
      pXmlVid->src    = pGroupItem->pCache->qsFileName;
      pXmlVid->rotate = pGroupItem->pCache->fRotate;
      pXmlVid->text   = pGroupItem->text ( 0 );
      pVidCache       = Global::pThumbsCache->find ( pXmlVid->src, false );
      if ( pVidCache )  {
           pXmlVid->length = theUtils.getMsFromString ( pVidCache->qsLength );
           pXmlVid->fDuration = (float)pXmlVid->length / 1000.0f;
      }
      rescueImageAttributes ( pXmlVid, &tempSlideshow );
    }
    // pXmlImg has the following vars ...
    // float fDuration; float fStartTime; float fEndTime; QString text;
    // int width; int height; float rotate; effect_struct effect[2]; // start / end
 
    //pGroupItem = (GroupView::Item *)pGroupItem->nextSibling ( );  //ooo
    //pGroupItem = (GroupView::Item *)pGroupItem->parent()->child(pGroupItem->parent()->indexOfChild(pGroupItem)+1);	//xxx
    pGroupItem = (GroupView::Item *)pGroupItem->treeWidget()->itemBelow(pGroupItem);	//xxx
  }
}

}; // end namespace Input

