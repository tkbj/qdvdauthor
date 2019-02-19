/***************************************************************************
    render_client.cpp
                             -------------------
    Class QRender::Client
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class handles the client side and is Qt 3.x code so that it can be
   used from within QDVDAuthor.

   The Manager is there to simplify handle multiple rendering of DVDMenus.
   
   Th Client is a separate thread which handles the incoming messages from
   the rendering server.
   There are three different types of messages.
   
   1) The status/communication messages
   2) The Exception messages
   3) The terminate message
   
   All message IDs are defined in qrender/shared.h
   The basic message structure is as simple as can be.
   2 Bytes   : Message ID
   8 Bytes   : Message Length
   remainder : Message body ( Text, Image, or Final video )
    
****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>   //xxx

#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
//#include <qtimer.h> //ooo
#include <QTimer> //xxx
#include <qobject.h>
#include <qbuffer.h>
#include <qpainter.h>
#include <qapplication.h>
//#include <q3socketdevice.h>	//ooo
//#include <QtNetwork/QTcpSocket>		//xxx
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	//oxx
//#include <QtCore/QCustomEvent>	//ooo
#include <QtCore/QEvent>	//xxx
#include <QMutexLocker>     //xxx
#include <QHostAddress>     //xxx
//#include <QCustomEvent>     //xxx
#include <QMessageBox>  //xxx

#include "render_client.h"

#include "global.h"
#include "messagebox.h"
#include "qdvdauthor.h"
#include "buttonobject.h"
#include "xml_slideshow.h"
#include "sourcefileentry.h"
#include "../qrender/shared.h"

#ifdef debug_out
#undef debug_out
#endif
#if 1 
static void dummy(const char*,...){};
//#define debug_out dummy   //ooo
#define debug_out printf    //xxx
//#define sdebug_out dummy  //ooo
#define sdebug_out printf   //xxx
#else
//#define debug_out printf ( "%s::%s::%d >", __FILE__, __FUNCTION__, __LINE__ ); printf
#define debug_out printf ( "thr:%X obj:%p :: %s::%s::%d >", (uint)pthread_self(), this, __FILE__, __FUNCTION__, __LINE__ ); printf
#define sdebug_out printf ( "thr:%X obj:          :: %s::%s::%d >", (uint)pthread_self(), __FILE__, __FUNCTION__, __LINE__ ); printf
#endif

namespace Render
{

// Init the one and only Manager
Manager *Manager::m_pSelf = NULL;

Manager::ServerInfo::ServerInfo ( unsigned long iServerIP, unsigned int iPort, unsigned int iConcurrentThreads )
{
  m_iServerIP          = iServerIP;
  m_iServerPort        = iPort;
  m_iConcurrentThreads = iConcurrentThreads;
  m_bRecoveryMode      = false;
  m_bStartingServer    = false;
}

Manager::ServerInfo::~ServerInfo ( )
{
  Client *pClient = NULL;
  QList<Client *>::iterator it = m_listOfClients.begin ( );	//oxx
  while ( it != m_listOfClients.end ( ) )  {
    pClient = *it++;
//  pClient->closeConnection ( );
    //if ( pClient->running    ( ) )	//ooo
    //if ( pClient->isRunning    ( ) )	//xxx
         //pClient->terminate  ( ); //ooo
    delete pClient;
  }
  m_listOfClients.clear ( );
}

unsigned long Manager::ServerInfo::getIP ( )
{
  return m_iServerIP;
}

unsigned int Manager::ServerInfo::getPort ( )
{
  return m_iServerPort;
}

void Manager::ServerInfo::setRecovery ( bool bRecovery )
{
  m_bRecoveryMode = bRecovery;
}

bool Manager::ServerInfo::inRecovery ( )
{
  return m_bRecoveryMode;
}

bool Manager::ServerInfo::startingServer ( )
{
  return m_bStartingServer;
}

void Manager::ServerInfo::setStarting ( bool bStarting )
{
  m_bStartingServer = bStarting;
}

void Manager::ServerInfo::startServer ( )
{  
  if ( startingServer ( ) )
    return;
  
  // We need to block other threads from trying the same to give qrender time to get started
  setStarting ( true );

  Utils theUtils;
  QString qsCommand = theUtils.getToolPath ( "qrender" );
  if ( qsCommand.isEmpty ( ) )  {
    qsCommand = Global::qsSystemPath + "/bin/qrender";
    QFileInfo fileInfo ( qsCommand );
    if ( ! fileInfo.exists ( ) )  {
      qsCommand = theUtils.checkForExe ( "qrender" );
      if ( qsCommand.isEmpty ( ) )
           qsCommand = "qrender";
    }
  }

  qsCommand += " -v 5";   //ooo
  //qsCommand += " -v 5 -ix \"/tmp/Unnamed/Images (2018_09_12).xml\"";  //xxx

  //int iRet = system ( qsCommand.ascii ( ) );		//ooo
  int iRet = system ( qsCommand.toLatin1 ( ).data () );	//xxx

  iRet = iRet; // quiet down the compiler.
  //debug_out ( "ret<%d> = <%s>\n", iRet, qsCommand.ascii ( ) );		//ooo
  debug_out ( "ret<%d> = <%s>\n", iRet, qsCommand.toLatin1 ( ).data () );	//xxx
}

bool Manager::ServerInfo::hasClient ( Client *pClient )
{
  QList<Client *>::iterator it = m_listOfClients.begin ( );	//oxx
  while ( it != m_listOfClients.end ( ) )  {
    if ( *it++ == pClient )
      return true;
  }
  return false;
}

Client *Manager::ServerInfo::addClient ( SourceFileEntry *pEntry )
{
  if ( ! pEntry ) {
    return NULL;
  }

  Client *pClient = new Client ( pEntry  );   //ooo
  //Client *pClient = new Client ( 0 , pEntry  );   //xxx
  m_listOfClients.append       ( pClient );

  return pClient;
}

bool Manager::ServerInfo::removeClient ( SourceFileEntry *pEntry )
{  
  if ( ! pEntry )
    return false;

  Client *pClient = NULL;
  QList<Client *>::iterator it = m_listOfClients.begin ( );	//oxx
  while ( it != m_listOfClients.end ( ) )  {
    pClient = *it++;
    if ( pClient->sourceFileEntry ( ) == pEntry )  {
      //m_listOfClients.remove      ( pClient );	//ooo
      m_listOfClients.removeOne      ( pClient );	//xxx
      pClient->killServerThread   ( );

      delete pClient;
      startNextClient ( );
      return true;
    }
  }
  return false;
}

Client *Manager::ServerInfo::addClient ( ButtonTransition *pTrans )
{
  if ( ! pTrans ) {
    return NULL;
  }

  Client *pClient = new Client ( pTrans  );   //ooo
  //Client *pClient = new Client ( 0, pTrans  );  //xxx
  m_listOfClients.append       ( pClient );

  return pClient;
}

bool Manager::ServerInfo::removeClient ( ButtonTransition *pTrans )
{
  if ( ! pTrans )
    return false;

  Client *pClient = NULL;
  QList<Client *>::iterator it = m_listOfClients.begin ( );	//oxx
  while ( it != m_listOfClients.end ( ) )  {
    pClient = *it++;
    if ( *pClient->buttonTransition ( ) == *pTrans )  {
      //m_listOfClients.remove        ( pClient );	//ooo
      m_listOfClients.removeOne        ( pClient );	//xxx
      pClient->killServerThread     ( );

      delete pClient;
      startNextClient ( );
      return true;
    }
  }
  return false;
}

int Manager::ServerInfo::clientCount ( )
{
  return (int)m_listOfClients.count ( );
}

bool Manager::ServerInfo::startNextClient ( )
{   
  bool    bReturn = false;
  Client *pClient = NULL;
  QList<Client *>::iterator it = m_listOfClients.begin ( );	//oxx
  // In the first run we count how many are currently running.
  unsigned int iCount = 0;

  while ( it != m_listOfClients.end ( ) )  {
    pClient = *it++;
    if ( ! pClient->waitingInQueue ( ) )
      iCount ++;
  }

  it = m_listOfClients.begin ( );
  while ( it != m_listOfClients.end  ( ) )  {
    if  ( iCount >= m_iConcurrentThreads )
      break;
    pClient = *it++;

    if ( pClient->sourceFileEntry ( ) )  {
      //debug_out ( "Starting Client for <%s> waiting<%s>\n", pClient->sourceFileEntry()->qsDisplayName.ascii(), pClient->waitingInQueue ( ) ? "true" : "false");		//ooo
      debug_out ( "Starting Client for <%s> waiting<%s>\n", pClient->sourceFileEntry()->qsDisplayName.toLatin1().data(), pClient->waitingInQueue ( ) ? "true" : "false");	//xxx
    }
    else  {
      //debug_out ( "Starting Client for <%s> waiting<%s>\n", pClient->buttonTransition()->qsName.ascii(), pClient->waitingInQueue ( ) ? "true" : "false");		//ooo
      debug_out ( "Starting Client for <%s> waiting<%s>\n", pClient->buttonTransition()->qsName.toLatin1().data(), pClient->waitingInQueue ( ) ? "true" : "false");	//xxx
    }

    if ( pClient->waitingInQueue ( ) )  {
         pClient->setWaiting ( false );
         pClient->startMe    ( );
         iCount ++;
         bReturn = true;
    }
  }
  return bReturn;
}

Manager::Manager ( )
{
}

Manager::~Manager ( )
{
  lock ( );
  QList<ServerInfo *>::iterator it = m_listOfServer.begin ( );	//oxx
  while ( it != m_listOfServer.end ( ) )
    delete *it++;
  m_listOfServer.clear ( );
  unlock ( );
}

Manager::ServerInfo *Manager::getServerInfo ( unsigned long iServerIP )
{
  ServerInfo *pServerInfo;
  QList<ServerInfo *>::iterator it = m_listOfServer.begin ( );	//oxx
  while ( it != m_listOfServer.end ( ) )  {
    pServerInfo = *it++;
    if ( pServerInfo->getIP ( ) == iServerIP )
      return pServerInfo;
  }
  return NULL;
}

Manager::ServerInfo *Manager::getServerInfo ( Client *pClient )
{
  ServerInfo *pServerInfo;
  QList<ServerInfo *>::iterator it = m_listOfServer.begin ( );	//oxx
  while ( it != m_listOfServer.end ( ) )  {
    pServerInfo = *it++;
    if ( pServerInfo->hasClient ( pClient ) )
      return pServerInfo;
  }
  return NULL;
}

QStringList &Manager::getFilterNames ( )
{
  if ( m_listFilterNames.count ( ) > 0 )
    return m_listFilterNames;

  Utils theUtils;
  QStringList filterList;
  QString qsTempFile, qsFilterName, qsCommand;

  qsCommand = theUtils.getToolPath ( "qrender" );
  if ( qsCommand.isEmpty ( ) )  {
    qsCommand = Global::qsSystemPath + "/bin/qrender";
    QFileInfo fileInfo ( qsCommand );
    if ( ! fileInfo.exists ( ) )  {
      qsCommand = theUtils.checkForExe ( "qrender" );
      if ( qsCommand.isEmpty ( ) )
           qsCommand = "qrender";
    }
  }
  m_listFilterNames.clear ( );

  qsTempFile = Global::qsTempPath  +  "/filterList.txt";
  qsCommand += QString ( " -f >%1" ).arg   ( qsTempFile );
  //int iRet   = system  ( qsCommand.ascii ( ) );		//ooo
  int iRet   = system  ( qsCommand.toLatin1 ( ).data () );	//xxx

  //printf ( "%s\n", qsCommand.ascii ( ) );
  iRet = iRet; // quiet down the compiler.
  //chmod ( qsTempFile.utf8 ( ), S_IRUSR | S_IWUSR | S_IWRITE | S_IROTH | S_IWOTH | S_IWGRP | S_IRGRP );	//ooo
  chmod ( qsTempFile.toUtf8 ( ), S_IRUSR | S_IWUSR | S_IWRITE | S_IROTH | S_IWOTH | S_IWGRP | S_IRGRP );	//xxx

  // Next we read in the temp file
  QFileInfo fileInfo ( qsTempFile );
  if ( fileInfo.size ( ) > 10 )  {
    QFile file ( qsTempFile );
    if ( file.open ( QIODevice::ReadOnly ) )  {
      QTextStream in ( &file ) ;		//xxx
      //while ( ! file.atEnd   ( ) ) {		//ooo
      while ( ! in.atEnd ( ) ) {		//xxx
        //file.readLine ( qsFilterName, 4096 );	//ooo
	qsFilterName = in.readLine ( 4096 );	//xxx
        qsFilterName  = qsFilterName.remove ( "\n" );
        if (  qsFilterName.length  ( ) > 0 )
          m_listFilterNames.append ( qsFilterName );
      }
    }
    file.close ( );
  }

  if ( m_listFilterNames.size ( ) < 1 )  {
    // Hardcode at least the Cross Fade and KenBurns filter.
    m_listFilterNames.append ( "Cross Fade" );
//    m_listFilterNames.append ( "Ken Burns"  );
  }
  return m_listFilterNames;
}

bool Manager::registerEntry ( SourceFileEntry *pEntry )
{ 
  if ( ! pEntry || ! pEntry->bIsSlideshow || pEntry->listFileInfos.count ( ) < 1 )
    return false;

  SourceFileInfo *pInfo = pEntry->listFileInfos[0];
  if ( ! pInfo || ! pInfo->pSlideshow )
    return false;

  // Check if we need to create a new object ...
  if ( m_pSelf == NULL )
       m_pSelf = new Manager;

/* For as long as I develop this Client, we can disregard this check
   This check will make sure a SourceFileEntry ( Slideshow ) is only rendered once.
  QValueList<Client *>::iterator it = m_pSelf->m_listOfClients.begin ( );
  while ( it != m_pSelf->m_listOfClients.end ( ) )  {
    if  ( pEntry == (*it++)->sourceFileEntry ( ) )
      return false;
  }
*/
  usleep ( 10000 ); // wait 10 ms to avoid double clicks to regiseter multiple instances
  unsigned long iServerIP = 0x0F000001;  // 127.0.0.1
  ServerInfo *pServerInfo = m_pSelf->getServerInfo ( iServerIP );
  if ( ! pServerInfo )  {
         pServerInfo =   new ServerInfo ( iServerIP, SERVER_PORT, Global::iMaxRunningThreads );
         m_pSelf->m_listOfServer.append ( pServerInfo );
  }

