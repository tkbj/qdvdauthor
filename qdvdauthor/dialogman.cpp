/***************************************************************************
    dialogman.cpp
                             -------------------
    MenuPreview class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This file handles the user input to generate a DVD menu.
   Please note that this is work in progress and depends also oin the
   developement of dvdauthor, which is at the moment in version 0.67.
   Future versions of dvdauthort might need changes to this file.
    
****************************************************************************/

#include "dialogman.h"

#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextEdit>
#include <QLineEdit>
#include <QTextStream>
#include <QFileInfo>

//DialogMan::DialogMan( QStringList &listOfCommands, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )	//ooo
DialogMan::DialogMan( QStringList &listOfCommands, QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl )	//xxx
    //: QDialog( parent, name, modal, fl )	//ooo
    : QDialog( parent )				//xxx

{
  uint t;
  //setName( "DialogMan" );			//ooo
  setWindowTitle( tr( "DialogMan" ) );		//xxx
  //MyDialogLayout = new Q3GridLayout( this, 1, 1, 11, 6, "MyDialogLayout");	//ooo
  MyDialogLayout = new QGridLayout( this );					//xxx
  
  //Layout1 = new Q3HBoxLayout( 0, 0, 6, "Layout1");	//ooo
  Layout1 = new QHBoxLayout( this); 			//xxx

  //buttonHelp = new QPushButton( this, "buttonHelp" );	//ooo
  buttonHelp = new QPushButton( this  );		//xxx
  //buttonHelp->setAutoDefault( TRUE );			//ooo
  buttonHelp->setAutoDefault( true );
  Layout1->addWidget( buttonHelp );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout1->addItem( spacer );
  
  //m_pEditFind = new QLineEdit ( this, "m_pEditFind" );	//ooo
  m_pEditFind = new QLineEdit ( this );				//xxx
  Layout1->addWidget( m_pEditFind );
  
  //buttonFind = new QPushButton( this, "buttonFind" );		//ooo
  buttonFind = new QPushButton( this );				//xxx
  //buttonFind->setAutoDefault( TRUE );				//ooo
  buttonFind->setAutoDefault( true );				//xxx
  //buttonFind->setDefault( TRUE );				//ooo
  buttonFind->setDefault( true );				//xxx
  Layout1->addWidget( buttonFind );
    
  //buttonOk = new QPushButton( this, "buttonOk" );		//ooo
  buttonOk = new QPushButton( this );				//xxx
  //buttonOk->setAutoDefault( TRUE );				//ooo
  buttonOk->setAutoDefault( true );				//xxx
  //buttonOk->setDefault( TRUE );				//ooo
  buttonOk->setDefault( true );					//xxx
  Layout1->addWidget( buttonOk );

  //buttonCancel = new QPushButton( this, "buttonCancel" );	//ooo
  buttonCancel = new QPushButton( this );			//xxx
  //buttonCancel->setAutoDefault( TRUE );			//ooo
  buttonCancel->setAutoDefault( true );				//xxx
  Layout1->addWidget( buttonCancel );
  
  MyDialogLayout->addLayout( Layout1, 1, 0 );
  
  //tabWidget2 = new QTabWidget( this, "tabWidget2" );	//ooo
  tabWidget2 = new QTabWidget( this );			//xxx
  
  QWidget		*pTab;
  //Q3GridLayout	*pTabLayout;	//ooo
  QGridLayout	*pTabLayout;		//xxx
  //Q3TextEdit	*pTextEdit;		//ooo
  QTextEdit	*pTextEdit;		//xxx
  QString		 command;
  QString		line;
  for (t=0;t<(uint)listOfCommands.count();t++)	{
    //pTab = new QWidget( tabWidget2, (const char *)listOfCommands[t] );			//ooo
    pTab = new QWidget( tabWidget2 );								//xxx
    //pTabLayout = new Q3GridLayout( pTab, 1, 1, 11, 6, QString("tabLayout%1").arg(t));		//ooo
    pTabLayout = new QGridLayout( pTab );							//xxx

    //pTextEdit = new Q3TextEdit( pTab, QString("textEdit%1").arg(t) );		//ooo
    pTextEdit = new QTextEdit( QString("textEdit%1").arg(t), pTab );		//xxx
    //pTextEdit->setPaletteForegroundColor( QColor( 119, 194, 40 ) );		//ooo
    pTextEdit->setTextColor       ( QColor( 119, 194, 40 ) );			//xxx
    //pTextEdit->setPaletteBackgroundColor( QColor( 0, 0, 0 ) );		//ooo
    QPalette palette;								//xxx
    //palette.setBrush(pTextEdit->backgroundRole(), QColor( 0, 0, 0 ));		//xxx
    palette.setBrush(QPalette::Base, QColor( 0, 0, 0 ));			//xxx
    pTextEdit->setPalette(palette);						//xxx
    pTextEdit->setReadOnly ( true );

    pTabLayout->addWidget( pTextEdit, 0, 0 );
    //tabWidget2->insertTab( pTab, (QString ("man ") + listOfCommands[t] ));	//ooo
    tabWidget2->addTab( pTab, (QString ("man ") + listOfCommands[t] ));		//xxx

    // here we put the man pages into a text file and read tis file into the textEdit.
    command = QString("man %1 2>/dev/null | col -b > /tmp/%2.man").arg(listOfCommands[t]).arg(listOfCommands[t]);
    //if ( system ((const char *)command) == -1 )	//ooo
    if ( system (command.toLatin1().data()) == -1 )	//xxx
      return;
    command = QString("/tmp/%1.man").arg(listOfCommands[t]);
    QFileInfo fileInfo(command);
    // Check to see if we found a man page for this command ...
    if (fileInfo.size() < 10)
      pTextEdit->append(QString("No man page found for command %1.").arg(listOfCommands[t]));
    else	{	// Okay seems like we foun d a valid man page ...
      QFile file (command);
      file.open (QIODevice::ReadOnly);
      QTextStream in(&file);					//xxx
      //while (file.readLine(line, 1024) > -1)	{	//ooo
      //	pTextEdit->append(line);
      //}
      while (!in.atEnd()) {					//xxx
        QString line = in.readLine();
        pTextEdit->append(line);
      }
      file.close();
    }
    // go back to the top.
    //pTextEdit->center(0, 0);		//ooo
    m_listTextEdit.append(pTextEdit);
  }
  
  MyDialogLayout->addWidget( tabWidget2, 0, 0 );
  languageChange();
  resize( QSize(783, 614).expandedTo(minimumSizeHint()) );

  // signals and slots connections
  connect( buttonFind, SIGNAL( clicked() ), this, SLOT( slotFind() ) );
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DialogMan::~DialogMan()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogMan::languageChange()
{
	//setCaption( tr( "DialogMan" ) );				//ooo
	buttonHelp->setText( tr( "&Help" ) );
	//buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );		//ooo
	buttonFind->setText( tr( "&Find" ) );
	//buttonFind->setAccel( QKeySequence( QString::null ) );	//ooo
	buttonOk->setText( tr( "&OK" ) );
	//buttonOk->setAccel( QKeySequence( QString::null ) );		//ooo
	buttonCancel->setText( tr( "&Cancel" ) );
	//buttonCancel->setAccel( QKeySequence( QString::null ) );	//ooo
}

void DialogMan::slotFind ()
{
  // This function will take the entered text and search for it in the Man page currently active ...
  if (m_listTextEdit.count() < 1)
    return;

  //int iIndex = tabWidget2->currentPageIndex ( );	//ooo
  int iIndex = tabWidget2->currentIndex ( );		//xxx
  //Q3TextEdit *pTextEdit = m_listTextEdit[ iIndex < (int)m_listTextEdit.count ( ) ? iIndex : 0 ];	//ooo
  QTextEdit *pTextEdit = m_listTextEdit[ iIndex < (int)m_listTextEdit.count ( ) ? iIndex : 0 ];
  //bool bFound = pTextEdit->find (m_pEditFind->text(), false, false);	//ooo
  bool bFound = pTextEdit->find (m_pEditFind->text());			//xxx
  if (!bFound)	{
    // Set cursor back to the top ...
    //pTextEdit->setCursorPosition(0, 0);		//ooo
    pTextEdit->cursorForPosition(QPoint(0, 0));		//xxx
    //bFound = pTextEdit->find (m_pEditFind->text(), false, false);	//ooo
    bFound = pTextEdit->find (m_pEditFind->text());			//xxx
    if (!bFound)
      return;
  }
}
