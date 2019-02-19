/****************************************************************************
** SourceFileEntry - class
**
** Created: Thu Jun 10 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class will use the xineEngine to obtain information about the 
** stream currently set by this engine.
**
****************************************************************************/

#ifndef MPLAYERINFO_H
#define MPLAYERINFO_H

#include <qobject.h>
//#include <q3process.h>	//ooo
#include <QProcess>		//xxx
#include <qimage.h>
#include <QMutex>		//xxx

#include "../mediainfo.h"

#include <QThread>	//xxx

class MPlayerInfo;

class SleeperThread : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

//class MPlayerInfoProcess : public Q3Process		//ooo
class MPlayerInfoProcess : public QProcess		//xxx
{
  Q_OBJECT

public:
   MPlayerInfoProcess ( MPlayerInfo * );
  ~MPlayerInfoProcess ( );

  bool hasExited ( );

private slots:
  void slotHasExited ( );

protected:
  bool         m_bHasExited;
  MPlayerInfo *m_pParent;
};

class MPlayerInfo : public QObject, public MediaInfo
{
	Q_OBJECT

public:
	MPlayerInfo ( );
	MPlayerInfo ( void * );
	virtual ~MPlayerInfo ( );

	// Mandatory function getScreenshot ()
	virtual QImage getScreenshot ( long ms=0 );

	// Non-standard functions (not implemented through MediaInfo ...)
	virtual bool hasChapters ( );
protected:
	// Mandatory function queryValues ()
	virtual void queryValues ( );
	void waitForProcess      ( ); // crude way to wait for the process to finish up.

protected slots:
	virtual void slotReadStdout ( );
	virtual void slotRetryQuery ( );
	virtual void slotStart      ( );

private:
	void queryValues   ( bool );
	void deleteSlaveProcess ( );
	void initMe  ( );
	void Lock    ( ) {        m_pMutex->lock    ( ); };
	void Unlock  ( ) {        m_pMutex->unlock  ( ); };
	bool TryLock ( ) { return m_pMutex->tryLock ( ); };

private:
	// Private member variables
	MPlayerInfoProcess *m_pSlaveProcess;
        QMutex     *m_pMutex;
	bool        m_bAcquireLock;
	int         m_iRetryCounter;
	QStringList m_listOfStdout;
	QString     m_qsTempPathName;

	friend class MPlayerInfoProcess;
};

#endif	// MPLAYERINFO_H

