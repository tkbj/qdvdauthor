/***************************************************************************
    sourcefileentry.cpp
                             -------------------
    SourceFileEntry - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is a simple container class to store the info
   for a entry in the Source File List.
    
****************************************************************************/

//#include <qfileinfo.h>	//ooo
#include <QFileInfo>		//xxx
//#include <qimage.h>		//ooo
#include <QImage>		//xxx
//#include <qdom.h>		//ooo
#include <QtXml/QDomElement>	//xxx
//#include <qdatetime.h>	//ooo
#include <QDateTime>		//xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QMessageBox>		//xxx

#include "global.h"
#include "utils.h"
#include "xml_dvd.h"
#include "messagebox.h"
#include "sourcefileentry.h"
#include "xml_slideshow.h"
//#include "render_client.h"	//ooo

AudioEntry::AudioEntry ( )
{
  iAudioNumber = -1;
  qsIso639     = Global::qsDefaultLanguage;
}

AudioEntry &AudioEntry::operator= ( AudioEntry &theOther )
{
  iAudioNumber = theOther.iAudioNumber;
  qsIso639     = theOther.qsIso639;
  return *this;
}

Audio::Audio ( )
{
  iAudioNumber        =   -1;
  pTranscodeInterface = NULL;
}

Audio::~Audio ( )
{
  if ( pTranscodeInterface )
    delete pTranscodeInterface;
  pTranscodeInterface = NULL;
}

Audio &Audio::operator = ( Audio &theOther )
{
  iAudioNumber = theOther.iAudioNumber;
  qsFileName   = theOther.qsFileName;
  qsIso639     = theOther.qsIso639;
  if ( pTranscodeInterface )
    delete pTranscodeInterface;
  pTranscodeInterface = NULL;
  if ( theOther.pTranscodeInterface ) {
     pTranscodeInterface =  new TranscodeInterface;
    *pTranscodeInterface = *theOther.pTranscodeInterface;
  }
  return *this;
}

bool Audio::operator == ( Audio &theOther )
{
  bool bReturn = (
    ( iAudioNumber == theOther.iAudioNumber ) &&
    ( qsFileName   == theOther.qsFileName   ) &&
    ( qsIso639     == theOther.qsIso639     ) );
  if ( ! bReturn )
    return false;

  if ( pTranscodeInterface && theOther.pTranscodeInterface )
    return *pTranscodeInterface == *theOther.pTranscodeInterface;
  if ( ! pTranscodeInterface && ! theOther.pTranscodeInterface )
    return true;

  return false;
}

