/***************************************************************************
    log.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <stdarg.h>
#include <qstring.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "log.h"

Log::Log (type enLogLevel)
{
  m_enLogLevel = enLogLevel;
  m_cBuffer  [0] = 0;
  m_bLogObjectInfo = m_bLogDateTime = m_bLogFileFunction = true;
}

Log::~Log ()
{

}

void Log::setLogObjectInfo ( bool bLogObjectInfo )
{
  m_bLogObjectInfo = bLogObjectInfo;
}

void Log::setLogDateTime ( bool bLogDateTime )
{
  m_bLogDateTime = bLogDateTime;
}

void Log::setLogFileFunction ( bool bLogFileFunction )
{
  m_bLogFileFunction = bLogFileFunction;
}

void Log::log (const char *string, ...)
{
  // at this point m_cBuffer should have file/func/line or should be empty.
  va_list theList;
  va_start(theList, string);
  vsprintf((char *)&(m_cBuffer[ strlen(m_cBuffer) ]), string, theList);
  va_end(theList);

  // and finally we can print the result ...
  char *pTemp=strdup ( m_cBuffer );
  log          ( logDebug, pTemp );
  free                   ( pTemp );
}

void Log::log (type enLogLevel, const char *fmt, ...)
{
  int iLogLevel = (int) enLogLevel;
  if (enLogLevel >= m_enLogLevel) {
    m_cBuffer[0] = '\0';
    return;
  }

  // sanity check ..
  if ( ( enLogLevel < logNone ) || (enLogLevel >= maxLogLevel ) )
    iLogLevel = 0; 
  const char *logLevel [] = { "None   ", "Debug  ", "Info   ", "Warning", "Error  ", "Fatal"};

  // at this point m_cBuffer should have file/func/line or should be empty.
  va_list theList;
  va_start(theList, fmt);
  vsprintf((char *)&(m_cBuffer[ strlen(m_cBuffer) ]), fmt, theList);
  va_end(theList);
 
  // and finally we can print the result ...
  printf ( "%s : %s\n", logLevel[iLogLevel], m_cBuffer );
  // Oh and don't forget to re-set the log.
  m_cBuffer  [0] = '\0';
}

void Log::log (QString qsLog)
{
  //log ((const char *)qsLog);		//ooo
}

void Log::log (type iLogLevel, QString qsLog)
{
  //log (iLogLevel, (const char *)qsLog);	//ooo
}

void Log::line (void *pObj, unsigned long iThreadID, const char *pFile, const char *pFunction, long iLine, type enLogLevel)
{
  m_cBuffer[0] = '\0';
  if (enLogLevel < m_enLogLevel) {
    if ( m_bLogObjectInfo ) {
      char cForm[]={"%016p %08x "};
      sprintf (m_cBuffer, cForm, pObj, iThreadID );
      //      sprintf (m_cBuffer, "%p %ld ", pObj, iThreadID );
    //      sprintf (m_cBuffer, "%016p %08x ", pObj, iThreadID );
    }
    if ( m_bLogDateTime ) {
      char cBuffer[36];
      time_t theTime = time ( NULL );
      strftime(cBuffer, 36,"%Y/%m/%d %H:%M:%S", localtime ( &theTime ) );
      sprintf (&(m_cBuffer[ strlen(m_cBuffer) ]), "%s ", cBuffer );
    }
    if ( m_bLogFileFunction )
      sprintf (&(m_cBuffer[ strlen(m_cBuffer) ]), "%s::%s:%04ld > ", pFile, pFunction, iLine);
  }
}


