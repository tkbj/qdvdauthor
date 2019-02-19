/***************************************************************************
    dialogvfolder.cpp
                             -------------------
    Implementation for DialogVFolder
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Handling of VirtualFolder from within DialogFiles.
    
****************************************************************************/

#include <qimage.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QMessageBox>		//xxx
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	//oxx

#include "global.h"
#include "qdvdauthor.h"
#include "dialogvfolder.h"
#include "filepreviewdialog.h"
#include "qplayer/mediacreator.h"

namespace Input 
{

//DialogVFolder::VFolderItem::VFolderItem ( Cache::Thumbs::Entry *p, Q3ListView *pListView, QString qsLabel  )  //ooo
DialogVFolder::VFolderItem::VFolderItem ( Cache::Thumbs::Entry *p, QTreeWidget *pListView, QString qsLabel  )   //xxx
  //: Q3ListViewItem ( pListView, qsLabel ) //ooo
  : QTreeWidgetItem ( pListView )  //xxx
{
  bAlternateColor = true;
  pEntry          = p;
}

/*void DialogVFolder::VFolderItem::paintCell ( QPainter *p, const QColorGroup & cg, int column, int width, int align)
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
}*/ //oooo

//DialogVFolder::DialogVFolder ( QWidget *pParent, const char *pName, bool bModal, Qt::WFlags flags )	//ooo
DialogVFolder::DialogVFolder ( QWidget *pParent )							//xxx
  //: uiDialogVFolder ( pParent, pName, bModal, flags )		//ooo
  : QDialog ( pParent )						//xxx
{
  setupUi(this);		//xxx
  m_bIsImageFolder = false;
  m_pNextItem      = NULL;
}

DialogVFolder::~DialogVFolder ( )
{
}

void DialogVFolder::initMe ( QString qsVirtualFolder, bool bImageFolder )
{
  m_pLabelHeader->setText ( "<p align=\"center\"><b>" + qsVirtualFolder + "</b></p>" );
  Cache::Thumbs::VirtualFolder *pVFolder = Global::pThumbsCache->findVirtualFolder ( qsVirtualFolder, bImageFolder );

  m_bCanClose      = true;
  m_iThumbSize     = 65;
  m_qsFolderName   = qsVirtualFolder;
  m_bIsImageFolder = bImageFolder;
  //m_pListView->setSortColumn ( -1 );  //oooo
  if ( ! pVFolder )
    return;

  QImage    theImage;
  QPixmap   thePixmap;
  QFileInfo fileInfo;
  //Q3ListViewItem *pItem = NULL;   //ooo
  QTreeWidgetItem *pItem = NULL;    //xxx
  Cache::Thumbs::Entry *pEntry = NULL;
  Cache::Thumbs::VirtualFolder::VFile *pVFile = NULL;
  QList<Cache::Thumbs::VirtualFolder::VFile *>::iterator it;		//oxx

  it = pVFolder->listOfFiles.end ( );
  while ( it != pVFolder->listOfFiles.begin ( ) ) {
    pVFile = *--it;
    fileInfo.setFile ( pVFile->qsFileName );
    pEntry = Global::pThumbsCache->find ( pVFile->iFileHashValue, bImageFolder );
    if ( !pEntry && bImageFolder )  // Special handling for ImageFolder, which could hold VIDs
          pEntry = Global::pThumbsCache->find ( pVFile->iFileHashValue, false );

    pItem  = new VFolderItem ( pEntry, m_pListView, fileInfo.fileName( ) );
    if ( pEntry ) {
      if ( ! pEntry->arrayOfThumbs )
             pEntry->loadImages  ( );
      //theImage = pEntry->arrayOfThumbs[0]->smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );   //ooo
      theImage = pEntry->arrayOfThumbs[0]->scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );          //xxx
      thePixmap.convertFromImage ( theImage );
      //pItem->setPixmap ( 0, thePixmap );  //ooo
      pItem->setIcon ( 0, thePixmap );      //xxx
    }
  }
  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );

  connect ( m_pButtonAdd,    SIGNAL ( clicked ( ) ), this, SLOT ( slotAdd    ( ) ) );
  connect ( m_pButtonDelete, SIGNAL ( clicked ( ) ), this, SLOT ( slotDelete ( ) ) );
  connect ( m_pButtonUp,     SIGNAL ( clicked ( ) ), this, SLOT ( slotUp     ( ) ) );
  connect ( m_pButtonDown,   SIGNAL ( clicked ( ) ), this, SLOT ( slotDown   ( ) ) );
}