//  Client *pNewClient = pServerInfo->addClient ( pEntry );
//  pNewClient->start ( );
  pServerInfo->addClient ( pEntry );

  pServerInfo->startNextClient  ( );
  
  //sdebug_out ( " - and start <%s>\n", pEntry->qsDisplayName.ascii ( ) );		//ooo
  sdebug_out ( " - and start <%s>\n", pEntry->qsDisplayName.toLatin1 ( ).data ( ) );	//xxx
  return true;
}

bool Manager::unregisterEntry ( SourceFileEntry *pEntry )
{  
  bool bReturn = false;
  if ( ! pEntry )
    return false;

  // Check if we need to create a new object ...
  if ( m_pSelf == NULL )
       m_pSelf = new Manager;

  m_pSelf->lock ( );

  ServerInfo *pServerInfo = NULL;
  QList<ServerInfo *>::iterator it = m_pSelf->m_listOfServer.begin ( );	//oxx

  while ( it != m_pSelf->m_listOfServer.end ( ) )  {
    pServerInfo = *it++;
    if ( pServerInfo->removeClient  ( pEntry ) )  {
      if ( pServerInfo->clientCount ( )  < 1 )  {
        //m_pSelf->m_listOfServer.remove ( pServerInfo );	//ooo
	    m_pSelf->m_listOfServer.removeOne ( pServerInfo );	//xxx
        delete pServerInfo;
      }
      bReturn = true;
      break;
    }
  }

  m_pSelf->unlock ( );
  return bReturn;
}

bool Manager::registerTrans ( ButtonTransition *pTrans )
{
  if ( ! pTrans || pTrans->bFinishedRendering )
    return false;

  // Check if we need to create a new object ...
  if ( m_pSelf == NULL )
       m_pSelf = new Manager;

//  usleep ( 10000 ); // wait 10 ms to avoid double clicks to regiseter multiple instances
  unsigned long iServerIP = 0x0F000001;  // 127.0.0.1
  ServerInfo *pServerInfo = m_pSelf->getServerInfo ( iServerIP );
  if ( ! pServerInfo )  {
         pServerInfo =   new ServerInfo ( iServerIP, SERVER_PORT, Global::iMaxRunningThreads );
         m_pSelf->m_listOfServer.append ( pServerInfo );
  }

  pServerInfo->addClient ( pTrans );
  pServerInfo->startNextClient  ( );

  //sdebug_out ( " - and start <%s>\n", pTrans->qsName.ascii ( ) );		//ooo
  sdebug_out ( " - and start <%s>\n", pTrans->qsName.toLatin1 ( ).data ( ) );	//xxx

  return true;
}

bool Manager::unregisterTrans ( ButtonTransition *pTrans )
{
  bool bReturn = false;
  if ( ! pTrans )
    return false;

  // Check if we need to create a new object ...
  if ( m_pSelf == NULL )
       m_pSelf = new Manager;

  m_pSelf->lock ( );

  ServerInfo *pServerInfo = NULL;
  QList<ServerInfo *>::iterator it = m_pSelf->m_listOfServer.begin ( );	//oxx
  while ( it != m_pSelf->m_listOfServer.end ( ) )  {
    pServerInfo = *it++;
    if ( pServerInfo->removeClient  ( pTrans ) )  {
      if ( pServerInfo->clientCount ( )  < 1 )  {
        //m_pSelf->m_listOfServer.remove ( pServerInfo );	//ooo
	m_pSelf->m_listOfServer.removeOne ( pServerInfo );	//xxx
        delete pServerInfo;
      }
      bReturn = true;
      break;
    }
  }

  m_pSelf->unlock ( );
  return bReturn;
}

void Manager::lock ( )
{
  m_mutex.lock ( );
}

void Manager::unlock ( )
{
  m_mutex.unlock ( );
}

/**********************************************************
 **
 **  The Client to the Rendering process ( Server )
 **
 **********************************************************/
Client::Client ( SourceFileEntry *pEntry )    //ooo
//Client::Client ( QObject *parent, SourceFileEntry *pEntry ) //xxx-!
  //: QObject ( ), QThread ( )		//ooo
  : QThread ( )				//xxx
  //: QThread (parent)				//xxx-!
  //: QObject ( )				//xxx-!
  //: QObject (parent)				//xxx
  //, m_pSocket(new QTcpSocket(this))   //xxx
  , m_pSocket ( new QTcpSocket )   //xxx
{ 
  initMe ( ); //ooo
  m_pSourceFileEntry = pEntry;
}

Client::Client ( ButtonTransition *pTrans )   //ooo
//Client::Client ( QObject *parent, ButtonTransition *pTrans )    //xxx-!
  //: QObject ( ), QThread ( )		//ooo
  : QThread ( )				//xxx
  //: QThread (parent)			//xxx-!
  //: QObject ( )				//xxx-!
  //: QObject (parent)				//xxx
  , m_pSocket ( new QTcpSocket )   //xxx
{
  initMe ( ); //ooo
  m_pButtonTransition = pTrans;
}

