/***************************************************************************
    ffmpeg_enc.h
                             -------------------
    Encode - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    FFRMpeg based encoder class.

    Goal: take images and add it to stream to be written 
          to a file in VOB format

    Enhanced version should be able to 
    1) addImages to the slideshow
    2) addAudio to the slideshow with fade-in / fade-out
    3) addVids to the slideshow with orig audio

    Note: The main audio stream should look like this :

           Start       IMG1  X  IMG2   X        short VID1        X   IMG3
    Video |----------*******XXX*******XXX------------------------XXX*******XXX--------------> time
            __________________________   ________________________   ______________ _  _   _
    Audio |/  background audio stream \_| short vid audio stream |_/ continued audio bakground stream
    
****************************************************************************/

#ifndef ENCODER_FFMPEG_ENC_H
#define ENCODER_FFMPEG_ENC_H

#include <QList>
#include <QMutex>

#ifndef UINT64_C
#define UINT64_C uint64_t
#endif

#include <libavutil/avutil.h>

#if ( LIBAVUTIL_VERSION_MAJOR >= 50 )
#include <libavutil/pixfmt.h>
#endif

#include "../encoder.h"

class QString;

struct AVFormatContext;
struct ReSampleContext;
struct AVCodecContext;
struct SwsContext;
struct AVPacket;
struct AVStream;
struct AVFrame;


namespace Encoder
{

class FFmpeg : public Encoder
{
  class Buffer  {
  public:
     Buffer ( int );
    ~Buffer (     );

    int   iSampleRate;
    int   iChannels;
    int   iSamplesPerChannel; // The # of samples per channel. I.e. the total # is iChannels * iSamples
    short *pBuffer;
  };

  // Base class for Audio, and Vid
  class Media  {
  public:
             Media ( );
    virtual ~Media ( );

    virtual bool openStream  ( QString q=QString::null );
    virtual void closeStream ( );
    virtual void resetStream ( );

    QString streamName ( );
    int     streamID   ( bool );

    void setOutputContext ( AVCodecContext *, AVCodecContext * );

  protected:
    QString          m_qsStreamName;
    int              m_iAudioStreamID;
    int              m_iVideoStreamID;

    AVCodecContext  *m_pAudioCodecCtx;  // Codec info of the first Audio stream
    AVCodecContext  *m_pVideoCodecCtx;  // Codec info of the first Video stream
    AVFormatContext *m_pFormatCtx;      // File with multiple streams

    // Require some basic info about the output stream for the transcoding part.
    AVCodecContext  *m_pOutputAudio;
    AVCodecContext  *m_pOutputVideo;
  };

  class Audio : public Media  {
  public:
             Audio ( QString, unsigned int, AVStream * );
             Audio ( unsigned int );
    virtual ~Audio ( );
    virtual bool openStream  ( QString q=QString::null );
    virtual void closeStream ( );

    Buffer  *resample    ( AVPacket * );
    Buffer  *decode      ( AVPacket * );
    Buffer  *decode      ( bool & );
    Buffer  *resample    ( ); // converts raw samples in AudioFmt to raw samples in specified fmt
    Buffer  *decodeAudio ( bool & );
    Buffer  *decodeAudio ( AVPacket * );
    Buffer  *getRemainingAudio ( AVStream * );

    // Get the current Buffer stats
    unsigned int getBytes             ( );
    unsigned int getBitrate           ( );
    unsigned int getChannels          ( );
    unsigned int getSampleRate        ( );
    unsigned int getTotalSamples      ( );
    unsigned int getSamplesPerChannel ( );

  private:
    Buffer          *m_pSampleBuffer;
    Buffer          *m_pRemainingSamples;
    ReSampleContext *m_pResample;
    unsigned int     m_iSampleSize;
  };

  class Vid : public Audio  {
  public:
             Vid ( );
    virtual ~Vid ( );
    virtual bool openStream ( QString q=QString::null );
    virtual void closeStream( );

    QImage &getFrame        ( AVFrame *, int, int );
    QImage &getFrame        ( double );
    void   delFrame         ( AVFrame *& );
    bool   isValidStream    ( );
    double getFrameRate     ( );

    void        setSWScale  ( int, int, int a=0, int c=0 );
    SwsContext *getSWScale  ( );
    void       freePicture  ( AVFrame *& );
    AVFrame   *allocPicture ( uint8_t *&, PixelFormat, int, int   );
    AVPacket *getNextPacket ( AVStream *, AVStream  *, int & );

  protected:
    int       reallySeek        ( AVFormatContext *, int, int64_t, int, double );
    double    guessDuration     ( AVFormatContext *, int );
    AVPacket *hasRemainingAudio ( AVStream * );

  private:
    SwsContext *m_pImgConvertCtx;
    AVPacket   *m_pPacket; // returned packet
    QImage      m_frame;   // single frame generated in getFrame

    // Required for Transcoding of the video through ffmpeg
    uint8_t    *m_pVideoBuffer;
    uint8_t    *m_pAudioBuffer;
    int         m_iVideoOutSize;
    int         m_iPadding[2];
    int         m_iPaddingYUV[3];
    AVFrame    *m_pFrame;       // Buffer for the current converted frame ( 720x480 )
    AVFrame    *m_pScaledFrame; // In case we need to scale, and pad the frame to keep the videos AspectRatio
  };

public:
           FFmpeg ( );
  virtual ~FFmpeg ( );

  virtual bool initStream     ( QString , enVideo, enAudio, uint i=1 );
  virtual bool addImage       ( QImage *, int, float fVolume=1.0f );
  virtual void endStream      ( );

  virtual bool addVid         ( QString, int, int, int a=0, int c=0, double r=0.0 );
  virtual bool initVid        ( QString );
  virtual void closeVid       (         );
  virtual QImage &getVidFrame (  double );

protected:
  bool addVideoStream    ( int, enVideo, int );
  bool addAudioStream    ( int );
  void getAudioSamples   ( );
  bool OpenAudioEncoder  ( );
  void CloseAudioEncoder ( );
  bool OpenVideoEncoder  ( );
  void CloseVideoEncoder ( );
  bool writeAudioFrame   ( float );
  bool writeVideoFrame   ( );
  void nextInputAudio    ( );
  Buffer *decodeAudio    ( float );
  void RGBtoYUV420P      ( const uint8_t *, uint8_t *, uint, bool, int, int, bool b=false );

  static void lockEngine   ( );
  static void unlockEngine ( );

private:
  AVFormatContext *m_pOutputCtx;     // Output Context
  AVStream        *m_pVideoStream;   // Output Video Stream
  AVStream        *m_pAudioStream;   // Output Audio Stream
  AVFrame         *m_pFrame;         // Current Frame ???
  QList<Audio *>   m_listInputAudio; // Input audio files
  Audio           *m_pInputAudio;    // Current audio file being processed
  Vid              m_vid;            // Handles vids streams.

  unsigned int     m_iSampleSize;    // Buffer size for audio samples
  int16_t         *m_pSamples;       // decoded input audio
  uint8_t         *m_pAudioBuffer;
  uint8_t         *m_pVideoBuffer;
  static QMutex    m_lock;
};

}; // end of namespace Encoder

#endif // ENCODER_FFMPEG_ENC_H
