/***************************************************************************
    server.h
                             -------------------
    Client - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef RENDER_SERVER_H
#define RENDER_SERVER_H

#include <QList>
#include <QTcpServer>
#include <QObject>  //xxx

#include "logger.h"

namespace Render
{

class Render;

class Server : public QTcpServer  //ooo
//class Server : public QObject   //xxx
{
    Q_OBJECT
    
public:
    //Server ( QObject *p=NULL );   //ooo
    Server(QObject *parent = 0);    //xxx
    //Server();   //xxx

    void setLogInfo ( int, QString );
    Render *findSlideshowThread ( quint64, Render * );

protected:
    //void incomingConnection(int socketDescriptor);    //ooo
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;  //xxx
    //void incomingConnection(qint64 socketDescriptor) Q_DECL_OVERRIDE;  //xxx
    //virtual void incomingConnection(qintptr socketDescriptor);  //xxx
    //virtual void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;  //xxx
    //void incomingConnection(qintptr socketDescriptor) override;  //xxx
    //void incomingConnection(qintptr socketDescriptor);  //xxx
    //virtual void incomingConnection(quintptr socketDescriptor);  //xxx
    //void incomingConnection(quintptr socketDescriptor) Q_DECL_OVERRIDE;  //xxx
    
protected slots:
    virtual void slotRenderExited ( ::Render::Render * );

private:
    QList<Render *> m_listOfRenderTasks;
    int     m_iLogLevel;
    QString m_qsLogFileName;
    Logger  m_logger;
};

}; // end of namespace Render

#endif