void Client::initMe ( )
{ 
  m_pSourceFileEntry  = NULL;
  m_pButtonTransition = NULL;
  m_bRemoteServer     = true;
  m_bSendImageFiles   = true;
  m_bWaitInQueue      = true;
  m_bSentAllInfo      = false;
  m_fProgress         = 0.0f;
  m_pFile             = NULL;
  m_iMessageID        = 0;
  m_iMessageLen       = 0;
  m_iRetryCounter     = 3;  // Try three times to re-establish a lost connection
  m_iKeepaliveTimer   = 0;
  //m_pSocket = new Q3Socket ( this );	//ooo
  //m_pSocket = new QTcpSocket ( this );	//xxx
  //m_pSocket = new QTcpSocket ( );	//xxx

  stream.setDevice(m_pSocket);    //xxx
  stream.setVersion(QDataStream::Qt_4_0);   //xxx
  
  //connect ( m_pSocket, SIGNAL ( connected        ( ) ), SLOT ( socketConnected ( ) ) );   //ooo
  connect ( m_pSocket, SIGNAL ( connected        ( ) ), this, SLOT ( socketConnected ( ) ) );   //xxx
  //connect ( m_pSocket, SIGNAL ( readyRead        ( ) ), SLOT ( socketReadyRead ( ) ) );   //ooo
  connect ( m_pSocket, SIGNAL ( readyRead        ( ) ), this, SLOT ( socketReadyRead ( ) ) );   //xxx
  //connect ( m_pSocket, &QIODevice::readyRead, this, SLOT ( socketReadyRead ( ) ) );   //xxx
  //connect  (m_pSocket, &QIODevice::readyRead, this, &Client::socketReadyRead ); //xxx
  //connect  (m_pSocket, &QIODevice::readyRead, this, &Render::Client::socketReadyRead ); //xxx
  //connect ( m_pSocket, SIGNAL ( error        ( int ) ), SLOT ( socketError ( int ) ) );   //xxx
  connect ( m_pSocket, SIGNAL ( error ( QAbstractSocket::SocketError ) ), this, SLOT ( socketError ( QAbstractSocket::SocketError ) ) );   //xxx
  //connect ( m_pSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::displayError);  //xxx
  //connect ( m_pSocket, SIGNAL ( error ( QAbstractSocket::SocketError ) ), this, SLOT ( displayError ( QAbstractSocket::SocketError )));  //xxx
  //connect ( m_pSocket, SIGNAL ( connectionClosed ( ) ), SLOT ( socketConnectionClosed ( ) ) );    //ooo
  connect ( m_pSocket, SIGNAL ( disconnected ( ) ), this, SLOT ( socketConnectionClosed ( ) ) );    //xxx
  connect ( this,         SIGNAL ( signalWrite(QByteArray) ), this, SLOT ( slotWrite(QByteArray) ) );    //xxx
  connect ( this,         SIGNAL ( signalWrite(QString) ), this, SLOT ( slotWrite(QString) ) );    //xxx
  //connect ( this,         SIGNAL ( signalWrite(string) ), this, SLOT ( slotWrite(string) ) );    //xxx
  connect ( this,         SIGNAL ( signalFlush( ) ), this, SLOT ( slotFlush( ) ) );    //xxx
  connect ( this,         SIGNAL ( signalEnableTimer(bool) ), this, SLOT ( enableTimer(bool) ) );    //xxx
}

Client::~Client ( )
{
  //enableTimer ( false );  //ooo
  emit signalEnableTimer ( false ); //xxx
  debug_out ( "Client DESTRUCTOR\n" );
  if ( m_pSocket )   {
    //if ( ( m_pSocket->state ( ) != Q3Socket::Closing ) && ( m_pSocket->state ( ) != QAbstractSocket::UnconnectedState ) )		//ooo
    if ( ( m_pSocket->state ( ) != QTcpSocket::ClosingState ) && ( m_pSocket->state ( ) != QAbstractSocket::UnconnectedState ) )	//xxx
           m_pSocket->close ( );
    delete m_pSocket;
    m_pSocket = NULL;
  }
}

/*Worker::Worker(QObject *parent) //xxx
    : QObject(parent)
{   
    //startTimer(50);     // 50-millisecond timer
    //startTimer(1000);   // 1-second timer
    //startTimer(60000);  // 1-minute timer
}

void Worker::enableTimer ( bool bEnable )   //xxx
{
  //m_NotificationTimer = new QTimer(this);   //xxx
  //m_NotificationTimer.moveToThread(this); //xxx
  QTimer timer; //xxx
    
  if ( bEnable )  {
    m_iKeepaliveTimer = startTimer ( I_AM_ALIVE_INTERVAL ); // check every 20 secs if we lost contact to the server   //ooo
    //m_iKeepaliveTimer = m_NotificationTimer->startTimer ( I_AM_ALIVE_INTERVAL ); // check every 20 secs if we lost contact to the server    //xxx
    //m_iKeepaliveTimer = QTimer::start ( I_AM_ALIVE_INTERVAL ); // check every 20 secs if we lost contact to the server    //xxx
    //timer.start(I_AM_ALIVE_INTERVAL);   //xxx
    //m_iKeepaliveTimer = timer.startTimer(I_AM_ALIVE_INTERVAL);   //xxx
    m_lastPing = QDateTime::currentDateTime ( );
  }
  else  {
    killTimer ( m_iKeepaliveTimer );  //ooo
    //m_NotificationTimer->killTimer ( m_iKeepaliveTimer );   //xxx
    //this->killTimer ( m_iKeepaliveTimer );   //xxx
    m_iKeepaliveTimer = 0;
  }
}*/ //xxx

/*void Client::enableTimer1 ( bool bEnable )  //xxx
{
  //m_NotificationTimer = new QTimer();   //xxx
  //m_NotificationTimer->setInterval(1000);   //xxx
  //m_NotificationTimer->start(); //xxx
  //m_NotificationTimer.moveToThread(this); //xxx
  //m_NotificationTimer->moveToThread(this);  //xxx
  QTimer timer; //xxx
    
  if ( bEnable )  {
    m_iKeepaliveTimer = startTimer ( I_AM_ALIVE_INTERVAL ); // check every 20 secs if we lost contact to the server   //ooo
    //m_iKeepaliveTimer = startTimer ( static_cast<int> ( I_AM_ALIVE_INTERVAL ) );  //xxx
    //m_iKeepaliveTimer = m_NotificationTimer->startTimer ( I_AM_ALIVE_INTERVAL );  //xxx-?
    //m_NotificationTimer->start ( I_AM_ALIVE_INTERVAL );    //xxx
    //m_iKeepaliveTimer = QTimer::start ( I_AM_ALIVE_INTERVAL );    //xxx
    //timer.start(I_AM_ALIVE_INTERVAL);   //xxx
    //m_iKeepaliveTimer = timer.startTimer(I_AM_ALIVE_INTERVAL);   //xxx
    //timer.start(I_AM_ALIVE_INTERVAL);   //xxx
    m_lastPing = QDateTime::currentDateTime ( );
  }
  else  {
    killTimer ( m_iKeepaliveTimer );  //ooo
    //m_NotificationTimer->killTimer ( m_iKeepaliveTimer );   //xxx
    //this->killTimer ( m_iKeepaliveTimer );   //xxx
    m_iKeepaliveTimer = 0;
  }
}*/ //xxx

void Client::enableTimer ( bool bEnable )
{
  if ( bEnable )  {
    m_iKeepaliveTimer = startTimer ( I_AM_ALIVE_INTERVAL ); // check every 20 secs if we lost contact to the server
    m_lastPing = QDateTime::currentDateTime ( );
  }
  else  {
    killTimer ( m_iKeepaliveTimer );
    m_iKeepaliveTimer = 0;
  }
}

void Client::slotReconnect ( )
{ 
  Manager::ServerInfo *pServerInfo = Manager::m_pSelf->getServerInfo ( this );
  if ( pServerInfo )
       pServerInfo->setStarting ( false );
  
  //QHostAddress addr("127.0.0.1");   //xxx
  //quint16 m_serverPort = 5066;  //xxx
  if ( m_pSocket ) {   //ooo
       m_pSocket->abort();  //xxx
       //m_pSocket->connectToHost ( "localhost", 5066 );    //ooo
       //m_pSocket->connectToHost ( addr, 5066 );  //xxx
       m_pSocket->connectToHost(QHostAddress::LocalHost, 5066);  //xxx
       //m_pSocket->connectToHost(QHostAddress::LocalHost, m_serverPort);  //xxx
  }

  debug_out ( "\n" ); //ooo
}

void Client::closeConnection ( )
{
  debug_out ( "\n" );
  m_pSocket->close ( );
  //if ( m_pSocket->state ( ) == Q3Socket::Closing ) // We have a delayed close.	//ooo
  if ( m_pSocket->state ( ) == QTcpSocket::ClosingState ) // We have a delayed close.	//xxx
      //connect ( m_pSocket, SIGNAL ( delayedCloseFinished ( ) ), SLOT ( socketClosed ( ) ) );  //ooo
      connect ( m_pSocket, SIGNAL ( disconnected ( ) ), this, SLOT ( socketClosed ( ) ) );  //xxx
  else // The socket is closed.
      socketClosed ( );
}

