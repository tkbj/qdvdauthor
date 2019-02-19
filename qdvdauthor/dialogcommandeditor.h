/***************************************************************************
    dialogcommandeditor.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
****************************************************************************/

#ifndef DIALOGCOMMANDEDITOR_H
#define DIALOGCOMMANDEDITOR_H


// the designer created GUI.
//#include "uicommandeditor.h"		//ooo
#include "ui_uicommandeditor.h"		//xxx
#include "qdvdauthorinit.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

//class DialogCommandEditor : public uiDialogCommandEditor			//ooo
class DialogCommandEditor : public QDialog, Ui::uiDialogCommandEditor		//xxx
{
	Q_OBJECT
public:
	 //DialogCommandEditor ( QWidget *, const char *n=0, Qt::WFlags f=0 );	//ooo
	 DialogCommandEditor ( QWidget *pParent=0 );				//xxx
	~DialogCommandEditor ( );

	void initMe ( QDVDAuthorInit::MenuBlock::Block *, uint );
	void initMe ( QDVDAuthorInit::MenuBlock::Block *, uint, QStringList, uint i=0 );

private slots:
	virtual void slotAddCommand    ( );
	virtual void slotAddToCommand  ( );
	virtual void slotDeleteCommand ( );
	virtual void slotCheckDefault  ( );

private:
	virtual void accept ( );

	void addCommandTab( QString );

private:
	QDVDAuthorInit::MenuBlock::Block *m_pCommandBlock;
	QList<QWidget *>   m_listCommandTabs;
	QList<QCheckBox *> m_listCheckDefault;
	QList<QTextEdit *> m_listEditCommands;
};

#endif // DIALOGCOMMANDEDITOR_H

