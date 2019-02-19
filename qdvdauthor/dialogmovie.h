/***************************************************************************
    dialogmovie.h
                             -------------------
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
 ***************************************************************************/

#ifndef DIALOGMOVIE_H
#define DIALOGMOVIE_H

#include "sourcefileentry.h"
#include "ui_uidialogmovie.h"

class MovieObject;
class DVDMenu;
class QDVDAuthor;
class MediaInterface;
class StartStopControl;

class DialogMovie : public QDialog, public Ui::uiDialogMovie		//xxx
{
	Q_OBJECT
public:
  //DialogMovie(QWidget *p=NULL, const char *n=NULL, Qt::WFlags f=0);	//ooo
  DialogMovie(QWidget *pParent=NULL);					//xxx
 ~DialogMovie();

  void initMe(SourceFileInfo  *, const void *, QDVDAuthor * );
  void initMe(SourceFileEntry *);
  void initMe(MovieObject     *);
  void initMe(DVDMenu         *);	//oooo
  
  bool transcodeEnabled ( );
  void initTranscodeTab ( );
  
  SourceFileInfo *sourceFileInfo ( );
  
protected slots:
  virtual void timerEvent       ( QTimerEvent * );
  virtual void slotNewVolume    ( int );
  virtual void accept           ( );
  virtual void slotPlay         ( );
  virtual void slotStop         ( );
  virtual void slotSetThumbnail ( );
  virtual void slotStart        ( );
  virtual void slotEnd          ( );
  virtual void slotTranscode    ( );
  virtual void slotEncodeNow    ( );
  
  virtual void slotAddChapter   ( );
  virtual void slotDelChapters  ( );
  virtual void slotAutoChapters ( );

  virtual void slotNewPosition    ( int, const QString &);  
  virtual void slotSliderValues   ( float, float );
  virtual void slotSliderReleased ( bool );
  
  virtual void slotVideoOutChanged        (const QString &);
  
  virtual void slotSetStartStop       ( );
  virtual void slotSetMediaInfo       ( );
  virtual void slotInitMediaInterface ( );
  virtual void slotSameAsMenuLength   ( );
  virtual void slotMenuLoop           ( );
  
signals:
  void signalSetThumbnail ( long );
  
private:
  QString keepClean    ( const QString  & );
  void setStartStop    ( bool             );
  void addChapter      ( QString &        );
  void initMediaInterface ( DVDMenu    * );
  void initAudioTab    ( SourceFileInfo *, DVDMenu *p=NULL );
  void setHeader       ( SourceFileInfo * );
  void applyTransitions(     );
  void createAudioList (     );
  QTime maxEndTime     (     );
  
private:
  int               m_iTransitionForAll; // -1==doNothing / 0=remove all transitions / 1=set all transitions
  QRect             m_rectGeometry;
  QString           m_qsFileName;
  QString           m_qsPosition;
  int               m_iMovieLength;
  QWidget          *m_pTranscodeTab;
  //Q3ListViewItem    *m_pSelectItem;	//ooo
  MediaInterface   *m_pMediaInterface;
  StartStopControl *m_pStartStopControl;
  SourceFileInfo    m_sourceFileInfo;
  SourceFileEntry  *m_pParentSourceFileEntry;
  MovieObject      *m_pMovieObject;
  DVDMenu          *m_pDVDMenu;

  // the following is only used for the initializaion part
  const void       *m_pExistingEngine;
};

#endif // DIALOGMOVIE_H
