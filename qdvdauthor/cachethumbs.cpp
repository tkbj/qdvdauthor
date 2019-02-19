/***************************************************************************
    cachethumbs.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   This file holds the implementation of the caching logic.
   Note: Each video file will get N numbers of thumbs to allow a short anim
         of the contents. 
         Only those image files which do not hold Exif thumbnails OR which
         have been tagged / modified by the user will get 1 thumb.
         
   Note2: The 'timeout' effect to keep cache size down needs to be
           implemented.
    
****************************************************************************/

#include <dlfcn.h>
#include <stdio.h>

#include <qfileinfo.h>
#include <qmatrix.h>
#include <qstring.h>
#include <qimage.h>
#include <qfile.h>
#include <qdir.h>
#include <qdom.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#include "global.h"
#include "utils.h"
#include "messagebox.h"
#include "cachethumbs.h"
#include "qplayer/mediacreator.h"


namespace Cache
{

Thumbs::MultiThumbRequest::MultiThumbRequest ( QObject *pObj, Entry *p )
  : ExecuteJob ( pObj )
{
  pEntry          = p;
  pEntry->iScanStatus = 1; // Scanning
  iDelta          = -1;
  iMovieLength    = -1;
  iCurrentThumb   = -1;

  if ( pEntry->arrayOfThumbs )
       pEntry->freeImages  ( );

  pEntry->arrayOfThumbs      = new QImage *[pEntry->iNumberOfThumbs];
  for ( uint t=0; t<pEntry->iNumberOfThumbs; t++ ) {
    //pEntry->arrayOfThumbs[t] = new QImage ( QImage ( ).fromMimeSource( "error.jpg" ) );	//ooo
    pEntry->arrayOfThumbs[t] = new QImage ( ":/images/error.jpg" );				//xxx
  }
  // init base class variables
  theType    = TYPE_MULTI_THUMB;
  qsFileName = pEntry->qsFileName;

  pImage     = new QImage;
}

Thumbs::MultiThumbRequest::~MultiThumbRequest ( )
{
  if ( pImage )
    delete pImage;
  pImage = NULL;
}

bool Thumbs::MultiThumbRequest::response ( )
{
  // qsFileName == length ...
  if ( iCurrentThumb < 0 ) {
    pEntry->qsLength = qsFileName;
    pEntry->pCache->saveCacheDB ( ); // store the new found length in the xml file
    // The first time around we have to init the offset
    if ( iMSecondsOffset == -1 )
         pEntry->iScanStatus = 2;
    Utils theUtils;
    iMovieLength    = theUtils.getMsFromString ( qsFileName );
    iDelta          = (long)( (double)( iMovieLength + 500 ) / pEntry->iNumberOfThumbs );
    iCurrentThumb   = 0;
    iMSecondsOffset = 0;
    return true;
  }
  // here we store the image and increase the iMScOffset for the next thumbnail.
  if ( iCurrentThumb-1 < (int)pEntry->iNumberOfThumbs )
    //*pEntry->arrayOfThumbs[iCurrentThumb] = pImage->smoothScale ( 300, 300, Qt::KeepAspectRatio );    //ooo
    *pEntry->arrayOfThumbs[iCurrentThumb] = pImage->scaled ( 300, 300, Qt::KeepAspectRatio );           //xxx

  iMSecondsOffset = ++iCurrentThumb * iDelta;

  if ( iMSecondsOffset >= iMovieLength ) {
    pEntry->iScanStatus = 2;
    pEntry->saveImages ( );
  }
  return true;
}

Thumbs::Entry::Entry ( Thumbs *pParent )
{
  iHashValue      = 0;
  iNumberOfThumbs = 0;
  iStarRating     = 0;
  iScanStatus     = 0;
  iFileSize       = 0;
  fRotate         = 0.0f;
  bStoreData      = true;
  bSourceExists   = true;
  bIsVideo        = true;
  arrayOfThumbs   = NULL;
  pCache          = pParent;
  dateCreated     = QDateTime::currentDateTime ( );
  dateLastUsed    = QDateTime::currentDateTime ( );
}

Thumbs::Entry::~Entry ( )
{
}

bool Thumbs::Entry::readXml ( QDomElement *pSource )
{
  QString qsDateCreated, qsDateLastUsed, qsVirtualFolder;

  QDomAttr a = pSource->attributeNode ( CACHE_FILE_SIZE );
  iFileSize        = a.value ( ).toUInt ( );
  a = pSource->attributeNode ( CACHE_RATING );
  iStarRating      = a.value ( ).toUInt( );
  a = pSource->attributeNode ( CACHE_ROTATE );
  fRotate          = a.value ( ).toFloat( );
  a = pSource->attributeNode ( CACHE_NR_OF_THUMBS );
  iNumberOfThumbs  = a.value ( ).toUInt( );
  a = pSource->attributeNode ( CACHE_COMMENT );
  qsComment        = a.value ( );
  a = pSource->attributeNode ( CACHE_VIRTUAL );
  qsVirtualFolder  = a.value ( );
  a = pSource->attributeNode ( CACHE_CREATED );
  qsDateCreated    = a.value ( );
  a = pSource->attributeNode ( CACHE_LENGTH );
  qsLength         = a.value ( );
  a = pSource->attributeNode ( CACHE_FILE_NAME);
  qsFileName       = a.value ( );
  a = pSource->attributeNode ( CACHE_LAST_USED );
  qsDateLastUsed   = a.value ( );

  dateCreated  = QDateTime::fromString ( qsDateCreated  );
  dateLastUsed = QDateTime::fromString ( qsDateLastUsed );
  //listVirtualFolders = QStringList::split ( ";", qsVirtualFolder );   //ooo
  listVirtualFolders = qsVirtualFolder.split ( ";" );                   //xxx

  // At this point we have all bu the thumbnails
  //iHashValue = pCache->hashFromFile ( qsFileName );
  bool bOkay = true;
  bStoreData = true;
  iHashValue = pSource->text ( ).toULongLong ( &bOkay, 16 );
  if ( ! bOkay )
    iHashValue = pCache->hashFromFile ( qsFileName );
  //printf ( "Thumbs::Entry::readXml <0x%016llX> <%s> <%s>\n", iHashValue, pSource->text ( ).ascii ( ), qsFileName.ascii ( ) );
  if ( iHashValue == 0LL ) {
    // File does not exist anymore. We want to keep the record around
    // in case this was on a memory stick or some other removable media
    bSourceExists   = false;
    return false;
  }
  return true;
}

bool Thumbs::Entry::writeXml ( QDomElement *pCacheElement )
{
  if ( ! bStoreData )
    return true;

  QString qsHash, qsVirtualFolder;
  qsHash.sprintf ( "%016llX", iHashValue );
  QDomDocument   theDoc = pCacheElement->ownerDocument ( );
  QDomElement entryNode = theDoc.createElement  ( CACHE_TAG_SOURCE );  // <Source>
  QDomText    entryText = theDoc.createTextNode ( qsHash );

  qsVirtualFolder = listVirtualFolders.join ( ";" );

  // Here we set the attributes of the <Source> tag
  entryNode.setAttribute  ( CACHE_FILE_SIZE,    QString ( "%1" ).arg ( iFileSize       ) );
  entryNode.setAttribute  ( CACHE_RATING,       QString ( "%1" ).arg ( iStarRating     ) );
  if ( fRotate != 0.0f )
    entryNode.setAttribute( CACHE_ROTATE,     QString ( "%1" ).arg ( fRotate      ) );
  entryNode.setAttribute  ( CACHE_NR_OF_THUMBS, QString ( "%1" ).arg ( iNumberOfThumbs ) );
  entryNode.setAttribute  ( CACHE_VIRTUAL,   qsVirtualFolder           );
  entryNode.setAttribute  ( CACHE_COMMENT,   qsComment                 );
  entryNode.setAttribute  ( CACHE_CREATED,   dateCreated.toString  ( ) );
  entryNode.setAttribute  ( CACHE_LENGTH,    qsLength                  );
  //entryNode.setAttribute  ( CACHE_FILE_NAME, qsFileName.utf8       ( ) );	//ooo
  //entryNode.setAttribute  ( CACHE_FILE_NAME, QString(qsFileName.utf8 ( )) );	//xxx
  entryNode.setAttribute  ( CACHE_FILE_NAME, QString(qsFileName.toUtf8 ( )) );	//xxx
  entryNode.setAttribute  ( CACHE_LAST_USED, dateLastUsed.toString ( ) );
  entryNode.appendChild   ( entryText );

  pCacheElement->appendChild (  entryNode );
  return true;
}

bool Thumbs::Entry::loadImages ( )
{
  if ( arrayOfThumbs )
       freeImages  ( );

  uint t;
  QImage *pImage, theImage;
  QString qsFilePath, qsHashBaseName, qsFullName, qsCount;
  QList<QImage *>list;			//oxx
  QFileInfo fileInfo ( qsFileName );

  qsFilePath     = pCache->getCachePath ( fileInfo.baseName ( ).left ( 1 ) );
  qsHashBaseName = qsFilePath + qsHashBaseName.sprintf ( "%016llX", iHashValue );

  // Read in all files $HOME/.qdvdauthor/cache/a/7653ABC87683ABCD1234_xx.jpg
  for ( t=0;t<iNumberOfThumbs; t++ ) {
    qsCount.sprintf ( "_%02d.jpg", t );
    qsFullName = qsHashBaseName + qsCount;
    if ( theImage.load  ( qsFullName ) ) {
      pImage = new QImage ( theImage );
      list.append ( pImage );
    }
  }
  if ( list.count ( ) > 0 ) {
    iNumberOfThumbs = list.count ( );
    arrayOfThumbs   = new QImage *[list.count ( )];
    //for ( t=0; t<list.count ( ); t++ ) {                      //ooo
    for ( t=0; t<static_cast<uint> ( list.count ( ) ); t++ ) {  //xxx
      arrayOfThumbs[t] = list[t];
    }
  }
  else {
    iNumberOfThumbs  = 1;
    arrayOfThumbs    = new QImage *[1];
    //arrayOfThumbs[0] = new QImage ( QImage ( ).fromMimeSource( "error.jpg" ) );	//ooo
    arrayOfThumbs[0] = new QImage ( ":/images/error.jpg" );				//xxx
    return false;
  }
  return true;
}

void Thumbs::Entry::saveImages ( )
{
  if ( qsFileName.length ( ) < 1 )
    return;

  uint    t;
  QImage *pImage;
  QString qsFilePath, qsFile, qsCount;
  QFileInfo fileInfo ( qsFileName );
  qsFile = pCache->getCachePath ( fileInfo.baseName ( ).left ( 1 ) );

  // Next we need to ensure that the path exists ...
  QDir   theDir ( qsFile );
  if ( ! theDir.exists ( ) ) {
    Utils theUtils;
    theUtils.recMkdir ( qsFile );
  }

  qsFilePath = qsFilePath.sprintf ( "%s%016llX", (const char *)QFile::encodeName ( qsFile ), iHashValue );
  for ( t=0; t<iNumberOfThumbs; t++ )  {
    qsCount.sprintf ( "_%02d.jpg", t );
    qsFile = qsFilePath + qsCount;
    pImage = arrayOfThumbs[t];
    pImage->save ( qsFile, "JPEG", 85 );
    //printf ( "  Cache::Thumbs::saveImages <%s>\n", (const char *)QFile::encodeName ( qsFile ) );
  }
}

void Thumbs::Entry::freeImages ( )
{
  if ( ! arrayOfThumbs )
    return;

  uint t;
  for ( t=0; t<iNumberOfThumbs; t++ )
    delete arrayOfThumbs[t];
  delete []arrayOfThumbs;
  arrayOfThumbs = NULL;
}

void Thumbs::Entry::scanImages ( QObject *pTarget )
{
  MultiThumbRequest *pRequest = new MultiThumbRequest ( pTarget, this );
  MediaCreator::registerWithMediaScanner ( pRequest );
}

QMatrix *Thumbs::Entry::getMatrix ( )
{
  return NULL;
}

Thumbs::ImageEntry::ImageEntry ( Thumbs *pParent )
  : Entry ( pParent )
{
  bSaveImage   = true;
  iOrientation = 0;
  bIsVideo     = false;
  pMatrix      = NULL;
}

Thumbs::ImageEntry::~ImageEntry ( )
{
  if ( pMatrix )
    delete pMatrix;
  pMatrix = NULL;
}

bool Thumbs::ImageEntry::readXml ( QDomElement *pSource )
{
  QString qsHashValue, qsDateCreated, qsDateLastUsed, qsVirtualFolder;
  qsComment       = "";
  iStarRating     = 0;
  iNumberOfThumbs = 1;
  listVirtualFolders.clear ( );

  QDomAttr a = pSource->attributeNode ( CACHE_FILE_SIZE );
  iFileSize        = a.value ( ).toUInt ( );
  a = pSource->attributeNode ( CACHE_RATING );
  iStarRating      = a.value ( ).toUInt ( );
  a = pSource->attributeNode ( CACHE_ORIENTATION );
  iOrientation     = a.value ( ).toInt  ( );
  a = pSource->attributeNode ( CACHE_ROTATE );
  fRotate          = a.value ( ).toFloat( );
  a = pSource->attributeNode ( CACHE_COMMENT );
  qsComment        = a.value ( );
  a = pSource->attributeNode ( CACHE_VIRTUAL );
  qsVirtualFolder  = a.value ( );
  a = pSource->attributeNode ( CACHE_CREATED );
  qsDateCreated    = a.value ( );
  a = pSource->attributeNode ( CACHE_DIMENSION );
  qsDimension      = a.value ( );
  a = pSource->attributeNode ( CACHE_FILE_NAME);
  qsFileName       = a.value ( );
  a = pSource->attributeNode ( CACHE_LAST_USED );
  qsDateLastUsed   = a.value ( );
  a = pSource->attributeNode ( CACHE_HASH_VALUE );
  qsHashValue      = a.value ( );

  bStoreData   = true;
  qsImageInfo  = pSource->text ( );
  dateCreated  = QDateTime::fromString ( qsDateCreated,  Qt::ISODate );
  dateLastUsed = QDateTime::fromString ( qsDateLastUsed, Qt::ISODate );
  //listVirtualFolders = QStringList::split ( ";", qsVirtualFolder );   //ooo
  listVirtualFolders = qsVirtualFolder.split ( ";" );                   //xxx

  //QStringList listSize = QStringList::split ( "x", qsDimension );     //ooo
  QStringList listSize = qsDimension.split ( "x" );                     //xxx
  bSaveImage = true;
  if ( listSize.count ( ) > 1 )  {
    int iWidth  = listSize[0].toInt ( );
    int iHeight = listSize[1].toInt ( );
    bSaveImage  = ( ( iWidth > 640 ) || ( iHeight > 480 ) || ( iWidth == 0 ) || ( iHeight == 0 ) );
  }
  // At this point we have all bu the thumbnails
  bool bOkay = true;
  iHashValue = qsHashValue.toULongLong ( &bOkay, 16 );
  if ( ! bOkay )
    iHashValue = pCache->hashFromFile ( qsFileName );
  //printf ( "Thumbs::Entry::readXml <0x%016llX> <%s> <%s>\n", iHashValue, pSource->text ( ).ascii ( ), qsFileName.ascii ( ) );

  initMatrix ( ); // maybe needed ...
  if ( iHashValue == 0LL ) {
    // File does not exist anymore. We want to keep the record around
    // in case this was on a memory stick or some other removable media
    bSourceExists   = false;
    return false;
  }
  return true;
}

bool Thumbs::ImageEntry::writeXml ( QDomElement *pCacheElement )
{
  if ( ! bStoreData )
    return true;

  QString qsHash, qsVirtualFolder;
  qsHash.sprintf ( "%016llX", iHashValue );

  QDomDocument   theDoc = pCacheElement->ownerDocument ( );
  QDomElement entryNode = theDoc.createElement  ( CACHE_TAG_IMAGE );  // <Image>
  QDomText    entryText = theDoc.createTextNode ( "" ); //qsImageInfo );

  qsVirtualFolder = listVirtualFolders.join ( ";" );

  // Here we set the attributes of the <Image> tag
  if ( iFileSize > 0 )
    entryNode.setAttribute ( CACHE_FILE_SIZE,  QString ( "%1" ).arg ( iFileSize    ) );
  if ( iStarRating > 0 )
    entryNode.setAttribute ( CACHE_RATING,     QString ( "%1" ).arg ( iStarRating  ) );
  if ( iOrientation > 0 )
    entryNode.setAttribute ( CACHE_ORIENTATION,QString ( "%1" ).arg ( iOrientation ) );
  if ( fRotate != 0.0f )
    entryNode.setAttribute ( CACHE_ROTATE,     QString ( "%1" ).arg ( fRotate      ) );
  entryNode.setAttribute   ( CACHE_HASH_VALUE, qsHash                    );
  if ( ! qsVirtualFolder.isEmpty ( ) )
    entryNode.setAttribute ( CACHE_VIRTUAL,    qsVirtualFolder           );
  if ( ! qsComment.isEmpty       ( ) )
    entryNode.setAttribute ( CACHE_COMMENT,    qsComment                 );
  entryNode.setAttribute   ( CACHE_DIMENSION,  qsDimension               );
  entryNode.setAttribute   ( CACHE_CREATED,    dateCreated.toString  ( "yyyy-MM-dd hh:mm:ss" ) );
  //entryNode.setAttribute   ( CACHE_FILE_NAME,  qsFileName.utf8       ( ) );		//ooo
  //entryNode.setAttribute   ( CACHE_FILE_NAME,  QString(qsFileName.utf8 ( )) );		//xxx
  entryNode.setAttribute   ( CACHE_FILE_NAME,  QString(qsFileName.toUtf8 ( )) );		//xxx
  entryNode.setAttribute   ( CACHE_LAST_USED,  dateLastUsed.toString ( "yyyy-MM-dd hh:mm:ss" ) );
  entryNode.appendChild    ( entryText );

  pCacheElement->appendChild (  entryNode );
  return true;
}

void Thumbs::ImageEntry::saveImages ( ) // store images to cache
{
  if ( qsFileName.length ( ) < 1 )
    return;

  if ( ! bSaveImage )
    return;

  QString qsHash, qsFilePath, qsFile, qsCount;
  qsHash.sprintf ( "%016llX", iHashValue );
  qsFilePath = pCache->getCachePath ( "img" + qsHash[0] );

  // Next we need to ensure that the path exists ...
  QDir   theDir ( qsFilePath );
  if ( ! theDir.exists  ( )  ) {
    Utils theUtils;
    theUtils.recMkdir ( qsFilePath );
  }

  qsFile = qsFilePath + qsHash + ".jpg";
  if ( arrayOfThumbs && arrayOfThumbs[0] )
       arrayOfThumbs[0]->save ( qsFile, "JPEG", 85 );
}

bool Thumbs::ImageEntry::loadImages ( ) // load images from cache or from the file.
{
  if ( arrayOfThumbs )
       freeImages  ( );

  QImage theImage;
  QFileInfo fileInfo ( qsFileName );
  //QString qsExt = fileInfo.extension ( FALSE ).lower ( );     //ooo
  QString qsExt = fileInfo.suffix ( ).toLower ( );              //xxx
  QString qsFilePath, qsHashBaseName, qsFullName;
  arrayOfThumbs = new QImage *[1];

  qsHashBaseName = qsHashBaseName.sprintf ( "%016llX", iHashValue );
  qsFilePath     = pCache->getCachePath   ( "img" + qsHashBaseName.left ( 1 ) );
  qsFullName     = qsFilePath + qsHashBaseName + ".jpg";

  jhead::JHead *pPlugin = pCache->plugin ( );
  if ( theImage.load  ( qsFullName ) )  {
    arrayOfThumbs[0] = new QImage ( theImage );
    iScanStatus      = 2;
    iNumberOfThumbs  = 1;
  }
  else if ( ( qsExt != "jpg" ) && ( qsExt != "jpeg" ) && theImage.load ( qsFileName ) )  {
    // This is for images which are NOT jpegs AND are < 680x480
    arrayOfThumbs[0] = new QImage ( theImage );
    iScanStatus      = 2;
    iNumberOfThumbs  = 1;
  }
  else if ( pPlugin )  {
    jhead::JHead::Info *pInfo = pPlugin->getFileInfo ( qsFileName );
    if ( pInfo )  {
      if ( pInfo->pThumbnail )  {
        // take ownership of the thumbnail
        arrayOfThumbs[0]  = pInfo->pThumbnail;
        pInfo->pThumbnail = NULL;
        bSaveImage        = false;  // Set to false if the image size is < 641 x 481
//      bStoreData        = false;  // no need to keep the record in the cache
        iScanStatus       = 2;      // 0=undef, 1=scanning, 2=scanning done.
      }
      else {
        //arrayOfThumbs[0] = new QImage ( QImage ( ).fromMimeSource( "error.jpg" ) );		//ooo
        arrayOfThumbs[0] = new QImage ( ":/images/error.jpg" );					//xxx
      }
      qsDimension     = QString ( "%1x%2" ).arg ( pInfo->iWidth ).arg ( pInfo->iHeight ); // The width x height
      iOrientation    = pInfo->iOrientation;
      iNumberOfThumbs = 1;
      initMatrix ( );
      delete pInfo;
    }
  }
  if ( iScanStatus != 2 )  {
    if ( theImage.load ( qsFileName ) )  {
      qsDimension      = QString (   "%1x%2"  ).arg ( theImage.width ( ) ).arg ( theImage.height ( ) );
      bSaveImage       = ( ( theImage.width ( ) > 640 ) || ( theImage.height ( ) > 480 ) );
      bStoreData       = true;
      iNumberOfThumbs  = 1;
      //arrayOfThumbs[0] = new QImage ( theImage.smoothScale ( 300, 300, Qt::KeepAspectRatio ) );   //ooo
      arrayOfThumbs[0] = new QImage ( theImage.scaled ( 300, 300, Qt::KeepAspectRatio ) );          //xxx
      saveImages   ( );
    }
    else  {
      //arrayOfThumbs[0] = new QImage ( QImage ( ).fromMimeSource( "error.jpg" ) );	//ooo
      arrayOfThumbs[0] = new QImage ( ":/images/error.jpg" );				//xxx
      bSaveImage       = false;
      bStoreData       = false;
      return false;
    }
  }
  return true;
}

void Thumbs::ImageEntry::scanImages ( QObject * ) // load images from cache
{
  if ( arrayOfThumbs )
       freeImages  ( );

  QFileInfo fileInfo ( qsFileName );
  iFileSize     = fileInfo.size ( );
  //QString qsExt = fileInfo.extension ( FALSE ).lower ( );     //ooo
  QString qsExt = fileInfo.suffix ( ).toLower ( );              //xxx
  arrayOfThumbs = new QImage *[1];
  jhead::JHead *pPlugin = NULL;

  if ( ( qsExt == "jpg" ) || ( qsExt == "jpeg" ) )
    pPlugin = pCache->plugin ( );

  if ( pPlugin )  {
    jhead::JHead::Info *pInfo = pPlugin->getFileInfo ( qsFileName );
    if ( pInfo && pInfo->pThumbnail )  {
// printf ( "%s::%s : %d > imgDate<%s> vs created<%s>\n", __FILE__, __FUNCTION__, __LINE__, pInfo->imageDate.toString  ( "yyyy-MM-dd hh:mm:ss" ).ascii ( ), dateCreated.toString  ( "yyyy-MM-dd hh:mm:ss" ).ascii ( ) );
      if ( pInfo->imageDate.isValid ( ) )
        dateCreated = pInfo->imageDate;

      // Take ownership of the thumbnail
      arrayOfThumbs[0]  = pInfo->pThumbnail;
      pInfo->pThumbnail = NULL;
      iOrientation = pInfo->iOrientation;
      qsDimension  = QString ( "%1x%2" ).arg ( pInfo->iWidth ).arg ( pInfo->iHeight ); // The width x height
      bSaveImage   = false;  // Set to false if the image size is < 641 x 481
      iScanStatus  = 2;   // 0=undef, 1=scanning, 2=scanning done.
      initMatrix ( );
      return;
    }
  }

  QImage theImage;
  if ( theImage.load ( qsFileName ) )  {
    qsDimension = QString ( "%1x%2" ).arg ( theImage.width ( ) ).arg ( theImage.height ( ) );
    bSaveImage  = ( ( theImage.width ( ) > 640 ) || ( theImage.height ( ) > 480 ) );
    //arrayOfThumbs[0] = new QImage ( theImage.smoothScale ( 300, 300, Qt::KeepAspectRatio ) );     //ooo
    arrayOfThumbs[0] = new QImage ( theImage.scaled ( 300, 300, Qt::KeepAspectRatio ) );            //xxx
    saveImages   ( );
  }
  else {
    //arrayOfThumbs[0] = new QImage ( QImage ( ).fromMimeSource( "error.jpg" ) );	//ooo
    arrayOfThumbs[0] = new QImage ( ":/images/error.jpg" );				//xxx
  }
}

QMatrix *Thumbs::ImageEntry::getMatrix ( )
{
  return pMatrix;
}

void Thumbs::ImageEntry::initMatrix ( )
{
  double fRot  =   0.0;
  bool   bFlip = false;

  while ( fRotate >=  360.0 )
          fRotate -=  360.0;
  while ( fRotate <= -360.0 )
          fRotate +=  360.0;

  if ( ( ( iOrientation < 2 ) || ( iOrientation > 8 ) ) &&
         ( fRotate  == 0.0f ) )  {
    if ( pMatrix )
      delete pMatrix;
    pMatrix = NULL;
    return;
  }
  if ( iOrientation == 8 )
    fRot = 270.0;
  else if ( iOrientation == 3 )
    fRot = 180.0;
  else if ( iOrientation == 6 )
    fRot = 90.0;
  else  {
    bFlip = true;
    if ( iOrientation == 7 )
      fRot = 270.0;
    else if ( iOrientation == 4 )
      fRot = 180.0;
    else if ( iOrientation == 5 )
      fRot = 90.0;
  }

  // And finally we set the matrix ...
  QMatrix flip ( -1, 0, 0, 1, 1, 0 );
  if ( ! pMatrix )
    pMatrix = new QMatrix;
  else
    pMatrix->reset ( );

  pMatrix->rotate ( fRot + fRotate );
  if ( bFlip )
    *pMatrix *= flip;
}

Thumbs::VirtualFolder::VirtualFolder ( Thumbs *pThumbs )
{
  pCache     = pThumbs;
  iHashValue = 0;
}

Thumbs::VirtualFolder::~VirtualFolder ( )
{
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) )
    delete *it++;

  listOfFiles.clear ( );
}

