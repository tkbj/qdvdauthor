/****************************************************************************
** dialogkenburns.cpp
**
**   Created : Tue Jul 13 2010
**        by : Varol Okan using vi
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** Dialog to manually adjust the KenBurns effects of each image in the slideshow.
** Note: The KenBurns effect is randomly auto-generated, so this dialog
**       will allow to adjust the KenBurns of the random generated effect.
**
*****************************************************************************/

#include <qinputdialog.h>
#include <qapplication.h>
#include <qpushbutton.h>
//#include <q3popupmenu.h>  //ooo
#include <QMenu>            //xxx
#include <qcheckbox.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qslider.h>
#include <qcursor.h>
#include <qtimer.h>
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>


#include "global.h"
#include "messagebox.h"
#include "xml_slideshow.h"
#include "cachethumbs.h"

#include "dialogkenburns.h"


PreviewMouseHandler::PreviewMouseHandler ( DialogKenBurns *pDialog )
{
  m_pDialog = pDialog;
}

PreviewMouseHandler::~PreviewMouseHandler ( )
{
}

bool PreviewMouseHandler::eventFilter ( QObject *, QEvent *pEvent )
{
  if ( ! m_pDialog )
    //return FALSE; //ooo
    return false;   //xxx

  switch ( pEvent->type ( ) )  {
  case QEvent::MouseButtonPress:
    m_pDialog->previewMousePressed ( (QMouseEvent *)pEvent );
  break;
  case QEvent::MouseButtonRelease:
    m_pDialog->previewMouseReleased ( (QMouseEvent *)pEvent );
  break;
  case QEvent::MouseMove:
    m_pDialog->previewMouseMove ( (QMouseEvent *)pEvent );
  break;
  default:
  break;
  }
  //return FALSE; // Always handle events, and never eat them !!!   //ooo
  return false; // Always handle events, and never eat them !!!     //xxx
}

//DialogKenBurns::Item::Item ( Cache::Thumbs::Entry *p, CXmlSlideshow::img_struct *pXmlImg, float fDelay, int iCounter, Q3ListView *pListView, DialogKenBurns::Item *pAfter, QString qsLabel )  //ooo
DialogKenBurns::Item::Item ( Cache::Thumbs::Entry *p, CXmlSlideshow::img_struct *pXmlImg, float fDelay, int iCounter, QTreeWidget *pListView, DialogKenBurns::Item *pAfter, QString qsLabel )
  //: Q3ListViewItem ( pListView, pAfter, qsLabel ) //ooo
  : QTreeWidgetItem ( pListView )   //xxx
{
  bAlternateColor = true;
  pCache          = p;
  pXmlImage       = NULL;
  pXmlVideo       = NULL;
  fDefaultDelay   = fDelay;
  bRecreateImage  = true;

  setText ( 1, QString ( "%1" ).arg ( iCounter ) );
  if ( p && pXmlImg->node_name == VID_NODE )  {
    setText ( 2, p->qsLength );
    pXmlVideo = (CXmlSlideshow::vid_struct *)pXmlImg;
    return;
  }
  if ( pXmlImg->node_name != IMG_NODE )  { 
    delete pXmlImg;
    return;
  }

  pXmlImage = pXmlImg;
  if ( ( pXmlImage->width == 0 ) || ( pXmlImage->height == 0 ) )  {
    QImage img ( pXmlImage->src );
    pXmlImage->width  = img.width  ( );
    pXmlImage->height = img.height ( );
  }

  setText ( 0, QString ( "%1\n(%2x%3)" ).arg ( qsLabel ).arg ( pXmlImage->width ).arg ( pXmlImage->height ) );
  update  ( );
}

DialogKenBurns::Item::~Item ( )
{
  if ( pXmlImage )
    delete pXmlImage;
  pXmlImage = NULL;
  if ( pXmlVideo )
    delete pXmlVideo;
  pXmlVideo = NULL;
}

void DialogKenBurns::Item::update ( )
{
  unsigned long iLength = 0UL;

  if ( ! pXmlImage )
    return;

  if ( pXmlImage->fDuration > 0.0f )
    iLength  = (unsigned long)( pXmlImage->fDuration * 1000.0 ); // in MSec
  else
    iLength  = (unsigned long)( fDefaultDelay * 1000.0 ); // in MSec

//  setText ( 0, QString ( "%1\n(%2x%3)" ).arg ( qsLabel ).arg ( pXmlImage->width ).arg ( pXmlImage->height ) );
//  setText ( 1, QString ( "%1" ).arg ( pXmlImage->fStartTime * 1000.0 ) );
  setText ( 2, QString ( "%1" ).arg ( iLength ) );
  bool bKenBurns = pXmlImage->effect[0].name == EFFECT_TYPE_KENBURNS;

  if ( bKenBurns )  {
    setText ( 3, QString ( "( %1, %2, %3, %4 )" ).arg ( pXmlImage->effect[0].x0  ).arg ( pXmlImage->effect[0].y0  ).arg ( pXmlImage->effect[0].x1  ).arg ( pXmlImage->effect[0].y1  ) );
    setText ( 4, QString ( "( %1, %2, %3, %4 )" ).arg ( pXmlImage->effect[0].xe0 ).arg ( pXmlImage->effect[0].ye0 ).arg ( pXmlImage->effect[0].xe1 ).arg ( pXmlImage->effect[0].ye1 ) );
  }
  else  {
    setText ( 3, "( ----, ----, ----, ---- )" );
    setText ( 4, "( ----, ----, ----, ---- )" );
  }
}

/*void DialogKenBurns::Item::paintCell ( QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
  QColorGroup theColors = cg;
  if ( isSelected ( ) )   {
    theColors.setColor ( QColorGroup::Base, theColors.highlight       ( ) );
    theColors.setColor ( QColorGroup::Text, theColors.highlightedText ( ) );
  }
  else {
    QColor colorAlternate ( 250, 250, 250 );
    if ( bAlternateColor )   // every second file we change the color slightly
         colorAlternate  = QColor ( 235, 235, 235 );
    theColors.setColor ( QColorGroup::Base, colorAlternate );
  }
  Q3ListViewItem::paintCell ( p, theColors, column, width, align );
}*/ //oooo

QRect DialogKenBurns::Item::getCurrentRect ( double fPosition )
{
  // Returns the rect of the visible region based on the percentage progress passed into this function
  // fPosition [ 0.0 .. 1.0 ]
  QRect rect ( 10, 10, 10, 10 );
  CXmlSlideshow::effect_struct *pEffect = &pXmlImage->effect[0];
  int iWidth  = pXmlImage->width;
  int iHeight = pXmlImage->height;

  double fKenBurns[2][6]; // in the order [start,stop][x1,y1,x2,y2,w,h]
  // populate the array
  fKenBurns[0][0] =   (double)pEffect->x0  / iWidth;  // x1
  fKenBurns[0][1] =   (double)pEffect->y0  / iHeight; // y1
  fKenBurns[0][2] =   (double)pEffect->x1  / iWidth;  // x2
  fKenBurns[0][3] =   (double)pEffect->y1  / iHeight; // y2
  fKenBurns[0][4] = ( (double)pEffect->x1  / iWidth  ) - fKenBurns[0][0]; // w
  fKenBurns[0][5] = ( (double)pEffect->y1  / iHeight ) - fKenBurns[0][1]; // h
  fKenBurns[1][0] =   (double)pEffect->xe0 / iWidth;  // xe1
  fKenBurns[1][1] =   (double)pEffect->ye0 / iHeight; // ye1
  fKenBurns[1][2] =   (double)pEffect->xe1 / iWidth;  // xe2
  fKenBurns[1][3] =   (double)pEffect->ye1 / iHeight; // ye2
  fKenBurns[1][4] = ( (double)pEffect->xe1 / iWidth  ) - fKenBurns[1][0]; // w
  fKenBurns[1][5] = ( (double)pEffect->ye1 / iHeight ) - fKenBurns[1][1]; // h

  double x = ( fKenBurns[1][0] - fKenBurns[0][0] ) * fPosition + fKenBurns[0][0];
  double y = ( fKenBurns[1][1] - fKenBurns[0][1] ) * fPosition + fKenBurns[0][1];
  double w = ( fKenBurns[1][4] - fKenBurns[0][4] ) * fPosition + fKenBurns[0][4];
  double h = ( fKenBurns[1][5] - fKenBurns[0][5] ) * fPosition + fKenBurns[0][5];

  rect.setX      ( (int)( x * pix.width  ( ) ) );
  rect.setY      ( (int)( y * pix.height ( ) ) );
  rect.setWidth  ( (int)( w * pix.width  ( ) ) );
  rect.setHeight ( (int)( h * pix.height ( ) ) );
  return rect;
}

