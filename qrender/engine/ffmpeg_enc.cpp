/***************************************************************************
    ffmpeg_enc.cpp
                             -------------------
    Class Encode::FFmpeg implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    pktdumper.c demo how to in -> enc -> out
    
****************************************************************************/

#include <QImage>

#ifndef UINT64_C
#define UINT64_C uint64_t
#endif

extern "C" {
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <engine/colorspace.h>
}

#define AUDIO_BUF_SIZE 192 * 1024 * 4
#define VIDEO_BUF_SIZE 1835008

// Changes happened at around Version 0.6.3
//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  #define QRENDER_TYPE_AUDIO CODEC_TYPE_AUDIO
  #define QRENDER_TYPE_VIDEO CODEC_TYPE_VIDEO
  #define QRENDER_PKT_FLAG_KEY PKT_FLAG_KEY
  #define QRENDER_URL_WRITE_ONLY URL_WRONLY
  #define QRENDER_SAMPLE_FORMAT_U8 SAMPLE_FMT_U8
  #define QRENDER_SAMPLE_FMT_S16 SAMPLE_FMT_S16
  #define qrender_guess_format guess_format
  #define qrender_url_fopen  url_fopen
  #define qrender_url_fclose url_fclose
  #define qrender_new_stream av_new_stream
#else
  #define QRENDER_TYPE_AUDIO AVMEDIA_TYPE_AUDIO
  #define QRENDER_TYPE_VIDEO AVMEDIA_TYPE_VIDEO
  #define QRENDER_PKT_FLAG_KEY AV_PKT_FLAG_KEY
  #define QRENDER_URL_WRITE_ONLY AVIO_FLAG_WRITE
  #define QRENDER_SAMPLE_FORMAT_U8 AV_SAMPLE_FMT_U8
  #define QRENDER_SAMPLE_FMT_S16 AV_SAMPLE_FMT_S16
  #define qrender_guess_format av_guess_format
  #define qrender_url_fopen  avio_open
  #define qrender_url_fclose avio_close
  #define qrender_new_stream avformat_new_stream
//  #define qrender_new_stream av_new_program
#endif

#ifdef debugOut
#undef debugOut
#endif
#if 0
#define debugOut printf
#else
void dummy ( const char *, ... ) {};
#define debugOut dummy
#endif

#include "ffmpeg_enc.h"

namespace Encoder
{

QMutex FFmpeg::m_lock;

void printBuf ( short *pBuffer, int iSize )
{
  int t;
  for ( t=0; t<iSize; t++ )  {
    printf ( "[%.04X]", (unsigned short) pBuffer[t] );
  }
  printf ( "\n TOTAL = %d\n", iSize );
}


void SWSScale (struct SwsContext *pCtx, uint8_t **pData, int *iSrc, int iWidth, int iHeight, uint8_t **pDest, int *iDst )
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 48, 0 )
  sws_scale ( pCtx, pData, iSrc, iWidth, iHeight, pDest, iDst );
#else
  sws_scale ( pCtx, (const uint8_t**)pData, iSrc, iWidth, iHeight, pDest, iDst );
#endif
}

int qrender_avcodec_open ( AVCodecContext *pAvctx, AVCodec *pCodec, void *ppDict = NULL )
{
//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  ppDict = ppDict;
  return avcodec_open ( pAvctx, pCodec );
#else
  return avcodec_open2 ( pAvctx, pCodec, (AVDictionary **)ppDict );
#endif
}

FFmpeg::Buffer::Buffer ( int iSampleSize )
{
  iSampleRate        = 0;
  iChannels          = 0;
  iSamplesPerChannel = 0;
  pBuffer            = NULL;
  pBuffer            = (short *) av_malloc ( iSampleSize * sizeof ( short ) );
}

FFmpeg::Buffer::~Buffer ( )
{
  if ( pBuffer )
    av_free ( pBuffer );
  pBuffer = NULL;
}

//////////////////////////////////////////////////
//
//  Media - Baseclass
//
//////////////////////////////////////////////////
FFmpeg::Media::Media ( )
{
  m_iAudioStreamID = -1;
  m_iVideoStreamID = -1;
  m_pAudioCodecCtx = NULL;
  m_pVideoCodecCtx = NULL;
  m_pFormatCtx     = NULL;
}

FFmpeg::Media::~Media ( )
{
  closeStream ( );
}

bool FFmpeg::Media::openStream ( QString qsStreamName )
{
  if ( qsStreamName != QString::null )
     m_qsStreamName  = qsStreamName;

  int iStreamID;
  AVStream       *pStream   = NULL;
  AVCodec        *pCodec    = NULL;
  AVCodecContext *pCodecCtx = NULL;

  debugOut ( "%s : %d : streamName<%s>\n", __FUNCTION__, __LINE__, (const char *)m_qsStreamName.toUtf8 ( ) );
  // Open audio file
//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  if ( av_open_input_file ( &m_pFormatCtx, (const char *)m_qsStreamName.toUtf8 ( ), NULL, 0, NULL ) !=0 )
    return false; // Couldn't open file

  // Retrieve stream information
  if ( av_find_stream_info ( m_pFormatCtx ) < 0 )
    return false; // Couldn't find stream information
#else
  if ( avformat_open_input ( &m_pFormatCtx, (const char *)m_qsStreamName.toUtf8 ( ), NULL, NULL ) !=0 )
    return false; // Couldn't open file

  // Retrieve stream information
  if ( avformat_find_stream_info ( m_pFormatCtx, NULL ) < 0 )
    return false; // Couldn't find stream information
#endif

  // pFormatCtx->streams is just an array of pointers, of size pFormatCtx->nb_streams,
  // so let's walk through it until we find a video stream.
  bool bHasAudio = false;
  bool bHasVideo = false;
  for  ( iStreamID=0; iStreamID < (int)m_pFormatCtx->nb_streams; iStreamID++ )  {
    pStream = m_pFormatCtx->streams[iStreamID];
    if ( pStream->codec->codec_type == QRENDER_TYPE_VIDEO )  {
      if ( bHasVideo )
        continue;
      // Get a pointer to the codec context for the first video stream
      pCodecCtx = pStream->codec;

      // Find the decoder for the video stream
      pCodec = avcodec_find_decoder ( pCodecCtx->codec_id );
      if ( ! pCodec ) {
        fprintf ( stderr, "Unsupported codec!\n");
        continue; // Codec not found
      }
      // Inform the codec that we can handle truncated bitstreams -- i.e.,
      // bitstreams where frame boundaries can fall in the middle of packets
      if( pCodec->capabilities  &  CODEC_CAP_TRUNCATED )
          pCodecCtx->flags     |=  CODEC_FLAG_TRUNCATED;

      // Open codec
      lockEngine ( );
      if ( qrender_avcodec_open ( pCodecCtx, pCodec ) < 0 )  {
        unlockEngine ( );
        continue; // Could not open codec
      }
      unlockEngine ( );

//      if ( ( pCodecCtx->sample_aspect_ratio.num == 0 ) && ( pCodecCtx->sample_aspect_ratio.den == 1 ) )  {
//        // Assume input aspect ratio of 16:9
//        float fAspectRatio =  16.0f / 9.0f;
//        pCodecCtx->sample_aspect_ratio = av_d2q ( fAspectRatio * pCodecCtx->height / pCodecCtx->width, 255 );
//      }
      bHasVideo = true;
      m_pVideoCodecCtx = pCodecCtx;
      m_iVideoStreamID = iStreamID;
    }
    else if ( pStream->codec->codec_type == QRENDER_TYPE_AUDIO )  {
      if ( bHasAudio )
        continue;
      // Get a pointer to the codec context for the first audio stream
      pCodecCtx = pStream->codec;

      // Find the decoder for the audio stream
      pCodec = avcodec_find_decoder ( pCodecCtx->codec_id );
      if ( ! pCodec ) {
        fprintf ( stderr, "Unsupported codec!\n");
        continue; // Codec not found
      }

      // Open codec
      lockEngine ( );
      if ( qrender_avcodec_open ( pCodecCtx, pCodec ) < 0 )  {
        unlockEngine ( );
        continue; // Could not open codec
      }
      unlockEngine ( );
//      // Hack to correct wrong frame rates that seem to be generated by some codecs
//      if ( pCodecCtx->frame_rate > 1000 && pCodecCtx->frame_rate_base == 1 )
//           pCodecCtx->frame_rate_base = 1000;

      bHasAudio = true;
      m_pAudioCodecCtx = pCodecCtx;
      m_iAudioStreamID = iStreamID;
    }
  }
  // Didn't find an video stream
  if ( ( ! bHasVideo ) && ( ! bHasAudio ) )
    return false;  // Dont forget to close the stream though ...

  return true;
}

void FFmpeg::Media::closeStream ( )
{
  // Close the codec
  lockEngine ( );
  if ( m_pAudioCodecCtx )
    avcodec_close ( m_pAudioCodecCtx );
  if ( m_pVideoCodecCtx )
    avcodec_close ( m_pVideoCodecCtx );
  unlockEngine ( );
  // Close the file
  if ( m_pFormatCtx )
    av_close_input_file ( m_pFormatCtx ); //ooo
    //avformat_close_input ( m_pFormatCtx );  //xxx

  m_pAudioCodecCtx = NULL;
  m_pVideoCodecCtx = NULL;
  m_pFormatCtx     = NULL;
}

void FFmpeg::Media::setOutputContext ( AVCodecContext *pVideoContext, AVCodecContext *pAudioContext )
{
  m_pOutputVideo = pVideoContext;
  m_pOutputAudio = pAudioContext;
}

void FFmpeg::Media::resetStream ( )
{
}

QString FFmpeg::Media::streamName ( )
{
  return m_qsStreamName;
}

int FFmpeg::Media::streamID ( bool bAudio )
{
  if ( bAudio )
    return m_iAudioStreamID;
  return m_iVideoStreamID;
}

//////////////////////////////////////////////////
//
//  Audio - Class
//
//////////////////////////////////////////////////
FFmpeg::Audio::Audio ( QString qsFileName, unsigned int iSampleSize, AVStream *pOutputStream )
  : Media ( )
{
  m_qsStreamName  = qsFileName;
  m_iSampleSize   = iSampleSize;
  m_pOutputAudio  = pOutputStream->codec;
  m_pFormatCtx    = NULL;
  if (  m_iSampleSize < AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE )
        m_iSampleSize = AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE;

  m_pSampleBuffer     = new Buffer ( m_iSampleSize );
  m_pRemainingSamples = new Buffer ( m_iSampleSize );
  m_pResample = NULL;
}

