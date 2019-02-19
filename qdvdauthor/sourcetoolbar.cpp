/***************************************************************************
    sourcetoolbar.cpp
                             -------------------
    SourceToolBar - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will create a toolbar, which displays all currently selected
   sources for the DVD, such as all Videos, and all Audio files.

   Besides displaying these it will also do some background checks.
   For this it will spawn a new thread, which will analyse the file
   and see if it needs to get converted for a propper usage.

   tcscan is your friend.
 
   The result will be color coded.
   o	Gray, not yet determined
   o	any shades of green -> In DVD format.
   o	any shades of blue  -> will be converted before creating DVD
   o	any shades of red   -> Can not automatically handle this format.
    
****************************************************************************/

#include <QMessageBox>
#include <QPixmap>
#include <QAction>
#include <QMenu>

#include "global.h"
#include "dvdmenu.h"		//oooo
#include "sourcetoolbar.h"
#include "qdvdauthor.h"
#include "listviewitemmedia.h"	//ooo
#include "dvdmenuundoobject.h"	//oooo
#include "menupreview.h"	//oooo
#include "messagebox.h"
#include "dialogmovie.h"
#include "filepreviewdialog.h"
#include "filepreview_dialog.h" //xxx
//#include "sourcefileentry.h"	//xxx
#include "dialogfiles.h"
#include "dialogimages.h"
#include "smalldialogaudio.h"
#include "xml_slideshow.h"

#include "render_client.h"
#include "qplayer/mediainfo.h"
#include "qplayer/mediacreator.h"

#define SOURCE_FILE_ENTRY_ID   0
//#define SOURCE_FILE_ENTRY_NAME 1	//ooo
#define SOURCE_FILE_ENTRY_NAME 0	//xxx

SourceToolBar::SourceToolBar ( const char *pName, QMainWindow *pMainWindow )		//xxx
  : QDockWidget(pName, pMainWindow )
{
  
  m_pMediaInfo       = NULL;
  //m_pThumbnailDialog = NULL;	//oooo
  m_pContexMenuEntry = NULL;
  m_pContexMenuInfo  = NULL;
  
  // Next is to create a MediaEngine in a separate thread to keep the application running and not stottering ...
  m_pMediaInfo   = MediaCreator::createInfo ( );
  
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  /*customerList = new QListWidget;
  customerList->addItems(QStringList()
           << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
           << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
           << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
           << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
           << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
           << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
  setWidget(customerList);
  pMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);*/
   
  m_pSourceWidget = new SourceWidget ( this );
  
  //m_pSourceWidget->m_pListViewStructure->clear   ( );		//xxx
  m_pSourceWidget->m_pListViewAllSources->clear  ( );
  m_pSourceWidget->m_pListViewVideoSources->clear ( );
  m_pSourceWidget->m_pListViewAudioSources->clear ( );
  
  //m_pSourceWidget->m_pListViewVideoSources->setItemsExpandable ( true );	//xxx
  
  /*int x, iSize = 32, type = 1;  // 19 icons stored in this one pix.
  x = type * iSize;
   
  QPixmap allImages ( ":/images/structure_icons.png" );
  QPixmap thePixmap ( iSize, iSize );
  thePixmap = allImages.copy(x, 0, iSize, iSize);
   
  QTreeWidgetItem *planets2 = new QTreeWidgetItem(m_pSourceWidget->m_pListViewStructure);
  planets2->setText(0, tr("DVD Project"));
  planets2->setIcon(0, thePixmap);*/
  
  //QTreeWidgetItem *sources = new QTreeWidgetItem(m_pSourceWidget->m_pListViewAllSources);	//xxx
  //sources->setText(0, tr("DVD Sources"));
   
  /*setResizeEnabled (  true );
  setMovingEnabled (  true );
  setNewLine       ( false );*/
  pMainWindow->addDockWidget ( Qt::LeftDockWidgetArea, this );		//xxx
   
  setWidget(m_pSourceWidget);			//xxx
  //resize(1500,1500);  //xxx
  //setMaximumSize(200,200);  //xxx
  //setMaximumWidth(800); //xxx
   
  //QGridLayout *tabLayout = new QGridLayout   ( m_pSourceWidget->tab, 1, 1, 4, 4, "tabLayout" );
   
  //connect( m_pListViewAllSources, SIGNAL( doubleClicked ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT (slotDoubleClickedOnVideoList(Q3ListViewItem *, const QPoint &, int) ) );		//ooo
  connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemClicked ( QTreeWidgetItem *, int ) ), this, SLOT (slotRightClickedOnVideoList(QTreeWidgetItem *, int) ) );	//xxx
  //connect( m_pListViewAllSources, SIGNAL( clicked ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT (slotClickedOnVideoList      (Q3ListViewItem *, const QPoint &, int) ) );			//ooo
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemClicked ( QTreeWidgetItem *, int ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );		//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemSelectionChanged ( ) ), this, SLOT ( slotAddMovie     ( ) ) );						//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );	//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( clicked ( QTreeWidgetItem *, int ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );		//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemSelectionChanged ( ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );		//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( activated ( ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );		//xxx
  //connect( m_pSourceWidget->m_pListViewAllSources, SIGNAL( itemActivated ( QTreeWidgetItem *, int ) ), this, SLOT (slotClickedOnVideoList      (QTreeWidgetItem *, int) ) );		//xxx
   
  connect ( m_pSourceWidget->m_pButtonAddMovie,  SIGNAL( clicked ( ) ), this, SLOT ( slotAddMovie     ( ) ) );
  connect ( m_pSourceWidget->m_pButtonAddSlide,  SIGNAL( clicked ( ) ), this, SLOT ( slotAddSlideshow ( ) ) );
  
  //connect( pDVDAuthor, SIGNAL( signalAddBackground ( ) ), this, SLOT( slotAddBackground ( ) ) );	//xxx
  //connect( pMainWindow, SIGNAL( signalAddSound ( ) ), this, SLOT( slotAddSound ( ) ) );				//xxx
  connect( pMainWindow, SIGNAL( signalAddVideo ( ) ), this, SLOT( slotAddVideo ( ) ) );				//xxx
   
}

SourceToolBar::~SourceToolBar ()
{
  uint t;
  MediaCreator::unregisterFromMediaScanner   (  this  );
  for ( t=0; t<(uint)m_listSourceFileEntries.count ( ); t++ )	//ox
    delete m_listSourceFileEntries[t];
  m_listSourceFileEntries.clear ( );	//oooo
  if ( m_pMediaInfo )
    delete m_pMediaInfo;
  m_pMediaInfo = NULL;
/*  if ( m_pThumbnailDialog )
    delete m_pThumbnailDialog;
  m_pThumbnailDialog = NULL; */		//oooo
}

//QListView *SourceToolBar::getStructureListView ( )	//ooo
//QTreeView *SourceToolBar::getStructureListView ( )	//xxx
//QTreeWidgetItem *SourceToolBar::getStructureListView ( )	//xxx
QTreeWidget *SourceToolBar::getStructureListView ( )	//xxx
{
  return m_pSourceWidget->m_pListViewStructure;			//ooo
  //return m_pSourceWidget->m_pListViewStructure->currentItem();	//xxx
  //return m_pSourceWidget->m_pListViewStructure;				//xxx
  //return m_pSourceWidget->m_pTreeViewStructure;			//xxx
}

void *SourceToolBar::getMediaEngine ()
{
	static int iCounter = 0;
	while (!m_pMediaInfo)	{
		// Init does not seem to have finished yet ...
		// So we wait for a second ...
		//sleep (1);				//ooo
		SleeperThread7::msleep ( 1000 );	//xxx
		// this counter will break out of the loop after 5 unsuccessfull seconds
		if (iCounter ++ > 5)
			return NULL;
	}
	return m_pMediaInfo->getMediaEngine ();	//oooo
}

MediaInfo *SourceToolBar::getMediaInfo ()
{
  // This is just to ensure we have a engine already created ...
  getMediaEngine ();
  return m_pMediaInfo;
}

int SourceToolBar::sourceFileCount()
{
	return (int)m_listSourceFileEntries.count();
}

SourceFileEntry *SourceToolBar::sourceFileEntry(uint iEntryNumber)
{
	return m_listSourceFileEntries[iEntryNumber];
}

SourceFileEntry *SourceToolBar::sourceFileEntry ( CXmlSlideshow *pSlideshow )
{
  return (SourceFileEntry *)pSlideshow->id;
if ( 0 )  {
// Version 1 ... Make sure the Entry still exists
  SourceFileEntry *pEntry = NULL;
  QList<SourceFileEntry *>::iterator it = m_listSourceFileEntries.begin ( );		//oxx
  while ( it != m_listSourceFileEntries.end ( ) )  {
    pEntry = *it++;
    if ( pEntry->listFileInfos.count ( ) != 1 )
      continue;
    if ( pSlideshow->id == pEntry )
      return pEntry;
  }
  return NULL;
}
if ( 0 )  {
// Version 2 Check that the slideshow pointer is the same
  SourceFileEntry *pEntry = NULL;
  SourceFileInfo  *pInfo  = NULL;
  QList<SourceFileEntry *>::iterator it = m_listSourceFileEntries.begin ( );		//oxx
  while ( it != m_listSourceFileEntries.end ( ) )  {
    pEntry = *it++;
    if ( pEntry->listFileInfos.count ( ) != 1 )
      continue;
    pInfo = pEntry->listFileInfos[0];
    if ( ! pInfo->pSlideshow )
      continue;
    if ( pInfo->pSlideshow == pSlideshow )
      return pEntry;
  }
  return NULL;
}
}

bool SourceToolBar::sourceFileEntryExists ( SourceFileEntry *pEntry )
{
  QList<SourceFileEntry *>::iterator it = m_listSourceFileEntries.begin ( );		//oxx
  while ( it != m_listSourceFileEntries.end ( ) )  {
    if ( pEntry == *it++ )
      return true;
  }
  return false;
}

SourceFileEntry *SourceToolBar::getCurrentSourceFileEntry ()
{
  SourceFileEntry *pSourceFileEntry = m_pContexMenuEntry;
  if ( pSourceFileEntry )  {
    m_pContexMenuEntry = NULL;
    return pSourceFileEntry;
  }

  //ListViewItemMedia *pItem = (ListViewItemMedia *)m_pListViewAllSources->currentItem();			//ooo
  ListViewItemMedia *pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->currentItem();	//xxx
  if (!pItem)
    return NULL;
  pSourceFileEntry = pItem->sourceFileEntry ();
  // If the SourceFileEntry is NULL then we have a SourceFileInfo and
  // thus get the SourceFileEntry from the  parent item ..
  if (!pSourceFileEntry)
    pSourceFileEntry = ((ListViewItemMedia *)pItem->parent())->sourceFileEntry();
  return pSourceFileEntry;
}

SourceFileInfo *SourceToolBar::getCurrentSourceFileInfo ()
{
  SourceFileInfo  *pSourceFileInfo  = m_pContexMenuInfo;
  SourceFileEntry *pSourceFileEntry = NULL;

  if ( pSourceFileInfo )  {
    m_pContexMenuInfo = NULL;
    return pSourceFileInfo;
  }

  //ListViewItemMedia *pItem = (ListViewItemMedia *)m_pListViewAllSources->currentItem();			//ooo
  ListViewItemMedia *pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->currentItem();	//xxx

  if (!pItem)
    return NULL;

  pSourceFileInfo  = pItem->sourceFileInfo  ();

  // If the Info is NULL then we have a header and want the first FileInfo of it ...
  if (!pSourceFileInfo)	{
    pSourceFileEntry = pItem->sourceFileEntry ();
    pSourceFileInfo = pSourceFileEntry->listFileInfos[0];
  }

  return pSourceFileInfo;
}

//void SourceToolBar::slotDoubleClickedOnVideoList(Q3ListViewItem *pItem, const QPoint &, int)	//ooo
void SourceToolBar::slotDoubleClickedOnVideoList(QTreeWidgetItem *pItem, int)			//xxx
{
/*  // First we check if the item is valid ...
  if (pItem == NULL)	{
    slotAddMovieOld ( );
    return;
  }

  if (m_listSourceFileEntries.count() > 0) {
    SourceFileEntry *pEntry = getCurrentSourceFileEntry ( );
    if ( ( pEntry ) && ( pEntry->bSoundSource ) )
      slotEditAudio ( );
    else
      slotEditMovie ( );
  }*/
}

