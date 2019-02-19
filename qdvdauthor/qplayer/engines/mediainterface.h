/******************************************************************************
** class MediaInterface.
**
** Created: Tue Nov 16 12:09:08 2004
**      by: Varol Okan using kate editor
**
** This class will be used to control the foreign MediaObject. 
** The implementations looks as follows ...
**
**
**   QXineWidget            => Found on the web widget ...
**	   |
**	   +-> XineWidget       => implementation of interface to the QXineWidget
**	   |
** MediaInterface	=> pure virtual interface description
**	   |
**	   +-> MPLayerWidget    => implementation of interface to KmpMPlayerWidget
**	   |
**  KmpMPLayerWidget        => Found on the web widget ...
** 
** we have the following class hirarchiy:
**
** QXineWidget : public QWidget, public QThread (Xine player widget found on the web)
** +-> XineWidget : protected QXineWidget, public MediaInterface
**
** KmpMPlayer : public QWidget  (MPlayer player widget found on the web)
** +-> MPlayerWidget : protected KmpMPlayer, public MediaInterface
**
** VLCWindow : public QWidget  (VLC player widget found on the web)
** +-> VLCWidget : protected VLCWindow, public MediaInterface
**
********************************************************************************/

#ifndef MEDIAINTERFACE_H
#define MEDIAINTERFACE_H

//#include <qobject.h>		//ooo
#include <QObject>		//xxx
//Added by qt3to4:
#include <QPixmap>
class MediaInfo;

class MediaInterface : public QObject
{
	Q_OBJECT
public:
  enum enNavigate {
    NavigateUp=0,
    NavigateDown,
    NavigateLeft,
    NavigateRight,
    NavigateSelect
  };
  enum enMenu {
    MenuRoot=0,
    MenuTitle
  };
	MediaInterface          ( );
	virtual ~MediaInterface ( );

	// Mandatory mediaPlayer dependand implementations of the Interface
	virtual QWidget *getWidget         ( ) = 0;
	virtual const char *getEngineName  ( ) = 0;
	virtual void       *getMediaEngine ( ) = 0;
	virtual void setupDialog           ( ) = 0;

	virtual void playMRL ( const QString ) = 0;
	virtual bool openMRL ( const QString ) = 0;
	virtual void play                  ( ) = 0;
	virtual void pause                 ( ) = 0;
	virtual void stop                  ( ) = 0;
	virtual void setSpeed        ( float ) = 0;
	virtual void setVolume       ( float ) = 0;
	virtual void setPosition      ( uint ) = 0;
	virtual void setPositionByTime( uint ) = 0;

	virtual void setNavigate( enNavigate ) = 0; // up, down, left, right, select		//ooo
	virtual void setMenu        ( enMenu ) = 0; // root, title				//ooo
	virtual void setSubtitleTrack  ( int ) = 0; // subtitle number				//ooo
	virtual void setAudioTrack     ( int ) = 0; // audio track				//ooo

	virtual float volume               ( ) = 0;
	virtual float position             ( ) = 0;
	virtual QImage getScreenshot       ( ) = 0;
 	virtual QImage getScreenshot ( float ); // value in Seconds ...
	virtual bool   setScreenshot ( long  ) = 0; // value in MSeconds
	virtual bool   setScreenshot ( QPixmap & ) = 0;

	// Functions handled in the MediaInterface - calss
	const QString& getMRL();
	void  slotTogglePause();

	// Non mandatory functions. 
	virtual QString supportedExtensions( );
	virtual MediaInfo *getMediaInfo    ( );
	virtual void initMediaEngine       ( );
	virtual void setAspectRatio   ( uint ); // Can be ASPECT_AUTO, ASPECT_34, ASPECT_169, or ASPECT_SQUARE
	virtual void setEqualizer( uint, int ); // Tune the sound 
	
	virtual bool hasChapters           ( );
	virtual void playChapter       ( int ); // 0=current / -x-=play x chapters before / +x=play current chapter + x

	virtual bool isPlaying             ( );
	virtual bool isPaused              ( );

	// Here some usefull signals to get hooked to ...
signals:
	void signalQuit();
	void signalNewInfo          ( const QString & );
	void signalPlaybackFinished ( );
	// newPos : [0 .. 65535], 00:00:00.000
	void signalNewPosition      ( int, const QString & ); 
	void signalNewPosition      ( long ); // position in MSec
	void signalToggleFullscreen ( );
  
protected:
	QString    m_qsMRL;      // Keeps the name of the current/last media stream
	MediaInfo *m_pMediaInfo; // NULL or hold all infos of the current/last media stream
	float      m_fVolume;
	float      m_fPosition;
	bool       m_bPaused;
};

#endif	// MEDIAINTERFACE_H
