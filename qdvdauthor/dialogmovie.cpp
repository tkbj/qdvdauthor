/***************************************************************************
    dialogmovie.cpp
                             -------------------
    DialogMovie class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QTimer>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>
#include <QGridLayout>

#include "dvdmenu.h"
#include "qdvdauthor.h"

#include "qplayer/mediainfo.h"
#include "qplayer/mediacreator.h"
#include "qplayer/startstopcontrol.h"
#include "qplayer/engines/mediainterface.h"

#include "messagebox.h"
#include "movieobject.h"
#include "dialogmovie.h"

#define PROPERTIES_TAB  0
#define SUBTITLE_TAB    1
#define CHAPTERS_TAB    2
#define GEOMETRY_TAB    3
#define TRANSCODING_TAB 4

#define COLOR_TRANSCODE   238,  33,  43
#define COLOR_NOTRANSCODE 209, 226, 238

//DialogMovie::DialogMovie(QWidget * parent, const char * name, Qt::WFlags f)		//ooo
DialogMovie::DialogMovie(QWidget * pParent)						//xxx
	//: uiDialogMovie (parent, name, f)	//ooo
	: QDialog (pParent)			//xxx
{
  setupUi(this);
  
  srand ( time ( NULL ) );

  m_pMediaInterface   = NULL;
  m_pStartStopControl = NULL;	//ooo
  m_pTranscodeTab     = NULL;	//ooo
  m_pMovieObject      = NULL;	//ooo
  m_iMovieLength      =    1;// 1ms default, so we never have 0
  m_iTransitionForAll =   -1;
  m_pDVDMenu          = NULL;
  //m_pSelectItem       = NULL;	//ooo
  m_pParentSourceFileEntry = NULL; // needed to get correct audio / subtitle setting for a SourceFileInfo
  connect ( m_pEditWidth,       SIGNAL ( lostFocus ( ) ), this, SLOT ( slotWidth          ( ) ) );
  connect ( m_pEditHeight,      SIGNAL ( lostFocus ( ) ), this, SLOT ( slotHeight         ( ) ) );
  connect ( m_pButtonPlay,      SIGNAL ( clicked   ( ) ), this, SLOT ( slotPlay           ( ) ) );
  connect ( m_pButtonStop,      SIGNAL ( clicked   ( ) ), this, SLOT ( slotStop           ( ) ) );
  connect ( m_pButtonStart,     SIGNAL ( clicked   ( ) ), this, SLOT ( slotStart          ( ) ) );
  connect ( m_pButtonEnd,       SIGNAL ( clicked   ( ) ), this, SLOT ( slotEnd            ( ) ) );
  connect ( m_pButtonThumbnail, SIGNAL ( clicked   ( ) ), this, SLOT ( slotSetThumbnail   ( ) ) );
  connect ( m_pButtonTranscode, SIGNAL ( clicked   ( ) ), this, SLOT ( slotTranscode      ( ) ) );
  connect ( m_pButtonEncodeNow, SIGNAL ( clicked   ( ) ), this, SLOT ( slotEncodeNow      ( ) ) );
  connect ( m_pButtonAnimation, SIGNAL ( clicked   ( ) ), this, SLOT ( slotAnimation      ( ) ) );
  connect ( m_pButtonAddCK,     SIGNAL ( clicked   ( ) ), this, SLOT ( slotAddColorKey    ( ) ) );
  connect ( m_pButtonDelCK,     SIGNAL ( clicked   ( ) ), this, SLOT ( slotDelColorKey    ( ) ) );
  connect ( m_pButtonCK,        SIGNAL ( clicked   ( ) ), this, SLOT ( slotBrowseColorKey ( ) ) );
  connect ( m_pButtonEdit,      SIGNAL ( clicked   ( ) ), this, SLOT ( slotEditMediaInfo  ( ) ) );
  connect ( m_pCheckCreateSubtitles, SIGNAL ( toggled ( bool ) ), this, SLOT ( slotSubLangChanged ( bool ) ) );

  connect( buttonCancel,  SIGNAL( clicked ( ) ),     this, SLOT( reject      ( ) ) );	//xxx
  connect( buttonAccept,  SIGNAL( clicked ( ) ),     this, SLOT( accept      ( ) ) );	//xxx

}

DialogMovie::~DialogMovie()
{
  if ( m_pMediaInterface )
    delete m_pMediaInterface;
  if ( m_pStartStopControl )
    delete m_pStartStopControl;
}

void DialogMovie::initMe ( DVDMenu *pMenu )
{
  // Called when handling the background of a menu (Image background or MovieBackground.)
  if ( ! pMenu )
    return;

  CDVDMenuInterface *pInterface = pMenu->getInterface ( );
  if ( ( pInterface->qsMovieFileName.isNull ( ) ) && ( pInterface->qsBackgroundFileName.isNull ( ) ) )
    return;

  QFileInfo fileInfo;
  QString qsFormat, qsBackgroundName;
  int iPause = -1;

  m_pDVDMenu = pMenu;
  qsFormat = QString ("hh:mm:ss.zzz");
  QTime endingTime = pInterface->timeOffset.addMSecs ( -pInterface->timeDuration.msecsTo ( QTime ( ) ) );
  m_pEditStarting2->setText ( pInterface->timeOffset.toString ( qsFormat ) );
  m_pEditEnding2  ->setText ( endingTime.toString ( qsFormat ) );

  // Okay coming from the MovieObject we need only the Geometry - tab
  //QWidget *pTab = m_pTabWidget->page (TRANSCODING_TAB);	//ooo
  QWidget *pTab = m_pTabWidget->widget (TRANSCODING_TAB);	//xxx
  //m_pTabWidget->removePage (pTab);				//ooo
  m_pTabWidget->removeTab (m_pTabWidget->indexOf(pTab));	//xxx
  delete pTab;
  //pTab = m_pTabWidget->page (CHAPTERS_TAB);			//ooo
  pTab = m_pTabWidget->widget (CHAPTERS_TAB);			//xxx
  //m_pTabWidget->removePage (pTab);				//ooo
  m_pTabWidget->removeTab (m_pTabWidget->indexOf(pTab));	//xxx
  delete pTab;
  // Also no transcoding buttons will be needed.
  delete m_pButtonTranscode;
  delete m_pButtonEncodeNow;
  m_pButtonTranscode = NULL;
  m_pButtonEncodeNow = NULL;
  // Here we trick the object in accepting the right QLineEdits
  m_pEditStarting1 = m_pEditStarting2;
  m_pEditEnding1   = m_pEditEnding2;
  m_pButtonEdit->setEnabled ( false );

  //pTab = m_pTabWidget->page ( GEOMETRY_TAB );	//ooo
  pTab = m_pTabWidget->widget ( GEOMETRY_TAB );	//xxx
  initAudioTab ( NULL, pMenu );

  // The following lines will display the [Change Size ...] button.
  QSizePolicy policy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  delete textLabel4;
  delete m_pListViewCK;
  delete m_pButtonAddCK;
  delete m_pButtonDelCK;
  delete m_pEditCK;
  delete textLabel3_3;
  delete textLabel2_2;
  m_pButtonCK->setText (tr("Change Size ..."));
  m_pButtonCK->setSizePolicy ( policy );
  disconnect ( m_pButtonCK,  SIGNAL ( clicked ( ) ), this, SLOT ( slotBrowseColorKey ( ) ) );
  connect    ( m_pButtonCK,  SIGNAL ( clicked ( ) ), this, SLOT ( slotSizeDialog     ( ) ) );
  connect    ( m_pCheckLoop, SIGNAL ( clicked ( ) ), this, SLOT ( slotMenuLoop       ( ) ) );
  connect    ( m_pButtonTransition, SIGNAL ( clicked ( ) ), this, SLOT ( slotTransition ( ) ) );

  if ( pInterface->qsPost == "Loop" )  {
    m_pCheckLoop->setChecked ( true );
    slotMenuLoop ( );
  }
  groupBox8->setTitle ("");
  m_pCheckSameAsMenuLength->hide ( );
  m_pButtonLoop->hide ( );

  if (  (  ! pInterface->qsPause.isEmpty ( ) ) && 
        (    pInterface->qsPause !=  "inf" ) ) 
    iPause = pInterface->qsPause.toInt   ( );

  m_pLabelTitle->setText       ( "SubMenu:"       );
  m_pSpinBoxTitle->setValue    ( pInterface->iMenu     );
  m_pSpinBoxTitleset->setValue ( pInterface->iTitleset );
  m_pSpinBoxPause->setValue    ( iPause );
  // The VMGM Must stay in Titleset 0 !!!
  if ( ! pInterface->bIsSubMenu ) {
    m_pSpinBoxTitle->setEnabled    ( false );
    m_pSpinBoxTitleset->setEnabled ( false );
  }
  // Here we handle a MovieBackground ...
  if ( ! pInterface->qsMovieFileName.isEmpty ( ) )  {
    initMediaInterface ( pMenu );
    //m_pTabWidget->setTabLabel   ( pTab, "MovieBackground" );				//ooo
    m_pTabWidget->setTabText   ( m_pTabWidget->indexOf(pTab), "MovieBackground" );	//xxx
    m_pButtonThumbnail->setText ( tr ( "Set Background" ) );
    //    m_pTabWidget->setCurrentPage ( 1 );
  }
  else	{	// Here we handle a ImageBackground ...
    if ( ! pInterface->qsBackgroundFileName.isEmpty ( ) )  {
      QPixmap thePixmap;
      QImage theImage ( pInterface->qsBackgroundFileName );
      //theImage = theImage.smoothScale (275, 275, Qt::IgnoreAspectRatio);				//ooo
      theImage = theImage.scaled (275, 275, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);		//xxx
      thePixmap.convertFromImage (theImage);
      m_pMediaPreview->setPixmap (thePixmap);
    }
    m_pMediaPreview->setFixedSize (275, 275);
    m_pEditStarting2->setEnabled  ( false );
    fileInfo.setFile ( pInterface->qsBackgroundFileName );
    m_pTextTitle->setText (tr ("ImageBackground : %1").arg(fileInfo.fileName()));
    //setCaption ( fileInfo.fileName () );	//oooo
    setWindowTitle ( fileInfo.fileName () );	//xxx
    //m_pTabWidget->setTabLabel (pTab, "ImageBackground");			//ooo
    m_pTabWidget->setTabText ( m_pTabWidget->indexOf(pTab), "ImageBackground");	//xxx

    // And finally we hide the play buttons etc.
    delete m_pButtonPlay;
    delete m_pButtonStop;
    delete m_pButtonThumbnail;
    delete m_pButtonStart;
    delete m_pButtonEnd;
    delete m_pSliderVolume;
    delete m_pLabelCurrentPos;
    delete m_pFrameControl;
  }

  m_pComboPostCommand->clear ( );
  //m_pComboPostCommand->insertItem ( " jump vmgm menu 1; " );	//ooo
  m_pComboPostCommand->insertItem ( 0, " jump vmgm menu 1; " );	//xxx

  if ( ! pInterface->qsPre.isEmpty  ( ) )
    m_pComboPreCommand->setCurrentText  ( pInterface->qsPre  );
  if ( ! pInterface->qsPost.isEmpty ( ) )
    m_pComboPostCommand->setCurrentText ( pInterface->qsPost );

  // These controls are not used ...
  delete groupBox2;
  resize ( 400, 300 );
}

void DialogMovie::initMe ( SourceFileEntry *pSourceFileEntry )
{
  if ( ( ! pSourceFileEntry ) || ( pSourceFileEntry->listFileInfos.count ( ) < 1 ) )
    return;

  m_sourceFileInfo   = *pSourceFileEntry->listFileInfos[0];
  
  QFileInfo fileInfo;
  QString qsFormat, qsBackgroundName;

  // Okay coming from transcodeAll ...
  //QWidget *pTabChap = m_pTabWidget->page (CHAPTERS_TAB);	//ooo
  QWidget *pTabChap = m_pTabWidget->widget (CHAPTERS_TAB);	//xxx
  //QWidget *pTabGeom = m_pTabWidget->page (GEOMETRY_TAB);	//ooo
  QWidget *pTabGeom = m_pTabWidget->widget (GEOMETRY_TAB);	//xxx
  //m_pTabWidget->removePage (pTabChap);			//ooo
  m_pTabWidget->removeTab (CHAPTERS_TAB);			//xxx
  delete pTabChap;
  //m_pTabWidget->removePage (pTabGeom);			//ooo
  m_pTabWidget->removeTab (GEOMETRY_TAB);			//xxx
  delete pTabGeom;
  // Also no transcoding buttons will be needed.
  delete m_pButtonTranscode;
  delete m_pButtonEncodeNow;
  delete m_pGroupCurrentFormat;
  m_pButtonTranscode = NULL;
  m_pButtonEncodeNow = NULL;
  m_pEditStarting1   = NULL;
  m_pEditEnding1     = NULL;
  m_pComboPreCommand->setCurrentText  ( pSourceFileEntry->qsPre  );
  m_pComboPostCommand->setCurrentText ( pSourceFileEntry->qsPost );
  m_pLabelTransition->hide  ( );
  m_pButtonTransition->hide ( );

  //setCaption ( "Transcoding all files of the SourceFileEntry" );	//ooo
  setWindowTitle ( "Transcoding all files of the SourceFileEntry" );	//xxx
  m_pTextTitle->setText (tr ("SourceFileEntry : %1 Files").arg(pSourceFileEntry->listFileInfos.count()));
  // And finally we hide the play buttons etc.
  delete m_pMediaPreview;
  delete m_pButtonPlay;
  delete m_pButtonStop;
  delete m_pButtonThumbnail;
  delete m_pButtonStart;
  delete m_pButtonEnd;
  delete m_pSliderVolume;
  delete m_pLabelCurrentPos;
  delete m_pFrameControl;
  m_pCheckLoop->hide ( );

  connect (m_pComboVideoOut,        SIGNAL( activated(const QString &)), this, SLOT (slotVideoOutChanged(const QString &)));
  connect (m_pComboVideoResolution, SIGNAL( activated(const QString &)), this, SLOT (slotVideoResolutionChanged(const QString &)));
  connect (m_pComboVideoRate,       SIGNAL( activated(const QString &)), this, SLOT (slotVideoRateChanged(const QString &)));
  connect (m_pComboVideoType,       SIGNAL( activated(const QString &)), this, SLOT (slotVideoTypeChanged(const QString &)));
  connect (m_pComboVideoAspect,     SIGNAL( activated(const QString &)), this, SLOT (slotVideoAspectChanged(const QString &)));
  connect (m_pComboVideoBitrate,    SIGNAL( activated(const QString &)), this, SLOT (slotVideoBitrateChanged(const QString &)));
  resize ( 300, 250 );
}

void DialogMovie::initMe ( SourceFileInfo *pInfo, const void *pExistingMediaEngine, QDVDAuthor *pDVDAuthor )
{ 
  Utils theUtil;
  if ( ! pInfo )
    return;

  int t, i, iThumbnailSize = pDVDAuthor->getDragNDropContainer()->sizeThumbnail.width ( );
  SourceFileEntry *pEntry  = NULL;
 
  for ( t=0; t<pDVDAuthor->sourceFileCount  ( ); t++ ) {
    pEntry   = pDVDAuthor->sourceFileEntry ( t );
    if ( pEntry ) {
      for ( i=0; i<(int)pEntry->listFileInfos.count ( ); i++ ) {
        if ( pEntry->listFileInfos[i] == pInfo ) {
          m_pParentSourceFileEntry = pEntry;
          t = pDVDAuthor->sourceFileCount ( ); // exit t and i - loop
          break;
        }
      }
    }
  }

  // Okay coming from the SourceToolbar we do not need the Geometry - tab
  //QWidget *pGeometry  = m_pTabWidget->page (GEOMETRY_TAB);		//ooo
  QWidget *pGeometry  = m_pTabWidget->widget (GEOMETRY_TAB);		//xxx
  //m_pTranscodeTab = m_pTabWidget->page (TRANSCODING_TAB);		//ooo
  m_pTranscodeTab = m_pTabWidget->widget (TRANSCODING_TAB);		//xxx
  //m_pTabWidget->removePage (pGeometry);				//ooo
  //m_pTabWidget->removeTab (GEOMETRY_TAB);				//xxx
  m_pTabWidget->removeTab (m_pTabWidget->indexOf(pGeometry));		//xxx
  //m_pTabWidget->removePage (m_pTranscodeTab);				//ooo
  //m_pTabWidget->removeTab (TRANSCODING_TAB);				//xxx
  m_pTabWidget->removeTab (m_pTabWidget->indexOf(m_pTranscodeTab));	//xxx
  delete pGeometry;
  m_pLabelTransition->hide  ( );
  m_pButtonTransition->hide ( );
  // we keep the Transcode widget around in case the user wants to transcode the file.

  // And next we init the chapters - tab
  int iLength = theUtil.getMsFromString ( pInfo->qsLength );

  m_pCheckLoop->hide ( );

  // Here we copy the infos over ...
  m_sourceFileInfo = *pInfo;

  // and now on to initializing the dialog. Length : 00:07:23.123 File : Cool Video.mpg
  setHeader ( pInfo );

  m_pListBoxChapters->clear ( );
  if ( m_sourceFileInfo.listChapters.count  ( ) == 0 )
    //m_pListBoxChapters->insertItem (QString ( "00:00:00.000" ) );		//ooo
    m_pListBoxChapters->addItem (QString ( "00:00:00.000" ) );			//xxx
  //m_pListBoxChapters->insertStringList   ( m_sourceFileInfo.listChapters );	//ooo
    m_pListBoxChapters->addItems   ( m_sourceFileInfo.listChapters );		//xxx
  m_pListBoxChapters->setCurrentItem     ( 0 );
  //m_pListBoxChapters->setSelected        ( 0, true );				//ooo
  m_pListBoxChapters->setSortingEnabled        ( true );			//xxx

  m_pComboPreCommand->setCurrentText  (m_sourceFileInfo.qsPreCommand);
  m_pComboPostCommand->setCurrentText (m_sourceFileInfo.qsPostCommand);
  m_pSpinBoxPause->setValue           (m_sourceFileInfo.iPause);

  m_pSpinBoxTitle->setEnabled  ( false );
  m_pSpinBoxTitleset->setValue ( m_sourceFileInfo.iTitleset );

  m_pMediaInterface = MediaCreator::createPreferredWidget (m_pMediaPreview, "ChapterXineWidget", (void *)pExistingMediaEngine);

  connect (m_pMediaInterface, SIGNAL(signalNewPosition(int, const QString&)), this, SLOT(slotNewPosition(int, const QString&)));
  //Q3GridLayout *pVideoLayout = new Q3GridLayout (m_pMediaPreview, 1, 1, 2, 2, "MediaMainLayout");	//ooo
  QGridLayout *pVideoLayout = new QGridLayout (m_pMediaPreview);					//xxx
  pVideoLayout->addWidget (m_pMediaInterface->getWidget(), 0, 0);

  // This is take care of in the polish () function for Xine and not necessary for MPlayer ...
  //m_pMediaInterface->initMediaEngine();	//oooo

  m_qsFileName      = m_sourceFileInfo.qsFileName;

  m_pExistingEngine = pExistingMediaEngine;

  // This is the new version, where I have a new WidgetControl.
  //Q3GridLayout *pLayout = new Q3GridLayout      ( m_pFrameControl, 1, 1, 2, 2, "pLayout");	//ooo
  QGridLayout *pLayout = new QGridLayout      ( m_pFrameControl );				//xxx
  m_pStartStopControl  = new StartStopControl ( m_pFrameControl );
  m_pStartStopControl->initMe ( m_sourceFileInfo.qsFileName, iThumbnailSize, iLength );
  connect (m_pStartStopControl, SIGNAL (signalSliderValues   ( float, float ) ), this, SLOT ( slotSliderValues   ( float, float ) ) );
  connect (m_pStartStopControl, SIGNAL (signalSliderReleased ( bool ) ),         this, SLOT ( slotSliderReleased ( bool ) ) );

  initAudioTab ( &m_sourceFileInfo );		//oooo
  m_iMovieLength = iLength;
  if ( m_iMovieLength < 1 )
       m_iMovieLength = 1;
  pLayout->addWidget( m_pStartStopControl, 0, 0 );

  if ( m_sourceFileInfo.pTranscodeInterface )	{
    if ( m_sourceFileInfo.pTranscodeInterface->iEndingAt == 0 ) 
         m_sourceFileInfo.pTranscodeInterface->iEndingAt = m_iMovieLength;
    int  iMovieLength = m_sourceFileInfo.pTranscodeInterface->iEndingAt;

    if ( iMovieLength > m_iMovieLength )
         iMovieLength = m_iMovieLength;
    QString  qsFormat = QString ("hh:mm:ss.zzz");
    //QTime endingTime, startingTime;			//ooo
    QTime endingTime(0,0,0,0), startingTime(0,0,0,0);	//xxx
    startingTime = startingTime.addMSecs ( m_sourceFileInfo.pTranscodeInterface->iStartingAt );
    endingTime   = endingTime.addMSecs   ( iMovieLength  );	//ooo
    //endingTime   = endingTime.addMSecs   ( 10000  );		//xxx

    m_pEditStarting1->setText ( startingTime.toString ( qsFormat ) );
    
    m_pEditEnding1  ->setText ( endingTime.toString   ( qsFormat ) );
  }

  m_pLabelVideoFormat->setText (QString ("%1(%2) %3Hz").arg( pInfo->qsVideoFormat ).arg ( pInfo->qsResolution ).arg ( pInfo->qsFPS ) );

  m_pLabelAudioFormat->setText(QString ("%1 %2bit %3").arg( pInfo->qsAudioFormat ).arg( pInfo->qsBits ).arg( pInfo->qsSample ) );

  m_pButtonEncodeNow->hide ( );
  
  // Here we init the properties - tab
  initTranscodeTab ( );			//oooo

  //m_pTabWidget->setCurrentPage ( 0 );		//ooo
  m_pTabWidget->setCurrentIndex ( 0 );		//xxx

  // And finally here are the callback functions ...
  connect ( m_pButtonAdd,       SIGNAL( clicked ( ) ), this, SLOT ( slotAddChapter   ( ) ) );
  connect ( m_pButtonImport,    SIGNAL( clicked ( ) ), this, SLOT ( slotImportChapter( ) ) );
  connect ( m_pButtonDel,       SIGNAL( clicked ( ) ), this, SLOT ( slotDelChapters  ( ) ) );
  connect ( m_pButtonAuto,      SIGNAL( clicked ( ) ), this, SLOT ( slotAutoChapters ( ) ) );
  //connect ( m_pListBoxChapters, SIGNAL( currentChanged (Q3ListBoxItem *)), this, SLOT (slotCurrentChanged(Q3ListBoxItem *)));	//oooo

  connect (m_pComboVideoOut,        SIGNAL( activated(const QString &)), this, SLOT ( slotVideoOutChanged(const QString &)));
  connect (m_pComboVideoResolution, SIGNAL( activated(const QString &)), this, SLOT ( slotVideoResolutionChanged(const QString &)));
  connect (m_pComboVideoRate,       SIGNAL( activated(const QString &)), this, SLOT ( slotVideoRateChanged(const QString &)));
  connect (m_pComboVideoType,       SIGNAL( activated(const QString &)), this, SLOT ( slotVideoTypeChanged(const QString &)));
  connect (m_pComboVideoAspect,     SIGNAL( activated(const QString &)), this, SLOT ( slotVideoAspectChanged(const QString &)));
  connect (m_pComboVideoBitrate,    SIGNAL( activated(const QString &)), this, SLOT ( slotVideoBitrateChanged(const QString &)));
  connect (m_pSliderVolume,         SIGNAL( valueChanged (int)),         this, SLOT ( slotNewVolume (int) ) );

  QTimer::singleShot (  50, this, SLOT ( slotSetStartStop ( ) ) );

  QTimer::singleShot ( 100, this, SLOT ( slotSetMediaInfo ( ) ) );	//oooo
}

void DialogMovie::setHeader ( SourceFileInfo *pInfo )
{
  if ( ! pInfo )
    return;

  Utils theUtil;
  QFileInfo fileInfo ( pInfo->qsFileName );

  // And next we init the chapters - tab
  int iLength = theUtil.getMsFromString ( pInfo->qsLength );

  // Check if the default 5 minutes chapter creation is out of range anyways ...
  QString qsReturn = theUtil.getStringFromMs ( iLength );
  if ( iLength < 5*60*1000 )      {
    m_pEditEvery->setText ( QString ("%1").arg ( (int)( iLength / 1000 ) ) );
    m_pEditAdd->setText   ( qsReturn );
  }
  m_pEditEnding1->setText ( qsReturn );

  // and now on to initializing the dialog. Length : 00:07:23.123 File : Cool Video.mpg
  m_pTextTitle->setText ( tr ( "Length : <%1> File : %2" ).arg ( pInfo->qsLength ).arg ( fileInfo.fileName ( ) ) );
  //setCaption ( fileInfo.fileName ( ) );	//ooo
  setWindowTitle ( fileInfo.fileName ( ) );	//xxx
}

void DialogMovie::slotSetMediaInfo ( )
{ 
  // Called from initMe ( SourceFileInfo * ... )
  bool bCreatedInfo = false;
  // This is required bc MPlayerInfo requires a running EventLoop to function
  // in the initMe function this is not yet given ( it is before the dialog.exec ( ) - call )
  m_pMediaInterface->initMediaEngine ( );

  m_pMediaInterface->openMRL  ( m_qsFileName );

  m_pMediaInterface->getScreenshot   ( );

  // Here we init the properties - tab
  MediaInfo *pMediaInfo = m_pMediaInterface->getMediaInfo ();

  if ( !  pMediaInfo )         {
    pMediaInfo = MediaCreator::createInfo ((void *)m_pExistingEngine);
    if ( ! pMediaInfo )
      return;
    pMediaInfo->setFileName ( m_qsFileName );
    bCreatedInfo = true;
  }

  m_pLabelVideoCodec->setText ( pMediaInfo->getCodec (  true ) );

  m_pLabelAudioCodec->setText ( pMediaInfo->getCodec ( false ) );  // Stereo 2.0

  if ( bCreatedInfo )
    delete pMediaInfo;
}

void DialogMovie::initMe ( MovieObject *pMovieObject )
{
  int t;
  // Called when coming from a MovieObject in the Menu.
  if ( ! pMovieObject )
    return;

  QString qsFormat;
  m_pMovieObject = pMovieObject;

  // Okay coming from the MovieObject we need only the Geometry - tab
  //QWidget *pTab = m_pTabWidget->page (TRANSCODING_TAB);	//ooo
  QWidget *pTab = m_pTabWidget->widget (TRANSCODING_TAB);	//xxx
  //m_pTabWidget->removePage ( pTab );	//ooo
  m_pTabWidget->removeTab ( TRANSCODING_TAB );	//xxx
  delete pTab;
  //pTab = m_pTabWidget->page ( CHAPTERS_TAB );	//ooo
  pTab = m_pTabWidget->widget ( CHAPTERS_TAB );	//xxx
  //m_pTabWidget->removePage  ( pTab );		//ooo
  m_pTabWidget->removeTab  ( CHAPTERS_TAB );		//xxx
  delete pTab;
  //pTab = m_pTabWidget->page ( SUBTITLE_TAB );	//ooo
  pTab = m_pTabWidget->widget ( SUBTITLE_TAB );	//xxx
  //m_pTabWidget->removePage  ( pTab );		//ooo
  m_pTabWidget->removeTab  ( SUBTITLE_TAB );		//xxx
  delete pTab;
  //pTab = m_pTabWidget->page ( PROPERTIES_TAB );	//ooo
  pTab = m_pTabWidget->widget ( PROPERTIES_TAB );	//xxx
  //m_pTabWidget->removePage  ( pTab );		//ooo
  m_pTabWidget->removeTab  ( PROPERTIES_TAB );	//xxx
  delete pTab;
  // ALso no transcoding buttons will be needed.
  delete m_pButtonTranscode;
  delete m_pButtonEncodeNow;
  m_pButtonTranscode = NULL;
  m_pButtonEncodeNow = NULL;
  m_pEditStarting1   = m_pEditStarting2;
  m_pEditEnding1     = m_pEditEnding2;

  // Set the values .
  m_pEditX->setText (QString ("%1").arg(pMovieObject->rect().x()));
  m_pEditY->setText (QString ("%1").arg(pMovieObject->rect().y()));
  m_pEditWidth ->setText (QString ("%1").arg(pMovieObject->rect().width()));
  m_pEditHeight->setText (QString ("%1").arg(pMovieObject->rect().height()));

  qsFormat = QString ("hh:mm:ss.zzz");
  m_iMovieLength = -pMovieObject->duration ( ).msecsTo  ( QTime ( ) );

  m_pEditStarting2->setText ( pMovieObject->offset ( ).toString ( qsFormat ) );
  // m_pEditStarting2 must be set before calling maxEndTime
  QTime endingTime = maxEndTime ( ); //pMovieObject->offset ( ).addMSecs ( m_iMovieLength );
  QTime   clipTime ( pMovieObject->offset ( ) );
  clipTime = clipTime.addMSecs ( m_iMovieLength );
  if  ( m_iMovieLength > 0 )
        endingTime = clipTime;
  m_pEditEnding2->setText ( endingTime.toString  ( qsFormat ) );

  m_rectGeometry = pMovieObject->rect     ( );
  m_qsFileName   = pMovieObject->fileName ( );
  m_pCheckSameAsMenuLength->setChecked ( pMovieObject->isMenuLength ( ) );
  m_pEditEnding2->setEnabled         ( ! pMovieObject->isMenuLength ( ) );
  m_pButtonLoop->setEnabled          ( ! pMovieObject->isMenuLength ( ) );
  QString strLoop = "Stretch";
  if ( pMovieObject->loop ( ) == MovieObject::Loop )
       strLoop  =  "Loop";
  else if ( pMovieObject->loop ( ) == MovieObject::Stop )
       strLoop  =  "Stop";
  m_pButtonLoop->setText ( strLoop );

  Rgba theColor;
  ImageManipulator::colorKeying *pKey;
  ImageManipulator *pMan = &pMovieObject->manipulator ( );
  for ( t=0; t<(int)pMan->listColorKeys.count(); t++ )	{
    pKey = pMan->listColorKeys[t];
    theColor.setRgb ( pKey->theColor );
    //new ListViewColorItem ( m_pListViewCK, theColor, theColor.toString ( ), QString ("%1").arg(pKey->fDeltaColor ) );		//oooo
  }

  QFileInfo fileInfo(m_qsFileName);
  // and now on to initializing the dialog. Length : 00:07:23.123 File : Cool Video.mpg
  m_pTextTitle->setText ( tr ( "File : %1" ).arg ( fileInfo.fileName ( ) ) );

  // we'll init the MediaEngine here
  DVDMenu *pDVDMenu = pMovieObject->dvdMenu ( );
  initMediaInterface ( pDVDMenu );
  QString qsCaption = tr  ( "Menu Length : %1" ).arg ( pDVDMenu->getInterface ( )->timeDuration.toString ( qsFormat ) );
  //setCaption ( qsCaption );	//oooo

  connect ( m_pCheckSameAsMenuLength, SIGNAL ( clicked ( ) ), this, SLOT ( slotSameAsMenuLength ( ) ) );
  connect ( m_pButtonLoop,    SIGNAL ( clicked   ( ) ), this, SLOT ( slotLoop       ( ) ) );
  connect ( m_pEditEnding2,   SIGNAL ( lostFocus ( ) ), this, SLOT ( slotCheckEnd   ( ) ) );
  connect ( m_pEditStarting2, SIGNAL ( lostFocus ( ) ), this, SLOT ( slotCheckStart ( ) ) );
}

void DialogMovie::initMediaInterface ( DVDMenu *pDVDMenu )
{
  QFileInfo fileInfo;
  QString qsBackgroundName, qsFormat;
  int iThumbnailSize = 50;
  MediaInfo *pMediaInfo           = NULL;
  void      *pExistingMediaEngine = NULL;
  qsFormat = QString ("hh:mm:ss.zzz");

  if ( pDVDMenu )	{
    pMediaInfo = pDVDMenu->getDVDAuthor()->getMediaInfo ( );
    pExistingMediaEngine = pMediaInfo->getMediaEngine   ( );
    iThumbnailSize = pDVDMenu->getDVDAuthor()->getDragNDropContainer()->sizeThumbnail.width();
    if (m_qsFileName.isEmpty() || m_qsFileName.isNull() )	{
      if (pDVDMenu->getInterface()->qsMovieFileName.isNull())
        m_qsFileName = pDVDMenu->getInterface ( )->qsBackgroundFileName;
      else
        m_qsFileName = pDVDMenu->getInterface ( )->qsMovieFileName;
    }
    fileInfo.setFile (m_qsFileName);
    qsBackgroundName = QString (" : %1 <%2>").arg(fileInfo.fileName()).arg(pDVDMenu->getInterface()->timeDuration.toString(qsFormat));

    m_pTextTitle->setText (pDVDMenu->name() + qsBackgroundName);
  }
  // Here we create the MediaInterface (the widget) If the mediaengine in NULL or not ...
  m_pMediaInterface = MediaCreator::createPreferredWidget ( m_pMediaPreview, "ChapterXineWidget", pExistingMediaEngine );
  connect (m_pMediaInterface, SIGNAL(signalNewPosition(int, const QString&)), this, SLOT(slotNewPosition(int, const QString&)));

  //Q3GridLayout *pVideoLayout = new Q3GridLayout ( m_pMediaPreview, 1, 1, 2, 2, "MediaMainLayout" );	//ooo
  QGridLayout *pVideoLayout = new QGridLayout ( m_pMediaPreview );					//xxx
  pVideoLayout->addWidget ( m_pMediaInterface->getWidget ( ), 0, 0 );
  // This is taken care of in the polish () function for Xine and not necessary for MPlayer ...
  m_pMediaInterface->initMediaEngine ( );

  //Q3GridLayout *pLayout = new Q3GridLayout      ( m_pFrameControl, 1, 1, 2, 2, "pLayout" );	//ooo
  QGridLayout *pLayout = new QGridLayout      ( m_pFrameControl );				//xxx
  m_pStartStopControl  = new StartStopControl ( m_pFrameControl );

  if ( m_pMovieObject || m_pDVDMenu )
    m_pStartStopControl->initMe ( m_qsFileName, iThumbnailSize );  // get length of movieObject a bit later.
  else
    m_pStartStopControl->initMe ( m_qsFileName, iThumbnailSize, m_iMovieLength );

  pLayout->addWidget( m_pStartStopControl, 0, 0 );

  QTimer::singleShot ( 500, this, SLOT ( slotInitMediaInterface ( ) ) );	//oooo
}

void DialogMovie::slotInitMediaInterface ( )
{
  m_pMediaInterface->openMRL ( m_qsFileName );
  m_pMediaInterface->getScreenshot ( );
  m_pMediaInterface->setScreenshot ( 1.0f );

  QFileInfo fileInfo ( m_qsFileName );
  MediaInfo *pMediaInfo = NULL;  
  //int iThumbnailSize = 70;
  // Here we ensure the availability of a MediaInfo - object
  pMediaInfo = m_pMediaInterface->getMediaInfo ( );
  if ( ! pMediaInfo )
    return;

  QString qsFormat = QString ("hh:mm:ss.zzz");
  // here we extract some information + the screenshot.
  pMediaInfo->setFileName ( m_qsFileName );

  if ( m_pMovieObject ) { // For MovieObjects, the actual video length is not the clip length
    QTime zeroTime;
    long  iMSecOffset = zeroTime.msecsTo ( m_pMovieObject->offset ( ) );
    unsigned long int iMovieLength = pMediaInfo->getLength ( );
    m_pMediaInterface->setScreenshot ( iMSecOffset );

    DVDMenu *pDVDMenu = m_pMovieObject->dvdMenu ( );
    long iMenuLength  = zeroTime.msecsTo  ( pDVDMenu->getInterface ( )->timeDuration );

    if ( iMenuLength >= (long)iMovieLength )  {
      //m_pCheckSameAsMenuLength->setChecked ( FALSE );		//ooo
      m_pCheckSameAsMenuLength->setChecked ( false );		//xxx
      //m_pCheckSameAsMenuLength->setEnabled ( FALSE );		//ooo
      m_pCheckSameAsMenuLength->setEnabled ( false );		//xxx
      slotSameAsMenuLength ( );
    }

    if ( iMovieLength < 1 )
      iMovieLength = m_iMovieLength;
    m_iMovieLength =   iMovieLength;
    //m_pEditEnding2->setText ( maxEndTime ( ).toString  ( qsFormat ) );
    m_pStartStopControl->createThumbnails ( m_iMovieLength );
  }
  else if ( m_pDVDMenu ) {
    QTime zeroTime;
    long  iMSecOffset = zeroTime.msecsTo ( m_pDVDMenu->getInterface ( )->timeOffset );
    unsigned long int iMovieLength = pMediaInfo->getLength ( );
    m_pMediaInterface->setScreenshot ( iMSecOffset );
    if ( iMovieLength < 1 )
      iMovieLength = m_iMovieLength;
    m_iMovieLength =   iMovieLength;
    m_pStartStopControl->createThumbnails ( m_iMovieLength );
  }

  QTime   lengthTime;
  lengthTime = lengthTime.addMSecs ( m_iMovieLength );
  QString qsLength = lengthTime.toString ( qsFormat );

  m_pTextTitle->setText   ( tr ("Length : <%1> File : %2").arg( qsLength ).arg ( fileInfo.fileName ( ) ) );
//  setCaption ( fileInfo.fileName ( ) );

  // next is to set the satrting / ending points ...
  QTimer::singleShot ( 50, this, SLOT ( slotSetStartStop ( ) ) );
  connect ( m_pStartStopControl, SIGNAL ( signalSliderValues ( float, float)), this, SLOT ( slotSliderValues   ( float, float ) ) );
  connect ( m_pStartStopControl, SIGNAL ( signalSliderReleased ( bool ) ),     this, SLOT ( slotSliderReleased ( bool ) ) );
  connect ( m_pSliderVolume,     SIGNAL ( valueChanged         ( int  ) ),     this, SLOT ( slotNewVolume      ( int  ) ) );
}

void DialogMovie::initAudioTab ( SourceFileInfo *pInfo, DVDMenu *pMenu )
{
  int t;
  Utils      theUtils;
  Subtitles *pSubtitle;
  if ( pInfo ) {
    for ( t=0; t<MAX_SUBTITLES; t++ ) {
      pSubtitle = pInfo->arraySubtitles[t];
      if ( pSubtitle ) {
        // #, code, Language, file
        //new Q3ListViewItem ( m_pListViewSubtitles, QString ( "%1" ).arg ( pSubtitle->m_iSubtitleNumber ), pSubtitle->m_qsIso639, theUtils.iso639 ( pSubtitle->m_qsIso639, false ), QString ( "%1" ).arg ( pSubtitle->m_listOfSubtitles.count ( ) ) );	//oooo
	//new QListViewItem ( m_pListViewSubtitles, QString ( "%1" ).arg ( pSubtitle->m_iSubtitleNumber ), pSubtitle->m_qsIso639, theUtils.iso639 ( pSubtitle->m_qsIso639, false ), QString ( "%1" ).arg ( pSubtitle->m_listOfSubtitles.count ( ) ) );	//xxx
      }
    }
    // The first pass we figure out the set audio tracks.
    createAudioList ( );
  }
  else if ( pMenu ) {
    CDVDMenuInterface *pInterface = pMenu->getInterface  ( );
    Utils theUtils;
    for ( t=0; t<(int)pInterface->listSoundEntries.count ( ); t++ ) {
      Audio *pAudio = pInterface->listSoundEntries[t];
      QString qsCode = theUtils.iso639 ( pAudio->qsIso639, false );
      if ( pAudio ) {
        //new Q3ListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( pAudio->iAudioNumber), pAudio->qsIso639, qsCode, pAudio->qsFileName );	//oooo
	//new QListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( pAudio->iAudioNumber), pAudio->qsIso639, qsCode, pAudio->qsFileName );	//xxx
      }
    }
    m_pFrameSubtitles->hide ( );
  }

  m_pButtonEditAudio     ->setEnabled ( false );
  m_pButtonDeleteAudio   ->setEnabled ( false );
  m_pButtonEditSubtitle  ->setEnabled ( false );
  m_pButtonDeleteSubtitle->setEnabled ( false );
  connect ( m_pButtonAddAudio,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAddAudio       ( ) ) );
  connect ( m_pButtonEditAudio,      SIGNAL ( clicked ( ) ), this, SLOT ( slotEditAudio      ( ) ) );
  connect ( m_pButtonDeleteAudio,    SIGNAL ( clicked ( ) ), this, SLOT ( slotDeleteAudio    ( ) ) );
  connect ( m_pButtonAddSubtitle,    SIGNAL ( clicked ( ) ), this, SLOT ( slotAddSubtitle    ( ) ) );
  connect ( m_pButtonEditSubtitle,   SIGNAL ( clicked ( ) ), this, SLOT ( slotEditSubtitle   ( ) ) );
  connect ( m_pButtonDeleteSubtitle, SIGNAL ( clicked ( ) ), this, SLOT ( slotDeleteSubtitle ( ) ) );
  //connect ( m_pListViewAudio,        SIGNAL ( pressed ( Q3ListViewItem * )), this, SLOT ( slotAudioSelectionChanged    ( Q3ListViewItem * ) ) );	//oooo
  //connect ( m_pListViewAudio,        SIGNAL ( pressed ( Q3ListViewItem * )), this, SLOT ( slotAudioSelectionChanged    ( Q3ListViewItem * ) ) );	//xxx
  //connect ( m_pListViewSubtitles,    SIGNAL ( pressed ( Q3ListViewItem * )), this, SLOT ( slotSubtitleSelectionChanged ( Q3ListViewItem * ) ) );	//oooo
  //connect ( m_pListViewSubtitles,    SIGNAL ( pressed ( Q3ListViewItem * )), this, SLOT ( slotSubtitleSelectionChanged ( Q3ListViewItem * ) ) );	//xxx
}

void DialogMovie::createAudioList ( )
{
  // Based on m_sourceFileInfo
  int t;
  Utils theUtils;
  //m_pListViewAudio->clear ( );	//oooo
  Audio *arrayAudio[MAX_AUDIO], *pAudio;
  int iMaxAudio = 0; // Audio Track sohould always be displayed.
  for ( t=0; t<MAX_AUDIO; t++ )
    arrayAudio[t] = NULL;
  for ( t=0; t<MAX_AUDIO; t++ )  {
    pAudio = m_sourceFileInfo.arrayAudio[t];
    if ( pAudio )  {
      if ( iMaxAudio < pAudio->iAudioNumber )
           iMaxAudio = pAudio->iAudioNumber;
      if ( pAudio->iAudioNumber < MAX_AUDIO )
           arrayAudio[pAudio->iAudioNumber] = pAudio;
    }
  }
  iMaxAudio++;
  // Note: m_pParentSourceFileEntry should be set.
  for ( t=0; t<iMaxAudio; t++ ) {
    pAudio = arrayAudio[t]; //pInfo->arrayAudio[t];
    if ( pAudio ) {
      // #, code, Language, file
      //new Q3ListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( pAudio->iAudioNumber ), pAudio->qsIso639, theUtils.iso639 ( pAudio->qsIso639, false ), pAudio->qsFileName );	//oooo
      //new QListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( pAudio->iAudioNumber ), pAudio->qsIso639, theUtils.iso639 ( pAudio->qsIso639, false ), pAudio->qsFileName );	//xxx
    }
    else if ( t == 0 )  {
      QFileInfo fileInfo ( m_sourceFileInfo.qsFileName );
      QString qsAudioName = tr ( "Audio track of : " ) + fileInfo.fileName ( );
      //new Q3ListViewItem  ( m_pListViewAudio, QString ( "%1" ).arg ( t ), "--", "---", ( m_sourceFileInfo.bHasAudioTrack ) ? qsAudioName : tr ( "No Audio" ) );	//oooo
      //new Q3istViewItem  ( m_pListViewAudio, QString ( "%1" ).arg ( t ), "--", "---", ( m_sourceFileInfo.bHasAudioTrack ) ? qsAudioName : tr ( "No Audio" ) );	//xxx
    }
    else {
      //new Q3ListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( t ), "--", "---", tr ( "No Audio" ) );	//oooo
      //new QListViewItem ( m_pListViewAudio, QString ( "%1" ).arg ( t ), "--", "---", tr ( "No Audio" ) );	//xxx
    }
  }
}

void DialogMovie::timerEvent ( QTimerEvent *pEvent )
{
/*  if ( pEvent->timerId ( ) == MEDIASCANNER_EVENT + 3 ) {  // ExecuteJob::TYPE_SUBTITLES ...
    //m_pListViewSubtitles->clear ( );						//oooo
    //m_pListViewSubtitles->model()->removeRows( 0, model()->rowCount() );	//xxx
    Utils theUtils;
    for ( int t=0; t<MAX_SUBTITLES; t++ ) {
      Subtitles *pSubtitle = m_sourceFileInfo.arraySubtitles[t];
      if ( pSubtitle ) {
	// #, code, Language, file
	//new Q3ListViewItem ( m_pListViewSubtitles, QString ( "%1" ).arg ( pSubtitle->m_iSubtitleNumber ), pSubtitle->m_qsIso639, theUtils.iso639 ( pSubtitle->m_qsIso639, false ), QString ( "%1" ).arg ( pSubtitle->m_listOfSubtitles.count ( ) ) );	//oooo
	//new Q3ListViewItem ( m_pListViewSubtitles, QString ( "%1" ).arg ( pSubtitle->m_iSubtitleNumber ), pSubtitle->m_qsIso639, theUtils.iso639 ( pSubtitle->m_qsIso639, false ), QString ( "%1" ).arg ( pSubtitle->m_listOfSubtitles.count ( ) ) );	//xxx
      }
    }
  }

  //uiDialogMovie::timerEvent ( pEvent );	//ooo
  QDialog::timerEvent ( pEvent );		//xxx
  */	//oooo
}