//void SourceToolBar::slotRightClickedOnVideoList ( Q3ListViewItem *pItem, const QPoint &thePoint, int )	//ooo
void SourceToolBar::slotRightClickedOnVideoList ( QTreeWidgetItem *pItem, int )					//xxx
{
  // First we check if the item is valid ...
  if ( !  pItem  )  {
    slotAddMovieOld ( );
    return;
  }		//ooo
  // here we handle the movie - right click ...
  // Here we pop open the properties dialog for the movie, where you can set
  // o size of movie (resizing tool used = ???
  // o Define Title # / Add Chapters to Title

  // Here we find out if we can actually play the file ... Meaning slideshows can not be played !!!
  uint t;
  bool bShow = true;
  SourceFileEntry *pSourceFileEntry = NULL;

  for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	{	//ox 
    if (pItem->text(SOURCE_FILE_ENTRY_NAME) == m_listSourceFileEntries[t]->qsDisplayName)	{
      if (m_listSourceFileEntries[t]->bIsSlideshow)
        bShow = false;
      pSourceFileEntry = m_listSourceFileEntries[t];
      break;
    }
  }		//ooo

  SourceFileEntry *pEntry = getCurrentSourceFileEntry ( );		//oooo

  QAction *deleteSourceAction = new QAction("Delete", this /*pItem->treeWidget()*/ /*m_pSourceWidget->m_pListViewVideoSources->viewport()*/);
  QAction *propertiesAudioAction = new QAction("&Properties ...", this);
  QAction *propertiesMovieAction = new QAction("&Properties ...", this);
  QAction *transcodeAllAction = new QAction("&Transcode All ...", this);
  QAction *playAction = new QAction("&Play", this);
  QAction *reRenderAction = new QAction("&Re-Render", this);
  QAction *thumbnailsAction = new QAction("&Thumbnails ...", this);
  
  //Q3PopupMenu *pMenu = new Q3PopupMenu ( m_pListViewAllSources );	//ooo
  QMenu *pMenu = new QMenu ( this );
  
  if ( pEntry ) {
    if ( pEntry->bSoundSource ) {
      pMenu->addAction(propertiesAudioAction);
    }
    else {
      SourceFileInfo *pInfo = getCurrentSourceFileInfo ( );
      if ( ! pInfo )  {
        delete pMenu;
        return;
      }
      // Not a sound source
      if ( bShow )
        pMenu->addAction(playAction);
      
      if ( pInfo->pSlideshow )  { // If the movie was created through a slideshow ...
        pMenu->addAction(reRenderAction); 
      }
      pMenu->addAction(propertiesMovieAction);
 
      if ( pSourceFileEntry && (pSourceFileEntry->listFileInfos.count() > 1) ) {	//ooo
      //if ( pSourceFileEntry && (pSourceFileEntry->listFileInfos.count() > 0) ) {		//xxx
        pMenu->addAction(transcodeAllAction);
      }
      else if ( !pSourceFileEntry ) { // Here we handle if the user clicked on a SourceFileInfo
        SourceFileInfo *pInfo = getCurrentSourceFileInfo ( );
        if ( pInfo && pInfo->listChapters.count ( ) > 2 )
          pMenu->addAction(thumbnailsAction);  
      }
      else {
      }
    }
    pMenu->addAction(deleteSourceAction);
  }

  connect(propertiesAudioAction, SIGNAL(triggered()), this, SLOT( slotEditAudio     ( ) ) );
  connect(propertiesMovieAction, SIGNAL(triggered()), this, SLOT( slotEditMovie     ( ) ) );
  connect(deleteSourceAction,    SIGNAL(triggered()), this, SLOT( slotDeleteSource  ( ) ) );
  connect(transcodeAllAction,    SIGNAL(triggered()), this, SLOT( slotTranscodeAll  ( ) ) );
  connect(playAction,            SIGNAL(triggered()), this, SLOT( slotPlayMovie     ( ) ) );
  connect(reRenderAction,        SIGNAL(triggered()), this, SLOT( slotRerenderSlide ( ) ) );
  
  pMenu->exec( QCursor::pos() );

  delete pMenu;

  /*if ( pEntry ) {
    if ( pEntry->bSoundSource ) {
      pMenu->insertItem ( tr ("Proprties ..."),           this, SLOT ( slotEditAudio      ( ) ) );
    }
    else {
      SourceFileInfo *pInfo = getCurrentSourceFileInfo ( );
      if ( ! pInfo )  {
        delete pMenu;
        return;
      }
      // Not a sound source
      if ( bShow )
        pMenu->insertItem ( tr ( "&Play" ),               this, SLOT ( slotPlayMovie      ( ) ) );

      if ( pInfo->pSlideshow )  { // If the movie was created through a slideshow ...
        pMenu->insertItem ( tr ( "&Slideshow" ),          this, SLOT ( slotEditSlideshow  ( ) ) );
        pMenu->insertItem ( tr ( "&Images ..." ),         this, SLOT ( slotImagesDialog   ( ) ) );
        pMenu->insertItem ( tr ( "&Re-Render" ),          this, SLOT ( slotRerenderSlide  ( ) ) );
      }
      pMenu->insertItem   ( tr ( "P&roperties ..." ),     this, SLOT ( slotEditMovie      ( ) ) );
      if ( pSourceFileEntry && (pSourceFileEntry->listFileInfos.count() > 1) ) {
        pMenu->insertItem ( tr ( "&Transcode All ..." ),  this, SLOT ( slotTranscodeAll   ( ) ) );
        pMenu->insertItem ( tr ( "&Disable Transcode" ),  this, SLOT ( slotTranscodeNone  ( ) ) );
        pMenu->insertItem ( tr ( "&All to titleset ..." ),this, SLOT ( slotAllToTitleset  ( ) ) );
        pMenu->insertItem ( tr ( "Thumbnails ..." ),      this, SLOT ( slotSFEThumbnailing( ) ) );
      }
      else if ( !pSourceFileEntry ) { // Here we handle if the user clicked on a SourceFileInfo
        SourceFileInfo *pInfo = getCurrentSourceFileInfo ( );
        if ( pInfo && pInfo->listChapters.count ( ) > 2 )
          pMenu->insertItem ( tr ( "Thumbnails ..." ),    this, SLOT ( slotThumbnailing   ( ) ) );
      }
      else {
        pMenu->insertItem ( tr ("Assign to titleset ..." ),this, SLOT( slotAllToTitleset  ( ) ) );
      }
    }
    pMenu->insertItem ( tr ("Delete"),                 this, SLOT(slotDeleteSource  ( ) ) );
    pMenu->exec(thePoint, 2);
  }*/	//oooo
  //delete pMenu;	//ooo
}

void SourceToolBar::slotAddVideo( )	//xxx
{
  slotAddMovieOld ( );

  // Finally we should guesstimate the new size.
  Global::pApp->slotCalculateSize ( );  //xxx  
}

void SourceToolBar::createContextMenu ( SourceFileEntry *pEntry, QPoint &pos )
{

}

//void SourceToolBar::slotClickedOnVideoList(Q3ListViewItem *pItem, const QPoint &thePoint, int)	//ooo
void SourceToolBar::slotClickedOnVideoList(QTreeWidgetItem *pItem, int i)
{
/*  // If no Item was clicked the user might want to add something ?
  if ( pItem == NULL )  {
    // Here we translate the clicked position to widget coordinates
    QPoint p ( m_pListViewAllSources->mapFromGlobal ( thePoint ) );
    // and since we actually are only looking for a height comparison,
    // we set x to always hit a entry (if there is one).
    p.setX ( 50 );
    if ( ! m_pListViewAllSources->itemAt ( p ) )*/
      //slotAddMovieOld ( );
  //}	//oooo

  if ( pItem->treeWidget()->indexOfTopLevelItem(pItem) == 0 ) {    	//xxx
    slotAddMovieOld ( );
    // Finally we should guesstimate the new size.
    Global::pApp->slotCalculateSize ( );  //xxx
  }
  
}

void SourceToolBar::slotSetThumbnail ( long iMSecOffset )
{  
  // The user wants to set the current frame as the thumbnail instead of frame 0.
  //ListViewItemMedia *pItem = (ListViewItemMedia *)m_pListViewAllSources->currentItem();                   //ooo
  ListViewItemMedia *pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->currentItem();    //xxx
  if ( ! pItem )
    return;
  SourceFileInfo *pSourceFileInfo = pItem->sourceFileInfo ( );
  if ( pSourceFileInfo )  { // create new thumbnail ...  
    pSourceFileInfo->iMSecPreview = iMSecOffset;
    MediaCreator::registerWithMediaScanner (this, pSourceFileInfo, iMSecOffset, false);
  }
}