// Data received from the QRender server
// Note: if QRender and QDVDAuthor are on the same computer
//       then we do NOT send the files over the socket
//       but instead send only the file names.
void Client::socketReadyRead ( )
{
  //Q_ULONG iLen = m_pSocket->bytesAvailable ( );	//ooo
  qint64 iLen = m_pSocket->bytesAvailable ( );		//xxx
  
  //sleep(30);    //xxx
  
  while ( iLen > 0 )  {
    //if  ( iLen < sizeof ( Q_UINT16 ) )	//ooo
    if  ( iLen < (uint)sizeof ( quint16 ) )	//xxx
      return;

    //Q_UINT16 iPrev = m_iMessageID;	//ooo
    quint16 iPrev = m_iMessageID;	//xxx
    if ( m_iMessageID == 0 )  {
      //char cMsgID  [ sizeof ( Q_UINT16 ) ];	//ooo
      char cMsgID  [ sizeof ( quint16 ) ];	//xxx
      //Q_LONG iRead1 = m_pSocket->readBlock ( cMsgID,  sizeof ( Q_UINT16 ) );	//ooo
      //qint64 iRead1 = m_pSocket->read ( cMsgID,  sizeof ( qint64 ) );		//xxx
      qint64 iRead1 = m_pSocket->read ( cMsgID,  sizeof ( quint16 ) );		//xxx
      
      //if ( iRead1 == sizeof ( Q_UINT16 ) )	//ooo
      if ( iRead1 == sizeof ( quint16 ) )	//xxx
        //m_iMessageID  = *(Q_UINT16 *)cMsgID;	//ooo
	    m_iMessageID  = *(quint16 *)cMsgID;	//xxx
        //memcpy( &m_iMessageID, cMsgID, 2 );   //xxx
    }
    debug_out ( " prev<0x%04X> curr<0x%04X>\n", iPrev, m_iMessageID );

    switch ( m_iMessageID )  {
      case SERVER_YOUR_PROGRESS_SIR:
        receivedProgress ( );
      break;
      case SERVER_ITS_ME_AND_I_AM_AT:  // The first response from the server is its version and host name
        receivedVersionAndHost ( );
      break;
      case SERVER_MY_STATUS_SIRE: // Status after exchange of XML file
        receivedServerState ( );
      break;
      case SERVER_I_AM_ALIVE:  // Will detect if the server went down or is unreachable
        receivedPing ( );      // Timer
      break;
      case SERVER_STICK_A_FORK_IN_ME:  // Will detect if the server went down or is unreachable
        receivedServerDone ( );
      break;
      case SERVER_GOT_A_PIECE: // QDVDAuthor sending files to server and server ack
        receivedFileAck ( );   // the reception so I can go ahead and send the next chunk
      break;
      case SERVER_TAKE_A_FILE: // The final vob file is coming in ...
        fileFromSocket ( );
        return;
      default:
        debug_out ( " Unknown MsgID : prevMsgID<0x%04X> currMsgID<0x%04X>\n", iPrev, m_iMessageID );
      break; // don't do nothin'
    }        // end switch statement
    m_iMessageID  = 0;
    m_iMessageLen = 0LL;
    iLen = m_pSocket->bytesAvailable ( );
  }
}

void Client::receivedProgress ( )
{  
  double fProgress;
  //char cMsgLen [ sizeof ( Q_UINT64 ) ];	//ooo
  char cMsgLen [ sizeof ( quint64 ) ];		//xxx
  //m_pSocket->readBlock  ( cMsgLen, sizeof ( Q_UINT64 ) );		//ooo
  //m_pSocket->read  ( cMsgLen, sizeof ( qint64 ) );			//xxx
  m_pSocket->read  ( cMsgLen, sizeof ( quint64 ) );			//xxx
  //m_pSocket->readBlock  ( (char *)&fProgress, sizeof ( double ) );	//ooo
  //m_pSocket->read  ( (char *)&fProgress, sizeof ( double ) );		//xxx
  m_pSocket->read  ( (char *)&fProgress, sizeof ( double ) );		//xxx
  // Modify Client's data
  lock ( );
  m_fProgress = fProgress;
  unlock ( );

  m_lastPing = QDateTime::currentDateTime ( );
  displayProgress ( );
}

void Client::receivedPing ( )
{
  // dully noted !
  // Need to implement the detection of WENT_MISSING_AFTER event
  // This message should come in every I_AM_ALIVE_INTERVAL mseconds
  // Only exception is when the final VOB file is sent back to QDVDAuthor
  // we disable this timer.

  m_lastPing = QDateTime::currentDateTime ( );
  debug_out ( "Received PING from Server\n" );
}

void Worker::timerEvent ( QTimerEvent *pEvent )
{
}

void Client::timerEvent ( QTimerEvent *pEvent )
{
  //Somehow I get remote server even though it should always be local ( for now ).
  Manager::ServerInfo *p = Manager::m_pSelf->getServerInfo ( this );
  printf ( "in <%p>\n", p );
  if ( ! p )
    return;
  in_addr in;
  in.s_addr = p->getIP   (  );
  const char *pServer = inet_ntoa ( in );
  //printf ( "bRemoteServer<%d> IP=<0x%016X> = <%s>\n", m_bRemoteServer, p->getIP ( ), pServer );    //oooo
  printf ( "bRemoteServer<%d> IP=<0x%016X> = <%s>\n", m_bRemoteServer, static_cast<unsigned int>(p->getIP ( )), pServer );    //xxx
  
  if ( pEvent->timerId ( ) == m_iKeepaliveTimer )  {
    //debug_out ( " last<%s> Curr<%s>\n", m_lastPing.toString ( ).ascii ( ), QDateTime::currentDateTime ( ).toString ( ).ascii ( ) );				//ooo
    debug_out ( " last<%s> Curr<%s>\n", m_lastPing.toString ( ).toLatin1 ( ).data ( ), QDateTime::currentDateTime ( ).toString ( ).toLatin1 ( ).data ( ) );	//xxx

    if ( m_lastPing.secsTo ( QDateTime::currentDateTime ( ) ) > WENT_MISSING_AFTER * 2 ) { //I_AM_ALIVE_INTERVAL )  {
      // Oh crap happened ... should I try to restart ?
      // Should I notify the user
      // should I handle it internally ?f
      Manager::ServerInfo *pServerInfo = Manager::m_pSelf->getServerInfo ( this );
      if ( ! pServerInfo )
        return;

      if ( m_bRemoteServer )  {
        if ( ! pServerInfo->inRecovery ( ) )  {
          QString qsHtml, qsTime, qsServer;
          in_addr in;
          in.s_addr = pServerInfo->getIP   (  );
          const char *pServer = inet_ntoa ( in );

          qsTime  = QString ( "%1" ).arg ( (int)(WENT_MISSING_AFTER * I_AM_ALIVE_INTERVAL / 1000.0 ) );
          qsServer= QString ( "%1:%2" ).arg ( pServer ).arg ( pServerInfo->getPort ( ) );

          qsHtml  = tr ( "I could not reach the server %1 for %2 seconds.\n" ).arg ( qsServer ).arg ( qsTime );
          qsHtml += tr ( "Please check if the server is still running and restart if neccesary.\n\n" );

          qsHtml += tr ( "Once the server is responsive again, QDVDAuthor will reestablish the connection\n" );
          qsHtml += tr ( "between QDVDAuthor and QRender.\n" );

          MessageBox::html ( NULL, tr ( "Render server down" ), qsHtml, QMessageBox::Ok, QMessageBox::NoButton );
          pServerInfo->setRecovery ( true );
        }
      }
      else  {
        if ( m_pSocket->isOpen ( ) && m_qsTempXMLFile.length ( ) > 0 )  {
          //QDataStream stream  ( m_pSocket );  //ooo
          QString Data; //xxx
          //stream << (Q_UINT16)CLIENT_I_LOST_CONTACT << (Q_UINT64)m_qsTempXMLFile.length ( );	//ooo
	      //stream << (quint16)CLIENT_I_LOST_CONTACT << (quint64)m_qsTempXMLFile.length ( );	//xxx
          //stream << static_cast<quint16>(CLIENT_I_LOST_CONTACT) << static_cast<quint64>(m_qsTempXMLFile.length ( ));	//xxx
          Data = byteArrayfromMessageID ( CLIENT_I_LOST_CONTACT ) + Message<quint64>::create ( m_qsTempXMLFile.length ( ) );	//xxx
          emit signalWrite(Data);   //xxx
          //stream << m_qsTempXMLFile;  //ooo
          emit signalWrite(m_qsTempXMLFile);   //xxx
          //m_pSocket->flush ( );   //ooo
          emit signalFlush();   //xxx
        }
        else  {
          startServer ( );
          pServerInfo->setRecovery ( true );
          // run ( );
          //start ( );
          //QDataStream stream  ( m_pSocket );  //ooo
          QString Data; //xxx
          //stream << (Q_UINT16)CLIENT_GO_TO_WORK << (Q_UINT64)0LL;	//ooo
	      //stream << (quint16)CLIENT_GO_TO_WORK << (quint64)0LL;		//xxx
	      Data = byteArrayfromMessageID ( CLIENT_GO_TO_WORK ) + Message<quint64>::create ( 0LL );		//xxx
	      emit signalWrite(Data);  //xxx
          //m_pSocket->flush ( );   //ooo
          emit signalFlush();   //xxx
        }
      }
    }
    else
      m_lastPing = QDateTime::currentDateTime ( );
  }
}

void Client::receivedFileAck ( )
{
  //Q_UINT64 iReceived;	//ooo
  quint64 iReceived;	//xxx
  QDataStream stream ( m_pSocket );
  stream >> iReceived;

  if ( THROTTLE )
    usleep ( THROTTLE ); // throttle a bit
  sendNextPacket ( );
}

void Client::receivedVersionAndHost ( )
{
  QString qsHostName, qsVersion;
  //char cMsgLen [ sizeof ( Q_UINT64 ) ];	//ooo
  char cMsgLen [ sizeof ( quint64 ) ];		//xxx
  //Q_LONG iRead2 = m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );
  //m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );	//ooo
  //m_pSocket->read ( cMsgLen, sizeof ( qint64 ) );		//xxx
  m_pSocket->read ( cMsgLen, sizeof ( quint64 ) );		//xxx
  //m_iMessageLen = *(Q_UINT64 *)cMsgLen;	//ooo
  m_iMessageLen = *(quint64 *)cMsgLen;		//xxx

  QDataStream ds ( m_pSocket );
  ds >> qsHostName >> qsVersion;

  char cHostName[1024];
  gethostname ( (char *)&cHostName, 1024 );
  QString qsClientHost ( cHostName       );
  if ( qsClientHost == qsHostName )
    m_bRemoteServer  = false;
#ifdef FAKE_REMOTE_HOST
  m_bRemoteServer = true;
#endif

  // On a local host the wakeAll can come in before the wait is entered, thus we give it a shot break here
  usleep     ( 10000 );  // 10 msec
  m_waiter.wakeAll ( );
}

