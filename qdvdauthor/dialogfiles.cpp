/***************************************************************************
    dialogfiles.cpp
                             -------------------
    DialogFiles
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <stdlib.h> // 0 system ( ... )
#include <unistd.h>

#include <qinputdialog.h>
#include <qapplication.h>
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qpushbutton.h>
//#include <qmessagebox.h>      //ooo
#include <QMessageBox>          //xxx
//#include <q3scrollview.h>     //ooo
#include <QScrollArea>          //xxx
#include <qtabwidget.h>
//#include <q3popupmenu.h>      //ooo
#include <QMenu>                //xxx
#include <qcheckbox.h>
#include <qcombobox.h>
//#include <q3iconview.h>       //ooo
#include <QGraphicsView>        //xxx
#include <qfileinfo.h>
#include <qlineedit.h>
//#include <q3groupbox.h>       //ooo
#include <QGroupBox>            //xxx
#include <qpainter.h>
#include <qtoolbox.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qslider.h>
//#include <q3header.h>         //ooo
#include <qregexp.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qdir.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QPixmap>
//#include <Q3GridLayout>       //ooo
#include <QGridLayout>          //xxx
//#include <Q3ValueList>	//oxx
//#include <Q3TextStream>       //ooo
#include <QTextStream>          //xxx
//#include <q3mimefactory.h>    //ooo
#include <QDragEnterEvent>
#include <QDropEvent>

#include "global.h"
#include "previewer.h"        //ooo
#include "qdvdauthor.h"
#include "messagebox.h"
#include "dialogfiles.h"
#include "dialogdelta.h"
#include "sourcetoolbar.h"
#include "dialogvfolder.h"
#include "filepreviewdialog.h"
#include "qplayer/mediacreator.h"
#include "qplayer/listviewfileitem.h"
#include "xml_slideshow.h"
#include "importsrt.h"
#include "importssa.h"

#include "dialogimages.h"     //ooo

#define COLOR_QUICK_SELECTED 250, 150, 50
#define COLOR_QUICK_NORMAL   170, 170, 255

#ifndef DEBUG_INFO
#define debug_cout printf
#else
void debug_cout(const char *, ...){};
#endif


namespace Input
{

const int GroupView::m_iSize       = 45;
int  DialogFiles::m_iThumbnailSize = 100;
bool DialogFiles::m_bStars         = true;
bool DialogFiles::m_bName          = true;
bool DialogFiles::m_bDate          = true;
bool DialogFiles::m_bLength        = true;
DialogFiles *DialogFiles::m_pIAmAlive = NULL; // There can only be one.
QMap <unsigned long long, DialogFiles::Manual *> DialogFiles::m_mapOfManualChange;


// Here we define the pixmaps for the directory tree
// as taken from the DirList example.
// These are static char fields which is more an ancient c-style
// but it'll save us the hassle of adding it to the project file
// plus keeping the pixs around.
static const char *xpmFolderClosed[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char *xpmFolderOpen[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

static const char *xpmFolderLocked[]={
    "16 16 10 1",
    "h c #808080",
    "b c #ffa858",
    "f c #c0c0c0",
    "e c #c05800",
    "# c #000000",
    "c c #ffdca8",
    ". c None",
    "a c #585858",
    "g c #a0a0a4",
    "d c #ffffff",
    "..#a#...........",
    ".#abc####.......",
    ".#daa#eee#......",
    ".#ddf#e##b#.....",
    ".#dfd#e#bcb##...",
    ".#fdccc#daaab#..",
    ".#dfbbbccgfg#ba.",
    ".#ffb#ebbfgg#ba.",
    ".#ffbbe#bggg#ba.",
    ".#fffbbebggg#ba.",
    ".##hf#ebbggg#ba.",
    "...###e#gggg#ba.",
    ".....#e#gggg#ba.",
    "......###ggg#b##",
    ".........##g#b##",
    "...........####."};

/*//static void setKey ( Q3IconViewItem *pItem, QString &qsKey )  //ooo
static void setKey ( QListWidgetItem *pItem, QString &qsKey )   //xxx
{
  if ( ! pItem )
    return;
  pItem->setKey ( qsKey );
}*/ //ooo

/*static QString getKey ( Q3IconViewItem *pItem )
{
  QString qsEmpty;
  if ( ! pItem )
    return qsEmpty;
  return pItem->key ( );
}*/ //ooo

static void filterNumbers ( QString &qsString )
{
  QRegExp rx( "(\\d+)" );
  QString str;
  int pos = 0;
  while ( pos >= 0 ) {
    //pos = rx.search ( qsString, pos );    //ooo
    pos = rx.indexIn ( qsString, pos );     //xxx
    if  ( pos > -1 )  {
      str += rx.cap ( 1 );
      pos += rx.matchedLength ( );
    }
  }
  qsString = str;
}

static bool isImage ( QString &qsFileName )
{
  QFileInfo fileInfo ( qsFileName );
  QString qsVids = Global::pApp->getImageFilter ( );
  //if ( qsVids.find ( fileInfo.extension ( FALSE ) ) > -1 )    //ooo
  if ( qsVids.indexOf ( fileInfo.suffix ( ) ) > -1 )            //xxx
    return true;
  return false;
}

  // from 2005 in Deutsch : http://www.pro-linux.de/t_multimedia/dvd-video-disk.html
  //

  // After playing with the dialog:
  // - Reset ProgressBar if/when reaching 100%
  // - Feature to show total lenth of selected videos.
  // - Feature to enlarge one of the thumbnails ( so the icons are mini but you can see focus on one Item )
  //   - Maybe similar to fish eye ? That would be cool
  // - Feature : remove all groups

  // - For larger projects it would be usefull to split it into sub projects to increase load time.
  //   - Work on one menu at a time.

  // - Implement smart scanning. After three false images, cancel scanning
  //   - Mark those who can not be scanned ( Film Frame left / Right )

//GroupView::Item::Item ( Q3ListView *pView, QString qsLabel, bool bAlt )   //ooo
GroupView::Item::Item ( QTreeWidget *pView, QString qsLabel, bool bAlt )
  //: Q3ListViewItem ( pView, qsLabel )     //ooo
  //: QTreeWidgetItem ( pView, QStringList ( qsLabel ) )      //xxx
  : QTreeWidgetItem ( pView )      //xxx
{  
  pCache          = NULL;
  bAlternateColor = bAlt;
  pSourceFileInfo = NULL;
  setText ( 0, qsLabel);    //xxx

  //setDragEnabled ( true );    //ooo
}

//GroupView::Item::Item ( Q3ListView *pView, Item *pAfter, QString qsLabel, bool bAlt ) //ooo
GroupView::Item::Item ( QTreeWidget *pView, Item *pAfter, QString qsLabel, bool bAlt )  //xxx
  //: Q3ListViewItem ( pView, pAfter, qsLabel ) //ooo
  : QTreeWidgetItem ( pView, pAfter )           //xxx
{
  pCache          = NULL;
  bAlternateColor = bAlt;
  pSourceFileInfo = NULL;
  setText ( 0, qsLabel);    //xxx

  //setDragEnabled ( true );    //ooo
}

GroupView::Item::~Item ( )
{
}

/*void GroupView::Item::paintCell ( QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
  QColorGroup theColors = cg;
  if ( isSelected ( ) )   {
    theColors.setColor(QColorGroup::Base, theColors.highlight() );
    theColors.setColor(QColorGroup::Text, theColors.highlightedText() );
  }
  QColor colorAlternate ( 250, 250, 250 );
  if ( bAlternateColor )   // every second file we change the color slightly
       colorAlternate  = QColor ( 235, 235, 235 );
  theColors.setColor ( QColorGroup::Base, colorAlternate );
  Q3ListViewItem::paintCell ( p, theColors, column, width, align );
}*/ //ooo

/*//int GroupView::Item::width (  const QFontMetrics &fontMetrics, const Q3ListView *pListView, int iColumn ) const   //ooo
int GroupView::Item::width (  const QFontMetrics &fontMetrics, const QListWidget *pListView, int iColumn ) const    //xxx
{
  //int iWidth = Q3ListViewItem::width ( fontMetrics, pListView, iColumn ); //ooo
  int iWidth = QListWidgetItem::width ( fontMetrics, pListView, iColumn );  //xxx
  QWidget *pToolbox = pListView->parentWidget ( )->parentWidget ( )->parentWidget ( );

  if ( iWidth < pToolbox->width ( ) )
       iWidth = pToolbox->width ( );

  return iWidth;
}*/ //ooo

//GroupView::GroupView ( QWidget *pParent, Q3IconView *pIconView, DialogFiles *pDialog, SourceFileEntry *pEntry )   //ooo
GroupView::GroupView ( QWidget *pParent, QListWidget *pIconView, DialogFiles *pDialog, SourceFileEntry *pEntry )    //xxx
  //: Q3ListView ( pParent )    //ooo
  : QTreeWidget ( pParent )     //xxx
{
  setAcceptDrops ( true );
  //setSorting     (   -1 );            //ooo
  //setAllColumnsShowFocus ( true );    //ooo
  //addColumn ( "Entry" );              //ooo
  //header  ( )->hide ( );              //ooo
  headerItem ( )->setHidden ( true );   //xxx
  setIconSize ( QSize (48, 48) );       //xxx
  m_pPreview   = pIconView; // the main IconView ( DialogFiles::m_pPreview )  //ooo
  m_pDialog    = pDialog;   // Only required to obtain DialogFiles::Thumbs structure for pSourceFileInfo pointer
  m_pSlideshow = NULL;
  m_pSourceFileEntry = pEntry;
}

GroupView::GroupView ( QWidget *pParent, SourceFileEntry *pEntry )
  //: Q3ListView ( pParent )    //ooo
  : QTreeWidget ( pParent )     //xxx
{
  setAcceptDrops ( true );
  //setSorting     (   -1 );            //ooo
  //setAllColumnsShowFocus ( true );    //ooo
  //addColumn ( "Entry" );              //ooo
  //header  ( )->hide ( );              //ooo
  headerItem ( )->setHidden ( true );   //xxx
  setIconSize ( QSize (48, 48) );       //xxx
  m_pPreview   = NULL;  // the main IconView ( DialogFiles::m_pPreview )
  m_pDialog    = NULL;  // Only required to obtain DialogFiles::Thumbs structure for pSourceFileInfo pointer
  m_pSlideshow = NULL;
  m_pSourceFileEntry = pEntry;
}

//GroupView::GroupView ( QWidget *pParent, Q3IconView *pIconView, DialogFiles *pDialog, CXmlSlideshow *pSlideshow ) //ooo
GroupView::GroupView ( QWidget *pParent, QListWidget *pIconView, DialogFiles *pDialog, CXmlSlideshow *pSlideshow )  //xxx
  //: Q3ListView ( pParent )    //ooo
  : QTreeWidget ( pParent )     //xxx
{
  setAcceptDrops ( true );
  //setSorting     (   -1 );            //ooo
  //setAllColumnsShowFocus ( true );    //ooo
  //addColumn ( "Entry" );              //ooo
  //header  ( )->hide ( );              //ooo
  headerItem ( )->setHidden ( true );   //xxx
  setIconSize ( QSize (48, 48) );       //xxx
  m_pPreview         = pIconView; // the main IconView ( DialogFiles::m_pPreview )    //ooo
  m_pDialog          = pDialog;      // Only required to obtain DialogFiles::Thumbs structure for pSourceFileInfo pointer
  m_pSourceFileEntry = NULL;
  m_pSlideshow       = pSlideshow;
}

GroupView::~GroupView ( )
{
  // Slideshows and SourceFileEntries are deleted in DialogFiles::deleteGroups
}

void GroupView::append ( void *pDialogThumbs )
{  
  DialogFiles::Thumbs *pThumbs = (DialogFiles::Thumbs *)pDialogThumbs;

  if ( ! pThumbs || ! pThumbs->pCache )
    return;
  
  if ( ! pThumbs->pCache->arrayOfThumbs )
         pThumbs->pCache->loadImages  ( );

  QFileInfo fileInfo ( pThumbs->pCache->qsFileName );

  //Item   *pItem     = new Item ( this, fileInfo.fileName ( ), true ); //ooo
  Item   *pItem     = new Item ( this, fileInfo.fileName ( ), true ); //xxx
  //new Item ( this, fileInfo.fileName ( ), true ); //xxx
  //QTreeWidgetItem *pItem = new QTreeWidgetItem ( this );   //xxx
  //pItem->setText(0, tr("Cities"));
  //QPixmap thePixmap = QPixmap ( pThumbs->pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio) );			//ooo
  QPixmap thePixmap;    //xxx
  thePixmap.convertFromImage ( pThumbs->pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) );	//xxx
  pItem->pCache     = pThumbs->pCache;    //ooo
  //pItem->setPixmap ( 0, thePixmap );  //ooo
  pItem->setIcon ( 0, thePixmap );      //xxx
  // this is in case we have a SourceFileInfo stored in the Thumbs
  pItem->pSourceFileInfo = pThumbs->pSourceFileInfo;  //ooo

  //m_pDialog->clearPreview ( );
  //QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

QString GroupView::groupName ( )
{
  if ( sourceFileEntry ( ) )
    return sourceFileEntry ( )->qsDisplayName;
  return QString ( "NoName" );
}

void GroupView::setGroupName ( QString &qsName )
{
  if ( sourceFileEntry ( ) )
       sourceFileEntry ( )->qsDisplayName = qsName;
}

void GroupView::dragEnterEvent ( QDragEnterEvent *pEvent )
{
  // Tell the Widget that we accept ImageDrops ...
  // This is necessary otherwise the dropEvent does not occur.

  // Image is coming from itself and Icon is coming from the m_pPreview
  //pEvent->accept ( Q3IconDrag::canDecode ( pEvent ) || Q3ImageDrag::canDecode ( pEvent ) );   //oooo
}

/*Q3DragObject *GroupView::dragObject ( )
{
  Q3DragObject *pDragObject = NULL;
  Item *pItem = (Item *)selectedItem ( );

  if ( ! pItem )
    return Q3ListView::dragObject ( );

  if ( pItem->pCache ) {
    if ( ! pItem->pCache->arrayOfThumbs )
           pItem->pCache->loadImages  ( );
    QImage  theImage;
    //QPixmap thePixmap =  QPixmap ( pItem->pCache->arrayOfThumbs[0]->smoothScale ( 100, 100, Qt::KeepAspectRatio) );				//ooo
    QPixmap thePixmap =  QPixmap ( pItem->pCache->arrayOfThumbs[0]->scaled ( 100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation) );	//xxx
    pDragObject = new Q3ImageDrag ( theImage, this );
    pDragObject->setPixmap ( thePixmap );
  }
//	pDragObject->drag();
//	pDragObject->dragMove();
//	pDragObject->dragCopy();
  return pDragObject;
}*/     //oooo

/*void GroupView::dropEvent ( QDropEvent *pDropEvent )
{
  Item *pTarget = (Item *)itemAt ( pDropEvent->pos ( ) );

  if ( pDropEvent->source ( ) == this ) {
    // Dropped from me to me. meaning we want to re-order
    //Item *pSource = (Item *)selectedItem ( ); //ooo
    Item *pSource = (Item *)listWidget ( ).currentItem ( ); //ooo
    if ( pSource && pTarget ) {
      // Here we will find out which is first in the list
      Item *pTemp = (Item *)pTarget->nextSibling ( );
      while ( pTemp ) {
	if  ( pTemp == pSource ) {
	  break;
	}
	pTemp = (Item *)pTemp->nextSibling ( );
      }
      if ( pTemp ) { // then we drag from the bottom
	if ( pTarget->itemAbove ( ) )
	  pSource->moveItem ( pTarget->itemAbove ( ) );
	else { // put as the top item
	  pSource->moveItem ( pTarget );
	  pTarget->moveItem ( pSource );
	}
      }
      else
	pSource->moveItem ( pTarget );
    }
    else if ( pSource ) // add to the end
      pSource->moveItem ( lastItem ( ) );
  }
  else if ( pDropEvent->source ( ) == m_pPreview->viewport ( ) ) {
    // Coming from the IconView.
    QString   qsFileName;
    QFileInfo fileInfo;
    QImage    theImage;
    //Q3ValueList<Q3IconViewItem *>listToBeDeleted;	//ooo
    QList<Q3IconViewItem *>listToBeDeleted;		//xxx
    Q3IconViewItem *pIcon = m_pPreview->firstItem ( );
    Cache::Thumbs::Entry *pCache = NULL;
    SourceFileInfo *pSourceFileInfo;
    while ( pIcon ) {
      if  ( pIcon->isSelected ( ) ){
	qsFileName =   getKey ( pIcon );
	fileInfo.setFile ( qsFileName );
	pCache = Global::pThumbsCache->find ( qsFileName, m_pDialog->isImageDialog ( qsFileName ) );
	if ( pCache ) {
	  pSourceFileInfo = m_pDialog->findSourceFileInfo ( pIcon );
	  if ( ! pCache->arrayOfThumbs )
	         pCache->loadImages  ( );

	  Item   *pItem     = new Item ( this, pTarget, fileInfo.fileName ( ), true );
	  pItem->pCache     = pCache;

          if ( pCache->getMatrix ( ) )  {
               theImage = pCache->arrayOfThumbs[0]->xForm ( *pCache->getMatrix ( ) );
               //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	       theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
          }
          else if ( ( pCache->fRotate != 0.0 ) && ( pCache->bIsVideo ) )  {
               QMatrix matrix;
               matrix.rotate ( pCache->fRotate );
               theImage = pCache->arrayOfThumbs[0]->xForm ( matrix );
               //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );					//ooo
	       theImage = pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
          }
          else
            theImage = pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );

	  QPixmap thePixmap = QPixmap  ( theImage );
//	  QPixmap thePixmap = QPixmap  ( pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, QImage::ScaleMin) );
	  pItem->setPixmap ( 0, thePixmap );
	  if ( pTarget ) // this is in case we drop onto the top item
	    pTarget->moveItem ( pItem );
	  else
	    pItem->moveItem ( lastItem ( ) ); // we drop to the bottom of the list.
	  // this is in case we have a SourceFileINfo stored in the Thumbs
	  pItem->pSourceFileInfo = pSourceFileInfo;
	  listToBeDeleted.append ( pIcon );
	}
      }
      pIcon = pIcon->nextItem ( );
    }
    QList<Q3IconViewItem *>::iterator it = listToBeDeleted.begin ( );		//oxx
    m_pDialog->lock ( );
    while ( it != listToBeDeleted.end ( ) )  {
      pIcon = *it++;
      m_pDialog->removeThumb ( pIcon );
      delete pIcon;
    }
    m_pDialog->unlock ( );
  }
  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}*/ //oooo

/*void GroupView::slotAlternateColors ( )
{
  Item *pItem = (Item *)firstChild ( );
  bool bAlternate = true;
  while ( pItem ) {
    bAlternate = ! bAlternate;
    pItem->bAlternateColor = bAlternate;
    pItem->repaint ( );
    pItem = (Item *)pItem->nextSibling ( );
  }
}*/

