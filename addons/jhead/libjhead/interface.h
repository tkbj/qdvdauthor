/***************************************************************
 **
 ** class jhead::JHead
 **
 ** This class is an inteface to the functions provided in jhead
 ** Please note that I will only expose those APIs which I need
 ** in QDVDAuthor to obtain Exif information from images.
 ** 
 ** The class is defined as an interface because I want to 
 ** dynamically load the library only when I need its 
 ** functionality. 
 ** 
 ***************************************************************/

#ifndef __JHEAD_H__
#define __JHEAD_H__

#include <qimage.h>
#include <qstring.h>
#include <qdatetime.h>

namespace jhead
{

class JHead
{
public:
  class GPSInfo  {
  public:
    GPSInfo ( ) { };

    QString qsLat;
    QString qsLon;
    QString qsAlt;
  };
  class Info  {
  public:
     Info ( ) { pGPSInfo=NULL; pThumbnail=NULL; iWidth=-1; iHeight=-1; iFileSize=-1; iOrientation=0; 
                iFlashFlags  = iISOEquiv    = iLightsource = iExposureProg = iDistanceRange = 0;
                fFocalLength = fDigitalZoom = fCCDWidth    = fExposureTime = fExposureBias  = fAperture = 0.0f;
                bIsColor = bWhitebalance = true; };
    ~Info ( ) { if ( pGPSInfo ) delete pGPSInfo; if ( pThumbnail ) delete pThumbnail; };

    int       iWidth;
    int       iHeight;
    int       iFileSize;
    int       iOrientation;
    QImage   *pThumbnail;
    QString   qsFileName;
    QString   qsComment;
    QDateTime fileDate;
    QDateTime imageDate;
    GPSInfo  *pGPSInfo;

    // More Exif info
    QString   qsCamera;
    QString   qsModel;
    bool      bIsColor;
    int       iFlashFlags;
    float     fFocalLength;
    float     fDigitalZoom;
    float     fCCDWidth;
    float     fExposureTime;
    float     fAperture;
    int       iISOEquiv;
    float     fExposureBias;
    bool      bWhitebalance;
    int       iLightsource;
    int       iExposureProg;
    int       iDistanceRange;
  };

  JHead ( ) { };
  virtual ~JHead ( ) { };

  // After some tweaking only one API call remained.
  virtual Info *getFileInfo ( QString & ) = 0;
};

}; // end of namespace jhead

// the types of the class factories
typedef jhead::JHead *createJHead_t ( );
typedef void destroyJHead_t ( jhead::JHead * );

#endif
