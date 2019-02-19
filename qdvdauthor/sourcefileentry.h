/***************************************************************************
    sourcefileentry.h
                             -------------------
    SourceFileEntry - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is a simple container class to store the info
   for a entry in the Source File List.
    
****************************************************************************/

#ifndef SOURCEFILEENTRY_H
#define SOURCEFILEENTRY_H

#include <qstringlist.h>
#include <qfont.h>
#include <qsize.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#include "subtitles.h"	//ooo

class QImage;
class QDomNode;
class QDomElement;
class CXmlSlideshow;

#define MAX_AUDIO 8

class TranscodeInterface
{
public:
	 TranscodeInterface ();
	~TranscodeInterface ();
	TranscodeInterface & operator = (TranscodeInterface &);
	bool operator == (const TranscodeInterface &);
	bool operator != (const TranscodeInterface &);
	bool readProjectFile  (QDomNode &);
	bool writeProjectFile (QDomElement &);

	QString qsVideoFormat;
	QString qsVideoType;
	QString qsResolution;
	int     iVideoBitrate;  // in bps
	float   fFrameRate;
	QString qsRatio;

	QString qsAudioFormat;  // "Stereo"
	QString qsAudioType;    // One of [ mp2, ac3, or pcm ]
	int     iAudioBitrate;  // in bps E.g. 192000 (instead of 196608 or 192k)
	int     iSample;

	int     iStartingAt;	// The movie should start here (in mSec)
	int     iEndingAt;	// The movie should stop here
	bool    bRemuxOnly;     // If we have the right sized video stream in Mpeg2 format we only need to remux instead of a complete transcoding process.
	bool    bTranscoding;	// This flag indicates that the transcoding process is currently ongoing.
	// Note: once the transcoding part terminates, 
	// o the transcoding interface is deleted, 
	// o the original file-name is replaced with the generated mpeg2 movie file-name

	// the following flag is set if we have meta info present in the video stream
	// Currently only DV material is handled.
	// If set, then a subtitle will be created using dv2sub and this flag will be set to -1
	int     iSubtitleFromMetaInfo; 
	QString qsTempFile;
};

class AudioEntry
{
public:
  AudioEntry ( );
  AudioEntry &operator = ( AudioEntry & );
  QString qsIso639;
  int     iAudioNumber;
};

class Audio : public AudioEntry
{
public:
   Audio ( );
  ~Audio ( );
  Audio &operator =      ( Audio       & );
  bool   operator ==     ( Audio       & );
  bool  readProjectFile  ( QDomNode    & );
  bool  writeProjectFile ( QDomElement & );

  QString             qsFileName;
  TranscodeInterface *pTranscodeInterface;
};

// the following class is filled with information after the source has been added to the SourceToolbar
class SourceFileInfo
{
public:
	 SourceFileInfo ( );
	~SourceFileInfo ( );

	SourceFileInfo & operator = (SourceFileInfo &);
	bool readProjectFile    ( QDomNode & );
	bool writeProjectFile   ( QDomElement &, bool b=false );

	QString getFileName     ( );
	int     audioCount      ( );
	void    deleteAudio     ( );
	void    addAudio        ( Audio * );
	int     subtitleCount   ( );
	void    deleteSubtitles ( );
	void    addSubtitles    ( Subtitles * );
	int     format          ( );

	QString qsFileName;	  // E.g. video.mpg
	QString qsSize;		  // E.g. 2.2M
	QString qsResolution;	  // E.g. 720x576
	QString qsFPS;		  // E.g. 25
	QString qsRatio;	  // E.g. 16:9
	QString qsLength;	  // E.g. 01:22:23
	QString qsVideoFormat;	  // E.g. 'PAL'
	QString qsVideoCodec;	  // E.g. 'mpeg'
	QString qsStatus;	  // E.g. Ok	-=> to say this file we can autmatically convert to a DVD video
	// For Audio Sources :
	bool    bHasAudioTrack;   // Elemntary streams only have Video
	QString qsAudioFormat;    // E.g. 'Stereo'
	QString qsAudioCodec;     // E.g. "mp2"
	QString qsBits;           // E.g. 16
	QString qsSample;         // E.g. 48kHz
	QString qsBPS;            // E.g. 00:05:32

	long    iMSecPreview;     // The preview is to be generated iMSec from the start.
	QImage *pPreview;         // Image of the size for the Button
	bool    bUpdateInfo;      // if the preview has been generated we set this flag to indicate this.

	QStringList listChapters; // The chapters of this SourceFile entry
	QString     qsPreCommand; // The Pre  Command tag for dvdauthor
	QString     qsPostCommand;// The Post command tag for dvdauthor
	int         iPause;       // The pause attribute for dvdauthor (-1=inf)
	int         iTitleset;    // In case the user wants to define the titleset number
	bool        bAutotranscode; // The user elected not to have this entry autotranscoded any more.
	bool        bMetaSubtitles; // per def for DV Subtitles are created. If the user unselect this info, then we should do no longer the auto thingy ...
	TranscodeInterface *pTranscodeInterface;
	Subtitles  *arraySubtitles[ MAX_SUBTITLES ];
	Audio      *arrayAudio    [ MAX_AUDIO ];
	CXmlSlideshow *pSlideshow;  // This is set through DialogImages. One SourceGroup = 1 SourceFileEntry with SourceFileInfo
};

// Note if the entry is a dvd-slideshow, then the qsDisplayName will end in .xml
// Once the slideshow has been generated, the name is changed to .mpg
class SourceFileEntry
{
public:
	 SourceFileEntry ( );
	~SourceFileEntry ( );

	bool readProjectFile  ( QDomNode    & );
	bool writeProjectFile ( QDomElement &, bool b=false );
	SourceFileEntry & operator = ( SourceFileEntry & );
	SourceFileEntry * flatClone  ( );
	int  audioCount    ( bool );
	int  subtitleCount ( bool );
	void synchTracks   ( bool a=true, bool s=true );
	void changeAudioTrack    ( uint, QString );
	void changeSubtitleTrack ( uint, QString );

	QString qsDisplayName;          // The name that appears on the display
	QList <SourceFileInfo *> listFileInfos;			//oxx
	QStringList listChapters;       // The chapters of this SourceFile entry
	bool bIsSlideshow;              // Will be re-set, once the slideshow has been created into a mpeg2 movie.
	bool bSoundSource;              // Indicates if this is a movie or a Sound source

	int iTitleset;                  // Set when processing createDvdauthorXml () function
	int iTitle;                     // Set when processing createDvdauthorXml () function
	int iJumpStart;                 // Set when processing createDvdauthorXml () function
	QSize sizeThumbnail;            // The size of the thumbnail
	QString qsPre;
	QString qsPost;
	int     iPause;                 // The pause attribute for dvdauthor (-1=inf)

	AudioEntry    arrayAudioEntries    [MAX_AUDIO];     // link between multiple Infos audio setting and the avail 8 audio streams.
	SubtitleEntry arraySubtitleEntries [MAX_SUBTITLES]; // link between multiple Infos subtitle settings and the 32 subtitle tracks.
};

#endif // SOURCEFILEENTRY_H

