/***************************************************************************
    qdvdauthor.h
                             -------------------
    DialogMovie class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class generates the command line for dvdauthor.
   Current Version of DVDAUTHOR used is 0.67
   You can find dvdauthor under http://dvdauthor.sourceforge.net/
    
****************************************************************************/

#ifndef QDVDAUTHOR_H
#define QDVDAUTHOR_H

class QDomNode;
class QDomElement;
class QMenu;

#include "render_client.h"
#include "plugins/interface.h"

class ColorToolBar;
class SourceToolBar;
class StructureToolBar;
class SourceFileEntry;
class SourceFileInfo;
class DVDMenu;
class MediaInfo;
class DialogExecute;
class UndoBuffer;
class DialogUndoStack;	//oooo
class ButtonTransition;
namespace DVDLayout { class Gui;    };
namespace Render    { class Client; };

#include <iostream> //xxx
#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QTabWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QAction>
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
//#include <QCustomEvent> //ooo
#include <QEvent>   //xxx

#define DVDAUTHOR_XML "dvdauthor.xml"

#include "dragndropcontainer.h"

class QCustomEvent : public QEvent
{
public:
    //static QEvent::Type customEventType; //xxx
    //QCustomEvent() : QEvent(QCustomEvent::type()) //ooo
    //static QEvent::Type type () { return static_cast<QEvent::Type>(generatedType);}   //xxx
    QCustomEvent() : QEvent(QEvent::type()) //xxx
    {
    };
    //QCustomEvent( int type ){};   //xxx
    //QCustomEvent(QEvent::Type test) : QEvent(QEvent::type())  //xxx-!
    QCustomEvent(QEvent::Type test) : QEvent(QCustomEvent::type())  //xxx-!
    //QCustomEvent(QEvent::Type test) : QEvent(test)
    {
        if ( test == myType_Event_Render_Progress )
        {
            //int generatedType = QEvent::registerEventType(EVENT_RENDER_PROGRESS);
            //QEvent::Type type () { return static_cast<Type>(generatedType);}
            //static QEvent::Type customEventType = static_cast<QEvent::Type>(generatedType);
            //myCustomEventType = myType_Event_Render_Progress;
            //my_customEventType = test;
            //f(test);
        }
        //QCustomEvent::my_customEventType = test;    //xxx
       // QEvent::Type QCustomEvent::customEventType = QEvent::None;  //xxx
    }; //xxx
    
    //QCustomEvent(int mytype) : QEvent(QEvent::type())   //xxx-!
    //QCustomEvent(int mytype) : QEvent(QCustomEvent::type())   //xxx-!
    //QCustomEvent(int mytype);   //xxx-?
    //QCustomEvent(int mytype) : QEvent(QCustomEvent::type()), myCustomEventType(static_cast<QEvent::Type>(mytype))   //xxx-?
    //QCustomEvent(int mytype) : QEvent(QCustomEvent::type()), customEventType(static_cast<QEvent::Type>(mytype))   //xxx-?
    //QCustomEvent(int mytype) : QEvent(QCustomEvent::type(customEventType)), customEventType(static_cast<QEvent::Type>(mytype))   //xxx
    QCustomEvent(int mytype) : QEvent(static_cast<QEvent::Type>(mytype)), customEventType(static_cast<QEvent::Type>(mytype))   //xxx
    {
        //static QEvent::Type customEventType = QEvent::None; //xxx
        //customEventType = QEvent::None; //xxx
        if ( mytype == EVENT_RENDER_PROGRESS )
        {
            //int generatedType = QEvent::registerEventType(EVENT_RENDER_PROGRESS);
            //static QEvent::Type customEventType = static_cast<QEvent::Type>(generatedType);
            //QCustomEvent::customEventType = static_cast<QEvent::Type>(generatedType);
            //my_customEventType = static_cast<QEvent::Type>(generatedType);
            //return static_cast<QEvent::Type>(generatedType);
            //myCustomEventType = myType_Event_Render_Progress;    //xxx-?
            //customEventType = myType_Event_Render_Progress;    //xxx-?
            //test = 1;
        }
        else if (mytype == EVENT_RENDER_EXIT )
        {
            customEventType = myType_Event_Render_Exit;
        }
        else if (mytype == EVENT_RENDER_EXCEPTION )
        {
            customEventType = myType_Event_Render_Exception;
        }
        
        //    myFunctionType ( static_cast<QEvent::Type>(EVENT_RENDER_PROGRESS) );    //xxx
        //my_customEventType = static_cast<QEvent::Type>(type);    //xxx
        //my_customEventType = myType_Event_Render_Progress;    //xxx
        //my_customEventType = QEvent::MaxUser;    //xxx
        //my_customEventType = QEvent::None;    //xxx
        //type(); //xxx
        //type ( static_cast<QEvent::Type>(EVENT_RENDER_PROGRESS) );    //xxx
        //customEventType = QEvent::None; //xxx
    }; //xxx

