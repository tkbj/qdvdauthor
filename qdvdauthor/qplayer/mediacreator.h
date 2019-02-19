/***************************************************************************
    mediacreator.h
                             -------------------
    MediaCreator - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class is only a helper class to create a appropriate Media object.
    Here we take care of the availability of the actual libraries etc.

    The main functions are static thus you don't need to first create 
    a MediaCreator object to use them.

****************************************************************************/
 
#ifndef MEDIACREATOR_H
#define MEDIACREATOR_H

//#include <qstringlist.h>	//ooo
#include <QStringList>		//xxx

// Include the interface since we will need this anyways ...
#include "engines/mediainterface.h"

#define MEDIASCANNER_EVENT 123

class SourceFileInfo;
class MediaScanner;
class ExecuteJob;
class Subtitles;
class MediaInfo;
class QImage;

class MediaCreator
{
	 MediaCreator ();
	~MediaCreator ();
public:
	static MediaInterface *createMPlayerWidget       ( QWidget *, const char*, void *p=NULL );
	static MediaInterface *createXineWidget          ( QWidget *, const char*, void *p=NULL );
	static MediaInterface *createKXineWidget         ( QWidget *, const char*, void *p=NULL );
	static MediaInterface *createVLCWidget           ( QWidget *, const char*, void *p=NULL );
	static MediaInterface *createPreferredWidget     ( QWidget *, const char*, void *p=NULL );

	static MediaInfo      *createInfo                ( void *p=NULL );

	// This will register a creation of a preview in a background task.
	static void            registerWithMediaScanner  ( QObject *, QString &, QImage *, long s=0L );
	static void            registerWithMediaScanner  ( QObject *, SourceFileInfo *, long s=0L,   bool b=false );
	static void            registerWithMediaScanner  ( QObject *, SourceFileInfo *, Subtitles *, float );
	static void            registerWithMediaScanner  ( ExecuteJob * );
	static void            unregisterFromMediaScanner( SourceFileInfo * );
	static void            unregisterFromMediaScanner( QObject * );

	static QStringList     getAvailableEngines  ( );
	static bool            setPreferredEngine   ( QString );
	static QString         getEngineDescription ( QString );
	static QString         preferredEngine      ( );

	static QString         m_qsPreferredEngine;
	static MediaScanner   *pPreviewObject;	// This is not a var but the child thread ...

private:
	static void            ensureMediaScannerExists ( );
};

#endif	// MEDIACREATOR_H
