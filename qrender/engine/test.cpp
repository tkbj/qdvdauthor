
#include <strings.h>
#include <unistd.h>
#include <getopt.h>

#include <QApplication>
#include <QImage>

extern "C" {
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <libavformat/avformat.h>
}

#define AUDIO_BUF_SIZE 192 * 1024 * 4
#define VIDEO_BUF_SIZE 1835008
#define VIDEO_BITRATE  6000 * 1024

#define INPUT_AUDIO_FILE  "what.mp3"
#define INPUT_IMAGE_FILE  "img.jpg"
#define OUTPUT_VIDEO_FILE "out.vob"

std::string g_strInputAudioFile  = INPUT_AUDIO_FILE;
std::string g_strInputImageFile  = INPUT_IMAGE_FILE;
std::string g_strOutputVideoFile = OUTPUT_VIDEO_FILE;

struct AudioCtx {
  const char      *pFileName;
  AVFormatContext *pFormatCtx;
  AVCodecContext  *pCodecCtx;
  ReSampleContext *pResampleCtx;

  short           *pBuffer;
  short           *pResampled;
  short           *pRemainingSamples;
  int              iAudioStreamIdx;
  int              iSamples;
  int              iRemainingSamples;
};

struct OutputCtx {
  const char      *pFileName;
  AVFormatContext *pOutputCtx;
  AVStream        *pVideoStream;
  AVStream        *pAudioStream;
  AVFrame         *pFrame;

  short           *pAudioBuffer;
  short           *pVideoBuffer;
};

void help ( )
{
  printf ( "Test program for mixing audio / image files into a standards compliant VOB stream.\n" );
  printf ( "This program has the following input parameters :\n\n" );
  printf ( " -h --help         : Prints this help page and exists.\n" );
  printf ( " -i --image <FILE> : Specify the image to use for this video.\n" );
  printf ( " -a --audio <FILE> : Specify the audio file to use for this video.\n" );
  printf ( " -o --out   <FILE> : Specify the output video file.\n" );
  printf ( "\n" );
  exit   (   0  );
}

void parse ( int iArgs, char *pArguments[] )
{
  int iChar = NULL;

  static struct option longOpts[] =  {
    { "help",    no_argument,       0, 'h' },
    { "audio",   required_argument, 0, 'a' },
    { "image",   required_argument, 0, 'i' },
    { "out",     required_argument, 0, 'o' },
    { 0, 0, 0, 0 } 
  };

  while ( true ) {
    iChar = getopt_long_only ( iArgs, pArguments, "ha:i:o:", longOpts, NULL );
    if ( iChar == -1 )
      break;

    switch ( iChar ) {
      case 'h':
        help ( );
      break;
      case 'a':
        g_strInputAudioFile = optarg;
      break;
      case 'i':
        g_strInputImageFile = optarg;
      break;
      case 'o':
        g_strOutputVideoFile = optarg;
      break;
    }
  }
}

bool openAudioStream ( AudioCtx *pAudioCtx )
{
  // InputAudioStream ...
  int iAudioStream;
  AVCodec *pCodec;

  // Open audio file
  if ( av_open_input_file ( &pAudioCtx->pFormatCtx, pAudioCtx->pFileName, NULL, 0, NULL ) !=0 )
    return false; // Couldn't open file

  // Retrieve stream information
  if ( av_find_stream_info ( pAudioCtx->pFormatCtx ) < 0 )
    return false; // Couldn't find stream information

  // Find the first video stream
  bool bHasAudio = false;
  for  ( iAudioStream=0; iAudioStream < (int)pAudioCtx->pFormatCtx->nb_streams; iAudioStream++ )
    if ( pAudioCtx->pFormatCtx->streams[iAudioStream]->codec->codec_type == CODEC_TYPE_AUDIO ) {
      bHasAudio = true;
      break;
  }
  if ( ! bHasAudio )
    return false; // Didn't find an audio stream

  // Get a pointer to the codec context for the audio stream
  pAudioCtx->pCodecCtx = pAudioCtx->pFormatCtx->streams[iAudioStream]->codec;
  pAudioCtx->iAudioStreamIdx = iAudioStream;

  // Find the decoder for the audio stream
  pCodec = avcodec_find_decoder ( pAudioCtx->pCodecCtx->codec_id );
  if ( ! pCodec ) {
    fprintf ( stderr, "Unsupported codec!\n");
    return false; // Codec not found
  }

  // Open codec
  if ( avcodec_open ( pAudioCtx->pCodecCtx, pCodec ) < 0 )
    return false; // Could not open codec

printf ( "%s : %d : streamName<%s>\n", __FUNCTION__, __LINE__, pAudioCtx->pFileName );

  return true;
}