void DialogVFolder::slotAdd ( )
{
  uint t;
  bool bNewEntry = false;
  QFileInfo      fileInfo;
  QImage         theImage;
  QPixmap        thePixmap;
  QString        qsMovieFilter;
  QStringList    list;
  //Q3ListViewItem *pItem;  //ooo
  QTreeWidgetItem *pItem;   //xxx
  if ( m_bIsImageFolder )  {
    qsMovieFilter  =  Global::pApp->getImageFilter ( );
    qsMovieFilter +=  Global::pApp->getVidFilter   ( );
    list          =  FilePreviewDialog::getOpenFileNames ( NULL, Global::qsCurrentPath, QString ("Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));
  }
  else  {
    qsMovieFilter =  Global::pApp->getMovieFilter ( );
    list          =  FilePreviewDialog::getOpenFileNames ( NULL, Global::qsCurrentPath, QString ("Images ( ") + qsMovieFilter + QString (" );;All ( * )"));
  }
  if ( list.count ( ) < 1 )
    return;

  Cache::Thumbs::Entry *pEntry = NULL;
  //for ( t=0; t<list.count ( ); t++ ) {    //ooo
  for ( t=0; t<static_cast<uint> ( list.count ( ) ); t++ ) { //ooo
    pEntry = Global::pThumbsCache->find ( list[t], m_bIsImageFolder );
    fileInfo.setFile ( list[t] );
    if ( pEntry )  {
      pItem  = new VFolderItem ( pEntry, m_pListView, fileInfo.fileName ( ) );
      if ( ! pEntry->arrayOfThumbs )
	pEntry->loadImages ( );
      //theImage = pEntry->arrayOfThumbs[0]->smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );   //ooo
      theImage = pEntry->arrayOfThumbs[0]->scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );          //xxx
      thePixmap.convertFromImage ( theImage );
      //pItem->setPixmap ( 0, thePixmap );  //ooo
      pItem->setIcon ( 0, thePixmap );      //xxx
    }
    else {
      // No cache entry exists. So we have to create one.
      bNewEntry = true;
      if ( m_bIsImageFolder )
        pEntry = Global::pThumbsCache->append ( list[t], (QImage *)NULL ); //ooooo
      else
        pEntry = Global::pThumbsCache->append ( list[t], this );
      pItem    = new VFolderItem ( pEntry, m_pListView, fileInfo.fileName ( ) );
      //theImage = QImage ( ).fromMimeSource( "please_wait.jpg" );	//ooo
      theImage = QImage ( ":/images/please_wait.jpg" );			//xxx
      //theImage = theImage.smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );    //ooo
      theImage = theImage.scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );           //xxx
      thePixmap.convertFromImage ( theImage );
      //pItem->setPixmap ( 0, thePixmap );  //ooo
      pItem->setIcon ( 0, thePixmap );      //xxx
      m_bCanClose = false;
    }
  }
  if ( bNewEntry )
    Global::pThumbsCache->saveImageDB ( );

  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

void DialogVFolder::timerEvent ( QTimerEvent *pEvent )
{
  // called back after the thumbs have been rendered in the CacheThumbs class.
  if ( (const uint)pEvent->timerId ( ) == MEDIASCANNER_EVENT + 5 ) { // TYPE_MULTI_THUMB
    QImage  theImage;
    QPixmap thePixmap;
    VFolderItem *pItem;
    Cache::Thumbs::Entry *pEntry = NULL;
    QList<Cache::Thumbs::Entry *>list = Global::pThumbsCache->getScannedThumbs ( );	//oxx
    QList<Cache::Thumbs::Entry *>::iterator it;						//oxx
    it = list. begin ( );
    while ( it != list.end ( ) ) {
      pEntry = *it++;
      // Next we have to find the corresponding Item in the ListView 
      //pItem = (VFolderItem *)m_pListView->firstChild ( ); //ooo
      pItem = (VFolderItem *)m_pListView->topLevelItem ( 0 );   //xxx
      while ( pItem ) {
	if  ( pItem->pEntry == pEntry ) {
	  pEntry->iScanStatus = 0;
	  if ( ! pEntry->arrayOfThumbs )
	         pEntry->loadImages  ( );
	  //theImage = pEntry->arrayOfThumbs[0]->smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );  //ooo
      theImage = pEntry->arrayOfThumbs[0]->scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );         //xxx
	  thePixmap.convertFromImage ( theImage );
	  //pItem->setPixmap ( 0, thePixmap ); //ooo
      pItem->setIcon ( 0, thePixmap );     //xxx
	  break;
	}
	//pItem = (VFolderItem *)pItem->nextSibling ( );   //ooo
    pItem = (VFolderItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
      }
    }
  }

  //uiDialogVFolder::timerEvent ( pEvent );	//ooo
  QDialog::timerEvent ( pEvent );		//xxx
  m_bCanClose = true;
}

void DialogVFolder::slotDelete ( )
{
  QList <QTreeWidgetItem *> m_pList;   //xxx
  //Q3ListViewItem *pItem = m_pListView->selectedItem ( );  //ooo
  m_pList.append ( m_pListView->selectedItems ( ) );    //xxx
  QTreeWidgetItem *pItem = m_pList[0];   //xxx
  //m_pNextItem  = pItem->nextSibling ( );  //ooo
  m_pNextItem  = pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx

  if ( pItem )
    delete pItem;

  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

void DialogVFolder::slotUp ( )
{
  QList <QTreeWidgetItem *> m_pList;   //xxx
  //Q3ListViewItem *pItem = m_pListView->selectedItem ( );  //ooo
  m_pList.append ( m_pListView->selectedItems ( ) );    //xxx
  QTreeWidgetItem *pItem = m_pList[0];   //xxx
  if ( ! pItem )
    return;
  //Q3ListViewItem *pPrevItem = pItem->itemAbove ( );   //ooo
  QTreeWidgetItem *pPrevItem = pItem->treeWidget ( )->itemAbove ( pPrevItem );    //xxx
  if ( ! pPrevItem )
    return;
  //pPrevItem = pPrevItem->itemAbove ( );   //ooo
  pPrevItem = pPrevItem->treeWidget ( )->itemAbove ( pPrevItem );   //xxx

  if ( ! pPrevItem ) {
    //m_pListView->takeItem    ( pItem );   //ooo
    m_pListView->removeItemWidget    ( pItem, 0 ); //xxx
    //m_pListView->insertItem  ( pItem );   //ooo
    m_pListView->insertTopLevelItem  ( 0, pItem );  //xxx
    //m_pListView->setSelected ( pItem, TRUE ); //ooo
    pItem->setSelected ( true );   //xxx
  }
  else
    //pItem->moveItem  ( pPrevItem );   //ooo
    pItem->treeWidget()->removeItemWidget  ( pPrevItem, 0 );  //xxx
  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

void DialogVFolder::slotDown ( )
{
  QList <QTreeWidgetItem *> m_pList;   //xxx
  //Q3ListViewItem *pItem = m_pListView->selectedItem ( );  //ooo
  m_pList.append ( m_pListView->selectedItems ( ) );    //xxx
  QTreeWidgetItem *pItem = m_pList[0];   //xxx
  if ( ! pItem )
    return;
  //Q3ListViewItem *pNextItem = pItem->itemBelow ( );   //ooo
  QTreeWidgetItem *pNextItem = pItem->treeWidget()->itemBelow ( pItem );    //xxx
  if ( ! pNextItem )
    return;
  //pItem->moveItem ( pNextItem );  //oooo
  //pItem = pItem->treeWidget()->setItemWidget ( pNextItem, 0 );  //xxx

  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

void DialogVFolder::slotAlternateColors ( )
{
  //VFolderItem *pItem = (VFolderItem *)m_pListView->firstChild ( );    //oooo
  VFolderItem *pItem = (VFolderItem *)m_pListView->topLevelItem ( 0 );  //xxx
  bool bAlternate = true;
  while ( pItem ) {
    bAlternate = ! bAlternate;
    pItem->bAlternateColor = bAlternate;
    //pItem->repaint ( );   //oooo
    //pItem = (VFolderItem *)pItem->nextSibling ( );    //ooo
    pItem = (VFolderItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }

  if ( m_pNextItem )
       //m_pNextItem->setSelected ( TRUE ); //ooo
       m_pNextItem->setSelected ( true );   //xxx
  m_pNextItem = NULL;
}

QList<Cache::Thumbs::Entry *> &DialogVFolder::getEntries ( )	//oxx
{
  return m_returnList;
}

void DialogVFolder::accept ( )
{
  if ( ! m_bCanClose ) {
    QMessageBox::warning ( this, tr ( "Can't close." ), tr ( "Can not close dialog while waiting for MediaScanner to finish.\nPlease wait for previews to be generated and try again." ), QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  
  m_returnList.clear ( );
  //VFolderItem *pItem = (VFolderItem *)m_pListView->firstChild ( );    //oooo
  VFolderItem *pItem = (VFolderItem *)m_pListView->topLevelItem ( 0 );  //xxx
  while ( pItem ) {
    m_returnList.append  ( pItem->pEntry );
    //pItem = (VFolderItem *)pItem->nextSibling ( );    //ooo
    pItem = (VFolderItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }      

  //uiDialogVFolder::accept ( );	//ooo
  QDialog::accept ( );			//xxx
}

void DialogVFolder::reject ( )
{
  if ( ! m_bCanClose ) {
    QMessageBox::warning ( this, tr ( "Can't close." ), tr ( "Can not close dialog while waiting for MediaScanner to finish.\nPlease wait for previews to be generated and try again." ), QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  //uiDialogVFolder::reject ( );	//ooo
  QDialog::reject ( );			//xxx
}

}; // end namespace Input
