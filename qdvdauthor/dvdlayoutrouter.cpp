/***************************************************************************
    dvdlayoutrouter.cpp
                             -------------------
    Class DVDLayout::Gui
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This is a sub-project to 'Q' DVD-Author to display the current DVD Layout
   
****************************************************************************/

#include "dvdlayoutgui.h"
#include "dvdlayoutrouter.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

namespace DVDLayout
{

LinearRouter::LinearRouter ( )
{
}

LinearRouter::~LinearRouter ( )
{
}

QList<Area::PieceOfLine *> LinearRouter::route ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )	//oxx
{
  QList<Area::PieceOfLine *> list;	//oxx
  if ( ! pConnection )
    return list;

  Area::enSide startSide = pConnection->side ( 0 );
  Area::enSide endSide   = pConnection->side ( 1 );

  const int iSPACE = 10; // distance from Container
  QPoint pos[2];
  pos[0] = startPos;
  if ( startSide == Area::SIDE_BOTTOM )
    pos[1] = QPoint ( startPos.x ( ), startPos.y ( ) + iSPACE );
  else if ( startSide == Area::SIDE_LEFT )
    pos[1] = QPoint ( startPos.x ( ) - iSPACE, startPos.y ( ) );
  else if ( startSide == Area::SIDE_RIGHT )
    pos[1] = QPoint ( startPos.x ( ) + iSPACE, startPos.y ( ) );
  list.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );

  pos[0] = pos[1];
  if ( endSide == Area::SIDE_BOTTOM )
    pos[1] = QPoint (  endPos.x ( ), endPos.y ( ) + iSPACE );
  else if ( endSide == Area::SIDE_LEFT  )
    pos[1] = QPoint (  endPos.x ( ) - iSPACE, endPos.y ( ) );
  else if ( endSide == Area::SIDE_RIGHT )
    pos[1] = QPoint (  endPos.x ( ) + iSPACE, endPos.y ( ) );
  list.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );

  list.append ( new Area::PieceOfLine ( pos[1], endPos ) );
  return list;
}


OthogonalRouter::OthogonalRouter ( )
  : iSPACE ( 10 ) // distance from Container
  , iDELTA (  5 )
{
}

OthogonalRouter::~OthogonalRouter ( )
{
}

QList<Area::PieceOfLine *> OthogonalRouter::route ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )	//oxx
{
  m_listLines.clear ( );
  QPoint pos[2];
  if ( ! pConnection )
    return m_listLines;

  Area::enSide startSide = pConnection->side ( 0 );
  Area::enSide endSide   = pConnection->side ( 1 );

  int iDelta = pConnection->m_iConNr[0] * iDELTA + iSPACE;
  // This is the starting off which separates the onnection 
  // from the Container by 10 pixels.
  pos[0] = startPos;
  if ( startSide == Area::SIDE_BOTTOM )
    pos[1] = QPoint ( startPos.x ( ), startPos.y ( ) + iDelta );
  else if ( startSide == Area::SIDE_LEFT )
    pos[1] = QPoint ( startPos.x ( ) - iDelta, startPos.y ( ) );
  else if ( startSide == Area::SIDE_RIGHT )
    pos[1] = QPoint ( startPos.x ( ) + iDelta, startPos.y ( ) );
  m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );

  // The final point before entering the Container.
  iDelta = pConnection->m_iConNr[1] * iDELTA + iSPACE;
  pos[0] = pos[1];
  if ( endSide == Area::SIDE_BOTTOM )
    pos[1] = QPoint (  endPos.x ( ), endPos.y ( ) + iDelta );
  else if ( endSide == Area::SIDE_LEFT  )
    pos[1] = QPoint (  endPos.x ( ) - iDelta, endPos.y ( ) );
  else if ( endSide == Area::SIDE_RIGHT )
    pos[1] = QPoint (  endPos.x ( ) + iDelta, endPos.y ( ) );

  if ( startSide == Area::SIDE_BOTTOM )
    pos[0] = routeBottom ( pos[0], pos[1], pConnection );
  else
    pos[0] = routeSide   ( pos[0], pos[1], pConnection );

  pConnection->m_iConNr[0] = pConnection->source ( )->getSourceIdx ( pConnection );
  pConnection->m_iConNr[1] = pConnection->target ( )->getTargetIdx ( pConnection );

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return m_listLines;
}