bool addVideoStream ( OutputCtx *pOutput )
{
  pOutput->pVideoStream = av_new_stream ( pOutput->pOutputCtx, 0 );
  if ( ! pOutput->pVideoStream )
    return false;

  AVCodecContext *pVideo  = pOutput->pVideoStream->codec;
  pVideo->codec_id        = (CodecID)pOutput->pOutputCtx->oformat->video_codec;
  pVideo->codec_type      = CODEC_TYPE_VIDEO;
  pVideo->bit_rate        = VIDEO_BITRATE;
  pVideo->sample_aspect_ratio.den = 4;
  pVideo->sample_aspect_ratio.num = 3;
//  pVideo->dtg_active_format = FF_DTG_AFD_4_3; only used for decoding
  pVideo->width           =   720;
  pVideo->height          =   480;
  pVideo->time_base.den   = 30000;
  pVideo->time_base.num   =  1001;
  pVideo->gop_size        =    18;

  pVideo->pix_fmt         = PIX_FMT_YUV420P;
  pVideo->rc_buffer_size  = VIDEO_BUF_SIZE;
  pVideo->rc_max_rate     = 9 * 1024 * 1024;
  pVideo->rc_min_rate     = 0;

  return true;
}

bool addAudioStream ( OutputCtx *pOutput )
{
  pOutput->pAudioStream = av_new_stream ( pOutput->pOutputCtx, 1 );
  if ( ! pOutput->pAudioStream )
    return false;

  AVCodecContext *pAudio = pOutput->pAudioStream->codec;
  pAudio->codec_id       = (CodecID)pOutput->pOutputCtx->oformat->audio_codec;
  pAudio->codec_type     = CODEC_TYPE_AUDIO;
  pAudio->bit_rate       = 448000;
  pAudio->sample_rate    = 48000;
  pAudio->channels       = 2;
  pAudio->time_base.num  = 1;
  pAudio->time_base.den  = pAudio->sample_rate;
  pAudio->rc_buffer_size = AUDIO_BUF_SIZE;

  return true;
}

AVFrame *allocPicture ( int iPixFormat, int iWidth, int iHeight )
{
  AVFrame *pFrame = avcodec_alloc_frame ( );
  if (  !  pFrame )
    return NULL;

  int iSize = avpicture_get_size ( iPixFormat, iWidth, iHeight );
  uint8_t *pBuffer = (uint8_t*) av_malloc ( iSize );
  if (  !  pBuffer )  {
      av_free ( pFrame );
      return NULL;
  }

  avpicture_fill ( (AVPicture *)pFrame, pBuffer, iPixFormat, iWidth, iHeight );
  return pFrame;
}

bool openVideoEncoder ( OutputCtx *pOutput )
{
  AVCodecContext *pVideo = pOutput->pVideoStream->codec;

  // find the video encoder and open it
  AVCodec *pCodec = avcodec_find_encoder ( pVideo->codec_id );
  if ( ! pCodec )  {
    printf ( "Video codec not found" );
    return false;
  }

  if ( avcodec_open ( pVideo, pCodec ) < 0 )  {
    printf ( "Could not open video codec" );
    return false;
  }

  pOutput->pVideoBuffer = (short *) av_malloc ( VIDEO_BUF_SIZE );

  // allocate the encoded raw picture
  pOutput->pFrame = allocPicture ( pVideo->pix_fmt, pVideo->width, pVideo->height );
  if ( ! pOutput->pFrame )  {
    printf ( "Could not allocate picture" );
    return false;
  }

  // The following settings will prevent warning messages from FFmpeg
  float fMuxPreload  = 0.5f;
  float fMuxMaxDelay = 0.7f;
  //For svcd you might set it to:
  //mux_preload= (36000+3*1200) / 90000.0; //0.44
  pOutput->pOutputCtx->preload   = (int)( fMuxPreload  * AV_TIME_BASE );
  pOutput->pOutputCtx->max_delay = (int)( fMuxMaxDelay * AV_TIME_BASE );

  return true;
}

