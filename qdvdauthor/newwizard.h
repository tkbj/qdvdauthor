/***************************************************************************
    newwizard.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#ifndef NEWWIZARD_H
#define NEWWIZARD_H 

#include <QWidget>
#include <QWizard>
#include <QLineEdit>
#include <QPushButton>
#include <QLineEdit>

class TempPath;
class DVDOutputPath;

class NewWizard : public QWizard
{
     Q_OBJECT
public:
   NewWizard(QWidget *pParent = 0);
   
   void accept();
   TempPath *tempPath;
   DVDOutputPath *dvdOutputPath;
public slots:
	//virtual void projectName      ( );
	//virtual void slotBrowseTemp();
public:
   QString projectName;
   QString dvdPath;
   QString tmpPath;
   QLineEdit *m_pEditProjectName;
   QLineEdit *m_pEditDVDPath;
   QLineEdit *m_pEditTempPath;

private:
   //QLineEdit *m_pEditProjectName;
   //QLineEdit *m_pEditDVDPath;
   //QLineEdit *m_pEditTempPath;

   //QPushButton *m_pButtonBrowseTemp;
};

class TempPath : public QWizardPage
{
     Q_OBJECT
public:
   TempPath(QWidget *pParent = 0);
public slots:
   //virtual void projectName      ( );
   virtual void slotBrowseTemp();
public:
   QLineEdit *m_pEditTempPath;
private:
   //QLineEdit *m_pEditTempPath;
   QPushButton *m_pButtonBrowseTemp;
};

class DVDOutputPath : public QWizardPage
{
    Q_OBJECT
public:
   DVDOutputPath(QWidget *pParent = 0);
public slots:
   virtual void slotBrowseDVDPath();
   virtual void slotSelectionCHanged( const QString & qsText );
public:
   QLineEdit *m_pEditProjectName;
   QLineEdit *m_pEditDVDPath;
private:
   //QLineEdit *m_pEditProjectName;
   //QLineEdit *m_pEditDVDPath;
   QPushButton *m_pButtonBrowseDVDPath; 
};

#endif
