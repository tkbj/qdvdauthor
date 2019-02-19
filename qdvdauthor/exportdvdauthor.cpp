/***************************************************************************
    exportdvdauthor.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Register assignement :
   g0 = Unused
   g1 = Unused
   g2 = jump index for VMGM to root menu of a titleset.
   g3 = jump index within a titleset to menu / title
   g4 = current button ( x * 1024 )
   g5 = Play Multiple. There could be more then one button of this type.
        The value of g5 if gt 0 will be the index of the button.
        
   g6  - g12 = Unused
   g13 - g16 = Internally used by dvdauthor
   
   Note : creating
   - one ExportTitleset - object per titleset
   - one JumpTableEntry per target per titleset I.e. ExportTitleset holds a list of JumpTableEntries.
   - one PlayMultiple per used SourceFileInfo which holds a list of targets to jump to from this video.
    
****************************************************************************/

#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#include "log.h"
#include "utils.h"
#include "global.h"
#include "dvdmenu.h"
#include "exportsrt.h"
#include "qdvdauthor.h"
#include "buttonobject.h"
#include "xml_dvdauthor.h"
#include "exportdvdauthor.h"
#include "sourcefileentry.h"

namespace Export
{

#define MAX_MENUS_PER_TITLESET 99

DVDAuthor::DVDAuthor ( )
{
  m_bOutOfSpecDialog = true;  // show dialog only once.
  m_bMenuZeroDialog  = true;
  m_bInsertComments  = true;
}

DVDAuthor::~DVDAuthor ( )
{
  cleanSourceFileEntris ( );

  ButtonTransition *pTransition = NULL;
  QMap<void *, ButtonTransition *>::iterator it = m_mapTransitionFiles.begin ( );
  while ( it != m_mapTransitionFiles.end ( ) )  {
    pTransition = *it++;
    delete pTransition;
  }
  m_mapTransitionFiles.clear ( );
}

void DVDAuthor::cleanSourceFileEntris ()
{
  uint t;
  // This will clean up the newly created SourceFileEntries.
  // Note that the SourceFileInfos stored in it belong to the document and 
  // are not to be deleted.
  //for (t=0;t<(uint)m_listNewEntries.count();t++) {                //ooo
  for (t=0;t<static_cast<uint> ( m_listNewEntries.count() );t++) {  //xxx
    m_listNewEntries[t]->listFileInfos.clear ();
    // iLOG ("listNewEntryObj<%d>=<%p> is beeing deleted", t, m_listNewEntries[t] );
    delete m_listNewEntries[t];
  }
  m_listNewEntries.clear ();

  //for ( t=0; t<(uint)m_listPlayMultiple.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( m_listPlayMultiple.count ( ) ); t++ )    //xxx
    delete m_listPlayMultiple[t];
  m_listPlayMultiple.clear ( );
}

/**
 * export will open a QFileDialog for selecting the dvdauthor xml file and will export
 * the dvdauthor.xml file,
 *
 */
bool DVDAuthor::exportXml ()
{
  //QString qsFileName = Q3FileDialog::getSaveFileName (Global::qsCurrentPath, QObject::tr ("Xml files (*.xml *.XML)"));				//ooo
  QString qsFileName = QFileDialog::getSaveFileName (NULL, QObject::tr("Save File"), Global::qsCurrentPath, QObject::tr ("Xml files (*.xml *.XML)"));	//xxx

  return createXml ( qsFileName );
}

bool DVDAuthor::createXml ()
{
  Utils theUtils;
  QString qsFileName = theUtils.getTempFile(QString (DVDAUTHOR_XML));
  return createXml ( qsFileName );
}

////////////////////////////////////////////////////////////////////////////////
// dvdauthort xml file creation:
//
// getMenu () -> getButton() -+-> getVideo ()
//                            +-> getAudio ()
//                            +-> getMenu  ()
//
// VMGM
//	menu entry="root"
//	<pre>if ( g2 gt 0 ) {
//	if ( g2 eq 1 ) jump titleset 1 menu entry root;
//	if ( g2 eq 2 ) jump titleset 2 menu entry root;
//	if ( g2 eq 3 ) jump titleset 3 menu entry root;
//	jump title 1; <!-- nothig there, just start the first title // -->
//	}</pre>
//
// TITLESET 1
//	menu entry="root"
//	<pre>if ( g3 gt 0 ) {
//	if ( g3 eq 0 ) jump title 1 chapter 2;
//	if ( g3 eq 1 ) jump title 1 chapter 5;
//	if ( g3 eq 2 ) jump title 2 chapter 1;
//	jump title 1; <!-- nothig there, just start the first title // -->
//	}</pre>
//
// TITLESET 2
//	menu entry="root"
//	<pre>if ( g3 gt 0 ) {
//	if ( g3 eq 0 ) jump title 1;
//	if ( g3 eq 1 ) jump title 2 chapter 8;
//	if ( g3 eq 2 ) jump title 3 chapter 4;
//	jump title 1; <!-- nothig there, just start the first title // -->
//	}</pre>
//	
//0) pre processing the SourceFileEntries:
//	if SourceFileEntry->iTileset is set, then assign all SourceFileInfos the same UNLESS the SourceFileInfo has its own iTitlset set.
//      if SourceFileInfos in SourceFileEntry differ in format, automatically put them into a diff titleset.
//
//1) create listExportTitlsets
// run through all menus (incl VMGM)
// pMenu->getButtonList()
//	listButtons : 
//	- extract associated movies
//	- check if SourceFileInfo has titleset set. (if SourceFileEntry, then all SourceFileInfos in same titleset).
//	- run through listExportTitleset, compare qsFormat (if no Format is avail, create new titleset for each entry)
//

bool DVDAuthor::createXml ( QString &qsFileName )
{  
  m_bInsertComments = false;
  // First step is to obtaiin the menu list (Incl VMGMenu ...).
  m_listOfAllMenus = Global::pApp->getSubMenus ( );

  if ( ( m_listOfAllMenus.count   ( ) == 0 ) &&
       ( Global::pApp->getVMGMenu ( )->isEmpty ( ) ) )
    return createNoMenuDVD ( qsFileName );

  m_listOfAllMenus.prepend ( Global::pApp->getVMGMenu ( ) );

  populateListOfTitlesets   (            ); // create m_listTitlesets from SourceFileInfos
  stowMenusAway             (            );
  createJumpTable           (            );
  return createDVDAuthorXml ( qsFileName );
}

bool DVDAuthor::populateListOfTitlesets ( )
{
  bool             bFound = false;
  uint             iMenu, t, i;
  DVDMenu         *pMenu;
  QString          qsFormat;
  int              iTitleset;
  ExportTitleset  *pTitleset = NULL;
  SourceFileInfo  *pInfo;
  SourceFileEntry *pEntry;
  QList<SourceFileEntry *> listTempButtonMovies, listButtonMovies;	//oxx

  //for ( iMenu=0; iMenu<(uint)m_listOfAllMenus.count ( ); iMenu++ )  {                 //ooo
  for ( iMenu=0; iMenu<static_cast<uint> ( m_listOfAllMenus.count ( ) ); iMenu++ )  {   //xxx
    pMenu = m_listOfAllMenus[iMenu];
    listTempButtonMovies  =  pMenu->getSourceFileEntries ( );
    listButtonMovies.clear ( );
    // Next we want to make sure we do not modify the orig SourceFileEntries ...
    for (t=0;t<(uint)listTempButtonMovies.count();t++) {
      pEntry = listTempButtonMovies[t]->flatClone ( );
      listButtonMovies.append ( pEntry );
      m_listNewEntries.append ( pEntry );
    }
    // clean Up will ensure that each SourceFileEntry only holds same format movies (WidthxHeightxFPS)
    cleanUpSourceFileEntries ( listButtonMovies );

    //for ( t=0; t<(uint)listButtonMovies.count ( ); t++ )  {               //ooo
    for ( t=0; t<static_cast<uint> ( listButtonMovies.count ( ) ); t++ )  { //xxx
      pEntry = listButtonMovies[t];
      if ( pEntry->listFileInfos.count ( ) < 1 )
        continue;

      bFound = false;
      // At this point we have already ensured that all videos in a SourceFileEntry
      // are of the same width/height/FPS
      pInfo = listButtonMovies[t]->listFileInfos[0];
      qsFormat = createFormatString ( pInfo, true );

      if ( pInfo->iTitleset > 0 ) {  // user set titleset number.
        pTitleset = addToTitleset ( pEntry, pInfo->iTitleset, true );
        bFound = true;
      }
      else {  // Next we check if we can find a titleset with the same format.
        //for ( i=0;i<(uint)m_listTitlesets.count();i++)  {                 //ooo
        for ( i=0;i<static_cast<uint> ( m_listTitlesets.count() );i++)  {   //xxx
          pTitleset = m_listTitlesets[i];
          // Note: If the user assigns Titleset numbers then we should not add any additional videos to this titleset
          if ( ( pTitleset->qsFormat == qsFormat ) && ( ! pTitleset->bSetByUser ) ) {
            pTitleset = addToTitleset ( pEntry, pTitleset->iTitlesetNumber );
            bFound = true;
            break; // exit the loop
          }
        }
      }
      // check if we could handle this movie yet.
      if ( ! bFound )  {
        iTitleset = getFirstEmptyTitleset ( );
        pTitleset = addToTitleset ( pEntry, iTitleset );
      }
      if ( ! pTitleset ) {
        //fprintf ( stderr, "Error, Could not add titleset for menu <%s> with format:<%s>\n", pMenu->name ( ).ascii ( ), qsFormat.ascii ( ) );				//ooo
	fprintf ( stderr, "Error, Could not add titleset for menu <%s> with format:<%s>\n", pMenu->name ( ).toLatin1 ( ).data(), qsFormat.toLatin1 ( ).data() );	//xxx
        return false; //ERROR ..
      }
    }

    //2)  // At this point we have all movies tucked away in m_listTitlesets ...
          // In the second step we handle the button action and generate the JumpTable entries

    // Determine JumpTable per titleset
    // also set buttonAction

    //     { g3=iJumpIndex; jump menu 2; } // entry "JumpTable"; }
    // OR  { g3=iJumpIndex; g2=iTargetTitleset; jump VMGM menu 2; } // entry "JumpTable"; }
    // OR  { jump title iTitle chapter iChapter; }
    //  Note : g4 = current Titleset (origin of call )
    //  Note : g1 = current Button
  } // end loop for ( iMenu=0; iMenu<m_listOfAllMenus.count ( ); iMenu++ )
  return true;
}

bool DVDAuthor::stowMenusAway ( )
{
  uint            iMenu, t;
  bool            bFound;
  QString         qsFormat;
  DVDMenu        *pMenu;
  ExportTitleset *pTitleset;
  // second we find a nice titleset for the menus
  //for ( iMenu=0; iMenu<(uint)m_listOfAllMenus.count ( ); iMenu++ )  {                 //ooo
  for ( iMenu=0; iMenu<static_cast<uint> ( m_listOfAllMenus.count ( ) ); iMenu++ )  {   //xxx
    // The titleset should be 
    // a) the same Format
    // b) have a movie which is referenced from the menu
    ////////////////////////////////////////////////////
    pMenu    = m_listOfAllMenus[iMenu];
    qsFormat = createFormatString ( pMenu );

    if ( pMenu->name ( ) == VMGM_LABEL )  {
      // A little special treatment for VMGM
      pTitleset = new ExportTitleset ( this );
      pTitleset->iTitlesetNumber = 0;
      pTitleset->qsFormat        = qsFormat;
      pTitleset->bSetByUser      = true;
      pTitleset->listMenus.append ( pMenu );
      m_listTitlesets.append  ( pTitleset );
      continue;
    }
    bFound    = false;
    pTitleset = NULL;
    if ( pMenu->getInterface ( )->iTitleset > 0 ) {
      // The user specified this Menu's titleset number.
      int iSetByUser = pMenu->getInterface ( )->iTitleset;
      pTitleset = findTitleset ( iSetByUser );
      if ( pTitleset ) {
	// Okay we found the same titleset, now let's make sure it has the same format ...
	if ( ( qsFormat ==  pTitleset->qsFormat ) &&
	     ( pTitleset->listMenus.count ( ) < MAX_MENUS_PER_TITLESET ) )  {
	  pTitleset->listMenus.append ( pMenu );
	  bFound = true;
	}
	else {
	  // Error format mismatch ...
	  // The only remedy is to check all menus in this Titleset
	  // and check if any of these has been set by the user as well.
	  bool bAlsoSetByUser = false;
	  //for ( t=0; t<(uint)pTitleset->listMenus.count ( ); t++ ) {                 //ooo
          for ( t=0; t<static_cast<uint> ( pTitleset->listMenus.count ( ) ); t++ ) {    //xxx
	    if ( pTitleset->listMenus[t]->getInterface ( )->iTitleset > 0 ) {
	      bAlsoSetByUser  = true;
	      break;
	    }
	  }
	  // Okay we can shift the found titleset to a new titleset number ...
	  if ( ! bAlsoSetByUser ) {
	    pTitleset->iTitlesetNumber = getFirstEmptyTitleset ( );
	    // and create a new Titleset with the user defined Titleset number
	    pTitleset = findTitleset ( pMenu );
	    bFound    = true;
	  }
	}
      }
    }
    else  {  // User did not specify a titleset number for this menu
      //for ( t=0; t<(uint)m_listTitlesets.count ( ); t++ ) {                   //ooo
      for ( t=0; t<static_cast<uint> ( m_listTitlesets.count ( ) ); t++ ) {     //xxx
	pTitleset = m_listTitlesets[t];
	// only one Menu in VMGM ... (you ask why. Why not ?)
	if ( pTitleset->iTitlesetNumber == 0 )
	  continue;
	if ( ( qsFormat ==  pTitleset->qsFormat ) &&
	     ( pTitleset->listMenus.count ( ) < MAX_MENUS_PER_TITLESET ) )  {
	  pTitleset->listMenus.append ( pMenu );
	  bFound = true;
	  break; // out of the inner loop
	}
      }
    }
    if ( ! bFound ) {
      // this line will create a new titleset, cause it aint gonna find it in the list.
      pTitleset = findTitleset ( pMenu );
    }
    if ( ! pTitleset )
      //fprintf ( stderr, "Error, Could not create titleset for menu <%s> with format:<%s>\n", pMenu->name ().ascii(), qsFormat.ascii());			//ooo
      fprintf ( stderr, "Error, Could not create titleset for menu <%s> with format:<%s>\n", pMenu->name ().toLatin1().data(), qsFormat.toLatin1().data());	//xxx
  }
  return true;
}

bool DVDAuthor::createJumpTable ( )
{  
  QString           qsAction, qsCurrentButton;
  uint              iMenu, t, iPlayMultipleIdx;
  bool              bAddTitle;
  void             *pObject;
  ButtonObject     *pButton;
  JumpTable        *pJumpTableEntry, *pJumpToMenu;
  DVDMenu          *pMenu, *pButtonMenu, *pVMGMenu;
  SourceFileInfo   *pInfo;
  ExportTitleset   *pTitleset, *pMenuTitleset;
  ButtonTransition *pTransition     = NULL;
  ButtonTransition::Source *pSource = NULL;
  QList<ButtonObject *>listButtons;		//oxx
  pVMGMenu = Global::pApp->getVMGMenu ( );

  // This first run is to ensure all VOBs that are connected to a buttonObject have at least one chapter.
  // the function ButtonObject::chapter will add any missing "00:00:00.000" chapter marker in that case.

  //for ( iMenu=0; iMenu<(uint)m_listOfAllMenus.count ( ); iMenu++ )  {                 //ooo
  for ( iMenu=0; iMenu<static_cast<uint> ( m_listOfAllMenus.count ( ) ); iMenu++ )  {   //xxx
    pMenu = m_listOfAllMenus[iMenu];
    listButtons = pMenu->getButtons ( );
  
    //for ( t=0; t<(uint)listButtons.count ( ); t++ )  {                //ooo
    for ( t=0; t<static_cast<uint> ( listButtons.count ( ) ); t++ )  {  //xxx
      pButton  = listButtons [t];
      pButton->chapter ( false );

      // populate the ButtonTransitions
      if (  pButton->getTransition ( ) )  { // for buttonTransitions we'll add this to the map
        QString qsFileName = pButton->getTransition ( )->qsTransitionVideoFileName;
        if (  ! qsFileName.isEmpty ( ) )  {
          pInfo = pButton->sourceFileInfo ( );
          pButtonMenu = pButton->dvdMenu  ( );  // The menu to which the button points
          pObject     = pInfo ? (void *)pInfo : (void *)pButtonMenu;
          pTransition = findTransition ( pObject );
          if ( ! pTransition )  {
            pTransition = new ButtonTransition ( pInfo, pButtonMenu );
            m_mapTransitionFiles[pObject] = pTransition;
          }
          pTransition->addSource ( pButton, pButtonMenu, qsFileName );
        }
      } // end if ( pButton->getTransition
    }
  }
  // Special handling for Transitions TO the VMGM as we can not add Titles to the VMGM.
  createVMGMTransitions ( );

  iPlayMultipleIdx = 0;

  // second loop through the menus, in case the titlesets have moved ...
  //for ( iMenu=0; iMenu<(uint)m_listOfAllMenus.count ( ); iMenu++ )  {                 //ooo
  for ( iMenu=0; iMenu<static_cast<uint> ( m_listOfAllMenus.count ( ) ); iMenu++ )  {   //xxx
    pMenu = m_listOfAllMenus[iMenu];
    if ( ! pMenu )
      continue;
    listButtons = pMenu->getButtons ( );
    // find the titleset which is associated with this menu
    pMenuTitleset = findTitleset ( pMenu );

    // Next create the Jump tables for all titlesets.
    //for ( t=0; t<(uint)listButtons.count ( ); t++ )  {                //ooo
    for ( t=0; t<static_cast<uint> ( listButtons.count ( ) ); t++ )  {  //xxx  
      // Create JumpTableEntry
      pButton = listButtons[t];
      if ( ! pButton )
        continue;
      pInfo   = pButton->sourceFileInfo ( );
      pJumpTableEntry = new JumpTable;
      pJumpToMenu     = NULL;
      pTitleset       = NULL;
      // store the active button number
      qsCurrentButton = QString ( "g4=%1; "  ).arg ( ( t + 1 ) * 1024 );
      qsAction        = QString ( " { %1%2 " ).arg ( qsCurrentButton ).arg ( getPreAction ( pButton ) );
        
      // replace the button name in qsAction with the associated number.
      getButtonNumberFromName ( listButtons, qsAction ); 

      if ( pInfo )  {  // button to a Video      
        pTitleset = findTitleset ( pInfo );
        // If we have a PlayMultiple-button,
        // we have to create new JumpTableEntries, one for each target.
        createPlayMultiple ( pButton, pMenu, ++iPlayMultipleIdx );

        pJumpTableEntry->iTitle     = pTitleset->findTitle ( pInfo );
        pJumpTableEntry->iChapter   = pButton->chapter ( false ) + pTitleset->getTotalChapters ( pInfo );
        pJumpTableEntry->iJumpIndex = pTitleset->listJumpTable.count ( ) + 1;
        // If the SourceFileInfo has a transition assigned BUT not from this button, then we'll
        // increase the chapter counter to jump straight to the video and skip the transition video.
        pTransition = findTransition ( pInfo, true ); // check for "00:00:00.000"
        if ( ( pTransition ) && ! pButton->getTransition ( ) )
          pJumpTableEntry->iChapter++;

        ButtonTransition *pTrans = pTransition; // need to keep info around
        pTransition = findTransition ( pInfo, false ); // check if we have a transition in general to this source
        if ( pTransition ) {
          bAddTitle = false;
          // Here we have a transition button to the video source
          pSource = pTrans ? pTrans->findChapter ( QString ( "00:00:00.000" ) ) : NULL;
          if ( pSource && ( pSource->pSourceButton != pButton ) )
            bAddTitle = true;

          if ( ! pSource )  { // Button transition to a chapter
            pSource   = pTransition->findButton ( pButton );
            pTrans    = pTransition;
            bAddTitle = true;
          }
          if ( pSource && pTrans && bAddTitle )  {
            // First transition does NOT come from this Source.
            pSource = pTrans->findButton ( pButton );
            if ( pSource )  {
              // At this point we know that this button is NOT the first to point to chapter 00:00:00.000
              // In fact the button coud point to any chapter of th video source.
              int  iChapterAdjust = 0;
              if ( pSource->pSourceButton != pButton )
                   iChapterAdjust = 1;
              pSource->iChapter   = pJumpTableEntry->iChapter + iChapterAdjust;
              pSource->iTitle     = pJumpTableEntry->iTitle;
              pJumpTableEntry->iTitle   = pTitleset->findTitle ( pTransition, pButton );
              pJumpTableEntry->iChapter = 1;
            }
          }
          // Note: The first transitionButton to chapter 00:00:00.000 does not need further attention here
        }
        optimizeJumpTable ( pTitleset, pJumpTableEntry );

        if ( pButton->getPlayMultipleList ( ).count ( ) > 1 )
          qsAction += QString ( "g5 = %1; " ).arg   ( iPlayMultipleIdx );

        // Add button and buttonAction ...
        if ( pMenuTitleset == pTitleset ) {
          if ( pJumpTableEntry->iChapter == 0 )
            qsAction += QString ("jump title %1; } ").arg ( pJumpTableEntry->iTitle );
          else
            qsAction += QString ("jump title %1 chapter %2; } ").arg (pJumpTableEntry->iTitle).arg (pJumpTableEntry->iChapter);
        }
        else if ( pMenuTitleset->iTitlesetNumber == 0 )  // VMGM
          qsAction += QString ("g3=%1; jump titleset %2 menu entry root; } " ).arg ( pJumpTableEntry->iJumpIndex ).arg ( pTitleset->iTitlesetNumber );
        else  // if we are in the wrong titleset then we have to go through the VMGM ...
          qsAction += QString ("g2=%1; g3=%2; jump vmgm menu entry title; } ").arg ( pTitleset->iTitlesetNumber ).arg ( pJumpTableEntry->iJumpIndex );
      }
      else {  // button to a menu3 eq 2 ) jump      
        qsAction    = QString ( " { g4=0; %1 " ).arg ( getPreAction ( pButton ) );
        pButtonMenu = getMenuFromButton ( pButton ); // button points to this menu
        if ( pButtonMenu ) { 
          pTitleset   = findTitleset   ( pButtonMenu );
          pTransition = findTransition ( pButtonMenu, false );
          pJumpTableEntry->iJumpIndex = pTitleset->listJumpTable.count  ( ) + 1;
          // Check IF there IS a transition to this menu AND IF it comes from this button.
          if ( ( pTransition )  &&  ( pTransition->findButton ( pButton ) ) && ( pButtonMenu != pVMGMenu ) ) {
            pJumpTableEntry->iTitle   = pTitleset->findTitle  ( pTransition );
            pJumpTableEntry->iChapter = 1;
            pTransition->iJumpIndex   = pJumpTableEntry->iJumpIndex + 1;
            // Next we need to assure that there is a jump to the actual menu, so we can come back to it.
            pJumpToMenu = new JumpTable;
            pJumpToMenu->iJumpIndex   = pTransition->iJumpIndex;
            pJumpToMenu->pButton      = pButton;
            pJumpToMenu->iMenu        = pTitleset->findMenu ( pButtonMenu );
            pJumpToMenu->pMenu        = pButtonMenu;
          }
          else  {
            pJumpTableEntry->iMenu    = pTitleset->findMenu ( pButtonMenu );
            pJumpTableEntry->pMenu    = pButtonMenu;
          }
          optimizeJumpTable ( pTitleset, pJumpTableEntry );

          if ( pMenuTitleset == pTitleset )  {
            if ( pJumpTableEntry->iMenu == 0 )
              qsAction += QString ( "g3=0; call menu entry root; } " );
            else if ( pMenuTitleset->iTitlesetNumber == 0 )
              qsAction += QString ( "g3=%1; jump menu entry title; } " ).arg ( pJumpTableEntry->iJumpIndex );
            else
              qsAction += QString ( "g3=%1; jump menu entry root; } " ).arg ( pJumpTableEntry->iJumpIndex );
          }
          else if ( pMenuTitleset->iTitlesetNumber == 0 )
            qsAction += QString ( "g3=%2; jump titleset %3 menu entry root; } " ).arg ( pJumpTableEntry->iJumpIndex ).arg ( pTitleset->iTitlesetNumber );
          else
            qsAction += QString ( "g2=%1; g3=%2; jump vmgm menu entry title; } " ).arg ( pTitleset->iTitlesetNumber ).arg ( pJumpTableEntry->iJumpIndex );
        }
        else { // error, no menu / video found that fits the bill ...
          delete pJumpTableEntry;
          continue;
        }
      }

      if ( m_bInsertComments )
        qsAction += QString (" <!-- %1 // -->").arg ( pButton->action ( ) );

      pJumpTableEntry->qsAction = qsAction;
      pJumpTableEntry->pButton  = pButton;

      pTitleset->listJumpTable.append  ( pJumpTableEntry );
      if ( pJumpToMenu )
           pTitleset->listJumpTable.append ( pJumpToMenu );
    } // end for listButtons
  }
  // at this point we have all information readily available to create the xml - file ...

  return true;
}

bool DVDAuthor::optimizeJumpTable ( ExportTitleset *pTitleset, JumpTable *pNewJumpTableEntry )
{
  // This function will parse through the JumTable and check wether or not this jumpTarget already exists.
  // In case the jumpTarget is found we'll substitute iJumpIndex and flag the Entry as AlreadyPresent
  JumpTable *pJumpTableEntry = NULL;
  if ( pNewJumpTableEntry->iMenu == -1 ) {
    // Okay this is a Jump to a Video / Chapter
    //for ( unsigned int t=0; t<(uint)pTitleset->listJumpTable.count ( ); t++ ) {                   //ooo
    for ( unsigned int t=0; t<static_cast<uint> ( pTitleset->listJumpTable.count ( ) ); t++ ) {     //xxx
      pJumpTableEntry = pTitleset->listJumpTable[t];
      if ( ( pJumpTableEntry->iTitle   == pNewJumpTableEntry->iTitle   ) && 
           ( pJumpTableEntry->iChapter == pNewJumpTableEntry->iChapter ) )  {
          pNewJumpTableEntry->iJumpIndex = pJumpTableEntry->iJumpIndex;
          pNewJumpTableEntry->bAlreadyPresent = true;
          return true;
      }
    }
  }
  else  {
    //for ( unsigned int t=0; t<(uint)pTitleset->listJumpTable.count ( ); t++ ) {                   //ooo
    for ( unsigned int t=0; t<static_cast<uint> ( pTitleset->listJumpTable.count ( ) ); t++ ) {     //xxx
      pJumpTableEntry = pTitleset->listJumpTable[t];
      if ( pJumpTableEntry->iMenu  == pNewJumpTableEntry->iMenu )  {
           pNewJumpTableEntry->iJumpIndex  = pJumpTableEntry->iJumpIndex;
           pNewJumpTableEntry->bAlreadyPresent = true;
           return true;
      }
    }
  }
  return false;
}

void DVDAuthor::createVMGMTransitions ( )
{
  DVDMenu *pVMGMenu = Global::pApp->getVMGMenu ( );
  ButtonTransition *pTransition = findTransition ( pVMGMenu, false );
  if ( ! pTransition )
    return;  // Not to worry.

  // This function will creae the Titleset which hods only the transition videos to the VMGMenu
  JumpTable *pJumpTable         = NULL;
  ExportTitleset *pVMGMTitleset = findTitleset   ( pVMGMenu );
  ExportTitleset *pTitleset     = new ExportTitleset ( this );
  pTitleset->iTitlesetNumber    = getFirstEmptyTitleset   ( );
  pTitleset->qsFormat           = pVMGMTitleset->qsFormat;
  pTitleset->listButtonTransitions.append ( pTransition );

  ButtonTransition::Source *pSource = NULL;
  QList<ButtonTransition::Source *>::iterator it = pTransition->listSource.begin ( );		//oxx
  int iIdx = 0;
  while ( it != pTransition->listSource.end ( ) )  {
    pSource = *it++;
    iIdx ++;
    // Create JumpTable entries for this Titleset.
    pJumpTable = new JumpTable;
    pJumpTable->iJumpIndex = iIdx;
    pJumpTable->iTitle     = iIdx;
    pJumpTable->iChapter   = 1;
    pJumpTable->pButton    = pSource->pSourceButton;
    pJumpTable->pMenu      = pVMGMenu;
    pTitleset->listJumpTable.append ( pJumpTable );
  }
  m_listTitlesets.append ( pTitleset );
}

void DVDAuthor::createPlayMultiple ( ButtonObject *pButton, DVDMenu *pDVDMenu, uint iPlayMultipleIdx )
{
  // What do we need here ...
  // We need the Source and Target SourceFileInfo
  // we need the button number
  ////////////////////////////////////
  if ( ! pButton )
    return;

  uint            t;
  SourceFileInfo *pSourceInfo,     *pTargetInfo;
  ExportTitleset *pSourceTitleset, *pTargetTitleset;
  JumpTable      *pJumpTableEntry;
  DVDMenu        *pTargetMenu;
  PlayMultiple   *pMultiple;
  QString         qsPlayMultiple;

  QStringList listPlayMultiple = pButton->getPlayMultipleList ( );
  if ( pButton->getLoopMultiple ( ) ) // if we want to loop we should add one more jump to the list
    listPlayMultiple.append ( listPlayMultiple[0] );

  if ( listPlayMultiple.count ( ) < 2 ) // first entry is already handled through pButton->action ( )
    return;

  pSourceInfo     = pDVDMenu->getInfoFromButtonAction ( listPlayMultiple[0] );
  pSourceTitleset = findTitleset  ( pSourceInfo );
  if ( ( ! pSourceInfo ) || ( ! pSourceTitleset ) )
    return;

  //for ( t=1; t<(uint)listPlayMultiple.count ( ); t++ ) {                  //ooo
  for ( t=1; t<static_cast<uint> ( listPlayMultiple.count ( ) ); t++ ) {    //xxx
    pTargetInfo = pDVDMenu->getInfoFromButtonAction ( listPlayMultiple[t] );

    if ( ! pTargetInfo ) { // The last target can be a Menu as well
      pTargetMenu = pDVDMenu->getMenuFromButtonAction ( listPlayMultiple[t] );
      if ( ! pTargetMenu )
        continue;
      // g5=0; g3=0 -> To reset PlayMultiple and to actually go to the Menu and not to a video
      createJumpToMenu ( pSourceInfo, pTargetMenu, iPlayMultipleIdx );
      return;
    }

    if ( ( ! pTargetInfo ) || ( ! pSourceInfo ) )
      continue;

    pMultiple = findMultiple ( pSourceInfo, false );
    if ( ! pMultiple ) {
      pMultiple = new PlayMultiple ( pSourceInfo, pTargetInfo );
      m_listPlayMultiple.append    ( pMultiple );
    }

    pTargetTitleset = findTitleset ( pTargetInfo );
    if ( ( ! pTargetTitleset )  || ( ! pSourceTitleset ) ) 
      continue;

    pJumpTableEntry = new JumpTable;
    // +1 because the buttonAction has not been added to the list
    pJumpTableEntry->iJumpIndex = pTargetTitleset->listJumpTable.count ( ) + 1;
    pJumpTableEntry->iTitle     = pTargetTitleset->findTitle   ( pTargetInfo );
    pJumpTableEntry->iChapter   = pTargetTitleset->getChapters ( pTargetInfo, listPlayMultiple[t] ) +
                                  pTargetTitleset->getTotalChapters ( pTargetInfo );
    if ( findTransition ( pTargetInfo, true ) )
      pJumpTableEntry->iChapter++;

    pJumpTableEntry->pButton    = pButton;
    optimizeJumpTable ( pTargetTitleset, pJumpTableEntry );

    qsPlayMultiple = QString ( "\t\tif ( g5 eq %1 ) { " ).arg ( iPlayMultipleIdx );
    // Add button and buttonAction ...
    if ( pSourceTitleset == pTargetTitleset ) {
      if ( pJumpTableEntry->iChapter == 0 )
        qsPlayMultiple += QString ("jump title %1; ").arg ( pJumpTableEntry->iTitle );
      else
        qsPlayMultiple += QString ("jump title %1 chapter %2; ").arg ( pJumpTableEntry->iTitle ).arg ( pJumpTableEntry->iChapter );
      // Since we jump directly to the target within the same Titleset,
      // we do not need another entry in the JumpTable
      delete pJumpTableEntry;
      pJumpTableEntry = NULL;
    }
    else  // if we are in the wrong titleset then we have to go through the VMGM ...
      qsPlayMultiple += QString ( "g2 = %2; g3 = %3; " ).arg ( pTargetTitleset->iTitlesetNumber ).arg ( pJumpTableEntry->iJumpIndex );

    //if ( ( t == (uint)listPlayMultiple.count( ) - 1 ) && // the last video played we should reset the g5 register                 //ooo
    if ( ( t == static_cast<uint> ( listPlayMultiple.count( ) - 1 ) ) && // the last video played we should reset the g5 register    //xxx
         ( ! pButton->getLoopMultiple ( )  )  )    // ... but only if the user does NOT want to loop
      qsPlayMultiple += "g5 = 0; ";
    qsPlayMultiple += "}";

    pMultiple->addPlayMultiple ( qsPlayMultiple );
    if ( pJumpTableEntry )
         pTargetTitleset->listJumpTable.append ( pJumpTableEntry );

    // lastly we set the new sourceTitleset and sourceInfo for the newxt loop
    pSourceTitleset = pTargetTitleset;
    pSourceInfo     = pTargetInfo;
  }
}

void DVDAuthor::createJumpToMenu ( SourceFileInfo *pSourceInfo, DVDMenu *pTargetMenu, uint iPlayMultipleIdx )
{
  // Only called from createPlayMultiple
  ExportTitleset *pTargetTitleset = findTitleset ( pTargetMenu );
  if ( ! pTargetTitleset )
    return;

  QString qsAction;
  PlayMultiple   *pMultiple;
  ExportTitleset *pSourceTitleset = findTitleset  ( pSourceInfo );
  JumpTable *pJumpTableEntry  = new JumpTable;
  pJumpTableEntry->iJumpIndex = pTargetTitleset->listJumpTable.count()+1;
  pJumpTableEntry->iMenu      = pTargetTitleset->findMenu ( pTargetMenu );
  pJumpTableEntry->pMenu      = pTargetMenu;
  optimizeJumpTable ( pTargetTitleset, pJumpTableEntry );

  // g5=0; g3=0 -> To reset PlayMultiple and to actually go to the Menu and not to a video
  qsAction = QString ( "\t\tif ( g5 eq %1 ) { g5=0; " ).arg ( iPlayMultipleIdx );

  if ( pTargetTitleset == pSourceTitleset ) {
    if ( pJumpTableEntry->iMenu == 0 )
      qsAction += QString ("g3=0; call menu entry root; } ");
    else
      qsAction += QString ("g3=%1; call menu; } ").arg ( pJumpTableEntry->iJumpIndex );
  }
  else
    qsAction += QString ("g2=%1; g3=%2; call vmgm menu entry title; } ").arg (pTargetTitleset->iTitlesetNumber).arg(pJumpTableEntry->iJumpIndex);

  pMultiple = findMultiple ( pSourceInfo, false );
  if ( ! pMultiple ) {
    pMultiple = new PlayMultiple ( pSourceInfo, NULL );
    m_listPlayMultiple.append    ( pMultiple );
  }
  pMultiple->addPlayMultiple     ( qsAction  );
  pTargetTitleset->listJumpTable.append ( pJumpTableEntry );
}

bool DVDAuthor::createNoMenuDVD ( QString &qsFileName )
{
  // This function will create a dvdauthor XML file for a DVD without a menu.
  // I.e. you put the DVD in and it will play all videos, one after another.
  int t, iSourceFileCount;
  CXmlDVDAuthor xmlDVDAuthor;
  CXmlDVDAuthor::titleset_struct *pXMLTitleset;

  if ( Global::qsProjectPath.isEmpty () )
    xmlDVDAuthor.m_dvdauthor.dest = QString ( Global::qsTempPath ); //"/tmp/");
  else
    xmlDVDAuthor.m_dvdauthor.dest = Global::qsProjectPath;

  pXMLTitleset = xmlDVDAuthor.m_dvdauthor.addTitleset();

  iSourceFileCount = Global::pApp->sourceFileCount ();
  for ( t=0;t<iSourceFileCount;t++)
    addToTitleset ( pXMLTitleset, Global::pApp->sourceFileEntry ( t ) );

  xmlDVDAuthor.writeXml ( qsFileName );
  return false;
}

bool DVDAuthor::createDVDAuthorXml ( QString &qsFileName )
{  
  uint t, i;
  QString qsPre;
  CXmlDVDAuthor::titleset_struct *pXMLTitleset;
  CXmlDVDAuthor::menus_struct    *pXMLMenu;
  CXmlDVDAuthor::pgc_struct      *pPgc;

  // main DVDAuthorXML - object
  CXmlDVDAuthor::titles_struct *pTitle;
  CXmlDVDAuthor         xmlDvdauthor;
  QList<DVDMenu *> listDVDMenus = Global::pApp->getSubMenus ( );	//oxx
  CDVDMenuInterface    *pInterface   = Global::pApp->getVMGMenu  ( )->getInterface ( );
  ExportTitleset       *pExpTitleset;
  int                   iMaxTitlesetNumber;

  // First we create the main menu ...
  xmlDvdauthor.m_dvdauthor.dest = Global::qsProjectPath;
  pXMLMenu = xmlDvdauthor.m_dvdauthor.vmgm.addMenus ( );

  addToVideo ( &pXMLMenu->video, pInterface->iWidth, pInterface->iHeight, pInterface->qsRatio );
  addToAudio (  pXMLMenu, Global::pApp->getVMGMenu ( ) );

  // First we create the JumpTable for the Main Menu VMGM
  // VMGM
  //	menu entry="root"
  //	<pre>if ( g2 gt 0 ) {
  //	if ( g2 eq 1 ) jump titleset 1 menu entry root;
  //	if ( g2 eq 2 ) jump titleset 2 menu entry root;
  //	if ( g2 eq 3 ) jump titleset 3 menu entry root;
  //	jump title 1; <!-- nothig there, just start the first title // -->
  //	}</pre>
  pPgc = pXMLMenu->addPgc ( );

  // the following line will add the actual VMGMenu
  addToMenu ( pPgc, Global::pApp->getVMGMenu ( ) );
  pPgc->entry = QString ("title");
  qsPre  = QString (" {" );
  if ( ! pInterface->qsPre.isEmpty ( ) )
    qsPre += pInterface->qsPre;
  qsPre += QString ( " if ( g4 gt 0 ) button=g4; \n");
  qsPre += QString ("\t   if ( g2 gt 0 )  { \n");
  //for (t=0;t<(uint)m_listTitlesets.count();t++) {                 //ooo
  for (t=0;t<static_cast<uint> ( m_listTitlesets.count() );t++) {   //xxx
    pExpTitleset = m_listTitlesets[t];
    // we need no jump from VMGM to VMGM
    if ( pExpTitleset->iTitlesetNumber == 0 )
      continue;
    qsPre += QString ("\t\tif ( g2 eq %1 ) jump titleset %2 menu entry root;\n").arg (pExpTitleset->iTitlesetNumber).arg (pExpTitleset->iTitlesetNumber);
  }

  qsPre   += QString ("\t\tjump title 1;");
  if ( m_bInsertComments )
    qsPre += QString (" <!-- nothing there, just start the first title // -->\n");
  qsPre   += QString ("\t\t}\n\t} ");
  pPgc->pre.value     = qsPre;
  pPgc->post.value    = QString (" { g4=button; g2=0; " );
  if ( pInterface->qsPost.isEmpty ( ) )
    pPgc->post.value += QString ( "jump vmgm menu 1;" );
  else if ( pInterface->qsPost == "Loop" ) // This is for the VMGMenu only ...
    pPgc->post.value += QString ( "jump vmgm menu 1;" );
  else
    pPgc->post.value += pInterface->qsPost;
  pPgc->post.value   += QString ( " } ");

  CXmlDVDAuthor::vob_struct *pVob = NULL;
  //for (t=0;t<(uint)pInterface->listIntroVobs.count ( );t++ ) {                //ooo
  for (t=0;t<static_cast<uint> ( pInterface->listIntroVobs.count ( ) );t++ ) {  //xxx
    // play only once is handled in the fpc - tag.
    if ( pInterface->listIntroVobs[t]->pause == "once" ) {
      xmlDvdauthor.m_dvdauthor.vmgm.fpc.value = "{ jump menu 2; }";
      pPgc = pXMLMenu->addPgc ( );
      pPgc->post.value = QString (" { jump vmgm menu 1; } ");
      pVob = pPgc->addVob ( );
      pVob->file  = pInterface->listIntroVobs[t]->file;
      pVob->pause = QString ( "0" );
      break; // Only one Intro - VOB allowed
    }
  }

  // Next we should take care of all sub-menus ...
  iMaxTitlesetNumber = getHighestTitlesetNumber ( );
  if ( iMaxTitlesetNumber < 0 )
       iMaxTitlesetNumber = 0;
  //for ( t=1; t<=(uint)iMaxTitlesetNumber; t++ )  {                //ooo
  for ( t=1; t<=static_cast<uint> ( iMaxTitlesetNumber ); t++ )  {  //xxx
    pExpTitleset = findTitleset ( t );
    pXMLTitleset = xmlDvdauthor.m_dvdauthor.addTitleset();
    // check if we found an empty titleset.
    if ( pExpTitleset ) {
      pXMLTitleset->comment = QString ( "Titleset %1" ).arg ( pExpTitleset->iTitlesetNumber );
      // Add the root menu with the JumpTableEntries to the Titleset
      addToMenu ( pXMLTitleset, pExpTitleset );

      // Next we insert all Menus ...
      //for ( i=0; i<(uint)pExpTitleset->listMenus.count ( ); i++ )                 //ooo
      for ( i=0; i<static_cast<uint> ( pExpTitleset->listMenus.count ( ) ); i++ )   //xxx
        addToMenu ( pXMLTitleset, pExpTitleset->listMenus[i] );

      // Next we insert all SourceFileInfos ...
      //for ( i=0; i<(uint)pExpTitleset->listSourceFileEntries.count ( ); i++ )                 //ooo
      for ( i=0; i<static_cast<uint> ( pExpTitleset->listSourceFileEntries.count ( ) ); i++ )    //xxx
        addToTitleset ( pXMLTitleset, pExpTitleset->listSourceFileEntries[i] );

      // And finally we add the titles which are required for the ButtonTransition to the SubMenu.
      //for ( i=0; i<(uint)pExpTitleset->listButtonTransitions.count ( ); i++ )                 //ooo
      for ( i=0; i<static_cast<uint> ( pExpTitleset->listButtonTransitions.count ( ) ); i++ )    //xxx
        addToTitleset ( pXMLTitleset, pExpTitleset->listButtonTransitions[i] );
    }

    // Each Titleset ought to have at least one title with one pgc
    if ( ! pXMLTitleset->ppArrayTitles ) {
      pTitle = pXMLTitleset->addTitles ( );
      pTitle->addPgc ( );
    }

    // Found out the hard way that a Titleset also ought to possess at least one menu - pgc - entry.
    if ( ! pXMLTitleset->ppArrayMenus )   {
      pXMLMenu = pXMLTitleset->addMenus ( );
      QString qsRatio ( "4:3" );
      addToVideo ( &pXMLMenu->video, 720, 480, qsRatio );
      pXMLMenu->addPgc ( );
    }
  }

  xmlDvdauthor.writeXml ( qsFileName );

  return true;
}

ExportTitleset *DVDAuthor::addToTitleset ( SourceFileEntry *pEntry, int iTitleset, bool bSetByUser /* = false */ )
{
  uint t, i;
  bool bFound = false;
  ExportTitleset  *pTitleset   = NULL;
  SourceFileEntry *pTSEntry    = NULL;
  SourceFileInfo  *pFormatInfo = pEntry->listFileInfos[0];
  QString qsFormat = createFormatString ( pFormatInfo, true );

  // iLOG ("pEntry=<%p> titleset=%d", pEntry, iTitleset );
  // bSetByUser flag will try to force the titlesetNumber
  //for (t=0;t<(uint)m_listTitlesets.count();t++)  {                //ooo
  for (t=0;t<static_cast<uint> ( m_listTitlesets.count() );t++)  {  //xxx
    pTitleset = m_listTitlesets[t];
    if ( pTitleset->iTitlesetNumber == iTitleset ) {
      // Check if the titleset is alraedy taken by the automatic assignement algol
      if ( bSetByUser && ! pTitleset->bSetByUser ) {
        // in which case we re-assign the current Tilesets titleset number.
        pTitleset->iTitlesetNumber = getFirstEmptyTitleset ( );
        // exit out of for-loop to create a new Titleset
        t=m_listTitlesets.count() + 1;
        bFound = false;
        continue;
      }
      // check if we already assigned this TitlesetNumber to some other video format that does not match
      if ( ( qsFormat != pTitleset->qsFormat ) )  {
        iTitleset = getFirstEmptyTitleset  ( );
        // exit out of for-loop to create a new Titleset
        t=m_listTitlesets.count() + 1;
        bFound = false;
        continue;
      }

      // Next run through the SourceFileInfo list and make sure the SourceFileInfo is not already present.
      //for ( i=0; i<(uint)pTitleset->listSourceFileEntries.count ( ); i++ )  {                 //ooo
      for ( i=0; i<static_cast<uint> ( pTitleset->listSourceFileEntries.count ( ) ); i++ )  {   //xxx
        pTSEntry = pTitleset->listSourceFileEntries[i];
        //if ( pTitleset->listSourceFileEntries[i] == pEntry ) { // would generate muliple PGS for the same Entry if it comes from different menus.
        if ( pTSEntry->qsDisplayName == pEntry->qsDisplayName ) {
          bFound = true;
          break;
        }
      }
      if ( ! bFound )  {
        pTitleset->listSourceFileEntries.append ( pEntry );
        if ( ! pTitleset->bSetByUser ) 
               pTitleset->bSetByUser = bSetByUser;
      }
      bFound = true;
    }
    else {
      // Next we check if this entry is in any other titleset and remove it
      //for ( i=0; i<(uint)pTitleset->listSourceFileEntries.count ( ); i++ )  {                 //ooo
      for ( i=0; i<static_cast<uint> ( pTitleset->listSourceFileEntries.count ( ) ); i++ )  {   //xxx
        pTSEntry = pTitleset->listSourceFileEntries[i];
        //if ( pTitleset->listSourceFileEntries[i] == pEntry ) // see above
        if ( pTSEntry->qsDisplayName == pEntry->qsDisplayName )
            //pTitleset->listSourceFileEntries.remove  ( pEntry );	//ooo
	    pTitleset->listSourceFileEntries.removeOne  ( pEntry );	//xxx
      }
    }
  }
  // Finally, if we have not found the titlsetNumber in the list, we should create a new titleset
  if ( ! bFound )  {
    pTitleset = new ExportTitleset ( this );
    pTitleset->listSourceFileEntries.append ( pEntry );
    pTitleset->iTitlesetNumber = iTitleset;
    pTitleset->qsFormat        = qsFormat;
    pTitleset->bSetByUser      = bSetByUser;
    m_listTitlesets.append ( pTitleset );
  }

  // NOTE: This algo can result in empty titlesets (no associated SourceFileInfos ) if the user defines the titlesetNumber
  //       fo a given SourceFileInfo / SourceFileEntry.	
  return pTitleset;
}

bool DVDAuthor::addToTitleset ( CXmlDVDAuthor::titleset_struct *pXMLTitleset, SourceFileEntry *pEntry )
{
  // This function creates the <titles> and the associated <video> tags.
  CXmlDVDAuthor::titles_struct *pTitle;
  SourceFileInfo *pInfo;
  TranscodeInterface *p;
  int t, iMaxAudio, iMaxSubtitle;
  // Note: according to DVDAuthor spec only one Titles - tag exists. My bad when designing the class !
  // iLOG ("pEntry=<%p> count=%d", pEntry,  pEntry->listFileInfos.count() );
  if ( ! pXMLTitleset->ppArrayTitles ) {
    pTitle = pXMLTitleset->addTitles ( );

    iMaxAudio = pEntry->audioCount ( true );
    for ( t=0;t<=iMaxAudio; t++ ) {
      if ( pEntry->arrayAudioEntries[ t ].iAudioNumber == -1 )
        pTitle->audio [ t ].lang = "en"; // default to english if nothing has been specified.
      //pTitle->audio [ pEntry->arrayAudioEntries[ t ].iAudioNumber ]. lang = pEntry->arrayAudioEntries[ t ].qsIso639;
      pTitle->audio [ t ].lang = pEntry->arrayAudioEntries[ t ].qsIso639;
    }
    iMaxSubtitle = pEntry->subtitleCount ( true );
    for ( t=0;t<iMaxSubtitle+1; t++ ) {
      if ( pEntry->arraySubtitleEntries[ t ].m_iSubtitleNumber == -1 )
        pTitle->subpicture [ t ].lang = "en"; // default to english if nothing has been specified.
      pTitle->subpicture [ t ].lang = pEntry->arraySubtitleEntries[ t ].m_qsIso639;
    }

    if ( pEntry->listFileInfos.count() ) {
      pInfo = pEntry->listFileInfos[0];
      if  ( pInfo )  {
        p = pInfo->pTranscodeInterface;
        if ( p )
          addToVideo ( &pTitle->video, p->qsResolution, p->qsRatio );
        else
          addToVideo ( &pTitle->video, pInfo->qsResolution, pInfo->qsRatio );
      }
    }
  }
  pTitle = pXMLTitleset->ppArrayTitles[0];
  return addToTitleset ( pTitle, pEntry );
}

bool DVDAuthor::addToTitleset ( CXmlDVDAuthor::titles_struct *pTitle, SourceFileEntry *pEntry )
{
  // This function creates the <titles> part of a titleset.
  CXmlDVDAuthor::pgc_struct *pPgc = NULL;
  ExportTitleset temp ( this );
  // There are two ways to do this.

  // The first way is if none of the info's has and post or pre
  // command. All videos are stored in one title ( pgc )

  if ( ! temp.checkForPrePostCommands ( pEntry, true ) ) { // true == the fist pPgc can have a pre and the last is allowed to have a post command
    pPgc = pTitle->addPgc ( );
    pPgc->comment = QString ( "Title %1" ).arg ( pTitle->pgcCount ( ) );

    return addToTitleset  ( pPgc, pEntry );
  }

  // The second way is to give each video a title. 
  // Note: according to DVD spec only 99 titles 
  // per titleset are allowed.
  uint i;
  //for (i=0;i<(uint)pEntry->listFileInfos.count();i++) {               //ooo
  for (i=0;i<static_cast<uint> ( pEntry->listFileInfos.count() );i++) { //xxx
    pPgc = pTitle->addPgc ( );
    if ( addToTitleset ( pPgc, pEntry->listFileInfos[i] ) )
         pPgc->post.value += "\n\t" + jumpToSubMenu ( pEntry );

    // just in case to ensure something is happening afterwards.
    if ( pPgc->post.value.isEmpty ( ) )
         pPgc->post.value = " { g2 = 0; call vmgm menu entry title; } ";
  }
  //  if ( pPgc && pPgc->post.value.isEmpty ( ) )
  //    pPgc->post.value = QString (" { g2 = 0; call vmgm menu entry title; } ");

  return true;
}

void DVDAuthor::addTransition ( CXmlDVDAuthor::pgc_struct *pPgc, SourceFileInfo *pInfo )
{
  if ( ! pInfo || ! pPgc )
    return;

  CXmlDVDAuthor::vob_struct *pVob = NULL;
  ButtonTransition   *pTransition = findTransition ( (void *)pInfo, true );
  if ( pTransition )  {
    ButtonTransition::Source *pSource = pTransition->findChapter ( QString ( "00:00:00.000" ) );
    if ( pSource && ( pSource->iChapter == -1 ) )  {
      pVob           = pPgc->addVob ( );
      pVob->file     = pSource->qsTransitionFileName;
      pVob->chapters = "00:00:00.000";
      pVob->pause    = "0";
    }
  }
}

bool DVDAuthor::addToTitleset ( CXmlDVDAuthor::pgc_struct *pPgc, SourceFileEntry *pEntry )
{
  uint i;
  //for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ )               //ooo
  for ( i=0; i<static_cast<uint> ( pEntry->listFileInfos.count ( ) ); i++ ) //xxx
     addToTitleset ( pPgc, pEntry->listFileInfos[i] );

  pPgc->pre.value  = pEntry->qsPre  + pPgc->pre.value;
  pPgc->post.value = pEntry->qsPost + pPgc->post.value;

  if ( pEntry->qsPost.isEmpty ( ) )
    pPgc->post.value = jumpToSubMenu ( pEntry );
  //pPgc->post.value = QString (" { g2 = 0; call vmgm menu entry title; } ");

  if ( pEntry->iPause == -1 )
    pPgc->pause = QString ("inf");
  else
    pPgc->pause = QString ("%1").arg (pEntry->iPause);

  return true;
}

bool DVDAuthor::addToTitleset ( CXmlDVDAuthor::pgc_struct *pPgc, SourceFileInfo *pInfo )
{
  bool bReturn = false;
  // This function assigns the Video files to the <titles> part of the TitleSet
  CXmlDVDAuthor::vob_struct *pVob = NULL;
  pPgc->pre.value  = pInfo->qsPreCommand;
  PlayMultiple *pMultiple = findMultiple ( pInfo, false );
  pPgc->post.value = "";
  if ( pMultiple )
    pPgc->post.value = pMultiple->getPostCommand ( );
  // signal the calling function that we still need the
  // standard post commands behind the PlayMultiple commands.
  if ( ! pPgc->post.value.isEmpty ( ) )
    bReturn = true;

  pPgc->post.value  += pInfo->qsPostCommand;

  addTransition ( pPgc, pInfo );

  pVob = pPgc->addVob ( );
  pVob->file     = pInfo->getFileName ( );
  pVob->chapters = pInfo->listChapters.join ( QString ( "," ) );
  // However if we transcode and crop the start, then we have to adjust the chapters.
  if ( ( pInfo->pTranscodeInterface ) &&
       ( pInfo->pTranscodeInterface->iStartingAt > 0 ) &&
       ( pInfo->listChapters.count ( ) > 0 ) )
    pVob->chapters = adjustChapterOffset ( pInfo->listChapters, pInfo->pTranscodeInterface->iStartingAt );

  // And finally we give it at least one chapter
  if ( pVob->chapters.isEmpty ( ) )
       pVob->chapters = "00:00:00.000";

  if ( pInfo->iPause == -1 )
    pVob->pause  = QString ( "inf" );
  else
    pVob->pause  = QString ( "%1" ).arg ( pInfo->iPause );

  return bReturn;
}

bool DVDAuthor::addToTitleset ( CXmlDVDAuthor::titleset_struct *pXMLTitleset, ButtonTransition *pTransition )
{
  // Note: according to DVDAuthor spec only one Titles - tag exists. My bad when designing the class !
  // iLOG ("pEntry=<%p> count=%d", pEntry,  pEntry->listFileInfos.count() );
  ButtonTransition::Source     *pSource = NULL;
  CXmlDVDAuthor::titles_struct *pTitle  = NULL;
  CXmlDVDAuthor::pgc_struct    *pPgc    = NULL;
  CXmlDVDAuthor::vob_struct    *pVob    = NULL;
  uint t, i;

  if ( ! pXMLTitleset->ppArrayTitles )  {
    pTitle = pXMLTitleset->addTitles ( );
    addToVideo ( &pTitle->video, pTransition );
  }
  pTitle = pXMLTitleset->ppArrayTitles[0];

  //for ( t=0; t<(uint)pTransition->listSource.size ( ); t++ )  {                   //ooo
  for ( t=0; t<static_cast<uint> ( pTransition->listSource.size ( ) ); t++ )  {     //xxx
    pSource = pTransition->listSource[t];
    if ( ! pSource )
      continue;
    // This check filters out the transition file which has become chapter 1 to the video
    if ( pTransition->pTargetInfo && pSource->iTitle < 1)
      continue;

    pPgc = pTitle->addPgc ( );
    pVob = pPgc->addVob   ( );

    pVob->file     = pTransition->fileName ( t );
    pVob->chapters = "00:00:00.000";
    pVob->pause    = "0";
    pPgc->pause    = "0";

    // Lets add a comment :
    i = 0;
    while ( pTitle->ppArrayPgc[i] )  {
      if  ( pTitle->ppArrayPgc[i++] == pPgc )  {
        pPgc->comment = QString ( "Title %1" ).arg ( i );
        break;
      }
    }
    if ( pTransition->pTargetInfo )  {
      // handling video transition
      if ( pSource->iTitle > 0 )
        pPgc->post.value = QString ( " g3=0; jump title %1 chapter %2; " ).arg ( pSource->iTitle ).arg ( pSource->iChapter );
      else  // Handling error case as best as possible.
        pPgc->post.value = QString ( " g3=0; call menu entry root; " );
    }
    else  {
      // Handling menus.
      if ( pTransition->iJumpIndex != -1 )
        pPgc->post.value = QString ( " g3=%1; call menu entry root; " ).arg ( pTransition->iJumpIndex );
      else  // Handling error case as best as possible.
        pPgc->post.value = QString ( " g3=0; call menu entry root; " );
    }
  }
  return true;
}

QString DVDAuthor::jumpToSubMenu ( SourceFileEntry *pEntry )
{
  // This function will generate the jump command to the DVDMenu 
  // which is associated to the SourceFileEntry.
//  int             iMenu;
  DVDMenu        *pDVDMenu;
  JumpTable      *pJumpTableEntry;
  ExportTitleset *pMenuTitleset, *pEntryTitleset;
  QString qsJump (" { g2 = 0; call vmgm menu entry title; } ");

  pDVDMenu   =  getMenuFromEntry (  pEntry  );
  if ( ! pDVDMenu )
    return qsJump;
  pEntryTitleset  = findTitleset (  pEntry  );
  if ( ! pEntryTitleset )
    return qsJump;
  pMenuTitleset   = findTitleset ( pDVDMenu );
  if ( ! pMenuTitleset )
    return qsJump;
  pJumpTableEntry = pMenuTitleset->findJumpTable ( pDVDMenu );
  if ( ! pJumpTableEntry )
    return qsJump;

//  iMenu  = pMenuTitleset->findMenu ( pDVDMenu );
  if ( pMenuTitleset == pEntryTitleset ) {
    if ( pJumpTableEntry->iMenu == 0 )
      qsJump = QString (" { g3=0; call menu entry root; } ");
    else
      qsJump = QString (" g3=%1; call menu; ").arg ( pJumpTableEntry->iJumpIndex );
  }
  else if ( pMenuTitleset->iTitlesetNumber != 0 )  // VMGM is the default
    qsJump = QString (" { g2=%1; g3=%2; call vmgm menu entry title; } ").
      arg ( pMenuTitleset->iTitlesetNumber ).arg( pJumpTableEntry->iJumpIndex );

  return qsJump;
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::titleset_struct *pXMLTitleset, ExportTitleset *pExpTitleset )
{
  CXmlDVDAuthor::menus_struct *pXMLMenu;
  // TITLESET X
  //	menu entry="root"
  //	<pre>if ( g3 gt 0 ) {
  //	if ( g3 eq 0 ) jump title 1 chapter 2;
  //	if ( g3 eq 1 ) jump title 1 chapter 5;
  //	if ( g3 eq 2 ) jump title 2 chapter 1;
  //	jump title 1; <!-- nothig there, just start the first title // -->
  //	}</pre>
  if ( pExpTitleset->listJumpTable.count ( ) > 0 ) {
    // Note there ought to be only one <menus> - tag.
    // my mistake for not understanding the dvdauthor-XML-spec
    if ( ! pXMLTitleset->ppArrayMenus )
           pXMLTitleset->addMenus   ( );
    pXMLMenu = pXMLTitleset->ppArrayMenus[0];
    return addToMenu ( pXMLMenu, pExpTitleset );
  }
  return true;
}

QString DVDAuthor::getFromFormat ( QString &qsFormat, int iWhich )
{
  // Input is expected to be "720x480x16:9x29.97"
  //QStringList list = QStringList::split ( qsFormat, "x" );	//ooo
  QStringList list = qsFormat.split ( "x" );			//xxx
  QString qsReturn = "720";
  switch ( iWhich )  {
  case 0: qsReturn = "720";   break;
  case 1: qsReturn = "480";   break;
  case 2: qsReturn = "16:9";  break;
  case 3: qsReturn = "29.97"; break;
  };
  if ( list.size ( ) >= 4 )  {
    switch ( iWhich )  {
    case 0: qsReturn = list[0]; break;
    case 1: qsReturn = list[1]; break;
    case 2: qsReturn = list[2]; break;
    case 3: qsReturn = list[3]; break;
    } 
  }
  return qsReturn;
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::menus_struct *pXMLMenu, ExportTitleset *pExpTitleset )
{
  CXmlDVDAuthor::pgc_struct *pPgc;

  QString qsWidth  = getFromFormat ( pExpTitleset->qsFormat, 0 );
  QString qsHeight = getFromFormat ( pExpTitleset->qsFormat, 1 );
  QString qsRatio  = getFromFormat ( pExpTitleset->qsFormat, 2 );
  if ( ( qsWidth.length ( ) > 0 ) && ( qsHeight.length ( ) > 0 ) && ( qsRatio.length ( ) > 0 ) )  {
    QString qsRes = qsWidth + "x" + qsHeight;
    addToVideo ( &pXMLMenu->video, qsRes, qsRatio );
  }
  pPgc = pXMLMenu->addPgc ( );
  return addToMenu ( pPgc, pExpTitleset );
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::pgc_struct *pPgc, ExportTitleset *pExpTitleset )
{
  uint i;
  QString qsPre;

  pPgc->entry=QString ("root");
  qsPre = QString (" {   if ( g3 gt 0 )  {\n");
  //for ( i=0; i<(uint)pExpTitleset->listJumpTable.count ( ); i++ )  {                  //ooo
  for ( i=0; i<static_cast<uint> ( pExpTitleset->listJumpTable.count ( ) ); i++ )  {    //xxx
    JumpTable *p = pExpTitleset->listJumpTable[i];
    if ( ! p ) // error but at least don't crash
      continue;
    if ( p->bAlreadyPresent )
      continue;
    if ( p->iMenu > -1 ) {
      // The following line confuses the xine virtual machine and rejected calling the menu.
      // qsPre += QString ("\t\tif ( g3 eq %1 ) { g3=0; jump menu %2; }\n").arg(p->iJumpIndex).arg(p->iMenu);
      // Note : .arg(p->iMenu+ 1); +1 caused error : ERR:  Cannot jump to menu PGC #3, only 2 exist
      qsPre += QString ("\t\tif ( g3 eq %1 ) jump menu %2; \n").arg(p->iJumpIndex).arg(p->iMenu); 
    }
    else {
      qsPre += QString ("\t\tif ( g3 eq %1 ) { g3=0; jump title %2 ").arg(p->iJumpIndex).arg(p->iTitle);
      if ( p->iChapter < 1 )
        qsPre += QString ( "; }\n" );
      else
        qsPre += QString ( " chapter %1; }\n" ).arg ( p->iChapter );
    }
  }

  qsPre   += QString ("\t\tjump vmgm menu entry title;\n");
  if ( m_bInsertComments )
    qsPre += QString (" <!-- nothing there, just start the first title // -->\n");
  qsPre   += QString ("\t\t}\n\t} ");
  pPgc->pre.value  = qsPre;

  pPgc->post.value = QString (" { g2 = 0; jump vmgm menu entry title; } ");
  return true;
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::titleset_struct *pXMLTitleset, DVDMenu *pMenu )
{
  CXmlDVDAuthor::menus_struct *pXMLMenu;
  //  <menus>
  //   <video format="ntsc" aspect="4:3" resolution="720x480" />
  //   <audio format="mp2" samplerate="48khz" lang="en">
  //   <audio format="mp2" samplerate="48khz" lang="de">
  //   <pgc entry="title" >
  //    <pre> g1=1; g2=1024; g3=1; </pre>
  //    <vob file="/tmp/09/Main Menu VMGM_menu.mpg" pause="inf" />
  //    <button name="01_Button_2" > { g3=2; jump titleset 1 menu entry root; } </button>
  //    <button name="02_Button_1" > { g3=5; jump titleset 1 menu entry root; } </button>
  //    <button name="03_Button_3" > { g3=10; jump titleset 1 menu entry root; } </button>
  //    <post> jump vmgm menu 1; </post>
  //   </pgc>
  //  </menus>
  // Note: the JumpTabel and the buttons of this menu better be in synch

  // The VMGMenu is already taken care of.
  if ( pMenu == Global::pApp->getVMGMenu () )
    return true;

  if ( ! pXMLTitleset->ppArrayMenus )
         pXMLTitleset->addMenus   ( );
  pXMLMenu = pXMLTitleset->ppArrayMenus[0];
  return addToMenu ( pXMLMenu, pMenu );
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::menus_struct *pXMLMenu, DVDMenu *pMenu )
{
  CDVDMenuInterface *pInterface = pMenu->getInterface ( );
  CXmlDVDAuthor::pgc_struct      *pPgc;
  addToVideo ( &pXMLMenu->video, pInterface->iWidth, pInterface->iHeight, pInterface->qsRatio );
  addToAudio ( pXMLMenu, pMenu );

  pPgc = getPgc ( pXMLMenu, pMenu );

  if ( ! pInterface->qsPre.isEmpty  ( ) && pPgc->pre.value.isEmpty  ( ) )
    pPgc->pre.value = pInterface->qsPre;
  if ( ! pInterface->qsPost.isEmpty ( ) && pPgc->post.value.isEmpty ( ) )
    pPgc->post.value = pInterface->qsPost;

  return addToMenu  ( pPgc, pMenu );
}

CXmlDVDAuthor::pgc_struct *DVDAuthor::getPgc ( CXmlDVDAuthor::menus_struct *pXMLMenu, DVDMenu *pMenu )
{
  CDVDMenuInterface *pInterface = pMenu->getInterface ( );
  CXmlDVDAuthor::pgc_struct      *pPgc = NULL;
  int t = 0;

  // Here we ensure that at least one Pgc is there.
  if  (  ! pXMLMenu->ppArrayPgc )
    pPgc = pXMLMenu->addPgc  (  );

  if ( pInterface->iMenu > -1 ) {
    // At this point we need to add PGCs for each missing Menu Numer ( In case the user define Menu=98 )
    for ( t=0; t<=pInterface->iMenu; t++ ) {
      if (  !  pXMLMenu->ppArrayPgc[t] )
	pPgc = pXMLMenu->addPgc     (  );
      else
	pPgc = pXMLMenu->ppArrayPgc[t];
    }
    if ( ( pInterface->iMenu == 0 ) && ( m_bMenuZeroDialog ) ) {
      m_bMenuZeroDialog = false;
      QMessageBox::information ( NULL, QObject::tr ( "MenuNr 0 no allowed." ), QObject::tr ( "You are not allowed to assign MenuNr=\"0\".\nThe Titleset Root menu is used to create the Jump table." ), QMessageBox::Ok, Qt::NoButton );
    }
  }
  else { // iMenu has not been set by the user
    t = 1; // Start at menu # 1 as menu # 0(root) is used as Jumping block
    pPgc = pXMLMenu->ppArrayPgc[0];
    while ( pPgc ) {
      pPgc = pXMLMenu->ppArrayPgc[t++];
      if ( pPgc && ! pPgc->ppArrayVob )
        break;
    }
    if ( ! pPgc )
      pPgc = pXMLMenu->addPgc (  );
  }

  if ( pInterface->qsPost == "Loop" )  {
    pPgc->post.value = QString ( " { g4=button; jump menu %1; } " ).arg ( t );
    // We want to keep track of the highlighted button, to avoid flicker
    pPgc->pre.value = "{ if ( g4 gt 0 ) button=g4; ";
    if ( ! pInterface->qsPre.isEmpty ( ) && pPgc->pre.value.isEmpty ( ) )
      pPgc->pre.value += pInterface->qsPre;
    pPgc->pre.value += "}";
  }

  return pPgc;
}

bool DVDAuthor::addToMenu ( CXmlDVDAuthor::pgc_struct *pPgc, DVDMenu *pMenu )
{
  uint t;
  CXmlDVDAuthor::vob_struct    *pVob;
  CXmlDVDAuthor::button_struct *pXMLButton;
  QList<ButtonObject *>    listButtonsFromMenu = pMenu->getButtons ( );		//oxx
  ButtonObject *pButton    = NULL;
  JumpTable    *pJumpTable = NULL;
  QString qsButtonName;
  Utils   theUtils;

  CDVDMenuInterface *pInterface = pMenu->getInterface ( );
  //for ( t=0; t<(uint)pInterface->listIntroVobs.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( pInterface->listIntroVobs.count ( ) ); t++ )  {  //xxx
    // play only once is handled in the fpc - tag.
    //if((pInterface->qsMenuName == QString (VMGM_LABEL)) && ( pInterface->listIntroVobs[t]->pause == "once"))
    if ( ( ! pInterface->bIsSubMenu ) && ( pInterface->listIntroVobs[t]->pause == "once" ) )
      continue;
    pVob = pPgc->addVob ( );
    pVob->file  = pInterface->listIntroVobs[t]->file;
    pVob->pause = QString ( "0" );
  }

