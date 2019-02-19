/***************************************************************************
    dialogvfolder.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    Form interface generated from reading ui file 'uidialogtemp.ui'

****************************************************************************/

#ifndef DIALOGVFOLDER_H
#define DIALOGVFOLDER_H

//#include <q3listview.h>   //ooo
#include <QListView>
//Added by qt3to4:
#include <QTimerEvent>
//#include <Q3ValueList>	//oxx
#include <QTreeWidget>      //xxx
#include <QTreeWidgetItem>  //xxx

#include "cachethumbs.h"
//#include "uidialogvfolder.h"		//ooo
#include "ui_uidialogvfolder.h"		//xxx

namespace Input 
{

//class DialogVFolder : public uiDialogVFolder				//ooo
class DialogVFolder : public QDialog, public Ui::uiDialogVFolder	//xxx
{
  Q_OBJECT

  //class VFolderItem : public Q3ListViewItem   //ooo
  class VFolderItem : public QTreeWidgetItem    //xxx
  {
  public:
    //VFolderItem ( Cache::Thumbs::Entry *, Q3ListView *, QString );    //ooo
    VFolderItem ( Cache::Thumbs::Entry *, QTreeWidget *, QString );     //xxx
    //virtual void paintCell ( QPainter *, const QColorGroup &, int, int, int );    //oooo

    bool bAlternateColor;
    Cache::Thumbs::Entry *pEntry;
  };

public:
   //DialogVFolder ( QWidget *p=0, const char *n=0, bool m=FALSE, Qt::WFlags fl=0 );	//ooo
   DialogVFolder ( QWidget *pParent=0 );						//xxx
  ~DialogVFolder ( );

  void initMe ( QString, bool );
  QList<Cache::Thumbs::Entry *> &getEntries ( );	//oxx

protected slots:
  virtual void slotAdd    ( );
  virtual void slotDelete ( );
  virtual void slotUp     ( );
  virtual void slotDown   ( );
  virtual void slotAlternateColors ( );

  virtual void accept ( );
  virtual void reject ( );

protected:
  virtual void timerEvent ( QTimerEvent * );

private:
  //Q3ListViewItem *m_pNextItem;    //ooo
  QTreeWidgetItem *m_pNextItem;     //xxx
  QString m_qsFolderName;
  int     m_iThumbSize;
  bool    m_bCanClose;
  bool    m_bIsImageFolder;
  QList<Cache::Thumbs::Entry *> m_returnList;		//oxx
};

}; // end namespace Input

#endif // DIALOGVFOLDER_H
