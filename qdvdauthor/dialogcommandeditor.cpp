/***************************************************************************
    dialogcommandeditor.cpp
                             -------------------
    DialogCommandEditor class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
****************************************************************************/

#include <QTextEdit>
#include <QLayout>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QTabWidget>
#include <QPushButton>
//Added by qt3to4:
#include <QGridLayout>

#include "dialogcommandeditor.h"

//DialogCommandEditor::DialogCommandEditor (QWidget *pParent, const char *pName, Qt::WFlags f )		//ooo
DialogCommandEditor::DialogCommandEditor (QWidget *pParent)						//xxx
	//: uiDialogCommandEditor (pParent, pName, f)							//ooo
	: QDialog (pParent)										//xxx
{
	setupUi(this);		//xxx
	// Setting of the classes variables ...
	m_pCommandBlock = NULL;
}

DialogCommandEditor::~DialogCommandEditor ()
{
	// The Dialog takes care of the destruction of the generated tabs,
	// so we won't have to do it manually here
}

void DialogCommandEditor::initMe (QDVDAuthorInit::MenuBlock::Block *pBlock, uint iCurrentTab, QStringList listVars, uint iMaxVars )
{
  uint t, iCount;
  initMe (pBlock, iCurrentTab);
  m_pComboVar->clear ();

  iCount = ( iMaxVars > 0 ) ? iMaxVars : listVars.count ();

  for (t=0;t<iCount;t++) 
    //m_pComboVar->insertItem (listVars[t].replace ("+", ""));	//ooo
    m_pComboVar->addItem (listVars[t].replace ("+", ""));	//xxx
}

void DialogCommandEditor::initMe (QDVDAuthorInit::MenuBlock::Block *pBlock, uint iCurrentTab)
{
	uint t;
 	// Here we store the datablock ...
	m_pCommandBlock = pBlock;
	m_pTextEditComment->setText(pBlock->listComments[0]);
	
	// This one tab is static and the others are generated dynamically ...
	m_pTextEditCommand->setText(pBlock->listCommands[0]);
	m_listCommandTabs.append(tab);
	m_listCheckDefault.append(m_pCheckDefault);
	m_listEditCommands.append(m_pTextEditCommand);

	for (t=1;t<(uint)pBlock->listCommands.count();t++)
		addCommandTab(pBlock->listCommands[t]);
	//m_pTabWidget->setCurrentPage(iCurrentTab);	//ooo
	m_pTabWidget->setCurrentIndex(iCurrentTab);	//xxx

	m_listCheckDefault[pBlock->iDefaultCommand]->setChecked(true);
	// And now let us make some connections
	connect (m_pButtonAddCommand, SIGNAL(clicked()), this, SLOT(slotAddCommand ()));
	connect (m_pButtonAddToCommand, SIGNAL(clicked()), this, SLOT(slotAddToCommand ()));
	connect (m_pButtonDeleteCommand, SIGNAL(clicked()), this, SLOT(slotDeleteCommand ()));
	connect (m_pCheckDefault, SIGNAL(clicked()), this, SLOT(slotCheckDefault ()));
}

void DialogCommandEditor::slotAddCommand ()
{
	addCommandTab (QString ());
}

void DialogCommandEditor::addCommandTab(QString qsCommand)
{
	QWidget     *pTab;
	QGridLayout *pLayout;
	QCheckBox   *pCheck;
	QLabel      *pLabel;
	QTextEdit   *pCommand;

	pTab     = new QWidget( m_pTabWidget ); //, QString("tab%1").arg(t) );
	//pLayout  = new Q3GridLayout( pTab, 1, 1, 4, 0 ); //, QString("tabLayout%1").arg(t));	//ooo
	pLayout  = new QGridLayout( pTab ); //, QString("tabLayout%1").arg(t));			//xxx
	pCheck   = new QCheckBox( pTab ); //, QString("m_pCheckDefault%1").arg(t) );		//ooo
	//pCheck   = new QCheckBox( ); //, QString("m_pCheckDefault%1").arg(t) );			//xxx
	//pLayout->addWidget ( pCheck );								//xxx
	pLabel   = new QLabel( pTab ); //, QString("textLabel%1").arg(t) );			//ooo
	//pLabel   = new QLabel( ); //, QString("textLabel%1").arg(t) );				//xxx
	//pLayout->addWidget ( pLabel );								//xxx
	pCommand = new QTextEdit( pTab ); //, QString("pCommand%1").arg(t) );			//ooo
	//pCommand = new QTextEdit( ); //, QString("pCommand%1").arg(t) );			//xxx
	//pLayout->addWidget ( pCommand );							//xxx
	//pCommand->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, m_pTextEditCommand->sizePolicy().hasHeightForWidth() ) );	//ooo
	//pCommand->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );	//xxx
	pCommand->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );	//xxx
	//pCommand->setPaletteForegroundColor( QColor( 255, 0, 0 ) );		//ooo
	//m_pTextEditCommand->setTextColor ( QColor( 255, 0, 0 ) );		//xxx
	pCommand->setTextColor ( QColor( 255, 0, 0 ) );				//xxx
	QPalette palette;							//xxx
	//palette.setBrush(QPalette::Base, QBrush(QColor( 255, 0, 0 )));	//xxx
        //pCommand->setPalette(palette);						//xxx
	//pCommand->setPaletteBackgroundColor( QColor( 194, 185, 255 ) );	//ooo
	//m_pTextEditCommand->setTextBackgroundColor ( QColor( 194, 185, 255 ) );	//xxx
	//pCommand->setTextBackgroundColor ( QColor( 194, 185, 255 ) );		//xxx
	palette.setBrush(QPalette::Base, QBrush(QColor( 194, 185, 255 )));	//xxx
        pCommand->setPalette(palette);						//xxx
	//palette.setBrush(QPalette::Base, QBrush(QColor( 194, 185, 255 )));	//xxx
        //m_pTextEditCommand->setPalette(palette);				//xxx

	m_listCommandTabs.append(pTab);
	m_listCheckDefault.append(pCheck);
	m_listEditCommands.append(pCommand);

	pLayout->addWidget( pCheck, 0, 1 );
	pLayout->addWidget( pLabel, 0, 0 );
	//pLayout->addMultiCellWidget( pCommand, 1, 1, 0, 1 );					//ooo
	pLayout->addWidget( pCommand, 1, 0, 1, 2 );						//xxx
	//setLayout(pLayout);									//xxx
	//m_pTabWidget->insertTab( pTab, QString("Cmd %1").arg(m_listEditCommands.count()) );	//ooo
	m_pTabWidget->addTab( pTab, QString("Cmd %1").arg(m_listEditCommands.count()) );	//xxx

	pCheck->setText( tr( "Use as default command" ) );
	pLabel->setText( tr( "<p align=\"center\">Command Line</p>" ) );
	pCommand->setText(qsCommand);
	connect (pCheck, SIGNAL(clicked()), this, SLOT(slotCheckDefault ()));
}

