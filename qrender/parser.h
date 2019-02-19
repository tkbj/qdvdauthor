/***************************************************************************
    parser.h
                             -------------------
    Parser - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class will parse the command line options and execute the propper
    actions.

****************************************************************************/

#ifndef RENDER_PARSER_H
#define RENDER_PARSER_H

#include <QString>

namespace Render
{

class Parser
{
public:
           Parser ( );
  virtual ~Parser ( );

  bool    parseCommandLine ( int, char ** );
  QString getLogfileName   ( );
  QString getPath          ( );
  int     getServerPort    ( );
  int     getLogLevel      ( );
  int     getRegionalZone  ( );
  bool    isTTY            ( );

protected:
  void printHelp       ( );
  void printFilter     ( );
  bool createSlideshow ( );

private:
  QString m_qsTempPath;
  QString m_qsInputName;
  QString m_qsOutputName;
  int     m_iRegionalZone;
  int     m_iServerPort;
  bool    m_bTTY;
  int     m_iLogLevel;
  QString m_qsLogFileName;
  QString m_qsPath;
};

}; // end of namespace Render

#endif