void SourceToolBar::slotTranscodeAll()
{
  uint t;
  QFileInfo fileInfo;
  // Called when the user clicks on the SourceFileEntry - header.
  // This function will transcode all associated SourceFiles into the same format.
  SourceFileEntry *pSourceFileEntry = getCurrentSourceFileEntry ( );
  SourceFileInfo  *pSourceFileInfo  = getCurrentSourceFileInfo  ( );
  SourceFileInfo  *pFirstFileInfo   = NULL;

  if ( ! pSourceFileEntry )
    return; // Could not find the currently selected SourceFileEntry

  // Okay we only want to convert if the FileInfos have the same transcoding information (Either none, or all values the same.
  pFirstFileInfo = pSourceFileEntry->listFileInfos[0];
  for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{	//ox
    pSourceFileInfo = pSourceFileEntry->listFileInfos[t];
    if ( pFirstFileInfo && pFirstFileInfo->pTranscodeInterface )  {
      if ( ! pSourceFileInfo ) // error
	return;
      if ( ! pSourceFileInfo->pTranscodeInterface ) {
	if ( MessageBox::question ( NULL, tr ( "No current Transcoding info." ), 
               tr ( "Do you want to transcode all associated videos into the same format ?" ), 
               QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	  break; // exit the test loop
	return;
      }
      else if ( *pSourceFileInfo->pTranscodeInterface  != *pFirstFileInfo->pTranscodeInterface ) {
	if ( MessageBox::question ( NULL, tr ( "Transcoding info not the same." ), 
		tr ( "Warning if you continue you will transcode all videos into the same format.\nContinue ?" ), 
		QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	  break; // exit the test loop
	return;
      }
    }
    else if ( pSourceFileInfo && pSourceFileInfo->pTranscodeInterface ) {
      // the first entry has no transcoding interface but one of the subsequent entries has one.
      if ( MessageBox::question ( NULL, tr ( "Transcoding info not the same." ), 
	     tr ( "Warning if you continue you will transcode all videos into the same format.\nContinue ?" ), 
	     QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
	break; // exit the test loop
      return;
    }
  }

  // Here we open the MovieDialog.
  Utils theUtils;
  DialogMovie theDialog (this);
  theDialog.initMe (pSourceFileEntry);
  if (theDialog.exec () == QDialog::Rejected)
    return;

  // Check if there was a transcodeInterface created by the dialog.
  if (theDialog.sourceFileInfo()->pTranscodeInterface)	{
    for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{	//ox
      pSourceFileInfo = pSourceFileEntry->listFileInfos[t];
      if ( ! pSourceFileInfo->pTranscodeInterface )
	pSourceFileInfo->pTranscodeInterface = new TranscodeInterface;
      *pSourceFileInfo->pTranscodeInterface = *theDialog.sourceFileInfo()->pTranscodeInterface;
      fileInfo.setFile ( pSourceFileInfo->qsFileName );
      //pSourceFileInfo->pTranscodeInterface->qsTempFile = QString ( "%1/%2.mpeg2" ).arg (theUtils.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg(fileInfo.baseName ( TRUE ) );	//ooo
      pSourceFileInfo->pTranscodeInterface->qsTempFile = QString ( "%1/%2.mpeg2" ).arg (theUtils.getTempPath ( fileInfo.baseName ( ) ) ).arg(fileInfo.baseName ( ) );			//xxx
      updateListViewItem ( pSourceFileInfo );
    }
  }
  else	{	// If there is none then remove all transcodeInterfaces from the files.
    for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{	//ox
      pSourceFileInfo = pSourceFileEntry->listFileInfos[t];
      if ( pSourceFileInfo->pTranscodeInterface )	{
	delete pSourceFileInfo->pTranscodeInterface;
	pSourceFileInfo->pTranscodeInterface = NULL;
	updateListViewItem ( pSourceFileInfo );
      }
    }
  }
  pSourceFileEntry->qsPre  = theDialog.m_pComboPreCommand->currentText  ( );
  pSourceFileEntry->qsPost = theDialog.m_pComboPostCommand->currentText ( );

  updateEntryHeader ( pSourceFileEntry );
}

// This is without background task to get the info ...
void SourceToolBar::slotAddMovie ( )
{
  Input::DialogFiles fileDialog ( this );
  fileDialog.initMe     ( );
  if ( fileDialog.exec  ( ) == QDialog::Rejected )
    return;

  uint t, i;
  QColor statusColor (COLOR_MOVIE_FILES_OK);
  QList<SourceFileEntry *>tempList;					//oxx
  QList<SourceFileEntry *>newList = fileDialog.getGroups ( );		//oxx
  SourceFileInfo  *pInfo  = NULL;
  SourceFileEntry *pEntry = NULL;

  // First we remove all 'old' SourceFileEntry objects
  for ( t=0; t<(uint)m_listSourceFileEntries.count ( ); t++ ) {	//ox
    if ( ! m_listSourceFileEntries[t]->bSoundSource )
      tempList.append  ( m_listSourceFileEntries[t] );
  }
  for ( t=0; t<(uint)tempList.count ( ); t++ ) 	//ox
    removeEntry ( tempList[t], true );
  
  // Next we add the new ones ...
  for ( t=0; t<(uint)newList.count ( ); t++ ) {	//ox
    pEntry = newList[t];
    pEntry->sizeThumbnail = Global::pApp->getDragNDropContainer ( )->sizeThumbnail;
    m_listSourceFileEntries.append ( pEntry );
    for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ ) {	//ox
      pInfo    = pEntry->listFileInfos[i];
      if ( pInfo->bUpdateInfo ) {
	pInfo->bUpdateInfo = false;
	// Add the please - wait image
	//pInfo->pPreview  = new QImage ( QImage ( ).fromMimeSource ( "please_wait.jpg" ) );	//ooo
	pInfo->pPreview  = new QImage ( ":/images/please_wait.jpg" );				//xxx
	// and the request the preview from a background thread.
	//*pInfo->pPreview = pInfo->pPreview->smoothScale ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio );				//ooo
	*pInfo->pPreview = pInfo->pPreview->scaled ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
	MediaCreator::registerWithMediaScanner ( this, pInfo, pInfo->iMSecPreview, true );
      }
      else if ( pInfo->qsStatus !=  QString ( "Ok" ) )
        statusColor = QColor ( COLOR_MOVIE_FILES_BAD );

      /*new Q3ListViewItem ( m_pSourceWidget->m_pListViewVideoSources, pInfo->qsVideoFormat,
                          pInfo->qsResolution, pInfo->qsLength, pInfo->qsSize, pInfo->qsRatio,
                          pInfo->qsFPS, pInfo->qsStatus, pInfo->qsFileName );*/ //ooo
      
      QTreeWidgetItem *video = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources);	//xxx
      video->setText(0, pInfo->qsVideoFormat);	//xxx
      video->setText(1, pInfo->qsResolution);	//xxx
      video->setText(2, pInfo->qsLength);		//xxx
      video->setText(3, pInfo->qsSize);		//xxx
      video->setText(4, pInfo->qsRatio);		//xxx
      video->setText(5, pInfo->qsFPS);		//xxx
      video->setText(6, pInfo->qsStatus);		//xxx
      video->setText(7, pInfo->qsFileName);	//xxx
      
      delete video; //xxx 
    }
    //new ListViewItemMedia ( m_pListViewAllSources, pEntry, statusColor );   //ooo
    new ListViewItemMedia ( m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor );   //xxx
    updateEntryHeader     ( pEntry );
  }    
   
  // Finally we should guesstimate the new size.
  Global::pApp->slotCalculateSize ( );  
}

void SourceToolBar::slotAddMovieOld ( )
{
  if ( ! Global::pApp->checkForNewProject ( ) )
    return;	//oooo
  // Some one clicked the "Add Movie" button.
  QString qsMovieFilter, qsAudioFilter, qsExtension, qsFileName;
  int t;
  QFileInfo   fileInfo;
  QStringList listAddMovies, listAddAudio;

  void   *pExistingMediaEngine = getMediaEngine ( );	//ooo
  //void   *pExistingMediaEngine = NULL;			//xxx

  //pExistingMediaEngine = getMediaEngine ( );		//xxx

  qsMovieFilter =  Global::pApp->getMovieFilter ( );
  qsAudioFilter =  Global::pApp->getSoundFilter ( );
  //listAddMovies =  FilePreviewDialog::getOpenFileNames ( pExistingMediaEngine, Global::qsCurrentPath, QString ("Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));
  //QStringList listAddMovies = QFileDialog::getOpenFileNames (tr("Movie Files (")+qsFilter+QString (")"), Global::qsCurrentPath, this, tr("Open file dialog"), tr ("Add To Project ..."));
  //QFileDialog* pDialog = new FilePreview_Dialog(this, pExistingMediaEngine, "Open artwork", Global::qsCurrentPath, QString ("Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));              //xxx
  FilePreview_Dialog* pDialog = new FilePreview_Dialog(this, pExistingMediaEngine, "Open artwork", Global::qsCurrentPath, QString ("Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));              //xxxx
  
  connect(pDialog, SIGNAL(currentChanged(const QString&)), pDialog, SLOT(OnCurrentChanged(const QString&)));    //xxx
  // I tried to get the sorting of the names out of here but
  // detailViewSelectionChanged and listBoxSelectionChanged
  // are not flexible enough to handle this (yet).
  
  //if ( pDialog->exec ( ) == QDialog::Accepted )
  //if ( pDialog->exec ( ) )    
  //  listAddMovies = pDialog->selectedFiles ( );

  if ( pDialog->exec ( ) == QDialog::Rejected) {  //xxx
      
    pDialog->m_pMediaInterface->stop ( );  
      
    disconnect(pDialog, SIGNAL(currentChanged(const QString&)), pDialog, SLOT(OnCurrentChanged(const QString&)));  
      
    //delete pDialog;
 
    return;
  } else {
    pDialog->m_pMediaInterface->stop ( );  
    disconnect(pDialog, SIGNAL(currentChanged(const QString&)), pDialog, SLOT(OnCurrentChanged(const QString&)));  
    listAddMovies = pDialog->selectedFiles ( );
  }

  if (listAddMovies.count() < 1)  //ooo
    return;

  // Here we will check to see if the user selected audio files ...
  for ( t=0; t<(int)listAddMovies.count ( ); t++ ) {
    qsFileName = listAddMovies[t];
    fileInfo.setFile ( qsFileName );
    //qsExtension = fileInfo.extension ( FALSE ).lower ( );	//ooo
    qsExtension = fileInfo.suffix ( ).toLower ( );		//xxx
    qsExtension = qsExtension+" "; // add a trailing space to differntiate between mpeg and mpega
    //if ( qsAudioFilter.find ( qsExtension ) > -1 ) {	//ooo
    if ( qsAudioFilter.indexOf ( qsExtension ) > -1 ) {	//xxx
      listAddAudio .append  ( qsFileName );
      //listAddMovies.remove  ( qsFileName );	//ooo
      listAddMovies.removeOne  ( qsFileName );	//xxx
    }
  }

  addMovieList ( listAddMovies );
  addSound     ( listAddAudio  );	//oooo
}

void SourceToolBar::addMovieList (QStringList &listAddMovies, QString *pTitle)
{
  if ( listAddMovies.count ( ) < 1 ) 
    return;
  uint t;
  QFileInfo fileInfo (listAddMovies[0]);
  //QString qsPath = fileInfo.dirPath ( TRUE );		//ooo
  QString qsPath = fileInfo.path ( );			//xxx
  Global::qsCurrentPath = qsPath;

  QCursor myCursor (Qt::WaitCursor);
  QApplication::setOverrideCursor (myCursor);
  SourceFileEntry *pEntry = new SourceFileEntry;
  pEntry->sizeThumbnail   = Global::pApp->getDragNDropContainer()->sizeThumbnail;
  pEntry->bSoundSource    = false;
  SourceFileInfo *pInfo;

  for (t=0;t<(uint)listAddMovies.count();t++)	{	//ox
    //pInfo = new SourceFileInfo;		//ooo
    pInfo = new SourceFileInfo ( );		//xxx
    pInfo->qsFileName = listAddMovies[t];
    //pInfo->qsStatus = QString("Ok");		//xxx
    //pInfo->qsLength = QString("01:22:23");	//xxx
    // Here we generate a dummy image as a place holder
    //pInfo->pPreview   = new QImage (QImage().fromMimeSource( "please_wait.jpg" ));	//ooo
    pInfo->pPreview   = new QImage ( ":/images/please_wait.jpg" );			//xxx
    //*pInfo->pPreview  = pInfo->pPreview->smoothScale ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio);			//ooo
    *pInfo->pPreview  = pInfo->pPreview->scaled ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);	//xxx
    // and then register to obtain the info in the background.
    MediaCreator::registerWithMediaScanner ( this, pInfo, 0L, true );
    // register again if we need to extract Subtitles from MetaInfo
    pEntry->listFileInfos.append(pInfo);
  }
  // Check if there is something in the Entry left to be added to the sourceToolbar.
  if (pEntry->listFileInfos.count() == 0)	{
    delete pEntry;
    return;
  }

  if (pTitle)	{	// If a title has been provided
    pEntry->qsDisplayName = *pTitle;
  }
  else if (listAddMovies.count () > 1)	{
    QString qsText = tr(" Movie Files>");
    pEntry->qsDisplayName.sprintf ("[%02d] - <%02d", getFreeSourceSlot (), (int)listAddMovies.count());
    pEntry->qsDisplayName += qsText;
  }
  else
    //pEntry->qsDisplayName.sprintf ("[%02d] - %s", getFreeSourceSlot (), (const char *)QFile::encodeName ( fileInfo.baseName ( true ) ) );	//ooo
    pEntry->qsDisplayName.sprintf ("[%02d] - %s", getFreeSourceSlot (), (const char *)QFile::encodeName ( fileInfo.baseName ( ) ) );		//xxx

  //m_mutexThumbnails.lock ();	//xxx
    
  m_listSourceFileEntries.append(pEntry);

  // Okay, now we should also add this into the list box ...
  addMovie (pEntry);
  QApplication::restoreOverrideCursor ();	//oooo
  
  //m_mutexThumbnails.unlock ();	//xxx
}

bool SourceToolBar::checkRightAudioFormat ( SourceFileInfo *pInfo )
{
  QFileInfo fileInfo ( pInfo->qsFileName );
  if ( ( pInfo->qsAudioCodec != "mp2" ) &&
       ( pInfo->qsAudioCodec != "ac3" ) &&
       ( pInfo->qsAudioCodec != "pcm" )  ) {
    // Lets check the fileExtension
    //QString qsExt = fileInfo.extension ( FALSE );	//ooo
    QString qsExt = fileInfo.suffix ( );		//xxx
    if ( ( qsExt != "mp2" ) &&
	 ( qsExt != "ac3" ) &&
	 ( qsExt != "pcm" )  )
      return false;
  }
  // At this point we have one of the above
  //if ( pInfo->qsSample.find ( "48" ) == -1 )		//ooo
  if ( pInfo->qsSample.indexOf ( "48" ) == -1 )		//xxx
    return false;
  return true;
}

void SourceToolBar::setAudioTranscode ( SourceFileInfo *pInfo )
{
  if ( pInfo->pTranscodeInterface )
    return;

  TranscodeInterface *pTranscode = new TranscodeInterface;
  pInfo->pTranscodeInterface = pTranscode;
  //  pTranscode->qsTempFile     = QString ( "%1/%2.mpeg2" ).arg ( theUtil.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg(fileInfo.baseName ( TRUE ) );

  pTranscode->qsAudioFormat = "Stereo";
  pTranscode->qsAudioType   = "mp2";
  pTranscode->iAudioBitrate = 192; // 192kbps
  pTranscode->iSample       = 48000;
  pTranscode->bTranscoding  = false;
}

bool SourceToolBar::checkRightFileFormat ( SourceFileInfo *pInfo )
{
  Utils theUtil;
  // this function will check if we have a VOB stream.
  // if not, if we have a Mpeg2 stream
  // if not the set transcode parameters for this SourceFileInfo - object.
  
  // If we have a VOB - stream we can safely return ...
  // Note, we assume VOBUs == right resolution. Why else would you have a VOB stream ?
  if ( theUtil.hasVobus ( pInfo->qsFileName ) ) 
    return true;

  // If the user selected not to transcode this file, so be it.
  if ( ! pInfo->bAutotranscode )
    return true;

  // Also if there is already a TranscodeInterface defined, then keep our hands off !!!
  if ( pInfo->pTranscodeInterface ) 
    return true;

  QFileInfo fileInfo ( pInfo->qsFileName );
  int       iWidth, iHeight;
  // Everything else needs to be transcoded !!!
  TranscodeInterface *pTranscode = new TranscodeInterface;
  pInfo->pTranscodeInterface = pTranscode;
  //pTranscode->qsTempFile     = QString ( "%1/%2.mpeg2" ).arg ( theUtil.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg(fileInfo.baseName ( TRUE ) );	//ooo
  pTranscode->qsTempFile     = QString ( "%1/%2.mpeg2" ).arg ( theUtil.getTempPath ( fileInfo.baseName ( ) ) ).arg(fileInfo.baseName ( ) );		//xxx

  iWidth  = theUtil.getWHFromResolution ( pInfo->qsResolution, true  );
  iHeight = theUtil.getWHFromResolution ( pInfo->qsResolution, false );

  if ( theUtil.isMpeg2  ( pInfo->qsFileName ) && 
       //theUtil.getFormat ( iWidth, iHeight ) != FORMAT_NONE ) {	//ooo
       ( theUtil.getFormat ( iWidth, iHeight ) != FORMAT_NONE ) ) {	//xxx
  
    pTranscode->qsVideoFormat = pInfo->qsVideoFormat;
    pTranscode->bRemuxOnly = true;
    return true;
  }

  // Mpeg 2 stream detected. Lets check resolution.
  QString qsFormat          = "ntsc";
  pTranscode->fFrameRate    = 29.97f;
  pTranscode->qsResolution  = "720x480"; 
  //if ( pInfo->qsVideoFormat.lower ( ) == "pal") {	//ooo
  if ( pInfo->qsVideoFormat.toLower ( ) == "pal") {	//xxx
    qsFormat                = "pal";
    pTranscode->fFrameRate  = 25.0;
    pTranscode->qsResolution= "720x576"; 
  }

  pTranscode->qsVideoFormat = qsFormat;
  pTranscode->qsVideoType   = "mpeg2";
  pTranscode->iVideoBitrate = 6600000;
  pTranscode->qsRatio       = pInfo->qsRatio;
  pTranscode->qsAudioFormat = "Stereo";
  pTranscode->qsAudioType   = "mp2";
  pTranscode->iAudioBitrate = 192; // 192kbps
  pTranscode->iSample       = 48000;
  pTranscode->bTranscoding  = false;

  return true;
}

void SourceToolBar::timerEvent ( QTimerEvent *pTimerEvent )
{   
  // Called after the background thread has generated the preview ...
  uint t, i;
  int itemCounter = 1, p;	//xxx
  SourceFileEntry *pEntry = NULL;
  SourceFileInfo  *pInfo  = NULL;

  switch ( pTimerEvent->timerId() )	{
  case MEDIASCANNER_EVENT + 1: //  ExecuteJob::TYPE_INFO:
    m_mutexThumbnails.lock ();
    // We could simply call this function but it would be a brute force approach as the whole tree would get rebuild.
    //refreshToolBarEntries ();
    // So rather we check all SourceFileInfo's for the flag bUpdateImage and do so singularily.

    for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	{      
      pEntry = m_listSourceFileEntries[t];
      for (i=0;i<(uint)pEntry->listFileInfos.count();i++)	{
	// Okay found the SourceFileInfo now we need the ListViewItemMedia - object for this Info.
	pInfo = pEntry->listFileInfos[i];

	if (pInfo->bUpdateInfo)	{
	  // next we check if the file is an VOB stream, Mpeg2 stream or any other ...
	  checkRightFileFormat ( pInfo );	//oooo
	  //if (checkRightFileFormat ( pInfo ))	//xxx
	    //pInfo->qsStatus = QString("Ok");

	  //ListViewItemMedia *pChild = (ListViewItemMedia *)m_pListViewAllSources->firstChild ();				//ooo
/*	  ListViewItemMedia *pChild = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem(0)->child(0);	//xxx
	  //while ( pChild )  {		//ooo
	  while ( pChild )  {	//xxx

	    // Got it and it fits ...
	    pChild->replaceInfo ( pInfo  );
	    
	    //pChild = (ListViewItemMedia *)pChild->nextSibling();	//ooo
	    
	    if ( itemCounter < m_pSourceWidget->m_pListViewAllSources->topLevelItemCount() ) {	//xxx
	      //pChild = (ListViewItemMedia *)pChild->parent()->child(pChild->parent()->indexOfChild(pChild)+1);
	      //pChild = (ListViewItemMedia *)pChild->treeWidget()->itemBelow(pChild->parent()->child(0))->child(0);
	      //pChild = (ListViewItemMedia *)pChild->treeWidget()->itemBelow ( pChild );		//xxx
	      pChild = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem(itemCounter)->child(0);
	      itemCounter++;
	    } else
	      break;
	    
	  }*/	//ooo
	  
	  ListViewItemMedia *pChild = NULL;	//xxx
	  for ( p = 0; p < m_pSourceWidget->m_pListViewAllSources->topLevelItemCount(); p++ ) {	//xxx
	    
	    pChild = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem(p)->child(0);
	    
	    // Got it and it fits ...
	    pChild->replaceInfo ( pInfo  );
	  }

	  // Oh and the ListViewVideoSources should also be updated ...
	  //Q3ListViewItem *pItem = m_pSourceWidget->m_pListViewVideoSources->firstChild ( );	//ooo
	  QTreeWidgetItem *pItem = m_pSourceWidget->m_pListViewVideoSources->topLevelItem(0);	//xxx

	  while ( pItem )  {
	    if  ( pItem->text (MOVIE_FILE_NAME_INDEX) == pInfo->qsFileName )  {
	      pItem->setText ( 0, pInfo->qsVideoFormat );
	      pItem->setText ( 1, pInfo->qsResolution  );
	      pItem->setText ( 2, pInfo->qsLength      );
	      pItem->setText ( 3, pInfo->qsSize        );
	      pItem->setText ( 4, pInfo->qsRatio       );
	      pItem->setText ( 5, pInfo->qsFPS         );
	      pItem->setText ( 6, pInfo->qsStatus      );
	    }
	    //pItem = pItem->nextSibling ( );						//ooo
	    //pItem = pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
	    pItem = pItem->treeWidget()->itemBelow(pItem);				//xxx
	  }

	  pInfo->bUpdateInfo = false;
	  Global::pApp->slotCalculateSize ( );

	  Global::pApp->updateDVDLayout   ( pEntry );

	  updateEntryHeader ( pEntry );
	  m_mutexThumbnails.unlock ( );

	  // Finally once we have the required info, we can go ahead and create subtitles if needs be
	  Subtitles::generateSubtitles ( this, pInfo, Global::qsLanguage );
	  
	  return;
	}
      }
    }
    m_mutexThumbnails.unlock ();
    break;
  }	//oooo
}

void SourceToolBar::updateEntryHeader ( SourceFileEntry *pEntry )
{
  uint t, i;
  QString qsFormat ("--");
//  SourceFileInfo *pInfo;
  //Q3ListViewItem  *pItem;	//ooo
  QTreeWidgetItem  *pItem;
  // Oh and the ListViewVideoSources should also be updated ...
  for (t=0;t<(uint)pEntry->listFileInfos.count();t++)	{	//ox
    // Okay found the SourceFileInfo now we need the ListViewItemMedia - object for this Info.
//    pInfo = pEntry->listFileInfos[t];
    //pItem = m_pListViewAllSources->firstChild ();	//ooo
    pItem = m_pSourceWidget->m_pListViewAllSources->topLevelItem(0);	//xxx
    while ( pItem )	{
      if  ( pItem->text ( 1 ) == pEntry->qsDisplayName ) {	//ooo
      //if  ( pItem->text ( 0 ) == pEntry->qsDisplayName ) {
	// Okay, we found the Item that belongs to this SourceFileEntry
	if ( pEntry->listFileInfos.count () > 0 )
	  qsFormat = pEntry->listFileInfos[0]->qsVideoFormat;
	for (i=1;i<(uint)pEntry->listFileInfos.count();i++)	{	//ox
	  if ( pEntry->listFileInfos[i]->qsVideoFormat != qsFormat ) {
	    qsFormat = "Mixed";
	    break; // out of the inner for - loop
	  }
	}
	pItem->setText ( 0, qsFormat );
	return;
      }
      //pItem = pItem->nextSibling();						//ooo
      //pItem = pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
      pItem = pItem->treeWidget()->itemBelow(pItem);				//xxx
    }
  }
}

bool SourceToolBar::updateListViewItem ( SourceFileInfo *pInfo )
{
  bool bReturn = false;
  ListViewItemMedia *pItem, *pSubItem;

  // Okay found the SourceFileInfo now we need the ListViewItemMedia - object for this Info.
  //pItem = (ListViewItemMedia *)m_pListViewAllSources->firstChild ();					//ooo
  pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem ( 0 );	//xxx
  int itemCounter = 0;	//xxx
  int itemsNumber = m_pSourceWidget->m_pListViewAllSources->topLevelItemCount();	//xxx
  while ( pItem )	{
    //pSubItem = (ListViewItemMedia *)pItem->firstChild ();	//ooo
    pSubItem = (ListViewItemMedia *)pItem->child ( 0 );		//xxx
    itemCounter++;	//xxx
    while ( pSubItem ) {
      if  ( pSubItem->sourceFileInfo () == pInfo ) {
        pSubItem->updateItem ( );
        //m_pListViewAllSources->repaintItem ( pSubItem );	//ooo
	m_pSourceWidget->m_pListViewAllSources->setCurrentItem ( pSubItem );	//xxx
        bReturn  = true;
        pSubItem = NULL;
      }
      else {
        //pSubItem = (ListViewItemMedia *)pSubItem->nextSibling();							//ooo
	//pSubItem = (ListViewItemMedia *)pSubItem->itemBelow ( pSubItem );						//xxx
	pSubItem = (ListViewItemMedia *)pSubItem->parent()->child(pSubItem->parent()->indexOfChild(pSubItem)+1);	//xxx
      }
    }

    //pItem = (ListViewItemMedia *)pItem->nextSibling();								//ooo
    //pItem = (ListViewItemMedia *)pItem->itemBelow ( pItem );								//xxx
    //pItem = (ListViewItemMedia *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);			//xxx
    if (itemCounter < itemsNumber)	//xxx
      pItem = (ListViewItemMedia *)pItem->treeWidget()->topLevelItem(itemCounter);			//xxx
    else
      break;
  }
  
  //Q3ListViewItem *pVideoItem = m_pSourceWidget->m_pListViewVideoSources->firstChild ();	//ooo
  QTreeWidgetItem *pVideoItem = m_pSourceWidget->m_pListViewVideoSources->topLevelItem ( 0 );	//xxx
  int videoItemCounter = 0;	//xxx
  int videoItemsNumber = m_pSourceWidget->m_pListViewVideoSources->topLevelItemCount();	//xxx
  while ( pVideoItem ) {
    videoItemCounter++;		//xxx
    
    if  ( pVideoItem->text(   MOVIE_FILE_NAME_INDEX ) == pInfo->qsFileName ) {
      pVideoItem->setText ( 0, pInfo->qsVideoFormat );
      pVideoItem->setText ( 1, pInfo->qsResolution  );
      pVideoItem->setText ( 2, pInfo->qsLength      );
      pVideoItem->setText ( 3, pInfo->qsSize        );
      pVideoItem->setText ( 4, pInfo->qsRatio       );
      pVideoItem->setText ( 5, pInfo->qsFPS         );
      pVideoItem->setText ( 6, pInfo->qsStatus      );
      bReturn    = true;
      pVideoItem = NULL;
    }
    else {
      //pVideoItem = pVideoItem->nextSibling ();							//ooo
      //pVideoItem = pVideoItem->itemBelow ( pVideoItem );						//xxx
      //pVideoItem = pVideoItem->parent()->child(pVideoItem->parent()->indexOfChild(pVideoItem)+1);	//xxx
      
      if ( videoItemCounter < videoItemsNumber )	//xxx
	pVideoItem = pVideoItem->treeWidget()->topLevelItem(itemCounter);			//xxx
      else
	break;
    }
  }

  //Q3ListViewItem *pAudioItem = m_pSourceWidget->m_pListViewAudioSources->firstChild ();		//ooo
  QTreeWidgetItem *pAudioItem = m_pSourceWidget->m_pListViewAudioSources->topLevelItem ( 0 );	//xxx
  int audioItemCounter = 0;	//xxx
  int audioItemsNumber = m_pSourceWidget->m_pListViewAudioSources->topLevelItemCount();	//xxx
  while ( pAudioItem ) {
    audioItemCounter++;		//xxx
    
    if  ( pAudioItem->text( AUDIO_FILE_NAME_INDEX ) == pInfo->qsFileName ) {
      pAudioItem->setText ( 0, pInfo->qsAudioFormat );
      pAudioItem->setText ( 1, pInfo->qsLength      );
      pAudioItem->setText ( 2, pInfo->qsBits        );
      pAudioItem->setText ( 3, pInfo->qsSample      );
      pAudioItem->setText ( 4, pInfo->qsBPS         );
      pAudioItem->setText ( 5, pInfo->qsSize        );
      pAudioItem->setText ( 6, pInfo->qsStatus      );
      bReturn    = true;
      pAudioItem = NULL;
    }
    else {
      //pAudioItem = pAudioItem->nextSibling ();
      //pAudioItem = pAudioItem->parent()->child(pAudioItem->parent()->indexOfChild(pAudioItem)+1);	//xxx
      if ( audioItemCounter < audioItemsNumber )
	pAudioItem = pAudioItem->treeWidget()->topLevelItem(itemCounter);			//xxx
      else
	break;  
    }
  }	//oooo
  
  return bReturn;	//oooo
}

int SourceToolBar::getFreeSourceSlot ()
{
	// this function will return a free number according to the SourceEntries currently in the list.
	// Note : a standard entry start like "[xx] - " where xx is a number
	uint t;
	int iPos, iSourceNumber, iInquireNumber;
	bool bOk;
	QString qsPart;
	iInquireNumber = 1;
	for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	{	//ox
		//iPos = m_listSourceFileEntries[t]->qsDisplayName.find ("]", 1);	//ooo
		iPos = m_listSourceFileEntries[t]->qsDisplayName.indexOf ("]");		//xxx
		if (iPos == -1)
			continue;
		qsPart = m_listSourceFileEntries[t]->qsDisplayName.mid (1, iPos-1);
		if (qsPart.isNull ())
			continue;
		iSourceNumber = qsPart.toInt(&bOk);
		if (!bOk)
			continue;
		if (iSourceNumber == iInquireNumber)	{
			iInquireNumber ++;
			t = 0;
		}
	}
	// And at this point we should have a correct number ...
	return iInquireNumber;
//	return m_listSourceFileEntries.count()+1;
}

void SourceToolBar::addMovie (SourceFileEntry *pEntry)
{
  uint t;
  QColor statusColor (COLOR_MOVIE_FILES_OK);
  QList<QTreeWidgetItem *> items;	//xxx
  QList<QColor> statusColors;		//xxx
  
  SourceFileInfo *pInfo;
  for (t=0;t<(uint)pEntry->listFileInfos.count();t++)	{ 
    pInfo  = pEntry->listFileInfos[t];
    // Here we check that ALL sources are ok
    if (pInfo->qsStatus != QString ("Ok"))
      statusColor = QColor(COLOR_MOVIE_FILES_BAD);

    /*new Q3ListViewItem  ( m_pSourceWidget->m_pListViewVideoSources, pInfo->qsVideoFormat,
                         pInfo->qsResolution, pInfo->qsLength, pInfo->qsSize, pInfo->qsRatio,
                         pInfo->qsFPS, pInfo->qsStatus, pInfo->qsFileName );*/									//oooo
    //(new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources))->setText(3, pInfo->qsSize/*tr("Test")*/);//, pInfo->qsVideoFormat,	//xxx
    //new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources, QStringList(pInfo->qsFileName));						//xxx

                         
    QTreeWidgetItem *video = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources);	//xxx
    video->setText(0, pInfo->qsVideoFormat);	//xxx
    video->setText(1, pInfo->qsResolution);	//xxx
    video->setText(2, pInfo->qsLength);		//xxx
    video->setText(3, pInfo->qsSize);		//xxx
    video->setText(4, pInfo->qsRatio);		//xxx
    video->setText(5, pInfo->qsFPS);		//xxx
    video->setText(6, pInfo->qsStatus);		//xxx
    video->setText(7, pInfo->qsFileName);	//xxx
                         
    //(new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewAllSources))->setText(0, pInfo->qsFileName);	//xxxx
    //items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));	//xxx
    //items.append(new QTreeWidgetItem(m_pSourceWidget->m_pListViewAllSources, QStringList(pInfo->qsFileName)));	//xxxx
    statusColors.append(statusColor);	//xxx
  }

  //new ListViewItemMedia (m_pListViewAllSources, pEntry, statusColor);								//oooo
  //(new ListViewItemMedia (m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor))->setText(0, pEntry->qsDisplayName);	//xxx
  new ListViewItemMedia (m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor);						//xxx
  //m_pSourceWidget->m_pListViewAllSources->insertTopLevelItems(0, items);							//xxx

  /*for (t=0;t<(uint)pEntry->listFileInfos.count();t++)	{
    //m_pSourceWidget->m_pListViewAllSources->itemBelow ( m_pSourceWidget->m_pListViewAllSources->currentItem() )->setBackground(t, QBrush(statusColors[t]) );	//xxx
    m_pSourceWidget->m_pListViewAllSources->itemBelow ( m_pSourceWidget->m_pListViewAllSources->topLevelItem(m_pSourceWidget->m_pListViewAllSources->topLevelItemCount()-1) )->setBackground(t, QBrush(statusColors[t]) );	//xxx
  }*/		//xxxx
}

void SourceToolBar::appendSourceFileEntry ( SourceFileEntry *pEntry, bool bUpdateAllInfo )
{
  // This function is called when loading from an XML-project file ???
  m_listSourceFileEntries.append ( pEntry );
  QColor statusColor (COLOR_MOVIE_FILES_BAD);
  // Here we set the preview size ...
  SourceFileInfo *pInfo;
  pEntry->sizeThumbnail = Global::pApp->getDragNDropContainer()->sizeThumbnail;
  if ( ! pEntry->bSoundSource )  {  // Here we handle a Movie SourceFileEntry ...
    if ( pEntry->bIsSlideshow )  {
      // Here we handle slideshow entries ( vide does not yet exist.
      pInfo = pEntry->listFileInfos[0];
      if ( pInfo && ( ! pInfo->pPreview ) )  {
        QImage     theImage;
        //theImage =  QImage::fromMimeSource ( "create_slideshow.jpg" );	//ooo
	    theImage =  QImage ( ":/images/create_slideshow.jpg" );			//xxx
        //theImage = theImage.smoothScale ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio );				//ooo
	    theImage = theImage.scaled ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
        pInfo->pPreview   = new QImage ( theImage );
      }
      // Okay, now we should also add this into the list box ...
      statusColor = QColor ( COLOR_SLIDE_FILES_OK );
    }
    else  {
      for (uint t=0;t<(uint)pEntry->listFileInfos.count();t++)	{	//ox
        pInfo = pEntry->listFileInfos[t];
        // Add the plaeas - wait image
        //pInfo->pPreview = new QImage (QImage().fromMimeSource( "please_wait.jpg" ));		//ooo
	    pInfo->pPreview = new QImage ( ":/images/please_wait.jpg" );				//xxx
        // and the request the preview from a background thread.
        bool bNeedUpdate = ( pInfo->qsStatus != QString ( "Ok" ) );
        MediaCreator::registerWithMediaScanner ( this, pInfo, pInfo->iMSecPreview, bUpdateAllInfo || bNeedUpdate );
        //*pInfo->pPreview = pInfo->pPreview->smoothScale ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio);				//ooo
	*pInfo->pPreview = pInfo->pPreview->scaled ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);		//xxx
        // And show what we have thus far.
        //new Q3ListViewItem (m_pSourceWidget->m_pListViewVideoSources, pInfo->qsVideoFormat, pInfo->qsResolution, pInfo->qsLength,  pInfo->qsSize, pInfo->qsRatio, pInfo->qsFPS, pInfo->qsStatus, pInfo->qsFileName);	//ooo
	QTreeWidgetItem *video = new QTreeWidgetItem (m_pSourceWidget->m_pListViewVideoSources);	//xxx
        video->setText(0, pInfo->qsVideoFormat);	//xxx
        video->setText(1, pInfo->qsResolution);	//xxx
        video->setText(2, pInfo->qsLength);		//xxx
        video->setText(3, pInfo->qsSize);		//xxx
        video->setText(4, pInfo->qsRatio);		//xxx
        video->setText(5, pInfo->qsFPS);		//xxx
        video->setText(6, pInfo->qsStatus);		//xxx
        video->setText(7, pInfo->qsFileName);	//xxx
      }
    }
  }
  else	{  // And here we have a Sound SourceFileEntry
    statusColor = QColor (COLOR_SOUND_FILES_OK);

    for (uint t=0;t<(uint)pEntry->listFileInfos.count();t++)	{
      SourceFileInfo *pInfo = pEntry->listFileInfos[t];
      if (pInfo->qsStatus != QString ("Ok"))
	statusColor = QColor (COLOR_SOUND_FILES_BAD);

       //pInfo->pPreview = new QImage (QImage().fromMimeSource( "sound_file.jpg" ));	//ooo
      pInfo->pPreview = new QImage ( ":/images/sound_file.jpg" );			//xxx
      //*pInfo->pPreview = pInfo->pPreview->smoothScale ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio);			//ooo
      *pInfo->pPreview = pInfo->pPreview->scaled ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);	//xxx
      //new Q3ListViewItem (m_pSourceWidget->m_pListViewAudioSources, pInfo->qsAudioFormat, pInfo->qsLength, pInfo->qsBits, pInfo->qsSample, pInfo->qsBPS, pInfo->qsSize, pInfo->qsStatus, pInfo->qsFileName);
      QTreeWidgetItem *audio = new QTreeWidgetItem (m_pSourceWidget->m_pListViewAudioSources);	//xxx
      audio->setText(0, pInfo->qsAudioFormat);	//xxx
      audio->setText(1, pInfo->qsLength);	//xxx
      audio->setText(2, pInfo->qsBits);		//xxx
      audio->setText(3, pInfo->qsSample);	//xxx
      audio->setText(4, pInfo->qsBPS);		//xxx
      audio->setText(5, pInfo->qsSize);		//xxx
      audio->setText(6, pInfo->qsStatus);	//xxx
      audio->setText(7, pInfo->qsFileName);	//xxx
    }
  }
  //new ListViewItemMedia (m_pListViewAllSources, pEntry, statusColor);			//ooo
  new ListViewItemMedia (m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor);	//xxx
}

bool SourceToolBar::setRenderProgress ( SourceFileEntry *pEntry, float fProgress )
{  
  // fProgress == -1.0 == reset
  ListViewItemMedia *pItem = getItem ( pEntry );
  if ( ! pItem )
    return false;

  pItem->setProgress ( fProgress );

  return true;
}

ListViewItemMedia *SourceToolBar::getItem ( SourceFileEntry *pEntry )
{ 
  ListViewItemMedia *pItem;
  // Okay found the SourceFileInfo now we need the ListViewItemMedia - object for this Info.
  //pItem = (ListViewItemMedia *)m_pListViewAllSources->firstChild ( ); //ooo
  //pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem(0)->child(0);  //xxx-!
  pItem = (ListViewItemMedia *)m_pSourceWidget->m_pListViewAllSources->topLevelItem(0);  //xxx

  while ( pItem )  {    
    if  ( pItem->sourceFileEntry ( ) == pEntry )
      return pItem;
    //pItem = (ListViewItemMedia *)pItem->nextSibling ( );              //ooo
    pItem = (ListViewItemMedia *)pItem->treeWidget()->itemBelow(pItem);	//xxx
  }

  return NULL;
}

void SourceToolBar::slotAddSound ( )
{
  // never used, the real function comes from ...
  // QDVDMenu::slotAddSound()->DVDAuthor::addSound()->SourceToolBar::addSound()
  int t;
  QString qsFilter, qsMovieFilter, qsExtension, qsFileName;
  QFileInfo fileInfo;

  // For now we assume this is slotEditAdd()
  QStringList listAddAudio, listAddMovies;

  qsMovieFilter = Global::pApp->getMovieFilter ( );
  qsFilter      = Global::pApp->getSoundFilter ( );
  //listAddAudio  = Q3FileDialog::getOpenFileNames(tr("Sound Files (")+qsFilter+QString(")"), Global::qsCurrentPath, this, tr("Open file dialog"), tr("Add To Project ..."));   //ooo
  listAddAudio  = QFileDialog::getOpenFileNames(this, tr("Open file dialog"), Global::qsCurrentPath, tr("Sound Files (")+qsFilter+QString(")"));    //xxx
  if ( listAddAudio.count ( ) < 1 )
    return;
  // Here we will check to see if the user selected audio files ...
  for ( t=0; t<(int)listAddAudio.count ( ); t++ ) {
    qsFileName = listAddAudio[t];
    fileInfo.setFile ( qsFileName );
    //qsExtension = fileInfo.extension ( FALSE ).lower ( ); //ooo
    qsExtension = fileInfo.suffix ( ).toLower ( );      //xxx
    //if ( qsMovieFilter.find ( qsExtension ) > -1 ) {  //ooo
    if ( qsMovieFilter.indexOf ( qsExtension ) > -1 ) { //xxx
      listAddMovies.append  ( qsFileName  );
      //listAddAudio .remove  ( qsFileName  );  //ooo
      listAddAudio .removeOne  ( qsFileName  ); //xxx
    }
  }

  // Okay, now we should also add this into the list box ...
  addSound     ( listAddAudio  );
  addMovieList ( listAddMovies );
}

void SourceToolBar::addSound ( QStringList &listSoundNames )
{  
  if ( listSoundNames.count ( ) < 1 )
    return;
	uint t;
	QFileInfo fileInfo ( listSoundNames[0] );

	// And here we set the  current Directory for the application ...
	//QString qsPath = fileInfo.dirPath ( TRUE );	//ooo
	QString qsPath = fileInfo.path ( );		//xxx
	Global::qsCurrentPath = qsPath;

	// Before we insert it we should remove all previous entries so that we hold only one entry for one sound file
	//	removeEntry (qsFileName);

	MediaInfo *pMediaInfo   = MediaCreator::createInfo (getMediaEngine());
	QColor statusColor (COLOR_SOUND_FILES_OK);
	SourceFileInfo  *pInfo  = NULL;
	SourceFileEntry *pEntry = new SourceFileEntry;
	pEntry->bSoundSource    = true;
	pEntry->arrayAudioEntries[0].iAudioNumber = -1; // Language settings etc is handled on the menu level for Audio tracks
	pEntry->sizeThumbnail   = Global::pApp->getDragNDropContainer()->sizeThumbnail;
	for (t=0;t<(uint)listSoundNames.count();t++)	{	//ox
		pMediaInfo->setFileName (listSoundNames[t]);
		// Lets create a new SoourceFileEntry for this sound file ...
		pInfo = new SourceFileInfo;

		pInfo->qsSize        = pMediaInfo->getSizeString();
		pInfo->qsBits        = QString ("%1").arg(pMediaInfo->getBits());
		pInfo->qsSample      = pMediaInfo->getSampleString ();
		pInfo->qsBPS         = pMediaInfo->getBPSString (false);
		pInfo->qsLength      = pMediaInfo->getLengthString ();
		pInfo->qsAudioFormat = pMediaInfo->getFormat (false);
		pInfo->qsStatus      = pMediaInfo->getStatus();
		pInfo->qsAudioCodec  = pMediaInfo->getCodec(false);
		pInfo->qsFileName    = listSoundNames[t];
		//pInfo->pPreview      = new QImage (QImage().fromMimeSource( "sound_file.jpg" ));	//ooo
		pInfo->pPreview      = new QImage ( ":/images/sound_file.jpg" );			//xxx
		//*pInfo->pPreview     = pInfo->pPreview->smoothScale ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio);			//ooo
		*pInfo->pPreview     = pInfo->pPreview->scaled ( Global::pApp->getDragNDropContainer()->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation);	//xxx

		if (pInfo->qsStatus != QString ("Ok"))
		  statusColor = QColor ( COLOR_SOUND_FILES_BAD );
		if ( ! checkRightAudioFormat ( pInfo ) ) {
		  // Enable automagically the transcoding interface
		  setAudioTranscode    ( pInfo );
		  statusColor = QColor ( COLOR_AUDIO_TRANSCODE );
		}

		pEntry->listFileInfos.append (pInfo);
	}

	if ( listSoundNames.count ( ) > 1 )
		pEntry->qsDisplayName.sprintf ("[%02d] - <%02d Sound Files>", getFreeSourceSlot ( ), (int)listSoundNames.count ( ) );
	else
		//pEntry->qsDisplayName.sprintf ("[%02d] - %s",getFreeSourceSlot (), (const char *)fileInfo.fileName());	//ooo
		pEntry->qsDisplayName.sprintf ("[%02d] - %s",getFreeSourceSlot (), fileInfo.fileName().toLatin1().data());	//xxx
	m_listSourceFileEntries.append(pEntry);

	// this new sound is assigned to the current DVDMenu's Preview. Phew what a long list to get there ...
	DVDMenu *pCurrentSubMenu = Global::pApp->getCurrentSubMenu ( );
	if ( pCurrentSubMenu )
	     pCurrentSubMenu->getMenuPreview ( )->undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_AUDIO, pEntry ) ); // And finally we add it to the list ...
   
	//new ListViewItemMedia (m_pListViewAllSources, pEntry, statusColor);                  //ooo
    new ListViewItemMedia (m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor);    //xxx

	// And further on to the m_pListViewVideoSources ...
	if ( pInfo ) {
		//new Q3ListViewItem ( m_pSourceWidget->m_pListViewAudioSources, pInfo->qsAudioFormat, pInfo->qsLength, pInfo->qsBits, pInfo->qsSample, pInfo->qsBPS, pInfo->qsSize,  pInfo->qsStatus, pInfo->qsFileName );	//ooo
		QTreeWidgetItem *audio = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewAudioSources);	//xxx
		audio->setText(0, pInfo->qsAudioFormat);	//xxx
		audio->setText(1, pInfo->qsLength);		//xxx
		audio->setText(2, pInfo->qsBits);		//xxx
		audio->setText(3, pInfo->qsSample);		//xxx
		audio->setText(4, pInfo->qsBPS);		//xxx
		audio->setText(5, pInfo->qsSize);		//xxx
		audio->setText(6, pInfo->qsStatus);		//xxx
		audio->setText(7, pInfo->qsFileName);		//xxx	
	}
	delete pMediaInfo;
}

