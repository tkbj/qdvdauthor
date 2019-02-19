/***************************************************************************
    qdvdauthorinit.cpp
                             -------------------
    Implementation of class QDVDAuthorInit
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class initialises the QDVDAuthor application with the
   last used settings.

   It is mainly used, to handle the CommandQueueDialogs options
   which can now be changed by the user.
    
****************************************************************************/

#include <QMessageBox>
#include <QFileInfo>	//xxx
#include <QDir>		//xxx
#include <QTextStream>

#include "qdvdauthor.h"
#include "qdvdauthorinit.h"
#include "messagebox.h"
#include "buttonobject.h"
#include "slidedefaults.h"
#include "sourcefileentry.h"
#include "dragndropcontainer.h"
#include "qplayer/mediainfo.h"
#include "qplayer/mediacreator.h"
#include "qplayer/MEDIA_CONFIG.h"

QDVDAuthorInit::MenuBlock::MenuBlock ()
{ 
	bValid  = false;
	bHidden = false;
	blockMovieBackground.qsBlockTag = QString ( BLOCK_MOVIE_BACKGROUND );
	blockImageBackground.qsBlockTag = QString ( BLOCK_IMAGE_BACKGROUND );
	blockEmptySound.qsBlockTag = QString ( BLOCK_EMPTY_SOUND );
	blockMplex.qsBlockTag  = QString ( BLOCK_MPLEX );
	blockSpumux.qsBlockTag = QString ( BLOCK_SPUMUX );
	blockOthers.qsBlockTag = QString ( BLOCK_OTHERS );
	iKeyset = Keyword::Keys;	//oooo
};

// o Loading of init file.
// o Saving the MenuBlock stored data back to the ini - file.
// o Saving of default init file.
// o The function to store an init file depending on the users modifications.
// o Added buttons to the CommandQueueDialog (DialogExecute)
// o The function convertSoundFile ()
// o Finalized class requires only bug fixes. 21'st May 2005 (bad weather outside)
//////////////////////////////////////////////////////////////////////////////////////////////

QDVDAuthorInit::QDVDAuthorInit()
{
	initMe ( );
	m_pDragNDropContainer    = new DragNDropContainer;
	m_bOwnDragNDropContainer = true;
	readIniFile ( );	//oooo
}

QDVDAuthorInit::QDVDAuthorInit(DragNDropContainer *pDNDContainer)
{
	initMe      ( );
	m_pDragNDropContainer = pDNDContainer;
	readIniFile ( );	//oooo
}

QDVDAuthorInit::QDVDAuthorInit ( QList<Utils::toolsPaths *> &listPaths, DragNDropContainer *pDNDContainer )	//oxx
{
	// First we init some data
	initMe      ( );
	// Then we load the current ini - file and set all Internals to those values
	readIniFile ( );	//oooo
	// And here we set the dragNDropContainer AFTER laoding to keep the values preserved !
	m_pDragNDropContainer  = pDNDContainer;
	// Here we indicate that we do not want to delete the objects when deleting this object ...
	m_bProtectToolsObjects = true;
	// and then we set the paths list to the one we want it to be ...
	setToolsPaths ( listPaths );
	// Finally we set the actual SlideDefaults
	//*m_pSlideDefaults      = *Global::pSlideDefaults;	//oooo
}

QDVDAuthorInit::~QDVDAuthorInit ( )
{
  int t;

  for (t=0;t<(int)m_listToBeDeleted.count();t++)
    delete m_listToBeDeleted[t];
  m_listToBeDeleted.clear ();
  m_listExecute.clear ();
  for (t=0;t<(int)m_arrayColors.count();t++)
    delete m_arrayColors[t];
  if (m_bOwnerOfHistoryPix)	{
    // If this flag is not set means that some other object (QDVDAuthor e.g.) is the master of these.
    for (t=0;t<(int)m_listHistoryPix.count();t++)
      delete m_listHistoryPix[t];
    m_listHistoryPix.clear ( );
  }

  /*if ( m_pSlideDefaults )
    delete m_pSlideDefaults;
  m_pSlideDefaults = NULL;*/	//oooo

  // Note: Since we clear the list every time getToolsPaths list is called we can delete 
  //       All objects which are at this time still in the list. 
  if (!m_bProtectToolsObjects)	{
    int  iCount = m_listToolsPaths.count() - 2;
    if ( iCount >= 0 ) {
      for (t=0;t<iCount;t++)
	delete m_listToolsPaths[t];
    }
    m_listToolsPaths.clear ( );
  }
  if ( m_bOwnDragNDropContainer )
    delete m_pDragNDropContainer;
  m_pDragNDropContainer = NULL;
  m_bOwnDragNDropContainer = false;
}

bool QDVDAuthorInit::initMe()
{
	int t;
	Utils theUtils;
	m_iMaxHistoryFiles      = 10;
	m_bOwnerOfHistoryPix    = true;
	m_iAutosave             = 300;
 	m_iColorCounter         = 0;
	m_iCurrentBlockID       = 0;	// To uniquely identify each block of commands ...
	m_bProtectToolsObjects  = false;
	m_bVisibleRegion        = false;
	m_qsStyle               = theUtils.currentStyleName ( );
	m_bActivateTabs[0]      = true;
	m_bActivateTabs[1]      = true;
	m_bActivateTabs[2]      = true;
	m_qsTempPath            = Global::qsTempPath + QString ("/") + Global::qsProjectName;
	m_qsProjectPath         = Global::qsProjectPath;
	m_qsDvdauthorFile       = m_qsTempPath + QString ("/") + QString(DVDAUTHOR_XML);
	m_pDragNDropContainer   = NULL;
	m_bOwnDragNDropContainer= false;
	//m_pSlideDefaults        = new SlideDefaults;			//oooo
	//*m_pSlideDefaults       = *Global::pSlideDefaults;		//oooo

	m_bFirstApplicationStart       = false;
	m_convertMenuBlock.qsMenuTag   = QString (TAG_CONVERT);
	m_convertMenuBlock.iKeyset     = Keyword::TransKeys;
	m_subtitlesMenuBlock.qsMenuTag = QString (TAG_SUBTITLES);
	m_subtitlesMenuBlock.iKeyset   = Keyword::SubKeys;
	m_slideshowMenuBlock.qsMenuTag = QString (TAG_SLIDESHOW);
	m_palMenuBlock.qsMenuTag       = QString (TAG_PAL);
	m_ntscMenuBlock.qsMenuTag      = QString (TAG_NTSC);
	m_dvdauthorMenuBlock.qsMenuTag = QString (TAG_DVDAUTHOR);
	m_burningMenuBlock.qsMenuTag   = QString (TAG_BURNING);
	for ( t=0; t<(int)m_listToBeDeleted.count ( ) - 2; t++ )
		delete m_listToBeDeleted[t];
	m_listToBeDeleted.clear ( );
	m_listExecute.clear     ( );

	return true;
}

void QDVDAuthorInit::setSlideshowDefaults ( )
{
	/*if ( Global::pSlideDefaults && m_pSlideDefaults )
	    *Global::pSlideDefaults = *m_pSlideDefaults;*/	//oooo
}

QString QDVDAuthorInit::preferredEngine()
{
	return m_qsPreferredEngine;
}

bool QDVDAuthorInit::visibleRegion()
{
	return m_bVisibleRegion;
}

void QDVDAuthorInit::setToolsPaths (QList<Utils::toolsPaths *> &listPaths)
{
	// Note: Deletion of the objects in the list only because we know where this function is called from.
	// I.e. every time QDVDAuthorInit is called it'll create a new list and new objects. Thus when setting 
	//      the list manually then we have to get rid of the created Objects.
	uint t;
	if (m_listToolsPaths.count() > 0)	{
		for (t=0;t<(uint)m_listToolsPaths.count();t++)
			delete m_listToolsPaths[t];
		m_listToolsPaths.clear();
	}
	m_listToolsPaths = listPaths;
}

QList<Utils::toolsPaths *> &QDVDAuthorInit::getToolsPaths ()
{
	static QList<Utils::toolsPaths *> listNew;
	listNew = m_listToolsPaths;
	// Note: Here we return the list and reset the member list. 
	// We do this because we want to ensure the objects in this list are
	// created/deleted only once.
	m_listToolsPaths.clear();
	return listNew;
}

void QDVDAuthorInit::mergeToolsPaths (QList<Utils::toolsPaths *> listToolsPaths)
{
	// This function will merge the information found in the init file with the information in the 
	// static structure found in global.h
	uint t, i;
	QList<Utils::toolsPaths *> newToolsPathsList;
	Utils::toolsPaths *pNewEntry;
	// Okay first we create a fresh and clean list form the static list defined in global.h
	struct structTools {
		char pExecutableName[16];
		char pDisplayName[16];
		char pDescription[1024];
	};
	const structTools toolsArray[] = { EXTERNAL_TOOLS };
	uint iNrOfTools = sizeof ( toolsArray ) / ( sizeof ( structTools ) );
	for (t=0;t<iNrOfTools;t++)	{
		pNewEntry = new Utils::toolsPaths;
		pNewEntry->qsExecutableName = QString (toolsArray[t].pExecutableName);
		newToolsPathsList.append (pNewEntry);
	}
	// then we copy all values over from the current list
	for (t=0;t<(uint)newToolsPathsList.count();t++)	{
		for (i=0;i<(uint)m_listToolsPaths.count();i++)	{
			if (m_listToolsPaths[i]->qsExecutableName == newToolsPathsList[t]->qsExecutableName)	{
				newToolsPathsList[t]->qsFullPath = m_listToolsPaths[i]->qsFullPath;
				newToolsPathsList[t]->bPresent = m_listToolsPaths[i]->bPresent;
				i = m_listToolsPaths.count();
			}
		}
	}
	// finally we copy all values over found in the ini file list ...
	for (t=0;t<(uint)listToolsPaths.count();t++)	{
		for (i=0;i<(uint)newToolsPathsList.count();i++)	{
			if (newToolsPathsList[i]->qsExecutableName == listToolsPaths[t]->qsExecutableName)	{
				newToolsPathsList[i]->qsFullPath = listToolsPaths[t]->qsFullPath;
				i = newToolsPathsList.count();
			}
		}
	}
	// And at the end we check if the executable exists in the place specified ...
	QString qsPath;
	QFileInfo fileInfo;
	for (t=0;t<(uint)newToolsPathsList.count();t++)	{
		fileInfo.setFile(newToolsPathsList[t]->qsFullPath);
		if (newToolsPathsList[t]->qsExecutableName == QString ("bash")) {
			if ( newToolsPathsList[t]->qsFullPath.isEmpty () ||  newToolsPathsList[t]->qsFullPath == "bash" )
				Global::qsBashString = QString ("#!/bin/bash\n\n");
			else
				Global::qsBashString = QString ("#!%1 \n\n").arg(newToolsPathsList[t]->qsFullPath);
		}
		if (fileInfo.exists())
			newToolsPathsList[t]->bPresent = true;
		else
			newToolsPathsList[t]->bPresent = false;
	}
	// Okay now we can delete the entries in the current list ...
	for (t=0;t<(uint)m_listToolsPaths.count();t++)
		delete m_listToolsPaths[t];
	m_listToolsPaths.clear ();
	// now copy the new list over and make it the current list
	m_listToolsPaths = newToolsPathsList;
	// Done here, lets go home ...
}

QStringList QDVDAuthorInit::getHistory ()
{
	return m_listHistory;
}

QList<QPixmap *> QDVDAuthorInit::getHistoryPix ()
{
	// This flag indicates that we moved all Pixmaps to the caller.
	m_bOwnerOfHistoryPix = false;
	return m_listHistoryPix;
}

int QDVDAuthorInit::maxHistoryFiles ()
{
	return m_iMaxHistoryFiles;
}

void QDVDAuthorInit::setStyle (QString qsStyle)
{
	m_qsStyle = qsStyle;
}

int QDVDAuthorInit::getAutosave ()
{
	return m_iAutosave;
}

void QDVDAuthorInit::setAutosave (int iAutosave)
{
	m_iAutosave = iAutosave;
}

void QDVDAuthorInit::setVisibleRegion (bool bVisibleRegion)
{
	m_bVisibleRegion = bVisibleRegion;
}

void QDVDAuthorInit::setVisibleTabs ( int iIdx, bool bActivateTab )
{
	if ( iIdx > 3 )
		return;
	m_bActivateTabs[iIdx] = bActivateTab;
}

void QDVDAuthorInit::setHistory (int iMaxHistoryFiles, QStringList listHistory, QList<QPixmap *> listHistoryPix)
{
	m_iMaxHistoryFiles   = iMaxHistoryFiles;
	m_listHistory        = listHistory;
	m_listHistoryPix     = listHistoryPix;
	m_bOwnerOfHistoryPix = false;
}