bool Audio::readProjectFile  ( QDomNode &xmlNode )
{
  QString     qsAttribute, qsFile, qsTagName;
  QDomElement theElement = xmlNode.toElement ( );
  QDomNode    infoNode;

  qsAttribute = theElement.attributeNode ( AUDIO_NUMBER ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    iAudioNumber = qsAttribute.toInt ();
  qsAttribute = theElement.attributeNode ( AUDIO_LANGUAGE ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsIso639 = qsAttribute;

  qsFile = theElement.text ();
  if ( ! qsFile.isEmpty ( ) )
    qsFileName = qsFile;

  infoNode = theElement.firstChild ( );
  while (!infoNode.isNull())    {
    qsTagName = infoNode.toElement ( ).tagName();
    if ( SOURCE_OBJECT_TRANSCODE == qsTagName ) {
      if ( pTranscodeInterface )
        delete pTranscodeInterface;
      pTranscodeInterface = new TranscodeInterface;
      pTranscodeInterface->readProjectFile ( infoNode );
    }
    infoNode = infoNode.nextSibling ( );
  }
  return true;
}

bool Audio::writeProjectFile ( QDomElement &inElement )
{ 
  QDomDocument xmlDoc = inElement.ownerDocument();
  QDomElement  tmpFileElement, theElement, transcodeElement;
  QDomText     text;

  theElement = xmlDoc.createElement ( SOURCE_OBJECT_AUDIO );

  // Here we set the attributes of the <dvdauthor> tag
  theElement.setAttribute( AUDIO_NUMBER, QString ("%1").arg ( (int)iAudioNumber ) );
  theElement.setAttribute( AUDIO_LANGUAGE,   qsIso639 );

  // store the file name too ...
  tmpFileElement = xmlDoc.createElement( AUDIO_FILE_NAME );
  //text = xmlDoc.createTextNode( qsFileName.utf8 ( ) );	//ooo
  text = xmlDoc.createTextNode( qsFileName.toUtf8 ( ) );	//xxx
  tmpFileElement.appendChild( text );
  theElement.appendChild ( tmpFileElement );
  if ( pTranscodeInterface )  {
    transcodeElement = xmlDoc.createElement( SOURCE_OBJECT_TRANSCODE );
    pTranscodeInterface->writeProjectFile  ( transcodeElement );
    theElement.appendChild( transcodeElement );
  }

  inElement.appendChild ( theElement );
  return true;
}

TranscodeInterface::TranscodeInterface ()
{
	iVideoBitrate = 0;
	fFrameRate    = 0.0f;
	iAudioBitrate = 0;
	iSample       = 0;
	iStartingAt   = 0;
	iEndingAt     = 1000000000;  // defaulting to a large number (1.000.000 sec)...
	bTranscoding  = false;
	bRemuxOnly    = false; 
	iSubtitleFromMetaInfo = -1;
}

TranscodeInterface::~TranscodeInterface ()
{

}

TranscodeInterface & TranscodeInterface::operator = (TranscodeInterface &theOther)
{
	qsVideoFormat = theOther.qsVideoFormat;
	qsVideoType   = theOther.qsVideoType;
	qsResolution  = theOther.qsResolution;
	iVideoBitrate = theOther.iVideoBitrate;
	fFrameRate    = theOther.fFrameRate;
	qsRatio       = theOther.qsRatio;
	qsAudioFormat = theOther.qsAudioFormat;
	qsAudioType   = theOther.qsAudioType;
	iAudioBitrate = theOther.iAudioBitrate;
	iSample       = theOther.iSample;
	iStartingAt   = theOther.iStartingAt;
	iEndingAt     = theOther.iEndingAt;
	bTranscoding  = theOther.bTranscoding;
	bRemuxOnly    = theOther.bRemuxOnly;
	qsTempFile    = theOther.qsTempFile;
	iSubtitleFromMetaInfo = theOther.iSubtitleFromMetaInfo;
	return *this;
}

bool TranscodeInterface::operator == (const TranscodeInterface &theOther)
{
	if ( 	( qsVideoFormat == theOther.qsVideoFormat ) && 
		( qsVideoType   == theOther.qsVideoType   ) &&
		( qsResolution  == theOther.qsResolution  ) &&
		( iVideoBitrate == theOther.iVideoBitrate ) &&
		( fFrameRate    == theOther.fFrameRate    ) &&
		( qsRatio       == theOther.qsRatio       ) &&
		( qsAudioFormat == theOther.qsAudioFormat ) &&
		( qsAudioType   == theOther.qsAudioType   ) &&
		( iAudioBitrate == theOther.iAudioBitrate ) &&
		( iSample       == theOther.iSample       ) &&
		( bRemuxOnly    == theOther.bRemuxOnly    ) &&
		( iSubtitleFromMetaInfo == theOther.iSubtitleFromMetaInfo ) &&
		( qsTempFile    == theOther.qsTempFile    ) )
		// The following can be different but the transcoding will still output the same Format.
//		( iEndingAt - iStartingAt == theOther.iEndingAt - theOther.iStartingAt ) )
		return true;
	return false;
}

bool TranscodeInterface::operator != (const TranscodeInterface &theOther)
{
	return !(*this == theOther);
}

bool TranscodeInterface::readProjectFile  (QDomNode &xmlNode)
{ 
  QString qsAttribute;
  QDomElement theElement = xmlNode.toElement();

  qsAttribute = theElement.attributeNode ( TRANSCODE_VIDEO_FORMAT ).value();
  if ( ! qsAttribute.isEmpty())
    qsVideoFormat = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_VIDEO_TYPE ).value();
  if ( ! qsAttribute.isEmpty())
    qsVideoType = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_RESOLUTION ).value();
  if ( ! qsAttribute.isEmpty())
    qsResolution = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_VIDEO_BITRATE ).value();
  if ( ! qsAttribute.isEmpty())
    iVideoBitrate = qsAttribute.toInt ();
  qsAttribute = theElement.attributeNode ( TRANSCODE_FRAME_RATE ).value();
  if ( ! qsAttribute.isEmpty())
    fFrameRate = qsAttribute.toFloat ();
  qsAttribute = theElement.attributeNode ( TRANSCODE_RATIO ).value();
  if ( ! qsAttribute.isEmpty())
    qsRatio = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_AUDIO_FORMAT ).value();
  if ( ! qsAttribute.isEmpty())
    qsAudioFormat = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_AUDIO_TYPE ).value();
  if ( ! qsAttribute.isEmpty())
    qsAudioType = qsAttribute;
  qsAttribute = theElement.attributeNode ( TRANSCODE_AUDIO_BITRATE ).value();
  if ( ! qsAttribute.isEmpty())
    iAudioBitrate = qsAttribute.toInt ();
  qsAttribute = theElement.attributeNode ( TRANSCODE_SAMPLE_RATE ).value();
  if ( ! qsAttribute.isEmpty())
    iSample = qsAttribute.toInt();
  qsAttribute = theElement.attributeNode ( TRANSCODE_STARTING_AT ).value();
  if ( ! qsAttribute.isEmpty())
    iStartingAt = qsAttribute.toInt();
  qsAttribute = theElement.attributeNode ( TRANSCODE_ENDING_AT ).value();
  if ( ! qsAttribute.isEmpty())
    iEndingAt = qsAttribute.toInt ();
  qsAttribute = theElement.attributeNode ( TRANSCODE_REMUX_ONLY ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    bRemuxOnly = ( qsAttribute == "true" ) ? true : false;
  qsAttribute = theElement.attributeNode ( TRANSCODE_SUBTITLE_FROM_META ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    iSubtitleFromMetaInfo = qsAttribute.toInt ();

  // Always get the TempFileName, empty or not ... here I come ... you can't run ...
  qsTempFile = theElement.text ();    

  return true;
}

bool TranscodeInterface::writeProjectFile (QDomElement &theElement)
{
	// Here we set the attributes of the <TranscodeInterface> tag
	if (!qsVideoFormat.isNull())
		theElement.setAttribute( TRANSCODE_VIDEO_FORMAT, qsVideoFormat );
	if (!qsVideoType.isNull())
		theElement.setAttribute( TRANSCODE_VIDEO_TYPE, qsVideoType );
	if (!qsResolution.isNull())
		theElement.setAttribute( TRANSCODE_RESOLUTION, qsResolution );
	if (iVideoBitrate > 0)
		theElement.setAttribute( TRANSCODE_VIDEO_BITRATE, QString ("%1").arg(iVideoBitrate) );
	if (fFrameRate > 0.0)
		theElement.setAttribute( TRANSCODE_FRAME_RATE, QString ("%1").arg(fFrameRate) );
	if (!qsRatio.isNull())
		theElement.setAttribute( TRANSCODE_RATIO, qsRatio );
	if (!qsAudioFormat.isNull())
		theElement.setAttribute( TRANSCODE_AUDIO_FORMAT, qsAudioFormat );
	if (!qsAudioType.isNull())
		theElement.setAttribute( TRANSCODE_AUDIO_TYPE, qsAudioType );
	if (iAudioBitrate > 0)
		theElement.setAttribute( TRANSCODE_AUDIO_BITRATE, QString ("%1").arg(iAudioBitrate) );
	if (iSample > 0)
		theElement.setAttribute( TRANSCODE_SAMPLE_RATE, QString ("%1").arg(iSample) );
	if (iStartingAt > 0)
		theElement.setAttribute( TRANSCODE_STARTING_AT, QString ("%1").arg(iStartingAt) );
	if (iEndingAt > 0)
		theElement.setAttribute( TRANSCODE_ENDING_AT, QString ("%1").arg(iEndingAt) );
	if ( bRemuxOnly )
	  theElement.setAttribute( TRANSCODE_REMUX_ONLY, QString ("true") );
	if (iSubtitleFromMetaInfo != -1)
	  theElement.setAttribute( TRANSCODE_SUBTITLE_FROM_META, QString ("%1").arg(iSubtitleFromMetaInfo) );
	if ( ! qsTempFile.isEmpty ( ) ) {	  
	  QDomDocument xmlDoc = theElement.ownerDocument( );
	  QDomText     text   = xmlDoc.createTextNode ( qsTempFile );
	  theElement.appendChild( text );
	}

	return true;
}

SourceFileInfo::SourceFileInfo ()
{
  pSlideshow     = NULL;
  pPreview       = NULL;
  bAutotranscode = true;
  bMetaSubtitles = true;
  bHasAudioTrack = true;
  pTranscodeInterface = NULL;
  bUpdateInfo    = false;
  iPause         =  0;
  iMSecPreview   =  0;
  iTitleset      = -1;
  for ( int t=0; t<MAX_SUBTITLES; t++ )
    arraySubtitles[t] = NULL;
  for ( int t=0; t<MAX_AUDIO; t++ )
    arrayAudio[t] = NULL;
}

SourceFileInfo::~SourceFileInfo ()
{
  if ( pPreview )
    delete pPreview;
  if ( pTranscodeInterface )
    delete pTranscodeInterface;
  if ( pSlideshow )
    delete pSlideshow;

  deleteSubtitles ( );
  deleteAudio     ( );
}

int SourceFileInfo::format ( )
{
  Utils theUtils;
  int iFormat = 0;
  if ( pTranscodeInterface )
    iFormat = theUtils.getFormat ( pTranscodeInterface->qsResolution );
  else
    iFormat = theUtils.getFormat ( qsResolution );

  return iFormat;
}

int SourceFileInfo::audioCount ( )
{
  int t, iCount = 0;
  for ( t=0; t<MAX_AUDIO; t++ ) {
    if ( arrayAudio[t] )
      iCount ++;
  }
  return iCount;
}

void SourceFileInfo::deleteAudio ( )
{
  int t;
  for ( t=0; t<MAX_AUDIO; t++ ) {
    if ( arrayAudio[t] )
      delete arrayAudio[t];
    arrayAudio[t] = NULL;
  }
}

void SourceFileInfo::addAudio ( Audio *pAudio )
{
  if ( ! pAudio )
    return;

  if ( arrayAudio[ pAudio->iAudioNumber ] )
    delete arrayAudio[ pAudio->iAudioNumber ];

  arrayAudio[ pAudio->iAudioNumber ] = pAudio;
}

int SourceFileInfo::subtitleCount ( )
{
  int t, iCount = 0;
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    if ( arraySubtitles[t] )
      iCount ++;
  }
  return iCount;
}

