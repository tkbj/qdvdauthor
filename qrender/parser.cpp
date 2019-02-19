/***************************************************************************
    parser.cpp
                             -------------------
    Class QRender::Parser                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug> //xxx

#include "utils.h"
#include "shared.h"
#include "parser.h"
#include "filter.h"
#include "slideshow.h"

// Note: Maybe later on I can import dvd-slideshow config files
#include "../qdvdauthor/xml_slideshow.h"

extern char *optarg;
extern int optind, opterr, optopt;

namespace Render
{

Parser::Parser ( )
{
  m_iRegionalZone = -1;     // No Zone.
  m_bTTY          = false;  // default is to deamonize
  m_iLogLevel     = 3;      // Anything warning and above
  m_iServerPort   = SERVER_PORT;
  m_qsTempPath    = QDir::tempPath ( ) + "/qrender/";
}

Parser::~Parser ( )
{
}

bool Parser::parseCommandLine ( int iArgs, char *pArguments[] )
{  
  bool bImportDVDSlideshow = false;
  int iChar = 0;

  // The default log file name
  m_qsLogFileName = QDir::tempPath ( ) + "/qrender/log.txt";

  static struct option longOpts[] =  {
    { "verbose", no_argument,       0, 'v' },
    { "help",    no_argument,       0, 'h' },
    { "filter",  no_argument,       0, 'f' },
    { "ix",      required_argument, 0, 'i' },
    { "id",      required_argument, 0, 'j' },
    { "port",    required_argument, 0, 'p' },
    { "log",     required_argument, 0, 'l' },
    { "tty",     no_argument,       0, 't' },
    { "zone",    required_argument, 0, 'z' },
    { "path",    required_argument, 0, 'a' },
    { 0, 0, 0, 0 } };

  while ( true ) {
    iChar = getopt_long_only ( iArgs, pArguments, "hti:o:p:d:v:l:", longOpts, NULL );
    if ( iChar == -1 )
      break;

    switch ( iChar ) {
      case 'i':
        m_qsInputName  = QString ( optarg );
      break;
      case 'j':  {
        m_qsInputName       = optarg;
        bImportDVDSlideshow = true;
      }
      break;
      case 'o':
        m_qsOutputName = QString ( optarg );
      break;
      case 'p':
        m_iServerPort  = atoi ( optarg );
      break;
      case 'v':
        m_iLogLevel = 5 - atoi ( optarg );
      break;
      case 'l':
        m_qsLogFileName = QString ( optarg );
      break;
      case 't':
        m_bTTY = true;
      break;
      case 'z':
        m_iRegionalZone = atoi ( optarg );
      break;
      case 'a':
        m_qsPath = QString ( optarg );
      break;
      case 'f':
        printFilter ( );
        return true;
      break;
      case 'h':
      default:
        printHelp ( );
        return true;
      break;
    }
  }

  // Make sure the qrender temp path exists
  Utils theUtils;
  QFileInfo fileInfo ( m_qsLogFileName );
  theUtils.recMkdir  ( fileInfo.absolutePath ( ) );

  if ( bImportDVDSlideshow )  {
    // Imprt a dvd-slideshow config file
  }
  else if ( ! m_qsInputName.isEmpty ( ) )  {
    return createSlideshow ( );
  }

  return false; // meaning to start render as a deamon
}

int Parser::getServerPort ( )
{
  return m_iServerPort;
}

int Parser::getLogLevel ( )
{
  return m_iLogLevel;
}

int Parser::getRegionalZone ( )
{
  return m_iRegionalZone;
}

QString Parser::getPath ( )
{
  return m_qsPath;
}

QString Parser::getLogfileName ( )
{
  return m_qsLogFileName;
}

bool Parser::isTTY ( )
{
  return m_bTTY;
}

void Parser::printHelp ( )
{ //"it:o:p:"
  printf ( "QRender version %s, built date %s\n", QRENDER_VERSION, __DATE__ );
  printf ( "Part of QDVDAuthor: http://qdvdauthor.sf.net\n\n" );

  // NOt yet ... For a future release.
  //  printf ( "qrender [-hvt] [-p port] [-o FILE] [-ix|id FILE] <... image files>\n" );
  printf ( "qrender [-hvt] [-p port] [-o FILE] [-ix FILE]\n" );
  printf ( " -h  --help        print this help and exit.\n" );
  printf ( " -f  --filter      print the list of supported filter.\n" );
  printf ( " -v  --verbose lvl logging level [0..5] where 0=no output.\n\n" );

  printf ( " -t  --tty         start qrender as background thread.\n"   );
  printf ( " -p  --port        set listening port.\n"   );
  printf ( " -o  --output FILE set output file.\n" );
  printf ( " -l  --log    FILE set log file name.\n" );
  printf ( " -ix FILE          import slideshow XML.\n" );
  // Not yet ... For a future release
  //  printf ( " -id FILE          import dvd-slideshow cfg file.\n" );
  printf ( "\n" );
}

void Parser::printFilter ( )
{
  Filter filter;
  QStringList filterList = filter.getFilterNames ( );

  QStringList::iterator it = filterList.begin ( );
  while (  it  !=  filterList.end ( ) )  {
    printf ( "%s\n", (const char *)(*it++).toUtf8 ( ) );
  }
}

bool Parser::createSlideshow ( )
{   
  CXmlSlideshow xmlSlide;
  if ( ! xmlSlide.readXml ( m_qsInputName, false ) )
    return true;  // Error happend.

  Slideshow slide   ( this );
  slide.initMe      ( &xmlSlide, m_qsTempPath );
  slide.setFileName ( m_qsOutputName );
  if  (  slide.exec ( ) )  {
    // Presumably success. Cool
    QString qsFile = xmlSlide.slideshow_name;
    qsFile.replace ( "/", "_" );
    QString qsFileName = m_qsTempPath + qsFile + ".vob";
    if ( ! m_qsOutputName.isEmpty ( ) )
      qsFileName = m_qsOutputName;

    QFileInfo fileInfo ( qsFileName );
    if ( fileInfo.exists ( ) )
      //printf ( "SUCCESS. The new file can be found under :\n%s\n\n", (const char *)qsFileName.toAscii ( ) );          //ooo
      printf ( "SUCCESS. The new file can be found under :\n%s\n\n", (const char *)qsFileName.toLatin1 ( ).data ( ) );  //xxx
    else
      //printf ( "Final slideshow file <%s> not found.\n", (const char *)qsFileName.toAscii ( ) );  //ooo
      printf ( "Final slideshow file <%s> not found.\n", (const char *)qsFileName.toLatin1 ( ) );   //xxx
  }
  return true;
}

};  // End of namespace Render

