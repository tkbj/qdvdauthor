/***************************************************************************
    render.cpp
                             -------------------
    Class QRender::Render
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This class will be created by Render::Server, when a new 
    socket connection is established

    1) The first thing to happen is the exchange of the Host and Version
    2) Next is the recetion of the Slideshow XML file
    3) Followed by a check if the slideshow exists and is up-to-date
    4) If not, then the client will send the image files
    5) Then we render the Slideshow and
    6) send the generated VOB file back to the client.
    
****************************************************************************/

#include <unistd.h>

#include <QDir>
#include <QFile>
#include <QImage>
#include <QFileInfo>
#include <QByteArray>
#include <QTimerEvent>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>   //xxx
#include <QMutexLocker> //xxx

#include "utils.h"
#include "shared.h"
#include "render.h"
#include "server.h"
#include "slideshow.h"

// Note: Maybe later on I can import dvd-slideshow config files
#include "../qdvdauthor/xml_slideshow.h"

extern Render::Server *g_pServer;

namespace Render
{

//Render::Render ( int iSocketDescriptor, QObject *pParent )    //ooo
Render::Render ( qintptr iSocketDescriptor, QObject *pParent )  //xxx
    //Render::Render ( qint64 iSocketDescriptor, QObject *pParent )  //xxx
     : QThread ( pParent ), m_iSocketDescriptor ( iSocketDescriptor )
     , m_pTcpSocket(new QTcpSocket)   //xxx
{  
  m_iMessageLen =    0;
  m_iMessageID  =    0;
  m_iTimerID    =   -1;
  m_iXMLHash    =  0LL;
  m_pFile       = NULL;
  m_pSlideshow  = NULL;
  m_bKillClient = false;
  m_qsTempPath  = QDir::tempPath ( ) + "/qrender/";
  m_bLocalClientAndServer = false;

  Utils theUtils;
  theUtils.recMkdir ( m_qsTempPath );

  QString qsLogFile, qsStatusFile;
  qsLogFile    = m_qsTempPath + "log.txt";
  qsStatusFile = m_qsTempPath + "status.txt";
  m_status.setLogInfo ( 0, qsStatusFile );
  m_logger.setLogInfo ( 3, qsLogFile    );
  m_logger.setLogObjectInfo ( true );
//printf ( "QRender::QRender <%p> thread<%X>\n", this, pthread_self ( ) );
  //m_pTcpSocket = new QTcpSocket;  //ooo

  if ( ! m_pTcpSocket->setSocketDescriptor ( m_iSocketDescriptor ) )  {
      emit ( error ( m_pTcpSocket->error ( ) ) );
      return;
  }

  connect ( m_pTcpSocket, SIGNAL ( readyRead ( ) ), this, SLOT ( slotReadSocket ( ) ) );  //ooo
  //connect ( m_pTcpSocket, SIGNAL ( readyRead ( ) ), this, SLOT ( slotReadSocket ( ) ), Qt::DirectConnection );  //xxx
  connect ( this,         SIGNAL ( finished  ( ) ), this, SLOT ( slotExiting    ( ) ) );
  //connect ( this,         SIGNAL ( Write(QByteArray) ), m_pTcpSocket, SLOT ( slotWrite(QByteArray Data) ) );    //xxx
  connect ( this,         SIGNAL ( Write(QByteArray) ), this, SLOT ( slotWrite(QByteArray) ) );    //xxx
  //connect ( this,         SIGNAL ( Write(QString) ), m_pTcpSocket, SLOT ( slotWrite(QString Data) ) );    //xxx
  connect ( this,         SIGNAL ( Write(QString) ), this, SLOT ( slotWrite(QString) ) );    //xxx
  //connect ( this,         SIGNAL ( MyExit(int) ), m_pTcpSocket, SLOT ( slotMyExit(int Data) ) );    //xxx
  connect ( this,         SIGNAL ( MyExit(int) ), this, SLOT ( slotMyExit(int) ) );    //xxx
  connect ( this,         SIGNAL ( flush( ) ), this, SLOT ( slotFlush( ) ) );    //xxx
}

Render::~Render ( )
{
  if ( m_pSlideshow )
    delete m_pSlideshow;
  if ( m_pTcpSocket )
    delete m_pTcpSocket;
  m_pTcpSocket = NULL;
  logger ( ).iLOG ( "Render::~Render <%p>\n", this );
}

QTcpSocket *Render::switchSockets ( QTcpSocket *pTakeMe )
{
  if ( pTakeMe && m_pTcpSocket )  {
    disconnect ( m_pTcpSocket, SIGNAL ( readyRead ( ) ), this, SLOT ( slotReadSocket ( ) ) );
    QTcpSocket *pOldSocket = m_pTcpSocket;
    m_pTcpSocket = pTakeMe;
    connect ( m_pTcpSocket, SIGNAL ( readyRead ( ) ), this, SLOT ( slotReadSocket ( ) ) );
    return pOldSocket;
  }
  return pTakeMe;
}

void Render::run ( )
{   
  // Kick off the pinger ...
  m_iTimerID = startTimer ( I_AM_ALIVE_INTERVAL );;

  logger ( ).iLOG ( "%s Build date : %s\n", QRENDER_VERSION, __DATE__ );
  status ( ).log  ( "START\n" );

  // Enter the TcpServers Event Loop to listen to events from the socket.
  int iExec = -1;
  while ( iExec != CLIENT_KILL_YOURSELF && ! m_bKillClient )  {
    iExec = exec ( );
    switch ( iExec )  {
      case CLIENT_GO_TO_WORK:
        createSlideshow ( );
      break;
      default:
      break;
    }
  }
  myExit ( 0 ); //iExec );
  killTimer ( m_iTimerID );
}

void Render::slotReadSocket ( )
{   
  qint64 iBytesAvail = m_pTcpSocket->bytesAvailable ( );
  
//printf ( "iBytesAvail<0x%016llX>\n", iBytesAvail );
  if  (  iBytesAvail <= 0 )
    return;

  while  ( true )  {
    if   ( m_iMessageLen == 0 )  {
      // Excuse me, come back when you're grown up.
      if ( iBytesAvail   < 10 )
        return;
      // Okay we have yet to see what message we just received
      QDataStream ds ( m_pTcpSocket );
      ds >> m_iMessageID;
      ds >> m_iMessageLen;
      iBytesAvail -= 10;

      logger ( ).dLOG ( "NewMessage<0x%04X> Len<%lld> bytesAvail<%lld>\n", m_iMessageID, m_iMessageLen, iBytesAvail );
    }

    // At this point we have one Message boy in the socket buffer
    switch ( m_iMessageID )      {
    case CLIENT_WHO_AND_WHERE_ARE_YOU:
      sendVersionAndHost ( );
    break;
    case CLIENT_TAKE_A_FILE:  {
      if   ( m_iMessageLen > 0 ) {
        if ( fileFromSocket(   ) == 0 )
          return; // all avail bytes were read. Okay
      }
    }
    break;
    case CLIENT_TAKE_THAT_XML:
      receivedXMLName ( );
    break;
    case CLIENT_GO_TO_WORK:
      exit ( CLIENT_GO_TO_WORK );  // will leave exec and continue in background thread ( in function run )
//      createSlideshow ( );
    break;
    case CLIENT_I_LOST_CONTACT:
      // A new connection was established and this Render-object may not be the 
      // original one, which is still rendering the slideshow in the background.
      // Need to re-connect the two, in order to continue with the progress display
      // in QDVDAuthor.
      if ( receivedSearchWarrant ( ) )
        exit ( );
    break;
    case CLIENT_GOT_A_PIECE:
      sendNextPacket ( );
    break;
    case CLIENT_RECONNECTING:
      // This render object was created after a socket error
      receivedReconnect ( );
    break;
    case CLIENT_KILL_YOURSELF:
      m_bKillClient = true;  // slideshow will eventually terminate
//      exit ( CLIENT_KILL_YOURSELF );
    break;
    default:
      // Error, unknown messageID
      logger ( ).eLOG ( "NewMessage<0x%04X> Len<%lld> bytesAvail<%lld>\n", m_iMessageID, m_iMessageLen, iBytesAvail );
    break; // don't do nothin'
    }      // end switch statement
    m_iMessageID  = 0;
    m_iMessageLen = 0;
    iBytesAvail = m_pTcpSocket->bytesAvailable ( );
  }
}

void Render::slotMyExit ( int iExitCode )   //xxx
{  
  m_pTcpSocket->flush   ( );
  m_pTcpSocket->disconnectFromHost ( );
  qApp->processEvents ( );

  if ( m_pTcpSocket->state ( ) != QAbstractSocket::UnconnectedState )
       m_pTcpSocket->waitForDisconnected ( 5000 );  // wait for up to 5 seconds to be disconnected
     
  qApp->processEvents ( );
}

void Render::myExit ( int iExitCode )
{
  /*m_pTcpSocket->flush   ( );
  m_pTcpSocket->disconnectFromHost ( );
  qApp->processEvents ( );

  if ( m_pTcpSocket->state ( ) != QAbstractSocket::UnconnectedState )
       m_pTcpSocket->waitForDisconnected ( 5000 );  // wait for up to 5 seconds to be disconnected
  qApp->processEvents ( );*/    //ooo
  
  emit MyExit(iExitCode);   //xxx

  QThread::exit ( iExitCode );
  logger ( ).iLOG ( "Server exited \n" );
  status ( ).log  ( "EXIT %d\n", iExitCode );
}

bool Render::killClient ( )
{
  return m_bKillClient;
}

void Render::sendProgress ( float fProgress )
{   
  logger ( ).dLOG ( "Progress <%f>\n", fProgress );
  status ( ).log  ( "PROGRESS:%f\n", fProgress );
  if ( m_pTcpSocket->state ( ) != QAbstractSocket::ConnectedState )
    return;

  QByteArray block;
  // build the progress message
  block = Message<quint16>::create ( SERVER_YOUR_PROGRESS_SIR )   +
          //Message<quint64>::create ( (quint64)sizeof ( double ) ) +   //ooo
          Message<quint64>::create ( static_cast<quint64>(sizeof ( double ) ) ) +   //xxx
          //Message<double>::create  ( (double)fProgress ); //ooo
          Message<double>::create  ( static_cast<double>(fProgress) );  //xxx

  // Send progress message
  //m_pTcpSocket->write ( block );  //ooo
  emit Write(block); //xxx
  //m_pTcpSocket->flush ( );    //ooo
  emit flush( );    //xxx
}

bool Render::isLocalServer ( )
{
  return m_bLocalClientAndServer;
}

Logger &Render::logger ( )
{
  return m_logger;
}

Logger &Render::status ( )
{
  return m_status;
}

void Render::slotWrite(QByteArray Data) //xxx
{
  // Must always be called on thread 1
  m_pTcpSocket->write(Data);
}

void Render::slotWrite(QString Data) //xxx
{
  // Must always be called on thread 1
  QDataStream stream ( m_pTcpSocket );
  stream << Data;
  //m_pTcpSocket->flush ( );
}

void Render::slotFlush( )   //xxx
{
  m_pTcpSocket->flush ( );
}

void Render::sendMessage ( quint16 iMessageID, QString qsMessage )
{
  if ( m_pTcpSocket->state ( ) != QAbstractSocket::ConnectedState )
    return;

  //m_pTcpSocket->write  ( Message<quint16>::create ( iMessageID ) );   //ooo
  
  // Fill with data
  QByteArray Data;  //xxx
  Data = Message<quint16>::create ( iMessageID );   //xxx
  
  // An event gets put onto thread 1's event queue after this
  emit Write(Data); //xxx
  //m_pTcpSocket->write( Data );  //xxx
  
  //m_pTcpSocket->write  ( Message<quint64>::create ( (quint64)qsMessage.length ( ) ) );    //ooo
  
  Data = Message<quint64>::create ( (quint64)qsMessage.length ( ) );    //xxx
  
  emit Write(Data); //xxx

/*
quint64 iLen = (quint64)qsMessage.length ( );
char *cLen = (char *)&iLen;
printf ( "%s::%d > len<%llu> [%02x %02x %02x %02x]\n", __FILE__, __LINE__, iLen, cLen[0], cLen[1], cLen[2], cLen[3] );
*/

  //QDataStream stream ( m_pTcpSocket );    //ooo
  //stream << qsMessage;    //ooo
  emit Write(qsMessage);   //xxx

  //m_pTcpSocket->flush ( );    //ooo
  emit flush(); //xxx
}

void Render::sendVersionAndHost ( )
{
  QString qsClientHost;
  char cHostName[1024];
  quint16 iMessageID = SERVER_ITS_ME_AND_I_AM_AT;
  QDataStream stream   ( m_pTcpSocket    );
  // At this point the socket shoould still have the hostname of the client intus.
  stream >> qsClientHost;

  // Okay lets compare if we are on the same host.
  gethostname ( (char *)&cHostName, 1024 );
  QString  qsHostName  ( cHostName       );
  QString  qsVersion   ( QRENDER_VERSION );
  if ( qsClientHost == qsHostName )
    m_bLocalClientAndServer = true;

  //logger ( ).iLOG ( " Version<%d> client<%s>\n", m_bLocalClientAndServer, (const char *)qsClientHost.toAscii ( ) );           //ooo
  logger ( ).iLOG ( " Version<%d> client<%s>\n", m_bLocalClientAndServer, (const char *)qsClientHost.toLatin1 ( ).data ( ) );   //xxx

#ifdef FAKE_REMOTE_HOST
  m_bLocalClientAndServer = false;
#endif

  // And finally notify the client about our location and age.
  stream << iMessageID << (quint64)( qsHostName.length ( ) + qsVersion.length ( ) );
  stream << qsHostName << qsVersion;
  m_pTcpSocket->flush ( );
}

void Render::sendServerState ( bool bSameSlideshow )
{
  QString qsState;

  // Okay lets compare if we are on the same host.
  if ( bSameSlideshow )
    qsState = "SameXML"; // Note, this string must be in synch with qdvdauthor/render_client.cpp::receivedServerState()

  logger ( ).iLOG ( " SameXML<%s>\n", bSameSlideshow ? "true" : "false" );
  status ( ).log  ( "SAME_FILE:%d",   bSameSlideshow );

  // And finally notify the client about our location and age.
  sendMessage ( SERVER_MY_STATUS_SIRE, qsState );

  m_pTcpSocket->flush  ( );
}

bool Render::sendFile ( QString qsFileName )
{
  if ( m_pFile )
    return false;

  if ( m_pTcpSocket->state ( ) != QAbstractSocket::ConnectedState )
    return false;

  m_pFile = new QFile ( qsFileName );
  if ( ! m_pFile->exists ( ) )  {
    delete m_pFile;
    m_pFile = NULL;
    return false;
  }
  m_pFile->open ( QIODevice::ReadOnly );
  killTimer ( m_iTimerID );

  //logger ( ).iLOG ( " <%s> msgID<0x%04X>\n", (const char *)qsFileName.toAscii ( ), (quint16)SERVER_TAKE_A_FILE );         //ooo
  logger ( ).iLOG ( " <%s> msgID<0x%04X>\n", (const char *)qsFileName.toLatin1 ( ).data ( ), (quint16)SERVER_TAKE_A_FILE ); //xxx
  status ( ).log  ( "SIZE:%lld\n", m_pFile->size ( ) );
  // This will kick off the sendPacket protocol until the whole file is transmittet.
  QByteArray cString = qsFileName.toUtf8 ( );
  m_pTcpSocket->write  ( Message<quint16>::create ( SERVER_TAKE_A_FILE ) );
  m_pTcpSocket->write  ( Message<quint64>::create ( (quint64)m_pFile->size ( ) ) );

  QDataStream stream ( m_pTcpSocket );
  stream << qsFileName;

  m_pTcpSocket->flush  ( );

  return true;
}

void Render::sendNextPacket ( )
{
  char data[BLOCK_LEN];
  if ( ! m_pFile )
    return;

  qint64 iLen  = m_pFile->read ( data, BLOCK_LEN );
  if (   iLen != -1 )  {
    m_pTcpSocket->write (  data, iLen );
    qint64 iPos   = m_pFile->pos  ( );
    qint64 iSize  = m_pFile->size ( );

    if ( iPos >= iSize  ) {
      m_pFile->close    ( );
      m_pTcpSocket->flush ( );
      //logger ( ).iLOG ( " Finished <%s> WAKE ALL\n", (const char *)m_pFile->fileName ( ).toAscii ( ) );           //ooo
      logger ( ).iLOG ( " Finished <%s> WAKE ALL\n", (const char *)m_pFile->fileName ( ).toLatin1 ( ).data ( ) );   //xxx
      status ( ).log  ( "VIDEO DONE\n" );
      delete m_pFile;
      m_pFile = NULL;
      usleep ( 100 );
      m_waiter.wakeAll ( );
      usleep ( 100 );
      // Is this required ?
      m_iTimerID = startTimer ( I_AM_ALIVE_INTERVAL );
      exit ( 0 ); // exit the EventLoop. This will cause the function run to exit
    }
//printf ( "Client::sendNextPacket thr<%X> <%f%%>\n", (unsigned)pthread_self ( ), fPercentage );
  }
}

void Render::slotExiting ( )
{
  logger ( ).dLOG ( "Exiting\n" );
  emit   ( signalIAmDone ( this ) );
  QThread::deleteLater   ( );
}

void Render::timerEvent ( QTimerEvent *pEvent )
{
  if ( pEvent->timerId ( ) == m_iTimerID )  {
    if ( m_pTcpSocket->state ( ) != QAbstractSocket::ConnectedState )
      return;

    logger ( ).iLOG ( " PING \n");
    // Every 20 seconds we'll send out a short ping.
    m_pTcpSocket->write  ( Message<quint16>::create ( SERVER_I_AM_ALIVE ) );
    m_pTcpSocket->flush  ( );
  }
}

quint64 Render::fileFromSocket ( )
{
  if ( ! m_pFile )  {
    Utils theUtils;
    QString qsFileName;
    QDataStream ds ( m_pTcpSocket );
    ds >> qsFileName;
    // printf ( "Render::fileFromSocket <%s>\n", (const char *)qsFileName.toAscii ( ) );
    // Store under e.g. /tmp/qrender/MySlide/MySlide.xml
    QFileInfo fileInfo ( qsFileName );
    QString qsExt, qsNewFileName;
    qsExt = fileInfo.suffix ( );

    if ( qsExt.toLower ( ) == "xml" )  {
      // The first file is the XmlSlideshow - file.
      m_qsTempPath  = QDir::tempPath ( ) + "/qrender/" + fileInfo.baseName ( ) + "/";
      theUtils.recMkdir ( m_qsTempPath );
      qsNewFileName = m_qsTempPath + fileInfo.fileName ( );
      m_iXMLHash    = createXMLHash ( qsNewFileName );
    }
    else
      qsNewFileName = getHashName ( qsFileName );

    m_pFile = new QFile  ( qsNewFileName );
    m_pFile->open ( QIODevice::WriteOnly );
    logger ( ).iLOG ( "Render::fileFromSocket new <%s> size<%lld> msgLen<%lld>\n", (const char *)m_pFile->fileName ( ).toUtf8 ( ), m_pFile->size  ( ), m_iMessageLen );
  }

  qint64 iBytesAvailable = m_pTcpSocket->bytesAvailable ( );
  quint64 iFileSize = (quint64)m_pFile->size ( );
  quint64 iDelta    = 0LL;
  if ( iFileSize + iBytesAvailable > m_iMessageLen )  {
    // Seems like the next message is entangled in this one ...
    iDelta = ( iFileSize + iBytesAvailable ) - m_iMessageLen;
    iBytesAvailable -= iDelta;
  }

  QByteArray data ( iBytesAvailable, 0 );
  qint64 iActualRead = m_pTcpSocket->read ( data.data ( ), iBytesAvailable );
  m_pFile->write  ( data.data ( ), iActualRead );

  // Let the Client know that I am ready for the next image.
  m_pTcpSocket->write ( Message<quint16>::create ( SERVER_GOT_A_PIECE ) ); 
  m_pTcpSocket->write ( Message<quint64>::create ( (quint64)m_pFile->size ( ) ) );

//printf ( "Render::fileFromSocket <%s> size<%lld> msgLen<%lld>\n", (const char *)m_pFile->fileName ( ).toUtf8 ( ), m_pFile->size  ( ), m_iMessageLen );
  if ( m_pFile->size  ( )  >=  (qint64)m_iMessageLen )  {
       m_pFile->flush ( );
       m_pFile->close ( );
       // the first file is the slideshow XML file
       if ( ! m_pSlideshow )  {
         bool bDifferentSlideshow = loadSlideshowXML ( m_pFile->fileName ( ) );
         status ( ).log ( "GOT XML\n" );
         sendServerState ( bDifferentSlideshow );
       }
       logger ( ).iLOG ( "Got all <%s> size<%lld> msgLen<%lld>\n", (const char *)m_pFile->fileName ( ).toUtf8 ( ), m_pFile->size  ( ), m_iMessageLen );

       delete m_pFile;
       m_pFile = NULL;
       m_iMessageID  = 0;
       m_iMessageLen = 0;
  }
  m_pTcpSocket->flush ( );
//printf ( "Render::fileFromSocket Avail<%lld> read<%lld> delta<%lld>\n", iBytesAvailable, iActualRead, iDelta );
  return ( iBytesAvailable - iActualRead + iDelta );
}

void Render::receivedReconnect ( )
{
  logger ( ).iLOG ( "receiveReconnect \n" );
  QString qsName;

//  qint64  iBytesAvail = m_tcpSocket.bytesAvailable ( );
  quint16 iLength;

  QDataStream ds ( m_pTcpSocket );
  ds >> iLength;
  ds >> qsName;

//printf ( "%s::%d > bytesAvail<%lld> len<%u> Reconnect Name<%s>\n", __FILE__, __LINE__, iBytesAvail, iLength, (const char *)qsName.toUtf8 ( ) );
  logger ( ).iLOG ( "Reconnect Name<%s>\n", (const char *)qsName.toUtf8 ( ) );
  status ( ).log  ( "RECONNECT\n" );
}

void Render::receivedXMLName ( )
{
  QString qsFileName, qsTempPath;
  QDataStream ds ( m_pTcpSocket );
  ds >> qsFileName >> qsTempPath;

  m_qsClientTempPath  = qsTempPath;
  m_pSlideshow = new CXmlSlideshow;
  m_pSlideshow->readXml ( qsFileName, false );
  logger ( ).iLOG ( " XML File <%s> tempPath<%s>\n", (const char *)qsFileName.toUtf8 ( ), (const char *)qsTempPath.toUtf8 ( ) );
  status ( ).log  ( "GOT XML\n" );
}

bool Render::receivedSearchWarrant ( )
{
  QString qsFileName, qsTempPath;
  QDataStream ds ( m_pTcpSocket );
  ds >> qsFileName;

  quint64 iHash  = createXMLHash ( qsFileName );
  Render *pOther = g_pServer->findSlideshowThread ( iHash, this );
  if ( pOther )  {
    disconnect ( m_pTcpSocket,   SIGNAL  (readyRead ( ) ), this, SLOT ( slotReadSocket ( ) ) );
    m_pTcpSocket = pOther->switchSockets ( m_pTcpSocket );
  }
  logger ( ).iLOG ( "Searching Server Soul for <%s> Server has %srunning thread for it\n", (const char *)qsFileName.toUtf8 ( ), pOther ? "a " : "no " );
  logger ( ).iLOG ( "Server <%016p> Render<%016p> thread<%lu>\n", g_pServer, this, pthread_self ( ) );
  status ( ).log  ( "Searching For running Slideshow\n" );
  return ( pOther != NULL );
}

QString Render::getHashName ( QString &qsFileName )
{
  // If server and client are on the same box, then we can simply use the original file
  if ( m_bLocalClientAndServer )  {
    QString qsFile = qsFileName;
    return  qsFile;
  }
  Utils theUtils;
  QString qsExt, qsHash, qsHashFileName;
  QFileInfo fileInfo      ( qsFileName );
  qsExt = fileInfo.suffix ( ).toLower( );

  unsigned long long iHashValue = theUtils.SDBMHash ( qsFileName );
  qsHash.sprintf ( "%016llX", iHashValue );
  qsHashFileName = m_qsTempPath + qsHash + "." + qsExt;
  fileInfo.setFile ( qsHashFileName );
  if ( ! fileInfo.exists ( ) )  {
    QString qsFile = qsFileName;
    return  qsFile;
  }
  return qsHashFileName;
}

quint64 Render::createXMLHash ( QString qsFileName )
{
  quint64 iXMLHash = 0LL;
  QFileInfo fileInfo ( qsFileName );
  if ( fileInfo.exists ( ) )   {
    // Okay, this kinda sucks but is a quick solution
    // Load the whole XML file into a QString and run 
    // the hash function over it
    // NOte: can be replaced later on with a more 
    // efficient way esp with large XML files
    Utils theUtils;
    QFile theFile ( qsFileName );
    theFile.open  ( QIODevice::ReadOnly );
#if (QT_VERSION > 0x0403FF)
    qint64 iSize = fileInfo.size ( );
    uchar *pFile = theFile.map   ( 0LL, iSize );
    if ( pFile )  {
      QString qsFileContents  ( (const char *)pFile );
      iXMLHash = theUtils.SDBMHash ( qsFileContents );
    }
#else
    QString qsContents  ( theFile.readAll ( ) );
    iXMLHash = theUtils.SDBMHash ( qsContents );
#endif
    theFile.close ( );
  }
  return iXMLHash;
}

quint64 Render::hash ( )
{
  return m_iXMLHash; 
}

bool Render::loadSlideshowXML ( QString qsFileName )
{
  bool bReturn = true;
  // The XML file has been transmitted.
  if   (  m_iXMLHash != 0LL )  {
    quint64 iXMLHash  = createXMLHash ( qsFileName );
    if (    iXMLHash == m_iXMLHash )  {
      // We can reasenably be sure the xml file has not changed
      // thus the current existing VOB file is up-to-date.
      //logger ( ).iLOG ( "File <%s> seems to exist already (Hash=<0x%016llX>)\n", (const char *)qsFileName.toAscii ( ), iXMLHash );            //ooo
      logger ( ).iLOG ( "File <%s> seems to exist already (Hash=<0x%016llX>)\n", (const char *)qsFileName.toLatin1 ( ).data ( ), iXMLHash );    //xxx
      bReturn = false;
    }
  }

  //logger ( ).iLOG ( "Create Slideshow object(Hash=<0x%016llX>)\n", (const char *)qsFileName.toAscii ( ), m_iXMLHash );            //ooo
  logger ( ).iLOG ( "Create Slideshow object(Hash=<0x%016llX>)\n", (const char *)qsFileName.toLatin1 ( ).data ( ), m_iXMLHash );    //xxx
  m_pSlideshow = new CXmlSlideshow;
  m_pSlideshow->readXml ( qsFileName, false );

  status ( ).log ( "LOAD XML\n" );
  return bReturn;
}

void Render::createSlideshow ( )
{  
  if ( ! m_pSlideshow )  {
    logger ( ).log ( "Error, No Slideshow object created.\n" );
    return;
  }
  QString qsTempPath = m_qsTempPath;
  if ( isLocalServer ( ) )  // If local server, then let us create the VOB in the right place.
    qsTempPath = m_qsClientTempPath;

  Utils theUtils;
  theUtils.recMkdir ( qsTempPath );

  status ( ).log  ( "START SLIDESHOW\n" );

  Slideshow slide ( this ); //ooo --> slideshow.cpp
  slide.initMe    ( m_pSlideshow, qsTempPath );
  if ( slide.exec ( ) )  {
    // Presumably success. Let us send the final product back to the client ( QDVDAuthor )
    QString qsFile = m_pSlideshow->slideshow_name;
    qsFile.replace ( "/", "_" );
    QString qsFileName = qsTempPath + qsFile + ".vob";
    QFileInfo fileInfo ( qsFileName );

    if  ( fileInfo.exists ( ) )  {
      if ( !isLocalServer ( ) )  // only send through the Socket if we are on a remote server.
        sendFile ( qsFileName );
      else  {
        sendMessage ( SERVER_STICK_A_FORK_IN_ME, tr ( "Done" ) );
        myExit ( 0 ); // exi the thread
      }
    }
    else  {
      sendMessage ( (quint16)SERVER_SHIT_HAPPENED, tr ( "Final slideshow file <%1> not found.\n" ).arg ( qsFileName ) );
      myExit ( -1 );
      //exit ( -1 ); // exit the EventLoop. This will cause the function run to exit
    }
  }
}

};  // End of namespace Render