bool openAudioEncoder ( OutputCtx *pOutput )
{
  AVCodecContext *pAudio = pOutput->pAudioStream->codec;

  // find the audio encoder and open it
  AVCodec *pCodec = avcodec_find_encoder ( pAudio->codec_id );
  if ( ! pCodec )  {
    printf ( "Audio codec not found" );
    return false;
  }
  if ( avcodec_open ( pAudio, pCodec ) < 0 )  {
    printf ("Could not open audio codec" );
    return false;
  }

  pOutput->pAudioBuffer = (short *) av_malloc ( AUDIO_BUF_SIZE );
  return true;
}

bool initOutputStream ( OutputCtx *pOutput )
{
  AVOutputFormat *pOutputFormat = guess_format ( "dvd", NULL, NULL );
  if ( ! pOutputFormat )
    return false;

  pOutputFormat->video_codec = CODEC_ID_MPEG2VIDEO;
  pOutputFormat->audio_codec = CODEC_ID_AC3;

  pOutput->pOutputCtx = av_alloc_format_context ( );
  if ( ! pOutput->pOutputCtx )
    return false;

  pOutput->pOutputCtx->oformat  = pOutputFormat;
  snprintf ( pOutput->pOutputCtx->filename, sizeof ( pOutput->pOutputCtx->filename ), "%s", pOutput->pFileName );

  // add video and audio streams
  if ( ! addVideoStream ( pOutput ) )
    return false;
  if ( ! addAudioStream ( pOutput ) )
    return false;

  if ( av_set_parameters ( pOutput->pOutputCtx, NULL ) < 0 )
    return false;

  pOutput->pOutputCtx->packet_size = 2048;
  dump_format ( pOutput->pOutputCtx, 0, g_strOutputVideoFile.c_str ( ), 1 );

  // open the audio and video codecs and allocate the necessary encode buffers
  if ( ! openVideoEncoder ( pOutput ) )
    return false;
  if ( ! openAudioEncoder ( pOutput ) )
    return false;

  // open the output file
  if ( url_fopen  (  &pOutput->pOutputCtx->pb, g_strOutputVideoFile.c_str ( ), URL_WRONLY ) < 0 )  {
    printf ( "Could not open '%s' for writing\n", g_strOutputVideoFile.c_str ( ) );
    return false;
  }

  // write the stream header
  pOutput->pOutputCtx->packet_size = 2048;
  pOutput->pOutputCtx->mux_rate    = 10080000;
  av_write_header ( pOutput->pOutputCtx );

  return true;
}

void closeVideoEncoder ( OutputCtx *pOutput )
{
  if ( ! pOutput->pVideoStream )
    return;

  avcodec_close ( pOutput->pVideoStream->codec );

  if ( pOutput->pFrame )  {
    if ( pOutput->pFrame->data[0] )
      av_free ( pOutput->pFrame->data[0] );
    av_free ( pOutput->pFrame );
    pOutput->pFrame = NULL;
  }

  if ( pOutput->pVideoBuffer )
    av_free ( pOutput->pVideoBuffer );
  pOutput->pVideoBuffer = NULL;
  pOutput->pVideoStream = NULL;
}

void closeAudioEncoder ( OutputCtx *pOutput )
{
  if ( ! pOutput->pAudioStream )
    return;
  avcodec_close ( pOutput->pAudioStream->codec );

  if ( pOutput->pAudioBuffer )
    av_free ( pOutput->pAudioBuffer );

  pOutput->pAudioBuffer = NULL;
  pOutput->pAudioStream = NULL;
}

void endOutputStream ( OutputCtx *pOutput )
{
  closeVideoEncoder ( pOutput );
  closeAudioEncoder ( pOutput );

  if ( ! pOutput->pOutputCtx )
    return;

  // write the trailer
  av_write_trailer ( pOutput->pOutputCtx );

  // free the streams
  unsigned int t;
  for  ( t=0; t<pOutput->pOutputCtx->nb_streams; t++ )  {
    av_freep ( &pOutput->pOutputCtx->streams[t]->codec );
    av_freep ( &pOutput->pOutputCtx->streams[t] );
  }

  // close the output file
#if LIBAVFORMAT_VERSION_INT >= (52<<16)
  url_fclose (  pOutput->pOutputCtx->pb );
#else
  url_fclose ( &pOutput->pOutputCtx->pb );
#endif

  // free the stream
  av_free ( pOutput->pOutputCtx );
  pOutput->pOutputCtx = NULL;
}

