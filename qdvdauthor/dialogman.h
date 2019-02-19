/***************************************************************************
    dialogman.h
                             -------------------
    Execute class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class provide a GUI to the man pages.
    
****************************************************************************/

#ifndef DIALOGMAN_H
#define DIALOGMAN_H

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>

class QPushButton;
class QTabWidget;
class QTextEdit;
class QLineEdit;

class DialogMan : public QDialog
{
    Q_OBJECT

public:
     //DialogMan( QStringList &commands, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );	//ooo
     DialogMan( QStringList &commands, QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0 );	//xxx
    ~DialogMan();

protected:
    
protected slots:
  virtual void slotFind ();
  virtual void languageChange();
  
protected:
  QPushButton *buttonHelp;
  QPushButton *buttonFind;
  QPushButton *buttonOk;
  QPushButton *buttonCancel;
  QLineEdit   *m_pEditFind;
  QTabWidget  *tabWidget2;
  
  QGridLayout* MyDialogLayout;
  QHBoxLayout* Layout1;
  QList <QTextEdit *> m_listTextEdit;
};

#endif // DIALOGMAN_H
