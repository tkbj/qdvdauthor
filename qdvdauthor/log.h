/***************************************************************************
    log.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef LOG_H
#define LOG_H

#define QDVD_LINUX	//xxx

#ifdef QDVD_LINUX 
  #include <pthread.h>
#else
static int pthread_self () { return 0; };
#endif
class Log
{
public:
  typedef enum {logNone=0, logDebug, logInformation, logWarning, logError, logFatal, maxLogLevel} type;
  Log (type);
  ~Log ();
  void setLogObjectInfo   ( bool );
  void setLogDateTime     ( bool );
  void setLogFileFunction ( bool );
#ifdef QDVD_WIN32
#define dLOG(a,b)
#define iLOG(a,b)
#define wLOG(a,b)
#define eLOG(a,b)
#define fLOG(a,b)

#define sdLOG(a,b,c) 
#define siLOG(a,b,c)
#define swLOG(a,b,c) 
#define seLOG(a,b,c)
#define sfLOG(a,b,c)
#else  
// Here we define the macro to handle logging.
#define dLOG(fmt, args...) ::Global::pLogObject->line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logDebug); ::Global::pLogObject->log (Log::LogDebug, fmt, ## args);
#define iLOG(fmt, args...) ::Global::pLogObject->line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logInformation); ::Global::pLogObject->log (Log::logInformation, fmt, ## args);
#define wLOG(fmt, args...) ::Global::pLogObject->line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logWarning); ::Global::pLogObject->log (Log::logWarning, fmt, ## args);
#define eLOG(fmt, args...) ::Global::pLogObject->line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logError); ::Global::pLogObject->log (Log::logError, fmt, ## args);
#define fLOG(fmt, args...) ::Global::pLogObject->line (this, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logFatal); ::Global::pLogObject->log (Log::logFatal, fmt, ## args);

// the following functions are to be clled from static functions
#define sdLOG(obj, fmt, args...) ::Global::pLogObject->line (obj, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logDebug); ::Global::pLogObject->log (Log::LogDebug, fmt, ## args);
#define siLOG(obj, fmt, args...) ::Global::pLogObject->line (obj, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logInformation); ::Global::pLogObject->log (Log::logInformation, fmt, ## args);
#define swLOG(obj, fmt, args...) ::Global::pLogObject->line (obj, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logWarning); ::Global::pLogObject->log (Log::logWarning, fmt, ## args);
#define seLOG(obj, fmt, args...) ::Global::pLogObject->line (obj, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logError); ::Global::pLogObject->log (Log::logError, fmt, ## args);
#define sfLOG(obj, fmt, args...) ::Global::pLogObject->line (obj, pthread_self (), __FILE__, __FUNCTION__, __LINE__, Log::logFatal); ::Global::pLogObject->log (Log::logFatal, fmt, ## args);
#endif

	void log (const char *string, ...);
	void log (type, const char *string, ...);
	void log (QString);
	void log (type, QString);

	void line (void *, unsigned long, const char *, const char *, long, type);
private:

	int  m_enLogLevel;
	bool m_bLogObjectInfo;
	bool m_bLogDateTime;
	bool m_bLogFileFunction;
	char m_cBuffer[1024];
};

#endif // LOG_H


