/****************************************************************************
** Derived off of DialogButtonTrans
**
**   Created : Sun June 12 09:13:25 2009
**        by : Varol Okan, using Kat
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#include <qdir.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qtabwidget.h>
//#include <qmessagebox.h>  //ooo
#include <QMessageBox>      //xxx
#include <qpushbutton.h>
//#include <q3progressbar.h>    //ooo
#include <QProgressBar>         //xxx
#include <qapplication.h>
#include <QListWidget>      //xxx
#include <QListWidgetItem>  //xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QGridLayout>
#include <QPixmap>

#include "global.h"
#include "qdvdauthor.h"
#include "buttonobject.h"
#include "filepreviewdialog.h"
#include "preview_file_dialog.h"    //xxx
#include "dialogbuttontrans.h"
#include "messagebox.h" //xxx

static const char * xpmNoCheck[] = {
"16 16 17 1",
"       c None",
".      c #110807",
"+      c #3A0D0F",
"@      c #B00000",
"#      c #A3080B",
"$      c #CD0000",
"%      c #C20407",
"&      c #E40A0B",
"*      c #862829",
"=      c #5C3440",
"-      c #C41A1E",
";      c #E51E21",
">      c #C4373A",
",      c #EA3037",
"'      c #AA545A",
")      c #E85B60",
"!      c #E99398",
"  >>)     -)'   ",
"  *!>     >!*.  ",
" +'!)@   @)!'+. ",
"=')))>@ #>)))'=.",
"=',,,,->-,,,,>==",
".+#&$$;);$$&#+. ",
" .+@%@@%@@%@+.  ",
"  .*-@@@@@-*+   ",
"   #>-####>#    ",
"  #@@@@@@@@@#   ",
" #-%@%$;$%@%-#  ",
">-%%$$&,&&$%%-> ",
")-$$&&&$&&&$$%>>",
"@%&&&&$ $&&&&%@ ",
" %;);&   $&;&%  ",
"  ;),     &,,   "};

static const char * xpmCheck[] = {
"16 16 17 1",
"       c None",
".      c #041504",
"+      c #024D01",
"@      c #3C5039",
"#      c #197313",
"$      c #148F0E",
"%      c #3C9237",
"&      c #32B229",
"*      c #53AA4B",
"=      c #7DB474",
"-      c #4AD53E",
";      c #78C570",
">      c #61D256",
",      c #A0CF9C",
"'      c #8EDF86",
")      c #6AF253",
"!      c #77F26C",
"                ",
"            ++  ",
"           $%%# ",
"          $*=%#+",
"         #*=*$#+",
"  %%#   $*;*$$#@",
" %,,%. $*;>&$#. ",
"#=,,;$$*'>-&#.  ",
"#*;'';;'>-&#.   ",
"##$>'''>)-$+    ",
" +#&-!!)-&#     ",
"  #$&))!&&      ",
"   $$>>>&       ",
"    &$*>        ",
"     )          ",
"                "};

//DialogButtonTrans::Item::Item ( Q3IconView *pIconView, const QString &qsText, const QPixmap &thePix, bool bChecked )  //ooo
DialogButtonTrans::Item::Item ( QListWidget *pIconView, const QString &qsText, const QPixmap &thePix, bool bChecked )   //xxx
  //: Q3IconViewItem ( pIconView )  //ooo
  : QListWidgetItem ( pIconView )   //xxx
{
  m_bChecked   = bChecked;
  m_pixmap     = thePix;
  m_qsFileName = qsText;
  refreshIcon ( );
}

QString DialogButtonTrans::Item::fileName ( )
{
  return m_qsFileName;
}

void DialogButtonTrans::Item::setChecked ( bool bChecked )
{
  if ( bChecked == m_bChecked )
    return;

  m_bChecked = bChecked;
  //QPoint position = pos ( );  //ooo
  QPoint position = listWidget()->pos ( );  //xxx
  refreshIcon   ( );
  //move ( position );  //oooo
  //iconView ( )->updateContents ( );   //oooo
}

bool DialogButtonTrans::Item::toggle ( )
{
  setChecked ( !m_bChecked );
  return m_bChecked;
}

bool DialogButtonTrans::Item::isChecked ( )
{
  return m_bChecked;
}

void DialogButtonTrans::Item::calcRect ( const QString &string )
{
  /*Q3IconViewItem::calcRect ( string );
  QRect rect  ( 0, 0, 0, 0 );
  setTextRect ( rect );
  setItemRect ( pixmapRect ( ) );*/ //oooo
}

