/***************************************************************************
    slidedefaults.cpp
                             -------------------
    MenuPreview class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file holds the implementation for the slideshow defaults which are
   set in DialogSetup
    
****************************************************************************/

#include <stdlib.h>
#include <time.h>

#include <qfileinfo.h>
#include <qdom.h>
#include <qdir.h>
#include <QMessageBox>

#include "global.h"
#include "qdvdauthor.h"
#include "xml_slideshow.h"
#include "slidedefaults.h"

SlideDefaults::SlideDefaults ( )
{
  srand  ( time ( NULL ) );
  initMe ( );
}

SlideDefaults::~SlideDefaults ( )
{
}

void SlideDefaults::initMe ( )
{
  srand ( time ( NULL ) );
  m_fImageDelay       = 5.0f;
  m_fFilterDelay      = 3.0f;
  m_iFormatType       = 0; // NTSC(4:3)[0] or PAL[1](4:3) NTSC(16:9)[2] or PAL[3](16:9)
  m_iTransitionType   = 1; // ImgImg[0] or ImgBckgImg[1]
  m_bCreateIntroSlide = true;
  m_bKenBurns         = true;
}

SlideDefaults &SlideDefaults::operator = ( const SlideDefaults &theOther )
{
  m_fImageDelay       = theOther.m_fImageDelay;
  m_fFilterDelay      = theOther.m_fFilterDelay;
  m_iFormatType       = theOther.m_iFormatType;
  m_iTransitionType   = theOther.m_iTransitionType;
  m_bCreateIntroSlide = theOther.m_bCreateIntroSlide;
  m_bKenBurns         = theOther.m_bKenBurns;
  m_listBackground    = theOther.m_listBackground;
  m_listAudio         = theOther.m_listAudio;
  m_listTransitions   = theOther.m_listTransitions;
  return *this;
}

void SlideDefaults::initSlideshow ( CXmlSlideshow *pSlideshow )
{  
  if ( ! pSlideshow )
    return;

  int t, iRandomAudioTracks = 5, iMaxIssues = 0;
  QStringList listAudio, tempList;
  QString qsAudio, qsAudioFilter, qsBackground ( "#000000" );
  qsAudioFilter = Global::pApp->getSoundFilter ( );

  if ( backgroundList ( ).size ( ) > 0 ) {
    //int iIdx = (int)( ( (float) rand ( ) / RAND_MAX ) * backgroundList ( ).size ( ) );
    int iIdx = rand ( )%backgroundList ( ).size ( );
    qsBackground     =  backgroundList ( )[iIdx];
  }

  tempList = audioList ( );

  if ( tempList.size ( ) > 0 )  {
    for ( t=0; t<iRandomAudioTracks; t++ )  {
      if ( tempList.size  ( ) < 1 )
           tempList = audioList ( );

      if ( iMaxIssues > 20 )
        break;

      //int iIdx = (int)( ( (float) rand ( ) / RAND_MAX ) * tempList.size ( ) );
      int iIdx = rand ( )%tempList.size ( );
      QString qsAudio = tempList[iIdx];
      //tempList.remove ( qsAudio );	//ooo
      tempList.removeOne ( qsAudio );	//xxx
      // Next we check if we found an audio file or a directory.
      QFileInfo fileInfo ( qsAudio );
      if ( fileInfo.isDir ( ) )  {
        QDir theDir ( qsAudio );
        theDir.setFilter      ( QDir::Files | QDir::Readable );
	QStringList qsAudioFilter_ ( qsAudioFilter );				//xxx
        //theDir.setNameFilter  ( qsAudioFilter );				//ooo
	theDir.setNameFilters  ( qsAudioFilter_ );				//xxx
        //QFileInfoList *pList = (QFileInfoList*)theDir.entryInfoList  ( );	//ooo
	QList<QFileInfo> pList = theDir.entryInfoList  ( );			//xxx
        //iIdx = (int)( ( (float) rand ( ) / RAND_MAX ) * pList->count ( ) );	//ooo
	iIdx = (int)( ( (float) rand ( ) / RAND_MAX ) * pList.count ( ) );	//xxx
        //qsAudio = pList->at ( iIdx )->absFilePath ( );			//ooo
	qsAudio = pList.at ( iIdx ).absoluteFilePath ( );			//xxx
        // found an issue with this file.
        if ( qsAudio.isEmpty ( ) )  {
          t --;
          iMaxIssues++;
        }
      }
      listAudio.append ( qsAudio );
    }
  }

  pSlideshow->delay             = imageDelay     ( );
  pSlideshow->filter_delay      = filterDelay    ( );
  pSlideshow->background        = qsBackground;
  pSlideshow->audio_list        = listAudio;
  pSlideshow->validFilterNames  = transitionList ( );
  pSlideshow->intro_page        = introSlide     ( );
  pSlideshow->bKenBurns         = kenBurns       ( );
  pSlideshow->bImgBackgroundImg = (bool)transitionType ( );
  pSlideshow->yres              = ( ( m_iFormatType == 0 ) || ( m_iFormatType == 2 ) ) ? 480 : 576;
  pSlideshow->xres              = 720;
  pSlideshow->aspect            = ( ( m_iFormatType == 0 ) || ( m_iFormatType == 1 ) ) ? 0 : 1;  // 4:3 or 16:9
}