FFmpeg::Audio::Audio ( unsigned int iSampleSize )
  : Media ( )
{
  m_iSampleSize   = iSampleSize;
  m_pOutputAudio  = NULL;
  if ( m_iSampleSize < AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE )
       m_iSampleSize = AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE;

  m_pSampleBuffer     = new Buffer ( m_iSampleSize );
  m_pRemainingSamples = new Buffer ( m_iSampleSize );
  m_pResample = NULL;
}

FFmpeg::Audio::~Audio ( )
{
  if ( m_pSampleBuffer )
    delete m_pSampleBuffer;
  m_pSampleBuffer = NULL;

  if ( m_pRemainingSamples )
    delete m_pRemainingSamples;
  m_pRemainingSamples = NULL;

//  closeStream ( );
//  if ( m_pCodecCtx )
//    delete m_pCodecCtx;
//  m_pCodecCtx = NULL;

//  if ( m_pFormatCtx )
//    delete m_pFormatCtx;
//  m_pFormatCtx = NULL;

//  av_free ( m_pSampleBuffer->pBuffer    );
//  av_free ( m_pSampleBuffer->pResampled );
//  audio_resample_close ( m_pResample );
}

void FFmpeg::Audio::closeStream ( )
{
  FFmpeg::Media::closeStream  ( );
  if ( m_pResample )
    audio_resample_close ( m_pResample );
  m_pResample = NULL;
}

FFmpeg::Buffer *FFmpeg::Audio::decode ( bool &bEOF )
{
  bEOF = false;
  if ( ! m_pAudioCodecCtx )
    return NULL;

  AVPacket packet;
  int iRet = -1;
  while ( 1 )  {
    iRet  = av_read_frame ( m_pFormatCtx, &packet );
    if ( ( packet.size <=1 ) && ( iRet >= 0 ) )
      continue;
    break;
  }

  if ( iRet == 0 )
    decode ( &packet  );
  else  if ( iRet < 0 )
    bEOF = true;

  return m_pSampleBuffer;
}

FFmpeg::Buffer *FFmpeg::Audio::decode ( AVPacket *pPacket )
{
  if ( ! pPacket )
    return NULL;

  unsigned int iOffset = 0;  // in Bytes
  int      iPacketSize = pPacket->size;
  uint8_t *pPacketData = pPacket->data;
  // Is this a packet from the video stream?
  if  ( pPacket->stream_index == m_iAudioStreamID )  {
    int16_t *pDecodedAudio = NULL;
    int      iDecodedSize, iLength;
    pDecodedAudio = (int16_t*) av_malloc ( m_iSampleSize );
    while  ( iPacketSize > 0 )  {
      iDecodedSize  = m_iSampleSize;
      memset ( pDecodedAudio, 0, m_iSampleSize );

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 26, 0 )
      iLength = avcodec_decode_audio2 ( m_pAudioCodecCtx, pDecodedAudio, &iDecodedSize, pPacketData, iPacketSize );
#else
      iLength = avcodec_decode_audio3 ( m_pAudioCodecCtx, pDecodedAudio, &iDecodedSize, pPacket );
#endif
      if ( iDecodedSize < 1 ) // Something completely strange happened ( E.g. ogg causes this )
        continue;

      if ( iLength <= 0 )  {  // Error handling ...
        iPacketSize --;
        pPacketData ++;
      }
      else  {
        iPacketSize -= iLength;
        pPacketData += iLength;
        memcpy ( (uint8_t*)(m_pSampleBuffer->pBuffer+iOffset), pDecodedAudio, iDecodedSize );
        iOffset += iDecodedSize;
      }
    }
    av_free ( pDecodedAudio );
  }
  if ( pPacket->data )
    av_free_packet ( pPacket );

  if ( iOffset == 0 )  {
    // Invalid package ... wtf.
    memset ( m_pSampleBuffer->pBuffer, 0, AUDIO_BUF_SIZE );
    m_pSampleBuffer->iSamplesPerChannel = 0;
    return NULL;
  }

  // Calculate the number of samples in the output buffer
  // Note this is the total number, so for stereo the format is LRLRLRLR
  m_pSampleBuffer->iSampleRate        = m_pAudioCodecCtx->sample_rate;
  m_pSampleBuffer->iChannels          = m_pAudioCodecCtx->channels;  // # of input Channels
  m_pSampleBuffer->iSamplesPerChannel = (unsigned int)iOffset / ( sizeof ( short ) * getChannels ( ) );
  if ( m_pAudioCodecCtx->sample_fmt  == QRENDER_SAMPLE_FORMAT_U8 )
       m_pSampleBuffer->iSamplesPerChannel *= 2;

  return m_pSampleBuffer;
}

FFmpeg::Buffer *FFmpeg::Audio::resample ( )
{
  if ( getSamplesPerChannel ( ) < 1 )  {
    return m_pSampleBuffer;
  }

  short *pResampled = (short *)av_malloc ( m_iSampleSize );

  memset ( pResampled, 0, m_iSampleSize );

  int iSamples = getSamplesPerChannel ( );
  iSamples     = audio_resample ( m_pResample, pResampled, (short *)m_pSampleBuffer->pBuffer, iSamples );

  av_free ( m_pSampleBuffer->pBuffer );
  m_pSampleBuffer->pBuffer     = pResampled;
  m_pSampleBuffer->iChannels   = m_pOutputAudio->channels;
  m_pSampleBuffer->iSampleRate = m_pOutputAudio->sample_rate;
  //m_pSampleBuffer->iSamplesPerChannel = (int)( (float)iSamples / getChannels ( ) );
  m_pSampleBuffer->iSamplesPerChannel = iSamples;

  return m_pSampleBuffer;
}

FFmpeg::Buffer *FFmpeg::Audio::resample ( AVPacket * )
{
  if ( getSamplesPerChannel ( ) < 1 )
    return m_pSampleBuffer;

  short *pResampled = (short *)av_malloc ( m_iSampleSize );

  memset ( pResampled, 0, m_iSampleSize );

  int iSamples = getSamplesPerChannel ( );
  iSamples     = audio_resample ( m_pResample, pResampled, (short *)m_pSampleBuffer->pBuffer, iSamples );

  av_free (  m_pSampleBuffer->pBuffer  );
  m_pSampleBuffer->pBuffer     = pResampled;
  m_pSampleBuffer->iChannels   = m_pOutputAudio->channels;
  m_pSampleBuffer->iSampleRate = m_pOutputAudio->sample_rate;
  //m_pSampleBuffer->iSamplesPerChannel = (int)( (float)iSamples / getChannels ( ) );
  m_pSampleBuffer->iSamplesPerChannel = iSamples;

  return m_pSampleBuffer;
}

unsigned int FFmpeg::Audio::getSamplesPerChannel ( )
{
  return m_pSampleBuffer->iSamplesPerChannel;
}

unsigned int FFmpeg::Audio::getTotalSamples ( )
{
  unsigned int iTotalSamples =  getSamplesPerChannel ( ) * getChannels ( );
  return iTotalSamples;
}

unsigned int FFmpeg::Audio::getChannels ( )
{
  return m_pSampleBuffer->iChannels;
}

unsigned int FFmpeg::Audio::getBitrate ( )
{
  return m_pAudioCodecCtx ? m_pAudioCodecCtx->bit_rate : 0;
}

unsigned int FFmpeg::Audio::getSampleRate ( )
{
  return m_pSampleBuffer->iSampleRate;
}

unsigned int FFmpeg::Audio::getBytes ( )
{
  unsigned int iBytes = getTotalSamples ( ) * sizeof ( short );
  return iBytes;
}

bool FFmpeg::Audio::openStream ( QString qsStreamName )
{
  bool bRet = Media::openStream ( qsStreamName );
  if ( ! m_pAudioCodecCtx )
    return false;
  return bRet;
}

FFmpeg::Buffer *FFmpeg::Audio::decodeAudio ( bool &bEOF )
{
  int    iSamples          = 0;
  int    iSampleSize       = sizeof ( short );
  int    iOutChannels      = m_pOutputAudio->channels;
  int    iOutSampleRate    = m_pOutputAudio->sample_rate;
  int    iOutFrameSize     = m_pOutputAudio->frame_size;

//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  if ( ! m_pResample && m_pAudioCodecCtx )
         m_pResample  = audio_resample_init ( iOutChannels, m_pAudioCodecCtx->channels, iOutSampleRate, m_pAudioCodecCtx->sample_rate );
#else
  AVSampleFormat enOutFormat = m_pOutputAudio->sample_fmt;
  AVSampleFormat enInFormat  = m_pAudioCodecCtx->sample_fmt;
  if ( ! m_pResample && m_pAudioCodecCtx )
         m_pResample  = av_audio_resample_init ( iOutChannels, m_pAudioCodecCtx->channels, iOutSampleRate, m_pAudioCodecCtx->sample_rate, enOutFormat, enInFormat, 16, 10, 0, 0.8 );
#endif

  FFmpeg::Buffer *pBuffer = NULL;
  char *pData = (char *)av_malloc ( AUDIO_BUF_SIZE );
  memset ( pData, 0, AUDIO_BUF_SIZE );

  // Lets check if we have leftover from the previous decoding run
  if ( m_pRemainingSamples->iSamplesPerChannel > 0 )  {
    iSamples = m_pRemainingSamples->iSamplesPerChannel;
    if ( iSamples > iOutFrameSize )
         iSamples = iOutFrameSize;
    memcpy ( pData, (char *)m_pRemainingSamples->pBuffer, iSamples * iOutChannels * iSampleSize );

    m_pRemainingSamples->iSamplesPerChannel -= iSamples;
    if ( m_pRemainingSamples->iSamplesPerChannel > 0 )  {
      memcpy ( (char *)m_pRemainingSamples->pBuffer, ((char *)m_pRemainingSamples->pBuffer + iSamples * iOutChannels * iSampleSize), m_pRemainingSamples->iSamplesPerChannel );
    }
  }

  // Main Loop until we have enough samples for this frame.
  while ( iSamples  < iOutFrameSize )  {
    pBuffer = decode   ( bEOF );
    pBuffer = resample ( );

    if ( pBuffer->iSamplesPerChannel == 0 )
      break;
    memcpy ( (pData + iSamples * iOutChannels * iSampleSize), (char *)pBuffer->pBuffer, pBuffer->iSamplesPerChannel * iOutChannels * iSampleSize );

    iSamples += pBuffer->iSamplesPerChannel;
  }

  // Next we store the remaining samples in a temp buffer
  if ( iSamples - iOutFrameSize > 0 )  {
    m_pRemainingSamples->iSamplesPerChannel = iSamples - iOutFrameSize;
    memcpy ( (char *)m_pRemainingSamples->pBuffer, (pData + iOutFrameSize * iOutChannels * iSampleSize ), m_pRemainingSamples->iSamplesPerChannel * iOutChannels * iSampleSize );
  }
  else  {
    m_pRemainingSamples->iSamplesPerChannel = -1;
    iOutFrameSize -= iSamples;
  }

  av_free ( m_pSampleBuffer->pBuffer );
  m_pSampleBuffer->pBuffer            = (short *)pData;
  m_pSampleBuffer->iSamplesPerChannel = iOutFrameSize;

  return m_pSampleBuffer;
}

