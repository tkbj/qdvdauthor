/***************************************************************************
    objectcollection.cpp
                             -------------------
    ObjectCollection class
                             
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

#include <QMouseEvent>
#include <QMessageBox>

#include "objectcollection.h"
#include "structuretoolbar.h"

ObjectCollection::Collection::Collection ( MenuObject *pTheObject, bool bDelete )
{
  
}

ObjectCollection::Collection::~Collection ( )
{
  
}

ObjectCollection::ObjectCollection ( QWidget *pParent )
  : MenuObject ( pParent )
{
  
}

ObjectCollection::~ObjectCollection ( )
{
  
}

void ObjectCollection::clear ( )
{
  uint t;
  for (t=0;t<(uint)m_listObjectCollection.count();t++)
    delete m_listObjectCollection[t];

  m_listObjectCollection.clear ( );
}

int ObjectCollection::getCount ( )
{
  return m_listObjectCollection.count ( );
}

MenuObject *ObjectCollection::getObject (int iObjectNumber)
{
  if (iObjectNumber > (int)m_listObjectCollection.count())
    return NULL;
  return m_listObjectCollection[iObjectNumber]->pObject;
}

void ObjectCollection::append ( MenuObject *pNewObject, bool bDeleteThisObject )
{
  if ( pNewObject )
    m_listObjectCollection.append ( new Collection ( pNewObject, bDeleteThisObject ) );
}

void ObjectCollection::setObjects ( QList<MenuObject *> &list, bool bDeleteThem )	//oxxx
{
  clear ( );
  QList<MenuObject *>::iterator it = list.begin ( );	//oxxx
  while ( it != list.end ( ) )
    append ( *it++, bDeleteThem );
}

bool ObjectCollection::contains ( MenuObject *pObject )
{
  uint t;
  for ( t=0;t<m_listObjectCollection.count ();t++) {
    if ( m_listObjectCollection[t]->pObject == pObject )
      return true;
  }
  return false;
}

void ObjectCollection::drawContents (QPainter *pPainter)
{
  // Here we simply pass the painter to the attached objects. No magic here !!!
  uint t;
  for (t=0;t<(uint)m_listObjectCollection.count();t++)
    m_listObjectCollection[t]->pObject->drawContents(pPainter);
}

void ObjectCollection::drawContents (QPainter *pPainter, int iFrameNumber, int iTotalNumberOfFrames)
{
  // Here we simply pass the painter to the attached objects. No magic here !!!
  uint t;
  for (t=0;t<(uint)m_listObjectCollection.count();t++)
    m_listObjectCollection[t]->pObject->drawContents(pPainter, iFrameNumber, iTotalNumberOfFrames);
}

bool ObjectCollection::mousePressEvent ( QMouseEvent *pEvent )
{
  m_currentMousePos = pEvent->pos ( );
  if ( pEvent->button ( ) == Qt::RightButton )  {
    QPoint globalPos = pEvent->globalPos ( );
    return createContextMenu ( globalPos );
  }
  else
    m_bDrawRect = true;
  return false;
}

bool ObjectCollection::mouseReleaseEvent ( QMouseEvent * )
{
  m_bDrawRect = false;
  return false;
}

bool ObjectCollection::createContextMenu (QPoint globalPos)
{
  return true;
}

StructureItem *ObjectCollection::createStructure ( StructureItem *pParentItem )
{
  // Creating teh ListView structure of this Button.
  QString qsCollection = tr("Collection of %1").arg(m_listObjectCollection.count ( ) );

  if ( pParentItem )  {
    if ( ! m_pStructureItem )  {
      //m_pStructureItem = new StructureItem ( this, StructureItem::Unknown, pParentItem, name ( ), qsCollection );	//ooo
      m_pStructureItem = new StructureItem ( this, StructureItem::Unknown, pParentItem, name ( ), qsCollection );
      //m_pStructureItem->setExpandable (TRUE);		//ooo
      m_pStructureItem->setExpanded (true);		//xxx
    }
    else
      //m_pStructureItem->setText ( name ( ), qsCollection );	//ooo
      m_pStructureItem->setText ( 0, qsCollection );		//xxx
  }
  else
    m_pStructureItem = NULL;

  uint t;
  for ( t=0; t<(uint)m_listObjectCollection.count ( ); t++ )
    m_listObjectCollection[t]->pObject->createStructure ( m_pStructureItem );

  return m_pStructureItem;	//oooo
}

AnimationAttribute *ObjectCollection::getSpecificAttributes (long, QString)
{
	return NULL;
}