void DialogCommandEditor::slotAddToCommand ()
{
	// This slot will insert the selected string into the command 
	QTextEdit *pCommand;
	//pCommand = m_listEditCommands[m_pTabWidget->currentPageIndex()];			//ooo
	pCommand = m_listEditCommands[m_pTabWidget->currentIndex()];				//xxx
	//pCommand->insert(QString("+")+m_pComboVar->currentText()+QString("+"));		//ooo
	pCommand->insertPlainText(QString("+")+m_pComboVar->currentText()+QString("+"));	//xxx
}

void DialogCommandEditor::slotDeleteCommand ()
{
	// Here we delete a command and remove all 
	uint iCmdToRemove, t;
	// Sanity check, we need to keep at least one command
	if (m_pTabWidget->count() <= 1)
		return;
	//iCmdToRemove = m_pTabWidget->currentPageIndex ();	//ooo
	iCmdToRemove = m_pTabWidget->currentIndex ();		//xxx
	// Here we shift all widgets over to the left and delete the last tabwidget ...
	for (t=iCmdToRemove;t<(uint)m_listCheckDefault.count()-1;t++)
		m_listCheckDefault[t]->setChecked(m_listCheckDefault[t+1]->isChecked());
	for (t=iCmdToRemove;t<(uint)m_listEditCommands.count()-1;t++)
		//m_listEditCommands[t]->setText(m_listEditCommands[t+1]->text());	//ooo
		m_listEditCommands[t]->setText(m_listEditCommands[t+1]->toPlainText());	//xxx
	// Next we remove the references  from the dynamic lists ...
	QWidget *pTab = m_listCommandTabs.last();
	//m_listCommandTabs.remove(pTab);				//ooo
	m_listCommandTabs.removeOne(pTab);				//xxx
	//m_listCheckDefault.remove(m_listCheckDefault.last());		//ooo
	m_listCheckDefault.removeOne(m_listCheckDefault.last());	//xxx
	//m_listEditCommands.remove(m_listEditCommands.last());		//ooo
	m_listEditCommands.removeOne(m_listEditCommands.last());	//xxx
	// And then finally we delete the tab widget ...
	delete pTab;
}

void DialogCommandEditor::slotCheckDefault ()
{
	// here we work the checks ...
	uint t;
	bool bActiveChecked = true;

	// Finally we ensure that only one check is set ...
	for (t=0;t<(uint)m_listCheckDefault.count();t++)	{
		if (m_listCheckDefault[t]->isVisible())
			bActiveChecked = m_listCheckDefault[t]->isChecked ();
		else
			m_listCheckDefault[t]->setChecked(false);
	}
	// In case the user actually unchecked the checkDefault, we set the first command as the default.
	if (!bActiveChecked)
		m_listCheckDefault[0]->setChecked(true);
}

void DialogCommandEditor::accept ()
{
  // Handles the Ok - button ...
  uint t;
  m_pCommandBlock->listCommands.clear();
  // First we store the command list of this execution block in the data structure ...
  for (t=0;t<(uint)m_listEditCommands.count();t++)	{
    //if (!m_listEditCommands[t]->text().isEmpty())		//ooo
    if (!m_listEditCommands[t]->toPlainText().isEmpty())	//xxx
      //m_pCommandBlock->listCommands.append(m_listEditCommands[t]->text());		//ooo
      m_pCommandBlock->listCommands.append(m_listEditCommands[t]->toPlainText());	//xxx
  }
  // Here we store the default commands index ...
  for (t=0;t<(uint)m_listCheckDefault.count();t++)	{
    if (m_listCheckDefault[t]->isChecked())
      m_pCommandBlock->iDefaultCommand = t;
  }
  // And finally we store the header description of this command lines function
  //m_pCommandBlock->listComments[0] = m_pTextEditComment->text();		//ooo
  m_pCommandBlock->listComments[0] = m_pTextEditComment->toPlainText();		//xxx

  //uiDialogCommandEditor::accept();	//ooo
  QDialog::accept();			//xxx
}


