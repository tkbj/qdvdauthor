/***************************************************************************
    render.h
                             -------------------
    Client - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

#include <QMutex>
#include <QThread>
#include <QTcpSocket>
#include <QEventLoop>
#include <QWaitCondition>
//Added by qt3to4:
#include <QTimerEvent>

#include "logger.h"

class QFile;
class CXmlSlideshow;

namespace Render
{
// The block size we shuffle over the socket at once.
#define BLOCK_LEN 1024
// Throttling transfere over socket for better debugging (delay in usec)
#define THROTTLE 0

class Render : public QThread
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
           //Render ( int, QObject * );     //ooo
           //Render ( int iSocketDescriptor, QObject *parent );     //xxx
           //Render ( qintptr, QObject * );   //xxx
           Render ( qintptr iSocketDescriptor, QObject *parent );     //xxx
           //Render ( qint64 iSocketDescriptor, QObject *parent );     //xxx
  virtual ~Render ( );

  void run    (     );    //ooo
  //void run    (     ) Q_DECL_OVERRIDE;  //xxx
  void myExit ( int );
  bool killClient ( );

  QString getHashName ( QString & );
  void sendProgress   (   float   );
  bool isLocalServer  ( );
  quint64 hash        ( );
  Logger &logger      ( );
  Logger &status      ( );

protected:
  virtual void timerEvent  ( QTimerEvent * );
  quint64 fileFromSocket   ( );
  void    createSlideshow  ( );
  bool    loadSlideshowXML ( QString );
  quint64 createXMLHash    ( QString );
  unsigned long long SDBMHash ( QString & );

  void sendMessage           ( quint16, QString );
  bool sendFile              ( QString );
  void sendServerState       ( bool );
  void sendNextPacket        ( );
  void sendVersionAndHost    ( );
  void receivedXMLName       ( );
  bool receivedSearchWarrant ( );
  void receivedReconnect     ( );
  QTcpSocket *switchSockets  ( QTcpSocket * );

protected slots:
  virtual void slotReadSocket  (       );
  virtual void slotExiting     (       );
  void slotWrite(QByteArray Data);  //xxx
  void slotWrite(QString Data);  //xxx
  void slotMyExit(int iExitCode);   //xxx
  void slotFlush( );    //xxx

signals:
  void error ( QTcpSocket::SocketError );
  void signalIAmDone ( ::Render::Render * );
  void Write(QByteArray Data);  //xxx
  void Write(QString Data);  //xxx
  void MyExit(int Data);    //xxx
  void flush( );   //xxx

private:
  CXmlSlideshow *m_pSlideshow;
  bool m_bKillClient;
  // this var is used to determine if the XML file has changed
  quint64 m_iXMLHash; // between a previous run and now.
  Logger  m_logger;
  Logger  m_status;
  QString m_qsTempPath;
  // Receiving from Client :
  quint16 m_iMessageID;
  quint64 m_iMessageLen;
  QFile  *m_pFile;

  bool m_bLocalClientAndServer;
  QString m_qsClientTempPath;
  QWaitCondition m_waiter;
  QMutex         m_mutex;

  //int m_iSocketDescriptor;    //ooo
  qintptr m_iSocketDescriptor;  //xxx
  int m_iTimerID;
  QTcpSocket *m_pTcpSocket;
};

}; // end of namespace Render

#endif