void DialogMovie::slotSameAsMenuLength ( )
{
  bool bMenuLength = m_pCheckSameAsMenuLength->isChecked ( );
  m_pEditEnding2->setEnabled ( ! bMenuLength );
  if ( ! bMenuLength && m_pMovieObject )  {
    DVDMenu *pDVDMenu = m_pMovieObject->dvdMenu  ( );
    m_pMovieObject->setDuration ( pDVDMenu->getInterface ( )->timeDuration );
  }
  m_pEditEnding2->setText   ( maxEndTime (  ).toString  ( "hh:mm:ss.zzz" ) );
  m_pButtonLoop->setEnabled ( ! bMenuLength );

  if ( bMenuLength ) {
    // we should set the stopSlider and the duration of the MovieObject accordingly
  }
}

void DialogMovie::slotMenuLoop ( )
{
  // The user wants the menu video to loop.
  // So we need to
  if ( m_pCheckLoop->isChecked ( ) )  {
    // Gray out Pause after and set to '0'
    m_pSpinBoxPause->setEnabled ( false );
    m_pSpinBoxPause->setValue   ( 0 );
    // Gray out Post command and fill in "Loop"
    m_pComboPostCommand->setEnabled     (  false );
    m_pComboPostCommand->setCurrentText ( "Loop" );
  }
  else  {
    // Enable Pause after and set to '-1'
    m_pSpinBoxPause->setEnabled ( true );
    m_pSpinBoxPause->setValue   (  -1  );
    // Gray out Post command and fill in "Loop"
    m_pComboPostCommand->setEnabled     ( true );
    m_pComboPostCommand->setCurrentText ( " jump vmgm menu 1; " );
  }
}

