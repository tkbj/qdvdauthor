/****************************************************************************
** dialogslideshow.cpp
**
**   Created : Wed Jun 4'th 2008 
**        by : Varol Okan using vi
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
*****************************************************************************/

#include <unistd.h>

#include <qlabel.h>
#include <qimage.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qspinbox.h>
//#include <q3listbox.h>    //ooo
#include <qfileinfo.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
//#include <q3groupbox.h>   //ooo
#include <QGroupBox>        //xxx
//#include <q3listview.h>   //ooo
#include <QTreeWidget>      //xxx
#include <qpushbutton.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qcolordialog.h>

#include "utils.h"
#include "global.h"
#include "messagebox.h"
#include "qdvdauthor.h"
#include "messagebox.h"
#include "xml_slideshow.h"
#include "render_client.h"
#include "dialogkenburns.h"   //ooo
#include "dialogslideshow.h"
//#include "dialogalphatrans.h" //ooo
#include "filepreviewdialog.h"
#include "qplayer/mediainfo.h"

namespace Input
{

DialogSlideshow::DialogSlideshow ( QWidget *pParent )
  //: uiSlideshow ( pParent )	//ooo
  : QDialog ( pParent )		//xxx
{
  int t;
  for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
    m_arrayPix[t] = new QPixmap;
  }
  m_iCurrentPix = 0;
  m_pTimer      = NULL;
  m_bExitNow    = false;
  m_bAnimated   = false;
  m_pSlideshow  = NULL;
  
  setupUi(this);	//xxx

  connect ( m_pButtonOkay,      SIGNAL ( clicked ( ) ), this, SLOT ( accept ( ) ) );
  connect ( m_pButtonCancel,    SIGNAL ( clicked ( ) ), this, SLOT ( reject ( ) ) );
  connect ( m_pButtonImage,     SIGNAL ( clicked ( ) ), this, SLOT ( slotImage       ( ) ) );
  connect ( m_pButtonColor,     SIGNAL ( clicked ( ) ), this, SLOT ( slotColor       ( ) ) );
  connect ( m_pButtonAdd,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAddAudio    ( ) ) );
  connect ( m_pButtonDelete,    SIGNAL ( clicked ( ) ), this, SLOT ( slotDeleteAudio ( ) ) );
  connect ( m_pButtonSelect,    SIGNAL ( clicked ( ) ), this, SLOT ( slotSelect      ( ) ) );
  connect ( m_pButtonAll,       SIGNAL ( clicked ( ) ), this, SLOT ( slotAll         ( ) ) );
  connect ( m_pButtonClear,     SIGNAL ( clicked ( ) ), this, SLOT ( slotClear       ( ) ) );
  connect ( m_pButtonAnim,      SIGNAL ( clicked ( ) ), this, SLOT ( slotAnim        ( ) ) );
  connect ( m_pButtonKBParams,  SIGNAL ( clicked ( ) ), this, SLOT ( slotKBParams    ( ) ) );
  connect ( m_pCheckIntroPage,  SIGNAL ( toggled     ( bool ) ), this, SLOT ( slotIntroPage   ( bool ) ) );
  connect ( m_pCheckKenBurns,   SIGNAL ( toggled     ( bool ) ), this, SLOT ( slotKenBurns    ( bool ) ) );
  connect ( m_pSpinImageDelay,  SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotValueChanged ( int ) ) );
  connect ( m_pSpinFilterDelay, SIGNAL ( valueChanged ( int ) ), this, SLOT ( slotValueChanged ( int ) ) );
  connect ( m_pListViewTransitions,  SIGNAL  ( selectionChanged ( Q3ListViewItem * ) ), this, SLOT ( slotSelectionChanged ( Q3ListViewItem * ) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogSlideshow::~DialogSlideshow ( )
{
  m_pSlideshow = NULL;

  if ( m_pTimer )
    delete m_pTimer;
  m_pTimer = NULL;

  int t;
  for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
    if ( m_arrayPix[t] )
      delete m_arrayPix[t];
    m_arrayPix[t] = NULL;
  }
}

void DialogSlideshow::accept ( )
{
  m_bExitNow = true;
  QDialog::accept     ( ); //done ( QDialog::Accepted ); //accept ( );
  qApp->processEvents ( );
  usleep ( 100000 );
}

void DialogSlideshow::reject ( )
{
  m_bExitNow = true;
  QDialog::reject     ( ); //done ( QDialog::Accepted ); //accept ( );
  qApp->processEvents ( );
  usleep ( 100000 );
}

QString DialogSlideshow::background ( )
{
  return m_qsBackground;
}

QStringList &DialogSlideshow::audioList ( )
{
  return m_listAudio;
}

QStringList DialogSlideshow::filterList ( )
{
  QStringList listOfCurrentSelections;

  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );   //xxx
  bool bAllFilter = true;
  while ( pItem )  {
    //if  ( pItem->isOn ( ) )   //ooo
    if  ( pItem->checkState ( 0 ) )    //xxx
      //listOfCurrentSelections.append ( pItem->text ( ) ); //ooo
      listOfCurrentSelections.append ( pItem->text ( 0 ) ); //xxx
    else
      bAllFilter = false;
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }
  // If all filter were selected, then we don't need to bother
  // But hey we only know if we go through all of them ... right ?
  if ( bAllFilter )
    listOfCurrentSelections.clear ( );

  return listOfCurrentSelections;
}

