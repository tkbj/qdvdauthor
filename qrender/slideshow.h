/***************************************************************************
    slideshow.h
                             -------------------
    Utils - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

****************************************************************************/

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QSize>
#include <QImage>
#include <QString>

#include "engine/ffmpeg_enc.h"
#include "../qdvdauthor/xml_slideshow.h"

namespace Render
{

class Render;
class Parser;

class Slideshow
{
public:
   Slideshow  ( Render * );
   Slideshow  ( Parser * );
  ~Slideshow  ( );

  void initMe ( CXmlSlideshow *, QString & );
  bool exec   ( );

  void setFileName ( QString & );

protected:
  CXmlSlideshow::img_struct *createStartEnd ( bool );
  void prescaleImage      ( QImage & );
  void createBackground   ( );
  void createIITransition ( CXmlSlideshow::img_struct *, CXmlSlideshow::img_struct *, Encoder::FFmpeg *, double );
  void createVITransition ( CXmlSlideshow::vid_struct *, CXmlSlideshow::img_struct *, Encoder::FFmpeg *, double );
  void createIVTransition ( CXmlSlideshow::img_struct *, CXmlSlideshow::vid_struct *, Encoder::FFmpeg *, double );
  void createVVTransition ( CXmlSlideshow::vid_struct *, CXmlSlideshow::vid_struct *, Encoder::FFmpeg *, double );
  void createFromImage    ( CXmlSlideshow::img_struct *, Encoder::FFmpeg *, double );
  void createKBFromImage  ( CXmlSlideshow::img_struct *, Encoder::FFmpeg *, double );
  void createFromVid      ( CXmlSlideshow::vid_struct *, Encoder::FFmpeg *, double );

  void displayProgress    ( float   );
  QString getHashName     ( QString );

  void log ( QString, int l=1 );  // 1==Logger::logDebug
  float getDuration       ( CXmlSlideshow::img_struct * );
  float getFilterDuration ( CXmlSlideshow::img_struct * );

private:
  Render        *m_pParent;
  Parser        *m_pParser;
  CXmlSlideshow *m_pSlideshow;
  QString        m_qsTempPath;  // the tem path
  QString        m_qsFileName;  // the resulting .vob file
  QSize          m_size;
  QImage         m_background;
};

}; // and of namespace Render

#endif // SLIDESHOW_H
