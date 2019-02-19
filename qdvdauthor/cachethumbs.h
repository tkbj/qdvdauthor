/***************************************************************************
    cachethumbs.h
                             -------------------
    Class Cache::Thumbs
    
    This class handles the caching of thumbnails created by DialogFiles
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
   The cache is stored under :
   $HOME/.qdvdauthor/cache
   The following files should/must exist for the cache to work.
   o  thumbs_db.xml This file is the database for the cache.
                    The xml structure of this flie is described below
   o  virtual_folder.xml This file holds the user created
                    VirtualFolder information
   o  /<first Letter Of Video File>/<Hash Name Of File>                 
    
****************************************************************************/

/* XML File structure :
 <CacheThumbs>
   <Source FileSize="" Rating="" VirtualFolders="" DateCreated="" 
           VideoLength="" NumberOfThumbs="" FileName="" 
	   DateLastUsed="">ThumbFileName</Source> Note: ThumbFileName is generated through a hashing function
   <Source ... >
 </CacheThumbs>

 <VirtualFolder>
   <Folder name="" created="" >
     // Note: ThumbFileName is generated through a hashing function
     //       the hashed name has fileSize/date encoded in it
     <FolderEntry FileName="" >ThumbFileName</FolderEntry>
   </Folder>
 </VirtualFolder>
*/

#ifndef __CACHE_THUMBS_H__
#define __CACHE_THUMBS_H__

#include <qdatetime.h>
#include <QStringList>		//xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#define CACHE_DB_FILE      "cache_db.xml"
#define IMAGE_DB_FILE      "image_db.xml"
#define VIRTUAL_DB_FILE    "virtual_db.xml"
#define VIMAGE_DB_FILE     "virtual_image_db.xml"
#define CACHE_DOCTYPE      ""

// Deines for the cache xml file
#define CACHE              "CacheThumbs"
#define CACHE_TAG_SOURCE   "Source"
#define CACHE_TAG_IMAGE    "Image"
#define CACHE_HASH_VALUE   "Hash"
#define CACHE_NR_OF_THUMBS "NrOfThumbs"
#define CACHE_FILE_SIZE    "FileSize"
#define CACHE_RATING       "Rating"
#define CACHE_VIRTUAL      "VirtualFolders"
#define CACHE_COMMENT      "Comment"
#define CACHE_ORIENTATION  "Orientation"
#define CACHE_ROTATE       "Rotate"
#define CACHE_DIMENSION    "Dimension"
#define CACHE_CREATED      "DateCreated"
#define CACHE_LENGTH       "VideoLength"
#define CACHE_FILE_NAME    "FileName"
#define CACHE_LAST_USED    "DateLastUsed"
#define CACHE_DIMENSION    "Dimension"
#define CACHE_IMAGE_INFO   "ImageInfo"

// Defines for Virtual Folder
#define VIRTUAL            "VirtualFolder"
#define VIRTUAL_TAG_FOLDER "Folder"
#define VIRTUAL_TAG_ENTRY  "FolderEntry"
#define VIRTUAL_NAME       "Name"
#define VIRTUAL_HASH       "Hash"
#define VIRTUAL_CREATED    "Created"
#define VIRTUAL_FILE_NAME  "FileName"

class QDomElement;
#include "qplayer/mediascanner.h"
#include "../addons/jhead/libjhead/interface.h"

namespace Cache
{

class Thumbs
{
public:
  class Entry {
  public:
             Entry ( Thumbs * );
    virtual ~Entry ( );
    virtual bool readXml    ( QDomElement * );
    virtual bool writeXml   ( QDomElement * );

    virtual void saveImages ( ); // store images to cache
    virtual bool loadImages ( ); // load images from cache
    virtual void freeImages ( ); // frees the memory 
    virtual void scanImages ( QObject *p=NULL ); // scans the video file
    virtual QMatrix *getMatrix ( );

    unsigned long long iHashValue;
    bool     bIsVideo;
    bool     bSourceExists; // If the file is not reachable ( removable storage or deleted )
    uint     iScanStatus;   // 0=undef, 1=scanning, 2=scanning done.
    QString  qsFileName;
    QString  qsLength;      // "00:00:00"
    uint     iFileSize;     // 1234 in bytes
    QImage **arrayOfThumbs;
    uint     iNumberOfThumbs;
    uint     iStarRating;
    float    fRotate;       // Manually set by user if != 0.0f
    QString  qsComment;     // User provided commant ( to be used as subtitle )
    QStringList listVirtualFolders;

