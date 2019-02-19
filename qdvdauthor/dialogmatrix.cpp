/***************************************************************************
    dialogmatrix.cpp
                             -------------------
    DialogMatrix class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QtGui>
#include <qmatrix.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>

#include "dialogmatrix.h"
#include "menuobject.h"
#include "imageobject.h"
#include "movieobject.h"

DialogMatrix::DialogMatrix(QWidget *pParent)
	: QDialog(pParent)
{
	setupUi(this);
}

DialogMatrix::~DialogMatrix()
{
}

void DialogMatrix::slotClose()
{
  accept ( );
  QTimer::singleShot ( 10, m_pParent, SLOT ( slotDeleteMtxDlg ( ) ) );
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogMatrix::languageChange()
{
    retranslateUi(this);
}

void DialogMatrix::slotZoom ( int iPos )
{
  //double fZoom   = (double) ( iPos / 1000.0 );            //ooo
  double fZoom   = static_cast<double> ( iPos / 1000.0 );   //xxx
  m_pEditZoom->setText ( QString ("%1").arg ( fZoom ) );
  m_pParent->modifiers ( )->fScaleX = fZoom;
  m_pParent->modifiers ( )->fScaleY = fZoom;

  m_pParent->update    ( );
}

void DialogMatrix::slotRotate(int iPos)
{
  //double fRotate = (double)(iPos / 10.0);          //ooo
  double fRotate = static_cast<double>(iPos / 10.0);  //xxx
  m_pEditRotate->setText (QString ("%1").arg(fRotate));
  m_pParent->modifiers()->fRotate = fRotate;
  m_pParent->update();
}

void DialogMatrix::slotShearX(int iPos)
{
	//double fShear = (double)(iPos / 1000.0);         //ooo
        double fShear = static_cast<double>(iPos / 1000.0); //xxx
	m_pEditShearX->setText (QString ("%1").arg(fShear));
	m_pParent->modifiers()->fShearX = fShear;
	m_pParent->update();
}


void DialogMatrix::slotShearY (int iPos)
{
	//double fShear = (double)(iPos / 1000.0);             //ooo
        double fShear = static_cast<double> (iPos / 1000.0);    //xxx
	m_pEditShearY->setText (QString ("%1").arg(fShear));
	m_pParent->modifiers()->fShearY = fShear;
	m_pParent->update();
}


void DialogMatrix::initMe ( MenuObject *pParent )
{
	m_pParent = pParent;
	// Here we set the matrix values ...
	m_pSliderRotate->setRange ( -1800, +1800 );
	m_pSliderZoom  ->setRange (     0, +2000 );
	m_pSliderShearX->setRange ( -2000, +2000 );
	m_pSliderShearY->setRange ( -2000, +2000 );
	//m_pSliderRotate->setValue ( (int)( pParent->modifiers ( )->fRotate *   10.0 ) ); // 0            //ooo
        m_pSliderRotate->setValue ( static_cast<int>( pParent->modifiers ( )->fRotate *   10.0 ) ); // 0    //xxx
	//m_pSliderZoom  ->setValue ( (int)( pParent->modifiers ( )->fScaleX * 1000.0 ) ); // 1000         //ooo
        m_pSliderZoom  ->setValue ( static_cast<int>( pParent->modifiers ( )->fScaleX * 1000.0 ) ); // 1000 //xxx
	//m_pSliderShearX->setValue ( (int)( pParent->modifiers ( )->fShearX * 1000.0 ) ); // 1000         //ooo
        m_pSliderShearX->setValue ( static_cast<int>( pParent->modifiers ( )->fShearX * 1000.0 ) ); // 1000 //xxx
	//m_pSliderShearY->setValue ( (int)( pParent->modifiers ( )->fShearY * 1000.0 ) ); // 1000         //ooo
        m_pSliderShearY->setValue ( static_cast<int>( pParent->modifiers ( )->fShearY * 1000.0 ) ); // 1000 //xxx
}

void DialogMatrix::slotSliderPressed ( )
{
  // Initiate the fast drawing algol of the MenuObject ...
  //QMouseEvent fakeMouseEvent ( QEvent::MouseButtonPress, QPoint ( ), Qt::LeftButton, Qt::LeftButton );                //ooo
  QMouseEvent fakeMouseEvent ( QEvent::MouseButtonPress, QPoint ( ), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );  //xxx  
  m_pParent->mousePressEvent ( &fakeMouseEvent );     //oooo
}

void DialogMatrix::slotSliderReleased ( )
{
	//QMouseEvent fakeMouseEvent   ( QEvent::MouseButtonPress, QPoint(), Qt::LeftButton, Qt::LeftButton );                 //ooo
        QMouseEvent fakeMouseEvent   ( QEvent::MouseButtonPress, QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier );    //xxx
	m_pParent->mouseReleaseEvent ( &fakeMouseEvent );    //oooo
	m_pParent->update ( );

	ImageObject theImage;
	MovieObject theMovie;
	if (m_pParent->objectType() == theImage.objectType ( ) )  {
		ImageObject *pImage = (ImageObject *)m_pParent;
		pImage->updatePixmap();
	}
	else if (m_pParent->objectType() == theMovie.objectType())	{
		MovieObject *pMovie = (MovieObject *)m_pParent;
		pMovie->updatePixmap();
	}
}

void DialogMatrix::slotDeleteObject( MenuObject * )
{
  accept ( );
}

void DialogMatrix::slotZoomChanged ( )
{
  bool  bOk    = false;
  float fValue = m_pEditZoom->text().toFloat ( &bOk );
  if ( bOk ) {
    m_pParent->modifiers ( )->fScaleX = fValue;
    m_pParent->modifiers ( )->fScaleY = fValue;
    m_pParent->update    ( );
    //m_pSliderZoom->setValue ( (int) ( fValue * 1000.0  ) );           //ooo
    m_pSliderZoom->setValue ( static_cast<int> ( fValue * 1000.0  ) );  //xxx
  }
}

void DialogMatrix::slotRotateChanged()
{
  bool  bOk    = false;
  float fValue = m_pEditRotate->text().toFloat ( &bOk );
  if ( bOk ) {
    m_pParent->modifiers()->fRotate = fValue;
    m_pParent->update();    
    //m_pSliderRotate->setValue ( (int) ( fValue * 10.0 ) );            //ooo
    m_pSliderRotate->setValue ( static_cast<int> ( fValue * 10.0 ) );   //xxx
  }
}

void DialogMatrix::slotShearXChanged()
{
  bool  bOk    = false;
  float fValue = m_pEditShearX->text().toFloat ( &bOk );
  if ( bOk ) {
    m_pParent->modifiers()->fShearX = fValue;
    m_pParent->update();
    //m_pSliderShearX->setValue ( (int) ( fValue * 1000.0 ) );          //ooo
    m_pSliderShearX->setValue ( static_cast<int> ( fValue * 1000.0 ) ); //xxx
  }
}

void DialogMatrix::slotShearYChanged()
{
  bool  bOk    = false;
  float fValue = m_pEditShearY->text().toFloat ( &bOk );
  if ( bOk ) {
    m_pParent->modifiers()->fShearX = fValue;
    m_pParent->update();
    //m_pSliderShearY->setValue ( (int) ( fValue * 1000.0 ) );          //ooo
    m_pSliderShearY->setValue ( static_cast<int> ( fValue * 1000.0 ) ); //xxx
  }
}

