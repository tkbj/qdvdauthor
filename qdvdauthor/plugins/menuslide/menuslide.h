/****************************************************************************
** Execute class
**
** Created: Tue Jan 26 12:09:08 2003
**      by: Varol Okan using kate editor
**
** This class provide a GUI to the man pages.
**
****************************************************************************/

#ifndef MENUSLIDE_H
#define MENUSLIDE_H

#include <qimage.h>
#include <qpixmap.h>
//#include <q3valuelist.h>	//oxx
//Added by qt3to4:
#include <QTimerEvent>

#include "../interface.h"

//#include "uimenuslide.h"	//ooo
#include "ui_uimenuslide.h"	//xxx

namespace Plugin
{

//class MenuSlide : public uiDialogMenuSlide				//ooo
class MenuSlide : public QDialog, public Ui::uiDialogMenuSlide		//xxx
{
  Q_OBJECT

  class ImageContainer {
  public:
    ImageContainer ( QString & );
    QString qsFileName;
    QString qsBaseName;
    QImage  thumbnail;
  };

public:
           //MenuSlide  ( QWidget *p=NULL, const char *n=NULL, bool m=FALSE, Qt::WFlags f=0 );		//ooo
	   MenuSlide  ( QWidget *pParent=NULL );							//xxx
  virtual ~MenuSlide  ( );
  virtual void accept ( );

protected:
  virtual void doEmit ( SourceGroup::type, QString &, QString & ) {}; // pass to the parent class ...
  QString getMenuXml       ( );
  QString buildAnimation   ( int );
  QString buildImageObject ( ImageContainer *, int );

  float getImageDuration   ( );

protected slots:
  virtual void polish           ( );
  virtual void slotUp           ( );
  virtual void slotDown         ( );
  virtual void slotDelete       ( );
  virtual void slotHelp         ( );
  virtual void slotAddImages    ( );
  virtual void slotPreview      ( );
  virtual void slotBackground   ( );
  virtual void slotToggledID    ( bool );
  virtual void slotToggledTD    ( bool );
  //virtual void slotImageChanged ( Q3ListBoxItem * );  //ooo
  virtual void slotImageChanged ( QListWidgetItem * );  //xxx

protected:
  void timerEvent ( QTimerEvent * );

private:  // private member functions.
  void loadBackgroundImage ( );
  void createAnimationData ( bool );
  void animate             ( );
  void animate             ( unsigned long );

private:
  int m_iMaxWidth;
  int m_iMaxHeight;
  int m_iConcurrentImages;
  QString testMenu ( );
  QList<ImageContainer *> m_listImages;		//oxx

  int m_iAnimationTimerID;
  unsigned long m_iCurrentFrame;
  unsigned long m_iTotalFrames;
  float m_fTotalDuration;
  float m_fImageDuration;
  QPixmap m_backgroundPixmap;
};

class MenuSlideInterface : public Interface, public MenuSlide
{
public:
  MenuSlideInterface    ( );  //ooo
  //MenuSlideInterface    ( QWidget * );  //xxx
  virtual ~MenuSlideInterface   ( ) { };
//  virtual float version ( ) { return 0.1f; };
  virtual QWidget *getConfigWidget ( QWidget * ) { return 0; };
  virtual bool execute  ( );
  virtual void doEmit   ( SourceGroup::type, QString &, QString & );
};

}; // end of namespace Plugins
#endif // DIALOGMAN_H
