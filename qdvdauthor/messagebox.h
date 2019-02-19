/***************************************************************************
    messagebox.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class was neccesary when all static functions from QMessageBox
   seemed to behave out of order on SuSE 10.x (KDE only)
   E.g. the return val of
   QMessageBox::warning (..., QMessageBox::Yes, QMessageBox::No );
   was 0 or 1, instead of QMessageBox::[Yes or No].
   Other statics such as QFileDialog::getFileName(..) expanded way to long
    
****************************************************************************/

//#include <qmessagebox.h>	//ooo
#include <QMessageBox>		//xxx

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

class MessageBox
{
   MessageBox ();
  ~MessageBox ();
public:
  static int information ( QWidget *parent, const QString &caption, const QString& text, int button0, int button1=0, int button2=0 );
  static int information ( QWidget *parent, const QString &caption, const QString& text, const QString &button0Text=QString::null, const QString &button1Text=QString::null, const QString &button2Text=QString::null, int defaultButtonNumber=0, int escapeButtonNumber=-1 );
  static int question    ( QWidget *parent, const QString &caption, const QString& text, int button0, int button1=0, int button2=0 );
  static int question    ( QWidget *parent, const QString &caption, const QString& text, const QString &button0Text=QString::null, const QString &button1Text=QString::null, const QString &button2Text=QString::null, int defaultButtonNumber=0, int escapeButtonNumber=-1 );
  static int warning     ( QWidget *parent, const QString &caption, const QString& text, int button0, int button1, int button2=0 );
  static int warning     ( QWidget *parent, const QString &caption, const QString& text, const QString &button0Text=QString::null, const QString &button1Text=QString::null, const QString &button2Text=QString::null, int defaultButtonNumber=0, int escapeButtonNumber=-1 );
  static int html        ( QWidget *parent, const QString &caption, const QString& text, int b1=QMessageBox::Ok, int b2=Qt::NoButton );
  static int html        ( QWidget *parent, const QString &caption, const QString& text, QSize s, int b1=QMessageBox::Ok, int b2=Qt::NoButton );

  static int help        ( QWidget *parent, const QString &caption, const QString& text, QSize s, int b1=QMessageBox::Ok, int b2=Qt::NoButton );
};

#endif // MESSAGEBOX_H