void SourceToolBar::slotAddSlideshow ( )
{
  Utils theUtil;
  if ( ! Global::pApp->checkForNewProject ( ) )
    return;

  // First we pop up the slideshow generation dialog box
  Input::DialogImages fileDialog ( this );
  fileDialog.initMe    ( );   //oooo
  if ( fileDialog.exec ( ) == QDialog::Rejected )
    return;

  CXmlSlideshow   *pXmlSlideshow;
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;
  QList<CXmlSlideshow *>newList = fileDialog.getSlideshows ( ); //oxx
  QList<CXmlSlideshow *>::iterator it = newList.begin ( );      //oxx

  while ( it != newList.end  ( ) )  {
    pXmlSlideshow = *it++;
    pEntry = sourceFileEntry ( pXmlSlideshow );
    pInfo  = pEntry ? pEntry->listFileInfos[0] : NULL;
    if ( pXmlSlideshow->count ( ) < 1 )  {
      // No more images in the slideshow
//      delete pXmlSlideshow; done in destructor of GroupView
      if ( pEntry && pInfo )  {
           if ( pInfo->pSlideshow )
                delete pInfo->pSlideshow;
           pInfo->pSlideshow    = NULL;
           pEntry->bIsSlideshow = false;
      }
      // Also if the slideshow has not finished rendering, Then get rid of the Entry all together
      if ( pEntry && pEntry->bIsSlideshow )  {
           removeEntry ( pEntry, false );
           delete pEntry;
      }
      continue;
    }

    // Re-render the slideshow if different
    if ( pEntry && pInfo )  {
      CXmlSlideshow *pSlideshow = pInfo->pSlideshow;
      if ( ! ( *pSlideshow == *pXmlSlideshow ) )  {
         Render::Manager::unregisterEntry ( pEntry );

         // In case the Slideshow name has changed. We need to also change the file name.
         QString qsFileName = pXmlSlideshow->slideshow_name;
         qsFileName.replace ( "/", "_" );
         pInfo->qsFileName    = theUtil.getTempFile ( qsFileName + ".vob" );
         pEntry->bIsSlideshow = true;

        *pSlideshow = *pXmlSlideshow;
         updateSlideshowItem ( pEntry );
//pSlideshow->writeXml ( );
         //pSlideshow->writeXml ( );  //xxx
         Render::Manager::registerEntry ( pEntry );
      }
      continue;
    } //ooo
 
    entryFromSlideshow ( pXmlSlideshow );
  }
}

