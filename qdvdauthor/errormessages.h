/***************************************************************************
    errormessages.h
                             -------------------
    ErrorMessages - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class holds error strings and messages for the user on how to
   rectify these errors.
    
****************************************************************************/

#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#include <qobject.h>
//#include <q3valuelist.h>	//oxx

class ErrorMessages : public QObject
{
  Q_OBJECT

  class Message
  {
  public:
    QString qsError;
    QString qsExampleError;
    QString qsHelp;
  };

public: 
   ErrorMessages ( );
  ~ErrorMessages ( );

  QString errorString ( int );
  QString helpMessage ( int );
  int count ( );

protected:
  void initMe ( );
  void add ( QString &, QString &, QString & );

private:
  QList<Message *> m_listOfMessages;	//oxx
};

#endif // ERRORMESSAGES_H
