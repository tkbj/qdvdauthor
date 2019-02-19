/***************************************************************************
    menupreview.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class generates the command line for dvdauthor.
   Current Version of DVDAUTHOR used is 0.67
   You can find dvdauthor under http://dvdauthor.sourceforge.net/
    
****************************************************************************/

#ifndef MENUPREVIEW_H
#define MENUPREVIEW_H

#include <QMouseEvent>

#include "selectionobject.h"		//xxx
#include "buttonpreview.h"
#include "menuobject.h"

class ButtonObject;
class DialogButton;
class MovieObject;
class SourceFileEntry;

class MenuPreview : public ButtonPreview
{
  Q_OBJECT
  // m_iCreatreObjectStyle
  enum { TypeNone=0, FrameType, TextType, ImageType, MovieType, CollectionType };
  // m_iPointerState
  enum { StateUnknown=0, StateNormal, StateMoveSelected, StateReadyToCreate, StateCreating, StateSelecting, StateSelected };

public:
  //MenuPreview ( QWidget * parent, const char * name = 0, Qt::WindowFlags f = 0 );	//ooo
  MenuPreview ( QWidget * parent );							//xxx
  ~MenuPreview ( );
  void clear   ( Rgba * );
  void initMe  ( Rgba * );
  MenuPreview &operator = ( MenuPreview & );
  
  void respondSourceFiles ( QList<SourceFileEntry *> );		//oxx
  void respondSubMenus    ( QStringList );
  
  bool readProjectFile    ( QDomNode    & );
  bool writeProjectFile   ( QDomElement & );
  bool createContextMenu  ( QPoint);
  
  void setIsSubMenu       ( bool );
  MenuObject *readObject  ( QDomNode &objectNode );
  
  void setVisibleRegion   ( bool );
  void setImageButton     ( bool );
  void createAnimationData( QString &, long );	//ooo
  
  void          connectStdSlots ( MenuObject *, bool );
  ButtonObject *defineAsButton  ( MenuObject *, bool b=true );	//oooo
  
  MenuObject *createFrameObject      ( bool b=true );
  MenuObject *createTextObject       ( bool b=true );
  MenuObject *createImageObject      ( bool b=true );
  MenuObject *createMovieObject      ( bool b=true );
  MenuObject *createButtonObject     ( bool b=true );
  
  //int iX;   //xxx
  //int iY;   //xxx
 
public slots:
  virtual void slotAddFrameObject        ( );
  virtual void slotAddTextObject         ( );
  virtual void slotAddImageObject        ( );
  virtual void slotAddMovieObject        ( );
  virtual void slotUpdateStructure       ( );
  virtual void slotUpdatePixmap          ( );
  virtual void slotAddOverlay            ( MenuObject   * );
  virtual void slotAddShadow             ( MenuObject   * );
  virtual void slotDefineAsButton        ( MenuObject   * );
  virtual void slotDeleteObject          ( MenuObject   * );
  virtual void slotUndoObject            ( MenuObject   * );
  virtual void slotModifyObject          ( MenuObject   * );
  virtual void slotUnbutton              ( ButtonObject * );
  virtual void slotCreateButtonDialog    ( ButtonObject * );
  virtual void slotEmitDeleteMe          ( );
  virtual void slotDestroyedButtonDialog ( );
  
signals:
  void signalUpdateStructure    ( );
  void signalRequestSourceFiles ( );
  void signalRequestSubMenus    ( );
  void signalDeleteMe           ( );
  void signalMaskColorChanged   ( Rgba, Rgba );
  void signalCreatedMovieObject ( MovieObject  *, bool );
  
protected:
  virtual void mousePressEvent	   	( QMouseEvent * );
  virtual void mouseReleaseEvent	( QMouseEvent * );
  virtual void mouseMoveEvent 	    ( QMouseEvent * );
  virtual void paintEvent           ( QPaintEvent * );       //xxx
  
  QString &newButtonName ( );
  
private:	// Private functions ...
  void        checkSelection          ( );  
  bool        isAnimationData         ( QString & );
  
private:
  QPixmap m_doubleBuffer;
  int     m_iCreateObjectStyle;  // What object, text, image, frame, or movie ?
  int     m_iPointerState;
  bool    m_bImageButton;        // Should we create ImageButton objects or MovieButtons when dropping ?
  QRect   m_rectCurrentObject;
  bool    m_bIsSubMenu;
  bool    m_bIsMovieMenu;
  Rgba   *m_pPgcColors;
  DialogButton   *m_pDialogButton;	//ooo
  SelectionObject m_selectionObject;	//ooo
  int iX;   //xxx
  int iY;   //xxx
  QMouseEvent *p_Event; //xxx
};

#endif // MENUPREVIEW_H