  pVob = pPgc->addVob ( );
  pVob->file=QString  ( "%1_menu.mpg" ).arg ( theUtils.getTempFile ( pMenu->name ( ) ) );
  // since version 0.0.9 we also support movieMenus, so we need to loop here
  // We are now also keeping the last button selected so that after restarting the
  // menu movie the same button is highlighted (using g4 register).
  //for (t=0;t<(uint)pInterface->listExtroVobs.count ( );t++ ) {                //ooo
  for (t=0;t<static_cast<uint> ( pInterface->listExtroVobs.count ( ) );t++ ) {  //xxx
    pVob = pPgc->addVob ( );
    pVob->file  = pInterface->listExtroVobs[t]->file;
    pVob->pause = QString ( "0" );
  }

  if ( pInterface->qsPause.isEmpty ( ) )  {
    if ( pMenu->isMovieMenu ( ) )
      pVob->pause = QString ("0");
    else
      pVob->pause = QString ("inf");
  }
  else
    pVob->pause = pInterface ->qsPause;

  //for ( t=0;t<(uint)listButtonsFromMenu.count ( ); t++ )  {               //ooo
  for ( t=0;t<static_cast<uint> ( listButtonsFromMenu.count ( ) ); t++ )  { //xxx
    pButton = listButtonsFromMenu[ t ];
    qsButtonName = pButton->name  ( );
    qsButtonName.replace ( " ", "_" );
    pXMLButton = pPgc->addButton  ( );
    //pXMLButton->name.sprintf ( "%02d_%s", t+1, qsButtonName.ascii ( ) );		//ooo
    pXMLButton->name.sprintf ( "%02d_%s", t+1, qsButtonName.toLatin1 ( ).data() );	//xxx
    pJumpTable = findJumpTableEntry ( pButton );
    if ( ! pJumpTable ) { // In case I could not make heads or tails out of the action ...
      pXMLButton->value  = QString ("<!-- Could not find associated ButtonObject in JumpTables // -->\n" );
      //if ( pButton->action ( ).find ( STRING_SEPARATOR ) < 0 ) { // this indicates action was created through qdvdauthor. Internal error (missing source/menu) ! ignore !	//ooo
      if ( pButton->action ( ).indexOf ( STRING_SEPARATOR ) < 0 ) { // this indicates action was created through qdvdauthor. Internal error (missing source/menu) ! ignore !	//xxx
        // In here indicates manual changes to the buttons - action tag. We should honor these.
        if ( ! pButton->preAction ( ).isEmpty ( ) ) //
          pXMLButton->value += QString ( " { " ) + getPreAction ( pButton ) + pButton->action ( ) + QString ( " } " );
        else
          pXMLButton->value += pButton->action ( );
        getButtonNumberFromName ( listButtonsFromMenu, pXMLButton->value );
      }
      else if ( pButton->action ( ) == QString ( "resume+-++-+" ) )
        pXMLButton->value = QString ( " { " ) + getPreAction ( pButton ) + QString ( " resume; } " );
    }
    else
      pXMLButton->value = pJumpTable->qsAction;
    // Sorry but for ButtonTransitions to the VMGM menu, we need an overwrite ( exeption handling )
    if ( pButton->getTransition ( ) && pButton->dvdMenu ( ) == Global::pApp->getVMGMenu ( ) )  {
      ButtonTransition *pTransition = findTransition ( Global::pApp->getVMGMenu  ( ), false );
      if ( pTransition )  {
        ExportTitleset  *pVMGMTransitions = m_listTitlesets.last    ( );
        ButtonTransition::Source *pSource = pTransition->findButton ( pButton );
        if ( pSource )  {
          //int iIdx = pTransition->listSource.findIndex ( pSource ) + 1;	//ooo
	  int iIdx = pTransition->listSource.indexOf ( pSource ) + 1;		//xxx
          pXMLButton->value  = QString ( " { " ) + getPreAction ( pButton );
          pXMLButton->value += QString ( " g4=0; g2=0; g3=%1; jump titleset %2 menu entry root; } " ).arg ( iIdx ).arg ( pVMGMTransitions->iTitlesetNumber );
        }
      }
    }
  }
  return true;
}

