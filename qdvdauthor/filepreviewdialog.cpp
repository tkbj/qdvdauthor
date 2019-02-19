/***************************************************************************
    filepreviewdialog.cpp
                             -------------------
    FilePreviewDialog - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <qlayout.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>
#include <QUrl>			//xxx
//Added by qt3to4:
//#include <Q3GridLayout>	//ooo
#include <QGridLayout>		//xxx
//#include <QLabel>		//ooo
#include <QMessageBox>

#ifdef QDVD_LINUX
extern bool qt_resolve_symlinks; // defined in qapplication.cpp
#else
static bool qt_resolve_symlinks = false;
#endif

#include "qplayer/mediacreator.h"
#include "filepreviewdialog.h"

FilePreviewDialog::FilePreviewDialog ( void *pExistingEngine, QWidget *pParent )
  : QLabel ( pParent )		//ooo
  //: QFileDialog ( pParent )	//xxx
{
/*  QVBoxLayout* box = new QVBoxLayout(this);		//xxx
  
  QLabel *mpPreview = new QLabel(tr("Preview"), this);	//xxx
  mpPreview->setAlignment(Qt::AlignCenter);		//xxx
  mpPreview->setObjectName("labelPreview");		//xxx
  mpPreview->setFixedSize(300,200);			//xxx
  box->addWidget(mpPreview);				//xxx
  
  box->addStretch();					//xxx
  
  // add to QFileDialog layout
  {
    QGridLayout *layout = (QGridLayout*)this->layout();	//xxx
    layout->addLayout(box, 1, 3, 3, 1);			//xxx
  }*/
  
  m_pMediaInterface = MediaCreator::createPreferredWidget (this, "MediaWidget", pExistingEngine);
  //Q3GridLayout *m_pVideoLayout = new Q3GridLayout (this, 1, 1, 4, 6, "MediaMainLayout");	//ooo
  //QGridLayout *m_pVideoLayout = new QGridLayout (this, 1, 1, 4, 6, "MediaMainLayout");	//xxx
  QGridLayout *m_pVideoLayout = new QGridLayout (this);						//xxx
  //m_pVideoLayout->addWidget (m_pMediaInterface->getWidget(), 0, 0);		//ooo
  m_pVideoLayout->addWidget (m_pMediaInterface->getWidget(), 1, 1, 4, 6);		//xxx

  m_pMediaInterface->initMediaEngine();
  setMinimumWidth ( 120 );	//ooo
  m_bSetupPreview = false;
}

FilePreviewDialog::~FilePreviewDialog()
{
  if ( m_pMediaInterface )  {
    m_pMediaInterface->stop ( );
    delete m_pMediaInterface;
  }
}

//void FilePreviewDialog::previewUrl ( const Q3Url &theUrl )		//ooo
void FilePreviewDialog::previewUrl ( const QUrl &theUrl )		//xxx
//void FilePreviewDialog::previewUrl ( const QString &path )		//xxx
{
  //QUrl theUrl = QUrl(path);  //xxx
    
  if ( m_bSetupPreview )
    return;

  if ( m_qsPath == theUrl.path () )
    return;

  if ( theUrl.path().right ( 1 ) == "/" )
    return;
  //printf ("FilePreviewDialog::previewUrl <%s> vs qsPAth=<%s>\n", theUrl.path ().ascii (), qsPath.ascii() );
  m_qsPath = theUrl.path ();

  m_bSetupPreview = true;
  QTimer::singleShot ( 100, this, SLOT ( slotSetPreview ( ) ) );	//ooo
}

void FilePreviewDialog::slotSetPreview ( )
{
  QImage theImage ( m_qsPath );
  if ( ! theImage.isNull ( ) ) {
    QPixmap thePixmap;
    m_pMediaInterface->stop ( );
    m_pMediaInterface->getWidget  ( )->hide ( );
    //theImage = theImage.smoothScale ( width ( ), height ( ), Qt::KeepAspectRatio );		//ooo
    //theImage = theImage.scaled ( width ( ), height ( ), Qt::KeepAspectRatio );			//xxx
    thePixmap.convertFromImage ( theImage );
    //setPixmap ( thePixmap );		//ooo
  }
  else {
    //setText ( "." );			//ooo
    m_pMediaInterface->getWidget ( )->show ( );
    m_pMediaInterface->playMRL   ( m_qsPath  );
    m_pMediaInterface->setVolume    ( 0.1f );
  }

  m_bSetupPreview = false;
}