    virtual ~QCustomEvent()
    {};
    
    //static const QEvent::Type my_customEventType;    //xxx
    //QEvent::Type QCustomEvent::customEventType = QEvent::None;  //xxx
    
    //static QEvent::Type customEventType = static_cast<QEvent::Type>(EVENT_RENDER_PROGRESS);  //xxx
    
    
    static QEvent::Type f(QEvent::Type type)    //xxx
    {
        static QEvent::Type customEventType = type;
     
        return customEventType;
    }
    
    static QEvent::Type g(int t)    //xxx
    {
        static QEvent::Type customEventType = myType_Event_Render_Progress;
     
        return customEventType;
    }
    
    //static QEvent::Type type()    //ooo
    QEvent::Type type()    //xxx
    //virtual QEvent::Type type()    //xxx
    //static QEvent::Type function()    //xxx
    //static QEvent::Type type(QEvent::Type customEventType)    //xxx
    {
        //static QEvent::Type customEventType = f(myType_Event_Render_Progress);
        //if ( mytype == EVENT_RENDER_PROGRESS ) //xxx
            //static QEvent::Type customEventType = myType_Event_Render_Progress; //xxx-!
            //customEventType = myType_Event_Render_Progress; //xxx
            //static QEvent::Type customEventType = f(myCustomEventType);
            //static QEvent::Type customEventType; //xxx
            //static QEvent::Type customEventType = f(myType_Event_Render_Progress);    //xxx
            //QEvent::Type customEventType = QEvent::None;    //xxx
            //QEvent::Type customEventType = myType_Event_Render_Progress;    //xxx-!
            //static QEvent::Type customEventType = myCustomEventType;    //xxx-!
            //QEvent::Type customEventType = g(test);    //xxx
            //static QEvent::Type customEventType = myCustomEventType;    //xxx-?
        //static QEvent::Type customEventType = QCustomEvent::customEventType;    //xxx
        //static QEvent::Type customEventType = static_cast<QEvent::Type>(type());    //xxx
        //static QEvent::Type customEventType = test;    //xxx
        //static QEvent::Type customEventType = my_customEventType;    //xxx
        //customEventType = static_cast<QEvent::Type>(type);    //xxx
        //customEventType = myType_Event_Render_Progress;    //xxx
        //int hint;   //xxx
        //int generatedType;  //xxx
        //static QEvent::Type CustomEventType;    //xxx
        
        if (customEventType == QEvent::None)
        {
            int generatedType = QEvent::registerEventType();  //ooo
            //generatedType = QEvent::registerEventType();  //xxx
            //int generatedType = QEvent::registerEventType(hint);    //xxx
            customEventType = static_cast<QEvent::Type>(generatedType);
            printf("QEvent::None\n");
        }
        else if (customEventType == myType_Event_Render_Progress)    //xxx
        {
            int generatedType = QEvent::registerEventType(EVENT_RENDER_PROGRESS);  //ooo
            //generatedType = QEvent::registerEventType(EVENT_RENDER_PROGRESS);  //xxx
            //int generatedType = QEvent::registerEventType(hint);    //xxx
            customEventType = static_cast<QEvent::Type>(generatedType);   //xxx
            //CustomEventType = static_cast<QEvent::Type>(generatedType); //xxx
            //CustomEventType = myType_Event_Render_Progress; //xxx
        }
        else if (customEventType == myType_Event_Render_Exit)    //xxx
        {
            int generatedType = QEvent::registerEventType(EVENT_RENDER_EXIT);  //ooo
            //generatedType = QEvent::registerEventType(EVENT_RENDER_EXIT);  //xxx
            //int generatedType = QEvent::registerEventType(hint);    //xxx
            customEventType = static_cast<QEvent::Type>(generatedType);
        }
        else if (customEventType == myType_Event_Render_Exception)    //xxx
        {
            int generatedType = QEvent::registerEventType(EVENT_RENDER_EXCEPTION);  //ooo
            //generatedType = QEvent::registerEventType(EVENT_RENDER_EXCEPTION);  //xxx
            //int generatedType = QEvent::registerEventType(hint);    //xxx
            customEventType = static_cast<QEvent::Type>(generatedType);
        }
        return customEventType; //ooo
        //return f(customEventType); //xxx
        //return CustomEventType; //xxx
        //return static_cast<QEvent::Type>(generatedType);    //xxx
    }

private:  //ooo
//public:  //xxx
    //static QEvent::Type customEventType;  //ooo
    QEvent::Type customEventType;  //xxx
    //QEvent::Type customEventType = QEvent::None;  //xxx
    //static const QEvent::Type customEventType = static_cast<QEvent::Type>(EVENT_RENDER_PROGRESS);  //xxx

public:
    //static const QEvent::Type myType_Event_Render_Exit      = static_cast<QEvent::Type>(11111); //xxx
    //static const QEvent::Type myType_Event_Render_Progress  = static_cast<QEvent::Type>(11112); //xxx
    //static const QEvent::Type myType_Event_Render_Exception = static_cast<QEvent::Type>(11113); //xxx
    
