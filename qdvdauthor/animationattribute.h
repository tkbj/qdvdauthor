/****************************************************************************
** Interface to the GUI.
**
**   Created : Mon Jun 10 22:48:00 2005
**        by : Varol Okan using kate editor
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#ifndef ANIMATIONATTRIBUTE_H
#define ANIMATIONATTRIBUTE_H

#include <qstring.h>
#include <qpoint.h>

#include "rgba.h"

#define TYPE_COLOR_RED   0
#define TYPE_COLOR_GREEN 1 
#define TYPE_COLOR_BLUE  2
#define TYPE_COLOR_ALPHA 3
#define TYPE_POSITION_X  4
#define TYPE_POSITION_Y  5

// Needed to access the function of the stored function pointer to a function of a class
class CallbackClass
{
public:
  virtual ~CallbackClass () {}; 
  // The member function(s) will take and return an int
  virtual int operator()( int ) = 0;
}; 

// Basically needed to store a function pointer to a class member function.
template <class T>
class FunctionCallback : public CallbackClass
{
public:
  // define function prototypes ...
  typedef void   (T::*Fn)        ( int );
  typedef bool   (T::*Fn2)       ( const QString & );
  typedef void   (T::*Fn3)       ( QPoint & );
  typedef QPoint (T::*FnGetPos ) ( );

  FunctionCallback( T& obj, Fn fn )
    : m_obj( obj ), m_fn( fn ), m_iWhichType ( -1 )
  {};
  FunctionCallback( T& obj, Fn2 fn2 )
    : m_obj( obj ), m_fn2( fn2 ), m_iWhichType ( -1 )
  {};
  FunctionCallback( T& obj, Fn3 fn3, FnGetPos fnGetPos, int c )
    : m_obj( obj ), m_fn3( fn3 ), m_fnGetPos( fnGetPos ), m_iWhichType ( c )
  {};
  FunctionCallback( T& obj, int c )
    : m_obj( obj ), m_iWhichType ( c )
  {};
  virtual ~FunctionCallback () 
  {};

  virtual int operator()( int n )
  {
    // call the function on the object
    if (m_iWhichType == -1)	{
      (m_obj.*m_fn)( n );
    }
    else if ( (m_iWhichType >= TYPE_COLOR_RED) && (m_iWhichType <= TYPE_COLOR_BLUE) )	{// Special handling for QColor
      Rgba &theColor = ((Rgba &)m_obj);
      int r = theColor.red   ( );
      int g = theColor.green ( );
      int b = theColor.blue  ( );
	  int a = theColor.alpha ( );
      switch (m_iWhichType)	{
      case TYPE_COLOR_RED:   r = n; break;
      case TYPE_COLOR_GREEN: g = n; break;
      case TYPE_COLOR_BLUE:  b = n; break;
	  case TYPE_COLOR_ALPHA: a = n; break;
      }
      theColor.setRgb ( r, g, b, a );
      //		(m_obj.*m_colorFn)( n, ((QColor &)m_obj).green(), ((QColor &)m_obj).blue() );
    }
    else if ( ( m_iWhichType == TYPE_POSITION_X ) || (  m_iWhichType == TYPE_POSITION_Y ) ) {
      QPoint thePosition = (m_obj.*m_fnGetPos)( );
      int iX = thePosition.x ( );
      int iY = thePosition.y ( );
      if ( m_iWhichType == TYPE_POSITION_X )
	iX = n;
      if ( m_iWhichType == TYPE_POSITION_Y )
	iY = n;
      thePosition = QPoint ( iX, iY );
      (m_obj.*m_fn3)( thePosition );
    }
    return 0;
  }
  virtual void operator()( QString &str )
  {
    // call the function on the object
    (m_obj.*m_fn2)( str );
  }
  
private:
    T&       m_obj; 	   // object reference
    Fn       m_fn;  	   // function pointer
    Fn2      m_fn2;  	   // function pointer
    Fn3      m_fn3;  	   // function pointer
    FnGetPos m_fnGetPos;   // function to get value
    int      m_iWhichType; // which color to set 
}; 

class AnimationAttribute
{
public:
	 AnimationAttribute (long, QString, int  ,   CallbackClass *);
	 AnimationAttribute (long, QString, QString, CallbackClass *);
	 AnimationAttribute (long, QString, int  ,   int   *);
	 AnimationAttribute (long, QString, bool ,   bool  *);
	 AnimationAttribute (long, QString, float,   float *);
	 AnimationAttribute (long, QString, double,  double *);
	 AnimationAttribute (long, QString, QString, QString *);
	~AnimationAttribute ();

	void    initMe     (long, QString);
	void    append     (long, QString);
	const   QString name  ( );
	bool    setValue   (long);
	int     iValue     (long);
	bool    bValue     (long);
	double  dValue     (long);
	float   fValue     (long);
	QString qsValue    (long);
	long    maxFrames  ( );

	// the following two functions indicate that the Image needs to be reloaded etc.
	bool    needRefresh    ( );
	void    setNeedRefresh ( bool );

private:
	bool           m_bNeedRefresh;
	int           *m_piValue;	// Pointer to the real deal (if int)
	bool          *m_pbValue;	// Pointer to the real deal (if int)
	float         *m_pfValue;	// pointer to the real deal (if float)
	double        *m_pdValue;	// pointer to the real deal (if double)
	QString       *m_pqsValue;	// pointer to the real deal (if string)
	CallbackClass *m_setFunction;	// Pointer to function to set value (E.g. QRect::setX(int))

	int           *m_arrayInts;	// Holds the values (if int)
	bool          *m_arrayBools;	// Holds the values (if bool)
	float         *m_arrayFloats;	// holds the values (if float)
	double        *m_arrayDoubles;	// holds the values (if double)
	QString       *m_arrayStrings;	// Holds the values (if string)
	QString        m_qsName;	// is the property name (E.g. rect.x1 or zoom)
	int            m_iMaxFrames;	// Number of frames (also size of the array)
};

#endif // ANIMATIONATTRIBUTE_H

