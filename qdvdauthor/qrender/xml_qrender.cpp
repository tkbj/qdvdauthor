
#include <stdlib.h>

//#include <qdom.h>     //ooo
#include <QDomNode>		//xxx
#include <QList>
#include <qfile.h>

#include "xml_qrender.h"


XmlQRender::clip_struct::clip_struct ()
{
	// init the values ...
	node_name = QString (NODE_CLIP);
	x = y = width = height = startframe = 0;
}

XmlQRender::mmm_struct::mmm_struct ()
{
	// init the values ...
	node_name  = QString (NODE_MMM);
	tmpdir     = QString ("/tmp");
	resolution = QString ("720x480");
	format     = QString ("ntsc");
	images     = QString ("jpg");
	background = QString ("#000000");
	successive = 0;
	frames     = 0;
}

XmlQRender::qrender_struct::qrender_struct ()
{
	// init the values ...
	node_name  = QString (NODE_QRENDER);
}

XmlQRender::XmlQRender ()
{

}

XmlQRender::~XmlQRender ()
{

}

void XmlQRender::clear ()
{
	// Here we remove all objects currentlky in memory and clear all entries.
}

XmlQRender::qrender_struct &XmlQRender::qrender ()
{
	return m_qrender;
}

void XmlQRender::addStatus (QString qsStatusText)
{
	QString qsCommand;
 	qsCommand = QString ("echo \"%1\" >> \"%2\"").arg(qsStatusText).arg(m_qsStatusFile);
	int iRet = system (qsCommand);
	iRet = iRet;
}

bool XmlQRender::readXml (QString qsFileName)
{
	bool bReturn = false;
	// Assign the file
	QFile xmlFile(qsFileName);
	if (!xmlFile.open(IO_Raw | IO_ReadWrite))
		return false;

	QDomDocument xmlDoc;
	if (!xmlDoc.setContent (&xmlFile))	{
		// Error handling ...
		xmlFile.close();
		addStatus (QString ("echo \"Fatal Error : The XML file %1 seems to be defective. ABORTING.\n").arg(m_qsXmlFile));
	}
	// And at last lets try to read the information of the file.
	QDomElement docElem = xmlDoc.documentElement();
	bReturn = m_qrender.readXml (&docElem);
	
	xmlFile.close();
	return bReturn;

}

bool XmlQRender::qrender_struct::readXml(QDomElement *pNodeElement)
{
	if (pNodeElement->tagName() != node_name)
		return false;
	QDomNode    mmmNode    = pNodeElement->firstChild();
	QDomElement mmmElement = mmmNode.toElement ();
	return mmm.readXml (&mmmElement);
}

bool XmlQRender::mmm_struct::readXml(QDomElement *pNodeElement)
{
	if (pNodeElement->tagName() != node_name)
		return false;
	QDomAttr a = pNodeElement->attributeNode ( QRENDER_TMPDIR );
	tmpdir = a.value();
	a = pNodeElement->attributeNode ( QRENDER_RESOLUTION );
	resolution = a.value();
	a = pNodeElement->attributeNode ( QRENDER_FORMAT );
	format = a.value();
	a = pNodeElement->attributeNode ( QRENDER_IMAGES );
	images = a.value();
	a = pNodeElement->attributeNode ( QRENDER_SUCCESSIVE );
	successive = a.value().toInt();
	a = pNodeElement->attributeNode ( QRENDER_BACKGROUND );
	background = a.value();
	a = pNodeElement->attributeNode ( QRENDER_FRAMES );
	frames = a.value().toInt();
	a = pNodeElement->attributeNode ( QRENDER_LENGTH );
	length = a.value().toFloat();
	a = pNodeElement->attributeNode ( QRENDER_OUTPUT );
	output = a.value();

	// And now read in all remaining nodes and handle them accordingly.
	bool bReturn = true;
	clip_struct tempClip, *pNewClip;
	QDomNode xmlNode = pNodeElement->firstChild();
	while( !xmlNode.isNull() ) {
		QDomElement searchTree = xmlNode.toElement();
		if (searchTree.tagName() == tempClip.node_name)	{
			pNewClip = new clip_struct;
			bReturn = pNewClip->readXml(&searchTree);
		}
		// If there has been a problem then return false.
		if (!bReturn)
			return false;
		// Otherwise go to the next node ...
		xmlNode = xmlNode.nextSibling();
	}
	return true;
}

bool XmlQRender::clip_struct::readXml(QDomElement *pNodeElement)
{
	if (pNodeElement->tagName() != node_name)
		return false;
	QDomAttr a = pNodeElement->attributeNode ( CLIP_X );
	x = a.value().toInt();
	a = pNodeElement->attributeNode ( CLIP_Y );
	y = a.value().toInt();
	a = pNodeElement->attributeNode ( CLIP_VOB );
	vob = a.value();
	a = pNodeElement->attributeNode ( CLIP_STARTFRAME );
	startframe = a.value().toInt();
	a = pNodeElement->attributeNode ( CLIP_WIDTH );
	width = a.value().toInt();
	a = pNodeElement->attributeNode ( CLIP_HEIGHT );
	height = a.value().toInt();
	a = pNodeElement->attributeNode ( CLIP_MASK );
	mask = a.value().toInt();
	return true;
}

