/***************************************************************************
    overlayobject.h
                             -------------------
    Overlay object
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will create a OverlayObjet ( Mask ) for the associated MenuObject.

****************************************************************************/

#ifndef OVERLAY_OBJECT_H
#define OVERLAY_OBJECT_H

#include <qimage.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "rgba.h"
#include "imageobject.h"
//#include "movieobject.h"	//ooo

class OverlayObject : public ImageObject
{
  Q_OBJECT

public:
  OverlayObject ( MenuObject *, QWidget *p=0 );
  virtual ~OverlayObject ( );

  virtual void drawContents         ( QPainter      *, int, int);
  virtual void drawContents         ( QPainter      * );

  //virtual bool mouseMoveEvent       ( QMouseEvent * );
  virtual bool mousePressEvent      ( QMouseEvent   * );
  virtual bool mouseReleaseEvent    ( QMouseEvent   * );
  virtual bool mouseDoubleClickEvent( QMouseEvent   * );

  virtual bool readProjectFile      ( QDomNode      & );
  virtual bool writeProjectFile     ( QDomElement   & );
  virtual bool createContextMenu    ( QPoint   );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual MenuObject    *clone      ( QWidget *p1=NULL, MenuObject *p2=NULL );

  OverlayObject &operator =         ( OverlayObject & );
  bool operator          ==         ( OverlayObject & );

  void setColor  ( Rgba   & );
  void setOffset ( QPoint & );
  void setOffset ( int, int );
  void setIsMask ( bool     );
  void setRotate ( float    );
  void setScale  ( float, float );
  void setShear  ( float, float );
  QPoint &offset ( );
  Rgba   &color  ( );
  bool   isMask  ( );
  float  scaleX  ( );
  float  scaleY  ( );
  float  shearX  ( );
  float  shearY  ( );
  float  rotate  ( );

  QImage &image      ( QImage & );
  bool   setOverlay  ( QString  );  // fileName
  void   setFastDraw ( bool );
  bool   fastDraw    ( );
//  void   createMaskFrames ( );
//  void   createMaskImages ( );

protected:
  // This function will return Object specific attributes for Animation purposes
  virtual AnimationAttribute *getSpecificAttributes ( long, QString );

private:
  MenuObject *m_pMenuObject;  // the object which has the Overlay
  QImage      m_imageOverlay; // the overlay is stored as a QImage - object
  QImage      m_imageScaled;  // buffers the overlay before transformations and transp.
  bool        m_bFastDraw;    // a bit faster drawing w/o masking etc. for interactivity.

  // Overlay attributes
  Rgba   m_color;
  QPoint m_offset;
  bool   m_bIsMask;
  float  m_fScaleX; // Scaled relative the the parents MenuObject size.
  float  m_fScaleY; // Scaled relative the the parents MenuObject size.
  float  m_fShearX;
  float  m_fShearY;
  float  m_fRotate;
};

#endif // OVERLAY_OBJECT_H

