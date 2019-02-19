/***************************************************************************
    structuretoolbar.cpp
                             -------------------
    StructureToolBar - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will create a toolbar, which holds only one widget.
   The StructureWidget is displaying the current structure of the DVD.
    
****************************************************************************/

#include <QMessageBox>
#include <QTreeView>

#include "dvdmenu.h"
#include "menuobject.h"
#include "menupreview.h"
#include "sourcetoolbar.h"
#include "structuretoolbar.h"

StructureItem::~StructureItem ( )
{
  initMe ( StructureItem::Unknown ); 
}

void StructureItem::initMe ( enType type )
{
//printf ( "    StructureItem : CREATE  <%p> <%s>\n", this, text( 0 ).ascii ( ) );
  m_pDVDMenu     = NULL; 
  m_pMenuPreview = NULL; 
  m_pMenuObject  = NULL; 	//oooo
  setPixmap ( type );
}

StructureItem *StructureItem::setSibling ( MenuObject *pMenuObject, StructureItem *pItem, QString qsName, QString qsArgs )
{
  StructureItem *pSibling = NULL;
  //QTreeWidgetItem *current = ...;						//xxx
  //QTreeWidgetItem *parent = current->parent();				//xxx
  //QTreeWidgetItem *nextSibling;						//xxx
  //nextSibling =parent()->child(parent->indexOfChild(current)+1);		//xxx
  
  StructureItem *nextSibling = NULL;						//xxx
  //nextSibling =parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx

  if ( pItem ) {
    //pSibling = (StructureItem *)pItem->nextSibling ( );						//ooo
    nextSibling = (StructureItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
    pSibling = (StructureItem *)nextSibling;								//xxx
  } else {
    //pSibling = (StructureItem *)firstChild ( );	//oooo
    pSibling = (StructureItem *)child ( 0 );	//xxx
  }

  if ( ! pSibling ) {
    pSibling = new StructureItem ( pMenuObject, StructureItem::Unknown, this, pItem, qsName, qsArgs );
    //pSibling->setText ( 0, qsName );		//xxx
    //pSibling->setText ( 1, qsArgs );		//xxx
  } else {
    //pSibling->setText ( qsName, qsArgs );	//oooo
    pSibling->setText ( 0, qsName );		//xxx
    pSibling->setText ( 1, qsArgs );		//xxx
  }

  return pSibling;
}

void StructureItem::setPixmap ( StructureItem::enType type )
{
  // This function will load the apropriate pixmap from the resources.
  m_type = type;
  int x, iSize = 32;  // 19 icons stored in this one pix.
  x = (int)type * iSize;

  //QPixmap allImages = qPixmapFromMimeSource ( "structure_icons.png" );	//ooo
  QPixmap allImages ( ":/images/structure_icons.png" );				//xxx
//  QPixmap allImages = QPixmap::fromMimeSource ( "checkered.png" );
  QPixmap thePixmap ( iSize, iSize );
  //copyBlt ( &thePixmap, 0, 0, &allImages, x, 0, iSize, iSize );		//ooo
  thePixmap = allImages.copy(x, 0, iSize, iSize);				//xxx

  if ( ! thePixmap.isNull ( ) )
    //Q3ListViewItem::setPixmap ( 0, thePixmap );				//ooo
    QTreeWidgetItem::setIcon ( 0, thePixmap );				//xxx

}

StructureItem *StructureItem::lastChild ( )
{
  StructureItem *nextSibling = NULL;						//xxx
  
  //StructureItem *pPrev, *pLastItem = (StructureItem *)firstChild ( );		//ooo
  StructureItem *pPrev, *pLastItem = (StructureItem *)child ( 0 );		//xxx
  //StructureItem *pPrev, *pLastItem = (StructureItem *)parent (  );		//xxx
  pPrev = pLastItem;
  while ( pLastItem ) {
    // Special handling for this item ...
    if  ( pLastItem->text ( 0 ) == QObject::tr ( "Specified Buttons" ) )
      return pPrev;
    pPrev     = pLastItem;
    //pLastItem = (StructureItem *)pLastItem->nextSibling ( );		//oooo
    nextSibling = (StructureItem *)pLastItem->parent()->child(pLastItem->parent()->indexOfChild(pLastItem)+1);	//xxx
    pLastItem = (StructureItem *)nextSibling;	//xxx
    
  }
  return pPrev;
}

StructureToolBar::StructureToolBar ( QDockWidget *pSourceToolbar )
{
  m_pSourceToolBar = (SourceToolBar *)pSourceToolbar;
  //Q3ListView  *pListView = m_pSourceToolBar->getStructureListView ( );	//ooo
  QTreeWidget  *pListView = m_pSourceToolBar->getStructureListView ( );		//xxx
  //QTabWidget *pTabCtrl  = m_pSourceToolBar->getTabCtrl           ( );		//oooo
  
  /*connect( pListView, SIGNAL( rightButtonPressed ( Q3ListViewItem *, const QPoint &, int )), this, SLOT (slotRightClickedOnStructure (Q3ListViewItem *, const QPoint &, int)) );
  connect( pListView, SIGNAL( rightButtonPressed ( Q3ListViewItem *, const QPoint &, int )), this, SLOT (slotRightClickedOnTitleset  (Q3ListViewItem *, const QPoint &, int)) );

  connect ( pTabCtrl, SIGNAL ( currentChanged ( QWidget * ) ), this, SLOT ( slotTabChanged ( QWidget * ) ) );*/		//oooo
}

StructureToolBar::~StructureToolBar ()
{
}

StructureItem *StructureToolBar::clean ( bool )
{
  StructureItem *pMainItem = NULL;

  //QListView     *pListView;		//ooo
  QTreeWidget     *pListView;		//xxx
  
  pListView = m_pSourceToolBar->getStructureListView ( );

  // if there is already a 'first' Item, then we return this.
  //if  ( pListView->firstChild ( ) )
  //if  ( pListView->firstChild ( ) )	//ooo
    //if  ( pListView->currentItem ( ) ) {		//xxx
    //if  ( pListView->indexOfTopLevelItem( pListView->currentItem ( ) ) >= 0 ) {		//xxx
    //if  ( pListView->topLevelItem (0)->childCount ( ) > 0 ) {		//xxx
  if  ( pListView->topLevelItemCount ( ) > 0 ) {		//xxx
  //if  ( pListView->topLevelItemCount ( ) > 0 && pListView->topLevelItem ( 0 )->childCount ( ) > 0 ) {		//xxx
    //return (StructureItem *)pListView->firstChild ( );	//ooo
      //return (StructureItem *)pListView->currentItem ( );		//xxx
      //return (StructureItem *)pListView->headerItem ( );		//xxx
      //return (StructureItem *)pListView->topLevelItem (0)->child(0);	//xxx
    return (StructureItem *)pListView->topLevelItem ( 0 );		//xxx
  }

  //pListView->clear ();
  //pMainItem = new StructureItem ( pListView, StructureItem::DVDProject, tr ("DVD Project") );		//ooo
  //pMainItem = (new QTreeWidgetItem(pListView))->setText(0, tr("DVD Project"));			//xxx
  //(new QTreeWidgetItem(pListView))->setText(0, tr("DVD Project"));					//xxx							//xxx
  //pMainItem = (new StructureItem ( pListView, StructureItem::DVDProject))->setText(0, tr("DVD Project"));		//xxx
  //pMainItem = new StructureItem ( pListView, StructureItem::DVDProject, tr ("DVD Project") );		//xxx
  //(new StructureItem ( pListView, StructureItem::DVDProject))->setText(0, tr ("DVD Project") );		//xxx
  pMainItem = new StructureItem ( pListView, StructureItem::DVDProject);				//xxx
  pMainItem->setText(0, tr ("DVD Project") );								//xxx

  //pListView->setSorting( -1, TRUE );		//oooo
  //pMainItem->setExpandable ( TRUE );		//ooo
  pMainItem->setExpanded ( true );		//xxx
  //pMainItem->setOpen       ( TRUE );		//oooo

  return pMainItem;
}