FFmpeg::Buffer *FFmpeg::Audio::decodeAudio ( AVPacket *pPacket )
{
  int    iSampleSize       = sizeof ( short );
  int    iOutChannels      = m_pOutputAudio->channels;
  int    iOutSampleRate    = m_pOutputAudio->sample_rate;
  int    iOutFrameSize     = m_pOutputAudio->frame_size;

//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  if ( ! m_pResample && m_pAudioCodecCtx )
         m_pResample  = audio_resample_init ( iOutChannels, m_pAudioCodecCtx->channels, iOutSampleRate, m_pAudioCodecCtx->sample_rate );
#else
  AVSampleFormat enOutFormat = m_pOutputAudio->sample_fmt;
  AVSampleFormat enInFormat  = m_pAudioCodecCtx->sample_fmt;
  // First we ensure we have a resample context if required ...
  if ( ! m_pResample && m_pAudioCodecCtx )
         m_pResample    = av_audio_resample_init ( iOutChannels, m_pAudioCodecCtx->channels, iOutSampleRate, m_pAudioCodecCtx->sample_rate, enOutFormat, enInFormat, 16, 10, 0, 0.8 );
#endif

  // Alloc some memory ...
  FFmpeg::Buffer *pBuffer = NULL;
  // Get the new data from the packet ...
  pBuffer = decode   ( pPacket );
  pBuffer = resample ( pPacket );
  // important, audio_resample returns the number of resampled samples. We need the total number of samples here
  // I.e. if input is mono and output is stereo, then we need to multiply by 2.
  //pBuffer->iSamplesPerChannel *= iOutChannels;

  // Next we append the new data to the remaining data ...
  char *pRemainingStart = ( (char *)m_pRemainingSamples->pBuffer + m_pRemainingSamples->iSamplesPerChannel * iOutChannels * iSampleSize );
  memcpy ( pRemainingStart, (char *)pBuffer->pBuffer, pBuffer->iSamplesPerChannel * iOutChannels * iSampleSize );
  m_pRemainingSamples->iSamplesPerChannel += pBuffer->iSamplesPerChannel;

  // Then we check if we have enough samples for at least one frame
  if ( m_pRemainingSamples->iSamplesPerChannel >= iOutFrameSize )  {
    char *pData = (char *)av_malloc ( AUDIO_BUF_SIZE );
    memset ( pData, 0, AUDIO_BUF_SIZE );

    pRemainingStart = ( (char *)m_pRemainingSamples->pBuffer + iOutFrameSize * iOutChannels * iSampleSize );
    memcpy  (  pData,   (char *)m_pRemainingSamples->pBuffer,  iOutFrameSize * iOutChannels * iSampleSize );
    m_pRemainingSamples->iSamplesPerChannel -= iOutFrameSize;
    // Should not happen but to be safe ...
    if ( m_pRemainingSamples->iSamplesPerChannel < 0 )
         m_pRemainingSamples->iSamplesPerChannel = 0;
    memcpy  ( (char *)m_pRemainingSamples->pBuffer,  pRemainingStart, m_pRemainingSamples->iSamplesPerChannel * iOutChannels * iSampleSize );

    av_free ( m_pSampleBuffer->pBuffer );
    m_pSampleBuffer->pBuffer            = (short *)pData;
    m_pSampleBuffer->iSamplesPerChannel = iOutFrameSize;

    return m_pSampleBuffer;
  }

  return NULL;
}

FFmpeg::Buffer *FFmpeg::Audio::getRemainingAudio ( AVStream *pOutputAudioStream )
{
  if ( ! pOutputAudioStream )
    return NULL;

  int iOutFrameSize = pOutputAudioStream->codec->frame_size;
  int iOutChannels  = pOutputAudioStream->codec->channels;
  int iSampleSize   = sizeof ( short );

  if ( ( m_pRemainingSamples->iSamplesPerChannel < iOutFrameSize ) || ( iOutFrameSize < 1 ) )
    return NULL;

  m_pSampleBuffer->iSamplesPerChannel      = iOutFrameSize;
  m_pRemainingSamples->iSamplesPerChannel -= iOutFrameSize;
  memcpy ( (char *)m_pSampleBuffer->pBuffer,      (char *)m_pRemainingSamples->pBuffer,  iOutFrameSize * iOutChannels * iSampleSize );
  memcpy ( (char *)m_pRemainingSamples->pBuffer, ((char *)m_pRemainingSamples->pBuffer + iOutFrameSize * iOutChannels * iSampleSize ), m_pRemainingSamples->iSamplesPerChannel * iSampleSize * iOutChannels );

  return m_pSampleBuffer;
}

//////////////////////////////////////////////////
//
//  Vid - Class
//
//////////////////////////////////////////////////
FFmpeg::Vid::Vid ( )
  : Audio ( AUDIO_BUF_SIZE )
{
  m_pPacket        = new AVPacket;
  av_init_packet   ( m_pPacket );
  m_pFrame         = NULL;
  m_pScaledFrame   = NULL;
  m_pImgConvertCtx = NULL;
  m_iVideoOutSize  = 0;
  m_pVideoBuffer   = (uint8_t *) av_malloc ( VIDEO_BUF_SIZE );
  m_pAudioBuffer   = (uint8_t *) av_malloc ( AUDIO_BUF_SIZE );
  m_pOutputVideo   = NULL;
  m_pOutputAudio   = NULL;
  m_iPaddingYUV[0] = 16;
  m_iPaddingYUV[1] = 128;
  m_iPaddingYUV[2] = 128;
  m_iPadding[0]    = 0;
  m_iPadding[1]    = 0;
}

FFmpeg::Vid::~Vid ( )
{
  if ( m_pPacket )
    delete m_pPacket;

  freePicture ( m_pFrame );
  freePicture ( m_pScaledFrame );

  if ( m_pVideoBuffer )
    av_free ( m_pVideoBuffer );
  if ( m_pAudioBuffer )
    av_free ( m_pAudioBuffer );

  if ( m_pImgConvertCtx )
    sws_freeContext ( m_pImgConvertCtx );
  m_pImgConvertCtx = NULL;
}

bool FFmpeg::Vid::openStream ( QString qsStreamName )
{
  bool bRet = Media::openStream ( qsStreamName );
  m_iVideoOutSize = 0;
  if ( ! m_pVideoCodecCtx )
    return false;
  return bRet;
}

void FFmpeg::Vid::closeStream ( )
{
  m_iVideoOutSize = 0;
  FFmpeg::Audio::closeStream ( );
}

