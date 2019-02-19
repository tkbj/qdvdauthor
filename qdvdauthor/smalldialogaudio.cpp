/****************************************************************************
** Form implementation generated from reading ui file 'smalldialogaudio.ui'
**
**   Created : Mon Jan 29 21:19:51 2007
**        by : Varol Okan using XEmacs and designer
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "smalldialogaudio.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcombobox.h>
//#include <q3groupbox.h>   //ooo
#include <QGroupBox>        //xxx
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>  //ooo
#include <QWhatsThis>       //xxx
#include <qfileinfo.h>
#include <qspinbox.h>
//Added by qt3to4:
//#include <Q3HBoxLayout>   //ooo
#include <QHBoxLayout>      //xxx
//#include <Q3GridLayout>   //ooo
#include <QGridLayout>      //xxx
//#include <Q3VBoxLayout>   //ooo
#include <QVBoxLayout>      //xxx
#include <QPalette>         //xxx
#include <QMessageBox>      //xxx

#include "utils.h"
#include "messagebox.h"
#include "sourcefileentry.h"
//#include "uidialogsubtitle.h"		//ooo
//#include "dialogsubtitle.h"		//xxx

/*
 *  Constructs a SmallDialogAudio as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
//SmallDialogAudio::SmallDialogAudio( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )	//ooo
SmallDialogAudio::SmallDialogAudio( QWidget* pParent )							//xxx
    //: QDialog( parent, name, modal, fl )		//ooo
    : QDialog( pParent )				//xxx
{
    //setupUi(this);	//xxx

    //if ( !name )				//ooo
	//setName( "SmallDialogAudio" );

    //setPaletteBackgroundColor( QColor( 147, 77, 238 ) );                  //xxx
    QPalette palette;                                                       //xxx
    palette.setColor(QPalette::Window, QColor( 147, 77, 238 ));             //xxx
    setPalette(palette);                                                    //xxx
    
    //setSizeGripEnabled( TRUE );   //ooo
    //SmallDialogAudioLayout = new Q3VBoxLayout( this, 11, 6, "SmallDialogAudioLayout");    //ooo
    SmallDialogAudioLayout = new QVBoxLayout( this );                                      //xxx
    SmallDialogAudioLayout->setSpacing(6);
    SmallDialogAudioLayout->setContentsMargins(11, 11, 11, 11);
    SmallDialogAudioLayout->setObjectName(QString::fromUtf8("SmallDialogAudioLayout"));
    //layout9 = new Q3HBoxLayout( 0, 0, 6, "layout9");  //ooo
    layout9 = new QHBoxLayout( );                       //xxx
    layout9->setSpacing(6);  //xxx
    layout9->setObjectName(QString::fromUtf8("layout9"));   //xxx
    //textLabel1 = new QLabel( this, "textLabel1" );    //ooo
    textLabel1 = new QLabel( this );                   //xxx
    textLabel1->setObjectName(QString::fromUtf8("textLabel1")); //xxx
    textLabel1->setWordWrap(false); //xxx
    
    layout9->addWidget( textLabel1 );

    //m_pLabelLanguage = new QLabel( this, "m_pLabelLanguage" );    //ooo
    m_pLabelLanguage = new QLabel( this );                         //xxx
    m_pLabelLanguage->setObjectName(QString::fromUtf8("m_pLabelLanguage"));  //xxx
    m_pLabelLanguage->setWordWrap(false);
    
    layout9->addWidget( m_pLabelLanguage );
    
    SmallDialogAudioLayout->addLayout( layout9 );

    //m_pComboAudioEntry = new QComboBox( FALSE, this, "m_pComboAudioEntry" );  //ooo
    m_pComboAudioEntry = new QComboBox( this );                                 //xxx
    m_pComboAudioEntry->setObjectName(QString::fromUtf8("m_pComboAudioEntry")); //xxx
    //m_pComboAudioEntry->setEnabled( TRUE );   //ooo
    m_pComboAudioEntry->setEnabled( true );     //xxx
    
    SmallDialogAudioLayout->addWidget( m_pComboAudioEntry );

    //m_pGroupAudioFile = new Q3GroupBox( this, "m_pGroupAudioFile" );  //ooo
    m_pGroupAudioFile = new QGroupBox( this );                          //xxx
    m_pGroupAudioFile->setObjectName(QString::fromUtf8("m_pGroupAudioFile"));   //xxx
    //m_pGroupAudioFile->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, m_pGroupAudioFile->sizePolicy().hasHeightForWidth() ) ); //ooo
    //m_pGroupAudioFile->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ); //xxx
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);   //xxx
    sizePolicy.setHorizontalStretch(0); //xxx
    sizePolicy.setVerticalStretch(0);   //xxx
    sizePolicy.setHeightForWidth(m_pGroupAudioFile->sizePolicy().hasHeightForWidth());  //xxx
    m_pGroupAudioFile->setSizePolicy(sizePolicy);   //xxx
    //m_pGroupAudioFile->sizePolicy().setHeightForWidth(true);				//xxxx
    //m_pGroupAudioFile->setColumnLayout(0, Qt::Vertical ); //ooo
    //m_pGroupAudioFile->layout()->setSpacing( 2 ); //ooo
    //m_pGroupAudioFile->layout()->setMargin( 4 );  //ooo
    //m_pGroupAudioFileLayout = new Q3GridLayout( m_pGroupAudioFile->layout() );    //ooo
    m_pGroupAudioFileLayout = new QGridLayout( m_pGroupAudioFile );       //xxx
    //m_pGroupAudioFileLayout->addLayout( m_pGroupAudioFile->layout(), 0, 0, Qt::AlignVCenter );  //xxx
    //m_pGroupAudioFileLayout->setAlignment( Qt::AlignTop );    //ooo
    m_pGroupAudioFileLayout->setSpacing(2);
    m_pGroupAudioFileLayout->setContentsMargins(11, 11, 11, 11);
    m_pGroupAudioFileLayout->setObjectName(QString::fromUtf8("m_pGroupAudioFileLayout"));
    m_pGroupAudioFileLayout->setContentsMargins(4, 4, 4, 4);

    //layout21 = new Q3VBoxLayout( 0, 0, 6, "layout21");    //ooo
    layout21 = new QVBoxLayout( );                          //xxx
    layout21->setSpacing(6);    //xxx
    layout21->setObjectName(QString::fromUtf8("layout21")); //xxx
    //m_pCheckEnable = new QCheckBox( m_pGroupAudioFile, "m_pCheckEnable" );    //ooo
    m_pCheckEnable = new QCheckBox( m_pGroupAudioFile );                        //xxx
    m_pCheckEnable->setObjectName(QString::fromUtf8("m_pCheckEnable")); //xxx
    
    layout21->addWidget( m_pCheckEnable );

    //m_pGroupAudio = new Q3GroupBox( m_pGroupAudioFile, "m_pGroupAudio" ); //ooo
    m_pGroupAudio = new QGroupBox( m_pGroupAudioFile );                     //xxx
    m_pGroupAudio->setObjectName(QString::fromUtf8("m_pGroupAudio"));       //xxx
    //m_pGroupAudio->setEnabled( TRUE );    //ooo
    m_pGroupAudio->setEnabled( true );      //xxx
    //m_pGroupAudio->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, m_pGroupAudio->sizePolicy().hasHeightForWidth() ) ); //ooo
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred); //xxx
    sizePolicy1.setHorizontalStretch(0);    //xxx
    sizePolicy1.setVerticalStretch(0);  //xxx
    sizePolicy1.setHeightForWidth(m_pGroupAudio->sizePolicy().hasHeightForWidth()); //xxx
    m_pGroupAudio->setSizePolicy(sizePolicy1);  //xxx
    //m_pGroupAudio->setColumnLayout(0, Qt::Vertical ); //ooo
    //m_pGroupAudio->layout()->setSpacing( 2 ); //ooo
    //m_pGroupAudio->layout()->setMargin( 2 );  //ooo
    //m_pGroupAudioLayout = new Q3HBoxLayout( m_pGroupAudio->layout() );    //ooo
    m_pGroupAudioLayout = new QHBoxLayout( m_pGroupAudio );       //xxx
    //m_pGroupAudioLayout->setAlignment( Qt::AlignTop );    //ooo
    m_pGroupAudioLayout->setSpacing(2); //xxx
    m_pGroupAudioLayout->setContentsMargins(11, 11, 11, 11);    //xxx
    m_pGroupAudioLayout->setObjectName(QString::fromUtf8("m_pGroupAudioLayout"));   //xxx
    m_pGroupAudioLayout->setContentsMargins(2, 2, 2, 2);    //xxx
    //layout47 = new Q3VBoxLayout( 0, 0, 6, "layout47");    //ooo
    layout47 = new QVBoxLayout( );                          //xxx
    layout47->setSpacing(6);    //xxx
    layout47->setObjectName(QString::fromUtf8("layout47")); //xxx
    //layout29 = new Q3VBoxLayout( 0, 0, 6, "layout29");    //ooo
    layout29 = new QVBoxLayout( );                          //xxx
    layout29->setSpacing(6);    //xxx
    layout29->setObjectName(QString::fromUtf8("layout29")); //xxx
    //textLabel1_6_4_2 = new QLabel( m_pGroupAudio, "textLabel1_6_4_2" );   //ooo
    textLabel1_6_4_2 = new QLabel( m_pGroupAudio );                         //xxx
    textLabel1_6_4_2->setObjectName(QString::fromUtf8("textLabel1_6_4_2")); //xxx
    //textLabel1_6_4_2->setEnabled( TRUE ); //ooo
    textLabel1_6_4_2->setEnabled( true );   //xxx
    textLabel1_6_4_2->setWordWrap(false);   //xxx
    
    layout29->addWidget( textLabel1_6_4_2 );

    //textLabel1_6_4 = new QLabel( m_pGroupAudio, "textLabel1_6_4" );   //ooo
    textLabel1_6_4 = new QLabel( m_pGroupAudio );                       //xxx
    textLabel1_6_4->setObjectName(QString::fromUtf8("textLabel1_6_4")); //xxx
    //textLabel1_6_4->setEnabled( TRUE );   //ooo
    textLabel1_6_4->setEnabled( true );     //xxx
    textLabel1_6_4->setWordWrap(false); //xxx
    
    layout29->addWidget( textLabel1_6_4 );
    
    layout47->addLayout( layout29 );

    //layout31 = new Q3VBoxLayout( 0, 0, 6, "layout31");    //ooo
    layout31 = new QVBoxLayout( );                          //xxx
    layout31->setSpacing(6);    //xxx
    layout31->setObjectName(QString::fromUtf8("layout31")); //xxx

    //textLabel1_6_4_2_2 = new QLabel( m_pGroupAudio, "textLabel1_6_4_2_2" );   //ooo
    textLabel1_6_4_2_2 = new QLabel( m_pGroupAudio );                           //xxx
    textLabel1_6_4_2_2->setObjectName(QString::fromUtf8("textLabel1_6_4_2_2")); //xxx
    //textLabel1_6_4_2_2->setEnabled( TRUE );   //ooo
    textLabel1_6_4_2_2->setEnabled( true );     //xxx
    textLabel1_6_4_2_2->setWordWrap(false);     //xxx
    
    layout31->addWidget( textLabel1_6_4_2_2 );

    //textLabel1_2_3_4_2 = new QLabel( m_pGroupAudio, "textLabel1_2_3_4_2" );   //ooo
    textLabel1_2_3_4_2 = new QLabel( m_pGroupAudio );                           //xxx
    textLabel1_2_3_4_2->setObjectName(QString::fromUtf8("textLabel1_2_3_4_2")); //xxx
    //textLabel1_2_3_4_2->setEnabled( TRUE );   //ooo
    textLabel1_2_3_4_2->setEnabled( true );     //xxx
    textLabel1_2_3_4_2->setWordWrap(false); //xxx
    
    layout31->addWidget( textLabel1_2_3_4_2 );
    
    layout47->addLayout( layout31 );
    
    m_pGroupAudioLayout->addLayout( layout47 );

    //layout48 = new Q3VBoxLayout( 0, 0, 6, "layout48");    //ooo
    layout48 = new QVBoxLayout( );                          //xxx
    layout48->setSpacing(6);    //xxx
    layout48->setObjectName(QString::fromUtf8("layout48")); //xxx

    //layout30 = new Q3VBoxLayout( 0, 0, 6, "layout30");    //ooo
    layout30 = new QVBoxLayout( );                          //xxx
    layout30->setSpacing(6);    //xxx
    layout30->setObjectName(QString::fromUtf8("layout30"));  //xxx

    //m_pComboAudioOut = new QComboBox( FALSE, m_pGroupAudio, "m_pComboAudioOut" ); //ooo
    m_pComboAudioOut = new QComboBox( m_pGroupAudio );                              //xxx
    m_pComboAudioOut->setObjectName(QString::fromUtf8("m_pComboAudioOut")); //xxx
    //m_pComboAudioOut->setEnabled( TRUE ); //ooo
    m_pComboAudioOut->setEnabled( true );   //xxx
    //m_pComboAudioOut->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 3, 0, m_pComboAudioOut->sizePolicy().hasHeightForWidth() ) );   //ooo
    QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); //xxx
    sizePolicy2.setHorizontalStretch(3);    //xxx
    sizePolicy2.setVerticalStretch(0);  //xxx
    sizePolicy2.setHeightForWidth(m_pComboAudioOut->sizePolicy().hasHeightForWidth());  //xxx
    m_pComboAudioOut->setSizePolicy(sizePolicy2);   //xxx
    
    layout30->addWidget( m_pComboAudioOut );

    //m_pComboAudioSampleRate = new QComboBox( FALSE, m_pGroupAudio, "m_pComboAudioSampleRate" );   //ooo
    m_pComboAudioSampleRate = new QComboBox( m_pGroupAudio );                                       //xxx
    m_pComboAudioSampleRate->setObjectName(QString::fromUtf8("m_pComboAudioSampleRate"));   //xxx
    //m_pComboAudioSampleRate->setEnabled( TRUE );  //ooo
    m_pComboAudioSampleRate->setEnabled( true );    //xxx
    //m_pComboAudioSampleRate->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 3, 0, m_pComboAudioSampleRate->sizePolicy().hasHeightForWidth()  ) ); //ooo
    sizePolicy2.setHeightForWidth(m_pComboAudioSampleRate->sizePolicy().hasHeightForWidth());   //xxx
    m_pComboAudioSampleRate->setSizePolicy(sizePolicy2);    //xxx
    //m_pComboAudioSampleRate->setEditable( TRUE ); //ooo
    m_pComboAudioSampleRate->setEditable( true );   //xxx
    
    layout30->addWidget( m_pComboAudioSampleRate );
    
    layout48->addLayout( layout30 );

    //layout32 = new Q3VBoxLayout( 0, 0, 6, "layout32");    //ooo
    layout32 = new QVBoxLayout( );                          //xxx
    layout32->setSpacing(6);    //xxx
    layout32->setObjectName(QString::fromUtf8("layout32")); //xxx
    //m_pComboAudioType = new QComboBox( FALSE, m_pGroupAudio, "m_pComboAudioType" );   //ooo
    m_pComboAudioType = new QComboBox( m_pGroupAudio );                                 //xxx
    m_pComboAudioType->setObjectName(QString::fromUtf8("m_pComboAudioType"));   //xxx
    //m_pComboAudioType->setEnabled( TRUE );    //ooo
    m_pComboAudioType->setEnabled( true );      //xxx
    //m_pComboAudioType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 3, 0, m_pComboAudioType->sizePolicy().hasHeightForWidth() ) ); //ooo
    sizePolicy2.setHeightForWidth(m_pComboAudioType->sizePolicy().hasHeightForWidth()); //xxx
    m_pComboAudioType->setSizePolicy(sizePolicy2);  //xxx
    
    layout32->addWidget( m_pComboAudioType );

    //m_pComboAudioBitrate = new QComboBox( FALSE, m_pGroupAudio, "m_pComboAudioBitrate" ); //ooo
    m_pComboAudioBitrate = new QComboBox( m_pGroupAudio );                                  //xxx
    m_pComboAudioBitrate->setObjectName(QString::fromUtf8("m_pComboAudioBitrate")); //xxx
    //m_pComboAudioBitrate->setEnabled( TRUE ); //ooo
    m_pComboAudioBitrate->setEnabled( true );   //xxx
    //m_pComboAudioBitrate->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 3, 0, m_pComboAudioBitrate->sizePolicy().hasHeightForWidth() ) );   //ooo
    sizePolicy2.setHeightForWidth(m_pComboAudioBitrate->sizePolicy().hasHeightForWidth());  //xxx
    m_pComboAudioBitrate->setSizePolicy(sizePolicy2);   //xxx
    //m_pComboAudioBitrate->setEditable( TRUE );    //ooo
    m_pComboAudioBitrate->setEditable( true );      //xxx
    
    layout32->addWidget( m_pComboAudioBitrate );
    
    layout48->addLayout( layout32 );
    
    m_pGroupAudioLayout->addLayout( layout48 );
    
    layout21->addWidget( m_pGroupAudio );

    m_pGroupAudioFileLayout->addLayout( layout21, 0, 0 );       //ooo
    m_pGroupAudioFileLayout->addLayout( layout21, 0, 0, 1, 1) ; //xxx
    
    SmallDialogAudioLayout->addWidget( m_pGroupAudioFile );
    
    spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    
    SmallDialogAudioLayout->addItem( spacer5 );

    //layout9_2 = new Q3GridLayout( 0, 1, 1, 0, 6, "layout9_2");    //ooo
    layout9_2 = new QGridLayout( );                                 //xxx
    layout9_2->setSpacing(6);   //xxx
    layout9_2->setObjectName(QString::fromUtf8("layout9_2"));   //xxx
    //m_pButtonLanguage = new QPushButton( this, "m_pButtonLanguage" ); //ooo
    m_pButtonLanguage = new QPushButton( this );                        //xxx
    m_pButtonLanguage->setObjectName(QString::fromUtf8("m_pButtonLanguage"));   //xxx
    //m_pButtonLanguage->setAutoDefault( TRUE );    //ooo
    m_pButtonLanguage->setAutoDefault( true );      //xxx

    //layout9_2->addWidget( m_pButtonLanguage, 0, 1 );      //ooo
    layout9_2->addWidget(m_pButtonLanguage, 0, 1, 1, 1 );   //xxx

    //m_pButtonHelp = new QPushButton( this, "m_pButtonHelp" ); //ooo
    m_pButtonHelp = new QPushButton( this );                    //xxx
    m_pButtonHelp->setObjectName(QString::fromUtf8("m_pButtonHelp"));   //xxx
    //m_pButtonHelp->setAutoDefault( TRUE );    //ooo
    m_pButtonHelp->setAutoDefault( true );      //xxx

    //layout9_2->addWidget( m_pButtonHelp, 0, 0 );      //ooo
    layout9_2->addWidget(m_pButtonHelp, 0, 0, 1, 1 );   //xxx

    //m_pButtonOk = new QPushButton( this, "m_pButtonOk" ); //ooo
    m_pButtonOk = new QPushButton( this );                  //xxx
    //m_pButtonOk->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, m_pButtonOk->sizePolicy().hasHeightForWidth() ) ); //ooo
    m_pButtonOk->setObjectName(QString::fromUtf8("m_pButtonOk"));   //xxx
    QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); //xxx
    sizePolicy3.setHorizontalStretch(0);    //xxx
    sizePolicy3.setVerticalStretch(0);  //xxx
    sizePolicy3.setHeightForWidth(m_pButtonOk->sizePolicy().hasHeightForWidth());   //xxx
    m_pButtonOk->setSizePolicy(sizePolicy3);    //xxx
    //m_pButtonOk->setAutoDefault( TRUE );  //ooo
    m_pButtonOk->setAutoDefault( true );    //xxx
    //m_pButtonOk->setDefault( TRUE );      //ooo
    m_pButtonOk->setDefault( true );        //xxx

    //layout9_2->addWidget( m_pButtonOk, 1, 0 );    //ooo
    layout9_2->addWidget(m_pButtonOk, 1, 0, 1, 1 ); //xxx

    //m_pButtonCancel = new QPushButton( this, "m_pButtonCancel" ); //ooo
    m_pButtonCancel = new QPushButton( this );                      //xxx
    m_pButtonCancel->setObjectName(QString::fromUtf8("m_pButtonCancel"));   //xxx
    //m_pButtonCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, m_pButtonCancel->sizePolicy().hasHeightForWidth() ) ); //ooo
    sizePolicy3.setHeightForWidth(m_pButtonCancel->sizePolicy().hasHeightForWidth());   //xxx
    m_pButtonCancel->setSizePolicy(sizePolicy3);    //xxx
    //m_pButtonCancel->setAutoDefault( TRUE );  //ooo
    m_pButtonCancel->setAutoDefault( true );    //xxx

    //layout9_2->addWidget( m_pButtonCancel, 1, 1 );    //ooo
    layout9_2->addWidget(m_pButtonCancel, 1, 1, 1, 1 ); //xxx
    
    SmallDialogAudioLayout->addLayout( layout9_2 );
    
    languageChange();
    //resize( QSize(260, 359).expandedTo(minimumSizeHint()) );  //ooo
    resize(260, 359);   //xxx
    setSizeGripEnabled(true);   //xxx
    //clearWState( WState_Polished );			//ooo
    setAttribute(Qt::WA_WState_Polished, false);	//xxx

    m_pAudio                  = NULL;
    m_pSourceFileEntry        = NULL;
    m_pOrigSourceFileEntry    = NULL;
    m_pPreviousSourceFileInfo = NULL;

    // signals and slots connections
    connect( m_pButtonOk,        SIGNAL( clicked  (      ) ), this, SLOT( accept   ( ) ) );
    connect( m_pButtonCancel,    SIGNAL( clicked  (      ) ), this, SLOT( reject   ( ) ) );
    connect( m_pButtonLanguage,  SIGNAL( clicked  (      ) ), this, SLOT( slotAudioLanguage ( ) ) );
    connect( m_pButtonHelp,      SIGNAL( clicked  (      ) ), this, SLOT( slotHelp ( ) ) );   //ooo
    //connect( m_pButtonHelp,      SIGNAL( triggered  (      ) ), this, SLOT( slotHelp ( ) ) );   //xxx
    connect( m_pComboAudioEntry, SIGNAL( activated(  int ) ), this, SLOT( slotAudioEntryChanged ( int ) ) );
    connect( m_pCheckEnable,     SIGNAL( toggled  ( bool ) ), this, SLOT( slotTranscoding ( bool ) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SmallDialogAudio::~SmallDialogAudio()
{
  // no need to delete child widgets, Qt does it all for us
  if ( m_pSourceFileEntry )
    delete m_pSourceFileEntry;
  m_pSourceFileEntry = NULL;
}

void SmallDialogAudio::initMe ( Audio *pAudio )
{
  if ( ! pAudio )
    return;

  unsigned  int t = 0;
  QString   qsLang;
  QFileInfo fileInfo;
  Utils     theUtils;
  initAudioInfo ( pAudio );

  fileInfo.setFile ( pAudio->qsFileName );
  //m_pComboAudioEntry->insertItem ( fileInfo.fileName ( ) );   //ooo
  m_pComboAudioEntry->addItem ( fileInfo.fileName ( ) );        //xxx

  qsLang = theUtils.iso639 ( pAudio->qsIso639, false, t++ );
  setLanguageText ( qsLang );

  // Finally we set the current file ...
  slotAudioEntryChanged ( 0 );
}

void SmallDialogAudio::initMe ( SourceFileEntry *pSourceFileEntry, bool bShowLanguage )
{
  if ( ! pSourceFileEntry || ! pSourceFileEntry->bSoundSource )
    return;

   m_pOrigSourceFileEntry = pSourceFileEntry;
   m_pSourceFileEntry     = new SourceFileEntry;
  *m_pSourceFileEntry     = *pSourceFileEntry;

  if ( ! bShowLanguage ) {
    m_pButtonLanguage->setEnabled ( false );
    m_pLabelLanguage->hide  ( );
    textLabel1->hide        ( );
  }

  //  initAudio ...
  int             t;
  QFileInfo       fileInfo;
  SourceFileInfo *pInfo = NULL;
    
  for ( t=0; t<(int)pSourceFileEntry->listFileInfos.count ( ); t++ ) {
    pInfo = pSourceFileEntry->listFileInfos[t];
    if ( ! pInfo )
      continue;
    fileInfo.setFile ( pInfo->qsFileName );
    //m_pComboAudioEntry->insertItem ( fileInfo.fileName ( ) ); //ooo
    m_pComboAudioEntry->addItem ( fileInfo.fileName ( ) );      //xxx
  }
  // Finally we set the current file ...
  slotAudioEntryChanged ( 0 );
}

void SmallDialogAudio::slotTranscoding ( bool bEnabled )
{
  m_pGroupAudio->setEnabled ( bEnabled );
}

void SmallDialogAudio::slotAudioLanguage ( )
{
    // This will add a new audio track to the PGC (Title)
  // For simplicity we abuse the SubtitleDialog.
  //uiDialogSubtitle theDialog;		//ooo
  /*DialogSubtitle theDialog;		//xxx
  theDialog.m_pTextNumber->hide ( );
  theDialog.m_pSpinboxNumber->hide ( );
  theDialog.setCaption ( "Select language" );
  int     t;
  Utils   theUtils;
  QString qsLang, qsCode, qsEmpty;

  t = 0;
  qsLang = "Somthing"; //qsNone;
  theDialog.m_pComboLanguage->clear ( );
  while ( ! qsLang.isEmpty ( ) ) {
    qsLang = theUtils.iso639 ( qsCode,  false,  t++ );
    theDialog.m_pComboLanguage->insertItem ( qsLang );
  }

  if ( theDialog.exec ( ) == QDialog::Rejected )
    return;

  // At this point we should get the language ...
  qsLang = theDialog.m_pComboLanguage->currentText ( );
  setLanguageText ( qsLang );*/ //oooo
}

