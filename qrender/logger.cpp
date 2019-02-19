/***************************************************************************
    logger.cpp
                             -------------------
    Main file
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This is the main file.
    
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <QString>
#include <QFile>

#include "logger.h"

namespace Render
{

QMutex Logger::m_mutex;

Logger::Logger ( )
{
  m_cBuffer[0]       = 0;
  m_enLogLevel       = logWarning;
  m_bLogObjectInfo   = false;
  m_bLogDateTime     = true;
  m_bLogFileFunction = true;
  m_pLogFile         = NULL;
}

Logger::~Logger ( )
{
  if ( m_pLogFile )  {
    m_pLogFile->flush ( );
    m_pLogFile->close ( );
  }
  delete m_pLogFile;
  m_pLogFile = NULL;
}

int Logger::logLevel ( )
{
  return (int)m_enLogLevel;
}

void Logger::setLogInfo (int iLogLevel, QString qsLogFileName )
{
  m_enLogLevel    = (type)iLogLevel;
  m_qsLogFileName = qsLogFileName;
  // Lets create the file ...
  if ( ! m_qsLogFileName.isEmpty ( ) )  {
    m_pLogFile = new QFile  ( m_qsLogFileName );
    QIODevice::OpenModeFlag enAppendFlag = QIODevice::Append;
    // Trunkate if the size is > 2 Megs
    if ( m_pLogFile->size ( ) > 1024 * 1024 * 2 )
         enAppendFlag = QIODevice::NotOpen;
    if ( ! m_pLogFile->open ( QIODevice::WriteOnly | QIODevice::Text | enAppendFlag ) )  {
      delete m_pLogFile;
      log ( logFatal, "ERROR : Could not open log file <%s> for writing.\n", (const char *)m_qsLogFileName.toUtf8 ( ) );
      log ( logFatal, "Please check directory or filoe permissions.\n" );
      m_pLogFile = NULL;
    }
  }
}

void Logger::setLogObjectInfo ( bool bLogObjectInfo )
{
  m_bLogObjectInfo = bLogObjectInfo;
}

void Logger::setLogDateTime ( bool bLogDateTime )
{
  m_bLogDateTime = bLogDateTime;
}

void Logger::setLogFileFunction ( bool bLogFileFunction )
{
  // Should the File name and function name be displayed ?
  m_bLogFileFunction = bLogFileFunction;
}

void Logger::lock ( )
{
  m_mutex.lock ( );
}

void Logger::unlock ( )
{
  m_mutex.unlock ( );
}

long long Logger::buffLen ( )
{
  long long iStrLen = strlen ( m_cBuffer );
  if ( iStrLen < 0 )
       iStrLen = 0;
  if ( iStrLen > MAX_LOG_LEN )
       iStrLen = MAX_LOG_LEN - 1;
  return iStrLen;
}

void Logger::log ( const char *string, ... )
{
  // at this point m_cBuffer should have file/func/line or should be empty.
  va_list theList;
  lock ( );
  va_start ( theList, string );
  vsprintf ( (char *)&(m_cBuffer[ buffLen ( ) ]), string, theList );
  va_end   ( theList );
  // and finally we can print the result ...
  char *pTemp=strdup ( m_cBuffer );
  unlock ( );

  log ( logDebug, pTemp );
  free          ( pTemp );
}

void Logger::log ( type enLogLevel, const char *fmt, ... )
{
  int  iLogLevel  = (int) enLogLevel;
  if (enLogLevel <= m_enLogLevel) {
     m_cBuffer[0] = '\0';
     return;
  }

  // sanity check ..
  if ( ( enLogLevel < logNone ) || (enLogLevel >= maxLogLevel ) )
    iLogLevel = 0;
  const char *logLevel [] = { "None   ", "Debug  ", "Info   ", "Warning", "Error  ", "Fatal"};

  // at this point m_cBuffer should have file/func/line or should be empty.
  va_list theList;
  lock ( );

  sprintf  ( m_cBuffer, "%s : ", logLevel[iLogLevel] );
  va_start ( theList, fmt );
  vsprintf ( (char  *)&(m_cBuffer[ buffLen ( ) ]), fmt, theList );
  va_end   ( theList );
  // and finally we can print the result ...
  if ( m_pLogFile )  {
       long long iStrLen = buffLen ( );
       m_pLogFile->write ( m_cBuffer, iStrLen );
       m_pLogFile->flush ( );
  }
  else
       printf ( "%s", m_cBuffer );
  unlock ( );
  // Oh and don't forget to re-set the log.
  m_cBuffer  [0] = '\0';
}

void Logger::log ( QString qsLog )
{
  //log ((const char *)qsLog.toAscii ( ) );         //ooo
  log ((const char *)qsLog.toLatin1 ( ).data ( ) ); //xxx
}

void Logger::log ( type iLogLevel, QString qsLog )
{
  //log ( iLogLevel, (const char *)qsLog.toAscii ( ) );         //ooo
  log ( iLogLevel, (const char *)qsLog.toLatin1 ( ).data ( ) ); //xxx
}

void Logger::line ( void *pObj, unsigned long iThreadID, const char *pFile, const char *pFunction, long iLine, type enLogLevel, const char *fmt, ... )
{
  const char *logLevel [] = { "None   ", "Debug  ", "Info   ", "Warning", "Error  ", "Fatal"};
  m_cBuffer[0] = '\0';

  int   iLogLevel  = (int) enLogLevel;
  if ( enLogLevel <= m_enLogLevel  )
     return;

  lock ( );
  // sanity check ..
  if ( ( enLogLevel < logNone ) || (enLogLevel >= maxLogLevel ) )
    iLogLevel = 0; 

  if ( m_bLogObjectInfo ) {
    char cForm[]={"%016p %08x "};
    sprintf (m_cBuffer, cForm, pObj, iThreadID );
  }
  if ( m_bLogDateTime ) {
    char cBuffer[36];
    time_t theTime = time ( NULL );
    strftime(cBuffer, 36,"%Y/%m/%d %H:%M:%S", localtime ( &theTime ) );
    sprintf (&(m_cBuffer[ buffLen ( ) ]), "%s ", cBuffer );
  }
  if ( m_bLogFileFunction )
    sprintf (&(m_cBuffer[ buffLen ( ) ]), "%s::%s:%04ld > ", pFile, pFunction, iLine);

  // at this point m_cBuffer should have file/func/line or should be empty.
  va_list theList;

  va_start ( theList, fmt );
  vsprintf ( (char  *)&(m_cBuffer[ buffLen ( ) ]), fmt, theList );
  va_end   ( theList );
  // and finally we can print the result ...
  if ( m_pLogFile )  {
       char cBuffer[12];
       sprintf ( cBuffer, "%s : ", logLevel[iLogLevel] );
       m_pLogFile->write (   cBuffer, qstrlen (   cBuffer ) );
       m_pLogFile->write ( m_cBuffer, buffLen ( ) );
       m_pLogFile->flush ( );
  }
  else
       printf ( "%s : %s", logLevel[iLogLevel], m_cBuffer );
  unlock ( );
  // Oh and don't forget to re-set the log.
  m_cBuffer  [0] = '\0';
}

}; // end of namespace Render