void FFmpeg::Vid::setSWScale ( int iTargetWidth, int iTargetHeight, int iAspectRatio, int iPaddingColor )
{
  if ( ! m_pVideoCodecCtx )
    return;

  int r = ( ( iPaddingColor >> 16 ) & 0xFF );
  int g = ( ( iPaddingColor >>  8 ) & 0xFF );
  int b = (   iPaddingColor         & 0xFF );

  m_iPaddingYUV[0] = RGB_TO_Y ( r, g, b    );
  m_iPaddingYUV[1] = RGB_TO_U ( r, g, b, 0 );
  m_iPaddingYUV[2] = RGB_TO_V ( r, g, b, 0 );
  m_iPadding[0]    = 0;
  m_iPadding[1]    = 0;

  // as we only generate a YUV420P picture, we must convert it to the codec pixel format if needed
  int iSourceWidth  = m_pVideoCodecCtx->width;
  int iSourceHeight = m_pVideoCodecCtx->height;

  int iAspectNum = m_pVideoCodecCtx->sample_aspect_ratio.num;
  int iAspectDen = m_pVideoCodecCtx->sample_aspect_ratio.den;

  // Next we need to figure out if we need padding ( vertically or horizontally or non )
  if ( iAspectRatio == 1 )  { // requesting the result to be 16/9
    // In case we have input pixel ratio the same as the requested ratio we don't need padding.
    double fRatio = (double)iSourceWidth / iSourceHeight;
    if ( fRatio != 16.0 / 9.0 )  {
      // could not establish the same input ratio as output ratio from source resoltion

      // δ = 16/9 x 480/720 = (16x480)/(9x720) = 7680/6480 = ( 2^9 x 3 x 5 ) / ( 2^4 x 3^4 x 5 ) = 2^5 / 3^3 == 32/27
      // we want to create a 16:9 video so ...
      // if the source is not also 16:9 we need to prescale and pad the final video accordingly.
      if ( ( iAspectNum != 32 ) || ( iAspectDen != 27 ) )  {
        // Could not deduct same input ratio as output ratio from sample_aspect_ratio of input video
        float fScale = 27.0 / 32.0;
        if  ( ( iAspectDen != 0 ) && ( iAspectNum != 0 ) )
              fScale  *= (float)iAspectNum / iAspectDen;
        // ( 640x480  ) == 720 - ( 0.84375 * 640  ) == 720 -  540 ==  180
        // ( 1280x720 ) == 720 - ( 0.84375 * 1280 ) == 720 - 1080 == -360
        double fWRatio = (double)iSourceWidth  / iTargetWidth;  // ( 640x480 ) : 0.888' || ( 1280x720 ) : 1.777'
        double fHRatio = (double)iSourceHeight / iTargetHeight; // ( 640x480 ) : 1.0000 || ( 1280x720 ) : 1.5000
        // Input to output ratio to determine if padding is on X or Y axis
        if ( fWRatio > fHRatio )  {
          // we need to pad horizontally ( top and bottom )
          // so width = 720 / scaling factor == fWRatio => height = sourceHeight / fWRatio
          double fDelta = (double)iTargetHeight - ( fScale * iSourceHeight / fWRatio );
          m_iPadding[1] = (int)( fDelta / 2.0 );
          iTargetHeight = iTargetHeight - 2 * m_iPadding[1];
        }
        else  {
          // we need to pad vertically ( left and right )
          // So height = 480 / scaling factor == fHRatio => width = sourceWidth / fHRatio
          double fDelta = (double)iTargetWidth - ( fScale * iSourceWidth / fHRatio ); 
          m_iPadding[0] = (int)( fDelta / 2.0 );
          iTargetWidth  = iTargetWidth - 2 * m_iPadding[0];
        }
      }
    }
  }
  else  { // requesting the result to be 4:3
    // In case we have input pixel ratio the same as the requested ratio we don't need padding.
    double fRatio = (double)iSourceWidth / iSourceHeight;
    if ( fRatio != 4.0 / 3.0 )  {
      // could not establish the same input ratio as output ratio from source resoltion

      // δ' = 4/3 / (720/480) = ( 480 x 4 ) / ( 720 x 3 ) = 160 / 180 = 8/9
      // We want to create a 4:3 video, so ...
      // if the source is not also 4:3 we need to prescale and pad the final video accordingly.
      if ( ( iAspectNum != 9 ) || ( iAspectDen != 8 ) )  {
        // Could not deduct same input ratio as output ratio from sample_aspect_ratio of input video
        float fScale = 9.0 / 8.0;
        if  ( ( iAspectDen != 0 ) && ( iAspectNum != 0 ) )
              fScale  *= (float)iAspectNum     / iAspectDen;
        double fWRatio = (double)iSourceWidth  / iTargetWidth;
        double fHRatio = (double)iSourceHeight / iTargetHeight;
        // Input to output ratio to determine if padding is on X or Y axis
        if ( fWRatio > fHRatio )  {
          // we need to pad horizontally ( top and bottom )
          // so width = 720 / scaling factor == fWRatio => height = sourceHeight / fWRatio
          double fDelta = (double)iTargetHeight - ( fScale * iSourceHeight / fWRatio );
          m_iPadding[1] = (int)( fDelta / 2.0 );
          iTargetHeight = iTargetHeight - 2 * m_iPadding[1];
        }
        else  {
          // we need to pad vertically ( left and right )
          // So height = 480 / scaling factor == fHRatio => width = sourceWidth / fHRatio
          double fDelta = (double)iTargetWidth - ( fScale * iSourceWidth / fHRatio );
          m_iPadding[0] = (int)( fDelta / 2.0 );
          iTargetWidth  = iTargetWidth - 2 * m_iPadding[0];
        }
      }
    }
  }

  if ( ( m_pVideoCodecCtx->pix_fmt != PIX_FMT_YUV420P ) || ( iSourceWidth != iTargetWidth ) || ( iSourceHeight != iTargetHeight ) )  {
    m_pImgConvertCtx = sws_getContext ( iSourceWidth, iSourceHeight, m_pVideoCodecCtx->pix_fmt, iTargetWidth, iTargetHeight, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL );
    m_pScaledFrame   = allocPicture   ( m_pVideoBuffer, PIX_FMT_YUV420P, iTargetWidth, iTargetHeight );
  }
  else
    m_pImgConvertCtx = NULL;

  // allocate the encoded raw picture
  m_pFrame = allocPicture ( m_pVideoBuffer, PIX_FMT_YUV420P, iTargetWidth + 2 * m_iPadding[0], iTargetHeight + 2 * m_iPadding[1] );

  if ( ! m_pFrame )
    printf ( "Could not allocate picture\n" );
//#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 72, 2 ) //ooo
#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 123, 0 )  //xxx
  else  {
    m_pFrame->width  = iTargetWidth  + 2 * m_iPadding[0]; // Should always be 720
    m_pFrame->height = iTargetHeight + 2 * m_iPadding[1]; // Should always be 480;
  }
#endif
}

AVFrame *FFmpeg::Vid::allocPicture ( uint8_t *&, PixelFormat pixFormat, int iWidth, int iHeight )
{
  // Note: Might cause memory leak ? Should be invested at some point in time
  AVFrame *pFrame = avcodec_alloc_frame ( );
  if (  !  pFrame )
    return NULL;

  int iSize = avpicture_get_size ( pixFormat, iWidth, iHeight );
  uint8_t *pBuffer = (uint8_t *) av_malloc ( iSize );
  if (  !  pBuffer )  {
      av_free ( pFrame );
      return NULL;
  }

  avpicture_fill ( (AVPicture *)pFrame, pBuffer, pixFormat, iWidth, iHeight );
  return pFrame;
}

void FFmpeg::Vid::freePicture ( AVFrame *&pFrame )
{
  if ( pFrame )  {
    if ( pFrame->data[0] )
      av_free ( pFrame->data[0] );
    av_free ( pFrame );
    pFrame = NULL;
  }
}

SwsContext *FFmpeg::Vid::getSWScale ( )
{
  return m_pImgConvertCtx;
}

AVPacket *FFmpeg::Vid::hasRemainingAudio ( AVStream *pOutputAudioStream )
{
  Buffer *pSamples = getRemainingAudio ( pOutputAudioStream );
  if (  ! pSamples )
     return NULL;

  // Free the packet that was previously allocated by av_read_frame
  av_init_packet ( m_pPacket );
  unsigned int iSize = getSamplesPerChannel ( ) * sizeof ( short ); //getTotalSamples ( ) * sizeof ( short );

  AVCodecContext *pCodec = pOutputAudioStream->codec;
  const struct AVCodec *codec = pCodec->codec;

  if ( codec )
//#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 72, 2 ) //ooo
#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 123, 0 )   //xxx
    m_pPacket->size = codec->encode2 ( pCodec, m_pAudioBuffer, iSize, (void *)pSamples->pBuffer );    //ooo
    //m_pPacket->size = codec->encode2 ( pCodec, m_pPacket, m_pFrame, (int *)pSamples->pBuffer );      //xxx
#else   
    m_pPacket->size = codec->encode ( pCodec, m_pAudioBuffer, iSize, pSamples );
#endif
  pOutputAudioStream->codec->frame_number++;

//  if ( pOutputAudioStream->codec->coded_frame && pOutputAudioStream->codec->coded_frame->pts != AV_NOPTS_VALUE )
    m_pPacket->pts  = av_rescale_q ( pOutputAudioStream->codec->coded_frame->pts, pOutputAudioStream->codec->time_base, pOutputAudioStream->time_base );

  m_pPacket->flags       |= QRENDER_PKT_FLAG_KEY;
  m_pPacket->stream_index = pOutputAudioStream->index;
  m_pPacket->data         = m_pAudioBuffer;

  return m_pPacket;
}