void GroupView::timerEvent ( QTimerEvent *pEvent )
{
  // called back after the thumbs have been rendered in the CacheThumbs class.
  if ( (const uint)pEvent->timerId ( ) == MEDIASCANNER_EVENT + 5 ) { // TYPE_MULTI_THUMB     
    QImage  theImage;
    QPixmap thePixmap;
    GroupView::Item      *pItem  = NULL;
    Cache::Thumbs::Entry *pEntry = NULL;
    QList<Cache::Thumbs::Entry *>list = Global::pThumbsCache->getScannedThumbs ( );		//oxx
    QList<Cache::Thumbs::Entry *>::iterator it;		//oxx
    it = list. begin ( );
    while ( it != list.end ( ) ) {
      pEntry = *it++;
      // Next we have to find the corresponding Item in the ListView 
      //pItem = (GroupView::Item *)firstChild ( );  //ooo
      //pItem = (GroupView::Item *)pItem->child (0);  //xxx
      pItem = (GroupView::Item *)topLevelItem (0)->child (0);  //xxx
      while ( pItem ) {
	if  ( pItem->pCache == pEntry ) {
	  pEntry->iScanStatus = 0;
	  if ( ! pEntry->arrayOfThumbs )
	         pEntry->loadImages  ( );
	  //theImage = pEntry->arrayOfThumbs[0]->smoothScale ( m_iSize, m_iSize, Qt::KeepAspectRatio );				//ooo
	  theImage = pEntry->arrayOfThumbs[0]->scaled ( m_iSize, m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	  thePixmap.convertFromImage ( theImage );
	  //pItem->setPixmap ( 0, thePixmap );     //ooo
      pItem->setIcon ( 0, thePixmap );         //xxx
	  break;
	}
	//pItem = (GroupView::Item *)pItem->nextSibling ( );   //ooo
	pItem = (GroupView::Item *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
      }
    }
  }
  m_pDialog->m_bCanClose = true;
}

SourceFileEntry *GroupView::sourceFileEntry ( )
{
  return m_pSourceFileEntry;
}

CXmlSlideshow *GroupView::slideshow ( )
{
  return m_pSlideshow;
}

void GroupView::setSlideshow ( CXmlSlideshow *pSlideshow )
{
  m_pSlideshow = pSlideshow;
}

DialogFiles::ThumbnailRequest::ThumbnailRequest ( DialogFiles *pDialog )
  : ExecuteJob ( pDialog )
{
  pImage          = new QImage;
  pOwnerItem      = NULL;
  pParent         = pDialog;
  iNumberOfThumbs = 0;
}

//DialogFiles::ThumbnailRequest::ThumbnailRequest ( DialogFiles *pDialog, Q3IconViewItem *pItem, QString qsFile, uint iNr ) //ooo
DialogFiles::ThumbnailRequest::ThumbnailRequest ( DialogFiles *pDialog, QListWidgetItem *pItem, QString qsFile, uint iNr )  //xxx
  : ExecuteJob ( pDialog )
{
  pImage          = new QImage;
  pOwnerItem      = pItem;
  pParent         = pDialog;
  iNumberOfThumbs = iNr;
  qsFileName      = qsFile;
}

DialogFiles::ThumbnailRequest::~ThumbnailRequest ( )
{
  if ( pImage )
    delete pImage;
}

bool DialogFiles::ThumbnailRequest::response ( )
{
  if ( ( ! pParent ) || ( ! DialogFiles::isMyself ( pParent ) ) )
    return false;

  if ( ! pParent->isValid ( pOwnerItem ) )
    return false;

  // Rather then copy data back and forth I simply steal the pointer to the QImage object
  QImage *pTempImage = pImage;
  pImage = NULL;
  bool bMarkIcon = bSuccess && ( iNumberOfThumbs > 1 );
  pParent->initWork ( pOwnerItem, pTempImage, bMarkIcon );

  // This is the response to the first thumbnail
  // If we return successfully we want to get the length of the video
  // And we want to get a number of other thumbnails or this item.
  if ( bSuccess && ( iNumberOfThumbs > 1 ) ) {
    MultiThumbRequest *pRequest = new MultiThumbRequest ( pParent, pOwnerItem, qsFileName, pTempImage, iNumberOfThumbs );
    // Since we are still in the worker thread we can not register this one with the MediaScanner
    // Thus we call a timer to get us into the main thread.
    pParent->registerRequest ( pRequest );
  }

  return true;
}

void DialogFiles::registerRequest ( MultiThumbRequest *pRequest )
{
  m_mutexRequest.lock     ( );
  m_listOfRequests.append ( pRequest );
  m_mutexRequest.unlock   ( );

  QTimer::singleShot      ( 10, this, SLOT ( slotRegisterRequests ( ) ) );
}

void DialogFiles::slotRegisterRequests ( )
{
  uint t;
  QList<MultiThumbRequest *> listTemp;		//oxx
  MultiThumbRequest *pRequest = NULL;

  m_mutexRequest.lock ( );
  {
    for ( t=0; t<(uint)m_listOfRequests.count ( ); t++ )	//ox
      listTemp.append ( m_listOfRequests[t] );

    m_listOfRequests.clear ( );
  }
  m_mutexRequest.unlock ( );

  //int iProgress = m_pProgressBar->totalSteps ( );     //ooo
  int iProgress = m_pProgressBar->value ( );            //xxx
  for ( t=0; t<(uint)listTemp.count ( ); t++ ) {	//ox
    pRequest = listTemp[t];
    if ( isValid ( pRequest->pOwnerItem ) ) {
      MediaCreator::registerWithMediaScanner ( pRequest );
      iProgress += pRequest->iNumberOfThumbs;
      markIcon  (  pRequest->pOwnerItem, 1 );
    }
    else
      delete pRequest;
  }

  // resetWait will skip over the 5 second wait at the end of the event loop
  MediaCreator::pPreviewObject->resetWait ( );

  // Oh oh, there is more work to be done ...
  //m_pProgressBar->setTotalSteps ( iProgress );    //ooo
  //m_pProgressBar->setValue ( iProgress );           //xxx
  //m_pProgressBar->setProgress ( m_iProgress );    //ooo
  m_pProgressBar->setValue ( m_iProgress );         //xxx

  debug_cout ( "DialogFiles::slotRegisterRequests Registered <%d> andResetWait\n", (int)listTemp.count ( ) );
  listTemp.clear ( );
}

//void DialogFiles::play ( Cache::Thumbs::Entry *pEntry, Q3IconViewItem *pItem )    //ooo
void DialogFiles::play ( Cache::Thumbs::Entry *pEntry, QListWidgetItem *pItem )     //xxx
{
  m_pPreviewer->setItem ( pEntry, pItem );
}

void DialogFiles::lock ( )
{
  m_mutex.lock ( );
}

void DialogFiles::unlock ( )
{
  m_mutex.unlock ( );
}

bool DialogFiles::lockDialog ( )
{
  m_mutexDialog.lock ( );

  return ( m_pIAmAlive );
}

void DialogFiles::unlockDialog ( )
{  
  m_mutexDialog.unlock ( );
}

bool DialogFiles::isImageDialog ( )
{
  return m_bIsImageDialog;
}

bool DialogFiles::isImageDialog ( QString qsFileName )
{
  // This function is called for the ImageDialog, which can have either images or vids.
  //return m_bIsImageDialog;
  QFileInfo fileInfo ( qsFileName );
  QString qsVids = Global::pApp->getVidFilter ( );
  //if ( qsVids.find ( fileInfo.extension ( FALSE ) ) > -1 )    //ooo
  if ( qsVids.indexOf ( fileInfo.suffix ( ) ) > -1 )            //xxx
    return false;
  return m_bIsImageDialog;
}

//DialogFiles::MultiThumbRequest::MultiThumbRequest ( DialogFiles *pDialog, Q3IconViewItem *pItem, QString qsFile, QImage *pFirstImage, uint iThumbs )  //ooo
DialogFiles::MultiThumbRequest::MultiThumbRequest ( DialogFiles *pDialog, QListWidgetItem *pItem, QString qsFile, QImage *pFirstImage, uint iThumbs )   //xxx
  : ThumbnailRequest ( pDialog )
{
  theType         = TYPE_MULTI_THUMB;
  pParent         = pDialog;
  pOwnerItem      = pItem;
  qsFileName      = qsFile;
  iNumberOfThumbs = iThumbs;
  iSize           = pDialog->thumbnailSize ( );
  iDelta          = -1;
  iMovieLength    = -1;
  iCurrentThumb   =  0;

  // Init the array
  //*pFirstImage = pFirstImage->smoothScale ( 300, 300, Qt::KeepAspectRatio );				//ooo
  *pFirstImage = pFirstImage->scaled ( 300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx

  arrayOfThumbs      = new QImage *[iThumbs];
  for ( uint t=0; t<iThumbs; t++ )
    arrayOfThumbs[t] = new QImage ( *pFirstImage );
}

DialogFiles::MultiThumbRequest::~MultiThumbRequest ( )
{
  if ( arrayOfThumbs ) {
    for ( uint t=0; t<iNumberOfThumbs; t++ )
      delete arrayOfThumbs[t];
    delete arrayOfThumbs;
    arrayOfThumbs = NULL;
  }
  if ( pImage )
    delete pImage;
  pImage = NULL;
}

DialogFiles::MultiThumbRequest *DialogFiles::MultiThumbRequest::clone ( )
{
  // Ceates a clone of the current MultiThumbRequest, but simply takes over the 
  // arrayOfThumbs rather than copy them.
  MultiThumbRequest *pCopy = new MultiThumbRequest ( *this );
  
  return pCopy;
}

bool DialogFiles::MultiThumbRequest::response ( )
{  
  // Runs in the background thread. No QPixmap or GUI manipulations please.
  if ( ( ! pParent ) || ( ! DialogFiles::isMyself ( pParent ) ) )
    return false;

  if ( ! pParent->isValid ( pOwnerItem ) )
    return false;

  // qsFileName holds the length of the video and no longer the name
  if ( iMovieLength < 0 ) {
    // The first time around we have to init the offset

    Utils theUtils;
    //QFileInfo fileInfo ( getKey ( pOwnerItem ) ); //oooo
    QFileInfo fileInfo ( pOwnerItem->text ( ) );     //xxx
    iMovieLength    = theUtils.getMsFromString ( qsFileName );
    // Look we want to have the end displayed even with rounding errors.
    iDelta          = (long)( (double)( iMovieLength - 500 ) / ( iNumberOfThumbs - 1 ) );
    if ( iDelta < 500 ) // sanity check
      iDelta        = (long)( (double)( iMovieLength - (iDelta/2) ) / ( iNumberOfThumbs - 1 ) );
    iCurrentThumb   = 1;
    QString  qsFile = fileInfo.fileName ( );
    debug_cout ( "DialogFiles::MultiThumbRequest::response <%s> = movieLength<%d> NrOfTh<%d> delta<%d>\n",
             //qsFileName.ascii(), (int)iMovieLength, (int)iNumberOfThumbs, (int)iDelta );          //ooo
             qsFileName.toLatin1().data(), (int)iMovieLength, (int)iNumberOfThumbs, (int)iDelta );  //xxx

    qsTitle = qsFile + "\n" + qsFileName;
    handleResponse ( ( iMSecondsOffset < 0 ), qsTitle );
    iMSecondsOffset = iDelta;
    return true;
 }

  // here we store the image and increase the iMSecOffset for the next thumbnail.
  if ( iCurrentThumb < iNumberOfThumbs )  {
    if ( ! pImage->isNull ( ) )
      //*arrayOfThumbs[iCurrentThumb] = pImage->smoothScale ( 300, 300, Qt::KeepAspectRatio );  //ooo
      *arrayOfThumbs[iCurrentThumb] = pImage->scaled ( 300, 300, Qt::KeepAspectRatio );         //xxx
  }

  debug_cout ( "DialogFiles::MultiThumbRequest::response iCurrent<%d> movieLength<%d> of<%d> <%p>\n",
             (int)iCurrentThumb, (int)iMovieLength, (int)iMSecondsOffset, pParent );

  iMSecondsOffset = ++iCurrentThumb * iDelta;

  if ( ( iMSecondsOffset >= iMovieLength ) || ( iMSecondsOffset < 0 ) )
    handleResponse ( true, QString ( ) );

  return true;
}

void DialogFiles::MultiThumbRequest::handleResponse ( bool bCreateThumbs, QString qsTitle )
{
  if ( ! pParent->isValid ( pOwnerItem ) )
    return;

  MultiThumbRequest *pClone = clone ( );
  pClone->qsTitle        = qsTitle;
  pClone->bUpdateAllInfo = bCreateThumbs; // Let's hijack this string object for the clone
  pClone->pImage  = NULL;
  if ( bCreateThumbs ) // The clone takes ownership of the generated thumbs ...
    arrayOfThumbs = NULL;
  else
    pClone->arrayOfThumbs = NULL;

  pParent->addClone  ( pClone );
  QTimer::singleShot ( 1, pParent, SLOT ( slotMultiThumbResponse ( ) ) );
}

void DialogFiles::addClone (  DialogFiles::MultiThumbRequest *pClone )
{
  m_mutexRequest.lock ( );
  m_listOfClones.push_back ( pClone );
  m_mutexRequest.unlock ( );
}

void DialogFiles::slotMultiThumbResponse ( )
{
  m_mutexRequest.lock ( );

  DialogFiles::MultiThumbRequest *pClone = NULL;
  QList<DialogFiles::MultiThumbRequest *>::iterator it = m_listOfClones.begin ( );	//oxx
  while ( it != m_listOfClones.end ( ) )  {
    pClone = *it++;
    // Should always run in the main GUI thread.
    if ( ! pClone )
      continue;
    if ( ! isValid ( pClone->pOwnerItem ) )  {
      delete pClone;
      continue;
    }

    if ( pClone->qsTitle.length ( ) > 0 )
         //pClone->pOwnerItem->setText ( pClone->qsTitle );     //ooo
         pClone->pOwnerItem->setText ( pClone->qsTitle );    //xxx

    if ( pClone->bUpdateAllInfo )
         pClone->createThumbs ( );

    // Now I can delete the clone.
    delete pClone;
  }
  m_listOfClones.clear  ( );
  m_mutexRequest.unlock ( );
}
/*
bool DialogFiles::MultiThumbRequest::response ( )
{
  if ( ( ! pParent ) || ( ! DialogFiles::isMyself ( pParent ) ) )
    return false;

  if ( ! pParent->isValid ( pOwnerItem ) )
    return false;

  // qsFileName holds the length of the video and no longer the name
  if ( iMovieLength < 0 ) {
    // The first time around we have to init the offset

    Utils theUtils;
    QFileInfo fileInfo ( getKey ( pOwnerItem ) );
    iMovieLength    = theUtils.getMsFromString ( qsFileName );
    // Look we want to have the end displayed even with rounding errors.
    iDelta          = (long)( (double)( iMovieLength - 500 ) / ( iNumberOfThumbs - 1 ) );
    if ( iDelta < 500 ) // sanity check
      iDelta        = (long)( (double)( iMovieLength - (iDelta/2) ) / ( iNumberOfThumbs - 1 ) );
    iCurrentThumb   = 1;
    QString  qsFile = fileInfo.fileName ( );
    debug_cout ( "DialogFiles::MultiThumbRequest::response <%s> = movieLength<%d> NrOfTh<%d> delta<%d>\n",
             qsFileName.ascii(), (int)iMovieLength, (int)iNumberOfThumbs, (int)iDelta );
    pOwnerItem->setText ( qsFile + "\n" + qsFileName );

    if ( iMSecondsOffset < 0 )
         createThumbs ( );

    iMSecondsOffset = iDelta;
    return true;
  }

  // here we store the image and increase the iMSecOffset for the next thumbnail.
  if ( iCurrentThumb < iNumberOfThumbs )  {
    if ( ! pImage->isNull ( ) )
      // *arrayOfThumbs[iCurrentThumb] = pImage->smoothScale ( 300, 300, QImage::ScaleMin );				//ooo
      *arrayOfThumbs[iCurrentThumb] = pImage->scaled ( 300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
  }

  debug_cout ( "DialogFiles::MultiThumbRequest::response iCurrent<%d> movieLength<%d> of<%d> <%p>\n",
             (int)iCurrentThumb, (int)iMovieLength, (int)iMSecondsOffset, pParent );

  iMSecondsOffset = ++iCurrentThumb * iDelta;

  if ( ( iMSecondsOffset >= iMovieLength ) || ( iMSecondsOffset < 0 ) )
    createThumbs ( );

  return true;
}
*/

void DialogFiles::MultiThumbRequest::createThumbs ( )
{  
  // coming here is the last time we get to for this MultiThumbRequest
  // In other words we have all screenshots.
  // Note: Can ONLY be executed in main GUI thread !
  if ( ! DialogFiles::isMyself ( pParent ) )
    return;

  if ( ! pParent->isValid ( pOwnerItem ) )
    return;

  if ( ! pParent->lockDialog ( ) )
    return;

  // So the first thing we ought to do is to store this entry into the cache.
  QString qsFile;
  //qsFile = getKey ( pOwnerItem ); //oooo
  qsFile = pOwnerItem->text ( );  //xxx
 
  Cache::Thumbs::Entry *pCache = NULL;
  pCache = Global::pThumbsCache->append ( qsFile, qsFileName, iNumberOfThumbs, arrayOfThumbs );

  // Next we create the required object for this dialog.
  Thumbs *pThumbs       = new Thumbs;
  pThumbs->pOwnerItem   = pOwnerItem;
  pThumbs->iNumberOfPix = iNumberOfThumbs;
  pThumbs->arrayOfPix   = new QPixmap *[iNumberOfThumbs];
  pThumbs->pCache       = pCache;

  for ( uint t=0; t<iNumberOfThumbs; t++ ) {
    pThumbs->arrayOfPix[t] = new QPixmap;
    pParent->createPix ( pThumbs, t );
  }

  // the array of pointers to the QImage objects reside now in the cache.
  bool bSetIcon = true;
  arrayOfThumbs = NULL;
  pParent->append ( pThumbs );
  if ( iMSecondsOffset  > 0 ) // if DV format then scan MetaInfo
    bSetIcon =  ! ( pParent->initScanDV ( pThumbs ) );
  pParent->addProgress ( iNumberOfThumbs );
  // If there is no scanning for DV Meta Data then we should refresh the icon here
  if ( bSetIcon )
    //pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[0] );    //ooo
    pOwnerItem->setIcon ( *pThumbs->arrayOfPix[0] );     //xxx
  pParent->unlockDialog ( );
}

//DialogFiles::WorkWaiting::WorkWaiting ( Q3IconViewItem *pViewItem, QImage *pImg, bool bMark ) //ooo
DialogFiles::WorkWaiting::WorkWaiting ( QListWidgetItem *pViewItem, QImage *pImg, bool bMark )  //xxx
{
  pItem     = pViewItem;
  pImage    = pImg;
  bMarkIcon = bMark;
}

DialogFiles::WorkWaiting::~WorkWaiting ( )
{
  if ( pImage )
    delete pImage;
  pImage = NULL;
};

//ScanDVDate::ScanDVDate ( DialogFiles *pDlg, QString qsFile, Q3IconViewItem *pItm )    //ooo
ScanDVDate::ScanDVDate ( DialogFiles *pDlg, QString qsFile, QListWidgetItem *pItm )     //xxx
{  
  pDialog    = pDlg;
  pItem      = pItm;
  qsFileName = qsFile;
  bExited    = false;

  Utils theUtils;
  QString qsTool = theUtils.getToolPath ( "dv2sub" );
  
  QStringList arguments;    //xxx
  //arguments << qsTool << "-d" << qsFileName;    //xxx
  arguments << "-d" << qsFileName;    //xxx

  //addArgument ( qsTool ); //ooo
  //addArgument (  "-d"  ); //ooo
  //addArgument ( qsFileName ); // fileName //ooo
  setArguments ( arguments );   //xxx

  connect ( this, SIGNAL ( processExited ( ) ), this, SLOT ( slotExited ( ) ) );

  //start ( ); // kick off the worker thread
  start ( qsTool, arguments ); // kick off the worker thread    //xxx
}

ScanDVDate::~ScanDVDate ( )
{
}

void ScanDVDate::ScanDVDate::slotExited ( )
{
  Cache::Thumbs::Entry *pEntry = NULL;
  QDateTime theDate;
  QRegExp rx    (  "\\d.*" );
  rx.setMinimal ( true );

  bExited = true;
  // Should be something like : "\tCreation Date: 2000-04-03 14:07:45"
  //QString qsStdout = Q3Process::readStdout ( );               //ooo
  QString qsStdout = QProcess::readAllStandardOutput().data();  //xxx
  qsStdout.remove ( "\t" );
  qsStdout.remove ( "\n" );

  //int     iPos   = rx.search      ( qsStdout, 0 );    //ooo
  int     iPos   = rx.indexIn      ( qsStdout, 0 );     //xxx
  QString qsDate = qsStdout.right ( qsStdout.length ( ) - iPos );
  if ( qsDate.length ( ) > 1 ) {
    theDate = QDateTime::fromString ( qsDate, Qt::ISODate ); //"yyyy-MM-dd hh:mm:ss" );
    if ( theDate.isValid ( ) ) {
      // Okay we seem to have a valid date at hand.

      // Fisrst we change the date in the cache ...
      pEntry = Global::pThumbsCache->find ( qsFileName );
      if ( pEntry ) {
        pEntry->dateCreated = theDate;
        Global::pThumbsCache->saveCacheDB ( );
      }
      // Next we update the pItem ...
      pDialog->initWork ( pItem, NULL, false );

      QApplication::postEvent ( pDialog, new QTimerEvent ( MEDIASCANNER_EVENT + 4 ) );
      return;
    }
  }
  // there was an error so the progress needs to be decreased.
  pDialog->addProgress ( -1 );
  QApplication::postEvent ( pDialog, new QTimerEvent ( MEDIASCANNER_EVENT + 4 ) );
}

bool ScanDVDate::hasExited ( )
{
  return bExited;
}

//DialogFiles::DirItem::DirItem ( Q3ListView *pParentView, QString qsPath ) //ooo
DialogFiles::DirItem::DirItem ( QTreeWidget *pParentView, QString qsPath )  //xxx
  //: Q3ListViewItem ( pParentView )    //ooo
  : QTreeWidgetItem ( pParentView )     //xxx
{
  pParent    = NULL;
  bReadable  = QDir( qsPath ).isReadable ( );
  qsPathName = qsPath;
  //setPixmap ( 0, QPixmap( xpmFolderOpen ) );  //ooo
  setIcon ( 0, QPixmap( xpmFolderOpen ) );      //xxx
  setText   ( 0, QFileInfo ( qsPath ).fileName ( ) );
  setText   ( 1, bReadable ? "Dir" : "Unreadable Dir" );
  //setExpandable ( TRUE ); //ooo
  setExpanded ( true );     //xxx
}

DialogFiles::DirItem::DirItem ( DirItem *pParentItem, QString qsPath )
  //: Q3ListViewItem ( pParentItem )    //ooo
: QTreeWidgetItem ( pParentItem )       //xxx
{
  QDir theDir ( qsPath );
  theDir.setFilter ( QDir::Dirs );

  pParent    = pParentItem;
  bReadable  = theDir.isReadable ( );
  qsPathName = qsPath;
  if ( qsPath[qsPath.length ()-1] != '/' )
    qsPathName = qsPathName + "/";
  if ( theDir.count ( ) > 2 ) {
    //setPixmap ( 0, QPixmap( xpmFolderOpen ) );    //ooo
    setIcon ( 0, QPixmap( xpmFolderOpen ) );        //xxx
    //setExpandable ( TRUE );   //ooo
    setExpanded ( true );       //xxx
  }
  else {
    //setPixmap ( 0, QPixmap( xpmFolderLocked ) );  //ooo
    setIcon ( 0, QPixmap( xpmFolderLocked ) );      //xxx
    //setExpandable ( FALSE );  //ooo
    setExpanded ( false );      //xxx
  }

  setText ( 0, QFileInfo ( qsPath ).fileName ( ) );
  setText ( 1, bReadable ? "Dir" : "Unreadable Dir" );
}

void DialogFiles::DirItem::setOpen ( bool bOpen )
{
  if ( bOpen )
    //setPixmap ( 0, QPixmap( xpmFolderOpen ) );  //ooo
    setIcon ( 0, QPixmap( xpmFolderOpen ) );      //xxx
  else
    //setPixmap ( 0, QPixmap( xpmFolderClosed ) ); //FolderClosed );    //ooo
    setIcon ( 0, QPixmap( xpmFolderClosed ) ); //FolderClosed );        //xxx

  if ( bOpen && ! childCount ( ) ) {
    QString qsDir ( qsPathName );
    QDir  thisDir ( qsDir );
    if ( !thisDir.isReadable ( ) ) {
      bReadable = false;
      //setExpandable ( FALSE );    //ooo
      setExpanded ( false );        //xxx
      return;
    }

    //listView ( )->setUpdatesEnabled ( FALSE );    //oooo
    //listView ( )->setUpdatesEnabled ( false );      //xxx
    //const QFileInfoList *pFiles = thisDir.entryInfoList ( );	//ooo
    const QFileInfoList pFiles = thisDir.entryInfoList ( );	//xxx
    //if ( pFiles ) {					//ooo
    if ( !pFiles.isEmpty() ) {				//xxx
      //QFileInfoListIterator it( *pFiles );		//ooo
      QListIterator<QFileInfo> it( pFiles );		//xxx
      //QFileInfo *pFileInfo;				//ooo
      QFileInfo fileInfo;				//xxx
      //while( (pFileInfo = it.current ( ) ) != 0 )  {	//ooo
      while( it.hasNext() ) {				//xxx
        //++it;						//ooo
	fileInfo = it.next();				//xxx
        //if ( ( pFileInfo->fileName ( ) == "."  ) || 	//ooo
	if ( ( fileInfo.fileName ( ) == "."  ) ||	//xxx
	     //( pFileInfo->fileName ( ) == ".." ) )	//ooo
	     ( fileInfo.fileName ( ) == ".." ) )	//xxx
	  continue;
        //else if ( pFileInfo->isDir ( ) )		//ooo
        else if ( fileInfo.isDir ( ) )			//xxx
	  //new DirItem ( this, pFileInfo->absFilePath ( ) );  //pFileInfo->fileName ( ) );	   //ooo
	  new DirItem ( this, fileInfo.absoluteFilePath ( ) );  //pFileInfo.fileName ( ) );		//xxx
      }
    }
    //listView ( )->setUpdatesEnabled ( TRUE ); //oooo
  }
  //Q3ListViewItem::setOpen( bOpen );   //oooo
  setExpanded ( true );                 //xxx
}

DialogFiles::DirItem *DialogFiles::DirItem::findSubDir ( QString qsSubDir )
{ 
  //DirItem *pDirItem = (DirItem *)firstChild ( );      //ooo
  //DirItem *pDirItem = (DirItem *)pDirItem->child (0);   //xxx
  DirItem *pDirItem = (DirItem *)child (0);   //xxx
  //DirItem *pDirItem = (DirItem *)treeWidget()->itemBelow (treeWidget()->topLevelItem ( 0 ) );   //xxx
  //DirItem *pDirItem = (DirItem *)treeWidget()->itemBelow ( treeWidget()->currentItem ( ) );   //xxx
 
  while ( pDirItem ) {    
    if ( pDirItem->text ( 0 ) == qsSubDir )
      return pDirItem;
    //pDirItem = (DirItem *)pDirItem->nextSibling ( );  //ooo
    pDirItem = (DirItem *)pDirItem->parent()->child(pDirItem->parent()->indexOfChild(pDirItem)+1);	//xxx
  }
  return NULL;
}

QString DialogFiles::DirItem::dir ( )
{
  return qsPathName;
}

DialogFiles::Manual::Manual ( )
{
  iHashValue = 0LL;
}

DialogFiles::Manual::~Manual ( )
{
  QMap<unsigned long long, Change *>::iterator it = mapOfChange.begin ( );
  while ( it != mapOfChange.end ( ) )
    delete *it++; //.data ( );
  mapOfChange.clear  ( );
}

DialogFiles::Manual::Change::Change ( )
{
  iHashValue = 0LL;
}

DialogFiles::Manual::Change::~Change ( )
{
  clearAudioList    ( );
  clearSubtitleList ( );
}

void DialogFiles::Manual::Change::clearAudioList ( )
{
  QList<Audio *>::iterator it = audioList.begin ( );		//oxx
  while ( it != audioList.end ( ) )
    delete *it++;
  audioList.clear ( );
}

void DialogFiles::Manual::Change::clearSubtitleList ( )
{
  QList<SubtitleEntry *>::iterator it = subtitleList.begin ( );		//oxx
  while ( it != subtitleList.end ( ) )
    delete *it++;
  subtitleList.clear ( );
}

void DialogFiles::Manual::Change::copyAudioList ( QList<Audio *> &original )		//oxx
{
  clearAudioList ( );

  Audio *pAudio;
  QList<Audio *>::iterator it = original.begin ( );	//oxx
  while ( it != original.end ( ) )  {
     pAudio   = new Audio;
    *pAudio   = *(*it++);
     audioList.append ( pAudio );
  }
}

void DialogFiles::Manual::Change::copySubtitleList ( QList<SubtitleEntry *>  &original )	//oxx
{
  clearSubtitleList ( );

  SubtitleEntry *pSubtitle;
  QList<SubtitleEntry *>::iterator it = original.begin ( );	//oxx
  while ( it  != original.end ( ) )  {
     pSubtitle = new SubtitleEntry;
    *pSubtitle = *(*it++);
     subtitleList.append  ( pSubtitle );
  }
}

QList<Audio *> DialogFiles::Manual::Change::copyAudioList ( )		//oxx
{
  QList<Audio *> list;		//oxx
  Audio *pAudio;
  QList<Audio *>::iterator it = audioList.begin ( );	//oxx
  while ( it != audioList.end ( ) )  {
     pAudio   = new Audio;
    *pAudio   = *(*it++);
     list.append ( pAudio );
  }
  return list;
}

QList<SubtitleEntry *> DialogFiles::Manual::Change::copySubtitleList ( )	//oxx
{
  QList<SubtitleEntry *> list;		//oxx
  SubtitleEntry *pSubtitle;
  QList<SubtitleEntry *>::iterator it = subtitleList.begin ( );		//oxx
  while ( it  != subtitleList.end ( ) )  {
     pSubtitle = new SubtitleEntry;
    *pSubtitle = *(*it++);
     list.append  ( pSubtitle );
  }
  return list;
}

//DialogFiles::VFSelectItem::VFSelectItem ( bool bAlt, Q3ListBox *pParent, const QString &text )    //ooo
//DialogFiles::VFSelectItem::VFSelectItem ( bool bAlt, QTreeWidget *pParent, const QString &text )    //xxx
DialogFiles::VFSelectItem::VFSelectItem ( bool bAlt, QListWidget *pParent, const QString &text )    //xxx
  //: Q3ListBoxText ( pParent, text )   //ooo
  //: QTreeWidgetItem ( pParent )   //xxx
  : QListWidgetItem ( pParent )   //xxx
{
  bAlternate = bAlt;
  setText(text);    //xxx
}

void DialogFiles::VFSelectItem::paint ( QPainter *pPainter )
{
  QColor colorAlternate ( 250, 250, 250 );
  if ( isSelected ( ) )
    colorAlternate  = QColor ( 255, 150, 50 );
  else if ( bAlternate )   // every second file we change the color slightly
    colorAlternate  = QColor ( 235, 235, 235 );
  pPainter->fillRect  ( pPainter->viewport ( ), colorAlternate );
  //Q3ListBoxText::paint ( pPainter );  //oooo
}

//int DialogFiles::VFSelectItem::height ( const Q3ListBox * ) const //ooo
//int DialogFiles::VFSelectItem::height ( const QTreeWidget * ) const //xxx
int DialogFiles::VFSelectItem::height ( const QListWidget * ) const //xxx
{
  return 50;
}

DialogFiles::Thumbs::Thumbs ( )
{
  pSourceFileInfo = NULL; // only set if the item is created through a drop from a GroupView.
  arrayOfPix      = NULL;
  pOwnerItem      = NULL;
  pCache          = NULL;
  iNumberOfPix    = 0;
  iCurrentPix     = 0;
}

DialogFiles::Thumbs::~Thumbs ( )
{
  clearSubtitles ( );
  clearAudio     ( );
  if ( arrayOfPix ) {
    for ( uint t=0; t<iNumberOfPix; t++ )
      delete arrayOfPix[t];
    delete []arrayOfPix;
  }
}

void DialogFiles::Thumbs::clearAudio ( )
{
  QList<Audio *>::iterator it = audioList.begin ( );		//oxx
  while ( it != audioList.end ( ) )
    delete *it++;

  audioList.clear ( );
}

void DialogFiles::Thumbs::clearSubtitles ( )
{
  QList<SubtitleEntry *>::iterator it = subtitleList.begin ( );		//oxx
  while ( it != subtitleList.end ( ) )
    delete *it++;

  subtitleList.clear ( );
}

bool DialogFiles::Thumbs::findTracks ( )
{
  // This function will scan the files directory and add any audio file
  // to the Thumbs, which contains the same baseName
  QDir         theDir;
  QFileInfo    fileInfo;
  QStringList  list;
  QStringList::iterator it;
  QString qsBaseName, qsPath, qsFileName, qsVideoFileName, qsLang, qsTemp, qsFilter, qsExt;
  bool bFound, bAudio, bSubtitle, bIgnoreSubtitleFiles, bIgnoreAudioFiles;
  unsigned long  long iPathHash = 0LL, iFileHash = 0LL;
  int iSubtitleNumber = 0;
  Manual::Change *pChange = NULL;
  bIgnoreAudioFiles = bIgnoreSubtitleFiles = false;

  if ( pSourceFileInfo )
    qsVideoFileName = pSourceFileInfo->qsFileName;
  else if ( pCache )
    qsVideoFileName = pCache->qsFileName;
  else return false;

  fileInfo.setFile ( qsVideoFileName );
  qsBaseName = fileInfo.baseName ( );
  //qsPath     = fileInfo.dirPath  ( TRUE );    //ooo
  qsPath     = fileInfo.path ( );               //xxx
  theDir.setPath ( qsPath );
  qsFilter   = qsBaseName + "*";

  iPathHash = Global::pThumbsCache->hashFromPath ( qsPath );
  iFileHash = Global::pThumbsCache->hashFromFile ( qsVideoFileName );
  Manual *pManual = DialogFiles::m_mapOfManualChange[iPathHash];

  clearSubtitles ( );
  clearAudio     ( );

  // Here we check if we have User modified audio tracks or subtitle tracks.
  if ( pManual )  {
    pChange = pManual->mapOfChange[iFileHash];
    if ( pChange )  {
      // Yep, so lets see audio OR subtitles OR both.
      if ( pChange->audioList.size ( ) > 0 )  {
        bIgnoreAudioFiles = true;
        audioList = pChange->copyAudioList ( );
      }
      if ( pChange->subtitleList.size ( ) > 0 )  {
        bIgnoreSubtitleFiles = true;
        subtitleList = pChange->copySubtitleList ( );
      }
    }
  }

  if ( bIgnoreAudioFiles && bIgnoreSubtitleFiles )
    return true;

  //list = theDir.entryList ( qsFilter, QDir::Files );  //oooo
  list = theDir.entryList ( QStringList(qsFilter));     //xxx
  it   = list.begin ( );
  while ( it != list.end ( ) )  {
    qsFileName = *it++;

    fileInfo.setFile ( qsFileName );
    //qsExt = fileInfo.extension ( FALSE ).lower ( );   //ooo
    qsExt = fileInfo.suffix ( ).toLower ( );            //xxx

    // for efficiency we'll check for the extensions up front.
    bSubtitle = bAudio = false;
    if ( ( qsExt == "srt" ) || ( qsExt == "saa" ) || ( qsExt == "ass" ) )
      bSubtitle = true;
    else if ( ( qsExt == "ac3" ) || ( qsExt == "mp3" ) || ( qsExt == "mp2" ) ||
              ( qsExt == "mpa" ) || ( qsExt == "m2a" ) || ( qsExt == "ogg" ) ||
              ( qsExt == "wav" ) || ( qsExt == "mpega" ) )
      bAudio = true;

    if ( ! bAudio && ! bSubtitle )
      continue; // I don't know what you are but I don't want you

    // First we search for language codes ...
    //qsTemp = fileInfo.baseName ( TRUE );  //ooo
    qsTemp = fileInfo.baseName ( );         //xxx
    qsTemp.remove ( qsBaseName );
    qsLang = Global::qsDefaultLanguage;
    if ( qsTemp.length ( ) >  3 )
      qsTemp = qsTemp.right ( 3 );
    if ( qsTemp.length ( ) >= 2 )  {
      if ( ( qsTemp[0] == '.' ) || ( qsTemp[0] == '_' ) || ( qsTemp[0] == '-' ) )
        qsTemp = qsTemp.right ( qsTemp.length ( ) -1 );
      qsLang = qsTemp.left  ( 2 );
    }

    // Check if we have this track already present
    qsFileName = qsPath + "/" + qsFileName;
    bFound = false;

    // Sort out subtitle files
    if ( bSubtitle && ! bIgnoreSubtitleFiles )  {
      // Check if we have this audio track already present
      for ( uint t=0; t<(uint)subtitleList.count ( ); t++ ) {		//ox
        if ( subtitleList[t]->m_qsFileName == qsFileName )  {
          bFound = true;
          break;
        }
      }
      if ( ( ! bFound ) && ( subtitleList.count ( ) <= MAX_SUBTITLES ) ) {
        // Now we can create the new Audio entry if not already existing
        SubtitleEntry *pSubtitle     = new SubtitleEntry;
        pSubtitle->m_qsFileName      = qsFileName;
        pSubtitle->m_iSubtitleNumber = iSubtitleNumber++;
        pSubtitle->m_qsIso639        = qsLang;
        subtitleList.append ( pSubtitle );
      }
      continue;
    }

    // sort out audio files
    if ( ( ! bAudio ) || ( bIgnoreAudioFiles ) )
      continue;

    // Check if we have this audio track already present
    for ( uint t=0; t<(uint)audioList.count ( ); t++ ) {	//ox
      if ( audioList[t]->qsFileName == qsFileName )  {
        bFound = true;
        break;
      }
    }
    if ( ( ! bFound ) && ( audioList.count ( ) <= MAX_AUDIO ) ) {
      // Now we can create the new Audio entry if not already existing
      Audio *pAudio        = new Audio;
      pAudio->qsFileName   = qsFileName;
      pAudio->qsIso639     = qsLang;
      pAudio->iAudioNumber = audioList.count ( ) + 1;
      audioList.append ( pAudio );
    }
  }

  if ( ! pManual )  {
    pManual = new Manual;
    pManual->qsPath = qsPath;
    pManual->iHashValue = iPathHash;
    DialogFiles::m_mapOfManualChange[iPathHash] = pManual;
  }
  if ( ! pChange )  {
    pChange = new Manual::Change;
    pChange->qsFileName = qsVideoFileName;
    pChange->iHashValue = iFileHash;
    pChange->copyAudioList    (    audioList );
    pChange->copySubtitleList ( subtitleList );
    pManual->mapOfChange[iFileHash] =  pChange;
  }

  return true;
}

void DialogFiles::Thumbs::toggleAudioTrack ( Audio *pAudio )
{
  // This function will toggle the Audio entry in mapOfManualChange
  bool bFound = false;
  unsigned long long iPathHash, iFileHash;
  Manual         *pManual = NULL;
  Manual::Change *pChange = NULL;
  getManualChange ( pManual, pChange, iPathHash, iFileHash );

  if ( pChange )  {
    Audio *pAudio2;
    QList<Audio *>::iterator it = pChange->audioList.begin ( );		//oxx
    while ( it != pChange->audioList.end ( ) )  {
      pAudio2 = *it++;
      if ( pAudio2->qsFileName == pAudio->qsFileName )  {
        //pChange->audioList.remove ( pAudio2 );    //ooo
        pChange->audioList.removeOne ( pAudio2 );   //xxx
        delete pAudio2;
        bFound = true;
      }
    }
    if ( ! bFound )  {
       pAudio2 = new Audio;
      *pAudio2 = *pAudio;
       pChange->audioList.append ( pAudio2 );
    }
  }
}

void DialogFiles::Thumbs::toggleSubtitleTrack ( SubtitleEntry *pSubtitle )
{
  bool bFound = false;
  unsigned long long iPathHash, iFileHash;
  Manual         *pManual = NULL;
  Manual::Change *pChange = NULL;
  getManualChange ( pManual, pChange, iPathHash, iFileHash );

  if ( pChange )  {
    SubtitleEntry *pSubtitle2;
    QList<SubtitleEntry *>::iterator it = pChange->subtitleList.begin ( );	//oxx
    while ( it != pChange->subtitleList.end ( ) )  {
      pSubtitle2 = *it++;
      if ( pSubtitle2->m_qsFileName == pSubtitle->m_qsFileName )  {
        //pChange->subtitleList.remove ( pSubtitle2 );  //ooo
        pChange->subtitleList.removeOne ( pSubtitle2 ); //xxx
        delete pSubtitle2;
        bFound = true;
      }
    }
    if ( ! bFound )  {
       pSubtitle2 = new SubtitleEntry;
      *pSubtitle2 = *pSubtitle;
       pChange->subtitleList.append ( pSubtitle2 );
    }
  }
}

bool DialogFiles::Thumbs::audioTrackEnabled ( QString &qsAudioFile )
{
  // This function will check the mapOfManualChange to see if the audio file is part of it.
  // Note: once you loose the Thumbs object, the disabled tracks will no longer be displayed
  //       and you will have to refresh the tracks
  Manual::Change *pChange = NULL;
  getManualChange ( pChange );
  if ( pChange )  {
    Audio *pAudio;
    QList<Audio *>::iterator it = pChange->audioList.begin ( );		//oxx
    while ( it != pChange->audioList.end ( ) )  {
      pAudio = *it++;
      if ( pAudio->qsFileName == qsAudioFile )
        return true;
    }
  }
  else
    return true;

  return false;
}

bool DialogFiles::Thumbs::subtitleTrackEnabled ( QString &qsSubtitleFile )
{
  Manual::Change   *pChange = NULL;
  getManualChange ( pChange );
  if ( pChange )  {
    SubtitleEntry *pSubtitle;
    QList<SubtitleEntry *>::iterator it = pChange->subtitleList.begin ( );	//oxx
    while ( it != pChange->subtitleList.end ( ) )  {
      pSubtitle = *it++;
      if ( pSubtitle->m_qsFileName == qsSubtitleFile )
        return true;
    }
  }
  else
    return true;

  return false;
}

bool DialogFiles::Thumbs::getManualChange ( DialogFiles::Manual::Change *&pChange )
{
  DialogFiles::Manual *pManual = NULL;
  unsigned long long iHash1, iHash2;

  return getManualChange ( pManual, pChange, iHash1, iHash2 );
}

bool DialogFiles::Thumbs::getManualChange ( DialogFiles::Manual *&pManual, DialogFiles::Manual::Change *&pChange, unsigned long long &iPathHash, unsigned long long &iFileHash )
{
  QFileInfo fileInfo;
  QString qsPath, qsFileName;
  pManual   = NULL;
  pChange   = NULL;
  iPathHash =  0LL;
  iFileHash =  0LL;

  if ( pSourceFileInfo )
    qsFileName = pSourceFileInfo->qsFileName;
  else if ( pCache )
    qsFileName = pCache->qsFileName;
  else return false;

  fileInfo.setFile ( qsFileName );
  //qsPath = fileInfo.dirPath ( TRUE ); //ooo
  qsPath = fileInfo.path ( );           //xxx

  iPathHash = Global::pThumbsCache->hashFromPath ( qsPath );
  iFileHash = Global::pThumbsCache->hashFromFile ( qsFileName );

  pManual   = DialogFiles::m_mapOfManualChange[iPathHash];
  if ( pManual )  {
    pChange   = pManual->mapOfChange[iFileHash];
    return true;
  }
  return false;
}

/****************************************************************
 **
 ** The actual Dialog starts here.
 **
 ****************************************************************/
//DialogFiles::DialogFiles ( QWidget *pParent, bool bIsImageDialog, const char *pName, Qt::WFlags f )	//ooo
DialogFiles::DialogFiles ( QWidget *pParent, bool bIsImageDialog )					//xxx
  //: uiDialogFiles ( pParent, pName, f )	//ooo
  : QDialog ( pParent )				//xxx
{
  setupUi(this);	//xxx
  m_bIsImageDialog = bIsImageDialog;
  m_pIAmAlive = this;
}

DialogFiles::~DialogFiles ( )
{
  m_pIAmAlive = NULL;
  lockDialog ( );

  // First we should remove outstanding tasks ...
  MediaCreator::unregisterFromMediaScanner ( this );

  if ( m_pVirtualFolderDialog )
    delete m_pVirtualFolderDialog;
  if ( m_pTimerAutoOpen )
    delete m_pTimerAutoOpen;
  if ( m_pTimerScanDir )
    delete m_pTimerScanDir;
  if ( m_pTimerThumbing )
    delete m_pTimerThumbing;
  if ( m_pPreviewer )
       delete m_pPreviewer;

  m_pVirtualFolderDialog = NULL;
  m_pTimerAutoOpen       = NULL;
  m_pTimerScanDir        = NULL;
  m_pTimerThumbing       = NULL;
  m_pPreviewer           = NULL;

  uint t;
  for ( t=0; t<(uint)m_listOfThumbs.count ( ); t++ )	//ox
    delete m_listOfThumbs[t];
  m_listOfThumbs.clear ( );

  for ( t=0; t<(uint)m_listOfClones.size ( ); t++ )	//ox
    delete m_listOfClones[t];
  m_listOfClones.clear ( );

  unlockDialog ( );
}

bool DialogFiles::isMyself ( DialogFiles *pDialog )
{
  return ( DialogFiles::m_pIAmAlive == pDialog );
}

void DialogFiles::initMe ( )
{  
  m_pVirtualFolderDialog = NULL;
  m_pActiveQuickButton   = NULL;
  m_pActiveThumbs        = NULL;
  m_pTimerAutoOpen       = NULL;
  m_pTimerThumbing       = NULL;
  m_pTimerScanDir        = NULL;
  m_pContextMenu         = NULL;
  m_pToolbox             = NULL;
  m_pScrollView          = NULL;
  m_bCanClose            = true;
  m_bCanChangeIcons      = true;
  m_bTooSmallForText     = ( m_iThumbnailSize < 51 );
  m_iProgress            = 0;
  m_iMultiThumbNumber    = 10; // Number of thumbs per video
  m_bSortAscending       = true;
  m_iSortType            = 0;
  m_iVidCounter          = 0;

  m_pPreviewer = new Previewer ( m_pFramePreview, m_pPreview );
  
  //(this->layout ())->addChildWidget ( m_pPreviewer );  //xxx

  // Next we create the scrollView and the ToolBox for th groups.
  createScrollView ( );
  
  QFont theFont        ( QApplication::font ( ) );
  //theFont.setBold      ( TRUE );  //ooo
  theFont.setBold      ( true );    //xxx
  theFont.setPointSize ( 8 );
  m_pPreview->setFont  ( theFont );

  m_pCheckStars ->setChecked ( m_bStars  );
  m_pCheckName  ->setChecked ( m_bName   );
  m_pCheckDate  ->setChecked ( m_bDate   );
  m_pCheckLength->setChecked ( m_bLength );
  
  //m_pListViewVirtual->setAllColumnsShowFocus ( true );    //oooo
  //m_pToolbox->removeItem ( m_pToolbox->item  ( 0  ) );    //ooo
  m_pToolbox->removeItem ( 0 );                             //xxx

  initMapOfChange    ( );

  initDirectories    ( );

  initVirtualFolders ( ); //ooo

  initGroups         ( ); //ooo

  m_pButtonReload->setEnabled ( false );

  // And finally we connect the other 'stuff' 
  connect ( m_pButtonOk,     SIGNAL ( clicked ( ) ), this, SLOT ( accept     ( ) ) );     //ooo
  //connect ( m_pButtonOk,     SIGNAL ( clicked ( ) ), this, SLOT ( slotAccept ( ) ) );       //xxx
  connect ( m_pButtonCancel, SIGNAL ( clicked ( ) ), this, SLOT ( reject     ( ) ) );
  connect ( m_pButtonPlay,   SIGNAL ( clicked ( ) ), this, SLOT ( slotPlay   ( ) ) );
  connect ( m_pButtonRefresh,SIGNAL ( clicked ( ) ), this, SLOT ( slotRefresh( ) ) );
  connect ( m_pButtonReload, SIGNAL ( clicked ( ) ), this, SLOT ( slotReload ( ) ) );

  connect ( m_pButtonAutoGroup, SIGNAL ( clicked (   ) ), this, SLOT ( slotAutoGroup   ( ) ) );
  connect ( m_pComboSort,    SIGNAL ( activated  (int) ), this, SLOT ( slotSortBy (  int ) ) );
  connect ( m_pCheckStars,   SIGNAL ( toggled ( bool ) ), this, SLOT ( slotAttrib ( bool ) ) );
  connect ( m_pCheckName,    SIGNAL ( toggled ( bool ) ), this, SLOT ( slotAttrib ( bool ) ) );
  connect ( m_pCheckDate,    SIGNAL ( toggled ( bool ) ), this, SLOT ( slotAttrib ( bool ) ) );
  connect ( m_pCheckLength,  SIGNAL ( toggled ( bool ) ), this, SLOT ( slotAttrib ( bool ) ) );

  //connect ( m_pPreview, SIGNAL ( mouseButtonClicked ( int, Q3IconViewItem *, const QPoint & ) ),  //ooo
  //connect ( m_pPreview, SIGNAL ( itemClicked ( QTreeWidgetItem *, int ) ),                          //xxx
                //this,   SLOT ( slotPreviewClicked ( int, Q3IconViewItem *, const QPoint & ) ) );  //ooo
                //this,   SLOT ( slotPreviewClicked ( QTreeWidgetItem *, int ) ) );                   //xxx
  //connect ( m_pPreview, SIGNAL ( rightButtonPressed ( Q3IconViewItem *,      const QPoint & ) ),  //ooo
  connect ( m_pPreview, SIGNAL ( itemActivated ( QListWidgetItem * ) ),  //xxx
                //this,   SLOT ( slotPreviewPressed ( Q3IconViewItem *,      const QPoint & ) ) );  //ooo
                this,   SLOT ( slotPreviewPressed ( QListWidgetItem * ) ) );    //xxx
  connect ( m_pPreview, SIGNAL ( dropped ( QDropEvent *, const QList<Q3IconDragItem> & ) ),			//oxx
                this,   SLOT ( slotDroppedOnPreview ( QDropEvent *, const QList<Q3IconDragItem> & ) ) );	//oxx
  //connect ( m_pPreview, SIGNAL ( selectionChanged ( ) ), this, SLOT ( slotIconSelectionChanged ( ) ) );   //xxx
  //connect ( m_pPreview, SIGNAL ( itemClicked ( QTreeWidgetItem *, int ) ), this, SLOT ( slotIconSelectionChanged ( QTreeWidgetItem *, int ) ) );  //xxx

  m_pGroupTab->resize ( 260, m_pGroupTab->height   ( ) );
}

void DialogFiles::createScrollView ( )
{
  //Q3GridLayout *pLayout = new Q3GridLayout ( m_pToolFrame, 1, 1, 0, 6, "m_pLayout1"); //ooo
  QGridLayout *pLayout = new QGridLayout ( m_pToolFrame );                              //xxx
  //uint iFlags = Qt::WStaticContents;			//ooo
  //Qt::WindowFlags iFlags = Qt::WStaticContents;		//xxx
  Qt::WindowFlags iFlags = windowFlags();		//xxx
//#if (QT_VERSION > 0x0301FF)		//ooo
//  iFlags |= Qt::WNoAutoErase;
//#else
//  iFlags |= Qt::WPaintClever;
//#endif
  //iFlags |= Qt::WNoAutoErase;		//xxx
  //iFlags |= QWidget::setAttribute(Qt::WA_NoBackground);		//xxx
  //m_pScrollView = new Q3ScrollView     ( m_pToolFrame, "DFToolFRame", iFlags );   //ooo
  m_pScrollView = new QScrollArea     ( m_pToolFrame );                             //xxx
  //m_pScrollView->setWidgetResizable( true );				//xxx
  //m_pScrollView->setSizeAdjustPolicy ( QAbstractScrollArea::AdjustToContents );   //xxx
  //m_pScrollView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );    //xxx
  //m_pToolbox    = new QToolBox        ( m_pScrollView->viewport ( ), "DFScrollView", iFlags );    //ooo
  m_pToolbox    = new QToolBox        ( m_pScrollView->viewport ( ) );                              //xxx
  //m_pToolbox    = new QToolBox        ( m_pScrollView->widget ( ) );                              //xxx
  //m_pScrollView->addChild             ( m_pToolbox ); //ooo
  //m_pScrollView->setWidget             ( m_pToolbox );  //xxx
  //m_pScrollView->setVScrollBarMode    ( Q3ScrollView::AlwaysOn ); //oooo
  //m_pScrollView->setWidgetResizable( true );				//xxx
  //m_pScrollView->setSizeAdjustPolicy ( QAbstractScrollArea::AdjustToContents );   //xxx
  //m_pScrollView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );    //xxx
  m_pScrollView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );    //xxx
  pLayout->addWidget ( m_pScrollView, 0, 0 ); //ooo
  QTimer::singleShot ( 10, this, SLOT ( slotResizeScrollView ( ) ) ); //oooo
}

void DialogFiles::slotResizeScrollView ( )
{
  int iWidth  = m_pGroupTab->width  ( ) -  38;
  int iHeight = m_pGroupTab->height ( ) - 100;
  int iCount  = m_pToolbox->count   ( );
  if ( iWidth  <  50 )
       iWidth  = 200;
  if ( iHeight <  50 )
       iHeight = 200;
  if ( iHeight < iCount * 30 + 130 )
       iHeight = iCount * 30 + 130;
  if ( ( m_pToolbox->width ( )  !=  iWidth ) || ( m_pToolbox->height ( ) != iHeight ) )
         m_pToolbox->setFixedSize ( iWidth, iHeight );
  // Ugly workaround to refresh to the proper extensions
  // Why don't we have resize signals anywhere ?
  QTimer::singleShot ( 1500, this, SLOT ( slotResizeScrollView ( ) ) );
}

void DialogFiles::initMapOfChange ( )
{
  // Here we set the entries in the map to the current SourceFileEntry's settings
  uint t, i, iCount = Global::pApp->sourceFileCount ( );
  unsigned long long iPathHash, iFileHash;
  QList<SourceFileInfo *>::iterator it;		//oxx
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;
  Manual::Change  *pChange   = NULL;
  Manual          *pManual   = NULL;
  SubtitleEntry   *pSubtitle = NULL;
  Audio           *pAudio    = NULL;
  Thumbs thumbs;

  for ( t=0; t<iCount; t++ )  {
    pEntry = Global::pApp->sourceFileEntry ( t );
    it = pEntry->listFileInfos.begin ( );
    while ( it != pEntry->listFileInfos.end ( ) )  {
      pInfo = *it++;
      if ( ( pInfo->audioCount ( ) < 1 ) && ( pInfo->subtitleCount ( ) < 1 ) )
        continue;

      thumbs.pSourceFileInfo = pInfo;
      thumbs.getManualChange ( pManual, pChange, iPathHash, iFileHash );
      if ( ! pManual )  {
        QFileInfo fileInfo   ( pInfo->qsFileName );
        //QString qsPath = fileInfo.dirPath ( TRUE );   //ooo
        QString qsPath = fileInfo.path ( );             //xxx
        pManual = new Manual;
        pManual->qsPath     = qsPath;
        pManual->iHashValue = iPathHash;
        DialogFiles::m_mapOfManualChange[iPathHash] = pManual;
      }
      if ( ! pChange )  {
        pChange = new Manual::Change;
        pChange->qsFileName = pInfo->qsFileName;
        pChange->iHashValue = iFileHash;
        pManual->mapOfChange[iFileHash] = pChange;
      }

      if ( pInfo->audioCount ( ) > 0 )  {
        pChange->clearAudioList    ( );
        for ( i=0; i<MAX_AUDIO;  i++ )  {
          if (  pInfo->arrayAudio[i] )  {
            pAudio = new Audio;
           *pAudio = *pInfo->arrayAudio[i];
            pChange->audioList.append ( pAudio );
          }
        }
      }
      if ( pInfo->subtitleCount  ( ) > 0 )  {
        pChange->clearSubtitleList     ( );
        for ( i=0; i<MAX_SUBTITLES;  i++ )  {
          if (  pInfo->arraySubtitles[i] )  {
            pSubtitle = new SubtitleEntry;
           *pSubtitle = *pInfo->arraySubtitles[i];
            pChange->subtitleList.append ( pSubtitle );
          }
        }
      }
    }
  }
}

void DialogFiles::initDirectories( )
{
  QIcon::setThemeName("oxygen");  //xxx
    
  // Initialize the Quick buttons
  //m_pButtonProject  ->setPixmap( qPixmapFromMimeSource ( "project.png" ));	//ooo
  //m_pButtonProject  ->setIcon( QPixmap ( ":/images/project.png" ));   		//xxx
  m_pButtonProject  ->setIcon( QIcon::fromTheme ( "continue-data-project" ));	//xxx
  //m_pButtonDocuments->setPixmap( qPixmapFromMimeSource ( "documents.png" ));	//ooo
  //m_pButtonDocuments->setIcon( QPixmap ( ":/images/documents.png" )); 		//xxx
  m_pButtonDocuments->setIcon( QIcon::fromTheme ( "folder-documents" ));		//xxx
  //m_pButtonDesktop  ->setPixmap( qPixmapFromMimeSource ( "desktop.png" ));	//ooo
  //m_pButtonDesktop  ->setIcon( QPixmap ( ":/images/desktop.png" ));   		//xxx
  //m_pButtonDesktop  ->setIcon( QPixmap ( ":/images/desktop.png" ));   		//xxx
  m_pButtonDesktop  ->setIcon( QIcon::fromTheme ( "user-desktop" ));       		//xxx
  //m_pButtonHome     ->setPixmap( qPixmapFromMimeSource ( "home.png" ));   	//ooo
  //m_pButtonHome     ->setIcon( QPixmap ( ":/images/home.png" ));        		//xxx
  m_pButtonHome     ->setIcon( QIcon::fromTheme ( "user-home" ));          		//xxx
  //m_pButtonRoot     ->setPixmap( qPixmapFromMimeSource ( "root.png" ));   	//ooo
  //m_pButtonRoot     ->setIcon( QIcon ( ":/images/root.png" ));        		//xxx
  m_pButtonRoot     ->setIcon( QIcon::fromTheme ( "run-build-install-root" ));	//xxx
  //m_pButtonTemp     ->setPixmap( qPixmapFromMimeSource ( "temp.png" ));   	//ooo
  //m_pButtonTemp     ->setIcon( QPixmap ( ":/images/temp.png" ));        		//xxx
  m_pButtonTemp     ->setIcon( QIcon::fromTheme ( "folder-temp" ));      		//xxx

  m_qsProjectDir   = Global::qsProjectPath; //QDir::currentDirPath() + QString ("/");
  //m_qsHomeDir      = QDir::homeDirPath() + QString ("/"); //ooo
  m_qsHomeDir      = QDir::homePath() + QString ("/");      //xxx
  //m_qsRootDir      = QDir::rootDirPath(); //ooo
  m_qsRootDir      = QDir::rootPath();      //xxx
  m_qsDocumentsDir = m_qsHomeDir + QString ("Desktop/Documents/");
  m_qsDesktopDir   = m_qsHomeDir + QString ("Desktop/");
  m_qsTempDir      = Global::qsTempPath;
  if ( m_qsTempDir[m_qsTempDir.length ( )-1] != '/' )
    m_qsTempDir  = m_qsTempDir + "/";
  if ( m_qsProjectDir[m_qsProjectDir.length ( )-1] != '/' )
    m_qsProjectDir  = m_qsProjectDir + "/";

  // Here we refine the Desktop directory ...
  QString qsRCDir = m_qsHomeDir + QString (".kderc");
  QString qsDesktopDir;
  bool bFound = findDesktopDir ( qsRCDir );
  if ( ! bFound )  {
    qsRCDir = QString ("/etc/kderc");
    bFound  = findDesktopDir ( qsRCDir );
  }
  if ( bFound )  {
    // So we found the kde default directory, we should use it
    // instead of my dummy directory (if present).
    QFile file ( Global::qsTempPath + QString ( "/lucas.txt" ) );
    file.open (QIODevice::ReadOnly);
    QTextStream in(&file);	//xxx
    m_qsDesktopDir = QString ();
    QString qsLine;
    //while ( file.readLine ( qsLine, 2048 ) > -1 )	//ooo
      //qsDesktopDir += qsLine;				//ooo
    while (!in.atEnd()) {	//xxx
      qsLine = in.readLine();
      qsDesktopDir += qsLine;
    }    

    file.close ( );
    if ( qsDesktopDir.length ( ) > 1 )
       m_qsDesktopDir = qsDesktopDir;
  }

  // And we should also search for a Documents directory ...
  const char *pArray[5] = {"Documents/", "documents/", "MyDocuments/", "myDocuments/", "mydocuments/" };
  QString qsDocs, qsPreDir = m_qsDesktopDir;
  QDir docDir;
  int t, i;
  // The first time we are looking under the desktop directory for the documents directory.
  for (i=0;i<2;i++)	{
    for (t=0;t<5;t++)	{
      qsDocs = qsPreDir + QString (pArray[t]);
      docDir.setPath(qsDocs);
      if (docDir.exists())	{
	m_qsDocumentsDir = qsDocs;
	t = i = 6; 	// exit both loops ...
      }
    }
    qsPreDir = m_qsHomeDir;	// the second time around we are searching in the Home Directory
  }

  m_pTimerAutoOpen = new QTimer ( this );
  m_pTimerScanDir  = new QTimer ( this );
  m_pTimerThumbing = new QTimer ( this );
  connect ( m_pTimerAutoOpen, SIGNAL( timeout ( ) ), this, SLOT ( slotAutoOpen ( ) ) );
  connect ( m_pTimerScanDir,  SIGNAL( timeout ( ) ), this, SLOT ( slotScanDir  ( ) ) );
  connect ( m_pTimerThumbing, SIGNAL( timeout ( ) ), this, SLOT ( slotThumbing ( ) ) );

  connect ( m_pButtonProject,   SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );
  connect ( m_pButtonDocuments, SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );
  connect ( m_pButtonDesktop,   SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );
  connect ( m_pButtonHome,      SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );
  connect ( m_pButtonRoot,      SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );
  connect ( m_pButtonTemp,      SIGNAL (clicked ()), this, SLOT (slotQuickDir()) );

  // next we take care of the Tree control
  //m_pTreeList->setTreeStepSize ( 20 );                                //oooo
  //m_pTreeList->setSingleStep ( 20 );                                    //xxx
  //m_pTreeList->setAcceptDrops ( TRUE );                               //ooo
  m_pTreeList->setAcceptDrops ( true );                                 //xxx
  //m_pTreeList->setSelectionMode( Q3ListView::Single );                //ooo
  m_pTreeList->setSelectionMode( QAbstractItemView::SingleSelection );  //xxx
  //m_pTreeList->setAllColumnsShowFocus       ( TRUE );                 //ooo
  m_pTreeList->setAllColumnsShowFocus       ( true );                   //xxx
  //m_pTreeList->viewport ( )->setAcceptDrops ( TRUE );                 //ooo
  m_pTreeList->viewport ( )->setAcceptDrops ( true );                   //xxx

  QString qsCurrentPath = Global::qsCurrentPath;

  //const QFileInfoList *pRoots = QDir::drives ( );	//ooo
  const QFileInfoList pRoots = QDir::drives ( );	//xxx
  //Q3PtrListIterator<QFileInfo> it(*pRoots);		//ooo
  QListIterator<QFileInfo> it(pRoots);			//xxx
  DirItem   *pItem;
  //QFileInfo *pFileInfo = it;	//ooo
  //while ( pFileInfo ) {	//ooo

  while ( it.hasNext() ) {  	//xxx
    QFileInfo fileInfo = it.next();
    //pItem = new DirItem ( m_pTreeList, pFileInfo->absFilePath ( ) );	//ooo
    pItem = new DirItem ( m_pTreeList, fileInfo.absoluteFilePath ( ) );	//xxx
    //if ( pRoots->count ( ) <= 1 )	//ooo
    if ( pRoots.count   ( ) <= 1 )	//xxx
      //pItem->setOpen( TRUE );     //ooo
      pItem->setOpen( true );       //xxx

    //pItem->setDragEnabled ( TRUE );   //oooo
    //pItem->setDropEnabled ( true );     //xxx
    //pFileInfo = ++it;	//ooo
    //++it;   //xxx
  }
 
  setCurrentDir      ( Global::qsCurrentPath );

  m_pEditSize->setText    ( QString (  "%1"  ).arg ( m_iThumbnailSize ) );

  m_pSliderSize->setValue ( m_iThumbnailSize );

  //connect ( m_pTreeList,   SIGNAL ( currentChanged(Q3ListViewItem *) ), this, SLOT( slotDirectoryChanged(Q3ListViewItem *) ) );   //ooo
  connect ( m_pTreeList,   SIGNAL ( itemClicked ( QTreeWidgetItem *, int ) ), this, SLOT( slotDirectoryChanged( QTreeWidgetItem *, int ) ) );
  connect ( m_pSliderSize, SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotSizeChanged   ( int ) ) );
  connect ( m_pSliderSize, SIGNAL ( sliderReleased   ( ) ), this, SLOT ( slotSizeReleased  ( ) ) );
}

void DialogFiles::initVirtualFolders ( )
{  
  uint t;
  int iWidth, iHeight;

  QPixmap thePixmap, tempPixmap;
  //QImage  theImage,  emptyImage ( 64, 64, 32 );                   //ooo
  QImage  theImage,  emptyImage ( 64, 64, QImage::Format_RGB32 );   //xxx
  //QImage  scaledStar, theStar = QImage::fromMimeSource ( "star.png" );	//ooo
  QImage  scaledStar, theStar = QImage ( ":/images/star.png" );			//xxx

  // because Qt is sharing QImage resources, the required empty image has to be done this way.
  //emptyImage.setAlphaBuffer ( FALSE );    //oooo
  emptyImage.fill ( 0x00000000 );
  //emptyImage.setAlphaBuffer ( TRUE  );    //oooo
  emptyImage.fill ( 0xFFFFFFFF );
  
  QPainter p;                          //xxx

  //scaledStar = theStar.smoothScale ( 40, 40, Qt::KeepAspectRatio );				//ooo
  scaledStar = theStar.scaled ( 40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
  theImage   = emptyImage.copy ( );
  //bitBlt ( &theImage, 12, 12,  &scaledStar );     //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( 12, 12, scaledStar );			//xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );
  //m_pButtonStar1->setPixmap  ( thePixmap );       //ooo
  m_pButtonStar1->setIcon  ( thePixmap );           //xxx
  
  // Next we need to scale ...
  iWidth   = (int)( emptyImage.width  ( ) / 2.0 );
  iHeight  = (int)( emptyImage.height ( ) / 2.0 );
  //scaledStar = theStar.smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio );				//ooo
  scaledStar = theStar.scaled ( iWidth, iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx

  // Here we create the two Stars-Button
  theImage = emptyImage.copy ( );
  //bitBlt ( &theImage,      0,       0,  &scaledStar );    //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( 0, 0, scaledStar );			                //xxx
  //bitBlt ( &theImage, iWidth, iHeight,  &scaledStar );    //ooo
  p.drawImage( iWidth, iHeight, scaledStar );		        //xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );

  //m_pButtonStar2->setPixmap  ( thePixmap );   //ooo
  m_pButtonStar2->setIcon  ( thePixmap );       //xxx

  // And since we have this in place we can also create the 4 Star-Button right next
  //bitBlt ( &theImage, iWidth,       0,  &scaledStar );    //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( iWidth, 0, scaledStar );			            //xxx
  //bitBlt ( &theImage,      0, iHeight,  &scaledStar );    //ooo
  p.drawImage( 0, iHeight, scaledStar );			        //xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );

  //m_pButtonStar4->setPixmap  ( thePixmap );   //ooo
  m_pButtonStar4->setIcon  ( thePixmap );       //xxx

  // Next we will create the 3 Star-Button
  theImage = emptyImage.copy ( );
  //bitBlt ( &theImage, (int)(iWidth/2.0), 0, &scaledStar );    //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( (int)(iWidth/2.0), 0, scaledStar );			    //xxx
  //bitBlt ( &theImage,      0, iHeight, &scaledStar );         //ooo
  p.drawImage( 0, iHeight, scaledStar );			            //xxx
  //bitBlt ( &theImage, iWidth, iHeight, &scaledStar );         //ooo
  p.drawImage( iWidth, iHeight, scaledStar );			        //xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );

  //m_pButtonStar3->setPixmap  ( thePixmap );   //ooo
  m_pButtonStar3->setIcon  ( thePixmap );       //xxx

  // Time to scale again
  iWidth   = (int)( emptyImage.width  ( ) / 3.0 );
  iHeight  = (int)( emptyImage.height ( ) / 3.0 );
  //scaledStar = theStar.smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio );				//ooo
  scaledStar = theStar.scaled ( iWidth, iHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx

  // Next on the todo list is the 6 Star-Button
  theImage = emptyImage.copy ( );
  //bitBlt ( &theImage,        0,         0, &scaledStar );     //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( 0, 0, scaledStar );			                    //xxx
  //bitBlt ( &theImage,   iWidth,         0, &scaledStar );     //ooo
  p.drawImage( iWidth, 0, scaledStar );			            //xxx
  //bitBlt ( &theImage, 2*iWidth,         0, &scaledStar );     //ooo
  p.drawImage( 2*iWidth, 0, scaledStar );			            //xxx
  //bitBlt ( &theImage,        0, 2*iHeight, &scaledStar );     //ooo
  p.drawImage( 0, 2*iHeight, scaledStar );			        //xxx
  //bitBlt ( &theImage,   iWidth, 2*iHeight, &scaledStar );     //ooo
  p.drawImage( iWidth, 2*iHeight, scaledStar );			    //xxx
  //bitBlt ( &theImage, 2*iWidth, 2*iHeight, &scaledStar );     //ooo
  p.drawImage( 2*iWidth, 2*iHeight, scaledStar );			    //xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );

  //m_pButtonStar6->setPixmap  ( thePixmap );   //ooo
  m_pButtonStar6->setIcon  ( thePixmap );       //xxx

  // Last we create the 5 Star-Button
  theImage = emptyImage.copy ( );
  //bitBlt ( &theImage,        0,         0, &scaledStar );     //ooo
  p.begin(&theImage);   //xxx
  p.drawImage( 0, 0, scaledStar );			                //xxx
  //bitBlt ( &theImage, 2*iWidth,         0, &scaledStar );     //ooo
  p.drawImage( 2*iWidth, 0, scaledStar );			            //xxx
  //bitBlt ( &theImage,        0, 2*iHeight, &scaledStar );     //ooo
  p.drawImage( 0, 2*iHeight, scaledStar );			        //xxx
  //bitBlt ( &theImage, 2*iWidth, 2*iHeight, &scaledStar );     //ooo
  p.drawImage( 2*iWidth, 2*iHeight, scaledStar );			    //xxx
  //bitBlt ( &theImage,   iWidth,   iHeight, &scaledStar );     //ooo
  p.drawImage( iWidth, iHeight, scaledStar );			        //xxx
  p.end();  //xxx
  thePixmap.convertFromImage (  theImage );

  //m_pButtonStar5->setPixmap  ( thePixmap );   //ooo
  m_pButtonStar5->setIcon  ( thePixmap );       //xxx

  // Next we will create the pixmaps for the context menu
  iWidth = 20;
  //emptyImage = emptyImage.scale    ( 6*iWidth, iWidth );	//ooo
  emptyImage = emptyImage.scaled    ( 6*iWidth, iWidth );	//xxx
  //scaledStar = theStar.smoothScale (   iWidth, iWidth, Qt::KeepAspectRatio );		//ooo
  scaledStar = theStar.scaled (   iWidth, iWidth, Qt::KeepAspectRatio );		//xxx
  theImage   = emptyImage.copy ( );
  for ( t=0; t<6; t++ ) {
    //bitBlt ( &theImage, t*iWidth, 0,  &scaledStar );      //ooo
    p.begin(&theImage);   //xxx
    p.drawImage( t*iWidth, 0, scaledStar );			        //xxx
    p.end();  //xxx
    m_pixMenuStars[t].convertFromImage ( theImage );
  }

  // Next we will init the QListViewVirtual
  //m_pListViewVirtual->setSorting ( FALSE );       //ooo
  m_pListViewVirtual->setSortingEnabled ( false );  //xxx
  QString qsDate, qsCount;
  Cache::Thumbs::VirtualFolder *pVFolder = NULL;
  QList<Cache::Thumbs::VirtualFolder *>::iterator it;		//oxx

  QList<Cache::Thumbs::VirtualFolder *>list = Global::pThumbsCache->getVirtualFolders ( isImageDialog ( ) );	//ooo

  QTreeWidgetItem *pItem;   //xxx
  
  it = list.begin ( );
  
  while ( it != list.end ( ) ) {
    pVFolder  = *it++;
    qsCount   = QString ( "%1" ).arg ( pVFolder->listOfFiles.count ( ) );
    qsDate    = pVFolder->dateCreated.toString ( "yyyy/MM/dd hh:mm:ss" );
    //new Q3ListViewItem ( m_pListViewVirtual, pVFolder->qsFolderName, qsCount, qsDate );   //ooo
    pItem = new QTreeWidgetItem ( m_pListViewVirtual );    //xxx
    pItem->setText ( 0, pVFolder->qsFolderName);   //xxx
    pItem->setText ( 1, qsCount);   //xxx
    pItem->setText ( 2, qsDate);   //xxx
  }

  createIconStars ( );

  connect ( m_pButtonStar1, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );
  connect ( m_pButtonStar2, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );
  connect ( m_pButtonStar3, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );
  connect ( m_pButtonStar4, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );
  connect ( m_pButtonStar5, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );
  connect ( m_pButtonStar6, SIGNAL( clicked ( ) ), this, SLOT ( slotSelectByStars ( ) ) );

  connect ( m_pButtonAddVirtual,    SIGNAL ( clicked ( ) ), this, SLOT ( slotAddVirtual    ( ) ) );
  connect ( m_pButtonEditVirtual,   SIGNAL ( clicked ( ) ), this, SLOT ( slotEditVirtual   ( ) ) );
  connect ( m_pButtonDeleteVirtual, SIGNAL ( clicked ( ) ), this, SLOT ( slotDeleteVirtual ( ) ) );
  connect ( m_pListViewVirtual,     SIGNAL ( selectionChanged  ( ) ), this, SLOT( slotVirtualChanged (  ) ) );
  connect ( m_pListViewVirtual,     SIGNAL ( doubleClicked     (Q3ListViewItem *, const QPoint &, int ) ), this, SLOT( slotShowVirtual (Q3ListViewItem *, const QPoint &, int) ) );
  connect ( m_pListViewVirtual,     SIGNAL ( rightButtonClicked(Q3ListViewItem *, const QPoint &, int ) ), this, SLOT( slotVirtualContextMenu (Q3ListViewItem *, const QPoint &, int) ) );
}

void DialogFiles::clearMapOfChange ( )
{
  QMap<unsigned long long, Manual *>::iterator it = m_mapOfManualChange.begin ( );
  while ( it != m_mapOfManualChange.end ( ) )
    delete *it++; //.data ( );
  m_mapOfManualChange.clear  ( );
}

void DialogFiles::createIconStars ( )
{
  uint t, i, iSize, iX, iHalf;
  //QImage  scaledStar, theStar = QImage::fromMimeSource ( "star.png" );	//ooo
  QImage  scaledStar, theStar = QImage ( ":/images/star.png" );			//xxx

  iSize      = m_iThumbnailSize / 6;
  //scaledStar = theStar.smoothScale ( iSize, iSize, Qt::KeepAspectRatio );	//ooo
  scaledStar = theStar.scaled ( iSize, iSize, Qt::KeepAspectRatio );		//xxx

  //QImage theImage, emptyImage ( m_iThumbnailSize+4, iSize+2, 32 );                    //ooo
  QImage theImage, emptyImage ( m_iThumbnailSize+4, iSize+2, QImage::Format_RGB32 );    //xxx
  emptyImage.fill ( 0xFFFFFFFF );

  iHalf = (int)((float)m_iThumbnailSize / 2.0f);
  for ( t=0; t<7; t++ ) {
    theImage = emptyImage.copy ( );
    iX = 2 + iHalf - ( t * ( iSize / 2 ) );
    for ( i=0; i<t; i++  ) {
      //bitBlt ( &theImage, iX, 2, &scaledStar );       //ooo
      QPainter p( &theImage );                          //xxx
      p.drawImage( iX, 2, scaledStar );	        		//xxx
      iX += iSize;
    }
    m_pixIconStars[t].convertFromImage ( theImage );
  }
}

void DialogFiles::initGroups ( )
{
  // Here we init the already existing groups ...
  int t;
  SourceFileEntry *pEntry;

  // Init the Group tab.
  // I do it this way to keep DialogFiles as generic as possible
  // Maybe some future use in other programs ...
  for ( t=0; t<Global::pApp->sourceFileCount ( ); t++ )  {
      pEntry = Global::pApp->sourceFileEntry( t );
      if ( pEntry && !pEntry->bSoundSource )
           addGroup ( pEntry );
  }

  // Next we ensure we have at least one group.
  if ( m_pToolbox->count ( ) < 1 ) {
    QString qsTitle = isImageDialog ( ) ? tr ( "Slide 1" ) : tr ( "First Group" );
    QStringList emptyList;
    addGroup ( qsTitle, emptyList );
  }

  connect ( m_pButtonNew,    SIGNAL ( clicked ( ) ), this, SLOT ( slotAddGroup    ( ) ) );
  connect ( m_pButtonEdit,   SIGNAL ( clicked ( ) ), this, SLOT ( slotEditGroup   ( ) ) );
  connect ( m_pButtonDelete, SIGNAL ( clicked ( ) ), this, SLOT ( slotDeleteGroup ( ) ) );
}

void DialogFiles::slotAddGroup ( )
{
  QStringList emptyList;
  QString qsHeader = tr ( "Enter name for new group." );
  QString qsLabel  = tr ( "Enter Group Name : " );
  if ( isImageDialog ( ) )  {
    qsHeader = tr ( "Enter name for new Slideshow." );
    qsLabel  = tr ( "Enter Slideshow Name : " );
  }
  //QString qsTitle  = QInputDialog::getText ( qsHeader, qsLabel );     //ooo
  QString qsTitle  = QInputDialog::getText ( this, qsHeader, qsLabel ); //xxx
  if ( qsTitle.isEmpty ( ) )
    return;

  GroupView *pGroupView = new GroupView ( m_pToolbox, m_pPreview, this );
  //connect  ( pGroupView, SIGNAL ( contextMenuRequested ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotGroupContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );  //ooo
  connect  ( pGroupView, SIGNAL ( itemActivated ( QListWidgetItem *item ) ), this, SLOT ( slotGroupContextMenu ( QListWidgetItem *item ) ) );    //xxx
  m_pToolbox->addItem ( pGroupView, qsTitle );
  //m_pToolbox->setCurrentItem   ( pGroupView );    //ooo
  m_pToolbox->setCurrentWidget   ( pGroupView );    //xxx
//  QTimer::singleShot ( 10, this, SLOT ( slotResizeScrollView ( ) ) );
}

void DialogFiles::slotEditGroup ( )
{
  int iIdx = m_pToolbox->currentIndex ( );
  //QString qsCurrent = m_pToolbox->itemLabel ( iIdx ); //ooo
  QString qsCurrent = m_pToolbox->itemText ( iIdx );    //xxx
  QString qsHeader  = tr ( "Enter name for current group." );
  QString qsLabel   = tr ( "Enter Group Name : " );
  //QString qsTitle   = QInputDialog::getText ( qsHeader, qsLabel, QLineEdit::Normal, qsCurrent );      //ooo
  QString qsTitle   = QInputDialog::getText ( this, qsHeader, qsLabel, QLineEdit::Normal, qsCurrent );  //xxx
  if ( qsTitle.isEmpty ( ) )
    return;

  //m_pToolbox->setItemLabel ( iIdx, qsTitle ); //ooo
  m_pToolbox->setItemText ( iIdx, qsTitle );    //xxx
}

void DialogFiles::slotDeleteGroup ( )
{
  int      iIdx       = m_pToolbox->currentIndex ( );
  //QString  qsLabel    = m_pToolbox->itemLabel  ( iIdx );  //ooo
  QString  qsLabel    = m_pToolbox->itemText  ( iIdx );     //xxx
  //QWidget *pGroupView = m_pToolbox->item       ( iIdx );  //ooo
  QWidget *pGroupView = m_pToolbox->widget       ( iIdx );  //xxx
  if ( MessageBox::warning ( this, tr ( "Warning Delete Source Group." ),
         tr ( "Are you sure you want to delete the group\n%1" ).arg ( qsLabel ), 
         QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )  {
    //m_pToolbox->removeItem ( pGroupView );                    //ooo
    m_pToolbox->removeItem ( m_pToolbox->indexOf(pGroupView) ); //xxx
    delete pGroupView;
  }
//  QTimer::singleShot ( 10, this, SLOT ( slotResizeScrollView ( ) ) );
}

GroupView *DialogFiles::addGroup ( QString &qsTitle, QStringList &list )
{  
  uint      t;
  bool      bAlternate = true;
  QFileInfo fileInfo;
  QPixmap   thePixmap, errorPixmap;
  Cache::Thumbs::Entry *pCache;
  GroupView::Item *pItem      = NULL;
  GroupView       *pGroupView = new GroupView ( m_pToolbox, m_pPreview, this );
  //QImage theImage ( QImage ( ).fromMimeSource ( "error.jpg" ) );	//ooo
  QImage theImage ( QImage ( ":/images/error.jpg" ) );			//xxx

  //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize );		//ooo
  theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize );		//xxx
  errorPixmap.convertFromImage ( theImage );

  for ( t=0; t<(uint)list.count ( ); t++ ) {		//ox
    pCache = Global::pThumbsCache->find ( list[t], isImageDialog ( ) );
    fileInfo.setFile      ( list[t] );
    bAlternate = ! bAlternate;
    pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), bAlternate );
    if ( pCache )  {
      pItem->pCache = pCache;
      if ( ! pCache->arrayOfThumbs )
             pCache->loadImages ( );
      if ( pCache->arrayOfThumbs && pCache->arrayOfThumbs[0] ) {
           //theImage = pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
	       theImage = pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
           thePixmap.convertFromImage ( theImage );
          //pItem->setPixmap ( 0, thePixmap );  //ooo
          pItem->setIcon ( 0, thePixmap );      //xxx
      }
      else
        //pItem->setPixmap ( 0, errorPixmap );  //ooo
        pItem->setIcon ( 0, errorPixmap );      //xxx
    }
    else
      //pItem->setPixmap ( 0, errorPixmap );    //ooo
      pItem->setIcon ( 0, errorPixmap );        //xxx
  }
  //connect  ( pGroupView, SIGNAL ( contextMenuRequested ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotGroupContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );  //ooo
  connect  ( pGroupView, SIGNAL ( itemActivated ( QListWidgetItem *item ) ), this, SLOT ( slotGroupContextMenu ( QListWidgetItem *item ) ) );    //xxx

  // And finally we can add the listBox to the ToolBox
  m_pToolbox->addItem ( pGroupView, uniqueGroupName ( qsTitle ) );
//  QTimer::singleShot  ( 10, this, SLOT ( slotResizeScrollView ( ) ) );
  return pGroupView;
}

GroupView *DialogFiles::addGroup ( SourceFileEntry *pEntry )
{
  if ( ! pEntry )
    return NULL;

  // This flag is indicating that the slideshow is not yet done rendering.
  if ( pEntry->bIsSlideshow  )
    return NULL;

  int      t;
  bool      bAlternate = true;
  QFileInfo fileInfo;
  QPixmap   thePixmap, errorPixmap;
  SourceFileInfo *pInfo = NULL;
  Cache::Thumbs::Entry *pCache;
  GroupView::Item *pItem      = NULL;
  GroupView       *pGroupView = new GroupView ( m_pToolbox, m_pPreview, this, pEntry );
  //QImage theImage ( QImage ( ).fromMimeSource ( "error.jpg" ) );	//ooo
  QImage theImage ( QImage ( ":/images/error.jpg" ) );			//xxx

  //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize );							//ooo
  theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  errorPixmap.convertFromImage ( theImage );

  for ( t=(int)pEntry->listFileInfos.count ( )-1; t>=0; t-- ) {
    pInfo  = pEntry->listFileInfos[t];
    pCache = Global::pThumbsCache->find ( pInfo->qsFileName, isImageDialog ( ) );
    fileInfo.setFile ( pInfo->qsFileName );
    bAlternate = ! bAlternate;
    pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), bAlternate );
    pItem->pSourceFileInfo = pInfo;
    if ( pCache )  {
      pItem->pCache = pCache;
      if ( ! pCache->arrayOfThumbs )
        pCache->loadImages ( );
      if ( pCache->arrayOfThumbs && pCache->arrayOfThumbs[0] ) {
        //theImage = pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	theImage = pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
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
  //connect  ( pGroupView, SIGNAL ( contextMenuRequested ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotGroupContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );  //ooo
  connect  ( pGroupView, SIGNAL ( itemActivated ( QListWidgetItem *item ) ), this, SLOT ( slotGroupContextMenu ( QListWidgetItem *item ) ) );   //xxx

  // And finally we can add the listBox to the ToolBox
  m_pToolbox->addItem ( pGroupView, uniqueGroupName ( pEntry->qsDisplayName ) );
//  QTimer::singleShot  ( 10, this, SLOT ( slotResizeScrollView ( ) ) );
  return pGroupView;
}

QString DialogFiles::uniqueGroupName ( QString qsName )
{
  // This fnction will make sure all Groups hold a uniue name.
  int  t, iCount  = 0;
  QString qsGroup, qsReturn;
  bool     bFound = true;
  QRegExp rx ( ".*_\\d" );

  qsReturn = qsName.remove ( rx );
  while ( bFound ) {
    bFound = false;
    for ( t=0; t<m_pToolbox->count ( ); t++ ) {
      //qsGroup = m_pToolbox->itemLabel ( t );  //ooo
      qsGroup = m_pToolbox->itemText ( t );     //xxx
      if ( qsGroup == qsReturn ) {
	qsReturn    = qsName + QString ( "_%1" ).arg ( ++iCount, 2 );
	bFound      = true; // re-do the while loop
	break; // leave for - loop
      }
    }
  }
  return qsReturn;
}

void DialogFiles::addSourceToGroup ( GroupView *pGroupView )
{
  int t;
  QFileInfo        fileInfo;
  QImage           theImage;
  QPixmap          thePixmap;
  QString          qsMovieFilter;
  QStringList      list;
  GroupView::Item *pItem;
  qsMovieFilter =  getDirFilter ( );
  list          =  FilePreviewDialog::getOpenFileNames ( NULL, Global::qsCurrentPath, QString ("Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));
  if ( list.count ( ) < 1 )
    return;

  Cache::Thumbs::Entry *pEntry = NULL;
  for ( t=(int)list.count ( )-1; t>=0; t-- ) {
    pEntry = Global::pThumbsCache->find ( list[t], isImageDialog ( ) );
    fileInfo.setFile ( list[t] );
    if ( pEntry )  {
      pItem  = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), true );
      if ( ! pEntry->arrayOfThumbs )
             pEntry->loadImages  ( );
      if ( pEntry->getMatrix ( ) )  {
           //theImage = pEntry->arrayOfThumbs[0]->xForm ( *pEntry->getMatrix ( ) );     //ooo
           theImage = pEntry->arrayOfThumbs[0]->transformed ( *pEntry->getMatrix ( ) ); //xxx
           //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
	   theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
      }
      else if ( ( pEntry->fRotate != 0.0 ) && ( pEntry->bIsVideo ) )  {
           QMatrix matrix;
           matrix.rotate ( pEntry->fRotate );
           //theImage = pEntry->arrayOfThumbs[0]->xForm ( matrix );     //ooo
           theImage = pEntry->arrayOfThumbs[0]->transformed ( matrix ); //xxx
           //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
	   theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
      }
      else
        //theImage = pEntry->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
        theImage = pEntry->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );		//xxx
    }
    else {
      // No cache entry exists. So we have to create one.
      pEntry   = Global::pThumbsCache->append ( list[t], pGroupView );
      pItem    = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), true );
      //theImage = QImage ( ).fromMimeSource( "please_wait.jpg" );	//ooo
      theImage = QImage ( ":/images/please_wait.jpg" );			//xxx
      //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
      theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
      m_bCanClose = false;
    }
    thePixmap.convertFromImage ( theImage );
    //pItem->setPixmap ( 0, thePixmap );    //ooo
    pItem->setIcon ( 0, thePixmap );        //xxx
    pItem->pCache = pEntry;
  }
  QTimer::singleShot ( 10, pGroupView, SLOT ( slotAlternateColors ( ) ) );
}

