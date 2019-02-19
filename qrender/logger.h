/***************************************************************************
    logger.h
                             -------------------
    Logger - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef RENDER_LOGGER_H
#define RENDER_LOGGER_H

#include <QMutex>

#define MAX_LOG_LEN 4096

class QFile;

namespace Render
{

class Logger
{
public:
  typedef enum {logNone=0, logDebug, logInformation, logWarning, logError, logFatal, maxLogLevel} type;

   Logger ( );
  ~Logger ( );

  void setLogInfo ( int, QString );

  void setLogObjectInfo   ( bool );
  void setLogDateTime     ( bool );
  void setLogFileFunction ( bool );

// Here we define the macro to handle logging.
// dLOG for debugging the server
#define dLOG(fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, ::Render::Logger::logDebug, fmt, ## args);
// iLOG for information about workflow
#define iLOG(fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, ::Render::Logger::logInformation, fmt, ## args );
// wLOG for warning messages
#define wLOG(fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, ::Render::Logger::logWarning, fmt, ## args );
// eLOG for error messages ( recoverable )
#define eLOG(fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, ::Render::Logger::logError, fmt, ## args );
// fLOG for fatal errors ( unrecoverable )
#define fLOG(fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, ::Render::Logger::logFatal, fmt, ## args );
// fLOG for fatal errors ( unrecoverable )
#define LOG(lvl, fmt, args...) line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, lvl, fmt, ## args );

  void log  (       const char *, ... );
  void log  ( type, const char *, ... );
  void log  (       QString );
  void log  ( type, QString );

  void line ( void *, unsigned long, const char *, const char *, long, type, const char *, ... );
  int logLevel ( );

protected:
  void lock    ( );
  void unlock  ( );

  long long buffLen ( );

private:
  static  QMutex  m_mutex;
  QString m_qsLogFileName;
  QFile  *m_pLogFile;
  int     m_enLogLevel;
  bool    m_bLogObjectInfo;
  bool    m_bLogDateTime;
  bool    m_bLogFileFunction;
  char    m_cBuffer[MAX_LOG_LEN];
};

}; // end of namespace Render

#endif