    bool      bStoreData;   // Should this record be kept or removed from the cache.
    QDateTime dateCreated;  // obtained from meta data or file creation date
    QDateTime dateLastUsed; // to eventually purge from the cache
    Thumbs   *pCache;
  };
  class ImageEntry : public Entry  {
  public:
             ImageEntry     ( Thumbs * );
    virtual ~ImageEntry     ( );
    virtual bool readXml    ( QDomElement * );
    virtual bool writeXml   ( QDomElement * );

    virtual void saveImages ( ); // store images to cache
    virtual bool loadImages ( ); // load images from cache
    virtual void scanImages ( QObject *p=NULL ); // scans the video file
    virtual QMatrix *getMatrix ( );
    virtual void initMatrix     ( );

    QString   qsDimension; // The width x height
    QString   qsImageInfo; // All info returned by jhead
    int       iOrientation;// exif information
    bool      bSaveImage;  // Set to false if the image size is < 641 x 481 or if Exif preview exists
    QMatrix *pMatrix;     // Transformation-matrix which hold rotate, flip info etc.
  };

  class VirtualFolder {
  public:
    class VFile {
    public:
      VFile ( ) { iFileHashValue = 0LL; };
      QString qsFileName;
      unsigned long long iFileHashValue;
    };

     VirtualFolder ( Thumbs * );
    ~VirtualFolder ( );
    bool readXml   ( QDomElement * );
    bool writeXml  ( QDomElement * );
    void append    ( ImageEntry  * );
    void append    ( Entry * );
    void clear     ( );
    uint count     ( );
    VirtualFolder *clone ( );

    unsigned int iHashValue;
    QDateTime    dateCreated;
    QString      qsFolderName;
    Thumbs      *pCache;
    QList<VFile *>listOfFiles;			//oxx
  };
  class MultiThumbRequest : public ExecuteJob  {
  public:
     MultiThumbRequest ( QObject *, Entry * );
    ~MultiThumbRequest ( );
    virtual bool response ( );

    long    iDelta;
    long    iMovieLength;
    int     iCurrentThumb;
    Entry  *pEntry;
  };

public:
   Thumbs ( );
  ~Thumbs ( );

  void clearCache         ( );
  void clearImages        ( );
  void clearVirtualFolder ( );
  bool saveCacheDB        ( );
  bool saveImageDB        ( );
  bool saveVirtualFolder  ( bool b=false );

  Entry *find   ( QString &, bool b=false );
  Entry *find   ( unsigned long long, bool b=false );
  Entry *append ( QString &, QString q=QString::null, uint i=10,  QImage **pp=NULL );
  Entry *append ( QString &, QObject *, uint  i=10 );
  Entry *append ( QString &, QImage  * );

  unsigned int   addVirtualFolder    (  QString &,   bool b=false );
  void           deleteVirtualFolder (  QString &,   bool b=false );
  void           deleteVirtualFolder ( unsigned int, bool b=false );
  VirtualFolder *findVirtualFolder   ( unsigned int, bool b=false );
  VirtualFolder *findVirtualFolder   (  QString &,   bool b=false );
  QList<VirtualFolder *> &getVirtualFolders  (  bool b=false );		//oxx

  unsigned long long hashFromFile ( QString & );
  unsigned long long hashFromPath ( QString & );

  QList<Entry *> getThumbsByStar  ( uint, bool b=false );	//oxx
  QList<Entry *> getScannedThumbs ( );				//oxx

  jhead::JHead *plugin   ( );
private:
  bool loadPlugin        ( ); // Loading libjhead
  void unloadPlugin      ( ); // unloading libjhead

  bool loadCacheDB       ( );      // Load cache_db.xml file
  bool loadImageDB       ( );      // Load image_db.xml file
  bool loadVirtualFolder ( bool ); // Load virtual_db.xml or vimage_db.xml file
  void cleanCacheDB      ( ); // After a set amount of time the cache is freed of images based on dateLastUsed
  void cleanImageDB      ( ); // After a set amount of time the cache is freed of images based on dateLastUsed
  void freeImages        ( ); // to free some space after closing dialog
  QString getCachePath   ( QString );
  bool copyOver          ( QString &, QString & );

  unsigned long long SDBMHash ( QString & );

  QList<Entry *>         m_listOfThumbs;		//oxx
  QList<ImageEntry *>    m_listOfImages;		//oxx
  QList<VirtualFolder *> m_listOfVirtualFolders;	//oxx
  QList<VirtualFolder *> m_listOfImageFolders;		//oxx

  // For the exif information, we'll store the plugin information here.
  void           *m_pLibrary;
  destroyJHead_t *m_pDestroyFunction;
  jhead::JHead   *m_pPlugin;
};

}

#endif // __CACHE_THUMBS_H__