void Client::receivedServerState ( )
{
  QString qsStatus;
  //char cMsgLen [ sizeof ( Q_UINT64 ) ];	//ooo
  char cMsgLen [ sizeof ( quint64 ) ];		//xxx

  m_iMessageLen  = 0L;
  //Q_ULONG iAvail = 0L;	//ooo
  qint64 iAvail = 0L;		//xxx
  //Q_ULONG iRead  = 0L;	//ooo
  qint64 iRead  = 0L;		//xxx
  //if ( m_pSocket->state ( ) == Q3Socket::Connected )  {	//ooo
  if ( m_pSocket->state ( ) == QTcpSocket::ConnectedState )  {	//xxx
    iAvail  = m_pSocket->bytesAvailable ( );
    if ( iAvail > 0 )  {
      //iRead = m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );	//ooo
      //iRead = m_pSocket->read ( cMsgLen, sizeof ( qint64 ) );		//xxx
      iRead = m_pSocket->read ( cMsgLen, sizeof ( quint64 ) );		//xxx  
      if ( iRead > 0 )  {
        //m_iMessageLen = *(Q_UINT64 *)cMsgLen;	//ooo
        //m_iMessageLen = *(qint64 *)cMsgLen;	//xxx
        m_iMessageLen = *(quint64 *)cMsgLen;	//xxx  

        QDataStream ds ( m_pSocket );
        ds >> qsStatus;

        if ( qsStatus == "SameXML" ) 
          m_bSendImageFiles = false;
      }
      else  {
        //Q3SocketDevice::Error err = m_pSocket->socketDevice ( )->error ( );	//ooo
	    QTcpSocket::SocketError err = m_pSocket->error ( );			//xxx
        printf ( "%s::%d > There was an Socket Error <%d>\n", __FILE__, __LINE__, err );
      }
    }
  }

  usleep ( 100000 ); // 10 ms break ...
  qApp->processEvents ( );
  m_waiter.wakeAll    ( );
}

void Client::receivedServerDone ( )
{  
  //enableTimer ( false );  //ooo
  emit signalEnableTimer ( false ); //xxx

  //int hint;	//xxx
  QString qsStatus;
  //char cMsgLen [ sizeof ( Q_UINT64 ) ];	//ooo
  char cMsgLen [ sizeof ( quint64 ) ];		//xxx
  //memset ( &cMsgLen, 0, sizeof ( Q_UINT64 ) );//ooo
  memset ( &cMsgLen, 0, sizeof ( quint64 ) );	//xxx

  //Q_ULONG iAvail = 0L;	//ooo
  qint64 iAvail = 0L;		//xxx
  //Q_ULONG iRead  = 0L;	//ooo
  qint64 iRead  = 0L;		//xxx
  //if ( m_pSocket->state ( ) == Q3Socket::Connected )  {	//ooo
  if ( m_pSocket->state ( ) == QTcpSocket::ConnectedState )  {	//xxx
    iAvail  = m_pSocket->bytesAvailable ( );
    if ( iAvail > 0 )  {
      //iRead = m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );	//ooo
      //iRead = m_pSocket->read ( cMsgLen, sizeof ( qint64 ) );		//xxx
      iRead = m_pSocket->read ( cMsgLen, sizeof ( quint64 ) );		//xxx  
      if ( iRead > 0 )  {
        //Q_UINT64 iLen = *(Q_UINT64 *)cMsgLen;	//ooo
        quint64 iLen = *(quint64 *)cMsgLen;	//xxx

        if ( iLen < 10 )  { // Should be a length of 4 ( "Done" )
          QDataStream ds ( m_pSocket );
          ds >> qsStatus;
        }
        else  {
          //Q3SocketDevice::Error err = m_pSocket->socketDevice ( )->error ( );		//ooo
          QTcpSocket::SocketError err = m_pSocket->error ( );				//xxx
          printf ( "%s::%d > There was an Socket Error <%d>\n", __FILE__, __LINE__, err );
        }
      }
    }
  }

  QCustomEvent *pNewEvent = new QCustomEvent ( EVENT_RENDER_EXIT );	//ooo
  //pNewEvent->setData      ( (void *) this );		//ooo
  QApplication::postEvent ( Global::pApp, pNewEvent );
}

void Client::killServerThread ( )
{
  if ( ! m_pSocket->isOpen ( ) )
    slotReconnect ( );
  //QDataStream stream ( m_pSocket );   //ooo
  QString Data; //xxx
  //stream << (Q_UINT16)CLIENT_KILL_YOURSELF << (Q_UINT64)0LL;	//ooo
  //stream << (quint16)CLIENT_KILL_YOURSELF << (quint64)0LL;	//xxx
  Data = byteArrayfromMessageID ( CLIENT_KILL_YOURSELF ) + Message<quint64>::create ( 0LL );	//xxx
  emit slotWrite(Data); //xxx
  //m_pSocket->flush ( );   //ooo
  emit slotFlush(); //xxx
}

bool Client::sendFile ( QString qsFileName )
{
  if ( m_pFile )
    return false;

  if ( m_bRemoteServer )  {
    m_pFile = new QFile ( qsFileName );
    if ( ! m_pFile->exists ( ) )  {
      delete m_pFile;
      m_pFile = NULL;
      return false;
    }
    m_pFile->open ( QIODevice::ReadOnly );
  }

  // This will kick off the sendPacket protocol until the whole file is transmittet.
  //QDataStream stream ( m_pSocket );   //ooo
  //QString Data; //xxx
  //stream << (Q_UINT16)CLIENT_TAKE_A_FILE << (Q_UINT64)m_pFile->size ( );	//ooo
  stream << (quint16)CLIENT_TAKE_A_FILE << (quint64)m_pFile->size ( );		//xxx
  //Data = (quint16)CLIENT_TAKE_A_FILE + (quint64)m_pFile->size ( );		//xxx
  stream << qsFileName;   //ooo
  //Data += qsFileName;   //xxx
  //emit signalWrite(Data);   //xxx

  // This function will wait until the whole file has been sent to the server.
  //m_waiter.wait ( ); // in mSec or ULONG_MAX	//ooo
  m_waiter.wait (&m_waiterMutex, ULONG_MAX ); // in mSec or ULONG_MAX	//xxx
  //SleeperThread6::msleep ( 1 );			//xxx

  return true;
}

void Client::sendNextPacket ( )
{
  char data[BLOCK_LEN];
  if ( ! m_pFile )
    return;

  //int  iLen  = m_pFile->readBlock ( data, BLOCK_LEN );	//ooo
  int  iLen  = m_pFile->read ( data, BLOCK_LEN );		//xxx
  if ( iLen != -1 )  {
    //m_pSocket->writeBlock ( data, iLen );	//ooo
    m_pSocket->write ( data, iLen );		//xxx
    //Q_ULONG iPos  = m_pFile->at   ( );	//ooo
    qint64 iPos  = m_pFile->pos   ( );		//xxx
    //Q_ULONG iSize = m_pFile->size ( );	//ooo
    qint64 iSize = m_pFile->size ( );		//xxx

    if ( iPos >= iSize ) {
      m_pFile->close   ( );
      m_pSocket->flush ( );
      delete m_pFile;
      m_pFile = NULL;
      usleep ( 100 );
      m_waiter.wakeAll ( );
    }
//    emit sendingNonAckPercent ( fPercentage );
  }
}

bool Client::checkSocket ( )
{
  if ( ! m_pSocket )
    return false;

  if ( ! m_pSocket->isOpen ( ) )
    return false;
  //if ( m_pSocket->state ( ) != Q3Socket::Connected )		//ooo
  if ( m_pSocket->state ( ) != QTcpSocket::ConnectedState )	//xxx
    return false;

  return true;
}

bool Client::sendVersionAndHostRequest ( )
{   
  char cHostName[1024];
  gethostname ( (char *)&cHostName, 1024 );
  QString  qsHostName  ( cHostName       );
  if ( ! checkSocket ( ) )
    return false;
 
  //QDataStream stream ( m_pSocket );   //ooo
  QByteArray Data;  //xxx
  //stream << (Q_UINT16)CLIENT_WHO_AND_WHERE_ARE_YOU << (Q_UINT64)qsHostName.length ( );	//ooo
  //stream << static_cast<quint16> ( CLIENT_WHO_AND_WHERE_ARE_YOU ) << static_cast<quint64> ( qsHostName.length ( ) );		//xxx
  Data = byteArrayfromMessageID ( CLIENT_WHO_AND_WHERE_ARE_YOU ) + Message<quint64>::create ( qsHostName.length ( ) );		//xxx
  emit signalWrite(Data);   //xxx
  //stream << qsHostName;   //ooo
  emit signalWrite(qsHostName);   //xxx
  //m_pSocket->flush ( );   //ooo
  emit signalFlush();   //xxx
  debug_out ( " <0x%04X>\n", CLIENT_WHO_AND_WHERE_ARE_YOU );
  //return m_waiter.wait ( 30000 ); // wait for 30 seconds max			//ooo
  //return m_waiter.wait ( &m_waiterMutex, 30000 ); // wait for 30 seconds max	//xxx
  bool b_wait = m_waiter.wait ( &m_waiterMutex, 30000 ); // wait for 30 seconds max	//xxx

  return b_wait;    //xxx
  
}

void Client::startMe ( )
{ 
  m_iRetryCounter = 3;
  QTimer::singleShot ( 10, this, SLOT ( slotConnectAndStart ( ) ) );  //ooo-?
  //QTimer::singleShot ( 5, this, SLOT ( slotConnectAndStart ( ) ) );  //xxx
}

