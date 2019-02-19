/***************************************************************************
    dvdlayoutarea.cpp
                             -------------------
    Class DVDLayout::Gui
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This is a sub-project to 'Q' DVD-Author to display the current DVD Layout
    
****************************************************************************/

#include <qpushbutton.h>
#include <qpainter.h>
#include <qlayout.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QPixmap>
#include <QMouseEvent>
#include <QMessageBox>		//xxx
#include <QPalette>         //xxx

#include "global.h"
#include "dvdmenu.h"
#include "xml_dvd.h"
#include "qdvdauthor.h"
#include "printpreview.h"
#include "dvdlayoutgui.h"
#include "buttonobject.h"
#include "dvdlayoutarea.h"
#include "sourcefileentry.h"
#include "dvdlayoutrouter.h"
#include "dvdlayoutpainter.h"

namespace DVDLayout
{

Area::PieceOfLine::PieceOfLine ( QPoint &startPos, QPoint &endPos )
{
  m_startPos = startPos;
  m_endPos   = endPos;
}

/////////////////////////////////////////////////////////////////
//
// This class handles the Area where the structure is displayed on.
//
/////////////////////////////////////////////////////////////////
Area::Container::Connection::Connection ( Container *pTarget, Container *pSource )
{
  m_color     = 0x000000; // BLACK
  m_pTarget   = pTarget;
  m_pSource   = pSource;
  m_enSide[0] = SIDE_UNKNOWN;
  m_enSide[1] = SIDE_UNKNOWN;
  m_iConNr[0] = 0;
  m_iConNr[1] = 0;
}

Area::Container::Connection::Connection ( Connection &theOther )
{
  m_color     = theOther.m_color;
  m_pTarget   = theOther.m_pTarget;
  m_pSource   = theOther.m_pSource;
  m_iConNr[0] = theOther.m_iConNr[0];
  m_iConNr[1] = theOther.m_iConNr[1];
  m_enSide[0] = theOther.m_enSide[0];
  m_enSide[1] = theOther.m_enSide[1];

  PieceOfLine *pLine    = NULL;
  PieceOfLine *pNewLine = NULL;
  QList<Area::PieceOfLine *>::iterator it = theOther.lines ( ).begin ( );	//oxx
  while ( it != theOther.lines ( ).end ( ) )  {
    pLine = *it++;
    pNewLine = new PieceOfLine ( pLine->m_startPos, pLine->m_endPos );
    m_listLineStipples.append  ( pNewLine );
  }
}

Area::Container::Connection::~Connection ( )
{
  PieceOfLine *pLine = NULL;
  QList<PieceOfLine *>::iterator it = m_listLineStipples.begin ( );	        //oxx
  QList<Connection  *>::iterator it_2 = target ( )->sources ( ).end ( );    //xxx
  QList<Connection  *>::iterator it_3 = source ( )->sources ( ).end ( );    //xxx
  
  while ( it != m_listLineStipples.end ( ) )  {
    pLine = *it++;
    delete pLine;
  }

  m_listLineStipples.clear ( ); 

  // Finally remove this connection from the associated Container.
//  if ( target ( )->sources ( ).size   (      ) > 0 )
  //if ( target ( )->sources ( ).find   ( this ) != target ( )->sources ( ).end ( ) )   //oooo
  //if ( target ( )->sources ( ).indexOf   ( this ) != target ( )->sources ( ).end ( ) )  //xxx
  if ( target ( )->sources ( ).indexOf   ( this ) != target ( )->sources ( ).indexOf ( *it_2 ) )  //xxx
    //target ( )->sources ( ).remove ( this );   //ooo
    target ( )->sources ( ).removeOne ( this );   //xxx
//  if ( source ( )->targets ( ).size   (      ) > 0 )
  //if ( source ( )->targets ( ).find   ( this ) != source ( )->targets ( ).end ( ) )   //oooo
  //if ( source ( )->targets ( ).indexOf   ( this ) != source ( )->targets ( ).end ( ) )  //xxx
  if ( source ( )->targets ( ).indexOf   ( this ) != source ( )->targets ( ).indexOf ( *it_3 ) )  //xxx    
    //source ( )->targets ( ).remove ( this );   //ooo
    source ( )->targets ( ).removeOne ( this );   //xxx
}

Area::enSide Area::Container::Connection::side ( int iIdx )
{
  if ( iIdx > 2 )
    return SIDE_UNKNOWN;
  return m_enSide[iIdx];
}

int Area::Container::Connection::number ( int iIdx )
{
  if ( iIdx > 2 )
    return 0;
  return m_iConNr[iIdx];
}

Area::Container *Area::Container::Connection::target ( )
{
  return m_pTarget;
}

Area::Container *Area::Container::Connection::source ( )
{
  return m_pSource;
}

void Area::Container::Connection::clearLines ( )
{
  PieceOfLine *pLine = NULL;
  QList<Area::PieceOfLine *>::iterator it = m_listLineStipples.begin ( );	//oxx
  while ( it != m_listLineStipples.end ( ) )  {
    pLine = *it++;
    delete pLine;
  }

  m_listLineStipples.clear ( );
}

void Area::Container::Connection::setLines ( QList<Area::PieceOfLine *> lines )		//oxx
{
  clearLines ( );
  m_listLineStipples = lines;
}

QList<Area::PieceOfLine *> &Area::Container::Connection::lines ( )	//oxx
{
  return m_listLineStipples;
}

Area::Container::Container ( Object *pObject )
{
  m_pObject = pObject;

  // Information to process:
  // SourceFileEntry: qsPre / qsPost
  //                  Subtitles / Audio
  // SourceFileInfo:  qsPreCommands / qsPostCommands
  //                  Subtitles / Audio
  // DVDMenu: qsPre / qsPost
  //          QValueList<ButtonObject *>        &getButtons          ( );
  //          QValueList<MovieObject *>         &getMovieObjects     ( );
  //          QValueList<SourceFileEntry *>     &getSourceFileEntries( );
  //          QValueList<SourceFileInfo *>      &getSourceFileInfos  ( );
  //          SourceFileEntry    *getEntryFromButtonAction ( QString & );
  //          SourceFileInfo     *getInfoFromButtonAction  ( QString & );
  //          DVDMenu            *getMenuFromButtonAction  ( QString & );
  // ButtonObject:  qsAction / qsPreAction / PlayMultiple
  //
  // Export::DVDAuthor ? createXml () ?

  // This means N to 1 connections.
  // E.g. PlayMultiple, 1 ButtonObject one Target in serial
  //      Multiple Buttons to the same Video source ( chapters )

}

Area::Container::~Container ( )
{
  clearTargets ( );
  clearSources ( );
}

bool Area::ReadObject::readProjectFile  ( QDomNode &xmlNode )
{
  QDomElement theElement = xmlNode.toElement ( );
  QString     qsAttribute;

  qsAttribute = theElement.attributeNode ( LAYOUT_OBJECT_RECT ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )  {
    //QStringList list = QStringList::split ( ",", qsAttribute );   //ooo
    QStringList list = qsAttribute.split ( "," );                   //xxx
    if ( list.count ( ) == 4 )  {
      int x, y, w, h;
      x = list[0].toInt ( );
      y = list[1].toInt ( );
      w = list[2].toInt ( );
      h = list[3].toInt ( );
      m_rect = QRect ( x, y, w, h );
    }
  }
  m_qsObjectName = theElement.text ( );

  return true;
}

bool Area::ReadObject::writeProjectFile ( QDomElement &rootElement )
{
  QDomDocument xmlDoc = rootElement.ownerDocument   ( );
  QDomElement  rootContainer = xmlDoc.createElement ( LAYOUT_CONTAINER );
  int x, y, w, h;
  x = m_rect.x ( );
  y = m_rect.y ( );
  w = m_rect.width  ( );
  h = m_rect.height ( );

  rootContainer.setAttribute ( LAYOUT_OBJECT_RECT, QString ( "%1, %2, %3, %4" ).arg ( x ).arg ( y ).arg ( w ).arg ( h ) );
  //QDomText text = xmlDoc.createTextNode ( m_qsObjectName.utf8 ( ) );  //ooo
  QDomText text = xmlDoc.createTextNode ( m_qsObjectName.toUtf8 ( ) );  //xxx
  rootContainer.appendChild ( text );
  rootElement.appendChild   ( rootContainer );

  return true;
}

bool Area::Container::readProjectFile  ( QDomNode & )
{
  // Dummy function. Data is temporarily stored in ReadObjects
  return true;
}

bool Area::Container::writeProjectFile ( QDomElement &rootElement )
{
  QDomDocument xmlDoc = rootElement.ownerDocument   ( );
  QDomElement  rootContainer = xmlDoc.createElement ( LAYOUT_CONTAINER );
  QString qsObjectName;
  int x, y, w, h;
  x = y = 0;
  w = h = 200;
  if ( m_pObject )  {
    x = m_pObject->pos ( ).x ( );
    y = m_pObject->pos ( ).y ( );
    w = m_pObject->width  ( );
    h = m_pObject->height ( );
    qsObjectName = m_pObject->getObjectName ( );
  }

  rootContainer.setAttribute ( LAYOUT_OBJECT_RECT, QString ( "%1, %2, %3, %4" ).arg ( x ).arg ( y ).arg ( w ).arg ( h ) );
  //QDomText text = xmlDoc.createTextNode ( qsObjectName.utf8 ( ) );    //ooo
  QDomText text = xmlDoc.createTextNode ( qsObjectName.toUtf8 ( ) );    //xxx
  rootContainer.appendChild ( text );
  rootElement.appendChild   ( rootContainer );

  return true;
}

Object *Area::Container::object ( )
{
  return m_pObject;
}

QList<Area::Container *> &Area::Container::container ( )	//oxx
{
  return m_listContainer;
}

QList<Area::Container::Connection *> &Area::Container::targets ( )	//oxx
{
  return m_listTargets;
}

QList<Area::Container::Connection *> &Area::Container::sources ( )	//oxx
{
  return m_listSources;
}

QList<Area::Container::Connection *> Area::Container::side ( enSide side )	//oxx
{
  QList<Area::Container::Connection *> list;	//oxx

  Connection *pConnection = NULL;
  QList<Connection *>::iterator it = m_listTargets.begin ( );		//oxx
  while ( it != m_listTargets.end ( ) )  {
    pConnection = *it++;
    // First we get the connections FROM
    if ( pConnection->m_enSide[0] == side )
         list.append ( pConnection );
  }

  it = m_listSources.begin ( );
  while ( it != m_listSources.end ( ) )  {
    pConnection = *it++;
    // First we get the connections FROM
    if ( pConnection->m_enSide[1] == side )
         list.append ( pConnection );
  }

  return list;
}


QList<Area::Container::Connection *> Area::Container::copy ( )		//oxx
{
  QList<Connection *> list;		//oxx

  Connection *pConnection = NULL;
  QList<Connection *>::iterator it = m_listTargets.begin ( );		//oxx
  while ( it != m_listTargets.end ( ) )    {
    pConnection = new Connection  ( **it++ );
    list.append ( pConnection );
  }

  it = m_listSources.begin ( );
  while ( it != m_listSources.end ( ) )    {
    pConnection = new Connection  ( **it++ );
    list.append ( pConnection );
  }

  return list;
}

void Area::Container::addSource ( Area::Container::Connection *pSource )
{
  m_listSources.append ( pSource );
}

Area::Container::Connection *Area::Container::addTarget ( Area::Container *pTarget )
{
  Area::Container::Connection *pConnection = new Area::Container::Connection ( pTarget, this );
  m_listTargets.append ( pConnection );
  return pConnection;
}

Area::Container::Connection *Area::Container::getTarget ( Area::Container *pTarget )
{
  Connection *pProperties = NULL;
  QList<Connection *>::iterator it = m_listTargets.begin ( );		//oxx
  while ( it != m_listTargets.end ( ) )  {
    pProperties = *it++;
    if ( pProperties->m_pTarget == pTarget )
      return pProperties;
  }
  return NULL;
}

void Area::Container::clearTargets ( )
{
  QList<Connection *> list = m_listTargets;		//oxx
  Connection *pConnection = NULL;
  QList<Connection *>::iterator it = list.begin ( );	//oxx
  while ( it != list.end ( ) )  {
    pConnection = *it++;
    delete pConnection;
//    // At this point we could have removed a Connection from the list itself so be had better check
//    if ( m_listTargets.size ( ) < 1 )
//      break;
  }
  m_listTargets.clear ( );
}

void Area::Container::clearSources ( )
{
  QList<Connection *> list = m_listSources;		//oxx
  Connection *pConnection = NULL;
  QList<Connection *>::iterator it = list.begin ( );	//oxx
  while ( it != list.end ( ) )  {
    pConnection = *it++;
    delete pConnection;
//    // At this point we could have removed a Connection from the list itself so be had better check
//    if ( m_listSources.size ( ) < 1 )
//      break;
  }
  m_listSources.clear ( );
}

void Area::Container::clearConnections ( )
{
  clearTargets ( );
  clearSources ( );
}

int Area::Container::getSourceIdx ( Connection *pSource )
{
  int iCounter = 0;

  Connection *pConnection = NULL;
  Area::enSide whichSide = pSource->side ( 0 );
  QList<Connection *>::iterator it = m_listTargets.begin ( );		//oxx
  while (  it  != m_listTargets.end ( ) )  {
    pConnection = *it++;
    if  ( pConnection->side ( 0 ) != whichSide )
      continue;

    if  ( pConnection == pSource )
      return iCounter;
    iCounter++;
  }
  iCounter = 0;
  return iCounter;
}

int Area::Container::getTargetIdx ( Connection *pSource )
{
  int iCounter = 0;

  Connection *pConnection = NULL;
  Area::enSide whichSide = pSource->side ( 1 );
  QList<Connection *>::iterator it = m_listTargets.begin ( );		//oxx
  while (  it  != m_listTargets.end ( ) )  {
    pConnection = *it++;
    if  ( pConnection->side ( 0 ) != whichSide )
      continue;
    iCounter++;
  }

  it = m_listSources.begin ( );
  while (  it  != m_listSources.end ( ) )  {
    pConnection = *it++;
    if  ( pConnection->side ( 1 ) != whichSide )
      continue;
    if  ( pConnection == pSource )
      return iCounter;
    iCounter++;
  }

  iCounter = (int)m_listTargets.size ( );
  return iCounter;
}

void Area::Container::drawConnections ( QPainter &thePainter, Object *pExcept )
{  
  Container             *pTarget     = NULL;
  Container::Connection *pConnection = NULL;
  QList<Container::Connection *>::iterator it = m_listTargets.begin ( );	//oxx
  Area *pArea = (Area *)object ( )->parent ( );

  SimplePainter *pPainter = pArea->getPainter ( );
  LinearRouter  *pRouter  = pArea->getRouter  ( );

  QPoint startPos, endPos;
  // Only daw the outgoing connections
  while ( it != m_listTargets.end ( ) )  {
    pConnection = *it++;
    pTarget     = pConnection->target ( );
    if ( ( ! pTarget ) || ( pTarget->object ( ) == pExcept ) )
      continue;

    startPos =          getPos ( pConnection->side ( 0 ), pConnection->number ( 0 ) );
    endPos   = pTarget->getPos ( pConnection->side ( 1 ), pConnection->number ( 1 ) );
    
    // The routing class will hold different algorithms for routing the connection from point A to point B
    pConnection->setLines ( pRouter->route ( startPos, endPos, pConnection ) );
  }
  pPainter->draw ( thePainter, this, pExcept );   //ooo
}

void Area::Container::drawConnection ( QPainter &thePainter, Container::Connection *pConnection )
{  
  // Target / Source from the point of view of the Connection, not the Container
  Container *pSource, *pTarget;
//  QValueList<Container::Connection *>::iterator it = m_listSources.begin ( );
  QList<Container::Connection *>::iterator it = m_listSources.begin ( );	//xxx
  Area *pArea = (Area *)object ( )->parent ( );

  SimplePainter *pPainter = pArea->getPainter ( );
  LinearRouter  *pRouter  = pArea->getRouter  ( );

  QPoint startPos, endPos;
  // Only daw the outgoing connections
  pTarget = this;
  pSource = pConnection->source ( );
  if ( ! pSource )
    return;

  startPos = pSource->getPos ( pConnection->side ( 0 ), pConnection->number ( 0 ) );
  endPos   = pTarget->getPos ( pConnection->side ( 1 ), pConnection->number ( 1 ) );

  // The routing class will hold different algorithms for routing the connection from point A to point B
  pConnection->setLines ( pRouter->route ( startPos, endPos, pConnection ) );
  pPainter->draw ( thePainter, pConnection ); //ooo
}

QPoint Area::Container::getPos ( enSide whichSide, int iWhichNumber )
{
  QPoint pos;
  // Here we spread out the connections over the available width/height
  QList<Area::Container::Connection *> list = side ( whichSide );		//oxx
  float fDelta = 10.0f;
  int iX, iY, iSize;
  int iTotal = list.size ( ) + 1;
  iX = iY = 10;

  if ( whichSide == SIDE_BOTTOM )  {
    iSize = m_pObject->width  ( );
    fDelta = (float)iSize / iTotal;
    iX = (int)( fDelta * ( iWhichNumber + 1 ) + m_pObject->pos ( ).x ( ) );
    iY = m_pObject->pos ( ).y ( )  +  m_pObject->height ( ) - 2;
  }
  else  { // SIDE
    iSize  = m_pObject->height ( );
    fDelta = (float)iSize / iTotal;
    iY = (int)( fDelta  * ( iWhichNumber + 1 ) + m_pObject->pos ( ).y ( ) );
    iX = m_pObject->pos ( ).x ( ) - 2;
    if ( whichSide == SIDE_RIGHT )
       iX += m_pObject->width ( );
  }

  pos.setX ( iX );
  pos.setY ( iY );

  return pos;
}

//Area::Area ( QWidget *pParent, const char *pName, Qt::WFlags flags )      //ooo
Area::Area ( QWidget *pParent, const char *pName, Qt::WindowFlags flags )   //xxx
  //: Q3IconView ( pParent, pName, flags )  //ooo
  : QListView ( pParent )                   //xxx
{
  m_fZoom       = 1.0;
  m_bGrid       = false;
  m_pRouter     = new OthogonalRouter;
  m_pPainter    = new SimplePainter;
  m_iRouterType = 0;
  m_bMouseDown  = false;
  m_pActiveContainer = NULL;
  //setPaletteBackgroundColor ( QColor ( "#BACBE9" ) );     //oooo
  QPalette palette;								//xxx
  //palette.setBrush(QPalette::Base, QBrush(QColor ( "#BACBE9" ) ) );	//xxx
  palette.setBrush(QPalette::Base, QColor ( "#BACBE9" ) );	//xxx
  this->setPalette(palette);						//xxx
}

Area::~Area ( )
{
  clearContainer ( );
}

bool Area::readProjectFile ( QDomNode &xmlNode )
{
  QDomElement theElement = xmlNode.toElement ( );
  QString     qsAttribute, qsTagName, qsZoom;
  QDomNode    theNode;

  qsZoom = theElement.attributeNode ( LAYOUT_ZOOM ).value ( );
  if ( ! qsZoom.isEmpty ( ) )
    m_fZoom = qsZoom.toFloat ( );

  qsAttribute = theElement.attributeNode ( LAYOUT_GRID ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )  {
    //QStringList list = QStringList::split ( ",", qsAttribute );   //ooo
    QStringList list = qsAttribute.split ( "," );                   //xxx
    if ( list.count ( ) == 2 )  {
      m_sizeGrid.setWidth  ( list[0].toInt ( ) );
      m_sizeGrid.setHeight ( list[1].toInt ( ) );
      m_bGrid = true;
    }
  }
  qsAttribute = theElement.attributeNode ( LAYOUT_TYPE ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )  {
    if ( qsAttribute  == "Ortho" )
      setRouterType ( 0 );
    else if ( qsAttribute  == "Star" )
      setRouterType ( 1 );
    else if ( qsAttribute  == "Glow" )
      setRouterType ( 2 );
  }

  ReadObject *pReadObject = NULL;
  theNode = theElement.firstChild  ( );
  while ( ! theNode.isNull ( ) )     {
    qsTagName = theNode.toElement  ( ).tagName ( );
    if ( LAYOUT_CONTAINER == qsTagName )  {
      pReadObject = new ReadObject ( );
      pReadObject->readProjectFile ( theNode );
      m_listOfReadObjects.append   ( pReadObject );
    }
    theNode = theNode.nextSibling  ( );
  }
  setZoom ( m_fZoom );
  return true;
}

bool Area::writeProjectFile ( QDomElement &rootElement )
{
  // Store : Zoom, Object Position, PanPos
  QDomDocument xmlDoc        = rootElement.ownerDocument ( );
  QDomElement  rootDVDLayout = xmlDoc.createElement ( DVDAUTHOR_DVDLAYOUT );
  rootDVDLayout.setAttribute ( LAYOUT_ZOOM, QString ( "%1" ).arg ( m_fZoom ) );
  rootElement.appendChild    ( rootDVDLayout );

  if ( m_bGrid )
    rootDVDLayout.setAttribute ( LAYOUT_GRID, QString ( "%1, %2" ).arg ( m_sizeGrid.width ( ) ).arg ( m_sizeGrid.height ( ) ) );

  if ( m_iRouterType == 0 )
    rootDVDLayout.setAttribute ( LAYOUT_TYPE, QString ( "Ortho" ) );
  else if ( m_iRouterType == 1 )
    rootDVDLayout.setAttribute ( LAYOUT_TYPE, QString ( "Star"  ) );
  else if ( m_iRouterType == 2 )
    rootDVDLayout.setAttribute ( LAYOUT_TYPE, QString ( "Glow"  ) );


  // A little tricky here. After we read a project file, the files is written right back to autosave
  // but at this time the Container do not yet exist. So instaed be staore the ReadObject list back.
  if ( m_listOfContainer.size ( ) < 1 )  {
    ReadObject *pReadObject = NULL;
    QList<ReadObject *>::iterator it = m_listOfReadObjects.begin ( );		//oxx
    while ( it != m_listOfReadObjects.end ( ) )  {
      pReadObject = *it++;
      if ( ! pReadObject->writeProjectFile ( rootDVDLayout) )
        return false;
    }
  }
  else  {
    Container *pContainer = NULL;
    QList<Container *>::iterator it = m_listOfContainer.begin ( );	//oxx
    while ( it != m_listOfContainer.end ( ) )  {
      pContainer = *it++;
      if ( ! pContainer->writeProjectFile ( rootDVDLayout) )
        return false;
    }
  }

  return true;
}

void Area::refresh  ( )
{
  createConnections ( );
  drawConnections   ( );
}

void Area::deleteContainer ( Object  *pObject )
{
  // An Object was deleted, Now lets remove the Container and all Connections.
  Container *pContainer = findContainerFromObject ( pObject );
  if ( ! pContainer )
    return;

  if ( m_pActiveContainer == pContainer )  {
    Container::Connection   *pConnection = NULL;
    QList<Container::Connection *>::iterator it = m_listActiveLines.begin ( );		//oxx
    while ( it != m_listActiveLines.end ( ) )  {
      pConnection = *it++;
      delete pConnection;
    }
    m_listActiveLines.clear ( );
    m_pActiveContainer = NULL;
  }

  //pContainer->clearConnections  ( );
  //m_listOfContainer.remove ( pContainer );    //ooo
  m_listOfContainer.removeOne ( pContainer );   //xxx
  delete pContainer;

  // At this point we have removed all Connections plus the offending Container.
  refresh ( );
}

void Area::setActiveObject ( Object *pObject )
{
  Container::Connection *pConnection = NULL;
  QList<Container::Connection *>::iterator it = m_listActiveLines.begin ( );		//oxx
  while ( it != m_listActiveLines.end ( ) )  {
    pConnection = *it++;
    delete pConnection;
  }
  m_listActiveLines.clear ( );
  m_pActiveContainer = findContainerFromObject ( pObject );

  // Check if we have the slow method ...
  SimplePainter *pPainter = getPainter ( );
  pPainter->setFastDraw ( pObject ? true : false );

  drawConnections ( );
}

void Area::moveActiveObject ( )
{
  if ( m_pActiveContainer )
   drawContainer ( m_pActiveContainer );
}

void Area::slotRedrawAll ( )
{
  // Call DVDLayout::Gui::slotRefreshAll ()
  drawConnections ( );
}

void Area::setRouterType ( int iWhich )
{
  if ( m_pRouter )
    delete m_pRouter;

  if ( m_pPainter )
    delete m_pPainter;

  if ( ( iWhich == 0 ) || ( iWhich == 2 ) )
    m_pRouter = new OthogonalRouter;
  else
    m_pRouter = new LinearRouter;

  if ( iWhich == 2 )
    m_pPainter = new NicePainter;
  else
    m_pPainter = new SimplePainter;

  m_iRouterType = iWhich;
  refresh ( );
}

int Area::routerType ( )
{
  return m_iRouterType;
}

LinearRouter *Area::getRouter ( )
{
  return m_pRouter;
}

SimplePainter *Area::getPainter ( )
{
  return m_pPainter;
}

void Area::setZoom ( float fZoom )
{
  m_fZoom = fZoom;
}

float Area::getZoom ( )
{
  return m_fZoom;
}

void Area::setGrid ( bool bGrid, int iX, int iY )
{
  m_bGrid = bGrid;
  m_sizeGrid.setWidth  ( iX );
  m_sizeGrid.setHeight ( iY );
}

bool Area::getGrid ( int &iX, int &iY )
{
  iX = m_sizeGrid.width  ( );
  iY = m_sizeGrid.height ( );
  return m_bGrid;
}

void Area::drawGrid ( QPainter & /* thePainter */ )
{
  if ( ! m_bGrid )
    return;
}

void Area::print ( )
{
  QString qsHeader   = Global::qsProjectName + tr ( " - DVD Layout" );
  QPixmap screenshot = QPixmap::grabWidget ( this );
  PrintPreview preview ( this, qsHeader );
  preview.setPixmap    (     screenshot );
  preview.exec ( );
}

void Area::contentsMousePressEvent ( QMouseEvent *pMouseEvent )
{
  SimplePainter *pPainter = getPainter ( );
  pPainter->setFastDraw ( true );
  m_bMouseDown = true;
  m_prevPos    = pMouseEvent->pos ( );

  // something to clear the background please ....
}

void Area::contentsMouseReleaseEvent ( QMouseEvent * )
{
  m_bMouseDown = false;
  m_prevPos    = QPoint ( 0, 0 );
  SimplePainter *pPainter = getPainter ( );
  pPainter->setFastDraw ( false );
  drawConnections ( );
}

void Area::contentsMouseMoveEvent ( QMouseEvent *pMouseEvent )
{
  if ( ! m_bMouseDown )
    return;

  QPoint pos = pMouseEvent->pos ( );
  if ( ( pos.x ( ) < 0 ) || ( pos.y ( ) < 0 ) )
    return;

  int dX = m_prevPos.x ( ) - pos.x ( );
  int dY = m_prevPos.y ( ) - pos.y ( );
  moveAll ( dX, dY );

  m_prevPos = pos;
}

void Area::moveAll ( int iDeltaX, int iDeltaY )
{
  int x, y;
  Object    *pObject    = NULL;
  Container *pContainer = NULL;
  QList<Container  *>::iterator it = m_listOfContainer.begin ( );	//oxx
  while ( it != m_listOfContainer.end ( ) )  {
    pContainer = *it++;
    pObject = pContainer->object ( );
    x = pObject->pos ( ).x ( ) - iDeltaX;
    y = pObject->pos ( ).y ( ) - iDeltaY;
    pObject->move ( x, y );
  }
  drawConnections ( );
}

void Area::drawContainer ( QPainter &thePainter, Object *pExcept )
{
  Container *pContainer = NULL;
  // For each Container
  QList<Container *>::iterator it = m_listOfContainer.begin ( );	//oxx
  while ( it != m_listOfContainer.end ( ) )  {
    pContainer = *it++;
    if ( pExcept && pContainer->object ( ) == pExcept )
      continue;
    // We'll draw each outgoing connection to another container
    pContainer->drawConnections ( thePainter, pExcept );
  }
}

void Area::drawContainer ( Object *pObject )
{
  Container *pContainer = findContainerFromObject ( pObject );
  if ( pContainer )
       drawContainer ( pContainer );
}

void Area::drawContainer ( Container *pContainer )
{
  // Here we render only the connections to, and from this Container
  // All other connectios are already part of the paletteBackgroundPixmap()
  if ( ! pContainer )
    return;

  // First we erase the previous temp lines ...
  //QPen     thePen     ( backgroundColor ( ), 2 ); //oooo
  //QPen     thePen     ( QColor ( 1, 1, 1 ), 2 );    //xxx
  QPen     thePen     ( QColor ( 186, 203, 233 ), 2 );    //xxx
  QPainter thePainter ( viewport ( ) );
  thePainter.setPen   ( thePen );
  //thePainter.setBrush ( backgroundColor ( ) );    //oooo
  //thePainter.setBrush ( QColor ( 1, 1, 1 ) );       //xxx
  thePainter.setBrush ( QColor ( 186, 203, 233 ) );       //xxx
  SimplePainter painter;

  Container::Connection *pConnection = NULL;
  QList<Container::Connection *>::iterator it = m_listActiveLines.begin ( );		//oxx
  while ( it != m_listActiveLines.end ( ) )  {
    pConnection = *it++;
    painter.drawLines ( thePainter, pConnection->lines ( ) );
    delete pConnection;
  }
  m_listActiveLines.clear ( );
  m_listActiveLines = pContainer->copy   ( );

  // Now we can draw the outgoing connections
  pContainer->drawConnections ( thePainter );

  // and finally draw this incomming ( source ) connecitons
  it = pContainer->sources ( ).begin ( );
  while ( it != pContainer->sources ( ).end ( ) )  {
    pConnection = *it++;
    pContainer->drawConnection ( thePainter, pConnection );
  }
}

void Area::clearContainer ( )
{
  Container *pContainer = NULL;
  QList <Container *>::iterator it = m_listOfContainer.begin ( );	//oxx
  while ( it != m_listOfContainer.end ( ) )  {
    pContainer = *it++;
    delete pContainer;
  }
  m_listOfContainer.clear ( );
}

void Area::clearPixmap ( )
{
  m_pixmap = QPixmap ( width ( ), height ( ) );
  //m_pixmap.fill ( paletteBackgroundColor ( ) );   //oooo
  m_pixmap.fill ( QColor(170,180,255) );   //xxx
}

void Area::drawConnections ( )
{
  // Some basic preparations ...
  Object *pObject = NULL;
  if ( m_pActiveContainer )
    pObject = m_pActiveContainer->object ( );

  clearPixmap      (           );
  QPainter painter ( &m_pixmap );
  drawGrid         (   painter );
  drawContainer    (   painter, pObject );
  //setPaletteBackgroundPixmap ( m_pixmap );    //oooo
  QPalette palette;								//xxx
  //palette.setBrush(QPalette::Window, QBrush(m_pixmap));				//xxx
  palette.setBrush(QPalette::Base, m_pixmap);				//xxx
  this->setPalette(palette);					//xxx
}

void Area::createConnections ( )
{
  // This function will recreate the connections of the existing Container
}

void Area::createConnections ( QList<Object *> &list )		//oxx
{ 
  // This function ( called from Gui::refresh () ) will delete all Container
  // And re-create them to handle the Connections between Objects.

  // So first we delete all existing Conainer objects.
  clearContainer ( );

  ///////////////////////////////////////////////////////////////////////////////////////
  //                                                                                   //
  // Now here is the main routine to drw the connections between Menus, and Movies.    //
  // o Each Object only draws its own connections TOWARDS another object ( or itself ) //
  // o Menu to Menu   lines are RED    ( From Side   to Side   )                       //
  // o Menu to Movie  lines are BLUE   ( From bottom to Side   )                       //
  // o Movie to Movie lines are Green  ( From bottom to bottom )                       //
  // o Movie to Menu  lines are ORANGE ( From side   to bottom )                       //
  //                                                                                   //
  ///////////////////////////////////////////////////////////////////////////////////////

  // First we collect information about the connections and create a cache of information
  Object    *pObject    = NULL;
  Container *pContainer = NULL;

  // First we populate the Container Objects.
  QList<Object *>::iterator it = list.begin ( );	//oxx
  while ( it != list.end ( ) ) { 
    pObject = *it++;
    pContainer = new Container ( pObject );
    m_listOfContainer.append   ( pContainer );
  }

  // This run through we determine the X/Y pos of the connection points
  QList<Container *>::iterator itCon = m_listOfContainer.begin ( );		//oxx
  while ( itCon != m_listOfContainer.end ( ) )  {
    pContainer = *itCon++;
    if      ( pContainer->object  ( )->type ( ) == Object::TYPE_MOVIE )
      findMovieTargets ( pContainer );
    else if ( pContainer->object  ( )->type ( ) == Object::TYPE_MENU  )
      findMenuTargets  ( pContainer );

    setReadParams ( pContainer );
  }
  
  // If this MovieObject does not have any targets, then we assign the sub menu as its target
  // Meaning, after playing the movie, the DVD will jump back to the sub menu
  itCon = m_listOfContainer.begin ( );
  while ( itCon != m_listOfContainer.end ( ) )  {
    pContainer = *itCon++;
    if ( ( pContainer->object ( )->type ( ) == Object::TYPE_MOVIE ) && ( pContainer->container ( ).size ( ) < 1 ) ) {
      DVDMenu   *pDVDMenu = Global::pApp->getVMGMenu ( );
      Container *pTarget  = findContainerFromPtr ( pDVDMenu );
      if ( pTarget )
        pContainer->container ( ).append ( pTarget );
    }
  }

  // The following functin will remove double connection from one container to another.
  // These double connections are of no improtance to the DVDLayout itself.
  cleanListOfContainer ( );
//for (int x=0;x<m_listOfContainer.count();x++) { if (m_listOfContainer[x]) printf ("%s::%d > %d = cnt<%d>\n",__FILE__,__LINE__,x,m_listOfContainer[x]->container().count()); }
  
  // And finally we go and assign the objects inside the Container objects to Left, Right, and Bottom
  Container *pTarget   = NULL;
  QList<Container *> listContainer;		//oxx
  QList<Container *>::iterator it2;		//oxx
  itCon = m_listOfContainer.begin ( );
  while ( itCon != m_listOfContainer.end ( ) )  {
    pContainer    = *itCon++;
    listContainer = pContainer->container ( );
    it2 = listContainer.begin ( );
    while ( it2  != listContainer.end ( ) )  {
      pTarget     = *it2++;
      pContainer->addTarget ( pTarget );
    }
    createConnections ( pContainer );
  }

  // Finally after we assigned all sides to all Container
  // we can figure out the idx of the connection on each side.
  Container::Connection *pConnection = NULL;
  QList<Container::Connection *>::iterator it3;		//oxx
  itCon = m_listOfContainer.begin ( );
  while ( itCon != m_listOfContainer.end ( ) )  {
    pContainer   = *itCon++;
    if  ( pContainer->sources ( ).size  ( ) < 1 )  {
      if ( pContainer->object ( )->type ( ) == Object::TYPE_MOVIE )  {
        // For Movies which do not have any Connection ( E.g. button pointing to it )
        // we'll remove all source connections as well.
        it3 = pContainer->targets ( ).begin ( );
        while ( it3 != pContainer->targets  ( ).end ( ) )  {
          pConnection = *it3++;
          delete pConnection;
        }
        pContainer->sources ( ).clear ( );
      }
    }
    it3 = pContainer->targets ( ).begin ( );
    while ( it3 != pContainer->targets ( ).end ( ) )  {
      pConnection = *it3++;
      // Some special handling if a button in a menu points to the same menu.
      if ( ( pConnection->target ( ) == pConnection->source ( ) ) && 
           ( pConnection->target ( )->object ( )->type ( ) == Object::TYPE_MENU ) )  {
        delete pConnection;
        continue;
      }

      pConnection->m_iConNr[0] = pConnection->source ( )->getSourceIdx ( pConnection );
      pConnection->m_iConNr[1] = pConnection->target ( )->getTargetIdx ( pConnection );
    }
  }
}

void Area::cleanListOfContainer ( )
{
  Container *pContainer = NULL;
  Container *pTarget    = NULL;
  QList<Container *> tempList;						//oxx
  QList<Container *>::iterator it = m_listOfContainer.end ( );		//oxx
  QList<Container *>::iterator it2;					//oxx
  while ( --it != m_listOfContainer.begin ( ) )  {
    pContainer = *it;
    it2 = pContainer->container ( ).begin ( );
    while (  it2 != pContainer->container ( ).end ( ) )  {
      pTarget = *it2++;
      //tempList.remove   ( pTarget );  //ooo
      tempList.removeOne   ( pTarget ); //xxx
      if ( pTarget  != pContainer )  // Dont want to show connections from one to itself.
        tempList.append ( pTarget );
    }
    pContainer->container ( ) = tempList;
    tempList.clear ( );
  }
}

void Area::createConnections ( Container *pContainer )
{
  QRgb red    = 0xFF0000;
  QRgb green  = 0x008000;
  QRgb blue   = 0x0000FF;
  QRgb orange = 0xFF9632;
  QRgb color = red;
  Container *pTarget   = NULL;
  Container::Connection* pConnection = NULL;
  QList<Container *> listContainer;		//oxx
  QList<Container *>::iterator it2;		//oxx
//  QValueList<Container *>::iterator itCon = m_listOfContainer.begin ( );
  QList<Container *>::iterator itCon = m_listOfContainer.begin ( );		//xxx

  listContainer = pContainer->container ( );
  int iPosX     = pContainer->object    ( )->pos ( ).x ( );

  it2 = listContainer.begin ( );
  while ( it2  != listContainer.end ( ) )  {
    pTarget     = *it2++;
    //pConnection = pContainer->addTarget ( pTarget );
    pConnection = pContainer->getTarget ( pTarget );
    if ( ! pConnection )
           pConnection = pContainer->addTarget ( pTarget );

    pConnection->m_enSide[0] = SIDE_RIGHT;
    pConnection->m_enSide[1] = SIDE_RIGHT;
    if (  pContainer->object ( )->type ( ) == Object::TYPE_MOVIE )  {
      if (   pTarget->object ( )->type ( ) == Object::TYPE_MOVIE )  {
        pConnection->m_enSide[0] = SIDE_BOTTOM;
        pConnection->m_enSide[1] = SIDE_BOTTOM;
        color = green;
      }
      else if ( pTarget->object ( )->type ( ) == Object::TYPE_MENU )  {
        pConnection->m_enSide[1] = SIDE_BOTTOM;
        color = orange;
      }
    }
    else if ( pContainer->object ( )->type ( ) == Object::TYPE_MENU )  {
      if ( pTarget->object ( )->type ( ) == Object::TYPE_MOVIE )  {
        pConnection->m_enSide[0] = SIDE_BOTTOM;
        color = blue;
      }
      else if ( pTarget->object ( )->type ( ) == Object::TYPE_MENU )
        color = red;       // side to side
    }

    // Now I have color and bSide ...
    pConnection->m_color = color;
    pTarget->addSource ( pConnection );

    // now we have to see if the target is left or right of this Object
    if ( pTarget->object ( )->pos ( ).x ( ) > iPosX )  {
      if ( pConnection->m_enSide[1] == SIDE_RIGHT )
           pConnection->m_enSide[1]  = SIDE_LEFT;
    }
    else  {
      if ( pConnection->m_enSide[0] == SIDE_RIGHT )
           pConnection->m_enSide[0]  = SIDE_LEFT;
    }
  }
}

void Area::setReadParams ( Container *pContainer )
{
  if ( m_listOfReadObjects.size ( ) < 1 )
    return;

  Object *pObject = pContainer->object ( );
  if ( ! pObject )
    return;

  ReadObject *pReadObject = NULL;
  QList<ReadObject *>::iterator it = m_listOfReadObjects.begin ( );	//oxx
  while ( it != m_listOfReadObjects.end ( ) )  {
    pReadObject = *it++;
    if ( pReadObject->m_qsObjectName == pObject->getObjectName ( ) )  {
      pObject->setZoom ( m_fZoom );
      pObject->move    ( pReadObject->m_rect.x ( ), pReadObject->m_rect.y ( ) );
      //m_listOfReadObjects.remove (  pReadObject  );   //ooo
      m_listOfReadObjects.removeOne (  pReadObject  );  //xxx
      delete pReadObject;
      return;
    }
  }
}

void Area::findMovieTargets ( Container *pContainer )
{
  // This function will get the information from the SourceFileEntry, and SourceFileInfo's
  // and find the associated objects
  if ( ! pContainer )
    return;

  Container       *pTarget = NULL;
  SourceFileEntry *pEntry  = (SourceFileEntry *)pContainer->object ( )->getObject ( );
  if ( ! pEntry )
    return;

  // Not currently handling Audio, and Subtitles.
  // SourceFileEntry: qsPre / qsPost
  //                  Subtitles / Audio
  // SourceFileInfo:  qsPreCommands / qsPostCommands
  //                  Subtitles / Audio

  // If ( qsPost == "" ) then jump back to menu but this is handled by the button Object of the Menu it is assigned to.
  pTarget = findContainerFromAction ( pEntry->qsPost );
  if ( pTarget )
    pContainer->container ( ).append ( pTarget );

  SourceFileInfo *pInfo = NULL;
  QList<SourceFileInfo *>::iterator it = pEntry->listFileInfos.begin ( );	//oxx
  while ( it != pEntry->listFileInfos.end ( ) )  {
    pInfo   = *it++;
    pTarget = findContainerFromAction ( pInfo->qsPostCommand );
    if ( pTarget )
      pContainer->container ( ).append ( pTarget );
  }
}

void Area::findMenuTargets ( Container *pContainer )    //ooo-bug
{
  if ( ! pContainer )
    return;
  // DVDMenu: qsPre / qsPost
  //          QValueList<ButtonObject *>        &getButtons          ( );
  //          QValueList<SourceFileEntry *>     &getSourceFileEntries( );
  //          QValueList<SourceFileInfo *>      &getSourceFileInfos  ( );
  //          SourceFileEntry    *getEntryFromButtonAction ( QString & );
  //          SourceFileInfo     *getInfoFromButtonAction  ( QString & );
  //          DVDMenu            *getMenuFromButtonAction  ( QString & );
  // ButtonObject:  qsAction / qsPreAction / PlayMultiple
  Container       *pTarget = NULL;
  SourceFileEntry *pEntry  = NULL;
  DVDMenu *pMenu  = (DVDMenu *) pContainer->object  ( )->getObject ( );
  QList<SourceFileEntry *> list = pMenu->getSourceFileEntries ( false ); // Excluding the PlayMultiple entries		//oxx
  QList<SourceFileEntry *>::iterator it = list.begin ( );								//oxx
  while ( it != list.end ( ) )  {
    pEntry = *it++;
    pTarget = findContainerFromPtr ( pEntry );
    if ( pTarget )  {
      pContainer->container ( ).append ( pTarget );
    }
  }

  // preAction ( );
  // Lets handle PlayMultiple...
  Container    *pCurrentTarget = NULL;
  DVDMenu      *pOtherMenu     = NULL;
  ButtonObject *pButton        = NULL;
//  int         iSelfCounter;
  QString     strMultiple;
  QStringList listMultiple;
  QStringList::iterator itMultiple;
  QList<ButtonObject *> listButtons = pMenu->getButtons ( );			//oxx
  QList<ButtonObject *>::iterator itButtons = listButtons.begin ( );		//oxx
  while ( itButtons != listButtons.end ( ) )  {
    pButton = *itButtons++;
    listMultiple   = pButton->getPlayMultipleList ( );
    itMultiple     = listMultiple.begin ( );
    pCurrentTarget = NULL;
//    iSelfCounter   = 0;
    while ( itMultiple != listMultiple.end ( ) )  {
      strMultiple  = *itMultiple++;
      pTarget = findContainerFromAction ( strMultiple );
      if ( ( pCurrentTarget && pTarget ) && ( pCurrentTarget != pTarget ) )
             pCurrentTarget->container ( ).append ( pTarget );
/* Does cause issues with multiple SFEs in a MultiSelection.
      if ( pCurrentTarget && pTarget )  {
        // We only want to see one pointer to itself ( If required )
        if ( ( iSelfCounter < 1 ) || ( pCurrentTarget != pTarget ) )
             pCurrentTarget->container ( ).append ( pTarget );
        if ( pCurrentTarget != pTarget )
             iSelfCounter++;
      }
*/
      pCurrentTarget = pTarget;
    }
  }

  // Finally we can handle the Button Actions to other Menus.
  itButtons =  listButtons.begin ( );
  while ( itButtons != listButtons.end ( ) )  {
    pButton = *itButtons++;

    pOtherMenu = pMenu->getMenuFromButtonAction ( pButton->action ( ) );
    pTarget = findContainerFromPtr ( pOtherMenu );
    if ( pTarget )
      pContainer->container ( ).append ( pTarget );
  }
}

Area::Container *Area::findContainerFromAction ( QString &qsAction )
{
  void *p=NULL;
  if ( qsAction.isEmpty ( ) )
    return NULL;

  //QStringList stringList = QStringList::split ( STRING_SEPARATOR, qsAction );     //ooo
  QStringList stringList = qsAction.split ( STRING_SEPARATOR );                     //xxx
  if ( stringList.count ( ) < 1 )  {  // Maybe something like " call vmgm menu 1; "
    //stringList = QStringList::split ( " ", qsAction );    //ooo
    stringList = qsAction.split ( " " );                    //xxx
    //if ( stringList[1].lower ( ) == "vmgm" )  //ooo
    if ( stringList[1].toLower ( ) == "vmgm" )  //xxx
      p = Global::pApp->getVMGMenu ( );
    else
      return NULL;
  }
  else if ( stringList.count ( ) == 2 )  // To DVDMenu
    p = Global::pApp->getSubMenuByName ( stringList[1] );
  else if ( stringList.count ( ) == 3 )  // To SourceFileEntry
    p = Global::pApp->getSourceEntryByDisplayName ( stringList[1] );
  else if ( stringList.count ( ) == 4 )  // To SourceFileInfo
    p = Global::pApp->getSourceEntryByDisplayName ( stringList[1] );

  return findContainerFromPtr ( p );
}

Area::Container *Area::findContainerFromPtr ( void *p )
{
  if ( ! p )
    return NULL;

  Object    *pObject    = NULL;
  Container *pContainer = NULL;
  QList<Container *>::iterator it = m_listOfContainer.begin ( );	//oxx
  while ( it  != m_listOfContainer.end ( ) )  {
    pContainer = *it++;
    pObject    = pContainer->object    ( );
    if ( pObject && pObject->getObject ( ) == p )
      return pContainer;
  }
  return NULL;
}

Area::Container *Area::findContainerFromObject ( Object *pObject )
{
  if ( ! pObject )
    return NULL;

  Container *pContainer = NULL;
  QList<Container *>::iterator itCon = m_listOfContainer.begin ( );		//oxx
  while ( itCon !=  m_listOfContainer.end  ( ) )  {
    pContainer   = *itCon++;
    if ( pObject == pContainer->object ( ) )
      return pContainer;
  }
  return NULL;
}

}; // End namespace DVDLayout