void SourceToolBar::updateSlideshowItem ( SourceFileEntry *pEntry )
{ 
  if ( ! pEntry )
    return;

  QString qsInfo;
  Utils theUtils;
  QFileInfo fileInfo;
  unsigned int t, iLength;
  bool bPal = false;
  CXmlSlideshow::vid_struct *pXmlVid;
  ListViewItemMedia *pItem = getItem ( pEntry );
  SourceFileInfo    *pInfo = pEntry->listFileInfos[0];
  CXmlSlideshow     *pXmlSlideshow = pInfo ? pInfo->pSlideshow : NULL;

  if ( ( ! pXmlSlideshow ) || ( ! pItem ) )
    return;

  // First we parse the current Entry name and adjust it if required.
  //if ( pEntry->qsDisplayName.find ( " - " ) > 0 )  {  //ooo
  if ( pEntry->qsDisplayName.indexOf ( " - " ) > 0 )  { //xxx
    //QString qsTemp = pEntry->qsDisplayName.left ( pEntry->qsDisplayName.find ( " - " ) + 3 );     //ooo
    QString qsTemp = pEntry->qsDisplayName.left ( pEntry->qsDisplayName.indexOf ( " - " ) + 3 );    //xxx
    //pEntry->qsDisplayName = qsTemp + pXmlSlideshow->slideshow_name.ascii ( );             //ooo
    pEntry->qsDisplayName = qsTemp + pXmlSlideshow->slideshow_name.toLatin1 ( ).data ( );   //xxx
  }
  else
    //pEntry->qsDisplayName.sprintf ( "[%02d] - %s", getFreeSourceSlot ( ), pXmlSlideshow->slideshow_name.ascii ( ) );              //ooo
    pEntry->qsDisplayName.sprintf ( "[%02d] - %s", getFreeSourceSlot ( ), pXmlSlideshow->slideshow_name.toLatin1 ( ).data ( ) );    //xxx

  // Next we determine if we have PAL or NTSC
  //if ( pInfo->qsVideoFormat.find ( "pal", 0, FALSE ) > -1 )   //ooo
  if ( pInfo->qsVideoFormat.indexOf ( "pal" ) > -1 )  //xxx
       bPal = true;

  iLength = 1000 * pXmlSlideshow->slideLength ( );// (unsigned int) ( 1000 * ( pXmlSlideshow->countImg ( ) * pXmlSlideshow->filter_delay + (pXmlSlideshow->countImg ( )+1) * pXmlSlideshow->delay ) );
  for ( t=0; t<pXmlSlideshow->countVid ( ); t++ )  {
    pXmlVid  = pXmlSlideshow->getVid  ( t );
    if ( pXmlVid )
         iLength += (unsigned int)( pXmlVid->length + 1000 * pXmlSlideshow->filter_delay );
  }
  pInfo->qsLength = theUtils.getStringFromMs ( iLength );

  fileInfo.setFile ( pInfo->qsFileName );
  qsInfo = QString ( fileInfo.fileName ( ) + QString ( "\n" ) + pInfo->qsLength );

  pItem->setText ( 0, bPal ? "pal" : "ntsc" );
  pItem->setText ( 1, pEntry->qsDisplayName );

  //pItem = (ListViewItemMedia *)pItem->firstChild ( ); //ooo
  pItem = (ListViewItemMedia *)pItem->child ( 0 );      //xxx
  if ( pItem )
       pItem->setText ( 1, qsInfo );
}

