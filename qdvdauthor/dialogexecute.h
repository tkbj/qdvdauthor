/***************************************************************************
    dialogexecute.h
                             -------------------
    Form interface generated from reading ui file 'dialoggenerate.ui'
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef DIALOGEXECUTE_H
#define DIALOGEXECUTE_H

#include <QLabel>
#include <QFrame>

#include "execute.h"
#include "utils.h"

class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTabWidget;
class QWidget;
class QFrame;
class QGroupBox;
class QLabel;
class QCheckBox;
class QComboBox;
class QScrollArea;
class QDVDAuthorInit;

// Here we define the Interface from QDVDAuthor - class to the ExexcuteDialog.
class ExecuteInterface
{
public:
  enum InterfaceType { TYPE_DEFAULT, TYPE_TRANSCODING, TYPE_SUBTITLES, TYPE_SLIDESHOW };

  ExecuteInterface ( ) { bShowBlock=true; enType = TYPE_DEFAULT; };
  ~ExecuteInterface ( ) { };

  QString     qsBlockName;
  QColor      backgroundColor;

  // Okay, each line can have multiple command lines, to replace one if the other does not work.
  QList <QStringList> listCommentList;
  QList <QStringList> listCommandList;
  QList <int> listDefaults;
  QList <int> listBlockIDs;
  QList <QString> listKeywords;
  bool bShowBlock;
  InterfaceType enType;
  // indicates which of the listCommandList represent a split to the next tool (transcode / MEncoder ...)
  QList <int> listSplitAt;	// if empty, then normal handling
};

class DialogExecute : public QDialog
{
  Q_OBJECT

public:
  DialogExecute (QList<ExecuteInterface *>, bool b=true);
  ~DialogExecute();
  
  void createManPages(QStringList &listOfCommandsUsed);
  void setInit (QDVDAuthorInit *);
  void returnFromRenderMenu    ();

protected:
  void createMainForm ();
  void createPreprocessing  ();
  void createDVDAuthorTab   (QList<ExecuteInterface *>);
  void createPostprocessing ();
  QString getToolsPath(QString);
  
  QGridLayout *createBlockFrame ( QWidget *, QGridLayout *, ExecuteInterface *, int, int i=-1 );
  void createCommandBlock       ( QWidget *, QGridLayout *, ExecuteInterface *, int, int      );
  void createStandardBlock      ( QWidget *, QGridLayout *, ExecuteInterface *, int           );
  void createTranscodeBlock     ( ExecuteInterface *, int );
  void createSubtitlesBlock     ( ExecuteInterface *, int );
  void createSlideshowBlock     ( ExecuteInterface *, int );  
  
protected:	// member variables ...
  class GuiBlock 
  {
  public:
    GuiBlock ( );
    QLabel      *pLabelComment;          //
    QList<QWidget *>listTranscodeFrames;
    QComboBox   *pComboTranscode;        //
    QComboBox   *pComboCommands;         //
    QPushButton *pButtonMan;             //
    QPushButton *pButtonEdit;            //
    QPushButton *pButtonSetAsDefault;    //
    int          iBlockID;               //
    int          iBlockExecuteInterface; //
    QStringList  listComments;
  };
  QList<GuiBlock *>          m_listGui;
  QList<QGroupBox *>		m_listGroupBox;
  QList<QCheckBox *>		m_listCheckBox;
  QList<QCheckBox *>		m_listCheckBox2;
  QList<ExecuteInterface *>  m_listExecuteInterface;      // indexed by the above to fiund keywords ...
  QTabWidget* m_pTabWidget;
  QWidget* m_pTabExpert;
  
  QFrame* m_pFrameExplain;
  QLabel* m_pLabelExplain;
  
  QScrollArea* m_pScrollView;
  QFrame* m_pScrollFrame;
  
  QPushButton* buttonHelp;
  QPushButton* buttonOk;
  QPushButton* buttonCancel;
  QPushButton* buttonExport;
  
protected:
  QGridLayout* DialogExecuteLayout;
  QHBoxLayout* Layout1;
  QGridLayout* tabLayout;
  QGridLayout* m_pScrollFrameLayout;
  QGridLayout* m_pFrameExplainLayout;
  
protected slots:
  virtual void slotResize               ( );
  virtual void languageChange           ( );
  virtual void slotOkay                 ( );
  virtual void slotHelp                 ( );
  virtual void slotManButtonClicked     ( );
  virtual void slotEditButtonClicked    ( );
  virtual void slotDefaultButtonClicked ( );
  virtual void slotTabChanged ( QWidget * );
  virtual void slotCheckBlock ( bool      );
  virtual void slotRenderMenu ( QString   );
  virtual void slotNormalSelectionChanged    (int);
  
signals:
  void signalGenerateMasks ();
  void signalRenderMenu (QString);
  
private:
  int             m_iTranscodeOffset;
  QWidget        *m_pTranscodeParent;
  QGridLayout    *m_pTranscodeLayout;
  int             m_iSubtitlesOffset;
  QWidget        *m_pSubtitlesParent;
  QGridLayout    *m_pSubtitlesLayout;
  QWidget        *m_pSlideshowParent;
  QGridLayout    *m_pSlideshowLayout;
  Execute		m_doSomething;		//ooo
  QDVDAuthorInit *m_pInitObject;
  QList <Utils::toolsPaths *> m_listToolsPaths;
};

#endif // DIALOGEXECUTE_H
