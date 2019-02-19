/***************************************************************************
    listviewfilteritem.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
    listboxcolortext.h file.

    This file re-implements the QListBoxText class
    and adds only a background color to it.

    For later this could be enhanced to include a small Icon at the beginning
    Which indicates the type (I.e. single movie, movie collection, or dvd-slideshow).

    Another icon on the right side could indicate if a movie is already
    connected to a button or action.

    And yet another Icon could indicate the (intended) Format of the Movie.
    
****************************************************************************/

//#include <q3listview.h>   //ooo
#include <QListWidget>        //xxx
#include <qpainter.h>
#include <QMessageBox>		//xxx

#include "listviewfileitem.h"
  
//ListViewFileItem::ListViewFileItem (Q3ListView *parent, QString l1, QString l2, QString l3, QString l4, QString l5, QString l6, QString l7, QString l8)   //ooo
ListViewFileItem::ListViewFileItem (QListWidget *parent)      //xxx
	//: Q3ListViewItem (parent, l1, l2, l3, l4, l5, l6, l7, l8)    //ooo
    : QListWidgetItem (parent)       //xxx
{
}

ListViewFileItem::~ListViewFileItem ()
{
}

//int ListViewFileItem::compare ( Q3ListViewItem *i, int col, bool ascending ) const    //ooo
/*int ListViewFileItem::compare ( QListWidgetItem *i, int col, bool ascending ) const       //xxx
{
	// Name | Size | Type | Date | Attr

	// return -1 if this item is less than i
	// return if both items are the same as i
	// return +1 if this item is greater than i

	int iReturn;
	QString qsThisKey = key (4, ascending);	// this are the File attributes. Directories start with drwxrwxrwx files with -rwxrwxrwx
	QString qsOtherKey = i->key (4, ascending);
	// Here we handle the size, which is a numerical sort and not a ASCII sort
	// Note: for files only dir have no size).
	if ( (col == 1) && (qsThisKey[0] == '-') && (qsOtherKey[0] == '-') )	{
			if (key(col, ascending).toInt() > i->key(col, ascending).toInt())
				return 1;
			else
				return -1;
	}
	else 	{
		if (qsThisKey[0] == qsOtherKey[0])
			return key( col, ascending ).compare( i->key( col, ascending) );
	}
	if (qsThisKey[0] == 'd')
		iReturn = 1;
	else
		iReturn = -1;
	// reverse if descending, so we have always the directories up first.
	if (ascending)
		iReturn *= -1;
	return iReturn;
}*/ //ooo

/*void ListViewFileItem::paintCell ( QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	QColorGroup theColors = cg;
	if (isSelected())	{
//		QColor colorSelected (250, 150, 50);
//		theColors.setColor(QColorGroup::Base, colorSelected ); too bad orange does not work for the full line ...
		theColors.setColor(QColorGroup::Base, theColors.highlight() );
		theColors.setColor(QColorGroup::Text, theColors.highlightedText() );
	}
	else if (key(4, true)[0] == 'd')	{
		float fPos = (float)itemPos()/height();
		QColor colorDirectory (200, 200, 255);
		if ((int)fPos%2 == 1)	// every second file we change the color slightly
			colorDirectory = QColor(185, 185, 240);
		theColors.setColor(QColorGroup::Base, colorDirectory);
	}
	else 	{
		float fPos = (float)itemPos()/height();
		QColor colorAlternate (250, 250, 250);
		if ((int)fPos%2 == 1)	// every second file we change the color slightly
			colorAlternate = QColor(235, 235, 235);
		theColors.setColor(QColorGroup::Base, colorAlternate);
	}
	Q3ListViewItem::paintCell(p, theColors, column, width, align);
}*/ //ooo