bool Thumbs::VirtualFolder::readXml  ( QDomElement *pFolder )
{
  QString qsDateCreated;

  QDomAttr a = pFolder->attributeNode ( VIRTUAL_CREATED );
  qsDateCreated    = a.value ( );
  a = pFolder->attributeNode ( VIRTUAL_NAME );
  qsFolderName     = a.value ( );

  dateCreated = QDateTime::fromString ( qsDateCreated  );
  iHashValue  = (unsigned int)pCache->SDBMHash ( qsFolderName );

  VFile   *pFile;
  QDomNode xmlNode = pFolder->firstChild ( );
  while( ! xmlNode.isNull ( ) )  {
    QDomElement fileElem = xmlNode.toElement ( );
    if ( fileElem.tagName ( ) == VIRTUAL_TAG_ENTRY )  {
      pFile = new VFile;
      pFile->qsFileName     = fileElem.attributeNode  ( VIRTUAL_FILE_NAME ).value ( );
      pFile->iFileHashValue = fileElem.text ( ).toULongLong ( 0, 16 );
      listOfFiles.append    ( pFile );
    }
    xmlNode = xmlNode.nextSibling ( );
  };

  return true;
}

bool Thumbs::VirtualFolder::writeXml ( QDomElement *pFolderElement )
{
  QString qsHash;
  qsHash.sprintf ( "%08X", iHashValue );
  QDomDocument    theDoc = pFolderElement->ownerDocument ( );
  QDomElement  entryNode;
  QDomElement folderNode = theDoc.createElement  ( VIRTUAL_TAG_FOLDER );	// <Folder>
  QDomText     entryText;

  folderNode.setAttribute ( VIRTUAL_HASH,    qsHash       );
  folderNode.setAttribute ( VIRTUAL_NAME,    qsFolderName );
  folderNode.setAttribute ( VIRTUAL_CREATED, dateCreated.toString ( ) );

  folderNode.appendChild  ( entryText );

  VFile *pFile;
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) ) {
    pFile = *it++;
    qsHash.sprintf ( "%016llX", pFile->iFileHashValue );
    entryText = theDoc.createTextNode ( qsHash );
    entryNode = theDoc.createElement  ( VIRTUAL_TAG_ENTRY );	// <FolderEntry>
    entryNode.setAttribute ( VIRTUAL_FILE_NAME, pFile->qsFileName );
    entryNode.appendChild  ( entryText );
    folderNode.appendChild ( entryNode );
  }

  pFolderElement->appendChild ( folderNode );
  return true;
}

