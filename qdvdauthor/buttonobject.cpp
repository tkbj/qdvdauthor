/***************************************************************************
    buttonobject.cpp
                             -------------------
    ButtonObject class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   This class is the encapsulation of the ButtonObject. 

   A ButtonObject is the object in a menu which was created by
   either dragging a SourceFileEntry over to the  DVD menu or
   by choosing "define as button" from the context menu of an 
   MenuObject.
    
****************************************************************************/

#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>

#include "global.h"
#include "xml_dvd.h"
#include "dvdmenu.h"
#include "textobject.h"
#include "maskobject.h"
#include "qdvdauthor.h"
#include "imageobject.h"
#include "menupreview.h"
#include "frameobject.h"
#include "buttonobject.h"
#include "xml_slideshow.h"	//ooo
#include "sourcefileentry.h"
#include "structuretoolbar.h"

ButtonTransition::ButtonTransition ( )
{
  initMe ( );
}

ButtonTransition::~ButtonTransition ( )
{

}

void ButtonTransition::initMe ( )
{
  iFormat            = FORMAT_NTSC1;
  fDuration          = 3.0f;
  bUserFile          = false;
  iTransitionType    = 1;  // alpha_fade
  qsTransitionName   = Global::qsSystemPath + "/share/qdvdauthor/slideshow/transitions/luma/Etoile_002.png";  // in luma dir.
  bFinishedRendering = false;
  qsName             = "Menu/Button";  // will be replaced
  qsTransitionVideoFileName = Global::qsTempPath +"/menuTransition.vob";  // will be replaced
}

ButtonTransition &ButtonTransition::operator = ( const ButtonTransition &theOther )
{
  fDuration                 = theOther.fDuration;
  qsName                    = theOther.qsName;
  iFormat                   = theOther.iFormat;
  bUserFile                 = theOther.bUserFile;
  iTransitionType           = theOther.iTransitionType;
  qsTransitionName          = theOther.qsTransitionName;
  bFinishedRendering        = theOther.bFinishedRendering;
  qsTransitionVideoFileName = theOther.qsTransitionVideoFileName;

  return *this;
}

bool ButtonTransition::operator == ( const ButtonTransition &theOther )
{
  bool bReturn = ( ( qsName           == theOther.qsName           ) &&
                   ( iFormat          == theOther.iFormat          ) &&
                   ( bUserFile        == theOther.bUserFile        ) &&
                   ( fDuration        == theOther.fDuration        ) &&
                   ( iTransitionType  == theOther.iTransitionType  ) &&
                   ( qsTransitionName == theOther.qsTransitionName ) &&
                   ( qsTransitionVideoFileName == theOther.qsTransitionVideoFileName ) );
  return bReturn;
}

bool ButtonTransition::readProjectFile ( QDomNode &theNode )
{
  // Okay, here we retain the stored data from the xml file.
  initMe ( );
  QDomElement  theElement  = theNode.toElement ( );
  QDomAttr     attribute;

  attribute = theElement.attributeNode ( BUTTON_TRANS_DURATION );
  if ( ! attribute.isNull ( ) )
    fDuration = attribute.value ( ).toFloat ( );

  attribute = theElement.attributeNode ( BUTTON_TRANS_FORMAT );
  if ( ! attribute.isNull ( ) )
    iFormat = attribute.value ( ).toInt ( );

  attribute = theElement.attributeNode ( BUTTON_TRANS_USER_FILE );
  if ( ! attribute.isNull ( ) )
    bUserFile = ( attribute.value ( ) == "true" );

  attribute = theElement.attributeNode ( BUTTON_TRANS_TYPE );
  if ( ! attribute.isNull ( ) )
    iTransitionType = attribute.value ( ).toInt ( );

  attribute = theElement.attributeNode ( BUTTON_TRANS_NAME );
  if ( ! attribute.isNull ( ) )
    qsName = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_TRANS_TRANSITION_NAME );
  if ( ! attribute.isNull ( ) )
    qsTransitionName = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_TRANS_RENDERED );
  if ( ! attribute.isNull ( ) )
    bFinishedRendering = ( attribute.value ( ) == "true" );

  if ( ! theElement.text ( ).isNull ( ) )
    qsTransitionVideoFileName = theElement.text ( );

  return true;
}

bool ButtonTransition::writeProjectFile ( QDomElement &rootElement )
{
  QDomDocument xmlDoc     = rootElement.ownerDocument ( );
  QDomElement  theElement = xmlDoc.createElement  ( BUTTON_TRANSITION );
  //QDomText     theText    = xmlDoc.createTextNode ( qsTransitionVideoFileName.utf8 ( ) );	//ooo
  QDomText     theText    = xmlDoc.createTextNode ( qsTransitionVideoFileName.toUtf8 ( ) );	//xxx

  theElement.setAttribute ( BUTTON_TRANS_NAME,            qsName           );
  theElement.setAttribute ( BUTTON_TRANS_TRANSITION_NAME, qsTransitionName );
  theElement.setAttribute ( BUTTON_TRANS_FORMAT,   QString ( "%1" ).arg ( iFormat         ) );
  theElement.setAttribute ( BUTTON_TRANS_DURATION, QString ( "%1" ).arg ( fDuration       ) );
  theElement.setAttribute ( BUTTON_TRANS_TYPE,     QString ( "%1" ).arg ( iTransitionType ) );
  theElement.setAttribute ( BUTTON_TRANS_FORMAT,   bUserFile          ?  "true" : "false" );
  theElement.setAttribute ( BUTTON_TRANS_RENDERED, bFinishedRendering ?  "true" : "false" );

  theElement.appendChild  ( theText    );
  rootElement.appendChild ( theElement );
  return true;
}

CXmlSlideshow *ButtonTransition::createSlideshow ( )
{
  Utils theUtils;
  QString qsTempPath = theUtils.getTempFile ( "" );

  CXmlSlideshow *pSlideshow = new CXmlSlideshow;
  pSlideshow->slideshow_name    = "transition";
  pSlideshow->delay             = 0.1;
  pSlideshow->filter_delay      = fDuration;
  pSlideshow->intro_page        = false;
  pSlideshow->fade_out          = false;
  pSlideshow->bImgBackgroundImg = false;
  theUtils.getResolution ( iFormat, pSlideshow->xres, pSlideshow->yres );

  QFileInfo fileInfo ( qsTransitionName );
  pSlideshow->validFilterNames.append ( fileInfo.fileName ( ) );

  CXmlSlideshow::img_struct *pStartImage = pSlideshow->addImg ( );
  CXmlSlideshow::img_struct *pEndImage   = pSlideshow->addImg ( );
  pStartImage->src = qsTempPath + qsName + "/start.jpg";
  pEndImage->src   = qsTempPath + qsName + "/end.jpg";

  return pSlideshow;
}

ButtonObject::ButtonData::ButtonData ( )
  : MenuObject::Data ( )
{
  m_iDataType     = Data::BUTTON;
  m_bLoopMultiple = false;
  m_pTransition   = NULL;
}

ButtonObject::ButtonData::~ButtonData ( )
{
}