void Client::slotConnectAndStart ( )
{
  //start  ( );   //xxx-?

  //if ( m_pSocket->state ( ) == Q3Socket::Connected )  {	//ooo
  if ( m_pSocket->state ( ) == QTcpSocket::ConnectedState )  {	//xxx
  
    usleep ( 10000 ); // in case we are not yet out of run()
    start  ( ); // -=> Execue run() in new thread //ooo
    //run();  //xxx
    return;
  }
  else if ( m_pSocket->state ( ) == QAbstractSocket::UnconnectedState )  {   
    //disconnect ( m_pSocket, SLOT   ( socketConnected ( ) ) ); //ooo
    //disconnect ( m_pSocket, SIGNAL ( connected ( ) ), this, SLOT   ( socketConnected ( ) ) ); //xxx
    disconnect ( m_pSocket, SIGNAL ( disconnected ( ) ), this, SLOT   ( socketConnected ( ) ) ); //xxx
    //connect    ( m_pSocket, SIGNAL ( connected ( ) ), SLOT ( socketConnectAndStart ( ) ) );   //ooo
    connect    ( m_pSocket, SIGNAL ( connected ( ) ), this, SLOT ( socketConnectAndStart ( ) ) );   //xxx

    startServer( );
    //run();  //xxx
    usleep  ( 10000 );  // wait 10 milli second for the connection to happen.
    return;
  }
  
  if ( m_iRetryCounter-- > 0 )
    QTimer::singleShot ( 1000, this, SLOT ( slotConnectAndStart ( ) ) );
  else  {
    debug_out ( "Giving up trying to connect to server.\n" ); //ooo
  }   //oooo-?
}

void Client::socketConnectAndStart ( )
{  
  //disconnect ( m_pSocket, SLOT   ( socketConnectAndStart ( ) ) ); //ooo
  disconnect ( m_pSocket, SIGNAL ( disconnected ( ) ), this, SLOT   ( socketConnectAndStart ( ) ) );   //xxx
  //connect    ( m_pSocket, SIGNAL ( connected ( ) ), SLOT ( socketConnected ( ) ) );   //ooo
  connect    ( m_pSocket, SIGNAL ( connected ( ) )   , this, SLOT ( socketConnected ( ) ) );   //xxx
  start ( );  //ooo-!
  //run();    //xxx
}

void Client::socketConnected ( )
{
  debug_out ( "\n" );
  m_waiter.wakeAll ( );
}

void Client::socketConnectionClosed ( )
{
  debug_out ( "Connection Closed By server : progress<%f>\n", m_fProgress );
  if ( ( m_fProgress < 100.0f ) && (  m_fProgress > 0.0f  ) )
    restartServer ( );
}

void Client::socketClosed ( )
{
  debug_out ( "Connection closed\n" );
}

//void Client::socketError ( int e )    //ooo
void Client::socketError ( QAbstractSocket::SocketError socketError )   //xxx
{ 
  //debug_out ( "ErroNumber<%d>\n", e );    //ooo
  debug_out ( "ErroNumber<%d>\n", socketError );    //xxx
  
  if ( m_iRetryCounter > 0 )  {
    m_iRetryCounter --;
    // Here is the reconnection protocol
    // 1) Try to re-establish connection with server
    //    - If server is dead, then ( 3 times )
    //      o check if we have already sent all info.
    //      o try to restart server
    //      o try to reconnect
    //    - If connection is re-established, then
    //      o send msg RECONNECT ( worker id or worker name )
    //      o If server finds info, then send current progress
    //      o If server cant find info, then delete this Client and re-register
//    if ( ! m_bSentAllInfo )  {
    if ( ! m_bSentAllInfo )  {  //xxx
      Client::reRequest ( this );
      //return; //ooo
//    }
    } else  //xxx
        return;  //xxx
        
    debug_out ( "Retry<%d>\n", m_iRetryCounter );

    startServer ( );
    m_iRetryCounter = 5;
    QTimer::singleShot ( 1000, this, SLOT ( slotRecovered ( ) ) );
  }
}

/*void Client::displayError(QAbstractSocket::SocketError socketError) //xxx
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(nullptr, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(nullptr, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(nullptr, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(m_pSocket->errorString()));
    }

    //getFortuneButton->setEnabled(true);
}*/

void Client::slotRecovered ( )
{  
  // Called afer socket error.
  //if ( m_pSocket && m_pSocket->state ( ) == Q3Socket::Connected )  {		//ooo
  if ( m_pSocket && m_pSocket->state ( ) == QTcpSocket::ConnectedState )  {	//xxx
    QString qsName;
    if ( sourceFileEntry ( ) )
      qsName = sourceFileEntry  ( )->qsDisplayName;
    else if (  buttonTransition ( ) )
      qsName = buttonTransition ( )->qsName;

    //debug_out ( "Reconnecting for <%s>[%u]\n", qsName.ascii ( ), (Q_UINT16)qsName.length ( ) );		//ooo
    debug_out ( "Reconnecting for <%s>[%u]\n", qsName.toLatin1 ( ).data ( ), (quint16)qsName.length ( ) );	//xxx
    //QDataStream stream ( m_pSocket ); //ooo
    QByteArray Data;   //xxx
    //stream << (Q_UINT16)CLIENT_RECONNECTING << (Q_UINT16)qsName.length ( );	//ooo
    //stream << (quint16)CLIENT_RECONNECTING << (quint16)qsName.length ( );	//xxx
    Data = byteArrayfromMessageID ( CLIENT_RECONNECTING ) + Message<quint64>::create ( qsName.length ( ) );	//xxx
    emit signalWrite(Data); //xxx
    //stream << qsName; //ooo
    emit signalWrite(qsName); //xxx
    //m_pSocket->flush ( ); //ooo
    emit signalFlush(); //xxx
    //m_waiter.wait ( 30000 ); // wait for 30 seconds max		//ooo
    m_waiter.wait ( &m_waiterMutex, 30000 ); // wait for 30 seconds max		//xxx
    //SleeperThread6::msleep ( 30000 ); // wait for 30 seconds max	//xxx
  }
  else if ( m_iRetryCounter > 0 )  {
    m_iRetryCounter --;
    debug_out ( "Trying again <%d> left\n",  m_iRetryCounter );
    QTimer::singleShot  ( 1000, this, SLOT ( slotRecovered ( ) ) );
  }
  else  {
    // Can not establish connection, try the whole rendering again
    Client::reRequest ( this );
  }
}

void Client::reRequest ( Client *pSelf )
{   
  if ( pSelf->sourceFileEntry(  )  )  {
    sdebug_out ( "\n" );
    SourceFileEntry *pEntry = pSelf->sourceFileEntry ( );
    Manager::unregisterEntry ( pEntry );
    Manager::registerEntry   ( pEntry );
  }
  else if ( pSelf->buttonTransition ( ) )  {
    sdebug_out ( "\n" );
    ButtonTransition *pTransition = pSelf->buttonTransition ( );
    Manager::unregisterTrans ( pTransition );
    Manager::registerTrans   ( pTransition );
  }
}

void Client::startServer ( )
{
  Manager::ServerInfo *pServerInfo = Manager::m_pSelf->getServerInfo ( this );
  
  if ( pServerInfo )
       pServerInfo->startServer ( );

  //QTimer::singleShot ( 1000, this, SLOT ( slotReconnect ( ) ) );    //oooo  //
  // We must be blocking here otherwise the timer would be fired way later
  // if the main loop is busy E.g. creating meus from a plugin
  //usleep ( 100000 );  //ooo
  //usleep ( 10000000 );  //xxx
  sleep ( 10 );  //xxx
  //QThread::sleep(20); //xxx
  slotReconnect  ( ); //ooo

  debug_out ( "start server\n" );
}

void Client::restartServer ( )
{
  for ( int t=0; t<3; t++ )  {
    // if no connection after three attempts, then restart the server ...
    slotReconnect ( );
    //if ( ( m_pSocket->state ( ) == Q3Socket::Connected  ) ||		//ooo
    if ( ( m_pSocket->state ( ) == QTcpSocket::ConnectedState  ) ||	//xxx
         //( m_pSocket->state ( ) == Q3Socket::Connecting ) ) {		//ooo
         ( m_pSocket->state ( ) == QTcpSocket::ConnectingState ) ) {	//xxx
       debug_out ( "Reconnected\n" );
       return;
    }
    usleep ( 1000000 );  // Now wait a second ...
  }
  startServer  ( );
  usleep ( 10000 );  // give it 10 milli seconds
  //if ( running ( ) )	//ooo
  //if ( isRunning ( ) )	//xxx
    //terminate  ( ); //ooo

  start  ( ); //ooo
  //run ( );  //xxx  
  debug_out ( "restart\n" );
}

void Client::onTimeout()
{
  qDebug()<<"Worker::onTimeout get called from?: "<<QThread::currentThreadId();
}

void Client::run ( )
{
  debug_out ( "\n" );
  //initMe ( );   //xxx
  m_bSentAllInfo = false;

  if ( m_pSocket->state ( ) == QAbstractSocket::UnconnectedState )  {
    startMe ( );
    return;
//    startServer  ( );
//    usleep ( 10000 );  // wait 10 milli second for the connection to happen.
//    for ( int t=0; t<5; t++ )  {
//      if ( m_pSocket->state ( ) != QSocket::Connected )
//        usleep ( 1000000 );  // Now wait a second ...
//    }
  }
  //if ( m_pSocket->state ( ) != Q3Socket::Connected )  {	//ooo
  if ( m_pSocket->state ( ) != QTcpSocket::ConnectedState )  {	//xxx
    QString qsName = m_pSourceFileEntry ? m_pSourceFileEntry->qsDisplayName : m_pButtonTransition->qsName;
    //debug_out ( "Giving up connecting to the server for %s.\n", qsName.ascii ( ) );		//ooo
    debug_out ( "Giving up connecting to the server for %s.\n", qsName.toLatin1 ( ).data ( ) );	//xxx
    return;
  }
  
  // Executed in background thread and active as long as the rendering process is active for 
  // this SourceFileEntry.
  // Here is what we have to do;
  // 1) send slideshow XML file
  // 2) receive status from rendering process
  //    - New slideshow
  //    - Slideshow is already in the making
  //    - Slideshow was interrupted ( crash )
  //    - Slideshow ( .vob file ) already exists ( and log is clear )
  // 3) send all image files
  //    - Note: if on same server, then we can omit this step
  // 4) receive progress notification
  // 5) Finish up.
  
  if ( m_pSourceFileEntry )
    runEntry ( );
  else if ( m_pButtonTransition )
    runTrans ( );

  m_bSentAllInfo = true;
  //m_iKeepaliveTimer = startTimer ( I_AM_ALIVE_INTERVAL );   //xxx
  //exec();   //xxx-!
}

