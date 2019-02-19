/*****************************************************************************
 ** Class StartMenuRender
 **
 ** This class is part of QDVDAuthor suite and not really of QRender
 ** The purpose of this class is to 
 ** - gather all required information
 ** - create directories
 ** - write xml, and txt files
 ** - keep count of current running threads
 ** - start background threads (calling qrender executable)
 ** - report back status of current render progress
 ** 
 ** NOTE: this class handles all DVDMenus !!!
 *****************************************************************************/

#ifndef STARTMENURENDER_H
#define STARTMENURENDER_H
 
#include <qthread.h>
//#include <q3valuelist.h>	//oxx

#define STATUS_FILE_NAME "status.txt"

class MovieObject;
class DVDMenu;

class StartMenuRender : public QThread
{
	enum {STATUS_VOID=0, STATUS_RENDERING, STATUS_DONE};
public:
	StartMenuRender (MovieObject *);
	StartMenuRender (DVDMenu *, bool b=false);
	virtual ~StartMenuRender ();

	// mandatory function 
	virtual void run ();

	// Here are the working-horse fnuctions ...
	void renderMovieObject      ();
	void renderDVDMenu          ();
	int  checkStatusFile (QString);

	static void registerToExtract    (MovieObject *);
	static void waitForExtraction    (DVDMenu *);
	static void registerToRenderMenu (DVDMenu *);

	MovieObject *movieObject ();
	DVDMenu     *dvdMenu     ();

protected:
	void createDirectories   ();
	void createQRenderXml    ();
	void executeQRender      ();
	bool checkStatusFileDone ();

public:
	static QList<StartMenuRender *>m_listRenderObjects;		//oxx

protected:
	MovieObject *m_pMovieObject;
	DVDMenu     *m_pDVDMenu;
	bool         m_bCalledFromFifo;
	bool         m_bForceRender;    // The user decided to render even if the data exists already.
};

#endif // STARTMENURENDER_H


