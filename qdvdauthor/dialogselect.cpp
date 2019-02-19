/****************************************************************************
** dialogselect.cpp
**
**   Created : Sat Nov 10 2007
**        by : Varol Okan using Kate
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
*****************************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
//#include <q3valuelist.h>	//oxx
#include <qfileinfo.h>
#include <qcombobox.h>
//#include <q3listview.h>   //ooo
#include <QTreeWidget>      //xxx
#include <qcheckbox.h>
//#include <q3listbox.h>    //ooo
#include <QListWidget>      //xxx
#include <QListWidgetItem>  //xxx
//#include <q3header.h>     //xxx
#include <qtimer.h>

#include "sourcefileentry.h"
#include "messagebox.h"
#include "dialogselect.h"
#include "qdvdauthor.h"
#include "dvdmenu.h"
#include "global.h"

//DialogSelect::DialogSelect ( QWidget *parent, const char *name, bool modal, Qt::WFlags fl )	//ooo
DialogSelect::DialogSelect ( QWidget *parent )							//xxx
  //: uiDialogSelect ( parent, name, modal, fl )	//ooo
  : QDialog ( parent )					//xxx
{
  setupUi(this);	//xxx
  
  m_bShowAll = true;
  //m_pListSelected->setSorting ( -1 ); //ooo
  //m_pListSelected->setSelectionMode  ( Q3ListView::Single  ); //ooo
  //m_pListAvailable->setSelectionMode ( Q3ListBox::Extended ); //ooo

  connect ( m_pButtonUp,        SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonUp        ( ) ) );
  connect ( m_pButtonDown,      SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonDown      ( ) ) );
  connect ( m_pButtonAdd,       SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonAdd       ( ) ) );
  connect ( m_pButtonRemove,    SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonRemove    ( ) ) );
  connect ( m_pButtonAddAll,    SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonAddAll    ( ) ) );
  connect ( m_pButtonRemoveAll, SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonRemoveAll ( ) ) );
  connect ( m_pButtonShowAll,   SIGNAL ( clicked ( ) ), this, SLOT ( slotButtonShowAll   ( ) ) );
  connect ( m_pCheckLoop,       SIGNAL ( toggled ( bool ) ),     this, SLOT ( slotCheckLoop ( bool ) ) );
  connect ( m_pListAvailable,   SIGNAL ( selectionChanged ( ) ), this, SLOT ( slotSelectionChanged ( ) ) );

  /*Q3Header *pHeader = m_pListSelected->header ( );
  if ( pHeader )
       connect ( pHeader, SIGNAL ( clicked ( int ) ), this, SLOT ( slotHeaderClicked ( int ) ) );*/ //ooo
}

DialogSelect::~DialogSelect ( )
{
  uint t;
  for ( t=0; t<(uint)m_listSourceObjects.count ( ); t++ )   //oxx
    delete m_listSourceObjects[t];
}