/**
 * RGBtoYUV420P function is from Gnomemeeting
 */
#define rgbtoyuv(r, g, b, y, u, v) \
  y=(uint8_t)(((int)30*r   +(int)59*g +(int)11*b)/100); \
  u=(uint8_t)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
  v=(uint8_t)(((int)50*r   -(int)42*g -(int)8*b+12800)/100);
void RGBtoYUV420P ( const uint8_t *pRGB, uint8_t *pYUV, uint iRGBIncrement, bool bSwapRGB, int iWidth, int iHeight,  bool bFlip )
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

bool writeVideoFrame ( OutputCtx *pOutput )
{
  AVCodecContext *pVideo = pOutput->pVideoStream->codec;

  // encode the image
  int  out_size = avcodec_encode_video ( pVideo, (uint8_t *)pOutput->pVideoBuffer, VIDEO_BUF_SIZE, pOutput->pFrame );
  if ( out_size < 0 )
    return false;
  // if zero size, it means the image was buffered
  if ( out_size > 0 ) {
    AVPacket pkt;
    av_init_packet ( &pkt );

    pkt.pts= av_rescale_q ( pVideo->coded_frame->pts, pVideo->time_base, pOutput->pVideoStream->time_base );
    if ( pVideo->coded_frame->key_frame )
      pkt.flags     |= PKT_FLAG_KEY;

    pkt.stream_index = pOutput->pVideoStream->index;
    pkt.data         = (uint8_t *)pOutput->pVideoBuffer;
    pkt.size         = out_size;

    // write the compressed frame in the media file
    int  ret = av_write_frame ( pOutput->pOutputCtx, &pkt );
    if ( ret != 0 ) {
      printf ( "Error while writing video frame" );
      return false;
    }
  }
  return true;
}

void printBuf ( short *pBuffer, int iSize )
{
  int t;
  for ( t=0; t<iSize; t++ )  {
    printf ( "[%.04X]", (unsigned short) pBuffer[t] );
  }
  printf ( "\n TOTAL = %d\n", iSize );
}

short *decode ( AudioCtx *pAudio )
{
  AVPacket packet;
  unsigned int iOffset = 0;  // in Bytes
  int  iRet  = av_read_frame ( pAudio->pFormatCtx, &packet );
  if ( iRet == 0 )  {
    int      iPacketSize = packet.size;
    uint8_t *pPacketData = packet.data;
    // Is this a packet from the audio stream ?
    if  ( packet.stream_index == pAudio->iAudioStreamIdx )  {
      int16_t *pDecodedAudio = NULL;
      int      iDecodedSize, iLength;
      pDecodedAudio = (int16_t*) av_malloc ( AUDIO_BUF_SIZE );
      while  ( iPacketSize > 0 )  {
        iDecodedSize  = AUDIO_BUF_SIZE;
        memset ( pDecodedAudio, 0, AUDIO_BUF_SIZE );

        iLength = avcodec_decode_audio2 ( pAudio->pCodecCtx, pDecodedAudio, &iDecodedSize, pPacketData, iPacketSize );
        if ( iLength <= 0 )  { // Error, see if we can recover.
          iPacketSize --;
          pPacketData ++;
        }
        else  {
          iPacketSize -= iLength;
          pPacketData += iLength;

          memcpy ( (uint8_t*)(pAudio->pBuffer+iOffset), pDecodedAudio, iDecodedSize );
          iOffset += iDecodedSize;
        }
      }
      av_free ( pDecodedAudio );
    }
    if ( packet.data )
      av_free_packet ( &packet );
  }

  if ( iOffset == 0 )  {
    // Invalid package ... wtf.
    memset ( pAudio->pBuffer, 0, AUDIO_BUF_SIZE );
    pAudio->iSamples = 0;
    return NULL;
  }

  // Calculate the number of samples in the buffer
  // Note this is the total number, so for stereo the format is LRLRLRLR
  pAudio->iSamples = (unsigned int)iOffset / sizeof ( short );
  return pAudio->pBuffer;
}