QTime DialogMovie::maxEndTime ( )
{
  QTime movieTime, endingTime, startTime, duration, zeroTime;
  // Only used with MovieObject
  if ( ! m_pMovieObject )
    return endingTime;

  DVDMenu *pDVDMenu = m_pMovieObject->dvdMenu  ( );
  movieTime = zeroTime.addMSecs ( m_iMovieLength );
  startTime = QTime::fromString ( m_pEditStarting2->text ( ) );
  duration  = m_pMovieObject->duration ( );
  if ( ! pDVDMenu )
    return movieTime;

  // if the duration of the movie exceeds the length of the DVDMenu, we'll have to cut back.
  if ( ( m_pCheckSameAsMenuLength->isChecked ( ) ) ||
       ( duration > pDVDMenu->getInterface ( )->timeDuration ) )
         duration = pDVDMenu->getInterface ( )->timeDuration;

  endingTime = m_pMovieObject->offset    ( ).addMSecs ( zeroTime.msecsTo ( duration ) );
  endingTime = endingTime.addMSecs ( zeroTime.msecsTo ( startTime ) );
  // Now that we have the theoretical end time let us check if the movieLength is long enough
  if ( endingTime > movieTime )
       endingTime = movieTime;

  return endingTime;
}

bool DialogMovie::transcodeEnabled ()
{
  // If the background color of the title is set to -RED- then we are in the transcoding mode
  //return ( m_pTextTitle->paletteBackgroundColor ( ) == QColor ( COLOR_TRANSCODE ) );	//oooo
  return ( false );									//xxx
}

