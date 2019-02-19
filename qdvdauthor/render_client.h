/***************************************************************************
    render_client.h
                             -------------------
    Client - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef RENDER_CLIENT_H
#define RENDER_CLIENT_H

#include <qthread.h>
//#include <q3socket.h>			//ooo
//#include <QtNetwork/QTcpSocket>		//xxx
#include <QTcpSocket>		//xxx
//#include <qpixmap.h>			//ooo
#include <QPixmap>			//xxx
//#include <qdatetime.h>		//ooo
#include <QDateTime>			//xxx
//#include <q3valuelist.h>		//oxx
//#include <qwaitcondition.h>		//ooo
#include <QWaitCondition>		//xxx
#include <QMutex>			//xxx
//Added by qt3to4:
#include <QTimerEvent>
#include <string.h> //xxx
#include <QByteArray>   //xxx

class QFile;
class QTcpSocket;   //xxx

#define EVENT_RENDER_EXIT      11111
#define EVENT_RENDER_PROGRESS  11112
#define EVENT_RENDER_EXCEPTION 11113

// The block size we shuffle over the socket at once.
#define BLOCK_LEN 1024
// Throttling transfere over socket for better debugging (delay in usec)
#define THROTTLE 0

class CXmlSlideshow;
class SourceFileEntry;
class ButtonTransition;

/*class SleeperThread6 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};*/

/*class Worker : public QObject   //xxx
{
    Q_OBJECT
private slots:
    void onTimeout()
    {
        qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();
    }
protected:
    void enableTimer ( bool );  //xxx
};*/    //xxx

namespace Render
{
    
class Worker : public QObject   //xxx
{
    Q_OBJECT
    
public:
    Worker(QObject *parent = 0);
    
    void enableTimer ( bool );  //xxx
    
private slots:
    void onTimeout()
    {
        qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();
    }

protected:
    //void enableTimer ( bool );  //xxx
    //virtual void timerEvent ( QTimerEvent   * );    //xxx
    void timerEvent ( QTimerEvent   *pEvent );    //xxx
private:
  //void enableTimer ( bool );  //xxx
  int               m_iKeepaliveTimer; // Checks if the server has a heartbeat    //xxx
  QDateTime         m_lastPing;        // Last received ping    //xxx
};

//class Client : public QObject, public QThread		//ooo
class Client : public QThread				//xxx-!
//class Client : public QObject				//xxx-!
{
  Q_OBJECT

  template<class type> class Message  {
  public:
    Message ( ) { };
    static QByteArray create ( type value )
    {
      char cBuffer[ sizeof ( type ) ];
      (type &)cBuffer = value;
      return QByteArray ( cBuffer, sizeof ( type ) );
    }
  };

public:
  Client ( SourceFileEntry  * );  //ooo
  //Client ( QObject *parent = 0, SourceFileEntry  *pEntry = 0 );   //xxx
  Client ( ButtonTransition * );  //ooo
  //Client ( QObject *parent = 0, ButtonTransition *pTrans = 0 );   //xxx
  /*Client(int socketDescriptor)  //xxx
        : m_socketDescriptor(socketDescriptor)
    {
        //setAutoDelete(false);
    }*/
  virtual  ~Client ( );

  virtual void run ( );
  void lock   ( );
  void unlock ( );
  void initMe ( );
  void startMe( );

  float progress         ( );
  bool  waitingInQueue   ( );
  bool  checkSocket      ( );
  void  killServerThread ( );
  void  setWaiting  ( bool );
  SourceFileEntry  *sourceFileEntry  ( );
  ButtonTransition *buttonTransition ( );
  //void enableTimer ( bool );    //xxx
  //QTimer *m_NotificationTimer;  //xxx
  QByteArray byteArrayfromMessageID ( quint16 );    //xxx

protected:
  virtual void timerEvent ( QTimerEvent   * );
  void runSlideshow       ( CXmlSlideshow * );
  bool sendFile ( QString );
  //void enableTimer ( bool );  //ooo
  void runEntry         ( );
  void runTrans         ( );
  void startServer      ( );
  void restartServer    ( );
  void receivedPing     ( );
  void sendNextPacket   ( );
  void receivedFileAck  ( );
  void displayProgress  ( );
  void receivedProgress ( );
  //Q_UINT64 fileFromSocket        ( );		//ooo
  quint64 fileFromSocket        ( );		//xxx
  void receivedServerDone        ( );
  void receivedServerState       ( );
  void receivedVersionAndHost    ( );
  bool sendVersionAndHostRequest ( );
  static void reRequest ( Client * );
  
signals:    //xxx
  void signalEnableTimer    ( bool );
  //void error ( QAbstractSocket::SocketError );    //xxx
  void signalWrite(QByteArray Data);  //xxx
  void signalWrite(QString Data);  //xxx
  //void signalWrite(std::string Data);  //xxx
  void signalFlush( );   //xxx

private slots:
  virtual void closeConnection ( );
  virtual void socketReadyRead ( );
  virtual void socketConnected ( );
  virtual void socketClosed    ( );
  virtual void slotReconnect   ( );
  virtual void slotRecovered   ( );
  //virtual void socketError ( int );   //ooo
  virtual void socketError ( QAbstractSocket::SocketError );    //xxx
  //void displayError ( QAbstractSocket::SocketError socketError );  //xxx
  //void displayError ( QAbstractSocket::SocketError );  //xxx
  virtual void socketConnectionClosed ( );
  virtual void slotConnectAndStart    ( );
  virtual void socketConnectAndStart  ( );
  void onTimeout();  //xxx
  //void enableTimer1 ( bool );  //xxx
  void enableTimer ( bool );  //xxx
  void slotWrite(QByteArray Data);  //xxx
  void slotWrite(QString Data);  //xxx
  //void slotWrite(std::string Data);  //xxx
  void slotFlush( );    //xxx

private:
  // Something for the server connection
  QFile            *m_pFile;
  //Q3Socket          *m_pSocket;	//ooo
  QTcpSocket       *m_pSocket;		//xxx
  QDataStream       stream;    //xxx
  QDataStream       stream1;    //xxx
  QString           m_qsTempXMLFile;
  SourceFileEntry  *m_pSourceFileEntry;
  ButtonTransition *m_pButtonTransition;
  QWaitCondition    m_waiter;
  QMutex            m_waiterMutex;	//xxx
  QMutex            m_mutex;
  float             m_fProgress;
  //Q_ULONG           m_iMessageLen;     // to keep track of the incoming slideshow video file	//oooo
  qint64           m_iMessageLen;     // to keep track of the incoming slideshow video file
  //Q_UINT16          m_iMessageID;      // The current MessageID being processed	//oooo
  quint16          m_iMessageID;      // The current MessageID being processed		//xxx
  bool              m_bRemoteServer;   // The server is on a different computer
  bool              m_bSendImageFiles; // The server needs the image files ( def. true )
  bool              m_bWaitInQueue;    // If the server has currently more then the set threads running
  bool              m_bSentAllInfo;    // has the function run() completed ?
  int               m_iKeepaliveTimer; // Checks if the server has a heartbeat
  QDateTime         m_lastPing;        // Last received ping
  int               m_iRetryCounter;   // The numer of times to try to re-establish the connection
  QTimer           *m_NotificationTimer;  //xxx
  int               m_socketDescriptor;   //xxx
  QScopedPointer<QTcpSocket> m_socket;    //xxx
};

// Singleton class Manager
class Manager
{
public:
  class ServerInfo
  {
  public:
    ServerInfo ( unsigned long, unsigned int, unsigned int );
   ~ServerInfo ( );