short *resample ( AudioCtx *pAudio )
{
  if ( pAudio->iSamples < 1 )
    return pAudio->pBuffer;

  int iSamples = pAudio->iSamples / pAudio->pCodecCtx->channels;
  pAudio->iSamples  = audio_resample ( pAudio->pResampleCtx, pAudio->pResampled, pAudio->pBuffer, iSamples );

  return pAudio->pResampled;
}

short *decodeAudio ( OutputCtx *pOutput, AudioCtx *pAudio )
{
  short *pSamples    = NULL;
  int    iSamples    = 0;
  int    iChannels   = pOutput->pAudioStream->codec->channels;
  int    iInChannels = pAudio->pCodecCtx->channels;
  int    iSampleSize = sizeof ( short );
  int    iFrameSize  = pOutput->pAudioStream->codec->frame_size * iChannels;
  // Swap buffer to work with
  char   *pBuffer    = (char  *)pAudio->pResampled;
  pAudio->pResampled = (short *)av_malloc ( AUDIO_BUF_SIZE );

  memset ( pBuffer, 0, AUDIO_BUF_SIZE );

  // Lets check if we have leftover from the previous decoding run
  if ( pAudio->iRemainingSamples > 0 )  {
    iSamples = pAudio->iRemainingSamples;
    if ( iSamples > iFrameSize )
         iSamples = iFrameSize;
    memcpy ( pBuffer, (char *)pAudio->pRemainingSamples, iSamples * iSampleSize );

    pAudio->iRemainingSamples -= iSamples;
    if ( pAudio->iRemainingSamples > 0 )  {
      memcpy ( (char *)pAudio->pRemainingSamples, ((char *)pAudio->pRemainingSamples + iSamples*iSampleSize), pAudio->iRemainingSamples );
    }
  }

  // Main Loop until we have enough samples for this frame.
  while ( iSamples  < iFrameSize )  {
    pSamples = decode   ( pAudio );
    pSamples = resample ( pAudio );
    // important, audio_resample returns the number of resampled samples. We need the total number of samples here
    // I.e. if input is mono and output is stereo, then we need to multiply by 2.
    pAudio->iSamples *= iChannels;

    if ( pAudio->iSamples == 0 )
      break;
    memcpy ( (pBuffer + iSamples*iSampleSize), (char *)pSamples, pAudio->iSamples * iSampleSize );

    iSamples += pAudio->iSamples;
  }

  // Next we store the remaining samples in a temp buffer
  if ( iSamples - iFrameSize > 0 )  {
    pAudio->iRemainingSamples = iSamples - iFrameSize;
    memcpy ( (char *)pAudio->pRemainingSamples, (pBuffer + iFrameSize * iSampleSize ), pAudio->iRemainingSamples * iSampleSize );
  }
  else  {
    pAudio->iRemainingSamples = -1;
    iFrameSize -= iSamples;
  }

  av_free ( pAudio->pResampled );
  pAudio->pResampled = (short *)pBuffer;
  pAudio->iSamples   = iFrameSize;

  return pAudio->pResampled;
}

bool writeAudioFrame ( OutputCtx *pOutput, AudioCtx *pAudio )
{
  AVPacket pkt;
  av_init_packet ( &pkt );
  AVCodecContext *pAudioCodec = pOutput->pAudioStream->codec;

  short *pSamples = decodeAudio ( pOutput, pAudio );

  pkt.size   = avcodec_encode_audio ( pAudioCodec, (uint8_t *)pOutput->pAudioBuffer, AUDIO_BUF_SIZE, pSamples );
  pkt.pts    = av_rescale_q ( pAudioCodec->coded_frame->pts, pAudioCodec->time_base, pOutput->pAudioStream->time_base );
  pkt.flags |= PKT_FLAG_KEY;
  pkt.data   = (uint8_t *)pOutput->pAudioBuffer;
  pkt.stream_index = pOutput->pAudioStream->index;

  // write the compressed frame in the media file
  if ( av_write_frame ( pOutput->pOutputCtx, &pkt ) != 0 )  { // av_interleaved_write_frame
    printf ( "Error while writing audio frame" );
    return false;
  }

  return true;
}