void DialogMovie::initTranscodeTab ()
{
  // DVD restrictions ...
  // NTSC : 720x480 704x480 352x480 352x240 * 29.97Hz
  // PAL  : 720x576 704x576 352x576 352x288 * 25Hz
  // Mpeg2: up to 9800 kbps
  // Mpeg1: 352x288 or 352x240 limited to 4:3 up to 1856 kbps
  // Audio bitrate : 32 .. 1536 kbps
  // Video bitrate : Mpeg2 max9800kbps Mpeg1 max 1856 kbps
  // Allow for muxing with this error.
  // If Warn: skipping sector, waiting for first VOBU, then simply re-multiplex. (check kdvdauthor)

  QFileInfo fileInfo ( m_sourceFileInfo.qsFileName );
  Utils theUtils;
  //if ( fileInfo.extension ( ).lower ( ) != "dv" ) {	//ooo
  if ( fileInfo.suffix ( ).toLower ( ) != "dv" ) {	//xxx
    m_pCheckCreateSubtitles->setEnabled ( false );
    m_pComboLanguage->setEnabled        ( false );
  }
  else {
    QString qsCode, qsLang = "something";
    int t=0;
    while ( ! qsLang.isEmpty ( ) ) {
      qsLang = theUtils.iso639 ( qsCode, false, t++ );
      //m_pComboLanguage->insertItem ( qsLang );		//ooo
      m_pComboLanguage->addItem ( qsLang );		//xxx
    }
    m_pComboLanguage->setCurrentText ( "English" );
  }
  // Next step is to set the check (or not)
  Subtitles *pSubtitle;
  int t;
  bool bSetCheck = false;
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    pSubtitle = m_sourceFileInfo.arraySubtitles [ t ];
    if ( ! pSubtitle )
      continue;
    if ( ( pSubtitle->m_subtitleState == Subtitles::STATE_META_INFO  )  || 
         ( pSubtitle->m_subtitleState == Subtitles::STATE_EXTRACTING ) )  {
      bSetCheck = true;
      break;
    }
  }
  m_pCheckCreateSubtitles->setChecked ( bSetCheck );

  if (!m_sourceFileInfo.pTranscodeInterface) {
    QString qsDummy;
    //if ( m_sourceFileInfo.qsVideoFormat.lower ( ) == "pal" ) {		//ooo
    if ( m_sourceFileInfo.qsVideoFormat.toLower ( ) == "pal" ) {		//xxx
      m_pComboVideoOut->setCurrentText ( "PAL" );
      slotVideoOutChanged ( qsDummy );
    }
    return;
  }

  m_sourceFileInfo.pTranscodeInterface->qsVideoFormat.remove ( "Keep " );
  m_sourceFileInfo.pTranscodeInterface->qsVideoType.remove   ( "Keep " );
  m_sourceFileInfo.pTranscodeInterface->qsResolution.remove  ( "Keep " );
  m_sourceFileInfo.pTranscodeInterface->qsRatio.remove       ( "Keep " );
  m_sourceFileInfo.pTranscodeInterface->qsAudioType.remove   ( "Keep " );

  //m_pComboVideoOut->insertItem        (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsVideoFormat), 0);	//ooo
  m_pComboVideoOut->addItem        (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsVideoFormat), 0);		//xxx
  //m_pComboVideoType->insertItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsVideoType  ), 0);	//ooo
  m_pComboVideoType->addItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsVideoType  ), 0);		//xxx
  //m_pComboVideoResolution->insertItem (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsResolution ), 0);	//ooo
  m_pComboVideoResolution->addItem (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsResolution ), 0);		//xxx
  if (m_sourceFileInfo.pTranscodeInterface->iVideoBitrate < 0)
    m_sourceFileInfo.pTranscodeInterface->iVideoBitrate *= -1;
  //m_pComboVideoBitrate->insertItem    (QString ("Keep %1").arg(theUtils.longToNorm(m_sourceFileInfo.pTranscodeInterface->iVideoBitrate) + QString ("bps")), 0);	//ooo
  m_pComboVideoBitrate->addItem    (QString ("Keep %1").arg(theUtils.longToNorm(m_sourceFileInfo.pTranscodeInterface->iVideoBitrate) + QString ("bps")), 0);	//xxx
  if (m_sourceFileInfo.pTranscodeInterface->fFrameRate < 0.0)
    m_sourceFileInfo.pTranscodeInterface->fFrameRate *= -1.0;
  //m_pComboVideoRate->insertItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->fFrameRate), 0);	//ooo
  m_pComboVideoRate->addItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->fFrameRate), 0);	//xxx
  //m_pComboVideoAspect->insertItem     (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsRatio), 0);	//ooo
  m_pComboVideoAspect->addItem     (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsRatio), 0);		//xxx

  //m_pComboAudioOut->insertItem        ("Keep Original", 0);	//ooo
  m_pComboAudioOut->addItem        ("Keep Original", 0);		//xxx
  //m_pComboAudioType->insertItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsAudioType), 0);	//ooo
  m_pComboAudioType->addItem       (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->qsAudioType), 0);	//xxx

  if (m_sourceFileInfo.pTranscodeInterface->iAudioBitrate < 0)
    m_sourceFileInfo.pTranscodeInterface->iAudioBitrate *= -1;
  //m_pComboAudioBitrate->insertItem    (QString ("Keep %1").arg(theUtils.longToNorm(m_sourceFileInfo.pTranscodeInterface->iAudioBitrate) + QString ("bps")), 0);	//ooo
  m_pComboAudioBitrate->addItem    (QString ("Keep %1").arg(theUtils.longToNorm(m_sourceFileInfo.pTranscodeInterface->iAudioBitrate) + QString ("bps")), 0);	//xxx
  if (m_sourceFileInfo.pTranscodeInterface->iSample < 0)
    m_sourceFileInfo.pTranscodeInterface->iSample *= -1;
  //m_pComboAudioSampleRate->insertItem (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->iSample), 0);	//ooo
  m_pComboAudioSampleRate->addItem (QString ("Keep %1").arg(m_sourceFileInfo.pTranscodeInterface->iSample), 0);		//xxx

  // And make the transcode - tab visible again ...
  slotTranscode ();
}