void Thumbs::VirtualFolder::append ( Entry *pEntry )
{
  if ( ! pEntry )
    return;
  VFile *pFile;
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) ) {
    pFile = *it++;
    if ( pFile->iFileHashValue == pEntry->iHashValue )
      return;
  }
  pFile = new VFile;
  pFile->iFileHashValue = pEntry->iHashValue;
  pFile->qsFileName     = pEntry->qsFileName;
  listOfFiles.append        ( pFile );
  pCache->saveVirtualFolder ( false );
}

void Thumbs::VirtualFolder::append ( ImageEntry *pEntry )
{
  if ( ! pEntry )
    return;
  VFile *pFile;
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) ) {
    pFile = *it++;
    if ( pFile->iFileHashValue == pEntry->iHashValue )
      return;
  }
  pFile = new VFile;
  pFile->iFileHashValue = pEntry->iHashValue;
  pFile->qsFileName     = pEntry->qsFileName;
  listOfFiles.append        ( pFile );
  pCache->saveVirtualFolder (  true );
}

void Thumbs::VirtualFolder::clear ( )
{
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) )
    delete *it++;
  listOfFiles.clear ( );
}

Thumbs::VirtualFolder *Thumbs::VirtualFolder::clone ( )
{
  VirtualFolder *pFolder = new VirtualFolder ( pCache );
  VFile         *pFile,  *pNewFile;
  QList<VFile *>::iterator it;		//oxx
  it = listOfFiles.begin ( );
  while ( it != listOfFiles.end ( ) ) {
    pFile = *it++;
    if ( pFile ) {
      pNewFile = new VFile;
      pNewFile->qsFileName     = pFile->qsFileName;
      pNewFile->iFileHashValue = pFile->iFileHashValue;
      pFolder ->listOfFiles.append ( pNewFile );
    }
  }

  int     iCount          = 1;
  bool    bImageFolder    = false;
  QString qsClone         = QObject::tr ( "Clone_" );
  pFolder->qsFolderName   = qsClone + qsFolderName;

  // Here we have to determine if theis VFolder is ImageVirtualFolder or VirtualFolder
  VirtualFolder *pMe = pCache->findVirtualFolder ( iHashValue );
  if ( ! pMe )
    bImageFolder = true;

  while ( pCache->findVirtualFolder ( pFolder->qsFolderName, bImageFolder ) )
    pFolder->qsFolderName = qsClone + QString ( "%1_" ).arg ( iCount ++ ) + qsFolderName;

  pFolder->dateCreated    = QDateTime::currentDateTime ( );
  pFolder->iHashValue     = (unsigned int)pCache->SDBMHash ( pFolder->qsFolderName );

  pCache->getVirtualFolders ( bImageFolder ).append ( pFolder );
  pCache->saveVirtualFolder ( bImageFolder );
  return pFolder;
}