MenuObject::Data &ButtonObject::ButtonData::operator = ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  MenuObject::Data::operator = ( theOther );
  if ( m_iDataType == theOther.m_iDataType )  {
       ButtonData    *pOther = (ButtonData *)&theOther;
       m_qsAction            = pOther->m_qsAction;     // the action to take ...
       m_qsPreAction         = pOther->m_qsPreAction;  // ';' separated commands before the jump - action.
       m_qsUp                = pOther->m_qsUp;
       m_qsDown              = pOther->m_qsDown;
       m_qsRight             = pOther->m_qsRight;
       m_qsLeft              = pOther->m_qsLeft;
       m_pTransition         = pOther->m_pTransition;
       m_qsSourceDisplayName = pOther->m_qsSourceDisplayName;
       m_listPlayMultiple    = pOther->m_listPlayMultiple;     // If the user selected multiple files to be played
       m_bLoopMultiple       = pOther->m_bLoopMultiple;        // Loop after playing all files in the Multiple list.
       // The following lists hold the information for the different button states.
       //Q3ValueList<MenuObject *>::iterator it = m_listNormalState.begin ( );		//ooo
       QList<MenuObject *>::iterator it = m_listNormalState.begin ( );			//xxx
       while ( it != m_listNormalState.end ( ) )  {
         MenuObject *pMenuObject = *it++;
         pMenuObject->setData ( pOther );
         if ( ( pMenuObject->objectType ( ) == IMAGE_OBJECT ) ||
              ( pMenuObject->objectType ( ) == MOVIE_OBJECT )  )  {
           ImageObject *pImage = (ImageObject *)pMenuObject;
           pImage->updatePixmap  ( );		//oooo
         }
       }
       it = m_listSelectedState.begin    ( );
       while ( it != m_listSelectedState.end    ( ) )
             (*it++)->setData ( pOther );
       it = m_listHighlightedState.begin ( );
       while ( it != m_listHighlightedState.end ( ) )
             (*it++)->setData ( pOther );
  }

  return *this;
}

bool ButtonObject::ButtonData::operator == ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  bool bReturn = ( MenuObject::Data::operator == ( theOther ) );
  if ( ! bReturn )
    return false;

  if ( m_iDataType  == theOther.m_iDataType )  {
    ButtonData *pOther = (ButtonData *)&theOther;
    bReturn = ( ( m_qsAction            == pOther->m_qsAction            ) &&
                ( m_qsPreAction         == pOther->m_qsPreAction         ) &&
                ( m_qsUp                == pOther->m_qsUp                ) &&
                ( m_qsDown              == pOther->m_qsDown              ) &&
                ( m_qsRight             == pOther->m_qsRight             ) &&
                ( m_qsLeft              == pOther->m_qsLeft              ) &&
                ( m_pTransition         == pOther->m_pTransition         ) &&
                ( m_qsSourceDisplayName == pOther->m_qsSourceDisplayName ) &&
                ( m_listPlayMultiple    == pOther->m_listPlayMultiple    ) &&
                ( m_bLoopMultiple       == pOther->m_bLoopMultiple       ) );
    if ( ! bReturn )
      return false;

    if ( m_listNormalState.count ( ) != pOther->m_listNormalState.count ( ) )
      return false;
    if ( m_listSelectedState.count ( ) != pOther->m_listSelectedState.count ( ) )
      return false;
    if ( m_listHighlightedState.count ( ) != pOther->m_listHighlightedState.count ( ) )
      return false;

    MenuObject::Data *pData1 = NULL;
    MenuObject::Data *pData2 = NULL;
    QList<MenuObject *>::iterator it1 =         m_listNormalState.begin ( );		//oxx
    QList<MenuObject *>::iterator it2 = pOther->m_listNormalState.begin ( );		//oxx
    while ( it1 != m_listNormalState.end ( ) )  {
      pData1 = (*it1++)->data ( );
      pData2 = (*it2++)->data ( );
      if (  ! ( *pData1 == *pData2 ) )
        return false;
    }
    it1 =          m_listSelectedState.begin ( );
    it2 =  pOther->m_listSelectedState.begin ( );
    while ( it1 != m_listSelectedState.end ( ) )  {
      pData1 = (*it1++)->data ( );
      pData2 = (*it2++)->data ( );
      if (  ! ( *pData1 == *pData2 ) )
        return false;
    }
    it1 =          m_listHighlightedState.begin ( );
    it2 =  pOther->m_listHighlightedState.begin ( );
    while ( it1 != m_listHighlightedState.end ( ) )  {
      pData1 = (*it1++)->data ( );
      pData2 = (*it2++)->data ( );
      if (  ! ( *pData1 == *pData2 ) )
        return false;
    }
  }
  return bReturn;
}

MenuObject::Data *ButtonObject::ButtonData::clone ( )
{
  ButtonData *pData = new ButtonData;
  *pData  = *this;
  return pData;
}

ButtonObject::ButtonObject ( QWidget *pParent ) //, const char *pName, WFlags flags)
  : MenuObject ( pParent )
{
  if ( m_pData )
    delete m_pData;        // part of the base class
  m_pData = new ButtonData; // will be deleted in base class

  m_qsObjectType     = QString (BUTTON_OBJECT);
  m_iButtonState     = STATE_NORMAL;
  m_pSourceFileEntry = NULL;
  m_bMoveable        = true;
}

ButtonObject::~ButtonObject ( )
{
  setTransition ( NULL );
  uint t;
  ButtonData *pData = buttonData ( );

  //for ( t=0; t<(uint)pData->m_listHighlightedState.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( pData->m_listHighlightedState.count ( ) ); t++ ) //xxx
    delete pData->m_listHighlightedState[t];
  //for ( t=0; t<(uint)pData->m_listSelectedState.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( pData->m_listSelectedState.count ( ) ); t++ )    //xxx
    delete pData->m_listSelectedState[t];
  //for ( t=0; t<(uint)pData->m_listNormalState.count ( ); t++ )                //ooo
  for ( t=0; t<static_cast<uint> ( pData->m_listNormalState.count ( ) ); t++ )  //xxx
    delete pData->m_listNormalState[t];

  pData->m_listNormalState.clear ( );
  pData->m_listSelectedState.clear ( );
  pData->m_listHighlightedState.clear ( );
}

MenuObject *ButtonObject::clone ( QWidget *pParent, MenuObject * )
{
  if ( ! pParent )
    pParent = MenuObject::parent ( );
  uint t;
  ButtonObject *pNewObject = new ButtonObject ( pParent );
  //for ( t=0; t<(uint)buttonData ( )->m_listNormalState.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listNormalState.count ( ) ); t++ ) //xxx
    pNewObject->appendNormal ( buttonData ( )->m_listNormalState[t]->clone ( pParent, pNewObject ) );
  //for ( t=0; t<(uint)buttonData ( )->m_listSelectedState.count ( ); t++ )                 //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count ( ) ); t++ )   //xxx
    pNewObject->appendSelected ( buttonData ( )->m_listSelectedState[t]->clone ( pParent, pNewObject ) );
  //for ( t=0; t<(uint)buttonData ( )->m_listHighlightedState.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count ( ) ); t++ )    //xxx
    pNewObject->appendHighlighted ( buttonData ( )->m_listHighlightedState[t]->clone ( pParent, pNewObject ) );
  
  QString qsAction ( "jump+-+vmgm+-+" );    //xxx

  pNewObject->setSourceFileEntry ( sourceFileEntry ( ) );
  pNewObject->setAction     ( action    ( ) );    //ooo
  //pNewObject->setAction     ( qsAction );           //xxx
  pNewObject->setPreAction  ( preAction ( ) );
  pNewObject->setName       ( name      ( ) );
  pNewObject->setPlayMultipleList( getPlayMultipleList ( ) );
  pNewObject->setLoopMultiple    ( getLoopMultiple     ( ) );
  pNewObject->setNext       ( NEXT_BUTTON_UP,    buttonData ( )->m_qsUp    );
  pNewObject->setNext       ( NEXT_BUTTON_DOWN,  buttonData ( )->m_qsDown  );
  pNewObject->setNext       ( NEXT_BUTTON_RIGHT, buttonData ( )->m_qsRight );
  pNewObject->setNext       ( NEXT_BUTTON_LEFT,  buttonData ( )->m_qsLeft  );
  pNewObject->setTransition ( buttonData ( )->m_pTransition );

  if ( modifiers ( ) )
    pNewObject->setModifiers ( *modifiers ( ) );
  if ( shadow    ( ) )
    pNewObject->setShadow  ( shadow  ( )->clone ( ) );
  if ( overlay   ( ) )
    pNewObject->setOverlay ( overlay ( )->clone ( ) );

//  MenuObject *pNew  = (MenuObject *)pNewObject;
//  MenuObject *pSelf = (MenuObject *)this;
//  *pNew = *pSelf;
  return pNewObject;
}

