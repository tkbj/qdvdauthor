/***************************************************************************
    dvdmenu.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    This is the main class for the DVDMenu.
    
****************************************************************************/

#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QGridLayout>
#include <QPalette>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>			//xxx
#include <QFrame>			//xxx
#include <QMenu>			//xxx
#include <QTimer>
#include <QDomElement>

#include "xml_dvd.h"
#include "dvdmenu.h"
#include "qdvdauthor.h"
#include "messagebox.h"
#include "dialogmovie.h"
#include "menupreview.h"
#include "movieobject.h"		//ooo
#include "cachethumbs.h"
#include "dialogresize.h"
#include "qimagehelper.h"		//ooo
#include "buttonobject.h"		//ooo
#include "xml_slideshow.h"      //xxx
#include "sourcefileentry.h"
#include "structuretoolbar.h"
#include "dvdmenuundoobject.h"
#include "filepreviewdialog.h"
#include "preview_file_dialog.h"    //xxx
#include "utils.h"
#include "qrender/startmenurender.h"
#include "qplayer/engines/dummywidget.h"

//////////////////////////////////////////////////////////////
//
// The Interface class ...
//
//////////////////////////////////////////////////////////////

CDVDMenuInterface::CDVDMenuInterface ()
{
   initMe ();
}

CDVDMenuInterface::~CDVDMenuInterface ()
{
  
}

void CDVDMenuInterface::initMe ()
{
  uint t;
  bModified                 = false;
  bIsSubMenu                = false;
  bMovieBackgroundHasSound  = false;
  qsBackgroundFileName      = QString ("");
  qsMovieFileName           = QString ("");
  qsMenuName                = QString (VMGM_LABEL);
  qsResizeAlgo              = QString (QT_RESIZE_ALGO);
  qsAnimation               = QString ("");
  iStretchType              = StretchToFit;
  posOffset                 = QSize (0, 0);
  iFormat                   = FORMAT_NONE;
  iWidth                    = 0;
  iHeight                   = 0;
  qsRatio                   = QString ("4:3");
  iTitleset                 = -1;
  iMenu                     = -1;
  pgcColors[0]              = Rgba ( TRANSPARENT_COLOR       );
  pgcColors[1]              = Rgba ( START_HIGHLIGHTED_COLOR );
  pgcColors[2]              = Rgba ( START_SELECTED_COLOR    );
  pgcColors[3]              = Rgba ( START_FRAME_COLOR       );
  //timeDuration              = timeDuration.fromString ("00:00:01");   //ooo
  timeDuration              = timeDuration.fromString ("00:00:01", "hh:mm:ss");    //xxx
  qsSelectedMaskFileName    = QString ("");
  qsHighlightedMaskFileName = QString ("");
  for (t=0;t<(uint)listIntroVobs.count();t++)
    delete listIntroVobs[t];
  listIntroVobs.clear ();
  for (t=0;t<(uint)listExtroVobs.count();t++)
    delete listExtroVobs[t];
  listExtroVobs.clear ();
}

///////////////////////////////////////////////
//
// This function loads the xml project file
// specified by the  user.
//
// The file specs are defined in xml_dvd.h
//
////////////////////////////////////////////////
bool CDVDMenuInterface::readProjectFile ( QDomNode &theNode, float fLoadVersion )
{ 
	QDomElement theElement;// = theNode.toElement();
	QString  tagName;// = theElement.tagName();
	QString  nodeText;// = theElement.text ();
	int      t;
	QDomAttr a;

	for ( t=0; t<(int)listSoundEntries.count ( ); t++ )
	  delete listSoundEntries[t];
	listSoundEntries.clear();
	QDomNode xmlMenuVariables = theNode.firstChild();
	while ( !xmlMenuVariables.isNull () )	{
		// Okay, here we retain the stored data from the xml file.
		theElement = xmlMenuVariables.toElement();
		tagName    = theElement.tagName();
		nodeText   = theElement.text ();
		// Okay, this is ugly but after all it is the simplest of all xml file structure.
		// No need to get fancy ...
		if (tagName == MENU_HIGHLIGHTED_MASK)
			qsHighlightedMaskFileName = nodeText;
		else if (tagName == MENU_SELECTED_MASK)
			qsSelectedMaskFileName = nodeText;
		else if (tagName == MENU_SOUND) {
		  //listSoundEntries.append (nodeText);
		  Audio      *pAudio   = NULL;
		  QDomNode    xmlAudio = theElement.firstChild ( );
		  while ( !xmlAudio.isNull ( ) )  {
		    pAudio = new Audio;
		    if ( fLoadVersion > 1.0101 )
		      pAudio->readProjectFile( xmlAudio );
		    else {
		      pAudio->qsFileName   = nodeText;
		      pAudio->qsIso639     = Global::qsDefaultLanguage;
		      pAudio->iAudioNumber = listSoundEntries.count( );
		    }
		    listSoundEntries.append  ( pAudio );
		    xmlAudio = xmlAudio.nextSibling ( );
		  }
		}
		else if (tagName == MENU_MOVIE)
			qsMovieFileName = nodeText;
		else if (tagName == MENU_OFFSET)
			timeOffset = timeOffset.fromString (nodeText);
		else if (tagName == MENU_DURATION)
			timeDuration = timeDuration.fromString (nodeText);
		else if (tagName == MENU_ANIMATION)
			qsAnimation = nodeText;
		else if (tagName == MENU_BACKGROUND)	{
			int iOffsetX, iOffsetY;
			iOffsetX = iOffsetY = 0;
			a = theElement.attributeNode ( MENU_RESIZE_ALGO );
			qsResizeAlgo = a.value();
			if (qsResizeAlgo.isEmpty())
				qsResizeAlgo = QString ( QT_RESIZE_ALGO );
			a = theElement.attributeNode ( MENU_STRETCH_TYPE );
			if (!a.value().isEmpty())
				iStretchType = a.value().toInt();
			a = theElement.attributeNode ( MENU_OFFSET_X );
			if (!a.value().isEmpty())
				iOffsetX = a.value().toInt();
			a = theElement.attributeNode ( MENU_OFFSET_Y );
			if (!a.value().isEmpty())
				iOffsetY = a.value().toInt();
			posOffset = QSize (iOffsetX, iOffsetY);
			qsBackgroundFileName = nodeText;
		}
		else if (tagName == MENU_MASK_COLORS)	{
		  //QStringList listColors = QStringList::split (",", nodeText);	//ooo
		  QStringList listColors = nodeText.split (",");			//xxx
		  for ( uint t=0; t<(uint)listColors.count ( ); t++ )  {
		    if ( t>= MAX_MASK_COLORS )
		      continue;
		    pgcColors[t].fromString ( listColors[t] );
		  }
		}
		else if (tagName == MENU_NAME)
			qsMenuName = nodeText;
		else if (tagName == MENU_PRE)
			qsPre = nodeText;
		else if (tagName == MENU_PAUSE)
			qsPause = nodeText;
		else if (tagName == MENU_POST)
			qsPost = nodeText;
		/*else if (tagName == MENU_INTRO) {
		  CXmlDVDAuthor::vob_struct *pVob = NULL;
		  QDomNode    xmlIntro = theElement.firstChild();
		  QDomElement theIntro;
		  while ( !xmlIntro.isNull () )	{
		    pVob = new CXmlDVDAuthor::vob_struct;

		    theIntro = xmlIntro.toElement();
		    pVob->readXml ( &theIntro );
		    listIntroVobs.append ( pVob );
		    xmlIntro = xmlIntro.nextSibling ( );
		  }
		}
		else if (tagName == MENU_EXTRO) {
		  CXmlDVDAuthor::vob_struct *pVob = NULL;
		  QDomNode    xmlExtro = theElement.firstChild();
		  QDomElement theExtro;
		  while ( !xmlExtro.isNull () )	{
		    pVob = new CXmlDVDAuthor::vob_struct;

		    theExtro = xmlExtro.toElement();
		    pVob->readXml ( &theExtro );
		    listExtroVobs.append ( pVob );
		    xmlExtro = xmlExtro.nextSibling ( );
		  }
		}*/	//oooo
		// So lets get the next sibling ... until we hit hte end of DVDMenu ...
		xmlMenuVariables = xmlMenuVariables.nextSibling();
	}  	
	bModified = false;
	return true;
}

bool CDVDMenuInterface::writeProjectFile (QDomElement &rootDVDMenu)
{
	QDomDocument xmlDoc = rootDVDMenu.ownerDocument();

	uint i;
	QDomElement tag;
	QString qsFormat ("hh:mm:ss.zzz");
	QDomText t;

	t = xmlDoc.createTextNode(qsBackgroundFileName);
	
	if (!qsHighlightedMaskFileName.isEmpty())	{
		tag = xmlDoc.createElement( MENU_HIGHLIGHTED_MASK );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsHighlightedMaskFileName);
		tag.appendChild( t );
	}
	if (!qsSelectedMaskFileName.isEmpty())	{
		tag = xmlDoc.createElement( MENU_SELECTED_MASK );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsSelectedMaskFileName);
		tag.appendChild( t );
	}
	if ( timeOffset.isValid())	{
		tag = xmlDoc.createElement( MENU_OFFSET );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(timeOffset.toString (qsFormat));
		tag.appendChild( t );
	}
	if (timeDuration.isValid())	{
		tag = xmlDoc.createElement( MENU_DURATION );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(timeDuration.toString (qsFormat));
		tag.appendChild( t );
	}
	if ( ! qsAnimation.isEmpty())	{
		tag = xmlDoc.createElement( MENU_ANIMATION );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsAnimation);
		tag.appendChild( t );
	}
	if ( listSoundEntries.count ( ) != 0 )  {
	  tag = xmlDoc.createElement ( MENU_SOUND );
	  for ( i=0; i<(uint)listSoundEntries.count ( ); i++ )
	    listSoundEntries[i]->writeProjectFile ( tag );
	  rootDVDMenu.appendChild ( tag );
	}
	if (!qsMovieFileName.isEmpty())	{
		tag = xmlDoc.createElement( MENU_MOVIE );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsMovieFileName);
		tag.appendChild( t );
	}
	if (!qsBackgroundFileName.isEmpty())	{
		tag = xmlDoc.createElement( MENU_BACKGROUND );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsBackgroundFileName);
		tag.setAttribute ( MENU_RESIZE_ALGO, qsResizeAlgo );
		tag.setAttribute ( MENU_STRETCH_TYPE, QString ("%1").arg(iStretchType));
		tag.setAttribute ( MENU_OFFSET_X, QString ("%1").arg(posOffset.width ()));
		tag.setAttribute ( MENU_OFFSET_Y, QString ("%1").arg(posOffset.height()));
		tag.appendChild  ( t );
	}
	if (!qsMenuName.isEmpty())	{
		tag = xmlDoc.createElement( MENU_NAME );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsMenuName);
		tag.appendChild( t );
	}
	if (!qsPre.isEmpty())	{
		tag = xmlDoc.createElement( MENU_PRE );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsPre);
		tag.appendChild( t );
	}
	if (!qsPause.isEmpty())	{
		tag = xmlDoc.createElement( MENU_PAUSE );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsPause);
		tag.appendChild( t );
	}
	if (!qsPost.isEmpty())	{
		tag = xmlDoc.createElement( MENU_POST );
		rootDVDMenu.appendChild( tag );
		t = xmlDoc.createTextNode(qsPost);
		tag.appendChild( t );
	}
	if ( listIntroVobs.count() > 0 ) {
		tag = xmlDoc.createElement( MENU_INTRO );
		rootDVDMenu.appendChild( tag );
		for (i=0;i<(uint)listIntroVobs.count();i++)
		  listIntroVobs[i]->writeXml (&xmlDoc, &tag);
	}
	if ( listExtroVobs.count() > 0 ) {
		tag = xmlDoc.createElement( MENU_EXTRO );
		rootDVDMenu.appendChild( tag );
		for (i=0;i<(uint)listExtroVobs.count();i++)
		  listExtroVobs[i]->writeXml (&xmlDoc, &tag);
	}	//oooo
	// Here we take care of the mask colors
	QString qsColors;
	for ( int i=0; i<MAX_MASK_COLORS; i++ )
	  qsColors += pgcColors[i].toString ( ) + ",";
	
	tag = xmlDoc.createElement ( MENU_MASK_COLORS );
	rootDVDMenu.appendChild    ( tag );
	t = xmlDoc.createTextNode  ( qsColors );
	tag.appendChild ( t );

	return true;
}

CDVDMenuInterface &CDVDMenuInterface::operator =( CDVDMenuInterface &theOther )
{
  // variables.
  bModified                 = theOther.bModified;
  bIsSubMenu                = theOther.bIsSubMenu;
  qsMenuName                = theOther.qsMenuName;

  // the following entries belong to the movie background
  qsMovieFileName           = theOther.qsMovieFileName;
  bMovieBackgroundHasSound  = theOther.bMovieBackgroundHasSound;
  timeOffset                = theOther.timeOffset;
  timeDuration              = theOther.timeDuration;
  // The following var is the background image or screenshot of the Movie ...
  qsBackgroundFileName      = theOther.qsBackgroundFileName;

  qsSelectedMaskFileName    = theOther.qsSelectedMaskFileName;
  qsHighlightedMaskFileName = theOther.qsHighlightedMaskFileName;
  qsResizeAlgo              = theOther.qsResizeAlgo;
  qsAnimation               = theOther.qsAnimation;
  iStretchType              = theOther.iStretchType;	// StretchToFit, BlackBorders, CutOffset
  posOffset                 = theOther.posOffset;
  iFormat                   = theOther.iFormat;
  iWidth                    = theOther.iWidth;
  iHeight                   = theOther.iHeight;
  iTitleset                 = theOther.iTitleset;
  iMenu                     = theOther.iMenu;
  qsRatio                   = theOther.qsRatio;

  // generated by importdvdauthor.cpp::buildMenuFromPgc()
  int t;
  CXmlDVDAuthor::vob_struct *pNewVob, *pOrigVob;
  Audio *pAudio = NULL;
  for ( t=0; t<(int)listSoundEntries.count ( ); t++ )
    delete listSoundEntries[t];
  listSoundEntries.clear ( );
  for ( t=0; t<(int)theOther.listSoundEntries.count ( ); t++ ) {
     pAudio = new Audio;
    *pAudio = *theOther.listSoundEntries[t];
    listSoundEntries.append ( pAudio );
  }

  for ( t=0; t<(int)listIntroVobs.count ( ); t++ )
    delete listIntroVobs[t];
  listIntroVobs.clear ( );
  for ( t=0; t<(int)theOther.listIntroVobs.count ( ); t++ ) {
    pOrigVob = theOther.listIntroVobs[t];
    pNewVob  = new CXmlDVDAuthor::vob_struct;
    pNewVob->file     = pOrigVob->file;
    pNewVob->chapters = pOrigVob->chapters;
    pNewVob->pause    = pOrigVob->pause;
    listIntroVobs.append ( pNewVob );
  }

  for ( t=0; t<(int)listExtroVobs.count ( ); t++ )
    delete listExtroVobs[t];
  listExtroVobs.clear ( );
  for ( t=0; t<(int)theOther.listExtroVobs.count ( ); t++ )  {
    pOrigVob = theOther.listExtroVobs[t];
    pNewVob  = new CXmlDVDAuthor::vob_struct;
    pNewVob->file     = pOrigVob->file;
    pNewVob->chapters = pOrigVob->chapters;
    pNewVob->pause    = pOrigVob->pause;
    listExtroVobs.append ( pNewVob );
  }

  qsPre               = theOther.qsPre;
  qsPause             = theOther.qsPause;
  qsPost              = theOther.qsPost;

  backgroundImage     = theOther.backgroundImage;
  highlightedMask     = theOther.highlightedMask;
  selectedMask        = theOther.selectedMask;

  for (t=0; t<MAX_MASK_COLORS; t++ )
    pgcColors[t] = theOther.pgcColors[t];

  return *this;	//oooo
}