bool DialogFiles::findDesktopDir ( QString qsDir )
{
  // This function takes an input file and searches it for the Desktop=... entry.
  // It will then invoke the shell to write the absolute path into a file in the temp directory.
  bool bReturn = false;
  QFile file ( qsDir );
  if ( file.open ( QIODevice::ReadOnly ) ) {
    //Q3TextStream stream( &file ); //ooo
    QTextStream stream( &file );    //xxx
    QString line;
    while ( !stream.atEnd() ) {
      line = stream.readLine(); // line of text excluding '\n'
      //if (line.find ("Desktop=") != -1)	{     //ooo
      if (line.indexOf ("Desktop=") != -1)	{     //xxx
	// found the coresponding line Desktop=$HOME/KDesktop or similar
	QStringList theList;
	//theList = QStringList::split( QString("="), line );  //ooo
    theList = line.split( QString("=") );                  //xxx 
	if ( theList.count() > 1 )  {
	  QString qsDesktop = theList[1];
	  if ( qsDesktop[ qsDesktop.length ( )-1 ] != '/' )
	       qsDesktop = qsDesktop + "/";
	  // here we get the complete path to the Desktop ...
	  QString command = QString ("echo -n ") + qsDesktop + QString ( " > " ) + Global::qsTempPath + QString ("/lucas.txt");
	  //if ( system (command) == -1 )                  //ooo
      if ( system (command.toLatin1().data()) == -1 )  //xxx
	    bReturn = false;
	  bReturn = true;
	}
      }
    }
    file.close();
  }
  return bReturn;
}