void DialogMovie::slotSetStartStop ()
{
  int iStarting, iEnding;
  QTime      zeroTime;
  iStarting = 0;
  iEnding = m_iMovieLength;

  if (m_pDVDMenu)	{  
    iStarting = zeroTime.msecsTo (m_pDVDMenu->getInterface()->timeOffset);
    iEnding   = iStarting + zeroTime.msecsTo (m_pDVDMenu->getInterface()->timeDuration);
  }
  else if (m_pMovieObject)	{  
    iStarting = zeroTime.msecsTo (m_pMovieObject->offset()); 
 
    iEnding   = iStarting + zeroTime.msecsTo(m_pMovieObject->duration());
  }
  else if (m_sourceFileInfo.pTranscodeInterface)	{

    iStarting = m_sourceFileInfo.pTranscodeInterface->iStartingAt;
    iEnding   = m_sourceFileInfo.pTranscodeInterface->iEndingAt;
  }

  //m_pStartStopControl->setStart ((float) iStarting / m_iMovieLength);			//ooo
  m_pStartStopControl->setStart (static_cast<float> ( iStarting / m_iMovieLength) );	//xxx

  //m_pStartStopControl->setEnd   ((float) iEnding   / m_iMovieLength);			//ooo
  m_pStartStopControl->setEnd   ( static_cast<float> ( iEnding   / m_iMovieLength ) );	//xxx
}

SourceFileInfo *DialogMovie::sourceFileInfo ( )
{
  return &m_sourceFileInfo;
}