AVPacket *FFmpeg::Vid::getNextPacket ( AVStream *pOutputVideoStream, AVStream *pOutputAudioStream, int &iReuseFrame )
{
  // iAdjusFrame is decremented by one each time a frame is re-used
  // iAdjusrtFrame is set to 0 when a new frame is taken from the stream.

  bool bFound = false;
  int iFrameFinished;

  static int iInterleave = 3;
  iInterleave--;
  if   ( iInterleave  < 2 )  { // every other Image frame is followed one audio frame ( if we have some data )
    if ( iInterleave == 0 )
         iInterleave  = 3;
    // This function will check if we have enough data for another output audio frame
    AVPacket *pPacket = hasRemainingAudio ( pOutputAudioStream );
    if ( pPacket )
      return pPacket;
  }

  // Here we adjust the frame rate in case the source has less frames ( E.g. 10 FPS ) then the required frames ( E.g. 29.97 )
  if ( ( iReuseFrame >= 1 ) && ( m_iVideoOutSize > 0 ) )  {
    m_iVideoOutSize = avcodec_encode_video ( pOutputVideoStream->codec, m_pVideoBuffer, VIDEO_BUF_SIZE, m_pFrame );
    if ( m_iVideoOutSize >= 0 )  {
      av_init_packet  ( m_pPacket );
      iReuseFrame --;
      m_pPacket->pts = av_rescale_q ( pOutputVideoStream->codec->coded_frame->pts, pOutputVideoStream->codec->time_base, pOutputVideoStream->time_base );

      if ( pOutputVideoStream->codec->coded_frame->key_frame )
           m_pPacket->flags  |= QRENDER_PKT_FLAG_KEY;
      m_pPacket->stream_index = pOutputVideoStream->index;
      m_pPacket->data         = m_pVideoBuffer;
      m_pPacket->size         = m_iVideoOutSize;
      return m_pPacket;
    }
  }

  // Free the packet that was previously allocated by av_read_frame
  av_free_packet ( m_pPacket );

  while ( ! bFound && av_read_frame ( m_pFormatCtx, m_pPacket ) >= 0 )  {
    if  ( m_pPacket->stream_index == m_iVideoStreamID )  {
      // Ignore this frame if source frame rate is > the target frame rate
      if ( iReuseFrame < 0 )  {
        m_iVideoOutSize = avcodec_encode_video ( pOutputVideoStream->codec, m_pVideoBuffer, VIDEO_BUF_SIZE, m_pFrame );
        iReuseFrame ++;
        continue;
      }

      int iWidth, iHeight, iNumBytes;
      uint8_t   *pBuffer   = NULL;
      AVFrame   *pFrame    = NULL;

      pFrame    = avcodec_alloc_frame ( );
      iWidth    = m_pVideoCodecCtx->width;
      iHeight   = m_pVideoCodecCtx->height;
      // Determine required buffer size and allocate buffer
//    iNumBytes = avpicture_get_size ( PIX_FMT_YUV420P, iWidth, iHeight ) + FF_INPUT_BUFFER_PADDING_SIZE;
      int  x = m_pOutputVideo->width;
      int  y = m_pOutputVideo->height;
      if ( x < iWidth  )
           x = iWidth;
      if ( y < iHeight )
           y = iHeight;
      iNumBytes = avpicture_get_size ( PIX_FMT_YUV420P, x, y ) + FF_INPUT_BUFFER_PADDING_SIZE;
      pBuffer   = new uint8_t[iNumBytes];
      pBuffer[iNumBytes-1] = 0;

      // Assign appropriate parts of buffer to image planes in pFrameRGB
      avpicture_fill ( (AVPicture *)pFrame, pBuffer, PIX_FMT_YUV420P, iWidth, iHeight );

      iFrameFinished = 0;
      // Decode input video frame
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 26, 0 )
      int  iLen = avcodec_decode_video ( m_pVideoCodecCtx, pFrame, &iFrameFinished, m_pPacket->data, m_pPacket->size );
      if ( iFrameFinished == 0 ) // Mpeg streams might need this double encoding phase ...
           iLen = avcodec_decode_video ( m_pVideoCodecCtx, pFrame, &iFrameFinished, m_pPacket->data, m_pPacket->size );
      iLen = iLen;
#else
      int  iLen = avcodec_decode_video2 ( m_pVideoCodecCtx, pFrame, &iFrameFinished, m_pPacket );
      if ( iFrameFinished == 0 ) // Mpeg streams might need this double encoding phase ...
           iLen = avcodec_decode_video2 ( m_pVideoCodecCtx, pFrame, &iFrameFinished, m_pPacket );
      iLen = iLen;
#endif
      // Did we get a video frame?
      // Note : Instead of running the decoding again, it seems to be agreed on
      //        that simply ignoring the iFrameFinished check. However it works for me !
      if ( iFrameFinished )  {
        int iOutSize = -1;
        if ( ( m_iPadding[0] == 0 ) && ( m_iPadding[1] == 0 ) )  {
          if ( m_pImgConvertCtx )
            SWSScale ( m_pImgConvertCtx, pFrame->data, pFrame->linesize, 0, iHeight, m_pFrame->data, m_pFrame->linesize );
          else // av_picture_copy ( AVPicture *dst, const AVPicture *src, int pix_fmt, int width, int height);
            av_picture_copy ( (AVPicture *)m_pFrame, (AVPicture *)pFrame, PIX_FMT_YUV420P, iWidth, iHeight );
        }
        else  {
          int iOutWidth     = m_pOutputVideo->width;
          int iOutHeight    = m_pOutputVideo->height;
          int iScaledWidth  = iOutWidth -2*m_iPadding[0];
          int iScaledHeight = iOutHeight-2*m_iPadding[1];
          iScaledWidth = iScaledWidth; // avoid compiler complaints ...
          if ( m_pImgConvertCtx )  {
            SWSScale ( m_pImgConvertCtx, pFrame->data, pFrame->linesize, 0, iScaledHeight, m_pScaledFrame->data, m_pScaledFrame->linesize );
            av_picture_pad ( (AVPicture *)m_pFrame, (const AVPicture *)m_pScaledFrame, iOutHeight, iOutWidth, PIX_FMT_YUV420P, m_iPadding[1], m_iPadding[1], m_iPadding[0], m_iPadding[0], m_iPaddingYUV );
          }
          else 
            av_picture_pad ( (AVPicture *)m_pFrame, (const AVPicture *)pFrame, iOutHeight, iOutWidth, PIX_FMT_YUV420P, m_iPadding[1], m_iPadding[1], m_iPadding[0], m_iPadding[0], m_iPaddingYUV );
        }

        // encode the image
        iOutSize = avcodec_encode_video ( pOutputVideoStream->codec, m_pVideoBuffer, VIDEO_BUF_SIZE, m_pFrame );

        if ( iOutSize < 0 )
          continue;
        // if zero size, it means the image was buffered
        if ( iOutSize > 0 ) {
          av_init_packet  ( m_pPacket );
          m_pPacket->pts = av_rescale_q ( pOutputVideoStream->codec->coded_frame->pts, pOutputVideoStream->codec->time_base, pOutputVideoStream->time_base );

          if ( pOutputVideoStream->codec->coded_frame->key_frame )
               m_pPacket->flags  |= QRENDER_PKT_FLAG_KEY;
          m_pPacket->stream_index = pOutputVideoStream->index;
          m_pPacket->data         = m_pVideoBuffer;
          m_pPacket->size         = iOutSize;
          m_iVideoOutSize = iOutSize;
          iReuseFrame     = 0; // Signals to the calling function  that we have decoded a new frame.

          delete []pBuffer;
          if ( pFrame )
            av_free ( pFrame );

          return m_pPacket;
        }
      }
    }
    else if ( m_pPacket->stream_index == m_iAudioStreamID )  {
      Buffer *pSamples = decodeAudio ( m_pPacket );
      if  ( ! pSamples )
        continue; // with the next package from stream
      unsigned int iSize = getSamplesPerChannel  ( ) * sizeof ( short );

      av_init_packet ( m_pPacket );

      AVCodecContext *pCodec = pOutputAudioStream->codec;
      const struct AVCodec *codec = pCodec->codec;
      if ( codec )
//#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 72, 2 ) //ooo
#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT ( 52, 123, 0 )   //xxx
        m_pPacket->size = codec->encode2 ( pCodec, m_pAudioBuffer, iSize, (void *)pSamples->pBuffer );    //ooo
        //m_pPacket->size = codec->encode2 ( pCodec, m_pPacket, m_pFrame, (int *)pSamples->pBuffer );      //xxx
#else
        m_pPacket->size = codec->encode ( pCodec, m_pAudioBuffer, iSize, pSamples );
#endif
      //m_pPacket->size = pOutputAudioStream->codec->codec->encode2 ( pOutputAudioStream->codec, m_pAudioBuffer, iSize, (void *)pSamples->pBuffer );
      pOutputAudioStream->codec->frame_number++;

//      if ( pOutputAudioStream->codec->coded_frame && pOutputAudioStream->codec->coded_frame->pts != AV_NOPTS_VALUE )
        m_pPacket->pts = av_rescale_q ( pOutputAudioStream->codec->coded_frame->pts, pOutputAudioStream->codec->time_base, pOutputAudioStream->time_base );

      m_pPacket->flags       |= QRENDER_PKT_FLAG_KEY;
      m_pPacket->stream_index = pOutputAudioStream->index;
      m_pPacket->data         = m_pAudioBuffer;

      return m_pPacket;
    }
    else  {
      ; // error. not a valid streamID
    }
  }
  return NULL;
}

QImage &FFmpeg::Vid::getFrame ( AVFrame *pFrame, int iWidth, int iHeight )
{
  if ( ( ! pFrame ) || ( ! m_pVideoCodecCtx ) )  {
    // TODO: Create Error Image ...
    m_frame = QImage ( );
    return m_frame;
  }

  uint8_t    *pBuffer   = NULL;
  SwsContext *pSwsCtx   = NULL;
  AVFrame    *pFrameRGB = avcodec_alloc_frame ( );
  int iNumBytes;

  // Determine required buffer size and allocate buffer
  iNumBytes = avpicture_get_size ( PIX_FMT_RGB32, iWidth, iHeight );
  pBuffer   = new uint8_t[iNumBytes];

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  avpicture_fill ( (AVPicture *)pFrameRGB, pBuffer, PIX_FMT_RGB32, iWidth, iHeight );

  pSwsCtx = sws_getContext ( iWidth, iHeight, m_pVideoCodecCtx->pix_fmt, iWidth, iHeight, PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL );

  // Convert the image from its native format to RGB
  SWSScale ( pSwsCtx, pFrame->data, pFrame->linesize, 0, iHeight, pFrameRGB->data, pFrameRGB->linesize );

  {  // This to destroy the tmp image and force a deep copy of the bits into m_image.
    QImage tmp ( (uchar *)pBuffer, iWidth, iHeight, QImage::Format_RGB32 );
    m_frame = tmp.copy ( );
  }

  if ( pSwsCtx )
   sws_freeContext ( pSwsCtx );
  if ( pBuffer )
    delete []pBuffer;
  if ( pFrameRGB )
    av_free ( pFrameRGB );

  return m_frame;
}

QImage &FFmpeg::Vid::getFrame ( double fStreamPos )
{
  if ( ( fStreamPos < -1.0f ) || ( fStreamPos > 1.0f ) || ( ! m_pVideoCodecCtx ) )  {
    // TODO: Create Error Image ...
    m_frame = QImage ( );
    return m_frame;
  }

  AVPacket   packet;
  AVFrame   *pFrame    = NULL;
  bool bFound = false;

  int iFrameFinished;
  int iWidth, iHeight; //, iNumBytes;

  pFrame    = avcodec_alloc_frame ( );
  iWidth    = m_pVideoCodecCtx->width;
  iHeight   = m_pVideoCodecCtx->height;
  // Determine required buffer size and allocate buffer
  //  iNumBytes = avpicture_get_size ( PIX_FMT_RGB32, iWidth, iHeight ) + FF_INPUT_BUFFER_PADDING_SIZE;

  if ( fStreamPos < 0.0 )  {
    // get the last decoded frame, else invert the number ...
    fStreamPos *= -1.0;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 26, 0 )
    avcodec_decode_video ( m_pVideoCodecCtx, pFrame, &iFrameFinished, NULL, 0 );
#else
    packet.data = NULL;
    packet.size = 0;
    avcodec_decode_video2 ( m_pVideoCodecCtx, pFrame, &iFrameFinished, &packet );
#endif

    // Did we get a video frame?
    if ( iFrameFinished )  {
      getFrame ( pFrame, iWidth, iHeight );
      bFound = true;
    }
  }

  int iSeekPos  = -1;
  if ( ( fStreamPos > 0.0 ) && ( ! bFound ) )  {
    double fDuration =  (double) m_pFormatCtx->duration / AV_TIME_BASE; // can be unknown & can be incorrect (e.g. .vob files)
    if ( fDuration <= 0 )
         fDuration  = guessDuration ( m_pFormatCtx, m_iVideoStreamID );
    if ( fDuration <= 0 )  {
        ; //av_log ( NULL, AV_LOG_ERROR, "  duration is unknown: %.2f\n", duration);
    }
    AVStream *pVideoStream = m_pFormatCtx->streams[m_iVideoStreamID];
    int64_t iEndPosition = (int64_t)( fDuration * fStreamPos / av_q2d ( pVideoStream->time_base ) ); // in time_base uni
    iSeekPos = reallySeek ( m_pFormatCtx, m_iVideoStreamID, iEndPosition, 0, fDuration );
    iSeekPos = iSeekPos;
  }

  while ( !bFound && av_read_frame ( m_pFormatCtx, &packet ) >= 0 )  {
    // Decode video frame
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 26, 0 )
    avcodec_decode_video ( m_pVideoCodecCtx, pFrame, &iFrameFinished, packet.data, packet.size );
#else
    avcodec_decode_video2 ( m_pVideoCodecCtx, pFrame, &iFrameFinished, &packet );
#endif

    // Did we get a video frame?
    if ( iFrameFinished )  {
      getFrame ( pFrame, iWidth, iHeight );
      bFound = true;
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet ( &packet );
  }

  if ( pFrame )
    av_free ( pFrame );

  return m_frame;
}