bool DVDAuthor::addToVideo ( CXmlDVDAuthor::video_struct *pVideo, ButtonTransition *pTransition )
{
  if ( ! pTransition )
    return false;
  if ( pTransition->pTargetMenu )  {
    CDVDMenuInterface *pInterface = pTransition->pTargetMenu->getInterface ( );
    return addToVideo ( pVideo, pInterface->iWidth, pInterface->iHeight, pInterface->qsRatio );
  }
  else if ( pTransition->pTargetInfo )  {
    TranscodeInterface *p = pTransition->pTargetInfo->pTranscodeInterface;
    if ( p )
      return addToVideo ( pVideo, p->qsResolution, p->qsRatio );
    else
      return addToVideo ( pVideo, pTransition->pTargetInfo->qsResolution, pTransition->pTargetInfo->qsRatio );
  }
  return false;
}

bool DVDAuthor::addToVideo ( CXmlDVDAuthor::video_struct *pVideo, QString &qsResolution, QString &qsAspect )
{
  QString qsSupportedAspect ( "4:3" );
  // Currently dvdauthor only supports those two aspect ratios
  if ( qsAspect == "16:9" )
    qsSupportedAspect = "16:9";
  else if ( qsAspect == "4:3" )
    qsSupportedAspect = "4:3";
  QString qsRes = qsResolution;
  //int iWidth, iHeight, iIdx = qsRes.find ( "x" );	//ooo
  int iWidth, iHeight, iIdx = qsRes.indexOf ( "x" );	//xxx
  iWidth  = qsRes.left ( iIdx ).toInt();
  iHeight = qsRes.right( qsRes.length() - iIdx - 1 ).toInt();
  if ( ( iWidth < 1 ) || ( iHeight < 1 ) )
    return false;
  return addToVideo ( pVideo, iWidth, iHeight, qsSupportedAspect );
}