ButtonObject &ButtonObject::operator = (ButtonObject &theOther)
{
	uint t;
	clear ( );
	setSourceFileEntry ( theOther.sourceFileEntry ( ) );
	buttonData ( )->m_qsAction    = theOther.action    ( );
	buttonData ( )->m_qsPreAction = theOther.preAction ( );
	setName       ( theOther.name    ( ) );
	buttonData ( )->m_qsUp        = theOther.next ( NEXT_BUTTON_UP    );
	buttonData ( )->m_qsDown      = theOther.next ( NEXT_BUTTON_DOWN  );
	buttonData ( )->m_qsRight     = theOther.next ( NEXT_BUTTON_RIGHT );
	buttonData ( )->m_qsLeft      = theOther.next ( NEXT_BUTTON_LEFT  );
	buttonData ( )->m_listPlayMultiple = theOther.getPlayMultipleList ( );
	buttonData ( )->m_bLoopMultiple    = theOther.getLoopMultiple     ( );
	setTransition ( theOther.getTransition ( ) );
	// The following lists hold the information for the different button states.
	for ( t=0; t<theOther.getNormalCount ( );      t++ )
		appendNormal      ( theOther.getNormal      ( t ) );
	for ( t=0; t<theOther.getSelectedCount ( );    t++ )
		appendSelected    ( theOther.getSelected    ( t ) );
	for ( t=0; t<theOther.getHighlightedCount ( ); t++ )
		appendHighlighted ( theOther.getHighlighted ( t ) );
	return *this;
}

ButtonObject::ButtonData *ButtonObject::buttonData ( )
{
  return (ButtonData *)m_pData;
}

void ButtonObject::clear ( )
{
  uint t;

  m_iButtonState = STATE_NORMAL;
  buttonData ( )->m_qsAction     = QString ("");
  buttonData ( )->m_qsPreAction  = QString ("");
  buttonData ( )->m_qsUp         = QString ("");
  buttonData ( )->m_qsDown       = QString ("");
  buttonData ( )->m_qsRight      = QString ("");
  buttonData ( )->m_qsLeft       = QString ("");
  m_pSourceFileEntry = NULL;
  buttonData ( )->m_listPlayMultiple.clear ( );
  setTransition ( NULL );

  //for ( t=0; t<(uint)buttonData ( )->m_listNormalState.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listNormalState.count ( ) ); t++ ) //xxx
    delete buttonData ( )->m_listNormalState[t];
  //for ( t=0; t<(uint)buttonData ( )->m_listSelectedState.count ( ); t++ )                 //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count ( ) ); t++ )   //xxx
    delete buttonData ( )->m_listSelectedState[t];
  //for ( t=0; t<(uint)buttonData ( )->m_listHighlightedState.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count ( ) ); t++ )    //xxx
    delete buttonData ( )->m_listHighlightedState[t];
  buttonData ( )->m_listNormalState.clear  ( );
  buttonData ( )->m_listSelectedState.clear  ( );
  buttonData ( )->m_listHighlightedState.clear ( );
}

void ButtonObject::drawContents ( QPainter *pPainter )
{
	drawContents ( pPainter, BACKGROUND_IMG );
//	buttonData ( )->m_listNormalState[0]->drawContents(pPainter);
//	m_boundingRect = buttonData ( )->m_listNormalState[0]->boundingRect();
//	m_rect = buttonData ( )->m_listNormalState[0]->rect();
}

void ButtonObject::drawContents (QPainter *pPainter, uint iWhichState)
{
  switch ( iWhichState )  {
  case BACKGROUND_IMG:
    if ( buttonData ( )->m_listNormalState[0] )  {
      buttonData ( )->m_listNormalState[0]->drawContents ( pPainter );
      m_boundingRect  = buttonData ( )->m_listNormalState[0]->boundingRect ( );
      m_pData->m_rect = buttonData ( )->m_listNormalState[0]->rect( );
    }
    break;
  case HIGHLIGHTED_MASK:
    if ( buttonData ( )->m_listHighlightedState[0] )
      buttonData ( )->m_listHighlightedState[0]->drawContents ( pPainter );
    break;
  case SELECTED_MASK:
    if ( buttonData ( )->m_listSelectedState[0] )
      buttonData ( )->m_listSelectedState[0]->drawContents ( pPainter );
    break;
  }
}

void ButtonObject::drawContents (QPainter *pPainter, int iRenderFrameNumber, int iTotalFrames)
{
	if (buttonData ( )->m_listNormalState[0])	{
		buttonData ( )->m_listNormalState[0]->drawContents(pPainter, iRenderFrameNumber, iTotalFrames);
		m_boundingRect = buttonData ( )->m_listNormalState[0]->boundingRect();
		m_pData->m_rect = buttonData ( )->m_listNormalState[0]->rect();
	}
}

void ButtonObject::resetStructureItem ( )
{
  if ( m_pShadow )
       m_pShadow->resetStructureItem  ( );
  if ( m_pOverlay )
       m_pOverlay->resetStructureItem ( );

  QList<MenuObject *>::iterator it = buttonData ( )->m_listNormalState.begin ( );	//oxx
  while ( it != buttonData ( )->m_listNormalState.end ( ) )
        (*it++)->resetStructureItem   ( );

  it = buttonData ( )->m_listSelectedState.begin ( );
  while ( it != buttonData ( )->m_listSelectedState.end ( ) )
        (*it++)->resetStructureItem ( );

  it = buttonData ( )->m_listHighlightedState.begin ( );
  while ( it != buttonData ( )->m_listHighlightedState.end ( ) )
        (*it++)->resetStructureItem ( );

  if ( m_pStructureItem )
    delete m_pStructureItem;
  m_pStructureItem = NULL;
  MenuObject::resetStructureItem ( );
}

void ButtonObject::reParentChildren ( )
{
  uint t;
  for ( t=0; t<getNormalCount      ( ); t++ )
    reParent ( getNormal          ( t ) );
  for ( t=0; t<getHighlightedCount ( ); t++ )
    reParent ( getHighlighted     ( t ) );
  for ( t=0; t<getSelectedCount    ( ); t++ )
    reParent ( getSelected        ( t ) );
}

void ButtonObject::reParent ( MenuObject *pObject )
{
  if ( ! pObject )
    return;

  if ( pObject->objectType ( ) == MASK_OBJECT )  {
    MaskObject *pMask = (MaskObject *)pObject;
    pMask->setParent ( this );
  }
}

bool ButtonObject::mousePressEvent ( QMouseEvent *pEvent )
{
  m_currentMousePos = pEvent->pos ( );
  if ( pEvent->button ( ) == Qt::RightButton )  {
    QPoint globalPos = pEvent->globalPos ( );
    return createContextMenu ( globalPos );
  }
  else  { // Here I only want to activate the drawing of the frame rather then the full thing.
    ButtonData *pData = buttonData ( );
    if ( pData->m_listNormalState.count ( ) > 0 )
         pData->m_listNormalState[0]->mousePressEvent ( pEvent );
  }

  m_bDrawRect = true;
  return false;
}

bool ButtonObject::mouseReleaseEvent (QMouseEvent *pEvent)
{
  m_bDrawRect = false;
  if ( buttonData ( )->m_listNormalState.count ( ) > 0 )
       buttonData ( )->m_listNormalState[0]->mouseReleaseEvent ( pEvent );

  emit ( signalUpdatePixmap    ( ) );
  emit ( signalUpdateStructure ( ) );
  return false;
}

bool ButtonObject::mouseDoubleClickEvent(QMouseEvent *)
{
  // Call MenuPreview::slotCreateButtonDialog ()
  emit (signalCreateButtonDialog (this));
  return false;
}

void ButtonObject::appendNormal (MenuObject *pObject)
{
	buttonData ( )->m_listNormalState.append (pObject);
}

void ButtonObject::appendSelected (MenuObject *pObject)
{
	buttonData ( )->m_listSelectedState.append (pObject);
}

void ButtonObject::appendHighlighted (MenuObject *pObject)
{
	buttonData ( )->m_listHighlightedState.append (pObject);
}

