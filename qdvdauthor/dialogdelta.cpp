/***************************************************************************
    dialogdelta.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
    This class encapsulates the Dialog to define the delta between two
    groups in the DialogFiles / DialogImages dialogs.
    
****************************************************************************/

#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
//#include <q3datetimeedit.h>   //ooo
#include <QDateTimeEdit>        //xxx

#include "dialogdelta.h"

DialogDelta::DialogDelta ( QWidget *pParent, bool bImageDialog, unsigned int iGroupDelta, unsigned int iMinImages )
  //: uiDialogDelta ( pParent )		//ooo
  : QDialog ( pParent )			//xxx
{
  setupUi(this);
  
  m_iGroupDelta = iGroupDelta; // in seconds
  m_iMinImages  = iMinImages;
  initMe ( bImageDialog );
}

DialogDelta::~DialogDelta ( )
{
}

void DialogDelta::initMe ( bool bImageDialog )
{
  QString qsLabel, qsCheck;
  int     iSeconds, iDays;
  double  fValue = (double)m_iGroupDelta;

  iDays    = (int)( fValue / ( 24.0 * 60.0 * 60.0 ) );
  fValue  -= ( 24.0 * 60.0 * 60.0 * iDays );
  iSeconds = (int)fValue;

  QTime deltaTime;
  deltaTime = deltaTime.addSecs ( iSeconds );

  m_pSpinDeltaDays->setValue ( iDays );
  m_pDeltaTime->setTime  ( deltaTime );

  if ( m_iMinImages > 1 )  {
    m_pSpinMin->setEnabled ( true );
    m_pCheckMin->setChecked( true );
    m_pSpinMin->setValue   ( m_iMinImages );
  }

  if ( bImageDialog )  {
    qsLabel = tr ( "<b>Please enter the time difference between the first image and the last image in a Group.</b>" );
    qsCheck = tr ( "Min number of Images per group :" );
  }
  else  {
    qsLabel = tr ( "<b>Please enter the time difference between the first vidoe and the last video in a Group.</b>" );
    qsCheck = tr ( "Min number of Videos per group :" );
  }
  m_pLabelHeader->setText ( qsLabel );
  m_pCheckMin->setText    ( qsCheck );

  connect ( m_pCheckMin, SIGNAL ( toggled ( bool ) ), this, SLOT ( slotMin ( bool ) ) );
  connect ( buttonOk,     SIGNAL ( clicked ( ) ), this, SLOT ( accept ( ) ) );           //xxx
  connect ( buttonCancel, SIGNAL ( clicked ( ) ), this, SLOT ( reject     ( ) ) );       //xxx
}

unsigned int DialogDelta::groupDelta ( )
{
  return m_iGroupDelta;
}

unsigned int DialogDelta::minNumber ( )
{
  return m_iMinImages;
}

void DialogDelta::slotMin ( bool bOn )
{
  m_pSpinMin->setEnabled ( bOn );
}

void DialogDelta::accept ( )
{
  int iDays = m_pSpinDeltaDays->value ( );
  if ( m_pCheckMin->isChecked ( ) )
    m_iMinImages = m_pSpinMin->value ( );
  else
    m_iMinImages = 1;

  QTime zeroTime, deltaTime = m_pDeltaTime->time ( );

  m_iGroupDelta  = iDays * 24 * 60 * 60;
  m_iGroupDelta += zeroTime.secsTo ( deltaTime );
  QDialog::accept ( );
}