bool QDVDAuthorInit::readIniFile ( )
{
	// This function reads in the xml - init - file for 'Q' DVD-Author and
	// generates the neccesary data structures to be used by appendMenu.
	float fStoredVersion;
	int iReturn;
	QString qsChildTag, qsTabs;
	uint t, i, k;
	// Assign the file
	//QString qsInitFile = QDir::homeDirPath();	//ooo
	QString qsInitFile = QDir::homePath();		//xxx
	qsInitFile.append(INIT_DIR_NAME);
	QDir iniDir (qsInitFile);
	if (!iniDir.exists())
		iniDir.mkdir (qsInitFile);

	qsInitFile.append(INIT_FILE_NAME);
	QFile initFile(qsInitFile);
	if (!initFile.open(QIODevice::ReadWrite))
		return false;

	QDomDocument xmlDoc( INIT_DOCTYPE );
	if (!xmlDoc.setContent (&initFile))	{
		// Error handling ...
		initFile.close();

		iReturn = MessageBox::warning ( NULL, QObject::tr ("QDVDAuthor - init file seems to be defective."),
			   QObject::tr ("The file %1%2 seems  to have a problem. Do you want to reset this file ?").arg(INIT_DIR_NAME).arg(INIT_FILE_NAME),
			   QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape );
		if ( iReturn == QMessageBox::Yes  ) {
			//Global::pApp->slotVersions( );	//oooo
			resetInitFile ( );
			return initMe ( );
		}
	}
	// And at last lets try to read the information of the file.
	QDomElement docElem = xmlDoc.documentElement();
	// Sanity check ...
	if (docElem.tagName() != QString (TAG_INIT))
		return false;

	QDomAttr a = docElem.attributeNode ( ATTRIB_VERSION );
	fStoredVersion = a.value().toFloat();
	a = docElem.attributeNode ( ATTRIB_STYLE );
	if (!a.value().isEmpty())
		m_qsStyle = a.value();
	a = docElem.attributeNode ( ATTRIB_LANGUAGE );
	if ( ! a.value().isEmpty  ( ) )
	  Global::qsLanguage = a.value ( );
	a = docElem.attributeNode ( ATTRIB_PNGRENDER );
        if ( ! a.value().isEmpty  ( ) )
          Global::bPNGRender =    ( a.value ( ) == "true" );
	a = docElem.attributeNode ( ATTRIB_FIRST_START );
	if (!a.value().isEmpty())
	  m_bFirstApplicationStart = (bool) a.value().toInt ();
	a = docElem.attributeNode ( ATTRIB_VISIBLE_REGION );
	m_bVisibleRegion = false;
	if (a.value() == QString ("true"))
		m_bVisibleRegion = true;
	a = docElem.attributeNode ( ATTRIB_TABS );
	qsTabs = a.value ( );
	a = docElem.attributeNode ( ATTRIB_MAX_HISTORY );
	if (a.value().toInt() > 0)
		m_iMaxHistoryFiles = a.value().toInt();
	a = docElem.attributeNode ( ATTRIB_MAX_THREADS );
	if ( a.value ( ).toInt ( ) > 0 )
		Global::iMaxRunningThreads = a.value ( ).toInt ( );
	a = docElem.attributeNode ( ATTRIB_DELETE_OLDER_THAN );
	Global::iDeleteTempOlderThan = a.value ( ).toInt ( );
	a = docElem.attributeNode ( ATTRIB_AUTOSAVE );
	if (a.value().toInt() > 0)
		m_iAutosave = a.value().toInt();
	if (fStoredVersion > (float)INIT_VERSION)	{
		iReturn = MessageBox::warning ( NULL, QObject::tr ("QDVDAuthor - init file is newer than the current version of 'Q' DVD-Author."),
			QObject::tr ("The file %1%2 was created with a more recent version.\nThis could lead to problems.\nDo you want to reset this file ?").arg(INIT_DIR_NAME).arg(INIT_FILE_NAME),
			QMessageBox::Yes, QMessageBox::No);
		if ( iReturn == QMessageBox::Yes )  {
			resetInitFile ();
			return initMe ();
		}
	}
	else if (fStoredVersion < (float)INIT_VERSION)	{
		// Here we tell the user and then reset the initfile.
		iReturn = MessageBox::warning ( NULL, QObject::tr ("QDVDAuthor - init file is outdated."),
			QObject::tr ("The file %1%2 was created with an older version of 'Q' DVD-Author.\nThis could lead to problems.\n\nDo you want to reset this file ?").arg(INIT_DIR_NAME).arg(INIT_FILE_NAME),
			QMessageBox::Yes, QMessageBox::No);
		if (iReturn == QMessageBox::Yes)	{
			resetInitFile ();
			return initMe ();
		}
	}
	// Lets decode the visible tabs.
	m_bActivateTabs[0] = true;
	m_bActivateTabs[1] = true;
	m_bActivateTabs[2] = true;
	//QStringList listTabs = QStringList::split ( ',', qsTabs );	//ooo
	QStringList listTabs = qsTabs.split ( ',' );			//xxx
	if ( listTabs.size ( ) == 3 )  {
		if ( listTabs[0] == "0" )
			m_bActivateTabs[0] = false;
		if ( listTabs[1] == "0" )
			m_bActivateTabs[1] = false;
		if ( listTabs[2] == "0" )
			m_bActivateTabs[2] = false;
	}
	// Here we get the next step in the tree down ... ExecuteInterface
	QStringList listCommands, listComments;
	QList<QString> listColors;
	QList<int>     listOrder;
	QDomNode xmlNode;
	QDomElement childElem;
	QList<Utils::toolsPaths *> listToolsPaths;
	QDVDAuthorInit::MenuBlock::SubBlock *pSubBlock;
	QDVDAuthorInit::MenuBlock::Block *pSubSubBlock;
	QDomNode xmlExecuteInterfaceNode = docElem.firstChild();
	bool bHidden;
	MenuBlock *pMenuBlock;
	m_listHistory.clear ();
	m_iCurrentBlockID = 0;
	// Okay
	listComments.clear ( );
	listCommands.clear ( );
	while (!xmlExecuteInterfaceNode.isNull())	{
		bHidden = false;
		childElem  = xmlExecuteInterfaceNode.toElement ( );
		qsChildTag = childElem.tagName ( );
		if (qsChildTag == QString ( TAG_EXECUTE ) )  {
			// Correctly detects a ExecuteInterface - tag
			// So lets get the only attribute for ExecuteInterface
			a = childElem.attributeNode ( ATTRIB_HIDDEN );
			if (a.value() == QString ("true"))
				bHidden = true;
			// Here we get the next step in the tree down ... Pal, or Ntsc or Dvdauthor or Burning
			xmlNode = childElem.firstChild();
			while (!xmlNode.isNull())	{
				childElem  = xmlNode.toElement ( );
				qsChildTag = childElem.tagName ( );
				pMenuBlock = NULL;
				if (qsChildTag == QString (TAG_CONVERT))
					pMenuBlock = &m_convertMenuBlock;
				if (qsChildTag == QString (TAG_SUBTITLES))
					pMenuBlock = &m_subtitlesMenuBlock;
				if (qsChildTag == QString (TAG_SLIDESHOW))
					pMenuBlock = &m_slideshowMenuBlock;
				else if (qsChildTag == QString (TAG_PAL))
					pMenuBlock = &m_palMenuBlock;
				else if (qsChildTag == QString (TAG_NTSC))
					pMenuBlock = &m_ntscMenuBlock;
				else if (qsChildTag == QString (TAG_DVDAUTHOR))
					pMenuBlock = &m_dvdauthorMenuBlock;
				else if (qsChildTag == QString (TAG_BURNING))
					pMenuBlock = &m_burningMenuBlock;
				if (!pMenuBlock)
					continue;
				if (!readMenuBlock (pMenuBlock, &childElem))
					return false;
				pMenuBlock->bHidden = bHidden;
				xmlNode = xmlNode.nextSibling();
			}
		}
		else if (qsChildTag == QString(TAG_PREFERRED_ENGINE ) )
			m_qsPreferredEngine = childElem.text ( );
		else if (qsChildTag == QString(TAG_TEMPLATE_AUTHOR ) )
			m_qsTemplateAuthor = childElem.text ( );
		else if (qsChildTag == QString(TAG_TEMPLATE_CONTACT ) )
			m_qsTemplateContact = childElem.text ( );
		else if (qsChildTag == QString(TAG_EXTERNAL_PLAYER ) )
			Global::qsExternalPlayer = childElem.text ( );
		else if (qsChildTag == QString(TAG_COLOR))	{
			QDomAttr a = childElem.attributeNode ( ATTRIB_NUMBER );
			listOrder.append  ( a.value().toInt ( ) );
			listColors.append ( childElem.text  ( ) );
			m_arrayColors.append (   new   Rgba ( ) );
		}
		else if (qsChildTag == QString(TAG_PATH))	{
			QDomAttr a = childElem.attributeNode ( ATTRIB_NAME );
			Utils::toolsPaths *pPath = new Utils::toolsPaths;
			pPath->qsFullPath = childElem.text();
			pPath->qsExecutableName = a.value ();
			listToolsPaths.append(pPath);
		}
		else if (qsChildTag == QString(TAG_HISTORY))	{
			QPixmap *pPixmap = new QPixmap;
			Utils theUtil;
			QDomAttr a = childElem.attributeNode ( ATTRIB_NAME );
			QString qsImage = childElem.text();
			pPixmap->load (qsImage);
			m_listHistory.append(a.value());
			m_listHistoryPix.append(pPixmap);
/* The original version when we stored the images in the ini - file.
			QPixmap *pPixmap = new QPixmap;
			Utils theUtil;
			QDomAttr a = childElem.attributeNode ( ATTRIB_NAME );
			QString qsImage = childElem.text();
			m_listHistory.append(a.value());
			m_listHistoryPix.append(pPixmap);
			// And now we convert the string to a QImage - object
			QImage theImage = theUtil.convertStringToImage (qsImage);
			pPixmap->convertFromImage(theImage);
*/
		}
		else if (qsChildTag == QString(TAG_DRAGNDROP))	{
			if (m_pDragNDropContainer)	{
				DragNDropContainer *p = m_pDragNDropContainer;
				int iSizeWidth, iSizeHeight;
				// Here we add the DragNDrop - stuff to the init - file ...
				QDomAttr a = childElem.attributeNode ( ATTRIB_DND_THUMBNAIL );
				// The size is something like 50x50 so we ought to split
				//int iXPos = a.value().find ("x");	//ooo
				int iXPos = a.value().indexOf ("x");	//xxx
				if (iXPos)	{
					iSizeWidth  = a.value().left(iXPos).toInt();
					iSizeHeight = a.value().right(a.value().length()-iXPos-1).toInt();
					if (iSizeWidth && iSizeHeight)
						p->sizeThumbnail = QSize (iSizeWidth, iSizeHeight);
				}

				a = childElem.attributeNode ( ATTRIB_DND_BUTTON );
				// The size is something like 50x50 so we ought to split
				//iXPos = a.value().find ("x");		//ooo
				iXPos = a.value().indexOf ("x");	//xxx
				if (iXPos)	{
					iSizeWidth  = a.value().left(iXPos).toInt();
					iSizeHeight = a.value().right(a.value().length()-iXPos-1).toInt();
					if (iSizeWidth && iSizeHeight)
						p->sizeButton = QSize (iSizeWidth, iSizeHeight);
				}
				a = childElem.attributeNode ( ATTRIB_DND_BUTTON_TYPE );
				p->bImageButton = a.value().toInt();

				a = childElem.attributeNode ( ATTRIB_DND_WITH_TEXT );
				p->bWithText = a.value().toInt();

				a = childElem.attributeNode ( ATTRIB_DND_TEXT_POS );
				p->iTextPosition = a.value().toInt();

				a = childElem.attributeNode ( ATTRIB_DND_START );
				if (!a.value().isNull())
					p->timeStartOffset = QTime::fromString(a.value());

				a = childElem.attributeNode ( ATTRIB_DND_DURATION );
				if (!a.value().isNull())
					p->timeDuration = QTime::fromString (a.value());

				a = childElem.attributeNode   ( ATTRIB_DND_COLOR );
				p->colorForeground.fromString ( a.value ( ) );

				a = childElem.attributeNode ( ATTRIB_DND_START_WHEN );
				p->bStartAfterDropEvent = true;
				if (a.value() == QString ("false"))
					p->bStartAfterDropEvent = false;

				a = childElem.attributeNode ( ATTRIB_DND_NICE );
				p->iHowNiceShouldIBe = a.value().toInt();

				QDomElement fontElem = xmlExecuteInterfaceNode.toElement();
				if (fontElem.firstChild().toElement().tagName() == QString(TAG_FONT))
					p->font.fromString(fontElem.text());
			}
		}
		/*else if ( qsChildTag == QString ( TAG_SLIDE_DEFAULTS ) )  {
			m_pSlideDefaults->readIniFile ( childElem );
		}*/	//oooo
		// Get the next "ExecuteInterface"
		xmlExecuteInterfaceNode = xmlExecuteInterfaceNode.nextSibling();
	}

	// Okay, next we sort out the colors ...
	for (t=0;t<(uint)m_arrayColors.count();t++)
		m_arrayColors [ listOrder[t]]->fromString ( listColors[t] );

	// Next we take care that the tools we loaded are a tight fit for this program on this computer with the version of QDVDAuthor.
	mergeToolsPaths ( listToolsPaths );
	// Okay, we can get rid of all entries in the list we got form the ini - file ...
	for (t=0;t<(uint)listToolsPaths.count();t++)
		delete listToolsPaths[t];

	// Next in order to create the dvdauthor, and burn - block we need some data in the keywords to be used in resolvString()
	m_keyword.clear    ( Keyword::Keys );
	m_keyword.setValue ( Keyword::Keys, Keyword::KeyDVDAuthorXML, m_qsDvdauthorFile ); // +DVDAUTHOR_XML+
	m_keyword.setValue ( Keyword::Keys, Keyword::KeyProjectPath,  m_qsProjectPath );   // +PROJECT_PATH+
	m_keyword.setValue ( Keyword::Keys, Keyword::KeyTempPath,     m_qsTempPath );      // +TEMP_PATH+

	listCommands.clear ( );
	listComments.clear ( );
	// And finally we get the information for the dvdauthor block, and the burning block ...
	m_dvdauthorInterface.qsBlockName = m_keyword.resolvString( Keyword::Keys, m_dvdauthorMenuBlock.qsBlockName);
	m_dvdauthorInterface.backgroundColor = m_dvdauthorMenuBlock.colorBackground;
	for (t=0;t<(uint)m_dvdauthorMenuBlock.blockOthers.listComments.count();t++)
	  listComments.append( m_keyword.resolvString( Keyword::Keys, m_dvdauthorMenuBlock.blockOthers.listComments[t] ) );
	for (t=0;t<(uint)m_dvdauthorMenuBlock.blockOthers.listCommands.count();t++)
	  listCommands.append ( m_keyword.resolvString( Keyword::Keys, m_dvdauthorMenuBlock.blockOthers.listCommands[t] ) );
	m_dvdauthorInterface.listCommentList.append ( listComments );
	m_dvdauthorInterface.listCommandList.append ( listCommands );
	m_dvdauthorInterface.listDefaults.append ( m_dvdauthorMenuBlock.blockOthers.iDefaultCommand );
	m_dvdauthorInterface.bShowBlock = !m_dvdauthorMenuBlock.bHidden;
	m_dvdauthorInterface.listBlockIDs.append ( m_dvdauthorMenuBlock.blockOthers.iBlockID );

	listCommands.clear ( );
	listComments.clear ( );
	m_burnInterface.qsBlockName = m_keyword.resolvString( Keyword::Keys, m_burningMenuBlock.qsBlockName);
	m_burnInterface.backgroundColor = m_burningMenuBlock.colorBackground;
	m_burnInterface.bShowBlock = !m_burningMenuBlock.bHidden;
	for (i=0;i<(uint)m_burningMenuBlock.blockOthers.listSubBlocks.count();i++)	{
		pSubBlock = m_burningMenuBlock.blockOthers.listSubBlocks[i];
		for (t=0;t<(uint)pSubBlock->listBlocks.count();t++)	{
			pSubSubBlock = pSubBlock->listBlocks[t];
			for (k=0;k<(uint)pSubSubBlock->listComments.count ();k++)
			  listComments.append ( m_keyword.resolvString( Keyword::Keys, pSubSubBlock->listComments[k] ) );
			for (k=0;k<(uint)pSubSubBlock->listCommands.count ();k++)
			  listCommands.append ( m_keyword.resolvString( Keyword::Keys, pSubSubBlock->listCommands[k] ) );
			m_burnInterface.listBlockIDs.append (pSubSubBlock->iBlockID );
			m_burnInterface.listCommentList.append ( listComments );
			m_burnInterface.listCommandList.append ( listCommands );
			m_burnInterface.listDefaults.append ( pSubSubBlock->iDefaultCommand );
			listComments.clear ( );
			listCommands.clear ( );
		}
	}

	return true;
}

bool QDVDAuthorInit::saveIniFile ( )
{
  uint t;
  // This function writes in the xml - init - file for 'Q' DVD-Author.
  // the input for the file is taken from the structure stored in this class
  // and alsoo from the uses input.

  // Assign the file
  //QString qsInitFile = QDir::homeDirPath();	//ooo
  QString qsInitFile = QDir::homePath();		//xxx
  qsInitFile.append(INIT_DIR_NAME);
  qsInitFile.append(INIT_FILE_NAME);

  QFile initFile(qsInitFile);
  if (!initFile.open(QIODevice::WriteOnly))
    return false;

  QDomDocument xmlDoc( INIT_DOCTYPE );
  // Okay the document is set, now let us write into the document ...
  QDomElement rootInit = xmlDoc.createElement( TAG_INIT );	// <QDVDAuthorInitFile>
  rootInit.setAttribute( ATTRIB_VERSION,   QString ("%1").arg((float)INIT_VERSION));
  rootInit.setAttribute( ATTRIB_STYLE,     m_qsStyle );
  rootInit.setAttribute( ATTRIB_LANGUAGE,  Global::qsLanguage );
  rootInit.setAttribute( ATTRIB_PNGRENDER, Global::bPNGRender ? "true" : "false" );
  if ( m_bFirstApplicationStart )
    rootInit.setAttribute ( ATTRIB_FIRST_START, (int)m_bFirstApplicationStart);
  QString qsVisibleRegion ( "false" );
  if (m_bVisibleRegion)
    qsVisibleRegion = QString ( "true" );
  QString qsTabs = QString ("%1,%2,%3").arg ( (int)m_bActivateTabs[0] ).arg ( (int)m_bActivateTabs[1] ).arg ( (int)m_bActivateTabs[2] );
  rootInit.setAttribute( ATTRIB_VISIBLE_REGION,    qsVisibleRegion );
  rootInit.setAttribute( ATTRIB_TABS,              qsTabs );
  rootInit.setAttribute( ATTRIB_MAX_HISTORY,       QString ("%1").arg ( m_iMaxHistoryFiles ) );
  rootInit.setAttribute( ATTRIB_AUTOSAVE,          QString ("%1").arg ( m_iAutosave ) );
  rootInit.setAttribute( ATTRIB_MAX_THREADS,       QString ("%1").arg ( Global::iMaxRunningThreads   ) );
  rootInit.setAttribute( ATTRIB_DELETE_OLDER_THAN, QString ("%1").arg ( Global::iDeleteTempOlderThan ) );

  xmlDoc.appendChild ( rootInit );

  QDomElement preferredElement = xmlDoc.createElement (  TAG_PREFERRED_ENGINE );	// <PreferredEngine>
  QDomText domText = xmlDoc.createTextNode( MediaCreator::preferredEngine ( ) );
  preferredElement.appendChild (  domText );
  rootInit.appendChild ( preferredElement );

  QDomElement externalElement = xmlDoc.createElement( TAG_EXTERNAL_PLAYER );	// <ExternalPlayer>
  domText = xmlDoc.createTextNode (Global::qsExternalPlayer);
  externalElement.appendChild( domText );
  rootInit.appendChild ( externalElement );

  QDomElement templateAuthor = xmlDoc.createElement( TAG_TEMPLATE_AUTHOR );	// <TemplateAuthor>
  domText = xmlDoc.createTextNode ( m_qsTemplateAuthor );
  templateAuthor.appendChild( domText );
  rootInit.appendChild ( templateAuthor );

  QDomElement templateContact = xmlDoc.createElement( TAG_TEMPLATE_CONTACT );	// <TemplateContact>
  domText = xmlDoc.createTextNode ( m_qsTemplateContact );
  templateContact.appendChild( domText );
  rootInit.appendChild ( templateContact );

  // First things first ... the color table ...
  for (t=0;t<(uint)m_arrayColors.count();t++)	{
    QDomElement colorElement = xmlDoc.createElement( TAG_COLOR );	// <Color>
    domText = xmlDoc.createTextNode ( m_arrayColors[t]->toString ( ) );
    colorElement.setAttribute ( ATTRIB_NUMBER, QString ("%1").arg(t) );
    colorElement.appendChild  ( domText );
    rootInit.appendChild ( colorElement );
  }
  for (t=0;t<(uint)m_listToolsPaths.count();t++)	{
    QDomElement pathElement = xmlDoc.createElement( TAG_PATH );	// <Path>
    domText = xmlDoc.createTextNode (m_listToolsPaths[t]->qsFullPath);
    pathElement.setAttribute( ATTRIB_NAME, m_listToolsPaths[t]->qsExecutableName);
    pathElement.appendChild( domText );
    rootInit.appendChild ( pathElement );
  }
  for (t=0;t<(uint)m_listHistory.count();t++)	{
    //QString qsFileName = QDir::homeDirPath() + QString (INIT_DIR_NAME) + QString ("history%1.png").arg (t);	//ooo
    QString qsFileName = QDir::homePath() + QString (INIT_DIR_NAME) + QString ("history%1.png").arg (t);		//xxx
    m_listHistoryPix[t]->save(qsFileName, "PNG", 100);
    QDomElement historyElement = xmlDoc.createElement( TAG_HISTORY );	// <History>
    domText = xmlDoc.createTextNode (qsFileName);
    historyElement.setAttribute( ATTRIB_NAME, m_listHistory[t]);
    historyElement.appendChild( domText );
    rootInit.appendChild ( historyElement );
/* The original code when we had the images stored in the ini file itself.
    Utils theUtil;
    QImage theImage = m_listHistoryPix[t]->convertToImage();
    QDomElement historyElement = xmlDoc.createElement( TAG_HISTORY );	// <History>
    domText = xmlDoc.createTextNode (theUtil.convertImageToString(theImage));
    historyElement.setAttribute( ATTRIB_NAME, m_listHistory[t]);
    historyElement.appendChild( domText );
    rootInit.appendChild ( historyElement );
*/
  }
  
  // Here we add the DragNDrop - stuff to the init - file ...
  DragNDropContainer *p = m_pDragNDropContainer;
  if ( p ) {
    QDomElement dndElement  = xmlDoc.createElement( TAG_DRAGNDROP );  // <DragNDrop>
    QDomElement fontElement = xmlDoc.createElement( TAG_FONT );       // <Font>
    domText = xmlDoc.createTextNode (p->font.toString());
    dndElement.setAttribute( ATTRIB_DND_THUMBNAIL  , QString ("%1x%2").arg(p->sizeThumbnail.width()).arg(p->sizeThumbnail.height()));
    dndElement.setAttribute( ATTRIB_DND_BUTTON     , QString ("%1x%2").arg(p->sizeButton.width()).arg(p->sizeButton.height()));
    dndElement.setAttribute( ATTRIB_DND_BUTTON_TYPE, QString ("%1").arg(p->bImageButton));
    dndElement.setAttribute( ATTRIB_DND_WITH_TEXT  , QString ("%1").arg(p->bWithText));
    dndElement.setAttribute( ATTRIB_DND_TEXT_POS   , QString ("%1").arg(p->iTextPosition));
    dndElement.setAttribute( ATTRIB_DND_START      , QString ("%1").arg(p->timeStartOffset.toString(Qt::TextDate)));
    dndElement.setAttribute( ATTRIB_DND_DURATION   , QString ("%1").arg(p->timeDuration.toString(Qt::TextDate)));
    dndElement.setAttribute( ATTRIB_DND_COLOR      , QString ("%1").arg(p->colorForeground.toString ( ) ) );
    dndElement.setAttribute( ATTRIB_DND_START_WHEN , QString ("%1").arg(p->bStartAfterDropEvent ? "true" : "false"));
    dndElement.setAttribute( ATTRIB_DND_NICE       , QString ("%1").arg(p->iHowNiceShouldIBe));

    fontElement.appendChild( domText );
    dndElement.appendChild ( fontElement );
    rootInit.appendChild   ( dndElement );
  }

  /*if ( m_pSlideDefaults )  {
    QDomElement slideElement = xmlDoc.createElement ( TAG_SLIDE_DEFAULTS );  // <SlideDefaults>
    rootInit.appendChild          ( slideElement );
    m_pSlideDefaults->saveIniFile ( slideElement );
  }*/		//oooo

  // Next are the ExecuteInterface's The convert goes first bien sur ...
  QDomElement convertElement = xmlDoc.createElement( TAG_EXECUTE );	// <ExecuteInterface>
  saveMenuBlock (&m_convertMenuBlock,  &convertElement);
  rootInit.appendChild ( convertElement );

  // Next are the ExecuteInterface's Second is the Subtitles ...
  //	QDomElement subtitlesElement = xmlDoc.createElement( TAG_SUBTITLES );	// <ExecuteInterface>
  saveMenuBlock ( &m_subtitlesMenuBlock,  &convertElement );
  //	rootInit.appendChild ( subtitlesElement );

  // Create the ExecuteInterface for slideshows and ButtonTransitions
  QDomElement slideshowElement = xmlDoc.createElement ( TAG_EXECUTE );	// <ExecuteInterface>
  saveMenuBlock ( &m_slideshowMenuBlock,  &slideshowElement );
  rootInit.appendChild ( slideshowElement );

  // Next are the ExecuteInterface's Third are the menus ...
  QDomElement executeElement = xmlDoc.createElement( TAG_EXECUTE );	// <ExecuteInterface>
  saveMenuBlock (&m_palMenuBlock,  &executeElement);
  saveMenuBlock (&m_ntscMenuBlock, &executeElement);
  rootInit.appendChild ( executeElement );

  QDomElement dvdauthorElement = xmlDoc.createElement( TAG_EXECUTE );	// <ExecuteInterface>
  saveMenuBlock (&m_dvdauthorMenuBlock, &dvdauthorElement);
  rootInit.appendChild ( dvdauthorElement );

  QDomElement burnElement = xmlDoc.createElement( TAG_EXECUTE );	// <ExecuteInterface>
  saveMenuBlock (&m_burningMenuBlock, &burnElement);
  rootInit.appendChild ( burnElement );

  // And finally we will store the generated string into the init file ...
  QString xml = xmlDoc.toString ();
//printf("%s", (const char *)xml);
  //initFile.writeBlock(xml, qstrlen (xml));					//ooo
  initFile.write(xml.toLatin1().data(), qstrlen (xml.toLatin1().data()));		//xxx

  initFile.close();

  return true;
}