    unsigned long  getIP ( );
    unsigned int getPort ( );
    Client    *addClient ( SourceFileEntry  * );
    bool    removeClient ( SourceFileEntry  * );
    Client    *addClient ( ButtonTransition * );
    bool    removeClient ( ButtonTransition * );
    bool startNextClient ( );
    bool  startingServer ( );
    int      clientCount ( );
    void     startServer ( );
    bool      inRecovery ( ); // connection was lost and now trying to re-eastablish
    void     setRecovery ( bool );
    void     setStarting ( bool );
    bool       hasClient ( Client * );

  private:
    unsigned long        m_iServerIP;    // 32 bit notation instead of in_addr_t
    unsigned int         m_iServerPort;  // Defaults to Render::SERVER_PORT
    unsigned int         m_iConcurrentThreads;
    bool                 m_bRecoveryMode;
    bool                 m_bStartingServer; // After issuing the bash command through system() we should prevent other threads

    QList<Client *>      m_listOfClients;	//oxx
  };

public:
   Manager ( );
  ~Manager ( );
  static bool registerEntry   ( SourceFileEntry  * );
  static bool unregisterEntry ( SourceFileEntry  * );

  static bool registerTrans   ( ButtonTransition * );
  static bool unregisterTrans ( ButtonTransition * );

  ServerInfo  *getServerInfo  ( unsigned long      );
  ServerInfo  *getServerInfo  ( Render::Client   * );
  QStringList &getFilterNames ( );

  void lock   ( );
  void unlock ( );

  static Manager *m_pSelf; // Pointer to the sole instance of itself
  QMutex m_mutex;
  QList<ServerInfo *> m_listOfServer;	//oxx
  QStringList              m_listFilterNames;
};

}; // End of namespace Render

#endif // RENDER_CLIENT_H