void SourceFileInfo::deleteSubtitles ( )
{
  int t;
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    if ( arraySubtitles[t] )
      delete arraySubtitles[t];
    arraySubtitles[t] = NULL;
  }
}

void SourceFileInfo::addSubtitles ( Subtitles *pSubtitle )
{
  if ( ! pSubtitle )
    return;

  if ( arraySubtitles[ pSubtitle->m_iSubtitleNumber ] )
    delete arraySubtitles[ pSubtitle->m_iSubtitleNumber ];

  arraySubtitles[ pSubtitle->m_iSubtitleNumber ] = pSubtitle;
}

// The filename for the dvdauthor.xml file depends on the pre-processing
QString SourceFileInfo::getFileName ()
{
  Utils     theUtils;
  QFileInfo fileInfo ( qsFileName );
  if ( subtitleCount ( ) > 0 ) {
    int t;
    QString qsReturn;
    for ( t=0; t<MAX_SUBTITLES; t++ ) {
      if ( arraySubtitles[t] ) // the last file name wins.
        qsReturn = arraySubtitles[t]->m_qsTempFile;
    }
    if ( qsReturn.isEmpty ( ) )
      //qsReturn = QString ( "%1/subtitles.vob" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( TRUE ) ) );	//ooo
      qsReturn = QString ( "%1/subtitles.vob" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( ) ) );		//xxx
    return qsReturn;
  }
  if ( pTranscodeInterface ) {
    if ( ! pTranscodeInterface->qsTempFile.isEmpty () )
      return pTranscodeInterface->qsTempFile;
    else  // nothing in so we should create the file name
      //return QString ( "%1/%2.mpeg2" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg( fileInfo.baseName ( TRUE ) );	//ooo
      return QString ( "%1/%2.mpeg2" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( ) ) ).arg( fileInfo.baseName ( ) );		//xxx
  }
  else if ( audioCount ( ) > 0 )
    //return QString ( "%1/%2.mpeg2" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg( fileInfo.baseName ( TRUE ) );	//ooo
    return QString ( "%1/%2.mpeg2" ).arg ( theUtils.getTempPath ( fileInfo.baseName ( ) ) ).arg( fileInfo.baseName ( ) );		//xxx

  return qsFileName;
}