bool QDVDAuthorInit::readMenuBlock (MenuBlock *pMenuBlock, QDomElement *pElem)
{
  QString qsBlockName,qsTemp;
  MenuBlock::Block *pBlock;
  // This function reads in a MenuBlock (with multiple sub-Block)
  // First we get the attributes ...
  QDomAttr a = pElem->attributeNode ( ATTRIB_BLOCKNAME );
  pMenuBlock->qsBlockName = a.value();
  a = pElem->attributeNode ( ATTRIB_COLOR );
  pMenuBlock->colorBackground.setNamedColor(a.value());
  // Next we parse through the associated Blocks.
  QDomNode xmlNode = pElem->firstChild ();
  
  while (!xmlNode.isNull())	{
    QDomElement searchTree = xmlNode.toElement();
    if (searchTree.tagName() == QString (TAG_BLOCK) )	{
      pBlock = &pMenuBlock->blockOthers;
      // get the name of this block ...
      qsBlockName = searchTree.attributeNode ( ATTRIB_NAME ).value();
      if (qsBlockName == QString (BLOCK_MOVIE_BACKGROUND))
	pBlock = &pMenuBlock->blockMovieBackground;
      else if  (qsBlockName == QString (BLOCK_IMAGE_BACKGROUND))
	pBlock = &pMenuBlock->blockImageBackground;
      else if  (qsBlockName == QString (BLOCK_EMPTY_SOUND))
	pBlock = &pMenuBlock->blockEmptySound;
      else if  (qsBlockName == QString (BLOCK_MPLEX))
	pBlock = &pMenuBlock->blockMplex;
      else if  (qsBlockName == QString (BLOCK_SPUMUX))
	pBlock = &pMenuBlock->blockSpumux;
      if (!readBlock (pBlock, &searchTree)) 
	return false;
    }
    qsBlockName = searchTree.attributeNode ( ATTRIB_NAME ).value();
    
    xmlNode = xmlNode.nextSibling();
  }
  return true;
}

bool QDVDAuthorInit::readBlock ( MenuBlock::Block *pBlock, QDomElement *pElem )
{  
  // This function reads in the information of a Block.
  // It is a recursive function as for the listSubBlocks.
  int iCommandNumber = 0;
  MenuBlock::SubBlock *pSubBlock;
  QString qsTemp;
  QDomAttr a;
  // and now we dwelve into reading the rest of the fest ...
  qsTemp = pElem->attributeNode( ATTRIB_NAME ).value ( );
  if ( ( ! qsTemp.isNull ( ) ) && ( ! qsTemp.isEmpty ( ) ) )
    pBlock->qsBlockTag = qsTemp;
  a = pElem->attributeNode ( ATTRIB_ACTIVE );
  if ( a.value ( ) == QString ( "true" ) )
    pBlock->iActive = 1;
  else if ( a.value ( ) == QString ( "false" ) )
    pBlock->iActive = 0;
  pBlock->iBlockID = m_iCurrentBlockID ++;
  a = pElem->attributeNode ( ATTRIB_HIDDEN );
  pBlock->bHidden = false;
  if ( a.value ( ) == QString ( "true" ) )
    pBlock->bHidden = true;
  a = pElem->attributeNode ( ATTRIB_EDITABLE );
  pBlock->bEditable = false;
  if ( a.value ( ) == QString ( "true" ) )
    pBlock->bEditable = true;
  
  QDomNode xmlNode = pElem->firstChild ( );
  while ( ! xmlNode.isNull ( ) )  {
    QDomElement searchTree = xmlNode.toElement ( ); 
    if (searchTree.tagName() == QString ( TAG_COMMAND ) )  {
      a = searchTree.attributeNode ( ATTRIB_DEFAULT );
      if ( a.value ( ) == QString ( "true" ) )
	pBlock->iDefaultCommand = iCommandNumber;
      pBlock->listCommands.append ( searchTree.text ( ) );
      iCommandNumber ++;
    }
    else if ( searchTree.tagName ( ) == QString ( TAG_COMMENT ) )  {
      pBlock->listComments.append( searchTree.text ( ) );
    }
    else if ( searchTree.tagName ( ) == QString ( TAG_SUBBLOCK ) )  {
      // Next we go through the subNodes.
      pSubBlock = new MenuBlock::SubBlock;
      if ( ! readSubBlock ( pSubBlock, &searchTree ) ) //&subTree))
	return false;
      pBlock->listSubBlocks.append ( pSubBlock );
    }
    else 
      fprintf (stderr, "%s : %d : wrong nodeTag=<%s>\n", 
	       //__FILE__, __LINE__, (const char *)searchTree.tagName ( ) );	//ooo
	       __FILE__, __LINE__, searchTree.tagName ( ).toLatin1().data() );	//xxx
    xmlNode = xmlNode.nextSibling ( );
  }

  return true;
}

bool QDVDAuthorInit::readSubBlock ( MenuBlock::SubBlock *pSubBlock, QDomElement *pElem )
{
  // This function reads in the information of a Block.
  // It is a recursive function as for the listSubBlocks.
  MenuBlock::Block *pBlock;
  QString qsTemp;
  QDomAttr a;
  // and now we dwelve into reading the rest of the fest ...
  qsTemp = pElem->attributeNode ( ATTRIB_NAME ).value ( );
  if ( ( ! qsTemp.isNull ( ) )  && ( ! qsTemp.isEmpty ( ) ) )
    pSubBlock->qsName = qsTemp;
  a = pElem->attributeNode  ( ATTRIB_NUMBER );
  if ( a.value ( ) == QString ( "true" ) )
    pSubBlock->iNumber = a.value ( ).toInt ( );
  a = pElem->attributeNode  ( ATTRIB_FOREACH );
  if ( ( ! a.value ( ).isNull ( ) ) && ( ! a.value ( ).isEmpty ( ) ) )
    pSubBlock->iForEachInput = a.value ( ).toInt   ( );

  // Next we go through the subNodes.
  QDomNode subNode = pElem->firstChild ( );
  // Every SubBlock needs a block
  while ( ! subNode.isNull() )	{
    // and now we dwelve into reading the rest of the fest ...
    QDomElement subTree = subNode.toElement ( );
    // Here we check if the SubBlock has another BLOCK inside.    
    if ( subTree.tagName ( ) == QString ( TAG_BLOCK ) )  {
      // Next we go through the subNodes.
      // Every SubBlock needs a block
      pBlock = new MenuBlock::Block;
      // and now we dwelve into reading the rest of the fest ...
      if ( ! readBlock ( pBlock, &subTree ) )
        return false;

      pSubBlock->listBlocks.append (pBlock);
    }
    else
      fprintf (stderr, "%s : %d : wrong nodeTag=<%s>\n", 
	       //__FILE__, __LINE__, (const char *)pElem->tagName());//return false;	//ooo
	       __FILE__, __LINE__, pElem->tagName().toLatin1().data());//return false;	//xxx

    subNode = subNode.nextSibling();
  }
  return true;
}

bool QDVDAuthorInit::saveMenuBlock (MenuBlock *pMenuBlock, QDomElement *pElement)
{
	if (pMenuBlock->bHidden)
		pElement->setAttribute( ATTRIB_HIDDEN, QString ("true") );
	QDomDocument xmlDoc = pElement->ownerDocument();
	QDomElement systemElement = xmlDoc.createElement( pMenuBlock->qsMenuTag );
	systemElement.setAttribute( ATTRIB_BLOCKNAME, pMenuBlock->qsBlockName );
	if (!pMenuBlock->colorBackground.name().isEmpty())
		systemElement.setAttribute( ATTRIB_COLOR, pMenuBlock->colorBackground.name() );
	
	if (pMenuBlock->qsMenuTag == QString (TAG_CONVERT))	{
		// Print some status infos to check. Man unit testing is boring.
//		printBlock(&pMenuBlock->blockMovieBackground, 0);
		saveBlock (&pMenuBlock->blockMovieBackground, &systemElement); // Handling transcoding movie
		saveBlock (&pMenuBlock->blockImageBackground, &systemElement); // Handling transcoding + re multiplexing
		saveBlock (&pMenuBlock->blockMplex,           &systemElement); // Handling multiplexing only
		saveBlock (&pMenuBlock->blockEmptySound,      &systemElement); // Handling the sound conversion
		saveBlock (&pMenuBlock->blockOthers,          &systemElement); // Handling the sound conversion
	}
	else if ( (pMenuBlock->qsMenuTag == QString (TAG_PAL)) || (pMenuBlock->qsMenuTag == QString (TAG_NTSC)) )	{
		saveBlock (&pMenuBlock->blockMovieBackground, &systemElement);
		saveBlock (&pMenuBlock->blockImageBackground, &systemElement);
		saveBlock (&pMenuBlock->blockEmptySound,      &systemElement);
		saveBlock (&pMenuBlock->blockMplex,           &systemElement);
		saveBlock (&pMenuBlock->blockSpumux,          &systemElement);
	}
	else if ( pMenuBlock->qsMenuTag == QString ( TAG_SLIDESHOW ) )
		saveBlock (&pMenuBlock->blockMovieBackground, &systemElement); // Handling ButtonTransitions
	else 
		saveBlock (&pMenuBlock->blockOthers,          &systemElement);

	pElement->appendChild (systemElement);
	
	return true;
}

int QDVDAuthorInit::printBlock (MenuBlock::Block *pBlock, int iLevel)
{
	uint t, i;
	char cOffset[100];
	MenuBlock::SubBlock *pSubBlock;
	
	memset (cOffset,   0, 100);
	memset (cOffset, ' ', iLevel*4);
	printf ("%s<Block> MenuTag=<%s> active<%d> blockID<%d> default<%d> hidden<%d> editable<%d>\n",
		//cOffset, (const char *)pBlock->qsBlockTag, pBlock->iActive, pBlock->iBlockID,   //ooo
        cOffset, pBlock->qsBlockTag.toLatin1().data(), pBlock->iActive, pBlock->iBlockID,   //xxx
		pBlock->iDefaultCommand, pBlock->bHidden, pBlock->bEditable);

	if (pBlock->listComments.count() > 0)	{
		printf ("%s  <Comments> Count<%d>\n", cOffset,(int)pBlock->listComments.count());
		for (t=0;t<(uint)pBlock->listComments.count();t++)    //ox
			//printf ("%s    listComment[%d]=<%s>\n", cOffset, t, (const char *)pBlock->listComments[t]);    //ooo
            printf ("%s    listComment[%d]=<%s>\n", cOffset, t, pBlock->listComments[t].toLatin1().data()); //xxx
		printf ("%s  <\\Comments>\n", cOffset);
	}

	if (pBlock->listCommands.count() > 0)	{
		printf ("%s  <Commands> Count<%d>\n", cOffset, (int)pBlock->listCommands.count());
		for (t=0;t<(uint)pBlock->listCommands.count();t++)    //ox
			//printf ("%s    listCommand[%d]=<%s>\n", cOffset, t, (const char *)pBlock->listCommands[t]);    //ooo
            printf ("%s    listCommand[%d]=<%s>\n", cOffset, t, pBlock->listCommands[t].toLatin1().data()); //xxx
		printf ("%s  <\\Commands>\n", cOffset);
	}

	if (pBlock->listSubBlocks.count() > 0)	{
		for (t=0;t<(uint)pBlock->listSubBlocks.count();t++)	{ //ox
			pSubBlock = pBlock->listSubBlocks[t];
			printf ("%s  <SubBlocks> Count<%d> name=<%s> number=<%d>\n", cOffset, 
				//(int)pBlock->listSubBlocks.count(), (const char *)pSubBlock->qsName,  //ooo
                (int)pBlock->listSubBlocks.count(), pSubBlock->qsName.toLatin1().data(),    //xxx
				pSubBlock->iNumber);
			for (i=0;i<(uint)pSubBlock->listBlocks.count();i++)  //ox
				printBlock (pSubBlock->listBlocks[i], iLevel+1);
			printf ("%s  <\\SubBlocks>\n", cOffset);
		}
	}

	printf ("%s<\\Block>\n", cOffset);
	return iLevel + 1;
}