void DialogFiles::timerEvent ( QTimerEvent *pEvent )
{  
  // Called from the worker threads after a thumbnail has been generated.
  if ( pEvent->timerId ( ) == MEDIASCANNER_EVENT + 4 ) {
    int t;
    QPixmap  thePixmap;
    WorkWaiting *pWork;
    QList<WorkWaiting *> tempList;	//oxx
    // to decrease threading issues, we quickly remove the used Work objects from the member list.
    lock ( );
    for ( t=0;t<(int)m_listOfWorkToDo.count ( ); t++ ) {
      pWork = m_listOfWorkToDo [ t ];
      //m_listOfWorkToDo.remove( pWork );   //ooo
      m_listOfWorkToDo.removeOne( pWork );  //xxx
      tempList.append ( pWork );
    }
    unlock ( );
    for ( t=0;t<(int)tempList.count ( ); t++ ) {
      pWork = tempList [ t ];
      if ( pWork->pItem && pWork->pImage ) {
	// We have to also make sure the item is still valid.
//        if ( ! lockDialog ( ) )
//          return;
        
	//Q3IconViewItem *pValid = m_pPreview->firstItem ( );  //ooo
    QListWidgetItem *pValid = m_pPreview->item ( 0 );   //xxx 
	while ( pValid &&  m_pIAmAlive ) {    
	  if  ( pWork->pItem == pValid ) {
	    createPix (  &thePixmap, pWork->pImage );
	    //pWork->pItem->setPixmap    ( thePixmap );    //ooo
        pWork->pItem->setIcon    ( thePixmap );        //xxx    
	    if ( pWork->bMarkIcon )
	      markIcon ( pWork->pItem, 1 );

	    break;
	  }
	  //pValid = pValid->nextItem ( );                     //ooo
	  pValid = pValid->listWidget()->item ( pValid->listWidget()->row(pValid) + 1 ); //xxx
	}
//	unlockDialog ( );
      }
      else if ( pWork->pItem ) {
	// Here we return from ScanDVDate and want to update the item
//        if ( ! lockDialog ( ) )
//          return;
        
	Thumbs   *pThumbs;
	QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
	while ( it != m_listOfThumbs.end ( ) && m_pIAmAlive ) {
	  pThumbs = *it++;
	  //if ( getKey ( pThumbs->pOwnerItem ) == getKey ( pWork->pItem ) ) {	//ooo
	  if ( pThumbs->pOwnerItem->text ( ) == pWork->pItem->text ( ) ) {	//xxx
	    setIconText ( pThumbs ); // Update Icon with new dateInfo
	    //pThumbs->pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[0] );  //ooo
        pThumbs->pOwnerItem->setIcon ( *pThumbs->arrayOfPix[0] );   //xxx 
	    break;
	  }
	}
//	unlockDialog ( );
      }

      delete pWork;
      //m_pProgressBar->setProgress ( m_iProgress ++ ); //ooo
      m_pProgressBar->setValue ( m_iProgress ++ );      //xxx
    }
    //if ( (int)m_iProgress >= m_pProgressBar->totalSteps ( )-2 )                           //ooo
    //if ( (int)m_iProgress >= (m_pProgressBar->maximum() - m_pProgressBar->minimum())-2 )    //xxx
      //m_pPreview->arrangeItemsInGrid ( TRUE );    //oooo

    ScanDVDate *pScan = NULL;
    for ( t=0; t<(int)m_listOfDVScansToDo.count ( ); t++ ) {
      pScan = m_listOfDVScansToDo[t];
      if ( pScan->hasExited ( ) ) {
	//m_listOfDVScansToDo.remove ( pScan );    //ooo
    m_listOfDVScansToDo.removeOne ( pScan );   //xxx 
	delete pScan;
      }
    }
  }
  else if ( pEvent->timerId ( ) == MEDIASCANNER_EVENT + 5 ) {
    debug_cout ( "MultiThumbRequest timer event. \n" );
    // Called after user manually reloads an image.
//    if ( ! lockDialog ( ) )
//      return;

    QPixmap thePixmap;
    Thumbs *pThumbs = NULL;
    //Q3IconViewItem *pItem = NULL; //ooo
    QListWidgetItem *pItem = NULL;  //xxx
    Cache::Thumbs::Entry *pEntry = NULL;
    QList<Cache::Thumbs::Entry *>list = Global::pThumbsCache->getScannedThumbs ( );	//oxx
    QList<Cache::Thumbs::Entry *>::iterator it;						//oxx
    QList<Thumbs *>::iterator it2;							//oxx
    bool bFound = false;

    it = list. begin ( );
    while ( it != list.end ( ) ) {
      pEntry = *it++;
      // Next we have to find the corresponding Item in the ListView 
      //int iCurrent = 0, iTotal = m_listOfThumbs.count ( );
      it2 = m_listOfThumbs.begin ( );
      while ( it2 != m_listOfThumbs.end ( ) && m_pIAmAlive ) {
	pThumbs = *it2++;
	if  ( pThumbs->pCache == pEntry ) {
	  pEntry->iScanStatus = 0;
	  if ( ! pEntry->arrayOfThumbs )
	         pEntry->loadImages  ( );

	  createPix ( pThumbs, 0 );
	  //pThumbs->pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[0] );    //ooo
      pThumbs->pOwnerItem->setIcon ( *pThumbs->arrayOfPix[0] );     //xxx
	  bFound = true;
	  break; // exit inner while loop
	}
      }
      if ( ! bFound ) {
	it2 = m_listOfThumbs.begin ( );
	while ( it2 != m_listOfThumbs.end ( ) && m_pIAmAlive ) {
	  pThumbs = *it2++;
	  if  ( pThumbs->pOwnerItem == pItem ) {
	    pEntry->iScanStatus = 0;
	    if ( ! pEntry->arrayOfThumbs )
	      pEntry->loadImages  ( );

	    createPix ( pThumbs, 0 );
	    //pThumbs->pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[0] );  //ooo
        pThumbs->pOwnerItem->setIcon ( *pThumbs->arrayOfPix[0] );   //xxx
	    bFound = true;
	    break; // exit inner while loop
	  }
	}
      }
    }
    m_bCanClose = true;  
//    unlockDialog ( );
  }

  m_bCanChangeIcons = true;
  //uiDialogFiles::timerEvent ( pEvent );	//ooo
  QDialog::timerEvent ( pEvent );		//xxx
}

//void DialogFiles::initWork ( Q3IconViewItem *pItem, QImage *pImage, bool bMarkIcon )  //ooo
void DialogFiles::initWork ( QListWidgetItem *pItem, QImage *pImage, bool bMarkIcon )   //xxx
{
  WorkWaiting *pNewWork = new WorkWaiting ( pItem, pImage, bMarkIcon );
  lock   ( );
  m_listOfWorkToDo.append ( pNewWork );
  unlock ( );
}

// Virtual function overloaded in DialogImages
//void DialogFiles::newThumbnailRequest ( Q3IconViewItem *pItem )   //ooo
void DialogFiles::newThumbnailRequest ( QListWidgetItem *pItem )    //xxx
{
  //ThumbnailRequest *pRequest = new ThumbnailRequest ( this, pItem, getKey ( pItem ), m_iMultiThumbNumber );	//ooo
  ThumbnailRequest *pRequest = new ThumbnailRequest ( this, pItem, pItem->text ( ), m_iMultiThumbNumber );	//xxx

  MediaCreator::registerWithMediaScanner ( pRequest );
}

bool DialogFiles::initScanDV ( Thumbs *pThumbs )
{
  // Also let me check if we have a dv file which we can scan for met info
  ScanDVDate *pScan = NULL;
  QFileInfo fileInfo ( pThumbs->pCache->qsFileName );
  //if ( fileInfo.extension  ( ).lower ( ) == "dv" ) {  //ooo
  if ( fileInfo.suffix  ( ).toLower ( ) == "dv" ) {     //xxx
    pScan = new ScanDVDate ( this, pThumbs->pCache->qsFileName, pThumbs->pOwnerItem );
    // TODO: Might need to get pushed to main loop
    markIcon ( pThumbs->pOwnerItem, 2 );
    addProgress ( 1 ); // add on more thing to do.
    lock   ( );
    m_listOfDVScansToDo.append ( pScan );
    unlock ( );
    return true;
  }
  return false;
}

uint DialogFiles::thumbnailSize ( )
{
  return m_iThumbnailSize;
}

void DialogFiles::slotIconSelectionChanged ( )
{
  // Here we enable / disable the Buttons 
  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx
  int iCount = 0;

  while  ( pIcon ) {
    if   ( pIcon->isSelected ( )  ) {
      if ( ++iCount > 1 )
	break;
    }
    //pIcon = pIcon->nextItem ( );      //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 ); //xxx
  }

  m_pButtonReload->setEnabled ( iCount  > 0 );
  m_pButtonPlay->setEnabled   ( iCount == 1 );
}

void DialogFiles::slotIconSelectionChanged ( QTreeWidgetItem *, int )  //xxx
{
  // Here we enable / disable the Buttons 
  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx
  int iCount = 0;

  while  ( pIcon ) {
    if   ( pIcon->isSelected ( )  ) {
      if ( ++iCount > 1 )
	break;
    }
    //pIcon = pIcon->nextItem ( );      //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 ); //xxx
  }
  m_pButtonReload->setEnabled ( iCount  > 0 );
  m_pButtonPlay->setEnabled   ( iCount == 1 );
}

//void DialogFiles::slotPreviewPressed ( Q3IconViewItem *pItem, const QPoint &pos ) //ooo
void DialogFiles::slotPreviewPressed ( QListWidgetItem *pItem )  //xxx
{ 
  /*if ( ! pItem )// User clicked on empty space in QIconView ( m_pPreview )
    showBasicMenu   ( pos );
  else*/    //ooo
    //showContextMenu ( pos, pItem );   //ooo
    showContextMenu ( pItem );          //xxx
}

//void DialogFiles::slotPreviewClicked ( int iButton, Q3IconViewItem *pItem, const QPoint & )   //ooo
void DialogFiles::slotPreviewClicked ( QListWidgetItem *pItem, int )    //xxx
{  
  if ( ! pItem )
    return; // User clicked on empty space in QIconView ( m_pPreview )

  /*if ( iButton == Qt::RightButton )
    return;*/   //ooo

  pItem->setSelected ( false );
  if ( m_pTimerThumbing->isActive ( ) )
       stopThumbing  ( );
  else {
    uint t;
    Thumbs *pThumb  = NULL;
    for ( t=0; t<(uint)m_listOfThumbs.count ( ); t++ ) {	//ox
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
  m_pActiveThumbs = NULL;
}

//////////////////////////////////
QPixmap &DialogFiles::createMenuHeader ( QString qsText, QSize size )
{
  static QPixmap header;
  header = QPixmap ( size );
  header.fill  ( 0x9999DD );
  QPainter painter ( &header );
  painter.drawText ( 0, 0, size.width ( ), size.height ( ), Qt::AlignCenter, qsText );

  return header;
}

void DialogFiles::slotAudioSubtitle ( int iID )
{
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;

  Thumbs *pThumbs = NULL;
  QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      break;
    }
  }
  if ( ! pThumbs )
    return;

  unsigned long long iPathHash, iFileHash;
  Manual         *pManual = NULL;
  Manual::Change *pChange = NULL;

  if ( iID == 98 )  { // reset entries
    // remove the mapOfManualChange entry and re-load the track info from the directory.
    bool bHasManualChange = pThumbs->getManualChange ( pManual, pChange, iPathHash, iFileHash );
    if ( bHasManualChange )  {
      if ( pManual && pChange )  {
        pManual->mapOfChange.remove ( iFileHash );
        delete pChange;
        pThumbs->findTracks ( );
      }
      if ( pManual && pManual->mapOfChange.count ( ) < 1 )  {
        pManual->mapOfChange.remove ( iPathHash );
        delete pManual;
      }
    }
  }
  else if ( iID == 99 )  { // Don't add tracks
    bool bHasManualChange = pThumbs->getManualChange ( pManual, pChange, iPathHash, iFileHash );
    if ( bHasManualChange )  {
      // To disregard all found tracks, we leave the Change entry but clear the lists
      pChange->clearAudioList     ( );
      pChange->clearSubtitleList  ( );
      // Should we do this here or in accepted () ???
      if ( pThumbs->pSourceFileInfo )  {
           pThumbs->pSourceFileInfo->deleteAudio     ( );
           pThumbs->pSourceFileInfo->deleteSubtitles ( );
      }
    }
  }
  else if ( iID < 200 )  {
    // User toggled a Audio track
    int iIdx = iID - 100;
    if ( iIdx+1 > (int)pThumbs->audioList.count ( ) )
      return;
    Audio *pAudio = pThumbs->audioList[iIdx];
    pThumbs->toggleAudioTrack ( pAudio );
  }
  else if ( iID >= 200 )  {
    // user toggled a sutitle track
    int iIdx = iID - 200;
    if ( iIdx+1 > (int)pThumbs->subtitleList.count ( ) )
      return;
    SubtitleEntry *pSubtitle = pThumbs->subtitleList[iIdx];
    pThumbs->toggleSubtitleTrack ( pSubtitle );
  }
}

/////////////////////////////////

