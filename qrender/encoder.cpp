/***************************************************************************
    encoder.cpp
                             -------------------
    Class Utils implementation ...
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    Here we have some misc functions which are needed by a few classes 
    but can not really be assigned to any of those classes.
    
****************************************************************************/

#include "encoder.h"

namespace Encoder
{

Encoder::Encoder ( )
{
  m_VideoFormat   = vfNTSC;
  m_AudioFormat   = afAC3;
  m_AspectRatio   = arAspect4to3;
  m_iVideoBitrate = 6000;
  m_iAudioBitrate = 224000;
  m_iAudioSample  = 48000;
  m_iTotalFrames  = 1;
}

Encoder::~Encoder ( )
{
}

bool Encoder::initStream ( QString qsFileName, enVideo videoFormat, enAudio audioFormat, uint iFrames )
{
  m_qsFileName   = qsFileName;
  m_VideoFormat  = videoFormat;
  m_AudioFormat  = audioFormat;
  m_iTotalFrames = iFrames;
  return true;
}

void Encoder::endStream ( )
{
}

void Encoder::setVideoParam ( uint iBitrate, enAspect aspect )
{
  m_iVideoBitrate = iBitrate;
  m_AspectRatio   = aspect;
}

void Encoder::setAudioParam ( uint iBitrate, int iSample ) // 192000, 4800
{
  m_iAudioBitrate = iBitrate;
  m_iAudioSample  = iSample;
}

QString &Encoder::getFileName   ( )
{
  return m_qsFileName;
}

void Encoder::setAudioList ( QStringList list )
{
  m_audioList = list;
}

}; // end of namespace Encoder
