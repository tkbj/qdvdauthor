/***************************************************************************
    main.cpp
    Main file
                             -------------------
    ButtonObject class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This is the main file.
    
****************************************************************************/

#include <unistd.h>

#include <QApplication>
#include <QtCore>
#include <QtCore/QCoreApplication>  //xxx

#include "utils.h"
#include "shared.h"
#include "server.h"
#include "parser.h"
#include "logger.h"

Render::Server *g_pServer = NULL;

int main ( int argc, char *argv[] )
{
  qDebug() << Q_FUNC_INFO;    //ppp
    
  //QApplication app ( argc, argv );    //ooo
  QCoreApplication app ( argc, argv );  //xxx
  Render::Logger logger;
  // The parser class handles user input.
  // We can exit here if QRender is
  // not executed as deamon ( Client-Server )
  Render::Parser parser;
  if ( parser.parseCommandLine ( argc, argv ) )
    return 0;

  // First we check if the work can be done without starting a server
  // E.g. writing the regional code to the final DVD.
  if ( parser.getRegionalZone ( ) != -1 )  {
    Utils theUtils;
    theUtils.writeZoneCode ( (unsigned char)parser.getRegionalZone ( ), parser.getPath ( ) );
    return 0;
  }

  pid_t iPid = getpid ( );

  /*if ( ! parser.isTTY ( ) )
    daemon ( 1, 1 );*/  //ooo
  
  // At this point we start the server thread and listen to the set port.
  Render::Server server;  //ooo
  
  if ( ! parser.isTTY ( ) ) //xxx
    daemon ( 1, 1 );    //xxx
  
  server.setLogInfo ( parser.getLogLevel ( ), parser.getLogfileName ( ) );
  logger.setLogInfo ( parser.getLogLevel ( ), parser.getLogfileName ( ) );
  g_pServer = &server;
 
  //quint16 m_serverPort = 5066;  //xxx
  
  if ( ! server.listen ( QHostAddress::Any, parser.getServerPort ( ) ) )  {   //ooo
  //if ( ! server.listen ( QHostAddress::Any, static_cast<quint16> ( parser.getServerPort ( ) ) ) )  {    //xxx
  //if ( ! server.listen ( QHostAddress::Any, m_serverPort ) )  {    //xxx
    // QRender is most likely alreadyt running in the background. So this is not an error per-se
    // but rather an expected exception.
    //logger.log ( Render::Logger::logError, "ERROR: Can not bind to port 5066\n%s\n", (const char *)server.errorString ( ).toAscii ( ) );          //ooo
    logger.log ( Render::Logger::logError, "ERROR: Can not bind to port 5066\n%s\n", server.errorString ( ).toLatin1 ( ).data ( ) );  //xxx
  } else  {
    if ( ! parser.isTTY ( ) ) {
      //logger.log ( Render::Logger::logInformation, "\nStarting QRender[%d] <%016%p> in deamon mode, Listening to port : %d\n\n", (int)iPid+1, g_pServer, parser.getServerPort ( ) );    //ooo
      logger.log ( Render::Logger::logInformation, "\nStarting QRender[%d] <%p> in deamon mode, Listening to port : %d\n\n", (int)iPid+1, g_pServer, parser.getServerPort ( ) );    //xxx
    } else
      //logger.log ( Render::Logger::logInformation, "\nStarting QRender[%d] <%016%p>in the foreground, Listening to port : %d\n\n", (int)iPid+1, g_pServer, parser.getServerPort ( ) );    //ooo
      logger.log ( Render::Logger::logInformation, "\nStarting QRender[%d] <%016%d>in the foreground, Listening to port : %d\n\n", (int)iPid+1, g_pServer, (int)parser.getServerPort ( ) ); //xxx
    return app.exec ( );
  }
  return -1;
}

