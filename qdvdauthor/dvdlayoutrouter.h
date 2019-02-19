/***************************************************************************
    dvdlayoutrouter.h
                             -------------------
    Class DVDLayout::Router
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class handles the display area for the DVD Layout tab.
   
****************************************************************************
   
   The DVD Layout must handle
   o DVDMenuObjects
   o MovieObjects
   o FirstObject ( FPC )
   o PreObjects / PostObjects
    
****************************************************************************/

#ifndef DVDLAYOUTROUTER_H
#define DVDLAYOUTROUTER_H

#include "dvdlayoutarea.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

namespace DVDLayout
{

class LinearRouter
{
public:
  LinearRouter ( );
  virtual ~LinearRouter  ( );

  virtual QList<Area::PieceOfLine *> route ( QPoint, QPoint, Area::Container::Connection * );	//oxx
};

class OthogonalRouter : public LinearRouter
{
  const int iSPACE;
  const int iDELTA;
public:
  OthogonalRouter ( );
  virtual ~OthogonalRouter  ( );

  virtual QList<Area::PieceOfLine *> route ( QPoint, QPoint, Area::Container::Connection * );	//oxx

protected:
  QPoint routeBottomToBottomBelowTarget ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeBottomBelowTarget         ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeBottomToBottom            ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeBottom                    ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeSide                      ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeSideToBottom              ( QPoint, QPoint, Area::Container::Connection * );
  QPoint routeSideBelowTarget           ( QPoint, QPoint, Area::Container::Connection * );

private:
  QList<Area::PieceOfLine *> m_listLines;	//oxx

};

}; // End of namespace DVDLayout

#endif // DVDLAYOUTROUTER_H