bool XmlQRender::writeXml (QString qsFileName)
{
	// First we create the document, ...
	QDomDocument xmlDoc;	// <""> for now.
	QDomElement rootQRender = xmlDoc.createElement( m_qrender.node_name );	// <qrender>

	if (!m_qrender.writeXml (&rootQRender))	{
		addStatus (QString ("echo \"Fatal Error : Could not generate xml structure XmlQRender::writeXml()\n"));
		return false;
	}

	// Then we open the file for writing ...
	QFile projectFile(qsFileName);
	if (!projectFile.open(IO_WriteOnly))	{
		addStatus (QString ("echo \"Fatal Error : Could not open xml file for writing\n"));
		return false;
	}
		
	QString xml = xmlDoc.toString();
	// printf ("%s\n", (const char *)xml);
	projectFile.writeBlock(xml, qstrlen (xml));

	projectFile.close();
	return true;
}

bool XmlQRender::qrender_struct::writeXml(QDomElement *pNodeElement)
{
	bool bReturn;
	QDomDocument xmlDoc = pNodeElement->ownerDocument();
	bReturn = mmm.writeXml (pNodeElement);
	xmlDoc.appendChild( *pNodeElement );
	return bReturn;
}

bool XmlQRender::mmm_struct::writeXml(QDomElement *pNodeElement)
{
	uint t;
	bool bReturn;
	QDomDocument xmlDoc = pNodeElement->ownerDocument();
	QDomElement mmmNode = xmlDoc.createElement( node_name );	// <mmm>

	if (!tmpdir.isNull())
		mmmNode.setAttribute( QRENDER_TMPDIR, tmpdir );
	if (!resolution.isNull())
		mmmNode.setAttribute( QRENDER_RESOLUTION, resolution );
	if (!format.isNull())
		mmmNode.setAttribute( QRENDER_FORMAT, format );
	if (!images.isNull())
		mmmNode.setAttribute( QRENDER_IMAGES, images );
	if (successive != 0)
		mmmNode.setAttribute( QRENDER_SUCCESSIVE, QString ("%1").arg(successive) );
	if (!background.isNull())
		mmmNode.setAttribute( QRENDER_BACKGROUND, background );
	if (startframe != 0)
		mmmNode.setAttribute( QRENDER_FRAMES, QString ("%1").arg(startframe) );
	if (frames != 0)
		mmmNode.setAttribute( QRENDER_FRAMES, QString ("%1").arg(frames) );
	if (offset != 0.0)
		mmmNode.setAttribute( QRENDER_OFFSET, QString ("%1").arg(offset) );
	if (frames != 0.0)
		mmmNode.setAttribute( QRENDER_FRAMES, QString ("%1").arg(frames) );
	if (!output.isNull())
		mmmNode.setAttribute( QRENDER_OUTPUT, output );
	
	pNodeElement->appendChild( mmmNode );
	for (t=0;t<listOfClips.count();t++)	{
		bReturn = listOfClips[t]->writeXml(&mmmNode);
		if (bReturn == false)
			return false;
	}
	return true;
}

bool XmlQRender::clip_struct::writeXml(QDomElement *pNodeElement)
{
	QDomDocument xmlDoc = pNodeElement->ownerDocument();
	QDomElement clipNode = xmlDoc.createElement( node_name );	// <pgc>
	// Here we set the attributes of the <dvdauthor> tag
	if (x != 0)
		clipNode.setAttribute( CLIP_X, QString ("%1").arg(x) );
	if (y != 0)
		clipNode.setAttribute( CLIP_Y, QString ("%1").arg(y) );
	if (width != 0)
		clipNode.setAttribute( CLIP_WIDTH , QString ("%1").arg(width) );
	if (height != 0)
		clipNode.setAttribute( CLIP_HEIGHT, QString ("%1").arg(height) );
	if (startframe != 0)
		clipNode.setAttribute( CLIP_STARTFRAME, QString ("%1").arg(startframe) );
	if (frames != 0)
		clipNode.setAttribute( CLIP_FRAMES, QString ("%1").arg(frames) );
	if (offset != 0.0)
		clipNode.setAttribute( CLIP_OFFSET, QString ("%1").arg(offset) );
	if (length != 0.0)
		clipNode.setAttribute( CLIP_LENGTH, QString ("%1").arg(length) );
	if (!vob.isNull())
		clipNode.setAttribute( CLIP_VOB, vob );
	if (!mask.isNull())
		clipNode.setAttribute( CLIP_MASK, mask );

	pNodeElement->appendChild( clipNode );
	return true;
}

// comparison operator ...
bool XmlQRender::operator == (XmlQRender &)
{
	bool   bReturn = false;
	return bReturn;
}

