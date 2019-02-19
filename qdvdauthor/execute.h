/***************************************************************************
    execute.h
                             -------------------
    Execute class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is an enhaced version of QProcess. It will display the 
   output and the error messages (if any) in a graphical GUI.

   5 seconds after the termination of the process the GUI will terminate.
    
****************************************************************************/
 
#ifndef EXECUTE_H
#define EXECUTE_H

#include <QProcess>
#include <QDialog>

#define RENDER_MENU          "#=- Internal : Render Menu -=# "
#define CONVERT_VIDEOTRANS   "#=- VideoTrans -=# "
#define CONVERT_TRANSCODE    "#=- Transcode -=# "
#define CONVERT_MENCODER     "#=- MEncoder -=# "
#define CONVERT_FFMPEG       "#=- FFMpeg -=# "
#define BACKGROUND_MISCTOOLS "#=- Misc Tools -=#"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextEdit;
class QLineEdit;
class QCheckBox;
class QTimer;
class QSplitter;

class MyDialog : public QDialog
{
  Q_OBJECT

public:
  //MyDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );	//ooo
  MyDialog( QWidget* parent = 0 );	//xxx
 ~MyDialog();
 
  void processingFinished ( int );
  void startTimer ( int );
  void stopTimer  ( );
 
  // This function sets the number of seconds the GUI should wait
  // AFTER the process has terminated before auto closing the GUI.
  // range can be [0 - int]
  void setCloseDelay (uint iSeconds);
  
  void saveOutput    (QString);
 
public:
  QPushButton* buttonHelp;
  QCheckBox* checkKeepOpen;
  QPushButton* buttonAnalyze;
  QPushButton* buttonExport;
  QPushButton* buttonPlay;
  QPushButton* buttonOk;
  QPushButton* buttonCancel;
  QLineEdit* editCloseText;
  QSplitter* splitter1;
  QTextEdit* editCommandText;
  QTextEdit* textOutput;
 
protected:
  QGridLayout* MyDialogLayout;
  QHBoxLayout* layout2;
  
protected slots:
  virtual void languageChange ( );
  virtual void slotTimerDone  ( );
  virtual void slotKeepOpen   ( bool );
  virtual void slotExport     ( );
  virtual void slotPlay       ( );
  virtual void slotAnalyze    ( );
  
protected:
  void writeZoneCode ( );
protected:
  QTimer *m_pTimer;
  uint    m_iSeconds;
  bool    m_bDoneProcessing;
};

class Execute : public QProcess
{
  Q_OBJECT
public:
  Execute ( QObject * parent = 0, const char * name = 0 );	//ooo
  //Execute ( QObject * parent = 0 );				//xxx
  //Execute ( const QString & arg0, QObject * parent = 0, const char * name = 0 );	//ooo
  //Execute ( const QStringList & args, QObject * parent = 0, const char * name = 0 );	//ooo
 ~Execute ( );
 
  // The main new function added. simply call something, like
  // system ("cd hallo; touch base; more of_this", &env);
  // This function will return after all has been executed.
  int system ( QString &commandString, QStringList *listEnv=NULL);
  // Same as above but executes line of commands after each other.
  int system ( QStringList &commandString, QStringList *listEnv=NULL);
  int startCommand ( );
  
  // This function sets the number of seconds the GUI should wait
  // AFTER the process has terminated before auto closing the GUI.
  // range can be [0 - int]
  void setCloseDelay ( uint iSeconds );
  
  // This function is called when the command == "-- Render Menu --"
  int  renderMenu  (QString);
  void raise();	// This function will bring the ExecuteDialog back on top.
  
signals:
  // This signal connects to QDVDAuthor::renderMenu(QString)
  void signalRenderMenu(QString);
  
public slots:
  virtual void slotReadFromStderr();
  virtual void slotReadFromStdout();
  virtual void slotProcessExited ();

private:
  void	initMe();
  void 	createDialog(bool bKeep=false);
  
  MyDialog  *m_pDialog;

  uint m_iMaxWait;
  uint m_iSeconds;
  uint m_iCurrentCommand;
  QStringList m_commandList;
  QStringList m_environmentList;
};

#endif // EXECUTE_H