void DialogMovie::slotNewVolume ( int iNewVolume )
{
  // slider min = 0 slider max = 100
  float fNewVolume = (100 - iNewVolume) / 100.0;

  m_pMediaInterface->setVolume ( fNewVolume );
}

void DialogMovie::slotVideoOutChanged (const QString &)
{
  // NTSC : 720x480 704x480 352x480 352x240 * 29.97Hz
  // PAL  : 720x576 704x576 352x576 352x288 * 25Hz
  m_pComboVideoRate->clear ();
  m_pComboVideoAspect->clear ();
  m_pComboVideoResolution->clear ();
//QString qsCurrentResolution = m_pComboVideoResolution->currentText ();
  //QString qsNewString = m_pComboVideoOut->currentText ().upper();	//ooo
  QString qsNewString = m_pComboVideoOut->currentText ().toUpper();	//xxx

  MediaInfo *pMediaInfo = NULL;
  if (m_pMediaInterface)
    m_pMediaInterface->getMediaInfo ();
  if (pMediaInfo)	{
    //m_pComboVideoResolution->insertItem (QString ("Keep %1").arg(pMediaInfo->getResolutionString()));	//ooo
    m_pComboVideoResolution->addItem (QString ("Keep %1").arg(pMediaInfo->getResolutionString()));	//xxx
    //m_pComboVideoAspect->insertItem     (QString ("Keep %1").arg(pMediaInfo->getRatio()));		//ooo
    m_pComboVideoAspect->addItem     (QString ("Keep %1").arg(pMediaInfo->getRatio()));			//xxx
    //m_pComboVideoRate->insertItem       (QString ("Keep %1").arg(pMediaInfo->getFPS()));		//ooo
    m_pComboVideoRate->addItem       (QString ("Keep %1").arg(pMediaInfo->getFPS()));			//xxx
  }
  if ( (qsNewString == "PAL") || (qsNewString == "CUSTOM") )	{
    if (m_pComboVideoType->currentText () != "Mpeg 1")	{
      //m_pComboVideoResolution->insertItem ("720x576");	//ooo
      m_pComboVideoResolution->addItem ("720x576");		//xxx
      //m_pComboVideoResolution->insertItem ("704x576");	//ooo
      m_pComboVideoResolution->addItem ("704x576");		//xxx
      //m_pComboVideoResolution->insertItem ("352x576");	//ooo
      m_pComboVideoResolution->addItem ("352x576");		//xxx
    }
    //m_pComboVideoResolution->insertItem ("352x288");		//ooo
    m_pComboVideoResolution->addItem ("352x288");		//xxx
    //m_pComboVideoRate->insertItem ("25");			//ooo
    m_pComboVideoRate->addItem ("25");				//xxx
  }
  if ( (qsNewString == "NTSC") || (qsNewString == "CUSTOM") )	{
    if (m_pComboVideoType->currentText () != "Mpeg 1")	{
      //m_pComboVideoResolution->insertItem ("720x480");	//ooo
      m_pComboVideoResolution->addItem ("720x480");		//xxx
      //m_pComboVideoResolution->insertItem ("704x480");	//ooo
      m_pComboVideoResolution->addItem ("704x480");		//xxx
      //m_pComboVideoResolution->insertItem ("352x480");	//ooo
      m_pComboVideoResolution->addItem ("352x480");		//xxx
    }
    //m_pComboVideoResolution->insertItem ("352x240");		//ooo
    m_pComboVideoResolution->addItem ("352x240");		//xxx
    //m_pComboVideoRate->insertItem ("29.97");			//ooo
    m_pComboVideoRate->addItem ("29.97");			//xxx
  }

  //m_pComboVideoAspect->insertItem ("4:3");			//ooo
  m_pComboVideoAspect->addItem ("4:3");				//xxx
  //m_pComboVideoAspect->insertItem ("1:1");			//ooo
  m_pComboVideoAspect->addItem ("1:1");				//xxx
  if (m_pComboVideoType->currentText () != "Mpeg 1")	{
    //m_pComboVideoAspect->insertItem ("16:9");			//ooo
    m_pComboVideoAspect->addItem ("16:9");			//xxx
    //m_pComboVideoAspect->insertItem ("2.21:1");		//ooo
    m_pComboVideoAspect->addItem ("2.21:1");			//xxx
  }
}

void DialogMovie::slotAutoChapters ()
{
  uint t, i, iDeltaStep, iStarting, iEnding, iChapter, iPreviousChapter;
  int iTotalLength, iNumberOfNewChapters;
  QString qsTimeString;
  Utils theUtil;
  // Here we read in the list of currently generated chapters
  // then we convert them into milliseconds, and store them in an array
  // And finally we generate the chapters, sort the array and put it back into the
  // m_pListBoxChapters, and m_sourceFileInfo - structures.
  int *pArrayExistingChapters = new int[m_pListBoxChapters->count()];
  for (t=0;t<(uint)m_pListBoxChapters->count(); t++)	{
    //qsTimeString = m_pListBoxChapters->text(t);		//ooo
    qsTimeString = m_pListBoxChapters->item(t)->text();	//xxx
    pArrayExistingChapters[t] = theUtil.getMsFromString (qsTimeString);
  }
  iStarting  = theUtil.getMsFromString ( m_pEditStarting1->text ( ) );
  iEnding    = theUtil.getMsFromString ( m_pEditEnding1->text   ( ) );
  iDeltaStep = theUtil.getMsFromString ( m_pEditEvery->text     ( ) );
  iTotalLength = (int)(iEnding - iStarting);
  // a DeltaStep of 0 would generate infinite number of chapters
  if ( ( iDeltaStep < 1 ) || ( iTotalLength < 1 ) ) {
    delete []pArrayExistingChapters;
    return;
  }
  if ( iDeltaStep > (uint)iTotalLength )
       iDeltaStep = iTotalLength;
  iNumberOfNewChapters = (int)((float)iTotalLength / (float)iDeltaStep) + 1;

  QList<int> listAllChapters;	//oxx
  // Okay this is the worst sort algo around but we would'nt sort thousands of chapters, now would we ???
  i = 0;
  for (t=0;t<(uint)iNumberOfNewChapters;t++)	{
    iChapter = iStarting + t * iDeltaStep;
    for (i=0;i<(uint)m_pListBoxChapters->count();i++)	{
      // If we find an entry which is the same as a calculated chapter we should skip it ...
      if (pArrayExistingChapters[i] == (int)iChapter)
        pArrayExistingChapters[i] = -1;
      // If we find an entry which is smaller but not 0, then we should include them ...
      else if ( ( pArrayExistingChapters[i] < (int)iChapter ) && 
          ( pArrayExistingChapters[i] >       -1 ) )  {
        listAllChapters.append ( pArrayExistingChapters[i] );
        // Here we ensure the next time this entry is higher
        pArrayExistingChapters[i] = -1;
      }
    }
    listAllChapters.append (iChapter);
  }

  m_pListBoxChapters->clear ();
  iChapter = iPreviousChapter = 0;
  for (t=0;t<(uint)listAllChapters.count();t++)	{	//ox
    iChapter = listAllChapters[t];
    if (iChapter >= iPreviousChapter)
      //m_pListBoxChapters->insertItem(theUtil.getStringFromMs (iChapter));	//ooo
      m_pListBoxChapters->addItem(theUtil.getStringFromMs (iChapter));	//xxx
    iPreviousChapter = iChapter;
  }

  delete []pArrayExistingChapters;
}

void DialogMovie::addChapter ( QString &qsChapter )
{
  // Here we add a chapter in the right space,
  // but only if this chapter does not already exists ...
  Utils theUtil;
  int iChapter, iAddChapter;
  uint t;
  iChapter = 0;
  iAddChapter = theUtil.getMsFromString  ( qsChapter ); //m_pEditAdd->text() );
  for ( t=0; t<(uint)m_pListBoxChapters->count ( ); t++ )  {
    //iChapter = theUtil.getMsFromString   ( m_pListBoxChapters->text ( t ) );		//ooo
    iChapter = theUtil.getMsFromString   ( m_pListBoxChapters->item ( t )->text ( ) );	//xxx
    if ( iAddChapter == iChapter )
      break;
    else if ( iAddChapter < iChapter )  {
      //m_pListBoxChapters->insertItem ( theUtil.getStringFromMs ( iAddChapter ), t );	//ooo
      m_pListBoxChapters->addItem ( theUtil.getStringFromMs ( iAddChapter ) );		//xxx
      break;
    }
  }
  // and in case the Chpater is to be added at the end ...
  if ( iAddChapter > iChapter )
    //m_pListBoxChapters->insertItem ( theUtil.getStringFromMs ( iAddChapter ) );	//ooo
    m_pListBoxChapters->addItem ( theUtil.getStringFromMs ( iAddChapter ) );		//xxx
}

void DialogMovie::slotAddChapter ( )
{
  QString qsChapter = m_pEditAdd->text ( );
  addChapter ( qsChapter );
}

void DialogMovie::slotDelChapters ()
{
  // This function will remove all chapters which are currently selected in m_pListBoxChapters
  uint t;
  //QList<Q3ListBoxItem *> listItems;	//ooo
  QList<QListWidgetItem *> listItems;	//xxx
  for (t=0;t<(uint)m_pListBoxChapters->count();t++)	{
    //if (m_pListBoxChapters->isSelected (t))		//ooo
    if (m_pListBoxChapters->item ( t )->isSelected ( ))	//xxx
      listItems.append (m_pListBoxChapters->item(t));
  }
  for (t=0;t<(uint)listItems.count();t++)
      delete listItems[t];
}

void DialogMovie::slotTranscode ()
{
  if (m_pTranscodeTab)	{
    m_pTabWidget->addTab ( m_pTranscodeTab, tr ("Transcoding") );
    m_pTranscodeTab = NULL;
    //m_pTabWidget->setCurrentPage                  ( 3 );	//ooo
    m_pTabWidget->setCurrentIndex                   ( 3 );	//xxx
    m_pButtonTranscode->setText                   ( tr ("<<< Transcode") );
    //m_pTextTitle->setBackgroundColor              ( QColor ( COLOR_TRANSCODE ) );		//ooo
    QPalette palette;										//xxx
    palette.setBrush(m_pTextTitle->backgroundRole(), QColor ( COLOR_TRANSCODE ));		//xxx
    m_pTextTitle->setPalette(palette);								//xxx
    //m_pTextTranscoding->setBackgroundColor        ( QColor ( COLOR_TRANSCODE ) );		//ooo
    palette.setBrush(m_pTextTranscoding->backgroundRole(), QColor ( COLOR_TRANSCODE ));		//xxx
    m_pTextTranscoding->setPalette(palette);							//xxx
    //m_pTextTranscoding->setPaletteBackgroundColor ( QColor ( COLOR_TRANSCODE ) );		//ooo
    //palette.setBrush(m_pTextTranscoding->backgroundRole(), QColor ( COLOR_TRANSCODE ));	//xxx
    //m_pTextTranscoding->setPalette(palette);							//xxx
    //m_pFrameHeader->setPaletteBackgroundColor     ( QColor ( COLOR_TRANSCODE ) );		//ooo
    palette.setBrush(m_pFrameHeader->backgroundRole(), QColor ( COLOR_TRANSCODE ));		//xxx
    m_pFrameHeader->setPalette(palette);							//xxx
    m_pFrameHeader->setAutoFillBackground(true);						//xxx
    QTimer::singleShot       ( 1,  this,  SLOT    ( slotBlink ( ) ) );
    m_pButtonEncodeNow->show ( );
  }
  else	{
    //m_pTranscodeTab = m_pTabWidget->page          ( m_pTabWidget->count ( ) - 1 );		//ooo
    m_pTranscodeTab = m_pTabWidget->widget          ( m_pTabWidget->count ( ) - 1 );		//xxx
    //m_pTabWidget->removePage                      ( m_pTranscodeTab );			//ooo
    m_pTabWidget->removeTab                         ( m_pTabWidget->indexOf(m_pTranscodeTab) );	//xxx
    m_pButtonTranscode->setText                     ( tr ("Transcode >>>") );
    //m_pTextTitle->setBackgroundColor              ( QColor ( COLOR_NOTRANSCODE ) );		//ooo
    QPalette palette;										//xxx
    palette.setBrush(m_pTextTitle->backgroundRole(), QColor ( COLOR_NOTRANSCODE ));		//xxx
    m_pTextTitle->setPalette(palette);								//xxx
    //m_pTextTranscoding->setBackgroundColor        ( QColor ( COLOR_NOTRANSCODE ) );		//ooo
    palette.setBrush(m_pTextTranscoding->backgroundRole(), QColor ( COLOR_NOTRANSCODE ));	//xxx
    m_pTextTranscoding->setPalette(palette);							//xxx
    //m_pTextTranscoding->setPaletteForegroundColor ( QColor ( COLOR_NOTRANSCODE ) );		//ooo
    //palette.setBrush(m_pTextTranscoding->backgroundRole(), QColor ( COLOR_NOTRANSCODE ));	//xxx
    //m_pTextTranscoding->setPalette(palette);							//xxx
    //m_pFrameHeader->setPaletteBackgroundColor     ( QColor ( COLOR_NOTRANSCODE ) );		//ooo
    palette.setBrush(m_pFrameHeader->backgroundRole(), QColor ( COLOR_NOTRANSCODE ));		//xxx
    m_pFrameHeader->setPalette(palette);							//xxx
    m_pFrameHeader->setAutoFillBackground(true);						//xxx
    m_pButtonEncodeNow->hide      (      );
    m_pStartStopControl->setStart ( 0.0f );
    m_pStartStopControl->setEnd   ( 1.0f );
  }
}

