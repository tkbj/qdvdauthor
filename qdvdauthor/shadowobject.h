/***************************************************************************
    shadowobject.h
                             -------------------
    Shadow object
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will create a shadow of the associated MenuObject
    
****************************************************************************/

#ifndef SHADOW_H
#define SHADOW_H

#include <qimage.h>
//Added by qt3to4:
#include <QMouseEvent>
//#include <Q3ValueList>	//oxx

#include "imageobject.h"

class ShadowObject : public ImageObject
{
  Q_OBJECT

  friend class SelectionObject;

  enum enMType {
    MIRROR_TOP = 0,
    MIRROR_LEFT,
    MIRROR_RIGHT,
    MIRROR_BOTTOM,
  };

public:
  enum enType { 
    TYPE_SHADOW,     // Shadow under object
    TYPE_REFLECTION, // Shadow reflection (shades of grey)
    TYPE_MIRROR      // Object is mirrored and stretched accordingly
  };

  ShadowObject ( MenuObject *, QWidget *p=0 );
  virtual ~ShadowObject ( );

  virtual void drawContents         ( QPainter      *, int, int);
  virtual void drawContents         ( QPainter      * );
  virtual void drawShadow           ( QPainter      * );
  virtual void drawMirror           ( QPainter      * );
  virtual void drawReflection       ( QPainter      * );
  static  void blurImage            ( QImage &,   int );

  //virtual bool mouseMoveEvent       ( QMouseEvent * );
  virtual bool mousePressEvent      ( QMouseEvent   * );
  virtual bool mouseReleaseEvent    ( QMouseEvent   * );
  virtual bool mouseDoubleClickEvent( QMouseEvent   * );

  virtual bool readProjectFile      ( QDomNode      & );
  virtual bool writeProjectFile     ( QDomElement   & );
  virtual bool createContextMenu    ( QPoint   );
  virtual StructureItem *createStructure ( StructureItem * );
  virtual MenuObject    *clone      ( QWidget *p1=NULL, MenuObject *p2=NULL );
  virtual void updateShadow         ( bool );
  virtual void updateShadow         ( );

  ShadowObject &operator =          ( ShadowObject & );
  bool operator         ==          ( ShadowObject & );

  bool filterColorKeys  ( QImage      & );

  void setType          ( enType );
  void setTransparency  ( float  );
  void setBlur          ( int    );
  void setDistance      ( float  );
  void setSunAngle      ( float  );
  void setColor         ( QColor );
  void setShear         ( float, float );
  void setScale         ( float, float );
  void setCutOff        ( float, float );
  void setColorFading   ( QColor, float );

  float   transparency  ( ); // [ 0.0 .. 1.0 ]
  enType  type          ( );
  enMType mirrorType    ( );
  int     blur          ( );
  float   distance      ( );
  float   sunAngle      ( );
  QColor  &color        ( );
  float   shearX        ( );
  float   shearY        ( );
  float   scaleX        ( );
  float   scaleY        ( );
  float   cutOff        ( );
  float   cutOffRadius  ( );
  float   colorFading   ( );
  QColor  &colorFadeTo  ( );

  int deltaX            ( );
  int deltaY            ( );

protected:
  // This function will return Object specific attributes for Animation purposes
  virtual AnimationAttribute *getSpecificAttributes (long, QString);

private:
  bool filterColorKey ( QColor *, int, QList<QColor *> *, QList<QColor *>* );		//oxx
  void blurShadow        ( );
  void applyTransparency ( );
  void applyColorFading  ( );
  void createShadow   ( MenuObject *, int       );
  void createMirror   ( MenuObject *, int, bool );
  void filterColor    ( QImage &,     QColor    );
  void cutOffImage    ( QImage &,     enMType   );
  void drawRedRect    ( QImage &);

  MenuObject  *m_pMenuObject; // the object which has the shadow
  QImage       m_imageShadow; // the shadow is staored as a QImage - object
  QImage       m_imageBuffer; // buffers the shadow before transformations and transp.
  enType       m_enType;

  // Shadow attributes
  QColor m_color;
  float  m_fTransparency; // [ 0.0 .. 1.0 ]
  float  m_fDistance;     //  
  int    m_iBlurRadius;   //  [ 3 .. 23 ]
  float  m_fSunAngle;     //  in degrees
  const float m_fSunElevation; // default to 45% non changeable

  // the following variables are for Mirror type shadows.
  float  m_fShearX;
  float  m_fShearY;
  float  m_fScaleX;
  float  m_fScaleY;
  float  m_fCutOff;
  float  m_fCutOffRadius;
  float  m_fColorFading;
  QColor m_colorFadeTo;

  // Calculated values ...
  int m_iDeltaX;
  int m_iDeltaY;
};

#endif // SHADOWOBJECT_H