bool QDVDAuthorInit::saveBlock (MenuBlock::Block *pBlock, QDomElement *pElement)
{
	uint t;
	QDomElement blockElement;

	QDomDocument xmlDoc = pElement->ownerDocument();
	blockElement = xmlDoc.createElement( TAG_BLOCK );

	if ( (!pBlock->qsBlockTag.isEmpty()) && (!pBlock->qsBlockTag.isNull()) )
		blockElement.setAttribute ( ATTRIB_NAME, pBlock->qsBlockTag );

	if      (pBlock->iActive ==  0)
		blockElement.setAttribute( ATTRIB_ACTIVE, QString ("false"));
	else if (pBlock->iActive ==  1)
		blockElement.setAttribute( ATTRIB_ACTIVE, QString ("true"));
	else if (pBlock->iActive == -1) t=0; // Do nothing in this case
	
	if      (pBlock->bHidden ==  false)
		blockElement.setAttribute( ATTRIB_HIDDEN, QString ("false") );
	else
		blockElement.setAttribute( ATTRIB_HIDDEN, QString ("true") );

	if      (pBlock->bEditable ==  false)
		blockElement.setAttribute( ATTRIB_EDITABLE, QString ("false") );
	else
		blockElement.setAttribute( ATTRIB_EDITABLE, QString ("true") );

	blockElement.setAttribute( ATTRIB_HIDDEN, QString ("%1").arg(pBlock->iDefaultCommand) );

	for (t=0;t<(uint)pBlock->listComments.count();t++)	{
		QDomElement commentElement = xmlDoc.createElement( TAG_COMMENT );
		QDomText commentText = xmlDoc.createTextNode (pBlock->listComments[t]);
		commentElement.appendChild ( commentText );
		blockElement.appendChild ( commentElement );
	}
	for (t=0;t<(uint)pBlock->listCommands.count();t++)	{
	  QDomElement commandElement = xmlDoc.createElement( TAG_COMMAND );
	  if ( pBlock->iDefaultCommand == (int)t )
	    commandElement.setAttribute( ATTRIB_DEFAULT, QString ( "true" ) );
	  QDomText commandText = xmlDoc.createTextNode (pBlock->listCommands[t]);
	  commandElement.appendChild ( commandText );
	  blockElement.appendChild ( commandElement );
	}
	for (t=0;t<(uint)pBlock->listSubBlocks.count();t++)	{
	  if (!saveSubBlock(pBlock->listSubBlocks[t] , &blockElement) )
	    return false;
	}

	pElement->appendChild ( blockElement );

	return true;
}

bool QDVDAuthorInit::saveSubBlock (MenuBlock::SubBlock *pSubBlock, QDomElement *pElement)
{
  uint t;
  QDomElement subBlockElement;

  QDomDocument xmlDoc = pElement->ownerDocument ( );
  subBlockElement = xmlDoc.createElement( TAG_SUBBLOCK );

  if ( ( ! pSubBlock->qsName.isEmpty ( ) ) && ( ! pSubBlock->qsName.isNull ( ) ) )
    subBlockElement.setAttribute ( ATTRIB_NAME,   pSubBlock->qsName  );
  if ( pSubBlock->iForEachInput > -1  )
    subBlockElement.setAttribute ( ATTRIB_FOREACH, QString ( "%1" ).arg ( pSubBlock->iForEachInput ) );
  subBlockElement.setAttribute   ( ATTRIB_NUMBER,  pSubBlock->iNumber );
  
  for (t=0;t<(uint)pSubBlock->listBlocks.count();t++)	{
    if (!saveBlock(pSubBlock->listBlocks[t] , &subBlockElement) )
      return false;
  }
  
  pElement->appendChild ( subBlockElement );
  
  return true;
}

void QDVDAuthorInit::printCommandList( char *pFrom )
{
  printf ("%s =-> printCommandList \n", pFrom);

  for (uint t=0;t<(uint)m_listExecute.count();t++)  {	//ox
    for (uint i=0;i<(uint)m_listExecute[t]->listCommandList.count ();i++)  {
      QStringList qsCommandList = m_listExecute[t]->listCommandList[i];
      for (uint j=0;j<(uint)qsCommandList.count();j++)  {
	//printf ( "QDVDAuthorInit::printCommandList <%d><%d><%d> = <%s>\n", t, i, j, qsCommandList[j].ascii() );		//ooo
	printf ( "QDVDAuthorInit::printCommandList <%d><%d><%d> = <%s>\n", t, i, j, qsCommandList[j].toLatin1().data ( ) );	//xxx
      }
    }
  }
}

QList <ExecuteInterface *>&QDVDAuthorInit::getList()
{
  return m_listExecute;
}

Keyword *QDVDAuthorInit::getKeyword ()
{
  return &m_keyword;
}

void QDVDAuthorInit::setDefault( int iBlockID, uint iDefaultCommand )
{
  // This function will search through the MenuBlocks to find the appropriate comment 
  // in order to set the default command correctly.
  MenuBlock::Block *pBlock = getBlock(iBlockID);
  if ( pBlock ) // Sanity check
       pBlock->iDefaultCommand = iDefaultCommand;
}

QDVDAuthorInit::MenuBlock::Block *QDVDAuthorInit::getBlock ( int iBlockID, int *piKeyset )
{
  uint t, i, j, k;
  MenuBlock *arrayMenuBlocks[] = {&m_palMenuBlock, &m_ntscMenuBlock, &m_convertMenuBlock, &m_subtitlesMenuBlock, &m_slideshowMenuBlock, &m_dvdauthorMenuBlock, &m_burningMenuBlock};
  MenuBlock *pMenuBlock;
  MenuBlock::Block *pBlock, *pSubSubBlock, *pReturn = NULL;
  MenuBlock::SubBlock *pSubBlock;
  for (t=0;t<sizeof ( arrayMenuBlocks ) /  sizeof (MenuBlock *);t++)	{
    pMenuBlock = arrayMenuBlocks[t];
    if ( piKeyset )
        *piKeyset = pMenuBlock->iKeyset;
    if (pMenuBlock->blockMovieBackground.iBlockID == iBlockID)
      pReturn = &pMenuBlock->blockMovieBackground;
    else if (pMenuBlock->blockImageBackground.iBlockID == iBlockID)
      pReturn = &pMenuBlock->blockImageBackground;
    else if (pMenuBlock->blockEmptySound.iBlockID == iBlockID)
      pReturn = &pMenuBlock->blockEmptySound;
    else if (pMenuBlock->blockMplex.iBlockID      == iBlockID)
      pReturn = &pMenuBlock->blockMplex;
    else if (pMenuBlock->blockSpumux.iBlockID     == iBlockID)
      pReturn = &pMenuBlock->blockSpumux;
    else if (pMenuBlock->blockOthers.iBlockID     == iBlockID)
      pReturn = &pMenuBlock->blockOthers;
    else 	{	// No luck finding the block, let us check in the subBlocks of the MovieBackground - block 
      pBlock = &pMenuBlock->blockMovieBackground;
      // Note: if you add another sub-block to the functionality, you have to add it here too.
      for ( k=0; k<5; k++ ) {
	if ( k == 1 )
	  pBlock = &pMenuBlock->blockImageBackground;
	else if ( k == 2 )
	  pBlock = &pMenuBlock->blockOthers;
	else if ( k == 3 )
	  pBlock = &pMenuBlock->blockEmptySound;
	else if ( k == 4 )
	  pBlock = &pMenuBlock->blockMplex;
	for (i=0;i<(uint)pBlock->listSubBlocks.count();i++)	{
	  pSubBlock = pBlock->listSubBlocks[i];
	  for (j=0;j<(uint)pSubBlock->listBlocks.count();j++)	{
	    pSubSubBlock = pSubBlock->listBlocks[j];
	    if (pSubSubBlock->iBlockID == iBlockID)	{
	      return  pSubSubBlock;
	    }
	  }
	}
      }
    }
    if (pReturn)
      return pReturn;
  }
  if ( piKeyset )
      *piKeyset = Keyword::Keys;
  return NULL;
}

void QDVDAuthorInit::appendPreProcessing ()
{
  int t;
  // First we clear the 'old' list ...
  for (t=0;t<(int)m_listToBeDeleted.count();t++)
    delete m_listToBeDeleted[t]; 
  m_listToBeDeleted.clear ();
  m_listExecute.clear();

  m_keyword.clear( Keyword::Keys );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyDVDAuthorXML, m_qsDvdauthorFile );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyProjectPath,  m_qsProjectPath);
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyTempPath,     m_qsTempPath);
  // Then we append pre processing (movie conversion if neccesary).
  // Prep the convertInterface for Movie / Audio conversion ... 
  m_listExecute.append ( &m_convertInterface   );  
  m_listExecute.append ( &m_slideshowInterface );
  //printCommandList ("appendPreProcessing");
  //printCommandList ("appendPreProcessing");		//xxx

  m_listAudioFiles.clear ( );
}

void QDVDAuthorInit::appendConvert ( SourceFileEntry *pEntry )
{  
  uint t, i, j, k;
  int  iXPos, iSplitPos, iMaxAudio;
  bool bOkay;
  QString qsResolution, qsVideoFormat, qsRatio, qsWidth;
  QString qsAudioList, qsHeight, qsBaseName, qsCommand, qsFileName;
  QFileInfo fileInfo;

  TranscodeInterface  remuxInterface;
  TranscodeInterface *pInterface = NULL;
  Subtitles          *pSubtitles = NULL;
  ExecuteInterface   *pExecute   = NULL;
  SourceFileInfo     *pInfo      = NULL;
  Audio              *pAudio     = NULL;
  QDVDAuthorInit::MenuBlock::Block    *pBlock, *pSubSubBlock;
  QDVDAuthorInit::MenuBlock::SubBlock *pSubBlock;
  QStringList listCommands, listComments;

  if ( ! pEntry )
    return;

  // First we will handle the extra audio tracks of all SourceFileInfos in the SourceFileEntry.
  appendConvertAudioTracks ( pEntry );	//oooo

  remuxInterface.bRemuxOnly    = true;
  remuxInterface.qsAudioType   = "mp2";
  remuxInterface.iSample       = 48000;
  remuxInterface.iVideoBitrate = 6600000;

  // We need to fill all audio slots for any Info up to iMaxAudio slots
  for ( iMaxAudio=MAX_AUDIO-1; iMaxAudio>=0; iMaxAudio-- ) {
    if ( pEntry->arrayAudioEntries[iMaxAudio].iAudioNumber != -1 )
      break;
  }
  // index is 0 based, so we have to increase the count ...
  iMaxAudio++;

  for ( t=0;t<(uint)pEntry->listFileInfos.count ( ); t++ )	{
    pInfo  =  pEntry->listFileInfos[t];
    if ( ! pInfo )
      continue;
    pInterface = pInfo->pTranscodeInterface;
    //    pSubtitles = pInfo->arraySubtitles [ 0 ];
 
    // If we only add audio to the original we still need to re-multiplex the orig ...
    if ( ! pInterface && pInfo->audioCount ( ) > 0 ) {
      remuxInterface.qsAudioFormat = pInfo->qsAudioFormat;
      remuxInterface.qsResolution  = pInfo->qsResolution;
      remuxInterface.qsVideoFormat = pInfo->qsVideoFormat;
      remuxInterface.qsRatio       = pInfo->qsRatio;
      remuxInterface.fFrameRate    = pInfo->qsFPS.toFloat ( &bOkay );
      if ( ! bOkay )
	remuxInterface.fFrameRate = 29.97f;
      remuxInterface.iAudioBitrate = pInfo->qsBPS.toInt   ( &bOkay );
      if ( ! bOkay )
	remuxInterface.iAudioBitrate = 192;
      else if ( remuxInterface.iAudioBitrate > 2000 )
	//remuxInterface.iAudioBitrate = (int)((float)remuxInterface.iAudioBitrate / 1000.0);                          //ooo
        remuxInterface.iAudioBitrate = static_cast<int> ( static_cast<float> ( remuxInterface.iAudioBitrate ) / 1000.0); //xxx

      pInterface = &remuxInterface;
    }

    // The user wants this one to get converted.
    if ( pInterface ) {
      // It seems that the file is currently beeing transcoded.
      if ( pInterface->bTranscoding )
        continue;
      // Okay, this one needs to be converted.
      QString qsTrackName = pEntry->qsDisplayName;
      qsTrackName = qsTrackName.remove ( QRegExp ( "[\\[\\]<>]" ) );

      m_keyword.clear    ( Keyword::TransKeys );
      m_keyword.setValue ( Keyword::TransKeys, Keyword::TransDVDAuthorXML, m_qsDvdauthorFile );
      m_keyword.setValue ( Keyword::TransKeys, Keyword::TransProjectPath,  m_qsProjectPath   );
      m_keyword.setValue ( Keyword::TransKeys, Keyword::TransTempPath,     m_qsTempPath      );
      m_keyword.setValue ( Keyword::TransKeys, Keyword::TransTrackName,    qsTrackName       );
      setAudioAttributes ( pInfo, pInterface );	//oooo

      pExecute = new ExecuteInterface;
      pExecute->enType = ExecuteInterface::TYPE_TRANSCODING;
      pExecute->qsBlockName = m_keyword.resolvString ( Keyword::TransKeys, m_convertMenuBlock.qsBlockName );
      pExecute->backgroundColor = m_convertMenuBlock.colorBackground;
      listComments.clear ( );
      listCommands.clear ( );
 
      if (!pEntry->bSoundSource)	{ // here we handle the video sources.
	// default is to transcode only, no remultiplexing
	pBlock = &( m_convertMenuBlock.blockMovieBackground );
	if ( pInterface->bRemuxOnly ) // in case we detected that the mpeg2 is missing VOBs, we automatically mark it for replexing only
	  pBlock = & ( m_convertMenuBlock.blockMplex );
	else if ( pInfo->audioCount ( ) > 0 ) // if we want to add audio, then we have to transcode
	  pBlock = & ( m_convertMenuBlock.blockImageBackground );
	// Note the following line is kept out bc it represents the 
	// Transcode/MEncoder selectionDropDown and not used for conversion itself
	// pExecute->listBlockIDs.append (pBlock->iBlockID);
	qsResolution = pInterface->qsResolution;
	if ( qsResolution.isNull ( ) )
	  qsResolution = pInfo->qsResolution;
	//iXPos    = qsResolution.find   ( "x" );	//ooo
	iXPos    = qsResolution.indexOf   ( "x" );	//xxx
	qsWidth  = qsResolution.left  ( iXPos );
	qsHeight = qsResolution.right ( iXPos );
	qsWidth.toInt ( &bOkay );
	if ( ! bOkay )
	  qsWidth = "720";
	qsHeight.toInt ( &bOkay );
	if ( ! bOkay )
	  qsHeight = "480";
	
	qsVideoFormat = "ntsc";
	//if ( pInterface->qsVideoFormat.lower ( ) == "pal" )	//ooo
	if ( pInterface->qsVideoFormat.toLower ( ) == "pal" )	//xxx
	  qsVideoFormat = "pal";
	qsRatio = "4:3";
	if ( ! pInterface->qsRatio.isEmpty ( ) )
	  qsRatio = pInterface->qsRatio;
	else if ( ! pInfo->qsRatio.isEmpty ( ) )
	  qsRatio = pInfo->qsRatio;

	//if ( qsRatio.find ( "Keep" ) > -1 )	//ooo
	if ( qsRatio.indexOf ( "Keep" ) > -1 )	//xxx
	   qsRatio.remove ( "Keep" );

	qsAudioList = " ";
	for ( i=1; i<(uint)iMaxAudio; i++ ) { // We should never use 0 as this is used by the original audio track.
	  pAudio = pInfo->arrayAudio [ i ];
	  if (   pAudio   )   {
	    if ( pAudio->pTranscodeInterface ) {
	      fileInfo.setFile ( pAudio->qsFileName );
	      qsFileName = Global::qsTempPath + "/" + Global::qsProjectName + "/" + fileInfo.baseName ( ) + "." + pAudio->pTranscodeInterface->qsAudioType;
	      qsAudioList += QString ( " \"" ) + qsFileName + QString ( "\"" );
	    }
	    else
	      qsAudioList += QString ( " \"" ) + pAudio->qsFileName + QString ( "\"" );
	  }
	  else {
	    // At this point we have a gap between what is in the SFEntry and what is in this SFInfo
	    // which we need to fill.
	    qsAudioList += QString ( " \"" ) + Global::qsSystemPath + "share/qdvdauthor/silence.mp2" + QString ( "\"" );
	  }
	}

	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransWidth,        qsWidth                 );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransHeight,       qsHeight                );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoFormat,  qsVideoFormat           );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoType,    pInterface->qsVideoType );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoBitrate, QString ("%1").arg(pInterface->iVideoBitrate ) );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransFrameRate,    QString ("%1").arg(pInterface->fFrameRate    ) );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransRatio,        qsRatio                 );
	m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioList,    qsAudioList             );

	for (i=0;i<(uint)pBlock->listCommands.count();i++) { //ooo
        //for (i=0;i<(uint)pBlock->listComments.count();i++) {    //xxx
	  listCommands.append( m_keyword.resolvString( Keyword::TransKeys, pBlock->listCommands[i]));
      //listComments.append( m_keyword.resolvString( Keyword::TransKeys, pBlock->listComments[i])); //oooo
	}
    for (i=0;i<(uint)pBlock->listComments.count();i++) {    //xxx
      listComments.append( m_keyword.resolvString( Keyword::TransKeys, pBlock->listComments[i]));
    }

	pExecute->listCommandList.append ( listCommands );
	pExecute->listCommentList.append ( listComments );
	pExecute->listBlockIDs.append    ( pBlock->iBlockID );
	pExecute->listDefaults.append    ( pBlock->iDefaultCommand );
	listCommands.clear ( );
	listComments.clear ( );

	for (j=0, iSplitPos=0;j<(uint)pBlock->listSubBlocks.count ();j++)	{
	  // Some fields need to be reset between MEncode / Transcode / FFMpeg ... etc.
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransRatio,        qsRatio                 );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransWidth,        qsWidth                 );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransHeight,       qsHeight                );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoFormat,  qsVideoFormat           );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoType,    pInterface->qsVideoType );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransVideoBitrate, QString ("%1").arg(pInterface->iVideoBitrate ) );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransFrameRate,    QString ("%1").arg(pInterface->fFrameRate    ) );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransRatio,        qsRatio                 );
	  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioList,    qsAudioList             );

	  pExecute->listSplitAt.append  ( iSplitPos );
	  pSubBlock = pBlock->listSubBlocks[j];
	  for (k=0;k<(uint)pSubBlock->listBlocks.count();k++)	{
	    pSubSubBlock = pSubBlock->listBlocks[k];
	    for (i=0;i<(uint)pSubSubBlock->listComments.count();i++)
	      listComments.append( m_keyword.resolvString( Keyword::TransKeys, pSubSubBlock->listComments[i] ) );
	    for (i=0;i<(uint)pSubSubBlock->listCommands.count();i++)
	      listCommands.append( m_keyword.resolvConvert( pSubSubBlock->listCommands[i], pSubBlock->qsName ) );
	    iSplitPos ++;
	    pExecute->listCommandList.append ( listCommands );
	    pExecute->listCommentList.append ( listComments );
	    pExecute->listBlockIDs.append    ( pSubSubBlock->iBlockID );
	    pExecute->listDefaults.append    ( pSubSubBlock->iDefaultCommand );
	    listCommands.clear ( );
	    listComments.clear ( );
	  }
	}

	// Next we check if the output file name already exists
	// This is to prevent re-encoding if already existant.
	fileInfo.setFile ( QString ("%1/%2.mpg").arg( Global::qsTempPath ).arg( m_keyword.value ( Keyword::TransKeys, Keyword::TransBaseName ) ) ); 
	if ( fileInfo.exists ( ) )
	  pExecute->bShowBlock = false;
	pExecute->listKeywords = m_keyword.getValues ( Keyword::TransKeys );
      }
      else	{	// and here we handle the sound sources.
	int iAudioBitrate = pInterface->iAudioBitrate;
	if ( iAudioBitrate > 2000 )
	  //iAudioBitrate = (int)( (float)iAudioBitrate / 1000.0f );                                //ooo
          iAudioBitrate = static_cast<int> ( static_cast<float> ( iAudioBitrate ) / 1000.0f );      //xxx
        m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioBitrate,QString ("%1").arg ( iAudioBitrate ) );
	if ( pEntry->listFileInfos.count ( ) > 1 ) { 
	  // this block needs to concat all audio files into one audio file first ...
	  pBlock = &(m_convertMenuBlock.blockEmptySound);
	  // And since we fold all audios into one file, we should handle it as one file.
	  t = pEntry->listFileInfos.count ( );
	}
	else
	  pBlock = &(m_convertMenuBlock.blockOthers);

	addMenuAudioBlock ( pExecute, pInterface, pEntry, pBlock );	//oooo

	// Next we check if the output file name already exists
	// This is to prevent re-encoding if already existant.
	fileInfo.setFile (QString ("%1/%2.mp2").arg(Global::qsTempPath).arg( m_keyword.value ( Keyword::TransKeys, Keyword::TransBaseName ) ) );
	if (fileInfo.exists())	
	  pExecute->bShowBlock = false;
      }
      m_listExecute.append(pExecute);
      m_listToBeDeleted.append (pExecute);
    } // end if pInterface ...

    // Next we find the first subtitle available ...
    j=0;
    pSubtitles = NULL;
    while ( ( ! pSubtitles ) && ( j < MAX_SUBTITLES ) )
      pSubtitles = pInfo->arraySubtitles[ j++ ];

    if ( pSubtitles ) {
      QFileInfo fileInfo      ( pInfo->qsFileName );
      QString qsTranscodeFile = pInfo->qsFileName;
      if ( pInterface )
        //qsTranscodeFile = m_qsTempPath + QString ( "/" ) + fileInfo.baseName ( true )  + QString ( "/" ) + fileInfo.baseName ( true ) + QString ( ".mpeg2" );	//ooo
	qsTranscodeFile = m_qsTempPath + QString ( "/" ) + fileInfo.baseName ( )  + QString ( "/" ) + fileInfo.baseName ( ) + QString ( ".mpeg2" );		//xxx

      m_keyword.clear    ( Keyword::SubKeys );
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubProjectPath,    m_qsProjectPath            );
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubInputFileName,  qsTranscodeFile            );
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubTempPath,       m_qsTempPath               );
      //m_keyword.setValue ( Keyword::SubKeys, Keyword::SubBaseName,       fileInfo.baseName  (  true ) );	//ooo
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubBaseName,       fileInfo.baseName  ( ) );		//xxx
      //m_keyword.setValue ( Keyword::SubKeys, Keyword::SubFileExt,        fileInfo.extension ( false ) );	//ooo
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubFileExt,        fileInfo.suffix ( ) );			//xxx
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubXmlFile,        pSubtitles->m_qsXmlFile      );
      m_keyword.setValue ( Keyword::SubKeys, Keyword::SubSubtitleNumber, QString ( "%1" ).arg ( pSubtitles->m_iSubtitleNumber ) );

      pBlock = &(m_subtitlesMenuBlock.blockOthers);
      pExecute = new ExecuteInterface;
      pExecute->enType = ExecuteInterface::TYPE_SUBTITLES;
      pExecute->qsBlockName = m_keyword.resolvString ( Keyword::SubKeys, m_subtitlesMenuBlock.qsBlockName );
      //      pExecute->backgroundColor = m_convertMenuBlock.colorBackground;
      if (m_arrayColors.count() > 0)
        pExecute->backgroundColor = m_arrayColors[++m_iColorCounter%m_arrayColors.count()]->color ( );
      else
        pExecute->backgroundColor.setNamedColor ( "#9EEE81" );

      listCommands.clear ( );
      listComments.clear ( );

      pExecute->listDefaults.append (pBlock->iDefaultCommand);
      pExecute->listBlockIDs.append (pBlock->iBlockID);

      for (i=0;i<(uint)pBlock->listCommands.count();i++) {
        // Here we handle the +MULTIPLE_SUBTITLE_PIPE+ token. This will add '| spumux -sN subN.xml |' per subtitle.
        qsCommand = pBlock->listCommands[i];
        handleMultipleSubtitles( pInfo, qsCommand );	//oooo
        listCommands.append ( m_keyword.resolvString ( Keyword::SubKeys, qsCommand ) );
      }
      for (i=0;i<(uint)pBlock->listComments.count();i++)
        listComments.append( m_keyword.resolvString( Keyword::SubKeys, pBlock->listComments[i] ) );
      pExecute->listCommentList.append ( listComments );
      pExecute->listCommandList.append ( listCommands );

      m_listExecute.append     ( pExecute );
      m_listToBeDeleted.append ( pExecute );
    }
  }
  // printCommandList ("appendConvert");    //ooo//
}