bool DVDAuthor::addToVideo ( CXmlDVDAuthor::video_struct *pXMLVideo, int iWidth, int iHeight, QString &qsAspect)
{
  int iFormat;
  // iFormat > 4 equals PAL, otherwise NTSC
  iFormat = Global::pApp->getVMGMenu()->getFormat (iWidth, iHeight);
  // The first thing we specify is the video settings ...
  if (iFormat > 4)
    pXMLVideo->format   = QString ("pal");
  else
    pXMLVideo->format   = QString ("ntsc");
  pXMLVideo->resolution = QString ("%1x%2").arg(iWidth).arg(iHeight);
  pXMLVideo->aspect     = qsAspect;
  return true;
}

bool DVDAuthor::addToAudio ( CXmlDVDAuthor::menus_struct *pXMLMenu, DVDMenu *pMenu )
{
  CDVDMenuInterface *pInterface = pMenu->getInterface();  
  Audio *pAudio = NULL;
  int    iAudio;

  //for ( unsigned int t=0;t<(uint)pInterface->listSoundEntries.count ( ); t++ ) {                  //ooo
  for ( unsigned int t=0;t<static_cast<uint> ( pInterface->listSoundEntries.count ( ) ); t++ ) {    //xxx
    pAudio = pInterface->listSoundEntries[t];
    if ( pAudio && ( pAudio->iAudioNumber > -1 ) && ( pAudio->iAudioNumber < MAX_AUDIO ) ) {
      iAudio = pAudio->iAudioNumber;
      pXMLMenu->audio[iAudio].lang = pAudio->qsIso639;
    }
  }
  return true;
}