double FFmpeg::Vid::getFrameRate ( )
{
  if ( ! m_pVideoCodecCtx )
    return 30000.0 / 1001.0;

  AVStream *pStream = m_pFormatCtx->streams[m_iVideoStreamID];
  if ( ( m_pVideoCodecCtx->time_base.den != pStream->r_frame_rate.den ) ||
       ( m_pVideoCodecCtx->time_base.num != pStream->r_frame_rate.num )  )
    return (double)pStream->r_frame_rate.num / pStream->r_frame_rate.den;

  return (double)m_pVideoCodecCtx->time_base.num / m_pVideoCodecCtx->time_base.den;
}

/*
from moviethumbnail.sf.net
try hard to seek
assume flags can be either 0 or AVSEEK_FLAG_BACKWARD
*/
int FFmpeg::Vid::reallySeek ( AVFormatContext *pFormatCtx, int index, int64_t timestamp, int flags, double duration )
{
    if ( ( flags != 0 ) && ( flags != AVSEEK_FLAG_BACKWARD ) )
      return -1;

    int ret;

    /* first try av_seek_frame */
    ret = av_seek_frame ( pFormatCtx, index, timestamp, flags );
    if (ret >= 0) { // success
        return ret;
    }
    ret = av_seek_frame ( pFormatCtx, -1, timestamp, flags );
    if (ret >= 0) { // success
        return ret;
    }

    /* then we try seeking to any (non key) frame AVSEEK_FLAG_ANY */
    ret = av_seek_frame(pFormatCtx, index, timestamp, flags | AVSEEK_FLAG_ANY);
    if (ret >= 0) { // success
        //av_log(NULL, LOG_INFO, "AVSEEK_FLAG_ANY: timestamp: %"PRId64"\n", timestamp); // DEBUG
        return ret;
    }

    /* and then we try seeking by byte (AVSEEK_FLAG_BYTE) */
    // here we assume that the whole file has duration seconds.
    // so we'll interpolate accordingly.
    AVStream *pStream   = pFormatCtx->streams[index];
    int64_t duration_tb = (int64_t)( duration / av_q2d ( pStream->time_base ) ); // in time_base unit
    duration_tb = duration_tb; // avoid compiler complaints ...
    double start_time   = (double) pFormatCtx->start_time / AV_TIME_BASE; // in seconds
    // if start_time is negative, we ignore it; FIXME: is this ok?
    if (start_time < 0) {
        start_time = 0;
    }

    // normally when seeking by timestamp we add start_time to timestamp
    // before seeking, but seeking by byte we need to subtract the added start_time
    timestamp -= (int64_t)( start_time / av_q2d ( pStream->time_base ) );
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 54, 0, 100 )
    if (pFormatCtx->file_size <= 0) {
        return -1;
    }

    if (duration > 0) {
        int64_t byte_pos = av_rescale(timestamp, pFormatCtx->file_size, duration_tb);
        //av_log(NULL, LOG_INFO, "AVSEEK_FLAG_BYTE: byte_pos: %"PRId64", timestamp: %"PRId64", file_size: %"PRId64", duration_tb: %"PRId64"\n", byte_pos, timestamp, pFormatCtx->file_size, duration_tb);
        return av_seek_frame(pFormatCtx, index, byte_pos, AVSEEK_FLAG_BYTE);
    }
#endif
    return -1;
}
/*
from moviethumbnail.sf.net
return the duration. guess when unknown.
must be called after codec has been opened
*/
double FFmpeg::Vid::guessDuration ( AVFormatContext *pFormatCtx, int index )
{
    double duration = (double) pFormatCtx->duration / AV_TIME_BASE; // can be incorrect for .vob files
    if (duration > 0) {
        return duration;
    }
    AVStream *pStream = pFormatCtx->streams[index];

    // if stream bitrate is known we'll interpolate from file size.
    // pFormatCtx->start_time would be incorrect for .vob file with multiple titles.
    // pStream->start_time doesn't work either. so we'll need to disable timestamping.
    if ( NULL == pStream || NULL == pStream->codec )
      return 0.0;

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 54, 0, 100 )
    if ( pStream->codec->bit_rate > 0 && pFormatCtx->file_size > 0 ) {
        double guess = 0.9 * pFormatCtx->file_size / (pStream->codec->bit_rate / 8.0 );
        if ( guess > 0.0 )  {
            av_log ( NULL, AV_LOG_ERROR, "  ** duration is unknown: %.2f; guessing: %.2f s from bit_rate\n", duration, guess );
            return guess;
        }
    }
#endif
    return -1;
}

bool FFmpeg::Vid::isValidStream ( )
{
  return ( m_pVideoCodecCtx != NULL );
}

//////////////////////////////////////////////////
//
//  FFmpeg - Encoder Class
//
//////////////////////////////////////////////////
FFmpeg::FFmpeg  ( )
      : Encoder ( )
{
  m_pOutputCtx   = NULL;
  m_pVideoStream = NULL;
  m_pAudioStream = NULL;
  m_pAudioBuffer = NULL;
  m_pVideoBuffer = NULL;
  m_pFrame       = NULL;
  m_pSamples     = NULL;
  m_pInputAudio  = NULL;
  m_iSampleSize  = 0;

  // Called automatically through avcodec_register, and avcodec_register_all
//#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 72, 2 )     //ooo
  #if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 123, 0 )    //xxx
  // must be called before using avcodec lib
  avcodec_init ( );
#endif 

  // register all the codecs
  avcodec_register_all ( );
  av_register_all      ( );
  av_log_set_level     ( AV_LOG_QUIET );
}

FFmpeg::~FFmpeg ( )
{
  if ( m_pVideoBuffer )
    delete []m_pVideoBuffer;
  m_pVideoBuffer = NULL;

  endStream ( );
}

void FFmpeg::lockEngine ( )
{
  FFmpeg::m_lock.lock ( );
}

void FFmpeg::unlockEngine ( )
{
  FFmpeg::m_lock.unlock ( );
}

bool FFmpeg::initStream ( QString qsFileName, enVideo videoFormat, enAudio audioFormat, uint iFrames )
{
  Encoder::initStream ( qsFileName, videoFormat, audioFormat, iFrames );
  endStream ( );
  AVOutputFormat *pOutputFormat = qrender_guess_format ( "dvd", NULL, NULL );
  if ( audioFormat == afUndef )
    pOutputFormat = qrender_guess_format ( NULL, (const char *)qsFileName.toUtf8 ( ), NULL );

  if ( ! pOutputFormat )
    return false;

  pOutputFormat->video_codec = CODEC_ID_MPEG2VIDEO;
  if ( audioFormat == afUndef )
    pOutputFormat->audio_codec = CODEC_ID_NONE;
  else if (audioFormat == afAC3 )
    pOutputFormat->audio_codec = CODEC_ID_AC3;
  else
    pOutputFormat->audio_codec = CODEC_ID_MP2;

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT ( 52, 27, 0 )
  m_pOutputCtx = av_alloc_format_context ( );
#else
  m_pOutputCtx = avformat_alloc_context ( );
#endif
  if ( ! m_pOutputCtx )
    return false;

  m_pOutputCtx->oformat = pOutputFormat;
  snprintf ( m_pOutputCtx->filename, sizeof ( m_pOutputCtx->filename ), "%s", (const char *)qsFileName.toUtf8 ( ) );

  // add video and audio streams
  if ( ! addVideoStream ( pOutputFormat->video_codec, videoFormat, m_iVideoBitrate ) )
    return false;
  if ( ! addAudioStream ( pOutputFormat->audio_codec ) )
    return false;

//#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 72, 2 ) //ooo
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 123, 0 )  //xxx
  if ( av_set_parameters ( m_pOutputCtx, NULL ) < 0 )
    return false;
#endif

  //dump_format ( m_pOutputCtx, 0, (const char*)qsFileName.toUtf8 ( ), 1 );
  m_pOutputCtx->packet_size = 2048;

  // open the audio and video codecs and allocate the necessary encode buffers
  if ( m_pVideoStream  )
    OpenVideoEncoder ( );
  if ( m_pAudioStream  )
    OpenAudioEncoder ( );

  // open the output file
  if ( qrender_url_fopen ( &m_pOutputCtx->pb, (const char *)qsFileName.toUtf8 ( ), QRENDER_URL_WRITE_ONLY ) < 0 )  {
    printf ( "Could not open '%s'\n", (const char *)qsFileName.toUtf8 ( ) );
    return false;
  }

  // write the stream header
  m_pOutputCtx->packet_size = 2048;

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 54, 0, 100 )
  m_pOutputCtx->mux_rate    = 10080000;
  m_pOutputCtx->preload     = (int)( 0.5*AV_TIME_BASE );
#endif
  m_pOutputCtx->max_delay   = (int)( 0.7*AV_TIME_BASE );

//#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 72, 2 )    //ooo
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT ( 52, 123, 0 )     //xxx
  av_write_header ( m_pOutputCtx );
#else
  avformat_write_header ( m_pOutputCtx, NULL );
#endif

  // The last step we init all input audio streams ...
  bool bOkay = false;
  for ( int t=0; t<m_audioList.count ( ); t++ )  {
    Audio *pAudio = new Audio ( m_audioList[t], m_iSampleSize, m_pAudioStream );
    m_listInputAudio.append   ( pAudio );
    if ( ! bOkay )  {
      bOkay = pAudio->openStream ( );
      if ( bOkay )
        m_pInputAudio = pAudio;
    }
  }

  return true;
}

/**
 * RGBtoYUV420P function is from Gnomemeeting
 */
#define rgbtoyuv(r, g, b, y, u, v) \
  y=(uint8_t)(((int)30*r   +(int)59*g +(int)11*b)/100); \
  u=(uint8_t)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
  v=(uint8_t)(((int)50*r   -(int)42*g -(int)8*b+12800)/100); \