Rgba ButtonObject::getMaskColor ( bool bHighlighted, int iIdx )
{
  Rgba returnColor ( 0, 0, 0, 0 );
  MenuObject *pObject = NULL;
  if ( bHighlighted ) {
    if ( iIdx >= (int)buttonData ( )->m_listHighlightedState.count ( ) )
      return returnColor;
    pObject = buttonData ( )->m_listHighlightedState[iIdx];
  }
  else {
    if ( iIdx >= (int)buttonData ( )->m_listSelectedState.count ( ) )
      return returnColor;
    pObject = buttonData ( )->m_listSelectedState[iIdx];
  }

  if ( pObject->objectType ( ) == MASK_OBJECT ) {
    MaskObject   *pMask = (MaskObject *)pObject;
    returnColor = pMask->color ( );
  }
  else if ( pObject->objectType ( ) == FRAME_OBJECT ) {
    FrameObject  *pFrame = (FrameObject *)pObject;
    returnColor = pFrame->color ( );
  }
  else if ( pObject->objectType ( ) == TEXT_OBJECT ) {
    TextObject   *pText = (TextObject *)pObject;
    returnColor = pText->getForegroundColor ( );
  }
  return returnColor;
}

bool ButtonObject::setMaskColor ( Rgba newColor, bool bHighlighted, int iIdx )
{
  MenuObject *pObject = NULL;
  if ( bHighlighted ) {
    if ( iIdx >= (int)buttonData ( )->m_listHighlightedState.count ( ) )
      return false;
    pObject = buttonData ( )->m_listHighlightedState[iIdx];
  }
  else {
    if ( iIdx >= (int)buttonData ( )->m_listSelectedState.count ( ) )
      return false;
    pObject = buttonData ( )->m_listSelectedState[iIdx];
  }

  if ( pObject->objectType ( ) == MASK_OBJECT ) {
    MaskObject  *pMask = (MaskObject *)pObject;
    pMask->setColor ( newColor );
  }
  else if ( pObject->objectType ( ) == FRAME_OBJECT ) {
    FrameObject *pFrame = (FrameObject *)pObject;
    pFrame->setFrameColor ( newColor );
  }
  else if ( pObject->objectType ( ) == TEXT_OBJECT ) {
    TextObject  *pText = (TextObject *)pObject;
    pText->setForegroundColor ( newColor );
  }
  else
    return false;
  return true;
}

uint ButtonObject::getNormalCount ()
{
  return buttonData ( )->m_listNormalState.count();
}

MenuObject *ButtonObject::getNormal ( uint iNr )
{
  //if ( iNr >= (uint)buttonData ( )->m_listNormalState.count ( ) )                 //ooo
  if ( iNr >= static_cast<uint> ( buttonData ( )->m_listNormalState.count ( ) ) )  //xxx
    return NULL;
  return buttonData ( )->m_listNormalState[iNr];
}

MenuObject *ButtonObject::removeNormal ( uint iNr )
{
  MenuObject *pMenuObject = getNormal ( iNr );
  if ( pMenuObject )
      //buttonData ( )->m_listNormalState.remove ( pMenuObject );   //ooo
      buttonData ( )->m_listNormalState.removeOne ( pMenuObject );  //xxx
  return pMenuObject;
}

bool ButtonObject::removeNormal ( MenuObject *pMenuObject )
{
  if ( ! pMenuObject )
    return false;
  //return buttonData ( )->m_listNormalState.remove ( pMenuObject );	//ooo
  return buttonData ( )->m_listNormalState.removeOne ( pMenuObject );	//xxx
}

uint ButtonObject::getSelectedCount ()
{
  return buttonData ( )->m_listSelectedState.count();
}

MenuObject *ButtonObject::getSelected ( uint iNr )
{
  //if ( iNr >= (uint)buttonData ( )->m_listSelectedState.count ( ) )                   //ooo
  if ( iNr >= static_cast<uint> ( buttonData ( )->m_listSelectedState.count ( ) ) )     //xxx
    return NULL;
  return buttonData ( )->m_listSelectedState[iNr];
}

bool ButtonObject::removeSelected ( MenuObject *pMenuObject )
{
  if ( ! pMenuObject )
    return false;
  //return buttonData ( )->m_listSelectedState.remove ( pMenuObject );	//ooo
  return buttonData ( )->m_listSelectedState.removeOne ( pMenuObject );	//xxx
}

uint ButtonObject::getHighlightedCount ()
{
  return buttonData ( )->m_listHighlightedState.count();
}

MenuObject *ButtonObject::getHighlighted ( uint iNr )
{
  //if ( iNr >= (uint)buttonData ( )->m_listHighlightedState.count ( ) )                    //ooo
  if ( iNr >= static_cast<uint> ( buttonData ( )->m_listHighlightedState.count ( ) ) )      //xxx
    return NULL;
  return buttonData ( )->m_listHighlightedState[iNr];
}

bool ButtonObject::removeHighlighted ( MenuObject *pMenuObject )
{
  if ( ! pMenuObject )
    return false;
  //return buttonData ( )->m_listHighlightedState.remove ( pMenuObject );	//ooo
  return buttonData ( )->m_listHighlightedState.removeOne ( pMenuObject );	//xxx
}

ButtonTransition *ButtonObject::getTransition ( )
{
  return buttonData ( )->m_pTransition;
}

void ButtonObject::setTransition ( ButtonTransition *pTransition )
{
  if (   ! pTransition )  {
    if ( buttonData ( )->m_pTransition )  {
      delete buttonData ( )->m_pTransition;
      buttonData ( )->m_pTransition = NULL;
    }
  }
  else  {
    if ( ! buttonData ( )->m_pTransition )
       buttonData ( )->m_pTransition  = new ButtonTransition;
      *buttonData ( )->m_pTransition = *pTransition;
  }
}

QStringList &ButtonObject::getPlayMultipleList ( )
{
  return buttonData ( )->m_listPlayMultiple;
}

void ButtonObject::setPlayMultipleList ( QStringList &newList )
{
  buttonData ( )->m_listPlayMultiple = newList;
}

bool ButtonObject::getLoopMultiple ( )
{
  return buttonData ( )->m_bLoopMultiple;
}

void ButtonObject::setLoopMultiple ( bool bLoopMultiple )
{
  buttonData ( )->m_bLoopMultiple = bLoopMultiple;
}

void ButtonObject::deleteMasks()
{
  // This function only handles the case of the cancelation of the Button creation
  // From the DialogButton - class.
  // I.e. the user just created the Button and decides to cancel out.
  // In case the User cancels the ButtonDialog when the ButtonObject already 
  // existed before, nothing ought to be done here.
  uint t;
  ButtonData *pData = buttonData ( );
  // we can leave the buttonData ( )->m_listNormalState - objects as they reflect the original Objects.
  // However the m_listSelected / Highlighted should be deleted ...
  //for ( t=0; t<(uint)pData->m_listSelectedState.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( pData->m_listSelectedState.count ( ) ); t++ )   //xxx
       delete  pData->m_listSelectedState[t];
  //for ( t=0; t<(uint)pData->m_listHighlightedState.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( pData->m_listHighlightedState.count ( ) ); t++ ) //xxx
       delete  pData->m_listHighlightedState[t];
  pData->m_listSelectedState.clear ( );
  pData->m_listHighlightedState.clear ( );
}