//void DialogFiles::showContextMenu ( const QPoint &pos, Q3IconViewItem * ) //ooo
void DialogFiles::showContextMenu ( QListWidgetItem *pItem )  //xxx
//void DialogFiles::contextMenuEvent(QContextMenuEvent *event)  //xxx
{
  /*if ( QContextMenuEvent::Mouse) {  //xxx
    QMouseEvent *event;
  }*/
  
  QAction *noStarsAct, *star1Act, *star2Act, *star3Act, *star4Act, *star5Act, *star6Act;    //xxx
    
  int t, iID, iIDs[12], iStarRating = -2;
  Thumbs     *pThumbs = NULL;
  if ( m_pContextMenu )
    delete m_pContextMenu;
  //m_pContextMenu = new Q3PopupMenu ( this );  //ooo
  m_pContextMenu = new QMenu ( this );          //xxx
  //m_pContextMenu->setCheckable ( TRUE );      //oooo

  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );  //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );
  //QList<Q3IconViewItem *>listIcons;		//oxx
  QList<QListWidgetItem *>listIcons;		//xxx
  //QList<Q3IconViewItem *>::iterator it2;	//oxx
  QList<QListWidgetItem *>::iterator it2;	//xxx
  QList<Thumbs *>listSelected;			//oxx
  QList<Thumbs *>::iterator it;			//oxx

  while ( pIcon ) {
    if  ( pIcon->isSelected ( )  )
      listIcons.append   ( pIcon );
    //pIcon = pIcon->nextItem ( );                      //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 );   //xxx
  }

  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      listSelected.append ( pThumbs );
      if ( iStarRating == -2 )
           iStarRating  = (int)pThumbs->pCache->iStarRating;
      if ( iStarRating != (int)pThumbs->pCache->iStarRating )
           iStarRating  = -1;
    }
  }

  QPoint pos; //xxx
  //pos.setX(this->pos().x()+pItem->listWidget()->pos().x());  //xxx
  //pos.setY(this->pos().y()+pItem->listWidget()->pos().y());  //xxx
  pos.setX(this->pos().x()+m_pPreview->pos().x()+pItem->listWidget()->pos().x());  //xxx
  pos.setY(this->pos().y()+m_pPreview->pos().y()+pItem->listWidget()->pos().y());  //xxx
  
  QPoint globalPos = pos, globalPos2 = pos;   //ooo

  //Q3PopupMenu *pStackMenu;    //ooo
  QMenu *pStackMenu;            //xxx
  pStackMenu = NULL;

  if ( listSelected.count ( ) == 1 )  {    
    // Display language / subtitle menu only for single selectoin.
    Audio         *pAudio    =  NULL;
    SubtitleEntry *pSubtitle =  NULL;
    bool bHasAudioTracks     = false;
    bool bHasSubtitleTracks  = false;
    bool    bChecked         = false;
    int     iTrackNr;
    Utils   theUtils;
    QSize   size;
    QString qsTrackNr, qsFileName;
    QFileInfo fileInfo;

    pThumbs = listSelected[0];
    if ( pThumbs->audioList.count ( ) > 0 )
      bHasAudioTracks    = true;
    if ( pThumbs->subtitleList.count ( ) > 0 )
      bHasSubtitleTracks = true;

    if ( bHasAudioTracks || bHasSubtitleTracks )  {
      //pStackMenu = new  Q3PopupMenu (  this  );   //ooo
      pStackMenu = new  QMenu (  this  );           //xxx
      //pStackMenu->insertItem  ( tr ( "&Reload all entries" ),  98 );  //ooo
      pStackMenu->addAction  ( tr ( "&Reload all entries" ) );          //xxx
      size = pStackMenu->sizeHint  ( );
      //pStackMenu->insertItem  ( tr ( "&Don't add tracks" ),    99 );  //ooo
      pStackMenu->addAction  ( tr ( "&Don't add tracks" ) );            //xxx
      if ( ( size.width ( ) < 5 ) || ( size.height ( ) < 5 ) )
        size =  QSize ( 100, 15 );

      if ( bHasAudioTracks )  {
        //pStackMenu->insertSeparator  ( ); //ooo
        pStackMenu->addSeparator  ( );      //xxx
        //iID = pStackMenu->insertItem ( createMenuHeader ( tr ( "Audio tracks :" ), size ) );  //ooo
        QAction *audioAct = new QAction ( QIcon ( createMenuHeader ( tr ( "Audio tracks :" ), size ) ), QString(""), this );   //xxx
        audioAct->setEnabled(false);                //xxx
        pStackMenu->addAction ( audioAct );         //xxx
        //pStackMenu->setEnabled   ( iID, FALSE );  //ooo
        //iID      = 100;                           //ooo
        iTrackNr =   0;
        for ( t=0; t<(int)pThumbs->audioList.count ( ); t++ )  {
          pAudio     = pThumbs->audioList[t];
          fileInfo.setFile ( pAudio->qsFileName );
          qsFileName = fileInfo.fileName ( );
          qsTrackNr  = "x";
          // To decide if the track is enabled or not, we'll have to inquire the mapOfManualChange - entry
          bChecked  = pThumbs->audioTrackEnabled ( pAudio->qsFileName );
          if ( bChecked )  {
            iTrackNr ++;
            qsTrackNr = QString ( "%1" ).arg ( pAudio->iAudioNumber ); //iTrackNr );
          }
          //iID = pStackMenu->insertItem ( tr ( "[%1]<%2> %3" ).arg ( qsTrackNr ).arg ( pAudio->qsIso639 ).arg ( qsFileName ), iID );   //ooo
          QAction *audio_Act = new QAction ( tr ( "[%1]<%2> %3" ).arg ( qsTrackNr ).arg ( pAudio->qsIso639 ).arg ( qsFileName ), this );                 //xxx
          audio_Act->setChecked ( bChecked );                   //xxx
          pStackMenu->addAction ( audio_Act );                  //xxx
          //pStackMenu->setItemChecked   ( iID++, bChecked );   //ooo
        }
      }
      if ( bHasSubtitleTracks )  {
        //pStackMenu->insertSeparator  ( );     //ooo
        pStackMenu->addSeparator  ( );          //xxx
        //iID = pStackMenu->insertItem ( createMenuHeader ( tr ( "Subtitle tracks :" ), size ) );   //ooo
        QAction *subtitlesAct = new QAction ( QIcon ( createMenuHeader ( tr ( "Subtitle tracks :" ), size ) ), QString(""), this );   //xxx
        subtitlesAct->setEnabled ( false );                   //xxx
        pStackMenu->addAction ( subtitlesAct );               //xxx
        //pStackMenu->setItemEnabled   ( iID, FALSE );        //ooo
        //iID         = 200;    //ooo
        iTrackNr    =   0;
        for ( t=0; t<(int)pThumbs->subtitleList.count ( ); t++ )  {
          pSubtitle  = pThumbs->subtitleList[t];
          fileInfo.setFile ( pSubtitle->m_qsFileName );
          qsFileName = fileInfo.fileName ( );
          qsTrackNr  = "x";
          bChecked   = pThumbs->subtitleTrackEnabled ( pSubtitle->m_qsFileName );
          if ( bChecked )  {
            iTrackNr ++;
            qsTrackNr = QString ( "%1" ).arg ( pSubtitle->m_iSubtitleNumber ); //iTrackNr );
          }

          //pStackMenu->insertItem      ( tr ( "[%1]<%2> %3" ).arg ( qsTrackNr ).arg ( pSubtitle->m_qsIso639 ).arg ( qsFileName ), iID );   //ooo
          QAction *subtitles_Act = new QAction ( tr ( "[%1]<%2> %3" ).arg ( qsTrackNr ).arg ( pSubtitle->m_qsIso639 ).arg ( qsFileName ), this );    //xxx
          subtitles_Act->setChecked ( bChecked );                   //xxx
          pStackMenu->addAction ( subtitles_Act );  //xxx
          //pStackMenu->setItemChecked  ( iID++, bChecked );    //ooo
        }
      }
      /*globalPos.setY    ( globalPos.y ( ) - 65 );
      globalPos.setX    ( globalPos.x ( ) - pStackMenu->sizeHint ( ).width ( ) - 4 );*/ //ooo
      connect ( pStackMenu, SIGNAL ( activated ( int ) ), this, SLOT ( slotAudioSubtitle ( int ) ) );
      //pStackMenu->popup ( globalPos, 3 ); //ooo
      pStackMenu->exec ( );      //xxx
    }
  }

  //iIDs[6]  = m_pContextMenu->insertItem ( tr ( "&Play" ),                   9 );  //ooo
  QAction *playAct = new QAction ( tr ( "&Play" ), this );                          //xxx

  //iIDs[7]  = m_pContextMenu->insertItem ( tr ( "to &Source Group ..." ),   10 );  //ooo
  QAction *sourceGroupAct = new QAction ( tr ( "to &Source Group ..." ), this );    //xxx

  //iIDs[8]  = m_pContextMenu->insertItem ( tr ( "to &Virtual Folder ..." ), 11 );  //ooo
  QAction *virtualFolderAct = new QAction ( tr ( "to &Virtual Folder ..." ), this );//xxx

  //iIDs[9]  = m_pContextMenu->insertItem ( tr ( "&Reload" ),                12 );  //ooo
  QAction *reloadAct = new QAction ( tr ( "&Reload" ), this );                      //xxx

  //iIDs[10] = m_pContextMenu->insertItem ( tr ( "R&emove" ),                13 );  //ooo
  QAction *removeAct = new QAction ( tr ( "R&emove" ), this );                      //xxx
  
  m_pContextMenu->addAction ( playAct );                                            //xxx
  m_pContextMenu->addAction ( sourceGroupAct );                                     //xxx
  m_pContextMenu->addAction ( virtualFolderAct );                                   //xxx
  m_pContextMenu->addAction ( reloadAct );                                          //xxx
  m_pContextMenu->addAction ( removeAct );                                          //xxx
  
  if ( listIcons.count ( ) == listSelected.count ( ) ) {
    //m_pContextMenu->insertSeparator ( );  //ooo
    m_pContextMenu->addSeparator ( );       //xxx
    //iIDs[11] = m_pContextMenu->insertItem ( tr ( "No Stars" ),              1 );  //ooo
    noStarsAct = new QAction ( tr ( "No Stars" ), this );  //xxx
 
    /*for ( t=0; t<6; t++ )
      //iIDs[t] = m_pContextMenu->insertItem( m_pixMenuStars[t], t+2 );             //oooo
      m_pContextMenu->addAction( QIcon(m_pixMenuStars[t]), QString(""), this );     //xxx
      */    //oooo
    star1Act = new QAction ( QIcon(m_pixMenuStars[0]), QString(""), this );  //xxx
    star2Act = new QAction ( QIcon(m_pixMenuStars[1]), QString(""), this );  //xxx
    star3Act = new QAction ( QIcon(m_pixMenuStars[2]), QString(""), this );  //xxx
    star4Act = new QAction ( QIcon(m_pixMenuStars[3]), QString(""), this );  //xxx
    star5Act = new QAction ( QIcon(m_pixMenuStars[4]), QString(""), this );  //xxx
    star6Act = new QAction ( QIcon(m_pixMenuStars[5]), QString(""), this );  //xxx
 
    m_pContextMenu->addAction ( noStarsAct );   //xxx
    m_pContextMenu->addAction ( star1Act ); //xxx
    m_pContextMenu->addAction ( star2Act ); //xxx
    m_pContextMenu->addAction ( star3Act ); //xxx
    m_pContextMenu->addAction ( star4Act ); //xxx
    m_pContextMenu->addAction ( star5Act ); //xxx
    m_pContextMenu->addAction ( star6Act ); //xxx    
  }
  if ( listIcons.count ( ) < 1 ) {
    delete m_pContextMenu;
    m_pContextMenu = NULL;
    if ( pStackMenu )
      delete pStackMenu;
    return;
  }
  else if ( listIcons.count ( ) > 1 )
    //m_pContextMenu->setItemEnabled ( iIDs[6], false );    //ooo
   playAct->setEnabled ( false );           //xxx

  if ( listIcons.count ( ) != listSelected.count ( ) ) {
    //m_pContextMenu->setItemEnabled ( iIDs[7], false );    //ooo
    sourceGroupAct->setEnabled ( false );                   //xxx
    //m_pContextMenu->setItemEnabled ( iIDs[8], false );    //ooo
    virtualFolderAct->setEnabled ( false );                 //xxx
  }

  /*if ( iStarRating > -1 )
    m_pContextMenu->setItemChecked ( iStarRating + 1, TRUE );*/ //ooo

  // last we check if we do have VirtualFolders ...
  /*if ( m_pListViewVirtual->childCount ( ) == 0 )
    //m_pContextMenu->setItemEnabled ( iIDs[8],  FALSE );   //ooo
    virtualFolderAct->setEnabled ( false );                 //xxx */    //ooo

  //globalPos2.setY   ( globalPos2.y ( ) - 110 );   //ooo
  //iID = m_pContextMenu->exec ( globalPos2 );  //ooo
  
  globalPos2.setX   ( globalPos2.x ( ) + 310 );   //xxx
  globalPos2.setY   ( globalPos2.y ( ) + 110 );   //xxx
  QAction *action;  //xxx
  action = m_pContextMenu->exec ( globalPos2 ); //xxx
  //m_pContextMenu->exec ( mapToGlobal(event->pos()) ); //xxx
  
  if ( action == noStarsAct ) //xxx
    iID = 1;
  else if ( action == star1Act )
    iID = 2;
  else if ( action == star2Act )
    iID = 3;
  else if ( action == star3Act )
    iID = 4;
  else if ( action == star4Act )
    iID = 5;
  else if ( action == star5Act )
    iID = 6;
  else if ( action == star6Act )
    iID = 7;
  else if ( action == playAct )
    iID = 9;
  else if ( action == sourceGroupAct )
    iID = 10;
  else if ( action == virtualFolderAct )
    iID = 11;
  else if ( action == reloadAct )
    iID = 12;
  else if ( action == removeAct )
    iID = 13;

  /*if ( iID == 0 ) { // Nothing selected ...
    delete m_pContextMenu;
    m_pContextMenu = NULL;
    if ( pStackMenu )
      delete pStackMenu;
    return;
  }*/   //oooo

  stopThumbing ( );
  if ( ( iID > 0 ) && ( iID < 8 ) ) {  // StarRating
    if ( ! lockDialog ( ) )
      return;

    it = listSelected.begin ( );
    while ( it != listSelected.end ( ) ) {
      pThumbs = *it++;
      pThumbs->pCache->iStarRating = iID - 1;
      recreateThumbs  ( pThumbs );
    }
    Global::pThumbsCache->saveCacheDB ( );
    unlockDialog ( );
  }
  else if (  iID ==   9 ) {  // Play
    if ( listSelected.count ( ) )  {
      pThumbs = listSelected[0];
      play ( pThumbs->pCache, pThumbs->pOwnerItem );
    }
  }
  else if (  iID ==  10 )    // toSourceGroup
    toSourceGroup   ( 3 );
  else if (  iID ==  11 )    // toVirtualFolder
    toVirtualFolder ( 3 );
  else if (  iID ==  12 )    // Reload
    slotReload ( );
  else if (  iID ==  13 ) {  // Remove
    lock ( );
    it = listSelected.begin ( );
    while ( it != listSelected.end ( ) ) {
      pThumbs = *it++;
      //m_listOfThumbs.remove  ( pThumbs ); //ooo
      m_listOfThumbs.removeOne  ( pThumbs );    //xxx
      //listIcons.remove ( pThumbs->pOwnerItem );   //ooo
      listIcons.removeOne ( pThumbs->pOwnerItem );  //xxx
      if ( m_pActiveThumbs == pThumbs )
           m_pTimerThumbing->stop   ( );
      delete pThumbs->pOwnerItem;
      delete pThumbs;
    }
    it2 = listIcons.begin ( );
    while  (  it2 != listIcons.end ( ) )
      delete *it2++;
    unlock ( );
  }

  if ( pStackMenu )
    delete pStackMenu;
  delete m_pContextMenu;
  m_pContextMenu = NULL;
}

void DialogFiles::showBasicMenu ( const QPoint &pos )
{
  int t, iID, iIDs[8];
  //Q3PopupMenu *pMenu   = new Q3PopupMenu ( this );    //ooo
  QMenu *pMenu   = new QMenu ( this );                  //xxx
  //pMenu->setCheckable ( TRUE );                       //ooo

  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx
  //QList<Q3IconViewItem *>listFailed;		//oxx
  QList<QListWidgetItem *>listFailed;		//xxx
  //QList<Q3IconViewItem *>::iterator it2;	//oxx
  QList<QListWidgetItem *>::iterator it2;	//xxx
  QList<Thumbs *>listSelected;			//oxx
  QList<Thumbs *>::iterator it;			//oxx

  while ( pIcon ) {
    listFailed.append ( pIcon );
    //pIcon = pIcon->nextItem ( );                      //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 );   //xxx
  }

  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) )
    //listFailed.remove ( (*it++)->pOwnerItem );    //ooo
    listFailed.removeOne ( (*it++)->pOwnerItem );   //xxx

  //maybe submenu ...
  //iIDs[0] = pMenu->insertItem ( tr ( "All to &Virtual Folder ..." ), 1 ); //ooo
  pMenu->addAction ( tr ( "All to &Virtual Folder ..." ) );                 //xxx
/*  iIDs[1] = pMenu->insertItem ( tr ( "All to &Source Group ..." ),   2 );
  iIDs[2] = pMenu->insertItem ( tr ( "&Remove failed Thumbs" ),      3 );
  iIDs[3] = pMenu->insertItem ( tr ( "Re&load failed Thumbs" ),      4 );
  pMenu->insertSeparator ( );
  iIDs[4] = pMenu->insertItem ( tr ( "&Auto Group" ),                5 );
  iIDs[5] = pMenu->insertItem ( tr ( "&Sort By ..." ),               6 );
  iIDs[6] = pMenu->insertItem ( tr ( "&Refresh" ),                   7 );
  iIDs[7] = pMenu->insertItem ( tr ( "&Clear" ),                     8 );

  if ( m_pPreview->count ( ) < 1 ) {
    for ( t=0; t<7; t++ )
      pMenu->setItemEnabled ( iIDs[t], false );      
  }
  else if ( listFailed.count ( ) < 1 ) {
    pMenu->setItemEnabled ( iIDs[2], false );
    pMenu->setItemEnabled ( iIDs[3], false );
  }

  iID = pMenu->exec ( pos );

  if ( iID == 0 ) { // Nothing selected ...
    delete pMenu;
    return;
  }

  stopThumbing ( );
  if ( iID ==  1 ) // All to Virtual Folder
    toVirtualFolder ( 2 );
  else if ( iID  == 2 ) // All to Source Group
    toSourceGroup ( 2 );
  else if ( iID  == 3 ) { // Remove Failed
    it2 = listFailed.begin ( );
    while ( it2 != listFailed.end ( ) )
      delete *it2++;
  }
  else if ( iID == 4 ) { // Reload Failed
    // In case we have some error Icons around ...
    QPixmap thePixmap;
    //QImage  theImage = QImage ( QImage ( ).fromMimeSource ( "please_wait.jpg" ) );	//ooo
    QImage  theImage = QImage ( ":/images/please_wait.jpg" );				//xxx
    createPix ( &thePixmap, &theImage );

    it2 = listFailed.begin ( );
    while ( it2 != listFailed.end ( ) ) {
      //Q3IconViewItem    *pItem = *it2++;  //ooo
      QTreeWidgetItem    *pItem = *it2++;   //xxx
      pItem->setPixmap ( thePixmap );
      newThumbnailRequest  ( pItem );
    }
    m_pPreview->arrangeItemsInGrid ( TRUE );
  }
  else if ( iID == 5 )  // Auto Group
    slotAutoGroup  ( );
  else if ( iID == 6 )  // SortBy
    slotSortBy   ( 0 );
  else if ( iID == 7 )  // Refresh
    slotRefresh    ( );
  else if ( iID == 8 )  // Clear
    clearPreview   ( );
*/  //oooo
  delete pMenu;
}

//QList<Q3ListViewItem *> DialogFiles::getSelectedVirtual ( )	//oxx
QList<QTreeWidgetItem *> DialogFiles::getSelectedVirtual ( )	//xxx
{
  //QList<Q3ListViewItem *> list;		//oxx
  QList<QTreeWidgetItem *> list;		//xxx

  //Q3ListViewItem *pItem = m_pListViewVirtual->firstChild ( );     //ooo
  QTreeWidgetItem *pItem = m_pListViewVirtual->topLevelItem ( 0 );  //xxx
  while ( pItem )  {
     if ( pItem->isSelected ( ) )
       list.append ( pItem );
    //pItem = pItem->nextSibling ( );   //ooo
    pItem = pItem->treeWidget()->itemBelow(pItem);	//xxx
  }
  return list;
}

/*void DialogFiles::slotDroppedOnPreview ( QDropEvent *pDropEvent, const QList<Q3IconDragItem> & )	//oxx
{
  // First we check if it comes from one of the Groups
  if (  pDropEvent->source ( ) == m_pToolbox->currentItem ( ) )
    currentSourceToPreview ( );
}*/ //oooo

void DialogFiles::currentSourceToPreview ( )
{
  // Okay this drop comes from a Group
  Thumbs    *pThumbs     = NULL;
  //GroupView *pGroupView  = (GroupView       *)m_pToolbox->currentItem ( );    //ooo
  GroupView *pGroupView  = (GroupView       *)m_pToolbox->currentWidget ( );    //xxx
  GroupView::Item *pItem = (GroupView::Item *)pGroupView->currentItem ( );
  if ( ! lockDialog ( ) )
    return;

  if ( pItem ) {
    setQuickDir      ( -1 );
    setVirtualFolder ( -1 );
    m_qsCurrentDir = "/";

    // gotcha ...
    pThumbs = createFromCache ( pItem->pCache );
    pThumbs->pSourceFileInfo  = pItem->pSourceFileInfo;
    delete pItem;
    QTimer::singleShot ( 10, pGroupView, SLOT ( slotAlternateColors ( ) ) );
  }
  unlockDialog ( );
}

void DialogFiles::allSourcesToPreview ( )
{
  // Okay this drop comes from a Group
  Thumbs    *pThumbs     = NULL;
  //GroupView *pGroupView  = (GroupView       *)m_pToolbox->currentItem ( );    //ooo
  GroupView *pGroupView  = (GroupView       *)m_pToolbox->currentWidget ( );    //xxx
  //GroupView::Item *pItem = (GroupView::Item *)pGroupView->firstChild  ( );    //oooo
  //GroupView::Item *pItem = (GroupView::Item *)pItem->child  ( 0 );         //xxx
  GroupView::Item *pItem = (GroupView::Item *)pGroupView->topLevelItem ( 0 )->child  ( 0 );         //xxx
  GroupView::Item *pTemp = NULL;
  if ( pItem ) {
    setQuickDir      ( -1 );
    setVirtualFolder ( -1 );
    m_qsCurrentDir = "/";
  }

  if ( ! lockDialog ( ) )
    return;

  while ( pItem ) {
    // gotcha ...
    pThumbs = createFromCache ( pItem->pCache );
    if ( pThumbs ) {
      pThumbs->pSourceFileInfo  = pItem->pSourceFileInfo;
      //pTemp = (GroupView::Item *)pItem->nextSibling ( );  //ooo
      pTemp = (GroupView::Item *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
      delete  pItem;
      pItem = pTemp;
    }
    else
      //pItem = (GroupView::Item *)pItem->nextSibling ( );  //ooo
      pItem = (GroupView::Item *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }

  QTimer::singleShot ( 10, pGroupView, SLOT ( slotAlternateColors ( ) ) );
  unlockDialog ( );
}

void DialogFiles::slotVirtualChanged ( )
{
  bool bEnabled = false;
  //QList<Q3ListViewItem *> list = getSelectedVirtual ( );	//oxx
  QList<QTreeWidgetItem *> list = getSelectedVirtual ( );	//xxx
  if ( list.count ( ) > 0 )
    bEnabled = true;

  m_pButtonEditVirtual  ->setEnabled ( bEnabled );
  m_pButtonDeleteVirtual->setEnabled ( bEnabled );
}

//void DialogFiles::slotShowVirtual ( Q3ListViewItem *pItem, const QPoint &, int iReset )   //ooo
void DialogFiles::slotShowVirtual ( QTreeWidgetItem *pItem, const QPoint &, int iReset )    //xxx
{
  if ( ! pItem )
    return;

  // reset button states
  setVirtualFolder ( -1 );
  setQuickDir      ( -1 );
  m_qsCurrentDir = "/";

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  QString qsFolderName = pItem->text ( 0 );

  unsigned long long iHashValue;
  Cache::Thumbs::Entry *pEntry = NULL;
  Cache::Thumbs::VirtualFolder *pFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );
  if ( pFolder ) {

    if ( ! lockDialog ( ) )
      return;

    if ( iReset != -1 )
       clearPreview ( );

    m_iProgress = 0;
    m_pProgressBar->reset ( );
    //m_pProgressBar->setTotalSteps ( pFolder->listOfFiles.count ( ) ); //oooo
    m_pProgressBar->setValue ( pFolder->listOfFiles.count ( ) );        //xxx
    int iCount  = 0;
    QList<Cache::Thumbs::VirtualFolder::VFile *>::iterator it;		//oxx
    it = pFolder->listOfFiles.begin ( );
    while ( it != pFolder->listOfFiles.end ( ) ) {
      iHashValue = (*it++)->iFileHashValue;
      pEntry = Global::pThumbsCache->find  ( iHashValue, isImageDialog ( ) );
      if ( ! pEntry && isImageDialog ( ) )
             pEntry = Global::pThumbsCache->find ( iHashValue, false );
      if (   pEntry )
        createFromCache ( pEntry );

      iCount ++;
      if ( iCount%5 == 0 ) {
        //m_pProgressBar->setProgress ( iCount );   //ooo
        m_pProgressBar->setValue ( iCount );        //xxx
        // Before we process Events we have to unlock the dialog, else we could get into a deadlock situation
        unlockDialog ( );
        qApp->processEvents ( );
        if ( ! lockDialog ( ) )
          return;
      }
    }
    m_pProgressBar->reset ( );
    unlockDialog ( );
  }

  QApplication::restoreOverrideCursor ( );
}

//void DialogFiles::slotVirtualContextMenu ( Q3ListViewItem *pItem, const QPoint &pos, int )    //ooo
void DialogFiles::slotVirtualContextMenu ( QTreeWidgetItem *pItem, const QPoint &pos, int )     //xxx
{
  //Q3PopupMenu *pMenu   = new Q3PopupMenu  ( this );   //ooo
  QMenu *pMenu   = new QMenu  ( this );                 //xxx
  int iIDs[4];

/*  iIDs[0] = pMenu->insertItem ( tr ( "&Show" ),   1 );
  iIDs[0] = pMenu->insertItem ( tr ( "&Clone" ),  2 );
  pMenu->insertSeparator ( );
  iIDs[1] = pMenu->insertItem ( tr ( "&Add" ),    3 );
  iIDs[2] = pMenu->insertItem ( tr ( "&Edit" ),   4 );
  iIDs[3] = pMenu->insertItem ( tr ( "&Delete" ), 5 );

  if ( ! pItem ) {
    pMenu->setItemEnabled ( iIDs[0], false );
    pMenu->setItemEnabled ( iIDs[2], false );
    pMenu->setItemEnabled ( iIDs[3], false );
  }

  int iID = pMenu->exec  ( pos );

  if ( iID == 1 )  {
   clearPreview ( );
   //QList<Q3ListViewItem *>list = getSelectedVirtual ( );	//oxx
   QList<QTreeWidgetItem *>list = getSelectedVirtual ( );	//xxx
    for ( uint t=0; t<(uint)list.count ( ); t ++ )		//ox
      slotShowVirtual ( list[t], pos, -1 ); // col );
  }
  else if  ( iID == 2 )
    slotCloneVirtual( );
  else if  ( iID == 3 )
    slotAddVirtual  ( );
  else if  ( iID == 4 )
    slotEditVirtual ( );
  else if  ( iID == 5 )
    slotDeleteVirtual ( );*/    //oooo
  delete pMenu;
}

void DialogFiles::toVirtualFolder ( int iFromSourceGroup )
{  
  // Pops open a new QListBox based Dialog.
  // No Ok / Cancel et. simply clicking on the
  // VirtualFolder Item will assign the object to this folder.
  bool bAlternate = false;
  if ( m_pVirtualFolderDialog )
    delete m_pVirtualFolderDialog;
  m_pVirtualFolderDialog = NULL;

  //m_pTabWidget->setCurrentPage ( 1 ); //ooo
  m_pTabWidget->setCurrentIndex ( 1 );  //xxx

  //m_pVirtualFolderDialog = new Q3ListBox ( NULL, "Test", Qt::WType_Dialog | Qt::WShowModal | Qt::WStyle_NoBorder  | Qt::WStyle_StaysOnTop );  //ooo
  //m_pVirtualFolderDialog = new QTreeWidget ( );  //xxx
  m_pVirtualFolderDialog = new QListWidget ( );  //xxx
  m_pVirtualFolderDialog->resize ( 200, 450 );

  //Q3ListViewItem *pFolder = m_pListViewVirtual->firstChild ( );   //ooo
  QTreeWidgetItem *pFolder = m_pListViewVirtual->topLevelItem ( 0 );    //xxx
  while ( pFolder ) {
    bAlternate = ! bAlternate;
    new VFSelectItem ( bAlternate, m_pVirtualFolderDialog, pFolder->text ( 0 ) );
    //pFolder = pFolder->nextSibling ( );   //ooo
    pFolder = pFolder->parent()->child(pFolder->parent()->indexOfChild(pFolder)+1);	//xxx
  }

  if ( iFromSourceGroup == 1 )
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotSourceToVirtualFolder ( Q3ListBoxItem * ) ) );     //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QTreeWidgetItem * ) ), this, SLOT ( slotSourceToVirtualFolder ( QTreeWidgetItem * ) ) );   //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QListWidgetItem * ) ), this, SLOT ( slotSourceToVirtualFolder ( QListWidgetItem * ) ) );   //xxx
  else if ( iFromSourceGroup == 2 )
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotAllToVirtualFolder ( Q3ListBoxItem * ) ) );        //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QTreeWidgetItem * ) ), this, SLOT ( slotAllToVirtualFolder ( QTreeWidgetItem * ) ) );      //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QListWidgetItem * ) ), this, SLOT ( slotAllToVirtualFolder ( QListWidgetItem * ) ) );      //xxx
  else
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotToVirtualFolder ( Q3ListBoxItem * ) ) );           //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QTreeWidgetItem * ) ), this, SLOT ( slotToVirtualFolder ( QTreeWidgetItem * ) ) );         //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( QListWidgetItem * ) ), this, SLOT ( slotToVirtualFolder ( QListWidgetItem * ) ) );         //xxx
  m_pVirtualFolderDialog->show ( );   //oooo
}

void DialogFiles::toSourceGroup ( int iFromSourceGroup )
{ 
  // Pops open a new QListBox based Dialog.
  // No Ok / Cancel et. simply clicking on the
  // VirtualFolder Item will assign the object to this folder.
  int t;
  bool bAlternate = false;
  if ( m_pVirtualFolderDialog )
    delete m_pVirtualFolderDialog;
  m_pVirtualFolderDialog = NULL;


  //m_pVirtualFolderDialog = new Q3ListBox ( NULL, "Test", Qt::WType_Dialog | Qt::WShowModal | Qt::WStyle_NoBorder  | Qt::WStyle_StaysOnTop );  //ooo
  //m_pVirtualFolderDialog = new QTreeWidget ( ); //xxx
  m_pVirtualFolderDialog = new QListWidget ( ); //xxx
  m_pVirtualFolderDialog->resize ( 200, 450 );

  for ( t=0; t<m_pToolbox->count ( ); t++ ) {
    bAlternate = ! bAlternate;
    //new VFSelectItem ( bAlternate, m_pVirtualFolderDialog, m_pToolbox->itemLabel ( t ) ); //ooo
    new VFSelectItem ( bAlternate, m_pVirtualFolderDialog, m_pToolbox->itemText ( t ) );    //xxx
  }

  if ( iFromSourceGroup == 1 )
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotSourceToSourceGroup ( Q3ListBoxItem * ) ) );   //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( itemActivated ( QTreeWidgetItem * ) ), this, SLOT ( slotSourceToSourceGroup ( QTreeWidgetItem * ) ) ); //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( itemPressed ( QListWidgetItem * ) ), this, SLOT ( slotSourceToSourceGroup ( QListWidgetItem * ) ) ); //xxx
  else if ( iFromSourceGroup == 2 )
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotAllToSourceGroup ( Q3ListBoxItem * ) ) );      //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( itemActivated ( QTreeWidgetItem * ) ), this, SLOT ( slotAllToSourceGroup ( QTreeWidgetItem * ) ) );    //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( itemPressed ( QListWidgetItem * ) ), this, SLOT ( slotAllToSourceGroup ( QListWidgetItem * ) ) );    //xxx
  else
    //connect ( m_pVirtualFolderDialog, SIGNAL ( pressed ( Q3ListBoxItem * ) ), this, SLOT ( slotToSourceGroup ( Q3ListBoxItem * ) ) );         //ooo
    //connect ( m_pVirtualFolderDialog, SIGNAL ( itemActivated ( QTreeWidgetItem * ) ), this, SLOT ( slotToSourceGroup ( QTreeWidgetItem * ) ) );       //xxx
    connect ( m_pVirtualFolderDialog, SIGNAL ( itemPressed ( QListWidgetItem * ) ), this, SLOT ( slotToSourceGroup ( QListWidgetItem * ) ) );       //xxx
  m_pVirtualFolderDialog->show ( );   //oooo
}

//void DialogFiles::slotToVirtualFolder ( Q3ListBoxItem *pSelItem ) //ooo
void DialogFiles::slotToVirtualFolder ( QTreeWidgetItem *pSelItem ) //xxx
{
  uint t, i;
  bool bSelected = true;
  //Q3ListViewItem *pFolderItem = NULL;     //ooo
  QTreeWidgetItem *pFolderItem = NULL;      //xxx
  VFSelectItem  *pItem = (VFSelectItem *)pSelItem;
  Cache::Thumbs::VirtualFolder *pVirtualFolder = NULL;
  Thumbs       *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pItem )  {
    QString qsFolderName = pItem->text ( );   //ooo
    //QString qsFolderName = pItem->text ( 0 );   //xxx
    pVirtualFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );
    if ( pVirtualFolder ) {
      // First we have to get the selected items
      it = m_listOfThumbs.begin ( );
      while ( it != m_listOfThumbs.end ( ) ) {
	pThumbs = *it++;
	if ( pThumbs->pOwnerItem->isSelected ( ) )
	  pVirtualFolder->append ( pThumbs->pCache );
      }
      //pFolderItem = m_pListViewVirtual->firstChild ( );   //ooo
      pFolderItem = m_pListViewVirtual->topLevelItem ( 0 ); //xxx
      while ( pFolderItem ) {
	if  ( pFolderItem->text ( 0 ) == qsFolderName )
	  break;
	//pFolderItem = pFolderItem->nextSibling ( );  //ooo
    pFolderItem = pFolderItem->parent()->child(pFolderItem->parent()->indexOfChild(pFolderItem)+1);	//xxx
      }
      if ( pFolderItem )
	   pFolderItem->setText ( 1, QString ( "%1" ).arg ( pVirtualFolder->count ( ) ) );
      // And finally we give som visual feedback
      for ( t=0; t<9; t++ )  {
	//m_pVirtualFolderDialog->setSelected ( pItem, bSelected );    //ooo
    m_pVirtualFolderDialog->setCurrentItem ( pItem );      //xxx
	bSelected = ! bSelected;
	for ( i=0; i<10; i++ )  {
	  usleep ( 8000 ); // 8 ms
	  qApp->processEvents ( );
	}
      }
    }
    Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );
}

void DialogFiles::slotDeleteVFDialog ( )
{
  if ( ! m_pVirtualFolderDialog )
    return;

  delete m_pVirtualFolderDialog;
  m_pVirtualFolderDialog = NULL;
}

//void DialogFiles::slotAllToVirtualFolder ( Q3ListBoxItem *pSelItem )  //ooo
void DialogFiles::slotAllToVirtualFolder ( QTreeWidgetItem *pSelItem )  //xxx
{
  uint t, i;
  bool bSelected = true;
  //Q3ListViewItem *pFolderItem = NULL;     //ooo
  QTreeWidgetItem *pFolderItem = NULL;      //xxx
  VFSelectItem  *pItem = (VFSelectItem *)pSelItem;
  Cache::Thumbs::VirtualFolder *pVirtualFolder = NULL;
  //Thumbs       *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pItem )  {
    QString qsFolderName = pItem->text ( );   //ooo
    //QString qsFolderName = pItem->text ( 0 );   //xxx
    pVirtualFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );
    if ( pVirtualFolder ) {
      // First we have to get the selected items
      it = m_listOfThumbs.begin ( );
      while ( it != m_listOfThumbs.end ( ) )
	pVirtualFolder->append ( (*it++)->pCache );

      //pFolderItem = m_pListViewVirtual->firstChild ( );   //ooo
      pFolderItem = m_pListViewVirtual->topLevelItem ( 0 ); //xxx
      while ( pFolderItem ) {
	if  ( pFolderItem->text ( 0 ) == qsFolderName )
	  break;
	//pFolderItem = pFolderItem->nextSibling ( );  //ooo
    pFolderItem = pFolderItem->parent()->child(pFolderItem->parent()->indexOfChild(pFolderItem)+1);	//xxx
      }
      if ( pFolderItem )
	   pFolderItem->setText ( 1, QString ( "%1" ).arg ( pVirtualFolder->count ( ) ) );
      // And finally we give some visual feedback
      for ( t=0; t<9; t++ )  {
	//m_pVirtualFolderDialog->setSelected ( pItem, bSelected );    //oooo
    m_pVirtualFolderDialog->setCurrentItem ( pItem );              //xxx
	bSelected = ! bSelected;
	for ( i=0; i<10; i++ )  {
	  usleep ( 8000 ); // 8 ms
	  qApp->processEvents ( );
	}
      }
    }
    Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );
}

