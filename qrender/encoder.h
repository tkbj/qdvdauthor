/***************************************************************************
    encoder.h
                             -------------------
    Encode - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Pure virtual calss to define the encoder interface.
    The first incarnation will be based on ffmpeg.
    At a later point we can create another encoder class 
    based on e.g. Phonon or Xine in which case we should create
    a plug-in based system.

    Goal: take images and add it to stream to be written 
          to a file in VOB format

****************************************************************************/
#ifndef ENCODER_ENCODER_H
#define ENCODER_ENCODER_H

#include <QString>
#include <QStringList>

class QImage;

namespace Encoder
{

class Encoder
{
public:
  enum enVideo {
    vfUndef=-1,
    vfPAL,
    vfNTSC,
    vfCopy
  };
  enum enAudio {
    afUndef=-1,
    afMP2,
    afAC3,
    afCopy
  };
  enum enAspect {
    arUndef=-1,
    arAspect4to3,
    arAspect16to9,
    arCopy
  };
           Encoder ( );
  virtual ~Encoder ( );

  void setVideoParam ( uint iBitrate, enAspect a=arAspect4to3 );
  void setAudioParam ( uint iBitrate, int s=48000 ); // 192000, 48000
  virtual bool initStream    ( QString, enVideo, enAudio, uint i=1 );
  virtual void setAudioList  ( QStringList   );
  virtual bool addImage      ( QImage *, int, float fVolume=1.0f ) = 0;
  virtual void endStream     ( );
  QString &getFileName       ( );

  virtual bool addVid        ( QString, int, int, int a=0, int c=0, double r=0.0 ) = 0;
  virtual bool initVid       ( QString ) = 0;
  virtual void closeVid      (         ) = 0;
  virtual QImage &getVidFrame(  double ) = 0;

protected:
  QString     m_qsFileName;
  QStringList m_audioList;
  enVideo     m_VideoFormat;
  enAudio     m_AudioFormat;
  enAspect    m_AspectRatio;
  uint        m_iVideoBitrate;
  uint        m_iAudioBitrate;
  int         m_iAudioSample;
  uint        m_iTotalFrames;
};

}; // end of namespace Encoder

#endif // ENCODER_ENCODER_H