void Client::runSlideshow ( CXmlSlideshow *pSlideshow )
{
  if ( ! pSlideshow )
    return;

  Utils theUtils;
  // Now that the connection is/should be established, we can start the Keepalive - timer.
  //enableTimer ( true );   //ooo-!
  emit signalEnableTimer ( true );  //xxx

  if ( m_pSourceFileEntry )  {
    //debug_out ( "Run for entry<%s> \n", m_pSourceFileEntry->qsDisplayName.ascii ( ) );		//ooo
    debug_out ( "Run for entry<%s> \n", m_pSourceFileEntry->qsDisplayName.toLatin1 ( ).data ( ) );	//xxx
  }
  else if ( m_pButtonTransition )  {
    //debug_out ( "Run for entry<%s> \n", m_pButtonTransition->qsName.ascii ( ) );		//ooo
    debug_out ( "Run for entry<%s> \n", m_pButtonTransition->qsName.toLatin1 ( ).data ( ) );	//xxx
  }

  int iCounter = 0;
  while ( iCounter++ <  10 )  {
    //if  ( m_pSocket->state ( ) != Q3Socket::Connected )  {		//ooo
    if  ( m_pSocket->state ( ) != QTcpSocket::ConnectedState )  {	//xxx
      // wait for the connection event to kick in.
      debug_out ( "before WAIT <%d> \n", iCounter );
      //m_waiter.wait  ( 1000 ); // in mSec or ULONG_MAX				//ooo
      m_waiter.wait  ( &m_waiterMutex, 1000 ); // in mSec or ULONG_MAX			//xxx
      //usleep ( 100000 );  // Socket just connected. Give it 10 msec.			//ooo
      //SleeperThread6::msleep ( 10 );  // Socket just connected. Give it 10 msec.	//xxx
      sleep ( 10 );  // Socket just connected. Give it 10 msec.	//xxx
      //usleep ( 10000 ); //xxx
    }
  }

  if ( ! sendVersionAndHostRequest ( ) )  {
    // Timed out, Assume local host.
    debug_out ( "After 30 seconds : did not receive ServerVersionAndHost response\n" );
    m_bRemoteServer = false;
    m_waiter.wakeAll ( );
  }   //oooo

  QString qsFileName = pSlideshow->slideshow_name;
  qsFileName.replace ( "/", "_" );
  if ( m_pButtonTransition ) {
    qsFileName = theUtils.getTempFile ( "" ) + m_pButtonTransition->qsName + "/" + qsFileName + ".xml";
  } else {
    qsFileName = theUtils.getTempFile ( qsFileName + ".xml" );    //ooo
    //qsFileName = "\"" + qsFileName + "\"";  //xxx
  }

  pSlideshow->writeXml ( qsFileName );
  m_qsTempXMLFile = qsFileName;

  if ( ! m_pSocket )  {
    setWaiting ( false );
    return;
  }   //ooo
  
  //QDataStream stream   (  m_pSocket );    //ooo
  //QString Data; //xxx
  QByteArray Data; //xxx
  //std::string Data;  //xxx
  
  if ( m_bRemoteServer )  {
    sendFile ( qsFileName );

    // At this point we'll wait for the server to say something about the status.
    // There are two conditions to abort at this point
    // Slideshow already exists ( Server will now send the slideshow file )
    // Another process is already creating the slideshow.
    iCounter = 0;
    while ( iCounter < 50 )  {
      //if  ( m_waiter.wait ( 100 ) )			//ooo
      if  ( m_waiter.wait ( &m_waiterMutex, 100 ) )	//xxx
            iCounter = 50;
    }

    if ( m_bSendImageFiles )  {
      uint t, iCount = pSlideshow->countImg ( );
      CXmlSlideshow::img_struct *pXmlImg = NULL;
      for ( t=0; t<iCount; t++ )  {
        pXmlImg = pSlideshow->getImg ( t );
        if ( ! pXmlImg )
          continue;
        sendFile ( pXmlImg->src );
      }
      for ( t=0; t<(uint)pSlideshow->audio_list.count ( ); t++ )
        sendFile ( pSlideshow->audio_list[t] );
    }
  }
  else  {
    QString qsTempPath = theUtils.getTempFile ( QString ( ) );
    if ( m_pButtonTransition )
      qsTempPath += m_pButtonTransition->qsName + "/";
    //stream.startTransaction();    //xxx
    //stream << (Q_UINT16)CLIENT_TAKE_THAT_XML << (Q_UINT64)( qsFileName.length ( ) + qsTempPath.length ( ) );	//ooo
    //stream << (quint16)CLIENT_TAKE_THAT_XML << (quint64)( qsFileName.length ( ) + qsTempPath.length ( ) );	//xxx
    //stream << (quint16)CLIENT_TAKE_THAT_XML;    //xxx
    //m_pSocket->write  ( Message<quint16>::create ( CLIENT_TAKE_THAT_XML ) );    //xxx
    //stream << (quint64)( qsFileName.length ( ) + qsTempPath.length ( ) );	//xxx
    //stream << static_cast<quint16>(CLIENT_TAKE_THAT_XML) << static_cast<quint64>( qsFileName.length ( ) + qsTempPath.length ( ) );	//xxx
    //Data = static_cast<quint16>(CLIENT_TAKE_THAT_XML) + static_cast<quint64>( qsFileName.length ( ) + qsTempPath.length ( ) );	//xxx
    //Data.append ( static_cast<quint16>(CLIENT_TAKE_THAT_XML) + static_cast<quint64>( qsFileName.length ( ) + qsTempPath.length ( ) ) );	//xxx
    //Data = Message<quint16>::create ( CLIENT_TAKE_THAT_XML )   +   //xxx
    //       Message<quint64>::create ( static_cast<quint64>( qsFileName.length ( ) + qsTempPath.length ( ) ) );   //xxx
           //Message<quint64>::create ( static_cast<quint64>( qsFileName + qsTempPath ) );  //xxx
    //char cMsgID  [ sizeof ( quint16 ) ];	//xxx
    //(quint16 &)cMsgID = CLIENT_TAKE_THAT_XML;   //xxx
    //char temp[16];   //xxx
    //sprintf(temp, "0x%04X", CLIENT_TAKE_THAT_XML);  //xxx
    //QString str = "0xF00D"; //xxx
    //QString str; //xxx
    //QString str = QString(static_cast<char *>(temp)); //xxx
    //str.append(CLIENT_TAKE_THAT_XML); //xxx
    //str.insert(0, CLIENT_TAKE_THAT_XML); //xxx
    //str.replace(0, 2, "0x");    //xxx
    //QString value =  str.mid(2);    //xxx
    //QByteArray array2 = QByteArray::fromHex(value.toLatin1());  //xxx
    //char temp2[16];   //xxx
    //sprintf(temp2, "%04x", CLIENT_TAKE_THAT_XML);  //xxx
    //QByteArray array3(temp2);   //xxx
    //QByteArray macAddress = QByteArray::fromHex(temp2);
    //QByteArray ba;   //xxx
    //ba.resize(2);   //xxx
    //ba[0] = 0xf0;   //xxx
    //ba[1] = 0x0d;   //xxx
    //*cMsgID  = static_cast<quint16>(CLIENT_TAKE_THAT_XML); //xxx
    //m_pSocket->write  ( Message<quint16>::create ( CLIENT_TAKE_THAT_XML ) );    //xxx
    //m_pSocket->write  ( cMsgID,  sizeof ( quint16 ) );    //xxx
    //m_pSocket->write  ( cMsgID );    //xxx
    //Data.append ( CLIENT_TAKE_THAT_XML );  //xxx
    Data = byteArrayfromMessageID(CLIENT_TAKE_THAT_XML) + Message<quint64>::create ( qsFileName.length ( ) + qsTempPath.length ( ) );  //xxx
    //m_pSocket->write ( Data );    //xxx
    //m_pSocket->write ( Message<quint16>::create ( CLIENT_TAKE_THAT_XML ) );    //xxx
    //m_pSocket->write ( array2 );    //xxx
    //m_pSocket->write ( macAddress );    //xxx
    //m_pSocket->write ( byteArrayfromMessageID(CLIENT_TAKE_THAT_XML) );    //xxx
    //m_pSocket->write ( QByteArray ( cMsgID, sizeof ( quint16 ) ) );    //xxx
    //m_pSocket->write ( ba );    //xxx
    //m_pSocket->write ( QByteArray(static_cast<char *>(temp)) );    //xxx
    //m_pSocket->write ( QByteArray(static_cast<char *>(temp), sizeof ( quint16 )) );    //xxx
    //m_pSocket->write ( Message<quint64>::create ( qsFileName.length ( ) + qsTempPath.length ( ) ) );  //xxx
    emit signalWrite(Data); //xxx
    //stream << qsFileName << qsTempPath;   //ooo
    //Data.append(qsFileName + qsTempPath);  //xxx
    //emit signalWrite(qsFileName + qsTempPath); //xxx
    //Data.append(qsTempPath);  //xxx
    emit signalWrite(qsFileName); //xxx
    emit signalWrite(qsTempPath); //xxx
    //emit signalWrite(Data); //xxx
    if ( m_pSocket ) {
         //m_pSocket->flush ( );    //ooo
         emit signalFlush(); //xxx
    }
    usleep ( 1000000 );
  }
  
  //stream << (Q_UINT16)CLIENT_GO_TO_WORK << (Q_UINT64)0LL;	//ooo
  //stream << (quint16)CLIENT_GO_TO_WORK << (quint64)0LL;		//xxx
  //stream << static_cast<quint16>(CLIENT_GO_TO_WORK) << static_cast<quint64>(0LL);		//xxx
  //Data = static_cast<quint16>(CLIENT_GO_TO_WORK) + static_cast<quint64>(0LL);		//xxx
  Data = byteArrayfromMessageID ( CLIENT_GO_TO_WORK ) + Message<quint64>::create (0LL);		//xxx
  emit signalWrite(Data);   //xxx

  debug_out ( "Get the server to work ... msgID<0x%04X>\n",  CLIENT_GO_TO_WORK );
  if ( m_pSocket )
       //m_pSocket->flush ( );  //ooo
       emit signalFlush();  //xxx

  setWaiting ( false );
}

