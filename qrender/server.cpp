/***************************************************************************
    server.cpp
                             -------------------
    Class QRender::Server
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include "server.h"
#include "render.h"

#include <stdlib.h>

#include <QApplication>
#include <qdir.h>
#include <QMessageBox>

#include "utils.h"

namespace Render
{

//Server::Server ( QObject *pParent )   //ooo
Server::Server ( QObject *parent )  //xxx
    //: QTcpServer ( pParent )  //ooo
    : QTcpServer ( parent ) //xxx
{
  QString qsTempPath, qsLogFile;
  qsTempPath  = QDir::tempPath ( ) + "/qrender/";

  Utils theUtils;
  theUtils.recMkdir ( qsTempPath );

  qsLogFile    = qsTempPath + "log.txt";
  m_logger.setLogInfo ( 3, qsLogFile    );
  m_logger.setLogObjectInfo ( true );
}

//void Server::incomingConnection ( int iSocketDescriptor ) //ooo
void Server::incomingConnection ( qintptr socketDescriptor )   //xxx
{ 
  //Render *pRender  =  new Render ( iSocketDescriptor, this      );    //ooo
  Render *pRender  =  new Render ( socketDescriptor, this      );  //xxx --> render.cpp
  pRender->logger ( ).setLogInfo ( m_iLogLevel, m_qsLogFileName );

  connect ( pRender, SIGNAL ( signalIAmDone ( ::Render::Render * ) ), this, SLOT ( slotRenderExited ( ::Render::Render * ) ) );
  //connect ( pRender, SIGNAL(finished()), pRender, SLOT(deleteLater())); //xxx
  pRender->start ( QThread::LowestPriority );
  //m_logger.iLOG ( "Server::incomingConnection<%d> start<%p>\n", iSocketDescriptor, pRender ); //ooo
  m_logger.iLOG ( "Server::incomingConnection<%d> start<%p>\n", socketDescriptor, pRender );    //xxx
  m_listOfRenderTasks.append ( pRender );
} //ooo

void Server::setLogInfo ( int iLogLevel, QString qsLogFileName )
{
  m_iLogLevel     = iLogLevel;
  m_qsLogFileName = qsLogFileName;
}

void Server::slotRenderExited ( ::Render::Render *pRender )
{
  m_logger.iLOG ( "Server::slotRenderExited \n" );
  m_listOfRenderTasks.removeAll ( pRender );
  // If Server and Client are on different machines, then keep it running. 
  // Else terminating it will not cause any harm as QDVDAuthor will be able 
  // to re-start QRender when needed.
  if ( ( pRender->isLocalServer ( ) ) && ( m_listOfRenderTasks.count ( ) == 0 ) )
    qApp->exit ( );
}

Render *Server::findSlideshowThread ( quint64 iHash, Render *pNotMe )
{
  Render *pRender = NULL;
  QList<Render *>::iterator it = m_listOfRenderTasks.begin ( );
  while ( it != m_listOfRenderTasks.end ( ) )  {
    pRender = *it++;
    if ( pNotMe && pNotMe == pRender )
      continue;
    if ( pRender->hash ( ) == iHash )
      return pRender;
  }
  return NULL;
}

}; // end of namespace Render

