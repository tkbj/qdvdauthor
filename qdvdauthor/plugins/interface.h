/*****************************************************************************
 ** After the User has selected and grouped all Images, and Videos, we'll have a list of groups.
 **
 ** To create a DVD structure from this list the following information should be provided.
 ** 
 ** General :
 **  - Plugin to use for DVD creation
 **  - type ( NTSC/PAL )
 **  - Max # menu buttons
 **  - Project Name
 **  - temp path 
 ** 
 ** Group : 
 **  - Type ( Slideshow / Videos )
 **  - Name of the Group
 **  - Create Chapter menu(s) -yes/no-
 **  - Date from - to of material within the group
 **  - Source files 
 **  - Audio File(s) for Slideshow
 **
 ** Source Files :  ( Slideshow already in VOB + Chapters )
 **  - File Name
 **  - Start Date - End Date
 **
 ** NOTE: Slideshow information is profided as video file(s). 
 **       I.e. The individual images are already converted int VOBs.
 **
 *****************************************************************************/

#ifndef __PLUGIN_INTERFACE_H__
#define __PLUGIN_INTERFACE_H__

#include <qobject.h>
#include <qdatetime.h>
#include <qstringlist.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#include "sourcegroups.h"

// This define should be set in the .pro file of the plugin like this :
//VERSION  = 1.4
//DEFINES += PLUGIN_VERSION=$${VERSION}
#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION 0.0
#endif

namespace Plugin
{

class Interface : public QObject
{
  Q_OBJECT

  public:
    Interface ( ) { m_enType=SourceGroup::NTSC; m_qsProjectName="Unnamed"; m_qsTempPath="/tmp"; };
    virtual ~Interface ( ) {
      QList<SourceGroup *>::iterator it=m_listOfGroups.begin ( );	//oxx
      while ( it != m_listOfGroups.end ( ) )
        delete *it++;
    };
    void addGroup ( SourceGroup *pGroup )  { m_listOfGroups.append ( pGroup ); };
    void setTempPath   ( QString &qsTempPath ) { m_qsTempPath    = qsTempPath; };
    void setProjectName( QString &qsName     ) { m_qsProjectName = qsName;     };

    virtual QString &getPluginName    ( ) { return m_qsPluginName; };
    virtual double   getPluginVersion ( ) { return PLUGIN_VERSION; };

    virtual QWidget *getConfigWidget  ( QWidget * ) = 0;
    virtual bool execute ( ) = 0;

  signals:
    // return to the empty qdvdauthor
    // This signal will create the DVDMenu
    void signalNewMenuResponse ( int, QString, QString );
    // This signal will create a new SourceFileEntry
    void signalNewSource       ( Plugin::SourceGroup * );

  protected:
    // General :
    QString m_qsPluginName;
    SourceGroup::type m_enType;
    QString m_qsProjectName;
    QString m_qsTempPath;

    QList<SourceGroup *> m_listOfGroups;	//oxx
}; // end class Interface

}; // end namespace Plugin

// the types of the class factories
typedef Plugin::Interface *create_t ( );
typedef void destroy_t ( Plugin::Interface *);

#endif // __PLUGIN_CREATEDVD_H__ 

