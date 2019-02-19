/***************************************************************************
    undobuffer.h
                             -------------------
    UndoBuffer and UndoObject classes
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef UNDOBUFFER_H
#define UNDOBUFFER_H

#include <qstring.h>
//#include <q3ptrlist.h>	//ooo
#include <QList>		//xxx

// The UndoBuffer class will collect all UndoObjects on a stack of a defined size.
// If the stack runs out of space, the last UndoObject will get deleted.
// If the user calls one UndoObject, then this Object is moved to the Re-do buffer
// and only deleted, when the user does another operation in the middle of the stack.
//////////////////////////////////////////////////////////////////////////////////////

class DVDMenu;

class UndoObject
{
public:
	         UndoObject ( );
	virtual ~UndoObject ( );

	int  getType        ( );

	virtual bool    hasChanged     ( ) = 0;
	virtual QString getTypeString  ( ) = 0;
	virtual void  undo   ( DVDMenu * ) = 0;
	virtual void  redo   ( DVDMenu * ) = 0;

protected:
	int m_enUndoType;
};

// Note that no UndoObject is deleted from the UndoBuffer, Rather they are deleted from the RedoBuffer
// This is due to the storing of the Objects for as long as another 'new' UndoObject comes onto the Undo-stack.
//class UndoBuffer : public Q3PtrList<UndoObject>	//ooo
class UndoBuffer : public QList<UndoObject*>		//xxx
{
public:
	 UndoBuffer ( uint iDepth );
	~UndoBuffer ( );

	// get the last undo Object
	UndoObject *pop     ( );
	// get the last redo Object
	UndoObject *popRedo ( );
	// push the UndoObject onto the stack.
	void push ( UndoObject * );
	// The number of objects on the Redo buffer
	uint countRedo ( );
	// return the appropriate UndoObject
	UndoObject *atRedo ( uint );

	bool removeUnchanged( );

protected: // Misc functions, not yet used
	void pushRedo ( UndoObject * );

private:
	uint m_iDepth;
	bool m_bFromOutsidePush;
	bool m_bWait;
	//Q3PtrList<UndoObject> m_redoBuffer;	//ooo
	QList<UndoObject*> m_redoBuffer;	//xxx
};

#endif // UNDOBUFFER_H