bool DialogSelect::initMe ( QStringList &list, bool bLoop )
{  
  QDVDAuthor *pQDVDAuthor  = Global::pApp;
  QList<DVDMenu *>listMenus = pQDVDAuthor->getSubMenus ( );	//oxx
  DVDMenu    *pCurrentMenu = pQDVDAuthor->getCurrentSubMenu ( );
  uint t, i,  iNrOfEntries = pQDVDAuthor->sourceFileCount   ( );
  SourceFileEntry  *pEntry = NULL;
  SourceFileInfo   *pInfo  = NULL;
  QStringList listFinalDestination;
  QString     qsEntry;

  //  QValueList<SourceFileEntry *> listEntries = pCurrentMenu->getSourceFileEntries( );
  QList<SourceFileInfo *> listFileInfos = pCurrentMenu->getSourceFileInfos  ( );	//oxx
  m_qsCurrentMenuName = pCurrentMenu->name ( );

  listFinalDestination.append     ( "-- None --" );
  listFinalDestination.append     ( pQDVDAuthor->getVMGMenu ( )->getInterface ( )->qsMenuName );
  for ( t=0; t<(uint)listMenus.count    ( ); t++ )  //oxx
    listFinalDestination.append   ( listMenus[t]->getInterface ( )->qsMenuName );
  //m_pComboFinal->insertStringList ( listFinalDestination );   //ooo
  m_pComboFinal->addItems ( listFinalDestination ); //xxx
  m_pComboFinal->setEnabled       ( ! bLoop );
  m_pCheckLoop->setChecked        (   bLoop );

  // populate the list of all source files
  for ( t=0; t<iNrOfEntries; t++ ) {
    pEntry = pQDVDAuthor->sourceFileEntry ( t );
    if ( pEntry->bSoundSource )
      continue;
    for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ ) { //oxx
      ObjectEntry *pObjectEntry = new ObjectEntry;
      pObjectEntry->qsDisplayName = pEntry->qsDisplayName;
      pInfo = pEntry->listFileInfos[i];
      if ( pInfo ) {
	pObjectEntry->qsFileName    = pInfo->qsFileName;
	pObjectEntry->listChapters  = pInfo->listChapters;
	if ( pObjectEntry->listChapters.count ( ) < 1 )
	  pObjectEntry->listChapters.append ( "00:00:00.000" );
      }
      m_listSourceObjects.append ( pObjectEntry );
    }
  }

  if ( m_listSourceObjects.count ( ) < 1 ) {
    MessageBox::warning ( this, tr ( "Nothing to select from." ), tr ( "There are no video sources in the project.\n" ), QMessageBox::Ok, Qt::NoButton );
    return false;
  }

  for ( t=0; t<(uint)listFileInfos.count ( ); t++ ) //oxx
    m_listDVDMenuSourceFiles.append ( listFileInfos[t]->qsFileName );

  populateList ( );

  // After we have all in place we look at what we have gotten from the associated button
  initButton ( list );    //oooo

  return true;
}

void DialogSelect::initButton ( QStringList &list )
{  
  // entries in the list should look like this :
  // jump+-+<SourceFileEntry-DisplayName>+-+FileName+-+chapter
  int t, i;
  QStringList listEntry;
  QString qsEntryName, qsFileName, qsChapter, qsFinal;

  if ( ! list.isEmpty() ) { //xxx

    // first we check if the last entry in the list is a call to a Menu.
    qsFinal = list.last ( );

    //listEntry = QStringList::split  ( STRING_SEPARATOR, qsFinal );  //ooo
    listEntry = qsFinal.split  ( STRING_SEPARATOR );  //xxx

    if ( listEntry.count ( ) == 2 )
      m_pComboFinal->setCurrentText ( listEntry[1] );

    // Now for this to work we nee to have m_pListAvailable populated
    for ( t=list.count ( )-1; t>=0; t-- ) {
      //listEntry = QStringList::split  ( STRING_SEPARATOR, list[t] );    //ooo
      listEntry = list[t].split  ( STRING_SEPARATOR );    //xxx
      if ( listEntry.count ( ) == 4 ) {
        qsEntryName = listEntry[1];
        qsFileName  = listEntry[2];
        qsChapter   = listEntry[3];

        for ( i=0; i<(int)m_pListAvailable->count ( ); i++ ) {
	      //if ( m_pListAvailable->text ( i ) == qsFileName ) {  //ooo
          if ( m_pListAvailable->item ( i )->text ( ) == qsFileName ) {   //xxx
	        //new Q3ListViewItem ( m_pListSelected, qsFileName, qsChapter ); //ooo
            QTreeWidgetItem *video = new QTreeWidgetItem ( m_pListSelected );   //xxx
            video->setText(0, qsFileName);	//xxx
            video->setText(1, qsChapter);	//xxx
	        //m_pListAvailable->removeItem ( i );    //ooo
            m_pListAvailable->removeItemWidget ( m_pListAvailable->item ( i ) );  //xxx
	        break; // leave the inner loop.
	      }
        }
      }
    }
  } //xxx
}

void DialogSelect::slotCheckLoop ( bool bChecked )
{
  m_pComboFinal->setEnabled ( !bChecked );
}

void DialogSelect::slotSelectionChanged ( )
{
  QTimer::singleShot ( 1, this, SLOT ( slotChangeChapters ( ) ) );
}

