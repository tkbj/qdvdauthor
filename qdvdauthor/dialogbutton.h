/***************************************************************************
    dialogbutton.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
****************************************************************************/

#ifndef DIALOGBUTTON_H
#define DIALOGBUTTON_H

#include "ui_uidialogbutton.h"

#include "global.h"
#include "rgba.h"

class DVDMenu;
class MenuPreview;
class ButtonObject;
class ButtonPreview;
class SourceFileEntry;
class SourceFileInfo;

class DialogButton : public QDialog, public Ui::uiDialogButton		//xxx
{
  Q_OBJECT
public:
   //DialogButton ( QWidget *, const char *p=0, Qt::WFlags f=0 );	//ooo
   DialogButton ( QWidget *pParent );				//xxx
  ~DialogButton ( );

  void initMe            ( ButtonObject *, MenuPreview * );
  void setSourceFiles    ( QList<SourceFileEntry *> );
  void setSubMenus       ( QStringList );
  void setMenuButtons    ( QStringList );
  void setButtonCreation ( bool        );

protected:
  void initNext         ( );
  void initMultiple     ( );
  void initColors       ( );
  void initActions      ( );
  void initActionState  ( );		//oooo
  void initTransition   ( );
  void initSystem  ( MenuPreview  *, ButtonObject * );
  void initButtons ( ButtonObject * );
  void initButtons ( DVDMenu      * );
  
  SourceFileEntry *getSelectedSourceEntry  ( );
  SourceFileInfo  *getSelectedSourceFile   ( );
  DVDMenu         *getSelectedDVDMenu      ( );
  bool setObjectColor   ( MenuObject*, Rgba &);
  void storeColorsInMenu( );
  
signals:
  void signalUpdateStructure();
  void signalUpdatePixmap();

protected slots:
  virtual void slotAdjustSize ( );
  virtual void slotAction1    ( int );
  virtual void slotSourceEntry( int );
  virtual void slotSourceFile ( int );
  virtual void slotChapters   ( int );
  virtual void slotChapters2  ( int );
  virtual void slotMultiple   ( );
  virtual void slotHelpAction ( );
  virtual void slotHelpGoto   ( );
  virtual void slotHelp       ( );
  virtual void slotHelpSystem ( );
  virtual void slotChangeColor( );
  virtual void slotOkay       ( );
  virtual void slotCancel     ( );
  virtual void slotAdvanced   ( );
  virtual void slotTransition ( );  

protected:
  ButtonObject *m_pButtonObject;
  ButtonObject *m_pClonedButton;
  MenuObject   *m_pHighlightedMask[2];  // Holds the Frame[0]/Mask[1], depending on the selected check box
  MenuObject   *m_pSelectedMask   [2];  // Holds the Frame[0]/Mask[1], depending on the selected check box
  
  ButtonPreview *m_pFramePreviewNormal;
  ButtonPreview *m_pFrameHighlighted;
  ButtonPreview *m_pFrameSelected;
  MenuPreview   *m_pMenuPreview;
  Rgba           m_pgcColorsOrig[MAX_MASK_COLORS];  // Original colors of current menu
  Rgba           m_pgcColors    [MAX_MASK_COLORS];  // colors if changed by user.
  uint           m_iColorIdx    [2];                // current selected ColorIdx Highlighted[1]/Selected[0]
  QList<SourceFileEntry *> m_listSourceFileEntries;	// Retrieve the info from the QDVDAuthor class
  QStringList m_listSubMenus;     // Retrieve the info from the CDVDMeun class
  QStringList m_listMenuButtons;  // Retrieve the info from the CDVDMenu class
  
  // The following flag is needed to diff between a newly created Button and a opening of the Dialog with
  // an already existing Button. This is helping the cancelation of the Dialog.
  bool m_bButtonCreation;
  bool m_bAutomatic;
};

#endif // DIALOGBUTTON_H