void DialogButtonTrans::Item::refreshIcon ( )
{
  QPixmap pixCheck, thePix;
  int iX, iY, iWidth, iHeight;
  QColor clrBlue ( 120, 120, 255 );

  if ( m_bChecked )
    pixCheck = QPixmap ( xpmCheck   );
  else
    pixCheck = QPixmap ( xpmNoCheck );

  iX = iY = 1;
  iWidth  = m_pixmap.width  ( );
  iHeight = m_pixmap.height ( );

  int  iSize = iWidth;
  if ( iSize < iHeight )
       iSize = iHeight;
  if ( iSize < 16 )
       iSize = 16;
  thePix = QPixmap ( iSize + 2, iSize + 2 + 16);
  thePix.fill ( 0xFFFFFF );
  
  iX = (int)( ( iSize - iWidth  ) / 2.0 ) + 1;
  iY = (int)( ( iSize - iHeight ) / 2.0 ) + 1;
  
  QPainter thePainter    ( &thePix );    //xxx

  //copyBlt ( &thePix, iX, iY, &m_pixmap ); //ooo
  thePainter.drawPixmap ( iX, iY, m_pixmap );   //xxx

  // Frame the pix
  iWidth  += 2;
  iHeight += 2;
  //QPainter thePainter ( &thePix );    //ooo
  thePainter.setPen   ( clrBlue );
  thePainter.drawLine (       0,       0, iSize-1,       0 );
  thePainter.drawLine ( iSize-1,       0, iSize-1, iSize-1 );
  thePainter.drawLine ( iSize-1, iSize-1,       0, iSize-1 );
  thePainter.drawLine (       0, iSize-1,       0,       0 );

  // next we copy the check box
  iX = (int)( ( (float)iWidth - 16.0 ) / 2.0 );
  //copyBlt ( &thePix, iX, iSize + 1, &pixCheck, 0, 0, 16, 16 );    //ooo
  thePainter.drawPixmap ( iX, iSize + 1, pixCheck, 0, 0, 16, 16 );   //xxx

  //setPixmap ( thePix );   //ooo
  setIcon ( thePix );       //xxx
}

DialogButtonTrans::DialogButtonTrans ( ButtonTransition *pTransition, QWidget *pParent )
  //: uiDialogButtonTrans ( pParent )	//ooo
  : QDialog ( pParent )			//xxx
{
  setupUi(this);			//xxx
  m_pTransition = new ButtonTransition;
  if ( pTransition )
    *m_pTransition = *pTransition;

  initMe ( false );
}

DialogButtonTrans::DialogButtonTrans ( ButtonTransition *pTransition, QStringList &list, QWidget *pParent )
  //: uiDialogButtonTrans ( pParent )	//ooo
  : QDialog ( pParent )			//xxx
{
  setupUi(this);			//xxx
  m_pTransition = new ButtonTransition;
  if ( pTransition )
    *m_pTransition = *pTransition;
  initEnabled ( list );
  initMe      ( true );
}

DialogButtonTrans::~DialogButtonTrans ( )
{
  if ( m_pTransition )
    delete m_pTransition;
  m_pTransition = NULL;

  if ( m_pTimer )
    delete m_pTimer;
  m_pTimer = NULL;

  for ( int t=0; t<THUMBNAIL_COUNT; t++ )  {
    delete m_arrayPix[t];
    m_arrayPix[t] = NULL;
  }
}

