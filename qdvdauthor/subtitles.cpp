/***************************************************************************
    subtitles.cpp
                             -------------------
    Subtitles - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class encapsulates the attributes for the subtitle generation.
    
****************************************************************************/

//#include <qdesktopwidget.h>	//ooo
#include <QDesktopWidget>	//xxx
//#include <q3progressbar.h>	//ooo
#include <QProgressBar>		//xxx
//#include <qapplication.h>	//ooo
#include <QApplication>		//xxx
//#include <qdatetime.h>	//ooo
#include <QDateTime>		//xxx
//#include <q3textedit.h>	//ooo
#include <QTextEdit>		//xxx
//#include <qfileinfo.h>	//ooo
#include <QFileInfo>		//xxx
//#include <qpainter.h>		//ooo
#include <QPainter>		//xxx
//#include <qdialog.h>		//ooo
#include <QDialog>		//xxx
//#include <qlayout.h>		//ooo
#include <QLayout>		//xxx
//#include <qimage.h>		//ooo
#include <QImage>		//xxx
//#include <qdom.h>		//ooo
#include <QDomElement>		//xxx
//#include <qdir.h>		//ooo
#include <QDir>			//xxx
//Added by qt3to4:
//#include <Q3GridLayout>	//ooo
#include <QGridLayout>		//xxx
#include <QPixmap>

#include <QMessageBox>		//xxx

#include "global.h"
#include "utils.h"
#include "xml_dvd.h"
#include "sourcefileentry.h"
#include "subtitles.h"
#include "qplayer/mediacreator.h"

SubtitleEntry::SubtitleEntry ( )
{
  m_iSubtitleNumber = -1;
}

SubtitleEntry &SubtitleEntry::operator= ( SubtitleEntry &theOther )
{
  m_iSubtitleNumber = theOther.m_iSubtitleNumber;
  m_qsIso639        = theOther.m_qsIso639;
  m_qsFileName      = theOther.m_qsFileName;
  return *this;
}

Subtitles::entry::entry ()
{
  iIndex = 0;
  iTimeStart = -1;
  iTimeStop  = -1;
}

Subtitles::entry &Subtitles::entry::operator = ( Subtitles::entry &theOther )
{
  iIndex      = theOther.iIndex;
  qsTimeStart = theOther.qsTimeStart;
  qsTimeStop  = theOther.qsTimeStop;
  iTimeStart  = theOther.iTimeStart;
  iTimeStop   = theOther.iTimeStop;
  qsText      = theOther.qsText;
  return *this;
}

Subtitles::Subtitles (  int iWidth, int iHeight )
{
  Utils theUtils;
  if ( theUtils.getFormat ( iWidth, iHeight ) == FORMAT_NONE )  {
    iWidth  = 720; // standard NTSC res.
    iHeight = 480; // standard NTSC res.
  }
  m_videoSize = QSize ( iWidth, iHeight );

  m_subColors[0]    = Rgba ( TRANSPARENT_COLOR       );
  m_subColors[1]    = Rgba ( START_HIGHLIGHTED_COLOR );
  m_subColors[2]    = Rgba ( START_SELECTED_COLOR    );
//m_subColors[3]    = Rgba ( START_FRAME_COLOR       );

  m_iSubtitleNumber = 0;
  m_bFit            = true;
  m_iOutline        = 2; // Default to outline with a shadow of 2 pixels...
  m_bTextSubtitles  = false; // default to image based subtitles (render subtitles in this class)
  m_iTextFontSize   = 24;
  m_qsTextFont      = "Vera";
  m_alignment       = QPoint ( 1, 4 ); // HAlign=Center / VAlign=top
  m_subtitleState   = STATE_MANUAL;
  m_rect            = QRect ( 0, 0, iWidth, iHeight );
}

Subtitles::~Subtitles ()
{
  for (uint t=0;t<(uint)m_listOfSubtitles.count(); t++)		//oxx
    delete m_listOfSubtitles[t];
  m_listOfSubtitles.clear ();
}

Subtitles &Subtitles::operator = ( Subtitles &theOther )
{
  uint t;
  m_bTextSubtitles  = theOther.m_bTextSubtitles;
  m_qsTextFont      = theOther.m_qsTextFont;
  m_iTextFontSize   = theOther.m_iTextFontSize;
  m_bFit            = theOther.m_bFit;
  m_iOutline        = theOther.m_iOutline;
  m_font            = theOther.m_font;
  m_videoSize       = theOther.m_videoSize;
  m_rect            = theOther.m_rect;
  m_alignment       = theOther.m_alignment;
  m_iSubtitleNumber = theOther.m_iSubtitleNumber;
  m_qsIso639        = theOther.m_qsIso639;
  m_qsFileName      = theOther.m_qsFileName;
  m_qsXmlFile       = theOther.m_qsXmlFile;
  m_subtitleState   = theOther.m_subtitleState;

  for ( t=0;t<MAX_SUBTITLE_COLORS;t++)
    m_subColors[t] = theOther.m_subColors[t];

  if ( m_listOfSubtitles.count () ) {
    for ( t=0;t<(uint)theOther.m_listOfSubtitles.count();t++) 	//oxx
      delete m_listOfSubtitles[t];
  }
  m_listOfSubtitles.clear ();

  for ( t=0;t<(uint)theOther.m_listOfSubtitles.count();t++) {	//oxx
    entry *pNewEntry = new entry;
    *pNewEntry = *theOther.m_listOfSubtitles[t];
    m_listOfSubtitles.append ( pNewEntry );
  }

  return *this;
}

