/***************************************************************************
    structuretoolbar.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This are the classes for the StructureWidget.
   The StructureWidget is displaying the current structure of the DVD.
    
****************************************************************************/
 
#ifndef STRUCTURETOOLBAR_H
#define STRUCTURETOOLBAR_H

#include <QDockWidget>
#include <QListView>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

class SourceToolBar;
class MenuPreview;
class MenuObject;
class DVDMenu;

class StructureItem : public QTreeWidgetItem
{
public:
  enum enType { Unknown=0, DVDProject, MainMenu, SubMenu, Intro, Extro, Background, Audio, Buttons, Button, Frame, Text, Image, Video, Highlighted, Selected, Geometry, Modifiers, Attributes };
  
  StructureItem ( MenuObject *pMenuObject, enType type, QTreeWidgetItem *pListViewItem, QString q1=QString::null, QString q2=QString::null)		//xxx (1)
    : QTreeWidgetItem ( pListViewItem )
    //{ initMe ( type ); m_pMenuObject = pMenuObject; }					//ooo
    { initMe ( type ); m_pMenuObject = pMenuObject; setText(0, q1); setText(1, q2); }	//xxx
    
  StructureItem ( MenuObject *pMenuObject, enType type, QTreeWidgetItem *pParent, QTreeWidgetItem *pAfter, QString q1=QString::null, QString q2=QString::null)	//xxxx (2)
    : QTreeWidgetItem ( pParent, pAfter )
    //{ initMe ( type ); m_pMenuObject = pMenuObject; };					//ooo
    { initMe ( type ); m_pMenuObject = pMenuObject; setText(0, q1); setText(1, q2); };		//xxx
    
  StructureItem ( MenuPreview *pMenuPreview, enType type, QTreeWidgetItem *pListViewItem, QTreeWidgetItem *pAfter, QString q1=QString::null, QString q2=QString::null )	//xxx (3)
      //: QTreeWidgetItem (pListViewItem, pAfter, q1)		//xxx
      : QTreeWidgetItem (pListViewItem, pAfter)			//xxx
    //{ initMe ( type ); m_pMenuPreview = pMenuPreview; };      //ooo
    { initMe ( type ); m_pMenuPreview = pMenuPreview; setText(0, q1); setText(1, q2); };    //xxxx
    
  StructureItem ( DVDMenu *pDVDMenu, enType type, QTreeWidgetItem *pListViewItem, QTreeWidgetItem *pAfter, QString q1=QString::null, QString q2=QString::null)	//xxxx (4)
    : QTreeWidgetItem (pListViewItem, pAfter) 
    //{ initMe ( type ); m_pDVDMenu = pDVDMenu; };					//ooo
    { initMe ( type ); m_pDVDMenu = pDVDMenu; setText(0, q1); setText(1, q2); };	//xxx
   
  StructureItem ( QTreeWidgetItem *pListViewItem, enType type, QString q1=QString::null, QString q2=QString::null)	//xxx (5)
    : QTreeWidgetItem ( pListViewItem )
    //{ initMe ( type ); };					//ooo
    { initMe ( type ); setText(0, q1); setText(1, q2); };	//xxx
   
  //StructureItem ( Q3ListView *pListView, enType type, QString q1=QString::null)	//xxx (6)
  //StructureItem ( QTreeWidget *pListView, enType type, q1)				//xxx
  //StructureItem ( QTreeWidget *pListView, enType type)					//xxxx
  StructureItem ( QTreeWidget *pListView, enType type, QString q1=QString::null)	//xxx
    //: QTreeWidgetItem ( pListView )->setText(0, tr(q1))					//xxx
    : QTreeWidgetItem ( pListView )							//xxx
    //{ initMe ( type ); };								//xxxx
    { initMe ( type ); setText(0, q1); };						//xxx
    
  virtual ~StructureItem ( );
  
  void initMe  ( enType );
  void setPixmap             ( enType );
  
  StructureItem  *setSibling ( MenuObject *,  StructureItem *, QString, QString );
  StructureItem  *lastChild  ( );
  MenuObject  *getMenuObject ( ) { return m_pMenuObject; };
  
private:
  MenuObject  *m_pMenuObject;
  MenuPreview *m_pMenuPreview;
  enType       m_type;
  DVDMenu     *m_pDVDMenu; // used or Intro / Extro ...
};

class StructureToolBar : public QWidget //QDockWindow
{
  Q_OBJECT

public:
  StructureToolBar ( QDockWidget * );
  virtual ~StructureToolBar   ( );

  StructureItem *clean   ( bool );

private:
  SourceToolBar *m_pSourceToolBar;
};

#endif // STRUCTURETOOLBAR_H