void DialogButtonTrans::initMe ( bool bMultiMode )
{ 
  m_pTimer      =  NULL;
  m_iThumbSize  =    80;
  m_iCurrentPix =     0;
  m_bAnimated   = false;
  m_bMultiMode  = bMultiMode;

  if ( bMultiMode )  {
    m_pButtonNoTransition->setText ( tr ( "&No Transitions" ) ); // plural in case wecome from the DVDMenu
    //QWidget *pTab = m_pTabWidget->page ( 0 ); //ooo
    QWidget *pTab = m_pTabWidget->widget ( 0 ); //xxx
    while  ( pTab ) {
      delete pTab;
      //pTab = m_pTabWidget->page ( 0 );    //ooo
      pTab = m_pTabWidget->widget ( 0 );    //xxx
    }

    connect ( m_pButtonSelectAll, SIGNAL ( clicked ( ) ), this, SLOT ( slotSelectAll ( ) ) );
    connect ( m_pButtonClearAll,  SIGNAL ( clicked ( ) ), this, SLOT ( slotClearAll  ( ) ) );
  }
  else  {
    m_pButtonSelectAll->hide ( );
    m_pButtonClearAll->hide  ( );
    m_pFrameUserDefined->setEnabled ( m_pTransition->bUserFile );
    if ( m_pTransition->bUserFile )
         m_pEditUserFile->setText ( m_pTransition->qsTransitionVideoFileName );
    connect ( m_pButtonBrowse,     SIGNAL ( clicked ( ) ), this, SLOT ( slotBrowse      ( ) ) );
    connect ( m_pCheckUserDefined, SIGNAL ( clicked ( ) ), this, SLOT ( slotUserDefined ( ) ) );
  }
  
  connect ( m_pButtonHelp,         SIGNAL ( clicked ( ) ), this, SLOT ( slotHelp         ( ) ) );   //xxx
  connect ( buttonOk,              SIGNAL ( clicked ( ) ), this, SLOT ( accept           ( ) ) );   //xxx
  connect ( buttonCancel,          SIGNAL ( clicked ( ) ), this, SLOT ( reject           ( ) ) );   //xxx

  for ( int t=0; t<THUMBNAIL_COUNT; t++ )
    m_arrayPix[t] = new QPixmap;

  m_pSpinFilterDelay->setValue ( (int)m_pTransition->fDuration );
  QString qsLabel = m_pTransition->qsTransitionName;

  setLabel ( qsLabel ); //m_pTransition->qsTransitionName );

  // build all library tabs.
  int      t;
  QDir     theDir;
  QString  qsPrefixDir, qsDirName;
  QStringList listDirs, listFiles;

  qsPrefixDir = QString ("%1/share/qdvdauthor/slideshow/transitions").arg ( Global::qsSystemPath );
  if ( !  theDir.exists ( qsPrefixDir ) )
    return;

  //theDir.cd ( qsPrefixDir, TRUE );	//ooo
  theDir.cd ( qsPrefixDir );		//xxx
  listDirs = theDir.entryList ( QDir::Dirs, QDir::Name );
  if ( listDirs.count ( ) < 3 ) // 3 = this dir / parent dir / background file
    return;

  // The following widgets will create th progress dialog on the fly.
  QDialog       progressDialog  ( NULL );
  //Q3GridLayout  *pProgressLayout = new Q3GridLayout  ( &progressDialog ); //ooo
  QGridLayout  *pProgressLayout = new QGridLayout  ( &progressDialog );     //xxx
  //Q3ProgressBar *pProgressBar    = new Q3ProgressBar ( &progressDialog ); //ooo
  QProgressBar *pProgressBar    = new QProgressBar ( &progressDialog );     //xxx
  pProgressLayout->addWidget( pProgressBar, 0, 0 );
  progressDialog.resize( QSize(537, 50).expandedTo ( progressDialog.minimumSizeHint ( ) ) );
  progressDialog.show  ( );

  for ( t=0; t<(int)listDirs.count ( ); t++ )  {
    // Here we go through all directories 
    qsDirName = listDirs[t];
    if ( ( qsDirName == ".") || ( qsDirName == "..") )
      continue;

    //progressDialog.setCaption ( tr ( "Creating preview for buttons : %1" ).arg ( listDirs[t] ) ); //ooo
    progressDialog.setWindowTitle ( tr ( "Creating preview for buttons : %1" ).arg ( listDirs[t] ) );   //xxx

    // Next we need to get the file list of the found directory
    QString qsPath = qsPrefixDir + "/" + qsDirName;
    theDir.cd ( qsPath );
    listFiles = theDir.entryList ( QDir::Files, QDir::Name );
    createTab ( qsDirName, qsPath, listFiles );

    //pProgressBar->setProgress ( (int)((float)t / listDirs.count ( ) * 100.0) );   //ooo
    pProgressBar->setValue ( (int)((float)t / listDirs.count ( ) * 100.0) );    //xxx
    //qApp->processEvents ( 100 ); // do some event processing ...				//ooo
    qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx
  }

  connect ( m_pButtonNoTransition, SIGNAL ( clicked ( ) ), this, SLOT ( slotNoTransition ( ) ) );
  connect ( m_pButtonAnim,         SIGNAL ( clicked ( ) ), this, SLOT ( slotAnim         ( ) ) );

  QTimer::singleShot ( 200, this, SLOT ( slotAfterInit ( ) ) );
}