ExportTitleset *DVDAuthor::findTitleset ( int iTitlesetNumber )
{
	uint t;
	//for (t=0;t<(uint)m_listTitlesets.count ();t++)  {                //ooo
        for (t=0;t<static_cast<uint> ( m_listTitlesets.count () );t++)  {   //xxx
		if ( m_listTitlesets[t]->iTitlesetNumber == iTitlesetNumber )
			return m_listTitlesets[t];
	}
	return NULL;
}

ExportTitleset *DVDAuthor::findTitleset ( SourceFileInfo *pInfo )
{
	uint t;
	//for (t=0;t<(uint)m_listTitlesets.count ();t++)  {                //ooo
        for (t=0;t<static_cast<uint> ( m_listTitlesets.count () );t++)  {   //xxx
		if ( m_listTitlesets[t]->findTitle ( pInfo ) != -1 )
			return m_listTitlesets[t];
	}
	return NULL;
}

ExportTitleset *DVDAuthor::findTitleset ( SourceFileEntry *pEntry )
{
	uint t;
	//for (t=0;t<(uint)m_listTitlesets.count ();t++)  {                //ooo
        for (t=0;t<static_cast<uint> ( m_listTitlesets.count () );t++)  {   //xxx
		if ( m_listTitlesets[t]->findTitle ( pEntry ) != -1 )
			return m_listTitlesets[t];
	}
	return NULL;
}

ExportTitleset *DVDAuthor::findTitleset ( DVDMenu *pMenu )
{
        uint t, i;
	QString            qsMenuFormat;
	ExportTitleset    *pTitleset;
	CDVDMenuInterface *pMenuInfo = pMenu->getInterface ( );

	//for ( t=0; t<(uint)m_listTitlesets.count ( ); t++ )  {               //ooo
        for ( t=0; t<static_cast<uint> ( m_listTitlesets.count ( ) ); t++ )  {  //xxx
	  pTitleset = m_listTitlesets[t];
	  //for ( i=0; i<(uint)pTitleset->listMenus.count ( ); i++ ) {                 //ooo
          for ( i=0; i<static_cast<uint> ( pTitleset->listMenus.count ( ) ); i++ ) {    //xxx
	    if ( pTitleset->listMenus[i] == pMenu )
	      return m_listTitlesets[t];
	  }
	}
	// We have to ensure that we return a titleset for this menu ...
	qsMenuFormat = createFormatString ( pMenu );
	//QString ("%1x%2x%3").arg(pMenuInfo->iWidth).arg(pMenuInfo->iHeight).arg(pMenu->fps());
	pTitleset = new ExportTitleset ( this );
	if ( pMenuInfo->iTitleset > -1 )
	  pTitleset->iTitlesetNumber = pMenuInfo->iTitleset;
	else
	  pTitleset->iTitlesetNumber = getFirstEmptyTitleset ( );
	pTitleset->qsFormat          = qsMenuFormat;
	pTitleset->bSetByUser        = false;
	pTitleset->listMenus.append ( pMenu );
	m_listTitlesets.append  ( pTitleset );

	return pTitleset;
}

