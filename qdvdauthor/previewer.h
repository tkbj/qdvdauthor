/****************************************************************************
** Class Previewer
**
**   Created : Tue Jun 3 09:13:25 2008
**        by : Varol Okan, using Kate
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
** Previewer for the DialogFiles / DialogImages - classes
**
****************************************************************************/

#ifndef PREVIEWER_H
#define PREVIEWER_H

#include "cachethumbs.h"
//#include "uipreviewer.h"	//ooo
#include "ui_uipreviewer.h"	//xxx
//Added by qt3to4:
#include <QMouseEvent>
//#include <Q3ValueList>	//oxx
#include <QPixmap>

class QTimer;
//class Q3IconView;     //ooo
class QListWidget;
//class Q3IconViewItem; //ooo
class QListWidgetItem;  //xxx

class MediaInterface;

namespace Input
{

//class Previewer : public uiPreviewer				//ooo
class Previewer : public QWidget, public Ui::uiPreviewer	//xxx
{  // Class to overlay on top of the QIconVIew
   //  to give a preview of the Image / Video
  Q_OBJECT
public:
   //Previewer ( QWidget *, Q3IconView * ); //ooo
   Previewer ( QWidget *, QListWidget * ); 
  ~Previewer ( );

  // Sets the item to be displayed in more detail.
  //void setItem ( Cache::Thumbs::Entry *, Q3IconViewItem * );  //ooo
  void setItem ( Cache::Thumbs::Entry *, QListWidgetItem * );   //xxx
  //void setItem ( Q3IconViewItem * );  //ooo
  void setItem ( QListWidgetItem * );   //xxx
  void clear   ( );

protected slots:
  virtual void slotFirst ( );
  virtual void slotLast  ( );
  virtual void slotNext  ( );
  virtual void slotPrev  ( );
  virtual void slotPlay  ( );
  virtual void slotStop  ( );
  virtual void slotLoadWholeImage ( );
  virtual void slotShowImageInfo  ( );
  virtual void slotAutoPlay       ( );
  virtual void slotResizeM        ( );

protected:
  virtual void mouseReleaseEvent  ( QMouseEvent * );

  void setButtonState ( );
  bool isImage        ( QString & );
  void setMovieFrame  ( );
  void stop           ( );

private:
  bool            m_bImageItem;
  bool            m_bPaused;
  QMatrix       *m_pMatrix;
  //Q3IconView      *m_pIconView;   //ooo
  QListWidget      *m_pIconView;    //xxx
  QString         m_qsFileName;
  //Q3IconViewItem  *m_pCurrentItem;    //ooo
  QListWidgetItem  *m_pCurrentItem;     //xxx
  QTimer         *m_pTimerAutoPlay;
  MediaInterface *m_pMediaInterface;
  QList<QPixmap *> m_listOfThumbs;	//oxx
  int m_iCurrentPix;
};

};  // End of namespace Input

#endif // DIALOGALIGN_H