void DialogButtonTrans::initEnabled ( QStringList &list )
{
  m_listEnabled = list;
//  QStringList::iterator it = m_listEnabled.begin ( );
//  while ( it != m_listEnabled.end ( ) )  {
//    printf ( "In List<%s>\n", (*it++).ascii() );
//  }
}

void DialogButtonTrans::setLabel ( QString qsTransition )
{
  QString qsLabel;
  if ( m_bMultiMode )
    qsLabel = QString ( "<p align=\"center\"><b>Random Transitions</b></p>" );
  else  {
    QFileInfo fileInfo ( qsTransition );
    qsLabel = QString ( "<p align=\"center\"><b>%1</b></p>" ).arg ( fileInfo.fileName ( ) );
  }
  m_pLabelName->setText ( qsLabel );
}

void DialogButtonTrans::createTab ( QString qsDirName, QString qsPath, QStringList &listFiles )
{
  int t;
  bool bChecked;
  if ( listFiles.count ( ) < 1 )
    return;

  // Okay, ready to create the tab ... 
  QFileInfo fileInfo;
  QString qsFileName, qsFile;
  QSize grid ( 80, 80 );
  //QWidget       *pTab      = new QWidget     ( m_pTabWidget, "pTab" );    //ooo
  QWidget       *pTab      = new QWidget     ( m_pTabWidget );  //xxx
  //Q3GridLayout   *pLayout   = new Q3GridLayout ( pTab, 1, 1, 4, 2, "pLayout" );   //ooo
  QGridLayout   *pLayout   = new QGridLayout ( pTab );  //xxx
  //Q3IconView     *pIconView = new Q3IconView   ( pTab, "pIconViewLibrary" );  //ooo
  QListWidget     *pIconView = new QListWidget   ( pTab );   //xxx
  //Q3IconViewItem *pItem     = NULL;   //ooo
  QListWidgetItem *pItem     = NULL;    //xxx
  m_listViews.append ( pIconView );

  pLayout->addWidget         ( pIconView,  0, 0 );
  //pIconView->setAutoArrange  ( true  );   //oooo
  //pIconView->setShowToolTips ( true );    //oooo
  //m_pTabWidget->insertTab    ( pTab,  qsDirName, 0 ); //ooo
  m_pTabWidget->addTab    ( pTab,  qsDirName );  //xxx

  for ( t=0; t<(int)listFiles.count ( ); t++ ) {
    if ( listFiles[t] == "crossfade.png" )
      continue;
    qsFileName = qsPath + "/" + listFiles[t];
    QPixmap thePix;
    QImage  theImage ( qsFileName );

    if ( theImage.isNull ( ) )
      continue;

    if ( ( theImage.width ( ) > m_iThumbSize ) || ( theImage.height ( ) > m_iThumbSize ) )
      //theImage = theImage.smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );				//ooo
      theImage = theImage.scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );		//xxx

    thePix.convertFromImage ( theImage );
    fileInfo.setFile ( listFiles[t] );
    qsFile   = fileInfo.baseName  ( );
    bChecked = isChecked   ( qsFile );

    pItem = new Item ( pIconView, listFiles[t], thePix, bChecked );
    //pItem->setKey ( qsFileName ); //oooo
    pItem->setText ( qsFileName );  //xxx
  }

  /*pIconView->setSelectionMode ( Q3IconView::NoSelection );
  pIconView->setAutoArrange   (  TRUE );
  pIconView->setItemsMovable  ( FALSE );
  pIconView->setResizeMode    ( Q3IconView::Adjust );
  pIconView->sort ( );
  pIconView->arrangeItemsInGrid ( grid, TRUE );*/   //oooo

  connect ( pIconView, SIGNAL ( mouseButtonClicked ( int, Q3IconViewItem *, const QPoint & ) ), this, SLOT ( slotClicked ( int, Q3IconViewItem *, const QPoint & ) ) );
}

