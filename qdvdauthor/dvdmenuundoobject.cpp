/***************************************************************************
    dvdmenuundoobject.cpp
                             -------------------
    DVDMenuUndoObject - code
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <qpixmap.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QMessageBox>		//xxx

#include "global.h"
#include "xml_dvd.h"
#include "dvdmenu.h"
#include "qdvdauthor.h"
#include "imageobject.h"	//ooo
#include "menupreview.h"
#include "buttonobject.h"
#include "sourcefileentry.h"
#include "selectionobject.h"
#include "dvdmenuundoobject.h"

DVDMenuUndoObject::DVDMenuUndoObject ( )
{
  initMe ( );
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, MenuObject *pMenuObject, ButtonPreview *pButtonPreview, int iTwiggle )
{
  initMe ( );
  m_iTwiggle       = iTwiggle; // Used for moving with cursor keys.
  m_enUndoType     = (int)type;
  m_pMenuObject    = pMenuObject;
  m_pButtonPreview = pButtonPreview;
  switch ( m_enUndoType )  {
  case MOVED_OBJECT :
    m_rectPos = pMenuObject->rect ( );
    if (  pMenuObject->objectType ( ) == SELECTION_OBJECT )  {
      SelectionObject *pSelection = (SelectionObject *)pMenuObject;
      int t, iCount = pSelection->getCount ( );
      for ( t=0; t<iCount; t++ )
        m_listOfObjects.append ( pSelection->getObject ( t ) );
    }
  break;
  case DELETED_OBJECT :
    m_bDeleteObject = true;
    m_pMenuObject->resetStructureItem ( );
  break;
  }
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, MenuObject *pMenuObject, MenuObject *pShadow )
{
  initMe ( );
  m_enUndoType      = (int)type;
  m_pMenuObject     = pMenuObject;

  if ( ( m_enUndoType == NEW_BUTTON     ) ||
       ( m_enUndoType == UNBUTTON       ) ||
       ( m_enUndoType == DRAGGED_IMAGE  ) ||
       ( m_enUndoType == DRAGGED_BUTTON )  )  {
    m_pNormalObject = pShadow;
  }
  else  {  // Note, Shadow or Overlay objects are handled the same.
    m_pShadowObject = pShadow ? pShadow->clone ( pMenuObject->parent ( ), pMenuObject ) : NULL;
    m_bDeleteObject = true;
  }
  if ( ( m_enUndoType == EDIT_OBJECT     ) ||
       ( m_enUndoType == DELETED_SHADOW  ) ||
       ( m_enUndoType == DELETED_OVERLAY ) )  {
    m_bDeleteObject = true;
    m_pMenuObject->resetStructureItem ( );
  }
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, QList<MenuObject *> &list )		//oxx
{
  initMe ( );
  m_enUndoType    = (int)type;
  m_listOfObjects = list;
}

DVDMenuUndoObject::DVDMenuUndoObject ( MenuObject *pMenuObject, MenuObject::enState state )
{
  initMe ( );
  m_enUndoType     = (int)EDIT_OBJECT;
  m_objectState    = state;
  m_pMenuObject    = pMenuObject;
  m_pObjectData    = pMenuObject->data ( )->clone ( );
  m_bDeleteObject  = true;
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, QPixmap &theOldBackground, CDVDMenuInterface *pInterface )
{
  initMe ( );
  if ( type != NEW_BACKGROUND )
    return;
  
  m_enUndoType = (int)type; 
  m_backgroundInfo.background = theOldBackground;		//ooo
  m_backgroundInfo.getBackgroundInfo ( pInterface );
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, DVDMenu *pMenu )
{
  initMe ( );

  m_enUndoType        = (int)type; // could be EDIT_MENU
  m_pDVDMenuInterface =  new CDVDMenuInterface;
 *m_pDVDMenuInterface = *pMenu->getInterface ( );
}

DVDMenuUndoObject::DVDMenuUndoObject ( enUndoType type, SourceFileEntry *pSourceFileEntry )
{
  initMe ( );
  if ( type != NEW_AUDIO )
    return;
  m_enUndoType       = (int)type;
  m_pSourceFileEntry = pSourceFileEntry;
}

DVDMenuUndoObject::~DVDMenuUndoObject ( )
{
  // Okay if we have a m_pMenuObject and this UndoObject is to be deleted 
  // then we want to delete the m_pMenuObject only if this UndoObject is deleted while 
  // the UndoBuffer::redoBuffer is emptied 
  // BUT NOT when the UndoObject is deleted due to the MAX_UNDO_BUFFER is reached.
//printf ( "%s::%s::%d > obj<%p> delete<%d> <%s>\n", __FILE__, __FUNCTION__, __LINE__, m_pMenuObject, m_bDeleteObject, getTypeString ( ).ascii ( ) );

  if ( ( m_enUndoType == NEW_SHADOW  ) || ( m_enUndoType == DELETED_SHADOW  ) ||
       ( m_enUndoType == NEW_OVERLAY ) || ( m_enUndoType == DELETED_OVERLAY ) )  {
    if ( m_bDeleteObject && m_pShadowObject )
      delete m_pShadowObject;
    m_pShadowObject = NULL;
  }
  else if ( ( m_enUndoType == DRAGGED_IMAGE   ) ||
            ( m_enUndoType == DRAGGED_BUTTON  ) ) {
       if ( m_bDeleteObject && m_pMenuObject )
            delete m_pMenuObject;
       if ( m_bDeleteObject && m_pNormalObject )
            delete m_pNormalObject;
       m_pMenuObject   = NULL;
       m_pNormalObject = NULL;
  }
  else  {
    if ( m_bDeleteObject && m_pMenuObject && ! m_pObjectData )
      delete m_pMenuObject;
    m_pMenuObject = NULL;
  }
  if ( m_bDeleteObject && m_pSourceFileEntry )
    delete m_pSourceFileEntry;
  m_pSourceFileEntry = NULL;

  if ( m_pObjectData ) // Always delete the ObjectData
    delete m_pObjectData;
  m_pObjectData = NULL;

  if ( m_pDVDMenuInterface )
    delete m_pDVDMenuInterface;
  m_pDVDMenuInterface = NULL;

  if ( m_bDeleteObject && m_listOfObjects.count ( ) > 0 )  {
    QList<MenuObject *>::iterator it = m_listOfObjects.begin ( );	//oxx
    while ( it != m_listOfObjects.end ( ) )
      delete *it++;
  }
  m_listOfObjects.clear ( );
}

void DVDMenuUndoObject::changeType ( enUndoType type )
{
  m_enUndoType = type;
}

void DVDMenuUndoObject::initMe ( )
{
	m_enUndoType       = (int)UNDEFINED;
	m_objectState      = MenuObject::StateUnknown;
	m_iTwiggle         =   -1;
	m_pMenuObject      = NULL;
	m_pSourceFileEntry = NULL;
	m_pButtonPreview   = NULL;
	m_pShadowObject    = NULL;
	m_pObjectData      = NULL;
	m_pNormalObject    = NULL;
	m_pDVDMenuInterface= NULL;
	m_bDeleteObject    = false;
}

DVDMenuUndoObject::BackgroundInfo::BackgroundInfo ( )
{
	bModified = false;
	bMovieBackgroundHasSound = false;
	iFormat = 0;
	iHeight = 0;
	iWidth  = 0;
	iStretchType = -1;
}

QString DVDMenuUndoObject::getTypeString ( )
{
  QString qsType;
  switch ( m_enUndoType )  {
  case DRAGGED_TEXT :
    qsType = QObject::tr ( "New Text" );
  break;
  case DRAGGED_IMAGE :
    qsType = QObject::tr ( "Dragged Image" );
  break;
  case DRAGGED_MOVIE :
    qsType = QObject::tr ( "Dragged Clip" );
  break;
  case DRAGGED_BUTTON :
    qsType = QObject::tr ( "Dragged Button" );
  break;
  case DRAGGED_CHAPTERS :
    qsType = QObject::tr ( "Dragged Chapters" );
  break;
  case MOVED_OBJECT :
    qsType = QObject::tr ( "Move" );
  break;
  case NEW_BACKGROUND :
    qsType = QObject::tr ( "New Background" );
  break;
  case NEW_AUDIO :
    qsType = QObject::tr ( "New Sound" );
  break;
  case NEW_OBJECT :
    qsType = QObject::tr ( "New" );
  break;
  case EDIT_OBJECT :
    qsType = getStateString ( );
  break;
  case DELETED_OBJECT :
    qsType = QObject::tr ( "Deleted" );
  break;
  case NEW_BUTTON:
    qsType = QObject::tr ( "New Button" );
  break;
  case UNBUTTON:
    qsType = QObject::tr ( "Unbutton" );
  break;
  case DELETED_BUTTON:
    qsType = QObject::tr ( "Delete Button" );
  break;
  case NEW_SHADOW :
    qsType = QObject::tr ( "New Shadow" );
  break;
  case DELETED_SHADOW :
    qsType = QObject::tr ( "Del Shadow" );
  break;
  case NEW_OVERLAY :
    qsType = QObject::tr ( "New Mask" );
  break;
  case DELETED_OVERLAY :
    qsType = QObject::tr ( "Del Mask" );
  break;
  case EDIT_MENU :
    qsType = QObject::tr ( "Menu Change" );
  break;
  default:
    qsType = QObject::tr ( "Undefined" );
  }
  return qsType;
}

QString DVDMenuUndoObject::getStateString ( )
{
  QString qsState = QObject::tr ( "Edited" );
  switch ( m_objectState )  {
  case MenuObject::StateUnknown:
  case MenuObject::StateInactive:     // No object is currently active
  break;
  case MenuObject::StateMoveObject:   // the object is being moved
    qsState = QObject::tr ( "Moved" );
  break;
  case MenuObject::StateScaleTLXY:    // the object is being scaled equally TopLeft
    qsState = QObject::tr ( "Scale (Top L)" );
  break;
  case MenuObject::StateScaleTRXY:    // the object is being scaled equally TopRight
    qsState = QObject::tr ( "Scale (Top R)" );
  break;
  case MenuObject::StateScaleBLXY:    // the object is being scaled equally BottomLeft
    qsState = QObject::tr ( "Scale (Bottom L)" );
  break;
  case MenuObject::StateScaleBRXY:    // the object is being scaled equally BottmRight
    qsState = QObject::tr ( "Scale (Bottom R)" );
  break;
  case MenuObject::StateScaleLeftX:   // the object is only scaled in X direction
    qsState = QObject::tr ( "Scale (Left X)" );
  break;
  case MenuObject::StateScaleRightX:  // the object is only scaled in X direction
    qsState = QObject::tr ( "Scaled (Right X)" );
  break;
  case MenuObject::StateScaleTopY:    // the object is only scaled in Y direction
    qsState = QObject::tr ( "Scaled (Top Y)" );
  break;
  case MenuObject::StateScaleBottomY: // the object is only scaled in Y direction
    qsState = QObject::tr ( "Scaled (Bottom Y)" );
  break;
  case MenuObject::StateRotate:
    qsState = QObject::tr ( "Rotated" );
  break;
  }
  return qsState;
}

// Here is the code, to restore the previous configuration.
void DVDMenuUndoObject::undo ( DVDMenu *pDVDMenu )
{
  QPixmap tempPix;
  BackgroundInfo tempInfo;
  CDVDMenuInterface *pInterface = NULL;

  switch ( m_enUndoType )  {
  case DRAGGED_BUTTON :
  case DRAGGED_IMAGE  :
    if ( m_pNormalObject )  {
      pDVDMenu->getMenuPreview ( )->removeObject ( m_pNormalObject );
      m_pNormalObject->resetStructureItem ( );
    }
  case DRAGGED_MOVIE  :
  case DRAGGED_TEXT   :
  case NEW_OBJECT     :
    pDVDMenu->getMenuPreview ( )->removeObject ( m_pMenuObject );
    m_pMenuObject->resetStructureItem ( );
    m_bDeleteObject = true;
  break;
  case DRAGGED_CHAPTERS :
    handleChapters ( pDVDMenu, true );
  break;
  case MOVED_OBJECT   :
    handleMoved ( );
  break;
  case NEW_BACKGROUND :
    // First we change the Pixmap
    //tempPix = *pDVDMenu->getMenuPreview()->paletteBackgroundPixmap ();	//oooo
    pDVDMenu->getMenuPreview()->setPixmap (m_backgroundInfo.background);
    // STore the prvious Pixmap
    m_backgroundInfo.background = tempPix;
    // Next we get additional information
    pInterface = pDVDMenu->getInterface();
    tempInfo.getBackgroundInfo(pInterface);
    m_backgroundInfo.setBackgroundInfo(pInterface);
    m_backgroundInfo = tempInfo;
    pDVDMenu->getMenuPreview ( )->resize       ( 720, 480 );
    pDVDMenu->getMenuPreview ( )->setFixedSize ( 720, 480 );
    pDVDMenu->updateDVDMenu  ( );
  break;
  case NEW_AUDIO :
    // We remove the SourceFileEntry from the list and from the SourceToolBar
    pDVDMenu->removeSourceFileEntry (m_pSourceFileEntry);
    // last we mark the entry for deletion if this undo-object gets deleted.
    m_bDeleteObject = true;
  break;
  case EDIT_OBJECT  :
    handleObject ( );
  break;
  case DELETED_OBJECT :
    m_pButtonPreview->addMenuObject ( m_pMenuObject );
    // last we make sure the entry is preserved when the UndoObject is deleted
    m_bDeleteObject = false;
  break;
  case NEW_BUTTON :
    unbutton ( pDVDMenu );
  break;
  case UNBUTTON :
    rebutton ( pDVDMenu );
  break;
  case DELETED_BUTTON :
    // Same as Deleted_Object. This function is never called
  break;
  case NEW_SHADOW :
  case DELETED_SHADOW :
    handleShadow ( );
  break;
  case EDIT_MENU :
    handleMenu ( pDVDMenu );
  break;
  case NEW_OVERLAY :
  case DELETED_OVERLAY :
    handleOverlay ( );
  break;
  };  // End of the switch loop.
  pDVDMenu->refresh ( );
}

void DVDMenuUndoObject::redo ( DVDMenu *pDVDMenu )
{
  QPixmap tempPix;
  BackgroundInfo tempInfo;
  CDVDMenuInterface *pInterface = NULL;

  switch ( m_enUndoType )  {
  case DRAGGED_BUTTON :
  case DRAGGED_IMAGE  :
    if ( m_pNormalObject )  {
      pDVDMenu->getMenuPreview ( )->addMenuObject ( m_pNormalObject );
      m_pNormalObject->resetStructureItem ( );
    }
  case DRAGGED_MOVIE  :
  case DRAGGED_TEXT   :
  case NEW_OBJECT     :
    // inserted dragged text, image or button ... All's the smae !
    pDVDMenu->getMenuPreview ( )->addMenuObject ( m_pMenuObject );
    m_pMenuObject->resetStructureItem ( );
    m_bDeleteObject = false;
  break;
  case DRAGGED_CHAPTERS :
    handleChapters ( pDVDMenu, false );
  break;
  case MOVED_OBJECT   :
    handleMoved ( );
  break;
  case NEW_BACKGROUND :
    // First we change the Pixmap
    //tempPix = *pDVDMenu->getMenuPreview ( )->paletteBackgroundPixmap  ( );	//oooo
    pDVDMenu->getMenuPreview ( )->setPixmap ( m_backgroundInfo.background );
    // STore the prvious Pixmap
    m_backgroundInfo.background = tempPix;
    // Next we get additional information
    pInterface = pDVDMenu->getInterface ( );
    tempInfo.getBackgroundInfo ( pInterface );
    m_backgroundInfo.setBackgroundInfo ( pInterface );
    m_backgroundInfo = tempInfo;
    pDVDMenu->getMenuPreview ( )->resize       ( 720, 480 );
    pDVDMenu->getMenuPreview ( )->setFixedSize ( 720, 480 );
    pDVDMenu->updateDVDMenu  ( );
  break;
  case NEW_AUDIO :
    pDVDMenu->appendSourceFileEntry ( m_pSourceFileEntry );
    // last we make sure the entry is preserved when the UndoObject is deleted
    m_bDeleteObject = false;
  break;
  case NEW_BUTTON :
    rebutton ( pDVDMenu );
  break;
  case UNBUTTON :
    unbutton ( pDVDMenu );
  break;
  case DELETED_BUTTON :
    // Same as Deleted_Object. This function is never called
  break;
  case EDIT_OBJECT :
    handleObject ( );
  break;
  case DELETED_OBJECT :
    m_pButtonPreview->removeObject ( m_pMenuObject );
    m_pMenuObject->resetStructureItem ( );
    // last we make sure the entry is preserved when the UndoObject is deleted
    m_bDeleteObject = true;
  break;
  case NEW_SHADOW :
  case DELETED_SHADOW :
    handleShadow ( );
  break;
  case NEW_OVERLAY :
  case DELETED_OVERLAY :
    handleOverlay ( );
  break;
  case EDIT_MENU :
    handleMenu ( pDVDMenu );
  break;
  };  // End of the switch loop.
  pDVDMenu->refresh ( );
}

void DVDMenuUndoObject::handleMoved ( )
{
  if ( m_pMenuObject->objectType ( ) == SELECTION_OBJECT )  {
    SelectionObject *pSelection = (SelectionObject *)m_pMenuObject;
    pSelection->setObjects ( m_listOfObjects, false );
  }
  QRect rectPos = m_pMenuObject->rect    ( );
  m_pMenuObject->setRect ( m_rectPos );
  m_rectPos = rectPos;
}

void DVDMenuUndoObject::handleMenu ( DVDMenu *pDVDMenu )
{
  if ( ! m_pDVDMenuInterface || ! pDVDMenu )
    return;

  CDVDMenuInterface *pCurrent = pDVDMenu->getInterface ( );
  if ( m_pDVDMenuInterface->qsMenuName  != pCurrent->qsMenuName )  {
       QString qsMenuName     = m_pDVDMenuInterface->qsMenuName;
       m_pDVDMenuInterface->qsMenuName   = pCurrent->qsMenuName;
       Global::pApp->replaceMenuName     ( pDVDMenu, qsMenuName );
  }
  else if ( ! ( *pCurrent == *m_pDVDMenuInterface ) )  {
    CDVDMenuInterface *pTemp = new CDVDMenuInterface;
    *pTemp    = *pCurrent;
    *pCurrent = *m_pDVDMenuInterface;
     delete m_pDVDMenuInterface;
     m_pDVDMenuInterface = pTemp;
  }
  Global::pApp->slotUpdateStructure ( );
}

void DVDMenuUndoObject::handleObject ( )
{
  if ( m_pMenuObject && m_pObjectData )  {
    MenuObject::Data *pObjectData = m_pMenuObject->data ( );
    if ( pObjectData )  {
      MenuObject::Data *pObject = pObjectData->clone ( );
      m_pMenuObject->setData  ( m_pObjectData );
      delete m_pObjectData;
      m_pObjectData = pObject;
      if ( ( m_pMenuObject->objectType ( ) == IMAGE_OBJECT ) ||
           ( m_pMenuObject->objectType ( ) == MOVIE_OBJECT )  )  {
        ImageObject *pImage = (ImageObject *)m_pMenuObject;
        pImage->updatePixmap ( );	//oooo
      }
    }
  }
}

void DVDMenuUndoObject::handleShadow ( )
{
  MenuObject *pObject = m_pMenuObject->shadow ( );
  if ( ( m_pShadowObject ) && ( pObject ) )  {
    pObject->resetStructureItem ( );
    MenuObject *pTemp = pObject->clone ( m_pMenuObject->parent ( ), m_pMenuObject );
    m_pMenuObject->setShadow ( m_pShadowObject );
    m_pMenuObject->updateShadow ( );
    m_pShadowObject = pTemp;
  }
  else if ( ( ! m_pShadowObject ) && ( pObject ) )  {
    pObject->resetStructureItem ( );
    m_pShadowObject = pObject->clone ( m_pMenuObject->parent ( ), m_pMenuObject );
    m_pMenuObject->setShadow ( NULL );
  }
  else if ( ( m_pShadowObject ) && ( ! pObject ) )  {
    m_pMenuObject->setShadow ( m_pShadowObject );
    m_pMenuObject->updateShadow ( );
    m_pShadowObject = NULL;
  }
  Global::pApp->slotUpdateStructure ( );
}

void DVDMenuUndoObject::handleOverlay ( )
{
  MenuObject *pObject = m_pMenuObject->overlay ( );
  if ( ( m_pShadowObject ) && ( pObject ) )  {
    pObject->resetStructureItem ( );
    MenuObject *pTemp = pObject->clone ( m_pMenuObject->parent ( ), m_pMenuObject );
    m_pMenuObject->setOverlay ( m_pShadowObject );
    m_pMenuObject->updateOverlay ( );
    m_pShadowObject = pTemp;
  }
  else if ( ( ! m_pShadowObject ) && ( pObject ) )  {
    pObject->resetStructureItem ( );
    m_pShadowObject = pObject->clone ( m_pMenuObject->parent ( ), m_pMenuObject );
    m_pMenuObject->setOverlay ( NULL );
  }
  else if ( ( m_pShadowObject ) &&  ( ! pObject ) )  {
    m_pMenuObject->setOverlay ( m_pShadowObject );
    m_pMenuObject->updateOverlay ( );
    m_pShadowObject = NULL;
  }
  Global::pApp->slotUpdateStructure ( );
}

void DVDMenuUndoObject::handleChapters ( DVDMenu *pDVDMenu, bool bUndo )
{
  QList<MenuObject *>::iterator it = m_listOfObjects.begin ( );		//oxx
  MenuObject *pObject = NULL;
  while ( it != m_listOfObjects.end ( ) )  {
    pObject = *it++;
    if ( bUndo )
        pDVDMenu->getMenuPreview ( )->removeObject  ( pObject );
    else
        pDVDMenu->getMenuPreview ( )->addMenuObject ( pObject );
    pObject->resetStructureItem  ( );
  }
  m_bDeleteObject = bUndo;
}

void DVDMenuUndoObject::unbutton ( DVDMenu *pDVDMenu )
{
  MenuPreview *pMenuPreview = pDVDMenu->getMenuPreview ( );
  if ( pMenuPreview )  {
    uint t;
    MenuObject   *pObject = NULL;
    ButtonObject *pButton = (ButtonObject *)m_pMenuObject;
    // First we disconnect the button from this MenuPreview
//    pButton->disconnect ( );
    pMenuPreview->removeObject ( pButton );

    for ( t=0; t<pButton->getNormalCount ( ); t++ )  {
      // Then we grab the Normal-Object from the ButtonObject
      pObject =  pButton->getNormal ( t );
      pButton->resetStructureItem   (   );
      pButton->removeNormal   ( pObject );
      // And add the object to this MenuPreview.
      pMenuPreview->addMenuObject ( pObject );
      pObject->disconnect ( );
      // The StructureItem's parent is currently the ButtonObject. So deleting the button would crash the app
//      pObject->resetStructureItem   ( );
      pMenuPreview->connectStdSlots ( pObject, true );
    }
    pDVDMenu->slotUpdateStructure ( );
    m_bDeleteObject = true;
  }
}

void DVDMenuUndoObject::rebutton ( DVDMenu *pDVDMenu )
{
  MenuPreview *pMenuPreview = pDVDMenu->getMenuPreview ( );
  if ( pMenuPreview )  {
    pMenuPreview->removeObject ( m_pNormalObject );
    m_pNormalObject->resetStructureItem   ( );

    ButtonObject *pButton = (ButtonObject *)m_pMenuObject;
    pButton->resetStructureItem   ( );

    pButton->appendNormal ( m_pNormalObject );
    pMenuPreview->addMenuObject   ( pButton );
    pDVDMenu->slotUpdateStructure ( );
    m_bDeleteObject = false;
  }
}

MenuObject *DVDMenuUndoObject::getMenuObject ( )
{
  return m_pMenuObject;
}

// This function is called after a MouseRelease event in the ButtonPreview class
// It'll check if the object was moved or not.
bool DVDMenuUndoObject::hasMoved ( )
{
  if ( ! m_pMenuObject )
    return false;
  if ( ( m_pMenuObject->rect ( ).x ( ) != m_rectPos.x ( ) ) ||
       ( m_pMenuObject->rect ( ).y ( ) != m_rectPos.y ( ) ) )
    return true;
  return false;
}

bool DVDMenuUndoObject::isMoving ( int iButton )
{
  // Used for determining Cursor key moving
  if ( ( m_enUndoType == MOVED_OBJECT ) && ( iButton == m_iTwiggle ) )
    return true;
  return false;
}

bool DVDMenuUndoObject::hasChanged ( )
{
  bool bReturn = false;
  if ( m_pMenuObject && m_pObjectData && ( getType ( ) == EDIT_OBJECT ) )
    bReturn = ( ! ( *m_pMenuObject->data ( ) == *m_pObjectData ) );

  return bReturn;
}

DVDMenuUndoObject::BackgroundInfo &DVDMenuUndoObject::BackgroundInfo::operator = ( DVDMenuUndoObject::BackgroundInfo &theOther )
{
  bModified                = theOther.bModified;
  // the following entries belong to the movie background
  qsMovieFileName          = theOther.qsMovieFileName;
  bMovieBackgroundHasSound = theOther.bMovieBackgroundHasSound;
  timeOffset               = theOther.timeOffset;
  timeDuration             = theOther.timeDuration;
  // The following var is the background image or screenshot of themovie ...
  qsBackgroundFileName     = theOther.qsBackgroundFileName;
  qsResizeAlgo             = theOther.qsResizeAlgo;
  iStretchType             = theOther.iStretchType;	// StretchToFit, BlackBorders, CutOffset
  posOffset                = theOther.posOffset;
  iFormat                  = theOther.iFormat;
  iWidth                   = theOther.iWidth;
  iHeight                  = theOther.iHeight;
  return *this;
}

void DVDMenuUndoObject::BackgroundInfo::getBackgroundInfo ( CDVDMenuInterface *pInterface )
{
  qsBackgroundFileName     = pInterface->qsBackgroundFileName;
  bModified                = pInterface->bModified;
  // the following entries belong to the movie background
  qsMovieFileName          = pInterface->qsMovieFileName;
  bMovieBackgroundHasSound = pInterface->bMovieBackgroundHasSound;
  timeOffset               = pInterface->timeOffset;
  timeDuration             = pInterface->timeDuration;
  // The following var is the background image or screenshot of themovie ...
  qsBackgroundFileName     = pInterface->qsBackgroundFileName;
  qsResizeAlgo             = pInterface->qsResizeAlgo;
  iStretchType             = pInterface->iStretchType;	// StretchToFit, BlackBorders, CutOffset
  posOffset                = pInterface->posOffset;
  iFormat                  = pInterface->iFormat;
  iWidth                   = pInterface->iWidth;
  iHeight                  = pInterface->iHeight;
}

void DVDMenuUndoObject::BackgroundInfo::setBackgroundInfo (CDVDMenuInterface *pInterface)
{
  pInterface->qsBackgroundFileName     = qsBackgroundFileName;
  pInterface->bModified                = bModified;
  // the following entries belong to the movie background
  pInterface->qsMovieFileName          = qsMovieFileName;
  pInterface->bMovieBackgroundHasSound = bMovieBackgroundHasSound;
  pInterface->timeOffset               = timeOffset;
  pInterface->timeDuration             = timeDuration;
  // The following var is the background image or screenshot of themovie ...
  pInterface->qsBackgroundFileName     = qsBackgroundFileName;
  pInterface->qsResizeAlgo             = qsResizeAlgo;
  pInterface->iStretchType             = iStretchType;	// StretchToFit, BlackBorders, CutOffset
  pInterface->posOffset                = posOffset;
  pInterface->iFormat                  = iFormat;
  pInterface->iWidth                   = iWidth;
  pInterface->iHeight                  = iHeight;
}