ExportTitleset *DVDAuthor::findTitleset ( ButtonObject *pButton )
{
  DVDMenu *pMenu = getMenuFromButton ( pButton );
  if ( ! pMenu )
    return NULL;
  return findTitleset ( pMenu );
}

Export::ButtonTransition *DVDAuthor::findTransition ( void *pObject, bool bAsIntro )
{
  ButtonTransition *pTransition = NULL;
  QMap<void *, ButtonTransition *>::iterator it = m_mapTransitionFiles.find ( pObject );
  if ( it != m_mapTransitionFiles.end ( ) )  {
    pTransition = *it;
    if ( bAsIntro && ( pTransition->findChapter ( QString ( "00:00:00.000" ) ) == NULL ) )
      return NULL;
    return pTransition;
  }
  return NULL;
}

PlayMultiple *DVDAuthor::findMultiple ( SourceFileInfo *pInfo, bool bTarget )
{
  uint t;
  SourceFileInfo *pListInfo;
  //for ( t=0; t<(uint)m_listPlayMultiple.count ( ); t++ ) {                //ooo
  for ( t=0; t<static_cast<uint> ( m_listPlayMultiple.count ( ) ); t++ ) {  //xxx
    if ( bTarget )
      pListInfo = m_listPlayMultiple[t]->pTargetInfo;
    else
      pListInfo = m_listPlayMultiple[t]->pSourceInfo;

    if ( pListInfo == pInfo )
      return m_listPlayMultiple[t];
  }
  return NULL;
}

JumpTable *DVDAuthor::findJumpTableEntry ( ButtonObject *pButton )
{
  // This function will find the stored ButtonObject in the JumpTables
  // to obtain qsAction.
  // Note: we can have multiple JumpTableEntries per button if the button 
  //       has the PlayMultiple feature enabled.
  uint t, i;
  //for (t=0;t<(uint)m_listTitlesets.count();t++) {                 //ooo
  for (t=0;t<static_cast<uint> ( m_listTitlesets.count() );t++) {   //xxx
    //for (i=0;i<(uint)m_listTitlesets[t]->listJumpTable.count();i++) {                 //ooo
    for (i=0;i<static_cast<uint> ( m_listTitlesets[t]->listJumpTable.count() );i++) {   //xxx
      if ( ( pButton == m_listTitlesets[t]->listJumpTable[i]->pButton ) && 
	   ( m_listTitlesets[t]->listJumpTable[i]->qsAction.length ( ) > 0 ) )
	return m_listTitlesets[t]->listJumpTable[i];
    }
  }
  return NULL;
}

int DVDAuthor::getHighestTitlesetNumber ()
{
	uint t;
	int iTitlesetNumber = -1;
	//for (t=0;t<(uint)m_listTitlesets.count ();t++)  {                //ooo
        for (t=0;t<static_cast<uint> ( m_listTitlesets.count () );t++)  {   //xxx
		if ( m_listTitlesets[t]->iTitlesetNumber > iTitlesetNumber )
			iTitlesetNumber = m_listTitlesets[t]->iTitlesetNumber;
	}
	return iTitlesetNumber;
}

int DVDAuthor::getFirstEmptyTitleset ()
{
  // This function runs through all m_listTitlesets and finds the lowest number that
  // either does not have any SourceFileInfos 
  // OR is not used (E.g. if titleset is not used but 4 is defined, then we use 3 ...
  uint t;
  int iTitlesetNumber = 1;
  bool bFound = true;

  while ( bFound )  {
    bFound = false;
    //for (t=0;t<(uint)m_listTitlesets.count ();t++)  {                 //ooo
    for (t=0;t<static_cast<uint> ( m_listTitlesets.count () );t++)  {   //xxx
      if ( m_listTitlesets[t]->iTitlesetNumber == iTitlesetNumber ) {
	// Okay, this titleset number already exists, try the next one ...
	iTitlesetNumber++;
	bFound = true;
	break; // out of the for - loop
      }
    }
  }
  if ( ( iTitlesetNumber > 99 ) && ( m_bOutOfSpecDialog ) ) {
    m_bOutOfSpecDialog = false;
    QMessageBox::information ( NULL, QObject::tr ( "Out of Spec." ), QObject::tr ( "Exceeded max number of titlesets ( 99 ).\nCompiling DVD will fail." ), QMessageBox::Ok, QMessageBox::NoButton );
  }
  return iTitlesetNumber;
}

void DVDAuthor::getButtonNumberFromName ( QList <ButtonObject *> &listButtons, QString &qsAction )	//oxx
{
  uint i;
  // replace button="Button Name" with E.g. button=2048
  qsAction.replace ( "\"", "&quot;" ); // this to make sure we catch either way " and &quot; ...
  QRegExp rx ( "button=&quot;.*&quot;" );
  //if ( rx.search ( qsAction ) > -1 ) {	//ooo
  if ( rx.indexIn ( qsAction ) > -1 ) {		//xxx
    QString qsButtonName = rx.cap ( 0 );
    qsButtonName.remove ( "button=" );
    qsButtonName.remove ( "&quot;" );
    for ( i=0; i<(uint)listButtons.count ( ); i++ ) {
      if ( listButtons[i]->name ( ) == qsButtonName ) {
	qsButtonName = QString ( "g4=%1; button=%2" ).arg ( ( i + 1 ) * 1024 ).arg ( ( i + 1 ) * 1024 );
	qsAction.replace ( rx, qsButtonName );
	return;
      }
    }
  }
}

QString DVDAuthor::getPreAction ( ButtonObject *pButton )
{  
  // In this function we replace button="ButtonName" with the actual button number ( * 1024 )
  QString qsPreAction, qsButtonName;
  DVDMenu *pDVDMenu = NULL;
  if ( ! pButton || ! pButton->dvdMenu ( ) )
    return qsPreAction;

  pDVDMenu    = pButton->dvdMenu   ( );
  qsPreAction = pButton->preAction ( );
  if ( ! pDVDMenu )
    return qsPreAction;

  QList<ButtonObject *> list = pDVDMenu->getButtons ( );	//oxx
  QList<ButtonObject *>::iterator it;				//oxx

  QRegExp rx ( "button=\".*\"" );    // primitive floating point matching
  int iIdx, iPos = 0;
  //while ( ( iPos = rx.search ( qsPreAction, iPos ) ) != -1 )  {	//ooo
  while ( ( iPos = rx.indexIn ( qsPreAction, iPos ) ) != -1 )  {	//xxx
    iPos += rx.matchedLength ( );
    qsButtonName = rx.cap   ( 0 );
    if ( qsButtonName.length ( ) > 9 )  {
      qsButtonName = qsButtonName.mid ( 8, qsButtonName.length ( ) - 9 );
      iIdx = 0;
      it   = list.begin ( );
      while ( it != list.end ( ) )  {
        if  ( (*it++)->name ( ) == qsButtonName )
          break;
        iIdx ++;
      }
      qsButtonName = QString ( "button=%1" ).arg ( ++iIdx * 1024 );
    }
    qsPreAction.replace ( rx, qsButtonName );
  }

  return qsPreAction;
}

DVDMenu *DVDAuthor::getMenuFromButton ( ButtonObject *pButton )
{  
  uint               t;
  QString            qsButtonMenuName;
  QStringList        actionList;

  //actionList = QStringList::split (QString (STRING_SEPARATOR), pButton->action() );	//ooo
  actionList = pButton->action().split ( QString (STRING_SEPARATOR) );			//xxx

  if ( actionList.count() < 2 )
    return NULL;
  // A little special handling for VMGM menu jumps ...
  if ( actionList[1] == "vmgm" )
    qsButtonMenuName = VMGM_LABEL;
  else
    qsButtonMenuName = actionList[1];

  //for (t=0;t<(uint)m_listOfAllMenus.count ();t++)  {                  //ooo
  for (t=0;t<static_cast<uint> ( m_listOfAllMenus.count () );t++)  {    //xxx  
    if ( m_listOfAllMenus[t]->name() == qsButtonMenuName )
      return m_listOfAllMenus[t];
  }
  return NULL;
}

DVDMenu *DVDAuthor::getMenuFromEntry ( SourceFileEntry *pEntry )
{
  uint t, i, iSourceAssignedToCounter = 0;
  QList<SourceFileEntry *> listOfMenuEntries;		//oxx
  DVDMenu *pDVDMenu, *pFoundDVDMenu = NULL;

  //for (t=0;t<(uint)m_listOfAllMenus.count ( );t++)  {                 //ooo
  for (t=0;t<static_cast<uint> ( m_listOfAllMenus.count ( ) );t++)  {   //xxx
    pDVDMenu = m_listOfAllMenus[t];
    listOfMenuEntries = pDVDMenu->getSourceFileEntries ( );
    //for ( i=0; i<(uint)listOfMenuEntries.count ( ); i++ ) {               //ooo
    for ( i=0; i<static_cast<uint> ( listOfMenuEntries.count ( ) ); i++ ) { //xxx
      if ( pEntry->qsDisplayName == listOfMenuEntries[i]->qsDisplayName ) {
	iSourceAssignedToCounter ++;
	pFoundDVDMenu = pDVDMenu;
      }
    }
  }
  // Only valid if the SourceFileEntry is assigned to only one menu.
  if ( iSourceAssignedToCounter == 1 )
    return pFoundDVDMenu;
  return NULL;
}

QString DVDAuthor::createFormatString ( SourceFileInfo *pInfo, bool /* bUseRatio */ )
{
  QString qsFormat;
  if ( ! pInfo )
    return qsFormat;

  TranscodeInterface *p = pInfo->pTranscodeInterface;
  // A little self healing. Only two aspects are allowed. If it is not 16:9, it has to be 4:3
  if (  pInfo->qsRatio != "16:9" )
        pInfo->qsRatio  =  "4:3";
  if ( p && p->qsRatio != "16:9" )
       p->qsRatio = "4:3";

  qsFormat = QString ("%1x%2x%3").arg ( pInfo->qsResolution ).arg( pInfo->qsRatio ).arg ( pInfo->qsFPS );
  if ( p )
    qsFormat = QString ("%1x%2x%3").arg( p->qsResolution ).arg( p->qsRatio ).arg ( p->fFrameRate );

  //  printf ( "%s  = %s\n", qsFormat.ascii(), pInfo->qsFileName.ascii() );
  return qsFormat;

  /* older version ...
  if ( bUseRatio ) {
    qsFormat = QString ("%1x%2").arg ( pInfo->qsResolution ).arg( pInfo->qsRatio );
    if ( p )
      qsFormat = QString ("%1x%2").arg( p->qsResolution).arg( p->qsRatio );
  }
  else {
    qsFormat = QString ("%1x%2").arg ( pInfo->qsResolution ).arg( pInfo->qsFPS );
    if ( p )
      qsFormat = QString ("%1x%2").arg( p->qsResolution).arg( (uint)(p->fFrameRate) );
  }
  return qsFormat;
  */
}

QString DVDAuthor::createFormatString ( DVDMenu *pDVDMenu )
{
  QString qsRatio, qsFormat;
  int     iWidth, iHeight;
  float   fFPS;
  iWidth  = pDVDMenu->getInterface ( )->iWidth;
  iHeight = pDVDMenu->getInterface ( )->iHeight;
  qsRatio = pDVDMenu->getInterface ( )->qsRatio;
  fFPS    = pDVDMenu->fps ( );
  if ( iWidth  < 352 )
       iWidth  = 480;
  if ( iHeight < 240 )
       iHeight = 720;
  if ( fFPS < 25.0f )
       fFPS = 29.97f;
  if ( qsRatio != "16:9" )
       qsRatio  =  "4:3";

  qsFormat = QString ("%1x%2x%3x%4").arg ( iWidth ).arg ( iHeight ).arg( qsRatio ).arg ( fFPS );

  //printf ( "%s  = %s\n", qsFormat.ascii(), pDVDMenu->getInterface ( )->qsMenuName.ascii() );
  return qsFormat;
}

bool DVDAuthor::cleanUpSourceFileEntries ( QList<SourceFileEntry *> &listEntries )	//oxx
{
  uint t, i, j;
  bool bFound;
  QString qsFormat1, qsFormat2;
  SourceFileEntry *pEntry, *pNewEntry;
  SourceFileInfo  *pInfo, *pInfo1, *pInfo2;

  //for (t=0;t<(uint)listEntries.count();t++) {                 //ooo
  for (t=0;t<static_cast<uint> ( listEntries.count() );t++) {   //xxx
    pEntry = listEntries[t];
    if ( pEntry->listFileInfos.count ( ) < 2 )
      continue;
    pInfo  = pEntry->listFileInfos[0];
    if ( ! pInfo )
      continue;

    qsFormat1 = createFormatString ( pInfo );
    //for (i=1;i<(uint)pEntry->listFileInfos.count();i++) {                 //ooo
    for (i=1;i<static_cast<uint> ( pEntry->listFileInfos.count() );i++) {   //xxx
      pInfo1 = pEntry->listFileInfos[i];
      if ( ! pInfo1 )
        continue;

      qsFormat2 = createFormatString ( pInfo1 );
      bFound = false;
      // Verify same format.
      if ( qsFormat1 != qsFormat2 ) {
	// We will strip this from the orig SourceFileEntry and put it in a new
	// one. Note: We do not plug it in one of the originals.
	//for (j=0;j<(uint)m_listNewEntries.count();j++) {                  //ooo
        for (j=0;j<static_cast<uint> ( m_listNewEntries.count() );j++) {    //xxx
	  pInfo2 = m_listNewEntries[j]->listFileInfos[0];
	  if ( ! pInfo2 ) 
	    continue;

	  qsFormat2 = createFormatString ( pInfo2 );
	  if ( qsFormat1 == qsFormat2 ) {
	    //pEntry->listFileInfos.remove ( pInfo1 );	//ooo
	    pEntry->listFileInfos.removeOne ( pInfo1 );	//xxx
            // Make sure we only add the info one time to the Entry
            // This could happen if we link from multiple DVDMenus to one video.
            //if ( m_listNewEntries[j]->listFileInfos.contains ( pInfo1 ) < 1 )		//ooo
	    if ( ! m_listNewEntries[j]->listFileInfos.contains ( pInfo1 ) )		//xxx
	         m_listNewEntries[j]->listFileInfos.append   ( pInfo1 );
	    bFound = true;
	    break; // exit out of the inner loop listNewEntries.
	  }
	}
	// In case we did not find a new SourceFileEntry of suitable format
	if ( ! bFound ) {
	  pNewEntry = new SourceFileEntry;
	  // iLOG ("Just created SourceFileEntry <%p>", pNewEntry );
	  //pEntry->listFileInfos.remove    ( pInfo1 );		//ooo
	  pEntry->listFileInfos.removeOne    ( pInfo1 );	//xxx
	  pNewEntry->listFileInfos.append ( pInfo1 );
	  m_listNewEntries.append ( pNewEntry );
	}
      }
    }
  }
  // finally we add all new entries ...
  for ( t=0; t<(uint)m_listNewEntries.count ( ); t++ )
    listEntries.append ( m_listNewEntries[t] );

  return true;
}

QString DVDAuthor::adjustChapterOffset ( QStringList &listChapters, int iOffsetInMSec )
{
  // This will simply subtract the the offset from the chapters.
  // The only thing we have to take care of is that we need the same numer of chapters
  // I.e. if iOffset > then the first few chapters we have to create them
  QString qsChapters;
  QTime timeChapters, zeroTime;
  long  iChapterOffset, iPreviousOffset, iOffset;
  Utils theUtils;
  uint  t;

  iPreviousOffset  = -100;
  //for ( t=0; t<(uint)listChapters.count ( ); t++ )  {                 //ooo
  for ( t=0; t<static_cast<uint> ( listChapters.count ( ) ); t++ )  {   //xxx
    iChapterOffset = theUtils.getMsFromString ( listChapters[t] );
    iOffset = iChapterOffset - iOffsetInMSec;
    if ( iOffset < 0 )
         iOffset = iPreviousOffset + 100;
    timeChapters = zeroTime.addMSecs ( iOffset );

    qsChapters  += timeChapters.toString ( "hh:mm:ss.zzz" ) + ",";
    iPreviousOffset = iOffset;
  }
  return qsChapters;
}