bool DialogButtonTrans::isChecked ( QString &qsString )
{
  QStringList::iterator it = m_listEnabled.begin ( );
  while ( it != m_listEnabled.end ( ) )  {
    if ( *it++ == qsString )
      return true;
  }

  return false;
}

void DialogButtonTrans::slotAfterInit ( )
{
  // Somehow the first IconView does not arrange in a grid.
  // so we have to force it to arrange
  if ( m_listViews.count ( ) < 1 )
    return;

  const QPoint pos;
  //m_listViews[0]->sort ( true );  //oooo

  int  iTabCounter = 0;
  Item      *pItem = NULL;
  //Q3IconView *pView = NULL;   //ooo
  QListWidget *pView = NULL;    //xxx
  //QList<Q3IconView *>::iterator it = m_listViews.begin ( );	//oxx
  QList<QListWidget *>::iterator it = m_listViews.begin ( );	//xxx
  while ( it != m_listViews.end ( ) )  {
    pView = *it++;

    //pItem = (Item *)pView->firstItem ( ); //ooo
    pItem = (Item *)pView->item ( 0 );  //xxx
    while ( pItem )  {
      //if  ( pItem->key ( ) == m_pTransition->qsTransitionName )  {    //ooo
      if  ( pItem->text ( ) == m_pTransition->qsTransitionName )  {     //xxx
        //m_pTabWidget->setCurrentPage ( iTabCounter ); //ooo
        m_pTabWidget->setCurrentIndex ( iTabCounter );  //xxx
        slotClicked   ( Qt::LeftButton, pItem, pos );
        //pView->ensureItemVisible ( pItem );   //oooo
        return;
      }
      //pItem = (Item *)pItem->nextItem ( );    //ooo
      pItem = (Item *)pView->item ( pView->row ( pItem ) + 1 ); //xxx
    }
    iTabCounter ++;
  }

  //slotClicked ( Qt::LeftButton, m_listViews[0]->firstItem ( ), pos ); //ooo
  slotClicked ( Qt::LeftButton, m_listViews[0]->item ( 0 ), pos ); //xxx
}

ButtonTransition *DialogButtonTrans::getTransition ( )
{
  return m_pTransition;
}

