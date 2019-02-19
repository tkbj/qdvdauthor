/***************************************************************************
    dvdmenuundoobject.h
                             -------------------
    UndoBuffer and UndoObject classes
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef DVDMENUUNDOOBJECT_H
#define DVDMENUUNDOOBJECT_H

#include <qdatetime.h>
//Added by qt3to4:
#include <QPixmap>
//#include <Q3ValueList>	//oxx

#include "menuobject.h"
#include "undobuffer.h"

class QPixmap;

class CDVDMenuInterface;
class SourceFileEntry;
class ButtonPreview;
// This class will encapsulate the Undo / redo Objects for the ConstructionArea - class
// Any UndoObject will hold only the most neccesary information for undo/redo functionality 
// to work, thus no complete FunctionPainter objects or other objects will be stored herein.
///////////////////////////////////////////////////////////////////////////////////////////

class DVDMenuUndoObject : public UndoObject
{
	class BackgroundInfo	{
	public:
		BackgroundInfo ( );
		void getBackgroundInfo (CDVDMenuInterface  *);
		void setBackgroundInfo (CDVDMenuInterface  *);
		BackgroundInfo &operator = (BackgroundInfo &);

		QPixmap background;
		bool bModified;

		// the following entries belong to the movie background
		QString qsMovieFileName;
		bool    bMovieBackgroundHasSound;
		QTime   timeOffset;
		QTime   timeDuration;
		// The following var is the background image or screenshot of themovie ...
		QString qsBackgroundFileName;

		QString qsResizeAlgo;
		int iStretchType;  // StretchToFit, BlackBorders, CutOffset
		QSize posOffset;
		int iFormat;
		int iWidth;
		int iHeight;
	};

public:
	enum enUndoType { UNDEFINED = 0, DRAGGED_TEXT, DRAGGED_IMAGE, DRAGGED_MOVIE, DRAGGED_BUTTON, DRAGGED_CHAPTERS, MOVED_OBJECT, NEW_BACKGROUND, NEW_AUDIO, NEW_OBJECT, EDIT_OBJECT, DELETED_OBJECT, NEW_SHADOW, DELETED_SHADOW, NEW_OVERLAY, DELETED_OVERLAY, NEW_BUTTON, UNBUTTON, DELETED_BUTTON, EDIT_MENU };

	DVDMenuUndoObject ( enUndoType, MenuObject *, ButtonPreview  *p=NULL, int i=-1 );
	DVDMenuUndoObject ( enUndoType, MenuObject *, MenuObject     * );
	DVDMenuUndoObject ( enUndoType, QList<MenuObject *>     & );		//oxx
	DVDMenuUndoObject ( enUndoType, QPixmap &, CDVDMenuInterface * );
	DVDMenuUndoObject ( MenuObject *, MenuObject::enState );
	DVDMenuUndoObject ( enUndoType, SourceFileEntry * );
	DVDMenuUndoObject ( enUndoType, DVDMenu * );
	virtual ~DVDMenuUndoObject();

	virtual bool hasChanged       ( );
	virtual void undo   ( DVDMenu * );
	virtual void redo   ( DVDMenu * );

	virtual QString getTypeString ( );
	QString getStateString        ( );
	MenuObject *getMenuObject     ( );
	void    changeType ( enUndoType );
	bool        isMoving      ( int );
	bool        hasMoved          ( );

private:
	DVDMenuUndoObject  ( );
	void initMe        ( );
	void handleMoved   ( );
	void handleObject  ( );
	void handleShadow  ( );
	void handleOverlay ( );
	void handleChapters( DVDMenu *, bool );
	void handleMenu    ( DVDMenu * );
	void unbutton      ( DVDMenu * );
	void rebutton      ( DVDMenu * );

private:
	// Private member variables ...
	MenuObject         *m_pMenuObject;
	MenuObject         *m_pShadowObject; // ShadowObject OR OverlayObject
	MenuObject         *m_pNormalObject; // Only used for Button objects
	QList<MenuObject *> m_listOfObjects;		//oxx
	MenuObject::Data   *m_pObjectData;
	MenuObject::enState m_objectState; // In case we have EDIT_OBJECT, this details what has changed
	bool                m_bDeleteObject;
	int                 m_iTwiggle; // Always wanted to call a variable twiggle
	QRect               m_rectPos;
	BackgroundInfo      m_backgroundInfo;    // For changing the backround
	CDVDMenuInterface  *m_pDVDMenuInterface; // For everything else
	SourceFileEntry    *m_pSourceFileEntry;
	ButtonPreview      *m_pButtonPreview;
};

#endif // DVDMENUUNDOOBJECT_H