/////////////////////////////////////////////////////////////////
//
// SPUMUX class implementation
//
/////////////////////////////////////////////////////////////////
Spumux::Spumux ()
{
}

Spumux::~Spumux ()
{
}

bool Spumux::createXml (DVDMenu *pMenu)
{ 
  uint t, i;
  QString qsUp, qsDown, qsLeft, qsRight, qsButtonName, qsTransparent;
  CXmlSpumux xmlSpumux;
  CXmlSpumux::spu_struct *pSpu = xmlSpumux.m_subpictures.stream.addSpu();
  CXmlSpumux::button_struct *pXmlButton;
  ButtonObject *pButton;
  Utils theUtil;

  QRgb rgbTransparent = pMenu->getColor ( 0 ).rgb ( ) & 0x00FFFFFF; // filter out the Alpha
  qsTransparent = QString ( "%1" ).arg ( (unsigned int)rgbTransparent, 6, 16 );
  qsTransparent = qsTransparent.replace ( " ", "0" );

  pSpu->start        = QString ( "00:00:00.0" );
  pSpu->end          = QString ( "00:00:00.0" );
  pSpu->force        = QString ( "yes"   );   // force is required for menus.
  pSpu->transparent  = qsTransparent;
  //pSpu->autooutline  = QString ( "infer" );
  //pSpu->autoorder    = QString ( "rows"  );
  pSpu->highlight    = theUtil.getTempFile(pMenu->name()) + QString("/") + QString(HIGHLIGHTED_NAME);
  pSpu->select       = theUtil.getTempFile(pMenu->name()) + QString("/") + QString(SELECTED_NAME);
    // Need to figure this one out ...
  //pSpu->outlinewidth = 5;
  QList<ButtonObject *> listButtons;			//oxx
  QList<QRect>          listButtonRect;			//oxx
  QList<CXmlSpumux::button_struct *> listXmlButtons;	//oxx
  listButtons    = pMenu->getButtons();

  // First thing is to ensure we have a 1:1 relationship between rect and ButtonObjects.
  //for (t=0;t<(uint)listButtons.count();t++)                   //ooo
  for (t=0;t<static_cast<uint> ( listButtons.count() );t++)     //xxx
    listButtonRect.append   ( listButtons[t]->boundingRect ( ) );
  correctOverlappingButtons ( listButtonRect, pMenu->getInterface()->iWidth, pMenu->getInterface()->iHeight );
  //for (t=0;t<(uint)listButtons.count();t++)	{                     //ooo
  for (t=0;t<static_cast<uint> ( listButtons.count() );t++)	{   //xxx
    pXmlButton = pSpu->addButton();
    pXmlButton->x0 = listButtonRect[t].left   ( ); // boundingRect ( )
    pXmlButton->y0 = listButtonRect[t].top    ( );
    pXmlButton->x1 = listButtonRect[t].right  ( );
    pXmlButton->y1 = listButtonRect[t].bottom ( );
    //		pXmlButton->label=QString ("%1").arg(t+1);
    qsButtonName = listButtons[t]->name ();
    qsButtonName.replace (" ", "_");
    //pXmlButton->label.sprintf ("%02d_%s", t+1, qsButtonName.ascii());		//ooo
    pXmlButton->label.sprintf ("%02d_%s", t+1, qsButtonName.toLatin1().data());	//xxx
    listXmlButtons.append(pXmlButton);
  }
  // After we have generated the Xml Buttons and assigned them some nice names,
  // we should connect up/down/left/right.
  //for (t=0;t<(uint)listButtons.count ();t++)	{                    //ooo
  for (t=0;t<static_cast<uint> ( listButtons.count () );t++)	{  //xxx
    pXmlButton = listXmlButtons[t];	// Ought to be same number as listButtons.count()
    pButton = listButtons[t];
    //for (i=0;i<(uint)listButtons.count();i++)	{                   //ooo
    for (i=0;i<static_cast<uint> ( listButtons.count() );i++)	{ //xxx
      // Here we find the button label which fits the button. 
      // I.e. we associate the label in the xml file to the label in the ButtonObject
      qsUp    = pButton->next(NEXT_BUTTON_UP);
      qsDown  = pButton->next(NEXT_BUTTON_DOWN);
      qsRight = pButton->next(NEXT_BUTTON_RIGHT);
      qsLeft  = pButton->next(NEXT_BUTTON_LEFT);
      if ( ( ! qsUp.isEmpty ( ) ) && ( qsUp != QString ("-- default --")) && 
             (qsUp    == listButtons[i]->name()) ) {
        qsButtonName = qsUp.replace (" ", "_");
        //pXmlButton->up.sprintf ("%02d_%s", i+1, qsButtonName.ascii());		//ooo
        pXmlButton->up.sprintf ("%02d_%s", i+1, qsButtonName.toLatin1().data());	//xxx
      }
      if ( ( qsDown.isEmpty ( ) ) && ( qsDown  != QString ("-- default --")) && 
           (qsDown  == listButtons[i]->name()) ) {
        qsButtonName = qsDown.replace (" ", "_");
        //pXmlButton->down.sprintf ("%02d_%s", i+1, qsButtonName.ascii());		//ooo
        pXmlButton->down.sprintf ("%02d_%s", i+1, qsButtonName.toLatin1().data());	//xxx
      }
      if ( ( qsLeft.isEmpty ( ) ) && ( qsLeft  != QString ("-- default --")) && 
           (qsLeft  == listButtons[i]->name()) ) {
        qsButtonName = qsLeft.replace (" ", "_");
        //pXmlButton->left.sprintf ("%02d_%s", i+1, qsButtonName.ascii());		//ooo
        pXmlButton->left.sprintf ("%02d_%s", i+1, qsButtonName.toLatin1().data());	//xxx
      }
      if ( ( qsRight.isEmpty ( ) ) && ( qsRight != QString ("-- default --")) && 
           (qsRight == listButtons[i]->name()) ) {
        qsButtonName = qsRight.replace (" ", "_");
        //pXmlButton->right.sprintf ("%02d_%s", i+1, qsButtonName.ascii());		//ooo
        pXmlButton->right.sprintf ("%02d_%s", i+1, qsButtonName.toLatin1().data());	//xxx
      }
    }
  }
  QString fileSpumux = theUtil.getTempFile(pMenu->name()) + QString ("/menu.xml");
    
  xmlSpumux.writeXml (fileSpumux);
  return true;
}

bool Spumux::correctOverlappingButtons ( QList<QRect> &listOfRect, int iWidth, int iHeight )
{
  uint   t, i;
  int    iDelta;
  QRect *pRect1, *pRect2, intersection;
  
  // None or only one button, return empty 
  if ( listOfRect.count() < 1 )
    return false;

  // First dvdauthor has something agains odd lines. So this one needs fixing first.
  //for (t=0;t < (uint)listOfRect.count();t++) {                //ooo
  for (t=0;t < static_cast<uint> ( listOfRect.count() );t++) {  //xxx
    pRect1 = & listOfRect[t];
    if ( pRect1->top ( ) % 2 != 0 )
      pRect1->setTop ( pRect1->top ( ) + 1 );
    if ( pRect1->bottom ( ) % 2 != 0 )
      pRect1->setBottom ( pRect1->bottom ( ) + 1 );
  }
  //for (t=0;t < (uint)listOfRect.count();t++) {                //ooo
  for (t=0;t < static_cast<uint> ( listOfRect.count() );t++) {  //xxx
    pRect1 = & listOfRect[t];
    // First we check out-of-bounds
    if ( pRect1->top    () < 0 )
         pRect1->setTop    ( 1 );
    if ( pRect1->left   () < 0 )
         pRect1->setLeft   ( 1 );
    if ( ( iWidth  > 1 ) && ( pRect1->right  () > iWidth ) )
         pRect1->setRight  ( iWidth  - 1 );
    if ( ( iHeight > 1 ) && ( pRect1->bottom () > iHeight ) )
         pRect1->setBottom ( iHeight - 1 );

    //for (i=t+1;i < (uint)listOfRect.count ();i++) {               //ooo
    for (i=t+1;i < static_cast<uint> ( listOfRect.count () );i++) { //xxx
      pRect2  =  & listOfRect[i];

      if ( pRect1->intersects ( *pRect2 ) ) {
	//intersection = pRect1->intersect ( *pRect2 );		//ooo
	intersection = pRect1->intersected ( *pRect2 );		//xxx
	
	// here we determine if the intersection is more long than tall
	if ( intersection.width ( ) > intersection.height ( ) ) {
	  // we subtract the delta from the top / bottom of the intersection rectangles.
	  iDelta = (int) ( ( intersection.height ( ) + 1 ) / 2.0 );
	  // ensure that the button which is higher to be rect1
	  if ( pRect1->top ( ) > pRect2->top ( ) ) {
	    pRect1 = & listOfRect [ i ];
	    pRect2 = & listOfRect [ t ];
	  }
	  // Now we subtract the delta from button1's bounding Rect ...
	  pRect1->setBottom ( pRect1->bottom ( ) - iDelta );
	  pRect2->setTop    ( pRect2->top    ( ) + iDelta );
	}
	else {
	  // we subtract the delta from the left / right of the intersection rectangles.
	  iDelta = (int) ( ( intersection.width ( ) + 1 ) / 2.0 );
	  // ensure that the button which is closer to the left button1
	  if ( pRect1->left () > pRect2->left ( ) ) {
	    pRect1 = & listOfRect [ i ];
	    pRect2 = & listOfRect [ t ];
	  }

	  pRect1->setRight ( pRect1->right ( ) - iDelta );
	  pRect2->setLeft  ( pRect2->left  ( ) + iDelta );
	}
      }
      // Ensure pRect1 is the button we expect it to be.
      pRect1 = & listOfRect [ t ];
    }
  }
  return true;
}


// The following function takes care of creating the spumux file for subtitles.
//<subpictures>
//   <stream>
//      <textsub filename="demo1.srt" characterset="ISO8859-1" 
//         fontsize="28.0" font="arial.ttf" horizontal-alignment="left" 
//         vertical-alignment="bottom" left-margin="60" right-margin="60" 
//         top-margin="20" bottom-margin="30" subtitle-fps="25" 
//         movie-fps="25" movie-width="720" movie-height="574" />
//   </stream>
//</subpictures>
///////////////////////////////////////////////////////////////////////////////////
bool Spumux::createXml ( SourceFileInfo *pInfo )
{
  if ( ! pInfo )
    return false;

  int t;
  Subtitles *pSubtitles;
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    pSubtitles = pInfo->arraySubtitles [ t ];
    if ( ! pSubtitles )
      continue;
    if ( ! createXml ( pInfo, pSubtitles ) )
      return false;
  }
  return true;
}

bool Spumux::createXml ( SourceFileInfo *pInfo, Subtitles *pSubtitles )
{
  if ( pSubtitles->m_bTextSubtitles )
    return createTextXml  ( pInfo, pSubtitles );
  else
    return createImageXml ( pInfo, pSubtitles );
}

bool Spumux::createTextXml ( SourceFileInfo *pInfo, Subtitles *pSubtitles )
{
  if ( ! pInfo )
    return false;

  if ( ! pSubtitles )
    return true;

  // Okay at this point we have all the info we need
  Utils      theUtil;
  CXmlSpumux xmlSpumux;
  Export::Srt srtFile;
  QFileInfo fileInfo ( pInfo->qsFileName );
  QString   fileSpumux, fileSrt, qsSubIdx;
  CXmlSpumux::textsub_struct *pTextsub = xmlSpumux.m_subpictures.stream.addTextsub ();

  // next is to save the srt - file ...
  fileSrt = theUtil.getTempFile( fileInfo.baseName () ) + QString ("/subtitles_%1.srt").arg ( pSubtitles->m_iSubtitleNumber, 2 );
  srtFile.writeFile ( pSubtitles, fileSrt );

  pTextsub->filename             = fileSrt;
  pTextsub->characterset         ="UTF-8";
  pTextsub->fontsize             = pSubtitles->m_iTextFontSize; //m_font.pixelSize ( );
  // note: a link has to exist in ~/.spumux/fontFamily to its actual location.
  pTextsub->font                 = pSubtitles->m_qsTextFont; //m_font.family    ( ); 
  pTextsub->subtitle_fps         = pInfo->qsFPS.toFloat ();
  pTextsub->movie_fps            = pInfo->qsFPS.toFloat ();
  pTextsub->movie_width          = theUtil.getWHFromResolution ( pInfo->qsResolution, true );
  pTextsub->movie_height         = theUtil.getWHFromResolution ( pInfo->qsResolution, false );
  // simple sanity check ...
  if ( pTextsub->movie_width < 350 )
    pTextsub->movie_width = 720;
  if ( pTextsub->movie_height < 240 ) {
    pTextsub->movie_height = 480;
    if ( pInfo->qsVideoFormat == QString ( "PAL" ) )
      pTextsub->movie_height = 576;
  }
  pTextsub->horizontal_alignment   = "center"; // "left" - "right"
  pTextsub->vertical_alignment     = "center"; // "top" - "bottom"
  if ( pSubtitles->m_rect.width () > 5 ) {
    //pTextsub->left_margin   = (int)((float)(pTextsub->movie_width - pSubtitles->m_rect.width   ( ) / 2.0 ) );                         //ooo
    pTextsub->left_margin   = static_cast<int>(static_cast<float>(pTextsub->movie_width - pSubtitles->m_rect.width   ( ) / 2.0 ) );     //xxx
    pTextsub->right_margin  = pTextsub->left_margin;
  }
  if ( pSubtitles->m_rect.height () > 5 ) {
    //pTextsub->top_margin    = (int)((float)(pTextsub->movie_height - pSubtitles->m_rect.height ( ) / 2.0 ) );                         //ooo
    pTextsub->top_margin    = static_cast<int>(static_cast<float>(pTextsub->movie_height - pSubtitles->m_rect.height ( ) / 2.0 ) );     //xxx
    pTextsub->bottom_margin = pTextsub->top_margin;
  }
  // and lastly we save the xml file.
  qsSubIdx.sprintf ( "/subtitle_%d.xml", pSubtitles->m_iSubtitleNumber );
  fileSpumux = theUtil.getTempFile( fileInfo.baseName ( ) ) + qsSubIdx;
  xmlSpumux.writeXml ( fileSpumux );

  return true;
}