bool SourceFileInfo::readProjectFile ( QDomNode &xmlNode )
{
  QDomElement theElement = xmlNode.toElement  ( );

  // Okay, here we read the stored data from the xml file.
  QString qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_SIZE).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsSize = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_RESOLUTION).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsResolution = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_FPS).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsFPS = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_RATIO).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsRatio = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_LENGTH).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsLength = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_VIDEO_FORMAT).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsVideoFormat = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_VIDEO_CODEC).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsVideoCodec = qsAttribute;

  qsAttribute = theElement.attributeNode ( SOURCE_OBJECT_HAS_AUDIO ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    bHasAudioTrack = ( qsAttribute == "true" );

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_AUDIO_FORMAT).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsAudioFormat = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_AUDIO_CODEC).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsAudioCodec  = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_STATUS).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsStatus = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_BITS).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsBits = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_SAMPLE).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsSample = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_BPS).value();
  if ( ! qsAttribute.isEmpty ( ) )
    qsBPS = qsAttribute;

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_PAUSE).value();
  if ( ! qsAttribute.isEmpty ( ) )
    iPause = qsAttribute.toInt();

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_TITLESET).value();
  if ( ! qsAttribute.isEmpty ( ) )
    iTitleset = qsAttribute.toInt();

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_AUTOTRANSCODE).value();
  bAutotranscode = true;
  if ( ! qsAttribute.isEmpty ( ) )
    bAutotranscode = ( qsAttribute == QString ("true") );

  qsAttribute = theElement.attributeNode (SOURCE_OBJECT_METASUBTITLES).value();
  bMetaSubtitles = true;
  if ( ! qsAttribute.isEmpty ( ) )
    bMetaSubtitles = ( qsAttribute == QString ("true") );

  qsAttribute = theElement.attributeNode ( SOURCE_OBJECT_MSEC_OFFSET ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    iMSecPreview = qsAttribute.toInt ( );

  QDomNode infoNode = theElement.firstChild ( );
  deleteSubtitles ( );
  while ( ! infoNode.isNull ( ) )  {
    QDomElement infoElement = infoNode.toElement( );
    // Okay, here we read the stored data from the xml file.
    QString infoName = infoElement.tagName  ( );
    QString infoText = infoElement.text     ( );
    if (SOURCE_OBJECT_FILE_NAME == infoName)	{
      if ( ! infoText.isEmpty ( ) )
        qsFileName = infoText;
    }
    else if ( SOURCE_OBJECT_CHAPTERS == infoName )  {
      if ( ! infoText.isEmpty ( ) )
        //listChapters = QStringList::split (",", infoText);	//ooo
	listChapters = infoText.split (",");			//xxx
    }
    else if ( SOURCE_OBJECT_PROPERTIES == infoName )  {
      qsAttribute = infoElement.attributeNode ( SOURCE_PROPERTIES_POST ).value ( );
      if ( ! qsAttribute.isEmpty ( ) )
        qsPostCommand = qsAttribute;

      qsAttribute = infoElement.attributeNode (SOURCE_PROPERTIES_PRE).value();
      if ( ! qsAttribute.isEmpty ( ) )
        qsPreCommand = qsAttribute;

      qsAttribute = infoElement.attributeNode (SOURCE_PROPERTIES_PAUSE).value();
      if ( ! qsAttribute.isEmpty ( ) )
        iPause = qsAttribute.toInt ( );
    }
    else if (SOURCE_OBJECT_TRANSCODE == infoName)	{
      if ( pTranscodeInterface)
        delete pTranscodeInterface;
      pTranscodeInterface = new TranscodeInterface;
      pTranscodeInterface->readProjectFile (infoNode);
    }
    else if ( SLIDESHOW_TAG == infoName )	{
      if ( pSlideshow )
        delete pSlideshow;
      pSlideshow = new CXmlSlideshow;
      QDomElement theElement = infoNode.toElement ( );
      pSlideshow->readXml ( &theElement );
    }
    else if (SOURCE_OBJECT_SUBTITLES == infoName)	{
      Subtitles *pSubtitle = new Subtitles ( 0, 0 );
      pSubtitle->readProjectFile ( infoNode );
      addSubtitles   ( pSubtitle );
    }
    else if (SOURCE_OBJECT_AUDIO == infoName)	{
      Audio *pAudio = new Audio;
      pAudio->readProjectFile ( infoNode );
      addAudio    ( pAudio   );
    }
    infoNode = infoNode.nextSibling ();
  }
  return true;
}

