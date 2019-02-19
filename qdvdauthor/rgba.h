/***************************************************************************
    rgba.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will superceed the QColor class and add an Alpha channel
   to it.
    
****************************************************************************/

#ifndef RGBA_H
#define RGBA_H

#include <qcolor.h>

class Rgba
{
 public:
           Rgba ( );
           Rgba ( QRgb   );
           Rgba ( QColor );
           Rgba ( unsigned char r, unsigned char g, unsigned char b, unsigned char a=0xFF );
  virtual ~Rgba ( );

  Rgba operator  = ( Rgba );
  bool operator == ( Rgba );
  bool operator != ( Rgba );

  void     setRgb     ( QRgb );
  void     setRgb     ( Rgba );
  void     setRgb     ( unsigned char r, unsigned char g, unsigned char b, unsigned char a=0xFF );

  QString toString    ( );
  bool    fromString  ( QString );

  QRgb          rgb   ( );
  QColor        color ( );
  unsigned char red   ( );
  unsigned char green ( );
  unsigned char blue  ( );
  unsigned char alpha ( );

  void setRed   ( unsigned char );
  void setGreen ( unsigned char );
  void setBlue  ( unsigned char );
  void setAlpha ( unsigned char ); // QImage : 0xFF = opaque : 0x00 = transparent

protected:
  QRgb  m_color; // unsigned int ( #AARRGGBB )
};


#endif // RGBA_H


