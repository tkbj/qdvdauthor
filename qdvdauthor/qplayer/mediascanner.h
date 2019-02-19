/***************************************************************************
    mediascanner.h
                             -------------------
    PreviewThread class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class spinns off a new thread and generate a Preview of the Job 
    which was set through MediaCreator::registerWithPreviewThread.
 
    Once the preview is generated we will call the update function in 
    the main thread ( Currently MovieObject::updatePixmap () )

    if after 5 seconds after termination of the thread there are no new 
    requests in the queue then the thread will be termintaed to free 
    some memory.

****************************************************************************/

#ifndef MEDIASCANNER_H
#define MEDIASCANNER_H

//#include <q3valuelist.h>	//oxx
#include <qthread.h>
#include <qmutex.h>
#include <QMutex>	//xxx
#include <QImage>	//xxx
#include <QThread>	//xxx

class SleeperThread9 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

class MediaInfo;
class Subtitles;
class SourceFileInfo;

class ExecuteJob 
{
  ExecuteJob ( );
public:
  enum enType {
    TYPE_UNKNOWN = 0,
    TYPE_INFO,
    TYPE_IMAGE,
    TYPE_SUBTITLES,
    TYPE_REQUEST,
    TYPE_MULTI_THUMB
  };

  ExecuteJob ( QObject *pTheCallingObject ) 
  {
    initMe ( );
    theType         = TYPE_REQUEST;
    pOrigObject     = pTheCallingObject;
  };

  ExecuteJob (QObject *pTheCallingObject, QString &qsTheFileName, QImage *pTheImage, long iMSecOffset)
  {
    initMe ( );
    theType         = TYPE_IMAGE;
    qsFileName      = qsTheFileName;
    pImage          = pTheImage;
    pOrigObject     = pTheCallingObject;
    iMSecondsOffset = iMSecOffset;
  }
  ExecuteJob (QObject *pTheCallingObject, SourceFileInfo *pSourceInfo, int iMSecOffset, bool bUpdateInfo)
  {
    initMe ( );
    theType         = TYPE_INFO;
    pSourceFileInfo = pSourceInfo;
    pOrigObject     = pTheCallingObject;
    iMSecondsOffset = iMSecOffset;
    bUpdateAllInfo  = bUpdateInfo; 
  }
  ExecuteJob ( QObject *pTheCallingObject, SourceFileInfo *pSourceInfo, Subtitles *pSub, float f )
  {
    initMe ( );
    theType         = TYPE_SUBTITLES;
    pOrigObject     = pTheCallingObject;
    pSourceFileInfo = pSourceInfo;
    pSubtitles      = pSub;
    fFPS            = f;
  }
  virtual ~ExecuteJob (  ) { };

  void initMe ( )
  {
    theType         = TYPE_UNKNOWN;
    pImage          = NULL;
    pSourceFileInfo = NULL;
    pOrigObject     = NULL;
    iMSecondsOffset = 0;
    bUpdateAllInfo  = true;
    pSubtitles      = NULL;
    fFPS            = 29.97f;
    bFinished       = false;
    bSuccess        = true;
  }

  virtual bool response ( ) { return false; };

  enType          theType;
  long            iMSecondsOffset;
  bool            bUpdateAllInfo;
  QString         qsFileName;
  QImage         *pImage;
  float           fFPS;

  QObject        *pOrigObject;
  SourceFileInfo *pSourceFileInfo;
  Subtitles      *pSubtitles;
  bool            bFinished;
  bool            bSuccess;
};

class MediaScanner : public QThread
{
public:
	MediaScanner      ( );
	virtual ~MediaScanner ( );
	bool hasFinished  ( );
	void append       ( ExecuteJob * );
	void remove       ( SourceFileInfo * );
	void remove       ( QObject * );
	bool setSemaphore ( bool   );
	int  waitCounter  ( int );
	void resetWait    ( );

	virtual void run  ( );

private:
	void handleSubtitles ( ExecuteJob * );

private:
	bool                     m_bHasFinished;
	bool                     m_bSemaphore;
	QMutex                  *m_pMutex;
	MediaInfo               *m_pMediaInfo;
	volatile int             m_iWaitCounter;
	QList<ExecuteJob *> m_listExecuteFifo;	//oxx
};

#endif	// MEDIASCANNER_H