void QDVDAuthorInit::setAudioAttributes ( SourceFileInfo *pInfo, TranscodeInterface *pInterface, QString *pFileName, int iForEachCounter )
{
  QString qsFileName = pInfo->qsFileName;
  if ( pFileName )
      qsFileName = *pFileName;

  int  iAudioBitrate = pInterface->iAudioBitrate;
  if ( iAudioBitrate > 2000 ) 
       iAudioBitrate = (int)( (float)iAudioBitrate / 1000.0 );

  QFileInfo fileInfo ( qsFileName );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransForEachCounter, QString ( "%1" ).arg ( iForEachCounter ) );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransInputName,   qsFileName                 );
  //m_keyword.setValue ( Keyword::TransKeys, Keyword::TransBaseName,    fileInfo.baseName  (  true ) );		//ooo
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransBaseName,    fileInfo.baseName  (  ) );		//xxx
  //m_keyword.setValue ( Keyword::TransKeys, Keyword::TransFileExt,     fileInfo.extension ( false ) );		//ooo
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransFileExt,     fileInfo.suffix ( ) );			//xxx
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioFormat, pInterface->qsAudioFormat  );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioType,   pInterface->qsAudioType    );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransAudioBitrate,QString ("%1").arg( iAudioBitrate ) );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransSampleRate,  QString ("%1").arg( pInterface->iSample     ) );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransStartingAt,  QString ("%1").arg( pInterface->iStartingAt ) );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransEndingAt,    QString ("%1").arg( pInterface->iEndingAt   ) );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransDuration,    QString ("%1").arg( pInterface->iEndingAt - pInterface->iStartingAt ) );
}

void QDVDAuthorInit::appendConvertAudioTracks ( SourceFileEntry *pEntry )
{
  uint t, i, j;
  bool bFound;
  QString qsResolution, qsVideoFormat, qsRatio, qsWidth;
  QString qsAudioList, qsHeight, qsBaseName, qsCommand;

  TranscodeInterface *pInterface = NULL;
  ExecuteInterface   *pExecute   = NULL;
  SourceFileInfo     *pInfo      = NULL;
  QDVDAuthorInit::MenuBlock::Block *pBlock;
  QStringList   listCommands, listComments;
  Audio        *pAudio = NULL;

  if (!pEntry)
    return;

  // Okay, this one needs to be converted.
  m_keyword.clear    ( Keyword::TransKeys );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransDVDAuthorXML, m_qsDvdauthorFile );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransProjectPath,  m_qsProjectPath   );
  m_keyword.setValue ( Keyword::TransKeys, Keyword::TransTempPath,     m_qsTempPath      );

  for ( t=0;t<(uint)pEntry->listFileInfos.count ( ); t++ )	{	//ox
    pInfo   = pEntry->listFileInfos[t];

    for ( i=0;i<MAX_AUDIO; i++ ) {
      // Here we take care of Audio files attached to a SourceFileEntry (multiple audio channels).
      pAudio = pInfo->arrayAudio [ i ];
      if ( ( ! pAudio ) || ( pAudio->iAudioNumber < 0 ) || ( pAudio->iAudioNumber > MAX_AUDIO-1 ) || ( ! pAudio->pTranscodeInterface ) )
	continue;

      bFound = false;
      // Here we check if the same file is already being transcoded.
      for ( j=0; j<(uint)m_listAudioFiles.count ( ); j++ ) {	//ox
	if ( m_listAudioFiles[j] == pAudio->qsFileName )
	  bFound = true;
      }
      if ( bFound )
	continue;

      m_listAudioFiles.append ( pAudio->qsFileName );
      listCommands.clear ( );
      listComments.clear ( );

      pInterface = pAudio->pTranscodeInterface;
      pBlock = &(m_convertMenuBlock.blockOthers); //EmptySound);

      setAudioAttributes ( pInfo, pInterface, &pAudio->qsFileName );

      pExecute = new ExecuteInterface;
      pExecute->qsBlockName = m_keyword.resolvString ( Keyword::TransKeys, m_convertMenuBlock.qsBlockName );
      //      pExecute->backgroundColor = m_subtitlesMenuBlock.colorBackground;
      if (m_arrayColors.count() > 0)
	pExecute->backgroundColor = m_arrayColors[++m_iColorCounter%m_arrayColors.count()]->color ( );
      else
	pExecute->backgroundColor.setNamedColor ( "#9EEE81" );

      addMenuBlock ( pExecute, pBlock, Keyword::TransKeys );
      // Next we check if the output file name already exists
      // This is to prevent re-encoding if already existant.
      QFileInfo fileInfo;
      fileInfo.setFile ( QString ("%1/%2.mp2").arg(Global::qsTempPath).arg( m_keyword.value ( Keyword::TransKeys, Keyword::TransBaseName ) ) );
      if ( fileInfo.exists ( ) )
        pExecute->bShowBlock = false;

      m_listExecute.append     ( pExecute );
      m_listToBeDeleted.append ( pExecute );
    }
  }
}

void QDVDAuthorInit::handleMultipleSubtitles ( SourceFileInfo *pInfo, QString &qsCommand )
{
  QString qsToken ( "+MULTIPLE_SUBTITLE_PIPE+" );
  QString qsTemp, qsXmlFile;
  Subtitles *pSubtitles = NULL;
  int t, iSub = 0;

  while ( ( ! pSubtitles ) && ( iSub < MAX_SUBTITLES ) )
    pSubtitles = pInfo->arraySubtitles[ iSub++ ];

  if ( pSubtitles ) {
    // The first sbtitle is already handled ... now come the rest ...
    for ( t=iSub; t<MAX_SUBTITLES; t++ ) {
      pSubtitles = pInfo->arraySubtitles[ t ];

      if ( pSubtitles )  { 
	if ( pSubtitles->m_qsXmlFile.isEmpty ( ) ) {
	  Utils theUtils;
	  QFileInfo fileInfo ( pInfo->qsFileName );
	  qsXmlFile.sprintf ( "/subtitle_%d.xml", pSubtitles->m_iSubtitleNumber );
	  //qsXmlFile = theUtils.getTempFile ( fileInfo.baseName ( TRUE ) ) + qsXmlFile;	//ooo
	  qsXmlFile = theUtils.getTempFile ( fileInfo.baseName ( ) ) + qsXmlFile;		//xxx
	  pSubtitles->m_qsXmlFile = qsXmlFile;
	}
	qsTemp += QString ( " | spumux -s %1 \"%2\"" ).arg ( pSubtitles->m_iSubtitleNumber ).arg ( pSubtitles->m_qsXmlFile );
      }
    }
  }

  qsCommand.replace ( qsToken, qsTemp );
}