void FFmpeg::RGBtoYUV420P ( const uint8_t *pRGB, uint8_t *pYUV, uint iRGBIncrement, bool bSwapRGB, int iWidth, int iHeight,  bool bFlip )
{
  const unsigned iPlaneSize = iWidth * iHeight;
  const unsigned iHalfWidth = iWidth >> 1;

  // get pointers to the data
  uint8_t *yplane  = pYUV;
  uint8_t *uplane  = pYUV + iPlaneSize;
  uint8_t *vplane  = pYUV + iPlaneSize + (iPlaneSize >> 2);
  const uint8_t *pRGBIndex = pRGB;
  int iRGBIdx[3];
  iRGBIdx[0] = 0;
  iRGBIdx[1] = 1;
  iRGBIdx[2] = 2;
  if ( bSwapRGB )  {
    iRGBIdx[0] = 2;
    iRGBIdx[2] = 0;
  }

  for (int y = 0; y < (int) iHeight; y++) {
    uint8_t *yline  = yplane + (y * iWidth);
    uint8_t *uline  = uplane + ((y >> 1) * iHalfWidth);
    uint8_t *vline  = vplane + ((y >> 1) * iHalfWidth);

    if ( bFlip ) // Flip horizontally
      pRGBIndex = pRGB + ( iWidth * ( iHeight -1 -y ) * iRGBIncrement );

    for ( int x=0; x<iWidth; x+=2 ) {
      rgbtoyuv ( pRGBIndex[iRGBIdx[0]], pRGBIndex[iRGBIdx[1]], pRGBIndex[iRGBIdx[2]], *yline, *uline, *vline );
      pRGBIndex += iRGBIncrement;
      yline++;
      rgbtoyuv ( pRGBIndex[iRGBIdx[0]], pRGBIndex[iRGBIdx[1]], pRGBIndex[iRGBIdx[2]], *yline, *uline, *vline );
      pRGBIndex += iRGBIncrement;
      yline++;
      uline++;
      vline++;
    }
  }
}

void FFmpeg::endStream ( )
{
  CloseVideoEncoder ( );
  CloseAudioEncoder ( );

  if ( ! m_pOutputCtx )
    return;

  // write the trailer
  av_write_trailer ( m_pOutputCtx );

  // free the streams
  unsigned int t;
  for  ( t=0; t<m_pOutputCtx->nb_streams; t++ )  {
    av_freep ( &m_pOutputCtx->streams[t]->codec );
    av_freep ( &m_pOutputCtx->streams[t] );
  }

  // close the output file
#if LIBAVFORMAT_VERSION_INT >= (52<<16)
  qrender_url_fclose (  m_pOutputCtx->pb );
#else
  qrender_url_fclose ( &m_pOutputCtx->pb );
#endif

  // free the stream
  av_free ( m_pOutputCtx );
  m_pOutputCtx = NULL;

  // Free input audio streams ...
  for ( int t=0; t<m_listInputAudio.count ( ); t++ )
    delete m_listInputAudio[t];
  m_listInputAudio.clear ( );
}

bool FFmpeg::addVideoStream ( int iCodecID, enVideo videoFormat, int iBitrate )
{
  if ( iCodecID == CODEC_ID_NONE )  {
    m_pVideoStream = NULL;
    return true;
  }
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 54, 0, 100 )
  m_pVideoStream = qrender_new_stream ( m_pOutputCtx, 0 );
#else
  m_pVideoStream = qrender_new_stream ( m_pOutputCtx, NULL );
#endif
  if ( ! m_pVideoStream )
    return false;

//  if ( m_threadCount > 1 )
//    avcodec_thread_init ( m_videoStm->codec, m_threadCount );

  float fAspectRatio =  4.0f / 3.0f;
  if ( m_AspectRatio == arAspect16to9 )
        fAspectRatio = 16.0f / 9.0f;

  AVCodecContext *pVideo  = m_pVideoStream->codec;
  pVideo->codec_id        = (CodecID)iCodecID;
  pVideo->codec_type      = QRENDER_TYPE_VIDEO;
  pVideo->bit_rate        = iBitrate * 1024;
  pVideo->width           = 720;
//pVideo->dtg_active_format = FF_DTG_AFD_4_3; only used for decoding
  if ( videoFormat == vfPAL )  {
    pVideo->height        = 576;
    pVideo->time_base.den =  25;
    pVideo->time_base.num =   1;
    pVideo->gop_size      =  15;
  }
  else  {
    pVideo->height        =   480;
    pVideo->time_base.den = 30000;
    pVideo->time_base.num =  1001;
    pVideo->gop_size      =    18;
  }

  pVideo->pix_fmt         = PIX_FMT_YUV420P;
  pVideo->rc_buffer_size  = VIDEO_BUF_SIZE;
  pVideo->rc_max_rate     = 9 * 1024 * 1024;
  pVideo->rc_min_rate     = 0;
  pVideo->sample_aspect_ratio = av_d2q ( fAspectRatio * pVideo->height / pVideo->width, 255 );
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(52,21,0)
  m_pVideoStream->sample_aspect_ratio = pVideo->sample_aspect_ratio;
#endif

  return true;
}

bool FFmpeg::OpenAudioEncoder ( )
{
  AVCodecContext *pAudio = m_pAudioStream->codec;

  // find the audio encoder and open it
//#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 72, 2 ) //ooo
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 52, 123, 0 )  //xxx
  AVCodec *pCodec = avcodec_find_encoder ( pAudio->codec_id );
#else
  AVCodec *pCodec = NULL;
  if ( pAudio->codec_id == CODEC_ID_AC3 ) // don't want the floating point format.
    pCodec = avcodec_find_encoder_by_name ( "ac3_fixed" );
  else
    pCodec = avcodec_find_encoder ( pAudio->codec_id );

#endif
  if ( ! pCodec )  {
    printf ( "Audio codec not found" );
    return false;
  }
  lockEngine ( );
  if ( qrender_avcodec_open ( pAudio, pCodec ) < 0 )  {
    unlockEngine ( );
    printf ("Could not open audio codec" );
    return false;
  }
  unlockEngine ( );

  m_pAudioBuffer = (uint8_t *) av_malloc ( AUDIO_BUF_SIZE );

  int iAudioInputFrameSize = pAudio->frame_size;
  // ugly hack for PCM codecs (will be removed ASAP with new PCM
  // support to compute the input frame size in samples
  if ( pAudio->frame_size <= 1 )  {
    iAudioInputFrameSize = AUDIO_BUF_SIZE / pAudio->channels;
    switch ( m_pAudioStream->codec->codec_id ) {
    case CODEC_ID_PCM_S16LE:
    case CODEC_ID_PCM_S16BE:
    case CODEC_ID_PCM_U16LE:
    case CODEC_ID_PCM_U16BE:
      iAudioInputFrameSize >>= 1;
    break;
    default:
      break;
    }
  }

  m_iSampleSize = iAudioInputFrameSize * 2 * pAudio->channels + FF_INPUT_BUFFER_PADDING_SIZE;
  m_pSamples    = (int16_t*) av_malloc ( m_iSampleSize );
  // Zero out the audio so we have silence ...
  memset ( m_pSamples, 0, m_iSampleSize );
  return true;
}

void FFmpeg::CloseAudioEncoder ( )
{
  if ( ( ! m_pAudioStream ) || ( ! m_pAudioStream->codec ) || ( ! m_pAudioStream->codec->codec ) )
    return;

  lockEngine    ( );
  avcodec_close ( m_pAudioStream->codec );
  unlockEngine  ( );

  if ( m_pSamples )
    av_free ( m_pSamples );
  m_pSamples = NULL;

  if ( m_pAudioBuffer )
    av_free ( m_pAudioBuffer );

  m_pAudioBuffer = NULL;
  m_pAudioStream = NULL;
}

bool FFmpeg::OpenVideoEncoder ( )
{
  AVCodecContext *pVideo = m_pVideoStream->codec;

  // find the video encoder and open it
  AVCodec *pCodec = avcodec_find_encoder ( pVideo->codec_id );
  if ( ! pCodec )  {
    printf ( "Video codec not found" );
    return false;
  }

  lockEngine ( );
  if ( qrender_avcodec_open ( pVideo, pCodec ) < 0 )  {
    unlockEngine ( );
    printf ( "Could not open video codec" );
    return false;
  }
  unlockEngine ( );

  m_pVideoBuffer = (uint8_t*) av_malloc ( VIDEO_BUF_SIZE );

  // allocate the encoded raw picture
  m_pFrame = m_vid.allocPicture ( m_pVideoBuffer, pVideo->pix_fmt, pVideo->width, pVideo->height );
  if ( ! m_pFrame )  {
    printf ( "Could not allocate picture" );
    return false;
  }

  // The following settings will prevent warning messages from FFmpeg
  float fMuxMaxDelay = 0.7f;
  //For svcd you might set it to:
  //mux_preload= (36000+3*1200) / 90000.0; //0.44
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 54, 0, 100 )
  float fMuxPreload  = 0.5f;
  m_pOutputCtx->preload   = (int)( fMuxPreload  * AV_TIME_BASE );
#endif
  m_pOutputCtx->max_delay = (int)( fMuxMaxDelay * AV_TIME_BASE );

  return true;
}

void FFmpeg::CloseVideoEncoder ( )
{
  if ( ( ! m_pVideoStream ) || ( ! m_pVideoStream->codec ) || ( ! m_pVideoStream->codec->codec ) )
    return;

  lockEngine    ( );
  avcodec_close ( m_pVideoStream->codec );
  unlockEngine  ( );

  if ( m_pFrame )  {
    if ( m_pFrame->data[0] )
      av_free ( m_pFrame->data[0] );
    av_free ( m_pFrame );
    m_pFrame = NULL;
  }

  if ( m_pVideoBuffer )
    av_free ( m_pVideoBuffer );
  m_pVideoBuffer = NULL;
  m_pVideoStream = NULL;
}

bool FFmpeg::addAudioStream ( int iCodecID )
{
  if ( iCodecID == CODEC_ID_NONE ) {
    m_pAudioStream = NULL;
    return true;
  }

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT ( 53, 61, 100 )
  m_pAudioStream = qrender_new_stream ( m_pOutputCtx, 1 );
#else
  m_pAudioStream = qrender_new_stream ( m_pOutputCtx, NULL );
#endif
  if ( ! m_pAudioStream )
    return false;

//  if ( m_threadCount > 1 )
//    avcodec_thread_init ( m_audioStm->codec, m_threadCount );

  AVCodecContext *pAudio = m_pAudioStream->codec;
  pAudio->codec_id       = (CodecID)iCodecID;
  pAudio->codec_type     = QRENDER_TYPE_AUDIO;
//  pAudio->request_sample_fmt = SAMPLE_FMT_S16;
  pAudio->sample_fmt     = QRENDER_SAMPLE_FMT_S16; // SAMPLE_FMT_FLT; defaults now to floating point AC3 what the frack ?
  pAudio->bit_rate       = 448000;
  pAudio->sample_rate    = 48000;
  pAudio->channels       = 2;

  pAudio->frame_number   = 0;
  pAudio->time_base.num  = 1;
  pAudio->time_base.den  = pAudio->sample_rate;
  m_pAudioStream->time_base.num = pAudio->time_base.num;
  m_pAudioStream->time_base.den = pAudio->time_base.den;

  return true;
}