uint Thumbs::VirtualFolder::count ( )
{
  return listOfFiles.count ( );
}

Thumbs::Thumbs ( )
{
  // The first thing to do is to load the cache_db.xml file into memory, 
  // so we know what we have.
  loadPlugin  ( );
  loadCacheDB ( );
  loadImageDB ( );
  loadVirtualFolder ( false );  // Virtual Folder
  loadVirtualFolder (  true );  // Image Virtual Folder
}

Thumbs::~Thumbs ( )
{
  unloadPlugin       ( );
  clearCache         ( );
  clearImages        ( );
  clearVirtualFolder ( );
}

bool Thumbs::loadPlugin ( )
{
//  QString qsFilePath = "../lib/libjhead.so";
  m_pLibrary         = NULL;
  m_pDestroyFunction = NULL;
  m_pPlugin          = NULL;

  QString  qsFilePath ( Global::qsSystemPath + "/share/qdvdauthor/lib/libjhead.so" );
  //m_pLibrary = dlopen ( qsFilePath.ascii ( ), RTLD_LAZY );            //ooo
  m_pLibrary = dlopen ( qsFilePath.toLatin1().data ( ), RTLD_LAZY );    //xxx
  if ( ! m_pLibrary ) {
    //printf ( "Fatal: Cannot load library <%s>\n       %s\n", qsFilePath.ascii ( ), dlerror ( ) );         //ooo
    printf ( "Fatal: Cannot load library <%s>\n       %s\n", qsFilePath.toLatin1().data ( ), dlerror ( ) ); //xxx  
    return false;
  }

  // reset errors
  dlerror ( );

  // load the symbols
  createJHead_t *pCreateLibrary = (createJHead_t *) dlsym ( m_pLibrary, "createPlugin" );
  const char    *pErrorString   = dlerror ( );
  if ( pErrorString ) {
    printf ( "Cannot load symbol create: %s\n", pErrorString );
    return false;
  }

  m_pDestroyFunction = (destroyJHead_t *) dlsym ( m_pLibrary, "destroyPlugin" );
  pErrorString = dlerror ( );
  if ( pErrorString ) {
    printf ( "Cannot load symbol destroy: %s\n", pErrorString );
    return false;
  }

  // create an instance of the class
  m_pPlugin = pCreateLibrary ( );

  return true;
}