//void DialogButtonTrans::slotClicked ( int iButton, Q3IconViewItem *pItm, const QPoint & ) //ooo
void DialogButtonTrans::slotClicked ( int iButton, QListWidgetItem *pItm, const QPoint & )  //xxx
{
  if ( ! pItm )
    return;

  if ( iButton != Qt::LeftButton )
    return;

  Item *pItem = NULL;
  if ( ! m_bMultiMode )  {
    //QList<Q3IconView *>::iterator it = m_listViews.begin ( );		//oxx
      QList<QListWidget *>::iterator it = m_listViews.begin ( );		//xxx
    while ( it !=m_listViews.end ( ) )  {
      //Q3IconView *pIconView = *it++; //pItm->iconView ( );    //ooo
      QListWidget *pIconView = *it++; //pItm->iconView ( );     //xxx
      if ( ! pIconView )
        continue;

      //pItem = (Item *)pIconView->firstItem ( );   //ooo
      pItem = (Item *)pIconView->item ( 0 );        //xxx
      while ( pItem )  {
        pItem->setChecked ( pItem == pItm );
        //pItem = (Item *)pItem->nextItem ( );  //ooo
        pItem = (Item *)pIconView->item ( pIconView->row ( pItem ) + 1 );   //xxx
      }
    }
  }
  else  { // ( m_bMultiMode )  {
    pItem = (Item *)pItm;
    pItem->toggle ( );
  }

  pItem = (Item *)pItm;
  //QImage theImage ( pItem->key ( ) ); //ooo
  QImage theImage ( pItem->text ( ) );  //xxx
  if ( theImage.isNull ( ) )
    return;

  //m_qsTransitionName = pItem->key ( );    //ooo
  m_qsTransitionName = pItem->text ( );     //xxx
  // Stop the animation if currently ongoing.
  if ( m_bAnimated )
    slotAnim ( );

  //setLabel ( pItem->key ( ) );    //ooo
  setLabel ( pItem->text ( ) );     //xxx
  //theImage = theImage.smoothScale ( m_pLabelPreview->size ( ) );						//ooo
  theImage = theImage.scaled ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  QPixmap thePix;
  thePix.convertFromImage ( theImage );
  m_pLabelPreview->setScaledContents         ( true );		//xxx	
  m_pLabelPreview->setAutoFillBackground     ( true );		//xxx
  //m_pLabelPreview->setPaletteBackgroundPixmap ( thePix );	//ooo
  QPalette palette;						//xxx
  palette.setBrush(QPalette::Window, QBrush(thePix));		//xxx
  m_pLabelPreview->setPalette(palette);				//xxx
}

void DialogButtonTrans::slotUserDefined  ( )
{
  m_pFrameUserDefined->setEnabled ( m_pCheckUserDefined->isChecked ( ) );
}

void DialogButtonTrans::slotBrowse ( )
{
  QString qsVideoFilter = Global::pApp->getMovieFilter ( );
  QString qsFilter      = QString ( "Videos ( " ) + qsVideoFilter + QString (" );;All ( * )" );
  //QString qsVideoName   = FilePreviewDialog::getOpenFileName ( NULL, Global::qsCurrentPath, qsFilter, this, tr ( "Select video" ), tr ( "Select a video." ) );    //ooo
  
  QStringList qsVideoName;  //xxx
  QFileDialog* pDialog = new PreviewFileDialog(this, tr ( "Select video" ), Global::qsCurrentPath, qsFilter); //xxx
  pDialog->setAcceptMode(QFileDialog::AcceptOpen);  //xxx
  
  if ( pDialog->exec ( ) == QDialog::Accepted ) //xxx
    qsVideoName = pDialog->selectedFiles ( );
  
  //if ( qsVideoName.isNull ( ) )   //ooo
  if (qsVideoName.isEmpty())   //xxx
    return;

  //QFileInfo fileInfo  ( qsVideoName );    //ooo
  QFileInfo fileInfo  ( qsVideoName[0] );          //xxx
  //Global::qsCurrentPath = fileInfo.dirPath ( TRUE );  //ooo
  Global::qsCurrentPath = fileInfo.path ( );    //xxx

  //m_pEditUserFile->setText ( qsVideoName );   //ooo
  m_pEditUserFile->setText ( qsVideoName[0] );  //xxx
}

void DialogButtonTrans::slotSelectAll ( )
{
  if ( m_bMultiMode )  {
    Item *pItem = NULL;
    //QList<Q3IconView *>::iterator it = m_listViews.begin ( );		//oxx
    QList<QListWidget *>::iterator it = m_listViews.begin ( );		//xxx
    while ( it !=m_listViews.end ( ) )  {
      //Q3IconView *pIconView = *it++; //pItm->iconView ( );    //ooo
      QListWidget *pIconView = *it++; //pItm->iconView ( );     //xxx
      if ( ! pIconView )
        continue;

      //pItem = (Item *)pIconView->firstItem ( );   //ooo
      pItem = (Item *)pIconView->item ( 0 );    //xxx
      while ( pItem )  {
        pItem->setChecked ( true );
        //pItem = (Item *)pItem->nextItem ( );  //ooo
        pItem = (Item *)pIconView->item ( pIconView->row ( pItem ) + 1 );   //xxx
      }
    }
  }
}

