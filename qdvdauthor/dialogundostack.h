/***************************************************************************
    dialogundostack.h
                             -------------------
    Class DialogUndoStack                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  This class will display the undo-stack of the current DVDMenu.
  It will also group together all consecutive changes to one object
  in the tree-view, so that the user can undo the whole batch of changes
  applied to one object at once.
    
****************************************************************************/

#ifndef DIALOGUNDOSTACK_H
#define DIALOGUNDOSTACK_H

#include <QListView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>

#define MAX_TREE_SIZE 51

class QHBoxLayout;
//class QTreeWidget		//ooo
//class QTreeWidgetItem;	//ooo

class QSpacerItem;
class QPushButton;

class UndoBuffer;

class DialogUndoStack : public QDialog
{
  Q_OBJECT
  
  //class Item : public Q3ListViewItem  {	//ooo
  class Item : public QTreeWidgetItem  {	//xxx
    public:
      //Item ( Q3ListView *, int, bool, QString, QString s=QString::null );		//ooo
      Item ( QTreeWidget *, int, bool, QString, QString s=QString::null );		//xxx
      //Item ( Q3ListViewItem *, int, bool, QString, QString s=QString::null );		//ooo
      Item ( QTreeWidgetItem *, int, bool, QString, QString s=QString::null );		//xxx
      virtual ~Item ( );
      
      static bool m_bStartAdjust;
      
  };
  
public:
   //DialogUndoStack ( UndoBuffer *, QWidget* p=0, const char* n=0, bool m=FALSE, Qt::WFlags fl=0 );	//ooo
  DialogUndoStack ( UndoBuffer *, QWidget* p=0, const char* n=0, bool m=false, Qt::WindowFlags fl=0 );	//xxx
  ~DialogUndoStack ( );
  
  void setUndoBuffer ( UndoBuffer * );
  
protected slots:
  virtual void languageChange ( );
  
protected:
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    //Q3ListView* m_pListViewUndo;	//ooo
    QListView* m_pListViewUndo;		//xxx
    QGridLayout* DialogUndoStackLayout;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

private:  // private member varaibles.
  UndoBuffer    *m_pUndoBuffer;
  int            m_iCounter;
  //Q3ListViewItem *m_pBaseItem;	//ooo
  QTreeWidgetItem *m_pBaseItem;    	//xxx
};

#endif // DIALOGUNDOSTACK_H