void Thumbs::unloadPlugin ( )
{
  // destroy the class
  if ( m_pPlugin && m_pDestroyFunction )  {
    m_pDestroyFunction ( m_pPlugin );
  }
  m_pDestroyFunction = NULL;
  m_pPlugin = NULL;

  // unload the triangle library
  if ( m_pLibrary )
    dlclose ( m_pLibrary );
  m_pLibrary = NULL;
}

void Thumbs::clearCache ( )
{
  QList<Entry *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) 
    delete *it++;
  m_listOfThumbs.clear ( );
}

void Thumbs::clearImages ( )
{ 
  QList<ImageEntry *>::iterator it;	//oxx
  //QList<ImageEntry *>::const_iterator it;	//xxx
  it = m_listOfImages.begin ( );
  while ( it != m_listOfImages.end ( ) ) 
    delete *it++;
  m_listOfImages.clear ( );   //ooooo
}

void Thumbs::clearVirtualFolder ( )
{
  QList<VirtualFolder *>::iterator it;		//oxx
  it = m_listOfVirtualFolders.begin ( );
  while ( it != m_listOfVirtualFolders.end ( ) ) 
    delete *it++;
  m_listOfVirtualFolders.clear ( );

  it = m_listOfImageFolders.begin ( );
  while ( it != m_listOfImageFolders.end ( ) ) 
    delete *it++;
  m_listOfImageFolders.clear ( );
}

void Thumbs::freeImages ( )
{ 
  // to free some space after closing dialog
  Entry *pEntry;
  QList<Entry *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pEntry = *it++;
    pEntry->freeImages ( );
  }
}

jhead::JHead *Thumbs::plugin ( )
{
  return m_pPlugin;
}

QString Thumbs::getCachePath ( QString c )
{
  //QString qsPath = QString ( "%1/.qdvdauthor/cache/%2/" ). arg ( QDir::homeDirPath ( ) ).arg ( c.lower ( ) );     //ooo
  QString qsPath = QString ( "%1/.qdvdauthor/cache/%2/" ). arg ( QDir::homePath ( ) ).arg ( c.toLower ( ) );        //xxx
  return qsPath;
}