bool CDVDMenuInterface::operator == ( CDVDMenuInterface &theOther )
{
  bool bReturn = (
     ( bModified                 == theOther.bModified                 ) &&
     ( bIsSubMenu                == theOther.bIsSubMenu                ) &&
     ( qsMenuName                == theOther.qsMenuName                ) &&
     ( qsMovieFileName           == theOther.qsMovieFileName           ) &&
     ( bMovieBackgroundHasSound  == theOther.bMovieBackgroundHasSound  ) &&
     ( timeOffset                == theOther.timeOffset                ) &&
     ( timeDuration              == theOther.timeDuration              ) &&
     ( qsBackgroundFileName      == theOther.qsBackgroundFileName      ) &&
     ( qsSelectedMaskFileName    == theOther.qsSelectedMaskFileName    ) &&
     ( qsHighlightedMaskFileName == theOther.qsHighlightedMaskFileName ) &&
     ( qsResizeAlgo              == theOther.qsResizeAlgo              ) &&
     ( qsAnimation               == theOther.qsAnimation               ) &&
     ( iStretchType              == theOther.iStretchType              ) &&
     ( posOffset                 == theOther.posOffset                 ) &&
     ( iFormat                   == theOther.iFormat                   ) &&
     ( iWidth                    == theOther.iWidth                    ) &&
     ( iHeight                   == theOther.iHeight                   ) &&
     ( iTitleset                 == theOther.iTitleset                 ) &&
     ( iMenu                     == theOther.iMenu                     ) &&
     ( qsRatio                   == theOther.qsRatio                   ) &&
     ( qsPre                     == theOther.qsPre                     ) &&
     ( qsPause                   == theOther.qsPause                   ) &&
     ( qsPost                    == theOther.qsPost                    ) );
//     ( backgroundImage           == theOther.backgroundImage           ) &&
//     ( highlightedMask           == theOther.highlightedMask           ) &&
//     ( selectedMask              == theOther.selectedMask              ) );
  if ( ! bReturn )
    return false;

  // generated by importdvdauthor.cpp::buildMenuFromPgc()
  if ( listSoundEntries.count ( ) != theOther.listSoundEntries.count ( ) )
    return false;
  if ( listIntroVobs.count ( ) != theOther.listIntroVobs.count ( ) )
    return false;
  if ( listExtroVobs.count ( ) != theOther.listExtroVobs.count ( ) )
    return false;

  QList<Audio *>::iterator itAudio1 = listSoundEntries.begin ( );		//oxx
  QList<Audio *>::iterator itAudio2 = theOther.listSoundEntries.begin ( );	//oxx
  while  ( itAudio1 != listSoundEntries.end ( ) )  {
    if ( ! ( *(*itAudio1++) == *(*itAudio2++) ) )
      return false;
  }

  QList <CXmlDVDAuthor::vob_struct *>::iterator itVob1 = listIntroVobs.begin ( );		//oxx
  QList <CXmlDVDAuthor::vob_struct *>::iterator itVob2 = theOther.listIntroVobs.begin ( );	//oxx
  while ( itVob1 != listIntroVobs.end ( ) )  {
    if ( ! ( *(*itVob1++) == *(*itVob2++) ) )
      return false;
  }
  itVob1 = listExtroVobs.begin ( );
  itVob2 = theOther.listExtroVobs.begin ( );
  while ( itVob1 != listExtroVobs.end ( ) )  {
    if ( ! ( *(*itVob1++) == *(*itVob2++) ) )
      return false;
  }

  int t;
  for ( t=0; t<MAX_MASK_COLORS; t++ )  {
    if ( pgcColors[t] != theOther.pgcColors[t] )
      return false;
  }

  return true;
}

/*VMGMenu::VMGMenu ()
{
}*/

VMGMenu::VMGMenu (QDVDAuthor *pDVDAuthor)	//ooo
//VMGMenu::VMGMenu ()
	: DVDMenu (pDVDAuthor)	//ooo
	//: DVDMenu ()
{
  CreateToolBar	   ( pDVDAuthor );
  CreateMenuEntry  ( pDVDAuthor );
  slotUpdateStructure ( );
}

VMGMenu::~VMGMenu ( )
{
}

void VMGMenu::CreateToolBar (QDVDAuthor *pDVDAuthor)
{
  //m_pToolBar = new QToolBar( "", pDVDAuthor, Qt::DockTop );
  //m_pToolBar = new QToolBar( pDVDAuthor );
  //QIcon iconTBToDVD        = QIcon ( ":/images/to_dvd.png" );
  
  //m_pActionCreateDVD           = new QAction ( pDVDAuthor, "createDVD" );
  //m_pActionCreateDVD    = new QAction ( QIcon ( ":/images/to_dvd.png"     ), tr("&createDVD")   , pDVDAuthor );
  
  //pDVDAuthor->m_pToolBar->addAction( m_pActionCreateDVD );
  //m_pToolBar->addAction( m_pActionCreateDVD );
}

void VMGMenu::CreateMenuEntry (QDVDAuthor *pDVDAuthor)
{
  connect( pDVDAuthor, SIGNAL( signalAddBackground ( ) ), this, SLOT( slotAddBackground ( ) ) );
  connect( pDVDAuthor, SIGNAL( signalAddSound ( ) ),      this, SLOT( slotAddSound      ( ) ) );  
}
  
DVDMenu::DVDMenu ( )	//ooo
{
}

DVDMenu::DVDMenu  ( QDVDAuthor * )	//ooo
//DVDMenu::DVDMenu  ( )
	//: QObject ( )  //ooo
	: QWidget ( )  //xxx
{
  m_pPixmapMenu          = NULL;
  m_pStructureItem       = NULL;
  
  CreateTab ( getDVDAuthor ( )->m_pTabWidgetMain );	//ooo
  //CreateTab ( );					//xxx
  slotUpdateStructure ( );
}

DVDMenu::~DVDMenu ()
{
  delete m_pDVDMenuTab;
  if ( m_pStructureItem )
    delete m_pStructureItem;
}

UndoBuffer *DVDMenu::getUndoBuffer ( )
{
  if ( m_pPixmapMenu )
    return m_pPixmapMenu->undoBuffer ( );
  return NULL;
}

MenuPreview *DVDMenu::getMenuPreview ( )
{
  return m_pPixmapMenu;
}

bool DVDMenu::isEmpty ()
{
	// Checks if the menu has more then the default values.
	CDVDMenuInterface *p = getInterface();
	if (	(p->bModified == false) && 
		(p->listSoundEntries.count() < 1)         && 
		(p->qsMovieFileName.isEmpty())            &&
		(p->qsBackgroundFileName.isEmpty())       && 
		(p->qsSelectedMaskFileName.isEmpty())     && 
		(p->qsHighlightedMaskFileName.isEmpty ()) &&
		(p->iStretchType          ==  0) && 
		(p->posOffset.width()     ==  0) && 
		(p->posOffset.height()    ==  0) && 
		(p->iFormat               ==  0) &&
		(p->iWidth                ==  0) && 
		(p->iHeight               ==  0) && 
		(p->qsRatio == QString ("4:3") ) &&
		(p->backgroundImage.isNull ( ) ) && 
		(p->highlightedMask.isNull ( ) ) && 
		(p->selectedMask.isNull    ( ) ) &&
		(getMenuPreview()->objectCount() == 0)	)
		return true;
	/* Not checked ...
	QString qsMenuName;
	QString qsResizeAlgo;
	bool    bIsSubMenu;
	bool    bMovieBackgroundHasSound;
	QTime   timeOffset;
	QTime   timeDuration;
	QColor  pgcColors[MAX_MASK_COLORS];
	*/
	return false;
}

QString DVDMenu::checkDuration ( )
{
  bool bDummy1 = true;
  bool bDummy2 = true;
  return checkDuration ( bDummy1, bDummy2 );
}

QString DVDMenu::checkDuration ( bool &bCheckMovies, bool &bCheckAudio )
{  
  QString qsReturn, qsFormat ( "hh:mm:ss.zzz" );
  // Here we compare the Duration of the menu with the duration of all MovieObjects
  uint  t, i;
  bool  bFound = false, bFoundIssues = false;
  bool  bCheckMovieButtons, bCheckAudioLength;
  long long iDuration, iDifference;
  //QTime     zeroTime, timeMenu = getInterface( )->timeDuration;           //ooo
  QTime     zeroTime(0,0,0,0), timeMenu = getInterface( )->timeDuration;    //xxx
  Audio    *pAudio;

  bCheckMovieButtons = bCheckMovies;
  bCheckAudioLength  = bCheckAudio;
  bCheckMovies = false; // return, no MovieLength difference found
  bCheckAudio  = false; // return no AudioLength difference found

  iDuration = zeroTime.msecsTo ( timeMenu );

  //qsReturn  = tr ( "<P><FONT COLOR=\"ORANGE\"><B><U>%1 : Duration = %2 sec</U></B></FONT><BR><HR>" ).arg ( name ( ) ).arg ( (int)(iDuration / 1000 ) );   //ooo
  qsReturn  = tr ( "<P><FONT COLOR=\"ORANGE\"><B><U>%1 : Duration = %2 sec</U></B></FONT><BR><HR>" ).arg ( name ( ) ).arg ( static_cast<int>(iDuration / 1000 ) );
  if ( zeroTime.msecsTo ( timeMenu ) < 1000 ) {
    qsReturn += tr ( "This Menu does not seem to have a valid length. Its Duration is set to 0 seconds.<P>" );
    return qsReturn;
  }

  ButtonObject     tempButton,     *pButton;
  MovieObject      tempMovie,      *pMovieObject;
  ObjectCollection tempCollection, *pCollection;
  MenuObject      *pMenuObject;
  SourceFileEntry *pEntry;
  SourceFileInfo  *pInfo;
  Utils            theUtils;
  QStringList               listNames;
  QList<MovieObject *> listMovies;// = getMovieObjects ( );		//oxx
  if ( bCheckMovieButtons )  {
    for ( t=0; t<getMenuPreview ( )->objectCount ( ); t++ )  {
      pMenuObject = getMenuPreview ( )->menuObject ( t );
      if ( pMenuObject->objectType ( ) == tempButton.objectType ( ) )  {
        pButton = (ButtonObject *)pMenuObject;
        for ( i=0; i<pButton->getNormalCount ( ); i++ )  {
          if ( pButton->getNormal ( i )->objectType ( ) == tempMovie.objectType ( ) ) {
            pMovieObject = (MovieObject *)pButton->getNormal ( i );
            listMovies.append ( pMovieObject );
            listNames.append  ( pButton->name ( ) );
          }
        }
      }
      else if ( pMenuObject->objectType ( ) == tempCollection.objectType ( ) )  {
        pCollection = (ObjectCollection *)pMenuObject;
        for ( i=0; i<(uint)pCollection->getCount ( ); i++ )  {
          if ( pCollection->getObject ( i )->objectType ( ) == tempMovie.objectType ( ) ) {
            pMovieObject = (MovieObject *)pCollection->getObject ( i );
            listMovies.append ( pMovieObject );
            listNames.append  ( pMovieObject->name ( ) );
          }
        }
      }
      else if ( pMenuObject->objectType ( ) == tempMovie.objectType ( ) ) {
        pMovieObject = (MovieObject *)pMenuObject;
        listMovies.append ( pMovieObject );
        listNames.append  ( pMovieObject->name ( ) );
      }
    }
  }

  // After we have the list of MovieObjects and associated names, we can see if they actually 
  // are different in length.
  for ( t=0; t<(uint)listMovies.count ( ); t++ ) {	//ox
    pMovieObject = listMovies[t];
    if ( ! pMovieObject )
      continue;

    iDifference = pMovieObject->duration ( ).msecsTo ( timeMenu );
    if ( iDifference > 1000 ) {
      bFoundIssues = true;
      bFound       = true;
      qsReturn += tr ( "MovieObject <FONT COLOR=\"BLUE\"><B>%1</B></FONT> is <FONT COLOR=\"RED\"><B>%2</B></FONT> sec shorter then the menu duration." ).arg ( listNames[t] ).arg ( (int)( iDifference / 1000 ) );
    }
    else if ( iDifference < -1000 ) {
      bFoundIssues = true;
      bFound       = true;
      qsReturn += tr ( "MovieObject <FONT COLOR=\"BLUE\"><B>%1</B></FONT> is <FONT COLOR=\"RED\"><B>%2</B></FONT> sec longer then the menu duration." ).arg ( listNames[t] ).arg ( (int)( -iDifference / 1000 ) );
    }
    if ( bFound )  {
      if ( pMovieObject->loop ( ) == MovieObject::Stop )
        qsReturn += tr ( " Movie will stop after %1." ).arg ( pMovieObject->duration ( ).toString ( qsFormat ) );
      if ( pMovieObject->loop ( ) == MovieObject::Stretch )
        qsReturn += tr ( " Movie will stretch to %1." ).arg ( timeMenu.toString ( qsFormat ) );
      if ( pMovieObject->loop ( ) == MovieObject::Loop )
        qsReturn += tr ( " Movie will loop every %1." ).arg ( pMovieObject->duration ( ).toString ( qsFormat ) );
      qsReturn += "<BR>";
      bCheckMovies = true;
    }
    bFound = false;
  }

  qsReturn += "<P>";
  // Next we check the length of the attached SoundEntries.
  if ( bCheckAudioLength )  {
    for ( t=0; t<(uint)getInterface ( )->listSoundEntries.count ( ); t++ ) {	//ox
      pAudio  =  getInterface ( )->listSoundEntries[t];
      if ( ! pAudio )
        continue;

      // How to extract the info from the data in the SourceToolbar ???
      iDuration = 0;
      pEntry = getDVDAuthor ( )->getSourceEntryByDisplayName ( pAudio->qsFileName );
      if ( ! pEntry )
        continue;
      for ( i=0; i<(uint)pEntry->listFileInfos.count ( ); i++ ) {	//ox
        pInfo = pEntry->listFileInfos[i];
        if ( ! pInfo )
          continue;
        iDuration += theUtils.getMsFromString ( pInfo->qsLength );
      }

      iDifference = iDuration - zeroTime.msecsTo ( timeMenu );
      QTime timeDiff;
      if ( iDifference > 1000 ) {
        timeDiff = timeDiff.addMSecs ( iDifference );
        QString qsDiff;
        if ( iDifference > 60000 ) // Oly need this if > 60 seconds
             qsDiff = QString ( " <FONT COLOR=\"RED\"><B>(%1)</B></FONT> " ).arg ( timeDiff.toString ( "hh.mm.ss.zzz" ) );
        bFoundIssues   = true;
        bCheckAudio    = true; // Return info to caller
        qsReturn += tr ( "Menu audio file<FONT COLOR=\"BLUE\"><B>%1</B></FONT> is <FONT COLOR=\"RED\"><B>%2</B></FONT> sec longer %3then the menu duration.<BR>" ).arg ( pEntry->qsDisplayName ).arg ( (int)( iDifference / 1000 ) ).arg ( qsDiff );
      }
      else if ( iDifference < -1000 ) {
        timeDiff = timeDiff.addMSecs ( -iDifference );
        QString qsDiff;
        if ( iDifference > 60000 ) // Oly need this if > 60 seconds
             qsDiff = QString ( " <FONT COLOR=\"RED\"><B>(%1)</B></FONT> " ).arg ( timeDiff.toString ( "hh.mm.ss.zzz" ) );
        bFoundIssues   = true;
        bCheckAudio    = true;  // Return info to caller
        qsReturn += tr ( "Menu duration <FONT COLOR=\"BLUE\"><B>%1</B></FONT> is <FONT COLOR=\"RED\"><B>%2</B></FONT> sec shorter %3then the menu audio file.<BR>" ).arg ( pEntry->qsDisplayName ).arg ( (int)( -iDifference / 1000 ) ).arg ( qsDiff );
      }
    }
  }

  if ( ! bFoundIssues )
   qsReturn = QString ( );

  return qsReturn;
}