void DialogSlideshow::initMe ( CXmlSlideshow *pSlideshow, bool bEnableName )
{
  m_pSlideshow = pSlideshow;
  if ( ! pSlideshow )
    return;

  Utils theUtils;

  m_pSpinImageDelay->setValue   ( (int)pSlideshow->delay        );
  m_pCheckIntroPage->setChecked ( pSlideshow->intro_page        );
  m_pSpinFilterDelay->setValue  ( (int)pSlideshow->filter_delay );
  m_pEditName->setText          ( pSlideshow->slideshow_name    );
  m_pEditName->setEnabled       ( bEnableName                   );
  m_pCheckKenBurns->setChecked  ( pSlideshow->bKenBurns         );
  m_pButtonKBParams->setEnabled ( pSlideshow->bKenBurns         );
 
  int iFormat = 0;
  if ( theUtils.getFormat ( pSlideshow->xres, pSlideshow->yres ) > FORMAT_NTSC4 )  // set to PAL
    iFormat = ( pSlideshow->aspect == 0 ) ? 1 : 3;
  else
    iFormat = ( pSlideshow->aspect == 0 ) ? 0 : 2;

  //m_pComboFormat->setCurrentItem ( iFormat ); //ooo
  m_pComboFormat->setCurrentIndex ( iFormat );  //xxx

  bool      bBackgroundImage = false;
  QString   qsAudio;
  QFileInfo fileInfo;
  QStringList::iterator it = pSlideshow->audio_list.begin ( );
  while ( it  !=  pSlideshow->audio_list.end ( ) )  {
    qsAudio = *it++;
    fileInfo.setFile ( qsAudio );
    qsAudio = fileInfo.fileName ( );
    //m_pListAudio->insertItem    ( qsAudio );  //ooo
    m_pListAudio->insertItem    ( 0, qsAudio );  //xxx
    //m_pListAudio->addTopLevelItem ( new QTreeWidgetItem ( qsAudio ) );    //xxx
  }
  m_listAudio = pSlideshow->audio_list;

  if ( pSlideshow->background[0] == '#' )  {
//    QColor theColor ( pSlideshow->background );
//    m_pLabelBckground->setPaletteBackgroundColor ( theColor );
  }
  else if ( pSlideshow->background.isEmpty ( ) )
    pSlideshow->background = "#000000";
  else  {
    QFileInfo fileInfo   ( pSlideshow->background );
    if ( fileInfo.exists ( ) )  {
      // in order to get the correct size I need to show the dialog now.
      show ( );
      QPixmap thePixmap;
      QImage  theImage ( pSlideshow->background );
      //thePixmap.convertFromImage    ( theImage.smoothScale ( m_pLabelBackground->size ( ), Qt::IgnoreAspectRatio ) );				//ooo
      thePixmap.convertFromImage    ( theImage.scaled ( m_pLabelBackground->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );	//xxx
      m_pLabelBackground->setPixmap ( thePixmap );
      bBackgroundImage = true;
    }
    else
      pSlideshow->background = "#000000";
  }

  m_qsBackground  = pSlideshow->background;
  if ( ! bBackgroundImage )  {
    QColor theColor ( pSlideshow->background );
    //m_pLabelBackground->setPaletteBackgroundColor ( theColor );   //ooo
    QPalette palette;		//xxx
    palette.setBrush(QPalette::Base, theColor);	//xxx
    m_pLabelBackground->setPalette(palette);						//xxx
  }

  QString qsTitle = "Slideshow [";
  qsTitle += m_pSlideshow->slideshow_name + "] <";
  qsTitle += QString ( "%1 Imgs+ %2 Vids>" ).arg ( m_pSlideshow->countImg ( ) ).arg ( m_pSlideshow->countVid ( ) );

  //setCaption ( qsTitle );     //ooo
  setWindowTitle ( qsTitle );   //xxx

  // init Transitions will also call createVidsLength, and createAudioLength
  initTransitions ( );
}

void DialogSlideshow::initTransitions ( )
{
  // Here we add the system transitions plus 
  // load the available transitions from the system folder

  // Check if we need to create a new object ...
  if ( Render::Manager::m_pSelf == NULL )
       Render::Manager::m_pSelf = new Render::Manager;

  // Disable sorting.
  //m_pListViewTransitions->setSorting ( -1 );  //oooo

  // We need to also init the selected transitions for a Slideshow
  QString qsFilter;
  QStringList::iterator itFilter;
  //Q3CheckListItem    *pItem = NULL;   //ooo
  QTreeWidgetItem    *pItem = NULL;     //xxx
  QStringList         list = Render::Manager::m_pSelf->getFilterNames ( );
  QStringList::iterator it = list.end ( );
  bool bChecked;

  if ( list.count ( ) < 5 )  {
    Utils theUtils;
    QString qsQRender = theUtils.getToolPath ( "qrender");
    if ( qsQRender == "qrender" )  // qrender path not found
      qsQRender = tr ( "Could not find 'qrender'. Please make sure that 'qrender is in $PATH and executable.\n\n" );
    else
      qsQRender = "";
    QString qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
    QString qsText = qsQRender;
    qsText += tr ( "Please check if you have all transitions installed in " );
    qsText += tr ( "\n%1\n\nYou can donload them from:\nhttp://qdvdauthor.sourceforge.net/data/alpha_trans.tar.bz2\n\n" ).arg ( qsTransitionPath );
    qsText += tr ( "To install do the following:\n" );

    qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/" );
    qsText += tr ( "cd /tmp; wget http://qdvdauthor.sourceforge.net/data/alpha_trans.tar.bz2\n" );
    qsText += tr ( "cd %1; sudo tar -xjf /tmp/alpha_trans.tar.bz2\n\n" ).arg ( qsTransitionPath );
    qsText += tr ( "The filter will be available after restarting QDVDAuthor.\n" );

    QSize theSize    ( 600, 300 );
    MessageBox::html ( this, tr ( "Warning" ), qsText, theSize );
  }

  QPixmap pix;
  //QImage  img ( QImage::fromMimeSource ( "please_wait.jpg" ) );			//ooo
  QImage  img ( QImage ( ":/images/please_wait.jpg" ) );				//xxx
  //img  =  img.smoothScale ( 24, 24 );							//ooo
  img  =  img.scaled ( 24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  pix.convertFromImage  ( img );
  do  {
    qsFilter = *(--it);
    //pItem = new Q3CheckListItem ( m_pListViewTransitions, qsFilter, Q3CheckListItem::CheckBoxController );    //ooo
    pItem = new QTreeWidgetItem ( m_pListViewTransitions );   //xxx
    //pItem->setPixmap ( 0, pix );  //ooo
    pItem->setIcon ( 0, pix );      //xxx
    bChecked = false;
    if ( m_pSlideshow->validFilterNames.count ( ) > 0 )  {
      itFilter = m_pSlideshow->validFilterNames.begin ( );
      while (  itFilter   != m_pSlideshow->validFilterNames.end ( ) )  {
        if  ( *itFilter++ == *it )  {
          bChecked = true;
          break;
        }
      }
    }
    else
      bChecked  =   true;
    //pItem->setOn ( bChecked );    //oooo
  } while ( it != list.begin ( ) );

  bChecked = m_pSlideshow->imgBkgImg ( );
  m_pRadioImgImg   ->setChecked ( !bChecked );
  m_pRadioImgBkgImg->setChecked (  bChecked );

//  bChecked = m_pSlideshow->kenBurns ( );
//  m_pCheckKenBurns->setChecked  (  bChecked );

  QTimer::singleShot ( 1000, this, SLOT ( slotCreateListViewPix ( ) ) );
}

void DialogSlideshow::slotCreateListViewPix ( )
{
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );   //xxx

  while ( pItem )  {
    //pItem->setPixmap    ( 0, getListViewPix ( pItem->text ( ) ) );    //ooo
    pItem->setIcon    ( 0, getListViewPix ( pItem->text ( 0 ) ) );        //xxx
    //qApp->processEvents ( 500 );				//ooo
    qApp->processEvents ( QEventLoop::AllEvents, 500 );		//xxx
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);  //xxx
  }
  // After we initialized the pixies, we need to ge vids, and audio lengths
  QTimer::singleShot ( 10,   this, SLOT ( createVidsLength  ( ) ) );
}