bool SourceFileInfo::writeProjectFile ( QDomElement &sourceNode, bool /* bAutosave */ )
{
  uint i;
  QDomDocument xmlDoc = sourceNode.ownerDocument ( );
  QDomElement  fileElement, infoElement, chapterElement, propertiesElement;
  QDomElement  tag,  slideshowElement, transcodeElement, subtitlesElement;
  QDomText     text;
  // Here we set the attributes of the <SourceInfo> tag

  infoElement = xmlDoc.createElement( SOURCE_OBJECT_INFO );
  if ( ! qsSize.isNull ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_SIZE, qsSize );
  if ( ! qsResolution.isNull ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_RESOLUTION, qsResolution );
  if ( ! qsFPS.isNull        ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_FPS, qsFPS );
  if ( ! qsRatio.isNull      ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_RATIO, qsRatio );
  if ( ! qsLength.isNull     ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_LENGTH, qsLength );
  if ( ! qsVideoFormat.isNull( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_VIDEO_FORMAT, qsVideoFormat );
  if ( ! qsVideoCodec.isNull ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_VIDEO_CODEC,  qsVideoCodec  );
  infoElement.setAttribute   ( SOURCE_OBJECT_HAS_AUDIO, bHasAudioTrack ? "true" : "false" );
  if ( ! qsAudioFormat.isNull( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_AUDIO_FORMAT, qsAudioFormat );
  if ( ! qsAudioCodec.isNull ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_AUDIO_CODEC,  qsAudioCodec  );
  if ( ! qsStatus.isNull     ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_STATUS, qsStatus );
  if ( ! qsBits.isNull       ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_BITS, qsBits );
  if ( ! qsSample.isNull     ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_SAMPLE, qsSample );
  if ( ! qsBPS.isNull        ( ) )
    infoElement.setAttribute ( SOURCE_OBJECT_BPS, qsBPS );
  if ( iPause != -1 )
    infoElement.setAttribute ( SOURCE_OBJECT_PAUSE, QString ("%1").arg(iPause) );
  if ( iTitleset != -1 )
    infoElement.setAttribute ( SOURCE_OBJECT_TITLESET, QString ("%1").arg(iTitleset) );
  if ( bAutotranscode == false )
    infoElement.setAttribute ( SOURCE_OBJECT_AUTOTRANSCODE, "false" );
  if ( bMetaSubtitles == false )
    infoElement.setAttribute ( SOURCE_OBJECT_METASUBTITLES, "false" );
  if ( iMSecPreview != -1 )
    infoElement.setAttribute ( SOURCE_OBJECT_MSEC_OFFSET, QString ("%1").arg(iMSecPreview) );

  // Here we create another level in the XML tree for name and chapter ...
  // First the file name
  fileElement = xmlDoc.createElement( SOURCE_OBJECT_FILE_NAME );
  text = xmlDoc.createTextNode ( qsFileName );
  fileElement.appendChild( text );
  infoElement.appendChild( fileElement );
  // Second the properties (Post / Pre, and Pause)
  propertiesElement = xmlDoc.createElement( SOURCE_OBJECT_PROPERTIES );
  if (!qsPostCommand.isNull())
    propertiesElement.setAttribute( SOURCE_PROPERTIES_POST, qsPostCommand );
  if (!qsPreCommand.isNull())
    propertiesElement.setAttribute( SOURCE_PROPERTIES_PRE, qsPreCommand );
  propertiesElement.setAttribute( SOURCE_PROPERTIES_PAUSE, QString ("%1").arg(iPause ));
  infoElement.appendChild( propertiesElement );
  // Then chapters if avail.
  if (listChapters.count() > 0)	{
    chapterElement = xmlDoc.createElement( SOURCE_OBJECT_CHAPTERS );
    text = xmlDoc.createTextNode ( listChapters.join(QString(",")));
    chapterElement.appendChild( text );
    infoElement.appendChild( chapterElement );
  }
  // next to last the TranscodeInterface if any.
  if (pTranscodeInterface)	{
    transcodeElement = xmlDoc.createElement( SOURCE_OBJECT_TRANSCODE );
    pTranscodeInterface->writeProjectFile (transcodeElement);
    infoElement.appendChild( transcodeElement );
  }
  if ( pSlideshow )  {
    slideshowElement = xmlDoc.createElement (  SLIDESHOW_TAG );
    pSlideshow->writeXml ( &slideshowElement );
    infoElement.appendChild ( slideshowElement );
  }

  // Next to last we add the audio channels.
  for ( i=0; i<MAX_AUDIO; i++ ) {
    if ( arrayAudio[i] )
      arrayAudio[i]->writeProjectFile ( infoElement );
  }
  // And finally the Subtitles
  for ( i=0; i<MAX_SUBTITLES; i++ ) {
    if ( arraySubtitles[i] )
      arraySubtitles[i]->writeProjectFile ( infoElement );
  }
  sourceNode.appendChild ( infoElement );

  return true;
}

SourceFileInfo & SourceFileInfo::operator = (SourceFileInfo &theOther)
{
  int t;
  qsFileName     = theOther.qsFileName;
  qsSize         = theOther.qsSize;
  qsResolution   = theOther.qsResolution;
  qsFPS          = theOther.qsFPS;
  qsRatio        = theOther.qsRatio;
  qsLength       = theOther.qsLength;
  bHasAudioTrack = theOther.bHasAudioTrack;
  qsAudioFormat  = theOther.qsAudioFormat;
  qsAudioCodec   = theOther.qsAudioCodec;
  qsVideoFormat  = theOther.qsVideoFormat;
  qsVideoCodec   = theOther.qsVideoCodec;
  qsStatus       = theOther.qsStatus;
  qsBits         = theOther.qsBits;
  qsSample       = theOther.qsSample;
  qsBPS          = theOther.qsBPS;
  listChapters   = theOther.listChapters;
  qsPreCommand   = theOther.qsPreCommand;
  qsPostCommand  = theOther.qsPostCommand;
  iPause         = theOther.iPause;
  iMSecPreview   = theOther.iMSecPreview;
  iTitleset      = theOther.iTitleset;
  bAutotranscode = theOther.bAutotranscode;
  bMetaSubtitles = theOther.bMetaSubtitles;
  if (theOther.pTranscodeInterface)	{
    if (!pTranscodeInterface)
      pTranscodeInterface  = new TranscodeInterface ();
    *(pTranscodeInterface) = *(theOther.pTranscodeInterface);
  }
  else if ( pTranscodeInterface )	{
    delete pTranscodeInterface;
    pTranscodeInterface = NULL;
  }

  deleteSubtitles ( );
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    if ( theOther.arraySubtitles [ t ] ) {
        arraySubtitles [ t ]  = new Subtitles ( 0, 0 );
      *(arraySubtitles [ t ]) = *(theOther.arraySubtitles [ t ]);
    }
  }
  deleteAudio ( );
  for ( t=0; t<MAX_AUDIO; t++ ) {
    if ( theOther.arrayAudio [ t ] ) {
        arrayAudio [ t ]  = new Audio;
      *(arrayAudio [ t ]) = *(theOther.arrayAudio [ t ]);
    }
  }

  if ( pSlideshow )
    delete pSlideshow;
  pSlideshow = NULL;
  if ( theOther.pSlideshow )  {
     pSlideshow = new CXmlSlideshow;
    *pSlideshow = *theOther.pSlideshow;
  }

  return *this;
}

SourceFileEntry::SourceFileEntry ( )
{
  iTitleset     = -1;
  iTitle        = -1;
  iPause        =  0;
  iJumpStart    = -1;
  bIsSlideshow  = false;
  bSoundSource  = false;
  // the first audio track is used for the Video's associated audio 
  // Note: currently only the main audio track of a video is utilized.
  arrayAudioEntries[0].iAudioNumber = 0;
  arrayAudioEntries[0].qsIso639 = Global::qsDefaultLanguage;
  if ( Global::qsDefaultLanguage.length ( ) != 2 )
    arrayAudioEntries[0].qsIso639 = "en";
  // Any other audio entries are ordered by the SourceFileInfos settings.

  sizeThumbnail = QSize (150, 150);
}

SourceFileEntry::~SourceFileEntry()
{
	uint t;
	for (t=0;t<(uint)listFileInfos.count();t++)	//oxx
		delete listFileInfos[t];
}

bool SourceFileEntry::readProjectFile  (QDomNode &xmlNode)
{ 
  int t;
  QString qsAttribute, infoName, infoText;

  QDomNode    infoNode, transcodeNode;
  QDomElement infoElement;
  QDomElement theElement = xmlNode.toElement();

  QDomAttr a = theElement.attributeNode ( SOURCE_OBJECT_DISPLAY_NAME );
  qsDisplayName = a.value();
  a = theElement.attributeNode ( SOURCE_OBJECT_PRE );
  qsPre = a.value();
  a = theElement.attributeNode ( SOURCE_OBJECT_POST );
  qsPost = a.value();
  qsAttribute = theElement.attributeNode ( SOURCE_OBJECT_IS_SOUND_SOURCE ).value();
  bSoundSource = false;
  if ( (!qsAttribute.isEmpty ()) && (qsAttribute == QString ("true")) )
    bSoundSource = true;
  qsAttribute = theElement.attributeNode ( SOURCE_OBJECT_PAUSE ).value();
  if (!qsAttribute.isEmpty ())
    iPause = qsAttribute.toInt();

  for ( t=0; t<MAX_AUDIO; t++ ) {
    arrayAudioEntries[t].iAudioNumber = -1;
    arrayAudioEntries[t].qsIso639     = QString ( );
  }
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    arraySubtitleEntries[t].m_iSubtitleNumber = -1;
    arraySubtitleEntries[t].m_qsIso639        = QString ( );
  }

  QDomNode  sourceNode = theElement.firstChild ( );
  while ( ! sourceNode.isNull() )	{
    theElement   = sourceNode.toElement  ( );
    // Okay, here we read the stored data from the xml file.
    QString tagName = theElement.tagName ( );
    // Okay, this is ugly but after all it is the simplest of all xml file structure.
    // No need to get fancy ...
    if ( SOURCE_OBJECT_AUDIO == tagName )  {
      QString qsIso639;
      int iAudioNumber = -1;
      qsAttribute = theElement.attributeNode ( AUDIO_LANGUAGE ).value ( );
      if ( ! qsAttribute.isEmpty ( ) )
        qsIso639 = qsAttribute;  
      qsAttribute = theElement.attributeNode ( AUDIO_NUMBER ).value ( );
      if ( ! qsAttribute.isEmpty ( ) )
        iAudioNumber = qsAttribute.toInt ( );

      if ( ( iAudioNumber > -1 ) && ( iAudioNumber < MAX_AUDIO ) ) {
        arrayAudioEntries[ iAudioNumber ].iAudioNumber = iAudioNumber;
        arrayAudioEntries[ iAudioNumber ].qsIso639     = qsIso639;
      }
    }
    if ( SOURCE_OBJECT_SUBTITLES == tagName )  {
      QString qsIso639;
      int iSubtitleNumber = -1;
      qsAttribute = theElement.attributeNode ( SUBTITLES_LANG ).value ( );
      if ( ! qsAttribute.isEmpty ( ) )
        qsIso639 = qsAttribute;  
      qsAttribute = theElement.attributeNode ( SUBTITLES_NUMBER ).value ( );
      if ( ! qsAttribute.isEmpty ( ) )
        iSubtitleNumber = qsAttribute.toInt ( );

      if ( ( iSubtitleNumber > -1 ) && ( iSubtitleNumber < MAX_SUBTITLES ) ) {
        arraySubtitleEntries[ iSubtitleNumber ].m_iSubtitleNumber = iSubtitleNumber;
        arraySubtitleEntries[ iSubtitleNumber ].m_qsIso639        = qsIso639;
      }
    }
    else if ( SOURCE_OBJECT_INFO == tagName )  {
      SourceFileInfo *pFileInfos = new SourceFileInfo;
      pFileInfos->readProjectFile ( sourceNode );
      listFileInfos.append        ( pFileInfos );
    }
    else if (SOURCE_OBJECT_CHAPTERS == tagName)	{
      infoText = theElement.text ();
      //listChapters = QStringList::split ( QString (","), infoText);	//ooo
      listChapters = infoText.split ( "," );				//xxx
    }
    // Otherwise go to the next node ...
    sourceNode = sourceNode.nextSibling();
  }

  // Finally we check the slideshow flag.
  if ( listFileInfos.count ( ) == 1 )  {
    SourceFileInfo *pInfo = listFileInfos[0];
    if ( pInfo->pSlideshow )  {
      Utils theUtils;
      QString qsFileName = pInfo->pSlideshow->slideshow_name;
      qsFileName.replace ( "/", "_" );
      qsFileName = theUtils.getTempFile ( qsFileName + ".vob" );
      QFileInfo fileInfo ( qsFileName );
      if ( ! fileInfo.exists ( ) ) { // generated file does not exist.
        bIsSlideshow = true;
        //Render::Manager::registerEntry ( this );	//ooo
      }
    }
  }
  // DEBUG output 
  /*
  printf ( "%s::%s : %d > ", __FILE__, __FUNCTION__, __LINE__ );
  for ( int e=0;e<MAX_AUDIO;e++ )
    printf ( "[%d=%s] ", arrayAudioEntries[e].iAudioNumber, arrayAudioEntries[e].qsIso639.ascii() );
  printf ( "\n" );
  */
  return true;
}