QPoint OthogonalRouter::routeBottom ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  if ( ! pConnection )
    return endPos;

  Area::enSide endSide   = pConnection->side ( 1 );

  if ( endSide == Area::SIDE_BOTTOM )
    return routeBottomToBottom ( startPos, endPos, pConnection );

  // Now if the Source Container is below the Taret Container we need to follow a different pattern
  if ( startPos.y ( ) > endPos.y ( ) )
    return routeBottomBelowTarget ( startPos, endPos, pConnection );

  QPoint pos[2];
  bool bThreeLiner = false;
  int iDelta = pConnection->m_iConNr[1] * iDELTA + iSPACE;
  int iCenterXPos, iObjectXPos  = startPos.x ( );

  pos[0] = startPos;
  pos[1] = endPos;
  // Here we handle routing from Bottom to either side
  // E.g. From a Menu to a Movie Container
  // Lets get the propper xPos of the Target Object.
  Object *pTargetObject = pConnection->target ( )->object ( );
  iObjectXPos = pTargetObject->pos ( ).x ( );
  iCenterXPos = iObjectXPos + (int)( (float)pTargetObject->width ( ) / 2.0 );
  if ( startPos.x ( ) > iCenterXPos )  {
    pConnection->m_enSide[1] = Area::SIDE_RIGHT;
    if ( startPos.x ( ) < iObjectXPos + pTargetObject->width ( ) + iDelta )
      bThreeLiner = true;
  }
  else  {
    pConnection->m_enSide[1] = Area::SIDE_LEFT;
    if ( startPos.x ( ) > iObjectXPos - iDelta )
      bThreeLiner = true;
  }

  if ( bThreeLiner )  {
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1] = endPos;
  }
  pos[1].setX ( pos[0].x ( ) );
  m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
  pos[0] = pos[1];

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeBottomBelowTarget ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  int iDelta = pConnection->m_iConNr[1] * iDELTA + iSPACE;
//  int iCenterXPos;
//  int iObjectXPos  = startPos.x ( );
  if ( ! pConnection )
    return endPos;

  pos[0] = startPos;
  pos[1] = endPos;

  // Here we handle routing from Bottom to either side
  // E.g. From a Menu to a Movie Container
  // Lets get the propper xPos of the Target Object.
  Object *pTargetObject = pConnection->target ( )->object ( );
  Object *pSourceObject = pConnection->source ( )->object ( );
//  iObjectXPos = pTargetObject->pos ( ).x ( );
//  iCenterXPos = iObjectXPos + (int)( (float)pTargetObject->width ( ) / 2.0 );
  int iSourceRight = pSourceObject->pos ( ).x ( ) + pSourceObject->width ( );

  if ( endPos.x ( ) > pSourceObject->pos ( ).x ( ) + pSourceObject->width ( ) + iSPACE )  {
    pConnection->m_enSide[1] = Area::SIDE_LEFT;
    // We are all the way to the left...
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1] = endPos;
  }
  else if ( pSourceObject->pos ( ).x ( ) <= pTargetObject->pos ( ).x ( ) )  {
    pConnection->m_enSide[1] = Area::SIDE_LEFT;
    // We are in between the left and the center
    pos[1].setY ( pos[0].y ( ) );
    pos[1].setX ( pSourceObject->pos ( ).x ( ) - iDelta );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];

    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1] = endPos;
  }
  else if ( ( iSourceRight + iSPACE > endPos.x ( ) ) && ( pSourceObject->pos ( ).x ( ) - iSPACE < endPos.x ( ) ) ) {
    pConnection->m_enSide[1] = Area::SIDE_RIGHT;
    // We are in between the center and the right
    pos[1].setY ( pos[0].y ( ) );
    pos[1].setX ( pSourceObject->pos ( ).x ( ) + pSourceObject->width ( ) + iDelta );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];

    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1] = endPos;
  }
  else  {
    pConnection->m_enSide[1] = Area::SIDE_RIGHT;
    // We are all the way to the right...
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1] = endPos;
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeBottomToBottom ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  if ( ! pConnection )
    return endPos;

  // Now if the Source Container is below the Taret Container we need to follow a different pattern
  if ( startPos.y ( ) > endPos.y ( ) )
    return routeBottomToBottomBelowTarget ( startPos, endPos, pConnection );

  pos[0] = startPos;
  pos[1] = endPos;

  // Lets get the propper xPos of the Target Object.
  Object *pSourceObject  = pConnection->source ( )->object ( );
  Object *pTargetObject  = pConnection->target ( )->object ( );
