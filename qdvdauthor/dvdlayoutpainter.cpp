/***************************************************************************
    dvdlayoutpainter.cpp
                             -------------------
    Class DVDLayout::Painter
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This is a sub-project to 'Q' DVD-Author to display the current DVD Layout
    
****************************************************************************/

#include <qpainter.h>
//Added by qt3to4:
//#include <Q3PointArray>   //ooo
#include <QPolygon>         //xxx
//#include <Q3ValueList>	//oxx
#include <QMessageBox>      //xxx

#include "dvdlayoutgui.h"
#include "dvdlayoutpainter.h"

namespace DVDLayout
{

SimplePainter::SimplePainter ( )
{
}

SimplePainter::~SimplePainter ( )
{
}

void SimplePainter::draw ( QPainter &thePainter, Area::Container *pContainer, Object *pExcept )
{  
  Area::Container *pTarget = NULL;
  Area::Container::Connection *pConnection = NULL;
  QList<Area::Container::Connection *>::iterator it = pContainer->targets ( ).begin ( );	//oxx
  //thePainter.setPen(QPen(Qt::black, 3, Qt::DashDotLine, Qt::RoundCap));   //xxx
  //thePainter.drawLine   (1,1,100,100);   //xxx
  while ( it != pContainer->targets ( ).end ( ) )  {
    pConnection = *it++;
    pTarget     = pConnection->target ( );
    if ( ( ! pTarget ) || ( pTarget->object ( ) == pExcept ) )
      continue;

    //QPen thePen ( pConnection->m_color, 2 );			//ooo
    QPen thePen ( QBrush(QColor(pConnection->m_color)), 2 );	//xxx
    thePainter.setPen   ( thePen );
    //thePainter.setBrush ( pConnection->m_color );			//ooo
    thePainter.setBrush ( QBrush(QColor(pConnection->m_color)) );	//xxx
    drawLines ( thePainter, pConnection->lines ( ) );
  }   //ooo
}

void SimplePainter::draw ( QPainter &thePainter, Area::Container::Connection *pConnection )
{
  ////QPen thePen ( pConnection->m_color, 2 );			//ooo
  QPen thePen ( QBrush(QColor(pConnection->m_color)), 2 );	//xxx
  thePainter.setPen   ( thePen );
  //thePainter.setBrush ( pConnection->m_color );			//ooo
  thePainter.setBrush ( QBrush(QColor(pConnection->m_color)) );		//xxx
  drawLines ( thePainter, pConnection->lines ( ) );
}

void SimplePainter::drawLines ( QPainter &thePainter, QList<Area::PieceOfLine *> &lines )	//oxx
{  
  Area::PieceOfLine *pLine = NULL;
  QList<Area::PieceOfLine *>::iterator it = lines.begin ( );	//oxx
  while ( it != lines.end ( ) )  {
    pLine = *it++;  
    thePainter.drawLine   ( pLine->m_startPos, pLine->m_endPos ); //ooo
    if ( it ==  lines.end ( ) )  //m_listLineStipples.end ( ) ) // The last point receives the arrow.
      //thePainter.drawRect ( pLine->m_endPos.x ( ) - 4, pLine->m_endPos.y ( ) - 4, 8, 8 );
      drawArrow ( thePainter, pLine );
    else
      thePainter.drawRect ( pLine->m_endPos.x ( ) - 2, pLine->m_endPos.y ( ) - 2, 4, 4 );
  }
}

void SimplePainter::drawArrow ( QPainter &thePainter, Area::PieceOfLine *pLine )
{
  const int iWidth  = 9;
  const int iHeight = 7;
  //Q3PointArray array ( 3 );   //ooo
  QPolygon array ( 3 );         //xxx
  int x = pLine->m_endPos.x ( );
  int y = pLine->m_endPos.y ( );
  if ( x > pLine->m_startPos.x ( ) )      // Arrow to the right
    array.putPoints ( 0, 3, x - iWidth, y + iHeight/2, x, y, x - iWidth, y - iHeight/2 );
  else if ( x < pLine->m_startPos.x ( ) ) // Arrow to the Left
    array.putPoints ( 0, 3, x + iWidth, y - iHeight/2, x, y, x + iWidth, y + iHeight/2 );
  else if ( y > pLine->m_startPos.y ( ) ) // Arrow down 
    array.putPoints ( 0, 3, x - iHeight/2, y - iWidth, x, y, x + iHeight/2, y + iWidth );
  else
    array.putPoints ( 0, 3, x - iHeight/2, y + iWidth, x, y, x + iHeight/2, y + iWidth );

  thePainter.drawPolygon ( array );
}

void SimplePainter::paintEvent (QPaintEvent *pPaint)    //xxx
{
   QPixmap temp;    //xxx
   QPainter thePainter;        //xxx
   thePainter.begin(&temp);    //xxx
   thePainter.drawLine(1,1,100,100);   //xxx
   thePainter.end();    //xxx
}

////////////////////////////////////////////////////////////
//
//  A bit nicer painter using two colored brush 
//  per connection in Orthogonal style.
//
////////////////////////////////////////////////////////////
NicePainter::Brush::Brush ( QRgb &theColor )
{
  m_brushColor = theColor;

  // Next we get the 32 * 32 brush template and create the proper colored brush
  //QImage brush = QImage::fromMimeSource ( "brush.png" );	//ooo
  QImage brush = QImage ( ":/images/brush.png" );		//xxx
  //m_innerBrush = brush.smoothScale ( 4, 4 );  //ooo
  m_innerBrush = brush.scaled ( 4, 4 );         //xxx
  //m_outerBrush = brush.smoothScale ( 8, 8 );  //ooo
  m_outerBrush = brush.scaled ( 8, 8 );         //xxx
  int x, y;
  unsigned char red, green, blue;
  float fFactor;

  red   = qRed   ( theColor );
  green = qGreen ( theColor );
  blue  = qBlue  ( theColor );

  QRgb *pLine = NULL;
  for ( y=0; y<m_outerBrush.height  ( ); y++ )  {
    pLine = (QRgb *)m_outerBrush.scanLine  ( y );
    for ( x=0; x<m_outerBrush.width ( ); x++ )  {
      QRgb color = pLine[x];
      // E.g. 0x32040404. So we have alpha plus the multiplicator for the color.
      fFactor  = ( color & 0xFF ) / 255.0f;
      color    =   color & 0xFF000000;
      color   |= ( ( (int)(fFactor * red ) ) << 16 ) | ( ( (int)(fFactor * green ) ) << 8 ) | (int)( fFactor * blue );
      pLine[x] = color;
    }
  }

  // Inner Brush is pure white
  for ( y=0; y<m_innerBrush.height  ( ); y++ )  {
    pLine = (QRgb *)m_innerBrush.scanLine  ( y );
    for ( x=0; x<m_innerBrush.width ( ); x++ )  {
      QRgb color = pLine[x];
      // E.g. 0x32040404. So we have alpha plus the multiplicator for the color.
      red      = ( color & 0xFF );
      color    =   color & 0xFF000000;
      color   |= ( red << 16 ) | ( red << 8 ) | red;
      pLine[x] = color;
    }
  }
}

NicePainter::NicePainter ( )
  : SimplePainter ( )
{
  m_pCurrentBrush = NULL;
  m_bFastDraw     = false;
}

NicePainter::~NicePainter ( )
{
}

void NicePainter::setFastDraw ( bool bFast )
{
  m_bFastDraw = bFast;
}

NicePainter::Brush *NicePainter::getBrush ( QRgb &theColor )
{
  NicePainter::Brush *pBrush = NULL;
  QList<NicePainter::Brush *>::iterator it = m_listBrush.begin ( );	//oxx
  while ( it != m_listBrush.end ( ) )  {
    pBrush = *it++;
    if ( pBrush->m_brushColor == theColor )
      return pBrush;
  }
  pBrush = new Brush ( theColor );
  m_listBrush.append (   pBrush );
  return pBrush;
}

void NicePainter::draw ( QPainter &thePainter, Area::Container *pContainer, Object *pExcept )
{
  Area::Container *pTarget = NULL;
  Area::Container::Connection *pConnection = NULL;
  QList<Area::Container::Connection *>::iterator it = pContainer->targets ( ).begin ( );	//oxx
  while ( it != pContainer->targets ( ).end ( ) )  {
    pConnection = *it++;
    pTarget     = pConnection->target ( );
    if ( ( ! pTarget ) || ( pTarget->object ( ) == pExcept ) )
      continue;

    //QPen thePen ( pConnection->m_color, 2 );			//ooo
    QPen thePen ( QBrush(QColor(pConnection->m_color)), 2 );	//xxx
    thePainter.setPen   ( thePen );
    //thePainter.setBrush ( pConnection->m_color );			//ooo
    thePainter.setBrush ( QBrush(QColor(pConnection->m_color)) );	//xxx
    m_pCurrentBrush = getBrush ( pConnection->m_color );

    drawLines ( thePainter, pConnection->lines ( ) );
  }
}

void NicePainter::draw ( QPainter &thePainter, Area::Container::Connection *pConnection )
{
  //QPen thePen ( pConnection->m_color, 2 );			//ooo
  QPen thePen ( QBrush(QColor(pConnection->m_color)), 2 );	//xxx
  thePainter.setPen   ( thePen );
  //thePainter.setBrush ( pConnection->m_color );			//ooo
  thePainter.setBrush ( QBrush(QColor(pConnection->m_color)) );		//xxx
  m_pCurrentBrush = getBrush ( pConnection->m_color );

  drawLines ( thePainter, pConnection->lines ( ) );
}

#if 0
// Drawing BSplines .. looks bad though ...
void NicePainter::drawLines ( QPainter &thePainter, Q3ValueList<Area::PieceOfLine *> &lines )
{
  Area::PieceOfLine *pLine = NULL;
  Q3ValueList<Area::PieceOfLine *>::iterator it = lines.begin ( );

  int iIdx = 0;
  BSpline spline ( 2, lines.size ( )+1 );
  if ( lines.size ( ) > 0 )
      spline.set  ( iIdx++, lines[0]->m_startPos.x ( ), lines[0]->m_startPos.y ( ), 0 );
  while ( it != lines.end ( ) )  {
    pLine = *it++;
    spline.set ( iIdx++, pLine->m_endPos.x ( ), pLine->m_endPos.y ( ), 0 );
  }
  spline.createNodeVector ( );
  spline.drawLines ( thePainter, this, m_pCurrentBrush->m_outerBrush );
  spline.drawLines ( thePainter, this, m_pCurrentBrush->m_innerBrush );
}
#else
// Original, drawing linear lines instead of curved
void NicePainter::drawLines ( QPainter &thePainter, QList<Area::PieceOfLine *> &lines )		//oxx
{
  Area::PieceOfLine *pLine = NULL;
  QList<Area::PieceOfLine *>::iterator it = lines.begin ( );		//oxx
  while ( it != lines.end ( ) )  {
    pLine = *it++;
    if ( it ==  lines.end ( ) )  //m_listLineStipples.end ( ) ) // The last point receives the arrow.
      drawArrow ( thePainter, pLine, m_pCurrentBrush->m_outerBrush );
    else
      drawLine ( thePainter, pLine->m_startPos, pLine->m_endPos, m_pCurrentBrush->m_outerBrush );
  }
  it = lines.begin ( );
  while ( it != lines.end ( ) )  {
    pLine = *it++;
    if ( it ==  lines.end ( ) )  //m_listLineStipples.end ( ) ) // The last point receives the arrow.
      drawArrow ( thePainter, pLine, m_pCurrentBrush->m_innerBrush );
    else
      drawLine ( thePainter, pLine->m_startPos, pLine->m_endPos, m_pCurrentBrush->m_innerBrush );
  }
}
#endif

void NicePainter::drawArrow ( QPainter &thePainter, Area::PieceOfLine *pLine, QImage &brush )
{
  const int iWidth  = 9;
  const int iHeight = 8;
  //Q3PointArray array ( 3 );   //ooo
  QPolygon array ( 3 );         //xxx
  int x = pLine->m_endPos.x ( );
  int y = pLine->m_endPos.y ( );
  QPoint endPos = pLine->m_endPos;
  if ( x > pLine->m_startPos.x ( ) )  {    // Arrow to the right
    endPos.setX ( endPos.x ( ) - 7 );
    array.putPoints ( 0, 3, x - iWidth, y + iHeight/2, x, y, x - iWidth, y - iHeight/2 );
  }
  else if ( x < pLine->m_startPos.x ( ) )  { // Arrow to the Left
    endPos.setX ( endPos.x ( ) + 7 );
    array.putPoints ( 0, 3, x + iWidth, y - iHeight/2, x, y, x + iWidth, y + iHeight/2 );
  }
  else if ( y > pLine->m_startPos.y ( ) )  { // Arrow down
    endPos.setY ( endPos.y ( ) - 7 );
    array.putPoints ( 0, 3, x - iHeight/2, y - iWidth, x, y, x + iHeight/2, y + iWidth );
  }
  else  {
    endPos.setY ( endPos.y ( ) + 7 );
    array.putPoints ( 0, 3, x - iHeight/2, y + iWidth, x, y, x + iHeight/2, y + iWidth );
  }

  drawLine ( thePainter, pLine->m_startPos, endPos, brush );
  thePainter.drawPolygon ( array );
}

// Drawing a straight line
void NicePainter::drawLine ( QPainter &thePainter, int x1, int y1, int x2, int y2, QImage &brush )
{
  if ( m_bFastDraw )  {
    thePainter.drawLine ( x1, y1, x2, y2 );
    return;
  }

  int iDeltaX  = x1 - x2;
  int iDeltaY  = y1 - y2;
  float fSlope = 9999999.9f;
  if( iDeltaX != 0 )
        fSlope = (float)iDeltaY / (float)iDeltaX;
  int iOffsetX = (int)( (float)brush.width  ( ) / 2.0 );
  int iOffsetY = (int)( (float)brush.height ( ) / 2.0 );

  if ( ( fSlope < 1.0 ) && ( fSlope > -1.0 ) ) {
    // Horizontally aligned lines
    if ( x1 > x2 )  {
      int t = x2;
         x2 = x1;
         x1 = t;
    }
    int   iX;
    float fY = (float)y1;
    for ( iX = x1; iX < x2; iX++ )  {
      thePainter.drawImage ( (int)( iX - iOffsetX ), (int)( fY - iOffsetY ), brush );
      fY += fSlope;
    }
  }
  else  {
    // Vertically aligned lines
    if ( y1 > y2 )  {
      int t = y2;
         y2 = y1;
         y1 = t;
    }
    int iY;
    fSlope   = 1.0f / fSlope;
    float fX = (float)x1;
    for ( iY = y1; iY < y2; iY++ )  {
      thePainter.drawImage ( (int)( fX - iOffsetX ), (int)( iY - iOffsetY ), brush );
      fX += fSlope;
    }
  }
}

void NicePainter::drawLine ( QPainter &thePainter, QPoint &startPos, QPoint &endPos, QImage &brush )
{
  if ( m_bFastDraw )  {
    thePainter.drawLine ( startPos, endPos );
    return;
  }

  int iDeltaX  = startPos.x ( ) - endPos.x ( );
  int iDeltaY  = startPos.y ( ) - endPos.y ( );
  float fSlope = 9999999.9f;
  if( iDeltaX != 0 )
        fSlope = (float)iDeltaY / (float)iDeltaX;
  int iOffsetX = (int)( (float)brush.width  ( ) / 2.0 );
  int iOffsetY = (int)( (float)brush.height ( ) / 2.0 );

  if ( ( fSlope < 1.0 ) && ( fSlope > -1.0 ) ) {
    // Horizontally aligned lines
    int  x1 = startPos.x ( );
    int  x2 = endPos.x   ( );
    if ( x1 > x2 )  {
         x2 = startPos.x ( );
         x1 = endPos.x   ( );
    }
    int   iX;
    float fY = (float)startPos.y ( );
    for ( iX = x1; iX < x2; iX++ )  {
      thePainter.drawImage ( (int)( iX - iOffsetX ), (int)( fY - iOffsetY ), brush );
      fY += fSlope;
    }
  }
  else  {
    // Vertically aligned lines
    int  y1 = startPos.y ( );
    int  y2 = endPos.y   ( );
    if ( y1 > y2 )  {
         y2 = startPos.y ( );
         y1 = endPos.y   ( );
    }
    int iY;
    fSlope   = 1.0f / fSlope;
    float fX = (float)startPos.x ( );
    for ( iY = y1; iY < y2; iY++ )  {
      thePainter.drawImage ( (int)( fX - iOffsetX ), (int)( iY - iOffsetY ), brush );
      fX += fSlope;
    }
  }
}

////////////////////////////////////////////////////////////
//
// BSpline deBoor class
//
// Currently not used as the results look rather awefull.
//
////////////////////////////////////////////////////////////
BSpline::Point3D::Point3D ( )
{
  m_point[0] = 0;
  m_point[1] = 0;
  m_point[2] = 0;
}

BSpline::Point3D::Point3D ( int x, int y, int z )
{
  m_point[0] = x;
  m_point[1] = y;
  m_point[2] = z;
}

BSpline::Point3D BSpline::Point3D::operator = ( BSpline::Point3D other )
{
  m_point[0] = other.x ( );
  m_point[1] = other.y ( );
  m_point[2] = other.z ( );
  return *this;
}

BSpline::Point3D BSpline::Point3D::operator + ( BSpline::Point3D other )
{
  Point3D point ( m_point[0] + other.x ( ),
                  m_point[1] + other.y ( ),
                  m_point[2] + other.z ( ) );
  return point;
}

BSpline::Point3D BSpline::Point3D::operator * ( double fScalar )
{
  Point3D point ( (int)( m_point[0] * fScalar ),
                  (int)( m_point[1] * fScalar ),
                  (int)( m_point[2] * fScalar ) );
  return point;
}

int BSpline::Point3D::x ( )
{
  return m_point[0];
}

int BSpline::Point3D::y ( )
{
  return m_point[1];
}

int BSpline::Point3D::z ( )
{
  return m_point[2];
}


BSpline::BSpline ( )
{
  m_pNodeVector    = NULL;
  m_pControlPoints = NULL;
  setParms ( 2, 0 );
}

BSpline::BSpline ( int iCurveDegree, int iControlPoints )
{
  m_pNodeVector    = NULL;
  m_pControlPoints = NULL;
  setParms ( iCurveDegree, iControlPoints );
}

BSpline::~BSpline ( )
{
  clear ( );
}

void BSpline::clear ( )
{
  if ( m_pNodeVector )
    delete []m_pNodeVector;
  m_pNodeVector = NULL;
  if ( m_pControlPoints )
    delete []m_pControlPoints;
  m_pControlPoints = NULL;
  m_iControlPoints = 0;
  m_iCurveDegree   = 2;
}

void BSpline::setParms ( int iCurveDegree, int iControlPoints )
{
  clear ( );
  m_iCurveDegree   = iCurveDegree;
  m_iControlPoints = iControlPoints;
  if ( iControlPoints > 0 )  {
    m_pNodeVector    = new int[iCurveDegree + iControlPoints + 1];
    m_pControlPoints = new Point3D[iControlPoints + 1];
  }
}

void BSpline::set ( int iIdx, int x, int y, int z )
{
  if ( ( ! m_pControlPoints ) || ( iIdx > m_iControlPoints ) )
   return;
  m_pControlPoints[iIdx] = Point3D ( x, y, z );
}

void BSpline::drawLines ( QPainter &thePainter, NicePainter *p, QImage &brush )
{
  Point3D startPos, endPos;
  startPos = m_pControlPoints[0];
  for ( double i=0; i<m_pNodeVector[m_iCurveDegree + m_iControlPoints]; i += 0.1 )  {
    for (  int j=0; j<m_iControlPoints; j++ )  {  // until  every cP is passed
      if (   i >= j )
         endPos = deBoor ( m_iCurveDegree, j, i );
    }
    p->drawLine ( thePainter, startPos.x ( ), startPos.y ( ), endPos.x ( ), endPos.y ( ), brush );
//    thePainter.drawLine ( startPos.x ( ), startPos.y ( ), endPos.x ( ), endPos.y ( ) );
    startPos = endPos;
  }
}

BSpline::Point3D BSpline::deBoor ( int r, int i, double u )
{
  if ( r == 0 )
    return m_pControlPoints[i];

  // pre calculations
  double pre = ( u - m_pNodeVector[i + r] ) / ( m_pNodeVector[i + m_iCurveDegree + 1] - m_pNodeVector[i + r] );
  Point3D p1 = deBoor ( r - 1, i,     u ) * ( 1 - pre );
  Point3D p2 = deBoor ( r - 1, i + 1, u ) * (     pre );
  return ( p1 + p2 );
}

void BSpline::createNodeVector ( )
{
  int knoten = 0;
  for ( int i=0; i<( m_iCurveDegree + m_iControlPoints + 1 ); i++ )  {  // n+m+1 = nr of nodes
    if ( i > m_iCurveDegree )  {
      if ( i <= m_iControlPoints )
        m_pNodeVector[i] = ++knoten; 
      else
        m_pNodeVector[i] = knoten;
    }
    else
      m_pNodeVector[i] = knoten;
  }
}

}; // End namespace DVDLayout