void SlideDefaults::readIniFile ( QDomElement &rootNode )
{
  initMe ( );

  QString     qsChildTag, qsChildText;
  QDomElement childElem;
  QDomText    childText;
  QDomNode    xmlNode = rootNode.firstChild ( );
  QDomAttr    a = rootNode.attributeNode ( ATTRIB_IMAGE_DELAY );
  if ( ! a.value ( ).isEmpty ( ) )
    m_fImageDelay = a.value ( ).toFloat ( );
  a = rootNode.attributeNode ( ATTRIB_FILTER_DELAY );
  if ( ! a.value ( ).isEmpty ( ) )
    m_fFilterDelay = a.value ( ).toFloat ( );
  a = rootNode.attributeNode ( ATTRIB_FORMAT );
  m_iFormatType = 0; // NTSC 4:3
  if ( a.value ( ) == "PAL 4:3" )
    m_iFormatType   = 1;
  if ( a.value ( ) == "NTSC 16:9" )
    m_iFormatType   = 2;
  if ( a.value ( ) == "PAL 16:9" )
    m_iFormatType   = 3;
  a = rootNode.attributeNode ( ATTRIB_TRANSITION );
  if ( ! a.value ( ).isEmpty ( ) )
    m_iTransitionType = a.value( ).toInt ( );
  a = rootNode.attributeNode ( ATTRIB_INTRO );
  m_bCreateIntroSlide = true;
  if ( a.value ( ) == "false" )
    m_bCreateIntroSlide = false;
  a = rootNode.attributeNode ( ATTRIB_KENBURNS );
  m_bKenBurns = true;
  if ( a.value ( ) == "false" )
    m_bKenBurns = false;

  while ( ! xmlNode.isNull ( ) )  {
    childElem  = xmlNode.toElement ( );
    qsChildTag = childElem.tagName ( );
    if ( qsChildTag == QString ( TAG_BACKGROUND ) )  {
      qsChildText = childElem.text ( );
      if ( ! qsChildText.isEmpty ( ) )
        m_listBackground = qsChildText.split ( "," );
    }
    else if ( qsChildTag == QString ( TAG_AUDIO ) )  {
      qsChildText = childElem.text ( );
      if ( ! qsChildText.isEmpty ( ) )
        m_listAudio = qsChildText.split ( "," );
    }
    else if ( qsChildTag == QString ( TAG_TRANSITION ) )  {
      qsChildText = childElem.text ( );
      if ( ! qsChildText.isEmpty ( ) )
        m_listTransitions = qsChildText.split ( "," );
    }

    xmlNode = xmlNode.nextSibling ( );
  }
}