bool SourceFileEntry::writeProjectFile (QDomElement &theElement, bool bAutosave )
{ 
  uint t;
  SourceFileInfo *pInfo;
  QDomDocument   xmlDoc     = theElement.ownerDocument ( );
  QDomElement    sourceNode = xmlDoc.createElement ( SOURCE_OBJECT );  // <Source>
  QDomElement    infoElement, chapterElement;
  QDomText       text;

  // Here we set the attributes of the <dvdauthor> tag
  if ( ! qsDisplayName.isNull() )
    sourceNode.setAttribute ( SOURCE_OBJECT_DISPLAY_NAME, qsDisplayName );
  if ( ! qsPre.isNull  ( ) )
    sourceNode.setAttribute ( SOURCE_OBJECT_PRE, qsPre );
  if ( ! qsPost.isNull ( ) )
    sourceNode.setAttribute ( SOURCE_OBJECT_POST, qsPost );
  if ( bSoundSource )
    sourceNode.setAttribute ( SOURCE_OBJECT_IS_SOUND_SOURCE, "true" );
  sourceNode.setAttribute   ( SOURCE_OBJECT_PAUSE, QString ("%1").arg ( iPause ) ); 

  for ( t=0; t<MAX_AUDIO; t++ )  {
    if ( arrayAudioEntries [ t ].iAudioNumber > -1 ) {
      infoElement = xmlDoc.createElement( SOURCE_OBJECT_AUDIO );
      infoElement.setAttribute( AUDIO_LANGUAGE, arrayAudioEntries [ t ].qsIso639 );
      infoElement.setAttribute( AUDIO_NUMBER,   QString ( "%1" ).arg ( arrayAudioEntries [ t ].iAudioNumber ) );
      sourceNode.appendChild  ( infoElement );
    }
  }
  for ( t=0; t<MAX_SUBTITLES; t++ )  {
    if ( arraySubtitleEntries [ t ].m_iSubtitleNumber > -1 ) {
      infoElement = xmlDoc.createElement( SOURCE_OBJECT_SUBTITLES );
      infoElement.setAttribute( SUBTITLES_LANG,   arraySubtitleEntries [ t ].m_qsIso639 );
      infoElement.setAttribute( SUBTITLES_NUMBER, QString ( "%1" ).arg ( arraySubtitleEntries [ t ].m_iSubtitleNumber ) );
      sourceNode.appendChild  ( infoElement );
    }
  }

  if (listFileInfos.count() > 0)	{
    QList<SourceFileInfo *>::iterator it = listFileInfos.begin ( );	//oxx
    while ( it != listFileInfos.end ( ) )  {
      pInfo = *it++;
      pInfo->writeProjectFile ( sourceNode, bAutosave );
    }
  }

  if ( listChapters.count ( ) > 0 )  {
    chapterElement = xmlDoc.createElement ( SOURCE_OBJECT_CHAPTERS );
    sourceNode.appendChild( chapterElement );
    text =  xmlDoc.createTextNode ( listChapters.join ( QString ( "," ) ) );
    chapterElement.appendChild   ( text );
  }

  theElement.appendChild( sourceNode );
  return true;
}