bool ButtonObject::readProjectFile ( QDomNode &theNode )
{
  // Okay, here we retain the stored data from the xml file.
  QDomNode    xmlMultiple = theNode.firstChild ( );
  QDomElement theElement  = theNode.toElement  ( );
  QDomAttr    attribute;
  ButtonData *pData = buttonData ( );

  attribute = theElement.attributeNode ( BUTTON_OBJECT_NAME );
  if ( ! attribute.isNull ( ) )
    pData->m_qsName = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_OBJECT_ACTION );
  if ( ! attribute.isNull ( ) )
    pData->m_qsAction = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_OBJECT_PRE_ACTION );
  if ( ! attribute.isNull ( ) )
    pData->m_qsPreAction = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_OBJECT_UP );
  if ( ! attribute.isNull ( ) )
    pData->m_qsUp = attribute.value ( );
  else
    pData->m_qsUp = QString ("-- default --");

  attribute = theElement.attributeNode ( BUTTON_OBJECT_DOWN );
  if ( ! attribute.isNull ( ) )
    pData->m_qsDown = attribute.value ( );
  else
    pData->m_qsDown = QString ("-- default --");

  attribute = theElement.attributeNode ( BUTTON_OBJECT_LEFT );
  if ( ! attribute.isNull ( ) )
    pData->m_qsLeft = attribute.value ( );
  else
    pData->m_qsLeft = QString ("-- default --");

  attribute = theElement.attributeNode ( BUTTON_OBJECT_RIGHT );
  if ( ! attribute.isNull ( ) )
    pData->m_qsRight = attribute.value ( );
  else
    pData->m_qsRight = QString ("-- default --");

  attribute = theElement.attributeNode ( BUTTON_OBJECT_MOVEABLE );
  m_bMoveable = true;
  if ( ! attribute.isNull ( ) )  {
    if ( attribute.value  ( ) == QString ( "false" ) )
         m_bMoveable = false;
  }

  attribute = theElement.attributeNode ( BUTTON_OBJECT_SOURCE_ENTRY);
  if ( ! attribute.isNull ( ) )
    pData->m_qsSourceDisplayName = attribute.value ( );

  attribute = theElement.attributeNode ( BUTTON_OBJECT_LOOP_MULTIPLE );
  pData->m_bLoopMultiple = false;
  if ( ! attribute.isNull ( ) ) {
    if ( attribute.value  ( ) == QString ("true") )
      pData->m_bLoopMultiple = true;
  }

  // Please note that the nortmal / selected / highlighted objects are read
  // in by ButtonObject::readObjects(...);
  return true;
}

bool ButtonObject::readObjects (QDomNode &theNode, MenuPreview *pPreview)
{
  int t;
  QDomElement buttonElement;
  QDomNode    childNode;
  QDomNode    buttonNode = theNode.firstChild();
  MenuObject *pNewObject = NULL;
  MenuObject *pMenuObject;
  while ( ! buttonNode.isNull ( ) )  {
    buttonElement   = buttonNode.toElement  ( );
    QString tagName = buttonElement.tagName ( );
    if ( buttonElement.hasChildNodes ( ) ) { // For Masks we do not need any children.
      childNode = buttonNode.firstChild ( );
      // Next we should create the new Object but if we see the
      // MenoObject tag it means it is the MenuObject vars of the button
      // itself, thus we don't want to create anything. (It would put out a warning).
      if ( tagName == BUTTON_OBJECT_MULTIPLE ) {
        QString nodeText = buttonElement.text ( );
        buttonData ( )->m_listPlayMultiple.append ( nodeText );
      }
      else if ( tagName == BUTTON_TRANSITION )  {
        if  ( ! buttonData ( )->m_pTransition )
                buttonData ( )->m_pTransition = new ButtonTransition;
        buttonData ( )->m_pTransition->readProjectFile ( buttonNode );
      }
      else if ( tagName != MENU_OBJECT )
        pNewObject = pPreview->readObject ( childNode );

      if ( tagName == BUTTON_OBJECT_NORMAL )
        buttonData ( )->m_listNormalState.append ( pNewObject );

      else if ( tagName == BUTTON_OBJECT_SELECTED )
        buttonData ( )->m_listSelectedState.append( pNewObject );

      else if ( tagName == BUTTON_OBJECT_HIGHLIGHTED )
        buttonData ( )->m_listHighlightedState.append ( pNewObject );

      else if ( tagName == MENU_OBJECT )
        MenuObject::readProjectFile ( theNode );


      else if ( ( tagName != BUTTON_OBJECT_MULTIPLE ) && ( tagName != BUTTON_TRANSITION ) )
        //printf  ( "Warning : ButtonObject::readObjects -=> wrong XML Node <%s>\nContinuing ...\n", (const char *)tagName);	//ooo
	printf  ( "Warning : ButtonObject::readObjects -=> wrong XML Node <%s>\nContinuing ...\n", tagName.toLatin1().data());	//xxx
    }
    buttonNode = buttonNode.nextSibling ();
  }

  // short sanity check ..
  if ( buttonData ( )->m_listNormalState.count ( ) < 1 )
    return false;

  MaskObject *pMask = NULL;
  MenuObject *pParentObject = buttonData ( )->m_listNormalState[0];
  for ( t=0; t<(int)buttonData ( )->m_listSelectedState.count ( ); t++ ) {
    pMenuObject = buttonData ( )->m_listSelectedState[t];
    if ( pMenuObject->objectType ( ) == QString ( MASK_OBJECT ) ) {
      pMask = (MaskObject *)pMenuObject;
      pMask->setParent  ( pParentObject );
    }
  }
  for ( t=0; t<(int)buttonData ( )->m_listHighlightedState.count ( ); t++ ) {
    pMenuObject = buttonData ( )->m_listHighlightedState[t];
    if ( pMenuObject->objectType ( ) == QString ( MASK_OBJECT ) ) {
      pMask = (MaskObject *)pMenuObject;
      pMask->setParent ( pParentObject );
    }
  }	//oooo
  return true;
}

bool ButtonObject::writeProjectFile (QDomElement &theElement)
{
	uint t;
	QDomDocument xmlDoc    = theElement.ownerDocument ( );
	QDomElement buttonNode = xmlDoc.createElement ( BUTTON_OBJECT );  // <ButtonObject>
	// Here we set the attributes of the <ButtonObject> tag
	if ( ! m_pData->m_qsName.isNull ( ) )
		buttonNode.setAttribute ( BUTTON_OBJECT_NAME,   name ( ) );
	if ( ! buttonData ( )->m_qsAction.isNull    ( ) )
		buttonNode.setAttribute ( BUTTON_OBJECT_ACTION, buttonData ( )->m_qsAction );
	if ( ! buttonData ( )->m_qsPreAction.isNull ( ) )
		buttonNode.setAttribute ( BUTTON_OBJECT_PRE_ACTION, buttonData ( )->m_qsPreAction );
	if ( ( ! buttonData ( )->m_qsUp.isNull ( ) ) && ( buttonData ( )->m_qsUp != QString ("-- default --")) )
		buttonNode.setAttribute ( BUTTON_OBJECT_UP,     buttonData ( )->m_qsUp );
	if ( ( ! buttonData ( )->m_qsDown.isNull() ) && ( buttonData ( )->m_qsDown != QString ("-- default --")) )
		buttonNode.setAttribute ( BUTTON_OBJECT_DOWN,   buttonData ( )->m_qsDown );
	if ( ( ! buttonData ( )->m_qsLeft.isNull() ) && ( buttonData ( )->m_qsLeft != QString ("-- default --")) )
		buttonNode.setAttribute ( BUTTON_OBJECT_LEFT,   buttonData ( )->m_qsLeft );
	if ( ( ! buttonData ( )->m_qsRight.isNull()) && ( buttonData ( )->m_qsRight != QString ("-- default --")) )
		buttonNode.setAttribute ( BUTTON_OBJECT_RIGHT,  buttonData ( )->m_qsRight );
	if (!m_bMoveable)
		buttonNode.setAttribute( BUTTON_OBJECT_MOVEABLE, QString ("false") );
	if ( buttonData ( )->m_bLoopMultiple )
		buttonNode.setAttribute( BUTTON_OBJECT_LOOP_MULTIPLE, QString ("true") );

	if (m_pSourceFileEntry)
		buttonNode.setAttribute( BUTTON_OBJECT_SOURCE_ENTRY,  m_pSourceFileEntry->qsDisplayName );
	
	// And here we store the list of multiple objects ( if present )
	//for ( t=0; t<(uint)buttonData ( )->m_listPlayMultiple.count ( ); t++ ) {                 //ooo
        for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listPlayMultiple.count ( ) ); t++ ) {    //xxx
	  QDomElement multipleNode = xmlDoc.createElement  ( BUTTON_OBJECT_MULTIPLE );
	  QDomText text = xmlDoc.createTextNode ( buttonData ( )->m_listPlayMultiple[t] );
	  multipleNode.appendChild ( text );
	  buttonNode.appendChild ( multipleNode );
	}

	if ( buttonData ( )->m_pTransition )
		buttonData ( )->m_pTransition->writeProjectFile ( buttonNode );

	// And here we store the ButtonObjects children ...
	QDomElement normalNode = xmlDoc.createElement      ( BUTTON_OBJECT_NORMAL );
	//for ( t=0; t<(uint)buttonData ( )->m_listNormalState.count ( ); t++ )                //ooo
        for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listNormalState.count ( ) ); t++ )   //xxx
	  buttonData ( )->m_listNormalState[t]->writeProjectFile ( normalNode );

	QDomElement selectedNode = xmlDoc.createElement    ( BUTTON_OBJECT_SELECTED );
	//for ( t=0; t<(uint)buttonData ( )->m_listSelectedState.count ( ); t++ )              //ooo
        for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count ( ) ); t++ ) //xxx
	  buttonData ( )->m_listSelectedState[t]->writeProjectFile ( selectedNode );

	QDomElement highlightedNode = xmlDoc.createElement ( BUTTON_OBJECT_HIGHLIGHTED );
	//for (t=0;t<(uint)buttonData ( )->m_listHighlightedState.count();t++)                 //ooo
        for (t=0;t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count() );t++)    //xxx
	  buttonData ( )->m_listHighlightedState[t]->writeProjectFile ( highlightedNode );

	buttonNode.appendChild ( normalNode      );
	buttonNode.appendChild ( selectedNode    );
	buttonNode.appendChild ( highlightedNode );

	theElement.appendChild ( buttonNode );
	// And here we write the base class ...
	return MenuObject::writeProjectFile ( buttonNode );
}