void SmallDialogAudio::setLanguageText ( QString &qsLanguage )
{
  m_pLabelLanguage->setText ( "<p align=\"right\"><b>" + qsLanguage + "</b></p>" );
}

QString SmallDialogAudio::languageText ( )
{
  return m_pLabelLanguage->text ( );
}

void SmallDialogAudio::slotAudioEntryChanged ( int )
{
  if ( ! m_pSourceFileEntry )
    return;

  int t;
  QFileInfo fileInfo;
  SourceFileInfo *pInfo = NULL;
  QString qsFile = m_pComboAudioEntry->currentText ( );

  setTranscoding ( m_pPreviousSourceFileInfo );
  m_pPreviousSourceFileInfo = NULL;
  
  for ( t=0; t<(int)m_pSourceFileEntry->listFileInfos.count ( ); t++ ) {
    pInfo = m_pSourceFileEntry->listFileInfos[t];
    if ( ! pInfo )
      continue;
    fileInfo.setFile ( pInfo->qsFileName );
    if ( fileInfo.fileName ( ) == qsFile ) {
      initAudioInfo  ( pInfo );
      break;
    }
  }
}

void SmallDialogAudio::initAudioInfo ( SourceFileInfo *pInfo )
{
  if ( ! pInfo )
    return;
 
  m_pPreviousSourceFileInfo = pInfo; 
  initTranscode ( pInfo->pTranscodeInterface );
}