//void DialogFiles::slotSourceToVirtualFolder ( Q3ListBoxItem *pSelItem )   //ooo
void DialogFiles::slotSourceToVirtualFolder ( QTreeWidgetItem *pSelItem )   //xxx
{
  uint t, i;
  bool bSelected = true;
  //Q3ListViewItem *pFolderItem = NULL;     //ooo
  QTreeWidgetItem *pFolderItem = NULL;      //xxx
  VFSelectItem  *pItem = (VFSelectItem *)pSelItem;
  Cache::Thumbs::VirtualFolder *pVirtualFolder = NULL;

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pItem )  {
    QString qsFolderName = pItem->text ( );   //ooo
    //QString qsFolderName = pItem->text ( 0 );   //xxx
    pVirtualFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );
    if ( pVirtualFolder ) {
      // Here we determine the current selected SourceFileInfo
      //GroupView *pGroupView       = (GroupView       *)m_pToolbox->currentItem ( );   //ooo
      GroupView *pGroupView       = (GroupView       *)m_pToolbox->currentWidget ( );   //xxx
      GroupView::Item *pGroupItem = (GroupView::Item *)pGroupView->currentItem ( );
      if ( pGroupItem ) // gotcha ...
        pVirtualFolder->append ( pGroupItem->pCache );
      // Next we change the number on the VirtualFolder
      //pFolderItem = m_pListViewVirtual->firstChild ( );   //ooo
      pFolderItem = m_pListViewVirtual->topLevelItem ( 0 ); //xxx
      while ( pFolderItem ) {
        if  ( pFolderItem->text ( 0 ) == qsFolderName )
          break;
        //pFolderItem = pFolderItem->nextSibling ( );   //ooo
        pFolderItem = pFolderItem->parent()->child(pFolderItem->parent()->indexOfChild(pFolderItem)+1);	//xxx
      }
      if ( pFolderItem )
           pFolderItem->setText ( 1, QString ( "%1" ).arg ( pVirtualFolder->count ( ) ) );
      // And finally we give som visual feedback
      for ( t=0; t<9; t++ )  {
        //m_pVirtualFolderDialog->setSelected ( pItem, bSelected ); //ooo
        m_pVirtualFolderDialog->setCurrentItem ( pItem );           //xxx
        bSelected = ! bSelected;
        for ( i=0; i<10; i++ )  {
          usleep ( 8000 ); // 8 ms
          qApp->processEvents ( );
        }
      }
    }
    Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );
}

//void DialogFiles::slotToSourceGroup ( Q3ListBoxItem *pSelItem )   //ooo
//void DialogFiles::slotToSourceGroup ( QTreeWidgetItem *pSelItem )   //xxx
void DialogFiles::slotToSourceGroup ( QListWidgetItem *pSelItem )   //xxx
{  
  Thumbs       *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx
  bool       bSelected  = true;
  int        t, i;
  QFileInfo  fileInfo;
  QImage     theImage;
  QPixmap    thePixmap;
  GroupView *pGroupView = NULL;
  GroupView::Item *pItem;

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pSelItem ) {
    QString qsEntryName  =  pSelItem->text  ( );  //ooo
    //QString qsEntryName  =  pSelItem->text  ( 0 );  //xxx
    for  ( t=0; t<m_pToolbox->count  ( ); t++ ) {
      //if ( m_pToolbox->itemLabel    ( t ) == qsEntryName ) {  //ooo
      if ( m_pToolbox->itemText    ( t ) == qsEntryName ) {     //xxx
	//pGroupView = (GroupView *)m_pToolbox->item(  t  );   //ooo
    pGroupView = (GroupView *)m_pToolbox->widget(  t  );   //xxx
	break;
      }
    }
    if ( pGroupView ) {
      // First we have to get the selected items
      it = m_listOfThumbs.begin ( );
      while ( it != m_listOfThumbs.end ( ) ) {
	pThumbs = *it++;
	if ( pThumbs->pOwnerItem->isSelected ( ) ) {
	  fileInfo.setFile ( pThumbs->pCache->qsFileName );
	  if ( pThumbs->pCache )  {
	    if ( ! pThumbs->pCache->arrayOfThumbs )
	           pThumbs->pCache->loadImages  ( );
	    if (   pThumbs->pCache->getMatrix ( ) )  {
	      //theImage = pThumbs->pCache->arrayOfThumbs[0]->xForm ( *pThumbs->pCache->getMatrix ( ) );       //ooo
          theImage = pThumbs->pCache->arrayOfThumbs[0]->transformed ( *pThumbs->pCache->getMatrix ( ) );   //xxx
	      //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	      theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	    }
	    else if ( ( pThumbs->pCache->fRotate != 0.0 ) && ( pThumbs->pCache->bIsVideo ) )  {
              QMatrix matrix;
              matrix.rotate ( pThumbs->pCache->fRotate );
              //theImage = pThumbs->pCache->arrayOfThumbs[0]->xForm ( matrix );     //ooo
              theImage = pThumbs->pCache->arrayOfThumbs[0]->transformed ( matrix ); //xxx
              //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	      theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
            }
            else
	      //theImage = pThumbs->pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	      theImage = pThumbs->pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	    thePixmap.convertFromImage ( theImage );
	    //pGroupView->insertItem ( thePixmap, fileInfo.fileName ( ) );
	    pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), true );
	    //pItem->setPixmap ( 0, thePixmap );   //ooo
        pItem->setIcon ( 0, thePixmap );       //xxx
	    pItem->pCache  =  pThumbs->pCache;
	  }
	}
      } // end while loop
      // And finally we give som visual feedback
      for ( t=0; t<9; t++ )  {
	//m_pVirtualFolderDialog->setSelected ( pSelItem, bSelected ); //ooo
    m_pVirtualFolderDialog->setCurrentItem ( pSelItem );           //xxx
	bSelected = ! bSelected;
	for ( i=0; i<10; i++ )  {
	  usleep ( 8000 ); // 8 ms
	  qApp->processEvents ( );
	}
      }
    }
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );
}

//void DialogFiles::slotAllToSourceGroup ( Q3ListBoxItem *pSelItem )    //ooo
//void DialogFiles::slotAllToSourceGroup ( QTreeWidgetItem *pSelItem )    //xxx
void DialogFiles::slotAllToSourceGroup ( QListWidgetItem *pSelItem )    //xxx
{
  Thumbs       *pThumbs;
  bool       bSelected  = true;
  int        t, i;
  QFileInfo  fileInfo;
  QImage     theImage;
  QPixmap    thePixmap;
  GroupView *pGroupView  = NULL;
  GroupView::Item *pItem = NULL;

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pSelItem ) {
    QString qsEntryName  =  pSelItem->text  ( );  //ooo
    //QString qsEntryName  =  pSelItem->text  ( 0 );  //xxx
    for  ( t=0; t<m_pToolbox->count  ( ); t++ ) {
      //if ( m_pToolbox->itemLabel    ( t ) == qsEntryName ) {  //ooo
      if ( m_pToolbox->itemText    ( t ) == qsEntryName ) {     //xxx
	//pGroupView = (GroupView *)m_pToolbox->item(  t  );   //ooo
    pGroupView = (GroupView *)m_pToolbox->widget(  t  );   //xxx
	break;
      }
    }
    if ( pGroupView ) {
      // First we have to get the selected items

      //Q3IconViewItem *pIcon = m_pPreview->lastItem ( );   //oooo
      QListWidgetItem *pIcon = m_pPreview->item ( m_pPreview->count() );    //xxx
      while ( pIcon )  {
        pThumbs = findThumb ( pIcon );
        //pIcon = pIcon->prevItem ( );  //ooo
        pIcon = pIcon->listWidget ( )->item ( pIcon->listWidget ( )->row(pIcon) -1 ); //xxx
        if ( ! pThumbs )
          continue;

	fileInfo.setFile ( pThumbs->pCache->qsFileName );
	if ( pThumbs->pCache )  {
	  if ( ! pThumbs->pCache->arrayOfThumbs )
	         pThumbs->pCache->loadImages  ( );
	  if (   pThumbs->pCache->getMatrix ( ) )  {
	    //theImage = pThumbs->pCache->arrayOfThumbs[0]->xForm ( *pThumbs->pCache->getMatrix ( ) ); //ooo
        theImage = pThumbs->pCache->arrayOfThumbs[0]->transformed ( *pThumbs->pCache->getMatrix ( ) );  //xxx
	    //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
	    theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	  }
	  else if ( ( pThumbs->pCache->fRotate != 0.0 ) && ( pThumbs->pCache->bIsVideo ) )  {
            QMatrix matrix;
            matrix.rotate ( pThumbs->pCache->fRotate );
            //theImage = pThumbs->pCache->arrayOfThumbs[0]->xForm ( matrix );       //ooo
            theImage = pThumbs->pCache->arrayOfThumbs[0]->transformed ( matrix );   //xxx
            //theImage = theImage.smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );				//ooo
	    theImage = theImage.scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
          }
          else
	    //theImage = pThumbs->pCache->arrayOfThumbs[0]->smoothScale ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio );			//ooo
	  theImage = pThumbs->pCache->arrayOfThumbs[0]->scaled ( GroupView::m_iSize, GroupView::m_iSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	  thePixmap.convertFromImage ( theImage );
	  //pGroupView->insertItem ( thePixmap, fileInfo.fileName ( ) );
	  pItem = new GroupView::Item ( pGroupView, fileInfo.fileName ( ), true );
	  //pItem->setPixmap ( 0, thePixmap ); //ooo
      pItem->setIcon ( 0, thePixmap );     //xxx
	  pItem->pCache  =  pThumbs->pCache;
	}
      } // end while loop
      // And finally we give some visual feedback
      for ( t=0; t<9; t++ )  {
	//m_pVirtualFolderDialog->setSelected ( pSelItem, bSelected ); //ooo
    m_pVirtualFolderDialog->setCurrentItem ( pSelItem );           //xxx
	bSelected = ! bSelected;
	for ( i=0; i<10; i++ )  {
	  usleep ( 8000 ); // 8 ms
	  qApp->processEvents ( );
	}
      }
    }
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );
}

//void DialogFiles::slotSourceToSourceGroup ( Q3ListBoxItem *pSelItem ) //ooo
//void DialogFiles::slotSourceToSourceGroup ( QTreeWidgetItem *pSelItem ) //xxx
void DialogFiles::slotSourceToSourceGroup ( QListWidgetItem *pSelItem ) //xxx
{
  bool      bSelected  = true;
  int       t, i;
  QFileInfo fileInfo;
  QPixmap   thePixmap;
  GroupView *pTargetGroup = NULL;
  GroupView *pSourceGroup = NULL;
  GroupView::Item *pSourceItem, *pTargetItem;

  // This function is only called from m_pVirtualFolderDialog
  disconnect ( m_pVirtualFolderDialog );
  if ( pSelItem ) {
    // First we get th GroupView where we want to move this item to.
    QString qsEntryName  =  pSelItem->text ( );   //ooo
    //QString qsEntryName  =  pSelItem->text ( 0 );   //xxx
    for  ( t=0; t<m_pToolbox->count ( ); t++ ) {
      //if ( m_pToolbox->itemLabel   ( t ) == qsEntryName ) {   //ooo
      if ( m_pToolbox->itemText   ( t ) == qsEntryName ) {      //xxx
	//pTargetGroup = (GroupView *)m_pToolbox->item(  t  ); //ooo
    pTargetGroup = (GroupView *)m_pToolbox->widget(  t  );  //xxx
	break;
      }
    }
    //pSourceGroup = (GroupView *)m_pToolbox->currentItem ( );  //ooo
    pSourceGroup = (GroupView *)m_pToolbox->currentWidget ( );  //xxx
    if ( pTargetGroup && pSourceGroup )  {
      // First we have to get the selected items
      pSourceItem  = (GroupView::Item *)pSourceGroup->currentItem ( );
      if ( pSourceItem )  {
	// gotcha ...
	pTargetItem = new GroupView::Item ( pTargetGroup, pSourceItem->text ( 0 ), true );
	//const QPixmap *pPixmap = pSourceItem->pixmap ( 0 );  //oooo
    //const QPixmap *pPixmap = *pSourceItem->icon ( 0 ).pixmap ( QSize(22,22));      //xxx
    QPixmap pPixmap = pSourceItem->icon ( 0 ).pixmap(pSourceItem->icon ( 0 ).actualSize(QSize(32, 32)));      //xxx
	if ( &pPixmap )
	  //pTargetItem->setPixmap ( 0, *pPixmap );    //oooo
      pTargetItem->setIcon ( 0, pPixmap );    //xxx  
	pTargetItem->pSourceFileInfo = pSourceItem->pSourceFileInfo;
	pTargetItem->pCache          = pSourceItem->pCache;
	// FInally we can delete the source item
	delete pSourceItem;
      }

      // And finally we give som visual feedback
      for ( t=0; t<9; t++ )  {
	//m_pVirtualFolderDialog->setSelected ( pSelItem, bSelected ); //ooo
    m_pVirtualFolderDialog->setCurrentItem ( pSelItem );           //xxx
	bSelected = ! bSelected;
	for ( i=0; i<10; i++ )  {
	  usleep ( 8000 ); // 8 ms
	  qApp->processEvents ( );
	}
      }
    }
  }
  QTimer::singleShot ( 5, this, SLOT ( slotDeleteVFDialog ( ) ) );

  if ( pTargetGroup )
    QTimer::singleShot ( 20, pTargetGroup, SLOT ( slotAlternateColors ( ) ) );
  if ( pSourceGroup )
    QTimer::singleShot ( 20, pSourceGroup, SLOT ( slotAlternateColors ( ) ) );
}

void DialogFiles::slotAddVirtual ( )
{
  QString qsHeader = tr ( "Enter name for Virtual Folder" );
  QString qsLabel  = tr ( "Enter name : " );
  //QString qsNewFolder = QInputDialog::getText ( qsHeader, qsLabel );  //ooo
  QString qsNewFolder = QInputDialog::getText ( this, qsHeader, qsLabel );
  if ( qsNewFolder.isEmpty ( ) )
    return;

  unsigned int iFolderHash = Global::pThumbsCache->addVirtualFolder ( qsNewFolder, isImageDialog ( ) );
  if ( iFolderHash == 0 )
    return; // folder exists already.

  QString qsDate = QDateTime::currentDateTime ( ).toString ( "yyyy/MM/dd hh:mm:ss" );
  QStringList stringList;   //xxx
  stringList << qsNewFolder << "0" << qsDate;   //xxx
  //new Q3ListViewItem ( m_pListViewVirtual, qsNewFolder, "0",  qsDate );   //ooo
  new QTreeWidgetItem ( m_pListViewVirtual );    //xxx
  Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
}

void DialogFiles::slotCloneVirtual ( )
{
  //QList<Q3ListViewItem *>list = getSelectedVirtual ( );		//oxx
  QList<QTreeWidgetItem *>list = getSelectedVirtual ( );		//xxx

  for ( uint t=0; t<(uint)list.count ( ); t++ )  {	//ox
    //Q3ListViewItem *pItem = list[t];  //ooo
    QTreeWidgetItem *pItem = list[t];   //xxx
    QString qsFolderName = pItem ? pItem->text ( 0 ) : "";
    Cache::Thumbs::VirtualFolder *pFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );

    if ( pFolder ) {
      pFolder = pFolder->clone ( );
      QString qsDate = QDateTime::currentDateTime ( ).toString ( "yyyy/MM/dd hh:mm:ss" );
      QStringList stringList;   //xxx
      stringList << pFolder->qsFolderName << QString ( "%1" ).arg ( pFolder->count ( ) ) << qsDate; //xxx
      //new Q3ListViewItem ( m_pListViewVirtual, pFolder->qsFolderName, QString ( "%1" ).arg ( pFolder->count ( ) ),  qsDate );     //ooo
      new QTreeWidgetItem ( m_pListViewVirtual );      //xxx
    }
  }
  if ( list.count ( ) > 0 )
    Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
}

void DialogFiles::slotEditVirtual ( )
{
  //QList<Q3ListViewItem *> list = getSelectedVirtual ( );	//oxx
  QList<QTreeWidgetItem *> list = getSelectedVirtual ( );	//xxx
  if ( list.count ( ) != 1 )
    return;

  DialogVFolder theDialog ( this );

  //Q3ListViewItem *pItem = list[0];    //ooo
  QTreeWidgetItem *pItem = list[0];     //xxx
  QString qsFolderName = pItem ? pItem->text ( 0 ) : "";
  Cache::Thumbs::VirtualFolder *pFolder = Global::pThumbsCache->findVirtualFolder ( qsFolderName, isImageDialog ( ) );

  if ( pFolder ) {
    theDialog.initMe ( qsFolderName, isImageDialog ( ) );
    if ( theDialog.exec ( ) == QDialog::Accepted )  {
      // Next we get the info from the dialog and add it to the Virtual Folder
      pFolder->clear ( );
      QList<Cache::Thumbs::Entry *>list = theDialog.getEntries ( );	//oxx
      QList<Cache::Thumbs::Entry *>::iterator it;			//oxx
      it = list.begin ( );
      while ( it != list.end ( ) )
        pFolder->append (  *it++ );

      pItem->setText ( 1, QString ( "%1" ).arg ( list.count ( ) ) );
      Global::pThumbsCache->saveVirtualFolder ( isImageDialog ( ) );
    }
  }
}

void DialogFiles::slotDeleteVirtual ( )
{
  //QList<Q3ListViewItem *> list = getSelectedVirtual ( );	//oxx
  QList<QTreeWidgetItem *> list = getSelectedVirtual ( );	//xxx
  if ( list.count ( ) > 0 ) {
    QString qsFolderName;
    //Q3ListViewItem *pItem = NULL;     //ooo
    QTreeWidgetItem *pItem = NULL;      //xxx
    for ( uint t=0;  t<(uint)list.count ( ); t++ )	//ox
      qsFolderName += "\n" + list[t]->text ( 0 );
    if ( MessageBox::warning ( this, tr ( "Warning Delete Virtual Folder." ),
           tr ( "Are you sure you want to delete the Virtual Folder\n%1" ).arg ( qsFolderName ), 
           QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )  {
      for ( uint t=0; t<(uint)list.count ( ); t++ )  {		//ox
        pItem = list[t];
        delete pItem; 
      }
      //m_pButtonEditVirtual  ->setEnabled ( FALSE );   //ooo
      m_pButtonEditVirtual  ->setEnabled ( false );     //xxx
      //m_pButtonDeleteVirtual->setEnabled ( FALSE );   //ooo
      m_pButtonDeleteVirtual->setEnabled ( false );     //xxx
      Global::pThumbsCache->deleteVirtualFolder ( qsFolderName, isImageDialog ( ) );
    }
  }
}

//void DialogFiles::slotGroupContextMenu ( Q3ListViewItem *pSelItem, const QPoint &pos, int )   //ooo
void DialogFiles::slotGroupContextMenu ( QListWidgetItem *pSelItem )    //xxx
{ 
  QAction *toSourceGroupAction = new QAction("to &Source Group ...", this);
    
  QMenu *pMenu   = new QMenu  ( this );                 //xxx
  pMenu->addAction(toSourceGroupAction);    //xxx
  
  //connect(textNewAction, SIGNAL(triggered()), this, SLOT( slotAddTextObject  ( ) ) );   //xxx
  
  pMenu->exec( );   //xxx
  
  
  //Q3PopupMenu *pMenu   = new Q3PopupMenu  ( this );   //ooo
/*  int iIDs[8];
  iIDs[0] = pMenu->insertItem ( tr ( "to &Source Group ..." ),   1 ); // move this entry to different Group
  iIDs[1] = pMenu->insertItem ( tr ( "to &Virtual Folder ..." ), 2 ); // move this entry to Virtual Folder
  iIDs[2] = pMenu->insertItem ( tr ( "to &Preview -=>" ),        3 ); // move all entries to m_pPreview
  iIDs[3] = pMenu->insertItem ( tr ( "All &to Preview -=>" ),    4 ); // move all entries to m_pPreview
  if ( ! isImageDialog ( ) )
    iIDs[4] = pMenu->insertItem ( tr ( "&Play" ),                5 ); // play in player
  else
    iIDs[4] = pMenu->insertItem ( tr ( "C&omments ..." ),        5 ); // Edit Comment
  pMenu->insertSeparator ( );
  iIDs[5] = pMenu->insertItem ( tr ( "&Add" ),                   6 ); // add a new entry
  iIDs[6] = pMenu->insertItem ( tr ( "&Remove" ),                7 ); // remove this one entry
  iIDs[7] = pMenu->insertItem ( tr ( "&Clear All" ),             8 ); // simply remove entries

  if ( ! pSelItem ) {
    pMenu->setItemEnabled ( iIDs[0], false );
    pMenu->setItemEnabled ( iIDs[1], false );
    pMenu->setItemEnabled ( iIDs[2], false );
    pMenu->setItemEnabled ( iIDs[4], false );
    pMenu->setItemEnabled ( iIDs[6], false );
  }

  int iID = pMenu->exec  ( pos );

  GroupView::Item *pItem = (GroupView::Item *)pSelItem;
  GroupView *pGroupView  = (GroupView *)m_pToolbox->currentItem ( );

  if ( iID == 1 )       // to Source 
    toSourceGroup  ( 1 );
  else if ( iID == 2 )  // to Virtual
    toVirtualFolder( 1 );
  else if ( iID == 3 )  // to Preview
    currentSourceToPreview ( );
  else if ( iID == 4 )  // to Preview
    allSourcesToPreview    ( );
  else if ( iID == 5 )  // Play
    play  ( pItem->pCache );
  else if ( iID == 6 )  // Add
    addSourceToGroup ( pGroupView );
  else if ( iID == 7 )  // Remove
    delete pItem;
  else if ( iID == 8 ){ // Clear All
    GroupView::Item *pGroupItem, *pTemp;
    iID = m_pToolbox->currentIndex ( );
    QString qsLabel = m_pToolbox->itemLabel ( iID );
    if ( MessageBox::warning ( this, tr ( "Warning Removing all Sources." ),
	   tr ( "Are you sure you want to remove all sources from group\n%1" ).arg ( qsLabel ), 
           QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )  {
      if ( pGroupView )  {
	pGroupItem = (GroupView::Item *)pGroupView->firstChild ( );
	while ( pGroupItem ) {
	  pTemp = (GroupView::Item *)pGroupItem->nextSibling ( );
	  delete  pGroupItem;
	  pGroupItem = pTemp;
	}
      }
    }
  }
*/  //oooo
  delete pMenu;
}

void DialogFiles::slotPlay ( )
{  
  Thumbs *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx

  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) ) {
      play  ( pThumbs->pCache, pThumbs->pOwnerItem );
      return;
    }
  }
}

//void DialogFiles::slotDirectoryChanged ( Q3ListViewItem *pItem )  //ooo
void DialogFiles::slotDirectoryChanged ( QTreeWidgetItem *pItem, int )   //xxx
{  
  DirItem *pDirItem = (DirItem *)pItem;
  m_pActiveQuickButton = NULL;

  setCurrentDir ( pDirItem->dir ( ) );
  //debug_cout ( "DialogFiles::slotDirectoryChanged <%s>\n", pDirItem->dir ( ).ascii ( ) ); //ooo
  debug_cout ( "DialogFiles::slotDirectoryChanged <%s>\n", pDirItem->dir ( ).toLatin1 ( ).data ( ) );   //xxx
}

void DialogFiles::slotAutoOpen ( )
{
  // This function is called from m_pTimerAutoOpen
  printf ( "AutoOpen NOW ...\n" );
}

void DialogFiles::slotQuickDir()
{  
  // here we handle the quick directory buttons on the left side
  // of this dialog. They ought to be tagglod uniquely.
  QPushButton *pButtonArray[6] = {m_pButtonProject, m_pButtonDocuments, m_pButtonDesktop,
				  m_pButtonHome, m_pButtonRoot, m_pButtonTemp};
  QString *pStringArray[6] = {&m_qsProjectDir, &m_qsDocumentsDir, &m_qsDesktopDir,
			      &m_qsHomeDir, &m_qsRootDir, &m_qsTempDir};

  int t;

  setVirtualFolder ( -1 );
  for (t=0;t<6;t++)	{
    //if ( ( pButtonArray[t]->isOn ( ) ) &&     //ooo
    if ( ( pButtonArray[t]->isChecked ( ) ) &&  //xxx
	 ( pButtonArray[t] != m_pActiveQuickButton ) ) {

      setCurrentDir ( *pStringArray[t] );
    }
  }
}

void DialogFiles::setQuickDir ( int iCurrent )
{
  QPushButton *pButtonArray[6] = {m_pButtonProject, m_pButtonDocuments, m_pButtonDesktop,
				  m_pButtonHome, m_pButtonRoot, m_pButtonTemp};
  QLabel *pLabelArray[6] = {m_pLabelProject, m_pLabelDocuments, m_pLabelDesktop,
			    m_pLabelHome, m_pLabelRoot, m_pLabelTemp};

  QPalette palette;		//xxx
  int t;
  for  ( t=0; t<6; t++ )  {
    if ( t == iCurrent )  {
      m_pActiveQuickButton = pButtonArray[t];
      //pButtonArray[t]->setOn (true);  //oooo
      pButtonArray[t]->setDown (true);  //xxx
      //pButtonArray[t]->setPaletteBackgroundColor( QColor( COLOR_QUICK_SELECTED ) );   //ooo
      //QPalette palette;								//xxx
      palette.setBrush(QPalette::Base, QColor( COLOR_QUICK_SELECTED ));	//xxx
      pButtonArray[t]->setPalette(palette);						//xxx
      //pLabelArray[t] ->setPaletteBackgroundColor( QColor( COLOR_QUICK_SELECTED ) );   //ooo
      pLabelArray[t]->setPalette(palette);						//xxx
    }
    else  {
      //pButtonArray[t]->setOn (false); //oooo
      pButtonArray[t]->setDown(false); //xxx
      //pButtonArray[t]->setPaletteBackgroundColor( QColor( COLOR_QUICK_NORMAL ) ); //ooo
      palette.setBrush(QPalette::Base, QColor( COLOR_QUICK_NORMAL ));	//xxx
      pButtonArray[t]->setPalette(palette);						//xxx
      //pLabelArray[t] ->setPaletteBackgroundColor( QColor( COLOR_QUICK_NORMAL ) ); //ooo
      pLabelArray[t]->setPalette(palette);						//xxx
    }
  }
}

void DialogFiles::setCurrentDir ( QString qsDir )
{   
  int t, iCurrent = -1;
  for ( t=0; t<6; t++ )  {
    if ( qsDir == m_qsProjectDir )
      iCurrent = 0;
    else if ( qsDir == m_qsDocumentsDir )
      iCurrent = 1;
    else if ( qsDir == m_qsDesktopDir )
      iCurrent = 2;
    else if ( qsDir == m_qsHomeDir )
      iCurrent = 3;
    else if ( qsDir == m_qsRootDir )
      iCurrent = 4;
    else if ( qsDir == m_qsTempDir )
      iCurrent = 5;
  }

  if ( qsDir.right ( 1 ) != "/" )
    qsDir += "/";
  m_qsCurrentDir        = qsDir;
  Global::qsCurrentPath = qsDir;

  // We switch directories, so lets skip out of the preview generation ...
  MediaCreator::unregisterFromMediaScanner ( this );

  m_iProgress = 0;
  m_pProgressBar->reset ( );
  //m_pProgressBar->setTotalSteps ( 0 );    //oooo
  m_pProgressBar->setValue ( 0 );           //xxx

  setQuickDir ( iCurrent );

  setTreeDir  (    qsDir );

  m_pTimerScanDir->start ( 333 ); // set timer to 333 ms
}

QString DialogFiles::currentDir ( )
{
  return m_qsCurrentDir;
}

void DialogFiles::setTreeDir ( QString qsDir )
{  
  uint t;
  //QStringList treeList = QStringList::split ( '/', qsDir );   //ooo
  QStringList treeList = qsDir.split ( '/', QString::SkipEmptyParts );                   //xxx

  DirItem    *pSubDir;
  //DirItem    *pDirItem = (DirItem *)m_pTreeList->firstChild ( );  //oooo
  DirItem    *pDirItem = (DirItem *)m_pTreeList->topLevelItem ( 0 );    //xxx
  //DirItem    *pDirItem = (DirItem *)m_pTreeList->topLevelItem ( 0 )->child ( 0 );    //xxx
  //DirItem    *pDirItem = (DirItem *)m_pTreeList->selectedItems ( )[0];    //xxx
  

  QCursor myCursor     ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );
    
  for ( t=0; t<(uint)treeList.count ( ); t++ ) {	//ox 
    pSubDir = pDirItem->findSubDir ( treeList[t] );
    if ( ! pSubDir ) {    //ooo
    //if ( pSubDir ) {        //xxx
      pDirItem->setOpen ( true );     //ooo
      //pDirItem->setExpanded ( true );   //xxx
      //t++;  //xxx
      pSubDir = pDirItem->findSubDir ( treeList[t] ); //ooo
      //pSubDir = pDirItem->findSubDir ( treeList[t++] ); //xxx
      if ( ! pSubDir )    //ooo
      //if ( pSubDir ) {       //xxx
        //pDirItem = pSubDir; //xxx
        break; // exit for - loop.  //ooo
      //}
    }
    pDirItem = pSubDir;
  }

  if ( pDirItem ) {
    //m_pTreeList->ensureItemVisible ( pDirItem );  //oooo
    //m_pTreeList->setSelected ( pDirItem, TRUE );  //ooo
    m_pTreeList->setCurrentItem ( pDirItem );       //xxx
    //m_pTreeList->addTopLevelItem ( pDirItem );       //xxx
    pDirItem->setOpen ( true );   //ooo
    //pDirItem->setExpanded ( true ); //xxx
    //m_pTreeList->currentItem()->setExpanded ( true ); //xxx
  }
  QApplication::restoreOverrideCursor ( );
}

/**************************************************
 **
 ** Here are the functions for the VirtualFolder tab
 **
 **************************************************/
void DialogFiles::slotSelectByStars ( )
{
  // The user wants to see only the videos 
  // with the selected numbe of stars
  QPushButton *array[] = { m_pButtonStar1, m_pButtonStar2, m_pButtonStar3, m_pButtonStar4, m_pButtonStar5, m_pButtonStar6 };
  QPushButton *pButton = NULL;

  setQuickDir ( -1 ); // This will set all QuickDirButtons to unselected
  m_qsCurrentDir = "";
  m_pActiveQuickButton = NULL;
  uint t, iCurrent = 0;
  for  ( t=0; t<6; t++ )  {
    //if ( array[t]->hasMouse ( ) ) {   //ooo
    if ( array[t]->hasMouseTracking ( ) ) { //xxx
      iCurrent = t;
      pButton  = array[t];
      setVirtualFolder ( t );
    }
  }
  iCurrent ++;

  if ( ! pButton )
    return;

  if ( ! lockDialog ( ) )
    return;

  clearPreview ( );
  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  QList<Cache::Thumbs::Entry *>::iterator it;									//oxx
  QList<Cache::Thumbs::Entry *>list = Global::pThumbsCache->getThumbsByStar ( iCurrent, isImageDialog ( ) );	//oxx
  it = list.begin ( );
  while ( it != list.end ( ) )
    createFromCache  ( *it++ );

  updateTitlebar ( );
  QApplication::restoreOverrideCursor ( );
  unlockDialog ( );
}

// Allways protected by lockDialog ( )
DialogFiles::Thumbs *DialogFiles::createFromCache ( Cache::Thumbs::Entry *pCache )
{
  if ( ! pCache )
    return NULL;

  //Q3IconViewItem *pItem;  //ooo
  QListWidgetItem *pItem;   //xxx
  QString   qsFileName;

  m_pButtonAutoGroup->setEnabled ( true );

  //pItem = new Q3IconViewItem ( m_pPreview );  //ooo
  pItem = new QListWidgetItem ( m_pPreview );   //xxx
  //setKey ( pItem, pCache->qsFileName );   //oooo
  pItem->setText ( pCache->qsFileName ); //xxx

  Thumbs *pThumbs       = new Thumbs;
  pThumbs->pOwnerItem   = pItem;
  pThumbs->iNumberOfPix = pCache->iNumberOfThumbs;
  pThumbs->arrayOfPix   = new QPixmap *[pCache->iNumberOfThumbs];
  pThumbs->pCache       = pCache;
  if ( ! pCache->arrayOfThumbs )
         pCache->loadImages  ( );

  for ( uint i=0; i<pCache->iNumberOfThumbs; i++ ) {
    pThumbs->arrayOfPix[i] = new QPixmap;
    createPix ( pThumbs, i );
  }

  addPlayButton ( pItem, pThumbs->arrayOfPix[0] );
  setIconText   ( pThumbs );
  append ( pThumbs );
  return   pThumbs;
}

//void DialogFiles::markIcon ( Q3IconViewItem *pItem, int iWhich )  //ooo
void DialogFiles::markIcon ( QListWidgetItem *pItem, int iWhich )   //xxx
{
  if ( ( ! pItem ) || ( iWhich > 2 ) )
    return;

  QPixmap copy;
  QImage  play;
  //copy = *( pItem->pixmap ( ) );  //ooo
  copy = pItem->icon ( ).pixmap ( QSize(22,22));    //xxx
  QPainter painter  ( &copy );

  if ( iWhich == 1 )
    //play = QImage ( ).fromMimeSource ( "create_thumbs.png" );		//ooo
    play = QImage ( ":/images/create_thumbs.png" );			//xxx
  else
    //play = QImage ( ).fromMimeSource ( "scan_dv.png" );		//ooo
    play = QImage ( ":/images/scan_dv.png" );				//xxx

  //play = play.smoothScale ( m_iThumbnailSize, m_iThumbnailSize );							//ooo
  play = play.scaled ( m_iThumbnailSize, m_iThumbnailSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );		//xxx
  painter.drawImage ( 0, 0, play );
  //pItem->setPixmap  ( copy ); //ooo
  pItem->setIcon  ( copy );  //xxx
}