    static const QEvent::Type myType_Event_Render_Exit      = static_cast<QEvent::Type>(EVENT_RENDER_EXIT); //xxx
    static const QEvent::Type myType_Event_Render_Progress  = static_cast<QEvent::Type>(EVENT_RENDER_PROGRESS); //xxx
    static const QEvent::Type myType_Event_Render_Exception = static_cast<QEvent::Type>(EVENT_RENDER_EXCEPTION); //xxx
    
    QEvent::Type myCustomEventType;    //xxx
    //QEvent::Type customEventType;  //xxx
    
    //int generatedType;  //xxx
    int test;

};    //xxx

/*class QCustomEvent : public QEvent
{
public:
    QCustomEvent( int type );
    QCustomEvent( Type type, void *data )
	: QEvent(type), d(data) {};
    void       *data()	const	{ return d; }
    void	setData( void* data )	{ d = data; }
private:
    void       *d;
};*/    //xxx


class QDVDAuthor : public QMainWindow
{
   Q_OBJECT
public:
    enum enTabs { TabMainMenu=0, TabSubMenu, TabSubtitles, TabDVDLayout, TabDVDAuthorXml };
    
    QDVDAuthor(QWidget *parent = 0);
    virtual ~QDVDAuthor ( );
    
protected:
  //virtual void customEvent ( QCustomEvent * );    //ooo
  virtual void customEvent ( QEvent * );    //xxx-!
  //virtual bool event ( QCustomEvent * );    //xxx
  //virtual bool event ( QEvent * );    //xxx-!

public slots:
  virtual void fileNew      ( );
  virtual void fileOpen     ( );
  virtual void fileSave     ( );
  virtual void fileSave     ( QString, bool b=false );	//ooo
  virtual void fileSaveAs   ( );
  virtual void editUndo     ( );
  virtual void editRedo     ( );
  virtual void helpContents ( );
  virtual void helpGuide    ( );
  virtual void helpAbout    ( );
   
  virtual void polish                          ( );
  virtual void slotAutosave                    ( );
  virtual void slotActivatePlugin              ( );
  virtual void slotActivatePlugin         (  int );
  virtual void slotPluginCallback  ( int, QString, QString );
  virtual void slotSetup                       ( );
  virtual void slotEditAddSlideshow            ( );
  virtual void slotGenerateMasks               ( );
  virtual void slotCreateDVD                   ( );
  virtual void slotAddSubMenu                  ( );
  virtual void slotFeedback                    ( );
  //virtual void slotTabChanged        ( QWidget * );	//ooo
  virtual void slotTabChanged              ( int );	//xxx
  virtual void slotRemoveCurrentSubMenu        ( );
  virtual void slotRenameCurrentSubMenu        ( );
  virtual void slotCloneCurrentSubMenu         ( );
  virtual void slotUndoStack                   ( );
  virtual void slotUpdateStructure             ( );
  virtual void slotUpdateDVDLayout             ( );
  virtual void slotDeleteTempOlderThan         ( );
  virtual void slotCreateXml   ( const QString & );
  virtual void slotUpdateMenuBar               ( );
  virtual void slotRenderMenu          ( QString );
  virtual void slotCalculateSize               ( );
   
signals:
  void signalAddSound      ( );
  void signalAddBackground ( );
  void signalAddVideo ( );
   
public:
  void fileOpen             ( QString );
  void addDVDMenu           ( DVDMenu *, bool b=false );
  void returnFromRenderMenu ( );
  bool checkForNewProject   ( );
  void showUndoStack        ( );
  void updateUndoStack      ( );
  //bool tabVisible           ( enTabs );   //oooo
  
  void addMovieList      ( QStringList &, QString *p=NULL );
  int  sourceFileCount                                  ( );
  SourceFileEntry *sourceFileEntry                 ( uint );
  SourceFileEntry *getSourceEntryByDisplayName( QString & );
  void appendSourceFileEntry          ( SourceFileEntry * );
  void removeSourceFileEntry          ( SourceFileEntry * );  // called to remove the SourceFileEntry
  void removedSourceEntry             ( SourceFileEntry * );  // called after entry has been removed from SourceToolBar
  void updateSourceFileInfo           ( SourceFileInfo  * );
  void editSourceFileInfo             ( SourceFileInfo  * );
  void updateDVDLayout                ( SourceFileEntry * );
  void replaceMenuName             ( DVDMenu *, QString & );
  void replaceSourceDisplayName    ( QString &, QString & );
  bool isSourceEntryUsed                      ( QString & );
  void addSound                           ( QStringList & );
  void setColor                              ( uint, Rgba );
  MediaInfo  *getMediaInfo                              ( );
  UndoBuffer *getUndoBuffer                             ( );
  DragNDropContainer *getDragNDropContainer             ( );
   
