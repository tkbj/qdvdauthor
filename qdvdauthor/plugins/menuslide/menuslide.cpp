
#include <stdlib.h>

#include <qimage.h>
#include <qpixmap.h>
#include <qcursor.h>
//#include <q3listbox.h>    //ooo
#include <QListWidget>      //xxx
#include <qpainter.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>
#include <qradiobutton.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QMessageBox>  //xxx

#include "../../win32.h"
#include "../../CONFIG.h" // get PREFIX_DIRECTORY
#include "menuslide.h"

namespace Plugin
{

MenuSlideInterface::MenuSlideInterface ( )    //ooo
//MenuSlideInterface::MenuSlideInterface ( QWidget *pParent ) //xxx
  : Interface ( ),
    MenuSlide ( ) //ooo
    //MenuSlide ( pParent )   //xxx
{
}

bool MenuSlideInterface::execute  ( )
{
  MenuSlide::show ( );
  return true;
};

void MenuSlideInterface::doEmit ( SourceGroup::type theType, QString &qsMenuName, QString &qsText )
{
  emit ( signalNewMenuResponse ( theType, qsMenuName, qsText ) );
//  emit ( signalPluginResponse ( Plugin::Interface::NTSCMenuResponse, qsMenuName, qsText ) );
}

MenuSlide::ImageContainer::ImageContainer ( QString &qsFile )
{
  QFileInfo fileInfo ( qsFile );
  qsFileName = qsFile;
  qsBaseName = fileInfo.fileName ( );
}

//MenuSlide::MenuSlide( QWidget *pParent, const char *pName, bool bModal, Qt::WFlags fl )	//ooo
MenuSlide::MenuSlide( QWidget *pParent )							//xxx
    //: uiDialogMenuSlide ( pParent, pName, bModal, fl )	//ooo
    : QDialog ( pParent )					//xxx
{
  setupUi(this);	//xxx

  m_iMaxWidth  = 200;
  m_iMaxHeight = 200;
  m_iConcurrentImages = 3;

  m_pEditImageDuration->setText ( "5" );
  m_pEditTotalDuration->setText ( "0" );

  connect ( m_pButtonUp,           SIGNAL ( clicked ( ) ), SLOT ( slotUp         ( ) ) );
  connect ( m_pButtonDown,         SIGNAL ( clicked ( ) ), SLOT ( slotDown       ( ) ) );
  connect ( m_pButtonDelete,       SIGNAL ( clicked ( ) ), SLOT ( slotDelete     ( ) ) );
  connect ( m_pButtonHelp,         SIGNAL ( clicked ( ) ), SLOT ( slotHelp       ( ) ) );
  connect ( m_pButtonAddImages,    SIGNAL ( clicked ( ) ), SLOT ( slotAddImages  ( ) ) );
  connect ( m_pButtonPreview,      SIGNAL ( clicked ( ) ), SLOT ( slotPreview    ( ) ) );
  connect ( m_pButtonBackground,   SIGNAL ( clicked ( ) ), SLOT ( slotBackground ( ) ) );
  connect ( m_pRadioImageDuration, SIGNAL ( toggled ( bool ) ), SLOT ( slotToggledID ( bool ) ) );
  connect ( m_pRadioTotalDuration, SIGNAL ( toggled ( bool ) ), SLOT ( slotToggledTD ( bool ) ) );
  connect ( m_pListBoxImages,      SIGNAL ( selectionChanged ( Q3ListBoxItem * ) ), SLOT ( slotImageChanged ( Q3ListBoxItem * ) ) );
  
  //connect ( buttonOk,              SIGNAL ( clicked ( ) ), SLOT ( accept         ( ) ) );   //xxx
  
}

MenuSlide::~MenuSlide ( )
{
}

void MenuSlide::polish ( )
{
  //uiDialogMenuSlide::polish ( );	//ooo
  //QDialog::polish ( );			//xxx
  loadBackgroundImage ( );
}

void MenuSlide::loadBackgroundImage ( )
{  
  QString qsFileName = PREFIX_DIRECTORY"/share/qdvdauthor/plugins/menuslide/background/background00000.jpg";

  QPixmap thePixmap;
  QImage  theImage ( qsFileName );
  if ( theImage.isNull ( ) )
    return;

  //theImage = theImage.smoothScale ( m_pFrame->width ( ) - 10, m_pFrame->height ( ) - 10, Qt::KeepAspectRatio );   //ooo
  theImage = theImage.scaled ( m_pFrame->width ( ) - 10, m_pFrame->height ( ) - 10, Qt::KeepAspectRatio );  //xxx
  thePixmap.convertFromImage      ( theImage );
  //m_pFramePreview->setPaletteBackgroundPixmap ( thePixmap );  //ooo
  QPalette palette;								//xxx
  //palette.setBrush(QPalette::Base, QBrush(thePixmap));				//xxx
  palette.setBrush(m_pFramePreview->backgroundRole(), QBrush(thePixmap));				//xxx
  m_pFramePreview->setPalette(palette);					//xxx
  m_pFramePreview->setFixedSize   ( thePixmap.size ( ) );
  m_backgroundPixmap = thePixmap;
}

void MenuSlide::slotUp ( )
{

}

void MenuSlide::slotDown ( )
{

}

void MenuSlide::slotDelete ( )
{
  int iCount = m_pListBoxImages->count ( );
  if ( iCount < 1 ) {
    buttonOk->setEnabled         (   false );
    m_pButtonPreview->setEnabled (   false );
    m_pButtonDelete->setEnabled  (   false );
    m_pButtonUp->setEnabled      (   false );
    m_pButtonDown->setEnabled    (   false );
  }
  else {
    bool bEnable = ( m_pListBoxImages->count ( ) > 1 );
    buttonOk->setEnabled         (    true );
    m_pButtonPreview->setEnabled (    true );
    m_pButtonDelete->setEnabled  (    true );
    m_pButtonUp->setEnabled      ( bEnable );
    m_pButtonDown->setEnabled    ( bEnable );
  }
}

void MenuSlide::slotHelp ( )
{

}

void MenuSlide::slotToggledID   ( bool bToggled )
{
  m_pRadioTotalDuration->setChecked( ! bToggled );
  m_pEditImageDuration->setEnabled (   bToggled );
  m_pEditTotalDuration->setEnabled ( ! bToggled );
}

void MenuSlide::slotToggledTD   ( bool bToggled )
{
  m_pRadioImageDuration->setChecked( ! bToggled );
  m_pEditImageDuration->setEnabled ( ! bToggled );
  m_pEditTotalDuration->setEnabled (   bToggled );
}

//void MenuSlide::slotImageChanged ( Q3ListBoxItem *pItem ) //ooo
void MenuSlide::slotImageChanged ( QListWidgetItem *pItem ) //xxx
{
  // Called when the user selects an image in the ListBox to the left ...
  int t;
  QString qsImage = pItem->text ( );

  QCursor myCursor     ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  for ( t=0; t<(int)m_listImages.count ( ); t++ ) {
    if ( m_listImages[t]->qsBaseName == qsImage ) {
      QImage *pImage = &m_listImages[t]->thumbnail;
      QPixmap thePixmap;
      if ( pImage->isNull ( ) ) {
	int iWidth  = m_pFrame->width  ( ) - 10;
	int iHeight = m_pFrame->height ( ) - 10;
	*pImage = QImage ( m_listImages[t]->qsFileName );
	//*pImage = pImage->smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio );  //ooo
    *pImage = pImage->scaled ( iWidth, iHeight, Qt::KeepAspectRatio );  //xxx
      }
      thePixmap.convertFromImage ( *pImage );
      //m_pFramePreview->setPaletteBackgroundPixmap ( thePixmap );  //ooo
      QPalette palette;								//xxx
      palette.setBrush(QPalette::Window, QBrush(thePixmap));				//xxx
      m_pFramePreview->setPalette(palette);					//xxx
      m_pFramePreview->setFixedSize ( thePixmap.size (      ) );
      QApplication::restoreOverrideCursor ( );
      return;
    }
  }
  QApplication::restoreOverrideCursor ( );
}

void MenuSlide::slotAddImages ( )
{
  int   t;
  char *pHomeDir = getenv ( "HOME" );
  //QString qsFilter  ( " *.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.BMP *.XBM \n" );
  QString qsFilter  ( "Image Files ( *.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.BMP *.XBM )" );
  QString qsHomeDir ( "./" );
  ImageContainer *pImage;

  if ( pHomeDir )
    qsHomeDir = QString ( pHomeDir );

  //QStringList list = FilePreviewDialog::getOpenFileNames ( qsHomeDir, qsFilter, this, "FileOpenDialog", uiDialogMenuSlide::tr ( "Add Images to list." ) );
  //QStringList list = Q3FileDialog::getOpenFileNames ( qsFilter, qsHomeDir, this, "FileOpenDialog", tr ( "Add Images to list." ) );    //xxx
  QStringList list = QFileDialog::getOpenFileNames ( this, "FileOpenDialog", qsHomeDir, qsFilter ); //xxx

  if ( list.count ( ) < 1 )
    return;

  for ( t=0; t<(int)list.count  ( ); t++ ) {
    pImage = new ImageContainer ( list[t] );
    //m_pListBoxImages->insertItem( pImage->qsBaseName );   //ooo
    m_pListBoxImages->addItem( pImage->qsBaseName );   //xxx
    m_listImages.append ( pImage );
  }

  bool bEnable = ( m_pListBoxImages->count ( ) > 1 );
  buttonOk->setEnabled         (    true );
  m_pButtonPreview->setEnabled (    true );
  m_pButtonDelete->setEnabled  (    true );
  m_pButtonDown->setEnabled    ( bEnable );
  m_pButtonUp->setEnabled      ( bEnable );

  createAnimationData ( false );
}

void MenuSlide::slotPreview ( )
{
  m_iCurrentFrame   =    0;
  loadBackgroundImage   ( );
  createAnimationData ( true );
  animate ( );
  m_iAnimationTimerID = startTimer ( 100 );
}

void MenuSlide::slotBackground ( )
{  
  loadBackgroundImage ( );
  /*
  // Look first in the local .qdvdauthor - fo;der and second in the system folder ( /usr/share/qdvdauthor )
  QString qsHomeDir, qsPrefixPath, qsBackroundFileName, qsFilter;
  int iPluginPath;
  QDir pluginDir;
  bool bFound = false;

  qsFilter  ( "*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.BMP *.XBM\n" );
  qsHomeDir = QDir::homeDirPath ( );

  qsPrefixPath = QString ( "%1/.qdvdauthor/plugins/menuslide/" ).arg ( qsHomeDir );
  for (  iPluginPath=0; iTemplatePath<2; iTemplatePath++ ) {
    if ( iPluginPath == 1 ) {
      // for the second run we'll get the system wide installed Plugins
      // Note: locally installed Plugins overrule System wide installed Plugins.
      qsPrefixDir = QString ("%1/share/qdvdauthor/plugins/menuslide/").arg ( PREFIX_DIRECTORY );
    }
    if ( ! pluginDir.exists ( qsPrefixDir ) )
      continue;

    bFound = true;
    QString qsBackgroundFileName = FilePreviewDialog::getOpenFileName ( qsHomeDir, qsFilter, this, "FileOpenDialog", uiDialogMenuSlide::tr ( "Add Images to list." ) );
    //QStringList list = QFileDialog::getOpenFileNames ( qsFilter, qsHomeDir, this, "FileOpenDialog", tr ( "Add Images to list." ) );
    
    if ( list.count ( ) < 1 )
      return;
    

  }
  */
}

void MenuSlide::createAnimationData ( bool bCreateThumbs )
{
  int   iTotalImages = m_listImages.count ( );
  float fImageLength = getImageDuration   ( );
  float fTotalLength = fImageLength   *   ( iTotalImages / m_iConcurrentImages + 1 );
  m_fImageDuration   = fImageLength;

  QString qsValue;
  float   fFPS = 25.0; // PAL
  //if ( m_pComboFormat->currentItem ( ) == 0 ) //ooo
  if ( m_pComboFormat->currentIndex ( ) == 0 )  //xxx
    fFPS = 29.97; // NTSC

  m_iTotalFrames   = (unsigned long)( fFPS * fTotalLength );
  m_fTotalDuration = fImageLength * (float)m_pListBoxImages->count ( );

  if ( m_pRadioImageDuration->isChecked ( ) ) {
    qsValue.sprintf ( "%0.1f", m_fTotalDuration );
    m_pEditTotalDuration->setText ( qsValue );
  }
  else {
    qsValue.sprintf ( "%0.1f", m_fImageDuration );
    m_pEditImageDuration->setText ( qsValue );
  }
  
  if ( bCreateThumbs ) {
    QCursor myCursor     ( Qt::WaitCursor );
    QApplication::setOverrideCursor ( myCursor );

    // preloading all images
    ImageContainer *pContainer;
    int t, iWidth, iHeight;
    iWidth  = m_pFrame->width  ( ) - 10;
    iHeight = m_pFrame->height ( ) - 10;
    
    for ( t=0; t<(int)m_listImages.count (); t++ ) {
      pContainer = m_listImages[t];
      if ( pContainer->thumbnail.isNull ( ) ) {
	pContainer->thumbnail = QImage ( pContainer->qsFileName );
	//pContainer->thumbnail = pContainer->thumbnail.smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio );  //ooo
    pContainer->thumbnail = pContainer->thumbnail.scaled ( iWidth, iHeight, Qt::KeepAspectRatio );  //xxx
      }
    }
    QApplication::restoreOverrideCursor ( );
  }
}

void MenuSlide::animate ( )
{
  // Here we do the preview animation
  static bool bIAmBusy = false;
  m_iCurrentFrame ++;
  if ( m_iCurrentFrame >= m_iTotalFrames ) {
    killTimer ( m_iAnimationTimerID );
    return;
  }

  printf ( "MenuSlide::animate : rendering frame number %d\n", (int)m_iCurrentFrame );

  if ( bIAmBusy )
    return;
  bIAmBusy = true;

  // Animation is done here ...
  animate ( m_iCurrentFrame );

  bIAmBusy = false;
}

void MenuSlide::animate ( unsigned long iFrameNumber )
{
  int t, iImageCount, iStartImage, iEndImage, iImageFrames;
//  int iImageWidth, iWidth, iImageHeight, iHeight;
  int iWidth, iHeight;
  //int iStartFrame, iEndFrame;
  float   fDeltaZoom, fFPS = 25.0f;
  QImage  theImage;
  QPixmap thePixmap;

  iImageCount = m_listImages.count ( );
  iWidth  = m_pFrame->width  ( ) - 10;
  iHeight = m_pFrame->height ( ) - 10;

  //if ( m_pComboFormat->currentItem ( ) == 0 ) {   //ooo
  if ( m_pComboFormat->currentIndex ( ) == 0 ) {    //xxx
    fFPS = 29.97f; // NTSC
    //iTotalHeight = 480;
  }

  iImageFrames= (int)( fFPS * m_fImageDuration ); // Nr Of Frames an image is visible.
  iStartImage = (int)( ( iFrameNumber * m_iConcurrentImages ) / iImageFrames );
  iEndImage   = iStartImage + m_iConcurrentImages;
  if ( iEndImage > iImageCount )
    iEndImage = iImageCount;
  thePixmap   = m_backgroundPixmap;
  QPainter thePainter ( &thePixmap );

  // only animate the currently visible images, forget the rest ...
  for ( t=iStartImage; t<iEndImage; t++ ) {
    fDeltaZoom = 1.0  / ( iFrameNumber - ( t * iImageFrames ) );
    if ( ( fDeltaZoom > 0.0 ) && ( fDeltaZoom <= 1.0 ) ) {
      printf ( "Image<%d>=<%0.3f=%0.3f> ", t, fDeltaZoom, fDeltaZoom * iFrameNumber);
//      iImageWidth  = (int)( fDeltaZoom * iFrameNumber * iWidth  );
//      iImageHeight = (int)( fDeltaZoom * iFrameNumber * iHeight );
      //theImage     = m_listImages[t]->thumbnail.smoothScale ( iWidth, iHeight, Qt::KeepAspectRatio ); //ooo
      theImage     = m_listImages[t]->thumbnail.scaled ( iWidth, iHeight, Qt::KeepAspectRatio );    //xxx

      thePainter.drawImage ( 20, 20, theImage );
    }
  }
  printf ( "\n" );
  // Finally we copy over the created image.
  //m_pFramePreview->setPaletteBackgroundPixmap ( thePixmap );  //ooo
  QPalette palette;								//xxx
  palette.setBrush(QPalette::Window, QBrush(thePixmap));				//xxx
  m_pFramePreview->setPalette(palette);					//xxx
  m_pFramePreview->setFixedSize ( thePixmap.size (      ) );    
}
/*
  for ( t=0; t<iImageFrames; t++ ) {
    qsAnimation += QString ( "echo \"%1::scale.x=%2\"\n" )
      .arg ( t + iStartFrame ).arg ( fDeltaZoom * t );
    qsAnimation += QString ( "echo \"%1::scale.y=%2\"\n" )
      .arg ( t + iStartFrame ).arg ( fDeltaZoom * t );
  }

  qsAnimation += QString ( "iStartFrame=%1\n" ).arg  ( iStartFrame   );
  qsAnimation += QString ( "iEndFrame=%1\n" ).arg    ( iEndFrame     );
  qsAnimation += QString ( "iStartHeight=%1\n" ).arg ( iStartHeight  );
  qsAnimation += QString ( "iTotalHeight=%1\n" ).arg ( iTotalHeight  );
  qsAnimation += QString ( "iCenterX=360\n" ); // Both PAL and NTSC = 720 width. Center is 360
  qsAnimation += QString ( "iEndX=%1\n\n" ).arg((int)( fDeltaX * 360 ) + 360 );

  qsAnimation += QString ( "iTotalSteps=$(( $iEndFrame - $iStartFrame - 1 )); \n" );
  qsAnimation += QString ( "iDeltaHeight=$(( $iTotalHeight - $iStartHeight )); \n" );
  qsAnimation += QString ( "iDeltaX=$(( $iEndX - $iCenterX )); \n" );
  qsAnimation += QString ( "i=0;\n\n" );

  qsAnimation += QString ( "for (( t=$iStartFrame; t&lt;$iEndFrame; t++ )) {\n" );
  qsAnimation += QString ( "    echo \"$t::pos.y=$(( $iStartHeight + $i * $iDeltaHeight / $iTotalSteps ))\"\n" );
  qsAnimation += QString ( "    echo \"$t::pos.x=$(( $iCenterX + $i * $iDeltaX / $iTotalSteps ))\"\n" );
  qsAnimation += QString ( "    i=$(( $i + 1 ))\n" );
  qsAnimation += QString ( "}\n" );
  qsAnimation += QString ( "     </AnimationObject>\n" );
*/

void MenuSlide::timerEvent ( QTimerEvent *pEvent )
{
  if ( pEvent->timerId ( ) == m_iAnimationTimerID )
    animate ( );
}

///////////////////////////////////////////////////////////////////////
//
// Everything below this is to actually create the menu.
//
///////////////////////////////////////////////////////////////////////

void MenuSlide::accept ( )
{  
  srandom  ( 0 );
  //QString qsText = testMenu ( );
  QString qsText = getMenuXml ( );
  QString qsMenuName ( "MenuSlide" );

  Plugin::SourceGroup::type theType = Plugin::SourceGroup::NTSC;
  //if ( m_pComboFormat->currentItem ( ) > 0 )  //ooo
  if ( m_pComboFormat->currentIndex ( ) > 0 )   //xxx
    theType = Plugin::SourceGroup::PAL;

  doEmit ( theType, qsMenuName, qsText );
  //printf ( "Accepted <%s>\n", qsText.ascii( ) );

  //uiDialogMenuSlide::accept ( );	//ooo
  QDialog::accept ( );			//xxx
}

QString MenuSlide::getMenuXml ( )
{
  QTime   theDuration;
  QString qsDuration, qsFormat = ( "hh:mm:ss.zzz" );

  int   iTotalImages = m_listImages.count ( );
  float fImageLength = getImageDuration   ( );
  int   iTotalLength = (int)( fImageLength * ( iTotalImages / m_iConcurrentImages + 1 ) );

  theDuration = theDuration.addSecs  ( iTotalLength );
  qsDuration  = theDuration.toString ( qsFormat );
  int t;
  QString qsMenu ( "\n"
		   " <DVDMenu MenuWidth=\"720\" MenuHeight=\"480\" MenuFormat=\"1\" MenuRatio=\"4:3\" >\n"
		   "  <MenuVariables>\n"
		   "   <MenuBackground TitlesetNr=\"0\" MenuNr=\"0\" />\n"
		   "   <MenuOffset>00:00:00.000</MenuOffset>\n"
		   "   <MenuDuration>" + qsDuration + "</MenuDuration>\n"
		   "   <MenuBackground>"PREFIX_DIRECTORY"/share/qdvdauthor/plugins/menuslide/background/background00000.jpg</MenuBackground>\n"
		   "   <MenuName>MenuSlide</MenuName>\n"
		   "   <MaskColors>#fefffe,#182b8c,#ff1414,#24289a,</MaskColors>\n"
		   "  </MenuVariables>\n"
		   "  <PreviewVariables>\n" ); //.arg ( qsDuration );

  //for ( t=0; t<(int)m_listImages.count ( ); t++ )
  for ( t = (int)m_listImages.count( )-1; t >= 0; t-- ) 
    qsMenu += buildImageObject ( m_listImages [ t ], t );

  qsMenu += QString ( "  </PreviewVariables>\n" );
  qsMenu += QString (  " </DVDMenu>\n" );
  qsMenu += QString ("\n" );

   return qsMenu;
}

QString MenuSlide::buildImageObject ( MenuSlide::ImageContainer *pImage, int iImageNumber )
{
  QString qsImage;
  QPixmap thePix ( pImage->qsFileName );
  int iWidth, iHeight;
  float fRatio;

  if ( thePix.isNull ( ) )
    return qsImage;
  iWidth  = thePix.width  ( );
  iHeight = thePix.height ( );
  fRatio = (float)iWidth / iHeight;
  if ( fRatio > 1.0 ) {
    iWidth  = m_iMaxWidth;
    iHeight = (int)(m_iMaxHeight / fRatio );
  }
  else {
    iHeight = m_iMaxHeight;
    iWidth  = (int)(m_iMaxWidth  * fRatio );
  }
  
  QFileInfo fileInfo ( pImage->qsFileName );

  qsImage  = QString ( "   <ImageObject FileName=\"%1\" >\n" ).arg ( pImage->qsFileName );
  qsImage += QString ( "    <ShadowObject ColorFading=\"0.434\" SunAngle=\"180\" BlurRadius=\"0\" Transparency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#231e83\" />\n" );
  qsImage += QString ( "    <MenuObject Width=\"%1\"  Height=\"%2\" X=\"279\" Y=\"186\" ObjectName=\"%3\" ObjectExtractionPath=\"sea\" />\n" ).arg ( iWidth ).arg ( iHeight ).arg ( fileInfo.baseName ( ) );
  
  qsImage += buildAnimation ( iImageNumber );

  qsImage += QString ( "   </ImageObject>\n" );
  
  return qsImage;
}

////////////////////////////////////////////////////////////////////////////////////////
// Short desscription of the animation and how it works.
//
// It is making heavy usage of the StartFrame / EndFrame of he AnimationAttributes 
// Displaying ony those images which should be visible.
//
// fDeltaX [ -1.0 .. +1.0 ] represents the randomly picked value on the bottom where the 
// image is drifting to. E.g. a val of 0.0 would drift the image down the center line. 
// A val of -1 would end the image in the lower left corner.
//
// fZoom [ 0.0 .. +1.0 ]
//
////////////////////////////////////////////////////////////////////////////////////////
QString MenuSlide::buildAnimation ( int iImageNumber )
{  
//  int iTotalImages = m_listImages.count ( );
  double fDeltaX, fDeltaZoom, fFPS;
  double fImageLength;
  unsigned long iStartFrame, iEndFrame, iImageFrames, t;
  int iTotalHeight, iStartHeight = 230;

  QString qsAnimation; // the resulting Animation script.

  // There should be at least m_iConcurrentImages number of images visible at the same time.
  // So the total length of the animation is 
  // Length per Image * ( NrOfImages / ConcurrentImages + 1 )
  // the 2 = the fade in / fade out time for the first / last iConcurrent images

  // E.g. 9 images, 5 sec length per image, 3 concurrent images == 20 sec total length
  fImageLength = getImageDuration ( );
//  fTotalLength = fImageLength  *  ( iTotalImages / m_iConcurrentImages + 1 );

  fFPS = 25.0; // PAL
  iTotalHeight = 576;
  //if ( m_pComboFormat->currentItem ( ) == 0 ) {   //ooo
  if ( m_pComboFormat->currentIndex ( ) == 0 ) {    //xxx
    fFPS = 29.97; // NTSC
    iTotalHeight = 480;
  }

//  iTotalFrames = (unsigned long)( fFPS * fTotalLength );
  
  // Next we calculat the start / end frame.
  iStartFrame  = (unsigned long)( fImageLength * iImageNumber * fFPS / m_iConcurrentImages );
  iEndFrame    = (unsigned long)( iStartFrame  + fImageLength * fFPS );
  iImageFrames = iEndFrame - iStartFrame;

  fDeltaX    = ( (double) random ( ) / (double) RAND_MAX * 2.0 - 1.0 );
  fDeltaZoom = 1.0 / iImageFrames;

  qsAnimation += QString ( "     <AnimationObject>\n" );
  qsAnimation += QString ( "echo \"1::visibleFromTo.startFrame=%1\"\n" ).arg ( iStartFrame );
  qsAnimation += QString ( "echo \"1::visibleFromTo.stopFrame=%1\"\n"  ).arg ( iEndFrame   );
  for ( t=0; t<iImageFrames; t++ ) {
    qsAnimation += QString ( "echo \"%1::scale.x=%2\"\n" ).arg ( t + iStartFrame ).arg ( fDeltaZoom * t );
    qsAnimation += QString ( "echo \"%1::scale.y=%2\"\n" ).arg ( t + iStartFrame ).arg ( fDeltaZoom * t );
  }

  qsAnimation += QString ( "iStartFrame=%1\n" ).arg  ( iStartFrame   );
  qsAnimation += QString ( "iEndFrame=%1\n" ).arg    ( iEndFrame     );
  qsAnimation += QString ( "iStartHeight=%1\n" ).arg ( iStartHeight  );
  qsAnimation += QString ( "iTotalHeight=%1\n" ).arg ( iTotalHeight  );
  qsAnimation += QString ( "iCenterX=360\n" ); // Both PAL and NTSC = 720 width. Center is 360
  qsAnimation += QString ( "iEndX=%1\n\n" ).arg((int)( fDeltaX * 360 ) + 360 );

  qsAnimation += QString ( "iTotalSteps=$(( $iEndFrame - $iStartFrame - 1 )); \n" );
  qsAnimation += QString ( "iDeltaHeight=$(( $iTotalHeight - $iStartHeight )); \n" );
  qsAnimation += QString ( "iDeltaX=$(( $iEndX - $iCenterX )); \n" );
  qsAnimation += QString ( "i=0;\n\n" );

  qsAnimation += QString ( "for (( t=$iStartFrame; t&lt;$iEndFrame; t++ )) {\n" );
  qsAnimation += QString ( "    echo \"$t::pos.y=$(( $iStartHeight + $i * $iDeltaHeight / $iTotalSteps ))\"\n" );
  qsAnimation += QString ( "    echo \"$t::pos.x=$(( $iCenterX + $i * $iDeltaX / $iTotalSteps ))\"\n" );
  qsAnimation += QString ( "    i=$(( $i + 1 ))\n" );
  qsAnimation += QString ( "}\n" );
  qsAnimation += QString ( "     </AnimationObject>\n" );
 
  return qsAnimation;
}

float MenuSlide::getImageDuration ( )
{
  float    fValue;
  if ( m_pRadioImageDuration->isChecked ( ) ) {
    fValue = m_pEditImageDuration->text ( ).toFloat ( );
    if ( fValue < 0.5f )
         fValue = 5.0f;
  }
  else {
    fValue = m_pEditTotalDuration->text ( ).toFloat ( );
    if ( fValue < 1.0f )
      fValue = 5.0f;
    else
      fValue = fValue / (float)m_pListBoxImages->count ( );
  }
  return fValue;
}

/*
QString MenuSlide::getMenuXml ( )
{
  QString qsMenu ( "\n"
  " <DVDMenu MenuWidth=\"720\" MenuHeight=\"480\" MenuFormat=\"1\" MenuRatio=\"4:3\" >\n"
  "  <MenuVariables>\n"
  "   <MenuBackground TitlesetNr=\"0\" MenuNr=\"0\" />\n"
  "   <MenuOffset>00:00:00.000</MenuOffset>\n"
  "   <MenuDuration>00:00:01.000</MenuDuration>\n"
  "   <MenuBackground>/home/varol/.qdvdauthor/static/blueplanet/background/background00000.jpg</MenuBackground>\n"
  "   <MenuName>MenuSlide</MenuName>\n"
  "   <MaskColors>#fefffe,#182b8c,#ff1414,#24289a,</MaskColors>\n"
  "  </MenuVariables>\n"
  "  <PreviewVariables>\n" );
  "   <ImageObject FileName=\"/home/varol/dvdproject/sea.jpg\" >\n"
  "    <Manipulator>\n"
  "     <Modifiers Zoom=\"0.0911458\" />\n"
  "    </Manipulator>\n"
  "    <ShadowObject ColorFading=\"0.434\" SunAngle=\"180\" BlurRadius=\"0\" Transparency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#231e83\" />\n"
  "    <MenuObject Width=\"175\" X=\"279\" Y=\"186\" Height=\"100\" ObjectName=\"sea\" ObjectExtractionPath=\"sea\" >\n"
  "     <Modifiers Zoom=\"0.0911458\" />\n"
  "    </MenuObject>\n"
  "   </ImageObject>\n"
  "   <ImageObject FileName=\"/home/varol/dvdproject/beach.jpg\" >\n"
  "    <Manipulator>\n"
  "     <Modifiers Zoom=\"0.21875\" />\n"
  "    </Manipulator>\n"
  "    <ShadowObject ColorFading=\"0.434\" SunAngle=\"180\" BlurRadius=\"0\" Transparency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#231e83\" />\n"
  "    <MenuObject Width=\"175\" X=\"496\" Y=\"212\" Height=\"117\" ObjectName=\"beach\" ObjectExtractionPath=\"beach\" >\n"
  "     <Modifiers Zoom=\"0.21875\" />\n"
  "    </MenuObject>\n"
  "   </ImageObject>\n"
  "   <ImageObject FileName=\"/home/varol/dvdproject/elements.jpg\" >\n"
  "    <Manipulator>\n"
  "     <Modifiers Zoom=\"0.1025\" />\n"
  "    </Manipulator>\n"
  "    <ShadowObject ColorFading=\"0.457\" SunAngle=\"180\" BlurRadius=\"0\" Transarency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#231e83\" />\n"
  "    <MenuObject Width=\"200\" X=\"49\" Y=\"196\" Height=\"151\" ObjectName=\"elements\" ObjectExtractionPath=\"elements\" >\n"
  "     <Modifiers Zoom=\"0.1025\" />\n"
  "    </MenuObject>\n"
  "   </ImageObject>\n"
  "  </PreviewVariables>\n"
  " </DVDMenu>\n"
  "\n" );

   return qsMenu;
}
*/
QString MenuSlide::testMenu ( )
{
  // qsText = m_pEdit->text ( );
  QString qsText ( "\n"
" <DVDMenu MenuWidth=\"720\" MenuHeight=\"480\" MenuFormat=\"1\" MenuRatio=\"4:3\" >\n"
"  <MenuVariables>\n"
"   <MenuBackground TitlesetNr=\"0\" MenuNr=\"0\" />\n"
"   <MenuOffset>00:00:00.000</MenuOffset>\n"
"   <MenuDuration>00:00:01.000</MenuDuration>\n"
"   <MenuBackground ResizeAlgo=\"Normal Qt\" OffsetX=\"0\" OffsetY=\"0\" StretchType=\"0\" >/home/varol/.qdvdauthor//static/blueplanet/background/background00000.jpg</MenuBackground>\n"
"   <MenuName>blueplanet</MenuName>\n"
"   <MaskColors>#fefffe,#182b8c,#ff1414,#24289a,</MaskColors>\n"
"  </MenuVariables>\n"
"  <PreviewVariables>\n"
"   <ButtonObject Down=\"-- default --\" Left=\"-- default --\" Action=\"jump+-+vmgm+-+\" Right=\"-- default --\" ButtonName=\"Button 2\" Up=\"-- default --\" SourceEntry=\"[02] - source\" >\n"
"    <NormalState>\n"
"     <ImageObject FileName=\"/home/varol/dvdproject/earth.bmp\" >\n"
"      <Manipulator>\n"
"       <Modifiers Zoom=\"0.0911458\" />\n"
"      </Manipulator>\n"
"      <ShadowObject ColorFading=\"0.434\" SunAngle=\"186.32\" BlurRadius=\"0\" Transparency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#000000\" />\n"
"      <MenuObject Width=\"175\" X=\"279\" Y=\"186\" Height=\"88\" ObjectName=\"earth\" ObjectExtractionPath=\"earth\" >\n"
"       <Modifiers Zoom=\"0.0911458\" />\n"
"      </MenuObject>\n"
"     </ImageObject>\n"
"    </NormalState>\n"
"    <SelectedState>\n"
"     <FrameObject Width=\"2\" Color=\"#ff1414\" >\n"
"      <MenuObject Width=\"175\" X=\"279\" Y=\"186\" Height=\"88\" ObjectName=\"Frame (279, 186, 175, 98)\" >\n"
"       <Modifiers Zoom=\"0.0911458\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </SelectedState>\n"
"    <HighlightedState>\n"
"     <FrameObject Width=\"2\" Color=\"#182b8c\" >\n"
"      <MenuObject Width=\"175\" X=\"279\" Y=\"186\" Height=\"88\" ObjectName=\"Frame (279, 186, 175, 98)\" >\n"
"       <Modifiers Zoom=\"0.0911458\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </HighlightedState>\n"
"    <MenuObject Width=\"175\" X=\"279\" Y=\"186\" Height=\"88\" ObjectName=\"Button 2\" >\n"
"     <Modifiers Zoom=\"0.0911458\" />\n"
"    </MenuObject>\n"
"   </ButtonObject>\n"
"   <ButtonObject Down=\"-- default --\" Left=\"-- default --\" SourceEntry=\"[02] - source\" Action=\"jump+-+[02] - source+-+cordes.mpg.mpg+-+00:00:00.000\" Right=\"-- default --\" ButtonName=\"Button 3\" Up=\"-- default --\" >\n"
"    <NormalState>\n"
"     <MovieObject FileName=\"/home/varol/dvdproject/cordes.mpg.mpg\" >\n"
"      <Manipulator>\n"
"       <Modifiers Zoom=\"0.21875\" />\n"
"      </Manipulator>\n"
"      <ShadowObject ColorFading=\"0.434\" SunAngle=\"178.34\" BlurRadius=\"0\" Transparency=\"0.2063\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"2\" CutOff=\"0.306\" Color=\"#000000\" />\n"
"      <MenuObject Width=\"175\" X=\"496\" Y=\"212\" Height=\"117\" ObjectName=\"cordes\" ObjectExtractionPath=\"cordes\" >\n"
"       <Modifiers Zoom=\"0.21875\" />\n"
"      </MenuObject>\n"
"     </MovieObject>\n"
"    </NormalState>\n"
"    <SelectedState>\n"
"     <FrameObject Width=\"2\" Color=\"#ff1414\" >\n"
"      <MenuObject Width=\"175\" X=\"496\" Y=\"212\" Height=\"117\" ObjectName=\"Frame (496, 212, 175, 116)\" >\n"
"       <Modifiers Zoom=\"0.21875\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </SelectedState>\n"
"    <HighlightedState>\n"
"     <FrameObject Width=\"2\" Color=\"#182b8c\" >\n"
"      <MenuObject Width=\"175\" X=\"496\" Y=\"212\" Height=\"117\" ObjectName=\"Frame (496, 212, 175, 116)\" >\n"
"       <Modifiers Zoom=\"0.21875\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </HighlightedState>\n"
"    <MenuObject Width=\"175\" X=\"496\" Y=\"212\" Height=\"117\" ObjectName=\"Button 3\" >\n"
"     <Modifiers Zoom=\"0.21875\" />\n"
"    </MenuObject>\n"
"   </ButtonObject>\n"
"   <ButtonObject Down=\"-- default --\" Left=\"-- default --\" SourceEntry=\"[01] - elephantsdream-1024-h264-st-aac\" Action=\"jump+-+[01] - elephantsdream-1024-h264-st-aac+-+elephantsdream-1024-h264-st-aac.mov+-+00:00:00.000\" Right=\"-- default --\" ButtonName=\"Button 1\" Up=\"-- default --\" >\n"
"    <NormalState>\n"
"     <MovieObject FileName=\"/home/varol/dvdproject/elephantsdream-1024-h264-st-aac.mov\" >\n"
"      <Manipulator>\n"
"       <Modifiers Zoom=\"0.1025\" ScaleX=\"1.01\" />\n"
"      </Manipulator>\n"
"      <ShadowObject ColorFading=\"0.457\" SunAngle=\"180\" BlurRadius=\"0\" Type=\"2\" ColorFadeTo=\"#231e83\" Distance=\"5\" CutOff=\"0.306\" Color=\"#231e83\" />\n"
"      <MenuObject Width=\"200\" X=\"49\" Y=\"196\" Height=\"113\" ObjectName=\"elephantsdream-1024-h264-st-aac\" ObjectExtractionPath=\"elephantsdream-1024-h264-st-aac\" >\n"
"       <Modifiers Zoom=\"0.1025\" ScaleX=\"1.01\" />\n"
"      </MenuObject>\n"
"     </MovieObject>\n"
"    </NormalState>\n"
"    <SelectedState>\n"
"     <FrameObject Width=\"2\" Color=\"#ff1414\" >\n"
"      <MenuObject Width=\"200\" X=\"49\" Y=\"196\" Height=\"113\" ObjectName=\"Frame (49, 196, 200, 150)\" >\n"
"       <Modifiers Zoom=\"0.1025\" ScaleX=\"1.01\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </SelectedState>\n"
"    <HighlightedState>\n"
"     <FrameObject Width=\"2\" Color=\"#24289a\" >\n"
"      <MenuObject Width=\"200\" X=\"49\" Y=\"196\" Height=\"113\" ObjectName=\"Frame (49, 196, 200, 150)\" >\n"
"       <Modifiers Zoom=\"0.1025\" ScaleX=\"1.01\" />\n"
"      </MenuObject>\n"
"     </FrameObject>\n"
"    </HighlightedState>\n"
"    <MenuObject Width=\"200\" X=\"49\" Y=\"196\" Height=\"113\" ObjectName=\"Button 1\" >\n"
"     <Modifiers Zoom=\"0.1025\" ScaleX=\"1.01\" />\n"
"    </MenuObject>\n"
"   </ButtonObject>\n"
"  </PreviewVariables>\n"
" </DVDMenu>\n"
"\n" );

  return qsText;  
}

}; // end of namespace Plugin