bool Spumux::createImageXml ( SourceFileInfo *pInfo, Subtitles *pSubtitles )
{
  if ( ! pInfo ) 
    return false;

  if ( ! pSubtitles )
    return true;

  // Okay at this point we have all the info we need
  int t, iWidth, iHeight;
  Utils       theUtil;
  CXmlSpumux  xmlSpumux;
  QString     thePath, fileSpumux, qsStart, qsSubIdx;
  Export::Srt srtFile;
  QFileInfo fileInfo ( pInfo->qsFileName );

  CXmlSpumux::spu_struct *pSpu;
  Subtitles::entry *pEntry;
  qsSubIdx.sprintf ( "/sub%02d_", pSubtitles->m_iSubtitleNumber );
  thePath = theUtil.getTempFile( fileInfo.baseName ( ) ) + qsSubIdx;

  iWidth  = theUtil.getWHFromResolution ( pInfo->qsResolution, true  );
  iHeight = theUtil.getWHFromResolution ( pInfo->qsResolution, false );

  for (t=0;t< (int)pSubtitles->m_listOfSubtitles.count ( ); t++) {
    pEntry = pSubtitles->m_listOfSubtitles[t];
    pSpu   = xmlSpumux.m_subpictures.stream.addSpu ( );
    qsStart           = pEntry->qsTimeStart;
    qsStart.remove ( ":"      );
    qsStart.replace( ".", "_" );

    pSpu->start   = pEntry->qsTimeStart;
    pSpu->end     = pEntry->qsTimeStop;
    // disturbs when having transparency completely opaque.
    //  pSpu->transparent = pSubtitles->subColors[0].name ( );
    //  pSpu->transparent.remove ( "#" );

    pSpu->image   = QString ("%1%2.png").arg ( thePath ).arg( qsStart );
    pSpu->force   = "no";
    // Here we do some trickery .If the rect is NULL OR it is the same size as the Movie, then we do our own thingy here.
    pSpu->xoffset = pEntry->rect.x ( );
    pSpu->yoffset = pEntry->rect.y ( );
    if ( ! pSubtitles->m_bFit )  {
      if ( ( pEntry->rect.x     ( ) == 0 ) && ( pEntry->rect.y      ( ) == 0 ) &&
           ( pEntry->rect.width ( ) == 0 ) && ( pEntry->rect.height ( ) == 0 ) )  {
           pSpu->yoffset = iHeight - 45;
           pSpu->xoffset = (int)( (float)iWidth / 2.0 );
      }
    }
    else  {
      // The rect contains the actual width/height of the subtitle
      // Here we set xoffse/yoffset according to the m_alignment
      // x=1==Center / x=2==Left    / x=3==Right / x=4==Justify
      // y=1==Bottom / y=2==VCenter / y=3==top   / y=4==Bottom
      if ( pSubtitles->m_alignment.x ( ) == 1 ) // HorizCenter
        pSpu->xoffset = (int)( ( iWidth - pEntry->rect.width ( ) ) / 2.0 );
      else if ( pSubtitles->m_alignment.x ( ) == 3 ) // HorizRight
        pSpu->xoffset = iWidth - pEntry->rect.width ( );
      if ( pSubtitles->m_alignment.y ( ) == 1 ) // VertBottom
        pSpu->yoffset = iHeight - pEntry->rect.height ( );
      else if ( pSubtitles->m_alignment.y ( ) == 2 ) // VertCenter
        pSpu->yoffset = (int)( ( iHeight - pEntry->rect.height ( ) ) / 2.0 );
      else if ( pSubtitles->m_alignment.y ( ) == 4 ) // VertBottom
        pSpu->yoffset = iHeight - pEntry->rect.height ( );
      pSpu->xoffset += pSubtitles->m_rect.x ( );
      pSpu->yoffset += pSubtitles->m_rect.y ( );
      // Sanity check
      if ( pSpu->xoffset + pEntry->rect.width  ( ) > iWidth  )
           pSpu->xoffset = iWidth  - pEntry->rect.width  ( );
      if ( pSpu->yoffset + pEntry->rect.height ( ) > iHeight )
           pSpu->yoffset = iHeight - pEntry->rect.height ( );
    }
  }  // end for loop

  // and lastly we save the xml file.
  qsSubIdx.sprintf ( "/subtitle_%d.xml", pSubtitles->m_iSubtitleNumber );
  fileSpumux = theUtil.getTempFile( fileInfo.baseName ( ) ) + qsSubIdx;
  xmlSpumux.writeXml ( fileSpumux );
  return true;
}

bool Spumux::createXml ( )
{
  return false;
}

JumpTable::JumpTable ()
{
  iJumpIndex = iTitle = iChapter = iMenu = -1;
  bAlreadyPresent = false;
  pButton = NULL;
  pMenu   = NULL;
}

ExportTitleset::ExportTitleset ( DVDAuthor *pExpDVDAuthor )
{
  bSetByUser       = false;
  iTitlesetNumber  = -1;
  pExportDVDAuthor = pExpDVDAuthor;
}

ExportTitleset::~ExportTitleset ()
{
  uint t;
  //for (t=0;t<(uint)listJumpTable.count();t++)                 //ooo
  for (t=0;t<static_cast<uint> ( listJumpTable.count() );t++)   //xxx
    delete listJumpTable[t];
}

int ExportTitleset::findTitle ( ButtonTransition *pTransition, ButtonObject *pButton )
{
  //int  iIdx  = listButtonTransitions.findIndex ( pTransition );	//ooo
  int  iIdx  = listButtonTransitions.indexOf ( pTransition );		//xxx
  if ( iIdx == -1 )  {
    listButtonTransitions.append ( pTransition );
    iIdx = listButtonTransitions.size ( );
  }
  if ( pButton )  {
    // If the button is specified we should get the right Title
    iIdx = 0;
    ButtonTransition *pTrans = NULL;
    QList<ButtonTransition *>::iterator it = listButtonTransitions.begin ( );		//oxx
    while ( it != listButtonTransitions.end ( ) )  {
      pTrans = *it++;
      iIdx  += pTrans->calcTitleIdx ( pButton );
      if ( pTrans == pTransition )
        break;
    }
  }
  if ( listSourceFileEntries.count ( ) > 0 )  {
    SourceFileInfo    *pInfo = listSourceFileEntries.last ( )->listFileInfos.last ( );
    return findTitle ( pInfo ) + iIdx;
  }
  return iIdx;
}
/*
int ExportTitleset::findTitle ( ButtonTransition *pTransition )
{
  int  iIdx  = listButtonTransitions.findIndex ( pTransition );
  if ( iIdx == -1 )  {
    listButtonTransitions.append ( pTransition );
    iIdx = listButtonTransitions.size ( );
  }
  if ( listSourceFileEntries.count ( ) > 0 )  {
    SourceFileInfo    *pInfo = listSourceFileEntries.last ( )->listFileInfos.last ( );
    return findTitle ( pInfo ) + iIdx;
  }
  return iIdx;
}
*/
int ExportTitleset::findTitle ( SourceFileInfo *pInfo )
{
  uint t, i, iTitleCount;

  iTitleCount = 0;
  //for ( t=0; t<(uint)listSourceFileEntries.count ();t++)  {               //ooo
  for ( t=0; t<static_cast<uint> ( listSourceFileEntries.count () );t++)  { //xxx
    //for (i=0;i<(uint)listSourceFileEntries[t]->listFileInfos.count();i++) {                   //ooo
    for (i=0;i<static_cast<uint> ( listSourceFileEntries[t]->listFileInfos.count() );i++) {     //xxx
      if ( listSourceFileEntries[t]->listFileInfos[i] == pInfo ) {
        // Okay we found the info - object
        if ( checkForPrePostCommands ( listSourceFileEntries[t], true ) )
          iTitleCount += i+1;
        else
          iTitleCount ++;
        return iTitleCount;
      }
    }
    // If we find post/pre commands we will generate a title per video object (info)
    if ( checkForPrePostCommands ( listSourceFileEntries[t] ) )
      iTitleCount += listSourceFileEntries[t]->listFileInfos.count ();
    else // otherwise the whole entry is only one title
      iTitleCount ++;
  }
  // error if we get here but to keep things somewhat working, we should return 1
  return -1;
}

int ExportTitleset::findTitle ( SourceFileEntry *pEntry )
{
  uint t, iTitleCount;

  iTitleCount = 0;
  //for ( t=0; t<(uint)listSourceFileEntries.count ();t++)  {               //ooo
  for ( t=0; t<static_cast<uint> ( listSourceFileEntries.count () );t++)  { //xxx
    if ( listSourceFileEntries[t]->qsDisplayName == pEntry->qsDisplayName ) {
      iTitleCount ++;
      return iTitleCount;
    }

    // If we find post/pre commands we will generate a title per video object (info)
    if ( checkForPrePostCommands ( listSourceFileEntries[t] ) )
      iTitleCount += listSourceFileEntries[t]->listFileInfos.count ();
    else // otherwise the whole entry is only one title
      iTitleCount ++;
  }
  // error if we get here but to keep things somewhat working, we should return 1
  return -1;
}

int ExportTitleset::findMenu ( DVDMenu *pMenu )
{
  uint t;
  if ( pMenu->name() == VMGM_LABEL )
    return 1;

  //for ( t=0; t<(uint)listMenus.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( listMenus.count ( ) ); t++ )  {  //xxx
    if ( listMenus[t] == pMenu ) {
      if ( pMenu->getInterface ( )->iMenu > -1 )
        return pMenu->getInterface ( )->iMenu + 1;
      return t+2;
    }
  }
  return 1;
}

JumpTable *ExportTitleset::findJumpTable ( DVDMenu *pDVDMenu )
{
  uint t;
  if ( pDVDMenu ) {
    //for ( t=0; t<(uint)listJumpTable.count ( ); t++ ) {                   //ooo
    for ( t=0; t<static_cast<uint> ( listJumpTable.count ( ) ); t++ ) {     //xxx
      if ( listJumpTable[t]->pMenu == pDVDMenu )
        return listJumpTable[t];
    }
  }
  return NULL;
}

// This function will simply add all chapters of all existing JumpTableEntries
int ExportTitleset::getTotalChapters ( SourceFileInfo *pSourceFileInfo )
{
  if ( ! pSourceFileInfo )
    return 0;

  SourceFileInfo *pInfo;
  uint t, i, iTemp, iChapters = 0;
  // Note: as long as the format is the same we do not create a new PGC per SourceFileEntry ...
  //for ( t=0; t<(uint)listSourceFileEntries.count ();t++)  {               //ooo
  for ( t=0; t<static_cast<uint> ( listSourceFileEntries.count () );t++)  { //xxx
    // different handling if we find pre post commands
    //    if ( checkForPrePostCommands ( listSourceFileEntries[t] ) )
      iChapters = 0;

    //for (i=0;i<(uint)listSourceFileEntries[t]->listFileInfos.count();i++) {                   //ooo
    for (i=0;i<static_cast<uint> ( listSourceFileEntries[t]->listFileInfos.count() );i++) {     //xxx
      pInfo  = listSourceFileEntries[t]->listFileInfos[i];
      if ( pInfo == pSourceFileInfo ) {
	// if we find the info-object in a entry which has pre/post commands
	// then create a pgc per info, thus we return 0.

	// Note changed 20060922 if multiple SourceFileInfos in one Entry and a button jumps to not the first
	//if ( checkForPrePostCommands ( listSourceFileEntries[t] ) )

	if ( checkForPrePostCommands ( listSourceFileEntries[t], true ) )
	  return 0;
	else
	  return iChapters;
      }
      iTemp = pInfo->listChapters.count ( );
      if ( pExportDVDAuthor->findTransition ( (void *)pInfo ) )
           iTemp++;
      // if no chapters are defined, then we'll have to add one to stay in sync.
      //iTemp      = ( iTemp < 1 ) ? 1 : iTemp;
      iChapters += iTemp;
    }
  }

  return 0;
}

int ExportTitleset::getChapters ( SourceFileInfo *pInfo, QString qsTargetAction )
{
  uint t, iChapterNumber = 1;
  QStringList actionList;

  //actionList = QStringList::split ( STRING_SEPARATOR, qsTargetAction );	//ooo
  actionList = qsTargetAction.split ( STRING_SEPARATOR );		//xxx
  if ( actionList.count() < 1 )
    return iChapterNumber;

  if ( pInfo->listChapters.count () > 0 ) {
    //for ( t=0;t<(uint)pInfo->listChapters.count ();t++ ) {                    //ooo
    for ( t=0;t<static_cast<uint> ( pInfo->listChapters.count () );t++ ) {      //xxx
      if (pInfo->listChapters[t] == actionList[actionList.count()-1] ) {  // the chapter
        iChapterNumber += t;
        break;
      }
    }
  }
  return iChapterNumber;
}

bool ExportTitleset::checkForPrePostCommands ( SourceFileEntry *pEntry, bool bIgnoreFirstLast )
{
  uint i;
  // bIgnoreFirstLast will ignore the pre for the first SourceFileInfo
  // and the post command for the last SourceFileInfo.

  uint iCount = pEntry->listFileInfos.count();
  SourceFileInfo *pInfo     = NULL;
  //PlayMultiple   *pMultiple = NULL;
  for ( i=0; i<iCount; i++ ) {
    pInfo = pEntry->listFileInfos[i];

    if ( bIgnoreFirstLast )  {
      // check first SourceFileInfo
      if ( ( i == 0 ) && ( ! pInfo->qsPostCommand.isEmpty () ) )
	return true;

      // check last SourceFileInfo
      else if ( ( i == iCount - 1 ) && ( ! pInfo->qsPreCommand.isEmpty  () ) )
	return true;
      else if ( ( i != 0 ) && ( i != iCount-1 ) && (
		( ! pInfo->qsPreCommand.isEmpty  ( ) ) ||
		( ! pInfo->qsPostCommand.isEmpty ( ) ) ) )
	return true;
    } // if first and last are not to be ignored
    else if ( ( ! pInfo->qsPreCommand.isEmpty  ( ) ) || 
	      ( ! pInfo->qsPostCommand.isEmpty ( ) ) )
      return true;
    if ( ( pExportDVDAuthor ) && ( pExportDVDAuthor->findMultiple ( pInfo, false ) ) )
      return true;
  }

  return false;
}

PlayMultiple::PlayMultiple ( SourceFileInfo *pInfo1, SourceFileInfo *pInfo2 ) 
{
  pSourceInfo = pInfo1;
  pTargetInfo = pInfo2;
  bLoop = false; 
};

void PlayMultiple::addPlayMultiple ( QString &qsPlayEntry )
{
  qsPost += qsPlayEntry + "\n";
}

QString PlayMultiple::getPostCommand ( )
{
  QString qsPlayMultiple;
  if ( qsPost.isEmpty ( ) )
    return QString::null;
  qsPlayMultiple  = " if ( g5 gt 0 )  {\n";
  qsPlayMultiple += qsPost;
  qsPlayMultiple += "\t\tcall vmgm menu entry title;\n";
  qsPlayMultiple += "\t}";

  return qsPlayMultiple;
}

ButtonTransition::Source::Source ( )
{
  pSourceButton = NULL;
  pSourceMenu   = NULL;
  iTitle        =   -1;
  iChapter      =   -1;
}

ButtonTransition::ButtonTransition ( SourceFileInfo *pInfo, DVDMenu *pMenu )
{
  iJumpIndex  = -1;
  pTargetMenu = pMenu;
  pTargetInfo = pInfo;
}

ButtonTransition::~ButtonTransition ( )
{
  QList<Source *>::iterator it = listSource.begin ( );		//oxx
  while ( it != listSource.end ( ) )
    delete *it++;
  listSource.clear ( );
}

void ButtonTransition::addSource ( ButtonObject *pButton, DVDMenu *pMenu, QString qsFileName )
{
  Source *pSource               = new Source;
  pSource->pSourceButton        = pButton;
  pSource->pSourceMenu          = pMenu;
  pSource->qsTransitionFileName = qsFileName;
  // Next we need to extract the chapter
  //QStringList list = QStringList::split ( STRING_SEPARATOR, pButton->action ( ) );	//ooo
  QStringList list = pButton->action ( ).split (  STRING_SEPARATOR );		//xxx
  if ( ( list.size ( ) == 3 ) || ( list.size ( ) == 4 ) )
    pSource->qsChapter  = list.last ( );

  listSource.append ( pSource );
}

int ButtonTransition::calcTitleIdx ( ButtonObject *pButton )
{
  int iTitleCount = 0;
  Source *pSource, *pButtonSource = findButton ( pButton );
  QList<Source *>::iterator it = listSource.begin ( );		//oxx
  while ( it != listSource.end ( ) )  {
    pSource = *it++;
    if ( pSource->iTitle > -1 )
      iTitleCount ++;
    if ( pSource == pButtonSource )
      break;
  }
  return iTitleCount;
}

ButtonTransition::Source *ButtonTransition::findButton ( ButtonObject *pButton )
{
  ButtonTransition::Source *pSource = NULL;
  QList<ButtonTransition::Source *>::iterator it = listSource.begin ( );	//oxx
  while ( it != listSource.end ( ) )  {
    pSource = *it++;
    if ( pSource->pSourceButton == pButton )
      return pSource;
  }

  return NULL;
}

ButtonTransition::Source *ButtonTransition::findChapter ( QString qsChapter )
{
  ButtonTransition::Source *pSource = NULL;
  QList<ButtonTransition::Source *>::iterator it = listSource.begin ( );	//oxx
  while ( it != listSource.end ( ) )  {
    pSource = *it++;
    if ( pSource->qsChapter == qsChapter )
      return pSource;
  }
  return NULL;
}

QString ButtonTransition::fileName ( int iIdx )
{
  QString qsFileName;
  if ( listSource.size ( ) < 1 )
    return qsFileName;
  return listSource[iIdx]->qsTransitionFileName;
}

}; // End of namespace Export