bool ButtonObject::createContextMenu (QPoint globalPos)
{
/*	int     iID = -1;
	bool    bHasPropertyDialog;
	QString qsObject, qsEditObject;
        QPoint  globalPos2 = globalPos;
	Q3PopupMenu *pStackMenu = new Q3PopupMenu(m_pParent);
	pStackMenu->insertItem ( tr ("Cut") , this, SLOT ( slotCut  ( ) ) );
	pStackMenu->insertItem ( tr ("Copy"), this, SLOT ( slotCopy ( ) ) );
	pStackMenu->insertSeparator ( );
	pStackMenu->insertItem ( tr ("To Front")  , this, SLOT(slotToFront()));
	pStackMenu->insertItem ( tr ("To Back")   , this, SLOT(slotToBack()));
	globalPos.setY ( globalPos.y ( ) - 25 );
	globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint().width()); // -100);
	pStackMenu->popup(globalPos, 1);

	bHasPropertyDialog = false;
	qsEditObject = tr ( "Edit Object ..." );
	MenuObject *pObject = buttonData ( )->m_listNormalState[0];
	if ( ! pObject )
	  return true;

	if ( pObject->objectType ( ) == QString ( IMAGE_OBJECT ) ) {
	  qsEditObject = tr ( "Edit Image ..." );
	  qsObject     = tr ( "Image Properties ..." );
	  bHasPropertyDialog = true;
	}
	else if ( buttonData ( )->m_listNormalState[0]->objectType ( ) == QString ( MOVIE_OBJECT ) ) {
	  qsEditObject = tr ( "Edit Movie ..." );
	  qsObject     = tr ( "Movie Properties ..." );
	  bHasPropertyDialog = true;
	}

	if ( m_pContextMenu )
	     delete m_pContextMenu;
	m_pContextMenu = new Q3PopupMenu(m_pParent);
	m_pContextMenu->setCheckable (true);
	if ( bHasPropertyDialog )
	     m_pContextMenu->insertItem ( qsObject,          (QObject *)this, SLOT(slotObjectProperties ( ) ) );
	m_pContextMenu->insertItem ( qsEditObject,           (QObject *)this, SLOT(slotEditObject( ) ) );
	if ( pObject->shadow ( ) )
	  m_pContextMenu->insertItem ( tr ("Edit Shadow ..."),(QObject*)this, SLOT(slotEditShadow( ) ) );
	if ( pObject->overlay ( ) )
	  m_pContextMenu->insertItem ( tr ("Edit Mask ..."), (QObject *)this, SLOT(slotEditMask  ( ) ) );
	m_pContextMenu->insertSeparator ( );
	m_pContextMenu->insertItem ( tr ("Edit Button ..."), (QObject *)this, SLOT(slotEdit      ( ) ) );
	m_pContextMenu->insertItem ( tr ("Unbutton"),        (QObject *)this, SLOT(slotUnbutton  ( ) ) );
	m_pContextMenu->insertItem ( tr ("Delete"),          (QObject *)this, SLOT(slotDelete    ( ) ) );
	iID = m_pContextMenu->insertItem( tr ("Moveable"),   (QObject *)this, SLOT(slotMoveable  ( ) ) );
	m_pContextMenu->insertSeparator ( );
	m_pContextMenu->insertItem ( tr ("To Text"),         (QObject *)this, SLOT(slotToText    ( ) ) );
	m_pContextMenu->insertItem ( tr ("To Frame"),        (QObject *)this, SLOT(slotToFrame   ( ) ) );
	m_pContextMenu->insertItem ( tr ("To Image"),        (QObject *)this, SLOT(slotToImage   ( ) ) );
	m_pContextMenu->insertItem ( tr ("To Movie"),        (QObject *)this, SLOT(slotToMovie   ( ) ) );
	m_pContextMenu->setItemChecked  ( iID, m_bMoveable );

	m_pContextMenu->exec(globalPos2, 5);

	delete pStackMenu;
	if ( m_pContextMenu )
		delete m_pContextMenu;
	m_pContextMenu = NULL;
*/	//oooo

  QPoint  globalPos2 = globalPos;

  if ( m_pContextMenu )
    delete m_pContextMenu;
  
  QAction *editNewAction = new QAction("Edit Button ...", this);
  QAction *unbuttonNewAction = new QAction("Unbutton", this);
  QAction *deleteNewAction = new QAction("&Delete", this);
  QAction *toTextNewAction = new QAction("To Text", this);

  m_pContextMenu = new QMenu(m_pParent);
  m_pContextMenu->addAction(editNewAction);
  m_pContextMenu->addAction(unbuttonNewAction);
  m_pContextMenu->addAction(deleteNewAction);
  m_pContextMenu->addSeparator ( );
  m_pContextMenu->addAction(toTextNewAction);
  
  connect(editNewAction, SIGNAL(triggered()), (QObject *)this, SLOT( slotEdit      ( ) ) );
  connect(unbuttonNewAction, SIGNAL(triggered()), (QObject *)this, SLOT( slotUnbutton  ( ) ) );
  connect(deleteNewAction, SIGNAL(triggered()), (QObject *)this, SLOT( slotDelete    ( ) ) );
  connect(toTextNewAction, SIGNAL(triggered()), (QObject *)this, SLOT( slotToText    ( ) ) );
  
  m_pContextMenu->exec( globalPos2, deleteNewAction );
  
  //delete pStackMenu;		//oooo
  if ( m_pContextMenu )
    delete m_pContextMenu;
  m_pContextMenu = NULL;

  // Here we mark that the user called a menu item thus we don't want the
  // base classes to continue with the mouse event
  return true;
}