void SmallDialogAudio::initAudioInfo ( Audio *pAudio )
{
  if ( ! pAudio )
    return;

  m_pAudio = pAudio;
  TranscodeInterface *pTranscode = pAudio->pTranscodeInterface;
  initTranscode ( pTranscode );
}

void SmallDialogAudio::initTranscode ( TranscodeInterface *pTranscode )
{
  bool    bEnableTranscoding = ( pTranscode );
  QString qsTemp;
  Utils   theUtils;
  long    iBitrate, iSample;

  m_pCheckEnable->setChecked ( bEnableTranscoding );
  m_pGroupAudio ->setEnabled ( bEnableTranscoding );
  if ( bEnableTranscoding )  {
    qsTemp = pTranscode->qsAudioType;
    qsTemp.remove ( "Keep " );
    if ( qsTemp.length ( ) < 1 )
      qsTemp = "mp2";
    m_pComboAudioType->setCurrentText ( qsTemp );

    iBitrate = pTranscode->iAudioBitrate;
    if ( iBitrate < 0 )
         iBitrate *= -1;
    m_pComboAudioBitrate->setCurrentText ( theUtils.longToNorm( iBitrate ) + QString ( "bps" ) );
    iSample = pTranscode->iSample;
    if ( iSample < 0 )
         iSample *= -1;
    m_pComboAudioSampleRate->setCurrentText ( QString ( "%1" ).arg( iSample ) );
  }
}