bool DVDMenu::readProjectFile ( QDomNode &theNode, float fLoadVersion )
{
  QDomElement theElement = theNode.toElement ( );
  QString tagName;
  QString nodeText;

  QDomAttr a = theElement.attributeNode ( MENU_FORMAT );
  m_DVDMenuInterface.iFormat = a.value().toInt();
  a = theElement.attributeNode ( MENU_WIDTH );
  m_DVDMenuInterface.iWidth  = a.value().toInt();
  a = theElement.attributeNode ( MENU_HEIGHT );
  m_DVDMenuInterface.iHeight = a.value().toInt();
  a = theElement.attributeNode ( MENU_RATIO );
  if ( ! a.value().isEmpty() )
    m_DVDMenuInterface.qsRatio = a.value();
  a = theElement.attributeNode ( MENU_TITLESET_NR );
  if ( ! a.value().isEmpty() )
    m_DVDMenuInterface.iTitleset = a.value().toInt();
  a = theElement.attributeNode ( MENU_MENU_NR );
  if ( ! a.value().isEmpty() )
    m_DVDMenuInterface.iMenu = a.value().toInt();

  QDomNode xmlMenu = theNode.firstChild();
  while (!xmlMenu.isNull())	{  
    // Okay, here we retain the stored data from the xml file.
    theElement = xmlMenu.toElement  ( );
    tagName    = theElement.tagName ( );
    nodeText   = theElement.text    ( );

    if (DVDAUTHOR_MENU == tagName)	{   
      // Let us take care of the DVDMenus properties first
      if ( ! m_DVDMenuInterface.readProjectFile ( xmlMenu, fLoadVersion ) )
        return false;
// Might be wrong. I get the info for the SourceToolbar already through the SourceEntry.
//			if ( getDVDAuthor ( ) )
//			     getDVDAuthor ( )->addSound ( m_DVDMenuInterface.listSoundFiles );
    }
    else if (DVDAUTHOR_PREVIEW == tagName)	{
      // before we read in the infos in ButtonPreview::readProjectFile
      m_pPixmapMenu->readProjectFile (xmlMenu);
    }
    else
      //printf ("Warning: DVDMenu::readProjectFile -=> wrong XML Node <%s>\nContinuing ...\n", (const char *)tagName);	//ooo
      printf ("Warning: DVDMenu::readProjectFile -=> wrong XML Node <%s>\nContinuing ...\n", tagName.toLatin1().data());		//xxx
    // So lets get the next sibling ... until we hit hte end of DVDMenu ...
    xmlMenu = xmlMenu.nextSibling();
  }

  if ( ! m_DVDMenuInterface.qsBackgroundFileName.isEmpty ( ) ) {
    QImage tempImage (m_DVDMenuInterface.qsBackgroundFileName );
    int iWidth  = m_DVDMenuInterface.iWidth;
    int iHeight = m_DVDMenuInterface.iHeight;
    if ( ( iWidth > 10 ) && ( iHeight > 10 )  )
      //m_DVDMenuInterface.backgroundImage.convertFromImage ( tempImage.smoothScale ( iWidth, iHeight, Qt::IgnoreAspectRatio ) );				//ooo
      m_DVDMenuInterface.backgroundImage.convertFromImage ( tempImage.scaled ( iWidth, iHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );	//xxx
    else
      m_DVDMenuInterface.backgroundImage.convertFromImage ( tempImage );
    //m_pPixmapMenu->setPixmap ( (const QPixmap &) m_DVDMenuInterface.backgroundImage );    //oooo
    m_pPixmapMenu->set_Pixmap ( (const QPixmap &) m_DVDMenuInterface.backgroundImage );     //xxx
    refresh ( );
  }
  // If it is the Main Menu we are loading, we won't need to set the name.
  if ( m_DVDMenuInterface.qsMenuName != QString ( VMGM_LABEL ) )
    setTabLabel ( m_DVDMenuInterface.qsMenuName );	//oooo

  if ( getUndoBuffer ( ) )
       getUndoBuffer ( )->clear ( );	//oooo

  return true;
}

bool DVDMenu::writeProjectFile ( QDomElement &rootElement )
{ 
  QDomDocument xmlDoc = rootElement.ownerDocument();
  QDomElement rootDVDMenu;
  if (m_DVDMenuInterface.bIsSubMenu)
    rootDVDMenu = xmlDoc.createElement( DVDAUTHOR_ROOT_MENU );
  else
    rootDVDMenu = xmlDoc.createElement( DVDAUTHOR_ROOT_VMGM );

  QDomElement dvdMenu = xmlDoc.createElement( DVDAUTHOR_MENU );
  QDomElement dvdPreview = xmlDoc.createElement( DVDAUTHOR_PREVIEW );

  // We set the Main menu attributes here ...
  rootDVDMenu.setAttribute ( MENU_FORMAT, QString("%1").arg(m_DVDMenuInterface.iFormat)  );
  rootDVDMenu.setAttribute ( MENU_WIDTH,  QString("%1").arg(m_DVDMenuInterface.iWidth)   );
  rootDVDMenu.setAttribute ( MENU_HEIGHT, QString("%1").arg(m_DVDMenuInterface.iHeight)  );
  rootDVDMenu.setAttribute ( MENU_RATIO, m_DVDMenuInterface.qsRatio );
  if ( m_DVDMenuInterface.iTitleset > -1 )
    rootDVDMenu.setAttribute ( MENU_TITLESET_NR,QString("%1").arg ( m_DVDMenuInterface.iTitleset ) );
  if ( m_DVDMenuInterface.iMenu > -1 )
    rootDVDMenu.setAttribute ( MENU_MENU_NR,    QString("%1").arg ( m_DVDMenuInterface.iMenu  ) );

  // We encapsulate the Previe, and the DVDMenu variables from each other so the reading is uniform.
  rootDVDMenu.appendChild ( dvdMenu );
  rootDVDMenu.appendChild ( dvdPreview );
  rootElement.appendChild ( rootDVDMenu );

  // Okay, the main Node is created, now all DVDAuthor related information belon under it.
  if (!m_DVDMenuInterface.writeProjectFile( dvdMenu ))
    return false;
  return m_pPixmapMenu->writeProjectFile( dvdPreview );
}

QString DVDMenu::name()
{
  QString qsSubMenuName;  //xxx
  qsSubMenuName = getDVDAuthor ( )->m_pTabWidgetMain->tabText ( getDVDAuthor ( )->m_pTabWidgetMain->indexOf(m_pDVDMenuTab) );			//xxx
  
  //to remove shortcut "&"  //xxx
  if ( qsSubMenuName.indexOf("&") != -1 ) //xxx
    qsSubMenuName.remove(qsSubMenuName.indexOf("&"),1);    //xxx
    
  //return getDVDAuthor ( )->m_pTabWidgetMain->tabLabel ( m_pDVDMenuTab );	//oooo
  //return getDVDAuthor ( )->m_pTabWidgetMain->accessibleName (  );		//xxx
  //return getDVDAuthor ( )->m_pTabWidgetMain->tabText ( getDVDAuthor ( )->m_pTabWidgetMain->indexOf(m_pDVDMenuTab) );			//xxxx
  //return getDVDAuthor ( )->m_pTabWidgetMain->tabText ( 0 );			//xxx
  return qsSubMenuName;  //xxx
}

void DVDMenu::setTabLabel(QString &qsNewLabel)
{
  bool bIsSubMenu = true;
  //getDVDAuthor ( )->m_pTabWidgetMain->setTabLabel ( m_pDVDMenuTab, qsNewLabel );	//ooo
  getDVDAuthor ( )->m_pTabWidgetMain->setTabText ( getDVDAuthor ( )->m_pTabWidgetMain->indexOf(m_pDVDMenuTab), qsNewLabel );	//xxx
  //getDVDAuthor ( )->m_pTabWidgetMain->setTabText ( 0, qsNewLabel );			//xxx
  // Okay since we can not change the name of the VMGM, this ought to be a sub menu
  // As such we should enable the user to rename, or delete it.
  // Note: Rename, Delete is done in the QDVDAuthor - class

  if (VMGM_LABEL == qsNewLabel)
    bIsSubMenu = false;
  m_pPixmapMenu->setIsSubMenu    (bIsSubMenu);
  m_DVDMenuInterface.bIsSubMenu = bIsSubMenu;
}

void DVDMenu::modifyColor ( uint iWhichColor, Rgba theColor, bool bCalledFromDialog )
{
  // printf ("DVDMenu::modifyColor <%d> <%d> <%s>\n", iWhichColor, bCalledFromDialog, (const char *)theColor.name());
  uint t;
  // This function is called from ColorToolbar click mouse event or from a ButtonObject ...
  // This function changes the colors according to the following rules :
  // There are 4 colors available :
  // -	1 = Transparent color
  // -	2 = Highlighted color
  // -	3 = Selected color
  // -	4 = -=> FREE <=-
  
  // 1)	The first color change is associated to the 4'th color but only if the change comes from the dialog
  //		After all if the user clicks on the ColorToolbar then he should know what he is doing.
  // 2)	The second color change will affect all Buttons (Selected or Highlighted)
  //		a)	If only one ButtonObject in menu, then don't care
  //		b)	Else ask user to confirm
  //			-NO  : return
  //			-YES :  Go through all ButtonObjects of this menu and change this color
  //		c)	Change ColorToolbar ...
  //		d)	Change all ButtonObjects colors ...
  static bool bFirstModification = true;
  QList <ButtonObject *> listButtons = getButtons();		//oxx

  if (bFirstModification && bCalledFromDialog)	{
    iWhichColor = MAX_MASK_COLORS - 1;
    //		theColor = m_DVDMenuInterface.pgcColors[MAX_MASK_COLORS - 1];
    bFirstModification = false;
  }
  else if (listButtons.count () > 1)	{
    // Double check if the user really wants to affect all ButtonObjects.
    // This is done when we have more then one ButtonObject int this Menu ...
    QString qsWhichColor ("Highlighted");
    if (iWhichColor == SELECTED_MASK)
      qsWhichColor = QString ("Selected");

    if (MessageBox::warning ( NULL, tr("Change all colors ?"), tr("This modification will change will affect all %1 colors.\n"
								  "Do you want to proceed ?").arg(qsWhichColor), QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape ) == QMessageBox::No)
      return;
  }

  // The next check is to ensure that we have a unique color.
  // In order to give the User control, I will simply bump up one color (red) by one bit
  for (t=0;t<MAX_MASK_COLORS;t++)	{
    if (m_DVDMenuInterface.pgcColors[t] == theColor)	{
      //if (theColor.Qt::red() > 0)	//ooo
      if (theColor.red() > 0)		//xxx
	//theColor.setRgb ( theColor.Qt::red ( ) - 1, theColor.Qt::green ( ), theColor.Qt::blue ( ), theColor.alpha ( ) );	//ooo
        theColor.setRgb ( theColor.red ( ) - 1, theColor.green ( ), theColor.blue ( ), theColor.alpha ( ) );			//xxx
      else
	//theColor.setRgb ( theColor.Qt::red ( ) + 1, theColor.Qt::green ( ), theColor.Qt::blue ( ), theColor.alpha ( ) );	//ooo
        theColor.setRgb ( theColor.red ( ) + 1, theColor.green ( ), theColor.blue ( ), theColor.alpha ( ) );			//xxx
    }
  }

  Rgba oldColor = m_DVDMenuInterface.pgcColors[iWhichColor];
  // Here we propagate the color change through all ButtonObjects.
  for (t=0;t<(uint)listButtons.count();t++)					//ox
    listButtons[t]->replaceColor ( theColor, oldColor );
  // Next we set the color in the DVDMenuInterface (The most important one)
  m_DVDMenuInterface.pgcColors[iWhichColor] = theColor;
  // And finally we set the color in the m_pColorToolbar of DVDAuthor
  getDVDAuthor ( )->setColor ( iWhichColor, theColor );
}

// Standard initialization Functions.
//void DVDMenu::CreateTab ( )					//xxx
void DVDMenu::CreateTab ( QTabWidget *pTabWidgetMain )	//ooo
{
  // The insertIndex is a bit tricky
  int iInsertIndex = pTabWidgetMain->count ( ) - 3;
  /*iInsertIndex += getDVDAuthor ( )->tabVisible ( QDVDAuthor::TabDVDLayout    ) ? 0 : 1;
  iInsertIndex += getDVDAuthor ( )->tabVisible ( QDVDAuthor::TabSubtitles    ) ? 0 : 1;
  iInsertIndex += getDVDAuthor ( )->tabVisible ( QDVDAuthor::TabDVDAuthorXml ) ? 0 : 1;*/	//oooo
  
  QPixmap backgroundPixmap(":/images/checkered.png");
  
  //m_pDVDMenuTab = new QWidget     ( pTabWidgetMain, "m_pDVDMenuTab" );		//ooo
  m_pDVDMenuTab = new QWidget     ( pTabWidgetMain );					//xxx
  //m_pDVDMenuTab = new QWidget     ( this );						//xxx
  m_pDVDMenuTab->setAccessibleName("m_pDVDMenuTab");					//xxx
  //m_pDVDMenuTab -> resize   ( 720, 480 );						//xxxx
  m_pDVDMenuTab -> setMinimumSize ( 760, 576 );						//xxxx
  //m_pTabLayout  = new Q3GridLayout ( m_pDVDMenuTab, 1, 1, 6, 2, "m_pTabLayout");	//ooo
  m_pTabLayout  = new QGridLayout ( m_pDVDMenuTab );					//xxx
  //m_pTabLayout->setHorizontalSpacing(6);
  //m_pLayout1    = new QGridLayout ( 0, 1, 1, 0, 6, "m_pLayout1");			//ooo
  m_pLayout1    = new QGridLayout ( );							//xxx
  //m_pLayout1    = new QGridLayout ( m_pDVDMenuTab );					//xxx
  
  // Here we get the available screen resolution (all screens)
  // We want to keep the old style for resolutions above 1024x768 
  // But for lower res we need to use a scrollview to be able to
  // use QDVDAuthor.
  int iScreenWidth, iScreenHeight, iScreens, t;
  bool bUseScrollView = false;
  //Q3ScrollView *m_pScrollView = NULL;		//ooo
  QScrollArea *m_pScrollView = NULL;		//xxx
  QDesktopWidget desktop;
  iScreenWidth = iScreenHeight = 0;
  iScreens = desktop.numScreens();
  for (t=0;t<iScreens;t++)	{
    iScreenWidth  += desktop.screenGeometry(t).width();
    iScreenHeight += desktop.screenGeometry(t).height();
  }
  
  //Qt::WindowFlags iFlags = Qt::WStaticContents | Qt::WNoAutoErase;
  
  //Q3Frame *pFrame = new Q3Frame ( m_pDVDMenuTab, "innerFrame" );		//ooo
  QFrame *pFrame = new QFrame ( m_pDVDMenuTab );				//xxx
  pFrame->setFrameShape  ( QLabel::Box );
  pFrame->setFrameShadow ( QLabel::Raised );
  pFrame->setLineWidth   ( 2 );
  //pFrame->setMargin      ( 0 );
  m_pLayout1->addWidget  ( pFrame, 0, 0 );
  
  //m_pLayout3  = new Q3GridLayout  ( pFrame, 1, 1, 4, -1  );	//ooo
  m_pLayout3  = new QGridLayout  ( pFrame );			//xxx
  char *pScroll = getenv ( "QDVD_SCROLL" );
  int iUseScrollView = 0;
  if ( pScroll ) {
    QString qsScroll ( "yes" );
    if ( qsScroll == pScroll )
      iUseScrollView = +1;
    else
      iUseScrollView = -1;
  }
  
  // check the views dimensions, and the environment (yes will force to use the scrollview, no will inhibit it)
  if ( (iUseScrollView == 1 )     ||
    ( ( (iScreenHeight <= 768) || (iScreenWidth <= 1024) ) && ( iUseScrollView != -1 ) ) )  {
    bUseScrollView = true;
    // Here we embed the MenuPreview into a ScrollView object ...
    //m_pScrollView = new Q3ScrollView ( pFrame,        "usingScrollView", iFlags );// Don't change the name. Used in ButtonPreview !	//ooo
    m_pScrollView = new QScrollArea ( pFrame );// Don't change the name. Used in ButtonPreview !					//xxx
    m_pScrollView->setAccessibleName ( "usingScrollView" ); //xxx
    //m_pPixmapMenu = new MenuPreview ( m_pScrollView, "m_pPixmapMenu",   iFlags );							//ooo
    m_pPixmapMenu = new MenuPreview ( m_pScrollView );											//xxx
    m_pPixmapMenu->setAccessibleName ( "m_pPixmapMenu" ); //xxx
  }
  else {
    //m_pPixmapMenu = new MenuPreview ( pFrame, "m_pPixmapMenu", iFlags   );	//ooo
    m_pPixmapMenu = new MenuPreview ( pFrame );					//xxx
    m_pPixmapMenu->setAccessibleName ( "m_pPixmapMenu" ); //xxx
  }
  
  ///*this->*/setAutoFillBackground     ( true );
  ///*this->*/setPalette(palette);
  
  //m_pScrollView = new QScrollArea ( pFrame,        "usingScrollView", iFlags );// Don't change the name. Used in ButtonPreview !	//ooo
  m_pScrollView = new QScrollArea ( pFrame );												//xxx
  m_pScrollView->setAccessibleName ( "usingScrollView" ); //xxx
  //m_pPixmapMenu = new MenuPreview ( m_pScrollView, "m_pPixmapMenu",   iFlags );	//ooo
  m_pPixmapMenu = new MenuPreview ( m_pScrollView );					//xxx
  m_pPixmapMenu->setAccessibleName ( "m_pPixmapMenu" ); //xxx
   //m_pPixmapMenu = new MenuPreview ( this );						//xxx
   //m_pPixmapMenu = new MenuPreview ( m_pDVDMenuTab );					//xxx
  m_pPixmapMenu->setLineWidth              ( 0 );
  m_pPixmapMenu->setMargin                 ( 0 );
  m_pPixmapMenu->setScaledContents         ( true );
  m_pPixmapMenu->setAutoFillBackground     ( true );
  //m_pPixmapMenu->setPaletteBackgroundPixmap( backgroundPixmap );	//ooo
  QPalette palette;                                                 //xxx
  //palette.setBrush(QPalette::Window, QBrush(backgroundPixmap));     //xxx
  palette.setBrush(m_pPixmapMenu->backgroundRole(), QBrush(backgroundPixmap));     //xxx
  m_pPixmapMenu->setPalette(palette);						        //xxx
  ///*this->*/setPalette(palette);
  //m_pPixmapMenu->setPixmap( backgroundPixmap );					//xxx
  m_pPixmapMenu->setFocusPolicy( Qt::StrongFocus         );
  m_pPixmapMenu->initMe        ( m_DVDMenuInterface.pgcColors );
  
  if ( bUseScrollView )     {
    //m_pScrollView->addChild ( m_pPixmapMenu );
    m_pPixmapMenu->resize   ( 720, 480 );
    m_pLayout3->addWidget   ( m_pScrollView, 0, 0 );
  }
  else
    m_pLayout3->addWidget   ( m_pPixmapMenu, 0, 0 );
  
  m_pTabLayout->addLayout   ( m_pLayout1, 0, 0 );
  
  // Insert the tab as the second from the back (xml/subpictures are last).
  //pTabWidgetMain->insertTab ( m_pDVDMenuTab, QString(VMGM_LABEL), iInsertIndex );	//ooo
  pTabWidgetMain->insertTab ( iInsertIndex, m_pDVDMenuTab, QString(VMGM_LABEL) );	//xxx
  //pTabWidgetMain->setCurrentPage ( iInsertIndex );					//ooo
  pTabWidgetMain->setCurrentIndex ( iInsertIndex );					//xxx

  connect( m_pPixmapMenu, SIGNAL(signalUpdateStructure()),     this, SLOT( slotUpdateStructure     ( ) ) );
  connect( m_pPixmapMenu, SIGNAL(signalRequestSourceFiles()),  this, SLOT( slotRequestSourceFiles  ( ) ) );
  connect( m_pPixmapMenu, SIGNAL(signalRequestSubMenus()),     this, SLOT( slotRequestSubMenus     ( ) ) );
  connect( m_pPixmapMenu, SIGNAL(signalDeleteMe  ( ) ),     getDVDAuthor ( ), SLOT( slotRemoveCurrentSubMenu ( ) ) );
  connect( m_pPixmapMenu, SIGNAL(signalMaskColorChanged  (Rgba, Rgba)),     this, SLOT(slotMaskColorChanged  ( Rgba, Rgba          ) ) );
  connect( m_pPixmapMenu, SIGNAL(signalCreatedMovieObject(MovieObject *, bool)),this, SLOT(slotCreatedMovieObject( MovieObject *, bool     ) ) );
}

void DVDMenu::slotCreatedMovieObject (MovieObject *pMovieObject, bool bRegisterToRender)
{
  if ( pMovieObject )
       pMovieObject->setDVDMenu ( this, bRegisterToRender );
}

void DVDMenu::slotMaskColorChanged( Rgba theColor, Rgba oldColor )
{
// printf ("DVDMenu::slotMaskColorChanged <%s> <%s>\n", (const char *)theColor.name(), (const char *)oldColor.name());
  // This function is called when the user changes the color of the
  // Selected, or the Highlighted Mask from within the ButtonDialog.
  uint t, iWhichColor = MAX_MASK_COLORS - 1;
  for ( t=0; t<MAX_MASK_COLORS; t++ )  {
    if ( m_DVDMenuInterface.pgcColors[t] == oldColor )	{
      iWhichColor = t;
      break;
    }
  }
  // The rest is handled in this function.
  modifyColor ( iWhichColor, theColor, true );
}

// This function is called from the [Add Background] - button.
void DVDMenu::readBackgroundFile ( QString &fileName )
{  
  Utils      theUtil;
  QTime      zeroTime;
  QString    qsExtensions;
  QFileInfo  backgroundInfo ( fileName );
  MediaInfo *pMediaInfo = getDVDAuthor ( )->getMediaInfo ( );
  qsExtensions = pMediaInfo->getExtensions ( true );

  //if (qsExtensions.find ( "."+backgroundInfo.extension ( FALSE  ) ) > -1 )  {     //ooo
  if (qsExtensions.indexOf ( "."+backgroundInfo.suffix ( ) ) > -1 )  {              //xxx 
    getInterface ( )->qsMovieFileName = fileName;
    // Okay let us get the first frame of the game as a screenshot ...
    pMediaInfo->setFileName ( fileName, true ); //, false );
    QImage theImage = pMediaInfo->getScreenshot( 0 );
    getInterface ( )->timeOffset   = zeroTime;
    getInterface ( )->timeDuration = zeroTime.addMSecs ( pMediaInfo->getLength ( ) );
    getInterface ( )->qsPause      = "0";  // no pause for MovieBackgrounds
    // Then we save the screenshot in the temp dir
    fileName = theUtil.getTempFile ( backgroundInfo.baseName ( ) )+QString( ".png" );
    theImage.save ( fileName, "PNG" );
  }
  else  {  // Not a movie file, then we should reset this ...
    getInterface ( )->qsMovieFileName = QString ( );
    if ( getInterface ( )->qsPause == "0" )
         getInterface ( )->qsPause = "inf";
  }
}

// This function is called from the [Add Background] - button.
void DVDMenu::slotAddBackground()
{ 
  QDVDAuthor *pDVDAuthor = getDVDAuthor ( );
  if ( ! pDVDAuthor )
    return;
  
  DVDMenu *pMenu = pDVDAuthor->getCurrentSubMenu ( );
  if ( pMenu && pMenu != this )  {
       pMenu->slotAddBackground ( );
       return;
  }
  
  if ( ! pDVDAuthor->checkForNewProject ( ) )
    return;
  
  QString qsImageFilter      = pDVDAuthor->getImageFilter ( );
  QString qsMovieFilter      = pDVDAuthor->getMovieFilter ( );
  MediaInfo *pMediaInfo      = pDVDAuthor->getMediaInfo   ( );
  void *pExistingMediaEngine = NULL;
  if ( pMediaInfo )
    pExistingMediaEngine  = pMediaInfo->getMediaEngine   ( );
/*  
  //  QString fileName = QFileDialog::getOpenFileName ( Global::qsCurrentPath, QString ("Images (") + qsImageFilter + QString (");;Movies (") + qsMovieFilter + QString (");;All (*)"));
  QString fileName = FilePreviewDialog::getOpenFileName ( pExistingMediaEngine, Global::qsCurrentPath, QString ("Images ( ") + qsImageFilter + QString (" );;Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));

  if (fileName.isNull())
    return;

  QFileInfo fileInfo  ( fileName );
  QString qsPath = fileInfo.path ( );
  Global::qsCurrentPath = qsPath;

  initBackground ( fileName );*/	//oooo
  
  //QString fileName = QFileDialog::getOpenFileName ( Global::qsCurrentPath, QString ("Images (") + qsImageFilter + QString (");;Movies (") + qsMovieFilter + QString (");;All (*)"));
  //QString fileName = FilePreviewDialog::getOpenFileName ( pExistingMediaEngine, Global::qsCurrentPath, QString ("Images ( ") + qsImageFilter + QString (" );;Movies ( ") + qsMovieFilter + QString (" );;All ( * )"));    //ooo
  
  QStringList fileName;

  //QFileDialog* pDialog = new PreviewFileDialog(this, "Open artwork", "", tr("Image Files (*.png *.jpg *.bmp *.tif);;"));                                                                                  //xxx
  QFileDialog* pDialog = new PreviewFileDialog(this, "Open artwork", Global::qsCurrentPath, QString ("Images ( ") + qsImageFilter + QString (" );;Movies ( ") + qsMovieFilter + QString (" );;All ( * )")); //xxx
  pDialog->setAcceptMode(QFileDialog::AcceptOpen);
  //mpOpenDialog->exec();  				//ooo
  
  //if ( pDialog->exec ( ) == QDialog::Rejected) return;    //xxx
  
  if ( pDialog->exec ( ) == QDialog::Accepted )
    fileName = pDialog->selectedFiles ( );
  
  //if (fileName.isNull())  //ooo
  if (fileName.isEmpty())   //xxx
    return;

  //QFileInfo fileInfo  ( fileName );           //ooo
  QFileInfo fileInfo  ( fileName[0] );          //xxx
  //QString qsPath = fileInfo.dirPath ( TRUE ); //ooo
  QString qsPath = fileInfo.path ( );           //xxx
  Global::qsCurrentPath = qsPath;
  
  //initBackground ( fileName );    //ooo
  initBackground ( fileName[0] );   //xxx
}

void DVDMenu::initBackground ( QString &fileName )
{ 
  if ( fileName.isEmpty ( ) )
    return;
  readBackgroundFile ( fileName );	//ooo
  addBackground      ( fileName );
}

void DVDMenu::addBackground ( QString &fileName, QString *pResizeAlgo, bool bForceDialog )
{
  bool bOpenDialog = bForceDialog;
  QString qsResizeAlgo ( QT_RESIZE_ALGO );
  QPixmap theBackground = QPixmap ( fileName );
  // Now we check if the image has already the right format for PAL / NTSC / SECAM
  int iHeight = theBackground.height ( );
  int iWidth  = theBackground.width  ( );
  int iFormat = getFormat (iWidth, iHeight);
  // And we might need those ...
  int iStretchType = m_DVDMenuInterface.iStretchType;
  QSize posOffset  = m_DVDMenuInterface.posOffset;
  QString qsRatio  = m_DVDMenuInterface.qsRatio;
  if ( (pResizeAlgo) && (iFormat == FORMAT_NONE) )	{
    iWidth        = m_DVDMenuInterface.iWidth;
    iHeight       = m_DVDMenuInterface.iHeight;
    iFormat       = getFormat ( iWidth,iHeight );
    qsResizeAlgo  = *pResizeAlgo;
    theBackground = QPixmap   ( 1, 1 );    
  }
  else if ( (iFormat == FORMAT_NONE) || ( bForceDialog ) ) {
    int iReturn = MessageBox::warning (NULL, tr ("Wrong IMAGE size (%1 x %2)").arg(iWidth).arg(iHeight),
			tr ("This image is neither PAL, nor NTSC. For a DVD the size has to fit either :\n"
			    "NTSC -> 720x480, 704x480, 352x480, 352x240 (29.97 Hz)\n"
			    "PAL    -> 720x576, 704x576, 352x576, 352x288 (25 Hz)\n"
			    "You need to resize this image sooner or later. Do you want to resize now ?"), 
			QMessageBox::Yes|QMessageBox::Default,  QMessageBox::No|QMessageBox::Escape);
    if (iReturn == QMessageBox::Yes)
      bOpenDialog = true;
  }
  else {
    m_DVDMenuInterface.backgroundImage = QPixmap  ( theBackground );
    //m_pPixmapMenu->setPixmap  (  (const  QPixmap &) theBackground );	//oooo
    //m_pPixmapMenu->set_Pixmap  (  (const  QPixmap &) theBackground );	//xxx
    m_pPixmapMenu->set_Pixmap  ( const_cast < QPixmap & > ( theBackground ) );	//xxx
  }
  if ( bOpenDialog )  {
    // save some memory while doing all the resize work etc.
    theBackground = QPixmap (1,1);
    //CResizeDialog resizeDialog;	//ooo
    DialogResize dialogResize;		//xxx
    dialogResize.m_posOffset    = posOffset;
    dialogResize.m_iFormat      = iFormat;
    dialogResize.m_iStretchType = iStretchType;
    dialogResize.setFilename(fileName);
    dialogResize.m_pComboAspect->setCurrentText ( qsRatio );
    if (dialogResize.exec () == QDialog::Rejected)
      return;
    iWidth       = dialogResize.m_pEditWidth ->text().toInt();
    iHeight      = dialogResize.m_pEditHeight->text().toInt();
    qsRatio      = dialogResize.m_pComboAspect->currentText();
    iFormat      = dialogResize.m_iFormat;
    iStretchType = dialogResize.m_iStretchType;
    posOffset    = dialogResize.m_posOffset;

    if (iWidth < 1)
      iWidth = 720;
    if (iHeight < 1)
      iHeight = 480;
    qsResizeAlgo = dialogResize.comboResizeAlgorithm->currentText();
    theBackground = *dialogResize.m_pPixmapPreview->pixmap ();
    m_DVDMenuInterface.backgroundImage = theBackground;

    //QPalette palette;							                //xxx
    //palette.setBrush(QPalette::Window, QBrush(theBackground));	//xxx
    //m_pPixmapMenu->setPalette(palette);				             //xxx
    
    //m_pPixmapMenu->setPixmap ( (const QPixmap &) theBackground);	//oooo
    //m_pPixmapMenu->set_Pixmap ( (const QPixmap &) theBackground);	//xxx
    m_pPixmapMenu->set_Pixmap ( const_cast< QPixmap & > ( theBackground ) );	//xxx
  }

  //getUndoBuffer ( )->push ( new DVDMenuUndoObject(DVDMenuUndoObject::NEW_BACKGROUND, (QPixmap &) *m_pPixmapMenu->paletteBackgroundPixmap(), &m_DVDMenuInterface));	//ooo
  getUndoBuffer ( )->push ( new DVDMenuUndoObject(DVDMenuUndoObject::NEW_BACKGROUND, (QPixmap &) *m_pPixmapMenu->pixmap(), &m_DVDMenuInterface));	//xxx
  // This has to be done AFTER the undo-object has been created.
  //m_pPixmapMenu->setPixmap ( (const QPixmap &) theBackground);
  // And finally we set the interface (to store the dvd project file in DVDAuthor class)
  m_DVDMenuInterface.qsBackgroundFileName = fileName;
  m_DVDMenuInterface.iFormat              = iFormat;
  m_DVDMenuInterface.iWidth               = iWidth;
  m_DVDMenuInterface.iHeight              = iHeight;
  m_DVDMenuInterface.qsRatio              = qsRatio;
  m_DVDMenuInterface.qsResizeAlgo         = qsResizeAlgo;
  m_DVDMenuInterface.iStretchType         = iStretchType;
  m_DVDMenuInterface.posOffset            = posOffset;
  m_DVDMenuInterface.bModified            = true;

  // Okay we should also add something to the QListView
  if (!m_selectedMask.isNull())
    setMask (m_DVDMenuInterface.qsSelectedMaskFileName, SELECTED_MASK );
  if (!m_highlightedMask.isNull())
    setMask (m_DVDMenuInterface.qsHighlightedMaskFileName, HIGHLIGHTED_MASK );
  
  refresh ();		//ooo
  
    /*this->*///setAutoFillBackground     ( true );
    //QPixmap backgroundPixmap(":/images/checkered.png");
  //QPalette palette;
    //palette.setBrush(QPalette::Window, QBrush(QImage(fileName)));
    //palette.setBrush(QPalette::Window, QBrush(QPixmap(fileName)));
  //palette.setBrush(QPalette::Window, QBrush(theBackground));
  ///*this->*/setPalette(palette);
  //m_pPixmapMenu->setPalette(palette);   //xxx
}

void DVDMenu::refresh()
{
  slotUpdateStructure ( );

  if ( m_pPixmapMenu )	{
    m_pPixmapMenu->update       ( );  //oooo
    m_pPixmapMenu->updatePixmap ( );
  }	//ooo  
}

void DVDMenu::slotAddSound ( )
{
  QDVDAuthor *pDVDAuthor = getDVDAuthor ( );
  if ( ! pDVDAuthor )
    return;

  DVDMenu *pMenu = pDVDAuthor->getCurrentSubMenu ( );
  if ( pMenu && pMenu != this )  {
       pMenu->slotAddSound ( );
       return;
  }

  if ( ! pDVDAuthor->checkForNewProject ( ) )
    return;

  uint t, iCounter=0;
  SourceFileEntry *pEntry;
  for (t=0;t<(uint)m_DVDMenuInterface.listSoundEntries.count ();t++)	{	//ox
    pEntry = pDVDAuthor->getSourceEntryByDisplayName ( m_DVDMenuInterface.listSoundEntries[t]->qsFileName );
    if ( ! pEntry )
      break;
    iCounter += pEntry->listFileInfos.count();
  }
  if (iCounter > 7)	{
    MessageBox::warning (NULL, tr ("Too many sound files"),
			 tr ("A DVD can at a maximum take 8 soundtracks.\n"), QMessageBox::Ok,  QMessageBox::NoButton);
    return;
  }

  QStringList listAddMovies, listFileNames;
  QString   qsFileName, qsExtension;
  QFileInfo fileInfo;
  QString   qsMovieFilter    = pDVDAuthor->getMovieFilter ( );
  QString   qsFilter         = pDVDAuthor->getSoundFilter ( );
  QString   qsDialogFilter   = tr ("Sound Files (") + qsFilter + QString(")");

  listFileNames = FilePreviewDialog::getOpenFileNames ( Global::qsCurrentPath, QString::null, m_pPixmapMenu, "SoundFilesDialog", tr( "Select sound files" ), &qsDialogFilter );

  if ( listFileNames.count ( ) < 1)
    return;

  // Just making sure we did select Audio files (simple extensions based selection)
  for ( t=0; t<(uint)listFileNames.count ( ); t++ ) {		//ox
    qsFileName = listFileNames[t];
    fileInfo.setFile ( qsFileName );
    //qsExtension = fileInfo.extension ( FALSE ).lower ( ); //ooo
    qsExtension = fileInfo.suffix ( ).toLower ( );          //xxx
    //if ( qsMovieFilter.find ( qsExtension ) > -1 ) {  //ooo
    if ( qsMovieFilter.indexOf ( qsExtension ) > -1 ) { //xxx
      listAddMovies.append  ( qsFileName  );
      //listFileNames.remove  ( qsFileName  );  //ooo
      listFileNames.removeOne  ( qsFileName  ); //xxx
    }
  }

  if ( iCounter+listFileNames.count ( ) > 7 )	{
    MessageBox::warning (NULL, tr ("Too many sound files"),
			 tr ("A DVD can at a maximum take 8 soundtracks.\nI will only use the first %1 files.").arg(8-iCounter), QMessageBox::Ok,  QMessageBox::NoButton);
    QStringList listFileNames2 = listFileNames;
    listFileNames.clear ();
    for (t=0;t<8-iCounter;t++)
      listFileNames.append (listFileNames2[t]);
  }

  fileInfo.setFile ( listFileNames[0] );
  //QString qsPath = fileInfo.dirPath ( TRUE ); //ooo
  QString qsPath = fileInfo.path ( );           //xxx
  Global::qsCurrentPath = qsPath;

  if ( listAddMovies.count  ( ) > 0 )
    pDVDAuthor->addMovieList( listAddMovies );

  if ( listFileNames.count  ( ) > 0 )  {
    pDVDAuthor->addSound    ( listFileNames );
    // After we added the sound to the SourceToolBar I would like to add this here ...
    // The last one is the one just created.
    SourceFileEntry *pNewEntry = pDVDAuthor->sourceFileEntry ( pDVDAuthor->sourceFileCount ( ) -1 );
    if ( ! pNewEntry )
      return;

    int iFreeAudioNumber = 0;
    for ( t=0; t<(uint)m_DVDMenuInterface.listSoundEntries.count ( ); t++ )  {		//ox
      if ( iFreeAudioNumber == m_DVDMenuInterface.listSoundEntries[t]->iAudioNumber ) {
        iFreeAudioNumber ++;
        t = 0;
      }
    }
    if ( iFreeAudioNumber > 7 ) {
      // Error
      pDVDAuthor->removeSourceFileEntry ( pNewEntry );
      return;
    }

    Audio *pNewAudio = new Audio;
    pNewAudio->iAudioNumber = iFreeAudioNumber;
    pNewAudio->qsIso639     = Global::qsDefaultLanguage;
    pNewAudio->qsFileName   = pNewEntry->qsDisplayName;
    m_DVDMenuInterface.listSoundEntries.append ( pNewAudio );

    // Lastly we check if the duration is right
    bool bCheckMovies, bCheckAudio;
    bCheckMovies = false;
    bCheckAudio  = true;
    QString qsDifference = checkDuration ( bCheckMovies, bCheckAudio );
    if ( bCheckAudio )  {
      // Okay we should display waht has to be said
      QSize size ( 750, 300 );
      qsDifference += tr ( "&nbsp;<p><b>Do you want to adjust the length of the menu now ?</b><p>" );
      int iRet = MessageBox::html ( getMenuPreview ( ), tr ( "Found issues." ), qsDifference, size, QMessageBox::Yes, QMessageBox::No );
      if ( iRet == QMessageBox::Yes )
        slotEditTimeline ( );
    }
  }
  slotUpdateStructure ( );
}

void DVDMenu::removeSourceEntry ( QString &qsRemoveName )
{
  uint t, i, iRemoved;
  QStringList listButtonAction, listMultiple;
  QString qsTemp;
  QList<ButtonObject *> listButtons = getButtons ( );
  ButtonObject *pButton;
  iRemoved = 0;
  // the first step is to remove all entries from the Interface (which
  // holds the list of the sound files).
  for (t=0;t<(uint)m_DVDMenuInterface.listSoundEntries.count();t++)	{
    Audio *pAudio = m_DVDMenuInterface.listSoundEntries[t];
    qsTemp = pAudio->qsFileName;
    if ( qsTemp == qsRemoveName ) {
      //iRemoved += m_DVDMenuInterface.listSoundEntries.remove ( pAudio );	//ooo
      iRemoved += m_DVDMenuInterface.listSoundEntries.removeOne ( pAudio );	//xxx
      delete pAudio;
    }
  }

  // The second step is to check all button objects...
  qsTemp = QString ( );
  for ( t=0; t<(uint)listButtons.count ( );t++ )  {
    pButton = listButtons[t];
    if ( ! pButton )
      continue;
    listMultiple = pButton->getPlayMultipleList ( );
    // here we dwell into the buttons supposed action ...
    //listButtonAction = QStringList::split ( STRING_SEPARATOR, pButton->action ( ) );	//ooo
    listButtonAction = pButton->action ( ).split ( STRING_SEPARATOR );			//xxx
    if ( ( listButtonAction.count ( ) == 3 ) || ( listButtonAction.count ( ) == 4 ) )  {
      // Here we got ourself a button connected to something ...
      if ( listButtonAction[1] == qsRemoveName )  {
	pButton->setAction ( qsTemp );
	pButton->setSourceFileEntry ( NULL );
	iRemoved ++;
      }
    }
    if ( listMultiple.count ( ) > 0 ) { 
      QStringList listTemp = listMultiple;
      // here we dwell into buttons which play multiple files
      for ( i=0; i<(uint)listTemp.count ( ); i++ ) {
	//listButtonAction = QStringList::split ( STRING_SEPARATOR, listTemp[i] );	//ooo
	listButtonAction = listTemp[i].split ( STRING_SEPARATOR );			//xxx
	// Here we got ourself a button connected to something ...
	if ( listButtonAction[1] == qsRemoveName )  {
	  //listMultiple.remove ( listTemp[i] );	//ooo
	  listMultiple.removeOne ( listTemp[i] );	//xxx
	  iRemoved ++;
	}
      } // end for loop
      if ( iRemoved > 0 ) {
	// Set the first PlayMultiple entry to also be the buttonAction
	if ( listMultiple.count ( ) > 0 ) {
	  pButton->setAction ( listMultiple[0] );
	  pButton->setSourceFileEntry ( getEntryFromButtonAction ( pButton->action ( ) ) );
	}
	// if only one entry remains, then we have a simple buton left.
	if ( listMultiple.count ( ) == 1 )
	  listMultiple.clear ( );
	pButton->setPlayMultipleList  ( listMultiple );
      }
    } // end else
  } // end for listButtons
  
  if ( iRemoved > 0 )
    slotUpdateStructure ( );
}

void DVDMenu::removeSourceFileEntry (SourceFileEntry *pSourceFileEntry)
{
  getDVDAuthor ( )->removeSourceFileEntry ( pSourceFileEntry );
  removeSourceEntry ( pSourceFileEntry->qsDisplayName );
}

void DVDMenu::appendSourceFileEntry ( SourceFileEntry *pSourceFileEntry )
{
  int t, iFreeAudioNumber = 0;
  for ( t=0; t<(int)m_DVDMenuInterface.listSoundEntries.count ( ); t++ ) {
    if ( iFreeAudioNumber == m_DVDMenuInterface.listSoundEntries[t]->iAudioNumber ) {
      iFreeAudioNumber ++;
      t = 0;
    }
  }
  if ( iFreeAudioNumber > 7 ) {
    // Error 
    return;
  }
  getDVDAuthor ( )->appendSourceFileEntry ( pSourceFileEntry );

  Audio *pAudio = new Audio;
  pAudio->iAudioNumber = iFreeAudioNumber;
  pAudio->qsIso639     = Global::qsDefaultLanguage;
  pAudio->qsFileName   = pSourceFileEntry->qsDisplayName;
  m_DVDMenuInterface.listSoundEntries.append ( pAudio );
  slotUpdateStructure ( );
}

bool DVDMenu::isSourceEntryUsed ( QString &qsInquireEntry )
{
  uint t, i;
  QString qsAction;
  QStringList listButtonAction;
  // This function is called when the user wants to delete a SourceFileEntry
  // it returns true if the entry is in use.

  // Two steps : The first is to check all entries in the DVDMenu->interface
  CDVDMenuInterface *pInterface = getInterface();
  for (t=0;t<(uint)pInterface->listSoundEntries.count();t++)	{	//ox
    if (pInterface->listSoundEntries[t]->qsFileName == qsInquireEntry)
      return true;
  }

  // The second step is to check all button objects...
  QStringList listMultiple;
  QList<ButtonObject *> listButtons = getButtons ( );		//oxx
  for ( t=0; t<(uint)listButtons.count ( ); t++ )  {		//ox
    listMultiple = listButtons[t]->getPlayMultipleList ( );
    if ( listMultiple.count ( ) < 1 ) {
      // here we dwell into the buttons supposed action ...
      //listButtonAction = QStringList::split ( STRING_SEPARATOR,  listButtons[t]->action ( ) );	//ooo
      listButtonAction = listButtons[t]->action ( ).split ( STRING_SEPARATOR );				//xxx
      if ( ( listButtonAction.count ( ) == 3 ) || 
	   ( listButtonAction.count ( ) == 4 ) ) {
	// Here we got ourself a button connected to something ...
	if ( ( listButtonAction[1] == qsInquireEntry ) ||
	     ( listButtonAction[2] == qsInquireEntry ) )
	  return true;
      }
    }
    else {
      // here we dwell into buttons which play multiple files
      for ( i=0; i<(uint)listMultiple.count ( ); i++ ) {	//ox
	//listButtonAction = QStringList::split ( STRING_SEPARATOR, listMultiple[i] );	//ooo
	listButtonAction = listMultiple[i].split ( STRING_SEPARATOR );			//xxx
	// Here we got ourself a button connected to something ...
	if ( ( listButtonAction[1] == qsInquireEntry ) || 
	     ( listButtonAction[2] == qsInquireEntry ) )
	  return true;
      } // end for loop
    } // end else
  }
  return false;
}

void DVDMenu::replaceSourceDisplayName (QString &qsOriginalDisplayName, QString &qsNewDisplayName)
{
  uint t, i;
  QString qsAction;
  // This function is called when the user changed the DisplayName of a SourceFileEntry
  // DialogMovie::accept()->SourceToolBar::slotEditMovie->QDVDAuthor::replaceSourceDisplayName()

  // Two steps : The first is to change all entries from the Interface ...
  CDVDMenuInterface *pInterface = getInterface();
  for (t=0;t<(uint)pInterface->listSoundEntries.count();t++)	{	//ox
    if (pInterface->listSoundEntries[t]->qsFileName == qsOriginalDisplayName)
      pInterface->listSoundEntries[t]->qsFileName = qsNewDisplayName;
  }

  // The second step is to go through all buttons and see what they are up to ...
  QList<ButtonObject *> listButtons = getButtons();		//oxx
  QStringList listButtonAction, listMultiple;
  for ( t=0; t<(uint)listButtons.count ( ); t++ )  {		//ox
    listMultiple = listButtons[t]->getPlayMultipleList ( );
    // here we dwell into the buttons supposed action ...
    //listButtonAction = QStringList::split(STRING_SEPARATOR,  listButtons[t]->action());   //ooo
    listButtonAction = listButtons[t]->action().split(STRING_SEPARATOR);                    //xxx
    if ( ( listButtonAction.count ( ) == 3 ) || 
	 ( listButtonAction.count ( ) == 4 ) )  {
      // Here we got ourself a button connected to something ...
      if ( listButtonAction[1] == qsOriginalDisplayName )  {
	   listButtonAction[1]  = qsNewDisplayName;
	qsAction = listButtonAction.join ( STRING_SEPARATOR );
	listButtons[t]->setAction ( qsAction );
      }
    }
    if ( listMultiple.count ( ) > 0 ) {
      // here we dwell into buttons which play multiple files
      for ( i=0; i<(uint)listMultiple.count ( ); i++ ) {	//ox
	//listButtonAction = QStringList::split ( STRING_SEPARATOR, listMultiple[i] ); //ooo
    listButtonAction = listMultiple[i].split ( STRING_SEPARATOR );                 //xxx
	if ( ( listButtonAction.count ( ) == 3 ) || 
	     ( listButtonAction.count ( ) == 4 ) )  {
	  // Here we got ourself a button connected to something ...
	  if ( listButtonAction[1] == qsOriginalDisplayName )  {
	       listButtonAction[1]  = qsNewDisplayName;
	    qsAction = listButtonAction.join(STRING_SEPARATOR);
	    listButtons[t]->setAction (qsAction);
	  }
	}
      }
    }
  }
}

void DVDMenu::replaceMenuName ( QString &qsOriginalMenuName, QString &qsNewMenuName )
{  
  uint t;
  QString qsAction;
  // This function is called when the user changed the MenuName.
  // Here we'll check if any button is currently assigned to this menu and change it if required.

  QList<ButtonObject *> listButtons = getButtons ( );		//oxx
  QStringList listButtonAction, listMultiple;
  for ( t=0; t<(uint)listButtons.count ( ); t++ )  {		//ox
    // here we dwell into the buttons supposed action ...
    //listButtonAction = QStringList::split ( STRING_SEPARATOR, listButtons[t]->action ( ) );	//ooo
    listButtonAction = listButtons[t]->action ( ).split ( STRING_SEPARATOR );			//xxx
    if ( listButtonAction.count ( ) == 2 )  {
      // Here we got ourself a button connected to something ...
      if ( listButtonAction[1] == qsOriginalMenuName)	{
	   listButtonAction[1]  = qsNewMenuName;
	   qsAction = listButtonAction.join ( STRING_SEPARATOR );
	   listButtons[t]->setAction ( qsAction );
      }
    }

    // Note: for ListPlayMultiple only the last entry could possibly be a menu
    listMultiple = listButtons[t]->getPlayMultipleList ( );
    if ( listMultiple.count ( ) > 0 ) {
      qsAction = listMultiple.last ( );
      //listButtonAction = QStringList::split ( STRING_SEPARATOR, qsAction );	//ooo
      listButtonAction = qsAction.split ( STRING_SEPARATOR );			//xxx
      if ( listButtonAction.count ( ) == 2 ) {
	// Here we got ourself a button connected to something ...
	if ( listButtonAction[1] == qsOriginalMenuName )  {
	  listButtonAction[1]  = qsNewMenuName;
	  //listMultiple.remove ( qsAction );	//ooo
	  listMultiple.removeOne ( qsAction );	//xxx
	  qsAction = listButtonAction.join ( STRING_SEPARATOR );
	  listMultiple.append ( qsAction );
	  listButtons[t]->setPlayMultipleList ( listMultiple );
	}
      }
    }
  }
}

QString DVDMenu::checkButtonsOverlapping   ( )
{
  uint    t, i;
  QString qsReturn;
  QRect   theRect;
  ButtonObject *pButton1, *pButton2;

  QList<ButtonObject *>listOfButtons = getButtons ( );		//oxx
  // None or only one button, return empty
  if ( listOfButtons.count() < 2 )
    return qsReturn;

  for (t=0;t<(uint)listOfButtons.count();t++) {		//ox
    pButton1 = listOfButtons[t];
    if ( ! pButton1 )
      continue;
    theRect  = pButton1->boundingRect ( );
    for (i=t+1;i<(uint)listOfButtons.count ();i++) {	//ox
      pButton2 = listOfButtons[i];
      if ( ! pButton2 ) 
	continue;
      if ( theRect.intersects ( pButton2->boundingRect ( ) ) ) {
	// create the header info ...
	if ( qsReturn.isEmpty ( ) )
	  qsReturn = QString ( "<BR><B>[%1]</B><BR>" ).arg ( name ( ) );
	qsReturn  += tr ( "Button <B>[%1]</B> overlapping <B>[%2]</B><BR>" ).arg ( pButton1->name ( ) ).arg ( pButton2->name ( ) );
      }
    }
  }

  return qsReturn;
}

void DVDMenu::setMask (QString & fileName, uint iWhichMask)
{
	QImageHelper theMask (fileName, 0);
	// Sanity check ...
	uint iColorCount = theMask.countColors (); //theMask);

	if ( iColorCount > MAX_MASK_COLORS )	{
		int iReturn = MessageBox::warning (NULL, tr ("Image has too many colors"),
			tr ("This mask has too many colors. Only 4 colors are allowed for the mask.\n\n"
			"Do you want to decrease the number of colors to 4 ?"
			    ), QMessageBox::Yes|QMessageBox::Default,  QMessageBox::No|QMessageBox::Escape);
			if (iReturn == QMessageBox::Yes)
				theMask.dither (MAX_MASK_COLORS);
	}
	//

	// Dialog to choose the transparent color from ...

	//

	// Here we ensure that there is a background Image present.
	if (m_DVDMenuInterface.backgroundImage.isNull())	{
		displayMask(iWhichMask);
		slotUpdateStructure();
		return;
	}

	//QImage theBackground = m_DVDMenuInterface.backgroundImage.convertToImage();	//ooo
	QImage theBackground = m_DVDMenuInterface.backgroundImage.toImage();		//xxx
	int x, y, xMax, yMax;
	QRgb thePixel, theMaskColor = 0x00000000;
	// Here is the algol, to filter out the transparent background color of the mask
	// and overlay the remaining pixel to the background.

	// This test is to ensure that the smallest dimensions are taken from both images.
	xMax = (theMask.width () < theBackground.width ()) ? theMask.width () : theBackground.width ();
	yMax = (theMask.height() < theBackground.height()) ? theMask.height() : theBackground.height();

	theMaskColor = theMask.getMaskColor ();

	for (y=0;y<yMax;y++)	{
		for (x=0;x<xMax;x++)	{
			thePixel = theMask.pixel(x, y);
//			if ( (thePixel != theMaskColor) && (thePixel != 0xff000000) )
			if (thePixel != theMaskColor)
				theBackground.setPixel (x, y, thePixel);
		}
	}

	if (iWhichMask == SELECTED_MASK)	{
		m_selectedMask.convertFromImage ( theBackground );
		// and then we should keep the mask itself as well (remember we might have changed the color depth).
		//m_DVDMenuInterface.selectedMask = theMask;		//ooo
		m_DVDMenuInterface.selectedMask = ( QPixmap& ) theMask;	//xxx
	}
	else if (iWhichMask == HIGHLIGHTED_MASK)	{
		m_highlightedMask.convertFromImage ( theBackground );
		//m_DVDMenuInterface.highlightedMask = theMask;			//ooo
		m_DVDMenuInterface.highlightedMask = ( QPixmap& ) theMask;	//xxx
	}

	displayMask (iWhichMask);
	slotUpdateStructure ();
}

void DVDMenu::createMask(QString &qsFileName, int iWhichMask, bool bForceDrawing)
{
  // Called from QDVDAuthor::createDVD()
  uint t, i, iWidth, iHeight;
  // This function is called from QDVDAuthor::createMenuXml() and will first create the
  // 4 colored mask and then store it to a file.
  QPixmap theMask;
  QPainter thePainter;
  ButtonObject *pButton;

  iWidth  = 720;
  iHeight = 480;
  if ( ! m_DVDMenuInterface.backgroundImage.isNull() ) {
    iWidth  = m_DVDMenuInterface.backgroundImage.width ();
    iHeight = m_DVDMenuInterface.backgroundImage.height();
  }

  // Here we draw the appropriate mask.
  //theMask  = QPixmap ( iWidth, iHeight, QPixmap::defaultDepth (), QPixmap::NoOptim );		//ooo
  theMask  = QPixmap ( iWidth, iHeight );							//xxx
  theMask.fill ( m_DVDMenuInterface.pgcColors[0].rgb ( ) );

  // Check if we draw the Background image
  if (iWhichMask == BACKGROUND_IMG)	{
    if ( ! m_DVDMenuInterface.backgroundImage.isNull ( ) )
      theMask = QPixmap ( m_DVDMenuInterface.backgroundImage );

    thePainter.begin ( &theMask );
    // Note, when we have a MovieMenu we want a plain background
    if (  ( ! isMovieMenu ( ) ) || ( bForceDrawing )  )  {
      for ( t=0; t<m_pPixmapMenu->objectCount ( );t++ )
        m_pPixmapMenu->menuObject ( t )->drawContents ( &thePainter );
    }
    thePainter.end ( );
 
    theMask.save   ( qsFileName, "JPEG", 100 );
    return;
  }

  QList <ButtonObject *> listButtons = getButtons();	//oxx
  QRgb colors[4];
  //colors[0] = 0x00FEFFFE; // transparency color.
  colors[0] = 0xFFFFFFFF;
  colors[1] = 0xFF0000FF;
  colors[2] = 0xFF00FF00;
  colors[3] = 0xFFFF0000;
  // Here we draw the Selected or the highlighted mask
  thePainter.begin ( &theMask );

  for ( t=0;t<(uint)listButtons.count ();t++) {		//ox
    // Since we experienced problems with antialiasing text, we will temporarly change the button colors
    // to be very close together. This way no antialiasing should be performed (QFont::NoAntialias did not work)
    pButton = listButtons[t];
    pButton->replaceColor ( colors[0], m_DVDMenuInterface.pgcColors[0] );
    pButton->replaceColor ( colors[1], m_DVDMenuInterface.pgcColors[1] );
    pButton->replaceColor ( colors[2], m_DVDMenuInterface.pgcColors[2] );
    pButton->replaceColor ( colors[3], m_DVDMenuInterface.pgcColors[3] );
    pButton->drawContents ( &thePainter, iWhichMask );
    pButton->replaceColor ( m_DVDMenuInterface.pgcColors[0], colors[0] );
    pButton->replaceColor ( m_DVDMenuInterface.pgcColors[1], colors[1] );
    pButton->replaceColor ( m_DVDMenuInterface.pgcColors[2], colors[2] );
    pButton->replaceColor ( m_DVDMenuInterface.pgcColors[3], colors[3] );
  }

  thePainter.end ( );
  Utils theUtils;

  //theMask.save ( "/tmp/t0.png", "PNG", 100 );
  //QImage theImage = theMask.convertToImage ( );	//ooo
  QImage theImage = theMask.toImage ( );		//xxx

  //theImage.save ( "/tmp/t1.png", "PNG", 100 );
  theUtils.reduceColors ( theImage, 4, (QRgb *)&colors );
  //theImage.save ( "/tmp/t2.png", "PNG", 100 );
  //theImage.setAlphaBuffer ( true );			//oooo
  theImage.createAlphaMask ( Qt::AutoColor );		//xxx
  //theImage.save ( "/tmp/t3.png", "PNG", 100 );
  // Finally we replace the color entries with the original colors ...
  for (t=0;t<MAX_MASK_COLORS;t++) {
    for (i=t;i<MAX_MASK_COLORS;i++) {
      if ( theImage.color ( i ) == theImage.color ( t ) )
	theImage.setColor ( t, (m_DVDMenuInterface.pgcColors[t].rgb ( ) ) );
      //theImage.setColor ( t, (m_DVDMenuInterface.pgcColors[t].rgb ( ) ) | 0xFF000000 );
    }
  }

  theImage.save ( qsFileName, "PNG", 100 );
}

float DVDMenu::fps ()
{
	float fFPS;
	int  iFormat = getFormat (getInterface()->iWidth, getInterface()->iHeight);

	// The first thing we specify is the video settings ...
	fFPS = 29.97f;		// NTSC
	if (iFormat > 4)
		fFPS = 25.0f;	// PAL
	return fFPS;
}

long DVDMenu::framesToRender ( )
{  
	float fDeltaSeconds, fFormatFactor;
	long iNumberOfTotalFrames;

	// The first thing we specify is the video settings ...
	fFormatFactor = fps ( );
	//fDeltaSeconds = QTime().msecsTo ( getInterface ( )->timeDuration ) / 1000.0; //  10.0f;  //ooo
    QTime zeroTime(0,0,0,0); //xxx
    fDeltaSeconds = zeroTime.msecsTo ( getInterface ( )->timeDuration ) / 1000.0;   //xxx

	iNumberOfTotalFrames = (long)(fDeltaSeconds * fFormatFactor);

	return iNumberOfTotalFrames;
}

void DVDMenu::renderMenu()
{  
  // This function will take the already extracted images from the menu background
  // and render the final frames (Think movieObjects)

  // Note: This function is usually called from StartMenuRender::renderDVDMenu ()
  //       though nothing is preventing you from calling this function directly.

  QDir theDir;
  QPixmap thePixmap;
  QString qsFullPath;
  long iNumberOfTotalFrames = framesToRender ();

  qsFullPath = QString ("%1/%2/%3/background/").arg(Global::qsTempPath) .arg(Global::qsProjectName).arg(getInterface()->qsMenuName);

  // Okay here we get the Directory list of the extracted image files from the directory
  theDir.setPath(qsFullPath);
  //theDir.setNameFilter ("0*.jpg");				//ooo
  theDir.setNameFilters ( QStringList( QString("0*.jpg") ) );	//xxx
  if (theDir.count () == 0)	{
    // Okay nothing extracted, let us assume we have a backgournd image and NOT a movie.
    theDir.cd ("..");
    //theDir.setNameFilter ("*"BACKGROUND_NAME); //*background.jpg");				//ooo
    theDir.setNameFilters ( QStringList( QString( "*"BACKGROUND_NAME ) ) ); //*background.jpg");	//xxx
  }

  m_iTotalFramesToRender = iNumberOfTotalFrames;
  m_iCurrentFrameToRender = 0;

  //m_pDialogRenderPreview = new uiDialogRenderPreview ( getDVDAuthor ( ) );	//ooo
  m_pDialogRenderPreview = new DialogRenderPreview ( getDVDAuthor ( ) );	//xxx
  m_pDialogRenderPreview->show ();
  thePixmap = getMenuPreview()->renderMenu (getInterface(), 1, 2);

  m_pDialogRenderPreview->m_pLabelPreview->setFixedSize (thePixmap.size());
  m_pDialogRenderPreview->setProgressData (&thePixmap, 1, m_iTotalFramesToRender);
  //m_pDialogRenderPreview->setCaption (QString ("Render Menu : %1").arg(name()));	//oooo
  m_pDialogRenderPreview->setWindowTitle (QString ("Render Menu : %1").arg(name()));	//xxx

  // the last step before rendering the Menu is to make sure that all the clips have been extracted.
  QCursor myCursor (Qt::WaitCursor);
  QApplication::setOverrideCursor (myCursor);

  // In order to extract all infos we need at least one thread ...
  int iMaxThreads = Global::iMaxRunningThreads;
  if ( Global::iMaxRunningThreads < 1 )
    Global::iMaxRunningThreads = 1;
  QList<MovieObject *>  listMovieObjects = getMovieObjects();	//oxx
  for (uint t=0;t<(uint)listMovieObjects.count ( ); t++)		//ox
    listMovieObjects[t]->startConverting ( false );
  // at this point we are sure that all required MovieObjects for this menu are in the Queue.
  // Next is to wait for all clips to be converted before we continue.
  StartMenuRender::waitForExtraction (this);

  // This function will now create the data files for the animated objects 
  // I.e. we will run the script, which the user has provided and load the 
  //      output into memory of the MenuObject
  getMenuPreview()->createAnimationData (getInterface()->qsMenuName, framesToRender ());	//oooo

  // And here we restore the number of threads again to its previous value.
  Global::iMaxRunningThreads = iMaxThreads;
  QApplication::restoreOverrideCursor ();

  // And finally we can render the menu
  QTimer::singleShot (0, this, SLOT(slotRenderFrame()));
/* This code would be executed to keep everything in one function but then we could not update the progress Dialog !
  for (t=0;t<iNumberOfTotalFrames;t++)	{
    thePixmap = getMenuPreview()->renderMenu (getInterface()->qsMenuName, t, iNumberOfTotalFrames);
    qsFileName.sprintf ("%s%08d.png", (const char *)qsFullPath, t);

    // Okay we have the newly rendered background image ... display some progress ...
    if (m_pDialogRenderPreview)	{
      m_pDialogRenderPreview->setProgressData (&thePixmap, t, iNumberOfTotalFrames);
//      QApplication::postEvent(m_pDialogRenderPreview, new QTimerEvent(PROGRESS_EVENT));
//      m_pDialogRenderPreview->repaint ();
    }

    printf ("Rendering Frame <%X> <%d> of <%d> File <%s>\n",m_pDialogRenderPreview, t, iNumberOfTotalFrames, (const char *)qsFileName);
    thePixmap.save (qsFileName, "PNG", 100); //"PNG");
  }
  // Phew done. Lets get back to StartMenuRender ...

  // this will release the main threat ...
  if (m_pDialogRenderPreview)
    m_pDialogRenderPreview->reject ();
//  delete m_pDialogRenderPreview;
//  m_pDialogRenderPreview = NULL;
*/
}

void DVDMenu::slotRenderFrame ()
{  
	bool bPNG  = Global::bPNGRender;
	QPixmap thePixmap;
	QString qsFullPath, qsFileName;
	qsFullPath = QString ("%1/%2/%3/background/rendered_").arg(Global::qsTempPath).arg(Global::qsProjectName).arg(getInterface()->qsMenuName);
	thePixmap  = getMenuPreview()->renderMenu (getInterface(), m_iCurrentFrameToRender++, m_iTotalFramesToRender);
	//qsFileName.sprintf ("%s%08ld.%s", (const char *)qsFullPath, m_iCurrentFrameToRender, bPNG ? "png" : "jpg" ); //ooo
    qsFileName.sprintf ("%s%08ld.%s", qsFullPath.toLatin1().data(), m_iCurrentFrameToRender, bPNG ? "png" : "jpg" );    //xxx
 
	// Okay we have the newly rendered background image ... display some progress ...
	if (m_pDialogRenderPreview)
		m_pDialogRenderPreview->setProgressData (&thePixmap, m_iCurrentFrameToRender, m_iTotalFramesToRender);

	//	printf ("Rendering Frame <%d> of <%d> File <%s>\n", m_iCurrentFrameToRender, m_iTotalFramesToRender, (const char *)qsFileName);
    if ( bPNG )
		thePixmap.save ( qsFileName, "PNG",  100 );
	else
		thePixmap.save ( qsFileName, "JPEG", 100 );
	if (m_iCurrentFrameToRender <= m_iTotalFramesToRender-1)
		QTimer::singleShot (0, this, SLOT(slotRenderFrame()));
	else	{	// The last frame has been rendered.
		// Okay first we get the first frames of all MovieObjects again ...
		getMenuPreview()->renderMenu (getInterface(), 0, m_iTotalFramesToRender);
		// Next we get the original background image back
		qsFileName = QString ("%1/%2/%3/"BACKGROUND_NAME).arg(Global::qsTempPath).arg(Global::qsProjectName).arg(getInterface()->qsMenuName);
		thePixmap.load ( qsFileName );
		getMenuPreview()->setPixmap ( thePixmap );
		// And last but not least we return from this render - task.
		m_iTotalFramesToRender  = 0;
		m_iCurrentFrameToRender = 0;
		if (m_pDialogRenderPreview)
			delete m_pDialogRenderPreview;
		m_pDialogRenderPreview = NULL;
		getDVDAuthor ( )->returnFromRenderMenu ();
	}
}

Rgba DVDMenu::getColor (int iNumber)
{
  if ( iNumber < MAX_MASK_COLORS )
    return m_DVDMenuInterface.pgcColors[iNumber];
  Rgba   dummy;
  return dummy;
}

void DVDMenu::slotEditTimeline ( )
{
  if ( isEmpty ( ) )
    return;

  CDVDMenuInterface origInterface;
  origInterface = *getInterface ( );
  DVDMenuUndoObject *pUndo = new DVDMenuUndoObject ( DVDMenuUndoObject::EDIT_MENU, this );

  DialogMovie movieDialog ( NULL );
  movieDialog.initMe      ( this );
  connect ( &movieDialog, SIGNAL ( signalSetThumbnail ( long ) ), this, SLOT ( slotBackgroundFromDialog ( long ) ) ); // calls SourceToolbar::slotSetThumbnail ( long )
  movieDialog.exec        (      );

  disconnect ( this, SLOT ( slotBackgroundFromDialog ( long ) ) );

  if ( ! ( origInterface == *getInterface ( ) ) )  {
    // If something has changed here, we should inseryt an undo step.
    UndoBuffer *pBuffer = getUndoBuffer ( );
    if ( pBuffer )
         pBuffer->push ( pUndo );
  }
  else
    delete pUndo;
}

void DVDMenu::slotBackgroundFromDialog ( long iMSecOffset )
{
  // called in case we have a MovieBackground and the user wants to change the backround image.
  MediaInfo *pMediaInfo = getDVDAuthor ( )->getMediaInfo ( );

  if ( pMediaInfo && ! getInterface ( )->qsMovieFileName.isEmpty ( ) ) {
    Utils      theUtil;
    QString    qsExtensions, qsFileName;
    QFileInfo  backgroundInfo ( qsFileName );

    // Okay let us get the first frame of the game as a screenshot ...
    pMediaInfo->setFileName ( getInterface ( )->qsMovieFileName, false );
    QImage theImage = pMediaInfo->getScreenshot( iMSecOffset );
    qsFileName = theUtil.getTempFile ( backgroundInfo.baseName ( ) )+QString( ".png" );
    theImage.save ( qsFileName, "PNG" );
    getInterface  ( )->qsBackgroundFileName = qsFileName;
    updateDVDMenu ( );
  }
}

bool DVDMenu::isMovieMenu ( )
{
	uint t;
	MenuObject *pMenuObject;
	QList<MovieObject *>listMovieObjects = getMovieObjects ();	//oxx
	if (listMovieObjects.count() > 0)
		return true;

	// The second possibility is that the background itself is a movie 
	if ( ! getInterface()->qsMovieFileName.isEmpty ( ) )
	  return true;

	// The third possibility is that this menu has some kind of animation.
	if ( ! getInterface()->qsAnimation.isEmpty ())
		return true;
	
	// Finally the fourth possibility is to have animated objects
	for ( t=0; t<getMenuPreview ( )->objectCount ( ); t++ )  {
		pMenuObject = getMenuPreview ( )->menuObject ( t );
		if ( ! pMenuObject->animation( ).isEmpty ( ) )
			return true;
		if ( pMenuObject->shadow ( ) && ! pMenuObject->shadow ( )->animation ( ).isEmpty ( ) )
			return true;
		if ( pMenuObject->overlay( ) && ! pMenuObject->overlay( )->animation ( ).isEmpty ( ) )
			return true;
	}

	return false;
}

QList<MovieObject *> &DVDMenu::getMovieObjects()	//oxx
{
	static QList<MovieObject *> listReturn;		//oxx
	listReturn.clear ();
	uint t, i;
	MenuObject *pMenuObject;

	for ( t=0; t<getMenuPreview ( )->objectCount ( ); t++ )  {
		pMenuObject = getMenuPreview ( )->menuObject ( t );
		if (pMenuObject->objectType ( ) == BUTTON_OBJECT )  {
			ButtonObject *pButton = (ButtonObject *)pMenuObject;
			for (i=0;i< pButton->getNormalCount ( ); i++ )  {
				if (pButton->getNormal ( i )->objectType ( ) == MOVIE_OBJECT )
					listReturn.append ( (MovieObject *)pButton->getNormal ( i ) );
			}
		}
		else if (pMenuObject->objectType() == OBJECT_COLLECTION )  {
			ObjectCollection *pCollection = (ObjectCollection *)pMenuObject;
			for (i=0;i< (uint)pCollection->getCount ( );i++)	{
				if ( pCollection->getObject ( i )->objectType ( ) == MOVIE_OBJECT )
					listReturn.append ( (MovieObject *)pCollection->getObject ( i ) );
			}
		}
		else if ( pMenuObject->objectType ( ) == MOVIE_OBJECT )
			listReturn.append ( (MovieObject *)pMenuObject );
	}
	return listReturn;
}	//oooo

QList<ButtonObject *> &DVDMenu::getButtons()		//oxx
{
  static QList<ButtonObject *> listReturn;		//oxx

  uint t, i;
  listReturn.clear();
  ButtonObject tempButton;
  ObjectCollection tempCollection;
  MenuObject *pMenuObject;
  for (t=0;t<getMenuPreview()->objectCount();t++)	{
    pMenuObject = getMenuPreview()->menuObject(t);
    if (pMenuObject->objectType() == tempButton.objectType())
      listReturn.append((ButtonObject *)pMenuObject);
    else if (pMenuObject->objectType() == tempCollection.objectType())	{
      ObjectCollection *pCollection = (ObjectCollection *)pMenuObject;
      for (i=0;i< (uint)pCollection->getCount();i++)	{
	if (pCollection->getObject(i)->objectType () == tempButton.objectType())	
	  listReturn.append((ButtonObject *)pCollection->getObject(i));
      }
    }
  }
  return listReturn;
}

SourceFileEntry *DVDMenu::getEntryFromButtonAction ( QString &qsAction )
{
  SourceFileEntry *pEntry = NULL;
  //QStringList list = QStringList::split ( STRING_SEPARATOR, qsAction );	//ooo
  QStringList list = qsAction.split ( STRING_SEPARATOR );		//xxx
  pEntry = getDVDAuthor ( )->getSourceEntryByDisplayName ( list[1] );
  return pEntry;
}

SourceFileInfo *DVDMenu::getInfoFromButtonAction  ( QString &qsAction )
{
  uint t;
  QFileInfo fileInfo;
  SourceFileEntry *pEntry = NULL;
  //QStringList list = QStringList::split ( STRING_SEPARATOR, qsAction );	//ooo
  QStringList list = qsAction.split ( STRING_SEPARATOR );		//xxx
  pEntry = getDVDAuthor ( )->getSourceEntryByDisplayName ( list[1] );
  if ( ! pEntry )
    return NULL;
  if ( pEntry->listFileInfos.count ( ) == 1 )
    return pEntry->listFileInfos[0];

  if ( list.count ( ) == 4 ) {
    for  ( t=0 ; t<(uint)pEntry->listFileInfos.count ( ); t++ ) {	//ox
      fileInfo.setFile ( pEntry->listFileInfos[t]->qsFileName );
      if ( fileInfo.fileName ( ) == list[2] )
	return pEntry->listFileInfos[t];
    }
  }
  return NULL;
}

DVDMenu *DVDMenu::getMenuFromButtonAction  ( QString &qsAction )
{ 
  //QStringList list = QStringList::split ( STRING_SEPARATOR, qsAction );	//ooo
  QStringList list = qsAction.split ( STRING_SEPARATOR );		//xxx

  //QString qsMenuName = list[1];   //oooo-bug
  QString qsMenuName = "";  //xxx
  if ( list.count() > 1 )    //xxx
      qsMenuName = list[1]; //xxx

  if ( qsMenuName == "vmgm" )
       qsMenuName  = VMGM_LABEL;

  return getDVDAuthor ( )->getSubMenuByName ( qsMenuName );
}

QList<SourceFileEntry *> &DVDMenu::getSourceFileEntries ( bool bIncludePlayMultiple )		//oxx
{
  // Returns a list of all SourceFileEntries which belong to this menu.
  // Note: the list holds only one entry for an SourceFileEntry even if
  //       the menu holds multiple references to it.
  static QList<SourceFileEntry *> listReturn;			//oxx
  QList<ButtonObject *> listButtons = getButtons ( );		//oxx
  QStringList   listMultiple;
  //  ButtonObject  tempButton;
  ButtonObject    *pButton;
  SourceFileEntry *pEntry;
  bool bFound;
  uint t, i;

  listReturn.clear ( );
  for  ( t=0; t<(uint)listButtons.count ( ); t++ )  {		//ox
    bFound  = false;
    pButton = listButtons[t];
    pEntry  = pButton->sourceFileEntry ( );
    // If no SourceEntry in ReturnList than insert it.
    // Else check if not already inserted.
    if ( (int)listReturn.count ( ) > 0 )  {
      //Check if the SourceEntry isn't already inserted.
      for  ( int k=0; k<(int)listReturn.count ( ); k++ )  {
        if ( pEntry == listReturn[k] )  {
          bFound = true;
          break;
        }
      }
    }
    //If SourceEntry not found then insert it.
    if ( ! bFound && pEntry )
      listReturn.append ( pEntry );

    listMultiple = pButton->getPlayMultipleList ( );
    for ( i=0; i<(uint)listMultiple.count ( ); i++ )  {		//ox
      pEntry = getEntryFromButtonAction ( listMultiple[i] );
      bFound = false;
      if ( pEntry ) {
        for  ( int k=0; k<(int)listReturn.count ( ); k++ )  {
          if ( pEntry == listReturn[k] )  {
            bFound = true;
            break;
          }
        }
        //If SourceEntry not found then insert it.
        if ( ! bFound && pEntry && bIncludePlayMultiple )
          listReturn.append ( pEntry );
      }
    }
  }
  return listReturn;
}

QList<SourceFileInfo *> &DVDMenu::getSourceFileInfos()		//oxx
{
  // This function will return all SourceFileEntries actually used in this Menu.
  static QList<SourceFileInfo *> listReturn;			//oxx
  QList<ButtonObject *> listButtons = getButtons ( );		//oxx

  SourceFileInfo *pInfo = NULL;
  ButtonObject   *pButton;
  QStringList listMultiple;
  uint t, i;
  bool bFound = false;

  listReturn.clear();

  for ( t=0; t<(uint)listButtons.count ( ); t++ )  {		//ox
    pButton = listButtons[t];
    pInfo   = pButton->sourceFileInfo ( );
    bFound  = false;
    if ( pInfo ) {
      for  ( int k=0; k<(int)listReturn.count ( ); k++ )  {
	if ( pInfo == listReturn[k] )  {
	  bFound = true;
	  break;
	}
      }
      if ( ! bFound )
	listReturn.append( pInfo );
    }

    listMultiple = pButton->getPlayMultipleList ( );
    for ( i=0; i<(uint)listMultiple.count ( ); i++ ) {		//ox
      pInfo  = getInfoFromButtonAction ( listMultiple[i] );
      bFound = false;
      if ( pInfo ) {
	for  ( int k=0; k<(int)listReturn.count ( ); k++ )  {
	  if ( pInfo == listReturn[k] )  {
	    bFound = true;
	    break;
	  }
	}
	if ( ! bFound )
	  listReturn.append( pInfo );
      }
    }
  }
  return listReturn;
}

void DVDMenu::displayMask (uint iWhichMask)
{
	QPixmap *pMask;
	pMask = &m_highlightedMask;
	if (iWhichMask == SELECTED_MASK)
		pMask = &m_selectedMask;
	else if (iWhichMask == BACKGROUND_IMG)
		pMask = &m_DVDMenuInterface.backgroundImage;
	if (pMask->isNull())
		return;
	// And finally set the pixmap back again ...
	//m_pPixmapMenu->setPixmap(*pMask);    //oooo
    m_pPixmapMenu->set_Pixmap(*pMask);      //xxx
}

int DVDMenu::getFormat (int iWidth, int iHeight)
{
  Utils theUtil;
  return theUtil.getFormat ( iWidth, iHeight );
}

CDVDMenuInterface * DVDMenu::getInterface ()
{
  return &m_DVDMenuInterface;
}

QDVDAuthor *DVDMenu::getDVDAuthor ()
{
  return Global::pApp;
}

// This slot is called when the DialogButton is created (in MenuPreview, or ButtonObject)
void DVDMenu::slotRequestSourceFiles ( )
{
  int t;
  int iSourceFileCount = getDVDAuthor ( )->sourceFileCount ( );
  // Let us get the actual list of Source files ...
  QList<SourceFileEntry *> listSourceFileEntries;		//oxx
  for ( t=0; t<iSourceFileCount; t++ )
    listSourceFileEntries.append    ( getDVDAuthor ( )->sourceFileEntry ( t ) );
  m_pPixmapMenu->respondSourceFiles ( listSourceFileEntries );
}

// This slot is called when the DialogButton is created (in MenuPreview, or ButtonObject)
void DVDMenu::slotRequestSubMenus()
{
	uint t;
	static QStringList listSubMenus;
	listSubMenus.clear();
	QList<DVDMenu *> listDVDMenus = getDVDAuthor ( )->getSubMenus ( );	//oxx
	
	for (t=0;t<(uint)listDVDMenus.count();t++)	{	//ox
		listSubMenus.append (listDVDMenus[t]->name());
	}
	m_pPixmapMenu->respondSubMenus(listSubMenus);
}

void DVDMenu::updateDVDMenu ( bool bDisplayError )
{ 
  // Check if we already have background specified ...
  if (m_DVDMenuInterface.qsBackgroundFileName.isEmpty())
    return;
  // If we have, then lets check if the file exists.
  QString qsFileName = m_DVDMenuInterface.qsBackgroundFileName;
  QFileInfo fileInfo ( qsFileName );
  if ( ! fileInfo.exists ( ) )  {	// So most likely the Background was generated from a movie background
    if ( ! m_DVDMenuInterface.qsMovieFileName.isEmpty ( ) )  {
      qsFileName = m_DVDMenuInterface.qsMovieFileName;
      fileInfo.setFile (qsFileName);
      if ( fileInfo.exists ( ) )  {
	// So now we're talking. The movie file exists
	// All we have to do is to re-generate the backgfround file.
	Utils theUtil;
	MediaInfo *pMediaInfo = getDVDAuthor ( )->getMediaInfo ( );
	pMediaInfo->setFileName ( qsFileName, false );
	QImage theImage = pMediaInfo->getScreenshot(0);
	// Then we save the screenshot in the temp dir
	m_DVDMenuInterface.qsBackgroundFileName = theUtil.getTempFile(fileInfo.baseName())+QString (".png");
	theImage.save (m_DVDMenuInterface.qsBackgroundFileName, "PNG");
	updateDVDMenu (m_DVDMenuInterface.qsBackgroundFileName);
	return;
      }
    }
    if ( bDisplayError )
      MessageBox::warning ( NULL, tr ("File not found !"),
       tr ("Warning, could not find file %1.").arg(qsFileName), QMessageBox::Ok, Qt::NoButton );
    return;
  }
  updateDVDMenu ( m_DVDMenuInterface.qsBackgroundFileName );
}

void DVDMenu::updateDVDMenu (QString qsFileName)
{
	// Okay to tackle the background image generation
	QImageHelper theImage;
	QPixmap thePreview (qsFileName);
	QImageHelper theFile;
	int iResizeAlgorithm = 0;

	// determine the algo ID which was stored as the name of the Algo ...
	QStringList listAvailAlgos = theFile.getAvailableResizeAlgorithms();
	for (uint t=0;t<(uint)listAvailAlgos.count();t++)	{	//ox
		if (listAvailAlgos[t] == m_DVDMenuInterface.qsResizeAlgo)
			iResizeAlgorithm = t;
	}

	m_pPixmapMenu->fitDVDMenuBackground (&thePreview, iResizeAlgorithm, m_DVDMenuInterface.iStretchType, m_DVDMenuInterface.posOffset, m_DVDMenuInterface.iWidth, m_DVDMenuInterface.iHeight);

	m_pPixmapMenu->setFixedSize (m_DVDMenuInterface.iWidth, m_DVDMenuInterface.iHeight);
	//m_pPixmapMenu->setPixmap ( (const QPixmap &) thePreview);    //oooo
    m_pPixmapMenu->set_Pixmap ( (const QPixmap &) thePreview);      //xxx
	m_DVDMenuInterface.backgroundImage = thePreview;
}

DVDMenu *DVDMenu::clone ( )
{
  DVDMenu *pMenu = new DVDMenu ( getDVDAuthor ( ) );
  CDVDMenuInterface *pNewInterface = pMenu->getInterface ( );
  CDVDMenuInterface *pMyInterface  = getInterface ( );
  *pNewInterface  = *pMyInterface;

  MenuPreview   *pNewPreview = pMenu->getMenuPreview ( );
  MenuPreview   *pMyPreview  = getMenuPreview ( );
 *pNewPreview = *pMyPreview;

  return pMenu;
}

void DVDMenu::slotVisibleRegion (bool bEnable)
{
  m_pPixmapMenu->setVisibleRegion (bEnable);
}

void DVDMenu::slotUpdateStructure ( )
{
  if ( getDVDAuthor ( ) )					//ooo
       getDVDAuthor ( )->slotUpdateStructure ( );
}

void DVDMenu::createTransitionsXml ( )
{  
  // Here we only create the xml files, and the start / end images.
  Utils             theUtils;
  DVDMenu          *pDVDMenu    = NULL;
  ButtonObject     *pButton     = NULL;
  ButtonTransition *pTransition = NULL;

  QImage  theImage;
  QString qsTempPath  = theUtils.getTempFile (  ""  );
  CDVDMenuInterface   * pInterface = getInterface ( );

  QList<ButtonObject *> list = getButtons ( );			//oxx
  QList<ButtonObject *>::iterator it = list.begin ( );		//oxx

  while ( it != list.end ( ) )  {  
    pButton = *it++;
    pTransition = pButton->getTransition ( );
    if ( ( ! pTransition ) || ( pTransition->bUserFile ) )
      continue;

    QString qsName      = pInterface->qsMenuName + "/" + pButton->name ( );
    pTransition->qsName = qsName; // E.g. "Main VMGM/Button 1"
    theUtils.recMkdir   ( qsTempPath + qsName );
    qsName += "/transition.vob";
    pTransition->qsTransitionVideoFileName = qsTempPath + qsName; // E.g. "/tmp/TestProjec/Main VMGM/Button 1/transition.vob"

    // Next we create the start image.
    QString qsStartName = qsTempPath + pTransition->qsName + "/start.jpg";
    createMask ( qsStartName, BACKGROUND_IMG, true );
    QImage startImage ( qsStartName );
    // and then the end image.
    qsName = qsTempPath + pTransition->qsName + "/end.jpg";
    SourceFileInfo *pInfo = pButton->sourceFileInfo ( );
    if ( ! pInfo )  {
      pDVDMenu = pButton->dvdMenu ( );
      if ( pDVDMenu )  {
           pDVDMenu->createMask ( qsName, BACKGROUND_IMG, true );
           pTransition->iFormat = pDVDMenu->getInterface ( )->iFormat;
           QImage endImage ( qsName );
           if ( startImage.size ( ) != endImage.size ( ) )  {
             //startImage = startImage.smoothScale  ( endImage.size ( ) );						//ooo
	     startImage = startImage.scaled ( endImage.size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
             startImage.save  ( qsStartName, "JPEG", 100 );
           }
      }
    }
    else  {
      // Now let us request the acutal screenshot. Note, this could be to a chapter of a video.
      pTransition->iFormat  = pInfo->format ( );
      MediaInfo *pMediaInfo = Global::pApp->getMediaInfo ( );
      bool bFound = false;
      if ( pMediaInfo )  {
        long iMSecOffset = pButton->getOffset ( );
        pMediaInfo->setFileName ( pInfo->qsFileName, false );
        theImage = pMediaInfo->getScreenshot ( iMSecOffset );
        if ( ! theImage.isNull ( ) )  {
               theImage.save   ( qsName, "JPEG", 100 );
               if ( theImage.size ( ) != startImage.size ( ) )  {
                 //startImage = startImage.smoothScale  ( theImage.size ( ) );							//ooo
		 startImage = startImage.scaled ( theImage.size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );		//xxx
                 startImage.save  ( qsStartName, "JPEG", 100 );
               }
               bFound = true;
        }
        if ( ! bFound )  {
          // Just to ensure we have a image for the end :
          Cache::Thumbs::Entry *pCache = Global::pThumbsCache->find ( pInfo->qsFileName );
          if ( pCache )  {
            if ( ! pCache->arrayOfThumbs )
                   pCache->loadImages  ( );
            pCache->arrayOfThumbs[0]->save ( qsName, "JPEG", 100 );
          }
          else if ( pInfo->pPreview )
                    pInfo->pPreview->save  ( qsName, "JPEG", 100 );
        }	//oooo
      }
    }

    // Finally we can create the Transitions xml file
    CXmlSlideshow *pSlideshow = pTransition->createSlideshow ( );
    QString qsFileName = pSlideshow->slideshow_name;
 
    qsFileName.replace ( "/", "_" );
    qsFileName = theUtils.getTempFile ( "" ) + pTransition->qsName + "/" + qsFileName + ".xml";
    pSlideshow->writeXml ( qsFileName );	//oooo
  }
}

/*bool DVDMenu::createContextMenu ( QPoint globalPos )      //xxx
{
  QAction *frameNewAction = new QAction("&Add Frame", this);
  QAction *textNewAction = new QAction("&Add Text", this);
  QAction *imageNewAction = new QAction("&Add Image", this);
  QAction *movieNewAction = new QAction("&Add Movie", this);
  QAction *libraryNewAction = new QAction("&Add From Library ...", this);
  
  QMenu *pMenu = new QMenu (this);
  pMenu->addAction(frameNewAction);
  pMenu->addAction(textNewAction);
  pMenu->addAction(imageNewAction);
  pMenu->addAction(movieNewAction);
  pMenu->addAction(libraryNewAction);
  
  connect(textNewAction, SIGNAL(triggered()), this, SLOT( slotAddTextObject  ( ) ) );
  
  pMenu->exec( globalPos );
}*/

/*void DVDMenu::mousePressEvent (QMouseEvent *pEvent)	//xxx
{
  //createContextMenu ( pEvent->globalPos  ( ) );
  m_pPixmapMenu->createContextMenu( pEvent->globalPos  ( ) );
}*/

/*void DVDMenu::mouseReleaseEvent ( QMouseEvent *pEvent )
{

}*/

StructureItem *DVDMenu::createStructure ( StructureItem *pMainItem )
{
  StructureItem *pMenuItem;
  StructureItem *pMaskItem;
  StructureItem *pButtonItem;
  StructureItem *pTemp, *pNext;
  QString qsEmpty;
  uint t, iCountButtons = 0;
  MenuObject *pObject;

  if ( ! pMainItem )  { 
    for ( t=0; t<m_pPixmapMenu->objectCount ( ); t++ )  {
      pObject  = m_pPixmapMenu->menuObject ( t );
      pTemp    = pObject->createStructure ( NULL );
    }
    m_pStructureItem = NULL;
    return NULL;
  }

  if ( ! m_pStructureItem ) { 
    m_pStructureItem = new StructureItem ( getMenuPreview ( ), StructureItem::SubMenu, pMainItem, pMainItem->lastChild ( ), m_DVDMenuInterface.qsMenuName );
    m_pStructureItem->setText ( 0, m_DVDMenuInterface.qsMenuName );		//xxx
    //m_pStructureItem->setExpandable ( TRUE );		//ooo
    m_pStructureItem->setExpanded ( true );		//xxx
    //m_pStructureItem->setOpen       ( TRUE );		//oooo
    //m_pStructureItem->setDisabled       ( false );	//xxx
  }
  else {
    //m_pStructureItem->setText ( m_DVDMenuInterface.qsMenuName, qsEmpty );	//ooo
    m_pStructureItem->setText ( 0, m_DVDMenuInterface.qsMenuName );		//xxx
  }

  QString qsButtonMasks = tr ( "Button Masks" );
  QString qsButtons     = tr ( "Specified Buttons" );
  QString qsBackground  = tr ( "Background" );
  QString qsSoundFiles  = tr ( "Sound File(s)" );
  QString qsIntroFile   = tr ( "Intro File" );
  QString qsExtroFile   = tr ( "Extro File" );

  // Delete all but the MenuObjects ( Buttons and all )
  //pTemp = (StructureItem *)m_pStructureItem->firstChild ( );	//ooo
  //pTemp = (StructureItem *)m_pStructureItem->takeChild (0);		//xxx
  pTemp = (StructureItem *)m_pStructureItem->child (0);		//xxx
  
  while ( pTemp )  { 
    //pNext = (StructureItem *)pTemp->nextSibling ( );	//ooo
    //pNext = (StructureItem *)pTemp->parent()->child(/*parent->indexOfChild(current)+*/1);	//xxx
    pNext = (StructureItem *)pTemp->parent()->child(pTemp->parent()->indexOfChild(pTemp)+1);	//xxx

    if  ( pTemp->text ( 0 ) == qsButtonMasks )
      delete  pTemp;
    else if ( pTemp->text ( 0 ) == qsBackground )
      delete  pTemp;
    //else if ( pTemp->text ( 0 ).find ( qsSoundFiles ) > -1 )	//ooo
    else if ( pTemp->text ( 0 ).indexOf ( qsSoundFiles ) > -1 )	//xxx
      delete  pTemp;
    //else if ( pTemp->text ( 0 ).find ( qsIntroFile ) > -1 )	//ooo
    else if ( pTemp->text ( 0 ).indexOf ( qsIntroFile ) > -1 )	//xxx
      delete  pTemp;
    //else if ( pTemp->text ( 0 ).find ( qsExtroFile ) > -1 )	//ooo
    else if ( pTemp->text ( 0 ).indexOf ( qsExtroFile ) > -1 )	//xxx
      delete  pTemp;
    pTemp   = pNext;
  }

  // Here is the rest of the Main Menu ...
  QString qsFileName, qsName;
  if ( ( ! m_DVDMenuInterface.qsHighlightedMaskFileName.isEmpty ( ) ) || 
       ( ! m_DVDMenuInterface.qsSelectedMaskFileName.isEmpty    ( ) ) )  {
    
    qsFileName  = QFileInfo ( m_DVDMenuInterface.qsHighlightedMaskFileName ).fileName ( );
    //pMaskItem   = (StructureItem *)pTemp->nextSibling ( );	//ooo
    pMaskItem   = (StructureItem *)pTemp->parent()->child(/*parent->indexOfChild(current)+*/1);
    if ( ! pMaskItem ) {
      pMaskItem = new StructureItem ( m_pStructureItem, StructureItem::Unknown, qsButtonMasks, qsFileName );
      //pMaskItem->setExpandable ( TRUE );	//ooo
      pMaskItem->setExpanded ( true );		//xxx
    }
    else if ( pMaskItem->text ( 1 ) != qsFileName )
      pMaskItem->setText ( 1, qsFileName );

    qsFileName = QFileInfo ( m_DVDMenuInterface.qsHighlightedMaskFileName ).fileName ( );
    pMenuItem = new StructureItem(pMaskItem, StructureItem::Highlighted, tr (" Highlighted"), qsFileName);
    //pMenuItem->setExpandable (TRUE);	//ooo
    pMenuItem->setExpanded (true);	//xxx

    qsName = tr ("to images");
    pButtonItem = new StructureItem ( pMenuItem, StructureItem::Button, tr ("Button 1"), qsName);
    //pButtonItem->setExpandable (TRUE);	//ooo
    pButtonItem->setExpanded (true);
    new StructureItem ( pButtonItem, StructureItem::Geometry, tr ("Geometry"), QString ("22, 23, 50, 100"));

    // Here is the info for the Selected Mask
    qsFileName = QFileInfo (m_DVDMenuInterface.qsSelectedMaskFileName).fileName ();
    pMenuItem = new StructureItem ( pMaskItem, StructureItem::Selected, tr (" Selected"), qsFileName);
    //pMenuItem->setExpandable (TRUE);	//ooo
    pMenuItem->setExpanded (true);	//xxx
    qsName = tr ("to images");
    pButtonItem = new StructureItem ( pMenuItem, StructureItem::Button, tr ("Button 1"), qsName);
    //pButtonItem->setExpandable (TRUE);	//ooo
    pButtonItem->setExpanded (true);		//xxx
    new StructureItem ( pButtonItem, StructureItem::Geometry, tr ("Geometry"), QString ("22, 23, 50, 100"));
  }

  pTemp = NULL;  //(StructureItem *)m_pStructureItem->firstChild  ( );

  for ( t=0; t<(uint)m_DVDMenuInterface.listIntroVobs.count ( ); t++ )   {	//ox
    qsFileName = QFileInfo ( m_DVDMenuInterface.listIntroVobs[t]->file ).fileName ( );
    pTemp = new StructureItem ( this, StructureItem::Intro, m_pStructureItem, pTemp, QString ("%1[%2]").arg ( qsIntroFile ).arg(t), qsFileName );
  }
  for ( t=0; t<(uint)m_DVDMenuInterface.listExtroVobs.count ( ); t++ )  {	//ox
    qsFileName = QFileInfo ( m_DVDMenuInterface.listExtroVobs[t]->file ).fileName ( );
    pTemp = new StructureItem ( this, StructureItem::Extro, m_pStructureItem, pTemp, QString ("%1[%2]").arg ( qsExtroFile ).arg(t), qsFileName );
  }
  if ( ! m_DVDMenuInterface.qsMovieFileName.isEmpty ( ) )  {
    qsFileName = QFileInfo ( m_DVDMenuInterface.qsMovieFileName ).fileName ( );
    pTemp = new StructureItem ( m_pPixmapMenu, StructureItem::Background, m_pStructureItem, pTemp, qsBackground, qsFileName );
  }
  else if ( ! m_DVDMenuInterface.qsBackgroundFileName.isEmpty ( ) )  {
    qsFileName = QFileInfo ( m_DVDMenuInterface.qsBackgroundFileName ).fileName ( );
    pTemp = new StructureItem ( m_pPixmapMenu, StructureItem::Background, m_pStructureItem, pTemp, qsBackground, qsFileName );
    //pTemp->setText ( 0, qsBackground );	//xxx
    //pTemp->setText ( 1, qsFileName );	//xxx
  }
  for ( t=0; t<(uint)m_DVDMenuInterface.listSoundEntries.count ( ); t++ ) {	//ox
    pTemp = new StructureItem ( this, StructureItem::Audio, m_pStructureItem, pTemp, QString ( "%1 %2").arg ( qsSoundFiles ).arg(t), m_DVDMenuInterface.listSoundEntries[t]->qsFileName );
  }

  // The second time we go through the list of menuObjects we take everything in the order we receive it ...
  for ( t=0; t<m_pPixmapMenu->objectCount ( ); t++ )  {
    pObject  = m_pPixmapMenu->menuObject ( t );
    if ( pObject->objectType ( ) !=  BUTTON_OBJECT )  {
      pTemp  = pObject->createStructure ( m_pStructureItem );
    }
  }

  // And at the end, we list all Buttons.
  //pMenuItem  = (StructureItem *)m_pStructureItem->firstChild ( );	//ooo
  pMenuItem  = (StructureItem *)m_pStructureItem->child ( 0 );		//xxx
  while ( pMenuItem ) {
    if  ( pMenuItem->text ( 0 ) == qsButtons )
      break;
    //pMenuItem = (StructureItem *)pMenuItem->nextSibling ( );							//oooo
    pMenuItem = (StructureItem *)pMenuItem->parent()->child(pMenuItem->parent()->indexOfChild(pMenuItem)+1);	//xxx
  }

  if ( ! pMenuItem ) {
    // If we get herem, then we did not have ButtonObjects previously,
    // and we have to create a root node for the buttons to come.
    pMenuItem = new StructureItem( this, StructureItem::Buttons, m_pStructureItem, m_pStructureItem->lastChild ( ), qsButtons );
    //pMenuItem->setText( 0, qsButtons );		//xxxx
    //pMenuItem->setExpandable ( TRUE );	//ooo
    pMenuItem->setExpanded ( true );		//xxx
    //pMenuItem->setOpen       ( TRUE );	//oooo
  }
  pTemp = NULL;

  // And we want to have the buttons at the end ...
  for ( t=0; t<m_pPixmapMenu->objectCount ( ); t++ )  {
    pObject =  m_pPixmapMenu->menuObject ( t );
    if ( pObject->objectType ( ) ==  BUTTON_OBJECT )  {
      pTemp = pObject->createStructure ( pMenuItem );
      iCountButtons ++;
    }
  }

  // If there were no buttons, then we don't need to show this in the structure ...
  if ( ! iCountButtons )
    delete pMenuItem;

  return m_pStructureItem;
  
}