SourceFileEntry *SourceToolBar::entryFromSlideshow ( CXmlSlideshow *pXmlSlideshow )
{  
  QString qsFileName;
  Utils   theUtils;
  CXmlSlideshow::vid_struct *pXmlVid;
//  CXmlSlideshow::img_struct *pXmlImage;
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;
  unsigned int t, iLength = 0;

//  pXmlImage = pXmlSlideshow->getImg ( 0 );

  pEntry = new SourceFileEntry;
  //pEntry->qsDisplayName.sprintf ( "[%02d] - %s", getFreeSourceSlot ( ), pXmlSlideshow->slideshow_name.ascii ( ) );            //ooo
  pEntry->qsDisplayName.sprintf ( "[%02d] - %s", getFreeSourceSlot ( ), pXmlSlideshow->slideshow_name.toLatin1 ( ).data ( ) );  //xxx
  pEntry->sizeThumbnail = Global::pApp->getDragNDropContainer()->sizeThumbnail;
  pEntry->bIsSlideshow  = true;
  // Next we create the Info. Note, only one info per entry ( only one Slideshow )
  QImage     theImage;
  //theImage = QImage::fromMimeSource ( "create_slideshow.jpg" );	//ooo
  theImage = QImage ( ":/images/create_slideshow.jpg" );		//xxx
  //theImage = theImage.smoothScale   ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio );			//ooo
  theImage = theImage.scaled ( Global::pApp->getDragNDropContainer ( )->sizeButton, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx

  bool bPAL = ( pXmlSlideshow->yres == 576 );
  pInfo = new SourceFileInfo;
  qsFileName = pXmlSlideshow->slideshow_name;

  qsFileName.replace ( "/", "_" );

  pInfo->qsFileName    = theUtils.getTempFile ( qsFileName + ".vob" );

  pInfo->pSlideshow    = new CXmlSlideshow;
 *pInfo->pSlideshow    = *pXmlSlideshow;

  pInfo->qsVideoFormat = bPAL ? "PAL" : "NTSC";
  pInfo->qsFPS         = bPAL ?  "25" : "29.97";
  pInfo->qsResolution  = QString ( "%1x%2" ).arg ( pXmlSlideshow->xres ).arg ( pXmlSlideshow->yres );
  iLength              = (unsigned int)( 1000 * ( pXmlSlideshow->countImg ( ) * pXmlSlideshow->filter_delay + (pXmlSlideshow->countImg ( )+1) * pXmlSlideshow->delay ) );
  for ( t=0; t<pXmlSlideshow->countVid ( ); t++ )  {
    pXmlVid =  pXmlSlideshow->getVid  ( t );
    if ( pXmlVid )
         iLength      += (unsigned int)( pXmlVid->length + 1000 * pXmlSlideshow->filter_delay );
  }

  pInfo->qsLength      = theUtils.getStringFromMs ( iLength );
  pInfo->qsRatio       = "4:3";

  pInfo->pPreview   = new QImage ( theImage );
  pEntry->listFileInfos.append   (   pInfo  );
  m_listSourceFileEntries.append (  pEntry  );

  // Okay, now we should also add this into the list box ...
  //new ListViewItemMedia ( m_pListViewAllSources, pEntry, QColor ( COLOR_SLIDE_FILES_OK ) );   //ooo
  new ListViewItemMedia ( m_pSourceWidget->m_pListViewAllSources, pEntry, QColor ( COLOR_SLIDE_FILES_OK ) );    //xxx

  // And finally we should register the slideshow for rendering in the background.
  Render::Manager::registerEntry ( pEntry );

  setRenderProgress ( pEntry, 0.0f ); //oooo

  return pEntry;
}

void SourceToolBar::slideshowDone ( SourceFileEntry *pEntry )
{  
  if ( ! pEntry )
    return;

  ListViewItemMedia *pItem = getItem ( pEntry );
  if ( ! pItem )
    return;

  if ( pEntry->listFileInfos.count ( ) < 1 )
    return;

  SourceFileInfo *pInfo = pEntry->listFileInfos[0];
  // First we determine the fileName of the generated mpeg video ...
  pEntry->bIsSlideshow = false;	// now we hold a mpeg and not a xml - file ...
  pEntry->bSoundSource = false;

  delete pItem;
  //new ListViewItemMedia ( m_pListViewAllSources, pEntry, QColor ( COLOR_MOVIE_FILES_OK ) );  //ooo
  new ListViewItemMedia ( m_pSourceWidget->m_pListViewAllSources, pEntry, QColor ( COLOR_MOVIE_FILES_OK ) );    //xxx
  long iOffset = 0;
  if ( pInfo->pSlideshow ) // get us smack in the middle of the first image
       iOffset = (long)( 1000 * (long)(1.5*pInfo->pSlideshow->delay) + pInfo->pSlideshow->filter_delay );
  pInfo->iMSecPreview = iOffset;

  if ( ! updateListViewItem ( pInfo ) )  {
    /*new Q3ListViewItem ( m_pSourceWidget->m_pListViewVideoSources, pInfo->qsVideoFormat,
          pInfo->qsResolution, pInfo->qsLength, pInfo->qsSize, pInfo->qsRatio,
          pInfo->qsFPS, pInfo->qsStatus, pInfo->qsFileName );*/ //oooo
    
    QTreeWidgetItem *video = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources);	//xxx
    video->setText(0, pInfo->qsVideoFormat);	//xxx
    video->setText(1, pInfo->qsResolution);	//xxx
    video->setText(2, pInfo->qsLength);		//xxx
    video->setText(3, pInfo->qsSize);		//xxx
    video->setText(4, pInfo->qsRatio);		//xxx
    video->setText(5, pInfo->qsFPS);		//xxx
    video->setText(6, pInfo->qsStatus);		//xxx
    video->setText(7, pInfo->qsFileName);	//xxx
  }
  MediaCreator::registerWithMediaScanner ( this, pInfo, iOffset, true );

  Global::pApp->slotAutosave ( );
}