void DialogSelect::slotChangeChapters ( )
{
  //QString qsFile = m_pListAvailable->currentText ( ); //ooo
  QString qsFile = m_pListAvailable->currentItem ( )->text ( ); //xxx
  if ( qsFile.isEmpty ( ) )
    return;
  QFileInfo fileInfo;
  uint t;

  for ( t=0; t<(uint)m_listSourceObjects.count ( ); t++ ) { //oxx
    fileInfo.setFile ( m_listSourceObjects[t]->qsFileName );
    if ( fileInfo.fileName ( ) == qsFile ) {
      m_pComboChapters->clear ( );
      //m_pComboChapters->insertStringList( m_listSourceObjects[t]->listChapters ); //ooo
      m_pComboChapters->addItems( m_listSourceObjects[t]->listChapters );   //xxx
	  if ( m_pComboChapters->count  ( ) > 0 )
           //m_pComboChapters->setCurrentItem ( 0 );    //ooo
           m_pComboChapters->setCurrentIndex ( 0 );     //xxx
      return;
    }
  }
}

void DialogSelect::slotButtonUp ( )
{
  //Q3ListViewItem *pItem = m_pListSelected->currentItem ( );   //ooo
  QTreeWidgetItem *pItem = m_pListSelected->currentItem ( );    //xxx
  if ( ! pItem )
    return;
  //Q3ListViewItem *pPrevItem = pItem->itemAbove ( );   //ooo
  QTreeWidgetItem *pPrevItem = pItem->treeWidget ( )->itemAbove ( pItem );    //xxx
  if ( ! pPrevItem )
    return;
  //pPrevItem = pPrevItem->itemAbove ( );   //ooo
  pPrevItem = pPrevItem->treeWidget ( )->itemAbove ( pPrevItem );    //xxx

  //m_pListSelected->setSortColumn ( -1 );  //ooo
  /*if ( ! pPrevItem ) {
    //m_pListSelected->takeItem    ( pItem );   //ooo
    m_pListSelected->removeItemWidget    ( pItem, 0 ); //xxx
    //m_pListSelected->insertItem  ( pItem );   //ooo
    m_pListSelected->addTopLevelItem  ( pItem );    //xxx
    //m_pListSelected->setSelected ( pItem, TRUE ); //ooo
    return;
  }*/   //ooo
  //pItem->moveItem ( pPrevItem );  //oooo
  
  int index = pItem->treeWidget ( )->indexOfTopLevelItem(pItem);    //xxx
  QTreeWidget *parent = pItem->treeWidget ( );                                  //xxx
  QTreeWidgetItem *child = pItem->treeWidget ( )->takeTopLevelItem ( index );   //xxx
  parent->insertTopLevelItem ( index - 1, child );   //xxx
}

void DialogSelect::slotButtonDown ( )
{
  //Q3ListViewItem *pItem = m_pListSelected->currentItem ( );   //ooo
  QTreeWidgetItem *pItem = m_pListSelected->currentItem ( );    //xxx
  if ( ! pItem )
    return;
  //Q3ListViewItem *pNextItem = pItem->itemBelow ( );   //ooo
  QTreeWidgetItem *pNextItem = pItem->treeWidget ( )->itemBelow ( pItem );    //xxx
  if ( ! pNextItem )
    return;
  //m_pListSelected->setSortColumn ( -1 );  //ooo
  //pItem->moveItem ( pNextItem );  //oooo
  
  //QTreeWidgetItem* parent = pItem->parent();            //xxx
  //int index = parent->indexOfChild(pItem);              //xxx
  //QTreeWidgetItem* child = parent->takeChild(index);    //xxx
  //parent->insertChild(index+1, child);                  //xxx
  
  int index = pItem->treeWidget ( )->indexOfTopLevelItem(pItem);    //xxx
  QTreeWidget *parent = pItem->treeWidget ( );                                  //xxx
  QTreeWidgetItem *child = pItem->treeWidget ( )->takeTopLevelItem ( index );   //xxx
  parent->insertTopLevelItem ( index + 1, child );   //xxx
}

