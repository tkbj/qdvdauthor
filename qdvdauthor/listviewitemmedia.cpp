/***************************************************************************
    listviewitemmedia.cpp
                             -------------------
    DialogMovie class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file re-implements the QListBoxText class
   and adds only a background color to it.

   For later this could be enhanced to include a small Icon at the beginning
   Which indicates the type (I.e. single movie, movie collection, or dvd-slideshow).

   Another icon on the right side could indicate if a movie is already
   connected to a button or action.

   And yet another Icon could indicate the (intended) Format of the Movie.
    
****************************************************************************/

#include <QFileInfo>
#include <QPainter>
#include <QMessageBox>
#include <QUrl> //xxx

#include "global.h"
#include "qdvdauthor.h"
#include "sourcefileentry.h"
#include "listviewitemmedia.h"
#include "xml_slideshow.h"

//ListViewItemMedia::ListViewItemMedia (Q3ListView *pListView, SourceFileEntry *pEntry, const QColor &theColor)		//ooo
ListViewItemMedia::ListViewItemMedia (QTreeWidget *pListView, SourceFileEntry *pEntry, const QColor &theColor)		//xxx
	//: Q3ListViewItem (pListView, QString (), pEntry->qsDisplayName)	//ooo
	: QTreeWidgetItem (pListView)	//xxx
{
  //setText(0, pEntry->qsDisplayName);		//xxx
  //setBackground(0, theColor);			//xxx
  //setForeground(0, theColor);			//xxx
  initMe (pEntry, theColor);
}

//ListViewItemMedia::ListViewItemMedia (Q3ListViewItem *pListViewItem, SourceFileEntry *pEntry, const QColor &theColor)	//ooo
ListViewItemMedia::ListViewItemMedia (QTreeWidgetItem *pListViewItem, SourceFileEntry *pEntry, const QColor &theColor)	//xxx
	//: Q3ListViewItem (pListViewItem, QString (), pEntry->qsDisplayName)	//ooo
	: QTreeWidgetItem (pListViewItem)	//xxx
{
  //setBackground(0, theColor);			//xxx
  //setForeground(0, theColor);			//xxx
  initMe (pEntry, theColor);
}

//ListViewItemMedia::ListViewItemMedia (Q3ListView *pListView, Q3ListViewItem *pAfterItem, SourceFileEntry *pEntry, const QColor &theColor)	//ooo
ListViewItemMedia::ListViewItemMedia (QTreeWidget *pListView, QTreeWidgetItem *pAfterItem, SourceFileEntry *pEntry, const QColor &theColor)	//xxx
	//: Q3ListViewItem (pListView, pAfterItem, QString (), pEntry->qsDisplayName)	//ooo
	: QTreeWidgetItem (pListView, pAfterItem)	//xxx
{
  //setBackground(0, theColor);			//xxx
  //setForeground(0, theColor);			//xxx
  initMe (pEntry, theColor);
}

//ListViewItemMedia::ListViewItemMedia ( Q3ListViewItem *pListViewItem, SourceFileInfo *pInfo, const QColor &color, QString str, QSize size )	//ooo
ListViewItemMedia::ListViewItemMedia ( QTreeWidgetItem *pListViewItem, SourceFileInfo *pInfo, const QColor &color, QString str, QSize size )	//xxx
  //: Q3ListViewItem ( pListViewItem, QString ( ), str )	//ooo
  : QTreeWidgetItem ( pListViewItem)		//xxx
{
  m_iButtonState     = StateStop;
  m_backgroundColor  = color;
  m_pSourceFileEntry = NULL;
  m_pSourceFileInfo  = pInfo;
  m_pAnimateMini     = NULL;		//oooo
  m_sizeThumbnails   = size;
  m_pPixmap          = NULL;
  
  setBackground(0, QBrush(m_backgroundColor) );		//xxx
}