QPixmap DialogSlideshow::getListViewPix ( QString qsFilterName )
{
  QPixmap pix;
  QString qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  QString qsFileName       = qsTransitionPath + qsFilterName + ".jpg";

  QFileInfo fileInfo ( qsFileName );
  for ( int t=0; t<2;  t++ ) {
    if ( fileInfo.exists ( ) )
      break;

    qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    qsFileName       = qsTransitionPath + qsFilterName + ".png";
    fileInfo.setFile ( qsFileName );
  }
  if ( ! fileInfo.exists ( ) )
    return pix; // E.g. "Cross Fade"

  QImage img  (  qsFileName );
  //img =  img.scale ( 24, 24 );							//ooo
  img  =  img.scaled ( 24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  pix.convertFromImage( img );

  return pix;
}

void DialogSlideshow::createTransitions ( )
{
  // Here we compare the tranition selection from the slideshow to the current selection
  // If they have changed we'll re-create the transitions for this slideshow randomly.
  // Note: We need to preserve transitions which the user manually changed.
  QStringList listOfCurrentSelections;

  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );  //xxx
  while ( pItem )  {
    /*if  ( pItem->isOn ( ) )
      listOfCurrentSelections.append ( pItem->text ( ) );*/     //ooo
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);  //xxx
  }
}