void DialogSelect::slotButtonAdd ( )
{
  //Q3ListBoxItem *pNextInLine  = NULL; //ooo
  QListWidgetItem *pNextInLine  = NULL; //xxx
  //Q3ListBoxItem *pCurrentItem = NULL; //ooo
  QListWidgetItem *pCurrentItem = NULL; //xxx
  //Q3ListBoxItem *pItem        = m_pListAvailable->firstItem ( );  //ooo
  QListWidgetItem *pItem        = m_pListAvailable->item ( 0 );  //xxx
  //QList<Q3ListBoxItem *> list;		//oxx
  QList<QListWidgetItem *> list;		//xxx
  while ( pItem )  {
     if ( pItem->isSelected ( ) )
          list.append   ( pItem );
    //pItem = pItem->next ( );  //ooo
    pItem = pItem->listWidget ( )->item ( pItem->listWidget ( )->row ( pItem ) + 1 );  //xxx
  }
  if ( list.size ( ) < 1 )
    return;

  // This is so we can select the next in line ...
  pNextInLine = list.last ( );
  if ( pNextInLine ) {
    //pNextInLine = pNextInLine->next ( );  //ooo
    pNextInLine = pNextInLine->listWidget ( )->item ( pNextInLine->listWidget ( )->row ( pNextInLine ) + 1 );  //xxx
    if ( ! pNextInLine )
      //pNextInLine = list.last ( ) ->prev ( ); //ooo
      pNextInLine = list.last ( ) ->listWidget ( )->item ( list.last ( )->listWidget ( )->row ( list.last ( ) ) - 1 );  //xxx
    /*if ( pNextInLine )
       m_pListAvailable->setSelected ( pNextInLine, true );*/   //oooo
  }

  QString qsChapter = m_pComboChapters->currentText ( );
  //pCurrentItem = m_pListAvailable->item ( m_pListAvailable->currentItem ( ) );    //ooo
  pCurrentItem = m_pListAvailable->currentItem ( ); //xxx

  // only the first selected item gets the selected chapter.
  for ( int t=list.count ( )-1; t>=0; t-- ) {
    pItem = list[t];
    if ( pItem ) {
      if ( pItem == pCurrentItem ) {
	    //new Q3ListViewItem ( m_pListSelected, pItem->text ( ), qsChapter );  //ooo
        QTreeWidgetItem *video = new QTreeWidgetItem ( m_pListSelected );   //xxx
        video->setText(0, pItem->text ( ));	//xxx
        video->setText(1, qsChapter);	//xxx
      } else {
	    //new Q3ListViewItem ( m_pListSelected, pItem->text ( ), QString ( "00:00:00.000" ) ); //ooo
        QTreeWidgetItem *video = new QTreeWidgetItem ( m_pListSelected );   //xxx
        video->setText(0, pItem->text ( ));	//xxx
        video->setText(1, QString ( "00:00:00.000" ));	//xxx
      }
      delete pItem;
    }
  }
}

void DialogSelect::slotButtonRemove ( )
{
  //Q3ListViewItem *pItem = m_pListSelected->currentItem ( );   //ooo
  QTreeWidgetItem *pItem = m_pListSelected->currentItem ( );    //xxx
  if ( ! pItem )
    return;
  QString qsCurrent    = pItem->text ( 0 );
  if ( qsCurrent.isEmpty ( ) )
    return;
  
  //m_pListSelected->removeItem  ( m_pListSelected->currentItem ( ) );  //ooo
  //m_pListSelected->removeItemWidget  ( m_pListSelected->currentItem ( ), 0 );   //xxx
  m_pListSelected->takeTopLevelItem  ( m_pListSelected->indexOfTopLevelItem ( m_pListSelected->currentItem ( ) ) );   //xxx
  //m_pListAvailable->insertItem ( qsCurrent ); //ooo
  m_pListAvailable->insertItem ( 0, qsCurrent );    //xxx
}

void DialogSelect::slotButtonAddAll ( )
{
  int t;

  if ( m_bShowAll )
    m_pButtonShowAll->setText ( tr ( "All Available Videos" ) );
  else
    m_pButtonShowAll->setText ( tr ( "All of %1" ).arg ( m_qsCurrentMenuName ) );

  for ( t=(int)m_pListAvailable->count ( )-1; t>=0; t-- ) {
    //new Q3ListViewItem ( m_pListSelected, m_pListAvailable->text ( t ), "00:00:00.000" ); //ooo
    QTreeWidgetItem *video = new QTreeWidgetItem ( m_pListSelected ); //ooo
    video->setText(0, m_pListAvailable->item ( t )->text ( ));	//xxx
    video->setText(1, QString ( "00:00:00.000" ));	//xxx
  }
  m_pListAvailable->clear ( );
}

void DialogSelect::slotButtonRemoveAll ( )
{
  m_pListSelected->clear ( );
  populateList ( );
}

void DialogSelect::slotButtonShowAll ( )
{
  // switches between displaying all available vide sources or only the ones of the current subMenu.
  m_bShowAll = ! m_bShowAll;
  populateList ( );
}