DialogKenBurns::Undelete::Undelete ( uint iPos, Cache::Thumbs::Entry *p1, CXmlSlideshow::img_struct *p2 )
{
  iPosition = iPos;
  pCache    = p1;
  pXmlImage = p2;
}

DialogKenBurns::Undelete::~Undelete ( )
{
  if ( pXmlImage )
    delete pXmlImage;
  pCache = NULL;
}


//DialogKenBurns::DialogKenBurns ( QWidget *parent, const char *name, bool modal, Qt::WFlags fl )	//ooo
DialogKenBurns::DialogKenBurns ( QWidget *pParent )							//xxx
  //: uiDialogKenBurns ( parent, name, modal, fl )	//ooo
  : QDialog ( pParent )					//xxx
{
  setupUi(this);	//xxx
  initMe ( );
}

DialogKenBurns::~DialogKenBurns ( )
{
  m_pLabelPreview->removeEventFilter ( m_pEventFilter );
  delete m_pEventFilter;
  m_pEventFilter = NULL;
  QList<Undelete *>::iterator it = m_listUndelete.begin ( );	//oxx
  while ( it != m_listUndelete.end ( ) )  {
    delete *it++;
  }
  m_listUndelete.clear ( );
}

bool DialogKenBurns::initMe ( )
{
  m_pSlideshow     = NULL;
  m_pOrigSlideshow = NULL;
  m_pNextItem      = NULL;
  m_pActiveItem    = NULL;
  m_pChangedItem   = NULL;
  m_bMousePressed  = false;
  m_bFirstQuestion = true;
  m_iAnimation     = -1;
  m_iThumbSize     = 65;
  m_iCorner        = 0;

  // disable sorting.
  m_pSlider->setRange( 0, 1000 );
  //m_pListView->setSorting ( -1 ); //oooo
  connect ( m_pButtonAnimate,  SIGNAL ( clicked ( ) ), this, SLOT ( slotAnimate  ( ) ) );
  connect ( m_pButtonDuration, SIGNAL ( clicked ( ) ), this, SLOT ( slotDuration ( ) ) );
  connect ( m_pButtonRandom,   SIGNAL ( clicked ( ) ), this, SLOT ( slotRandom   ( ) ) );
  connect ( m_pButtonHide,     SIGNAL ( clicked ( ) ), this, SLOT ( slotHide     ( ) ) );
  connect ( m_pListView,       SIGNAL ( selectionChanged ( Q3ListViewItem * ) ), this,  SLOT ( slotCurrentChanged ( Q3ListViewItem * ) ) );
  connect ( m_pSlider,         SIGNAL ( valueChanged (  int ) ), this, SLOT ( slotSlider         (  int ) ) );
  connect ( m_pCheckKenBurns,  SIGNAL ( toggled      ( bool ) ), this, SLOT ( slotToggleKenBurns ( bool ) ) );
  connect ( m_pListView, SIGNAL ( contextMenuRequested ( Q3ListViewItem *, const QPoint &, int ) ), this, SLOT ( slotContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );

  m_pEventFilter = new PreviewMouseHandler ( this );
  m_pLabelPreview->installEventFilter ( m_pEventFilter );

  m_pButtonAnimate ->setEnabled ( false );
  m_pButtonDuration->setEnabled ( false );
  m_pButtonRandom  ->setEnabled ( false );
  m_pCheckKenBurns ->setEnabled ( false );
  m_pSlider        ->setEnabled ( false );

  updateText ( );
  return true;
}

void DialogKenBurns::setSlideshow ( CXmlSlideshow *pSlideshow )
{
  m_pOrigSlideshow = pSlideshow;
  if ( m_pOrigSlideshow )  {
     m_pSlideshow = new CXmlSlideshow;
    *m_pSlideshow = *m_pOrigSlideshow;

    populate ( );
    changeCaption ( );
  }
} 

void DialogKenBurns::changeCaption ( )
{
  if ( ! m_pSlideshow )
    return;

  int iImgCount = 0; // m_pSlideshow->countImg ( )
  int iVidCount = 0; // m_pSlideshow->countVid ( )

  //Item *pItem = (Item *)m_pListView->firstChild ( );  //ooo
  Item *pItem = (Item *)m_pListView->topLevelItem ( 0 )->child ( 0 ); //xxx
  while ( pItem )  {
    if  ( pItem->pXmlImage )
      iImgCount ++;
    else if ( pItem->pXmlVideo )
      iVidCount ++;
    //pItem = (Item *)pItem->nextSibling ( );   //ooo
    pItem = (Item *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }

  QString qsTitle = "KenBurns [";
  qsTitle += m_pSlideshow->slideshow_name + "] <";
  qsTitle += QString ( "%1 Imgs+ %2 Vids>" ).arg ( iImgCount ).arg ( iVidCount );
  //setCaption ( qsTitle ); //ooo
  setWindowTitle ( qsTitle );   //xxx
}

CXmlSlideshow *DialogKenBurns::getSlideshow ( )
{
  return m_pSlideshow;
}

void DialogKenBurns::populate ( )
{
  if ( ! m_pSlideshow )
    return;

  int    t, iCount;
  QFileInfo fileInfo;
  DialogKenBurns::Item  *pItem = NULL;
  Cache::Thumbs::Entry *pEntry = NULL;
  CXmlSlideshow *pSlideshow = m_pSlideshow;
  unsigned long long iFileHashValue = 0ULL;

  CXmlSlideshow::time_object *pXmlObj  = NULL;
  CXmlSlideshow::img_struct  *pXmlImg  = NULL;
  CXmlSlideshow::img_struct  *pXmlOrig = NULL;
  m_pListView->clear ( );

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  iCount = pSlideshow->count ( );
  for ( t=iCount-1; t>=0; t-- )  {
    pXmlObj = pSlideshow->getTimeObject ( t );
    pXmlImg = NULL;
    if ( ! pXmlObj )
      continue;

    pXmlOrig  = (CXmlSlideshow::img_struct *)pXmlObj;
    fileInfo.setFile ( pXmlOrig->src );
    iFileHashValue = Global::pThumbsCache->hashFromFile ( pXmlOrig->src );
    pEntry   = Global::pThumbsCache->find ( iFileHashValue,  true );
    if ( ! pEntry ) // MOst likely a vid
      pEntry = Global::pThumbsCache->find ( iFileHashValue, false );
    if ( ! pEntry )  {
      // Something went bad. Could not get the cache object for this source.
      // Skipping over 
      continue;
    }
    if ( pXmlOrig->node_name == IMG_NODE )  {
       pXmlImg = new CXmlSlideshow::img_struct;
      *pXmlImg = *pXmlOrig;
    }
    else if ( pXmlOrig->node_name == VID_NODE )  {
       CXmlSlideshow::vid_struct *pXmlVid = new CXmlSlideshow::vid_struct;
      *pXmlVid = *((CXmlSlideshow::vid_struct *)pXmlOrig);
       pXmlImg =   (CXmlSlideshow::img_struct *)pXmlVid; // typecast to img_struct
    }
    // pItem takes ownershipt of the pXmlImg object.
    pItem = new Item ( pEntry, pXmlImg, pSlideshow->delay, t+1, m_pListView, NULL, fileInfo.fileName ( ) );
    createPix ( pItem );
  }

  QApplication::restoreOverrideCursor ( ); 
  QTimer::singleShot ( 10, this, SLOT ( slotAlternateColors ( ) ) );
}

void DialogKenBurns::createPix ( DialogKenBurns::Item *pItem )
{
  if ( ! pItem )
    return;

  QImage  theImage;
  QPixmap thePixmap;
  QString qsFileName;
  if ( pItem->pXmlImage )
    qsFileName = pItem->pXmlImage->src;
  else if ( pItem->pXmlVideo )
    qsFileName = pItem->pXmlVideo->src;

  QFileInfo fileInfo ( qsFileName );
  Cache::Thumbs::Entry *pEntry = pItem->pCache; 
  if ( ! pEntry->arrayOfThumbs )
         pEntry->loadImages  ( );
  if (   pEntry->arrayOfThumbs && pEntry->arrayOfThumbs[0] ) {
    if ( pEntry->getMatrix ( ) )  {
      //theImage = pEntry->arrayOfThumbs[0]->xForm ( *pEntry->getMatrix ( ) );  //ooo
      theImage = pEntry->arrayOfThumbs[0]->transformed ( *pEntry->getMatrix ( ) );  //xxx
      adjustWidthHeight ( pItem, theImage );
      pItem->setText ( 0, QString ( "%1\n(%2x%3)" ).arg ( fileInfo.fileName ( ) ).arg ( pItem->pXmlImage->width ).arg ( pItem->pXmlImage->height ) );
    }
    else
      theImage = *pEntry->arrayOfThumbs[0];
    //theImage   = theImage.smoothScale ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );    //ooo
    theImage   = theImage.scaled ( m_iThumbSize, m_iThumbSize, Qt::KeepAspectRatio );           //xxx
    thePixmap.convertFromImage ( theImage );
    //pItem->setPixmap ( 0, thePixmap );    //ooo
    pItem->setIcon ( 0, thePixmap );        //xxx
  }
}

void DialogKenBurns::adjustWidthHeight ( DialogKenBurns::Item *pItem, QImage &img )
{
  if ( ! pItem )
    return;

  if ( ( pItem->pXmlImage->width == 0 ) || ( pItem->pXmlImage->height == 0 ) )  {
    // Should never get here, as the constructor of Item does take care of this ...
    QImage img ( pItem->pXmlImage->src );
    pItem->pXmlImage->width  = img.width  ( );
    pItem->pXmlImage->height = img.height ( );
  }

  if ( ! pItem->pCache->getMatrix ( ) )
    return;
  // If we have a Matrix, which rotates the image 90 or 27 degree, 
  // we will have to swap width and height in the CXmlSlideshow::img_struct
  // 
  // Note: that img is a scaled down and rotated thumbnail version of the original image
  if ( ( pItem->pCache->arrayOfThumbs[0]->width  ( ) == img.width  ( ) ) &&
       ( pItem->pCache->arrayOfThumbs[0]->height ( ) == img.height ( ) ) )
    return;
  // swap width and height
  if ( ( pItem->pCache->arrayOfThumbs[0]->width  ( ) == img.height ( ) ) &&
       ( pItem->pCache->arrayOfThumbs[0]->height ( ) == img.width  ( ) ) )  {
    // but only if it is not already in order ...
    if ( ( img.width ( ) >= img.height ( ) ) == ( pItem->pXmlImage->width >= pItem->pXmlImage->height ) )
      return;
    int w = pItem->pXmlImage->width;
    pItem->pXmlImage->width  = pItem->pXmlImage->height;
    pItem->pXmlImage->height = w;
    return;
  }
}

void DialogKenBurns::slotAlternateColors ( )
{
  //Item *pItem = (DialogKenBurns::Item *)m_pListView->firstChild ( );  //ooo
  Item *pItem = (DialogKenBurns::Item *)m_pListView->topLevelItem ( 0 )->child ( 0 );   //xxx
  bool bAlternate = true;
  while ( pItem ) {
    bAlternate = ! bAlternate;
    pItem->bAlternateColor = bAlternate;
    //pItem->repaint ( );   //oooo
    //pItem = (Item *)pItem->nextSibling ( );   //ooo
    pItem = (Item *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);	//xxx
  }

  /*if ( m_pNextItem )
       m_pNextItem->setSelected ( TRUE );*/ //oooo
  m_pNextItem = NULL;
}

//void DialogKenBurns::slotContextMenu ( Q3ListViewItem *, const QPoint &pos, int ) //ooo
void DialogKenBurns::slotContextMenu ( QTreeWidgetItem *, const QPoint &pos, int )  //xxx
{
  //Q3PopupMenu *pMenu = new Q3PopupMenu ( m_pListView );   //ooo
  QMenu *pMenu = new QMenu ( m_pListView ); //xxx
  /*int iIDs[7];
  QString qsName, qsCurrent;
  QFileInfo fileInfo;
  Undelete *pUndelete = NULL;
  if ( m_listUndelete.size ( ) > 0 )  {
    pUndelete = m_listUndelete.back ( );
    fileInfo.setFile ( pUndelete->pCache->qsFileName );
    qsName  = fileInfo.fileName ( );
  }
  if ( m_pActiveItem )  {
    fileInfo.setFile ( m_pActiveItem->pCache->qsFileName );
    qsCurrent = fileInfo.fileName ( );
    iIDs[0]=pMenu->insertItem ( tr ( "&Remove %1" ).arg ( qsCurrent ), 1 ); //
  }
  if ( pUndelete ) 
    iIDs[1]=pMenu->insertItem ( tr ( "Un-Remove %1" ).arg ( qsName ),  2 );
  pMenu->insertSeparator ( );
  iIDs[2] = pMenu->insertItem ( tr ( "Move &Up" ),    3 ); // 
  iIDs[3] = pMenu->insertItem ( tr ( "Move &Down" ),  4 ); // 
  pMenu->insertSeparator ( );
  iIDs[4] = pMenu->insertItem ( tr ( "Rotate &90" ),  5 ); // move this entry to Virtual Folder
  iIDs[5] = pMenu->insertItem ( tr ( "Rotate &180" ), 6 ); // move all entries to m_pPreview
  iIDs[6] = pMenu->insertItem ( tr ( "Rotate &270" ), 7 ); // move all entries to m_pPreview

  if ( m_pActiveItem && ! m_pActiveItem->pXmlImage )  {
    // Have a VID item, can't rotate
    pMenu->setItemEnabled ( iIDs[4], false );
    pMenu->setItemEnabled ( iIDs[5], false );
    pMenu->setItemEnabled ( iIDs[6], false );
  }
  if ( m_pListView->firstChild ( ) == m_pActiveItem )
    pMenu->setItemEnabled ( iIDs[2], false );
  if ( m_pListView->lastItem ( ) == m_pActiveItem )
    pMenu->setItemEnabled ( iIDs[3], false );

  int iID = pMenu->exec ( pos );
  if ( iID == 1 )       // Delete
    deleteCurrentItem( );
  else if ( iID == 2 )
    unDeleteItem     ( );
  else if ( iID == 3 )  // Move Up
    moveCurrentItem  (  true );
  else if ( iID == 4 )  // Move Down
    moveCurrentItem  ( false );
  else if ( iID == 5 )  // Rotate 90
    rotate(     90.0 );
  else if ( iID == 6 )  // Rotate 180
    rotate(    180.0 );
  else if ( iID == 7 )  // Rotate 270
    rotate(    270.0 );
*/  //oooo
  delete pMenu;
}

void DialogKenBurns::deleteCurrentItem ( )
{
/*  if ( ! m_pActiveItem )
    return;

  Item * pCurrItem = (Item *)m_pActiveItem->itemBelow ( );
  if ( ! pCurrItem )
    pCurrItem = (Item *)m_pActiveItem->itemAbove ( );

  Undelete *pUndelete = NULL;
  Item *pItem = (Item *)m_pListView->firstChild ( );
  int iIDX = 0; // Calculate the idx of the item
  while ( pItem != m_pActiveItem )  {
    iIDX++;
    pItem = (Item *)pItem->nextSibling ( );
  }

  if ( m_pActiveItem->pXmlImage && m_pActiveItem->pXmlImage->node_name == IMG_NODE ) {
    CXmlSlideshow::img_struct *pTmp = new CXmlSlideshow::img_struct;
    *pTmp = *m_pActiveItem->pXmlImage;
    pUndelete = new Undelete ( (uint)iIDX, m_pActiveItem->pCache, pTmp );
  }
  else if ( m_pActiveItem->pXmlVideo && m_pActiveItem->pXmlVideo->node_name == VID_NODE )  {
    CXmlSlideshow::vid_struct *pTmp = new CXmlSlideshow::vid_struct;
    *pTmp = *m_pActiveItem->pXmlVideo;
    pUndelete = new Undelete ( (uint)iIDX, m_pActiveItem->pCache, pTmp );
  }
  else
    return;

  delete m_pActiveItem;
  m_pActiveItem = pCurrItem;

  if ( pUndelete )
    m_listUndelete.push_back ( pUndelete );

//  Also need to re-index the entries in the listView.
  reindexItems ( );
  QTimer::singleShot ( 10, this, SLOT ( slotDisplayImage    ( ) ) );*/  //oooo
}

void DialogKenBurns::unDeleteItem ( )
{
/*  if ( m_listUndelete.size ( ) < 1 )
    return;

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  Undelete *pUndelete = m_listUndelete.back ( );
  m_listUndelete.pop_back ( );

  Item *pItem = (Item *)m_pListView->firstChild ( );
  int iIDX = pUndelete->iPosition;
  while ( pItem && iIDX > 0 )  {
    iIDX--;
    pItem = (Item *)pItem->nextSibling ( );
  }
  if ( ! pItem )
    pItem = (Item *)m_pListView->lastItem ( );

  QFileInfo fileInfo ( pUndelete->pCache->qsFileName );
  Item *pNewItem = new Item ( pUndelete->pCache, pUndelete->pXmlImage, m_pSlideshow->delay, pUndelete->iPosition, m_pListView, pItem, fileInfo.fileName ( ) );

  if ( pItem && pUndelete->iPosition == 0 )
    pItem->moveItem ( pNewItem );

  pUndelete->pXmlImage = NULL;
//  m_listUndelete.remove ( pUndelete );
  delete pUndelete;
  m_pActiveItem = pNewItem;
// Also need to re-index the entries in the listView.
  createPix ( pNewItem );
  reindexItems ( );
  QApplication::restoreOverrideCursor ( );
  QTimer::singleShot ( 10, this, SLOT ( slotDisplayImage    ( ) ) );*/  //oooo
}

void DialogKenBurns::reindexItems ( )
{
/*  // Walks through all Items in the ListView and re-IDX them
  int iIDX = 1;
  Q3ListViewItem *pItem = m_pListView->firstChild ( );
  while ( pItem )  {
    pItem->setText ( 1, QString ( "%1" ).arg ( iIDX++ ) ); 
    pItem = pItem->nextSibling ( );
  }
  changeCaption ( );
  QTimer::singleShot ( 5, this, SLOT ( slotAlternateColors ( ) ) );*/   //oooo
}

void DialogKenBurns::moveCurrentItem ( bool bUp )
{
/*  if ( ! m_pActiveItem )
    return;

  Q3ListViewItem *pNextItem = m_pActiveItem->nextSibling ( );
  if ( ! bUp && pNextItem )
    m_pActiveItem->moveItem ( pNextItem );
  else if ( bUp )  {
    Q3ListViewItem *pPrev, *pPrevPrev;
    Q3ListViewItem *pCurr = m_pListView->firstChild ( );
    pPrev = pPrevPrev = NULL;
    while ( pCurr != m_pActiveItem )  {
      pPrevPrev = pPrev;
      pPrev = pCurr;
      pCurr = pCurr->nextSibling ( );
    }
    if ( pPrevPrev )
      m_pActiveItem->moveItem ( pPrevPrev );
  }
  reindexItems ( );*/   //oooo
}

void DialogKenBurns::rotate ( double fRotate )
{
  // rotate thumb[0] and Pix[0]
  if ( ! m_pActiveItem || ! m_pActiveItem->pCache )
    return; // WTF ???

  QCursor myCursor ( Qt::WaitCursor );
  QApplication::setOverrideCursor ( myCursor );

  Cache::Thumbs::ImageEntry *pCache = (Cache::Thumbs::ImageEntry *)m_pActiveItem->pCache;
  if ( ! pCache->arrayOfThumbs )
         pCache->loadImages  ( );
  if ( pCache->arrayOfThumbs )  {
       pCache->bStoreData = true;
       pCache->fRotate   += (float)fRotate;
       pCache->initMatrix ( );
// switch width / height ( if required )
       if ( fRotate == 90.0 || fRotate == 270.0 )  {
         int w = m_pActiveItem->pXmlImage->width;
         m_pActiveItem->pXmlImage->width  = m_pActiveItem->pXmlImage->height;
         m_pActiveItem->pXmlImage->height = w;
       }
// Re-create pix
       createPix ( m_pActiveItem );
// force re-create KenBurns ( if avail )
       m_pSlider->setValue   ( 0 );
       m_pActiveItem->pXmlImage->effect[0].randomKenBurns ( 0.5, m_pActiveItem->pXmlImage->width, m_pActiveItem->pXmlImage->height, true );
       m_pActiveItem->bRecreateImage = true;
       m_pActiveItem->update ( );
       QTimer::singleShot    ( 5, this, SLOT ( slotDisplayImage ( ) ) );
       updateText ( );
// re-create pix in DialogImages -> Slideshow
// re-create pix in DialogImages -> Preview
       Global::pThumbsCache->saveImageDB ( );
  }
  QApplication::restoreOverrideCursor ( );
}

void DialogKenBurns::slotAnimate ( )
{
  static bool bBusy = false;
  if ( bBusy )
    return;

  // Will create 15 images per second of KenBurns effect
  // Will store them until [Animate] is clicked on any other image.
  // 
  if ( m_iAnimation >= 0 )  {
    m_pButtonAnimate->setText ( tr ( "&Animate" ) );
    m_iAnimation   = -1;
    Item *pItem    = m_pChangedItem;
    m_pChangedItem = NULL;
    m_pActiveItem  = NULL;
    m_pSlider->setValue ( 0 );
    m_pCheckAspect->setEnabled ( true );
    m_pButtonHide ->setEnabled ( true );
    slotCurrentChanged  ( pItem );
    return;
  }

  if ( ! m_pActiveItem || ! m_pActiveItem->pXmlImage )
    return;

  bBusy = true;
  if ( m_listAnimation.size  ( ) > 0 )
       m_listAnimation.clear ( );

  int t, iCount = (int)( m_pActiveItem->pXmlImage->fDuration * 15.0 );
  float  fPosition;
  QRect  rect;
  Item *pItem = m_pActiveItem;
  if ( iCount < 0 )
       iCount = (int)( m_pSlideshow->delay * 15.0 );

  m_pChangedItem = pItem;
  m_pButtonAnimate->setText( tr ( "&Stop Anim" ) );
  m_pCheckAspect   ->setEnabled ( false );
  m_pCheckKenBurns ->setEnabled ( false );
  m_pButtonRandom  ->setEnabled ( false );
  m_pButtonDuration->setEnabled ( false );
  m_pButtonHide    ->setEnabled ( false );

  for ( t=0; t<iCount; t++ )  {
    fPosition = (float)t / iCount;
    rect = pItem->getCurrentRect ( fPosition );
    QPixmap pix ( rect.width ( ), rect.height ( ) );
    //copyBlt ( &pix, 0, 0, &pItem->sizedPix, rect.x ( ), rect.y ( ), rect.width ( ), rect.height ( ) );    //ooo
    pix = pItem->sizedPix.copy( rect.x ( ), rect.y ( ), rect.width ( ), rect.height ( ) );			//xxx
    { // How stupid, only to scale I have to copy back and forth.
      //QImage img = pix.convertToImage ( );    //ooo
      QImage img = pix.toImage ( );             //xxx
      //img = img.smoothScale ( pItem->sizedPix.size ( ) ); //ooo
      img = img.scaled ( pItem->sizedPix.size ( ) );        //xxx
      pix.convertFromImage  ( img );
    }
    m_listAnimation.push_back  ( pix );
    m_pSlider->setValue ( (int)( fPosition * 1000.0 ) );
    qApp->processEvents ( );
  }
  m_iAnimation = 0;
  QTimer::singleShot ( 1000.0/15.0, this, SLOT ( slotAnimation ( ) ) );
  bBusy = false;
}

void DialogKenBurns::slotAnimation ( )
{
  if ( m_iAnimation < 0 )
    return; // Dunno why I am here.

  int iMax = m_listAnimation.size ( );
  m_iAnimation++;
  if ( m_iAnimation >= iMax )
       m_iAnimation = 0;

  m_pLabelPreview->setPixmap ( m_listAnimation[m_iAnimation] );
  float fPos = (float)m_iAnimation / iMax;
  m_pSlider->setValue ( (int)(fPos * 1000.0) );
  QTimer::singleShot  ( 1000.0/15.0, this, SLOT ( slotAnimation ( ) ) );
}

void DialogKenBurns::slotDuration ( )
{
  if ( ! m_pActiveItem )
    return;
  bool   bOkay;
  double fDuration = m_pActiveItem->pXmlImage->fDuration;
  if  (  fDuration < 0.0 )
         fDuration = m_pSlideshow->delay;

  //fDuration = QInputDialog::getDouble ( tr ( "Change Duration" ), tr ( "Enter Duration in Seconds : " ), fDuration, 0.001, 360.0, 2, &bOkay );    //ooo
  fDuration = QInputDialog::getDouble ( this, tr ( "Change Duration" ), tr ( "Enter Duration in Seconds : " ), fDuration, 0.001, 360.0, 2, &bOkay );    //xxx
  if ( bOkay )  {
    m_pActiveItem->pXmlImage->fDuration = fDuration;
    updateText ( );
    m_pActiveItem->update ( );
    m_pSlider->setValue   (   0 );
  }
}

void DialogKenBurns::slotToggleKenBurns ( bool bOn )
{
  if ( ! m_pActiveItem || ! m_pActiveItem->pXmlImage )
    return;

  m_pActiveItem->pXmlImage->effect[0].name = bOn ? EFFECT_TYPE_KENBURNS : "";
  if ( bOn )  {
    if ( ( m_pActiveItem->pXmlImage->width == 0 ) || ( m_pActiveItem->pXmlImage->height == 0 ) )  {
      QImage img ( m_pActiveItem->pXmlImage->src );
      m_pActiveItem->pXmlImage->width  = img.width  ( );
      m_pActiveItem->pXmlImage->height = img.height ( );
      m_pActiveItem->pXmlImage->effect[0].randomKenBurns ( 0.5, m_pActiveItem->pXmlImage->width, m_pActiveItem->pXmlImage->height, true );
    }
  }

  m_pActiveItem->update ( );
  updateText ( );
  m_pSlider->setValue   (   0 );
  m_pSlider->setEnabled ( bOn );
  m_pButtonRandom->setEnabled ( bOn );
  QTimer::singleShot ( 5, this, SLOT ( slotDisplayImage ( ) ) ); 
}

void DialogKenBurns::slotHide ( )
{
  QString qsHide = tr ( "Hide &Preview" );
  QString qsShow = tr ( "Show &Preview" );
  if ( m_pButtonHide->text ( ) == qsShow )  {
    m_pButtonHide->setText ( qsHide );
    m_pPreviewFrame->show  ( );
    m_pLabelTime->show     ( );
    m_pSlider->show        ( );
    m_pLabelInfo->show     ( );
    m_pLabelHeader->show   ( );
    m_pLabelPreview->show  ( );
    m_pFrameText->show     ( );
    m_pButtonAnimate->setEnabled ( true );
  }
  else  {
    m_pButtonHide->setText ( qsShow );
    m_pPreviewFrame->hide  ( );
    m_pLabelTime->hide     ( );
    m_pSlider->hide        ( );
    m_pLabelInfo->hide     ( );
    m_pLabelHeader->hide   ( );
    m_pLabelPreview->hide  ( );
    m_pFrameText->hide     ( );
    m_pButtonAnimate->setEnabled ( false );
  }
}

void DialogKenBurns::slotRandom ( )
{
  // Creates a randome KenBurns effect for the active image.
  if ( ! m_pActiveItem || ! m_pActiveItem->pXmlImage )
    return;

  if ( m_bFirstQuestion && MessageBox::question ( this, tr ( "Are you sure" ), tr ( "Are you sure you want to randomize the KenBurns effect or this image ?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
    return; 

  m_bFirstQuestion = false;
  m_pSlider->setValue   ( 0 ); 
  m_pActiveItem->pXmlImage->effect[0].randomKenBurns ( 0.5, m_pActiveItem->pXmlImage->width, m_pActiveItem->pXmlImage->height, true );
  m_pActiveItem->update ( );
  QTimer::singleShot    ( 5, this, SLOT ( slotDisplayImage ( ) ) );
  updateText ( );
}

//void DialogKenBurns::slotCurrentChanged ( Q3ListViewItem *pListItem ) //ooo
void DialogKenBurns::slotCurrentChanged ( QTreeWidgetItem *pListItem )  //xxx
{
  DialogKenBurns::Item *pItem = (DialogKenBurns::Item *)pListItem;

  if ( m_pActiveItem == pItem )
    return;

  if ( m_pChangedItem )  { // m_iAnimation >= 0 )  {
    m_pChangedItem = pItem;
    return;
  }

  bool   bImage = ( pItem->pXmlImage != NULL );
  m_pActiveItem =   pItem;

  if ( ! pItem->pix.isNull ( ) )  {
    m_pLabelPreview->setPixmap    ( pItem->pix );
    m_pLabelPreview->setFixedSize ( pItem->pix.size ( ) );
    m_pLabelInfo->setText ( "" );
  }
  else if ( pItem->pCache )  {
    if ( ! pItem->pCache->arrayOfThumbs )
           pItem->pCache->loadImages  ( );
    if ( ! pItem->pCache->arrayOfThumbs || ! pItem->pCache->arrayOfThumbs[0] )
      return;

    QImage img = *pItem->pCache->arrayOfThumbs[0];
    if ( bImage && img.isNull ( ) )  {
      QString str;
      //str.sprintf ( "Could not find image file <%s>\n.", pItem->pCache->qsFileName.ascii ( ) );   //ooo
      str.sprintf ( "Could not find image file <%s>\n.", pItem->pCache->qsFileName.toLatin1 ( ).data ( ) ); //xxx
      MessageBox::warning ( NULL, "Could not find image file\n.", str );
      return;
    }
    setImage ( img );
    pItem->bRecreateImage = true;
  }

  // Finally we can enable the buttons, as we have an image to work on
  m_pButtonAnimate ->setEnabled ( bImage );
  m_pButtonDuration->setEnabled ( bImage );
  m_pButtonRandom  ->setEnabled ( bImage );
  m_pSlider        ->setEnabled ( bImage );
  m_pCheckKenBurns ->setEnabled ( bImage );
  m_pCheckKenBurns ->setChecked ( bImage ? pItem->pXmlImage->effect[0].name == EFFECT_TYPE_KENBURNS : false );
  m_pSlider->setValue ( 0 );
  updateText  ( );
  QTimer::singleShot  ( 500, this, SLOT ( slotDisplayImage ( ) ) );
}

void DialogKenBurns::slotDisplayImage ( )
{
  if ( ! m_pActiveItem )
    return;

  if ( m_pActiveItem->bRecreateImage )  {
    if ( m_pActiveItem->pCache )  {
      QImage img ( m_pActiveItem->pCache->qsFileName );
      if ( img.isNull ( ) )  {
        if ( m_pActiveItem->pXmlImage )  {
          QString str;
          //str.sprintf ( "Could not find image file <%s>\n.", m_pActiveItem->pCache->qsFileName.ascii ( ) );   //ooo
          str.sprintf ( "Could not find image file <%s>\n.", m_pActiveItem->pCache->qsFileName.toLatin1 ( ).data ( ) ); //xxx
          MessageBox::warning ( NULL, "Could not find image file\n.", str );
        }
        return;
      }
      setImage ( img );
      m_pActiveItem->bRecreateImage = false;
    }
  }
  else 
    updateImage ( );
}

void DialogKenBurns::slotSlider ( int iPos )
{
  if ( m_pActiveItem )  {
    if ( m_pActiveItem->pix.isNull ( ) )
      slotDisplayImage ( );
    if ( m_pActiveItem->pix.isNull ( ) )
      return;

    // At this point we have a valid pre-rendered pix.
    // Note: iPos is in the range of [ 0 .. 1000 ]
    double fPosition = (double)iPos / 1000.0; // Between 0.0 and 1.0
    float  fDuration = m_pActiveItem->pXmlImage->fDuration;
    if (   fDuration < 0.0   )
           fDuration = m_pSlideshow->delay;

    QString qsColor, qsTime;
    qsColor.sprintf  ( "#%02X00%02X", (unsigned char)((1.0f - fPosition) * 255), (unsigned char)(fPosition * 255) );
    qsTime = QString ( "<b>%1 Sec</b>" ).arg ( fPosition * fDuration );
    m_pLabelTime->setText ( qsTime );
    if ( m_iAnimation >= 0 )  // m_pChangedItem )
      return; 

    QPixmap  pix     ( m_pActiveItem->pix );
    QPainter painter ( &pix );
    //QPen     pen     ( qsColor, 3 );			//ooo
    QPen     pen     ( QColor ( qsColor ), 3 );		//xxx
    QRect    rect = m_pActiveItem->getCurrentRect ( fPosition );

    painter.setPen   (  pen );
    painter.drawRect ( rect );
    // Finally we can set that pixmap ...
    m_pLabelPreview->setPixmap ( pix );
  }
}

void DialogKenBurns::previewMousePressed ( QMouseEvent *pEvent )
{
  m_bMousePressed = true;
  if ( ! m_pActiveItem || ! pEvent )
    return;

  if ( m_pActiveItem->sizedPix.isNull ( ) )
    return;

  // First we have to find if the mouse click occured close to one of the corners of the start or end - rect.
  QPoint pos = pEvent->pos ( );
  m_currentMousePos  = pos;

  m_iCorner = findCorner ( m_pActiveItem->rect[0], pos );
  if ( m_iCorner == 0 )
       m_iCorner = findCorner ( m_pActiveItem->rect[1], pos ) << 4;
  if ( m_iCorner != 0 )
    return;

  // Another option is that the cursor is close to a line, in which case 
  // We activate moving the complete box around
  m_iCorner = findEdge ( m_pActiveItem->rect[0], pos );
  if ( m_iCorner == 0 ) 
       m_iCorner = findEdge ( m_pActiveItem->rect[1], pos ) << 4;
}

void DialogKenBurns::previewMouseReleased ( QMouseEvent * )
{
  m_bMousePressed = false;
  m_iCorner       = 0;
  updateImage ( );
}

void DialogKenBurns::previewMouseMove ( QMouseEvent *pEvent )
{
  if ( m_bMousePressed && m_pActiveItem )  {
    QPoint pos   = pEvent->pos ( );
    QPoint delta = pos - m_currentMousePos;
    if ( ( m_iCorner & 0x000f ) != 0 )  {  // Move a edge in the Start Rect
      moveCorner ( m_pActiveItem->rect[0], pos );
      updateKenBurns ( true, false );
    }
    else if ( ( m_iCorner & 0x00f0 ) != 0 )  {  // Move a edge in the Stop Rect
      moveCorner ( m_pActiveItem->rect[1], pos );
      updateKenBurns ( false, false );
    }
    else if ( ( m_iCorner & 0x0f00 ) != 0 )  {  // Move the whole Start Rect
      moveRect ( m_pActiveItem->rect[0], delta );
      updateKenBurns ( true, true );
    }
    else if ( ( m_iCorner & 0xf000 ) != 0 )  {  // Move a edge in the Stop Rect
      moveRect ( m_pActiveItem->rect[1], delta );
      updateKenBurns ( false, true );
    }
    if ( m_iCorner != 0 )  {  // Somethng has changed
      updateText      ( );
      updateImageDyn  ( );
      m_pActiveItem->update ( );
    }
    m_currentMousePos = pEvent->pos ( );
  }
}

void DialogKenBurns::updateKenBurns ( bool bStartRect, bool bKeepWH )
{
  double fScaleX = (double)m_pActiveItem->pXmlImage->width  / m_pActiveItem->pix.width  ( );
  double fScaleY = (double)m_pActiveItem->pXmlImage->height / m_pActiveItem->pix.height ( );
  CXmlSlideshow::effect_struct *pEffect = &m_pActiveItem->pXmlImage->effect[0];
  QRect rect;

  if ( bStartRect )  {
    rect = m_pActiveItem->rect[0];
    int w = pEffect->x1 - pEffect->x0;
    int h = pEffect->y1 - pEffect->y0;

    pEffect->x0 = (int)( fScaleX * rect.left ( ) );
    pEffect->y0 = (int)( fScaleY * rect.top  ( ) );
    if ( bKeepWH )  {
      pEffect->x1 = pEffect->x0 + w;
      pEffect->y1 = pEffect->y0 + h;
    }
    else  {
      pEffect->x1 = (int)( fScaleX * rect.right  ( ) );
      pEffect->y1 = (int)( fScaleY * rect.bottom ( ) );
    }
  }
  else  {
    rect = m_pActiveItem->rect[1];
    int w = pEffect->xe1 - pEffect->xe0;
    int h = pEffect->ye1 - pEffect->ye0;

    pEffect->xe0 = (int)( fScaleX * rect.left ( ) );
    pEffect->ye0 = (int)( fScaleY * rect.top  ( ) );
    if ( bKeepWH )  {
      pEffect->xe1 = pEffect->xe0 + w;
      pEffect->ye1 = pEffect->ye0 + h;
    }
    else  {
      pEffect->xe1 = (int)( fScaleX * rect.right ( ) );
      pEffect->ye1 = (int)( fScaleY * rect.bottom( ) );
    }
  }
}

void DialogKenBurns::moveCorner ( QRect &rect, QPoint &delta )
{
  int iWidth  = m_pActiveItem->pix.width  ( );
  int iHeight = m_pActiveItem->pix.height ( );
  if ( delta.x ( ) < 0 )
    delta.setX ( 0 );
  else if ( delta.x ( ) > iWidth )
    delta.setX ( iWidth );
  if ( delta.y ( ) < 0 )
    delta.setY ( 0 );
  else if ( delta.y ( ) > iHeight )
    delta.setY ( iHeight );

  switch ( m_iCorner )  {
  case StartTopLeftCorner:
  case StopTopLeftCorner:
    rect.setLeft ( delta.x ( ) );
    rect.setTop  ( delta.y ( ) );
  break;
  case StartTopRightCorner:
  case StopTopRightCorner:
    rect.setRight ( delta.x ( ) );
    rect.setTop   ( delta.y ( ) );
  break;
  case StartBottomLeftCorner:
  case StopBottomLeftCorner:
    rect.setLeft   ( delta.x ( ) );
    rect.setBottom ( delta.y ( ) );
  break;
  case StartBottomRightCorner:
  case StopBottomRightCorner:
    rect.setRight  ( delta.x ( ) );
    rect.setBottom ( delta.y ( ) );
  break;
  default:
  break;
  }
  if ( m_pCheckAspect->isChecked ( ) )  {
    double fAspect  = (double)iHeight / iWidth;
    int w = rect.width ( );
    int h = (int)(fAspect * w );
    rect.setWidth  ( w );
    rect.setHeight ( h );
  }
}

void DialogKenBurns::moveRect ( QRect &rect, QPoint &delta )
{
  int iWidth  = m_pActiveItem->pix.width  ( );
  int iHeight = m_pActiveItem->pix.height ( );

  if ( rect.left( ) + delta.x ( ) < 0 )
    delta.setX  (  -rect.left ( ) );
  else if ( rect.right ( ) + delta.x ( ) > iWidth )
    delta.setX  ( iWidth - rect.right ( ) );
  if ( rect.top ( ) + delta.y ( ) < 0 )
    delta.setY  ( -rect.top ( ) );
  else if ( rect.bottom ( ) + delta.y ( ) > iHeight )
    delta.setY  ( iHeight - rect.bottom ( ) );

  //rect.moveBy ( delta.x ( ), delta.y ( ) );   //oooo
}

uint DialogKenBurns::findCorner ( QRect &rect, QPoint &pos )
{
  // Checks whether the pos is within the valid selection rect of a corner.
  int iMaxDist = 8;
  QRect r;
  r = QRect ( rect.left ( )-iMaxDist,  rect.top ( )-iMaxDist, iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0001; // TopLeft
  r = QRect ( rect.right ( )-iMaxDist, rect.top ( )-iMaxDist, iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0002; // TopRight
  r = QRect ( rect.left ( )-iMaxDist,  rect.bottom ( )-iMaxDist, iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0004; // BottomLeft
  r = QRect ( rect.right ( )-iMaxDist, rect.bottom ( )-iMaxDist, iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0008; // BottomRight
  return 0; 
}

uint DialogKenBurns::findEdge ( QRect &rect, QPoint &pos )
{
  int iMaxDist = 8;
  QRect r;
  r = QRect ( rect.left ( )-iMaxDist, rect.top ( )-iMaxDist, iMaxDist*2, rect.height ( )+iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0100; // LeftEdge
  r = QRect ( rect.left ( )-iMaxDist, rect.top ( )-iMaxDist, rect.width ( )+iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0200; // TopEdge
  r = QRect ( rect.right ( )-iMaxDist, rect.top ( )-iMaxDist, iMaxDist*2, rect.height( )+iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0400; // RightEdge
  r = QRect ( rect.left ( )-iMaxDist, rect.bottom ( )-iMaxDist, rect.width ( )+iMaxDist*2, iMaxDist*2 );
  if ( r.contains ( pos ) )
    return 0x0800; // BottomEdge

  return 0;
}

void DialogKenBurns::updateText ( )
{
  m_pLabelInfo->setText ( "" );
  if ( ! m_pActiveItem || ! m_pActiveItem->pXmlImage )
    return; // Somehow must have clicked on a VID.

  QString qsInfo;
  CXmlSlideshow::img_struct    *pXmlImg = m_pActiveItem->pXmlImage;
  CXmlSlideshow::effect_struct *pEffect = &pXmlImg->effect[0];

  int iWidth  = pXmlImg->width;
  int iHeight = pXmlImg->height;

  if ( ( iWidth < 5 ) || ( iHeight < 5 ) )  {
    QImage img ( pXmlImg->src );
    iWidth  = img.width  ( );
    iHeight = img.height ( );
  }
  if ( ( iWidth < 5 ) || ( iHeight < 5 ) )
    return; // Something wrong with tha image !!!!

  double fDuration = pXmlImg->fDuration;
  if   ( fDuration < 0.0 )
         fDuration = m_pSlideshow->delay;

  QFileInfo fileInfo ( pXmlImg->src );
  qsInfo   = QString ( "<b>File: %1</b><br />" ).arg ( fileInfo.fileName ( ) );
  qsInfo  += QString ( "<b>Size: %1x%2</b><br />&nbsp;<br />").arg ( iWidth ).arg ( iHeight );

  qsInfo  += "<b><u>KenBurns : </u></b><br />";

  if ( pEffect->name == EFFECT_TYPE_KENBURNS )  {
    qsInfo += "<font color='#FF0000'><u>From : </u><br />";
    qsInfo += QString ( "<b>( %1, %2, %3, %4 )</b></font><br />&nbsp;<br />" ).arg ( pEffect->x0 ).arg ( pEffect->y0 ).arg ( pEffect->x1 ).arg ( pEffect->y1 );

    qsInfo += "<font color='#0000FF'><u>To : </u><br />";
    qsInfo += QString ( "<b>( %1, %2, %3, %4 )</b></font><br />&nbsp;<br />" ).arg ( pEffect->xe0 ).arg ( pEffect->ye0 ).arg ( pEffect->xe1 ).arg ( pEffect->ye1 );

    qsInfo += "<font color='#00AA00'><p></p><u>Duration : </u><br />";
    qsInfo += QString ( "<b>%1 mS</b></font><br>&nbsp;</br>" ).arg ( fDuration * 1000.0 );
  }
  else  {
    qsInfo += "<br /><b><font size='+3'>Turned Off !</font></b><br />";
  }
  m_pLabelInfo->setText ( qsInfo );
}

void DialogKenBurns::updateImageDyn ( )
{
  if ( ! m_pActiveItem->pXmlImage )
    return; // Somehow must hav clicked on a VID.

  CXmlSlideshow::img_struct    *pXmlImg = m_pActiveItem->pXmlImage;
  CXmlSlideshow::effect_struct *pEffect = &pXmlImg->effect[0];

  if ( pEffect->name != EFFECT_TYPE_KENBURNS )  {
    m_pLabelPreview->setPixmap ( m_pActiveItem->sizedPix );
    return; // not KenBurs .. so what the heck ????
  }

  // And now we can paint the start / stop rectangles.
  QPixmap  pix ( m_pActiveItem->sizedPix );
  QPainter painter (   &pix );
  QPen     pen ( Qt::red, 3 );

  // Next we paint the start / stop rects ...
  painter.setPen   ( pen );
  painter.drawRect ( m_pActiveItem->rect[0] );

  pen.setColor( Qt::blue );
  painter.setPen   ( pen );
  painter.drawRect ( m_pActiveItem->rect[1] );

  pen.setColor     ( "#00AA00" );
  painter.setPen   ( pen );

  drawArrow ( &painter, m_pActiveItem->rect[0].left ( ), m_pActiveItem->rect[0].top   ( ), m_pActiveItem->rect[1].left ( ), m_pActiveItem->rect[1].top   ( ) );
  drawArrow ( &painter, m_pActiveItem->rect[0].right( ), m_pActiveItem->rect[0].top   ( ), m_pActiveItem->rect[1].right( ), m_pActiveItem->rect[1].top   ( ) );
  drawArrow ( &painter, m_pActiveItem->rect[0].right( ), m_pActiveItem->rect[0].bottom( ), m_pActiveItem->rect[1].right( ), m_pActiveItem->rect[1].bottom( ) );
  drawArrow ( &painter, m_pActiveItem->rect[0].left ( ), m_pActiveItem->rect[0].bottom( ), m_pActiveItem->rect[1].left ( ), m_pActiveItem->rect[1].bottom( ) );

  // Finally we can set that pixmap ...
  m_pLabelPreview->setPixmap ( pix );
}


void DialogKenBurns::updateImage ( )
{
  if ( ! m_pActiveItem->pXmlImage )
    return; // Somehow must hav clicked on a VID.

  CXmlSlideshow::img_struct    *pXmlImg = m_pActiveItem->pXmlImage;
  CXmlSlideshow::effect_struct *pEffect = &pXmlImg->effect[0];

  int iWidth  = pXmlImg->width;
  int iHeight = pXmlImg->height;

  if ( ( iWidth < 5 ) || ( iHeight < 5 ) )  {
    QImage img ( pXmlImg->src );
    iWidth  = img.width  ( );
    iHeight = img.height ( );
  }
  if ( ( iWidth < 5 ) || ( iHeight < 5 ) )
    return; // Something wrong with tha image !!!!

  if ( pEffect->name != EFFECT_TYPE_KENBURNS )  {
    m_pLabelPreview->setPixmap ( m_pActiveItem->sizedPix );
    return; // not KenBurs .. so what the heck ????
  }

  double fKenBurns[2][4]; // in the order [start,stop][x1,y1,w,h,x2,y2]
  // populate the array
  fKenBurns[0][0] =   (double)pEffect->x0  / iWidth;  // x1
  fKenBurns[0][1] =   (double)pEffect->y0  / iHeight; // y1
  fKenBurns[0][2] = ( (double)pEffect->x1  / iWidth  ) - fKenBurns[0][0];
  fKenBurns[0][3] = ( (double)pEffect->y1  / iHeight ) - fKenBurns[0][1];
  fKenBurns[1][0] =   (double)pEffect->xe0 / iWidth;
  fKenBurns[1][1] =   (double)pEffect->ye0 / iHeight;
  fKenBurns[1][2] = ( (double)pEffect->xe1 / iWidth  ) - fKenBurns[1][0];
  fKenBurns[1][3] = ( (double)pEffect->ye1 / iHeight ) - fKenBurns[1][1];

  iWidth  = m_pActiveItem->sizedPix.width  ( );
  iHeight = m_pActiveItem->sizedPix.height ( );
  m_pActiveItem->rect[0] = QRect ( (int)(iWidth * fKenBurns[0][0]), (int)(iHeight * fKenBurns[0][1]), (int)(iWidth * fKenBurns[0][2]), (int)(iHeight * fKenBurns[0][3]) );  m_pActiveItem->rect[1] = QRect ( (int)(iWidth * fKenBurns[1][0]), (int)(iHeight * fKenBurns[1][1]), (int)(iWidth * fKenBurns[1][2]), (int)(iHeight * fKenBurns[1][3]) );

  updateImageDyn ( );
  if ( m_pLabelPreview->pixmap ( ) )
       m_pActiveItem->pix = *m_pLabelPreview->pixmap ( ); // pix;
}

void DialogKenBurns::setImage ( QImage &img )
{
  Cache::Thumbs::Entry *pEntry = m_pActiveItem->pCache;

  QPixmap pix;
  if ( pEntry->getMatrix ( ) )
    //img = img.xForm ( *pEntry->getMatrix ( ) );   //ooo
    img = img.transformed ( *pEntry->getMatrix ( ) );   //xxx

  //img = img.smoothScale ( m_pPreviewFrame->width ( )-15, m_pPreviewFrame->height ( )-15, Qt::KeepAspectRatio );   //ooo
  img = img.scaled ( m_pPreviewFrame->width ( )-15, m_pPreviewFrame->height ( )-15, Qt::KeepAspectRatio );          //xxx
  pix.convertFromImage  ( img );
  m_pLabelPreview->setPixmap    ( pix );
  m_pLabelPreview->setFixedSize ( img.size ( ) );
  m_pActiveItem->sizedPix = pix;

  updateText  ( );
  updateImage ( );
}

void DialogKenBurns::drawArrow ( QPainter *pPainter, int x1, int y1, int x2, int y2 )
{
  QRect rect;

  pPainter->drawLine ( x1, y1, x2, y2 );
  rect.setX  (  x2   - 3 );
  rect.setY  (  y2   - 3 );
  rect.setWidth      ( 6 );
  rect.setHeight     ( 6 );
  pPainter->drawRect ( rect );
}

void DialogKenBurns::accept ( )
{
/*  // Before we quit the dialog, we need to re-assemble the slideshow.
  CXmlSlideshow::img_struct *pImg = NULL;
  CXmlSlideshow::vid_struct *pVid = NULL;
  m_pSlideshow->clearObjects ( );
  m_pSlideshow->audio_list         = m_pOrigSlideshow->audio_list;
  m_pSlideshow->validFilterNames   = m_pOrigSlideshow->validFilterNames;
  m_pSlideshow->total_audio_length = m_pOrigSlideshow->total_audio_length;

  Item *pItem = (Item *)m_pListView->firstChild ( );
  while ( pItem )  {
    if  ( pItem->pXmlImage )  {
       pImg = m_pSlideshow->addImg ( );
      *pImg = *pItem->pXmlImage;
    }
    else if ( pItem->pXmlVideo )  {
       pVid = m_pSlideshow->addVid ( );
      *pVid = *pItem->pXmlVideo;
    }
    pItem = (Item *)pItem->nextSibling ( );
  }

  //uiDialogKenBurns::accept ( );	//ooo
  QDialog::accept ( );			//xxx*/    //oooo
}

void DialogKenBurns::reject ( ) 
{
  //uiDialogKenBurns::reject ( );	//ooo
  QDialog::reject ( );			//xxx
}