void DialogSlideshow::createVidsLength ( )
{
  if ( ! m_pSlideshow )
    return;
  MediaInfo *pMediaInfo = Global::pApp->getMediaInfo ( );
  if ( ! pMediaInfo )
    return;

  uint t;
  CXmlSlideshow::vid_struct *pVid = NULL;
  for ( t=0; t<m_pSlideshow->countVid ( ); t++ )  {
    pVid = m_pSlideshow->getVid ( t );
    if ( pVid->length == 0L )   {
      pMediaInfo->setFileName   ( pVid->src, true );
      pVid->length = pMediaInfo->getLength ( );
    }
  }
  // Lastly we create the audio length
  QTimer::singleShot ( 10, this, SLOT ( createAudioLength ( ) ) );
}

void DialogSlideshow::createAudioLength ( )
{
  static bool bBusy = false;
  if ( bBusy )  {
    m_pSlideshow->total_audio_length = 0;
    displayLength ( );
    return;
  }
  bBusy = true;
  if ( m_pSlideshow )  {
    MediaInfo *pMediaInfo = Global::pApp->getMediaInfo ( );
    if ( pMediaInfo )  {
      QString qsFile;
      m_pSlideshow->total_audio_length = 0;
      QStringList tempList     = m_listAudio;
      QStringList::iterator it = tempList.begin ( );
      while ( it != tempList.end ( ) )  {
        qsFile = *it++;
        pMediaInfo->setFileName ( qsFile, true );
        if ( ( ! m_pSlideshow ) || ( m_bExitNow ) ) {
          displayLength ( );
          bBusy = false;
          m_bExitNow = false;
          return;
        }
        m_pSlideshow->total_audio_length += (uint)( pMediaInfo->getLength ( ) / 1000.0 );
      }
    }
    m_pButtonKBParams->setEnabled ( m_pSlideshow->bKenBurns );
  }
  displayLength ( );
  bBusy = false;
}