void DialogMovie::slotEncodeNow ()
{
	MessageBox::warning (NULL, tr("Warning"), tr("Not yet implemented"), QMessageBox::Ok, QMessageBox::NoButton);
}

void DialogMovie::slotPlay  ()
{
  if (m_pMediaInterface)	{
    bool bEnable = true;
    if      (m_pMediaInterface->isPaused ())
       m_pMediaInterface->play ();
    else if (m_pMediaInterface->isPlaying())
      m_pMediaInterface->pause();
    else
      m_pMediaInterface->playMRL(m_qsFileName);

    if ( m_pMediaInterface->isPaused ( ) )  {
      m_pButtonPlay->setText (tr("Play"));
      bEnable = false;
    }
    else
      m_pButtonPlay->setText (tr("Pause"));

    m_pButtonStart->setEnabled ( bEnable );
    m_pButtonEnd  ->setEnabled ( bEnable );
  }
}

void DialogMovie::slotStop  ()
{
	if (m_pMediaInterface)	{
		// In case the state is paused we should get back to play mode before stopping
		if (m_pMediaInterface->isPaused ())
			m_pMediaInterface->play ();
		m_pMediaInterface->stop ();
		// The button should display "Play" now
		m_pButtonPlay->setText (tr("Play"));
		// and then logically we should put the screen to the beginnning.
		m_pMediaInterface->getScreenshot ( );
		m_pButtonStart->setEnabled ( false );
		m_pButtonEnd  ->setEnabled ( false );
	}
}

void DialogMovie::slotSetThumbnail ()
{
  // The user presses SetAsThumbnail ...
  Utils theUtils;
  QRegExp rx ( "<[^>]*>" );
  QString qsCurrentTime =  m_pLabelCurrentPos->text ( );
  qsCurrentTime.replace ( rx, "" ); // = rx.cap ( 0 );

  long  iMSecOffset     = theUtils.getMsFromString ( qsCurrentTime );
  if  ( m_pEditStarting1 ) // indicates that we are coming from the Sourcetoolbar-SourceFileInfo
    emit ( signalSetThumbnail ( iMSecOffset ) ); // calls SourceToolbar::slotSetThumbnail ( long )

  m_sourceFileInfo.iMSecPreview = iMSecOffset;
}

void DialogMovie::slotStart ()
{	// This function will set the Start offset of the video clip
	if (m_qsPosition.length () < 8)
		m_pEditStarting1->setText (QString ("0") + m_qsPosition + QString (".000"));
	else
		m_pEditStarting1->setText (m_qsPosition);
	// But we also need to make sure that the start is not set before the end ...
	setStartStop (true);
}

void DialogMovie::slotEnd   ()
{
	if (m_qsPosition.length () < 8)
		m_pEditEnding1->setText (QString ("0") + m_qsPosition + QString (".000"));
	else
		m_pEditEnding1->setText (m_qsPosition);
	// But we also need to make sure that the end is not set before the start ...
	setStartStop (false);
}

void DialogMovie::setStartStop (bool bStart)
{
	QTime zeroTime;
	QTime startTime = QTime::fromString (m_pEditStarting1->text());
	QTime endTime   = QTime::fromString (m_pEditEnding1  ->text());
	if (endTime < startTime)	{
		if (bStart)
			m_pEditEnding1->setText (m_pEditStarting1->text());
		else
			m_pEditStarting1->setText (m_pEditEnding1->text());
	}
	// and last but not least we should set the slider controls ...
	int iStartTime = zeroTime.msecsTo (startTime);	
	int iEndTime   = zeroTime.msecsTo   (endTime);	
	//float fPercentage = (float)iStartTime / (float)m_iMovieLength;                           //ooo
        float fPercentage = static_cast<float>(iStartTime) / static_cast<float>(m_iMovieLength);    //xxx
	m_pStartStopControl->setStart (fPercentage);
	//fPercentage = (float)iEndTime / (float)m_iMovieLength;                           //ooo
        fPercentage = static_cast<float>(iEndTime) / static_cast<float>(m_iMovieLength);    //xxx
	m_pStartStopControl->setEnd   (fPercentage);
	if ( (iStartTime > 10) || (fPercentage < 0.9999) )	{
		if ( (m_pButtonTranscode) && ( ! transcodeEnabled ()) )
			slotTranscode ();
	}
}

void DialogMovie::slotSliderReleased ( bool bStartSlider )
{
  // if the start slider was released AND the MediaEngine is NOT playing, then we 
  // should create a new preview in the MediaInterface - window.
  if ( bStartSlider ) {
    if ( ! m_pMediaInterface->isPlaying ( ) ) {
      m_pLabelCurrentPos->setText ( m_pEditStarting1->text ( ) );
      // The user presses SetAsThumbnail ...
      Utils theUtils;
      QRegExp rx ( "<[^>]*>" );
      QString qsCurrentTime =  m_pLabelCurrentPos->text ( );
      qsCurrentTime.replace ( rx, "" );

      long  iMSecOffset     = theUtils.getMsFromString ( qsCurrentTime );
      m_pMediaInterface->setScreenshot ( iMSecOffset );
    }
  }
}

void DialogMovie::slotSliderValues (float fStart, float fEnd)
{
  // This function is called when the user uses the slider controls (values in percentage)
  QTime startTime;
  QTime endTime;
  QString qsFormat ("hh:mm:ss.zzz");
  //startTime = startTime.addMSecs ( (int)(fStart * m_iMovieLength ) );             //ooo
  startTime = startTime.addMSecs ( static_cast<int>(fStart * m_iMovieLength ) );    //xxx
  //endTime   = endTime.addMSecs   ( (int)(fEnd   * m_iMovieLength ) );             //ooo
  endTime   = endTime.addMSecs   ( static_cast<int>(fEnd   * m_iMovieLength ) );    //xxx
  if ( ( ! m_pMovieObject ) && ( ! m_pDVDMenu  ) )
         m_pEditAdd->setText ( startTime.toString ( qsFormat ) );
  m_pEditStarting1 ->setText ( startTime.toString ( qsFormat ) );
  m_pEditEnding1   ->setText ( endTime.toString   ( qsFormat ) );

  if ( m_pMovieObject )  {
    DVDMenu *pDVDMenu = m_pMovieObject->dvdMenu ( );
//    QTime zeroTime;
    bool bEnable = true;
    /*QColor theColor = m_pEditX->paletteBackgroundColor ( ); // default background color
    if ( duration ( ) > pDVDMenu->getInterface ( )->timeDuration )  {
      theColor = QColor ( 255, 150, 150 );
      bEnable  = false;
    }*/		//oooo
    m_pCheckSameAsMenuLength->setEnabled ( true );
    m_pCheckSameAsMenuLength->setChecked ( ! bEnable );
    m_pButtonLoop->setEnabled ( bEnable );
    //m_pEditEnding2->setPaletteBackgroundColor ( theColor );	//ooo
    m_pEditEnding2->setEnabled ( bEnable );
  }

  // For now we disable auto transcoding of only the start slider is moved ...
  //	if ( ( fStart > 0.0001 ) || ( fEnd < 0.9999) )	{
  if ( fEnd < 0.995 ){
    if ( ( m_pButtonTranscode ) && ( ! transcodeEnabled ( ) ) )
      slotTranscode ( );
  }
}

void DialogMovie::slotNewPosition(int, const QString &qsPosition)
{
  m_qsPosition = qsPosition;
  if (m_qsPosition.length() == 8)
    m_qsPosition += ".000";

  m_pLabelCurrentPos->setText ("<p align=\"center\">" + m_qsPosition + "</p>");
  if ( ( ! m_pDVDMenu ) && ( ! m_pMovieObject ) )	// if this is a BackgroundMovie/Image then this button does not exist.
    m_pEditAdd->setText (m_qsPosition);
  if (m_iMovieLength == 0)
    return;
  //QTime curTime = QTime::fromString(qsPosition);                      //ooo
  //QTime curTime = QTime::fromString("00:00:02.384", "hh:mm:ss.zzz");     //xxx
  QTime curTime = QTime::fromString(qsPosition, "hh:mm:ss.zzz");        //xxx
  //QTime zeroTime;           //ooo
  QTime zeroTime(0,0,0,0);  //xxx
  int iCurTime = zeroTime.msecsTo (curTime);	
  //float fPercentage = (float)iCurTime / (float)m_iMovieLength;                         //ooo
  float fPercentage = static_cast<float>(iCurTime) / static_cast<float>(m_iMovieLength);    //xxx
  m_pStartStopControl->setProgress (fPercentage); //ooo
  //m_pStartStopControl->setProgress (0.75);          //xxx
}

QString DialogMovie::keepClean ( const QString &qsInput )
{
  QString qsReturn = qsInput;
  qsReturn.remove ( "Keep " );
  return  qsReturn;
}