void SourceToolBar::slotEditMovie ( )
{
  // Here we open the MovieDialog.
  SourceFileInfo *pInfo = getCurrentSourceFileInfo ( );

  m_pContexMenuEntry    = NULL;
  if ( pInfo )
    editSourceFileInfo ( pInfo );
}

void SourceToolBar::editSourceFileInfo ( SourceFileInfo  *pSourceFileInfo )
{
  if ( ! pSourceFileInfo )
    return;

  DialogMovie *pDialog = new DialogMovie ( this );

  pDialog->initMe ( pSourceFileInfo, getMediaEngine ( ), Global::pApp );

  connect ( pDialog, SIGNAL ( signalSetThumbnail ( long ) ), this, SLOT ( slotSetThumbnail ( long ) ) );
  if ( pDialog->exec ( ) == QDialog::Accepted )  {
    *pSourceFileInfo  = *pDialog->sourceFileInfo ( );
    // next is to update the entries if neccesary ...
    updateListViewItem ( pSourceFileInfo );
    Global::pApp->slotCalculateSize ( );
  }
  disconnect ( pDialog, SIGNAL ( signalSetThumbnail ( long ) ), this, SLOT ( slotSetThumbnail ( long ) ) );

  //delete pDialog;	//oooo
}

void SourceToolBar::slotRerenderSlide ( )
{
  SourceFileInfo  *pInfo  = getCurrentSourceFileInfo  ( );
  SourceFileEntry *pEntry = getCurrentSourceFileEntry ( );
  // Re-render the slideshow if different
  if ( pEntry && pInfo )  {
    Render::Manager::unregisterEntry  ( pEntry );
    pEntry->bIsSlideshow = true;
    Render::Manager::registerEntry    ( pEntry );
  }
}

void SourceToolBar::slotEditAudio ( )
{
  SourceFileEntry *pEntry = getCurrentSourceFileEntry ( );
  SmallDialogAudio theDialog;
  // Note we deliver both the local copy as the original.
  theDialog.initMe ( pEntry, false );
  if ( theDialog.exec ( ) == QDialog::Rejected )
    return;

  //updateStatusColor ( m_pListViewAllSources->currentItem ( ) );   //oooo
}

void SourceToolBar::refreshToolBarEntries()
{
  uint t, i;
  QColor statusColor;
  SourceFileEntry *pEntry;
  SourceFileInfo *pInfo;
  // This function will remove all entries from all listBox objects and re-create all information from
  // The current m_listSourceFileEntries.
  //m_pListViewAllSources  ->clear ();				//ooo
  m_pSourceWidget->m_pListViewAllSources  ->clear ();		//xxx
  m_pSourceWidget->m_pListViewVideoSources->clear ( );
  m_pSourceWidget->m_pListViewAudioSources->clear ( );
  for (t=0;t<(uint)m_listSourceFileEntries.count();t++)   {
    pEntry = m_listSourceFileEntries[t];
    // Okay here we decide if we have a Movie or sound stream ...
    if (!pEntry->bSoundSource)	{
      statusColor = QColor(COLOR_MOVIE_FILES_OK);
      for (i=0;i<(uint)pEntry->listFileInfos.count();i++)	{	//ox
        pInfo = pEntry->listFileInfos[i];
        // Here we check that ALL sources are ok
        if ( pInfo->qsStatus != QString ( "Ok" ) )
          statusColor = QColor ( COLOR_MOVIE_FILES_BAD );

        /*new Q3ListViewItem  ( m_pSourceWidget->m_pListViewVideoSources, pInfo->qsVideoFormat,
                             pInfo->qsResolution, pInfo->qsLength, pInfo->qsSize, pInfo->qsRatio,
                             pInfo->qsFPS, pInfo->qsStatus, pInfo->qsFileName );*/		//oooo
                             
        QTreeWidgetItem *video = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewVideoSources);	//xxx
        video->setText(0, pInfo->qsVideoFormat);	//xxx
        video->setText(1, pInfo->qsResolution);	//xxx
        video->setText(2, pInfo->qsLength);		//xxx
        video->setText(3, pInfo->qsSize);		//xxx
        video->setText(4, pInfo->qsRatio);		//xxx
        video->setText(5, pInfo->qsFPS);		//xxx
        video->setText(6, pInfo->qsStatus);		//xxx
        video->setText(7, pInfo->qsFileName);	//xxx
      }
    }
    else 	{
      statusColor = QColor (COLOR_SOUND_FILES_OK);
      for (i=0;i<(uint)pEntry->listFileInfos.count();i++)	{	//ox
	pInfo = pEntry->listFileInfos[i];
	if (pInfo->qsStatus != QString ("Ok"))
	  statusColor = QColor (COLOR_SOUND_FILES_BAD);
	
	/*new Q3ListViewItem (m_pSourceWidget->m_pListViewAudioSources, pInfo->qsAudioFormat,
			   pInfo->qsLength, pInfo->qsBits, pInfo->qsSample, pInfo->qsBPS,
			   pInfo->qsSize,  pInfo->qsStatus, pInfo->qsFileName);*/		//oooo
		QTreeWidgetItem *audio = new QTreeWidgetItem  ( m_pSourceWidget->m_pListViewAudioSources);	//xxx
		audio->setText(0, pInfo->qsAudioFormat);	//xxx
		audio->setText(1, pInfo->qsLength);		//xxx
		audio->setText(2, pInfo->qsBits);		//xxx
		audio->setText(3, pInfo->qsSample);		//xxx
		audio->setText(4, pInfo->qsBPS);		//xxx
		audio->setText(5, pInfo->qsSize);		//xxx
		audio->setText(6, pInfo->qsStatus);		//xxx
		audio->setText(7, pInfo->qsFileName);		//xxx	
      }
    }
    //new ListViewItemMedia (m_pListViewAllSources, pEntry, statusColor);			//ooo
    new ListViewItemMedia (m_pSourceWidget->m_pListViewAllSources, pEntry, statusColor);	//xxx
  }
}