void DialogSlideshow::displayLength ( )
{
  if ( ! m_pSlideshow )
    return;
  Utils theUtils;
  QString qsLength = theUtils.getStringFromMs ( (long)( m_pSlideshow->slideLength ( ) * 1000 ) );
  m_pGroupGeneral->setTitle ( QString ( "General ( " )     + qsLength + QString ( " )" )  );
  qsLength = theUtils.getStringFromMs ( (long)( m_pSlideshow->audioLength ( ) * 1000 ) );
  m_pGroupAudio->setTitle   ( QString ( "Audio files ( " ) + qsLength + QString ( " )" )  );
}

void DialogSlideshow::slotKBParams ( )
{
  DialogKenBurns dialog;
  dialog.setSlideshow ( m_pSlideshow );
  if ( dialog.exec ( ) == QDialog::Accepted )   {
    if ( m_pSlideshow  && dialog.getSlideshow ( ) )
        *m_pSlideshow  = *dialog.getSlideshow ( );
         QString qsTitle = "Slideshow [";
         qsTitle += m_pSlideshow->slideshow_name + "] <";
         qsTitle += QString ( "%1 Imgs+ %2 Vids>" ).arg ( m_pSlideshow->countImg ( ) ).arg ( m_pSlideshow->countVid ( ) );
         //setCaption ( qsTitle );  //ooo
         setWindowTitle( qsTitle );	//xxx
  }   //oooo
}

void DialogSlideshow::slotKenBurns ( bool bChecked )
{
  if ( m_pSlideshow ) {
    // At this point we need the width / height information of the images,
    // which has not yet been generated.
    if ( bChecked && ! m_pSlideshow->bKenBurns )  {
      uint iImgCount = m_pSlideshow->countImg( );
      uint t;
      Utils theUtils;
      QCursor myCursor ( Qt::WaitCursor );
      QApplication::setOverrideCursor ( myCursor );
      Cache::Thumbs::ImageEntry *pCache = NULL;
      for ( t=0; t<iImgCount; t++ )  {
        CXmlSlideshow::img_struct *pImg = m_pSlideshow->getImg ( t );
        if ( ! pImg )
          continue;
        if ( ( pImg->width == 0 ) || ( pImg->height == 0 ) ) {
          pCache = (Cache::Thumbs::ImageEntry *)Global::pThumbsCache->find ( pImg->src, true );
          if ( pCache )  {
            if ( theUtils.getWidthHeight ( pCache->qsDimension, pImg->width, pImg->height ) )  {
              // Trying to figure out when we rotated the image by 90deg.
              if ( ( ( pCache->iOrientation != 6 && pCache->iOrientation != 8 ) && ( pCache->fRotate == 90.0 || pCache->fRotate == 270.0 ) ) || 
                   ( ( pCache->iOrientation == 6 || pCache->iOrientation == 8 ) && ( pCache->fRotate ==  0.0 || pCache->fRotate == 360.0 ) ) ) {
                int w = pImg->width;
                pImg->width  = pImg->height;
                pImg->height = w;
              }
            }
          }
          else  {
            QImage img ( pImg->src );
            pImg->width  = img.width  ( );
            pImg->height = img.height ( );
            if ( pImg->pMatrix )  {
              //img = img.xForm ( *pImg->pMatrix ); //ooo
              img = img.transformed ( *pImg->pMatrix ); //xxx
              pImg->width  = img.width  ( );
              pImg->height = img.height ( );
            }
          }
        }
        if ( t%10 == 9 )
          qApp->processEvents ( );
      }
    }
    m_pSlideshow->setKenBurns     ( bChecked );
    m_pButtonKBParams->setEnabled ( bChecked );
    QApplication::restoreOverrideCursor ();
  }
}

void DialogSlideshow::slotIntroPage ( bool )
{
  slotValueChanged ( 0 );
}

void DialogSlideshow::slotValueChanged ( int )
{
  Utils theUtils;
  int iFilterDelay     = m_pSpinFilterDelay->value ( );
  int iImageDelay      = m_pSpinImageDelay->value  ( );
  unsigned int iLength = ( iFilterDelay   + iImageDelay ) * m_pSlideshow->countImg ( );
  iLength += m_pSlideshow->vidsLength ( ) + iFilterDelay  * m_pSlideshow->countVid ( );
  if ( m_pCheckIntroPage->isChecked ( ) )
    iLength += iFilterDelay + iImageDelay;

  QString qsLength     = theUtils.getStringFromMs ( (long)( iLength * 1000 ) );
  m_pGroupGeneral->setTitle ( QString ( "General ( " )     + qsLength + QString ( " )" )  );
}

