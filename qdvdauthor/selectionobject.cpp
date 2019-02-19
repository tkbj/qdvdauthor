/***************************************************************************
    selectionobject.cpp
                             -------------------
    SelectionObject class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  This class is the encapsulation of the ObjectCollection.

  The ObjectCollection is not yet finished. It is intended to hold a list
  of MenuObject's which are linked to each other. Meaning if you move 
  the collection each member of it will move the same extend.

  Another use is to create a template which will fit the images/movies
  into a fixed scheme.
    
****************************************************************************/

#include <QPen>
#include <QPainter>
#include <QMouseEvent>

#include "xml_dvd.h"
#include "shadowobject.h"
#include "selectionobject.h"

SelectionObject::SelectionObject ( QWidget *pParent )
  : ObjectCollection ( pParent )
{
  m_qsObjectType = QString ( SELECTION_OBJECT );
  resetObjectStates ( );
  setActive ( true );
}

SelectionObject::~SelectionObject ( )
{
  
}

void SelectionObject::resetObjectStates ( )
{
  uint t;
  //for (t=0;t<m_listObjectCollection.count();t++)                      //ooo
  for (t=0;t<static_cast<uint> ( m_listObjectCollection.count() );t++)  //xxx
    m_listObjectCollection[t]->pObject->setObjectState ( MenuObject::StateUnknown );
}

void SelectionObject::setMoveState ( bool bSetMoving )
{
  uint t;
  MenuObject::enState enState = MenuObject::StateMoveObject;
  if ( ! bSetMoving )
    enState = MenuObject::StateUnknown;

  for ( t=0; t<(uint)m_listObjectCollection.count ( ); t++ )  {
    m_listObjectCollection[t]->pObject->setObjectState ( enState );
    m_listObjectCollection[t]->pObject->setActive   ( bSetMoving );
  }
}

void SelectionObject::setCurrentMousePos ( QPoint &thePos )
{
  m_currentMousePos = thePos;
  if ( m_listObjectCollection.count ( ) > 0 )
       MenuObject::setRect ( m_listObjectCollection[0]->pObject->rect ( ) );

  uint t;
  for ( t=0; t<(uint)m_listObjectCollection.count ( ); t++ )  {
    m_listObjectCollection[t]->pObject->setCurrentMousePos ( thePos );
  }
}

void SelectionObject::drawSelected ( QPainter *pPainter )
{
  MenuObject *pObject;
  QMatrix    theMatrix;
  QPoint      centerPos;
  QBrush      theBrush   ( QColor ( 100, 255, 100 ), Qt::Dense6Pattern );
  QPen        thePen     ( QColor ( 100, 255, 100 ), 1,  Qt::SolidLine );
  // Here we calculate the center point of gravity (rotation)
  uint t;
  //for (t=0;t<m_listObjectCollection.count();t++) {                        //ooo
  for (t=0;t<static_cast<uint> ( m_listObjectCollection.count() );t++) {    //xxx
    pObject = m_listObjectCollection[t]->pObject;

    //centerPos.setX (pObject->rect().x() + (int)((float)pObject->rect().width () / 2.0));                              //ooo
    centerPos.setX (pObject->rect().x() + static_cast<int> ( static_cast<float> ( pObject->rect().width () ) / 2.0));   //xxx
    //centerPos.setY (pObject->rect().y() + (int)((float)pObject->rect().height() / 2.0));                              //ooo
    centerPos.setY (pObject->rect().y() + static_cast<int> ( static_cast<float> ( pObject->rect().height() ) / 2.0));   //xxx
    // Here we define the cenetered rect.
    //QRect theRect ((int)-(pObject->rect().width()/ 2.0), (int)-(pObject->rect().height()/2.0), pObject->rect().width(), pObject->rect().height());    //ooo
    QRect theRect (static_cast<int> (- pObject->rect().width()/ 2.0 ), static_cast<int> (- pObject->rect().height()/2.0 ), pObject->rect().width(), pObject->rect().height());    //xxx

    // First we translate to the appropriate location,
    //theMatrix.translate ((double)centerPos.x(), (double)centerPos.y());                                   //ooo
    theMatrix.translate (static_cast<double> ( centerPos.x() ), static_cast<double> ( centerPos.y() ) );    //xxx

    //. then we apply the other modifications ...
    theMatrix.scale  ( pObject->modifiers ( )->fScaleX, pObject->modifiers ( )->fScaleY );
    theMatrix.shear  ( pObject->modifiers ( )->fShearX, pObject->modifiers ( )->fShearY  );
    theMatrix.rotate ( pObject->modifiers ( )->fRotate );

    pPainter->setWorldMatrix(theMatrix);

    pPainter->setBrush ( theBrush );
    pPainter->setPen   (  thePen  );
    pPainter->drawRect (  theRect );

    theMatrix.reset();
  }
  pPainter->setWorldMatrix(theMatrix);  
}