void QDVDAuthorInit::setMenuAttributes ( DVDMenu *pMenu, QString qsBackgroundFileName )
{
  // This function will generate the Commands needed per Menu.
  // The information comes from the menu on the one hand and on the other 
  // it comes from the init - file. This is the mating ground ...
  uint iNrOfFrames;
  QTime zeroTime;

  // First things first, here we determine the Format ...
  // iFormat > 4 equals PAL, otherwise NTSC
  int iFormat = pMenu->getFormat ( pMenu->getInterface ( )->iWidth, pMenu->getInterface ( )->iHeight );
  iNrOfFrames = zeroTime.msecsTo ( pMenu->getInterface ( )->timeDuration );

  // Next is to create the keyword list ... needed for resolvString() ...
  m_keyword.clear    ( Keyword::Keys );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyDVDAuthorXML, m_qsDvdauthorFile );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyProjectPath,  m_qsProjectPath   );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyTempPath,     m_qsTempPath      );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyWidth,  QString ("%1").arg(pMenu->getInterface()->iWidth  ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyHeight, QString ("%1").arg(pMenu->getInterface()->iHeight ) );

  if ( iFormat > FORMAT_NTSC4 )  {  // PAL
    iNrOfFrames = (uint)( (double)iNrOfFrames / 1000.0 * 25.0 );
    if ( iNrOfFrames < 25 )
         iNrOfFrames = 25;
    m_keyword.setValue ( Keyword::Keys, Keyword::KeyFormat, "PAL"  );
  }
  else	{
    iNrOfFrames = (uint)( (double)iNrOfFrames / 1000.0 * 29.97 );
    if ( iNrOfFrames < 30 )
         iNrOfFrames = 30;
    m_keyword.setValue ( Keyword::Keys, Keyword::KeyFormat, "NTSC" );
  }
  QFileInfo fileInfo ( qsBackgroundFileName );
  // AspectID is for mpeg2enc
  int iAspectID = 2; // 4:3
  if ( pMenu->getInterface()->qsRatio == "16:9" )
    iAspectID = 3;
  else if ( pMenu->getInterface()->qsRatio == "2.21:1" )
    iAspectID = 4;

  m_keyword.setValue ( Keyword::Keys, Keyword::KeyMenuName,   pMenu->name ( ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyStartPos,   pMenu->getInterface( )->timeOffset.toString   ( ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyDuration,   pMenu->getInterface( )->timeDuration.toString ( ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyNrOfFrames, QString ( "%1" ).arg ( iNrOfFrames ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyAspectRatio,QString ( "%1" ).arg ( iAspectID ) );

  m_keyword.setValue ( Keyword::Keys, Keyword::KeyBackgroundFileName, fileInfo.fileName ( ) );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeyBackgroundFullName, qsBackgroundFileName );
  m_keyword.setValue ( Keyword::Keys, Keyword::KeySoundList, " " ); 
}

void QDVDAuthorInit::appendButtonTransitions ( DVDMenu *pMenu )
{
  MenuBlock          *pMenuBlock  = &m_slideshowMenuBlock;
  ButtonObject       *pButton     = NULL;
  ButtonTransition   *pTransition = NULL;
  ExecuteInterface   *pInterface  = NULL;
  QList<ButtonObject *> list = pMenu->getButtons ( );		//oxx
  QList<ButtonObject *>::iterator it =list.begin ( );		//oxx
  while ( it != list.end ( ) )  {
    pButton = *it++;
    pTransition = pButton->getTransition ( );
    if ( pTransition && ! pTransition->bFinishedRendering && ! pTransition->bUserFile )  {
      pInterface = new ExecuteInterface;
      pInterface->enType = ExecuteInterface::TYPE_SLIDESHOW;
      pInterface->qsBlockName = m_keyword.resolvString ( Keyword::Keys, pMenuBlock->qsBlockName );
      // pInterface->backgroundColor = pMenuBlock->colorBackground;
      if ( m_arrayColors.count ( ) > 0 )
        pInterface->backgroundColor = m_arrayColors[++m_iColorCounter%m_arrayColors.count()]->color ( );
      else
        pInterface->backgroundColor.setNamedColor ( "#9EEE81" );

      if ( m_iColorCounter > (int)m_arrayColors.count ( ) )
           m_iColorCounter = 0;

      m_keyword.setValue ( Keyword::Keys, Keyword::KeyButtonName, pButton->name ( ) );
      addBlock ( pInterface, &pMenuBlock->blockMovieBackground );

      pInterface->bShowBlock   = !pMenuBlock->bHidden;
      pInterface->listKeywords = m_keyword.getValues ( Keyword::Keys );
      m_listExecute.append     ( pInterface );
      m_listToBeDeleted.append ( pInterface );
    }
  }
}

void QDVDAuthorInit::appendMenu ( DVDMenu *pMenu, QString qsMenuName, QString qsBackgroundFileName )
{
  if ( ! pMenu )
    return;

  setMenuAttributes       ( pMenu, qsBackgroundFileName );
  appendButtonTransitions ( pMenu );		//oooo

  uint t, i;
  MenuBlock           *pMenuBlock = NULL;
  MenuBlock::Block    *pBlock     = NULL;
  MenuBlock::SubBlock *pSubBlock  = NULL;
  ExecuteInterface    *pInterface = new ExecuteInterface;

  int iFormat = pMenu->getFormat ( pMenu->getInterface ( )->iWidth, pMenu->getInterface ( )->iHeight );

  if ( iFormat > FORMAT_NTSC4 )  // PAL
    pMenuBlock = &m_palMenuBlock;
  else
    pMenuBlock = &m_ntscMenuBlock;

  pInterface->qsBlockName = m_keyword.resolvString ( Keyword::Keys, pMenuBlock->qsBlockName );
  //  pInterface->backgroundColor = pMenuBlock->colorBackground;
  if (m_arrayColors.count() > 0)
    pInterface->backgroundColor = m_arrayColors[++m_iColorCounter%m_arrayColors.count()]->color ( );
  else
    pInterface->backgroundColor.setNamedColor ( "#9EEE81" );

  if ( m_iColorCounter > (int)m_arrayColors.count ( ) )
       m_iColorCounter = 0;

  // Here we check if the background is a movie or an image.
  QFileInfo backgroundInfo (qsBackgroundFileName);
  pBlock = &pMenuBlock->blockImageBackground;

  MediaInfo *pMediaInfo = pMenu->getDVDAuthor()->getMediaInfo ();

  QString qsExtensions  = pMediaInfo->getExtensions (true);		//ooo

  m_soundList.clear ( );
  if ( pMenu->getInterface ( )->listSoundEntries.count ( ) == 0 )  {
    // This is the only instance where we need a hardcoded naming scheme ...
    m_soundList.append (QString ("\"%1/menu.mp2\"").arg(qsMenuName));
    m_keyword.setValue ( Keyword::Keys, Keyword::KeySoundList, m_soundList[0] );
    //addBlock(pInterface, &pMenuBlock->blockEmptySound);
  }
  else	{
    QString qsConvertedSound;
    for ( t=0; t<(uint)pMenu->getInterface ( )->listSoundEntries.count ( ); t++ )  {
      Audio   *pAudio         = pMenu->getInterface ( )->listSoundEntries[t];
      QString  qsEntryName    = pAudio->qsFileName;
      SourceFileEntry *pEntry = pMenu->getDVDAuthor ( )->getSourceEntryByDisplayName ( qsEntryName );
      if ( ! pEntry )
	continue;
      if ( pEntry->listFileInfos.count ( ) > 1 ) {
	qsConvertedSound  = qsEntryName.remove ( QRegExp ( "[\\[\\]<>]" ) );
	qsConvertedSound += ".mp2";
	m_soundList.append ( QString ( "\"%1/%2\"").arg ( m_qsTempPath ).arg ( qsConvertedSound ) );
      }
      else if ( pEntry->listFileInfos.count ( ) == 1 ) {
	SourceFileInfo *pInfo = pEntry->listFileInfos[0];
	if ( pInfo->pTranscodeInterface ) {
	  if ( pInfo->pTranscodeInterface->qsTempFile.isEmpty ( ) ) {
	    QFileInfo fileInfo ( pInfo->qsFileName );
	    m_soundList.append ( QString ("\"%1/%2.mp2\"").arg ( m_qsTempPath ).arg ( fileInfo.baseName ( ) ) );
	  }
	  else
	    m_soundList.append ( QString ("\"%1\"").arg ( pInfo->pTranscodeInterface->qsTempFile ) );
	}
	else
	  m_soundList.append ( QString ( "\"%1\"" ).arg ( pInfo->qsFileName ) );
      }
    }
    m_keyword.setValue ( Keyword::Keys, Keyword::KeySoundList, m_soundList.join (" ") );
  }

  //if (qsExtensions.find (QString (".")+backgroundInfo.extension(FALSE)) > -1)	{	//ooo
  if (qsExtensions.indexOf (QString (".")+backgroundInfo.suffix( )) > -1)	{	//xxx
    // Okay we have a movieBackground. Now every frame of that background has to be rendered
    // so we need to run quite some commands in here ...
    // Fortunately all we need is stored in the blockMovieBackground structure.
    for (t=0;t<(uint)pMenuBlock->blockMovieBackground.listSubBlocks.count();t++)	{
      pSubBlock = pMenuBlock->blockMovieBackground.listSubBlocks[t];
      for (i=0;i<(uint)pSubBlock->listBlocks.count();i++)	{
        addBlock (pInterface, pSubBlock->listBlocks[i]);
      }
    }
    // After we have the movie extracted, we check if the user added sound files
    if ( (pMenu->getInterface()->bMovieBackgroundHasSound) && (pMenu->getInterface()->listSoundEntries.count() == 0) )	{
      QString qsBackgroundAudio = QString ("%1/%2/clip.wav").arg(qsMenuName).arg(backgroundInfo.fileName ());
      backgroundInfo.setFile(qsBackgroundAudio);
      Audio *pAudio      = new Audio;
      pAudio->qsFileName = qsBackgroundAudio;
      pMenu->getInterface()->listSoundEntries.append ( pAudio );
    }
  }
  else if ( pMenu->isMovieMenu ( ) )  {
    // The difference here is that the background is a image
    // So we need everything from the point of the rendering of the menu.
    bool bAddBlocks = false;
    for ( t=0;  t<(uint)pMenuBlock->blockMovieBackground.listSubBlocks.count ( );t++ )  {
      pSubBlock = pMenuBlock->blockMovieBackground.listSubBlocks[t];
      pBlock = pSubBlock->listBlocks[0];
      if (!bAddBlocks) {  // Only need to check up till we find the render command
	for (i=0;i<(uint)pBlock->listCommands.count();i++)	{
	  // Check every command for the RENDER_MENU - string
	  //if ( pBlock->listCommands[i].find ( QString ( RENDER_MENU ) ) > -1 )	//ooo
	  if ( pBlock->listCommands[i].indexOf ( QString ( RENDER_MENU ) ) > -1 )	//xxx
	    bAddBlocks = true;
	}
      }
      if ( bAddBlocks )
	addBlock ( pInterface, pBlock );
    }
  }		//oooo
  else  // blockImageBackground ( for static backgrounds ) 
    addBlock ( pInterface, pBlock );

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // the following was a trial to give a second choice to render static menu backgrounds.
  // because the way the command generation is currently strutured this would have broken
  // all dynamic logic. I.e. MovieBackgrounds, backgrounds with audio added etc.
  //
  // else
  //    addSubBlock ( pInterface, pBlock, Keyword::Keys, false );
  // Does no longer work because the commands are now stored as SubBlocks within the imageBackground
  // Thus for static backgrounds need to find a way to leave emptySound out if audio attached to Menu
  //
  // For MovieBackground need to find a way to get to the EmptySound, Mplex, and Spumux blocks,
  // which are now SubBlocks and no longer accesible this way ( commands are empty )
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( pMenu->getInterface ( )->listSoundEntries.count ( ) == 0 )
    addBlock ( pInterface, &pMenuBlock->blockEmptySound );

  addBlock ( pInterface, &pMenuBlock->blockMplex  );
  addBlock ( pInterface, &pMenuBlock->blockSpumux );

  pInterface->bShowBlock   = !pMenuBlock->bHidden;
  pInterface->listKeywords = m_keyword.getValues ( Keyword::Keys );
  m_listExecute.append     ( pInterface );
  m_listToBeDeleted.append ( pInterface );
}

void QDVDAuthorInit::appendPostProcessing ()
{
  // Here we check if transcoding was requested ...
  if ( m_convertInterface.listCommandList.count ( ) < 1 )
    //m_listExecute.remove ( &m_convertInterface );		//ooo
    m_listExecute.removeOne ( &m_convertInterface );		//xxx
  if ( m_slideshowInterface.listCommandList.count ( ) < 1 )
    //m_listExecute.remove ( &m_slideshowInterface );		//ooo
    m_listExecute.removeOne ( &m_slideshowInterface );		//xxx

  m_listExecute.append(&m_dvdauthorInterface);
  m_listExecute.append(&m_burnInterface);
}

void QDVDAuthorInit::addMenuAudioBlock ( ExecuteInterface *pExecute, TranscodeInterface *pInterface, SourceFileEntry *pEntry, MenuBlock::Block *pBlock )
{
  QDVDAuthorInit::MenuBlock::Block    *pSubSubBlock;
  QDVDAuthorInit::MenuBlock::SubBlock *pSubBlock;
  uint t, j, k;

  addBlock ( pExecute, pBlock, Keyword::TransKeys );
  for (j=0; j<(uint)pBlock->listSubBlocks.count (); j++)	{	//ox
    pSubBlock = pBlock->listSubBlocks[j];
    for (k=0;k<(uint)pSubBlock->listBlocks.count();k++)	{	//ox
      pSubSubBlock = pSubBlock->listBlocks[k];
      if ( pSubBlock->iForEachInput == -1 )
	addBlock ( pExecute, pSubSubBlock, Keyword::TransKeys );
      else {
      	for ( t=pSubBlock->iForEachInput;t<(uint)pEntry->listFileInfos.count ( ); t++ ) {	//ox
          setAudioAttributes ( pEntry->listFileInfos[t], pInterface, NULL, t ); //pEntry->pTranscodeInterface );
      	  addBlock ( pExecute, pSubSubBlock, Keyword::TransKeys );
      	}
      	setAudioAttributes ( pEntry->listFileInfos[0], pInterface ); //pEntry->pTranscodeInterface );
      }
    }
  }
}

void QDVDAuthorInit::addMenuBlock ( ExecuteInterface *pInterface, MenuBlock::Block *pBlock, Keyword::enKeyBase enWhichKeys )
{
  QDVDAuthorInit::MenuBlock::Block    *pSubSubBlock;
  QDVDAuthorInit::MenuBlock::SubBlock *pSubBlock;
  uint j, k;

  addBlock ( pInterface, pBlock, enWhichKeys );
  for (j=0; j<(uint)pBlock->listSubBlocks.count (); j++)	{	//ox
    pSubBlock = pBlock->listSubBlocks[j];
    for (k=0;k<(uint)pSubBlock->listBlocks.count();k++)	{	//ox
      pSubSubBlock = pSubBlock->listBlocks[k];
      addBlock ( pInterface, pSubSubBlock, enWhichKeys );
    }
  }
}

void QDVDAuthorInit::addSubBlock ( ExecuteInterface *pInterface, MenuBlock::Block *pBlock, Keyword::enKeyBase enWhichKeys )
{
  // This function will read in all Bloicks / SubBlocks and build the appropriate 
  // controls. E.g. the Transcoding interface in the CommandQueueDialog.
  unsigned int i, j, k, iSplitPos;
  QStringList listComments, listCommands;
  MenuBlock::SubBlock *pSubBlock;
  MenuBlock::Block *pSubSubBlock;

  // Fisrt we add the top most blocks ( non editable selection combo boxes )
  addBlock ( pInterface, pBlock );
  // The we parse through the SubBlocks and extract the SubSubBlocks
  for (j=0, iSplitPos=0;j<(uint)pBlock->listSubBlocks.count ();j++)	{   //ox
    pInterface->listSplitAt.append  ( iSplitPos );
    pSubBlock = pBlock->listSubBlocks[j];
    for (k=0;k<(uint)pSubBlock->listBlocks.count();k++)	{   //ox
      pSubSubBlock = pSubBlock->listBlocks[k];
      for (i=0;i<(uint)pSubSubBlock->listComments.count();i++)  //ox
	listComments.append( m_keyword.resolvString( enWhichKeys, pSubSubBlock->listComments[i] ) );
      for (i=0;i<(uint)pSubSubBlock->listCommands.count();i++)  //ox
	listCommands.append( m_keyword.resolvString( enWhichKeys, pSubSubBlock->listCommands[i] ) );
      iSplitPos ++;
      pInterface->listCommandList.append ( listCommands );
      pInterface->listCommentList.append ( listComments );
      pInterface->listBlockIDs.append    ( pSubSubBlock->iBlockID );
      pInterface->listDefaults.append    ( pSubSubBlock->iDefaultCommand );
      listCommands.clear ( );
      listComments.clear ( );
    }
  }
}

void QDVDAuthorInit::addBlock ( ExecuteInterface *pInterface, MenuBlock::Block *pBlock, Keyword::enKeyBase enWhichKeys )
{
  uint t;
  QStringList listComments, listCommands;
  // Currently not supported more then one comment per block ...
  if ( ( pBlock->listComments.count ( ) < 1 ) && ( pBlock->listCommands.count ( ) < 1 ) )
    return;

  for ( t=0; t<(uint)pBlock->listComments.count ( ); t++ )
	listComments.append( m_keyword.resolvString( enWhichKeys, pBlock->listComments[t] ) );
  for ( t=0; t<(uint)pBlock->listCommands.count ( ); t++ )
	listCommands.append( m_keyword.resolvString( enWhichKeys, pBlock->listCommands[t] ) );
  pInterface->listCommentList.append ( listComments );
  pInterface->listCommandList.append ( listCommands );
  pInterface->listDefaults.append    ( pBlock->iDefaultCommand );
  pInterface->listBlockIDs.append    ( pBlock->iBlockID );
}

/**
 * This function will set the ini - file to the default values.
 */
void QDVDAuthorInit::resetInitFile ()
{
  QString qsPreferedMediaEngine = "QMPlayerWidget";
#ifndef MPLAYER_SUPPORT
  qsPreferedMediaEngine = "KXineWidget";
  #ifndef XINE_SUPPORT
    qsPreferedMediaEngine = "QVLCWidget";
    #ifndef VLC_SUPPORT
      qsPreferedMediaEngine = "DummyWidget";
    #endif
  #endif
#endif

  Utils theUtils;
  QString qsToolameSample, qsPPMTOY4M;
  qsToolameSample = theUtils.getDefaultParameter ( Utils::twolameSampleRate );
  qsPPMTOY4M      = theUtils.getDefaultParameter ( Utils::ppmtoy4mChroma ); // For mjpegtools versions smaller than 1.8.0, use '-S 420_mpeg2' instead of '-S 420mpeg2' in your 'ppmtoy4m' line

  //QString qsInitFile = QDir::homeDirPath();	//ooo
  QString qsInitFile = QDir::homePath();	//xxx
  qsInitFile.append(INIT_DIR_NAME);
  qsInitFile.append(INIT_FILE_NAME);
  QString qsBackupFileName = qsInitFile + QString ( ".old" );

  QDir  theDir;
  QFile theFile ( qsInitFile );
  if  ( theFile.exists  ( )  ) { 
    //theFile.setName ( qsBackupFileName );	//ooo
    theFile.setFileName ( qsBackupFileName );	//xxx
//    if ( theFile.exists ( )  )
//      theDir.remove ( qsBackupFileName );
    theDir.rename   ( qsInitFile, qsBackupFileName );
  }
  QFile initFile   ( qsInitFile );
  if ( initFile.open( QIODevice::WriteOnly ) )	{
    QTextStream stream( &initFile );
    QString qsResetInitFile = QString ("<QDVDAuthorInitFile version=\"%1\" delete_older_than=\"30\" first_start=\"1\" >\n" ).arg ( (float)INIT_VERSION );
    qsResetInitFile += QString (
" <PreferredEngine>"+qsPreferedMediaEngine+"</PreferredEngine>\n"
"  <Color number=\"0\">#9EEE81</Color>\n"
"  <Color number=\"1\">#8FEED3</Color>\n"
"  <Color number=\"2\">#E0EE8F</Color>\n"
"  <Color number=\"3\">#ED97EE</Color>\n"
"  <Color number=\"4\">#9EEE81</Color>\n"
"  <Color number=\"5\">#8FEED3</Color>\n"
"  <Color number=\"6\">#E0EE8F</Color>\n"
"  <Color number=\"7\">#ED97EE</Color>\n"
"  <Color number=\"8\">#9EEE81</Color>\n"
"  <Color number=\"9\">#8FEED3</Color>\n"
"  <Color number=\"10\">#E0EE8F</Color>\n"
"  <Color number=\"11\">#F92F5E</Color>\n"
" <Path name=\"bash\" ></Path>\n"
" <Path name=\"dvdauthor\" ></Path>\n"
" <Path name=\"spumux\" ></Path>\n"
" <Path name=\"mplex\" ></Path>\n"
" <Path name=\"arecord\" ></Path>\n"
" <Path name=\"transcode\" ></Path>\n"
" <Path name=\"movie-to-dvd\" ></Path>\n"
" <Path name=\"png2yuv\" ></Path>\n"
" <Path name=\"oggdec\" ></Path>\n"
" <Path name=\"mp2enc\" ></Path>\n"
" <Path name=\"lame\" ></Path>\n"
" <Path name=\"mplayer\" ></Path>\n"
" <Path name=\"mencoder\" ></Path>\n"
" <Path name=\"jpegtopnm\" ></Path>\n"
" <Path name=\"ppmtoy4m\" ></Path>\n"
" <Path name=\"mpeg2enc\" ></Path>\n"
" <Path name=\"jpeg2yuv\" ></Path>\n"
" <Path name=\"pcm2aiff\" ></Path>\n"
" <Path name=\"twolame\" ></Path>\n"
" <Path name=\"mctoolame\" ></Path>\n"
" <Path name=\"dd\" ></Path>\n"
" <Path name=\"dvd-slideshow\" ></Path>\n"
" <Path name=\"sox\" ></Path>\n"
" <Path name=\"pngtopnm\" ></Path>\n"
" <Path name=\"convert\" ></Path>\n"
" <Path name=\"growisofs\" ></Path>\n"
" <Path name=\"dvd+rw-format\" ></Path>\n"
" <Path name=\"mkisofs\" ></Path>\n"
" <Path name=\"dvdrecord\" ></Path>\n"
" <Path name=\"wodim\" ></Path>\n"
"  <DragNDrop start_offset=\"00:00:00\" thumbnail_size=\"50x50\" button_type=\"1\" text_pos=\"1\" duration=\"00:00:00\" color=\"#ff0000\" with_text=\"1\" button_size=\"150x150\" >\n"
"    <Font>Sans Serif,24,-1,5,75,0,0,0,0,0</Font>\n"
"  </DragNDrop>\n"
" <ExecuteInterface>\n"
"  <Convert color=\"#f92f5e\" blockname=\"Conversion Block\" >\n"

// Here we do the default transcoding logic.
"   <Block editable=\"false\" hidden=\"0\" name=\"MOVIE_BACKGROUND\" >\n"
"    <Comment>Videotrans is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>Transcode is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>Mencoder is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>FFMpeg is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Command>"CONVERT_VIDEOTRANS" +INPUT_NAME+</Command>\n"
"    <Command>"CONVERT_TRANSCODE" +INPUT_NAME+</Command>\n"
"    <Command default=\"true\" >"CONVERT_MENCODER" +INPUT_NAME+</Command>\n"
"    <Command>"CONVERT_FFMPEG" +INPUT_NAME+</Command>\n"
"    <Command>+BASE_NAME+.+FILE_EXT+</Command>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_VIDEOTRANS"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_VIDEOTRANS"\" >\n"
"      <Comment>The following line will trancode the video (+BASE_NAME+.+FILE_EXT+)</Comment>\n"
"      <Command default=\"true\" >movie-to-dvd -M -c +AUDIO_TYPE+ -a +RATIO+ -f audio -m +VIDEO_FORMAT+ -o \"+TEMP_PATH+/+BASE_NAME+/\" \"+INPUT_NAME+\" &amp;&amp; mv \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.vob\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"      <Command>movie-to-dvd -M -c +AUDIO_TYPE+ -a +RATIO+ -f video -m +VIDEO_FORMAT+ -o \"+TEMP_PATH+/+BASE_NAME+/\" \"+INPUT_NAME+\" &amp;&amp; mv \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.vob\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_TRANSCODE"\" >\n"
"     <Block editable=\"false\" hidden=\"1\" name=\""SUB_BLOCK_TRANSCODE"1\" >\n"
"      <Comment>This will convert the input movie file from +FILE_EXT+ - format into DVD (+VIDEO_FORMAT+) conforming material.</Comment>\n"
"      <Command>transcode -i \"+INPUT_NAME+\" -x ffmpeg -V -w +VIDEO_BITRATE+ --encode_fields b --export_asr +RATIO+ -F \"8,-c -q 6 -4 2 -2 1 -N 0.5 -E -10\" -K tmpgenc -R 2 -y mpeg2enc,mp2enc -b +AUDIO_BITRATE+ -m \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" --print_status 10</Command>\n"
"      <Command default=\"true\" >transcode -i \"+INPUT_NAME+\" -y ffmpeg --export_prof dvd -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_TRANSCODE"2\" >\n"
"      <Comment>Here we check if the input had an audio channel attached.</Comment>\n"
"      <Command default=\"true\" >if [ ! -e \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" ]; then cp " + Global::qsSystemPath + "/share/qdvdauthor/silence.mp2 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" ; fi; </Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_TRANSCODE"3\" >\n"
"      <Comment>And then we multiplex the audio and video stream back into one file.</Comment>\n"
"      <Command default=\"true\" >mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_MENCODER"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"1\" >\n"
"      <Comment>This line will transcode the input file ( +BASE_NAME+.+FILE_EXT+ )</Comment>\n"
"      <Command default=\"true\" >mencoder -oac lavc -ovc lavc -ss +STARTING_AT+ -endpos +ENDING_AT+ -mpegopts format=dvd:tsaf -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:aspect=+RATIO+:vrc_maxrate=9800:vbitrate=+VIDEO_BITRATE+:keyint=15:acodec=+AUDIO_TYPE+:abitrate=+AUDIO_BITRATE+ -vf scale=+WIDTH+:+HEIGHT+,harddup -srate +SAMPLE_RATE+ -ofps +FRAME_RATE+ -of mpeg -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+INPUT_NAME+\" </Command>\n"
"      <Command>mencoder -oac lavc -ovc lavc -ss +STARTING_AT+ -endpos +ENDING_AT+ -mpegopts format=dvd:tsaf -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:aspect=+RATIO+:vrc_maxrate=9800:vbitrate=+VIDEO_BITRATE+:keyint=15:acodec=+AUDIO_TYPE+:abitrate=+AUDIO_BITRATE+ -vf scale=+WIDTH+:+HEIGHT+,harddup -srate +SAMPLE_RATE+ -af lavcresample=+SAMPLE_RATE+ -ofps +FRAME_RATE+ -of mpeg -o  \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+INPUT_NAME+\" </Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_FFMPEG"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"1\" >\n"
"      <Comment>Here we create an mpeg2 stream from the input material.</Comment>\n"
"      <Command dafault=\"true\" >ffmpeg -i \"+INPUT_NAME+\" -target +VIDEO_FORMAT+-dvd -ss +STARTING_AT+ -t +DURATION+ -vcodec mpeg2video -b +VIDEO_BITRATE+ -acodec ac3 -ab +AUDIO_BITRATE+ -ar +SAMPLE_RATE+ -s +WIDTH+x+HEIGHT+ -r +FRAME_RATE+ -aspect +RATIO+ -y \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"      <Command>ffmpeg -i \"+INPUT_NAME+\" -target +VIDEO_FORMAT+-dvd \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"   </Block>\n"

// Here we handle adding multiple audio streams to an existing video
"   <Block editable=\"false\" hidden=\"0\" name=\"IMAGE_BACKGROUND\" >\n"
"    <Comment>Videotrans is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>Transcode is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>Mencoder is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Comment>FFMpeg is a tool to convert the input movie file into a DVD (+VIDEO_FORMAT+) conforming VOB stream.</Comment>\n"
"    <Command>"CONVERT_VIDEOTRANS" +INPUT_NAME+</Command>\n"
"    <Command>"CONVERT_TRANSCODE" +INPUT_NAME+</Command>\n"
"    <Command default=\"true\" >"CONVERT_MENCODER" +INPUT_NAME+</Command>\n"
"    <Command>"CONVERT_FFMPEG" +INPUT_NAME+</Command>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_VIDEOTRANS"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_VIDEOTRANS"\" >\n"
"      <Comment>First we create the elementary streams (video and audio).</Comment>\n"
"      <Command default=\"true\" >movie-to-dvd -a +RATIO+ -c +AUDIO_TYPE+ -f audio -m +VIDEO_FORMAT+ -o \"+TEMP_PATH+/+BASE_NAME+/\" \"+INPUT_NAME+\"</Command>\n"
"      <Command>movie-to-dvd -c +AUDIO_TYPE+ -M -a +RATIO+ -f audio -m +VIDEO_FORMAT+ -o \"+TEMP_PATH+/+BASE_NAME+/\" \"+INPUT_NAME+\" &amp;&amp; mv \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.vob\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_VIDEOTRANS"1\" >\n"
"      <Comment>Then we need to multiplex all video and audio streams back together.</Comment>\n"
"      <Command default=\"true\" >mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" +AUDIO_LIST+</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_TRANSCODE"\" >\n"
"     <Block editable=\"false\" hidden=\"1\" name=\""SUB_BLOCK_TRANSCODE"1\" >\n"
"      <Comment>This will convert the input movie file from +FILE_EXT+ - format into DVD (+VIDEO_FORMAT+) conforming material.</Comment>\n"
"      <Command>transcode -i \"+INPUT_NAME+\" -x ffmpeg -V -w +VIDEO_BITRATE+ --encode_fields b --export_asr +RATIO+ -F \"8,-c -q 6 -4 2 -2 1 -N 0.5 -E -10\" -K tmpgenc -R 2 -y mpeg2enc,mp2enc -b +AUDIO_BITRATE+ -m \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" --print_status 10</Command>\n"
"      <Command default=\"true\" >transcode -i \"+INPUT_NAME+\" -y ffpeg --export_prof dvd -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_TRANSCODE"2\" >\n"
"      <Comment>Here we check if the input had an audio channel attached.</Comment>\n"
"      <Command default=\"true\" >if [ ! -e \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" ]; then cp " + Global::qsSystemPath + "/share/qdvdauthor/silence.mp2 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\" ; fi; </Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_TRANSCODE"3\" >\n"
"      <Comment>And then we multiplex the audio and video streams back into one file.</Comment>\n"
"      <Command default=\"true\" >mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpa\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_MENCODER"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"1\" >\n"
"      <Comment>Here we create an mpeg2 stream from the input material.</Comment>\n"
"      <Command default=\"true\" >mencoder -oac lavc -ovc lavc -ss +STARTING_AT+ -endpos +ENDING_AT+ -mpegopts format=dvd:tsaf -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:aspect=+RATIO+:vrc_maxrate=9800:vbitrate=+VIDEO_BITRATE+:keyint=15:acodec=+AUDIO_TYPE+:abitrate=+AUDIO_BITRATE+ -vf scale=+WIDTH+:+HEIGHT+,harddup -srate +SAMPLE_RATE+ -af lavcresample=+SAMPLE_RATE+ -ofps +FRAME_RATE+ -of mpeg -o  \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+INPUT_NAME+\" </Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"2\" >\n"
"      <Comment>We take the mpeg2 file and extract the elementary streams (audio and video).</Comment>\n"
"      <Command default=\"true\" >mplayer -dumpfile \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" -dumpvideo \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" &amp;&amp; mplayer -dumpfile \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" -dumpaudio \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"      <Command>demux \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" 0xe0 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" &amp;&amp; demux \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" 0xbd \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" 0x80</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"3\" >\n"
"      <Comment>And then we multiplex the video stream and all audio streams back into one VOB file.</Comment>\n"
"      <Command default=\"true\" >mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" +AUDIO_LIST+ </Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_FFMPEG"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"1\" >\n"
"      <Comment>First we extract the Audio from the input.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+INPUT_NAME+\" -vn -acodec +AUDIO_TYPE+ -ar +SAMPLE_RATE+ -ab +AUDIO_BITRATE+ -y \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"2\" >\n"
"      <Comment>Next we extract the Video from the input.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+INPUT_NAME+\" -an -acodec copy -y -qscale 0 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"3\" >\n"
"      <Comment>And then we multiplex the video stream and all audio streams back into one VOB file.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" -i \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" +AUDIO_LIST+ -aspect +RATIO+ -target +VIDEO_FORMAT+-dvd \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"   </Block>\n"

// Here we simply re-multiplex a valid mpeg2 stream to add VOBUs to make it VOB compliant.
"   <Block editable=\"false\" hidden=\"0\" name=\"MPLEX\" >\n"
"    <Comment>Here we simply take the input video and re-multiplex it</Comment>\n"
"    <Command>"CONVERT_MENCODER" +INPUT_NAME+</Command>\n"
"    <Command default=\"true\" >"CONVERT_FFMPEG" +INPUT_NAME+</Command>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_MENCODER"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"1\" >\n"
"      <Comment>We take the mpeg2 file and extract the Elementary streams (audio and video).</Comment>\n"
"      <Command default=\"true\" >mplayer -dumpfile \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" -dumpvideo \"+INPUT_NAME+\" &amp;&amp; mplayer -dumpfile \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2a\" -dumpaudio \"+INPUT_NAME+\"</Command>\n"
"      <Command>demux \"+INPUT_NAME+\" 0xe0 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" &amp;&amp; demux \"+INPUT_NAME+\" 0xbd \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2a\" 0x80</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_MENCODER"2\" >\n"
"      <Comment>And then we multiplex the video stream and all audio streams back into one file.</Comment>\n"
"      <Command default=\"true\" >mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2a\" +AUDIO_LIST+ </Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"0\" name=\""SUB_BLOCK_FFMPEG"\" >\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"1\" >\n"
"      <Comment>First we extract the Audio from the input.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+INPUT_NAME+\" -vn -acodec +AUDIO_TYPE+ -ar +SAMPLE_RATE+ -ab +AUDIO_BITRATE+ -y \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"2\" >\n"
"      <Comment>Next we extract the Video from the input.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+INPUT_NAME+\" -an -acodec copy -y -qscale 0 \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\"</Command>\n"
"     </Block>\n"
"     <Block editable=\"false\" hidden=\"0\" name=\""SUB_BLOCK_FFMPEG"3\" >\n"
"      <Comment>And then we multiplex the video stream and all audio streams back into one VOB file.</Comment>\n"
"      <Command default=\"true\" >ffmpeg -i \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.+AUDIO_TYPE+\" -i \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.m2v\" -aspect +RATIO+ -target +VIDEO_FORMAT+-dvd -ar +SAMPLE_RATE+ -ab +AUDIO_BITRATE+ -y \"+TEMP_PATH+/+BASE_NAME+/+BASE_NAME+.mpeg2\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"   </Block>\n"

// This is to convert input audio files into DVD compliant formats.
"   <Block editable=\"false\" hidden=\"0\" name=\"OTHERS\" >\n"
"    <Comment>This line converts the input audio file into a DVD conforming mp2 file.</Comment>\n"
"    <Command default=\"true\" >if [ ! -e \"+TEMP_PATH+/qdvd.fifo\" ]; then mkfifo -m 777 \"+TEMP_PATH+/qdvd.fifo\"; fi; cat \"+TEMP_PATH+/qdvd.fifo\" | mp2enc -b +AUDIO_BITRATE+ -r +SAMPLE_RATE+ -o \"+TEMP_PATH+/+BASE_NAME+.+AUDIO_TYPE+\" &amp; mplayer \"+INPUT_NAME+\" -vo null -ao pcm:file=\"+TEMP_PATH+/qdvd.fifo\"</Command>\n"
"    <Command>ffmpeg -i \"+INPUT_NAME+\" -ab +AUDIO_BITRATE+ -ar +SAMPLE_RATE+ -acodec +AUDIO_TYPE+ \"+TEMP_PATH+/+BASE_NAME+.+AUDIO_TYPE+\"</Command>"
"    <Command>lame --quiet --decode \"+INPUT_NAME+\" - | mp2enc -b +AUDIO_BITRATE+ -r +SAMPLE_RATE+ -o \"+TEMP_PATH+/+BASE_NAME+.+AUDIO_TYPE+\"</Command>\n"
"    <Command>mplayer \"+INPUT_NAME+\" -vo null -ao pcm:file=\"+TEMP_PATH+/temp.pcm\" ; mp2enc -b +AUDIO_BITRATE+ -r +SAMPLE_RATE+ -o \"+TEMP_PATH+/+BASE_NAME+.+AUDIO_TYPE+\" &lt; \"+TEMP_PATH+/temp.pcm\"</Command>\n"
"   </Block>\n"

// These commands concat multiple input audio files
"   <Block name=\"EMPTY_SOUND\" >\n"
"    <SubBlock number=\"1\">\n"
"     <Block>\n"
"      <Comment>First we create the temp directory.</Comment>\n"
"      <Command default=\"true\">mkdir -p \"+TEMP_PATH+/+TRACK_NAME+\"</Command>\n"
"     </Block>\n"
"     <Block>\n"
"      <Comment>The first audio file needs to include the wav header</Comment>\n"
"      <Command default=\"true\">mplayer \"+INPUT_NAME+\" -vo null -ao pcm:file=\"+TEMP_PATH+/+TRACK_NAME+/audio0.pcm\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"2\" foreachinput=\"1\">\n"
"     <Block>\n"
"      <Comment>This audio file does not need another header.</Comment>\n"
"      <Command default=\"true\">mplayer \"+INPUT_NAME+\" -vo null -ao pcm:nowaveheader:file=\"+TEMP_PATH+/+TRACK_NAME+/audio+FOREACH_COUNTER+.pcm\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"    <SubBlock number=\"3\">\n"
"     <Block>\n"
"      <Comment>Then we concatenate all audio files together.</Comment>\n"
"      <Command default=\"true\">pushd \"+TEMP_PATH+/+TRACK_NAME+\"; cat audio*.pcm > track.pcm; popd</Command>\n"
"     </Block>\n"
"     <Block>\n"
"      <Comment>Finally we convert the pcm file into mp2.</Comment>\n"
"      <Command default=\"true\">mp2enc -b +AUDIO_BITRATE+ -r +SAMPLE_RATE+ -o \"+TEMP_PATH+/+TRACK_NAME+.mp2\" &lt; \"+TEMP_PATH+/+TRACK_NAME+/track.pcm\"</Command>\n"
"     </Block>\n"
"    </SubBlock>\n"
"   </Block>\n"
"  </Convert>\n"
"  <Subtitles blockname=\"Subtitles\" >\n"
"   <Block editable=\"false\" hidden=\"0\" name=\"OTHERS\" >\n"
"    <Comment>&lt;p align=&quot;center&quot;>This command will multiplex subtitles into the video stream.&lt;/p></Comment>"
"    <Command default=\"true\" >spumux -m dvd -s +SUBTITLE_NUMBER+ &quot;+TEMP_PATH+/+BASE_NAME+/subtitle_+SUBTITLE_NUMBER+.xml&quot; &lt; &quot;+INPUT_FILE_NAME+&quot; +MULTIPLE_SUBTITLE_PIPE+ > &quot;+TEMP_PATH+/+BASE_NAME+/subtitles.vob&quot;</Command>\n"
"   </Block>\n"
"  </Subtitles>\n"
"  </ExecuteInterface>\n"
" <ExecuteInterface>\n"
"  <Slideshow color=\"#000000\" blockname=\"+MENU_NAME+ Button Transitions\" >\n"
"   <Block editable=\"true\" hidden=\"0\" name=\"MOVIE_BACKGROUND\" >\n"
"   <Comment>&lt;p align=&quot;center&quot;&gt;This command render slideshows for Button Transitions.&lt;/p&gt;</Comment>\n"
"   <Command default=\"true\">qrender -v 5 -ix \"+TEMP_PATH+/+MENU_NAME+/+BUTTON_NAME+/transition.xml\" -o \"+TEMP_PATH+/+MENU_NAME+/+BUTTON_NAME+/transition.vob\"</Command>\n"
"   </Block>\n"
"  </Slideshow>\n"
" </ExecuteInterface>\n"
"  <ExecuteInterface>\n"
"    <Pal blockname=\"+MENU_NAME+ (PAL +WIDTH+x+HEIGHT+)\">\n"
"      <Block name=\"MOVIE_BACKGROUND\">\n"
"        <SubBlock number=\"1\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This Command will extract the timeframe of interest.&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mkdir -p \"+TEMP_PATH+/+MENU_NAME+/background\"; mencoder -oac copy -ovc copy -ss +START_POS+ -endpos +DURATION+ -o \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\" \"+BACKGROUND_FULL_NAME+\"</Command>\n"
"          <Command>mkdir -p \"+TEMP_PATH+/+MENU_NAME+/background\"; ln -s \"+BACKGROUND_FULL_NAME+\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"; echo \"Skipping cleaning of source file\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"2\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This line of commands will clean up the source movie&lt;BR&gt;&lt;U&gt;Note :&lt;/U&gt;Might not be needed.&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">ln -s \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\" \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\"; echo \"Skipping cleaning of source file\"</Command>\n"
"          <Command>mencoder -oac null -ovc lavc -o \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"3\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;Here we extract the audio part of the source background&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mplayer -ao pcm:file=\"+TEMP_PATH+/+MENU_NAME+/background/clean.wav\" -vo null -vc dummy \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"          <Command>mplayer -ao pcm -vo null -vc dummy -aofile \"+TEMP_PATH+/+MENU_NAME+/background/clean.wav\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"4\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;And now we extract the image files of the source movie&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mplayer -ao null -vo jpeg:outdir=\"+TEMP_PATH+/+MENU_NAME+/background\" \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"6\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;And this is the place where we internally render the images&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\" canmodify=\"false\">"RENDER_MENU"+MENU_NAME+</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"5\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;We can remove the big temp files&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">cd \"+TEMP_PATH+/+MENU_NAME+/\"; find . -name clip.avi -exec rm {} \\;; find . -name clean.avi -exec rm {} \\;</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"6\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This line of commands will convert the extracted frames &lt;BR&gt;into a MPEG2 stream in PAL format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
"          <Command>png2yuv -j \"+TEMP_PATH+/+MENU_NAME+/background/rendered_%08d.png\" -I p -f 25 -b 1 | mpeg2enc -f 8 -n p -a +ASPECT+ -F 3 -M 1 -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"          <Command>jpeg2yuv -j \"+TEMP_PATH+/+MENU_NAME+/background/rendered_%08d.jpg\" -I p -f 25 -b 1 | mpeg2enc -f 8 -n p -a +ASPECT+ -F 3 -M 1 -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"       </Block>\n"
//"      <Block editable=\"false\" hidden=\"0\" name=\"IMAGE_BACKGROUND\" >\n"
//"        <Comment>This option creates the static background for +MENU_NAME+ using MEncoder.</Comment>\n"
//"        <Comment>This option creates the static background +MENU_NAME+ using misc tools.</Comment>\n"
//"        <Command>"CONVERT_MENCODER" +MENU_NAME+</Command>\n"
//"        <Command default=\"true\" >"BACKGROUND_MISCTOOLS" +MENU_NAME+</Command>\n"
//"        <SubBlock number=\"0\" name=\"MEncoder\" >\n"
//"          <Block>\n"
//"            <Comment>&lt;p align=\"center\"&gt;This line of command will convert the background image&lt;BR&gt;into a MPEG2 stream in PAL format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
//"            <Command default=\"true\" >mencoder -of mpeg -mpegopts tsaf:format=dvd:tsaf -ovc lavc -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:keyint=15:aspect=+RATIO+:vrc_maxrate=7500:vstrict=0:vbitrate=7500:vmax_b_frames=2 -o \"+TEMP_PATH+/+MENU_NAME+_menu.mpg\" mf://\"+BACKGROUND_FULL_NAME+\" -fps 1/+DURATION+ -ofps 24000/1001  -vf scale=+WIDTH+:+HEIGHT+,harddup -oac lavc -lavcopts acodec=ac3:abitrate=192 -audiofile "PREFIX_DIRECTORY"/share/qdvdauthor/silence.mp2</Command>\n"
//"          </Block>\n"
//"        </SubBlock>\n"
//"        <SubBlock number=\"1\" name=\"MiscTools\" >\n"
"          <Block name=\"IMAGE_BACKGROUND\" >\n"
"            <Comment>&lt;p align=\"center\"&gt;This line of command will convert the background image&lt;BR&gt;into a MPEG2 stream in PAL format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\" >jpegtopnm \"+BACKGROUND_FULL_NAME+\" | ppmtoy4m -n +NR_OF_FRAMES+ -r -S "+qsPPMTOY4M+" -F25:1 -A59:54 -I t -L | mpeg2enc -f 8 -n p -a +ASPECT+ -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"            <Command>jpeg2yuv -n +NR_OF_FRAMES+ -I p -f 25 -j \"+BACKGROUND_FULL_NAME+\" | mpeg2enc -n p -f 8 -a +ASPECT+ -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"          </Block>\n"
"          <Block name=\"EMPTY_SOUND\">\n"
"            <Comment>&lt;p align=\"center\"&gt;This line creates an empty sound file to mix into the menu-movie.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">cp " + Global::qsSystemPath + "/share/qdvdauthor/silence.ac3 \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>arecord -f dat -twav -d 1 | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>arecord -S -M -t 1 -w /dev/stdout | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>sox -t raw -s -w -c 2 -r 48000 /dev/zero -t wav -c 2 -r 48000 /dev/stdout trim 0 1 | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>dd if=/dev/zero bs=4 count=1920 | twolame -b192 -s "+qsToolameSample+" /dev/stdin \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"          </Block>\n"
"          <Block name=\"MPLEX\">\n"
"            <Comment>&lt;p align=\"center\"&gt;The following command will multiplex the sound file into the menu-movie.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+MENU_NAME+/menu.mpg\" \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\" +SOUND_LIST+</Command>\n"
"          </Block>\n"
"          <Block name=\"SPUMUX\">\n"
"            <Comment>&lt;p align=\"center\"&gt;Here we use spumux to add the subpicture (buttons) to the mpeg2 video.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">spumux -m dvd \"+TEMP_PATH+/+MENU_NAME+/menu.xml\" &lt; \"+TEMP_PATH+/+MENU_NAME+/menu.mpg\" &gt; \"+TEMP_PATH+/+MENU_NAME+_menu.mpg\"</Command>\n"
"          </Block>\n"
//"        </SubBlock>\n"
//"      </Block>\n" // IMAGE_BACKGROUND
"    </Pal>\n"
"    <Ntsc blockname=\"+MENU_NAME+ (NTSC +WIDTH+x+HEIGHT+)\">\n"
"      <Block name=\"MOVIE_BACKGROUND\">\n"
"        <SubBlock number=\"1\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This Command will extract the requested timeframe.&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mkdir -p \"+TEMP_PATH+/+MENU_NAME+/background\"; mencoder -oac copy -ovc copy -ss +START_POS+ -endpos +DURATION+ -o \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\" \"+BACKGROUND_FULL_NAME+\"</Command>\n"
"          <Command>mkdir -p \"+TEMP_PATH+/+MENU_NAME+/background\"; ln -s \"+BACKGROUND_FULL_NAME+\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"; echo \"Skipping cleaning of source file\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"2\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This line of command will clean up the source movie&lt;BR&gt;Might not be needed.&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">ln -s \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\" \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\"; echo \"Skipping cleaning of source file\"</Command>\n"
"          <Command>mencoder -oac null -ovc lavc -o \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"3\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;Here we extract the audio part of the source background&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mplayer -ao pcm:file=\"+TEMP_PATH+/+MENU_NAME+/background/clip.wav\" -vo null -vc dummy \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"          <Command>mplayer -ao pcm -vo null -vc dummy -aofile \"+TEMP_PATH+/+MENU_NAME+/background/clip.wav\" \"+TEMP_PATH+/+MENU_NAME+/background/clip.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"4\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;And now we extract the image files of the source movie&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">mplayer -ao null -vo jpeg:outdir=\"+TEMP_PATH+/+MENU_NAME+/background\" \"+TEMP_PATH+/+MENU_NAME+/background/clean.avi\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"6\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;And this is the place where we internally render the images&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\" canmodify=\"false\">"RENDER_MENU"+MENU_NAME+</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"5\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;We can remove the big temp files&lt;/p&gt;</Comment>\n"
"          <Command default=\"true\">cd \"+TEMP_PATH+/+MENU_NAME+/\"; find . -name clip.avi -exec rm {} \\;; find . -name clean.avi -exec rm {} \\;</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"6\">\n"
"         <Block>\n"
"          <Comment>&lt;p align=\"center\"&gt;This line of command will convert the extracted frames &lt;BR&gt;into a MPEG2 stream in NTSC format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
"          <Command>png2yuv -j \"+TEMP_PATH+/+MENU_NAME+/background/rendered_%08d.png\" -I p -f 29.97 -b 1 | mpeg2enc -f 8 -n n -a +ASPECT+ -F 4 -M 1 -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"          <Command>jpeg2yuv -j \"+TEMP_PATH+/+MENU_NAME+/background/rendered_%08d.jpg\" -I p -f 29.97 -b 1 | mpeg2enc -f 8 -n n -a +ASPECT+ -F 4 -M 1 -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"         </Block>\n"
"        </SubBlock>"
"      </Block>\n"
//"      <Block editable=\"false\" hidden=\"0\" name=\"IMAGE_BACKGROUND\" >\n"
//"        <Comment>This option creates the static background for +MENU_NAME+ using MEncoder.</Comment>\n"
//"        <Comment>This option creates the static background +MENU_NAME+ using misc tools.</Comment>\n"
//"        <Command>"CONVERT_MENCODER" +MENU_NAME+</Command>\n"
//"        <Command default=\"true\" >"BACKGROUND_MISCTOOLS" +MENU_NAME+</Command>\n"
//"        <SubBlock number=\"0\" name=\"MEncoder\" >\n"
//"          <Block name=\"IMAGE_BACKGROUND\" >\n"
//"            <Comment>&lt;p align=\"center\"&gt;This line of command will convert the background image&lt;BR&gt;into a MPEG2 stream in PAL format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
//"            <Command default=\"true\" >mencoder -of mpeg -mpegopts tsaf:format=dvd:tsaf -ovc lavc -lavcopts vcodec=mpeg2video:vrc_buf_size=1835:keyint=15:aspect=+ASPECT+:vrc_maxrate=7500:vstrict=0:vbitrate=7500:vmax_b_frames=2 -o \"+TEMP_PATH+/+MENU_NAME+_menu.mpg\" mf://\"+BACKGROUND_FULL_NAME+\" -fps $((24000/1001))/+DURATION+ -ofps 24000/1001  -vf scale=+WIDTH+:+HEIGHT+,harddup -oac lavc -lavcopts acodec=ac3:abitrate=192 -audiofile "PREFIX_DIRECTORY"/share/qdvdauthor/silence.mp2</Command>\n"
//"          </Block>\n"
//"        </SubBlock>\n"
//"        <SubBlock number=\"1\" name=\"MiscTools\" >\n"
"          <Block name=\"IMAGE_BACKGROUND\" >\n"
"            <Comment>&lt;p align=\"center\"&gt;This line of command will convert the background image&lt;BR&gt;into a MPEG2 stream in NTSC format +WIDTH+x+HEIGHT+&lt;/p&gt;</Comment>\n"
"            <Command>jpegtopnm \"+BACKGROUND_FULL_NAME+\" | ppmtoy4m -n +NR_OF_FRAMES+ -r -S "+qsPPMTOY4M+" -F30000:1001 -A10:11 -I t -L | mpeg2enc -f 8 -n n -a +ASPECT+ -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"            <Command default=\"true\">jpeg2yuv -n +NR_OF_FRAMES+ -I p -f 29.97 -j \"+BACKGROUND_FULL_NAME+\" | mpeg2enc -n n -f 8 -a +ASPECT+ -o \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\"</Command>\n"
"          </Block>\n"
"          <Block name=\"EMPTY_SOUND\">\n"
"            <Comment>&lt;p align=\"center\"&gt;This line creates an empty sound file to mix into the menu-movie.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">cp " + Global::qsSystemPath + "/share/qdvdauthor/silence.ac3 \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>arecord -f dat -twav -d 1 | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>arecord -S -M -t 1 -w /dev/stdout | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>sox -t raw -s -w -c 2 -r 48000 /dev/zero -t wav -c 2 -r 48000 /dev/stdout trim 0 1 | mp2enc -r 48000 -o \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"            <Command>dd if=/dev/zero bs=4 count=1601 | twolame -b192 -s "+qsToolameSample+" /dev/stdin \"+TEMP_PATH+/+MENU_NAME+/menu.mp2\"</Command>\n"
"          </Block>\n"
"          <Block name=\"MPLEX\">\n"
"            <Comment>&lt;p align=\"center\"&gt;The following command will multiplex the sound file into the menu-movie.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">mplex -f 8 -S 0 -M -o \"+TEMP_PATH+/+MENU_NAME+/menu.mpg\" \"+TEMP_PATH+/+MENU_NAME+/menu.m2v\" +SOUND_LIST+</Command>\n"
"          </Block>\n"
"          <Block name=\"SPUMUX\">\n"
"            <Comment>&lt;p align=\"center\"&gt;Here we use spumux to add the subpicture (buttons) to the mpeg2 video.&lt;/p&gt;</Comment>\n"
"            <Command default=\"true\">spumux -m dvd \"+TEMP_PATH+/+MENU_NAME+/menu.xml\" &lt; \"+TEMP_PATH+/+MENU_NAME+/menu.mpg\" &gt; \"+TEMP_PATH+/+MENU_NAME+_menu.mpg\"</Command>\n"
"          </Block>\n"
//"        </SubBlock>\n"
//"      </Block>\n"
"    </Ntsc>\n"
"  </ExecuteInterface>\n"
"  <ExecuteInterface>\n"
"    <Dvdauthor blockname=\"dvdauthor\" color=\"#F92F5E\">\n"
"      <Block>\n"
"        <Comment>Here we start dvdauthor with the generated xml file.</Comment>\n"
"        <Command>dvdauthor -x \"+DVDAUTHOR_XML+\"</Command>\n"
"      </Block>\n"
"    </Dvdauthor>\n"
"  </ExecuteInterface>\n"
"  <ExecuteInterface hidden=\"true\">\n"
"    <Burning blockname=\"Burn DVD\" color=\"#E0EE8F\">\n"
"      <Block>\n"
"        <SubBlock number=\"1\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>This command will format a empty -RW DVD (only needed for older drives).</Comment>\n"
"          <Command>dvd+rw-format -f /dev/srcd0</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"2\" active=\"true\">\n"
"         <Block>\n"
"          <Comment>Last we burn the DVD.</Comment>\n"
"          <Command>growisofs -Z /dev/dvd -dvd-video \"+PROJECT_PATH+/\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"3\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>This command generates an iso image of the DVD.</Comment>\n"
"          <Command>mkisofs -dvd-video -udf -o \"+TEMP_PATH+/dvd.iso\" \"+PROJECT_PATH+/\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"4\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>This command will add Error Correction Codes (ECC) to the iso image.</Comment>\n"
"          <Command>dvdisaster -i \"+TEMP_PATH+/dvd.iso\" -mRS02 -n 350000 -c</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"        <SubBlock number=\"5\" active=\"false\">\n"
"         <Block>\n"
"          <Comment>This command burns the iso DVD image to DVD</Comment>\n"
"          <Command>dvdrecord -dao speed=2 dev=0,0,0 \"+TEMP_PATH+/dvd.iso\"</Command>\n"
"          <Command>wodim -dao speed=2 dev=0,0,0 \"+TEMP_PATH+/dvd.iso\"</Command>\n"
"         </Block>\n"
"        </SubBlock>\n"
"      </Block>\n"
"    </Burning>\n"
"  </ExecuteInterface>\n"
"</QDVDAuthorInitFile>\n");
  		stream << qsResetInitFile << "\n";
		initFile.flush ( );
		initFile.close ( );
	}
}
