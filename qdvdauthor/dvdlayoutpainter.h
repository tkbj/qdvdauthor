/***************************************************************************
    dvdlayoutpainter.h
                             -------------------
    Class DVDLayout::Router
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class handles the display area for the DVD Layout tab.
  
****************************************************************************  
    
   The DVD Layout must handle
   o DVDMenuObjects
   o MovieObjects
   o FirstObject ( FPC )
   o PreObjects / PostObjects
    
****************************************************************************/

#ifndef DVDLAYOUTPAINTER_H
#define DVDLAYOUTPAINTER_H

#include "dvdlayoutarea.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

namespace DVDLayout
{

class Object;
class NicePainter;

class BSpline
{
  class Point3D
  {
  public:
    Point3D ( );
    Point3D ( int, int, int );
    int   x ( );
    int   y ( );
    int   z ( );
    Point3D operator = ( Point3D );
    Point3D operator + ( Point3D );
    Point3D operator * ( double  );

  private:
    int m_point[3];
  };
  public:
     BSpline ( int, int );
     BSpline ( );
    ~BSpline ( );

    void    clear            ( );
    void    setParms         ( int, int );
    void    set              ( int, int, int, int );
    Point3D deBoor           ( int, int,   double );
    void    createNodeVector ( );
    void    drawLines        ( QPainter &, NicePainter *, QImage & );

  private:

    int       m_iCurveDegree;   // degree of the courve (degree <= controllPointsNr)
    int       m_iControlPoints; // number of controll points
    int      *m_pNodeVector;    // node vector
    Point3D  *m_pControlPoints; // controll points (array)

    Point3D m_temp;    // temp Vector (for BSplineLine)
    Point3D m_temp2;
};

class SimplePainter
{
public:
  SimplePainter ( );
  virtual ~SimplePainter  ( );

  virtual void setFastDraw ( bool ) { };
  virtual void draw ( QPainter &, Area::Container *, Object *p=NULL );
  virtual void draw ( QPainter &, Area::Container::Connection * );

  virtual void drawLines ( QPainter &, QList<Area::PieceOfLine *> & );		//oxx
  void drawArrow ( QPainter &, Area::PieceOfLine * );
  virtual void paintEvent           ( QPaintEvent * );       //xxx
};

class NicePainter : public SimplePainter
{
public:
  class Brush
  {
  public:
    Brush ( QRgb & );

    QRgb   m_brushColor;
    QImage m_innerBrush;
    QImage m_outerBrush;
  };

  NicePainter ( );
  virtual ~NicePainter ( );

  virtual void setFastDraw ( bool );
  virtual void draw ( QPainter &, Area::Container *, Object *p=NULL );
  virtual void draw ( QPainter &, Area::Container::Connection * );

  virtual void drawLines ( QPainter &, QList<Area::PieceOfLine *> & );		//oxx

private:
  void   drawLine ( QPainter &, int, int, int, int, QImage & );
  void   drawLine ( QPainter &, QPoint &, QPoint &, QImage & );
  Brush *getBrush ( QRgb   & );
  void   drawArrow ( QPainter &, Area::PieceOfLine *, QImage & );

private:
  QList<Brush *> m_listBrush;		//oxx
  Brush *m_pCurrentBrush;
  bool   m_bFastDraw;
  friend class BSpline;
};

}; // End of namespace DVDLayout

#endif // DVDLAYOUTPAINTER_H