QStringList FilePreviewDialog::getOpenFileNames ( void *pExistingEngine, const QString &qsStartIn, const QString &qsFilter, QWidget *pParent, const char *pName, const QString &qsCaption, QString *pSelectedFilter, bool bResolveSymlinks )
{
  //////////////////////////////////////////////////////////////////////
  // This function mimics the QFileDialog::getOpenFileName function.  //
  // I implemented here again for two main reasons.                   // 
  // 1) I can create a preview for Video streams / Image files        //
  // 2) In SuSE 10.x the getOpenFile function was acting weired       //
  //    and using KDEs FileOpen Dialog with the max Screen extension. //
  //////////////////////////////////////////////////////////////////////
  

  QStringList listOfFile;

  FilePreviewDialog  *pPreview = new FilePreviewDialog ( pExistingEngine );
  //Q3FileDialog *pDialog = new Q3FileDialog ( qsStartIn, QString::null, pParent, pName, false );// true );		//ooo
  //QFileDialog *pDialog = new QFileDialog ( pParent, QString("Caption"), QString("/home"), QString("Filter") );	//xxx
  QFileDialog *pDialog = new QFileDialog ( pParent, qsCaption, QString("/home"), qsFilter );				//xxx

  bool save_qt_resolve_symlinks = qt_resolve_symlinks;
  qt_resolve_symlinks = bResolveSymlinks;

  if ( pSelectedFilter )
    //pDialog->setFilter               ( *pSelectedFilter );			//ooo
    pDialog->setNameFilter               ( *pSelectedFilter );			//xxx
  //pDialog->setMode                   ( Q3FileDialog::ExistingFiles );		//ooo
  pDialog->setFileMode                   ( QFileDialog::ExistingFiles );	//xxx
  //pDialog->setContentsPreviewEnabled ( true );
  //pDialog->setContentsPreview        ( pPreview, (Q3FilePreview *)pPreview );	//ooo
  //pDialog->setContentsPreview        ( pPreview, (QFileDialog *)pPreview );	//xxx
  //pDialog->setPreviewMode            ( Q3FileDialog::Contents );		//ooo
  //pDialog->setPreviewMode            ( QFileDialog::Contents );			//xxx
  //pDialog->setCaption                ( qsCaption );				//ooo
  //pDialog->layout ( )->setResizeMode ( QLayout::SetMinimumSize ); //FreeResize );	//ooo
  //pDialog->layout ( )->addWidget(pPreview);						//xxxx
  //pDialog->setFilters                ( qsFilter );	//ooo
  //pDialog->setFilter                 ( qsFilter );	//xxx
  //QStringList filters = pDialog->filters();		//ooo
  QStringList filters = pDialog->nameFilters();		//xxx
  filters << tr("JPEG files (*.jpg *.jpeg)");
  //pDialog->setFilters(filters);			//ooo
  pDialog->setNameFilters(filters);			//xxx
  //pDialog->resize                    ( 650, 300 );    //ooo
  pDialog->resize                    ( 910, 300 );      //xxx
  
  QVBoxLayout* box = new QVBoxLayout (pDialog);	            //xxx
  
  //QLabel *mpPreview = new QLabel(tr("Preview"), pDialog);   //xxx
  pPreview->setAlignment(Qt::AlignCenter);                 //xxx
  pPreview->setObjectName("labelPreview");                 //xxx
  pPreview->setFixedSize(300,200);                         //xxx
  box->addWidget(pPreview);                                //xxx
 
  box->addStretch();                                        //xxx
  
  QGridLayout *layout = (QGridLayout*)pDialog->layout();    //xxx
  layout->addLayout(box, 1, 3, 3, 1);                       //xxx
  
  connect(pDialog, SIGNAL(currentUrlChanged(const QUrl&)), pPreview, SLOT(previewUrl(const QUrl&)));  //xxx
  //connect(pDialog, SIGNAL(currentChanged(const QString&)), pDialog, SLOT(previewUrl(const QString&)));  //xxx
  //connect(pDialog, SIGNAL(currentChanged(const QString&)), pDialog, SLOT(slotSetPreview ( )));  //xxx

  if ( pDialog->exec () == QDialog::Accepted )	//ooo
    listOfFile = pDialog->selectedFiles ();		//ooo

  //if ( pPreview->exec () == QDialog::Accepted )		//xxx
    //listOfFile = pPreview->selectedFiles ();		//xxx
  
  //delete pPreview;	//ooo
  //delete pDialog;	//ooo

  qt_resolve_symlinks = save_qt_resolve_symlinks;

  return listOfFile;
}