bool Thumbs::loadCacheDB ( )
{
  // Assign the file
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx
  QString qsFileName = qsPath + QString ( CACHE_DB_FILE );

  QDir theDir ( qsPath );
  if ( ! theDir.exists ( ) ) {
    Utils theUtils;
    theUtils.recMkdir ( qsPath );
  }

  QFile projectFile ( qsFileName );
  // Check if we have not yet created a cach_db.xml file.
  if ( ! projectFile.exists ( ) )
    return true;

  if ( ! projectFile.open ( QIODevice::ReadOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  if ( ! xmlDoc.setContent ( &projectFile ) )  {
    // Error handling ...
    projectFile.close ( );
    MessageBox::warning ( NULL, QObject::tr ("Can not create cache file."),
			  QObject::tr ( "Please make sure you have write permissins set to\n%1" ).arg ( qsPath ),
			  QMessageBox::Ok, QMessageBox::NoButton );
    return false;
  }
  // And at last lets try to read the information of the file.
  QDomElement docElem = xmlDoc.documentElement ( );
  if ( docElem.tagName ( ) != CACHE )
    return false;

  Entry   *pEntry;
  QDomNode xmlNode = docElem.firstChild ( );
  while( ! xmlNode.isNull ( ) )  {
    QDomElement sourceElem = xmlNode.toElement ( );
    if ( sourceElem.tagName ( ) == CACHE_TAG_SOURCE ) {
      pEntry  = new Entry ( this );
      pEntry->readXml ( &sourceElem );
      // If there has been a problem then return false.
      //printf ( "Thumbs::loadCacheDB Entry<%s> <%016llX> \n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
      m_listOfThumbs.append ( pEntry );
    }
    // Otherwise go to the next node ...
    xmlNode = xmlNode.nextSibling ( );
  }

  projectFile.close ( );
  return true;
}

bool Thumbs::saveCacheDB ( )
{ 
  // Assign the file
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx
  QString qsFileName = qsPath  + QString ( CACHE_DB_FILE );
  QString qsTempName = qsFileName + ".tmp";

  QFile projectFile ( qsTempName );
  if ( ! projectFile.open ( QIODevice::WriteOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  QDomElement  root = xmlDoc.createElement ( CACHE );	// <CacheThumbs>
  // And now proceed to writing the rest of the file.
  xmlDoc.appendChild( root );

  Entry *pEntry = NULL;
  QList<Entry *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pEntry = *it++;
    pEntry->writeXml ( &root );
  }

  QString xml = xmlDoc.toString ( );
  // QDomDocument converts '<' to "&lt;", So if we want comments in
  // the generated file, we should replace them here.
  xml.replace ( "&lt;", "<"); 
  //debug_out ("%s\n", (const char *)xml);
  //projectFile.writeBlock ( xml, qstrlen ( xml ) );                                //ooo
  projectFile.write ( xml.toLatin1().data(), qstrlen ( xml.toLatin1().data() ) );    //xxx
  projectFile.close ( );

  // At this point we are sure to have written the whole file and can copy it over the original file.
  copyOver ( qsTempName, qsFileName );

  return true;
}

void Thumbs::cleanCacheDB ( )
{
  // Weeds out entries which exceed the max age of the record, to keep the cache from growing too big.
}

bool Thumbs::loadImageDB ( )
{  
  // Assign the file
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx

  QString qsFileName = qsPath + QString ( IMAGE_DB_FILE );

  QDir theDir ( qsPath );
  if ( ! theDir.exists ( ) ) {
    Utils theUtils;
    theUtils.recMkdir ( qsPath );
  }

  QFile projectFile ( qsFileName );
  // Check if we have not yet created a cach_db.xml file.
  if ( ! projectFile.exists ( ) )
    return true;

  if ( ! projectFile.open ( QIODevice::ReadOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  if ( ! xmlDoc.setContent ( &projectFile ) )  {
    // Error handling ...
    projectFile.close ( );
    MessageBox::warning ( NULL, QObject::tr ("Can not create cache file."),
			  QObject::tr ( "Please make sure you have write permissins set to\n%1" ).arg ( qsPath ),
			  QMessageBox::Ok, QMessageBox::NoButton );
    return false;
  }
  // And at last lets try to read the information of the file.
  QDomElement docElem = xmlDoc.documentElement ( );
  if ( docElem.tagName ( ) != CACHE )
    return false;

  ImageEntry *pEntry;
  QDomNode xmlNode = docElem.firstChild ( );
  while( ! xmlNode.isNull ( ) )  {
    QDomElement sourceElem = xmlNode.toElement ( );
    if ( sourceElem.tagName ( ) == CACHE_TAG_IMAGE ) {      
      pEntry  = new ImageEntry ( this );
      pEntry->readXml ( &sourceElem );
      // If there has been a problem then return false.
      //printf ( "Thumbs::loadCacheDB Entry<%s> <%016llX> \n", pEntry->qsFileName.ascii(), pEntry->iHashValue );            //ooo
      //printf ( "Thumbs::loadCacheDB Entry<%s> <%016llX> \n", pEntry->qsFileName.toLatin1().data(), pEntry->iHashValue );    //xxx
      m_listOfImages.append ( pEntry );   //ooooo
    }
    // Otherwise go to the next node ...
    xmlNode = xmlNode.nextSibling ( );
  }

  ImageEntry *p_Entry = NULL;  //xxx
  QList<ImageEntry *>::const_iterator it;   //xxx
  it = m_listOfImages.begin ( );    //xxx
  
  while ( it != m_listOfImages.end ( ) ) {  //xxx
    p_Entry = *it++;
    //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );   //ooo
    //printf ( "  Looking at <%s> = <0x%016llX>\n", p_Entry->qsFileName.toLatin1().data(), p_Entry->iHashValue );   //xxx
  }

  projectFile.close ( );
  return true;
}

bool Thumbs::saveImageDB ( )
{ 
  // Assign the file
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx
  QString qsFileName = qsPath  + QString ( IMAGE_DB_FILE );

  QString qsTempName = qsFileName + ".tmp";

  QFile projectFile ( qsTempName );
  if ( ! projectFile.open ( QIODevice::WriteOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  QDomElement  root = xmlDoc.createElement ( CACHE );	// <CacheThumbs>
  // And now proceed to writing the rest of the file.
  xmlDoc.appendChild( root );

  ImageEntry *pEntry = NULL;
  //QList<ImageEntry *>::iterator it;	//oxx
  QList<ImageEntry *>::const_iterator it;	//xxx
 
  it = m_listOfImages.begin ( );
 
  while ( it != m_listOfImages.end ( ) ) {
    pEntry = *it++;
    pEntry->writeXml ( &root );
  }

  QString xml = xmlDoc.toString ( );
  // QDomDocument converts '<' to "&lt;", So if we want comments in
  // the generated file, we should replace them here.
  xml.replace ( "&lt;", "<"); 
  //debug_out ("%s\n", (const char *)xml);
  //projectFile.writeBlock ( xml, qstrlen ( xml ) );                               //ooo
  projectFile.write ( xml.toLatin1().data(), qstrlen ( xml.toLatin1().data() ) );  //xxx
  projectFile.close ( );

  // At this point we are sure to have written the whole file and can copy it over the original file.
  copyOver ( qsTempName, qsFileName );

  return true;
}

void Thumbs::cleanImageDB ( )
{
  // Weeds out entries which exceed the max age of the record, to keep the cache from growing too big.
}

bool Thumbs::loadVirtualFolder ( bool bImageFolder )
{  
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx
  QString qsFileName;
  if ( bImageFolder )
    qsFileName = qsPath  + QString ( VIMAGE_DB_FILE );
  else
    qsFileName = qsPath  + QString ( VIRTUAL_DB_FILE );

  QDir theDir ( qsPath );
  if ( ! theDir.exists ( ) ) {
    Utils theUtils;
    theUtils.recMkdir ( qsPath );
  }

  QFile projectFile ( qsFileName );
  // Check if we have not yet created a cach_db.xml file.
  if ( ! projectFile.exists ( ) )
    return true;

  if ( ! projectFile.open ( QIODevice::ReadOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  if ( ! xmlDoc.setContent ( &projectFile ) )  {
    // Error handling ...
    projectFile.close ( );
    MessageBox::warning ( NULL, QObject::tr ("Can not create cache file."),
			  QObject::tr ( "Please make sure you have write permissins set to\n%1" ).arg ( qsPath ),
			  QMessageBox::Ok, QMessageBox::NoButton );
    return false;
  }
  // And at last lets try to read the information of the file.
  QDomElement docElem = xmlDoc.documentElement ( );
  if ( docElem.tagName ( ) != VIRTUAL )
    return false;

  VirtualFolder   *pFolder;
  QDomNode xmlNode = docElem.firstChild ( );
  while( ! xmlNode.isNull ( ) )  {
    QDomElement sourceElem = xmlNode.toElement ( );
    if ( sourceElem.tagName ( ) == VIRTUAL_TAG_FOLDER ) {
      pFolder  = new VirtualFolder ( this );
      pFolder->readXml ( &sourceElem );
      // If there has been a problem then return false.
      //printf ( "Thumbs::loadVirtualFolder Entry<%s> <%08X> \n", pFolder->qsFolderName.ascii(), pFolder->iHashValue );
      if ( bImageFolder )
        m_listOfImageFolders.append   ( pFolder );
      else
        m_listOfVirtualFolders.append ( pFolder );
    }
    // Otherwise go to the next node ...
    xmlNode = xmlNode.nextSibling ( );
  }

  projectFile.close ( );
  return true;
}

bool Thumbs::saveVirtualFolder ( bool bImageFolder )
{
  // Assign the file
  //QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homeDirPath ( ) );    //ooo
  QString qsPath     = QString ( "%1/.qdvdauthor/cache/" ). arg ( QDir::homePath ( ) );         //xxx
  QString qsFileName, qsTempName;
  if ( bImageFolder )
    qsFileName = qsPath  + QString ( VIMAGE_DB_FILE );
  else
    qsFileName = qsPath  + QString ( VIRTUAL_DB_FILE );

  qsTempName = qsFileName + ".tmp";

  QFile projectFile ( qsTempName );
  if ( ! projectFile.open ( QIODevice::WriteOnly ) )
    return false;

  QDomDocument xmlDoc( CACHE_DOCTYPE );
  QDomElement  root = xmlDoc.createElement ( VIRTUAL );	// <VirtualFolder>
  // And now proceed to writing the rest of the file.
  xmlDoc.appendChild( root );

  VirtualFolder *pFolder = NULL;
  QList<VirtualFolder *>::iterator it, itEnd;		//oxx
  it    = m_listOfVirtualFolders.begin ( );
  itEnd = m_listOfVirtualFolders.end   ( );
  if ( bImageFolder )  {
    it    = m_listOfImageFolders.begin ( );
    itEnd = m_listOfImageFolders.end   ( );
  }

  while ( it != itEnd ) {
    pFolder = *it++;
    pFolder->writeXml ( &root );
  }

  QString xml = xmlDoc.toString ( );
  // QDomDocument converts '<' to "&lt;", So if we want comments in
  // the generated file, we should replace them here.
  xml.replace ( "&lt;", "<"); 
  //debug_out ("%s\n", (const char *)xml);
  //projectFile.writeBlock ( xml, qstrlen ( xml ) );                                //ooo
  projectFile.write ( xml.toLatin1().data(), qstrlen ( xml.toLatin1().data() ) );   //xxx
  projectFile.close ( );

  // At this point we are sure to have written the whole file and can copy it over the original file.
  copyOver ( qsTempName, qsFileName );

  return true;
}


bool Thumbs::copyOver ( QString &qsTempFile, QString &qsFileName )
{   
  // Copy the temp file over the original file
  QFile fileOrig ( qsFileName );
  QFile fileTemp ( qsTempFile );
  if ( fileTemp.exists      ( ) && fileTemp.size ( ) > 10 )  {
    if ( fileOrig.exists    ( ) )  {
      if ( !fileOrig.remove ( ) )
        return false;
    }
    //int iRet = rename ( qsTempFile.utf8 ( ), qsFileName.utf8 ( ) );   //ooo
    int iRet = rename ( qsTempFile.toUtf8 ( ).data ( ), qsFileName.toUtf8 ( ).data ( ) ); //xxx
    if ( iRet != 0 )
         iRet  = iRet; // Error
  }
  return true;
}

Thumbs::Entry *Thumbs::find ( QString &qsFileName, bool bImage )
{   
  unsigned long long iHash = hashFromFile ( qsFileName );

  return find ( iHash, bImage );
}

Thumbs::Entry *Thumbs::find ( unsigned long long iHash, bool bImage )
{   
  if ( bImage )  {
    ImageEntry *pEntry = NULL;
    QList<ImageEntry *>::iterator it;		//oxx
 
    it = m_listOfImages.begin ( );

    while ( it != m_listOfImages.end ( ) ) {
      pEntry = *it++;
      //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );   //ooo
      //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.toLatin1().data(), pEntry->iHashValue );   //xxx
      if ( pEntry->iHashValue == iHash )
        return pEntry;
    }
  }
  else  {
    Entry *pEntry = NULL;
    QList<Entry *>::iterator it;	//oxx
    it = m_listOfThumbs.begin ( );
    while ( it != m_listOfThumbs.end ( ) ) {
      pEntry = *it++;
      //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
      if ( pEntry->iHashValue == iHash )
        return pEntry;
    }
  }
  return NULL;
}

Thumbs::Entry *Thumbs::append ( QString &qsFile, QString qsLength, uint iThumbs, QImage **ppArray )
{ 
  // Here we create a entry in the cache for this file and copy the images over
  // plus we have to store them into the cache under $HOME/.qdvdauthor/cache/X/<HasName>_xx.png
  QFileInfo fileInfo ( qsFile );
  Entry *pEntry = new Entry ( this );

  pEntry->qsFileName      = qsFile;
  pEntry->qsLength        = qsLength;
  pEntry->iHashValue      = hashFromFile ( qsFile );
  pEntry->arrayOfThumbs   = ppArray;
  pEntry->iNumberOfThumbs = iThumbs;
  pEntry->dateCreated     = fileInfo.created ( );

  if ( pEntry->iHashValue == 0LL ) {
    delete pEntry;
    return NULL;
  }

  m_listOfThumbs.append ( pEntry );
  saveCacheDB ( );

  // And after we have stored the xml file we will save the images in the cache.
  if ( ppArray )
    pEntry->saveImages ( );
  else 
    pEntry->scanImages ( );

  //  printf ( "Cache::Thumbs::append <%s> = <%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
  return pEntry; 
}

Thumbs::Entry *Thumbs::append ( QString &qsFile, QImage *pImage )   //ooooo
{    
  // Here we create a entry in the cache for this file and copy the images over
  // plus we have to store them into the cache under $HOME/.qdvdauthor/cache/X/<HasName>_xx.png
  QFileInfo fileInfo ( qsFile );
  ImageEntry *pEntry      = new ImageEntry ( this );
  pEntry->qsFileName      = qsFile;
  pEntry->iHashValue      = hashFromFile ( qsFile );
  pEntry->iNumberOfThumbs = 1;
  pEntry->dateCreated     = fileInfo.created ( );

  if ( pEntry->iHashValue == 0LL ) {
    delete pEntry;
    return NULL;
  }

  m_listOfImages.append ( pEntry );   //ooooo

  // And after we have stored the xml file we will save the images in the cache.
  if ( pImage )  {
     pEntry->arrayOfThumbs    = new  QImage *[1];
    *pEntry->arrayOfThumbs[0] = pImage->copy ( );
     pEntry->saveImages ( );
  }
  else
    pEntry->scanImages ( );

  // Now done at the calling function, outside the loop for the images.
  // otherwise we woulds write the XML file for every image.
  //  saveImageDB ( );  // save only if no Exif preview was found because now we have to generate the thumb

  //  printf ( "Cache::Thumbs::append <%s> = <%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
  return pEntry; 
}

Thumbs::Entry *Thumbs::append ( QString &qsFile, QObject *pTarget, uint iThumbs )
{  
  // Here we create a entry in the cache for this file and copy the images over
  // plus we have to store them into the cache under $HOME/.qdvdauthor/cache/X/<HasName>_xx.png
  QFileInfo fileInfo ( qsFile );
  Entry *pEntry = new Entry ( this );

  pEntry->qsFileName      = qsFile;
  pEntry->iHashValue      = hashFromFile ( qsFile );
  pEntry->iNumberOfThumbs = iThumbs;
  pEntry->dateCreated     = fileInfo.created ( );

  if ( pEntry->iHashValue == 0LL ) {
    delete pEntry;
    return NULL;
  }

  m_listOfThumbs.append ( pEntry );
  saveCacheDB ( );

  pEntry->scanImages ( pTarget );

  //  printf ( "Cache::Thumbs::append <%s> = <%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
  return pEntry; 
}

Thumbs::VirtualFolder *Thumbs::findVirtualFolder ( unsigned int iHash, bool bImageFolder )
{
  VirtualFolder *pVFolder = NULL;
  QList<VirtualFolder *>::iterator it, itEnd;		//oxx
  it    = m_listOfVirtualFolders.begin ( );
  itEnd = m_listOfVirtualFolders.end   ( );
  if ( bImageFolder )  {
    it    = m_listOfImageFolders.begin ( );
    itEnd = m_listOfImageFolders.end   ( );
  }

  while ( it != itEnd ) {
    pVFolder = *it++;
    //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
    if ( pVFolder->iHashValue == iHash )
      return pVFolder;
  }
  return NULL;
}

Thumbs::VirtualFolder *Thumbs::findVirtualFolder ( QString &qsFolderName, bool bImageFolder )
{
  VirtualFolder *pVFolder = NULL;
  QList<VirtualFolder *>::iterator it, itEnd;		//oxx
  it    = m_listOfVirtualFolders.begin ( );
  itEnd = m_listOfVirtualFolders.end   ( );
  if ( bImageFolder )  {
    it    = m_listOfImageFolders.begin ( );
    itEnd = m_listOfImageFolders.end   ( );
  }

  while ( it != itEnd ) {
    pVFolder = *it++;
    //printf ( "  Looking at <%s> = <0x%016llX>\n", pEntry->qsFileName.ascii(), pEntry->iHashValue );
    if ( pVFolder->qsFolderName == qsFolderName )
      return pVFolder;
  }
  return NULL;
}

unsigned int Thumbs::addVirtualFolder ( QString &qsFolderName, bool bImageFolder )
{
  unsigned int iHash = (unsigned int)SDBMHash ( qsFolderName );

  VirtualFolder *pVFolder = findVirtualFolder ( iHash, bImageFolder );
  if ( pVFolder )
    return 0;  // Ooops folder exists already

  pVFolder = new VirtualFolder ( this );
  pVFolder->qsFolderName = qsFolderName;
  pVFolder->iHashValue   = iHash;
  pVFolder->dateCreated  = QDateTime::currentDateTime ( );

  if ( bImageFolder )
    m_listOfImageFolders.append ( pVFolder );
  else
    m_listOfVirtualFolders.append ( pVFolder );
  saveVirtualFolder ( bImageFolder );
  return pVFolder->iHashValue;
}

void Thumbs::deleteVirtualFolder ( QString &qsFolderName, bool bImageFolder )
{
  unsigned int iHash = (unsigned int)SDBMHash ( qsFolderName );
  if ( iHash > 0 )
    deleteVirtualFolder ( iHash, bImageFolder );
}

void Thumbs::deleteVirtualFolder ( unsigned int iHash, bool bImageFolder )
{
  VirtualFolder *pVFolder = findVirtualFolder ( iHash, bImageFolder );
  //  printf ( "Thumbs::deleteVirtualFolder <%08X><%p>\n", iHash, pVFolder );
  if ( pVFolder ) {
    if ( bImageFolder )
      //m_listOfImageFolders.remove ( pVFolder );       //ooo
      m_listOfImageFolders.removeOne ( pVFolder );      //xxx
    else
      //m_listOfVirtualFolders.remove ( pVFolder );     //ooo
      m_listOfVirtualFolders.removeOne ( pVFolder );    //xxx
    delete pVFolder;
  }
  saveVirtualFolder ( bImageFolder );
}

QList<Thumbs::VirtualFolder *> &Thumbs::getVirtualFolders ( bool bImageFolder )		//oxx
{
  if ( bImageFolder )
    return m_listOfImageFolders;

  return m_listOfVirtualFolders;
}

QList<Thumbs::Entry *> Thumbs::getScannedThumbs ( )	//oxx
{
  QList<Entry *> list;		//oxx

  Entry *pEntry;
  QList<Entry *>::iterator it;		//oxx
  it = m_listOfThumbs.begin ( );
  while ( it != m_listOfThumbs.end ( ) ) {
    pEntry = *it++;
    if ( pEntry->iScanStatus == 2 ) // 2 == Scanning done
      list.append ( pEntry );
  }

  return list;
}

QList<Thumbs::Entry *> Thumbs::getThumbsByStar ( uint iStar, bool bImage )	//oxx
{
  QList<Entry *> list;		//oxx

  if ( bImage )  {
    ImageEntry *pEntry;
    //QList<ImageEntry *>::iterator it;		//oxx
    QList<ImageEntry *>::const_iterator it;		//xxx
    it = m_listOfImages.begin ( );
    while ( it != m_listOfImages.end ( ) ) {
      pEntry = *it++;
      if ( pEntry->iStarRating ==  iStar )
        list.append ( pEntry );
    }
  }
  else  {
    Entry *pEntry;
    QList<Entry *>::iterator it;	//oxx
    it = m_listOfThumbs.begin ( );
    while ( it != m_listOfThumbs.end ( ) ) {
      pEntry = *it++;
      if ( pEntry->iStarRating ==  iStar )
        list.append ( pEntry );
    }
  }
  return list;
}

unsigned long long Thumbs::hashFromFile ( QString &qsFileName )
{  
  QString   qsFingerprint;
  QFileInfo fileInfo ( qsFileName );

  // Sanity check.
  if ( ! fileInfo.exists ( ) )
    return 0LL;

  uint     iSize = fileInfo.size    ( );
  QDateTime time = fileInfo.created ( );
  qsFingerprint  = qsFileName + QString ( "%1" ).arg ( iSize ) + time.toString ( "yyyyMMddhhmmss" );

  return SDBMHash ( qsFingerprint );
}

unsigned long long Thumbs::hashFromPath ( QString &qsPathName )
{
  QString   qsFingerprint;
  QFileInfo theDir ( qsPathName );

  // Sanity check.
  if ( ! theDir.exists ( ) )
    return 0LL;

  QDateTime time = theDir.created ( );
  qsFingerprint  = qsPathName + time.toString ( "yyyyMMddhhmmss" );

  return SDBMHash ( qsFingerprint );
}

unsigned long long Thumbs::SDBMHash ( QString &str )
{   
   unsigned long long hash = 0;
   //for ( unsigned int i = 0; i < str.length ( ); i++ )                        //ooo
   for ( unsigned int i = 0; i < static_cast<uint> ( str.length ( ) ); i++ ) {    //xxx
     //hash = str[i].latin1 ( ) + (hash << 6) + (hash << 16) - hash;            //ooo
     hash = str[i].toLatin1 ( ) + (hash << 6) + (hash << 16) - hash;            //xxx
   }

   return hash;
}

}; // End of namespace Thumbs