bool addImage ( OutputCtx *pOutput, AudioCtx *pAudio, QImage *pImage, int iFrames )
{
  if ( ! pImage )
    return false;

  AVCodecContext *pVideo = pOutput->pVideoStream->codec;

  // Allocate buffer for FFMPeg ...
  int iWidth, iHeight;
  int iSize = pImage->width ( ) * pImage->height ( );
  uint8_t *pBuffer = new uint8_t [ ( ( iSize * 3 ) / 2 ) + 100 ]; // 100 bytes extra buffer
  iWidth  = pImage->width  ( );
  iHeight = pImage->height ( );

  pOutput->pFrame->data[0] = pBuffer;
  pOutput->pFrame->data[1] = pOutput->pFrame->data[0] + iSize;
  pOutput->pFrame->data[2] = pOutput->pFrame->data[1] + iSize / 4;
  pOutput->pFrame->linesize[0] = iWidth;
  pOutput->pFrame->linesize[1] = iWidth / 2;
  pOutput->pFrame->linesize[2] = pOutput->pFrame->linesize[1];

  // Copy data over from the QImage. Convert from 32bitRGB to YUV420P
  RGBtoYUV420P ( pImage->bits ( ), pBuffer, pImage->depth ( ) / 8, true, iWidth, iHeight, false );

  double fDuration = ((double) pOutput->pVideoStream->pts.val ) * pOutput->pVideoStream->time_base.num / pOutput->pVideoStream->time_base.den + ((double) iFrames) * pVideo->time_base.num / pVideo->time_base.den;

  while ( true )  {
    double fDeltaVideo =  (double)pOutput->pVideoStream->time_base.num / pOutput->pVideoStream->time_base.den;
    double fAudioPts   = pOutput->pAudioStream ? ((double) pOutput->pAudioStream->pts.val) * pOutput->pAudioStream->time_base.num / pOutput->pAudioStream->time_base.den : 0.0;
    double fVideoPts   = ((double) pOutput->pVideoStream->pts.val) * fDeltaVideo;

    if ( ( ! pOutput->pAudioStream || fAudioPts >= fDuration ) &&
         ( ! pOutput->pVideoStream || fVideoPts >= fDuration ) )
      break;

    // write interleaved audio and video frames
    if ( pOutput->pAudioStream && ( fAudioPts < fVideoPts + fDeltaVideo ) )  {
      if ( ! writeAudioFrame ( pOutput, pAudio ) )
        return false;
    }
    else {
      if ( ! writeVideoFrame ( pOutput ) )
        return false;
    }
  }

  delete pBuffer;
  pOutput->pFrame->data[0] = NULL;
  return true;
}

int main ( int argc, char *argv[] )
{
  // Init Qt libraries.
  QApplication theApp ( argc, argv );
  parse ( argc, argv );

  // must be called before using avcodec lib
  avcodec_init ( );

  // register all the codecs
  avcodec_register_all ( );
  av_register_all ( );

  // Testing slideshow generation 
  AudioCtx inputAudio;
  inputAudio.pFileName         = g_strInputAudioFile.c_str ( );
  inputAudio.pBuffer           = (short *) av_malloc ( AUDIO_BUF_SIZE );
  inputAudio.pResampled        = (short *) av_malloc ( AUDIO_BUF_SIZE );
  inputAudio.pRemainingSamples = (short *) av_malloc ( AUDIO_BUF_SIZE );
  inputAudio.iSamples          = 0;
  inputAudio.iRemainingSamples = 0;
  inputAudio.pResampleCtx      = NULL;
  inputAudio.pFormatCtx        = NULL;
  inputAudio.pCodecCtx         = NULL;

  if ( ! openAudioStream ( &inputAudio ) )
    return -1;

  OutputCtx outputVideo;
  outputVideo.pFileName = g_strOutputVideoFile.c_str ( );

  if ( ! initOutputStream ( &outputVideo ) )
    return -1;

  inputAudio.pResampleCtx = audio_resample_init ( 2, inputAudio.pCodecCtx->channels, 48000, inputAudio.pCodecCtx->sample_rate );

  QImage theImage ( g_strInputImageFile.c_str ( ) );
  if ( theImage.isNull ( ) )  {
    fprintf ( stderr, "Could not open Image file <%s>\n", g_strInputImageFile.c_str ( ) );
    return -1;
  }

  int iFrames = (int)( 5.0 * 29.97 ); // 5 second video.

  addImage ( &outputVideo, &inputAudio, &theImage, iFrames );

  endOutputStream ( &outputVideo );

  av_free ( inputAudio.pBuffer           );
  av_free ( inputAudio.pResampled        );
  av_free ( inputAudio.pRemainingSamples );
}