void SmallDialogAudio::slotHelp ( )
{  
  QSize helpSize ( 600, 400 );
  MessageBox::help ( this, tr ("DVD Audio streams" ), "audio_streams.html", helpSize );
}

void SmallDialogAudio::setTranscoding ( Audio *pAudio )
{
  if ( ! pAudio )
    return;

  if ( ! m_pCheckEnable->isChecked ( ) ) {
    if ( pAudio->pTranscodeInterface )
      delete pAudio->pTranscodeInterface;
    pAudio->pTranscodeInterface = NULL;
    return;
  }
  setTranscoding ( pAudio->pTranscodeInterface );
}

void SmallDialogAudio::setTranscoding ( SourceFileInfo *pSourceFileInfo )
{
  if ( ! pSourceFileInfo )
    return;

  if ( ! m_pCheckEnable->isChecked ( ) ) {
    if ( pSourceFileInfo->pTranscodeInterface )
      delete pSourceFileInfo->pTranscodeInterface;
    pSourceFileInfo->pTranscodeInterface = NULL;
    return;
  }
  setTranscoding ( pSourceFileInfo->pTranscodeInterface );
}

void SmallDialogAudio::setTranscoding ( TranscodeInterface *&pTranscode )
{
  QString qsTemp;
  Utils   theUtils;
  int     iBitrate, iSample;

  if ( ! pTranscode )
    pTranscode = new TranscodeInterface;

  pTranscode->qsAudioFormat = m_pComboAudioOut->currentText  ( );
  pTranscode->qsAudioType   = m_pComboAudioType->currentText ( );
  qsTemp = m_pComboAudioBitrate->currentText ( );
  iBitrate =  theUtils.normToLong ( qsTemp );
  if ( iBitrate < 2000 )
	  iBitrate *= 1000;
  pTranscode->iAudioBitrate = iBitrate;
  qsTemp = m_pComboAudioSampleRate->currentText ( );
  iSample =  qsTemp.toInt ( );
  pTranscode->iSample       = iSample;
}

