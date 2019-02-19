/***************************************************************************
    undobuffer.cpp
                             -------------------
    UndoBuffer and UndoObject code
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QMessageBox>		//xxx

#include "global.h"
#include "qdvdauthor.h"
#include "undobuffer.h"

UndoBuffer::UndoBuffer ( uint iDepth )
{
  // All objects belong to the UndoBuffer.
  // We handle the deletion manually !
  //  setAutoDelete(TRUE);
  if ( iDepth < 1 )
       iDepth = 1;
  m_iDepth = iDepth;
  m_bWait  = false;
  m_bFromOutsidePush = true;
}

UndoBuffer::~UndoBuffer ( )
{
  uint t;
  // Here we remove all Objects stored in the Undo- and Redo-Buffers
  //for ( t=0; t<(uint)count ( ); t++ )                 //ooo
  for ( t=0; t<static_cast<uint> ( count ( ) ); t++ )    //xxx
    delete at ( t );
  //for ( t=0; t<(uint)m_redoBuffer.count ( ); t++ )                //ooo
  for ( t=0; t<static_cast<uint> ( m_redoBuffer.count ( ) ); t++ )  //xxx
    delete m_redoBuffer.at ( t );
}

bool UndoBuffer::removeUnchanged ( )
{
  bool bFound = false;
  if ( count ( ) > 0 )  {
    UndoObject *pObject = last ( );
    if ( pObject )  {
      bool bChanged = pObject->hasChanged ( );
      if ( ! bChanged )  {
         removeLast ( );
         delete pObject;
         bFound =  true;
      }
    }
  }

  if ( bFound )
    Global::pApp->updateUndoStack ( );
  return bFound;
}

// PUBLIC : Get the last Undo Object.
UndoObject *UndoBuffer::pop ( )
{
//printf ( "%s::%s::%d > \n", __FILE__, __FUNCTION__, __LINE__ );
  // This function is called, to retrieve the last change
  // First check if the Undo Buffer is empty.
  if ( count ( ) < 1 )
    return NULL;
  // Now retrieve the UndoObject
  UndoObject *pUndoObject = last ( );
  removeLast ( );
  // and store it on the Redo - stack.
  m_redoBuffer.append ( pUndoObject );
  Global::pApp->updateUndoStack ( );

  return pUndoObject;
}

// PUBLIC : Get the last redo Object
UndoObject *UndoBuffer::popRedo ( )
{
//printf ( "%s::%s::%d > \n", __FILE__, __FUNCTION__, __LINE__ );
  // Check if the redo buffer is empty.
  if ( m_redoBuffer.count ( ) < 1 )
    return 0;
  UndoObject *pUndoObject = m_redoBuffer.last ( );
  m_redoBuffer.removeLast ( );
  // Redo means that we will increase the undo buffer again.
//  m_bFromOutsidePush = false;

  append ( pUndoObject );
  Global::pApp->updateUndoStack ( );

  return pUndoObject;
}

// PUBLIC : Store the UndoObject
void UndoBuffer::push ( UndoObject *pUndoObject )
{
//printf ( "%s::%s::%d > outsidePush<%s> redoCount<%d>\n", __FILE__, __FUNCTION__, __LINE__, m_bFromOutsidePush ? "true" : "false", m_redoBuffer.count ( ) );
  uint t;
  if ( m_bFromOutsidePush )  {
    // Here we check if push was called from this class itself,
    // then we're going to delete the redo Buffer
    for ( t=0; t<(uint)m_redoBuffer.count ( ); t++ )    //ox
      delete m_redoBuffer.at ( t );
    m_redoBuffer.clear ( );
  }
  m_bFromOutsidePush = true;
  //if ( (uint)count ( ) > m_iDepth )  {                    //ooo
  if ( static_cast<uint> ( count ( ) ) > m_iDepth )  {      //xxx
    // here we check and see if the maximal depth of the Undo Buffer has been reached.
    // If so, we delete the first entry before we add another one.
    UndoObject *pUndoObject = first ( );
    removeFirst ( );
    delete pUndoObject;
  }
  // Finally we can store the UndoObject onto the stack.
  append ( pUndoObject );

  Global::pApp->updateUndoStack ( );
}

// Misc functions not yet used, thus currently only protected.
uint UndoBuffer::countRedo ( )
{
  return m_redoBuffer.count ( );
}

UndoObject *UndoBuffer::atRedo ( uint iPosition )
{
  return m_redoBuffer.at ( iPosition );
}

void UndoBuffer::pushRedo ( UndoObject *pUndoObject )
{
  m_redoBuffer.append ( pUndoObject );
}

//////////////////////////////////////////////////////////////////////
//
// UndoObject class starts here
// This class holds the information for the undo function to take effect.
//
//////////////////////////////////////////////////////////////////////
UndoObject::UndoObject ( )
{
  m_enUndoType = -1; 
}

UndoObject::~UndoObject ( )
{
}

int UndoObject::getType ( )
{
  return m_enUndoType;
}