SourceFileEntry *SourceFileEntry::flatClone ()
{
  // Clones this SourceFileEntries without the deep copy of the 
  // SourceFileInfos.
  SourceFileEntry *pClone = new SourceFileEntry;

  pClone->listFileInfos = listFileInfos;
  pClone->qsDisplayName = qsDisplayName;
  pClone->listChapters  = listChapters;
  pClone->bSoundSource  = bSoundSource;
  pClone->bIsSlideshow  = bIsSlideshow;
  pClone->iJumpStart    = iJumpStart;
  pClone->iTitleset     = iTitleset;
  pClone->iTitle        = iTitle;
  pClone->qsPre         = qsPre;
  pClone->qsPost        = qsPost;
  pClone->iPause        = iPause;
  for ( int t=0; t<MAX_AUDIO; t++ ) 
    pClone->arrayAudioEntries[t] = arrayAudioEntries [t];
  for ( int t=0; t<MAX_SUBTITLES; t++ ) 
    pClone->arraySubtitleEntries[t] = arraySubtitleEntries [t];

  return pClone;
}

int SourceFileEntry::audioCount ( bool bMaxCount )
{
  // This function returns the max audio count for this entry.
  int t, i;
  int iMaxEntryAudio = 0, iMaxInfoAudio = 0;
  SourceFileInfo *pInfo;
  for ( iMaxEntryAudio=MAX_AUDIO-1;iMaxEntryAudio>=0; iMaxEntryAudio-- ) {
    if ( arrayAudioEntries[ iMaxEntryAudio ].iAudioNumber != -1 )
      break;
  }
  if ( ! bMaxCount )
    return iMaxEntryAudio;

  // Lets say the Entry has five audio tracks but none of the FileInfos has more then three
  // In that case we should only return 3 and not 5.
  for ( t=0; t<(int)listFileInfos.count ( ); t++ ) {
    pInfo = listFileInfos [ t ];
    if ( pInfo ) {
      for ( i=MAX_AUDIO-1; i>=0; i-- ) {
        if ( pInfo->arrayAudio [ i ] ) {
          if ( iMaxInfoAudio < i ) {
            iMaxInfoAudio = i;
            i=-1;
          }
        }
      }
    }
  }
  if ( iMaxInfoAudio < iMaxEntryAudio )
    iMaxEntryAudio = iMaxInfoAudio;

  return iMaxEntryAudio;
}