//void DialogFiles::setIconText ( Q3IconViewItem *pItem )   //ooo
void DialogFiles::setIconText ( QListWidgetItem *pItem )    //xxx
{  
  if ( ! pItem )
    return;

  if ( m_bTooSmallForText )  {
    //pItem->setText ( QString::null );     //ooo
    pItem->setText ( QString::null );    //xxx
    return;
  }

  QString   qsText;
  //QFileInfo fileInfo ( getKey ( pItem ) );		//ooo
  QFileInfo fileInfo( pItem->text ( ) );			//xxx

  if ( m_bName )
    qsText = fileInfo.fileName ( );

  if ( m_bDate )
    qsText += ( m_bName ? tr ( "\nD=" ) : tr ( "D=" ) ) + fileInfo.created ( ).toString ( "yy/MM/dd" );
  if ( m_bStars )
    qsText += ( ( m_bName || m_bDate ) ? tr ( "\nT=" ) : tr ( "T=" ) ) + fileInfo.created ( ).toString ( "hh:mm:ss" );

  if ( m_bLength )
    qsText += ( m_bName || m_bDate || m_bStars ) ? tr( "\nL=XX:XX:XX" ) : tr ( "L=XX:XX:XX" );

  //pItem->setText ( qsText );  //ooo
  pItem->setText ( qsText ); //xxx
}

void DialogFiles::setIconText ( DialogFiles::Thumbs *pThumbs )
{  
  if ( ( ! pThumbs ) || ( ! isValid ( pThumbs ) ) )
    return;

  if ( m_bTooSmallForText )  {
    //pThumbs->pOwnerItem->setText ( QString::null );   //ooo
    pThumbs->pOwnerItem->setText ( QString::null );  //xxx
    return;
  }

  QString   qsText;
  //QFileInfo fileInfo ( getKey ( pThumbs->pOwnerItem ) );	//ooo
  QFileInfo fileInfo ( pThumbs->pOwnerItem->text ( ) );		//xxx

  if ( m_bName )
    qsText = fileInfo.fileName ( );

  if ( m_bDate )
    qsText += ( m_bName ? tr ( "\nD=" ) : tr ( "D=" ) ) + pThumbs->pCache->dateCreated.toString ( "yy/MM/dd" );
  if ( m_bStars )
    qsText += ( ( m_bName || m_bDate ) ? tr ( "\nT=" ) : tr ( "T=" ) ) + pThumbs->pCache->dateCreated.toString ( "hh:mm:ss" );

  if ( m_bLength )
    qsText += ( ( m_bName || m_bDate || m_bStars ) ? tr ( "\nL=" ) : tr ( "L=" ) ) + pThumbs->pCache->qsLength;

  //pThumbs->pOwnerItem->setText ( qsText );    //ooo
  pThumbs->pOwnerItem->setText ( qsText );   //xxx
}

bool DialogFiles::isValid ( DialogFiles::Thumbs *pThumbs )
{
  QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end ( ) )  {
    if ( pThumbs == *it++ )
      return true;
  }
  return false;
}

//bool DialogFiles::isValid ( Q3IconViewItem *pItem )   //ooo
bool DialogFiles::isValid ( QListWidgetItem *pItem )    //xxx
{
  //Q3IconViewItem *pExisting = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pExisting = m_pPreview->item ( 0 );  //xxx
  while ( pExisting ) {
    if  ( pExisting == pItem )
      return true;
    //pExisting = pExisting->nextItem ( );  //ooo
    pExisting = pExisting->listWidget()->item ( pExisting->listWidget()->row(pExisting) + 1 );   //xxx
  }
  return false;
}

//bool DialogFiles::removeThumb ( Q3IconViewItem *pItem )   //ooo
bool DialogFiles::removeThumb ( QListWidgetItem *pItem )    //xxx
{
  Thumbs   *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if  ( pThumbs->pOwnerItem == pItem )  {
      if( m_pActiveThumbs   == pThumbs )
          stopThumbing ( );
      //m_listOfThumbs.remove  ( pThumbs ); //ooo
      m_listOfThumbs.removeOne  ( pThumbs );    //xxx
      delete pThumbs;
      return true;
    }
  }
  return false;
}

//DialogFiles::Thumbs *DialogFiles::findThumb ( Q3IconViewItem *pItem ) //ooo
DialogFiles::Thumbs *DialogFiles::findThumb ( QListWidgetItem *pItem )  //xxx
{
  Thumbs   *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if  ( pThumbs->pOwnerItem == pItem )  {
      return pThumbs;
    }
  }
  return NULL;
}

//void DialogFiles::addPlayButton ( Q3IconViewItem *pItem, QPixmap *pPixmap )   //ooo
void DialogFiles::addPlayButton ( QListWidgetItem *pItem, QPixmap *pPixmap )    //xxx
{   
  if ( ! pItem )
    return;

  int iX, iY, iFrameHeight, iDotSize;
  QPixmap copy;

  if ( pPixmap ) {   
    copy = *pPixmap;
  } else {
    //copy = *(pItem->pixmap ( ) ); //ooo
    copy = pItem->icon ( ).pixmap ( QSize(22,22));    //xxx
  }

  if ( ( 0 ) && ( m_iThumbnailSize > 50 ) ) {    
    // Hmmm, this looks nice but prevents the >Preview< part of the thumbnails
    // So for now we'll disable this feature
    //QImage  semi ( m_iThumbnailSize, m_iThumbnailSize, 32 );  //ooo
    QImage  semi ( m_iThumbnailSize, m_iThumbnailSize, QImage::Format_RGB32 );  //xxx
    //QImage  play  =  QImage  ( ).fromMimeSource ( "button_play.png" ); //( "thumbs_play.png" );	//ooo
    QImage  play  =  QImage ( ":/images/button_play.png" ); //( "thumbs_play.png" );			//xxx
    QPainter painter   ( &copy );

    semi.fill    ( 0xAAAAAAFF  );
    //semi.setAlphaBuffer ( true ); //oooo

    // Draw the PlayButton
    iX = (int)( ( m_iThumbnailSize - play.width  ( ) ) / 2.0 ) + 2;
    iY = (int)( ( m_iThumbnailSize - play.height ( ) ) / 2.0 ) + 2;
    painter.drawImage (  2,  2, semi );
    painter.drawImage ( iX, iY, play );

    // Draw the MovieFrame
    QBrush blackBrush ( QColor (   0,   0,   0 ) );
    QBrush whiteBrush ( QColor ( 220, 220, 255 ) );
    iDotSize     =  6;
    iFrameHeight = 12;
    iY = m_iThumbnailSize - iFrameHeight + 2;
    iX = 4;
    painter.fillRect  ( 2,  2, m_iThumbnailSize, iFrameHeight, blackBrush );
    painter.fillRect  ( 2, iY, m_iThumbnailSize, iFrameHeight, blackBrush );

    while ( iX < (int)m_iThumbnailSize ) {
      painter.fillRect ( iX,    5, iDotSize, iDotSize, whiteBrush );
      painter.fillRect ( iX, iY+3, iDotSize, iDotSize, whiteBrush );
      iX += iDotSize + 4;
    }
  }

  //pItem->setPixmap ( copy );  //ooo
  pItem->setIcon ( copy );      //xxx
}

// Allways proteced through lockDialog ( )
void DialogFiles::recreateThumbs ( Thumbs *pThumbs )
{
  if ( ( ! pThumbs ) || ( ! pThumbs->pCache ) )
    return;

  if ( ! pThumbs->pCache->arrayOfThumbs )
         pThumbs->pCache->loadImages  ( );

  for ( uint i=0; i < pThumbs->pCache->iNumberOfThumbs; i++ )
    createPix ( pThumbs, i );

  //pThumbs->pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[pThumbs->iCurrentPix] );  //ooo
  pThumbs->pOwnerItem->setIcon ( *pThumbs->arrayOfPix[pThumbs->iCurrentPix] ); //xxx
}

//SourceFileInfo *DialogFiles::findSourceFileInfo ( Q3IconViewItem *pIcon ) //ooo
SourceFileInfo *DialogFiles::findSourceFileInfo ( QListWidgetItem *pIcon )  //xxx
{
  Thumbs   *pThumbs = NULL;
  QList<Thumbs *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if  ( pThumbs->pOwnerItem == pIcon )
      return  pThumbs->pSourceFileInfo;
  }
  return NULL;
}

void DialogFiles::setVirtualFolder ( int iFolder )
{
  // The user wants to see only the videos 
  // with the selected numbe of stars
  QPushButton *array[] = { m_pButtonStar1, m_pButtonStar2, m_pButtonStar3, m_pButtonStar4, m_pButtonStar5, m_pButtonStar6 };
  QPalette palette;		//xxx
  int t;
  for ( t=0; t<6; t++ )  {
    if ( t == iFolder )  {
      //array[t]->setOn (  true );  //oooo
      //array[t]->setPaletteBackgroundColor( QColor( COLOR_QUICK_SELECTED ) );  //ooo
      palette.setBrush(QPalette::Base, QColor( COLOR_QUICK_SELECTED ));	//xxx
      array[t]->setPalette(palette);						//xxx
    }
    else {
      //array[t]->setOn ( false );  //oooo
      //array[t]->setPaletteBackgroundColor( QColor( COLOR_QUICK_NORMAL ) );
      palette.setBrush(QPalette::Base, QColor( COLOR_QUICK_SELECTED ));	//xxx
      array[t]->setPalette(palette);						//xxx
    }
  }
}

/**************************************************
 **
 ** Here the actual work starts.
 **
 ** first we are going to read in the directory,
 ** - displaying all movies with a temp thumb
 ** - scanning in the first frame of each movie
 ** - scan in X more preview thumbs per movie
 **
 **************************************************/

void DialogFiles::slotScanDir ( )
{  
  // called from the following timer from within setCurrentDir
  m_pTimerScanDir->stop ( );

  QDir        theDir ( m_qsCurrentDir );
  //QString     qsFilter    = getDirFilter ( ); //ooo
  QStringList     qsFilter ( getDirFilter ( ) );   //xxx
  //QStringList listOfFiles = theDir.entryList ( qsFilter, QDir::Files );   //ooo
  QStringList listOfFiles = theDir.entryList ( QDir::Files, QDir::Name );   //xxx
  QStringList listOfVideos;

  QStringList::Iterator it;
  it = listOfFiles.begin ( );
  // Make sure we always have the abs Path in m_qsCurrentPath
  //m_qsCurrentDir = theDir.absPath ( ) + "/";  //ooo
  m_qsCurrentDir = theDir.absoluteFilePath ( m_qsCurrentDir );   //xxx

  while ( it != listOfFiles.end ( ) ) {   
    listOfVideos.append ( m_qsCurrentDir + *it++ );
  }

  clearPreview ( );
  load ( listOfVideos );
}

QString DialogFiles::getDirFilter ( )
{
  return Global::pApp->getMovieFilter ( );    //oooo
  //return Global::pApp->getImageFilter ( );      //xxx
}

void DialogFiles::load ( QStringList &listOfVideos )
{ 
  QImage    theImage;
  QPixmap   thePixmap;
  QString   qsFileName, qsFile;
  QFileInfo fileInfo;
  //Q3IconViewItem *pItem = NULL;   //ooo
  QListWidgetItem *pItem = NULL;    //xxx

  if ( listOfVideos.count ( ) < 1 ) {
    m_pButtonAutoGroup->setEnabled ( false );
    return;
  }
  m_pButtonAutoGroup->setEnabled ( true );

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  m_iProgress            = 0;
  m_bExitScannFunction   = true; // DialogImages. Exit loop
  m_pProgressBar->reset ( );
  //m_pProgressBar->setTotalSteps ( listOfVideos.count ( ) );   //oooo
  m_pProgressBar->setValue ( listOfVideos.count ( ) );          //xxx

  // First we should remove outstanding tasks ...
  MediaCreator::unregisterFromMediaScanner ( this );

  if ( ! lockDialog ( ) )
    return;

  //theImage = QImage ( QImage ( ).fromMimeSource ( "please_wait.jpg" ) );	//ooo
  theImage = QImage ( ":/images/please_wait.jpg" );				//xxx
  
  createPix ( &thePixmap, &theImage );

  bool bIsImage = false;
  Cache::Thumbs::Entry *pCache = NULL;
  // get the first frame as the initial thumbnail.
  QStringList::iterator it = listOfVideos.begin ( );
  m_iVidCounter = 0;

  while ( it != listOfVideos.end ( ) )  {    
    qsFileName = *it++;
    fileInfo.setFile ( qsFileName );
    qsFile = fileInfo.fileName  ( );
  
    //pItem  = new  Q3IconViewItem ( m_pPreview ); //, qsFile+"\nXX:XX:XX" );   //ooo
    pItem  = new  QListWidgetItem ( m_pPreview ); //, qsFile+"\nXX:XX:XX" );    //xxx
    //pItem->setText ( 0, QString ( "Here should be an Icon" ) );     //xxx
    //thePixmap.convertFromImage ( theImage );    //xxx
    pItem->setIcon ( thePixmap );  //xxx
  
    //setKey ( pItem, qsFileName ); // pItem->text will be changed after scan returns with a preview    //oooo
    pItem->setText ( qsFileName ); //xxx
    //pItem->setText ( 0, qsFile ); //xxx
    //pItem->setToolTip ( 0, QString("They are there") ); //xxx
    bIsImage = isImageDialog ( qsFileName );
  
    pCache = Global::pThumbsCache->find ( qsFileName, bIsImage ); //oooo
  
    if ( ! bIsImage && isImageDialog ( ) )
      m_iVidCounter++;

    if ( pCache )  {      
      Thumbs  *pThumbs      = new Thumbs;
      pThumbs->pOwnerItem   = pItem;
      pThumbs->iNumberOfPix = pCache->iNumberOfThumbs;
      pThumbs->arrayOfPix   = new QPixmap *[pCache->iNumberOfThumbs];
      pThumbs->pCache       = pCache;
      if ( ! pCache->arrayOfThumbs )
             pCache->loadImages  ( );

      for ( uint i=0; i<pCache->iNumberOfThumbs; i++ )  {
        pThumbs->arrayOfPix[i] = new QPixmap;
        createPix ( pThumbs, i );
      }
  
      addPlayButton ( pItem, pThumbs->arrayOfPix[0] );
    
      setIconText   ( pThumbs );
      
   
      append ( pThumbs );
      
   
      m_iProgress++;
    }
    else  {   //ooo     
      // We could not find the information in the cache so we have to create it
      //pItem->setPixmap ( thePixmap ); //ooo
      pItem->setIcon ( thePixmap );  //xxx

      setIconText          ( pItem );
   
      newThumbnailRequest  ( pItem ); //oooo
    } //ooo
    //m_pProgressBar->setProgress ( m_iProgress );  //ooo
    m_pProgressBar->setValue ( m_iProgress );       //xxx
  }

  //m_pPreview->arrangeItemsInGrid ( TRUE );    //oooo
  //m_pPreview->arrangeItemsInGrid ( true );      //xxx
  m_pPreview->sortItems ( Qt::AscendingOrder );      //xxx

  //m_pProgressBar->setProgress ( m_iProgress );    //ooo
  m_pProgressBar->setValue ( m_iProgress );         //xxx
  updateTitlebar ( );

  unlockDialog   ( );
  QApplication::restoreOverrideCursor     ( );
}

void DialogFiles::createPix ( Thumbs *pThumbs, uint iIdx )
{  
  if ( ! pThumbs || ! pThumbs->pCache )
    return;

  if ( ! isValid ( pThumbs->pOwnerItem ) )
    return;

  float fPos = 0.0f;
  Cache::Thumbs::Entry *pCache = pThumbs->pCache;

  if ( pCache->iNumberOfThumbs > 1 ) {
       fPos = (float)iIdx / (pCache->iNumberOfThumbs-1);
  }

  if ( pCache->getMatrix ( ) ) {   
    //QImage tempImage = pCache->arrayOfThumbs[iIdx]->xForm ( *pCache->getMatrix ( ) ); //ooo
    QImage tempImage = pCache->arrayOfThumbs[iIdx]->transformed ( *pCache->getMatrix ( ) ); //xxx
    createPix ( pThumbs->arrayOfPix[iIdx], &tempImage, fPos, pCache->iStarRating );
  }
  else if ( ( pCache->fRotate != 0.0 ) && ( pCache->bIsVideo ) )  {   
    QMatrix matrix;
    matrix.rotate    ( pCache->fRotate );
    //QImage tempImage = pCache->arrayOfThumbs[iIdx]->xForm ( matrix ); //ooo
    QImage tempImage = pCache->arrayOfThumbs[iIdx]->transformed ( matrix ); //xxx
    createPix ( pThumbs->arrayOfPix[iIdx], &tempImage, fPos, pCache->iStarRating );
  }
  else {  
    createPix ( pThumbs->arrayOfPix[iIdx], pCache->arrayOfThumbs[iIdx], fPos, pCache->iStarRating );
  }
}

void DialogFiles::createPix ( QPixmap *pTarget, QImage *pSource, float fPosition, int iStarRating )
{ 
  if ( ( ! pTarget ) || ( ! pSource ) )
    return;

  int iBorder    = 2; // add 2 * 2 = 4 to m_iThumbnailSize
  int iThumbSize = m_iThumbnailSize + 2 * iBorder;
  int iPosLength = 0;
  int iWidth, iHeight, iX, iY;
  QPixmap scaledPix;
  QImage  scaledImage;
  
  //scaledImage = pSource->smoothScale ( m_iThumbnailSize, m_iThumbnailSize, Qt::KeepAspectRatio );				//ooo
  scaledImage = pSource->scaled ( m_iThumbnailSize, m_iThumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );		//xxx
  scaledPix.convertFromImage ( scaledImage );

  iPosLength = (int)(fPosition * iThumbSize);
  *pTarget   = QPixmap   ( iThumbSize, iThumbSize + m_pixIconStars[0].height ( ) );

  QPainter thePainter    (    pTarget    );
  QColor   clrBlue       ( 120, 120, 255 );
  QColor   clrDBlue      (  20,  20, 180 );
  pTarget->fill ( QColor ( 222, 222, 222 ) );
  iWidth  = scaledPix.width  ( );
  iHeight = scaledPix.height ( );
  iX = (int)((iThumbSize - iWidth ) / 2.0 );
  iY = (int)((iThumbSize - iHeight) / 2.0 );

  // First we center the image and copy it over
  //copyBlt ( pTarget, iX, iY, &scaledPix, 0, 0, iWidth, iHeight );     //ooo
  thePainter.drawPixmap ( iX, iY, scaledPix, 0, 0, iWidth, iHeight );   //xxx
  
  // Next we draw the Frame
  thePainter.setPen   ( clrBlue  );
  thePainter.drawLine (            0,            0, iThumbSize-1,            0 );
  thePainter.drawLine ( iThumbSize-1,            0, iThumbSize-1, iThumbSize-1 );
  thePainter.drawLine ( iThumbSize-1, iThumbSize-1,            0, iThumbSize-1 );
  thePainter.drawLine (            0, iThumbSize-1,            0,            0 );

  // next we check for StarRatings icons
  if ( iStarRating  < 0 )
       iStarRating  = 0;
  //if ( iStarRating >= 0 )
  //  copyBlt ( pTarget, 0, m_iThumbnailSize+4, &m_pixIconStars[iStarRating] ); //ooo
  thePainter.drawPixmap ( 0, m_iThumbnailSize+4, m_pixIconStars[iStarRating] ); //xxx

  // and lastly we draw the progress bar
  if ( iPosLength > 0 ) {
    thePainter.setPen   ( clrDBlue );
    thePainter.drawLine ( 0, iThumbSize+0,  iPosLength, iThumbSize+0 );
    thePainter.drawLine ( 0, iThumbSize+1,  iPosLength, iThumbSize+1 );
  }   //oooo
}

void DialogFiles::slotSizeReleased ( )
{
  stopThumbing ( );
  bool bOkay;
  int  iNewSize = m_pEditSize->text ( ).toInt ( &bOkay );
  if ( bOkay ) {
    m_iThumbnailSize = iNewSize;
    resizeIcons ( iNewSize );
  }
}

void DialogFiles::slotSizeChanged ( int iNewSize )
{
  m_pEditSize->setText  ( QString ( "%1" ).arg ( iNewSize ) );
  m_iThumbnailSize  = iNewSize;
}

void DialogFiles::slotThumbing ( )
{
  // called from the timer every 1.5 seconds
  if ( ! m_pActiveThumbs ) {
    stopThumbing ( );
    return;
  }
  // Increase the current counter.
  m_pActiveThumbs->iCurrentPix ++;
  if ( m_pActiveThumbs->iCurrentPix >= m_pActiveThumbs->iNumberOfPix )
       m_pActiveThumbs->iCurrentPix  = 0;
  //m_pActiveThumbs->pOwnerItem->setPixmap ( *m_pActiveThumbs->arrayOfPix[m_pActiveThumbs->iCurrentPix] );  //ooo
  m_pActiveThumbs->pOwnerItem->setIcon ( *m_pActiveThumbs->arrayOfPix[m_pActiveThumbs->iCurrentPix] );   //xxx
}

void DialogFiles::stopThumbing ( )
{
  m_pTimerThumbing->stop ( );
  if ( ! m_pActiveThumbs )
    return;

  if ( m_pActiveThumbs->iCurrentPix >= m_pActiveThumbs->iNumberOfPix )
       m_pActiveThumbs->iCurrentPix  = 0;

  addPlayButton ( m_pActiveThumbs->pOwnerItem, m_pActiveThumbs->arrayOfPix[m_pActiveThumbs->iCurrentPix] );
  m_pActiveThumbs = NULL;
}

void DialogFiles::append ( Thumbs *pThumbs )
{   
  lock   ( );
  m_listOfThumbs.append ( pThumbs );
  unlock ( );

  // Let us check the directory if there are some additional audio tracks.
  pThumbs->findTracks ( );

  setIconText ( pThumbs );
}

void DialogFiles::resizeIcons ( int iNewSize )
{
  uint   iNumberOfPix = 10;
  uint   t, iX, iY;
  double fRatio = 1.0;
  Thumbs *pThumbs;
  QList<Thumbs *>::iterator it;		//oxx

  if ( m_listOfThumbs.count ( ) < 1 )
    return;

  it = m_listOfThumbs.begin ( );
  pThumbs = m_listOfThumbs[0];
  if ( ! pThumbs )
    return;

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );
  m_bCanClose       = false;
  m_bCanChangeIcons = false;

  iNumberOfPix = pThumbs->iNumberOfPix;
  // keep the 4 extra pixels for the pixmap of the pOwnerItem in mind.
  //fRatio = (double)(4.0+iNewSize) / pThumbs->pOwnerItem->pixmap ( )->width  ( );  //oooo
  //fRatio = (double)(4.0+iNewSize) / pThumbs->pOwnerItem->icon ( 0 )->actualSize ( )->width  ( );      //xxx
  bool bReDrawText   = ( ( iNewSize < 51 ) != m_bTooSmallForText );
  m_bTooSmallForText =   ( iNewSize < 51 );

  m_pProgressBar->reset ( );
  //m_pProgressBar->setTotalSteps ( ( m_listOfThumbs.count ( )-1 ) * iNumberOfPix );    //oooo
  m_pProgressBar->setValue ( ( m_listOfThumbs.count ( )-1 ) * iNumberOfPix );           //xxx
  m_iProgress = 0;
  qApp->processEvents ( );
  // First we should resize m_pixIconStars [6]

  if ( ! lockDialog ( ) )
    return;

  createIconStars ( );
  // the first iteration we'll go through the first image only and set the IconViewItems image.
  while ( it != m_listOfThumbs.end ( ) )  {
    pThumbs   = *it++;
    if ( ! pThumbs->pCache->arrayOfThumbs )
           pThumbs->pCache->loadImages  ( );

    //iX = (int)( fRatio * pThumbs->pOwnerItem->x ( ) );    //oooo
    //iY = (int)( fRatio * pThumbs->pOwnerItem->y ( ) );    //oooo
    createPix ( pThumbs, 0 );

    //pThumbs->pOwnerItem->setPixmap ( *pThumbs->arrayOfPix[0] );   //ooo
    pThumbs->pOwnerItem->setIcon ( *pThumbs->arrayOfPix[0] );    //xxx
    //pThumbs->pOwnerItem->move      ( iX, iY );    //oooo

    if ( bReDrawText )
      setIconText ( pThumbs );
  }
  m_iProgress = m_listOfThumbs.count ( );
  //m_pPreview->updateContents  ( );                //oooo
  //m_pProgressBar->setProgress ( m_iProgress );    //ooo
  m_pProgressBar->setValue ( m_iProgress );         //xxx

  // Before we process Events we have to unlock the dialog, else we could get into a deadlock situation
  unlockDialog ( );
  qApp->processEvents ( );
  if ( ! lockDialog ( ) )
    return;

  // Second run through will generate all remaining thumbs ...
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    for (  t=1; t<pThumbs->iNumberOfPix; t++)
      createPix ( pThumbs, t );

    addPlayButton ( pThumbs->pOwnerItem, pThumbs->arrayOfPix[pThumbs->iCurrentPix] );

    m_iProgress  += (pThumbs->iNumberOfPix - 1);
    //m_pProgressBar->setProgress ( m_iProgress );  //ooo
    m_pProgressBar->setValue ( m_iProgress );       //xxx
    // Before we process Events we have to unlock the dialog, else we could get into a deadlock situation
    unlockDialog ( );
    qApp->processEvents ( );
    if ( ! lockDialog ( ) )
      return;
  }

  m_pProgressBar->reset ( );
  QApplication::restoreOverrideCursor ( );
  unlockDialog ( );

  m_bCanClose       = true;
  m_bCanChangeIcons = true;
}

void DialogFiles::addProgress ( int iAddProgress )
{
  // Called from another thread ...
  m_iProgress += iAddProgress;
  QTimer::singleShot ( 10, this, SLOT ( slotSetProgress ( ) ) );
}

void DialogFiles::slotSetProgress ( )
{
  // Update in main thread
  //m_pProgressBar->setProgress ( m_iProgress );    //ooo
  m_pProgressBar->setValue ( m_iProgress );         //xxx
}

void DialogFiles::slotAttrib ( bool )
{
  // One of the attribute check boxes was pushed.
  //m_bStars  = m_pCheckStars ->isOn ( );       //ooo
  m_bStars  = m_pCheckStars ->isChecked ( );    //xxx
  //m_bName   = m_pCheckName  ->isOn ( );       //ooo
  m_bName   = m_pCheckName  ->isChecked ( );    //xxx
  //m_bDate   = m_pCheckDate  ->isOn ( );       //ooo
  m_bDate   = m_pCheckDate  ->isChecked ( );    //xxx
  //m_bLength = m_pCheckLength->isOn ( );       //ooo
  m_bLength = m_pCheckLength->isChecked ( );    //xxx

  QString qsText;
  QFileInfo fileInfo;
  //QList<Q3IconViewItem *> fullList;	//oxx
  QList<QListWidgetItem *> fullList;	//xxx
  //Q3IconViewItem *pItem = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pItem = m_pPreview->item ( 0 );  //xxx
  while ( pItem ) {
    fullList.append ( pItem );
    //pItem = pItem->nextItem ( );  //ooo
    pItem = pItem->listWidget()->item ( pItem->listWidget()->row(pItem) + 1 ); //xxx
  }

  // First we handle all items which we have a Thumbs - structure for.
  Thumbs   *pThumbs;
  QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    // no longer required to handle
    //fullList.remove ( pThumbs->pOwnerItem );  //ooo
    fullList.removeOne ( pThumbs->pOwnerItem ); //xxx
    setIconText ( pThumbs );
  }

  //QList<Q3IconViewItem *>::iterator it2 = fullList.begin ( );	//oxx
  QList<QListWidgetItem *>::iterator it2 = fullList.begin ( );	//xxx
  while ( it2 != fullList.end ( ) )
    setIconText ( *it2++ );
}

void DialogFiles::slotAutoGroup ( )
{ 
  static unsigned int iGroupDelta = 60 * 60 * 24; // = 86400 == one day
  static unsigned int iMinNumber  = 1;

  //m_pTabWidget->setCurrentPage ( 2 ); //ooo
  m_pTabWidget->setCurrentIndex ( 2 );  //xxx

  bool    bFound;//,    bOkay;
  Thumbs *pThumbs,  *pTemp;
  QList<Thumbs *>list;							//oxx
  QList<Thumbs *>::iterator it2, it = m_listOfThumbs.begin ( );		//oxx
  int iSecsTo;
  unsigned int iVideoCount = 0;

  if ( m_listOfThumbs.count ( ) < 1 )
    return;

  DialogDelta deltaDialog ( this, isImageDialog ( ), iGroupDelta, iMinNumber );
  if ( deltaDialog.exec ( ) == QDialog::Rejected )
    return;

  iGroupDelta = deltaDialog.groupDelta ( );
  iMinNumber  = deltaDialog.minNumber  ( );

  // In order to build groups we have to sort by date
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    bFound  = false;
    it2 = list.begin ( );
    while ( it2 != list.end ( ) ) {
      pTemp = *it2;
      // Simple bubble sort to keep things ... well ... simple.
      if ( pTemp->pCache->dateCreated > pThumbs->pCache->dateCreated ) {
        list.insert ( it2, pThumbs ); // insert before it2
        bFound = true;
        break;
      }
      it2 ++;
    }
    if ( ! bFound )
      list.append ( pThumbs );
  }

  // There is one special check in case there is only one Group in the GroupView which is empty ...
  if ( ( list.count ( ) > 0 ) && ( m_pToolbox->count ( ) == 1 ) )  {
    //GroupView *pGroupView = (GroupView *)m_pToolbox->item ( 0 );  //ooo
    GroupView *pGroupView = (GroupView *)m_pToolbox->widget ( 0 );  //xxx

    //if ( pGroupView && pGroupView->childCount ( ) == 0 )  {   //ooo
    //if ( pGroupView && pGroupView->topLevelItem ( 0 )->childCount ( ) == 0 )  {   //xxx
    if ( pGroupView && pGroupView->topLevelItemCount ( ) == 0)  {    //xxx
      //m_pToolbox->removeItem ( pGroupView );  //oooo
      m_pToolbox->removeItem ( m_pToolbox->indexOf ( pGroupView ) );    //xxx
      delete pGroupView;
    }
  }

  // At this point we should have the thumbs sorted by date in the list
//  it = list.begin ( );
  //it = list.end ( );  //ooo
  it = list.begin ( );  //xxx
  QStringList emptyList;
  GroupView  *pGroupView;
  QString     qsDate, qsDateFormat = tr ( "(yyyy/MM/dd)" );
  if ( list.count ( ) > 0 ) {
    //pTemp  = *--it;   //ooo
    pTemp  = *it++;     //xxx
    if ( isImageDialog ( ) )
      qsDate = tr ( "Images " ) + pTemp->pCache->dateCreated.toString ( qsDateFormat );
    else
      qsDate = tr ( "Videos " ) + pTemp->pCache->dateCreated.toString ( qsDateFormat );
    //printf ( "<%010d> <%s> <%s>\n", 0, pTemp->pCache->dateCreated.toString ( ).ascii ( ), pThumbs->pCache->qsFileName.ascii ( ) );    //ooo
    //printf ( "<%010d> <%s> <%s>\n", 0, pTemp->pCache->dateCreated.toString ( ).toLatin1 ( ).data ( ), pThumbs->pCache->qsFileName.toLatin1 ( ).data ( ) );  //xxx
    pGroupView  = addGroup ( qsDate, emptyList );
    pGroupView->append ( pTemp );
    //while ( it != list.begin ( ) ) {    //ooo
    while ( it != list.end ( ) ) {    //xxx
    //for ( it; it != list.begin ( ); --it ) {  //xxx
      //pThumbs  = *--it;   //ooo
      //pThumbs  = *it;     //xxx
      pThumbs  = *it++;   //xxx
      //iSecsTo  = pTemp->pCache->dateCreated.secsTo ( pThumbs->pCache->dateCreated );
      iSecsTo  = pThumbs->pCache->dateCreated.secsTo ( pTemp->pCache->dateCreated );
      if ( iSecsTo <  0 )
           iSecsTo *=-1;
      //printf ( "<%010d> <%s> <%s>\n", iSecsTo, pThumbs->pCache->dateCreated.toString ( ).ascii ( ), pThumbs->pCache->qsFileName.ascii ( ) );  //ooo
      //printf ( "<%010d> <%s> <%s>\n", iSecsTo, pThumbs->pCache->dateCreated.toString ( ).toLatin1 ( ).data ( ), pThumbs->pCache->qsFileName.toLatin1 ( ).data ( ) );    //xxx
      if ( ( (unsigned int)iSecsTo > iGroupDelta ) && ( iVideoCount >= iMinNumber ) )  {
        if ( isImageDialog ( ) )
          qsDate = tr ( "Images " ) + pThumbs->pCache->dateCreated.toString ( qsDateFormat );
        else
          qsDate = tr ( "Videos " ) + pThumbs->pCache->dateCreated.toString ( qsDateFormat );
        QTimer::singleShot ( 10, pGroupView, SLOT ( slotAlternateColors ( ) ) );
        pGroupView  = addGroup ( qsDate, emptyList );
        pTemp = pThumbs;
        iVideoCount = 0;
      }
      iVideoCount ++;
    
      pGroupView->append ( pThumbs );
//      pTemp = pThumbs;
      //--it; //xxx
    }
    QTimer::singleShot ( 10, pGroupView, SLOT ( slotAlternateColors ( ) ) );
    clearPreview ( );
  }
}

