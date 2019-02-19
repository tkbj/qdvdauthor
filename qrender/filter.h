/***************************************************************************
    filter.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Class to implement the base for additional filter.
    This class also includes the basic CrossFade, and
    KenBurns filter.

    Derived class AlphaFilter will handle gray scale
    filter.

****************************************************************************/

#ifndef RENDER_FILTER_H
#define RENDER_FILTER_H

// These are the currently supported filter types
#define FILTER_CROSS_FADE "crass_fade"
#define FILTER_KEN_BURNS  "ken_burns"
#define FILTER_ALPHA_FADE "alpha_fade"

#include <QImage>

#include "../qdvdauthor/xml_slideshow.h"

// Forward declaration of classes.
namespace Encoder { class FFmpeg; }

namespace Render
{

class Filter
{
public:
  // The audio transition while the filter is being displayed
  enum enAudioTrans { VolumeFull, VolumeNull, VolumeUp, VolumeDown };
             Filter ( );
  virtual   ~Filter ( );
  static Filter *create ( CXmlSlideshow *, CXmlSlideshow::filter_struct * );

  virtual QStringList getFilterNames ( );
  virtual bool exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage *p=NULL );
  virtual void setKenBurnsParams ( CXmlSlideshow::img_struct *, CXmlSlideshow::img_struct *, QImage &, int, int, int );

protected:
  void encode ( Encoder::FFmpeg *, QImage &, float, enAudioTrans );

  // The foloowing parameters are used for KenBurns only.
  bool m_bUseKenBurns;
  int  m_iAspectRatio; // 0==4/3  1==16/9
  int  m_iTotalImages[2];
  QImage m_background;
  CXmlSlideshow::img_struct m_kenBurns[2];
};

class CrossFadeFilter : public Filter
{
public:
           CrossFadeFilter ( );
  virtual ~CrossFadeFilter ( );

  virtual QStringList getFilterNames ( );
  virtual bool exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage *p=NULL );
};

class KenBurnsFilter : public Filter
{
public:
           KenBurnsFilter ( );
  virtual ~KenBurnsFilter ( );

  virtual QStringList getFilterNames ( );
  virtual bool exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage *p=NULL );

  bool exec ( QImage &, CXmlSlideshow::effect_struct *, QImage &, int, int );
};

class AlphaFilter : public Filter
{
public:
           AlphaFilter ( );
           AlphaFilter ( QString );
  virtual ~AlphaFilter ( );

  virtual QStringList getFilterNames ( );
  virtual bool exec ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage *p=NULL );

private:
  // Working area ... implementing KB animated transitions. I.e. image is moving which Alpha transitioning.
  bool execKB ( Encoder::FFmpeg *, QImage &, QImage &, int, enAudioTrans, QImage *p=NULL );

  // This one will simply transition rom one image to the next
  void augmentAlpha ( int, QImage &, QImage &, QImage &, QImage & );
  // This one will pass through the background
  void augmentAlpha ( int, int, QImage &, QImage &, QImage &, QImage &, QImage * );

private:
  QString m_qsFilterName;
};

}; // end of namespace Encoder

#endif // RENDER_FILTER_H
