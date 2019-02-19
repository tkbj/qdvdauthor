/****************************************************************************
** Class DialogFilter
**
**   Created : Sun May 22 09:13:25 2009
**        by : Varol Okan, using Kate
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#ifndef DIALOGBUTTONTRANS_H
#define DIALOGBUTTONTRANS_H

//#include <q3iconview.h>   //ooo
#include <QListWidget>      //xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QPixmap>

//#include "uidialogbuttontrans.h"	//ooo
#include "ui_uidialogbuttontrans.h"	//xxx

#ifndef THUMBNAIL_COUNT
#define THUMBNAIL_COUNT 5 * 10
#endif

class QPixmap;
class QTimer;

class ButtonTransition;

//class DialogButtonTrans : public uiDialogButtonTrans				//ooo
class DialogButtonTrans : public QDialog, public Ui::uiDialogButtonTrans	//xxx
{
  Q_OBJECT

protected:
    //class Item : public Q3IconViewItem  { //ooo
    class Item : public QListWidgetItem  {  //xxx
    public:
      //Item ( Q3IconView *, const QString &, const QPixmap &, bool );  //ooo
      Item ( QListWidget *, const QString &, const QPixmap &, bool );   //xxx
      void setChecked  ( bool );
      bool toggle      ( );
      bool isChecked   ( );
      QString fileName ( );

    protected:
      virtual void calcRect ( const QString &s=QString::null );

    private:
      void refreshIcon ( );

    private:
      bool    m_bChecked;
      QString m_qsFileName;
      QPixmap m_pixmap;
    };

public:
           DialogButtonTrans ( ButtonTransition *, QStringList &, QWidget *p=0 );
           DialogButtonTrans ( ButtonTransition *, QWidget *p=0 );
  virtual ~DialogButtonTrans ( );

  ButtonTransition *getTransition  ( );
  void initEnabled (  QStringList  & );
  QStringList      &getEnabledList ( );

protected:
  void initMe       ( bool );
  void createTab    ( QString, QString, QStringList & );
  bool isChecked    ( QString & );
  void setLabel     ( QString );
  void recreatePix  ( );

protected slots:
  //virtual void slotClicked      ( int, Q3IconViewItem *, const QPoint & );    //ooo
  virtual void slotClicked      ( int, QListWidgetItem *, const QPoint & );     //xxx
  virtual void slotNoTransition ( );
  virtual void slotUserDefined  ( );
  virtual void slotAfterInit    ( );
  virtual void slotSelectAll    ( );
  virtual void slotClearAll     ( );
  virtual void slotAnimPix      ( );
  virtual void slotBrowse       ( );
  virtual void slotAnim         ( );
  virtual void slotHelp         ( );    //xxx
  virtual void accept           ( );

protected:
  QPixmap                *m_arrayPix[THUMBNAIL_COUNT];
  QTimer                 *m_pTimer;
  bool                    m_bAnimated;
  bool                    m_bMultiMode;
  int                     m_iCurrentPix;
  int                     m_iThumbSize;
  QString                 m_qsTransitionName;
  //QList<Q3IconView *> m_listViews;		//oxx
  QList<QListWidget *> m_listViews;	    	//xxx
  ButtonTransition       *m_pTransition;
  QStringList             m_listEnabled;
};

#endif // DIALOGBUTTONTRANS_H

