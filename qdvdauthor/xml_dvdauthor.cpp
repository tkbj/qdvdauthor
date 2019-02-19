/***************************************************************************
    xml_dvdauthor.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

//#include "../bugs_workaround.h"	//ooo
#include "global.h"
#include "xml_dvdauthor.h"
#include "messagebox.h"

#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QObject>	//xxx
//#include <QDom>	//ooo

#ifndef MAX_AUDIO
#define MAX_AUDIO 8
#endif

#ifndef MAX_SUBTITLES
#define MAX_SUBTITLES 32
#endif

#ifndef DEBUG_INFO
#define debug_out printf
#else
void debug_out(const char *, ...){};
#endif

CXmlDVDAuthor::CXmlDVDAuthor ()
{

}

CXmlDVDAuthor::~CXmlDVDAuthor ()
{

}

CXmlDVDAuthor::dvdauthor_struct *CXmlDVDAuthor::getDVDAuthor ()
{
  return &m_dvdauthor;
}
//////////////////////////////////////////////////////////////
//
// READ XML file ...
//
//////////////////////////////////////////////////////////////
bool CXmlDVDAuthor::readXml ()
{
	// Here we read in a xml - file and create the neccesary underlying structure.
	//
	// For now we are going to ask for the file name here and handle the QDom...
	// Later on this is done a level further up and only QDomNode * is sent.
	//
	//////////////////////////////////////////////////////////////////////////////////
	//QString fileName = QFileDialog::getOpenFileName ( QString("./"), QObject::tr ("XML files ( *.xml *.XML)"));		//ooo
	QString fileName = QFileDialog::getOpenFileName ( NULL, QObject::tr("Open File"), QString("./"), QObject::tr ("XML files ( *.xml *.XML)"));
	return readXml ( fileName );
}

bool CXmlDVDAuthor::readXml (QString &fileName )
{
	// Assign the file
	QFile projectFile(fileName);
	if (!projectFile.open( QIODevice::ReadWrite))
		return false;

	QDomDocument xmlDoc( DVD_DOCTYPE );
	if (!xmlDoc.setContent (&projectFile))	{
		// Error handling ...
		projectFile.close();
		int iReturn = MessageBox::warning ( NULL, QObject::tr ("xml project file seems to be defective."),
			QObject::tr ("Do you want to try to load another project file ?"),
			QMessageBox::Yes, QMessageBox::No);
		if (iReturn == QMessageBox::Yes)
			return readXml (); // Okay, user wants to specify another project file.
	}
	// And at last lets try to read the information of the file.
	QDomElement docElem = xmlDoc.documentElement();
	bool bReturn = m_dvdauthor.readXml (&docElem);

	projectFile.close();
	return bReturn;
}

bool CXmlDVDAuthor::dvdauthor_struct::readXml ( QDomElement *pDocElem )
{
	titleset_struct  tempTitleset;  // temp to get the node_name.
	titleset_struct *pTitleset = NULL;
	bool bReturn = true;
	//debug_out ("CXmlDVDAuthor::dvdauthor_struct::readXml <%s><%s>\n",(const char *)pDocElem->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::dvdauthor_struct::readXml <%s><%s>\n",pDocElem->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	if ( pDocElem->tagName ( ) != node_name )
		return false;
	// So lets get first the attributes for this node.
	QDomAttr a = pDocElem->attributeNode ( DVDAUTHOR_DEST );
	dest = a.value ( );
	a = pDocElem->attributeNode ( DVDAUTHOR_JUMPPAD );
	jumppad = a.value ( );

	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pDocElem->firstChild ( );
	while ( !xmlNode.isNull ( ) )  {
		QDomElement searchTree = xmlNode.toElement  ( );
		if ( searchTree.tagName ( ) == vmgm.node_name )
			bReturn = vmgm.readXml  ( &searchTree );
		else if ( searchTree.tagName ( ) == tempTitleset.node_name )  {
			pTitleset = addTitleset ( );
			bReturn   = pTitleset->readXml ( &searchTree );
		}
		// If there has been a problem then return false.
		if ( ! bReturn )
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling ( );
	}
	return true;
}

bool CXmlDVDAuthor::vmgm_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ( "CXmlDVDAuthor::vmgm_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ( "CXmlDVDAuthor::vmgm_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	// vmgm has no attributes thus far I can tell.
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
//	QDomElement searchTree = xmlNode.toElement();
	menus_struct tempMenus;
	menus_struct *pNewMenus = NULL;
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement ( );
		if ( searchTree.tagName ( ) == fpc.node_name )
			bReturn = fpc.readXml ( &searchTree  );
		if ( searchTree.tagName ( ) == tempMenus.node_name )  {
			pNewMenus = addMenus ( );
			bReturn = pNewMenus->readXml ( &searchTree );
		}
		// If there has been a problem then return false.
		if ( ! bReturn )
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}

bool CXmlDVDAuthor::titleset_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::titleset_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::titleset_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	// titleset has no attributes thus far I can tell.
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	menus_struct tempMenus;
	menus_struct *pNewMenus = NULL;
	titles_struct tempTitles;
	titles_struct *pNewTitles = NULL;
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == tempMenus.node_name)	{
			pNewMenus = addMenus ();
			bReturn = pNewMenus->readXml(&searchTree);
		}
		else if (searchTree.tagName() == tempTitles.node_name)	{
			pNewTitles = addTitles ();
			bReturn = pNewTitles->readXml(&searchTree);
		}
		// If there has been a problem then return false.
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}

bool CXmlDVDAuthor::menus_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::menus_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::menus_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int iAudioChannel = 0;
	int iSubpicture   = 0;
	pgc_struct *pTempPGC = new pgc_struct();
	pgc_struct *pNewPgc = NULL;
	// Lets get the attributes first ...
	QDomAttr a = pNodeElement->attributeNode ( MENUS_LANG );
	lang = a.value();
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == video.node_name)
			bReturn = video.readXml(&searchTree);
		else if (searchTree.tagName() == audio[iAudioChannel].node_name)	{
			bReturn = audio[iAudioChannel].readXml(&searchTree);
			if ( ++iAudioChannel >= MAX_AUDIO )
			  iAudioChannel = MAX_AUDIO-1;

		}
		else if (searchTree.tagName() == subpicture[iSubpicture].node_name) {
		  bReturn = subpicture[iSubpicture].readXml(&searchTree);
		  if ( ++iSubpicture >= MAX_SUBTITLES )
		    iSubpicture = MAX_SUBTITLES-1;
		}
		else if (searchTree.tagName() == pTempPGC->node_name)	{
			pNewPgc = addPgc ();
			bReturn = pNewPgc->readXml(&searchTree);
		}
		// If there has been a problem then return false.
		if (!bReturn)	{
			delete pTempPGC;
			return false;
		}
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	delete pTempPGC;
	return true;
}

bool CXmlDVDAuthor::video_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::video_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::video_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// The video node has only attributes and nothing else.
	QDomAttr a = pNodeElement->attributeNode ( VIDEO_FORMAT );
	format = a.value();
	a = pNodeElement->attributeNode ( VIDEO_ASPECT );
	aspect = a.value();
	a = pNodeElement->attributeNode ( VIDEO_RESOLUTION );
	resolution = a.value();
	a = pNodeElement->attributeNode ( VIDEO_CAPTION );
	caption = a.value();
	a = pNodeElement->attributeNode ( VIDEO_WIDESCREEN );
	widescreen = a.value();
	return true;
}

bool CXmlDVDAuthor::audio_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::audio_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::audio_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// The audio node has only attributes and nothing else.
	QDomAttr a = pNodeElement->attributeNode ( AUDIO_FORMAT );
	format = a.value();
	a = pNodeElement->attributeNode ( AUDIO_CHANNELS );
	channels = a.value().toInt();
	a = pNodeElement->attributeNode ( AUDIO_QUANT );
	quant = a.value();
	a = pNodeElement->attributeNode ( AUDIO_DOLBY );
	dolby = a.value();
	a = pNodeElement->attributeNode ( AUDIO_LANG );
	lang = a.value();
	return true;
}

bool CXmlDVDAuthor::subpicture_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::subpicture_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::subpicture_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// The subpicture node has only attributes and nothing else.
	QDomAttr a = pNodeElement->attributeNode ( SUBPICTURE_LANG );
	lang = a.value();
	return true;
}
bool CXmlDVDAuthor::titles_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::titles_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::titles_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int iAudioChannel = 0;
	int iSubpicture   = 0;
	pgc_struct tempPGC;
	pgc_struct *pNewPgc = NULL;
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == video.node_name)
			bReturn = video.readXml(&searchTree);
		else if (searchTree.tagName() == audio[iAudioChannel].node_name)	{
			bReturn = audio[iAudioChannel].readXml(&searchTree);
			if ( ++iAudioChannel > MAX_AUDIO )
			  iAudioChannel = MAX_AUDIO - 1;
		}
		else if (searchTree.tagName() == tempPGC.node_name)	{
			pNewPgc = addPgc ();
			bReturn = pNewPgc->readXml(&searchTree);
		}
		else if (searchTree.tagName() == subpicture[iSubpicture].node_name)	{
			bReturn = subpicture[iSubpicture].readXml(&searchTree);
			if ( ++iSubpicture >= MAX_SUBTITLES )
			  iSubpicture = MAX_SUBTITLES - 1;
		}
		
		// If there has been a problem then return false.
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}

bool CXmlDVDAuthor::pgc_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::pgc_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::pgc_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	button_struct tempButton;
	vob_struct tempVob;
	button_struct *pNewButton = NULL;
	vob_struct *pNewVob = NULL;
	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( PGC_ENTRY );
	entry = a.value();
	a = pNodeElement->attributeNode ( PGC_PALETTE );
	palette = a.value();
	a = pNodeElement->attributeNode ( PGC_PAUSE );
	pause = a.value();
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == pre.node_name)
			bReturn = pre.readXml(&searchTree);
		else if (searchTree.tagName() == tempVob.node_name)	{
			pNewVob = addVob();
			bReturn = pNewVob->readXml(&searchTree);
		}
		else if (searchTree.tagName() == tempButton.node_name)	{
			pNewButton = addButton ();
			bReturn = pNewButton->readXml(&searchTree);
		}
		else if (searchTree.tagName() == post.node_name)
			bReturn = post.readXml(&searchTree);
		// If there has been a problem then return false.
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}

bool CXmlDVDAuthor::button_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::button_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::button_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( BUTTON_NAME );
	name = a.value();
	// The button can execute some commands, which are stored as the value of the node.
	value = pNodeElement->text();
	return true;
}

bool CXmlDVDAuthor::cell_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::cell_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::cell_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( CELL_START );
	start = a.value();
	a = pNodeElement->attributeNode ( CELL_END );
	end = a.value();
	a = pNodeElement->attributeNode ( CELL_CHAPTER );
	chapter = a.value();
	a = pNodeElement->attributeNode ( CELL_PROGRAM );
	program = a.value();
	a = pNodeElement->attributeNode ( CELL_PAUSE );
	pause = a.value();
	return true;
}

bool CXmlDVDAuthor::vob_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::vob_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::vob_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( VOB_FILE );
	file = a.value();
	// We assume Global::qsCurrentPath is set to the path of the xml file
	QFileInfo fileInfo   ( file );
	if ( fileInfo.isRelative () )
	  file = Global::qsCurrentPath + "/" + a.value ();

	a = pNodeElement->attributeNode ( VOB_CHAPTERS );
	chapters = a.value();
	a = pNodeElement->attributeNode ( VOB_PAUSE );
	pause = a.value();

	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == cell.node_name)
			bReturn = cell.readXml(&searchTree);
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return bReturn;
}

bool CXmlDVDAuthor::fpc_struct::readXml ( QDomElement *pNodeElement )
{
	//debug_out ("CXmlDVDAuthor::fpc_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::fpc_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// read the commands to be executed before.
	value = pNodeElement->text();
	return true;
}

bool CXmlDVDAuthor::pre_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::pre_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::pre_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// read the commands to be executed before.
	value = pNodeElement->text();
	return true;
}

bool CXmlDVDAuthor::post_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::post_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::post_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// read the commands to be executed after.
	value = pNodeElement->text();
	return true;
}
// Here the readXml functions for CXmlSpumux - class
bool CXmlSpumux::subpictures_struct::readXml(QDomElement *pNodeElement)
{
        //debug_out ("CXmlSpumux::subpictures_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::subpictures_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// vmgm has no attributes thus far I can tell.
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	QDomElement searchTree = xmlNode.toElement();
	// And only one menus entry is allowed ...
	if (searchTree.tagName() == stream.node_name)
		return stream.readXml(&searchTree);
	// If there has been a problem then return false.
	return false;
}

bool CXmlSpumux::stream_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumux::stream_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::stream_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool            bReturn = true;
	spu_struct      tempSpu;
	textsub_struct  tempTextsub;
	spu_struct     *pNewSpu     = NULL;
	textsub_struct *pNewTextsub = NULL;
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == tempSpu.node_name)	{
			pNewSpu = addSpu ();
			bReturn = pNewSpu->readXml(&searchTree);
		}
		if (searchTree.tagName() == tempTextsub.node_name)	{
			pNewTextsub = addTextsub ();
			bReturn = pNewTextsub->readXml(&searchTree);
		}
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	// If there has been a problem then return false.
	return true;
}
bool CXmlSpumux::spu_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumuxr::spu_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumuxr::spu_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	button_struct tempButton, *pNewButton=NULL;
	action_struct tempAction, *pNewAction=NULL;

	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( SPU_START );
	start = a.value();
	a = pNodeElement->attributeNode ( SPU_END );
	end = a.value();
	a = pNodeElement->attributeNode ( SPU_IMAGE );
	image = a.value();
	a = pNodeElement->attributeNode ( SPU_HIGHLIGHT );
	highlight = a.value();
	a = pNodeElement->attributeNode ( SPU_SELECT );
	select = a.value();
	a = pNodeElement->attributeNode ( SPU_TRANSPARENT );
	transparent = a.value();
	a = pNodeElement->attributeNode ( SPU_FORCE );
	force = a.value();
	a = pNodeElement->attributeNode ( SPU_AUTOOUTLINE );
	autooutline = a.value();
	a = pNodeElement->attributeNode ( SPU_OUTLINEWIDTH );
	outlinewidth = a.value().toInt();
	a = pNodeElement->attributeNode ( SPU_AUTOORDER );
	autoorder = a.value();
	a = pNodeElement->attributeNode ( SPU_XOFFSET );
	xoffset = a.value().toInt();
	a = pNodeElement->attributeNode ( SPU_YOFFSET );
	yoffset = a.value().toInt();
	// And now read in all remaining nodes and handle them accordingly.
	QDomNode xmlNode = pNodeElement->firstChild();
	// And only one menus entry is allowed ...
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == tempButton.node_name)	{
			pNewButton = addButton();
			bReturn = pNewButton->readXml(&searchTree);
		}
		else if (searchTree.tagName() == tempAction.node_name)	{
			pNewAction = addAction();
			bReturn = pNewAction->readXml(&searchTree);
		}
		// If there has been a problem then return false.
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}
bool CXmlSpumux::textsub_struct::readXml(QDomElement *pNodeElement)
{
  //debug_out ("CXmlSpumuxr::textsub_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
  debug_out ("CXmlSpumuxr::textsub_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
  // First read the attributes
  QDomAttr a = pNodeElement->attributeNode ( TEXTSUB_FILENAME );
  filename = a.value();
  a = pNodeElement->attributeNode ( TEXTSUB_CHARACTERSET );
  characterset = a.value();
  a = pNodeElement->attributeNode ( TEXTSUB_FONTSIZE );
  fontsize = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_HORIZ_ALIGNMENT );
  horizontal_alignment = a.value();
  a = pNodeElement->attributeNode ( TEXTSUB_VERT_ALIGNMENT );
  vertical_alignment = a.value();
  a = pNodeElement->attributeNode ( TEXTSUB_LEFT_MARGIN );
  left_margin = a.value().toInt ();
  a = pNodeElement->attributeNode ( TEXTSUB_RIGHT_MARGIN );
  right_margin = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_BOTTOM_MARGIN );
  bottom_margin = a.value().toInt ();
  a = pNodeElement->attributeNode ( TEXTSUB_TOP_MARGIN );
  top_margin = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_FONT );
  font = a.value();
  a = pNodeElement->attributeNode ( TEXTSUB_SUBTITLE_FPS );
  subtitle_fps= a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_MOVIE_FPS );
  movie_fps = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_MOVIE_WIDTH );
  movie_width = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_MOVIE_HEIGHT );
  movie_height = a.value().toInt();
  a = pNodeElement->attributeNode ( TEXTSUB_TRANSPARENT );
  transparent = a.value();

  return true;
}
bool CXmlSpumux::button_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumux::button_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::button_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// First read the attributes
	QDomAttr a = pNodeElement->attributeNode ( BUTTON_LABEL );
	label = a.value();
	a = pNodeElement->attributeNode ( BUTTON_X0 );
	x0 = a.value().toInt();
	a = pNodeElement->attributeNode ( BUTTON_Y0 );
	y0 = a.value().toInt();
	a = pNodeElement->attributeNode ( BUTTON_X1 );
	x1 = a.value().toInt();
	a = pNodeElement->attributeNode ( BUTTON_Y1 );
	y1 = a.value().toInt();
	a = pNodeElement->attributeNode ( BUTTON_LEFT );
	left = a.value();
	a = pNodeElement->attributeNode ( BUTTON_RIGHT );
	right = a.value();
	a = pNodeElement->attributeNode ( BUTTON_UP );
	up = a.value();
	a = pNodeElement->attributeNode ( BUTTON_DOWN );
	down = a.value();
	return true;
}
bool CXmlSpumux::action_struct::readXml(QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumux::action_struct::readXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::action_struct::readXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	// First read the attributes
	QDomAttr a;
#ifdef DVDAUTHOR_VERSION_LESS_0_6_10
	a = pNodeElement->attributeNode ( ACTION_LABEL_OLD );
#else
	a = pNodeElement->attributeNode ( ACTION_LABEL );
#endif
	label = a.value();
	return true;
}

//////////////////////////////////////////////////////////////
//
// WRITE XML file ...
//
//////////////////////////////////////////////////////////////
bool CXmlDVDAuthor::writeXml ()
{
	// Here we read in a xml - file and create the neccesary underlying structure.
	//
	// For now we are going to ask for the file name here and handle the QDom...
	// Later on this is done a level further up and only QDomNode * is sent.
	//
	//////////////////////////////////////////////////////////////////////////////////
	debug_out ("CXmlDVDAuthor::writeXml ()>\n");
	//QString fileName = QFileDialog::getSaveFileName ( QString("./"), QString ("XML files ( *.xml)"));					//ooo
	QString fileName = QFileDialog::getSaveFileName ( NULL, QObject::tr("Save File"), QString("./"), QString ("XML files ( *.xml)"));	//xxx
	if (fileName.isNull())
		return false;
	// Assign the file
	return writeXml(fileName);
}

bool CXmlDVDAuthor::writeXml (QString &fileName)
{ 
	//debug_out ("CXmlDVDAuthor::writeXml (%s)\n",(const char *)fileName);		//ooo
	debug_out ("CXmlDVDAuthor::writeXml (%s)\n",fileName.toLatin1().data());	//xxx
	// First we create the document, ...
	QDomDocument xmlDoc( DVD_DOCTYPE );	// <""> for now.
	if (!m_dvdauthor.writeXml (&xmlDoc))	{
		return false;
	}
	// Then we open the file for writing ...
	QFile projectFile(fileName);
	if (!projectFile.open(QIODevice::WriteOnly))
		return false;

	QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xml += xmlDoc.toString ( );
	// QDomDocument converts '<' to "&lt;", So if we want comments in
	// the generated file, we should replace them here.
	xml.replace ( "&lt;", "<"); 
	//debug_out ("%s\n", (const char *)xml);	//ooo
	debug_out ("%s\n", xml.toLatin1().data());	//xxx
	//xml = xml.utf8 ( );	//ooo
	xml = xml.toUtf8 ( );	//xxx
	//projectFile.writeBlock ( xml, qstrlen ( xml ) );				//ooo
	projectFile.write ( xml.toLatin1().data(), qstrlen ( xml.toLatin1().data() ) );	//xxx

	projectFile.close();
	return true;
}

bool CXmlDVDAuthor::dvdauthor_struct::writeXml ( QDomDocument *pDocument, QDomElement * )
{
	//debug_out ("CXmlDVDAuthor::dvdauthor_struct::writeXml <%s>\n", (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::dvdauthor_struct::writeXml <%s>\n", node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement rootDVDAuthor = pDocument->createElement ( node_name ); // <dvdauthor>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		rootDVDAuthor.appendChild ( domComment );
	}

	// Here we set the attributes of the <dvdauthor> tag
	if (!dest.isNull())
		rootDVDAuthor.setAttribute( DVDAUTHOR_DEST, dest );
	if (!jumppad.isNull())
		rootDVDAuthor.setAttribute( DVDAUTHOR_JUMPPAD, jumppad );
	// And now proceed to writing the rest of the file.
	pDocument->appendChild( rootDVDAuthor );
	bReturn = vmgm.writeXml (pDocument, &rootDVDAuthor);
	if (ppArrayTitleset)	{
		while (ppArrayTitleset[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayTitleset[t]->writeXml(pDocument, &rootDVDAuthor);
			t++;
		}
	}
	return bReturn;
}

bool CXmlDVDAuthor::vmgm_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::vmgm_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::vmgm_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;

	QDomElement vmgmNode = pDocument->createElement( node_name );	// <vmm>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		vmgmNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( vmgmNode );

	bReturn = fpc.writeXml (pDocument, &vmgmNode);
	if (ppArrayMenus)	{
		while (ppArrayMenus[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayMenus[t]->writeXml(pDocument, &vmgmNode);
			t++;
		}
	}
	return bReturn;
}

bool CXmlDVDAuthor::titleset_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::titleset_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::titleset_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement titlesetNode = pDocument->createElement( node_name );	// <titleset>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		titlesetNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( titlesetNode );
	t = 0;
	if (ppArrayMenus)	{
		while (ppArrayMenus[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayMenus[t]->writeXml(pDocument, &titlesetNode);
			t++;
		}
	}
	t = 0;
	if (ppArrayTitles)	{
		while (ppArrayTitles[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayTitles[t]->writeXml(pDocument, &titlesetNode);
			t++;
		}
	}
	return bReturn;
}

bool CXmlDVDAuthor::menus_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::menus_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::menus_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement menusNode = pDocument->createElement( node_name );	// <menus>
	// Here we set the attributes of the <dvdauthor> tag
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		menusNode.appendChild ( domComment );
	}
	if (!lang.isNull())
		menusNode.setAttribute( MENUS_LANG, lang );

	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( menusNode );
	bReturn = video.writeXml (pDocument, &menusNode);
	if (!bReturn)
		return false;

	for ( t=0; t<MAX_AUDIO; t++ )	{
		bReturn = audio[t].writeXml(pDocument, &menusNode);
		if (!bReturn)
			return false;
	}
	for ( t=0;t<MAX_SUBTITLES; t++ ) {
	  bReturn = subpicture[t].writeXml (pDocument, &menusNode);
	  if ( ! bReturn )
	    return false;
	}
	t = 0;
	if (ppArrayPgc)	{
		while (ppArrayPgc[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayPgc[t]->writeXml(pDocument, &menusNode);
			t++;
		}
	}
	return bReturn;
}
bool CXmlDVDAuthor::video_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( (format.isNull()) && (aspect.isNull()) && (resolution.isNull()) && (caption.isNull()) && (widescreen.isNull()) )
		return true;
	//debug_out ("CXmlDVDAuthor::video_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::video_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx

	QDomElement videoNode = pDocument->createElement( node_name );	// <video>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n   " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		videoNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!format.isNull())
		videoNode.setAttribute( VIDEO_FORMAT, format );
	if (!aspect.isNull())
		videoNode.setAttribute( VIDEO_ASPECT, aspect );
	if (!resolution.isNull())
		videoNode.setAttribute( VIDEO_RESOLUTION, resolution );
	if (!caption.isNull())
		videoNode.setAttribute( VIDEO_CAPTION, caption );
	if (!widescreen.isNull())
		videoNode.setAttribute( VIDEO_WIDESCREEN, widescreen );

	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( videoNode );
	return true;
}
bool CXmlDVDAuthor::audio_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( (format.isNull()) && (channels == 0) && (quant.isNull()) && (dolby.isNull()) && (lang.isNull()) )
		return true;
	//debug_out ("CXmlDVDAuthor::audio_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::audio_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx

	QDomElement audioNode = pDocument->createElement( node_name );	// <audio>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n   " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		audioNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!format.isNull())
		audioNode.setAttribute( AUDIO_FORMAT, format );
	if (!channels == 0)
		audioNode.setAttribute( AUDIO_CHANNELS, QString ("%1").arg(channels) );
	if (!quant.isNull())
		audioNode.setAttribute( AUDIO_QUANT, quant );
	if (!dolby.isNull())
		audioNode.setAttribute( AUDIO_DOLBY, dolby );
	if (!lang.isNull())
		audioNode.setAttribute( AUDIO_LANG, lang );

	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( audioNode );
	return true;
}

bool CXmlDVDAuthor::subpicture_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if (lang.isNull())
		return true;
	//debug_out ("CXmlDVDAuthor::subpicture_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::subpicture_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement subpictureNode = pDocument->createElement( node_name );	// <dvdauthor>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n   " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		subpictureNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!lang.isNull())
		subpictureNode.setAttribute( SUBPICTURE_LANG, lang );

	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( subpictureNode );
	return true;
}

bool CXmlDVDAuthor::titles_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::titles_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::titles_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement titlesNode = pDocument->createElement( node_name );	// <titles>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n  " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		titlesNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( titlesNode );
	bReturn = video.writeXml (pDocument, &titlesNode);
	if (!bReturn)
		return false;
	for ( t=0; t<MAX_AUDIO; t++ )	{
		bReturn = audio[t].writeXml(pDocument, &titlesNode);
		if (!bReturn)
			return false;
	}
	for ( t=0; t<MAX_SUBTITLES; t++ )	{
		bReturn = subpicture[t].writeXml(pDocument, &titlesNode);
		if (!bReturn)
			return false;
	}
	t = 0;
	if (ppArrayPgc)	{
		while (ppArrayPgc[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayPgc[t]->writeXml(pDocument, &titlesNode);
			t++;
		}
	}
	return bReturn;
}

bool CXmlDVDAuthor::pgc_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::pgc_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::pgc_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement pgcNode = pDocument->createElement( node_name );	// <pgc>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		pgcNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!entry.isNull())
		pgcNode.setAttribute( PGC_ENTRY, entry );
	if (!palette.isNull())
		pgcNode.setAttribute( PGC_PALETTE, palette );
	if (!pause.isNull())
		pgcNode.setAttribute( PGC_PAUSE, pause );

	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( pgcNode );
	bReturn = pre.writeXml (pDocument, &pgcNode);
	if (ppArrayVob)	{
		while (ppArrayVob[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayVob[t]->writeXml(pDocument, &pgcNode);
			t++;
		}
	}
	t = 0;
	if (ppArrayButton)	{
		while (ppArrayButton[t])	{
			if (!bReturn)
				return false;
			bReturn = ppArrayButton[t]->writeXml(pDocument, &pgcNode);
			t++;
		}
	}
	if (!bReturn)
		return false;
	return post.writeXml (pDocument, &pgcNode);
}

bool CXmlDVDAuthor::button_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( (name.isNull()) && (value.isNull()) )
		return true;
	//debug_out ("CXmlDVDAuthor::button_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
	debug_out ("CXmlDVDAuthor::button_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement buttonNode = pDocument->createElement( node_name );	// <button>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		buttonNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( buttonNode );
	// Here we set the attributes of the <button> tag
	if (!name.isNull())
		buttonNode.setAttribute( BUTTON_NAME, name );
	QDomText domText = pDocument->createTextNode (value);
	buttonNode.appendChild( domText );
	return true;
}

bool CXmlDVDAuthor::cell_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( (start.isNull()) && (end.isNull()) && (program.isNull()) )
		return true;
	//debug_out ("CXmlDVDAuthor::cell_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::cell_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement cellNode = pDocument->createElement( node_name );	// <cell>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		cellNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!start.isNull())
		cellNode.setAttribute( CELL_START, start );
	if (!end.isNull())
		cellNode.setAttribute( CELL_END, end );
	if (!chapter.isNull())
		cellNode.setAttribute( CELL_CHAPTER, chapter );
	if (!program.isNull())
		cellNode.setAttribute( CELL_PROGRAM, program );
	if (!pause.isNull())
		cellNode.setAttribute( CELL_PAUSE, pause );
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( cellNode );
	return true;
}

bool CXmlDVDAuthor::vob_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlDVDAuthor::vob_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::vob_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement vobNode = pDocument->createElement( node_name );	// <vob>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		vobNode.appendChild ( domComment );
	}
	// Here we set the attributes of the <dvdauthor> tag
	if (!file.isNull())
		vobNode.setAttribute( VOB_FILE, file );
	if (!chapters.isNull())
		vobNode.setAttribute( VOB_CHAPTERS, chapters );
	if (!pause.isNull())
		vobNode.setAttribute( VOB_PAUSE, pause );
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( vobNode );
	return cell.writeXml (pDocument, &vobNode);
}

bool CXmlDVDAuthor::fpc_struct::writeXml ( QDomDocument *pDocument, QDomElement *pNodeElement )
{
	// First check if this node holds any information at all ...
	if ( value.isNull ( ) )
		return true;
	//debug_out ("CXmlDVDAuthor::fpc_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::fpc_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement fpcNode = pDocument->createElement ( node_name );	// <fpc>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		fpcNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild ( fpcNode );
	QDomText domText = pDocument->createTextNode ( value );
	fpcNode.appendChild ( domText );
	return true;
}

bool CXmlDVDAuthor::pre_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( value.isEmpty ( ) )
	  return true;
	//debug_out ("CXmlDVDAuthor::pre_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::pre_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement preNode = pDocument->createElement( node_name );	// <pre>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		preNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( preNode );
	QDomText domText = pDocument->createTextNode (value);
	preNode.appendChild( domText );
	return true;
}

bool CXmlDVDAuthor::post_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( value.isEmpty ( ) )
	  return true;
	//debug_out ("CXmlDVDAuthor::post_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlDVDAuthor::post_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement postNode = pDocument->createElement( node_name );	// <post>
	if ( ! comment.isEmpty ( ) )  {
		QString  qsComment  = QString ( "<!-- %1 // -->\n    " ).arg ( comment );
		QDomText domComment = pDocument->createTextNode ( qsComment );
		postNode.appendChild ( domComment );
	}
	// And now proceed to writing the rest of the file.
	QDomText domText = pDocument->createTextNode (value);
	postNode.appendChild( domText );
	pNodeElement->appendChild( postNode );
	return true;
}

// Private member functions :
CXmlSpumux::CXmlSpumux ()
{

}

CXmlSpumux::~CXmlSpumux ()
{

}

bool CXmlSpumux::readXml ()
{
  // Here we read in a xml - file and create the neccesary underlying structure.
  //
  // For now we are going to ask for the file name here and handle the QDom...
  // Later on this is done a level further up and only QDomNode * is sent.
  //
  //////////////////////////////////////////////////////////////////////////////////
  //QString fileName = QFileDialog::getOpenFileName ( QString("./"), QObject::tr ("XML files ( *.xml *.XML)"));					//ooo
  QString fileName = QFileDialog::getOpenFileName ( NULL, QObject::tr("Open File"), QString("./"), QObject::tr ("XML files ( *.xml *.XML)"));	//xxx
  return readXml ( fileName );
}

bool CXmlSpumux::readXml ( QString &fileName )
{
  // Assign the file
  QFile projectFile(fileName);
  if (!projectFile.open( QIODevice::ReadWrite))
    return false;
  
  QDomDocument xmlDoc( DVD_DOCTYPE );
  if (!xmlDoc.setContent (&projectFile))	{
    // Error handling ...
    projectFile.close();
    int iReturn = MessageBox::warning ( NULL, QObject::tr ("xml spumux file seems to be defective."),
					 QObject::tr ("Do you want to try to load another project file ?"),
					 QMessageBox::Yes, QMessageBox::No);
    if (iReturn == QMessageBox::Yes)
      return readXml (); // Okay, user wants to specify another project file.
  }
	// And at last lets try to read the information of the file.
  QDomElement docElem = xmlDoc.documentElement();
  bool bReturn = m_subpictures.readXml (&docElem);
  
  projectFile.close();
  return bReturn;
}

bool CXmlSpumux::writeXml ()
{
	// Here we read in a xml - file and create the neccesary underlying structure.
	//
	// For now we are going to ask for the file name here and handle the QDom...
	// Later on this is done a level further up and only QDomNode * is sent.
	//
	//////////////////////////////////////////////////////////////////////////////////
	debug_out ("CXmlSpumux::writeXml\n");
	//QString fileName = QFileDialog::getSaveFileName ( QString("./"), QObject::tr ("XML files ( *.xml)"));					//ooo
	QString fileName = QFileDialog::getSaveFileName ( NULL, QObject::tr("Save File"), QString("./"), QObject::tr ("XML files ( *.xml)"));	//xxx
	if (fileName.isNull())
		return false;
	return writeXml(fileName);
}

bool CXmlSpumux::writeXml ( QString &fileName )
{  
  //debug_out ( "CXmlSpumux::writeXml (%s)\n",(const char *)fileName );		//ooo
  debug_out ( "CXmlSpumux::writeXml (%s)\n",fileName.toLatin1().data() );		//xxx
  // Assign the file
  QFile  projectFile ( fileName );
  if ( ! projectFile.open ( QIODevice::WriteOnly ) )
    return false;

  QDomDocument xmlDoc( DVD_DOCTYPE );	// <""> for now.
  bool bRet = m_subpictures.writeXml (&xmlDoc);
  if (bRet)	{
    QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml += xmlDoc.toString (    );
    xml.replace    ( "&lt;", "<"); 
    //debug_out      ("%s\n", (const char *)xml );		//ooo
    debug_out      ("%s\n", xml.toLatin1().data() );	//xxx
    //xml = xml.utf8 ( );		//ooo
    xml = xml.toUtf8 ( );		//xxx
    //projectFile.writeBlock ( xml, qstrlen ( xml ) );					//ooo
    projectFile.write ( xml.toLatin1().data(), qstrlen ( xml.toLatin1().data() ) );		//xxx
  }

  projectFile.close();
  return bRet;
}

bool CXmlSpumux::subpictures_struct::writeXml(QDomDocument *pDocument, QDomElement *)
{
	//debug_out ("CXmlSpumux::subpictures_struct::writeXml <%s>\n",(const char *)node_name);	//ooo
	debug_out ("CXmlSpumux::subpictures_struct::writeXml <%s>\n",node_name.toLatin1().data());	//xxx

	QDomElement subpicturesNode = pDocument->createElement( node_name );	// <subpictures>
	// And now proceed to writing the rest of the file.
	pDocument->appendChild( subpicturesNode );
	return stream.writeXml (pDocument, &subpicturesNode);
}

bool CXmlSpumux::stream_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumux::stream_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::stream_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement streamNode = pDocument->createElement( node_name );	// <stream>
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( streamNode );
	if (ppArraySpu)	{
		while (ppArraySpu[t])	{
			bReturn = ppArraySpu[t]->writeXml(pDocument, &streamNode);
			if (!bReturn)
				return false;
			t++;
		}
	}
	t = 0;
	if (ppArrayTextsub)	{
		while (ppArrayTextsub[t])	{
			bReturn = ppArrayTextsub[t]->writeXml(pDocument, &streamNode);
			if (!bReturn)
				return false;
			t++;
		}
	}
	return true;
}

bool CXmlSpumux::spu_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	//debug_out ("CXmlSpumux::spu_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::spu_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	bool bReturn = true;
	int t = 0;
	QDomElement spuNode = pDocument->createElement( node_name );	// <spu>
	if (!start.isNull())
		spuNode.setAttribute( SPU_START, start );
	if (!end.isNull())
		spuNode.setAttribute( SPU_END, end );
	if (!image.isNull())
		spuNode.setAttribute( SPU_IMAGE, image );
	if (!highlight.isNull())
		spuNode.setAttribute( SPU_HIGHLIGHT, highlight );
	if (!select.isNull())
		spuNode.setAttribute( SPU_SELECT, select );
	if (!transparent.isNull())
		spuNode.setAttribute( SPU_TRANSPARENT, transparent );
	if (!force.isNull())
		spuNode.setAttribute( SPU_FORCE, force );
	if (!autooutline.isNull())
		spuNode.setAttribute( SPU_AUTOOUTLINE, autooutline );
	if (outlinewidth != -1)
		spuNode.setAttribute( SPU_OUTLINEWIDTH, QString ("%1").arg(outlinewidth) );
	if (!autoorder.isNull())
		spuNode.setAttribute( SPU_AUTOORDER, autoorder );
	if (xoffset != -1)
		spuNode.setAttribute( SPU_XOFFSET, QString ("%1").arg(xoffset) );
	if (yoffset != -1)
		spuNode.setAttribute( SPU_YOFFSET, QString ("%1").arg(yoffset) );
	// And now proceed to writing the rest of the file.
	pNodeElement->appendChild( spuNode );
	t=0;
	if (ppArrayButton)	{
		while (ppArrayButton[t])	{
			bReturn = ppArrayButton[t]->writeXml(pDocument, &spuNode);
			if (!bReturn)
				return false;
			t++;
		}
	}
	t=0;
	if (ppArrayAction)	{
		while (ppArrayAction[t])	{
			bReturn = ppArrayAction[t]->writeXml(pDocument, &spuNode);
			if (!bReturn)
				return false;
			t++;
		}
	}
	return true;
}
bool CXmlSpumux::textsub_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
  //debug_out ("CXmlSpumux::textsub_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);	//ooo
  debug_out ("CXmlSpumux::textsub_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
  QDomElement txtNode = pDocument->createElement( node_name );	// <spu>
  if ( ! filename.isNull())
    txtNode.setAttribute( TEXTSUB_FILENAME, filename );
  if ( ! characterset.isNull())
    txtNode.setAttribute( TEXTSUB_CHARACTERSET, characterset );
  if ( ! font.isNull())
    txtNode.setAttribute( TEXTSUB_FONT, font );
  if ( ! transparent.isNull())
    txtNode.setAttribute( TEXTSUB_TRANSPARENT, transparent );
  if ( fontsize != 0 )
    txtNode.setAttribute( TEXTSUB_FONTSIZE, fontsize );
  if ( ! horizontal_alignment.isNull ( ) )
    txtNode.setAttribute( TEXTSUB_HORIZ_ALIGNMENT, horizontal_alignment );
  if ( ! vertical_alignment.isNull ( ) )
    txtNode.setAttribute( TEXTSUB_VERT_ALIGNMENT, vertical_alignment );
  if ( left_margin != 0 )
    txtNode.setAttribute( TEXTSUB_LEFT_MARGIN, left_margin );
  if ( right_margin != 0 )
    txtNode.setAttribute( TEXTSUB_RIGHT_MARGIN, right_margin );
  if ( bottom_margin != 0 )
    txtNode.setAttribute( TEXTSUB_BOTTOM_MARGIN, bottom_margin );
  if ( top_margin != 0 )
    txtNode.setAttribute( TEXTSUB_TOP_MARGIN, top_margin );
  if ( subtitle_fps != 0.0 )
    txtNode.setAttribute( TEXTSUB_SUBTITLE_FPS, subtitle_fps );
  if ( movie_fps != 0 )
    txtNode.setAttribute( TEXTSUB_MOVIE_FPS, movie_fps );
  if ( movie_width != 0 )
    txtNode.setAttribute( TEXTSUB_MOVIE_WIDTH, movie_width );
  if ( movie_height != 0 )
    txtNode.setAttribute( TEXTSUB_MOVIE_HEIGHT, movie_height );

  pNodeElement->appendChild( txtNode );

  return true;
}
bool CXmlSpumux::button_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if ( (label.isNull()) && (x0 == -1) && (y0 == -1) && (x1 == -1) && (y1 == -1) &&
		(up.isNull()) && (down.isNull()) && (left.isNull()) && (right.isNull()) )
		return true;
	//debug_out ("CXmlSpumux::button_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::button_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement buttonNode = pDocument->createElement( node_name );	// <button>
	if (!label.isNull())
		buttonNode.setAttribute( BUTTON_LABEL, label );
	if (x0 != -1)
		buttonNode.setAttribute( BUTTON_X0, QString ("%1").arg(x0) );
	if (y0 != -1)
		buttonNode.setAttribute( BUTTON_Y0, QString ("%1").arg(y0) );
	if (x1 != -1)
		buttonNode.setAttribute( BUTTON_X1, QString ("%1").arg(x1) );
	if (y1 != -1)
		buttonNode.setAttribute( BUTTON_Y1, QString ("%1").arg(y1) );
	if (!up.isNull())
		buttonNode.setAttribute( BUTTON_UP, up );
	if (!down.isNull())
		buttonNode.setAttribute( BUTTON_DOWN, down );
	if (!left.isNull())
		buttonNode.setAttribute( BUTTON_LEFT, left );
	if (!right.isNull())
		buttonNode.setAttribute( BUTTON_RIGHT, right );
	pNodeElement->appendChild( buttonNode );
	return true;
}
bool CXmlSpumux::action_struct::writeXml(QDomDocument *pDocument, QDomElement *pNodeElement)
{
	// First check if this node holds any information at all ...
	if (label.isNull())
		return true;
	//debug_out ("CXmlSpumux::action_struct::writeXml <%s><%s>\n",(const char *)pNodeElement->tagName(), (const char *)node_name);		//ooo
	debug_out ("CXmlSpumux::action_struct::writeXml <%s><%s>\n",pNodeElement->tagName().toLatin1().data(), node_name.toLatin1().data());	//xxx
	QDomElement actionNode = pDocument->createElement( node_name );	// <action>
	if (!label.isNull())
#ifdef DVDAUTHOR_VERSION_LESS_0_6_10
		actionNode.setAttribute( ACTION_LABEL_OLD, label );
#else
		actionNode.setAttribute( ACTION_LABEL, label );
#endif
	pNodeElement->appendChild( actionNode );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
CXmlDVDAuthor::pgc_struct::pgc_struct ()
{
	ppArrayVob		=	NULL;
	ppArrayButton	=	NULL;
	node_name=QString("pgc");
}
CXmlDVDAuthor::pgc_struct::~pgc_struct ()
{
	// Lets clean up a little bit ...
	int t=0;
	if (ppArrayVob)	{
		vob_struct *pVob = ppArrayVob[t];
		while (pVob)	{
			delete pVob;
			t++;
			pVob = ppArrayVob[t];
		}
		delete []ppArrayVob;
	}
	t=0;
	if (ppArrayButton)	{
		button_struct *pButton = ppArrayButton[t];
		while (pButton)	{
			delete pButton;
			t++;
			pButton = ppArrayButton[t];
		}
		delete []ppArrayButton;
	}
}
CXmlDVDAuthor::menus_struct::menus_struct ()
{
	ppArrayPgc=NULL;
	node_name=QString("menus");
}

CXmlDVDAuthor::menus_struct::~menus_struct()
{
	// Lets clean up a little bit ...
	int t=0;
	if (ppArrayPgc)	{
		pgc_struct *pPgc = ppArrayPgc[t];
		while (pPgc)	{
			delete pPgc;
			t++;
			pPgc = ppArrayPgc[t];
		}
		delete []ppArrayPgc;
	}
}

CXmlDVDAuthor::titles_struct::titles_struct ()
{
	ppArrayPgc = NULL;
	node_name=QString("titles");
}
CXmlDVDAuthor::titles_struct::~titles_struct ()
{
	int t=0;
	if (ppArrayPgc)	{
		pgc_struct *pPgc  = ppArrayPgc[t];
		while (pPgc)	{
			delete pPgc;
			t++;
			pPgc = ppArrayPgc[t];
		}
		delete []ppArrayPgc;
	}
}

CXmlDVDAuthor::vmgm_struct::vmgm_struct ()
{
	ppArrayMenus = NULL;
	node_name=QString("vmgm");
}
CXmlDVDAuthor::vmgm_struct::~vmgm_struct ()
{
	int t=0;
	if (ppArrayMenus)	{
		menus_struct *pMenus  = ppArrayMenus[t];
		while (pMenus)	{
			delete pMenus;
			t++;
			pMenus = ppArrayMenus[t];
		}
		delete []ppArrayMenus;
	}
}

CXmlDVDAuthor::titleset_struct::titleset_struct ()
{
	ppArrayMenus  = NULL;
	ppArrayTitles = NULL;
	node_name=QString("titleset");
}
CXmlDVDAuthor::titleset_struct::~titleset_struct ()
{
	int t=0;
	if (ppArrayMenus)	{
		menus_struct *pMenus  = ppArrayMenus[t];
		while (pMenus)	{
			delete pMenus;
			t++;
			pMenus = ppArrayMenus[t];
		}
		delete []ppArrayMenus;
	}
	t=0;
	if (ppArrayTitles)	{
		titles_struct *pTitles  = ppArrayTitles[t];
		while (pTitles)	{
			delete pTitles;
			t++;
			pTitles = ppArrayTitles[t];
		}
		delete []ppArrayTitles;
	}
}

CXmlDVDAuthor::dvdauthor_struct::dvdauthor_struct ()
{
	ppArrayTitleset=NULL;
	node_name=QString("dvdauthor");
}

CXmlDVDAuthor::dvdauthor_struct::~dvdauthor_struct ()
{
	// Lets clean up a little bit ...
	int t=0;
	if (ppArrayTitleset)	{
		titleset_struct *pTitleset = ppArrayTitleset[t];
		while (pTitleset)	{
			delete pTitleset;
			t++;
			pTitleset = ppArrayTitleset[t];
		}
		delete []ppArrayTitleset;
	}
}

CXmlSpumux::stream_struct::stream_struct ()
{
	node_name      = QString ("stream");
	ppArraySpu     = NULL;
	ppArrayTextsub = NULL;
}
CXmlSpumux::stream_struct::~stream_struct ()
{
  // Lets clean up a little bit ...
  int t=0;
  if (ppArraySpu)	{
    spu_struct *pSpu = ppArraySpu[t];
    while (pSpu)	{
      delete pSpu;
      t++;
      pSpu = ppArraySpu[t];
    }
    delete []ppArraySpu;
  }
  t=0;
  if (ppArrayTextsub)  {
    textsub_struct *pTextsub = ppArrayTextsub[t];
    while  ( pTextsub )	{
      delete pTextsub;
      t++;
      pTextsub = ppArrayTextsub[t];
    }
    delete []ppArrayTextsub;
  }
}
CXmlSpumux::spu_struct::spu_struct ()
{
	node_name = QString ("spu");
	outlinewidth	= -1;
	xoffset 		= -1;
	yoffset 		= -1;
	ppArrayButton=NULL;
	ppArrayAction=NULL;
}
CXmlSpumux::spu_struct::~spu_struct ()
{
	int t = 0;
	if (ppArrayButton)	{
		button_struct *pButton = ppArrayButton[t];
		while (pButton)	{
			delete pButton;
			t++;
			pButton = ppArrayButton[t];
		}
		delete []ppArrayButton;
	}
	if (ppArrayAction)	{
		action_struct *pAction = ppArrayAction[t];
		while (pAction)	{
			delete pAction;
			t++;
			pAction = ppArrayAction[t];
		}
		delete []ppArrayAction;
	}
}
CXmlSpumux::textsub_struct::textsub_struct ()
{
  node_name = QString ("textsub");
  horizontal_alignment   = "center";
  vertical_alignment     = "center";
  fontsize = left_margin = right_margin = 0;
  bottom_margin = top_margin = movie_width = movie_height = 0;
  subtitle_fps = movie_fps = 0.0;
}
CXmlSpumux::textsub_struct::~textsub_struct ()
{
}

CXmlSpumux::button_struct::button_struct ()
{
	node_name = QString ("button");
	x0	= -1;
	y0	= -1;
	x1	= -1;
	y1	= -1;
}


CXmlDVDAuthor::menus_struct *CXmlDVDAuthor::vmgm_struct::addMenus ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayMenus)	{
		// This is the first ...
		ppArrayMenus=new menus_struct *[2];
		ppArrayMenus[0]=new menus_struct;
		ppArrayMenus[1]=NULL;
		return ppArrayMenus[0];
	}

	int i,t=0;
	menus_struct *pMenus = ppArrayMenus[t];
	menus_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pMenus)	{
		t ++;
		pMenus = ppArrayMenus[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new menus_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayMenus[i];
	}
	ppNewArray[i] = new menus_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayMenus;
	ppArrayMenus = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayMenus[i];
}

CXmlDVDAuthor::menus_struct *CXmlDVDAuthor::titleset_struct::addMenus ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayMenus)	{
		// This is the first ...
		ppArrayMenus=new menus_struct *[2];
		ppArrayMenus[0]=new menus_struct;
		ppArrayMenus[1]=NULL;
		return ppArrayMenus[0];
	}

	int i,t=0;
	menus_struct *pMenus = ppArrayMenus[t];
	menus_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pMenus)	{
		t ++;
		pMenus = ppArrayMenus[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new menus_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayMenus[i];
	}
	ppNewArray[i] = new menus_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayMenus;
	ppArrayMenus = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayMenus[i];
}

CXmlDVDAuthor::titles_struct *CXmlDVDAuthor::titleset_struct::addTitles ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayTitles)	{
		// This is the first ...
		ppArrayTitles=new titles_struct *[2];
		ppArrayTitles[0]=new titles_struct;
		ppArrayTitles[1]=NULL;
		return ppArrayTitles[0];
	}

	int i,t=0;
	titles_struct *pTitles = ppArrayTitles[t];
	titles_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pTitles)	{
		t ++;
		pTitles = ppArrayTitles[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new titles_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayTitles[i];
	}
	ppNewArray[i] = new titles_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayTitles;
	ppArrayTitles = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayTitles[i];
}

CXmlDVDAuthor::titleset_struct *CXmlDVDAuthor::dvdauthor_struct::addTitleset ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayTitleset)	{
		// This is the first ...
		ppArrayTitleset=new titleset_struct *[2];
		ppArrayTitleset[0]=new titleset_struct;
		ppArrayTitleset[1]=NULL;
		return ppArrayTitleset[0];
	}

	int i,t=0;
	titleset_struct *pTitleset = ppArrayTitleset[t];
	titleset_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pTitleset)	{
		t ++;
		pTitleset = ppArrayTitleset[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new titleset_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayTitleset[i];
	}
	ppNewArray[i] = new titleset_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayTitleset;
	ppArrayTitleset = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayTitleset[i];
}

CXmlDVDAuthor::pgc_struct *CXmlDVDAuthor::menus_struct::addPgc ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayPgc)	{
		// This is the first ...
		ppArrayPgc=new pgc_struct *[2];
		ppArrayPgc[0]=new pgc_struct;
		ppArrayPgc[1]=NULL;
		return ppArrayPgc[0];
	}

	int i,t=0;
	pgc_struct *pPgc = ppArrayPgc[t];
	pgc_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pPgc)	{
		t ++;
		pPgc = ppArrayPgc[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new pgc_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayPgc[i];
	}
	ppNewArray[i] = new pgc_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayPgc;
	ppArrayPgc = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayPgc[i];
}

CXmlDVDAuthor::button_struct *CXmlDVDAuthor::pgc_struct::addButton ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayButton)	{
		// This is the first ...
		ppArrayButton=new button_struct *[2];
		ppArrayButton[0]=new button_struct;
		ppArrayButton[1]=NULL;
		return ppArrayButton[0];
	}

	int i,t=0;
	button_struct *pButton = ppArrayButton[t];
	button_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pButton)	{
		t ++;
		pButton = ppArrayButton[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new button_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayButton[i];
	}
	ppNewArray[i] = new button_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayButton;
	ppArrayButton = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayButton[i];
}

bool CXmlDVDAuthor::vob_struct::operator == ( CXmlDVDAuthor::vob_struct &theOther )
{
  return ( ( comment  == theOther.comment  ) &&
           ( file     == theOther.file     ) &&
           ( chapters == theOther.chapters ) &&
           ( pause    == theOther.pause    ) );
}

CXmlDVDAuthor::vob_struct *CXmlDVDAuthor::pgc_struct::addVob ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayVob)	{
		// This is the first ...
		ppArrayVob=new vob_struct *[2];
		ppArrayVob[0]=new vob_struct;
		ppArrayVob[1]=NULL;
		return ppArrayVob[0];
	}

	int i,t=0;
	vob_struct *pVob = ppArrayVob[t];
	vob_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pVob)	{
		t ++;
		pVob = ppArrayVob[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new vob_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayVob[i];
	}
	ppNewArray[i] = new vob_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayVob;
	ppArrayVob = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayVob[i];
}

int CXmlDVDAuthor::titles_struct::pgcCount ( )
{
	int t=0;
	if ( ! ppArrayPgc )
		return 0;
	while ( ppArrayPgc[t] ) t++;
	return t;
}

CXmlDVDAuthor::pgc_struct *CXmlDVDAuthor::titles_struct::addPgc ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayPgc)	{
		// This is the first ...
		ppArrayPgc=new pgc_struct *[2];
		ppArrayPgc[0]=new pgc_struct;
		ppArrayPgc[1]=NULL;
		return ppArrayPgc[0];
	}

	int i,t=0;
	pgc_struct *pPgc = ppArrayPgc[t];
	pgc_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pPgc)	{
		t ++;
		pPgc = ppArrayPgc[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new pgc_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayPgc[i];
	}
	ppNewArray[i] = new pgc_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayPgc;
	ppArrayPgc = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayPgc[i];
}

CXmlSpumux::spu_struct *CXmlSpumux::stream_struct::addSpu ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArraySpu)	{
		// This is the first ...
		ppArraySpu=new spu_struct *[2];
		ppArraySpu[0]=new spu_struct;
		ppArraySpu[1]=NULL;
		return ppArraySpu[0];
	}

	int i,t=0;
	spu_struct *pSpu = ppArraySpu[t];
	spu_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pSpu)	{
		t ++;
		pSpu = ppArraySpu[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new spu_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArraySpu[i];
	}
	ppNewArray[i] = new spu_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArraySpu;
	ppArraySpu = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArraySpu[i];
}

CXmlSpumux::textsub_struct *CXmlSpumux::stream_struct::addTextsub ()
{
  // This function simply enlarges the array of available titlesets.
  // That'll keep the structure dynamic.
  if (!ppArrayTextsub)	{
    // This is the first ...
    ppArrayTextsub=new textsub_struct *[2];
    ppArrayTextsub[0]=new textsub_struct;
    ppArrayTextsub[1]=NULL;
    return ppArrayTextsub[0];
  }

  int i,t=0;
  textsub_struct *pTextsub = ppArrayTextsub[t];
  textsub_struct **ppNewArray=NULL, **ppOldArray=NULL;
  while (pTextsub) 
    pTextsub = ppArrayTextsub[++t];
  // Now we have the count of actual titlesets.
  ppNewArray=new textsub_struct *[t+2];
  for (i=0;i<t;i++) 
    ppNewArray[i]=ppArrayTextsub[i];

  ppNewArray[i] = new textsub_struct();
  ppNewArray[i+1] = NULL;
  // Now we can delete th old array (but not the contents.
  ppOldArray     = ppArrayTextsub;
  ppArrayTextsub = ppNewArray;
  delete []ppOldArray;
  // And finally return the latest addition ...
  return ppArrayTextsub[i];
}

CXmlSpumux::button_struct *CXmlSpumux::spu_struct::addButton ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayButton)	{
		// This is the first ...
		ppArrayButton=new button_struct *[2];
		ppArrayButton[0]=new button_struct;
		ppArrayButton[1]=NULL;
		return ppArrayButton[0];
	}

	int i,t=0;
	button_struct *pButton = ppArrayButton[t];
	button_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pButton)	{
		t ++;
		pButton = ppArrayButton[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new button_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayButton[i];
	}
	ppNewArray[i] = new button_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayButton;
	ppArrayButton = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayButton[i];
}

CXmlSpumux::action_struct *CXmlSpumux::spu_struct::addAction ()
{
	// This function simply enlarges the array of available titlesets.
	// That'll keep the structure dynamic.
	if (!ppArrayAction)	{
		// This is the first ...
		ppArrayAction=new action_struct *[2];
		ppArrayAction[0]=new action_struct;
		ppArrayAction[1]=NULL;
		return ppArrayAction[0];
	}

	int i,t=0;
	action_struct *pAction = ppArrayAction[t];
	action_struct **ppNewArray=NULL, **ppOldArray=NULL;
	while (pAction)	{
		t ++;
		pAction = ppArrayAction[t];
	}
	// Now we have the count of actual titlesets.
	ppNewArray=new action_struct *[t+2];
	for (i=0;i<t;i++)	{
		ppNewArray[i]=ppArrayAction[i];
	}
	ppNewArray[i] = new action_struct();
	ppNewArray[i+1] = NULL;
	// Now we can delete th old array (but not the contents.
	ppOldArray = ppArrayAction;
	ppArrayAction = ppNewArray;
	delete []ppOldArray;
	// And finally return the latest addition ...
	return ppArrayAction[i];
}