void DialogSelect::slotHeaderClicked ( int iColumn )
{
  //m_pListSelected->setSortColumn ( iColumn ); //oooo
}

void DialogSelect::populateList ( )
{
  uint      t;
  QFileInfo fileInfo;
  bool      bFound;
  //Q3ListViewItem *pItem;  //ooo
  QTreeWidgetItem *pItem;   //xxx

  // switches between displaying all available vide sources or only the ones of the current subMenu.
  m_pListAvailable->clear ( );
  if ( m_bShowAll ) {
    m_pButtonShowAll->setText ( tr ( "All Available Videos" ) );
    for ( t=0; t<(uint)m_listSourceObjects.count ( ); t++ ) {   //oxx
      fileInfo.setFile ( m_listSourceObjects[t]->qsFileName );
      bFound = false;
      //pItem = m_pListSelected->firstChild ( );    //ooo
      //pItem = m_pListSelected->topLevelItem(0)->child(0); //xxx
      pItem = m_pListSelected->topLevelItem(0); //xxx
      while ( pItem ) {
	    if  ( pItem->text ( 0 ) == fileInfo.fileName ( ) ) {
	      bFound = true;
	      break; // exit inner loop
	    }
	    //pItem = pItem->nextSibling ( );  //ooo
	    pItem = pItem->treeWidget()->itemBelow(pItem);	//xxx
      }
      if ( ! bFound )
	    //m_pListAvailable->insertItem ( fileInfo.fileName ( ) );  //ooo
        m_pListAvailable->insertItem ( 0, fileInfo.fileName ( ) );  //xxx
    } // end for - loop
  }
  else {
    m_pButtonShowAll->setText ( tr ( "All of %1" ).arg ( m_qsCurrentMenuName ) );
    for ( t=0; t<(uint)m_listDVDMenuSourceFiles.count ( ); t++ ) {  //oxx
      fileInfo.setFile ( m_listDVDMenuSourceFiles[t] );
      bFound = false;
      //pItem = m_pListSelected->firstChild ( );    //ooo
      //pItem = m_pListSelected->topLevelItem(0)->child(0); //xxx
      pItem = m_pListSelected->topLevelItem(0); //xxx
      while ( pItem ) {
	    if  ( pItem->text ( 0 ) == fileInfo.fileName ( ) ) {
	      bFound = true;
	      break; // exit inner loop
	    }
	    //pItem = pItem->nextSibling ( );  //ooo
	    pItem = pItem->treeWidget()->itemBelow(pItem);	//xxx
      }
      if ( ! bFound )
	    //m_pListAvailable->insertItem ( fileInfo.fileName ( ) );  //ooo
        m_pListAvailable->insertItem ( 0, fileInfo.fileName ( ) );  //xxx
    } // end for - loop
  }
}

bool DialogSelect::getLoop ( )
{
  return m_pCheckLoop->isChecked ( );
}

QString DialogSelect::getFinalDestination ( )
{
  return m_pComboFinal->currentText ( );
}

QStringList DialogSelect::getSelected ( )
{
  uint           t;
  QString        qsItem;
  QFileInfo      fileInfo;
  QStringList    listReturn;
  //Q3ListViewItem *pItem;  //ooo
  QTreeWidgetItem *pItem;   //xxx
  ObjectEntry   *pSourceObject;

  //pItem = m_pListSelected->firstChild ( );    //ooo
  pItem = m_pListSelected->topLevelItem(0)->child(0); //xxx
  while ( pItem ) {
    for ( t=0; t<(uint)m_listSourceObjects.count ( ); t++ ) {   //oxx
      pSourceObject = m_listSourceObjects[t];
      fileInfo.setFile ( pSourceObject->qsFileName );
      if ( fileInfo.fileName ( ) == pItem->text ( 0 ) ) {
	qsItem  = QString ( "jump%1" ).arg ( STRING_SEPARATOR );
	qsItem += QString ( "%1%2"   ).arg ( pSourceObject->qsDisplayName ).arg ( STRING_SEPARATOR );
	qsItem += pItem->text ( 0 ) + STRING_SEPARATOR;
	qsItem += pItem->text ( 1 );
	listReturn.append( qsItem );
      }
    }
    //pItem = pItem->nextSibling ( );   //ooo
    pItem = pItem->treeWidget()->itemBelow(pItem);	//xxx
  }

  return listReturn;
}