void DialogButtonTrans::slotClearAll ( )
{
  if ( m_bMultiMode )  {
    Item *pItem = NULL;
    //QList<Q3IconView *>::iterator it = m_listViews.begin ( );		//oxx
    QList<QListWidget *>::iterator it = m_listViews.begin ( );		//xxx
    while ( it !=m_listViews.end ( ) )  {
      //Q3IconView *pIconView = *it++; //pItm->iconView ( );    //ooo
      QListWidget *pIconView = *it++; //pItm->iconView ( );     //xxx
      if ( ! pIconView )
        continue;

      //pItem = (Item *)pIconView->firstItem ( );   //ooo
      pItem = (Item *)pIconView->item ( 0 );    //xxx
      while ( pItem )  {
        pItem->setChecked ( false );
        //pItem = (Item *)pItem->nextItem ( );  //ooo
        pItem = (Item *)pIconView->item ( pIconView->row ( pItem ) + 1 );   //xxx
      }
    }
  }
}

void DialogButtonTrans::slotAnim ( )
{
  // User pushed button [Anim]
  QString qsAnim = tr ( "A\nn\ni\nm" );
  QString qsStop = tr ( "S\nt\no\np" );
  if ( m_bAnimated )  {
    m_pButtonAnim->setText  ( qsAnim );
    if ( m_pTimer )
         m_pTimer->stop ( );
    m_bAnimated = false;
  }
  else  {
    m_pButtonAnim->setText ( qsStop );
    // First we init the timer if required
    if ( ! m_pTimer )  {
      m_pTimer = new QTimer ( this );
      connect ( m_pTimer, SIGNAL ( timeout ( ) ), this, SLOT ( slotAnimPix ( ) ) );
    }
    // start the timer
    m_pTimer->start ( 100 );
    m_bAnimated = true;
  }
  // and lastly, we recreate the pixmaps / preview ...
  recreatePix ( );
}

void DialogButtonTrans::slotAnimPix ( )
{
  m_iCurrentPix ++;
  if ( m_iCurrentPix < 0 )
    return;

  if ( m_iCurrentPix >= THUMBNAIL_COUNT )  {
    m_iCurrentPix = -10; // 1 sec pause between each run ...
    return;
  }
  if ( m_arrayPix[m_iCurrentPix]->isNull ( ) )
    return;
  m_pLabelPreview->setScaledContents         ( true );					//xxx	
  m_pLabelPreview->setAutoFillBackground     ( true );					//xxx
  //m_pLabelPreview->setPaletteBackgroundPixmap ( *m_arrayPix[m_iCurrentPix] );		//ooo
  QPalette palette;									//xxx
  palette.setBrush(QPalette::Window, QBrush(*m_arrayPix[m_iCurrentPix]));		//xxx
  m_pLabelPreview->setPalette(palette);							//xxx
}

void DialogButtonTrans::recreatePix ( )
{
  Utils theUtils;
  QCursor myCursor     ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  //QPixmap *pPixmap = (QPixmap *)m_pLabelPreview->paletteBackgroundPixmap ( ); //ooo
  QPixmap *pPixmap = (QPixmap *)m_pLabelPreview->pixmap ( );    //xxx
  if ( ! pPixmap )
    return;
  QPixmap filterPix = *pPixmap;

  if ( m_bAnimated )  {
    int      t, iDiff = 0;
    float    fDelta;
    QPixmap *array[THUMBNAIL_COUNT];

    for ( t=0; t<THUMBNAIL_COUNT; t++ )
      array[t] = new QPixmap ( m_pLabelPreview->size ( ) );

    //QImage filter = filterPix.convertToImage ( ); //( qsFilterName ); //ooo
    QImage filter = filterPix.toImage ( ); //( qsFilterName );  //xxx
    //QImage start  = QImage::fromMimeSource ( "default.jpg" ).copy ( );	//ooo
    QImage start  = QImage ( ":/images/default.jpg" ).copy ( );			//xxx
    //QImage stop   = QImage::fromMimeSource (  "splash.jpg" ).copy ( );	//ooo
    QImage stop   = QImage (  ":/images/splash.jpg" ).copy ( );			//xxx
    QImage result     ( start ); // init the size
    //QImage background ( m_pLabelPreview->size ( ), 32 );  //ooo
    QImage background ( m_pLabelPreview->size ( ), QImage::Format_RGB32 );  //xxx
    background.fill   ( 0x000000 );

    //filter = filter.smoothScale ( m_pLabelPreview->size ( ) );						//ooo
    filter = filter.scaled ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    //start  = start.smoothScale  ( m_pLabelPreview->size ( ) );						//ooo
    start  = start.scaled  ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    //stop   = stop.smoothScale   ( m_pLabelPreview->size ( ) );						//ooo
    stop   = stop.scaled   ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    fDelta = 512.0f / ( THUMBNAIL_COUNT - iDiff - 1 );

    for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
      theUtils.augmentAlpha ( (int)( fDelta * t ), filter, start, stop, result );
      array[t]->convertFromImage ( result );
    }

    for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
      if ( m_arrayPix[t] )
        delete m_arrayPix[t];
      m_arrayPix[t] = array[t];
    }
  }
  m_iCurrentPix = 0;

  QApplication::restoreOverrideCursor ( );
}