StructureItem *ButtonObject::createStructure ( StructureItem *pParentItem )
{  
  // Creating teh ListView structure of this Button.
  uint t;
  QString qsAction;
  QStringList actionList;
  StructureItem *pStateItem, *pTransItem, *pTemp=NULL;

  if ( ! pParentItem )  {
    //for ( t=0; t<(uint)buttonData ( )->m_listNormalState.count      ( ); t++ )                //ooo
    for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listNormalState.count      ( ) ); t++ )   //xxx
      pTemp =    buttonData ( )->m_listNormalState[t]->createStructure     ( NULL );
    //for ( t=0; t<(uint)buttonData ( )->m_listHighlightedState.count ( ); t++ )                //ooo
    for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count ( ) ); t++ )  //xxx
      pTemp =    buttonData ( )->m_listHighlightedState[t]->createStructure( NULL );
    //for ( t=0; t<(uint)buttonData ( )->m_listSelectedState.count    ( ); t++ )                //ooo
    for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count    ( ) ); t++ )  //xxx
      pTemp =    buttonData ( )->m_listSelectedState[t]->createStructure   ( NULL );
    m_pStructureItem = NULL;
    return NULL;
  }

  if ( buttonData ( )->m_listPlayMultiple.count ( ) > 1 )
    qsAction = tr ( "Play Multiple Files" );
  else if ( ! buttonData ( )->m_qsAction.isEmpty ( ) )  { // For new buttons this might be empty ...
    //actionList = QStringList::split (QString (STRING_SEPARATOR), buttonData ( )->m_qsAction);		//ooo
    actionList = buttonData ( )->m_qsAction.split (QString (STRING_SEPARATOR) );			//xxx
    if (actionList.count() == 1)	
      qsAction = buttonData ( )->m_qsAction;
    else	{
      qsAction = actionList[0] + QString (" ") +actionList[1] + QString (" ") +actionList[2];
      if (actionList.count() == 4)
        qsAction += QString (" ") +actionList[3];
    }
  }
 
  if ( ! m_pStructureItem ) {  
    m_pStructureItem = new StructureItem ( this, StructureItem::Button, pParentItem, pParentItem->lastChild ( ), name ( ), qsAction );
    //m_pStructureItem->setText ( 0, name ( ) );			//xxx
    //m_pStructureItem->setText ( 1, qsAction );			//xxx
    //m_pStructureItem->setExpandable      ( TRUE );		//ooo
    m_pStructureItem->setExpanded      ( false );		//xxx
  }
  else {
    //m_pStructureItem->setText ( name ( ), qsAction );		//ooo
    m_pStructureItem->setText ( 0, name ( ) );			//xxxx
    m_pStructureItem->setText ( 1, qsAction );			//xxx    
  }
  
  QString qsEmpty;
  QString qsTransition  = tr ( "Transition"  );
  QString qsHighlighted = tr ( "Highlighted" );
  QString qsSelected    = tr ( "Selected"    );

  //for ( t=0; t<(uint)buttonData ( )->m_listNormalState.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listNormalState.count ( ) ); t++ ) //xxx
       pTemp = buttonData ( )->m_listNormalState[t]->createStructure ( m_pStructureItem );

  //pStateItem =(StructureItem *) m_pStructureItem->firstChild ( );	//ooo
  pStateItem =(StructureItem *) m_pStructureItem->child ( 0 );		//xxx
  while ( pStateItem )  {
    if  ( pStateItem->text ( 0 ) == qsHighlighted )
      break;
    //pStateItem = (StructureItem *)pStateItem->nextSibling ( );							//ooo
    pStateItem = (StructureItem *)pStateItem->parent()->child(pStateItem->parent()->indexOfChild(pStateItem)+1);	//xxx
  }
 
  if ( ! pStateItem ) {
    pStateItem = new StructureItem ( this, StructureItem::Highlighted, m_pStructureItem, pTemp, qsHighlighted, qsEmpty );
    //pStateItem->setText ( 0, qsHighlighted );			//xxx
    //pStateItem->setExpandable ( TRUE );	//ooo
    pStateItem->setExpanded ( true );		//xxx
  }
  
  //for ( t=0; t<(uint)buttonData ( )->m_listHighlightedState.count ( ); t++ )                  //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count ( ) ); t++ )    //xxx
    pTemp = buttonData ( )->m_listHighlightedState[t]->createStructure ( pStateItem );

  pTemp = pStateItem;
  //pStateItem =(StructureItem *) m_pStructureItem->firstChild ( );	//ooo
  pStateItem =(StructureItem *) m_pStructureItem->child ( 0 );		//xxx
  while ( pStateItem )  {
    if  ( pStateItem->text ( 0 ) == qsSelected )
      break;
    //pStateItem = (StructureItem *)pStateItem->nextSibling ( );							//ooo
    pStateItem = (StructureItem *)pStateItem->parent()->child(pStateItem->parent()->indexOfChild(pStateItem)+1);	//xxx
  }
 
  if ( ! pStateItem ) {
    pStateItem = new StructureItem ( this, StructureItem::Selected, m_pStructureItem, pTemp, qsSelected, qsEmpty );
    //pStateItem->setText ( 0, qsSelected );			//xxx
    //pStateItem->setExpandable ( TRUE );	//ooo
    pStateItem->setExpanded ( true );		//xxx
  }

  //for ( t=0; t<(uint)buttonData ( )->m_listSelectedState.count ( ); t++ )                 //ooo
  for ( t=0; t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count ( ) ); t++ )   //xxx
    pTemp = buttonData ( )->m_listSelectedState[t]->createStructure ( pStateItem );

  ButtonTransition *pTransition = getTransition ( );
  //pTransItem =(StructureItem *) m_pStructureItem->firstChild ( );	//ooo
  pTransItem =(StructureItem *) m_pStructureItem->child ( 0 );		//xxx
  while ( pTransItem )  {
    if  ( pTransItem->text ( 0 ) == qsTransition )
      break;
    //pTransItem = (StructureItem *)pTransItem->nextSibling ( );							//ooo
    pTransItem = (StructureItem *)pTransItem->parent()->child(pTransItem->parent()->indexOfChild(pTransItem)+1);	//xxx
  }
 
  if ( pTransition )  {
    if ( ! pTransItem )
      pTransItem = new StructureItem ( this, StructureItem::Modifiers, m_pStructureItem, pTemp, qsTransition, qsEmpty );

    qsTransition = QString ( "%1 Sec : %2" ).arg ( pTransition->fDuration ).arg ( pTransition->qsName );
    pTransItem->setText    ( 1, qsTransition );
  }
  else if ( pTransItem )
    delete  pTransItem;

  return m_pStructureItem;	//oooo
}

void ButtonObject::replaceColor ( Rgba theColor, Rgba oldColor )
{
	// We handle the actual color as a unique index. Thus we allow 
	// the user to utilise any of the 4 available colors for any purpose.
	uint t;
	
	// Does this make sense ??? I don't know yet ...
//	for (t=0;t<buttonData ( )->m_listNormalState.count();t++)	{
//		buttonData ( )->m_listNormalState[t]->replaceColor (theColor);
//	}
	//for (t=0;t<(uint)buttonData ( )->m_listSelectedState.count();t++)	{                  //ooo
        for (t=0;t<static_cast<uint> ( buttonData ( )->m_listSelectedState.count() );t++)	{ //xxx
		buttonData ( )->m_listSelectedState[t]->replaceColor ( theColor, oldColor );
	}
	//for (t=0;t<(uint)buttonData ( )->m_listHighlightedState.count();t++)	{                   //ooo
	for (t=0;t<static_cast<uint> ( buttonData ( )->m_listHighlightedState.count() );t++)	{ //xxx
		buttonData ( )->m_listHighlightedState[t]->replaceColor ( theColor, oldColor );
	}
}

void ButtonObject::setAction(QString &qsAction)
{  
	buttonData ( )->m_qsAction = qsAction;
}

QString &ButtonObject::action()
{
	return buttonData ( )->m_qsAction;
}

void ButtonObject::setPreAction(QString &qsPreAction)
{
  buttonData ( )->m_qsPreAction = qsPreAction;
}

QString &ButtonObject::preAction()
{
  return buttonData ( )->m_qsPreAction;
}

void ButtonObject::setSourceFileEntry(SourceFileEntry *pEntry)
{
	m_pSourceFileEntry = pEntry;
	if (pEntry)
		buttonData ( )->m_qsSourceDisplayName = pEntry->qsDisplayName;
	else
		buttonData ( )->m_qsSourceDisplayName = QString ();
}

QString &ButtonObject::sourceDisplayName ()
{
	return buttonData ( )->m_qsSourceDisplayName;
}

