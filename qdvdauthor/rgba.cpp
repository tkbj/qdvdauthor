/***************************************************************************
    rgba.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   Rgba class to encapsulate color plus Alpha channel.

****************************************************************************/

#include <QMessageBox>

#include "rgba.h"


Rgba::Rgba ( )
{
  m_color = 0xFFFFFFFF;
}

Rgba::Rgba ( QRgb color )
{
  m_color = color;
}

Rgba::Rgba ( QColor theColor )
{
  m_color = theColor.rgb ( );
}

Rgba::Rgba ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
  m_color = ( alpha << 24 ) + ( red << 16 ) + ( green << 8 ) + blue;
  if ( m_color == 0xFFFEFFFE ) // previous TRANSPARENT_COLOR
       m_color  = 0x00FEFFFE;
}

Rgba::~Rgba ( )
{
}
 
Rgba Rgba::operator = ( Rgba theOther )
{
  setRgb ( theOther.rgb ( ) );
  return *this;
}
 
bool Rgba::operator == ( Rgba theOther )
{
  return ( rgb ( ) == theOther.rgb ( ) );
}
 
bool Rgba::operator != ( Rgba theOther )
{
  return ( rgb ( ) != theOther.rgb ( ) );
}
 
QRgb Rgba::rgb ( )
{ 
  return m_color;
}

QColor Rgba::color ( )
{
  QColor theColor ( m_color );
  return theColor;
}

void Rgba::setRgb ( QRgb theColor )
{
  m_color = theColor;
}

void Rgba::setRgb ( Rgba theColor )
{
  m_color = theColor.rgb ( );
}

void Rgba::setRgb ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
  m_color = ( alpha << 24 ) + ( red << 16 ) + ( green << 8 ) + blue;
  //  if ( m_color == 0xFFFEFFFE )
  //       m_color  = 0x00FEFFFE;
}

QString Rgba::toString ( )
{
  QString strColor = QString ( "#%1" ).arg ( (unsigned int)m_color, 8, 16 );
  strColor = strColor.replace ( " ", "0" );
  return  strColor;
}

bool Rgba::fromString ( QString strColor )
{
  // Handle "#AARRGGBB" as well as "AARRGGBB" as well as "ARGB" as well as "#RRGGBB" as well as "RRGGBB" as well as "RGB"
  QRgb rgba  = 0x00FFFFFF;
  QString cleanColor;
  strColor   = strColor.remove ( "#" );
  //strColor   = strColor.upper ( );	//ooo
  strColor   = strColor.toUpper ( );	//xxx
  cleanColor = strColor;
  if ( strColor.length ( ) == 3 ) {
    // Format = RGB -> convert to AARRGGBB
    cleanColor = "00" + strColor.mid(0,1)+strColor.mid(0,1) + strColor.mid(1,1)+strColor.mid(1,1) + strColor.mid(2,1)+strColor.mid(2,1);
  }
  else if ( strColor.length ( ) == 4 ) {
    // Format = ARGB -> convert to AARRGGBB
    cleanColor = strColor.mid(0,1)+strColor.mid(0,1) + strColor.mid(1,1)+strColor.mid(1,1) + strColor.mid(2,1)+strColor.mid(2,1) + strColor.mid(3,1)+strColor.mid(3,1);
  }
  else if ( strColor.length ( ) == 6 ) {
    if ( "FEFFFE" == cleanColor )
      cleanColor = "00FEFFFE"; // Special handling to load TRANSPARENT_COLOR
    else
      cleanColor = "FF" + strColor;
  }

  if ( cleanColor.length ( ) == 8 ) {
    int iVal = 0;
    bool bOkay;
    QString strVal = cleanColor.mid(0,2);
    // here we have AARRGGBB. Let us create the Rgba value
    iVal    = strVal.toInt ( &bOkay, 16 ); // gets the Alpha Channel
    if ( bOkay )
      rgba  = iVal << 24;
    else
      return false;
    
    strVal  = cleanColor.mid(2,2);
    iVal    = strVal.toInt ( &bOkay, 16 ); // gets the Red channel
    if ( bOkay )
      rgba += iVal << 16;
    else
      return false;

    strVal  = cleanColor.mid(4,2);
    iVal    = strVal.toInt ( &bOkay, 16 ); // gets the Green channel
    if ( bOkay )
      rgba += iVal << 8;
    else
      return false;

    strVal  = cleanColor.mid(6,2);
    iVal    = strVal.toInt ( &bOkay, 16 ); // gets the Blue channel
    if ( bOkay )
      rgba += iVal;    
    else
      return false;
  }
  m_color = rgba;
  return true;
}

unsigned char Rgba::red ( )
{
  return ( m_color & 0x00FF0000 ) >> 16;
}

unsigned char Rgba::green ( )
{
  return ( m_color & 0x0000FF00 ) >> 8;
}

unsigned char Rgba::blue ( )
{
  return ( m_color & 0x000000FF );
}

unsigned char Rgba::alpha ( )
{
  return ( m_color & 0xFF000000 ) >> 24;
}

void Rgba::setRed ( unsigned char red )
{
  m_color = ( m_color & 0xFF00FFFF ) | ( red << 16 );
}

void Rgba::setGreen ( unsigned char green )
{
  m_color = ( m_color & 0xFFFF00FF ) | ( green << 8 );
}

void Rgba::setBlue ( unsigned char blue )
{
  m_color = ( m_color & 0xFFFFFF00 ) | ( blue );
}

void Rgba::setAlpha ( unsigned char alpha )
{
  m_color = ( m_color & 0x00FFFFFF ) | ( alpha << 24 );
}






