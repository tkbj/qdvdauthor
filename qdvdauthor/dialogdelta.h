/***************************************************************************
    dialogdelta.h
                             -------------------
    DialogAudio
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0

    This class encapsulates the Dialog to define the delta between two
    groups in the DialogFiles / DialogImages dialogs.

****************************************************************************/

#ifndef DIALOGDELTA_H
#define DIALOGDELTA_H

//#include "uidialogdelta.h"		//ooo
#include "ui_uidialogdelta.h"		//xxx

//class DialogDelta : public uiDialogDelta			//ooo
class DialogDelta : public QDialog, Ui::uiDialogDelta		//xxx
{
  Q_OBJECT

public:
           //DialogDelta ( QWidget *, bool, unsigned int, unsigned int );						//ooo
	   DialogDelta ( QWidget *pParent, bool bImageDialog, unsigned int iGroupDelta, unsigned int iMinImages );	//xxx
  virtual ~DialogDelta ( );

  unsigned int groupDelta ( );
  unsigned int minNumber  ( );

private:
  void initMe  ( bool );

protected slots:
  virtual void accept  (      );
  virtual void slotMin ( bool );

private:
  unsigned int m_iGroupDelta;
  unsigned int m_iMinImages;
};

#endif // DIALOGDELTA_H