bool Subtitles::readProjectFile ( QDomNode &xmlNode )
{ 
  int t;
  QString     qsAttribute, infoName, infoText;
  QDomNode    infoNode;
  QDomElement infoElement, theElement = xmlNode.toElement();
  infoName  = theElement.tagName();

  qsAttribute = theElement.attributeNode ( SUBTITLES_NUMBER ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    m_iSubtitleNumber = qsAttribute.toInt ();
  qsAttribute = theElement.attributeNode ( SUBTITLES_LANG ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    m_qsIso639 = qsAttribute;
  qsAttribute = theElement.attributeNode ( SUBTITLES_FILE_NAME ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    m_qsFileName = qsAttribute;
  qsAttribute = theElement.attributeNode ( SUBTITLES_TEXT_BASED ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    m_bTextSubtitles = ( qsAttribute == "true" );
  qsAttribute = theElement.attributeNode ( SUBTITLES_STATE ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    m_subtitleState = (enState)qsAttribute.toInt ( );
  qsAttribute = theElement.attributeNode ( SUBTITLES_TEXT_FONT ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    m_qsTextFont = qsAttribute;
  qsAttribute = theElement.attributeNode ( SUBTITLES_TEXT_SIZE ).value ( );
  if ( ! qsAttribute.isEmpty ( ) )
    m_iTextFontSize = qsAttribute.toInt ( );
  qsAttribute = theElement.attributeNode ( SUBTITLES_FIT ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    m_bFit = ( qsAttribute == "true" );
  qsAttribute = theElement.attributeNode ( SUBTITLES_OUTLINE ).value();
  m_iOutline = -1;
  if ( ! qsAttribute.isEmpty ( ) )
    m_iOutline = qsAttribute.toInt ( );
  qsAttribute = theElement.attributeNode ( SUBTITLES_SUBTITLE_FILE ).value();
  if ( ! qsAttribute.isEmpty ( ) )
    m_qsXmlFile = qsAttribute;
  qsAttribute = theElement.attributeNode ( SUBTITLES_FONT ).value();
  if ( ! qsAttribute.isEmpty ( ) ) {
    QFont theFont;
    if (theFont.fromString ( qsAttribute ))
      m_font = theFont;
  }
  qsAttribute = theElement.attributeNode ( SUBTITLES_RECT ).value();
  if ( ! qsAttribute.isEmpty ( ) ) {
    // QString "x,y,width,height" 
    //QStringList list = QStringList::split ( ",", qsAttribute );	//ooo
    QStringList list = qsAttribute.split ( "," );			//xxx
    QRect theRect;
    if (list.count() == 4)
      theRect = QRect(list[0].toInt(), list[1].toInt(),  list[2].toInt(),  list[3].toInt() );
    m_rect = theRect;
  }
  qsAttribute = theElement.attributeNode ( SUBTITLES_VIDEO_SIZE ).value();
  if ( ! qsAttribute.isEmpty ( ) ) {
    // QString "x,y,width,height" 
    //QStringList list = QStringList::split ( ",", qsAttribute );	//ooo
    QStringList list =  qsAttribute.split ( "," );			//xxx
    QSize theSize;
    if ( list.count ( ) == 2)
      theSize = QSize ( list[0].toInt ( ), list[1].toInt ( ) );
    m_videoSize = theSize;
  }
  qsAttribute = theElement.attributeNode ( SUBTITLES_ALIGNMENT ).value();
  if ( ! qsAttribute.isEmpty ( ) ) {
    // QString "x,y,width,height" 
    //QStringList list = QStringList::split ( ",", qsAttribute );	//ooo
    QStringList list = qsAttribute.split ( "," );			//xxx
    QPoint theAlignment;
    if (list.count() == 2)
      theAlignment = QPoint( list[0].toInt(), list[1].toInt() );
    m_alignment = theAlignment;
  }
  qsAttribute = theElement.attributeNode ( SUBTITLES_COLORS ) .value ();
  if ( ! qsAttribute.isEmpty ( ) ) {
    //QStringList list = QStringList::split ( ",", qsAttribute );	//ooo
    QStringList list = qsAttribute.split ( "," );			//xxx
    if ( list.count() == MAX_SUBTITLE_COLORS ) {
      for ( t=0;t<MAX_SUBTITLE_COLORS; t++ )
        m_subColors[t].fromString ( list[t] );
    }
  }
  qsAttribute = theElement.attributeNode ( SUBTITLES_TRANSPARENCY ) .value ();
  if ( ! qsAttribute.isEmpty ( ) ) {
    // old format when color / transparency was separate.
    //QStringList list = QStringList::split ( ",", qsAttribute );	//ooo
    QStringList list = qsAttribute.split ( "," );			//xxx
    if ( list.count ( ) == MAX_SUBTITLE_COLORS ) {
      for ( t=0;t<MAX_SUBTITLE_COLORS; t++ )  
	m_subColors[t].setAlpha ( list[t].toInt ( ) );
    }
  }

  infoNode = theElement.firstChild ();
  while (!infoNode.isNull())	{
    infoElement = infoNode.toElement();
    // Okay, here we read the stored data from the xml file.
    infoName = infoElement.tagName();
    infoText = infoElement.text ();
    if (SUBTITLES_SUBTITLE_FILE == infoName)	{
      m_qsTempFile = infoText;
    }
    else if (SUBTITLES_SUBTITLE_ENTRY == infoName)	{
      Subtitles::entry *pNewEntry = new entry;
      qsAttribute = infoElement.attributeNode ( SUBTITLES_SUBTITLE_INDEX ).value();
      if (!qsAttribute.isEmpty())
	pNewEntry->iIndex = qsAttribute.toInt ();
      qsAttribute = infoElement.attributeNode ( SUBTITLES_SUBTITLE_START ).value();
      if (!qsAttribute.isEmpty())
	pNewEntry->qsTimeStart = qsAttribute;
      qsAttribute = infoElement.attributeNode ( SUBTITLES_SUBTITLE_STOP ).value();
      if (!qsAttribute.isEmpty())
	pNewEntry->qsTimeStop = qsAttribute;
      pNewEntry->iTimeStart = getTimeFromString ( pNewEntry->qsTimeStart );
      pNewEntry->iTimeStop  = getTimeFromString ( pNewEntry->qsTimeStop  );

      pNewEntry->qsText = infoText;

      m_listOfSubtitles.append ( pNewEntry );
    }
    infoNode = infoNode.nextSibling ();
  }

  return true;
}

bool Subtitles::writeProjectFile ( QDomElement &inElement )
{ 
  uint t;
  QDomDocument xmlDoc = inElement.ownerDocument();
  QDomElement  infoElement, tmpFileElement, theElement;
  QDomText     text;  
  entry       *pEntry;
  QString      qsColors, qsTransparency;

  if ( m_listOfSubtitles.count ( ) < 1 )
    return true;

  theElement = xmlDoc.createElement ( SOURCE_OBJECT_SUBTITLES );

  // Here we set the attributes of the <dvdauthor> tag
  //  theElement.setAttribute( SUBTITLES_TRANSPARENCY,  QString ("%1").arg ( (int)iTransparency   ) );
  theElement.setAttribute( SUBTITLES_NUMBER,        QString ("%1").arg ( (int)m_iSubtitleNumber ) );
  theElement.setAttribute( SUBTITLES_LANG,          m_qsIso639   );
  theElement.setAttribute( SUBTITLES_FONT,          m_font.toString ( ) );
  theElement.setAttribute( SUBTITLES_STATE,         QString ("%1").arg ( (int)m_subtitleState ) );
  if ( m_qsFileName.length ( ) > 0 )
    theElement.setAttribute( SUBTITLES_FILE_NAME,   m_qsFileName );
  if ( m_bTextSubtitles ) {
    theElement.setAttribute( SUBTITLES_TEXT_BASED,  QString ("%1").arg ( m_bTextSubtitles ? "true" : "false" ) );
    theElement.setAttribute( SUBTITLES_TEXT_FONT,   m_qsTextFont );
    theElement.setAttribute( SUBTITLES_TEXT_SIZE,   QString ("%1").arg ( m_iTextFontSize ) );
  }
  theElement.setAttribute( SUBTITLES_FIT,           QString ("%1").arg ( m_bFit ? "true" : "false" ) );
  if ( m_iOutline > 0 )
    theElement.setAttribute( SUBTITLES_OUTLINE,     QString ("%1").arg ( m_iOutline ) );
  theElement.setAttribute( SUBTITLES_SUBTITLE_FILE, m_qsXmlFile );
  for ( t=0;t<MAX_SUBTITLE_COLORS; t++ ) 
    qsColors += QString ( "%1," ).arg ( m_subColors[t].toString ( ) );
  theElement.setAttribute( SUBTITLES_COLORS, qsColors );
  theElement.setAttribute( SUBTITLES_RECT, QString ("%1,%2,%3,%4").arg(m_rect.x ( ) ).arg(m_rect.y ( ) ).arg(m_rect.width ( ) ).arg(m_rect.height ( ) ) );
  theElement.setAttribute( SUBTITLES_VIDEO_SIZE, QString ("%1,%2").arg ( m_videoSize.width ( ) ).arg ( m_videoSize.height ( ) ) );
  theElement.setAttribute( SUBTITLES_ALIGNMENT, QString ("%1,%2").arg(m_alignment.x ( ) ).arg(m_alignment.y ( ) ) );

  // store the temp file name too ...
  if ( ! m_qsTempFile.isEmpty () ) {
    tmpFileElement = xmlDoc.createElement( SUBTITLES_SUBTITLE_FILE );
    text = xmlDoc.createTextNode( m_qsTempFile );
    tmpFileElement.appendChild( text );
    theElement.appendChild ( tmpFileElement );
  }

  if ( m_listOfSubtitles.count ( ) > 0)	{
    for (t=0;t<(uint)m_listOfSubtitles.count();t++)	{	//oxx
      pEntry = m_listOfSubtitles[t];
      infoElement = xmlDoc.createElement( SUBTITLES_SUBTITLE_ENTRY );
      infoElement.setAttribute( SUBTITLES_SUBTITLE_INDEX, QString ("%1").arg( pEntry->iIndex ) );
      infoElement.setAttribute( SUBTITLES_SUBTITLE_START, pEntry->qsTimeStart );
      infoElement.setAttribute( SUBTITLES_SUBTITLE_STOP,  pEntry->qsTimeStop  );

      text = xmlDoc.createTextNode( pEntry->qsText );
      infoElement.appendChild( text );

      theElement.appendChild ( infoElement );
    }
  }

  inElement.appendChild ( theElement );
  return true;
}

long Subtitles::getTimeFromString ( QString &qsTime )
{
  long iMSeconds = -1;
  QTime theTime;
  iMSeconds = (long)theTime.msecsTo ( QTime::fromString ( qsTime ) );

  return iMSeconds;
}

QString Subtitles::getStringFromTime ( long iMSTime )
{
  QString qsReturn;
  QTime theTime;
  theTime  = theTime.addMSecs ( iMSTime );
  qsReturn = theTime.toString ( "hh:mm:ss.zzz" );
  return qsReturn;
}

QString Subtitles::getXMLFileName ()
{
  QFileInfo fileInfo ( m_qsTempFile );
  QString qsReturn;

  qsReturn = m_qsXmlFile;
  if ( m_qsXmlFile.isEmpty ( ) )
    //qsReturn = QString ("%1/%2_%3.xml").arg( fileInfo.filePath () ).arg ( fileInfo.baseName ( TRUE ).arg ( m_iSubtitleNumber ) );	//ooo
    qsReturn = QString ("%1/%2_%3.xml").arg( fileInfo.filePath () ).arg ( fileInfo.baseName ( ).arg ( m_iSubtitleNumber ) );		//xxx
  return qsReturn;
}

QRect Subtitles::getSubtitlesRect ( QString &qsResolution )
{
  QRect theRect;
  Utils theUtils;
  int iVideoWidth, iVideoHeight;
  int iFontSize  = m_font.pointSize ();
  if ( iFontSize == -1 ) {
       iFontSize = m_font.pixelSize ();
       if ( iFontSize == -1 )
            iFontSize =  24;
  }
  iFontSize *= 3;

  iVideoWidth  = theUtils.getWHFromResolution ( qsResolution, true  );
  iVideoHeight = theUtils.getWHFromResolution ( qsResolution, false );
  if ( iVideoWidth  < 350 ) // out of spec ?
       iVideoWidth  = 720;
  if ( iVideoHeight < 240 ) // out of spec ?
       iVideoHeight = 480;

  theRect = m_rect;
  // Try to figure out if the user was too lazy to input values ...
  if ( ( m_rect.x() == 0 ) && ( m_rect.y() == 0 )  ) {
    if ( ( ( m_rect.width  ( ) == iVideoWidth   ) &&
           ( m_rect.height ( ) == iVideoHeight  )  ) ||   // rect == video's rect
         ( ( m_rect.width  ( ) == 0             ) &&
           ( m_rect.height ( ) == 0             )  )  ) { // or rect is empty
      theRect = QRect ( 0, iVideoHeight - 45 - iFontSize, iVideoWidth, iFontSize );
    }
  }
  else { // Otherwise default to the complete width and adjust the height accordingly
    if ( m_rect.height ( ) > 2 )
      iFontSize = m_rect.height ( );
    theRect = QRect ( 0, iVideoHeight - 45 - iFontSize, iVideoWidth, iFontSize );
  }

  return theRect;
}

bool Subtitles::alreadyRendered ( QString qsBasePath )
{
  Subtitles::entry *pEntry;
  QString   qsSubIdx, qsStart, qsThePath, qsFileName;
  QFileInfo fileInfo;
  int       t, iTotalCount;

  qsSubIdx.sprintf ( "/sub%02d_", m_iSubtitleNumber );
  qsThePath = qsBasePath + qsSubIdx;

  iTotalCount = (int)m_listOfSubtitles.count ( );
    for (t=0;t<iTotalCount; t++) {
    pEntry  = m_listOfSubtitles[t];
    qsStart = pEntry->qsTimeStart;
    qsStart.remove ( ":"      );
    qsStart.replace( ".", "_" );
    qsFileName = QString ("%1%2.png").arg ( qsThePath ).arg( qsStart );
    fileInfo.setFile ( qsFileName );
    if ( ( ! fileInfo.exists ( ) ) || ( fileInfo.size ( ) < 10 ) )
      return false;
  }
  return true; // AFAIK this subtitle has been rendered already
}

#if 1
Subtitles *Subtitles::render ( QString qsBasePath, QString qsResolution, QString qsFileInfoName, bool bForce )
{
  // If we are using Spumux's version to render subtitles, then we won't need to render them here ... right ?
  if ( m_bTextSubtitles )
    return NULL;

  // The first check is to see if the subtitles have been rendered already. If so we return this
  // and ask the user if they should be re-created or not.
  if ( ! bForce && alreadyRendered ( qsBasePath ) )
    return this;

  // Here we handle this in just one function as we can call 
  // qApp->processEvents ( 100 ); // process for 100 ms
  int       t, j, iTotalCount, iWidth, iHeight;
  QString   qsThePath, qsFileName, qsStart, qsSubIdx, qsLang;
  QRgb      theColor, colors[ MAX_SUBTITLE_COLORS ];
  Utils     theUtils;
  QImage    theImage;
  QRect     theRect;
  QPainter  thePainter;
  QDialog   progressDialog   ( NULL );
  //Q3ProgressBar *pProgressBar = NULL;		//ooo
  QProgressBar *pProgressBar = NULL;		//xxx
  QFileInfo fileInfo ( qsFileInfoName );
  iTotalCount = (int)m_listOfSubtitles.count ( );

  // The following widgets will create the progress dialog on the fly.
  if ( iTotalCount > 20 ) { // no progress for only a few subtitles.
    //Q3GridLayout  *pProgressLayout = new Q3GridLayout  ( &progressDialog );	//ooo
    QGridLayout  *pProgressLayout = new QGridLayout  ( &progressDialog );	//xxx
    //pProgressBar = new Q3ProgressBar ( &progressDialog );	//ooo
    pProgressBar = new QProgressBar ( &progressDialog );	//xxx
    pProgressLayout->addWidget( pProgressBar, 0, 0 );
    progressDialog.resize( QSize(537, 50).expandedTo ( progressDialog.minimumSizeHint ( ) ) );
    progressDialog.show  ( );
  }
  Subtitles::entry *pEntry;
  qsLang = theUtils.iso639  ( m_qsIso639, false );
  //progressDialog.setCaption ( QObject::tr ( "Creating %1 Subtitles[%2] for %3" ).arg ( qsLang ).arg ( m_iSubtitleNumber ).arg ( fileInfo.fileName ( ) ) );	//ooo

  qsSubIdx.sprintf ( "/sub%02d_", m_iSubtitleNumber );
  qsThePath = qsBasePath + qsSubIdx;

  // get the rect of the video ...
  iWidth  = theUtils.getWHFromResolution ( qsResolution, true  );
  iHeight = theUtils.getWHFromResolution ( qsResolution, false );
  if ( iWidth  < 350 )
       iWidth  = 720;
  if ( iHeight < 240 )
       iHeight = 480;

  theRect = m_rect;

  // Now we do some magic with colors ...
  // we will set the Background Color to be completely GREEN
  // and we set the foreground color to be completely RED
  // Then we do the drawing
  // After the drawing has been done we wll reduce the colors in the palette to MAX_SUBTITLE_COLORS (4)
  // Lastly we will replace the RED VALUES with the actual requested foreground color.
  // We will use the GREEN color as the transparency value for the color.
  // Note: color0 = background / color1 actual foreground / 
  //       color[2 .. MAX_SUBTITLE_COLORS] = fading foreground to background with delta transparency
  colors[0] = 0x0000FF00; // background
  colors[1] = 0x00FF0000; // foreground

  int   iMaxColors = MAX_SUBTITLE_COLORS - 2;
  float fDelta   = (float)0xFF / (iMaxColors+1);
  int   iBGRed, iBGGreen, iBGBlue, iRed, iGreen, iBlue, iTransparency;
  for ( t=0; t<iMaxColors; t++ )  {
    iGreen  = (int)((t+1) * fDelta);
    iRed    = 255 - iGreen;
    colors[t+2] = ( iRed << 16 ) + ( iGreen << 8 );
  }

  QRect boundingRect;
  iTotalCount = (int)m_listOfSubtitles.count ();
  // Processing some Qt events / messages
  //qApp->processEvents ( 100 ); // do some event processing ...				//ooo
  qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx

  int    iDiameter = m_iOutline*2 + 1;
  QImage theBrush;
  if ( m_iOutline > 0 ) {
    // First we need to create a brush to follow ...
    QPixmap  createCircle ( iDiameter, iDiameter ); //, 1 ); // monochrome pixmap
    QPainter circlePainter;
    //QPen     circlePen  ( colors[1], 1, Qt::SolidLine );			//ooo
    QPen     circlePen  ( QBrush(QColor(colors[1])), 1, Qt::SolidLine );	//xxx
    circlePainter.begin ( &createCircle ); {
      createCircle.fill ( colors[0] );
      circlePainter.setPen   ( circlePen );
      circlePainter.setBrush ( QBrush ( colors[1] ) );
      circlePainter.drawEllipse ( 0, 0, iDiameter, iDiameter );
    } circlePainter.end ( );
    //theBrush = createCircle.convertToImage ( );	//ooo
    theBrush = createCircle.toImage ( );		//xxx
    // Then we reduce the colors to the max avail color space.
    theUtils.reduceColors ( theBrush, 2, (QRgb *)&colors );
  }

  iWidth  = m_rect.width  ( );
  iHeight = m_rect.height ( );

  //Q3TextEdit edit ( NULL );	//ooo
  QTextEdit edit ( NULL );	//xxx
  /*Q3StyleSheet *pStyleSheet = edit.styleSheet ( ); // modify blockquote stylesheet to remove 40px margins left and right
  Q3StyleSheetItem *pStyle = NULL;
  if ( pStyleSheet )  {
    pStyle = new Q3StyleSheetItem ( pStyleSheet, "qdvd" );
    if ( pStyle )  {
      pStyle->setDisplayMode ( Q3StyleSheetItem::DisplayBlock );
      pStyle->setMargin ( Q3StyleSheetItem::MarginLeft,  4 + m_iOutline );
      pStyle->setMargin ( Q3StyleSheetItem::MarginRight, 4 + m_iOutline );
      if ( m_alignment.x ( ) == 1 ) // AlignCenter
        pStyle->setAlignment ( Qt::AlignHCenter );
      if ( m_alignment.x ( ) == 2 ) // AlignLeft
        pStyle->setAlignment ( Qt::AlignLeft );
      else if ( m_alignment.x ( ) == 3 ) // AlignRight
        pStyle->setAlignment ( Qt::AlignRight );
      else if ( m_alignment.x ( ) == 4 ) // AlignJustify
        pStyle->setAlignment ( Qt::AlignJustify );
    }
    // edit.setStyleSheet ( pStyleSheet ); not required ...
  }*/	//ooo

  //edit.setPaletteBackgroundColor ( colors[0] );	//ooo
  //edit.setPaletteForegroundColor ( colors[1] );	//ooo
  //edit.setTextFormat ( Qt::RichText );		//ooo
  edit.resize   ( iWidth + 2 * ( m_iOutline + 4 ), iHeight + 2 * ( m_iOutline + 4 ) );
  edit.setFont  ( m_font  );

  // This is to set the edit.viewport() to max width, so one line of text does not get word wrapped.
  edit.setText ( "<p>12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890</p>" );
  //qApp->processEvents ( 100 ); // do some event processing ...				//ooo
  qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx

  for ( t=0; t<iTotalCount; t++ )  {
    theRect = m_rect;
    pEntry  = m_listOfSubtitles[t];
    if ( pEntry->qsText.isEmpty ( ) )
      continue;

    qsStart = pEntry->qsTimeStart;
    qsStart.remove ( ":"      );
    qsStart.replace( ".", "_" );
    qsFileName = QString ("%1%2.png").arg ( qsThePath ).arg( qsStart );

    QString qsText = pEntry->qsText;
    qsText = qsText.replace ( "\n", "<br>" );

    edit.setText   ( "<qdvd>" + qsText + "</qdvd>" );
    //boundingRect = edit.paragraphRect ( 0 );		//ooo
    QPixmap theDrawingBoard = QPixmap::grabWidget ( edit.viewport ( ) );
    /*boundingRect.setX ( boundingRect.x ( ) + m_iOutline + 4 );
    boundingRect.setY ( boundingRect.y ( ) + m_iOutline + 4 );
    boundingRect.setWidth ( iWidth );*/		//ooo

/*
QString qq=qsHorizAlign + pEntry->qsText + qsHorizAlignClose;
printf ( "%s::%d > r<%d,%d x %d,%d> %s\n", __FILE__, __LINE__, boundingRect.x(), boundingRect.y(), boundingRect.width(),boundingRect.height(), qq.ascii() );

static int ii = 0;
QString q;
if ( ii < 10 )
  q = QString ( "/tmp/a_0%1.png" ).arg ( ii++ );
else
  q = QString ( "/tmp/a_%1.png" ).arg ( ii++ );
theDrawingBoard.save ( q, "PNG" );
*/

    thePainter.begin ( &theDrawingBoard  ); {
      int x, y, w, h;
      x = 0;
      y = 0;
      w = theRect.width  ( ); // should be iWidth
      h = theRect.height ( ); // should be iHeight
      // We should always adjust the height
      if ( m_bFit ) {
        /*x = boundingRect.x      ( ) - m_iOutline;
        y = boundingRect.y      ( ) - m_iOutline;
        w = boundingRect.width  ( ) + m_iOutline * 2;
        h = boundingRect.height ( ) + m_iOutline * 2;*/		//ooo
        if ( x <  0 )       x = 0;
        if ( y <  0 )       y = 0;
        if ( w >= iWidth  ) w = iWidth -1;
        if ( h >= iHeight ) h = iHeight-1;
        theRect = QRect ( x + theRect.x ( ), y + theRect.y ( ), w, h );
      }
      pEntry->rect = theRect;

      // Sanity check
      if ( w > theDrawingBoard.width  ( ) - x )
           w = theDrawingBoard.width  ( ) - x;
      if ( h > theDrawingBoard.height ( ) - y )
           h = theDrawingBoard.height ( ) - y;
      if ( w <=  0 )
           w  = 10;
      if ( h <=  0 )
           h  = 10;

      // create new dimensioned pixmap
      QPixmap thePixmap = QPixmap ( w, h );
//#if ( QT_VERSION > 0x0301FF )		//ooo
      //copyBlt ( &thePixmap, 0, 0, &theDrawingBoard, x, y, w, h );	//ooo
//#else
//      bitBlt  ( &thePixmap, 0, 0, &theDrawingBoard, x, y, w, h, Qt::CopyROP );
//#endif
      //theImage = thePixmap.convertToImage ( );	//ooo
      theImage = thePixmap.toImage ( );			//xxx

    }  thePainter.end ( );

    if ( m_bFit )  {
      // The QTextEdit paragraphRect returns the right height but not the right width ...
      // so we'll cut it out.
      cutHorizontal ( theImage, colors[0] );
      pEntry->rect = theImage.rect ( );
    }
    else if ( m_alignment.y ( ) != 3 )  {
      QImage temp = theImage.copy ( );//( theRect.width ( ), theRect.height ( ), 32 );
      theImage.fill ( colors[0] );
      // If VerticalAlignment not AlignTop, then we'll have to trick here...
      cutHorizontal ( temp, colors[0] ); // returns the minimum rect image

      int iOffsetX = 0;  // default left
      int iOffsetY = 0;  // default top
      if ( m_alignment.x ( ) == 1 ) // AlignCenter
        iOffsetX = (int)( ( theRect.width ( ) - temp.width ( ) ) / 2.0 );
      else if ( m_alignment.x ( ) == 3 ) // AlignRight
        iOffsetX = theRect.width ( ) - temp.width ( );
      if ( m_alignment.y ( ) == 2 ) // AlignVCenter
        iOffsetY = (int)( ( theRect.height ( ) - temp.height ( ) ) / 2.0 );
      else if ( m_alignment.y ( ) == 4 ) // AlignBottom
        iOffsetY = theRect.height ( ) - temp.height ( );

      // and finally we'll have to copy the text to the right position
      //bitBlt ( &theImage, iOffsetX, iOffsetY, &temp );	//oooo
      QPainter p( &theImage );					//xxx
      p.drawImage( iOffsetX, iOffsetY, temp );			//xxx
    }

    // Enable transparency ...
    //theImage.setAlphaBuffer ( true );		//ooo

    // Then we reduce the colors to the max avail color space.
    theUtils.reduceColors ( theImage, MAX_SUBTITLE_COLORS, (QRgb *)&colors );

    // next we generate the backround color ...
    theColor = m_subColors[0].rgb ( ) & 0x00FFFFFF;
    iBGRed   = qRed   ( theColor );
    iBGGreen = qGreen ( theColor );
    iBGBlue  = qBlue  ( theColor );
    theImage.setColor ( 0, m_subColors[0].rgb ( ) );

    // and then the rest of the colors
    theColor = m_subColors[1].rgb ( ) & 0x00FFFFFF;
    iRed     = qRed   ( theColor );
    iGreen   = qGreen ( theColor );
    iBlue    = qBlue  ( theColor );
    // Here we'll blend the max 4 colors for the subtitles
    for ( j=1; j<MAX_SUBTITLE_COLORS; j++ ) {
      int iDelta, r, g, b;
      iTransparency = 255 - (( colors[j] & 0xFF00 ) >> 8); // shift green to get the base transparency value
      // which must be added to the multiplied by the user defined transparency
      fDelta = (float)(255 - m_subColors[j].alpha ( ) ) / 0xFF;
      iTransparency = (int)( fDelta * (255-iTransparency ) ) + iTransparency;

      iDelta = ( (colors[j] & 0xFF0000 ) >> 16 );
      fDelta = (float)(0xFF - iDelta) / 0xFF;
      r = iRed   + (int)( ( iBGRed   - iRed   ) * fDelta );
      g = iGreen + (int)( ( iBGGreen - iGreen ) * fDelta );
      b = iBlue  + (int)( ( iBGBlue  - iBlue  ) * fDelta );

      theColor   =qRgba ( r, g, b, iTransparency );
      theImage.setColor ( j, theColor );
    }

    if ( m_iOutline > 0 ) {
      // Next is to inclrease the number of colors to 4
      //theImage.setNumColors ( 4 );	//ooo
      theImage.setColorCount ( 4 );	//xxx
      //theImage.setColor ( 3, 0xFF101010 ); // FF == Opaque // 00=Transparent
      theImage.setColor ( 0, 0x00000000 );
      theImage.setColor ( 3, m_subColors[0].rgb ( ) ); //real deal ...
      // Okay, we have outline enabled which means we should replace all background color with either 
      // transparency or with the pixelcolor of the backdrop or with the pixel color of the generated subtitle.
      int w, h, x, y, xb, yb, brushX, brushY, idx;
      w = theImage.width  ( );
      h = theImage.height ( );

      int pIdx;
      for ( x=0; x<w; x++ ) {
        for ( y=0; y<h; y++ ) { // Image x, and y ...
          // here we take m_iOutline number of pixels before and after an set them to the fourth color.
          pIdx = theImage.pixelIndex ( x, y );
          if ( ( pIdx == 1 ) || ( pIdx == 2 ) ) { // not background
            // So we should underlay the brush ...
            for ( yb=0; yb<iDiameter; yb++ ) { // copy brush over ...
              brushY = y - m_iOutline + yb;
              if ( ( brushY < 0 ) || ( brushY >= h ) )  // out of bounds
                continue;
              for ( xb=0; xb<iDiameter; xb++ ) {
                brushX = x - m_iOutline + xb; 
                if ( ( brushX < 0 ) || ( brushX >= w ) ) // out of bounds
                  continue;
                idx = theImage.pixelIndex ( brushX, brushY );
                if ( ( idx == 0 ) && ( theBrush.pixelIndex ( xb, yb ) == 1 ) )
                  theImage.setPixel   ( brushX, brushY, 3 );
              }
            }
          }
        }
      }
    }
    theImage.save ( qsFileName, "PNG", 100 );  // 100%== No compression. png image

    if ( ( iTotalCount > 20 ) && ( pProgressBar ) ) {
      //pProgressBar->setProgress ( (int)((float)t / iTotalCount * 100.0) );	//ooo
      pProgressBar->setValue ( (int)((float)t / iTotalCount * 100.0) );		//xxx
      if ( t%10 == 0 ) 
        //qApp->processEvents ( 100 ); // do some event processing ...					//ooo
	qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx
    }
  }
  // that's it ...
  return NULL;
}

#else

Subtitles *Subtitles::render ( QString qsBasePath, QString qsResolution, QString qsFileInfoName, bool bForce )
{
  // If we are using Spumux's version to render subtitles, then we won't need to render them here ... right ?
  if ( m_bTextSubtitles )
    return NULL;

  // The first check is to see if the subtitles have been rendered already. If so we return this
  // and ask the user if they should be re-created or not.
  if ( ! bForce && alreadyRendered ( qsBasePath ) )
    return this;

  // Here we handle this in just one function as we can call 
  // qApp->processEvents ( 100 ); // process for 100 ms
  int       t, j, iTotalCount, iWidth, iHeight;
  QString   qsThePath, qsFileName, qsStart, qsSubIdx, qsLang;
  QRgb      theColor, colors[ MAX_SUBTITLE_COLORS ];
  Utils     theUtils;
  QImage    theImage;
  QRect     theRect;
  QPainter  thePainter;
  QDialog   progressDialog   ( NULL );
  //Q3ProgressBar *pProgressBar = NULL;		//ooo
  QProgressBar *pProgressBar = NULL;		//xxx
  QFileInfo fileInfo ( qsFileInfoName );
  iTotalCount = (int)m_listOfSubtitles.count ( );

  // The following widgets will create the progress dialog on the fly.
  if ( iTotalCount > 20 ) { // no progress for only a few subtitles.
    Q3GridLayout  *pProgressLayout = new Q3GridLayout  ( &progressDialog );
    //pProgressBar = new Q3ProgressBar ( &progressDialog );	//ooo
    pProgressBar = new QProgressBar ( &progressDialog );	//xxx
    pProgressLayout->addWidget( pProgressBar, 0, 0 );
    progressDialog.resize( QSize(537, 50).expandedTo ( progressDialog.minimumSizeHint ( ) ) );
    progressDialog.show  ( );
  }
  Subtitles::entry *pEntry;
  qsLang = theUtils.iso639  ( m_qsIso639, false );
  progressDialog.setCaption ( QObject::tr ( "Creating %1 Subtitles for %2" ).arg ( qsLang ).arg ( fileInfo.fileName ( ) ) );

  qsSubIdx.sprintf ( "/sub%02d_", m_iSubtitleNumber );
  qsThePath = qsBasePath + qsSubIdx;

  // get the rect of the video ...
  iWidth  = theUtils.getWHFromResolution ( qsResolution, true  );
  iHeight = theUtils.getWHFromResolution ( qsResolution, false );
  if ( iWidth  < 350 )
       iWidth  = 720;
  if ( iHeight < 240 )
       iHeight = 480;

    theRect = m_rect;

//printf ( "%s::%d > theRect<%d,%d x x%d,%d> m_rect<%d,%d x %d,%d>\n", __FUNCTION__, __LINE__, theRect.x ( ), theRect.y ( ), theRect.width ( ), theRect.height ( ), m_rect.x ( ), m_rect.y ( ), m_rect.width ( ), m_rect.height ( ) );

  // Now we do some magic with colors ...
  // we will set the Background Color to be completely GREEN
  // and we set the foreground color to be completely RED
  // Then we do the drawing
  // After the drawing has been done we wll reduce the colors in the palette to MAX_SUBTITLE_COLORS (4)
  // Lastly we will replace the RED VALUES with the actual requested foreground color.
  // We will use the GREEN color as the transparency value for the color.
  // Note: color0 = background / color1 actual foreground / 
  //       color[2 .. MAX_SUBTITLE_COLORS] = fading foreground to background with delta transparency
  colors[0] = 0x0000FF00; // background
  colors[1] = 0x00FF0000; // foreground

  int   iMaxColors = MAX_SUBTITLE_COLORS - 2;
  float fDelta   = (float)0xFF / (iMaxColors+1);
  int   iBGRed, iBGGreen, iBGBlue, iRed, iGreen, iBlue, iTransparency;
  for ( t=0; t<iMaxColors; t++ )  {
    iGreen  = (int)((t+1) * fDelta);
    iRed    = 255 - iGreen;
    colors[t+2] = ( iRed << 16 ) + ( iGreen << 8 );
  }

  QRect boundingRect;
  QPen  thePen ( colors[1], 2, Qt::DashDotLine );
  int   iFlags = Qt::TextDontClip | Qt::TextWordWrap;

  switch   ( m_alignment.x ( ) ) {
  case 2:
    iFlags |= Qt::AlignLeft;
    break;
  case 3:
    iFlags |= Qt::AlignRight;
    break;
  case 4:
    iFlags |= Qt::AlignJustify;
    break;
  default:
    iFlags |= Qt::AlignHCenter;
  }
  switch   ( m_alignment.y ( ) )  {
  case 3:
    iFlags |= Qt::AlignTop;
  break;
  case 4:
    iFlags |= Qt::AlignBottom;
  break;
  default:
    iFlags |= Qt::AlignVCenter;
  }

  iTotalCount = (int)m_listOfSubtitles.count ();
  // Processing some Qt events / messages
  //qApp->processEvents ( 100 ); // do some event processing ...				//ooo
  qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx

  int    iDiameter = m_iOutline*2 + 1;
  QImage theBrush;
  if ( m_iOutline > 0 ) {
    // First we need to create a brush to follow ...
    QPixmap  createCircle ( iDiameter, iDiameter ); //, 1 ); // monochrome pixmap
    QPainter circlePainter;
    QPen     circlePen  ( colors[1], 1, Qt::SolidLine );
    circlePainter.begin ( &createCircle ); {
      createCircle.fill ( colors[0] );
      circlePainter.setPen   ( circlePen );
      circlePainter.setBrush ( QBrush ( colors[1] ) );
      circlePainter.drawEllipse ( 0, 0, iDiameter, iDiameter );
    } circlePainter.end ( );
    theBrush = createCircle.convertToImage ( );
    // Then we reduce the colors to the max avail color space.
    theUtils.reduceColors ( theBrush, 2, (QRgb *)&colors );
  }

  iWidth  = m_rect.width  ( );
  iHeight = m_rect.height ( );
  for ( t=0; t<iTotalCount; t++ )  {
    theRect = m_rect;
    pEntry  = m_listOfSubtitles[t];
    qsStart = pEntry->qsTimeStart;
    qsStart.remove ( ":"      );
    qsStart.replace( ".", "_" );
    qsFileName = QString ("%1%2.png").arg ( qsThePath ).arg( qsStart );

    // First we should fill the drawing board with the propper background  color.
    QPixmap theDrawingBoard ( iWidth, iHeight );
    thePainter.begin ( &theDrawingBoard  ); {
      theDrawingBoard.fill( colors[0] );
      thePainter.setPen   ( thePen );
      thePainter.setBrush ( QBrush ( colors[0] ) );
      thePainter.setFont  ( m_font );

      // Here we draw onto the drawing board
      QRect temp ( 0, 0, iWidth, iHeight );
      thePainter.drawText ( temp, iFlags, pEtry->qsText, -1, &boundingRect );

/*
static int ii = 0;
QString q;
if ( ii < 10 )
  q = QString ( "/tmp/a_0%1.png" ).arg ( ii++ );
else
  q = QString ( "/tmp/a_%1.png" ).arg ( ii++ );
theDrawingBoard.save ( q, "PNG" );
*/

      int x, y, w, h;
      x = 0;
      y = 0;
      w = theRect.width  ( ); // should be iWidth
      h = theRect.height ( ); // should be iHeight
      // We should always adjust the height
      if ( m_bFit ) {
        x = boundingRect.x      ( ) - m_iOutline;
        y = boundingRect.y      ( ) - m_iOutline;
        w = boundingRect.width  ( ) + m_iOutline * 2;
        h = boundingRect.height ( ) + m_iOutline * 2;
        if ( x <  0 )       x = 0;
        if ( y <  0 )       y = 0;
        if ( w >= iWidth  ) w = iWidth -1;
        if ( h >= iHeight ) h = iHeight-1;
        theRect = QRect ( x + theRect.x ( ), y + theRect.y ( ), w, h );
      }
      pEntry->rect = theRect; //QRect ( x, y, w, h );
//printf ( "%s::%d >   <%s> bFit<%d> w<%d> h<%d> pEntry->rect<%d,%d x %d,%d> boundingRect<%d,%d x %d,%d> XYWH<%d,%d x %d,%d>\n", __FUNCTION__, __LINE__, pEntry->qsText.ascii(), m_bFit, iWidth, iHeight, theRect.x(), theRect.y(), theRect.width(), theRect.height(), boundingRect.x ( ), boundingRect.y ( ), boundingRect.width ( ), boundingRect.height ( ), x, y, w, h );

      // create new dimensioned pixmap
      QPixmap thePixmap = QPixmap ( w, h );
//#if ( QT_VERSION > 0x0301FF )		//ooo
      copyBlt ( &thePixmap, 0, 0, &theDrawingBoard, x, y, w, h );
//#else
//      bitBlt  ( &thePixmap, 0, 0, &theDrawingBoard, x, y, w, h, Qt::CopyROP);
//#endif
      theImage = thePixmap.convertToImage ( );
    }  thePainter.end ( );
    // Enable transparency ...
    theImage.setAlphaBuffer ( true );

    // Then we reduce the colors to the max avail color space.
    theUtils.reduceColors ( theImage, MAX_SUBTITLE_COLORS, (QRgb *)&colors );

    // next we generate the backround color ...
    theColor = m_subColors[0].rgb ( ) & 0x00FFFFFF;
    iBGRed   = qRed   ( theColor );
    iBGGreen = qGreen ( theColor );
    iBGBlue  = qBlue  ( theColor );
    theImage.setColor ( 0, m_subColors[0].rgb ( ) );

    // and then the rest of the colors
    theColor = m_subColors[1].rgb ( ) & 0x00FFFFFF;
    iRed     = qRed   ( theColor );
    iGreen   = qGreen ( theColor );
    iBlue    = qBlue  ( theColor );
    // Here we'll blend the max 4 colors for the subtitles
    for ( j=1; j<MAX_SUBTITLE_COLORS; j++ ) {
      int iDelta, r, g, b;
      iTransparency = 255 - (( colors[j] & 0xFF00 ) >> 8); // shift green to get the base transparency value
      // which must be added to the multiplied by the user defined transparency
      fDelta = (float)(255 - m_subColors[j].alpha ( ) ) / 0xFF;
      iTransparency = (int)( fDelta * (255-iTransparency ) ) + iTransparency;

      iDelta = ( (colors[j] & 0xFF0000 ) >> 16 );
      fDelta = (float)(0xFF - iDelta) / 0xFF;
      r = iRed   + (int)( ( iBGRed   - iRed   ) * fDelta );
      g = iGreen + (int)( ( iBGGreen - iGreen ) * fDelta );
      b = iBlue  + (int)( ( iBGBlue  - iBlue  ) * fDelta );

      theColor   =qRgba ( r, g, b, iTransparency );
      theImage.setColor ( j, theColor );
    }

    if ( m_iOutline > 0 ) {
      // Next is to inclrease the number of colors to 4
      theImage.setNumColors ( 4 );
      //theImage.setColor ( 3, 0xFF101010 ); // FF == Opaque // 00=Transparent
      theImage.setColor ( 0, 0x00000000 );
      theImage.setColor ( 3, m_subColors[0].rgb ( ) ); //real deal ...
      // Okay, we have outline enabled which means we should replace all background color with either 
      // transparency or with the pixelcolor of the backdrop or with the pixel color of the generated subtitle.
      int w, h, x, y, xb, yb, brushX, brushY, idx;
      w = theImage.width  ( );
      h = theImage.height ( );

      int pIdx;
      for ( x=0; x<w; x++ ) {
        for ( y=0; y<h; y++ ) { // Image x, and y ...
          // here we take m_iOutline number of pixels before and after an set them to the fourth color.
          pIdx = theImage.pixelIndex ( x, y );
          if ( ( pIdx == 1 ) || ( pIdx == 2 ) ) { // not background
            // So we should underlay the brush ...
            for ( yb=0; yb<iDiameter; yb++ ) { // copy brush over ...
              brushY = y - m_iOutline + yb;
              if ( ( brushY < 0 ) || ( brushY >= h ) )  // out of bounds
                continue;
              for ( xb=0; xb<iDiameter; xb++ ) {
                brushX = x - m_iOutline + xb; 
                if ( ( brushX < 0 ) || ( brushX >= w ) ) // out of bounds
                  continue;
                idx = theImage.pixelIndex ( brushX, brushY );
                if ( ( idx == 0 ) && ( theBrush.pixelIndex ( xb, yb ) == 1 ) )
                  theImage.setPixel   ( brushX, brushY, 3 );
              }
            }
          }
        }
      }
    }
    theImage.save ( qsFileName, "PNG", 100 );  // 100%== No compression. png image

    if ( ( iTotalCount > 20 ) && ( pProgressBar ) ) {
      pProgressBar->setProgress ( (int)((float)t / iTotalCount * 100.0) );
      if ( t%10 == 0 ) 
        //qApp->processEvents ( 100 ); // do some event processing ...					//ooo
	qApp->processEvents ( QEventLoop::AllEvents, 100 ); // do some event processing ...		//xxx
    }
  }
  // that's it ...
  return NULL;
}

#endif

void Subtitles::cutHorizontal ( QImage &img, QRgb filter )
{
  // The height of the rect holds the max Y to search.
  // This function will search the max extension of the text in this image.
  // That is the first/last pixel that is not the filter color
  int   x, y, iMaxX, iMinX, iMaxY, iMinY;
  uint *pLine  = NULL;
  bool bFound  = false;
  int  iWidth  = img.width  ( );
  int  iHeight = img.height ( );

  iMaxX = 0;
  iMaxY = iHeight;
  iMinX = iWidth;
  iMinY = 0;
  for ( y=0; y<iHeight; y++ )  {
    pLine = (uint *)img.scanLine ( y );
    for ( x=0; x<iMinX; x++ )  {
      if ( ( pLine[x] & 0x00FFFFFF ) != filter )  {
        bFound = true;
        if ( iMinX > x )  {
             iMinX = x;
        }
      }
    }
    if ( ! bFound )
      iMinY = y;
  }
  bFound = false;
  for ( y=iHeight-1; y>=0; y-- )  {
    pLine = (uint *)img.scanLine ( y );
    for ( x=img.width ( )-1; x>=iMaxX; x-- )  {
      if ( ( pLine[x] & 0x00FFFFFF ) != filter )  {
        bFound = true;
        if ( iMaxX < x )  {
             iMaxX = x;
        }
      }
    }
    if ( ! bFound )
      iMaxY = y;
  }

  iMinX = iMinX - m_iOutline - 4;
  iMinY = iMinY - m_iOutline - 4;
  iMaxX = iMaxX + m_iOutline + 4;
  iMaxY = iMaxY + m_iOutline + 4;
  if ( iMinX < 0 )
       iMinX = 0;
  if ( iMinY < 0 )
       iMinY = 0;
  if ( iMaxX > iWidth  )
       iMaxX = iWidth;
  if ( iMaxY > iHeight )
       iMaxY = iHeight;

  iWidth  = iMaxX-iMinX;
  iHeight = iMaxY-iMinY;
  //QImage image ( iWidth, iHeight, 32 );			//ooo
  QImage image ( iWidth, iHeight, QImage::Format_RGB32 );	//xxx

  // Now we have the min/max values, lets cut the cake ...
  //bitBlt ( &image, 0, 0, &img, iMinX, iMinY, iWidth, iHeight );	//ooo
  img = image;
}

// static function to auto extract MetaInfo from a DV video stream in a backround thread
void Subtitles::generateSubtitles ( QObject *pOrigObject, SourceFileInfo *pInfo, QString qsIso639 )
{
  if ( ( ! pInfo ) || ( ! pInfo->bMetaSubtitles ) )
    return;

  QFileInfo  fileInfo   ( pInfo->qsFileName );
  Subtitles *pSubtitles = NULL;
  QString    qsLang, qsXmlFile;
  QRect      rect;
  Utils      theUtils;
  int        t, iFirstNewSubtitleNumber = 0;
  bool       bOkay;
  float      fFPS;

  // First we see if we already have a subtitle with MetaInfo created.
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    pSubtitles = pInfo->arraySubtitles [ t ];
    if ( pSubtitles ) { 
      if ( ( pSubtitles->m_subtitleState == Subtitles::STATE_META_INFO  ) ||
           ( pSubtitles->m_subtitleState == Subtitles::STATE_EXTRACTING )  )
             return;
    }
  }
  // lastly we see if we can extract the MetaInfo ( DV only at this time )
  //if ( fileInfo.extension ( ).lower ( ) != "dv" )	//ooo
  if ( fileInfo.suffix ( ).toLower ( ) != "dv" )	//xxx
    return;

  // Okay we do actually want to create subtitles here ...    
  for ( t=0; t<MAX_SUBTITLES; t++ ) {
    iFirstNewSubtitleNumber = t;
    if ( ! pInfo->arraySubtitles [ t ] ) 
      break;
  }

  // Next we should import the subtitles ...
  rect = QRect ( 0, 0, 720, 480 ); // standard NTSC res.
  fFPS =  pInfo->qsFPS.toFloat ( &bOkay );
  if ( ( ! bOkay ) || ( fFPS < 19.0f ) ) {
    fFPS = 29.97f;
    //if ( pInfo->qsVideoFormat.upper ( ) == "PAL" )  {		//ooo
    if ( pInfo->qsVideoFormat.toUpper ( ) == "PAL" )  {		//xxx
      fFPS = 25.0f;
      rect = QRect ( 0, 0, 720, 576 );
    }
  }

  // sanityCheckSubtitleTrack ( iFirstNewSubtitleNumber );
  if ( ( qsIso639.isEmpty ( ) ) || ( qsIso639.length ( ) != 2 ) ) 
    qsIso639 = "en";
  qsLang = theUtils.iso639 ( qsIso639, false );

  fileInfo.setFile  ( pInfo->qsFileName );
  qsXmlFile.sprintf ( "/subtitle_%d.xml", iFirstNewSubtitleNumber );
  //qsXmlFile = theUtils.getTempFile ( fileInfo.baseName ( TRUE ) ) + qsXmlFile;	//ooo
  qsXmlFile = theUtils.getTempFile ( fileInfo.baseName ( ) ) + qsXmlFile;		//xxx

  pSubtitles = new Subtitles ( rect.width ( ), rect.height ( ) );
  // Set subtitle parameters ...
  pSubtitles->m_subtitleState       = Subtitles::STATE_EXTRACTING;
  pSubtitles->m_iSubtitleNumber     = iFirstNewSubtitleNumber;
  pSubtitles->m_qsXmlFile           = qsXmlFile;
  pSubtitles->m_qsIso639            = qsIso639;
  pSubtitles->m_subColors[0]        = Rgba (   0,   0,  0, 255 ); // black background
  pSubtitles->m_subColors[1]        = Rgba ( 250, 250, 10, 255 ); // Yellow font color
  pSubtitles->m_iOutline            = 2;
  pSubtitles->m_rect                = rect;
  pSubtitles->m_font.fromString ( "Sans Serif,24,-1,5,75,0,0,0,0,0" );
  // link it with the SourceFileInfo ...
  if ( pInfo->arraySubtitles [iFirstNewSubtitleNumber] )  {
    pSubtitles->m_qsFileName = pInfo->arraySubtitles [iFirstNewSubtitleNumber]->m_qsFileName;
    delete pInfo->arraySubtitles [iFirstNewSubtitleNumber];
  }
  pInfo->arraySubtitles [iFirstNewSubtitleNumber] = pSubtitles;

  // And extract the subtitles in a background task ...
  MediaCreator::registerWithMediaScanner ( pOrigObject, pInfo, pSubtitles, fFPS );
}


