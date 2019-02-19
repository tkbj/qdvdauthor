/***************************************************************************
    dialogsetup.h
                             -------------------
    Form interface generated from reading ui file 'uidialogsetup.ui'
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   WARNING! All changes made in this file will be lost! 
    
****************************************************************************/

#ifndef DIALOGSETUP_H
#define DIALOGSETUP_H

#include "slidedefaults.h"
#include "ui_uidialogsetup.h"
#include "utils.h"

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QLineEdit>

#include <QWidget>

#ifndef THUMBNAIL_COUNT
#define THUMBNAIL_COUNT 5 * 10
#endif

class QScrollArea;
class QListWidget;
class QFrame;

class DragNDropContainer;

class DialogSetup : public QDialog, public Ui::uiDialogSetup
{
  Q_OBJECT

public:
  DialogSetup (QWidget *pParent);
  ~DialogSetup();
  
  QList<Utils::toolsPaths *> getToolsPathsList ();
  void setToolsPaths(QList<Utils::toolsPaths *> &);
  
public slots:
   virtual void slotBrowseProjectPath();
   virtual void slotPreferredEngineChanged(const QString &);
   virtual void slotScanSystem       ( );
   virtual void slotButtonInfo       ( );
   virtual void slotButtonMan        ( );
   virtual void slotBrowseTemp       ();
   virtual void accept ();
   
protected:
  void displayMan             ( QString );
  void searchTool             ( QString );
  void initExternalPlayerList ( );
  void initSlideshowTab       ( );
  void storeSlideshowDefaults ( );
  void initTransitions        ( );
  
private:	// private variables ...
  QPixmap                    *m_arrayPix[THUMBNAIL_COUNT];
  int                         m_iCurrentPix;
  bool                        m_bAnimated;
  bool                        m_bModifiedToolsPaths;
  SlideDefaults               m_slideDefaults;
  QTimer                     *m_pTimer;
  DragNDropContainer         *m_pDNDContainer;
  QScrollArea                *m_pScrollView;
  QFrame                     *m_pScrollFrame;
  QStringList                 m_listTools;
  QStringList                 m_listHistory;
  QList<QPixmap *>       m_listHistoryPix;
  int                         m_iMaxHistoryFiles;
  int                         m_iAutosave;
  QString                     m_qsStyle;
  QList<uint>            m_listIndex;
  QList<QPushButton *>   m_listManButtons;      // This is needed to get the button pressed in the slotButtonMan
  QList<QPushButton *>   m_listButtons;         // This is needed to get the button pressed in the slotButtonInfo
  QList<QLineEdit *>     m_listEditToolsPaths;  // This one is used to set the values for m_listToolsPaths
  QList<Utils::toolsPaths *> m_listToolsPaths;  // Holds the tools needed, and their paths and if they exist on this machine.
  
  //QListWidget *contentsWidget;
  //QStackedWidget *pagesWidget;
  //QLineEdit *m_pEditProjectPath;
  //QLineEdit *m_pEditTempPath;
   
};

class ConfigurationPage : public QWidget
{
public:
    ConfigurationPage(QWidget *parent = 0);
};

#endif // DIALOGSETUP_H
 