void SelectionObject::drawContents ( QPainter *pPainter )
{
  if ( isActive  ( ) )
    drawSelected ( pPainter );
  else
    ObjectCollection::drawContents ( pPainter );
}

void SelectionObject::drawShadows ( QPainter *pPainter )
{
  uint t;
  for ( t=0; t<(uint)m_listOfShadows.count ( ); t++ ) {
    m_listOfShadows[ t ]->drawContents ( pPainter );
  }
}

// returns if the mouse is within one of the selected objects
// AND sets the state accordingly
bool SelectionObject::mouseOver ( QPoint const &thePoint )
{
  uint t;
  for (t=0;t<(uint)m_listObjectCollection.count();t++) {
    if ( m_listObjectCollection[t]->pObject->boundingRect ().contains ( thePoint ) ) {
      setMoveState ( true );
      return true;
    }
  }
  setMoveState ( false );
  return false;
}

bool SelectionObject::mousePressEvent ( QMouseEvent *pEvent )
{
  uint t;
  if ( pEvent->button ( ) == Qt::RightButton ) {
    for (t=0;t< (uint) m_listObjectCollection.count();t++)  {
      if ( m_listObjectCollection [t]->pObject->boundingRect ( ).contains ( pEvent->pos ( ) ) ) {
        createContextMenu ( pEvent->globalPos ( ) );
        return true;
      }
    }
  }
  return false;
}

bool SelectionObject::createContextMenu ( QPoint globalPos )
{
  return true;
}

void SelectionObject::setActive ( bool bActive )
{
  uint t;
  for (t=0;t<(uint)m_listObjectCollection.count();t++)
    m_listObjectCollection[t]->pObject->setActive ( false );
  ObjectCollection::setActive ( bActive );
}

void SelectionObject::updateShadows ( ShadowObject *pParentShadow, bool bRecreate )
{
  uint t;
  ShadowObject *pShadow;
  MenuObject *pMenuObject;

  if ( bRecreate ) {
    clearShadows ( );
    for (t=0;t<(uint)m_listObjectCollection.count();t++)  {
      pMenuObject = m_listObjectCollection[ t ]->pObject;
      pShadow = new ShadowObject ( pMenuObject );
      if ( pParentShadow )
        *pShadow = *pParentShadow;
      pShadow->updateShadow  ( bRecreate );
      m_listOfShadows.append ( pShadow );
    } 
  }
  else {
    for (t=0;t<(uint)m_listOfShadows.count();t++)  {
      pShadow = m_listOfShadows[ t ];
      if ( pParentShadow )
        *pShadow = *pParentShadow;
      pShadow->updateShadow  ( bRecreate );
    } 
  }
}

void SelectionObject::clearShadows ( )
{
  uint t;
  for (t=0;t<m_listOfShadows.count();t++)
    delete m_listOfShadows[t];

  m_listOfShadows.clear ();
}
