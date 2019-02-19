/***************************************************************************
    global.cpp
                             -------------------
    Class Global implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Here we have a few global variables.
    
****************************************************************************/

#include "global.h"
#include "cachethumbs.h"	//ooo
#include <QEventLoop>

Log    *Global::pLogObject           = NULL;
QString Global::qsProjectFileName    = QString (PROJECT_FILENAME);
QString Global::qsProjectName        = QString (PROJECT_NAME);
QString Global::qsCurrentPath        = QString ("./");
QString Global::qsBashString         = QString ("#!/bin/bash\n\n");
QString Global::qsTempPath           = QString ("/tmp");
QString Global::qsSystemPath         = QString ("/usr");
QString Global::qsProjectPath        = QString ("");
QString Global::qsExternalPlayer     = QString ("qplayer");
QString Global::qsLanguage           = QString ("en");
QString Global::qsDefaultLanguage    = QString ("en");
QString Global::qsDefaultFormat      = QString ("NTSC");
uint    Global::iRegionalZone        = 0; // All zones
uint    Global::iMaxRunningThreads   = 2;
int     Global::iChildThreadPriority = 0;
int     Global::iDeleteTempOlderThan = 30;
bool    Global::bPNGRender           = true; // png or jpg to render animated menus.
QList <Utils::toolsPaths *> Global::listToolsPaths;	//ooo
QDVDAuthor    *Global::pApp          = NULL;
MenuObject    *Global::pCopyObject   = NULL;
Cache::Thumbs *Global::pThumbsCache  = NULL;
SlideDefaults *Global::pSlideDefaults= NULL;
QEventLoop     Global::eventLoop;

Global::Global()
{
}

Global::~Global()
{
}