bool SlideDefaults::saveIniFile ( QDomElement &rootElement )
{
  QDomDocument xmlDoc = rootElement.ownerDocument ( );
  QDomElement  theElement;
  QDomText     theText;
  QString qsList;

  rootElement.setAttribute ( ATTRIB_IMAGE_DELAY,  QString ( "%1" ).arg ( m_fImageDelay     ) );
  rootElement.setAttribute ( ATTRIB_FILTER_DELAY, QString ( "%1" ).arg ( m_fFilterDelay    ) );
  rootElement.setAttribute ( ATTRIB_TRANSITION,   QString ( "%1" ).arg ( m_iTransitionType ) );
  rootElement.setAttribute ( ATTRIB_INTRO,        m_bCreateIntroSlide    ? "true" : "false"  );
  rootElement.setAttribute ( ATTRIB_KENBURNS,     m_bKenBurns            ? "true" : "false"  );

  if ( m_iFormatType == 0 )
    rootElement.setAttribute ( ATTRIB_FORMAT, "NTSC 4:3" );
  if ( m_iFormatType == 1 )
    rootElement.setAttribute ( ATTRIB_FORMAT, "PAL 4:3" );
  if ( m_iFormatType == 2 )
    rootElement.setAttribute ( ATTRIB_FORMAT, "NTSC 16:9" );
  if ( m_iFormatType == 3 )
    rootElement.setAttribute ( ATTRIB_FORMAT, "PAL 16:9" );

  if ( m_listBackground.size ( ) > 0 )  {
    qsList     = m_listBackground.join  ( ","            );
    theText    = xmlDoc.createTextNode  ( qsList         );
    theElement = xmlDoc.createElement   ( TAG_BACKGROUND );
    theElement.appendChild  ( theText    );
    rootElement.appendChild ( theElement );
  }

  if ( m_listAudio.size ( ) > 0 )       {
    qsList     = m_listAudio.join       ( ","            );
    theText    = xmlDoc.createTextNode  ( qsList         );
    theElement = xmlDoc.createElement   ( TAG_AUDIO      );
    theElement.appendChild  ( theText    );
    rootElement.appendChild ( theElement );
  }

  if ( m_listTransitions.size ( ) > 0 ) {
    qsList     = m_listTransitions.join ( ","            );
    theText    = xmlDoc.createTextNode  ( qsList         );
    theElement = xmlDoc.createElement   ( TAG_TRANSITION );
    theElement.appendChild  ( theText    );
    rootElement.appendChild ( theElement );
  }

  return true;
}

float SlideDefaults::imageDelay ( )
{
  return m_fImageDelay;
}

void SlideDefaults::setImageDelay ( float fImageDelay )
{
  m_fImageDelay = fImageDelay;
}

float SlideDefaults::filterDelay ( )
{
  return m_fFilterDelay;
}

void SlideDefaults::setFilterDelay ( float fFilterDelay )
{
  m_fFilterDelay = fFilterDelay;
}

int SlideDefaults::formatType ( )
{
  return m_iFormatType;
}

void SlideDefaults::setFormatType ( int iFormatType )
{
  m_iFormatType = iFormatType;
}

int SlideDefaults::transitionType ( )
{
  return m_iTransitionType;
}

void SlideDefaults::setTransitionType ( int iTransitionType )
{
  m_iTransitionType = iTransitionType;
}

bool SlideDefaults::introSlide ( )
{
  return m_bCreateIntroSlide;
}

void SlideDefaults::setIntroSlide ( bool bIntroSlide )
{
  m_bCreateIntroSlide = bIntroSlide;
}

bool SlideDefaults::kenBurns ( )
{
  return m_bKenBurns;
}

void SlideDefaults::setKenBurns ( bool bKenBurns )
{
  m_bKenBurns = bKenBurns;
}

QStringList &SlideDefaults::backgroundList ( )
{
  return m_listBackground;
}

QStringList &SlideDefaults::audioList ( )
{
  return m_listAudio;
}

QStringList &SlideDefaults::transitionList ( )
{
  return m_listTransitions;
}

void SlideDefaults::setBackgroundList ( QStringList &list)
{
  m_listBackground = list;
}

void SlideDefaults::setAudioList ( QStringList &list)
{
  m_listAudio = list;
}

void SlideDefaults::setTransitionList ( QStringList &list)
{
  m_listTransitions = list;
}

void SlideDefaults::addAudio ( QString &qsAudio )
{
  m_listAudio.append ( qsAudio );
}

void SlideDefaults::addTransition ( QString &qsTransition )
{
  m_listTransitions.append ( qsTransition );
}

void SlideDefaults::addBackground ( QString &qsBackground )
{
  m_listBackground.append ( qsBackground );
}