void DialogSlideshow::slotAnim ( )
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
      connect ( m_pTimer, SIGNAL ( timeout ( ) ), this, SLOT ( slotAnimatePix ( ) ) );
    }
    // start the timer
    m_pTimer->start ( 100 );
    m_bAnimated = true;
  }
  // and lastly, we recreate the pixmaps / preview ...
  recreatePix ( );
}

//void DialogSlideshow::slotSelectionChanged ( Q3ListViewItem * )   //ooo
void DialogSlideshow::slotSelectionChanged ( QTreeWidgetItem * )    //xxx
{
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->selectedItem ( );   //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->selectedItems ( )[0];     //xxx
  if ( ! pItem )
    return;

  QString qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  //QString qsFilterName     = pItem->text ( ); //ooo
  QString qsFilterName     = pItem->text ( 0 ); //xxx
  QString qsFileName       = qsTransitionPath + qsFilterName + ".jpg";

  QFileInfo fileInfo ( qsFileName );
  for ( int t=0; t<2; t++ )  {
    if ( fileInfo.exists ( ) )
      break;

    qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    qsFileName       = qsTransitionPath + qsFilterName + ".png";
    fileInfo.setFile ( qsFileName );
  }
  if ( ! fileInfo.exists ( ) )
    return; // E.g. "Cross Fade"

  QImage theImage ( qsFileName );
  //theImage = theImage.smoothScale ( m_pLabelPreview->size ( ) );						//ooo
  theImage = theImage.scaled ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
  QPixmap thePix;
  thePix.convertFromImage ( theImage );
  //m_pLabelPreview->setPaletteBackgroundPixmap ( thePix ); //ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(thePix));		//xxx
  m_pLabelPreview->setPalette(palette);					//xxx

  recreatePix ( );
}

void DialogSlideshow::recreatePix ( )
{
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->selectedItem ( );   //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->selectedItems ( )[0]; //xxx
  if ( ( ! pItem ) || ( ! m_pSlideshow ) )
    return;

  Utils theUtils;
  QCursor myCursor     ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  QString qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/alpha/" );
  //QString qsFilterName     = qsTransitionPath + pItem->text (   ) + ".jpg";   //ooo
  QString qsFilterName     = qsTransitionPath + pItem->text ( 0 ) + ".jpg";     //xxx

  QFileInfo fileInfo ( qsFilterName );
  if ( ! fileInfo.exists ( ) )  {
    qsTransitionPath = Global::qsSystemPath + QString ( "/share/qdvdauthor/slideshow/transitions/luma/" );
    //qsFilterName     = qsTransitionPath + pItem->text (   ) + ".png"; //ooo
    qsFilterName     = qsTransitionPath + pItem->text ( 0 ) + ".png";   //xxx
  }

  if ( m_bAnimated )  {
    int      t, iDiff = 0;
    float    fDelta;
    bool     bImgBkgImg = m_pRadioImgBkgImg->isChecked ( );
    QPixmap *array[THUMBNAIL_COUNT];

    for ( t=0; t<THUMBNAIL_COUNT; t++ )
      array[t] = new QPixmap ( m_pLabelPreview->size ( ) );

    QImage filter ( qsFilterName );
    //QImage start  = QImage::fromMimeSource ( "default.jpg" );		//ooo
    QImage start  = QImage ( ":/images/default.jpg" );			//xxx
    //QImage stop   = QImage::fromMimeSource (  "splash.jpg" );		//ooo
    QImage stop   = QImage ( ":/images/splash.jpg" );			//xxx
    if ( m_pSlideshow->countImg ( ) >= 2 )  {
      CXmlSlideshow::img_struct *pStart = m_pSlideshow->getImg  ( 0 );
      CXmlSlideshow::img_struct *pStop  = m_pSlideshow->getImg  ( 1 );
      start = QImage ( pStart->src  );
      stop  = QImage ( pStop->src   );
    }

    QImage result     ( start ); // init the size
    QImage background ( start.copy ( ) ); // init the size
    if ( bImgBkgImg ) {
      QFileInfo fileInfo    ( m_qsBackground );
      if ( fileInfo.exists  ( ) )
        background = QImage ( m_qsBackground );
      else  {
        QColor theColor;
        theColor.setNamedColor ( m_qsBackground );
        background.fill ( theColor.rgb ( ) );
      }
      iDiff = 15;
    }
    //background = background.smoothScale ( m_pLabelPreview->size ( ) );    //ooo
    background = background.scaled ( m_pLabelPreview->size ( ) );           //xxx

    //filter = filter.smoothScale ( m_pLabelPreview->size ( ) );						//ooo
    filter = filter.scaled ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    //start  = start.smoothScale  ( m_pLabelPreview->size ( ) );						//ooo
    start  = start.scaled  ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    //stop   = stop.smoothScale   ( m_pLabelPreview->size ( ) );						//ooo
    stop   = stop.scaled   ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    //result = result.smoothScale ( m_pLabelPreview->size ( ) );						//ooo
    result   = result.scaled   ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    fDelta = 512.0f / ( THUMBNAIL_COUNT - iDiff - 1 );

    for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
      if ( bImgBkgImg )
        theUtils.augmentAlpha ( (int)( fDelta * t ), (int)(fDelta * (t-iDiff)), filter, start, stop, result, &background );
      else
        theUtils.augmentAlpha ( (int)( fDelta * t ), filter, start, stop, result );

      array[t]->convertFromImage ( result );
    }
    for ( t=0; t<THUMBNAIL_COUNT; t++ )  {
      if ( m_arrayPix[t] )
        delete m_arrayPix[t];
      m_arrayPix[t] = array[t];
    }
  }
  else  {
    // Simply display the filter and do not use animation.
    QPixmap pix;
    QImage  filter ( qsFilterName );
    //pix.convertFromImage ( filter.smoothScale   ( m_pLabelPreview->size ( ) ) );						//ooo
    pix.convertFromImage ( filter.scaled ( m_pLabelPreview->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );	//xxx
    //m_pLabelPreview->setPaletteBackgroundPixmap ( pix );  //ooo
    QPalette palette;							//xxx
    palette.setBrush(QPalette::Window, QBrush(pix));		//xxx
    m_pLabelPreview->setPalette(palette);					//xxx
  }
  m_iCurrentPix = 0;

  QApplication::restoreOverrideCursor ( );
}