//  int iSourceCenter = pSourceObject->pos ( ).x ( ) + (int) ( (float)pSourceObject->width ( ) / 2.0f );
  int iTargetRight  = pTargetObject->pos ( ).x ( ) + pTargetObject->width ( );

  if ( startPos.x ( ) < pTargetObject->pos ( ).x ( ) - iSPACE )  {
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( pSourceObject->pos ( ).x ( ) < pTargetObject->pos ( ).x ( ) )  {
    pos[1].setX ( pTargetObject->pos ( ).x ( ) - iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( pSourceObject->pos ( ).x ( ) < iTargetRight + iSPACE )  {
    pos[1].setX ( iTargetRight + iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else  {
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeBottomToBottomBelowTarget ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  if ( ! pConnection )
    return endPos;

  pos[0] = startPos;
  pos[1] = endPos;

  // Lets get the propper xPos of the Target Object.
  Object *pSourceObject  = pConnection->source ( )->object ( );
  Object *pTargetObject  = pConnection->target ( )->object ( );
  int iSourceRight       = pSourceObject->pos  ( ).x ( ) + pSourceObject->width ( );

  if ( iSourceRight + iSPACE < endPos.x ( ) )  {
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( pSourceObject->pos ( ).x ( ) < pTargetObject->pos ( ).x ( ) )  {
    pos[1].setX ( iSourceRight + iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( pSourceObject->pos ( ).x ( ) - iSPACE < endPos.x ( ) )  {
    pos[1].setX ( pSourceObject->pos ( ).x ( ) - iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else  {
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeSide ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  if ( ! pConnection )
    return endPos;

//  Area::enSide startSide = pConnection->side ( 0 );
  Area::enSide endSide   = pConnection->side ( 1 );
  if ( endSide == Area::SIDE_BOTTOM )
    return routeSideToBottom ( startPos, endPos, pConnection );

  pos[0] = startPos;
  pos[1] = endPos;

  int iDeltaSource = pConnection->m_iConNr[0] * iDELTA + iSPACE;
  int iDeltaTarget = pConnection->m_iConNr[1] * iDELTA + iSPACE;

  Object *pTargetObject = pConnection->target ( )->object ( );
  Object *pSourceObject = pConnection->source ( )->object ( );

  // Here we are going to route side to side ...
  if ( pSourceObject->pos ( ).x ( ) + pSourceObject->width ( ) + iDeltaSource < pTargetObject->pos ( ).x ( ) - iDeltaTarget )  {
    pConnection->m_enSide[0] = Area::SIDE_RIGHT;
    pConnection->m_enSide[1] = Area::SIDE_LEFT;
    pos[1].setY ( pos[0].y ( ) );
  }
  else if ( pSourceObject->pos ( ).x ( ) - iDeltaSource > pTargetObject->pos ( ).x ( ) + pTargetObject->width ( ) + iDeltaTarget )  {
    pConnection->m_enSide[0] = Area::SIDE_LEFT;
    pConnection->m_enSide[1] = Area::SIDE_RIGHT;
    pos[1].setY ( pos[0].y ( ) );
  }
  else if ( pSourceObject->pos ( ).x ( ) + iDeltaSource > pTargetObject->pos ( ).x ( ) + iDeltaTarget )  {
    pConnection->m_enSide[0] = Area::SIDE_RIGHT;
    pConnection->m_enSide[1] = Area::SIDE_RIGHT;
    pos[1].setX ( pos[0].x ( ) );
  }
  else  {
    pConnection->m_enSide[0] = Area::SIDE_LEFT;
    pConnection->m_enSide[1] = Area::SIDE_LEFT;
    pos[1].setX ( pos[0].x ( ) );
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
  pos[0] = pos[1];

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeSideToBottom ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  if ( ! pConnection )
    return endPos;

  // Now if the Source Container is below the Taret Container we need to follow a different pattern
  if ( startPos.y ( ) > endPos.y ( ) )
    return routeSideBelowTarget ( startPos, endPos, pConnection );

  pos[0] = startPos;
  pos[1] = endPos;
  // Here we handle routing from Bottom to either side
  // E.g. From a Menu to a Movie Container
  // Lets get the propper xPos of the Target Object.
  Object *pTargetObject = pConnection->target  ( )->object ( );
  Object *pSourceObject = pConnection->source  ( )->object ( );
  int iTargetRight  = pTargetObject->pos ( ).x ( ) + pTargetObject->width ( );
//  int iSourceCenter = pSourceObject->pos ( ).x ( ) + (int) ( (float)pSourceObject->width ( ) / 2.0f );

  if ( ( startPos.x ( ) < pTargetObject->pos ( ).x ( ) - iSPACE ) && ( pConnection->m_enSide[0] == Area::SIDE_RIGHT ) )  {
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( pSourceObject->pos ( ).x ( ) < pTargetObject->pos ( ).x ( ) )  {
    pConnection->m_enSide[0] = Area::SIDE_RIGHT;
    pos[1].setX ( pTargetObject->pos ( ).x ( ) + pTargetObject->width ( ) + iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( ( startPos.x ( ) >= iTargetRight + iSPACE ) && ( pConnection->m_enSide[0] == Area::SIDE_LEFT ) ) {
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else  {
    pConnection->m_enSide[0] = Area::SIDE_LEFT;
    pos[1].setX ( pTargetObject->pos ( ).x ( ) - iSPACE );
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
    pos[1].setY ( endPos.y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

QPoint OthogonalRouter::routeSideBelowTarget ( QPoint startPos, QPoint endPos, Area::Container::Connection *pConnection )
{
  QPoint pos[2];
  if ( ! pConnection )
    return endPos;

  pos[0] = startPos;
  pos[1] = endPos;

  // Lets get the propper xPos of the Target Object.
  Object *pSourceObject  = pConnection->source ( )->object ( );
  int iSourceCenter = pSourceObject->pos ( ).x ( ) + (int) ( (float)pSourceObject->width ( ) / 2.0f );

  if ( ( startPos.x ( ) < endPos.x ( ) ) && ( pConnection->m_enSide[0] == Area::SIDE_RIGHT ) )  {
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( iSourceCenter < endPos.x ( ) )  {
    pConnection->m_enSide[0] = Area::SIDE_RIGHT;
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else if ( ( startPos.x ( ) >= endPos.x ( ) ) && ( pConnection->m_enSide[0] == Area::SIDE_LEFT ) )  {
    pos[1].setY ( pos[0].y ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }
  else  {
    pConnection->m_enSide[0] = Area::SIDE_LEFT;
    pos[1].setX ( pos[0].x ( ) );
    m_listLines.append ( new Area::PieceOfLine ( pos[0], pos[1] ) );
    pos[0] = pos[1];
  }

  m_listLines.append ( new Area::PieceOfLine ( pos[0], endPos ) );
  return endPos;
}

}; // End namespace DVDLayout

