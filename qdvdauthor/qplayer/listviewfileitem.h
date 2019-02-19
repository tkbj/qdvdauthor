/***************************************************************************
    listviewfilteritem.h
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    This class re-implements the QListViewItem class and adds a background
    color to it.

****************************************************************************/

#ifndef LISTVIEWFILEITEM_H
#define LISTVIEWFILEITEM_H

//#include <q3listview.h>   //ooo
#include <QListWidgetItem>        //xxx

//class ListViewFileItem : public Q3ListViewItem    //ooo
class ListViewFileItem : public QListWidgetItem       //xxx
{
public: 
	//ListViewFileItem (Q3ListView *, QString, QString l2=QString::null, QString l3=QString::null, QString l4=QString::null,   //ooo
		//QString l5=QString::null, QString l6=QString::null, QString l7=QString::null, QString l8=QString::null);             //ooo
    ListViewFileItem (QListWidget *);      //xxx
	virtual ~ListViewFileItem ();

	//virtual int  compare ( Q3ListViewItem *, int, bool) const;   //ooo
    //virtual int  compare ( QListWidgetItem *, int, bool) const;      //xxx
	//virtual void paintCell ( QPainter *, const QColorGroup &, int, int, int);    //ooo
};

#endif // LISTVIEWFILEITEM_H