ListViewItemMedia::~ListViewItemMedia ()
{
  if ( m_pAnimateMini )
    delete m_pAnimateMini;
  m_pAnimateMini = NULL;
  if ( m_pPixmap )
    delete m_pPixmap;
  m_pPixmap = NULL;	//oooo
}

void ListViewItemMedia::initMe ( SourceFileEntry *pEntry, const QColor &theBaseColor )
{
  // Here we create the sub items according to the SourceFileEntry -> listFileInfos
  int t;
  Utils               theUtils;		//xxx
  QColor              theColor;
  QFileInfo           fileInfo;
  SourceFileInfo     *pInfo;
  m_backgroundColor  = theBaseColor;
  m_pSourceFileEntry = pEntry;
  m_pSourceFileInfo  = NULL;
  m_iButtonState     = StateStop;
  m_iPreviousButtonState = 1000;
  m_pAnimateMini     = NULL;		//ooo
  m_pPixmap          = NULL;
  m_sizeThumbnails   = QSize ( 50, 50 );

  //sortChildItems (-1, false);		//oooo
  //setOpen (true);			//oooo
  setExpanded (true);			//xxx
  /*setDragEnabled (true);
  setDropEnabled (true);*/		//oooo
  
  if ( ! pEntry )
    return;
  
  setText(0, pEntry->qsDisplayName);			//xxx
  setBackground(0, QBrush(m_backgroundColor) );		//xxx
  m_sizeThumbnails = pEntry->sizeThumbnail;

  for(t=pEntry->listFileInfos.count()-1;t>=0;t--)	{
    pInfo = pEntry->listFileInfos[t];
    if ( ! pInfo )
      continue;
    if ( pInfo->pTranscodeInterface )
      theColor = QColor ( COLOR_MOVIE_TRANSCODE );
    else
      theColor = theBaseColor;
    fileInfo.setFile ( pInfo->qsFileName );
    //setText(0, QString("%1\t %2").arg(pInfo->qsVideoFormat).arg(pEntry->qsDisplayName));	//xxx
    //int iLength = theUtils.getMsFromString ( QString("%1").arg(fileInfo.size()) );		//xxx
    //pInfo->qsLength = QString("%1").arg(fileInfo.size());					//xxx
    //pInfo->qsLength = theUtils.getStringFromMs ( iLength );					//xxx
    //pInfo->qsLength = theUtils.getMsFromString ( QString("%1").arg(fileInfo.size()) );	//xxx
    //pInfo->qsLength = theUtils.getMsFromString ( pInfo->qsFileName );				//xxx

    QString qsInfo (fileInfo.fileName () + QString ("\n") + pInfo->qsLength);			//ooo
    //QString qsInfo (fileInfo.fileName () + QString ("\n") + QString ("My Length") );		//xxx
    //QString qsInfo (fileInfo.fileName () + QString ("\n") + pInfo->qsLength);			//xxx
    ListViewItemMedia *pItem = new ListViewItemMedia ( this, pInfo, theColor, qsInfo, m_sizeThumbnails );
    //pItem->setOpen (true);		//ooo
    //pItem->setDragEnabled (true);	//ooo
    //pItem->setDropEnabled (true);	//ooo
    if (pInfo->pPreview)	{
      QPixmap thePixmap;
      //QImage scaledImage (pInfo->pPreview->smoothScale (pEntry->sizeThumbnail, Qt::KeepAspectRatio));				//ooo
      QImage scaledImage (pInfo->pPreview->scaled (pEntry->sizeThumbnail, Qt::KeepAspectRatio, Qt::SmoothTransformation));	//xxx
      if (pEntry->bSoundSource)	// No buttons for a sound - SourceFileEntry ...		//oooo
        //thePixmap = QPixmap (scaledImage);		//ooo
	    thePixmap = QPixmap::fromImage(scaledImage);	//xxx
      else
        thePixmap = pItem->attachButtons (scaledImage, pEntry->sizeThumbnail);	//oooo
      //pItem->setPixmap (0, thePixmap);			//oooo
      //pItem->setIcon (0, QIcon(thePixmap));				//xxx
        pItem->setIcon (0, thePixmap);				//xxx
      //pItem->setText (0, pInfo->qsFileName);			//xxx
      pItem->setText (0, qsInfo);				//xxx
      //pItem->setBackground(0, QBrush(m_backgroundColor) );	//xxx
      //pItem->setBackground(0, m_backgroundColor);			//xxx
      //pItem->setForeground(0, m_backgroundColor);			//xxxx
    }
    //pItem->setDragEnabled (true);	//oooo
  }
}