void SmallDialogAudio::accept ( )
{
  int    t, i;
  SourceFileInfo *pInfo1, *pInfo2;

  setTranscoding ( m_pPreviousSourceFileInfo );
  m_pPreviousSourceFileInfo = NULL;

  if ( m_pSourceFileEntry ) {
    // Here we move the transcoding interface(es) over to the orig SourceFileEntry.
    for ( t=0; t<(int)m_pSourceFileEntry->listFileInfos.count ( ); t++ ) {
      pInfo1 = m_pSourceFileEntry->listFileInfos[t];
      if ( ! pInfo1 )
	continue;
      for ( i=0; i<(int)m_pOrigSourceFileEntry->listFileInfos.count ( ); i++ ) {
	pInfo2 = m_pOrigSourceFileEntry->listFileInfos[i];
	if ( ! pInfo2 )
	  continue;
	if ( pInfo1->qsFileName == pInfo2->qsFileName ) {
	  if ( pInfo2->pTranscodeInterface )
	    delete pInfo2->pTranscodeInterface;
	  pInfo2->pTranscodeInterface = pInfo1->pTranscodeInterface;
	  pInfo1->pTranscodeInterface = NULL;
	  // exit the inner loop ...
	  i = m_pOrigSourceFileEntry->listFileInfos.count ( );
	}
      }
    }
  }
  else if ( m_pAudio )
	setTranscoding ( m_pAudio );

  QDialog::accept ( );
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SmallDialogAudio::languageChange()
{
    //setCaption( tr( "Audio settings" ) );     //ooo
    setWindowTitle( tr( "Audio settings" ) );   //xxx
    textLabel1->setText( tr( "<p align=\"left\">Language : </p>" ) );
    m_pLabelLanguage->setText( tr( "<p align=\"right\"><b>German</b></p>" ) );
    m_pGroupAudioFile->setTitle( tr( "Audio File" ) );
    m_pCheckEnable->setText( tr( "Enable transcoding" ) );
    m_pGroupAudio->setTitle( tr( "Audio Settings" ) );
    textLabel1_6_4_2->setText( tr( "Sound out :" ) );
    textLabel1_6_4->setText( tr( "Sample :" ) );
    textLabel1_6_4_2_2->setText( tr( "Type :" ) );
    textLabel1_2_3_4_2->setText( tr( "Bitrate :" ) );
    m_pComboAudioOut->clear();
    //m_pComboAudioOut->insertItem( tr( "Stereo 2.0" ) );   //ooo
    m_pComboAudioOut->addItem( tr( "Stereo 2.0" ) );        //xxx
    m_pComboAudioSampleRate->clear();
    //m_pComboAudioSampleRate->insertItem( tr( "48000" ) ); //ooo
    m_pComboAudioSampleRate->addItem( tr( "48000" ) );      //xxx
    m_pComboAudioType->clear();
    //m_pComboAudioType->insertItem( tr( "mp2" ) );         //ooo
    m_pComboAudioType->addItem( tr( "mp2" ) );              //xxx
    m_pComboAudioBitrate->clear();
    //m_pComboAudioBitrate->insertItem( tr( "192kbps" ) );  //ooo
    m_pComboAudioBitrate->addItem( tr( "192kbps" ) );       //xxx
    //m_pComboAudioBitrate->insertItem( tr( "32kbps" ) );   //ooo
    m_pComboAudioBitrate->addItem( tr( "32kbps" ) );        //xxx
    //m_pComboAudioBitrate->insertItem( tr( "64kbps" ) );   //ooo
    m_pComboAudioBitrate->addItem( tr( "64kbps" ) );        //xxx
    //m_pComboAudioBitrate->insertItem( tr( "128kbps" ) );  //ooo
    m_pComboAudioBitrate->addItem( tr( "128kbps" ) );       //xxx
    //m_pComboAudioBitrate->insertItem( tr( "224kbps" ) );  //ooo
    m_pComboAudioBitrate->addItem( tr( "224kbps" ) );       //xxx
    //m_pComboAudioBitrate->insertItem( tr( "384kbps" ) );  //ooo
    m_pComboAudioBitrate->addItem( tr( "384kbps" ) );       //xxx
    //m_pComboAudioBitrate->insertItem( tr( "768kbps" ) );  //ooo
    m_pComboAudioBitrate->addItem( tr( "768kbps" ) );       //xxx
    //m_pComboAudioBitrate->insertItem( tr( "1536kbps" ) ); //ooo
    m_pComboAudioBitrate->addItem( tr( "1536kbps" ) );      //xxx
    m_pButtonLanguage->setText( tr( "&Language" ) );
    //m_pButtonLanguage->setAccel( QKeySequence( tr( "Alt+L" ) ) ); //ooo
    m_pButtonHelp->setText( tr( "&Help" ) );
    //m_pButtonHelp->setAccel( QKeySequence( tr( "F1" ) ) );    //ooo
    m_pButtonOk->setText( tr( "&OK" ) );
    //m_pButtonOk->setAccel( QKeySequence( QString::null ) );   //ooo
    m_pButtonCancel->setText( tr( "&Cancel" ) );
    //m_pButtonCancel->setAccel( QKeySequence( QString::null ) );   //ooo
}