void DialogSlideshow::slotAnimatePix ( )
{
  m_iCurrentPix ++;
  if ( m_iCurrentPix < 0 )
    return;

  if ( m_iCurrentPix >= THUMBNAIL_COUNT )  {
    m_iCurrentPix = -10; // 1 sec pause between each run ...
    return;
  }
  //m_pLabelPreview->setPaletteBackgroundPixmap ( *m_arrayPix[m_iCurrentPix] ); //ooo
  QPalette palette;							//xxx
  palette.setBrush(QPalette::Window, QBrush(*m_arrayPix[m_iCurrentPix]));		//xxx
  m_pLabelPreview->setPalette(palette);					//xxx
}

void DialogSlideshow::slotSelect ( )
{
  QStringList listEnabled;
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );  //xxx
  while ( pItem )  {
    /*if  ( pItem->isOn ( ) )
      //listEnabled.append ( pItem->text ( ) ); //ooo
      listEnabled.append ( pItem->text ( 0 ) ); //xxx*/ //oooo
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }

  /*DialogAlphaTrans filterDialog ( listEnabled, this );
  if ( filterDialog.exec ( )  ==  QDialog::Rejected )
    return;*/   //oooo

  QString qsName;
  bool    bChecked;
  //pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( );  //ooo
  pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );  //xxx
  while ( pItem )  {
    //qsName = pItem->text ( ); //ooo
    qsName = pItem->text ( 0 ); //xxx
    bChecked = false;
    /*if  ( filterDialog.isChecked ( qsName ) )
      bChecked = true;*/    //oooo
    //pItem->setOn ( bChecked );    //oooo
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }
}

void DialogSlideshow::slotAll ( )
{
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );  //xxx
  while ( pItem )  {
    //pItem->setOn ( true );    //oooo
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }
}