SourceFileEntry *ButtonObject::sourceFileEntry()
{
  return m_pSourceFileEntry;
}

SourceFileInfo *ButtonObject::sourceFileInfo ( )
{
  uint             t;
  QString          qsName;
  QFileInfo        fileInfo;
  QStringList      actionList;
  SourceFileEntry *pEntry = sourceFileEntry ( );
  SourceFileInfo  *pInfo  = NULL;

  if  (  ! pEntry )
    return pInfo;

  // default to the first info 
  pInfo = pEntry->listFileInfos[0];

  if ( ! buttonData ( )->m_qsAction.isEmpty ( ) )  {  // For new buttons this might be empty ...
    //actionList = QStringList::split ( QString (STRING_SEPARATOR), buttonData ( )->m_qsAction );	//ooo
    actionList = buttonData ( )->m_qsAction.split ( QString (STRING_SEPARATOR) );			//xxx  
    if ( actionList.count ( ) == 4 )
      qsName = actionList[actionList.count ( ) - 2];
    else if ( actionList.count ( ) > 1 )
      qsName = actionList[1];
  }
  // find the right sourceFileInfo from the action - string
  //for ( t=0; t<(uint)pEntry->listFileInfos.count ( ); t++ ) {                 //ooo
  for ( t=0; t<static_cast<uint> ( pEntry->listFileInfos.count ( ) ); t++ ) {   //xxx
    fileInfo.setFile ( pEntry->listFileInfos[t]->qsFileName );
    if ( qsName == fileInfo.fileName ( ) )
      return pEntry->listFileInfos[t];
  }

  return pInfo;
}

DVDMenu *ButtonObject::dvdMenu ( )
{
  QString qsAction = action ( );
  //QStringList list = QStringList::split ( STRING_SEPARATOR, qsAction );	//ooo
  QStringList list = qsAction.split ( STRING_SEPARATOR );			//xxx
  QString qsMenuName = list[1];
  if ( qsMenuName == "vmgm" )
       qsMenuName  = VMGM_LABEL;

  return Global::pApp->getSubMenuByName ( qsMenuName );
}

long ButtonObject::getOffset ( )
{
  Utils theUtils;
  long iMSecOffset = 0L;

  if ( buttonData ( )->m_qsAction.isEmpty ( ) )
    return iMSecOffset;

  //QStringList actionList = QStringList::split ( STRING_SEPARATOR, buttonData ( )->m_qsAction );	//ooo
  QStringList actionList = buttonData ( )->m_qsAction.split ( STRING_SEPARATOR );			//xxx
  if ( actionList.count ( ) < 1 )
    return iMSecOffset;

  QString qsChapter = actionList[actionList.count  ( ) - 1];
  iMSecOffset = theUtils.getMsFromString ( qsChapter );
  return iMSecOffset;
}

int ButtonObject::chapter ( bool bAllEntries )
{
  uint t, iTemp;
  int iChapterNumber = 1;
  SourceFileEntry *pEntry = sourceFileEntry ();
  SourceFileInfo  *pInfo  = sourceFileInfo  ();
  if ( ( ! pEntry ) || ( ! pInfo ) )
    return 0;  // No Info present ... Error
  QStringList actionList;

  if ( ! buttonData ( )->m_qsAction.isEmpty() )  // For new buttons this might be empty ...
    //actionList = QStringList::split (QString (STRING_SEPARATOR), buttonData ( )->m_qsAction);		//ooo
    actionList = buttonData ( )->m_qsAction.split ( QString (STRING_SEPARATOR) );			//xxx  
  if ( actionList.count() < 1 )
    return iChapterNumber;

  QString qsChapter = actionList[actionList.count ( ) - 1];

  // To get the total chapter number you have to add up the SourceFileInfos
  // chapters in the SourceFileEntry that come before the actual SourceFileInfo
  // on which the button points.
  iChapterNumber = 1;
  if ( bAllEntries ) {
    //for (t=0;t<(uint)pEntry->listFileInfos.count();t++) {                 //ooo
    for (t=0;t<static_cast<uint> ( pEntry->listFileInfos.count() );t++) {   //xxx
      if ( pEntry->listFileInfos[t] == pInfo )
        break; // break out of the loop
      iTemp = pEntry->listFileInfos[t]->listChapters.count();
      //iTemp = (iTemp < 1) ? 1: iTemp; // removed 20060920. No chapter, no -pit-stop-, no count increase.
      iChapterNumber += iTemp;
    }
  }

  if ( pInfo->listChapters.count () > 0 ) {
    //for ( t=0;t<(uint)pInfo->listChapters.count ();t++ ) {                    //ooo
    for ( t=0;t<static_cast<uint> ( pInfo->listChapters.count () );t++ ) {      //xxx
      if ( pInfo->listChapters[t] == qsChapter )  // the chapter
        iChapterNumber += t;
    }
  }
  else // In order to reach this VOB it needs to have at least one chapter to get to. Remember pInfo is associated with this button.
    pInfo->listChapters.append ( "00:00:00.000" ); 

  return iChapterNumber;  // Info present but no chapter info...
}

void ButtonObject::setNext(uint iNext, QString &qsNext)
{ 
  switch (iNext)	{
  case NEXT_BUTTON_UP:
    buttonData ( )->m_qsUp = qsNext;
  break;
  case NEXT_BUTTON_DOWN:
    buttonData ( )->m_qsDown = qsNext;
  break;
  case NEXT_BUTTON_RIGHT:
    buttonData ( )->m_qsRight = qsNext;
  break;
  case NEXT_BUTTON_LEFT:
    buttonData ( )->m_qsLeft = qsNext;
  break;
  };
}

QString &ButtonObject::next(uint iNext)
{
	switch (iNext)	{
	case NEXT_BUTTON_UP:
		return buttonData ( )->m_qsUp;
	break;
	case NEXT_BUTTON_DOWN:
		return buttonData ( )->m_qsDown;
	break;
	case NEXT_BUTTON_RIGHT:
		return buttonData ( )->m_qsRight;
	break;
	case NEXT_BUTTON_LEFT:
		return buttonData ( )->m_qsLeft;
	break;
	};
	return buttonData ( )->m_qsUp;
}

void ButtonObject::slotEdit ( )
{
  mouseDoubleClickEvent ( NULL );
}

void ButtonObject::slotUnbutton ( )
{
  // get a slight delay to allow the ContextMenu to be destroyed ...
  QTimer::singleShot ( 10, this, SLOT ( slotEmitUnbuttonMe ( ) ) );
}

void ButtonObject::slotEmitUnbuttonMe ( )
{
  // Here we are going to split the Button object into its three distinct 
  // objects and add them to the Preview (MenuPreview that is).
  emit ( signalUnbuttonMe ( this ) );
  // The work is done in MenuPreview::slotUnbutton()
}

void ButtonObject::slotToText ( )
{ 
  // Converts the current ButtonObject type to a TextButton
  DVDMenu *pCurrentMenu = Global::pApp->getCurrentSubMenu ( );
  if ( ! pCurrentMenu )
    return;

  MenuPreview *pMenuPreview = pCurrentMenu->getMenuPreview ( );
  if ( ! pMenuPreview )
    return;

  MenuObject  *pNewObject = pMenuPreview->createTextObject ( true );
  if ( ! pNewObject )
    return;

  pMenuPreview->removeObject ( pNewObject );
  MenuObject *pOldObject = buttonData ( )->m_listNormalState[0];

  pNewObject->setRect        (  pOldObject->rect      ( ) );
  pNewObject->setModifiers   ( *pOldObject->modifiers ( ) );

  //buttonData ( )->m_listNormalState.remove   ( pOldObject );		//ooo
  buttonData ( )->m_listNormalState.removeOne   ( pOldObject );		//xxx
  buttonData ( )->m_listNormalState.append   ( pNewObject );

  emit ( signalUpdatePixmap    ( ) );
  emit ( signalUpdateStructure ( ) );

  delete pOldObject;
}

AnimationAttribute *ButtonObject::getSpecificAttributes (long, QString)
{
  return NULL;
}