void FFmpeg::nextInputAudio ( )
{
//  bool bFound = false;
  QList<Audio *>::iterator it = m_listInputAudio.begin ( );
  while  ( it != m_listInputAudio.end ( ) )  {
    if   ( m_pInputAudio == *it++ )  {
      if ( it != m_listInputAudio.end ( ) )  {
        m_pInputAudio->closeStream ( );
        m_pInputAudio = *it;
        m_pInputAudio->openStream  ( );
//        bFound = true;
        break; // out of the while loop
      }
    }
  }
//  if ( ! bFound )
//    m_pInputAudio = NULL;
}

FFmpeg::Buffer *FFmpeg::decodeAudio ( float fVolume )
{
  FFmpeg::Buffer *pBuf = NULL;
  if ( m_pInputAudio )  {
    bool bEOF;
    pBuf = m_pInputAudio->decodeAudio ( bEOF );
    if ( bEOF )
      nextInputAudio ( );
  }

  // Next we take care of the volume
  if ( pBuf )  {
    if ( ( fVolume >= 0.0f ) && ( fVolume < 1.0f ) )  {
      short *pBuffer = pBuf->pBuffer;
      for ( int t=0; t<pBuf->iSamplesPerChannel * pBuf->iChannels; t++ )  {
        *pBuffer = (short)(fVolume * *pBuffer );
         pBuffer++;
      }
    }
  }
  return pBuf;
}

bool FFmpeg::writeAudioFrame ( float fVolume )
{
  AVPacket pkt;
  av_init_packet ( &pkt );
  AVCodecContext *pAudioCodec = m_pAudioStream->codec;
  if ( ! pAudioCodec || ! pAudioCodec->codec )  {
    av_log ( NULL, AV_LOG_FATAL, "%s::%s  %d > No Audio Codec available.\n", __FILE__, __FUNCTION__, __LINE__ );
    return false;
  }

  if ( m_pInputAudio )  {
    Buffer *pSamples = decodeAudio ( fVolume );
    unsigned int iSize = m_pInputAudio->getTotalSamples ( );
    pkt.size = pAudioCodec->codec->encode ( pAudioCodec, m_pAudioBuffer, iSize, (void *)pSamples->pBuffer );  //ooo
    //pkt.size = pAudioCodec->codec->encode2 ( pAudioCodec, m_pPacket, m_pFrame, (int *)pSamples->pBuffer );      //xxx
  }
  else  {
    int   iSize   = m_iSampleSize * sizeof ( short );
    char *pBuffer = new char [ iSize ];
    memset ( pBuffer, 0, iSize );
    pkt.size = pAudioCodec->codec->encode ( pAudioCodec, m_pAudioBuffer, iSize, (void *)pBuffer );    //ooo
    //pkt.size = pAudioCodec->codec->encode2 ( pAudioCodec, m_pPacket, m_pFrame, (int *)pBuffer );      //xxx
    delete []pBuffer;
  }

  pAudioCodec->frame_number++;
  pkt.pts  = av_rescale_q ( pAudioCodec->coded_frame->pts, pAudioCodec->time_base, m_pAudioStream->time_base );
  pkt.flags       |= QRENDER_PKT_FLAG_KEY;
  pkt.stream_index = m_pAudioStream->index;
  pkt.data         = m_pAudioBuffer;

  // write the compressed frame in the media file
  if ( av_write_frame ( m_pOutputCtx, &pkt ) != 0 )  { // av_interleaved_write_frame
    printf ( "Error while writing audio frame" );
    return false;
  }

  return true;
}

bool FFmpeg::writeVideoFrame ( )
{
  AVCodecContext *pVideo = m_pVideoStream->codec;

  // encode the image
  int  out_size = avcodec_encode_video ( pVideo, m_pVideoBuffer, VIDEO_BUF_SIZE, m_pFrame );
  if ( out_size < 0 )
    return false;
  // if zero size, it means the image was buffered
  if ( out_size > 0 ) {
    AVPacket pkt;
    av_init_packet ( &pkt );

    pkt.pts= av_rescale_q ( pVideo->coded_frame->pts, pVideo->time_base, m_pVideoStream->time_base );
    if ( pVideo->coded_frame->key_frame )
      pkt.flags     |= QRENDER_PKT_FLAG_KEY;

    pkt.stream_index = m_pVideoStream->index;
    pkt.data         = m_pVideoBuffer;
    pkt.size         = out_size;

    // write the compressed frame in the media file
    int  ret = av_write_frame ( m_pOutputCtx, &pkt );
    if ( ret != 0 ) {
      printf ( "Error while writing video frame" );
      return false;
    }
  }
  return true;
}

bool FFmpeg::addImage ( QImage *pImage, int iFrames, float fVolume )
{
  if ( ( ! pImage ) || ( ! m_pFrame ) )
    return false;

  AVCodecContext *pVideo = m_pVideoStream->codec;

  // Allocate buffer for FFMPeg ...
  int iWidth, iHeight;
  int iSize = pImage->width  ( ) * pImage->height ( );
  uint8_t    *pBuffer = new uint8_t [ ( ( iSize * 3 ) / 2 ) + 100 ]; // 100 bytes extra buffer
  iWidth    = pImage->width  ( );
  iHeight   = pImage->height ( );

  m_pFrame->data[0] = pBuffer;
  m_pFrame->data[1] = m_pFrame->data[0] + iSize;
  m_pFrame->data[2] = m_pFrame->data[1] + iSize / 4;
  m_pFrame->linesize[0] = iWidth;
  m_pFrame->linesize[1] = iWidth / 2;
  m_pFrame->linesize[2] = m_pFrame->linesize[1];

  // Copy data over from the QImage. Convert from 32bitRGB to YUV420P
  RGBtoYUV420P ( pImage->bits ( ), pBuffer, pImage->depth ( ) / 8, true, iWidth, iHeight );

  double fDuration = ((double) m_pVideoStream->pts.val ) * m_pVideoStream->time_base.num / m_pVideoStream->time_base.den + ((double) iFrames) * pVideo->time_base.num / pVideo->time_base.den;

  while ( true )  {
    double fDeltaVideo = (double)m_pVideoStream->time_base.num / m_pVideoStream->time_base.den;
    double fAudioPts   = m_pAudioStream ? ((double) m_pAudioStream->pts.val) * m_pAudioStream->time_base.num / m_pAudioStream->time_base.den : 0.0;
    double fVideoPts = ((double) m_pVideoStream->pts.val) * fDeltaVideo;

    if ( ( ! m_pAudioStream || fAudioPts >= fDuration ) &&
         ( ! m_pVideoStream || fVideoPts >= fDuration ) )
      break;

    // write interleaved audio and video frames
    if ( m_pAudioStream  &&  ( fAudioPts < fVideoPts ))  {  //+ fDeltaVideo ) )  {
      if ( ! writeAudioFrame ( fVolume ) )
        return false;
    }
    else {
      if ( ! writeVideoFrame ( ) )
        return false;
    }
  }

  delete pBuffer;
  m_pFrame->data[0] = NULL;
  return true;
}

bool FFmpeg::addVid ( QString qsFileName, int iTargetWidth, int iTargetHeight, int iAspectRatio, int iPaddingColor, double fRotate )
{
  // iAspect=0 ==  4:3
  // iAspect=1 == 16:9

  // This function transcodes the VID into mpeg2 compatible material and adds it to the stream.
  // First cause of action is to ensure we have already an open stream with m_vid.
  if ( ! m_vid.isValidStream ( ) || ( m_vid.streamName ( ) != qsFileName ) )  {
    closeVid ( );
    initVid  ( qsFileName );
    if ( ! m_vid.isValidStream ( ) )  {
      closeVid ( ); // Error could not recover ...
      return false;
    }
  }
  m_vid.setSWScale ( iTargetWidth, iTargetHeight, iAspectRatio, iPaddingColor );

  double fTargetFrameRate = (double)m_pVideoStream->codec->time_base.den / m_pVideoStream->codec->time_base.num;
  double fSourceFrameRate = m_vid.getFrameRate ( ); //10.0;
  double fAdjustFrameRate =  0.0;
  int iAdjustFrameRate = 0;

  // At this point we are all set to start the transcoding of the input audio / video.
  AVPacket *pPacket = m_vid.getNextPacket ( m_pVideoStream, m_pAudioStream, iAdjustFrameRate );
  while  (  pPacket )  {
    // write the compressed frame in the media file
    if ( fRotate != 0.0 )  {
// VAROL
    }

    //int  ret = av_interleaved_write_frame ( m_pOutputCtx, pPacket );
    int  ret = av_write_frame ( m_pOutputCtx, pPacket );
    if ( ret != 0 ) {
      closeVid  ( );
      printf    ( "Error while writing frame\n" );
      return false;
    }

    if ( iAdjustFrameRate == 0 )  {
         // Last frame was taken from the stream. Lets see if we have to insert
         // some extra frame to adjust the frame rate.
         if ( fTargetFrameRate  / fSourceFrameRate < 1.0 )
              fAdjustFrameRate -= fSourceFrameRate / fTargetFrameRate - 1.0; // E.g. 29.97 / 25 = NTSC to PAL = -0.2
         else
              fAdjustFrameRate += fTargetFrameRate / fSourceFrameRate - 1.0; // E.g. 29.97 / 10 = AVI to NTSC =  1.997
         iAdjustFrameRate  = (int)fAdjustFrameRate;
    }
    if ( fAdjustFrameRate >= 1.0 )
         fAdjustFrameRate -= 1.0;
    else if ( fAdjustFrameRate <= -1.0 )
              fAdjustFrameRate +=  1.0;
    else if ( iAdjustFrameRate == 1 )// Between [ -1.0 .. 1.0 ]
              iAdjustFrameRate --;
    pPacket = m_vid.getNextPacket ( m_pVideoStream, m_pAudioStream, iAdjustFrameRate );
    // iAdjusFrame is decremented by one each time a frame is re-used
    // iAdjusrtFrame is set to 0 when a new frame is taken from the stream.
  }
  closeVid ( );
  return true;
}

bool FFmpeg::initVid ( QString qsFileName )
{
  bool bRet = m_vid.openStream ( qsFileName );
  m_vid.setOutputContext ( m_pVideoStream->codec, m_pAudioStream->codec );
  return bRet;
}

void FFmpeg::closeVid ( )
{
  m_vid.closeStream ( );
}

QImage &FFmpeg::getVidFrame ( double fPosition )
{
  return m_vid.getFrame ( fPosition );
}

}; // end of namespace Encoder