void DialogSlideshow::slotClear ( )
{
  //Q3CheckListItem *pItem = (Q3CheckListItem *)m_pListViewTransitions->firstChild ( ); //ooo
  QTreeWidgetItem *pItem = (QTreeWidgetItem *)m_pListViewTransitions->topLevelItem ( 0 )->child ( 0 );  //xxx  
  while ( pItem )  {
    //pItem->setOn ( false );   //oooo
    //pItem = (Q3CheckListItem *)pItem->nextSibling ( );    //ooo
    pItem = (QTreeWidgetItem *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }
}

void DialogSlideshow::slotImage ( )
{
  QString qsFilter   = Global::pApp->getImageFilter ( );
  QString qsFileName = FilePreviewDialog::getOpenFileName ( NULL, Global::qsCurrentPath, qsFilter, this );

  if ( qsFileName.isEmpty ( ) )
    return;

  QFileInfo fileInfo ( qsFileName );
  //Global::qsCurrentPath = fileInfo.dirPath ( TRUE );  //ooo
  Global::qsCurrentPath = fileInfo.path ( );            //xxx

  QImage  theImg ( qsFileName );
  QPixmap thePix;
  if ( theImg.isNull ( ) )
    return;

  //thePix.convertFromImage ( theImg.smoothScale ( m_pLabelBackground->size ( ), Qt::IgnoreAspectRatio ) );				//ooo
  thePix.convertFromImage ( theImg.scaled ( m_pLabelBackground->size ( ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );		//xxx
  m_pLabelBackground->setPixmap ( thePix );
  m_qsBackground = qsFileName;
}

void DialogSlideshow::slotColor ( )
{
  QColor theColor;
  if ( m_pSlideshow->background[0] != '/' )  {
    theColor.setNamedColor ( m_pSlideshow->background );

    if ( ! theColor.isValid ( ) )
      theColor.setNamedColor ( "#000000" );
  }
  else
    theColor.setNamedColor ( "#000000" );

  theColor = QColorDialog::getColor ( theColor, this );
  if ( ! theColor.isValid ( ) )
      return;

  QPixmap emptyPix;
  m_pLabelBackground->setPixmap  ( emptyPix );
  //m_pLabelBackground->setBackgroundColor ( theColor );    //ooo
  QPalette palette;										//xxx
  palette.setBrush(m_pLabelBackground->backgroundRole(), theColor);		//xxx
  m_pLabelBackground->setPalette(palette);								//xxx
  
  m_qsBackground = theColor.name ( );
}

void DialogSlideshow::slotAddAudio ( )
{
  QString qsFilter = "Audio ( " + Global::pApp->getSoundFilter  ( ) + ")";
  QStringList list = FilePreviewDialog::getOpenFileNames ( Global::qsCurrentPath, qsFilter, this );

  if ( list.count ( ) < 1 )
    return;

  m_listAudio += list;
  QFileInfo fileInfo ( list[0] );
  //Global::qsCurrentPath = fileInfo.dirPath ( TRUE );  //ooo
  Global::qsCurrentPath = fileInfo.path ( );            //xxx

  QString   qsAudio;
  QStringList::iterator it = list.begin ( );
  while ( it  !=  list.end ( ) )  {
    qsAudio = *it++;
    fileInfo.setFile ( qsAudio );
    qsAudio = fileInfo.fileName ( );
    //m_pListAudio->insertItem    ( qsAudio );  //ooo
    m_pListAudio->insertItem    ( 0, qsAudio ); //xxx
  }

  createAudioLength ( );
}

void DialogSlideshow::slotDeleteAudio ( )
{
  //QString qsAudio = m_pListAudio->currentText ( );    //ooo
    QString qsAudio = m_pListAudio->currentItem ( )->text ( );  //xxx
  if ( qsAudio.isEmpty ( ) )
    return;

  //m_pListAudio->removeItem ( m_pListAudio->currentItem ( ) ); //ooo
  m_pListAudio->removeItemWidget ( m_pListAudio->currentItem ( ) ); //xxx

  // The listBox only holds the file name, and not the whole path
  // thus we need to search for the right entry.
  QString qsInList;
  QStringList::iterator it = m_listAudio.begin ( );
  while ( it != m_listAudio.end ( ) )  {
    qsInList = *it++;
    //if ( qsInList.find   ( qsAudio  ) > -1 ) {    //ooo
    if ( qsInList.indexOf   ( qsAudio  ) > -1 ) {   //xxx
      //m_listAudio.remove ( qsInList );    //ooo
      m_listAudio.removeOne ( qsInList );   //xxx
      createAudioLength  ( );
      return;
    }
  }
}

}; // End of namespace Input