QStringList FilePreviewDialog::getOpenFileNames ( const QString &qsStartIn, const QString &qsFilter, QWidget *pParent, const char *pName, const QString &qsCaption, QString *pSelectedFilter, bool bResolveSymlinks )
{
  //////////////////////////////////////////////////////////////////////
  // This function mimics the QFileDialog::getOpenFileName function.  //
  // I implemented here again for two main reasons.                   // 
  // 1) I can create a preview for Video streams / Image files        //
  // 2) In SuSE 10.x the getOpenFile function was acting weired       //
  //    and using KDEs FileOpen Dialog with the max Screen extension. //
  //////////////////////////////////////////////////////////////////////

  QStringList listOfFile;

  //Q3FileDialog *pDialog = new Q3FileDialog ( qsStartIn, QString::null, pParent, pName, false );// true );	//ooo
  //QFileDialog *pDialog = new QFileDialog ( qsStartIn, QString::null, pParent, pName, false );// true );		//xxx
  QFileDialog *pDialog = new QFileDialog ( pParent, QString("Caption"), QString(), QString() );		//xxx

  bool save_qt_resolve_symlinks = qt_resolve_symlinks;
  qt_resolve_symlinks = bResolveSymlinks;

  if ( pSelectedFilter )
    //pDialog->setFilter               ( *pSelectedFilter );			//ooo
    pDialog->setNameFilter               ( *pSelectedFilter );			//xxx

  //pDialog->setMode                   ( Q3FileDialog::ExistingFiles );		//ooo
  pDialog->setFileMode                   ( QFileDialog::ExistingFiles );	//xxx
  //pDialog->setCaption                ( qsCaption );				//ooo

  //pDialog->layout ()->setResizeMode  ( QLayout::SetMinimumSize ); //FreeResize );	//ooo

  //pDialog->setFilters ( qsFilter );		//ooo
  pDialog->setNameFilter  ( qsFilter );		//xxx
  pDialog->resize ( 650, 300 );

  if ( pDialog->exec () == QDialog::Accepted )
    listOfFile = pDialog->selectedFiles ();

  delete pDialog;

  qt_resolve_symlinks = save_qt_resolve_symlinks;

  return listOfFile;
}

QString FilePreviewDialog::getOpenFileName ( void *pExistingEngine, const QString &qsStartIn, const QString &qsFilter, QWidget *pParent, const char *pName, const QString &qsCaption, QString *pSelectedFilter, bool bResolveSymlinks )
{
  //////////////////////////////////////////////////////////////////////
  // This function mimics the QFileDialog::getOpenFileName function.  //
  // I implemented here again for two main reasons.                   // 
  // 1) I can create a preview for Video streams / Image files        //
  // 2) In SuSE 10.x the getOpenFile function was acting weired       //
  //    and using KDEs FileOpen Dialog with the max Screen extension. //
  //////////////////////////////////////////////////////////////////////
  //QString qsFile;	//ooo
  QStringList qsFile;	//xxx

  FilePreviewDialog  *pPreview = new FilePreviewDialog ( pExistingEngine );
  //Q3FileDialog *pDialog = new Q3FileDialog ( qsStartIn, QString::null, pParent, pName, false );// true );	//ooo
  //QFileDialog *pDialog = new QFileDialog ( qsStartIn, QString::null, pParent, pName, false );// true );	//xxx
  QFileDialog *pDialog = new QFileDialog ( pParent, QString("Caption"), QString(), QString() );		//xxx

  bool save_qt_resolve_symlinks = qt_resolve_symlinks;
  qt_resolve_symlinks = bResolveSymlinks;

  if ( pSelectedFilter )
    //pDialog->setFilter               ( *pSelectedFilter );			//ooo
    pDialog->setNameFilter               ( *pSelectedFilter );			//xxx
  //pDialog->setMode                   ( Q3FileDialog::ExistingFile );		//ooo
  pDialog->setFileMode                   ( QFileDialog::ExistingFile );		//xxx
  //pDialog->setContentsPreviewEnabled ( true );				//ooo
  //pDialog->setContentsPreview        ( pPreview, (Q3FilePreview *)pPreview );	//ooo
  //pDialog->setContentsPreview        ( pPreview, (QFileDialog *)pPreview );	//xxx
  //pDialog->setPreviewMode            ( Q3FileDialog::Contents );		//ooo
  //pDialog->setPreviewMode            ( QFileDialog::Contents );			//xxx
  //pDialog->setCaption                ( qsCaption );				//ooo
  //pDialog->layout ( )->setResizeMode ( QLayout::SetMinimumSize ); //FreeResize );	//ooo
  //pDialog->setFilters                ( qsFilter );		//ooo
  pDialog->setNameFilter                 ( qsFilter );		//xxx
  pDialog->resize                    ( 650, 300 );

  if ( pDialog->exec ( ) == QDialog::Accepted )
    //qsFile = pDialog->selectedFile ( );	//ooo
    qsFile = pDialog->selectedFiles ( );	//xxx

  //delete pPreview;	//ooo
  //delete pDialog;	//ooo

  qt_resolve_symlinks = save_qt_resolve_symlinks;

  //return qsFile;	//ooo
  return qsFile[0];	//xxx
}