void DialogMovie::applyTransitions ( )
{
/*  // m_iTransitionForAll; // -1==doNothing / 0=remove all transitions / 1=set all transitions
  if ( ( m_iTransitionForAll == -1 ) || ! m_pDVDMenu )
    return; // do nothing ...

  QString qsMenuName, qsName, qsTempPath;
  ButtonTransition transition = m_allTransitions;

  ButtonObject   *pButton = NULL;
  QList<ButtonObject *> list = m_pDVDMenu->getButtons ( );	//oxx
  QList<ButtonObject *>::iterator it = list.begin ( );		//oxx
  if ( m_iTransitionForAll == 0 )  {  // Remove all transitions.
    while  ( it != list.end ( ) )  {
      pButton = *it++;
      pButton->setTransition ( NULL );
    }
    return;
  }

  if ( m_listButtonTransitions.size ( ) < 1 )
    return;

  int   iIdx;
  Utils theUtils;
  qsMenuName = m_pDVDMenu->getInterface ( )->qsMenuName;
  qsTempPath = theUtils.getTempFile ( "" );
  while  ( it != list.end ( ) )   {
    pButton = *it++;

    iIdx = rand ( )%m_listButtonTransitions.size ( );
    transition.qsTransitionName = m_listButtonTransitions[iIdx];

    qsName = qsMenuName + "/" + pButton->name ( );
    transition.qsName = qsName;
    theUtils.recMkdir ( qsTempPath + qsName );
    qsName += "/transition.vob";

    transition.qsTransitionVideoFileName = qsTempPath + qsName; // E.g. "/tmp/TestProjec/Main VMGM/Button
    pButton->setTransition ( &transition );
  }
  m_pDVDMenu->slotUpdateStructure ( );*/	//oooo
}

void DialogMovie::accept ( )
{
  uint      t;
  long      iBitrate, iTemp;
  float     fTemp;
  QFileInfo fileInfo;
  Utils     theUtils;
  QString   qsTemp, qsFormat ("hh:mm:ss.zzz");
  QTime     startingTime, endingTime, duration, zeroTime;

  if ( m_pDVDMenu )  {
    CDVDMenuInterface *pInterface = m_pDVDMenu->getInterface   ( );
    startingTime = QTime::fromString ( m_pEditStarting1->text  ( ) );
    int iSubMenu = m_pSpinBoxTitle->value ( );
    if ( ( ( iSubMenu == 0 ) ) && // Check that the selected SubMenuNumber is valid.
         ( pInterface->qsMenuName != VMGM_LABEL ) ) {
      if ( MessageBox::warning ( this, tr ( "Invalid SubMenu number" ), tr ( "Invalid SubMenu number\nDo you want to assign the menu to SubMenu number 1 instead ?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
        iSubMenu =  1;
      else
        iSubMenu = -1;
    }

    endingTime   = QTime::fromString ( m_pEditEnding1->text ( ) );
    if ( m_pSpinBoxPause->value ( ) == -1 )
      pInterface->qsPause = "inf";
    else 
      pInterface->qsPause = QString ("%1").arg ( m_pSpinBoxPause->value  ( ) );
    if ( ! m_pComboPreCommand->currentText ( ).isEmpty  ( ) )
      pInterface->qsPre   = m_pComboPreCommand->currentText  ( );
    if ( ! m_pComboPostCommand->currentText ( ).isEmpty ( ) )
      pInterface->qsPost  = m_pComboPostCommand->currentText ( );

    if ( pInterface->qsMovieFileName.isEmpty ( ) )
         startingTime = QTime ( );
    // Here we hande the start/stop time.
    if ( startingTime.isValid ( ) )       {
      pInterface->timeOffset = startingTime;
      if ( endingTime.isValid  ( ) )       {
        int iMSecs = startingTime.msecsTo ( endingTime );
        duration   = duration.addMSecs    ( iMSecs     );
        pInterface->timeDuration = duration;
      }
    }
    if ( pInterface->bIsSubMenu )  {
         pInterface->iMenu     = iSubMenu;
         pInterface->iTitleset = m_pSpinBoxTitleset->value ( );
    }
    // Lastly we handle the Transition settings.
    applyTransitions ( );
  }
  else if ( m_pMovieObject )	{
    // Here we handle MovieObjects. Check out if we have set Color Keying.
/*    ImageManipulator::colorKeying *pColorKey;
    ListViewColorItem *pItem;
    float  fDelta;
    Rgba   theColor;
    // First we should remove all entries in the color Key
    ImageManipulator *pMan= &m_pMovieObject->manipulator();
    for (t=0;t<(uint)pMan->listColorKeys.count();t++)	//ox
      delete pMan->listColorKeys[t];
    pMan->listColorKeys.clear();
    // And then we add whats currently in the widget.
    if ( m_pListViewCK->childCount() )	{
      pItem = (ListViewColorItem *)m_pListViewCK->firstChild ();
      while (pItem)	{
        theColor.fromString ( pItem->text ( 0 ) );  // The Color name is stored in the first text
        fDelta = pItem->text ( 1 ).toFloat ( );     // The delta value is stored in the second column.
        pColorKey = new ImageManipulator::colorKeying ( theColor.rgb ( ), fDelta );
        pMan->listColorKeys.append ( pColorKey );
        pItem = (ListViewColorItem *)pItem->itemBelow ( );
      }
    }*/		//oooo
  }		//oooo
  else if (m_pEditStarting1)	{// used as an indicator if we were coming from the SourceToolbar->SourceFileInfo
    startingTime = QTime::fromString ( m_pEditStarting1->text ( ) );
    endingTime   = QTime::fromString ( m_pEditEnding1->text   ( ) );
    m_sourceFileInfo.iPause         =  m_pSpinBoxPause->value ( );
    m_sourceFileInfo.iTitleset      =  m_pSpinBoxTitleset->value        ( );
    m_sourceFileInfo.qsPreCommand   =  m_pComboPreCommand ->currentText ( );
    m_sourceFileInfo.qsPostCommand  =  m_pComboPostCommand->currentText ( );
    m_sourceFileInfo.bAutotranscode =  false; // once the user clicks ok we don't want to change any transcoding settings any more.

    if ( transcodeEnabled ( ) )  {
            Utils theUtils;
      if (  ! m_sourceFileInfo.pTranscodeInterface )
        m_sourceFileInfo.pTranscodeInterface = new TranscodeInterface;
      m_sourceFileInfo.pTranscodeInterface->qsVideoFormat = keepClean ( m_pComboVideoOut->currentText        ( ) );
      m_sourceFileInfo.pTranscodeInterface->qsVideoType   = keepClean ( m_pComboVideoType->currentText       ( ) );
      m_sourceFileInfo.pTranscodeInterface->qsResolution  = keepClean ( m_pComboVideoResolution->currentText ( ) );
      fileInfo.setFile ( m_sourceFileInfo.qsFileName );
      //m_sourceFileInfo.pTranscodeInterface->qsTempFile = QString ( "%1/%2.mpeg2" ).arg (theUtils.getTempPath ( fileInfo.baseName ( TRUE ) ) ).arg(fileInfo.baseName ( TRUE ) );	//ooo
      m_sourceFileInfo.pTranscodeInterface->qsTempFile = QString ( "%1/%2.mpeg2" ).arg (theUtils.getTempPath ( fileInfo.baseName ( ) ) ).arg(fileInfo.baseName ( ) );			//xxx
      qsTemp   = keepClean  ( m_pComboVideoBitrate->currentText ( ) );
      iBitrate = theUtils.normToLong ( qsTemp );
      m_sourceFileInfo.pTranscodeInterface->iVideoBitrate = iBitrate;
      qsTemp   = keepClean ( m_pComboVideoRate->currentText ( ) );
      fTemp    = qsTemp.toFloat ( );
      m_sourceFileInfo.pTranscodeInterface->fFrameRate    = fTemp;
      m_sourceFileInfo.pTranscodeInterface->qsRatio       = keepClean ( m_pComboVideoAspect->currentText ( ) );
      m_sourceFileInfo.pTranscodeInterface->qsAudioFormat = keepClean ( m_pComboAudioOut->currentText    ( ) );
      m_sourceFileInfo.pTranscodeInterface->qsAudioType   = keepClean ( m_pComboAudioType->currentText   ( ) );
      qsTemp = keepClean ( m_pComboAudioBitrate->currentText ( ) );
      iBitrate =  theUtils.normToLong ( qsTemp );
      m_sourceFileInfo.pTranscodeInterface->iAudioBitrate = iBitrate;
      qsTemp = keepClean ( m_pComboAudioSampleRate->currentText ( ) );
      iTemp =  qsTemp.toInt();
      m_sourceFileInfo.pTranscodeInterface->iSample       = iTemp;
      m_sourceFileInfo.pTranscodeInterface->iStartingAt   = zeroTime.msecsTo ( startingTime );
      m_sourceFileInfo.pTranscodeInterface->iEndingAt     = zeroTime.msecsTo ( endingTime   );
      if ( m_pCheckCreateSubtitles->isChecked ( ) )
        //m_sourceFileInfo.pTranscodeInterface->iSubtitleFromMetaInfo = m_pComboLanguage->currentItem ( );	//ooo
	m_sourceFileInfo.pTranscodeInterface->iSubtitleFromMetaInfo = m_pComboLanguage->currentIndex ( );	//xxx
      else
        m_sourceFileInfo.pTranscodeInterface->iSubtitleFromMetaInfo = -1;
//    m_sourceFileInfo.pTranscodeInterface->bTranscoding  = later to indicate transcoding in progress.
    }
    else if (m_sourceFileInfo.pTranscodeInterface)	{
      // Okay there used to be a TranscodeInterface but now the user wants to undo the transcoding.
      delete m_sourceFileInfo.pTranscodeInterface;
      m_sourceFileInfo.pTranscodeInterface = NULL;
    }

    // Adding chapters ...
    m_sourceFileInfo.listChapters.clear ();
    for (t=0;t<(uint)m_pListBoxChapters->count ();t++)
      //m_sourceFileInfo.listChapters.append(m_pListBoxChapters->text (t));		//ooo
      m_sourceFileInfo.listChapters.append(m_pListBoxChapters->item ( t )->text ( ));	//xxx
  }
  else {	// Or if we are coming from the transcodeAll context menu
          m_sourceFileInfo.bAutotranscode = false;
    if (  ! m_sourceFileInfo.pTranscodeInterface )
      m_sourceFileInfo.pTranscodeInterface = new TranscodeInterface;
    m_sourceFileInfo.pTranscodeInterface->qsVideoFormat = keepClean ( m_pComboVideoOut->currentText        ( ) );
    m_sourceFileInfo.pTranscodeInterface->qsVideoType   = keepClean ( m_pComboVideoType->currentText       ( ) );
    m_sourceFileInfo.pTranscodeInterface->qsResolution  = keepClean ( m_pComboVideoResolution->currentText ( ) );
    iBitrate = theUtils.normToLong ( keepClean ( m_pComboVideoBitrate->currentText ( ) ) );
    m_sourceFileInfo.pTranscodeInterface->iVideoBitrate = iBitrate;
    m_sourceFileInfo.pTranscodeInterface->fFrameRate    = keepClean ( m_pComboVideoRate->currentText       ( ) ).toFloat();
    m_sourceFileInfo.pTranscodeInterface->qsRatio       = keepClean ( m_pComboVideoAspect->currentText     ( ) );
    m_sourceFileInfo.pTranscodeInterface->qsAudioFormat = keepClean ( m_pComboAudioOut->currentText        ( ) );
    m_sourceFileInfo.pTranscodeInterface->qsAudioType   = keepClean ( m_pComboAudioType->currentText       ( ) );
    iBitrate = theUtils.normToLong ( keepClean ( m_pComboAudioBitrate->currentText ( ) ) );
    m_sourceFileInfo.pTranscodeInterface->iAudioBitrate = iBitrate;
    m_sourceFileInfo.pTranscodeInterface->iSample       = keepClean ( m_pComboAudioSampleRate->currentText ( ) ).toInt();
    m_sourceFileInfo.pTranscodeInterface->iStartingAt   = 0;
    m_sourceFileInfo.pTranscodeInterface->iEndingAt     = 0;
  }
  QDialog::accept ( );
}


