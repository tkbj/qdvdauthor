/***************************************************************************
    utils.h
                             -------------------
    Utils - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef UTILS_H
#define UTILS_H

//#include <q3valuelist.h>	//oxx
//#include <qstringlist.h>	//ooo
#include <QStringList>		//xxx
//#include <qcolor.h>		//ooo
#include <QColor>		//xxx
#include <QImage>		//xxx
#include <QThread>		//xxx

class SleeperThread8 : public QThread	//xxx
{
public:
    static void msleep(unsigned long int msecs)
    {
        QThread::msleep(msecs);
    }
};

class Utils
{

public:
  enum enDefaultParameter {
    twolameSampleRate = 0,
    ppmtoy4mChroma,
    defaultParameterCount
  };
	// Structure to store the paths to the different tools.
	struct toolsPaths {
		toolsPaths ()	{ bPresent = false; };
		QString qsExecutableName; 
		QString qsFullPath; 
		bool bPresent;
	};

	 Utils ();
	~Utils ();

	void augmentAlpha ( int, QImage &, QImage &, QImage &, QImage & );
	void augmentAlpha ( int, int, QImage &, QImage &, QImage &, QImage &, QImage *p=NULL );
	void createStructureIcons    ( );
	QString currentStyleName     ( );
	QString formatChapter        ( QString );
	QString longToNorm           ( long );
	long    normToLong           ( QString );
	QString getAttribute         ( QString, QString);
	bool    recRmdir ( const QString &, const char *p=NULL) const;
	bool    recMkdir ( const QString & );
	QString iso639   ( QString, bool b=false, int i=-1 );
	QString checkForExe          ( QString );
	QString getToolsDisplayName  ( QString );
	int getIndexFromToolName     ( QString );
	QString getTempFile          ( QString );
	QString getTempPath          ( QString );
	QStringList getProjectsFromTempPath  ( );
	QString getUniqueTempFile    ( QString );
	QString getToolPath          ( QString );
	QString getToolByIndex       ( uint );
	bool    isMandatory          ( uint );
	QList<Utils::toolsPaths *> scanSystem ( );		//oxx
	QRgb    nearestColor         ( QRgb    *, int, QRgb   );
	void    reduceColors         ( QImage  &, int, QRgb * );
	long    countColors          ( QImage  & );
	float   getVersionInfo       ( QString & );
	QImage  convertStringToImage ( QString & );
	QString convertImageToString ( QImage  & );
	QString getVideoFormat       ( int,  int );
	int     getFormat            ( int,  int );
	int     getFormat            ( QString  &);
	bool    getResolution        ( int, int &, int & );
	//bool    webToFile            ( QString  &, QString & );	//ooo
	QStringList localTemplates   ( );
	QString findGuiSU            ( );

        bool    getWidthHeight       ( QString, int &, int & );
	long    getMsFromString      ( QString       );
	QString getStringFromMs      ( long          );
	int     getWHFromResolution  ( QString, bool );
	bool    isMpeg2              ( QString       );
	bool    hasVobus             ( QString       );
	bool    checkCommand         ( const char *  );
	QString getDefaultParameter  ( enDefaultParameter );
};

#endif  // UTILS_H