void SourceToolBar::slotDeleteSource ( )
{
  // Called when the user right clicks in the SourceFileEntry - list
  m_pContexMenuInfo = NULL;
  if ( m_pContexMenuEntry )  {
    SourceFileEntry *pEntry = m_pContexMenuEntry;
    m_pContexMenuEntry = NULL;
    // Okay we should politely check if this source is in use by a button.
    if ( Global::pApp->isSourceEntryUsed ( pEntry->qsDisplayName ) )  {
      if ( MessageBox::warning ( NULL, tr ("Warning, This source is in use."),
        tr ("This Source is in use.\nDo you realy want to delete this source ?\n"),
        QMessageBox::Yes, QMessageBox::No) == QMessageBox::No )
            return;
    }
    removeEntry ( pEntry, false );
    // At this point we can delete the pSourceFileEntry ...
    delete pEntry;
    return;
  }

  SourceFileEntry *pSourceFileEntry = getCurrentSourceFileEntry ( );
  if ( ! pSourceFileEntry )
    return;

  //Q3ListViewItem *pItem = m_pListViewAllSources->currentItem ( );			//ooo
  QTreeWidgetItem *pItem = m_pSourceWidget->m_pListViewAllSources->currentItem ( );	//xxx
  if ( ! pItem )
    return;

  QString qsCurrentText = pItem->text(SOURCE_FILE_ENTRY_NAME);
  // Okay now I added the total play time also to the filename, we should 
  // disregard this from here ... E.g. "Title.mpg\n00:13:01"
  //int iPos = qsCurrentText.find ("\n");		//ooo
  int iPos = qsCurrentText.indexOf ("\n");	//xxx
  qsCurrentText = qsCurrentText.left (iPos);

  if ( pSourceFileEntry->listFileInfos.count ( ) == 1 )
     qsCurrentText = pSourceFileEntry->qsDisplayName;

  if ( ! pSourceFileEntry->bSoundSource )  {
    // Okay we should politely check if this source is in use by a button.
    if ( Global::pApp->isSourceEntryUsed ( qsCurrentText ) )  {
      if (MessageBox::warning (NULL, tr ("Warning, This source is in use."),
        tr ("This Source is in use.\nDo you realy want to delete this source ?\n"), 
        QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
          return;
    }
  }

  removeEntry ( qsCurrentText, false );
}

void SourceToolBar::removeEntry ( QString &qsEntry, bool bSourceOnly )
{
  uint t;
  QFileInfo fileInfo;
  SourceFileInfo  *pSourceFileInfo  = NULL;
  SourceFileEntry *pSourceFileEntry = getCurrentSourceFileEntry ( );
  if ( ! pSourceFileEntry )
    return;

  // First we check if the user wants to remove a whole SourceFileEntry
  if (pSourceFileEntry->qsDisplayName == qsEntry)	{
    // Here we remove the full entry from the m_listSourceFileEntris - list.
    removeEntry (pSourceFileEntry, bSourceOnly);
    // At this point we can delete the pSourceFileEntry ...
    delete pSourceFileEntry;
  }
  else {	// Okay here we check for the sourecFileInfo's ...
    for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{
      pSourceFileInfo = pSourceFileEntry->listFileInfos[t];
      fileInfo.setFile (pSourceFileInfo->qsFileName);
      if (fileInfo.fileName() == qsEntry)	{
        //pSourceFileEntry->listFileInfos.remove   ( pSourceFileInfo );		//ooo
        pSourceFileEntry->listFileInfos.removeOne   ( pSourceFileInfo );	//xxx
        MediaCreator::unregisterFromMediaScanner ( pSourceFileInfo );
        delete pSourceFileInfo;
        refreshToolBarEntries ( );						//oooo
        // Check if we have just removed the last source from this entry.
        if (pSourceFileEntry->listFileInfos.count () < 1)	{
            // in which case we want to alos delete the header ...
            removeEntry (pSourceFileEntry, bSourceOnly);
            delete pSourceFileEntry;
            return;
        }
      }
    }
  }
}

void SourceToolBar::removeEntry (SourceFileEntry *pSourceFileEntry, bool bSourceOnly)
{
  // This function is also called from UndoObjects.
  uint t;
  int indexOfTopLevelItem;	//xxx
  QStringList listFileNames;
  if ( ! pSourceFileEntry )
    return;
  //m_listSourceFileEntries.remove (pSourceFileEntry);	//ooo
  m_listSourceFileEntries.removeOne (pSourceFileEntry);	//xxx

  /*if ( pSourceFileEntry->bIsSlideshow )
    Render::Manager::unregisterEntry ( pSourceFileEntry );*/	//oooo

  // The first thing we should ensure is that we are not registered with the MediaScanner, which could crash QDVDAuthor
  for ( t=0; t<(uint)pSourceFileEntry->listFileInfos.count ( ); t++ )
    MediaCreator::unregisterFromMediaScanner ( pSourceFileEntry->listFileInfos[t] );

  // Next we remove the entry from the AllSources - Tab
  //QList<Q3ListViewItem *> listDelItems;			//ooo
  QList<QListWidgetItem *> listDelItems;			//xxx
  //Q3ListViewItem *pItem = m_pListViewAllSources->firstChild();					//ooo
  QTreeWidgetItem *pItem = m_pSourceWidget->m_pListViewAllSources->currentItem();			//xxx
  //QTreeWidgetItem *pItem = m_pSourceWidget->m_pListViewAllSources->currentItem()->child(0);			//xxx
  
  //if current item is child we remove parent	//xxx
  if (pItem->childCount() == 0) {			//xxx
    indexOfTopLevelItem = m_pSourceWidget->m_pListViewAllSources->indexOfTopLevelItem(pItem->parent());
    m_pSourceWidget->m_pListViewAllSources->takeTopLevelItem(m_pSourceWidget->m_pListViewAllSources->indexOfTopLevelItem(pItem->parent()));
  }

  //for (t=0;t<(uint)m_pListViewAllSources->childCount();t++)	{				//ooo
  //for (t=0;t<(uint)m_pSourceWidget->m_pListViewAllSources->topLevelItemCount();t++)	{	//xxx

    //current item is parent so we remove it	//xxx
    if (pItem->text(SOURCE_FILE_ENTRY_NAME) == pSourceFileEntry->qsDisplayName)	{	//ooo
    //if (pItem->parent()->text(SOURCE_FILE_ENTRY_NAME) == pSourceFileEntry->qsDisplayName)	{
      // Here we remove this entry from m_pListViewAllSources - widget.
      //listDelItems.append (pItem);		//ooo
      //listDelItems.append (pItem);		//xxx
      //delete pItem->parent()->takeChild(pItem->parent()->indexOfChild(pItem));	//xxx
      //pItem->parent()->takeChild(pItem->parent()->indexOfChild(pItem));			//xxx
      //pItem->parent()->takeChild(0);			//xxx
      //pItem->takeChild(0);			//xxx
      
      indexOfTopLevelItem = m_pSourceWidget->m_pListViewAllSources->indexOfTopLevelItem(pItem);	//xxx
      m_pSourceWidget->m_pListViewAllSources->takeTopLevelItem(m_pSourceWidget->m_pListViewAllSources->indexOfTopLevelItem(pItem));	//xxx
      //m_pSourceWidget->m_pListViewAllSources->takeTopLevelItem(m_pSourceWidget->m_pListViewAllSources->indexOfTopLevelItem(pItem->parent()));	//xxx
    }
    //pItem = pItem->nextSibling();					//ooo
    //pItem = m_pSourceWidget->m_pListViewAllSources->itemBelow(m_pSourceWidget->m_pListViewAllSources->currentItem());	//xxx
  //}		//oooo
  for (t=0;t<(uint)listDelItems.count();t++)
    delete listDelItems[t];
  listDelItems.clear();

  // we create here a temp list of all file names ...
  for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)
    listFileNames.append(pSourceFileEntry->listFileInfos[t]->qsFileName);

  //pItem = m_pSourceWidget->m_pListViewVideoSources->firstChild();	//ooo
  //pItem = m_pSourceWidget->m_pListViewVideoSources->currentItem();	//xxx
  /*pItem = m_pSourceWidget->m_pListViewVideoSources->topLevelItem(0);	//xxx
  int nextIndex = 1;							//xxx
  while (pItem)	{
    
    // Here we remove this entry from m_pListViewVideos - widget.
    for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{
      
      if (pItem->text(MOVIE_FILE_NAME_INDEX) == listFileNames[t])	{
        //listDelItems.append(pItem);	//ooo
        //delete pItem->parent()->takeChild(pItem->parent()->indexOfChild(pItem));	//xxx
	m_pSourceWidget->m_pListViewVideoSources->takeTopLevelItem(m_pSourceWidget->m_pListViewVideoSources->indexOfTopLevelItem(pItem));	//xxx
        // FIXME: If we use SourceFileEntry->qsDisplay name rather then the single filename, we are better off here
        //        Since we remove only the associated files and leave the others untouched.
        // Okay if the user has the same file multiple times in there we only want to remove the entry once.
        listFileNames[t] = QString ("");
      }
    }
    //pItem = pItem->nextSibling();						//ooo
    pItem = m_pSourceWidget->m_pListViewVideoSources->topLevelItem(nextIndex);	//xxx
    nextIndex++;								//xxx
  }*/		//oooo
  
  // Here we remove this entry from m_pListViewVideos - widget.				//xxx
  m_pSourceWidget->m_pListViewVideoSources->takeTopLevelItem(indexOfTopLevelItem);	//xxx
  
  // And here we delete the items
  for (t=0;t<(uint)listDelItems.count();t++)
    delete listDelItems[t];
  listDelItems.clear();

  //pItem = m_pSourceWidget->m_pListViewAudioSources->firstChild();	//ooo
  //pItem = m_pSourceWidget->m_pListViewAudioSources->currentItem();	//xxx
  /*pItem = m_pSourceWidget->m_pListViewAudioSources->topLevelItem(0);	//xxx
  nextIndex = 1;							//xxx
  while (pItem)	{
    // Here we remove this entry from m_pListViewAudio - widget.
    for (t=0;t<(uint)pSourceFileEntry->listFileInfos.count();t++)	{
      if (pItem->text(AUDIO_FILE_NAME_INDEX) == listFileNames[t])	{
        //listDelItems.append(pItem);		//ooo
        //delete pItem->parent()->takeChild(pItem->parent()->indexOfChild(pItem));	//xxx
	m_pSourceWidget->m_pListViewAudioSources->takeTopLevelItem(m_pSourceWidget->m_pListViewAudioSources->indexOfTopLevelItem(pItem));	//xxx
        // Okay if the user has the same file multiple times in there we only want to remove the entry once.
        listFileNames[t] = QString ("");
      }
    }
    //pItem = pItem->nextSibling();						//ooo
    pItem = m_pSourceWidget->m_pListViewAudioSources->topLevelItem(nextIndex);	//xxx
    nextIndex++;								//xxx
  }*/	//oooo
  
  // Here we remove this entry from m_pListViewAudio - widget.				//xxx
  m_pSourceWidget->m_pListViewAudioSources->takeTopLevelItem(indexOfTopLevelItem);	//xxx
  
  for (t=0;t<(uint)listDelItems.count();t++)
    delete listDelItems[t];
  // If we should only remove the entry in the SoureToolBar and leave the ones in QDVDAuthor as is
  // This is the case when we create a new entry. The first thing done is to remove all
  // double entries but we want to leave the entries in the Menus intact.

  if ( ! bSourceOnly )
    // So the user has decided to delete this source, then we should change the associated
    // buttons - action !!!
    Global::pApp->removedSourceEntry ( pSourceFileEntry );	//oooo

  // and finally we should check the new size
  Global::pApp->slotCalculateSize ( );
}

void SourceToolBar::slotPlayMovie ( )
{
  // Called when the user right clicks in the SourceFileEntry - list
  uint i;
  QString qsCommand = QString ( "%1 " ).arg ( Global::qsExternalPlayer );
  SourceFileEntry *pEntry = getCurrentSourceFileEntry ( );
  SourceFileInfo  *pInfo  = getCurrentSourceFileInfo  ( );

  if ( pEntry )  {
    //for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ )	//ox
    for ( i=0; i<static_cast<uint> ( pEntry->listFileInfos.count ( ) ); i++ )	//xxx
      qsCommand += QString ("\"%1\" ").arg ( pEntry->listFileInfos[i]->qsFileName );
  }
  else	{
    if ( ! pInfo )  // This should never happen but to make sure ...
      return;
    qsCommand += QString ( "\"%1\" " ).arg ( pInfo->qsFileName );
  }
  
  // Here we execute the external QXine video viewer application ...
  qsCommand += QString (" &");    //oooo
  //if ( system ( (const char *)qsCommand ) == -1 )				//ooo
  //if ( system ( const_cast<char *>( qsCommand.toLatin1().data()) ) == -1 )	//xxx
  if ( system ( qsCommand.toLatin1().data()) == -1 )	//xxx
    return;
}

/*void SourceToolBar::mousePressEvent (QMouseEvent *pEvent)	//xxx
{
  //createContextMenu ( pEvent->globalPos  ( ) );
  //m_pPixmapMenu->createContextMenu( pEvent->globalPos  ( ) );
}*/

void SourceToolBar::clear ()
{
  uint t;
  for (t=0;t<(uint)m_listSourceFileEntries.count();t++)	//ox
    delete m_listSourceFileEntries[t];
  m_listSourceFileEntries.clear();

  //m_pListViewAllSources  ->clear();			//ooo
  m_pSourceWidget->m_pListViewAllSources  ->clear();	//xxx
  m_pSourceWidget->m_pListViewVideoSources->clear();
  m_pSourceWidget->m_pListViewAudioSources->clear();
}