QStringList &DialogButtonTrans::getEnabledList ( )
{
  return m_listEnabled;
}

void DialogButtonTrans::accept ( )
{  
  if ( m_bMultiMode )  {
    int   iCount = 0;
    Item *pItem  = NULL;
    QString   qsName;
    QFileInfo fileInfo;
    m_listEnabled.clear ( );

    //QList<Q3IconView *>::iterator it = m_listViews.begin ( );		//oxx
    QList<QListWidget *>::iterator it = m_listViews.begin ( );		//xxx
    while ( it !=m_listViews.end ( ) )  {
      //Q3IconView *pIconView = *it++; //pItm->iconView ( );    //ooo
      QListWidget *pIconView = *it++; //pItm->iconView ( );     //xxx
      if ( ! pIconView )
        continue;

      //pItem = (Item *)pIconView->firstItem ( );   //ooo
      pItem = (Item *)pIconView->item ( 0 );        //xxx
      while ( pItem )  {
        if  ( pItem->isChecked ( ) )  {
          fileInfo.setFile     ( pItem->fileName ( ) );
          qsName = fileInfo.baseName  ( );
          m_listEnabled.append ( qsName );
          iCount ++;
        }
        //pItem   = (Item *)pItem->nextItem ( );    //ooo
        pItem = (Item *)pIconView->item ( pIconView->row ( pItem ) + 1 );   //xxx
      }
    }
    if ( iCount == 0 )  {
      QMessageBox::warning ( NULL, tr ( "Warning" ), tr ( "Please select at least one Transition." ), QMessageBox::Ok, QMessageBox::NoButton );
      return;
    }
  }

  if ( m_pTransition )  {
       m_pTransition->fDuration        = (float)m_pSpinFilterDelay->value ( );
       m_pTransition->bUserFile        = false;
       m_pTransition->iTransitionType  = 1; // AlphaTrans
       m_pTransition->qsTransitionName = m_qsTransitionName;
       if ( ! m_bMultiMode && m_pCheckUserDefined->isChecked ( ) && ! m_pEditUserFile->text ( ).isEmpty ( ) )  {
         m_pTransition->bUserFile      = true;
         m_pTransition->qsTransitionVideoFileName = m_pEditUserFile->text ( );
       }
  }
  //uiDialogButtonTrans::accept ( );	//ooo
  QDialog::accept ( );			//xxx
}

void DialogButtonTrans::slotNoTransition ( )
{  
  if ( m_pTransition )
    delete m_pTransition;
  m_pTransition   = NULL;
  m_listEnabled.clear( );

  //uiDialogButtonTrans::accept ( );	//ooo
  QDialog::accept ( );			//xxx
}

void DialogButtonTrans::slotHelp()  //xxx
{
	MessageBox::warning(this, "ButtonDialog", "Cool you got a button !!!\n", 
		QMessageBox::Ok, QMessageBox::NoButton);
}