bool DialogFiles::sortIcons ( DialogFiles::Thumbs *pFirst, DialogFiles::Thumbs *pSecond, int iSortType )
{
  // this functions holds the sorting logic for the QIconView.
  switch ( iSortType )  {
  case 1:  {  // Name
    QFileInfo fi1 ( pFirst->pCache->qsFileName  );
    QFileInfo fi2 ( pSecond->pCache->qsFileName );
    return  ( fi1.fileName ( ) > fi2.fileName ( ) );
  }
  break;
  case 2:  { // Number
    QFileInfo fi1 ( pFirst->pCache->qsFileName  );
    QFileInfo fi2 ( pSecond->pCache->qsFileName );
    QString qsFileName1 = fi1.fileName ( );
    QString qsFileName2 = fi2.fileName ( );
    filterNumbers ( qsFileName1 );
    filterNumbers ( qsFileName2 );
    if ( qsFileName1.isEmpty ( ) || qsFileName2.isEmpty ( ) )
      return false;
    return ( qsFileName1.toInt ( ) > qsFileName2.toInt ( ) );
  }
  break;
  case 3: // Date
    return ( pFirst->pCache->dateCreated > pSecond->pCache->dateCreated );
  break;
  case 4: { // Smart  Sort images by date and movies by first by Number then by Name
    if ( ( isImage ( pFirst->pCache->qsFileName  ) ) && 
         ( isImage ( pSecond->pCache->qsFileName ) ) )
      return ( pFirst->pCache->dateCreated > pSecond->pCache->dateCreated );
    // Okay then by number
    QFileInfo fi1 ( pFirst->pCache->qsFileName  );
    QFileInfo fi2 ( pSecond->pCache->qsFileName );
    QString qsFileName1 = fi1.fileName ( );
    QString qsFileName2 = fi2.fileName ( );
    filterNumbers ( qsFileName1 );
    filterNumbers ( qsFileName2 );
    // Check if both have numbers ... If not filter by Name
    if ( qsFileName1.isEmpty ( ) || qsFileName2.isEmpty ( ) )
      return  ( fi1.fileName ( ) > fi2.fileName ( ) );
    // Okay to filter by Number
    return ( qsFileName1.toInt ( ) > qsFileName2.toInt ( ) );
  }
  default:
    return true;
  }
  return true;
}

//  QFileInfo fileInfo ( qsFileName );
//  QString qsVids = Global::pApp->getImageFilter ( );
//  if ( qsVids.find ( fileInfo.extension ( FALSE ) ) > -1 )
//    return true;
//  return false;


void DialogFiles::slotSortBy ( int iSortType )
{
  // Sort Type: 0 == None, 1 == Name, 2 == Number, 3 == Date, 4 == Smart
  // Sort the thumbs Accordingly.
  //    None:   No sorting
  //    Name:   By file name
  //    Number: Use numbers in file names. Ignores letters.
  //    Date:   Sort by date.
  //    Smart:  Sort images by date and movies by first by Number then by Name
  ///////////////////////////////////////////////////////////////////////////////
  if ( ( iSortType < 1 ) || ( iSortType > 4 ) )  {
    m_iSortType = iSortType;
    return;
  }
  // Toggle Sort-Order
  if ( ( m_iSortType == iSortType ) && ( m_bSortAscending ) )
    m_bSortAscending = false;
  else
    m_bSortAscending = true;
  m_iSortType = iSortType;

  // If no thumbs are in the current view ... Oh well ...
  if ( m_listOfThumbs.count ( ) < 1 )
    return;

  bool    bFound, bSortOnlySelected = false;
  uint    iCount = 0;
  Thumbs *pThumbs,  *pTemp;
  QList<Thumbs *>list;							//oxx
  //QList<Q3IconViewItem *> iconList;					//oxx
  QList<QListWidgetItem *> iconList;					//xxx
  //QList<Q3IconViewItem *>::iterator iconIt, itA, itB;			//oxx
  QList<QListWidgetItem *>::iterator iconIt, itA, itB;			//xxx
  QList<Thumbs *>::iterator it2, it = m_listOfThumbs.begin ( );		//oxx
  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx

  // Check if we should sort the selected icons or ALL.
  if ( ! lockDialog ( ) )
    return;

  while ( pIcon ) {
    if  ( pIcon->isSelected ( )  )
      iCount++;
    //pIcon = pIcon->nextItem ( );                    //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 ); //xxx
  }
  unlockDialog ( );
  if ( iCount > 1 )
    bSortOnlySelected = true;

  // sort by date
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    bFound  = false;
    // Make sure we only sort the ones selected, or ALL if requested
    if ( ! bSortOnlySelected || pThumbs->pOwnerItem->isSelected ( ) )  {
      it2 = list.begin ( );
      while ( it2 != list.end ( ) ) {
        // Simple bubble sort to keep things ... well ... simple.
        pTemp = *it2;
        if ( sortIcons ( pTemp, pThumbs, iSortType ) )  {
          list.insert ( it2, pThumbs ); // insert before it2
          bFound = true;
          break;
        }
        it2 ++;
      }
      if ( ! bFound )
        list.append ( pThumbs );
    }
  }

  // If we sort descending then we'll have to revert the order
  if ( ! m_bSortAscending && list.size ( ) > 1 )  {
    QList<Thumbs *> tempList;	//oxx
    it = list.end ( );
    while ( --it != list.begin ( ) )
      tempList.append ( *it );

    tempList.append ( *it );
    list.clear ( );
    list = tempList;
  }

  // So lets get to work re-ordering the icons
  if ( ! lockDialog ( ) )
    return;

  //pIcon = m_pPreview->firstItem ( );      //ooo
  pIcon = m_pPreview->item ( 0 );   //xxx
  while ( pIcon ) {
    if  ( pIcon->isSelected ( ) || ! bSortOnlySelected )
      iconList.append ( pIcon );
    //pIcon = pIcon->nextItem ( );                    //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 ); //xxx
  }
  // At this point the ist AND iconList MUST have the same count.
  if ( ( list.size ( ) < 2 ) || 
       ( list.size ( ) != iconList.size ( ) ) )  {
    unlockDialog ( );
    return;
  }

  //Q3IconViewItem *pPrevItem, *pCurrItem, *pSortedItem;    //ooo
  QListWidgetItem *pPrevItem, *pCurrItem, *pSortedItem;     //xxx
  int iX1, iY1, iX2, iY2;
  it  = list.begin ( );
  it2 = list.begin ( );
  it2++;
  iconIt = iconList.begin ( );
  iCount = -1;
  
  //while ( ++iCount <  list.size ( ) - 1 )  { //ooo
  while ( ++iCount < static_cast<uint> ( list.size ( ) - 1 ) )  { //xxx
    pCurrItem = iconList[iCount]; // *iconIt;
    //pPrevItem = pCurrItem->prevItem ( );  //ooo
    pPrevItem = pCurrItem->listWidget ( )->item ( pCurrItem->listWidget ( )->row(pCurrItem) - 1 ); //xxx
    pSortedItem = (*it)->pOwnerItem;

    if ( pSortedItem != pCurrItem )  {
      //m_pPreview->takeItem   ( pCurrItem );   //ooo
      m_pPreview->takeItem   ( pCurrItem->listWidget ( )->row ( pCurrItem ) );  //xxx
      //m_pPreview->insertItem ( pCurrItem, pSortedItem );  //ooo
      m_pPreview->insertItem ( pCurrItem->listWidget ( )->row ( pCurrItem ), pSortedItem ); //xxx

      //iX1 = pCurrItem->x   ( );   //oooo
      //iY1 = pCurrItem->y   ( );   //oooo
      //iX2 = pSortedItem->x ( );   //oooo
      //iY2 = pSortedItem->y ( );   //oooo

// printf ( "list<%d> Curr[%d]<%s>%dx%d  vs Sorted[%d]<%s>%dx%d Prev<%p>\n", list.size ( ), pCurrItem->index ( ), pCurrItem->key ( ).ascii ( ), iX1, iY1, pSortedItem->index ( ), pSortedItem->key ( ).ascii ( ), iX2, iY2, pPrevItem );

      //m_pPreview->takeItem   ( pSortedItem ); //ooo
      m_pPreview->takeItem   ( pSortedItem->listWidget ( )->row ( pSortedItem ) );  //xxx
      if ( pPrevItem )
        //m_pPreview->insertItem ( pSortedItem, pPrevItem );    //ooo
        m_pPreview->insertItem ( pSortedItem->listWidget ( )->row ( pSortedItem ), pPrevItem ); //xxx
      else  {
        //Q3IconViewItem *pFirst = m_pPreview->firstItem ( );       //ooo
        QListWidgetItem *pFirst = m_pPreview->item ( 0 );   //xxx
        //m_pPreview->insertItem ( pSortedItem, pFirst );   //ooo
        m_pPreview->insertItem ( pSortedItem->listWidget ( )->row ( pSortedItem ), pFirst ); //xxx
        //m_pPreview->takeItem   ( pFirst );    //ooo
        m_pPreview->takeItem   ( pFirst->listWidget ( )->row ( pFirst ) );  //xxx
        //m_pPreview->insertItem ( pFirst, pSortedItem );   //ooo
        m_pPreview->insertItem ( pFirst->listWidget ( )->row ( pFirst ), pSortedItem ); //xxx
      }

// Note: Does not seem to work as expected. 
      //pCurrItem->move   ( iX2, iY2 ); //oooo
      //pSortedItem->move ( iX1, iY1 ); //oooo

      // Finally we must also adjust the iconList ...
      //itA = iconList.find ( pCurrItem   );    //ooo
      //itA = iconList.takeAt ( iconList.indexOf ( pCurrItem )  );   //xxx
      *itA = iconList.at ( iconList.indexOf ( pCurrItem )  );   //xxx
      //itA = iconList.begin();   //xxx
      while ( itA != iconList.end() ) { //xxx
         if ( (*itA) ==  pCurrItem )
            break;
      }
      //itB = iconList.find ( pSortedItem );    //ooo
      //itB = iconList.indexOf ( pSortedItem );   //xxx
      *itB = iconList.at ( iconList.indexOf ( pSortedItem ) );   //xxx
      while ( itB != iconList.end() ) { //xxx
         if ( (*itB) ==  pSortedItem )
            break;
      }
      if ( itA != itB )  { // swap those
        (*itA) = pSortedItem;
        (*itB) = pCurrItem;
      }
    }
    it++;
    it2++;
    iconIt++;
  }
  // The only way I found to have all Icons appear after this function call.
  //m_pPreview->arrangeItemsInGrid ( TRUE );    //xxx
  m_pPreview->sortItems ( Qt::AscendingOrder );      //xxx
  unlockDialog ( );
}

void DialogFiles::slotReload ( )
{
  Thumbs *pThumbs = NULL;
  QImage  theImage;
  QPixmap thePixmap;

  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx
  //QList<Q3IconViewItem *>listIcons;		//oxx
  QList<QListWidgetItem *>listIcons;		//xxx
  //QList<Q3IconViewItem *>::iterator it2;	//oxx
  QList<QListWidgetItem *>::iterator it2;	//xxx
  QList<Thumbs *>listSelected;			//oxx
  QList<Thumbs *>::iterator it;			//oxx

  if ( ! lockDialog ( ) )
    return;

  while ( pIcon ) {
    if  ( pIcon->isSelected ( )  )
      listIcons.append   ( pIcon );
    //pIcon = pIcon->nextItem ( );  //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 );   //xxx
  }
  if ( listIcons.count ( ) < 1 )  {
    unlockDialog ( );
    return;
  }

  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pThumbs = *it++;
    if ( pThumbs->pOwnerItem->isSelected ( ) )
      listSelected.append ( pThumbs );
  }

  it = listSelected.begin ( );
  //theImage = QImage ( QImage ( ).fromMimeSource ( "please_wait.jpg" ) );	//ooo
  theImage = QImage ( ":/images/please_wait.jpg" );				//xxx
  createPix ( &thePixmap, &theImage );

  while ( it != listSelected.end  ( ) ) {
    pThumbs = *it++;
    pThumbs->pCache->scanImages ( this ); // This is the default way to generate preview images.
    //listIcons.remove  ( pThumbs->pOwnerItem );    //ooo
    listIcons.removeOne  ( pThumbs->pOwnerItem );   //xxx
    //pThumbs->pOwnerItem->setPixmap ( thePixmap ); //ooo
    pThumbs->pOwnerItem->setIcon ( thePixmap );  //xxx
    m_bCanClose       = false;
    m_bCanChangeIcons = false;
  }
  // In case we have some error Icons around ...
  it2 = listIcons.begin ( );
  while ( it2 != listIcons.end ( ) ) {
    //Q3IconViewItem    *pItem = *it2++;    //ooo
    QListWidgetItem    *pItem = *it2++;     //xxx
    //pItem->setPixmap ( thePixmap );   //ooo
    pItem->setIcon ( thePixmap );    //xxx
    newThumbnailRequest  ( pItem );
  }

  //m_pPreview->arrangeItemsInGrid ( TRUE );    //ooo
  m_pPreview->sortItems ( Qt::AscendingOrder );      //xxx
  unlockDialog ( );
}

void DialogFiles::slotRefresh ( )
{
  // Here we erase all Icons and reload / recreate them
  //Q3IconViewItem *pIcon = m_pPreview->firstItem ( );      //ooo
  QListWidgetItem *pIcon = m_pPreview->item ( 0 );  //xxx
  QStringList    list;

  while ( pIcon ) {
    //list.append ( getKey ( pIcon ) );		//ooo
    list.append ( pIcon->text( ) );		//xxx
    //pIcon = pIcon->nextItem ( );  //ooo
    pIcon = pIcon->listWidget()->item ( pIcon->listWidget()->row(pIcon) + 1 ); //xxx
  }

  clearPreview ( );
  load    ( list );
}

void DialogFiles::clearPreview ( )
{
  uint t;
  lock ( );
  stopThumbing ( );

  for ( t=0; t<(uint)m_listOfThumbs.count ( ); t++ ) {		//ox
    delete m_listOfThumbs[t];
  }
  m_pPreview->clear    ( );
  m_listOfThumbs.clear ( );

  if ( m_pPreviewer )
       m_pPreviewer->clear ( );

  m_pButtonAutoGroup->setEnabled ( false );
  updateTitlebar ( );

  unlock ( );
}

void DialogFiles::updateTitlebar( )
{
  QString qsTitle = tr ( "Select / Sort / Group Source Files" );
  if ( isImageDialog ( ) )  {
    //if ( m_pPreview->count ( ) > 0 )  {           //oooo
    if ( m_pPreview->count ( ) > 0 )  { //xxx
      if ( m_iVidCounter < 1 )
        //qsTitle += tr ( " [%1 Images]" ). arg ( m_pPreview->count ( ) );  //ooo
        qsTitle += tr ( " [%1 Images]" ). arg ( m_pPreview->count ( ) );    //xxx
      else
        //qsTitle += tr ( " [%1 Images/%2 Videos]" ). arg ( m_pPreview->count ( )-m_iVidCounter ).arg ( m_iVidCounter );    //ooo
        qsTitle += tr ( " [%1 Images/%2 Videos]" ). arg ( m_pPreview->count ( )-m_iVidCounter ).arg ( m_iVidCounter );  //xxx
    }
  }
  else  {
    //if ( m_pPreview->count ( ) > 0 )  //ooo
    if ( m_pPreview->count ( ) > 0 )    //xxx
      //qsTitle += tr ( " [%1 Videos]" ). arg ( m_pPreview->count ( ) );    //ooo
      qsTitle += tr ( " [%1 Videos]" ). arg ( m_pPreview->count ( ) );  //xxx
  }
  //setCaption ( qsTitle ); //ooo
  setWindowTitle ( qsTitle );   //xxx
}

void DialogFiles::accept ( )      //ooo
//void DialogFiles::slotAccept ( )    //xxx
{  
  // Here we build the SourceFileEntries from the information in m_pToolbox
  int t;
  GroupView *pGroupView       = NULL;
  GroupView::Item *pGroupItem = NULL;
  SourceFileEntry *pEntry     = NULL;
  SourceFileInfo  *pInfo      = NULL;

  if ( ! m_bCanClose ) {
    if ( MessageBox::warning ( this, tr ( "Can't close." ), tr ( "Can not close dialog while waiting for MediaScanner to finish.\nDo you want to force Quit ?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
      return;
  }

  // Check if the default group has not been filled with anything but the Preview holds images, 
  // then we should move them into the group. The user probably assumed that the Preview is a Slideshow group.
  if ( m_pToolbox->count ( ) == 1 )  {
    
    //pGroupView = (GroupView *)m_pToolbox->item ( 0 ); //ooo
    pGroupView = (GroupView *)m_pToolbox->widget ( 0 ); //xxx

    //if ( pGroupView && pGroupView->childCount ( ) == 0 )  {   //oooo
    //if ( pGroupView && pGroupView->topLevelItem ( 0 )->childCount ( ) == 0 )  { //xxx
    if ( pGroupView && pGroupView->topLevelItemCount ( ) == 0 )  { //xxx

      int iRes = MessageBox::question ( this, tr ( "No Files in Group." ),
            //tr ( "There were no files added to the group \"%1\".\nDo you want to add all %2 videos in the preview to this group ?" ).arg ( m_pToolbox->itemLabel ( 0 ) ).arg ( m_listOfThumbs.size ( ) ), //ooo
            tr ( "There were no files added to the group \"%1\".\nDo you want to add all %2 videos in the preview to this group ?" ).arg ( m_pToolbox->itemText ( 0 ) ).arg ( m_listOfThumbs.size ( ) ),   //xxx
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
    //pGroupView = (GroupView *)m_pToolbox->item ( t ); //ooo
    pGroupView = (GroupView *)m_pToolbox->widget ( t ); //xxx
 
    if ( pGroupView ) {
      pEntry     = NULL;
 
      //pGroupItem = (GroupView::Item *)pGroupView->firstChild ( ); //oooo
      //pGroupItem = (GroupView::Item *)pGroupView->topLevelItem ( 0 )->child ( 0 );   //xxx
      pGroupItem = (GroupView::Item *)pGroupView->topLevelItem ( 0 );   //xxx
      if ( pGroupItem ) {
	    pEntry = new SourceFileEntry;
	    //QString qsCurrentName  = m_pToolbox->itemLabel ( t );    //ooo
        QString qsCurrentName  = m_pToolbox->itemText ( t );       //xxx
	    // If a SourceFileEntry exists for this GroupView, then we'll copy over the information.
	    if ( pGroupView->sourceFileEntry ( ) ) {
	      copyEntry ( pEntry, pGroupView->sourceFileEntry ( ) );
	      if ( pEntry->qsDisplayName != qsCurrentName )  {
	        Global::pApp->replaceSourceDisplayName ( pEntry->qsDisplayName, qsCurrentName );
	      }
	    }
	    // And finally we set the name of the entry
	    pEntry->qsDisplayName  = qsCurrentName;
      }
      while ( pGroupItem ) {      
	    pInfo = new SourceFileInfo;
	    if ( pGroupItem->pSourceFileInfo )  {
	      *pInfo = *pGroupItem->pSourceFileInfo;
	      pInfo->pPreview = pGroupItem->pSourceFileInfo->pPreview;
	      pGroupItem->pSourceFileInfo->pPreview = NULL;
	    }
	    else if ( pGroupItem->pCache ) {
	      pInfo->qsFileName  = pGroupItem->pCache->qsFileName;
	      pInfo->qsLength    = pGroupItem->pCache->qsLength;
	      pInfo->bUpdateInfo = true;
	    }
	    pEntry->listFileInfos.append ( pInfo );
        
	    //pGroupItem = (GroupView::Item *)pGroupItem->nextSibling ( ); //ooo
        //pGroupItem = (GroupView::Item *)pGroupItem->parent()->child(pGroupItem->parent()->indexOfChild(pGroupItem)+1);	//xxx
        pGroupItem = (GroupView::Item *)pGroupItem->treeWidget()->itemBelow(pGroupItem);	//xxx
      }
      if ( pEntry )  {
        applyManualChange     ( pEntry );
        m_listOfGroups.append ( pEntry );
      }
    }
  }

  // Some special handling if the user has not used a group I.e. the first and only group is empty
  // BUT the user has selected some files ...
  if ( m_pToolbox->count ( ) == 1 )  {
    //pGroupView = (GroupView *)m_pToolbox->item ( 0 ); //ooo
    pGroupView = (GroupView *)m_pToolbox->widget ( 0 ); //xxx
 
    //if ( pGroupView && ( pGroupView->childCount ( ) == 0 ) && ( m_listOfThumbs.count ( ) > 0 ) )  {   //ooo
    //if ( pGroupView && ( pGroupView->topLevelItem ( 0 )->childCount ( ) == 0 ) && ( m_listOfThumbs.count ( ) > 0 ) )  { //xxx
    if ( pGroupView && ( pGroupView->topLevelItemCount ( ) == 0 ) && ( m_listOfThumbs.count ( ) > 0 ) )  { //xxx
      if (  lockDialog ( ) )  {
        SourceFileEntry *pEntry = new SourceFileEntry;
	    QList<Thumbs *>::iterator it = m_listOfThumbs.begin ( );	//oxx
        while ( it != m_listOfThumbs.end ( ) ) {
          Thumbs *pThumbs = *it++;
          if ( pThumbs->pOwnerItem && pThumbs->pCache && pThumbs->pOwnerItem->isSelected ( ) )  {
            SourceFileInfo *pInfo = new SourceFileInfo;
            pInfo->qsFileName     = pThumbs->pCache->qsFileName;
            pInfo->bUpdateInfo    = true;
            pEntry->listFileInfos.append ( pInfo );
          }
        }
        if ( pEntry->listFileInfos.size( ) > 0 )  {
          applyManualChange     ( pEntry );
          m_listOfGroups.append ( pEntry );
        }
        else
          delete pEntry;
      }
      unlockDialog ( );
    }
  }

  Global::pThumbsCache->saveImageDB ( );
  Global::pThumbsCache->saveCacheDB ( );

  QDialog::accept ( );
}

void DialogFiles::reject ( )
{ 
  if ( ! m_bCanClose ) {
    if ( QMessageBox::warning ( this, tr ( "Can't close." ), tr ( "Can not close dialog while waiting for MediaScanner to finish.\nDo you want to force Quit ?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
      return;
  }
  Global::pThumbsCache->saveImageDB ( );
  Global::pThumbsCache->saveCacheDB ( );

  //uiDialogFiles::reject ( );		//ooo
  QDialog::reject ( );			//xxx
}

QList<SourceFileEntry *> &DialogFiles::getGroups ( )		//oxx
{
  return m_listOfGroups;
}

void DialogFiles::copyEntry ( SourceFileEntry *pTarget, SourceFileEntry *pSource )
{
  int t;
  pTarget->qsDisplayName = pSource->qsDisplayName;
  pTarget->listChapters  = pSource->listChapters;
  pTarget->bSoundSource  = pSource->bSoundSource;
  pTarget->bIsSlideshow  = pSource->bIsSlideshow;
  pTarget->iJumpStart    = pSource->iJumpStart;
  pTarget->iTitleset     = pSource->iTitleset;
  pTarget->iTitle        = pSource->iTitle;
  pTarget->qsPre         = pSource->qsPre;
  pTarget->qsPost        = pSource->qsPost;
  pTarget->iPause        = pSource->iPause;
  for ( t=0; t<MAX_AUDIO; t++ )
    pTarget->arrayAudioEntries[t]    = pSource->arrayAudioEntries [t];
  for ( t=0; t<MAX_SUBTITLES; t++ )
    pTarget->arraySubtitleEntries[t] = pSource->arraySubtitleEntries [t];
}

void DialogFiles::applyManualChange ( SourceFileEntry *pTarget )
{
  // Here we get the ManualChange over to the SourceFileInfo.
  SourceFileInfo *pInfo;
  unsigned long long iPathHash, iFileHash;
  Manual         *pManual = NULL;
  Manual::Change *pChange = NULL;
  Thumbs thumbs;

  QList<Audio          *>  listAudio;						//oxx
  QList<SubtitleEntry  *>  listSubtitles;					//oxx
  QList<SourceFileInfo *>::iterator it = pTarget->listFileInfos.begin ( );	//oxx
  while ( it != pTarget->listFileInfos.end ( ) )  {
    pInfo = *it++;
    thumbs.pSourceFileInfo = pInfo;
    thumbs.getManualChange ( pManual, pChange, iPathHash, iFileHash );
    if ( pChange )  {
      listAudio = pChange->copyAudioList ( );
      mergeAudioTracks  ( listAudio, pInfo );

      listSubtitles = pChange->copySubtitleList ( );
      mergeSubtitleTracks  ( listSubtitles, pInfo );
    }
  }
  // Finally we synch the Entrie Audio tracks to the info's
  pTarget->synchTracks ( );
}

//void DialogFiles::mergeAudioTracks ( Q3ValueList<Audio *> &list, SourceFileInfo *pInfo )	//ooo
void DialogFiles::mergeAudioTracks ( QList<Audio *> &list, SourceFileInfo *pInfo )		//xxx
{
  QList<Audio *>::iterator it;		//oxx
  QList<Audio *> tempList;		//oxx
  Audio *pAudio, *pExists;
  bool bFound;

  it = list.begin ( );
  while ( it != list.end ( ) )  {
    pAudio = *it++;
    if ( ( pAudio->iAudioNumber > 0 ) && ( pAudio->iAudioNumber < MAX_AUDIO ) )  {
      // First we check if the Audio track exists
      bFound = false;
      for ( int t=0; t<MAX_AUDIO; t++ )  {
        pExists = pInfo->arrayAudio[t];
        if ( pExists && pExists->qsFileName == pAudio->qsFileName )  {
          // so the track exists, lets copy over the info we want to change and keep what we wan to keep.
          // E.g. pTranscodingInterface.
          pExists->iAudioNumber = pAudio->iAudioNumber;
          pExists->qsIso639     = pAudio->qsIso639;
          tempList.append ( pExists );
          pInfo->arrayAudio[t] = NULL;
          t = MAX_AUDIO; // exit for - loop
          bFound = true;
        }
      }
      if ( ! bFound )  {
        tempList.append ( pAudio );
        //list.remove     ( pAudio );   //ooo
        list.removeOne     ( pAudio );  //xxx
      }
    }
  }
  // After we saved all records we want to keep, we can delete the rest ( if any )
  pInfo->deleteAudio  ( );
  // And then fill in the blanks
  //int iAudioNumber  =  0;
  it = tempList.begin ( );
  while ( it != tempList.end ( ) )  {
    pAudio = *it++;
    //pAudio->iAudioNumber = iAudioNumber++;
    pInfo->arrayAudio[pAudio->iAudioNumber] = pAudio;
  }
  // Lastly we can delete the temporary objects in list
  it = list.begin ( );
  while ( it != list.end ( ) )
    delete *it++;
  list.clear ( );
}

void DialogFiles::mergeSubtitleTracks ( QList<SubtitleEntry *> &list, SourceFileInfo *pInfo )		//oxx
{
  bool bFound;
  Subtitles     *pSubtitle, *pExists;
  SubtitleEntry *pSubtitleEntry;
  QList<Subtitles *> tempList;					//oxx
  QList<Subtitles *>::iterator it2;				//oxx
  QList<SubtitleEntry *>::iterator it = list.begin ( );		//oxx
  while ( it != list.end ( ) )  {
    pSubtitleEntry = *it++;
    if ( ( pSubtitleEntry->m_iSubtitleNumber > 0 ) && ( pSubtitleEntry->m_iSubtitleNumber < MAX_SUBTITLES ) )  {
      // First we check if the Subtitle track exists
      bFound = false;
      for ( int t=0; t<MAX_SUBTITLES; t++ )  {
        pExists = pInfo->arraySubtitles[t];
        if ( pExists && ( pExists->m_qsFileName.length ( ) > 0 ) && ( pExists->m_qsFileName == pSubtitleEntry->m_qsFileName ) )  {
          pExists->m_iSubtitleNumber = pSubtitleEntry->m_iSubtitleNumber;
          pExists->m_qsIso639        = pSubtitleEntry->m_qsIso639;
          // Note at this point we do NOT re-load the subtitle file, as I assume the user made changes to the file.
          pInfo->arraySubtitles[t]   = NULL;
          tempList.append ( pExists );
          bFound = true;
          t = MAX_SUBTITLES; // exit for - loop
        }
      }
      // unfortunately we can not rely on the file name alone as the user could have created the subtitle
      // rather than load it from file.
      // So the next check is to see if the track itself is already occupied.
      pExists = pInfo->arraySubtitles[pSubtitleEntry->m_iSubtitleNumber];
      if ( ! bFound && pExists )  {
          pExists->m_qsIso639 = pSubtitleEntry->m_qsIso639;
          tempList.append ( pExists );
          pInfo->arraySubtitles[pSubtitleEntry->m_iSubtitleNumber] = NULL;
          bFound = true;
      }
      if ( bFound )  {
        //list.remove ( pSubtitleEntry );   //ooo
        list.removeOne ( pSubtitleEntry );  //xxx
        delete pSubtitleEntry;
      }
    }
  }
  // After we saved all records we want to keep, we can delete the rest ( if any )
  pInfo->deleteSubtitles ( );
  // And then fill in the blanks
  //int iSubtitleNumber = 0;
  it2 = tempList.begin ( );
  while ( it2 != tempList.end ( ) )  {
    pSubtitle  = *it2++;
    //pSubtitle->m_iSubtitleNumber = iSubtitleNumber++;
    pInfo->arraySubtitles[pSubtitle->m_iSubtitleNumber] = pSubtitle;
  }

  // After we took care of the already existing subtitles, we'll handle newly assigned Subtitles here.
  QFileInfo fileInfo;
  Import::Srt srtImporter;
  Import::Ssa ssaImporter;
  Import::BaseSubtitle *pImporter = &ssaImporter;

  it = list.begin ( );
  while ( it != list.end ( ) )  {
    pSubtitleEntry = *it++;

    pImporter   = &ssaImporter;
    fileInfo.setFile ( pSubtitleEntry->m_qsFileName );
    //if ( fileInfo.extension ( false ).lower ( ) == "srt" )    //ooo
    if ( fileInfo.suffix ( ).toLower ( ) == "srt" )             //xxx
      pImporter = &srtImporter;

    if ( pImporter->readFile ( pSubtitleEntry->m_qsFileName ) )  {
      pSubtitle = new Subtitles ( 0, 0 );
      pSubtitle->m_qsIso639        = pSubtitleEntry->m_qsIso639;
      pSubtitle->m_qsFileName      = pSubtitleEntry->m_qsFileName;
      //pSubtitle->m_iSubtitleNumber   = iSubtitleNumber++;
      pSubtitle->m_iSubtitleNumber = pSubtitleEntry->m_iSubtitleNumber;
      pSubtitle->m_listOfSubtitles = pImporter->takeSubtitles ( );
      // this should never happen ... but to be save ...
      if ( pInfo->arraySubtitles[pSubtitle->m_iSubtitleNumber] )
        delete pInfo->arraySubtitles[pSubtitle->m_iSubtitleNumber];
      pInfo->arraySubtitles[pSubtitle->m_iSubtitleNumber] = pSubtitle;
    }
    // And finally we can delete the SubtitleEntry.
    delete pSubtitleEntry;
  }
  list.clear ( );
}

}; // end namespace Input