void ListViewItemMedia::updateItem ()
{
  SourceFileInfo *pInfo = sourceFileInfo ( );
  if ( ! pInfo )
    return;

  m_backgroundColor   = QColor (COLOR_MOVIE_FILES_OK);
  if ( pInfo->qsStatus != QString ("Ok") )
    m_backgroundColor = QColor (COLOR_MOVIE_FILES_BAD);
  if ( pInfo->pTranscodeInterface )
    m_backgroundColor = QColor (COLOR_MOVIE_TRANSCODE);

  QFileInfo fileInfo ( pInfo->qsFileName );
  QString qsInfo ( fileInfo.fileName () + QString ("\n") + pInfo->qsLength );						//ooo
  //QString qsInfo ( fileInfo.fileName () + QString ("\n") + QString ("Updated Length" ) /*pInfo->qsLength*/ );		//xxx
  setText ( 1, qsInfo);
  setBackground(0, QBrush(m_backgroundColor) );		//xxx
}

void ListViewItemMedia::replaceInfo (SourceFileInfo *pInfo)
{
  SourceFileEntry *pEntry = NULL;
  if (sourceFileInfo() == pInfo)	{   
    pEntry = ((ListViewItemMedia *)parent())->sourceFileEntry();
    if (!pEntry)
      return;

    //QImage scaledImage ( pInfo->pPreview->smoothScale (pEntry->sizeThumbnail, Qt::KeepAspectRatio ) );			//ooo
    QImage scaledImage ( pInfo->pPreview->scaled (pEntry->sizeThumbnail, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );	//xxx
    QFileInfo fileInfo ( pInfo->qsFileName );
    QString   qsInfo   ( fileInfo.fileName () + QString ("\n") + pInfo->qsLength );						//ooo
    //QString   qsInfo   ( fileInfo.fileName () + QString ("\n") + QString ("Replaced Length") /*pInfo->qsLength*/ );		//xxx
  
    //setText (1, qsInfo);	//ooo
    setText (0, qsInfo);	//xxx
    m_backgroundColor   = QColor (COLOR_MOVIE_FILES_OK);
    if (pInfo->qsStatus != QString ("Ok"))
      m_backgroundColor = QColor (COLOR_MOVIE_FILES_BAD);
    if ( pInfo->pTranscodeInterface )
      m_backgroundColor = QColor (COLOR_MOVIE_TRANSCODE);

    // attach the buttons and force a repaint of the buttons ...
    m_iPreviousButtonState = StateUndef;
    QPixmap thePixmap = attachButtons (scaledImage, pEntry->sizeThumbnail);
    
    //QTreeWidgetItem::setIcon(0,thePixmap);   //xxxx
    setIcon(0,thePixmap);   //xxxx
    //setBackground(0, m_backgroundColor);	//xxxx
  }
  else if (sourceFileEntry ())	{ 
    pEntry = sourceFileEntry();
    //ListViewItemMedia *pChild = (ListViewItemMedia *)firstChild ();		//ooo
    ListViewItemMedia *pChild = (ListViewItemMedia *)child (0);			//xxx
    while (pChild)	{
      pChild->replaceInfo (pInfo);
      //pChild = (ListViewItemMedia *)pChild->nextSibling ();						//ooo
      pChild = (ListViewItemMedia *)pChild->parent()->child(pChild->parent()->indexOfChild(pChild)+1);	//xxx
    }
    // Here we check for the color of the header ... only if all status's are Ok do we give
    // the Ok color to the header too .
    m_backgroundColor     = QColor (COLOR_MOVIE_FILES_OK);
    //setBackground(0, m_backgroundColor);	//xxxx
    //if ( !pEntry->listFileInfos.isEmpty() ) {			//xxx
    //if ( pEntry->listFileInfos.count () > 0 ) {		//xxx
    //for (uint t=0; t<(uint)pEntry->listFileInfos.count ();t++)	{	//ox
    for (uint t=0; t<(uint)pEntry->listFileInfos.count ();t++)	{		//xxx
      if (pEntry->listFileInfos[t]->qsStatus != QString ("Ok"))	{
	    m_backgroundColor = QColor (COLOR_MOVIE_FILES_BAD);
        //setBackground(0, m_backgroundColor);	//xxxx
	    break;
      }
    }
    //}	//if count () > 0
    //}	//if !isEmpty ()
  }
  setBackground(0, m_backgroundColor);	//xxxx
}

void ListViewItemMedia::attachButtons ( QImage *pSourceImage, QPixmap *pTargetPix )
{
  QImage tempImage;
  //tempImage  = pSourceImage->smoothScale ( m_sizeThumbnails, Qt::KeepAspectRatio );				//ooo
  tempImage  = pSourceImage->scaled ( m_sizeThumbnails, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
  *pTargetPix = attachButtons ( tempImage,  m_sizeThumbnails );
}

QPixmap &ListViewItemMedia::attachButtons ( QImage &theImage, QSize sizeThumbnail )
{
  static QPixmap thePixmap;
  // This function will attach the buttons to the left side of the image.
  // Play / Stop / Back
  int iThirdHeight = (int)( (float)sizeThumbnail.height ( ) / 3.0 );		//ooo
  //int iThirdHeight = (int)( (float)sizeThumbnail.height ( ) * 2.0 );		//xxx
  int iOffset = iThirdHeight;
  int iDeltaY = (int)( ( sizeThumbnail.height ( ) - theImage.height ( ) ) / 2.0 );	//ooo
  //int iDeltaY = (int)( ( sizeThumbnail.height ( ) - theImage.height ( ) ) * 2.0 );	//xxx
  if ( ( iDeltaY < 1 ) || ( iDeltaY > sizeThumbnail.height ( ) ) )
         iDeltaY = 0;

  // Here we create the final imageExtension (empty QImage)
  //QImage finalImage ( sizeThumbnail.width ( ) + iOffset, sizeThumbnail.height ( ), theImage.depth ( ) );	//ooo
  QImage finalImage ( sizeThumbnail.width ( ) + iOffset, sizeThumbnail.height ( ), QImage::Format_RGB32 );	//xxx
  finalImage.fill ( 0 ); // fill all black 
  // Next we fill in the image
  //bitBlt ( &finalImage, iOffset, iDeltaY, (const QImage *)&theImage, 0, 0, theImage.width(), theImage.height(), Qt::CopyROP);		//ooo
  //bitBlt ( &finalImage, iOffset, iDeltaY, (const QImage *)&theImage, 0, 0, theImage.width(), theImage.height());			//xxx
  QPainter p( &finalImage );														//xxx
  p.drawImage( iOffset, iDeltaY, theImage, 0, 0, theImage.width(), theImage.height() );							//xxx
  
  thePixmap.convertFromImage ( finalImage );
  // And then we draw the Buttons ...
  drawButtonState ( &thePixmap );
  return thePixmap;
}

void ListViewItemMedia::drawButtonState (QPixmap *pPixmap)
{
	if ( ! pPixmap )
		return;

	// This function will draw the buttons over the passed Pixmap
	uint t;
	bool bSelected;
	QPixmap theButton;
	QImage theImage;
	//char arrayNormalButtonState [][20] = {"button_play.png",  "button_stop.png",  "button_back.png"};					//ooo
	char arrayNormalButtonState [][30] = {":/images/button_play.png",  ":/images/button_stop.png",  ":/images/button_back.png"};		//xxx
	//char arrayPressedButtonState[][20] = {"button_cplay.png", "button_cstop.png", "button_cback.png"};					//ooo
	char arrayPressedButtonState[][30] = {":/images/button_cplay.png", ":/images/button_cstop.png", ":/images/button_cback.png"};		//xxx
	int iThirdHeight = (int)((float)pPixmap->height() / 3.0);
	int iOffset = iThirdHeight;

	for (t=0;t<3;t++)	{
		bSelected = false;
		if ((t == 0) && (m_iButtonState == StatePlay) )
			bSelected = true;
		else if ((t == 1) && (m_iButtonState == StateStop) )
			bSelected = true;
		else if ((t == 2) && (m_iButtonState == StateBack) )
			bSelected = true;

		if (bSelected)
			//theImage = QImage().fromMimeSource(arrayPressedButtonState[t]).smoothScale (iOffset, iThirdHeight);	//ooo
			theImage = QImage( arrayPressedButtonState[t] ).scaled ( iOffset, iThirdHeight );			//xxx
		else
			//theImage = QImage().fromMimeSource(arrayNormalButtonState[t]).smoothScale(iOffset, iThirdHeight);	//ooo
			theImage = QImage( arrayNormalButtonState[t] ).scaled ( iOffset, iThirdHeight );			//xxx
		//theButton = theImage;				//ooo
		theButton = QPixmap::fromImage(theImage);	//xxx
		//bitBlt (pPixmap, 0, t*iThirdHeight, &theButton, 0, 0, theButton.width(), theButton.height(),  Qt::CopyROP);	//ooo
		//bitBlt (pPixmap, 0, t*iThirdHeight, &theButton, 0, 0, theButton.width(), theButton.height());			//xxx
		QPainter p( pPixmap );												//xxx
		p.drawPixmap( 0, (int) t*iThirdHeight, theButton, 0, 0, theButton.width(), theButton.height() );		//xxx
	}
	//setPixmap (0, *pPixmap);		//ooo
	//setIcon (0, QIcon(*pPixmap));		//xxx
	setIcon (0, *pPixmap);		//xxx
	m_iPreviousButtonState = m_iButtonState;
}

QColor ListViewItemMedia::backgroundColor ()
{
	return m_backgroundColor;

}

SourceFileEntry *ListViewItemMedia::sourceFileEntry ()
{
	return m_pSourceFileEntry;
}

SourceFileInfo *ListViewItemMedia::sourceFileInfo ()
{
	return m_pSourceFileInfo;
}

void ListViewItemMedia::miniButton ( int iState, QString &qsFileName )
{
  if ( ! m_pAnimateMini ) {
    Cache::Thumbs::Entry *pEntry = Global::pThumbsCache->find ( qsFileName );
    if ( pEntry ) {
      m_pAnimateMini = new AnimateMini ( this );
      m_pAnimateMini->initMe ( pEntry );
    }
    else // No cache object available yet ...
      return;
  }
  else
    m_pAnimateMini->refreshButtonState ( );

  if ( iState  == StateStop )
    m_pAnimateMini->stop  ( );
  else if ( iState == StatePlay )
    m_pAnimateMini->start ( );
  else if ( iState == StateBack )
    m_pAnimateMini->info  ( );
}

void ListViewItemMedia::miniButton ( int iState, CXmlSlideshow *pSlideshow )
{
  if ( ! m_pAnimateMini )  {
    if ( pSlideshow )  {
      m_pAnimateMini = new AnimateMini ( this );
      m_pAnimateMini->initMe ( pSlideshow );
    }
    else // No Slideshow ... etrange ...
      return;
  }
  else
    m_pAnimateMini->refreshButtonState ( );

  if ( iState  == StateStop )
    m_pAnimateMini->stop  ( );
  else if ( iState == StatePlay )
    m_pAnimateMini->start ( );
  else if ( iState == StateBack )
    m_pAnimateMini->info  ( );
}

void ListViewItemMedia::setProgress ( float fProgress )
{  
  ListViewItemMedia *pItem;

  // Okay found the SourceFileInfo now we need the ListViewItemMedia - object for this Info.
  //pItem = (ListViewItemMedia *)firstChild ( );    //ooo
  pItem = (ListViewItemMedia *)child ( 0 ); //xxx
  //pItem = (ListViewItemMedia *)treeWidget()->topLevelItem (0)->child ( 0 ); //xxx
  //pItem = (ListViewItemMedia *)treeWidget()->topLevelItem (0); //xxx
  
  while ( pItem )  {
    pItem->setProgress ( fProgress );
    //pItem = (ListViewItemMedia *)pItem->nextSibling();    //ooo
    pItem = (ListViewItemMedia *)pItem->treeWidget()->itemBelow(pItem);				//xxx
    //pItem = (ListViewItemMedia *)pItem->parent()->child(pItem->parent()->indexOfChild(pItem)+1);				//xxx
  }

  if ( m_pAnimateMini ) {     
       m_pAnimateMini->setProgress ( fProgress );
  //else if ( pixmap ( 0 ) )  { //ooo
  } else if ( child ( 0 ) )  { //xxx    
       if ( ! m_pPixmap  )  // The first time around we'll have to create the temp pixmap
              //m_pPixmap = new QPixmap ( *pixmap ( 0 ) );  //ooo
              //m_pPixmap = new QPixmap ( *icon ( 0 ).pixmap(QSize(32,32)) );  //xxx
              m_pPixmap = new QPixmap ( child(0)->icon ( 0 ).pixmap( QSize(150,150)) );  //xxx
       QPixmap thePixmap = *m_pPixmap;
       renderProgress  (   &thePixmap, fProgress );
       //setPixmap       ( 0, thePixmap );  //ooo
       //setIcon       ( 0, thePixmap );  //xxx
       child(0)->setIcon       ( 0, thePixmap );  //xxx
       //child(0)->setIcon       ( 0, *m_pPixmap );  //xxx
       //child(0)->setIcon       ( 0, QIcon::fromTheme ( "user-home" ) );  //xxx
  }   //oooo

  if ( ( fProgress == -1.0 ) && ( m_pPixmap ) )  {
    delete m_pPixmap;
    m_pPixmap = NULL;
  }
}

void ListViewItemMedia::renderProgress ( QPixmap *pPixmap, float fProgress )
{  
  if ( ( ! pPixmap ) || ( fProgress == -1.0 ) )
    return;

  if ( fProgress > 100.0f )
       fProgress = 100.0f;
  if ( fProgress <   0.0f )
       fProgress =   0.0f;

  //int iXOffset = (int)( (float)pPixmap->height ( ) / 3.0 ) + 5;   //ooo
  int iXOffset = static_cast<int>( static_cast<float>(pPixmap->height ( )) / 3.0 ) + 5; //xxx
  //int iYOffset = (int)(1.5 * iXOffset);   //ooo
  int iYOffset = static_cast<int>(1.5 * iXOffset);  //xxx

  //QFont theFont ( "Courier", (int)(iXOffset / 2.0), QFont::Bold ); // iXOffset == FontSize was 12 //ooo
  QFont theFont ( "Courier", static_cast<int>(iXOffset / 2.0), QFont::Bold ); // iXOffset == FontSize was 12   //xxx
  //QString   qsProgress = QString ( "%1%" ).arg ( (int)fProgress );    //ooo
  QString   qsProgress = QString ( "%1%" ).arg ( static_cast<int>(fProgress) ); //xxx
  QPainter  thePainter ( pPixmap );   //ooo
  //QPainter  thePainter;     //xxx
  //thePainter.begin(pPixmap);    //xxx
  thePainter.setPen    ( QColor ( 255, 255, 100 ) );
  thePainter.setBrush  ( Qt::Dense4Pattern );
  thePainter.setFont   ( theFont );
  //thePainter.begin(pPixmap);    //xxx
  thePainter.drawRect  ( iXOffset-5, 0, pPixmap->width ( )-iXOffset+5, pPixmap->height ( ) );
  thePainter.drawText  ( iXOffset, iYOffset, qsProgress );
  //thePainter.end();    //xxx
}

AnimateMini::AnimateMini ( ListViewItemMedia *pItem )
{
  m_pItem          = pItem;
  m_arrayPix       = NULL;
  m_iCurrentPix    = 0;
  m_iNumberOfPix   = 0;
  m_fProgress      = -1.0f;
  m_pTimerThumbing = new QTimer ( this );
  connect ( m_pTimerThumbing, SIGNAL( timeout ( ) ), this, SLOT ( slotAnimate ( ) ) );	//oooo
}

AnimateMini::~AnimateMini ( )
{
  stop  ( );
  clear ( );
  delete m_pTimerThumbing;	//oooo
}

void AnimateMini::slotAnimate ( )
{
  if ( m_arrayPix ) {
    m_iCurrentPix ++;
    if ( m_iCurrentPix >= m_iNumberOfPix )
         m_iCurrentPix  = 0;
    if ( m_fProgress == -1.0 )
         //m_pItem->setPixmap ( 0, *m_arrayPix[m_iCurrentPix] );    //ooo
         m_pItem->setIcon ( 0, *m_arrayPix[m_iCurrentPix] );        //xxx
    else
         m_pItem->setProgress ( m_fProgress );
  }
}

void AnimateMini::setProgress ( float fProgress )
{
  m_fProgress = fProgress;
  QPixmap thePixmap = *m_arrayPix[m_iCurrentPix];

  m_pItem->renderProgress ( &thePixmap, fProgress );

  //m_pItem->setPixmap      ( 0, thePixmap );   //ooo
  m_pItem->setIcon      ( 0, thePixmap );   //xxx
//printf ( "AnimateMini::setProgress <%f>\n", fProgress );
}

void AnimateMini::reset ( )
{
  m_iCurrentPix = 0;
  //m_pItem->setPixmap ( 0, m_pixmapOrig ); //ooo
  m_pItem->setIcon ( 0, m_pixmapOrig );     //xxx
}

void AnimateMini::clear ( )
{
  int t;
  if ( m_arrayPix ) {
    for ( t=0; t<m_iNumberOfPix; t++ )
      delete m_arrayPix[t];
    delete []m_arrayPix;
  }
}

void AnimateMini::initMe ( Cache::Thumbs::Entry *pCache )
{
  uint t;
  QImage tempImage;
  //const QPixmap *pPixmap = m_pItem->pixmap ( 0 ); //ooo
  QPixmap *pPixmap = new QPixmap(m_pItem->icon ( 0 ).pixmap(QSize(22,22)));     //xxx
  if ( pPixmap )
    m_pixmapOrig = *pPixmap;
  else {
    //QImage error = QImage::fromMimeSource ( "error.jpg" );				//ooo
    QImage error = QImage ( ":/images/error.jpg" );					//xxx
    //error = error.smoothScale     ( 45,45 );						//ooo
    error = error.scaled ( 45,45, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    m_pixmapOrig.convertFromImage ( error );
    pPixmap = &m_pixmapOrig;
  }

  stop  ( );
  clear ( );

  if ( ! pCache->arrayOfThumbs )
         pCache->loadImages  ( );

  m_iNumberOfPix = pCache->iNumberOfThumbs;
  m_arrayPix = new QPixmap *[m_iNumberOfPix];
  for ( t=0; t<(uint)m_iNumberOfPix; t++ ) {
     m_arrayPix[t] = new QPixmap;
     m_pItem->attachButtons ( pCache->arrayOfThumbs[t], m_arrayPix[t] );
  }
}

void AnimateMini::initMe ( CXmlSlideshow *pSlideshow )
{
  uint t, iMaxNrImages, iImgNr;
  QImage tempImage;
  CXmlSlideshow::img_struct *pXmlImage;
  //const QPixmap *pPixmap = m_pItem->pixmap ( 0 ); //ooo
  QPixmap *pPixmap = new QPixmap(m_pItem->icon ( 0 ).pixmap(QSize(22,22)));     //xxx

  if ( pPixmap )
    m_pixmapOrig = *pPixmap;
  else {
    //QImage error = QImage::fromMimeSource ( "error.jpg" );				//ooo
    QImage error = QImage ( ":/images/error.jpg" );					//xxx
    //error = error.smoothScale     ( 45,45 );						//ooo
    error = error.scaled ( 45,45, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );	//xxx
    m_pixmapOrig.convertFromImage ( error );
    pPixmap = &m_pixmapOrig;
  }

  stop  ( );
  clear ( );

  if ( pSlideshow )  {
    iMaxNrImages = pSlideshow->countImg ( );
    m_iNumberOfPix = ( iMaxNrImages < 10 ) ? iMaxNrImages : 10;
    m_arrayPix = new QPixmap *[m_iNumberOfPix];
    for ( t=0; t<(uint)m_iNumberOfPix; t++ ) {
       m_arrayPix[t] = new QPixmap;
       iImgNr = (uint)( (float)iMaxNrImages / (float)m_iNumberOfPix * (float)t );
       pXmlImage = pSlideshow->getImg ( iImgNr );
       if ( pXmlImage )
         tempImage.load ( pXmlImage->src );
       else
         //tempImage = QImage::fromMimeSource ( "error.jpg" );	//ooo
         tempImage = QImage ( ":/images/error.jpg" );		//xxx
       //tempImage = tempImage.smoothScale ( 45, 45, Qt::KeepAspectRatio );			//ooo
       tempImage = tempImage.scaled ( 45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation );	//xxx
       m_pItem->attachButtons ( &tempImage, m_arrayPix[t] );
    }
  }
}

void AnimateMini::refreshButtonState ( )
{
  int t;
  for ( t=0; t<m_iNumberOfPix; t++ )
    m_pItem->drawButtonState ( m_arrayPix[t] );
}

void AnimateMini::start ( )
{
  if ( ! m_pTimerThumbing->isActive ( ) )
         m_pTimerThumbing->start ( 1500 );
}

void AnimateMini::stop ( )
{
  if ( m_pTimerThumbing->isActive ( ) )
       m_pTimerThumbing->stop     ( );
  reset ( );
}

//#include "render_client.h"
void AnimateMini::info ( )
{
  if ( m_pTimerThumbing->isActive ( ) )
       m_pTimerThumbing->stop     ( );

  SourceFileInfo *pInfo = m_pItem->sourceFileInfo ( );
  if ( ! pInfo ) {
    SourceFileEntry *pEntry =  m_pItem->sourceFileEntry ( );
    if ( ( ! pEntry )  || ( pEntry->listFileInfos.count ( ) < 1 ) )
      return;

    pInfo = pEntry->listFileInfos[0];
  }

  // Check if the SourceFileEntry is in the process of being rendered ...
  SourceFileEntry *pEntry =  m_pItem->sourceFileEntry ( );
  if ( ! pEntry )  {
    ListViewItemMedia *pParent = (ListViewItemMedia *)m_pItem->parent ( );
    if ( pParent && pParent->sourceFileEntry ( ) )
      pEntry = pParent->sourceFileEntry ( );
    else 
      pEntry = Global::pApp->sourceFileEntry ( 0 );
  }
  if ( pEntry && pEntry->bIsSlideshow )
    return;
//    bool bRet = Render::Manager::registerEntry ( pEntry );
//printf ( "AnimateMini::info registerEntry <%p> <%s>\n", pEntry, bRet ? "true" : "false" );
//    return;

  Global::pApp->editSourceFileInfo ( pInfo );
}