int SourceFileEntry::subtitleCount ( bool bMaxCount )
{
  // This function returns the max audio count for this entry.
  int t, i;
  int iMaxEntrySubtitle = 0, iMaxInfoSubtitle = 0;
  SourceFileInfo *pInfo;
  for ( iMaxEntrySubtitle=MAX_SUBTITLES-1;iMaxEntrySubtitle>=0; iMaxEntrySubtitle-- ) {
    if ( arraySubtitleEntries[ iMaxEntrySubtitle ].m_iSubtitleNumber != -1 )
      break;
  }
  if ( ! bMaxCount )
    return iMaxEntrySubtitle;
  // Lets say the Entry has five audio tracks but none of the FileInfos has more then three
  // In that case we should only return 3 and not 5.
  for ( t=0; t<(int)listFileInfos.count ( ); t++ ) {
    pInfo = listFileInfos [ t ];
    if ( pInfo ) {
      for ( i=MAX_SUBTITLES-1; i>=0; i-- ) {
        if ( pInfo->arraySubtitles [ i ] ) {
          if ( iMaxInfoSubtitle < i ) {
            iMaxInfoSubtitle = i;
            i=-1;
          }
        }
      }
    }
  }
  if ( iMaxInfoSubtitle  < iMaxEntrySubtitle )
       iMaxEntrySubtitle = iMaxInfoSubtitle;

  return iMaxEntrySubtitle;
}

void SourceFileEntry::changeAudioTrack ( uint iAudioNumber, QString qsCode )
{
  SourceFileInfo *pInfo;
  bool bFound = false;

  if ( iAudioNumber >= MAX_AUDIO )
    return;

  QList<SourceFileInfo *>::iterator it = listFileInfos.begin ( );	//oxx
  while ( it != listFileInfos.end ( ) )  {
    pInfo = *it++;
    if ( pInfo->arrayAudio[iAudioNumber] )  {
         pInfo->arrayAudio[iAudioNumber]->qsIso639 = qsCode;
         bFound = true;
    }
  }

  if ( bFound )  {
    arrayAudioEntries[iAudioNumber].qsIso639     = qsCode;
    arrayAudioEntries[iAudioNumber].iAudioNumber = iAudioNumber;
  }
  else  {
    arrayAudioEntries[iAudioNumber].qsIso639     = "";
    arrayAudioEntries[iAudioNumber].iAudioNumber = -1;
  }
}

void SourceFileEntry::changeSubtitleTrack ( uint iSubtitleNumber, QString qsCode )
{
  SourceFileInfo *pInfo;
  bool bFound = false;

  if ( iSubtitleNumber >= MAX_SUBTITLES )
    return;

  QList<SourceFileInfo *>::iterator it = listFileInfos.begin ( );	//oxx
  while ( it != listFileInfos.end ( ) )  {
    pInfo = *it++;
    if ( pInfo->arraySubtitles[iSubtitleNumber] )  {
      pInfo->arrayAudio[iSubtitleNumber]->qsIso639 = qsCode;
      bFound = true;
    }
  }

  if ( bFound )  {
    arraySubtitleEntries[iSubtitleNumber].m_qsIso639        = qsCode;
    arraySubtitleEntries[iSubtitleNumber].m_iSubtitleNumber = iSubtitleNumber;
  }
  else  {
    arraySubtitleEntries[iSubtitleNumber].m_qsIso639        = "";
    arraySubtitleEntries[iSubtitleNumber].m_iSubtitleNumber = -1;
  }
}

void SourceFileEntry::synchTracks ( bool bSynchAudioTracks, bool bSynchSubtitleTracks )
{
  int t;
  Audio      *pAudio;
  AudioEntry *pAudioEntry;
  Subtitles *pSubtitle;
  SubtitleEntry *pSubtitleEntry;
  SourceFileInfo *pInfo;

  QList<SourceFileInfo *>::iterator it = listFileInfos.begin ( );	//oxx
  while ( it != listFileInfos.end ( ) )  {
    pInfo = *it++;
    if ( bSynchAudioTracks )  {
      for ( t=0; t<MAX_AUDIO; t++ )  {
        pAudio = pInfo->arrayAudio[t];
        if ( pAudio )  {
          pAudioEntry = &arrayAudioEntries[pAudio->iAudioNumber];
          pAudioEntry->iAudioNumber = pAudio->iAudioNumber;
          pAudioEntry->qsIso639     = pAudio->qsIso639;
        }
      }
    }

    if ( bSynchSubtitleTracks )  {
      for ( t=0; t<MAX_SUBTITLES; t++ )  {
        pSubtitle = pInfo->arraySubtitles[t];
        if ( pSubtitle )  {
          pSubtitleEntry = &arraySubtitleEntries[pSubtitle->m_iSubtitleNumber];
          pSubtitleEntry->m_iSubtitleNumber = pSubtitle->m_iSubtitleNumber;
          pSubtitleEntry->m_qsIso639        = pSubtitle->m_qsIso639;
        }
      }
    }
  }
}

SourceFileEntry & SourceFileEntry::operator = (SourceFileEntry &theOther)
{
	uint t;
	SourceFileInfo *pInfo;
	// First we take care of the entries in the list. Please note that we should keep the current ones around
	// in case we are cloning this entry.
	QList<SourceFileInfo *> listTemp2;		//oxx
	for (t=0;t<(uint)theOther.listFileInfos.count();t++)	{	//oxx
		pInfo = new SourceFileInfo ();
		*pInfo = *theOther.listFileInfos[t];
		listTemp2.append (pInfo);
	}
	// Now we should be save here to remove the entries ...	
	for (t=0;t<(uint)listFileInfos.count ();t++)	//oxx
		delete listFileInfos[t];
	listFileInfos.clear();

	listFileInfos = listTemp2;

	qsDisplayName = theOther.qsDisplayName;
	listChapters  = theOther.listChapters;
	bSoundSource  = theOther.bSoundSource;
	bIsSlideshow  = theOther.bIsSlideshow;
	iJumpStart    = theOther.iJumpStart;
	iTitleset     = theOther.iTitleset;
	iTitle        = theOther.iTitle;
	qsPre         = theOther.qsPre;
	qsPost        = theOther.qsPost;
	iPause        = theOther.iPause;
	for ( t=0; t<MAX_AUDIO; t++ ) 
	  arrayAudioEntries[t] = theOther.arrayAudioEntries [t];
	for ( t=0; t<MAX_SUBTITLES; t++ ) 
	  arraySubtitleEntries[t] = theOther.arraySubtitleEntries [t];

	return *this;
}