  DVDMenu *getCurrentSubMenu                            ( );
  DVDMenu *getSubMenuByName                     ( QString );
  DVDMenu *getVMGMenu                                   ( );
  QList<DVDMenu *> &getSubMenus                    ( );
  
  QString getUniqueMenuTitle  ( QString, int i=0, DVDMenu *p=NULL );
  QString getVidFilter        ( );
  QString getImageFilter      ( );
  QString getMovieFilter      ( );
  QString getSoundFilter      ( );
  
  QTabWidget *m_pTabWidgetMain;	//xxx
  QWidget *widget;
  QWidget *tab0;
  QWidget *tab;
  QWidget *tab1;
  QGridLayout *gridLayout1;
  QTextEdit *m_pTextEditXml;
  QLabel *textLabel1_2;
  QComboBox *m_pComboSource;
   
  //QAction *m_pActionCreateDVD;
    
private: // Private functions ...

  void initMe                 ( );
  bool cleanUp                ( );
  void LoadIni                ( );
  void SaveIni                ( );
  void newWizard              ( );
  void buildHistoryEntries    ( );
  void updateHistoryPix       ( );
  void initDVDLayoutTab       (         );
  void initSubtitlesTab       (           );
  void addHistory             ( QString     );
  bool writeProjectFile       ( QDomElement & );
  bool readProjectFile        ( QDomNode &, float & );
  bool renderSubtitles        ( SourceFileInfo *, bool b=false );
  void createTempDirStructure ( );
  void createDvdauthorXml     ( );
  void createSubtitlesXml     ( );
  void createTransitionsXml   ( );
  void createMenuXml          ( );
  void createDVD              ( );
  bool checkHealth            ( );
  void engageCreateDVD        ( bool ); // Called to put QDVDAuthor in wait mode for slideshows to finish
  QString  getTempFile        ( QString );
  QWidget *getTabWidget       ( enTabs,    int i=0 );
  unsigned long getAudioSize  ( SourceFileInfo   * );
  void eventEntry             ( Render::Client   * ); //ooo
  void eventTrans             ( Render::Client   * );
  bool buttonTransitionExists ( ButtonTransition * );
    
private:
  bool    m_bVisibleRegion;
  
  DVDMenu *m_pVMGMenu;
  QList<DVDMenu *>   m_listDVDMenus;
  DragNDropContainer      m_dragNDropContainer;
  QStringList             m_listHistory;
  QList<QPixmap *>   m_listHistoryPix;
  int                     m_iMaxHistoryFiles;
  int                     m_iAutosave;	// Autosave every x seconds ...
  QString                 m_qsStyle;
  bool                    m_bAutosaving;
  bool                    m_bCreateDVDAfterSlideshows;
  bool                    m_bWait;
  
  Plugin::Interface      *m_pPluginInterface; //ooo
  ColorToolBar           *m_pColorToolbar;
  SourceToolBar          *m_pSourceToolbar;
  StructureToolBar       *m_pStructureToolbar;
  DVDLayout::Gui         *m_pDVDLayoutGui;		//ooo
  QMenu                  *m_pMenuHistory;
  bool                    m_bModified;
  DialogExecute          *m_pDialogExecute;
  DialogUndoStack        *m_pUndoStackDialog;		//oooo
  
  QAction *editUndoAction;
  QAction *editRedoAction;
  
  QToolBar *m_pToolbar;
  QAction  *m_pActionToolbarNew;
  QAction  *m_pActionToolbarWizard;
  QAction  *m_pActionToolbarOpen;
  QAction  *m_pActionToolbarSave;
  QAction  *m_pActionToolbarSaveAs;
  QAction  *m_pActionToolbarExit;
   
  QAction  *m_pActionCreateDVD;
  QAction  *m_pActionAddBackgrnd;
  QAction  *m_pActionAddVideo;
  QAction  *m_pActionAddAudio;
  QAction  *m_pActionAddSlide;
  QAction  *m_pActionAddMenuSlide;
  QWidget  *m_mainTabBuffer[3];  // DVDLayout/Subtitles/XMLOut
  
  QAction *m_pActionUndoStack;
  QAction *helpContentsAction;
  QAction *helpGuideAction;
  QAction *helpAboutAction;
   
  QGridLayout *gridLayout;
  //QTabWidget *m_pTabWidgetMain;
  QWidget *TabPage;
  QHBoxLayout *hboxLayout;
  QLabel *m_pLabelSize;
  QProgressBar *m_pProgressSize;

};

#endif 