void Client::runEntry ( )
{   
  if ( ! m_pSourceFileEntry || ( m_pSourceFileEntry->listFileInfos.count ( ) < 1 ) )
    return;

  CXmlSlideshow *pSlideshow = m_pSourceFileEntry->listFileInfos[0]->pSlideshow;

  runSlideshow ( pSlideshow );
}

void Client::runTrans ( )
{  
  if ( ! m_pButtonTransition )
    return;

  CXmlSlideshow *pSlideshow = m_pButtonTransition->createSlideshow ( );
  runSlideshow ( pSlideshow );

  // Finally we can destroy the temp slideshow object. Thanks for existing.
  delete pSlideshow;
}

void Client::lock ( )
{
  m_mutex.lock ( );
}

void Client::unlock ( )
{
  m_mutex.unlock ( );
}

SourceFileEntry *Client::sourceFileEntry ( )
{
  return m_pSourceFileEntry;
}

ButtonTransition *Client::buttonTransition ( )
{
  return m_pButtonTransition;
}

float Client::progress ( )
{
  float fReturn;
  lock ( );
  fReturn = m_fProgress;
  unlock ( );
  return fReturn;
}

bool Client::waitingInQueue ( )
{
  return m_bWaitInQueue;
}

void Client::setWaiting ( bool bWaitInQueue )
{
  m_bWaitInQueue = bWaitInQueue;
}

void Client::displayProgress ( )
{  
  //int hint;	//xxx
  // QDVDAuthor::customEvent will call progress() to determinte the current progress.
  QCustomEvent *pNewEvent = new QCustomEvent ( EVENT_RENDER_PROGRESS );	//ooo

  //pNewEvent->setData      ( (void *) this );		//ooo
  QApplication::postEvent ( Global::pApp, pNewEvent );    //ooo
}

//Q_UINT64 Client::fileFromSocket ( )	//ooo
quint64 Client::fileFromSocket ( )	//xxx
{
  //int hint;	//xxx
  //Q_ULONG iBytesAvailable = 0LL;	//ooo
  qint64 iBytesAvailable = 0LL;		//xxx
  if ( ! m_pFile )  {
    Utils theUtils;
    QString qsFileName;

    //char cMsgLen [ sizeof ( Q_UINT64 ) ];	//ooo
    char cMsgLen [ sizeof ( quint64 ) ];	//xxx
    //Q_LONG iRead2 = m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );
    //m_pSocket->readBlock ( cMsgLen, sizeof ( Q_UINT64 ) );	//ooo
    //m_pSocket->read ( cMsgLen, sizeof ( qint64 ) );		//xxx
    m_pSocket->read ( cMsgLen, sizeof ( quint64 ) );		//xxx
    //m_iMessageLen = *(Q_UINT64 *)cMsgLen;	//ooo
    m_iMessageLen = *(quint64 *)cMsgLen;	//xxx

    QDataStream ds ( m_pSocket );
    ds >> qsFileName;

    //debug_out ( "<%s>\n", (const char *)qsFileName.ascii ( ) );		//ooo
    debug_out ( "<%s>\n", (const char *)qsFileName.toLatin1 ( ).data ( ) );	//xxx
    // Store under e.g. /tmp/qrender/MySlide/MySlide.vob
    QFileInfo fileInfo ( qsFileName );
    QString qsExt, qsNewFileName;

    qsNewFileName = Global::qsTempPath + "/" + Global::qsProjectName + "/" + fileInfo.fileName ( );

    fileInfo.setFile     ( qsNewFileName );
    if ( fileInfo.exists ( ) && fileInfo.size ( ) > 100 )  {
      // At this point we detected a file with the same ame exists already.
      // So we should ask for permission to delete it or move it out of the way.
      if ( MessageBox::warning ( NULL, tr ( "Slideshow Exists Already" ), tr ( "The slideshow <%1> exists already.\nDo you want to Delete the current file ( Yes ) or do you want to move it out of the way ( no ) ?" ).arg ( qsNewFileName ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )  {
        QFile temp ( qsNewFileName );
        if ( ! temp.remove ( ) )  {
          QString qsOrigFileName = qsNewFileName;
          qsNewFileName = theUtils.getUniqueTempFile ( qsNewFileName );
          MessageBox::warning ( NULL, tr ( "Could not delete file" ), tr ( "I could not delete the file <%1>.\nI will save the incomming slideshow under :\n%2" ).arg ( qsOrigFileName ).arg ( qsNewFileName ), QMessageBox::Ok, QMessageBox::NoButton );
        }
      }
    }
    //enableTimer ( false );    //ooo
    emit signalEnableTimer ( false );   //xxx
    m_pFile = new QFile ( qsNewFileName );
    m_pFile->open ( QIODevice::WriteOnly );
  }
  else  {
    iBytesAvailable = m_pSocket->bytesAvailable ( );
    //QByteArray data ( iBytesAvailable );	//ooo
    QByteArray data ( "", iBytesAvailable );	//xxx
    //Q_ULONG iActualRead = m_pSocket->readBlock ( data.data ( ), iBytesAvailable );	//ooo
    qint64 iActualRead = m_pSocket->read ( data.data ( ), iBytesAvailable );		//xxx
//printf ( "writeBlock <%s> bytesAvail<%ld> readIn<%ld> fSize<%ld> vs msgLen<%ld>\n", m_pFile->name ( ).ascii ( ), iBytesAvailable, iActualRead, m_pFile->size ( ), m_iMessageLen );
    //m_pFile->writeBlock ( data.data ( ), iActualRead );	//ooo
    m_pFile->write ( data.data ( ), iActualRead );		//xxx

    // Note: to be removed later ???
    m_pFile->flush ( );
  }

  // Let the Server know that I am ready for the next image.
  //QDataStream stream ( m_pSocket );   //ooo
  QString Data; //xxx
  //stream << (Q_UINT16)CLIENT_GOT_A_PIECE << (Q_UINT64)m_pFile->size ( );	//ooo
  //stream << (quint16)CLIENT_GOT_A_PIECE << (quint64)m_pFile->size ( );		//xxx
  Data = byteArrayfromMessageID ( CLIENT_GOT_A_PIECE ) + Message<quint64>::create ( m_pFile->size ( ) );		//xxx
  emit signalWrite(Data);   //xxx
  

  m_fProgress = 0.0;
  if ( m_iMessageLen > 0 )
       m_fProgress = 100.0 * ( 1.0 - (double)m_pFile->size ( ) / m_iMessageLen );
  displayProgress ( );

//printf ( "Render::fileFromSocket <%s> size<%lld> msgLen<%lld>\n", (const char *)m_pFile->fileName ( ).toUtf8 ( ), m_pFile->size  ( ), m_iMessageLen );
  //if ( m_pFile->size  ( ) >= (Q_ULONG)m_iMessageLen )  {	//ooo
  if ( m_pFile->size  ( ) >= (qint64)m_iMessageLen )  {		//xxx
       m_pFile->flush ( );
       m_pFile->close ( );

       delete m_pFile;
       m_pFile = NULL;
       m_iMessageID  = 0;
       m_iMessageLen = 0LL;

       QCustomEvent *pNewEvent = new QCustomEvent ( EVENT_RENDER_EXIT );	//ooo
       //pNewEvent->setData      ( (void *) this );	//ooo
       QApplication::postEvent ( Global::pApp, pNewEvent );
       //enableTimer ( false ); //ooo
       emit signalEnableTimer ( false );    //xxx
  }
  //m_pSocket->flush ( );   //ooo
  emit signalFlush();   //xxx

  return iBytesAvailable;
}

void Client::slotWrite(QByteArray Data) //xxx
{ 
  // Must always be called on thread 1
  m_pSocket->write(Data);
  /*QDataStream stream ( m_pSocket );
  stream << Data;*/
}

void Client::slotWrite(QString Data) //xxx
{
  // Must always be called on thread 1
  QDataStream stream ( m_pSocket );
  stream << Data;
  //m_pSocket->flush ( );
}

/*void Client::slotWrite(std::string Data) //xxx
{
  // Must always be called on thread 1
  QDataStream stream ( m_pSocket );
  stream << Data;
  //m_pSocket->flush ( );
}*/

void Client::slotFlush( )   //xxx
{
  m_pSocket->flush ( );
}

QByteArray Client::byteArrayfromMessageID ( quint16 iMessageID )  //xxx
{  
  char temp[16];   //xxx
  sprintf(temp, "%04x", iMessageID);
  QByteArray ba_messageID = QByteArray::fromHex(temp);

  return ba_messageID;
}

}; // end of namespace Render
