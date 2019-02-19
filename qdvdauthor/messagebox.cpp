/***************************************************************************
    messagebox.cpp
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class was neccesary when all static functions from QMessageBox
   seemed to behave out of order on SuSE 10.x (KDE only)
   E.g. the return val of
   QMessageBox::warning (..., QMessageBox::Yes, QMessageBox::No );
   was 0 or 1, instead of QMessageBox::[Yes or No].
   Other statics such as QFileDialog::getFileName(..) expanded way to long
    
****************************************************************************/

//#include <qlayout.h>		//ooo
#include <QLayout>		//xxx
//#include <qpushbutton.h>	//ooo
#include <QPushButton>		//xxx
//#include <q3textedit.h>	//ooo
#include <QTextEdit>		//xxx
//#include <qdialog.h>		//ooo
#include <QDialog>		//xxx
//#include <qfile.h>		//ooo
#include <QFile>		//xxx
//Added by qt3to4:
//#include <Q3GridLayout>	//ooo
#include <QGridLayout>		//xxx
//#include <Q3TextStream>	//ooo
#include <QTextStream>		//xxx
#include <QMessageBox>      //xxx

#include "CONFIG.h"
#include "global.h"

#include "messagebox.h"

MessageBox::MessageBox ()
{

}

MessageBox::~MessageBox ()
{

}

int MessageBox::information ( QWidget *parent, const QString &caption, const QString &text, int button0, int button1, int button2 )
{
  return QMessageBox::information ( parent, caption, text, button0, button1, button2 );
}

int MessageBox::information ( QWidget *parent, const QString &caption, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, int defaultButtonNumber, int escapeButtonNumber )
{
  return QMessageBox::information ( parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber, escapeButtonNumber );
}

int MessageBox::question ( QWidget *parent, const QString &caption, const QString &text, int button0, int button1, int button2 )
{
#if ( QT_VERSION > 0x0301FF )
  return QMessageBox::question ( parent, caption, text, button0, button1, button2 );
#else
  return QMessageBox::warning ( parent, caption, text, button0, button1, button2 );
#endif
}

int MessageBox::question ( QWidget *parent, const QString &caption, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, int defaultButtonNumber, int escapeButtonNumber )
{
#if ( QT_VERSION > 0x0301FF )
  return QMessageBox::question ( parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber, escapeButtonNumber );
#else 
  return QMessageBox::warning ( parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber, escapeButtonNumber );
#endif
}

int MessageBox::warning ( QWidget *parent, const QString &caption, const QString &text, int button0, int button1, int button2 )
{
  //  return QMessageBox::warning ( parent, caption, text, button0, button1, button2 );
  QMessageBox mb( caption, text, QMessageBox::Warning, button0, button1, button2, parent );
  return mb.exec();
}

int MessageBox::warning ( QWidget *parent, const QString &caption, const QString &text, 
const QString &button0Text, const QString &button1Text, const QString &button2Text, int, int )
{
  //  return QMessageBox::warning ( parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber, escapeButtonNumber );
  QMessageBox mb( parent );
  //mb.setCaption ( caption );			//ooo
  mb.setText    ( text    );
  mb.setIcon    ( QMessageBox::Warning);

  if ( button0Text == QString::null )
    mb.setButtonText ( 0, QObject::tr("Ok") );
  else
    mb.setButtonText ( 0, button0Text );
  if ( button1Text != QString::null )
    mb.setButtonText ( 1, button1Text );
  if ( button2Text != QString::null )
    mb.setButtonText ( 2, button2Text );

  return mb.exec();
}

/*
QMessageBox mb( QObject::tr ("QDVDAuthor - init file seems to be defective."),
		QObject::tr ("The file %1%2 seems  to have a problem. Do you want to reset this file ?").arg(INIT_DIR_NAME).arg(INIT_FILE_NAME),
		QMessageBox::Warning, QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape, QMessageBox::NoButton );

if ( mb.exec() == QMessageBox::Yes )
  printf ("(0) i=<%d> Yes=<%d> == <%s>\n", iReturn, QMessageBox::Yes, (iReturn == QMessageBox::Yes) ? "true" : "false");
*/
int MessageBox::html ( QWidget *pParent, const QString &qsCaption, const QString &qsText, int iButton1, int iButton2 )
{
  QSize defaultSize ( 460, 320 );
  return MessageBox::html ( pParent, qsCaption, qsText, defaultSize, iButton1, iButton2 );
}

int MessageBox::html ( QWidget *pParent, const QString &qsCaption, const QString &qsText, QSize initialSize, int iButton1, int iButton2 )
{
  QString qsButtonText, qsName ( "HtmlDialog" );
  //QDialog theDialog ( pParent, qsName, true );	//ooo
  QDialog theDialog ( pParent );			//xxx
  QPushButton *pButton, *pButton1, *pButton2;
  int iButton;
  
  //Q3GridLayout *pLayout = new Q3GridLayout( &theDialog, 2, 2, 11, 6, "HtmlDialogLayout");	//ooo
  QGridLayout *pLayout = new QGridLayout( &theDialog );						//xxx
  //pLayout->setColumnStretch( 2, 11 );    //xxx
  //pLayout->setRowStretch( 2, 6 );  //xxx
  pButton = pButton1 = pButton2 = NULL;
  QHBoxLayout *layout = new QHBoxLayout;    //xxx
  layout ->addStretch();    //xxx

  if ( iButton1 != Qt::NoButton ) {
    //pButton1 = new QPushButton( &theDialog, "pButton" );	//ooo
    pButton1 = new QPushButton( &theDialog );			//xxx
    //pButton1->setAutoDefault ( TRUE  );	//ooo
    pButton1->setAutoDefault ( true  );		//xxx
    //pButton1->setDefault     ( TRUE  );	//ooo
    pButton1->setDefault     ( true  );		//xxx
    pButton1->setText        ( "&OK" );
    //pButton1->setAccel       ( QKeySequence( QString::null ) );	//ooo
    if ( iButton2 == Qt::NoButton )
      //pLayout ->addMultiCellWidget ( pButton1, 1, 1, 0, 1 );		//ooo
      layout ->addWidget ( pButton1 );			//xxx
    else
      //pLayout ->addWidget    ( pButton1, 1, 0 );  //ooo
      //pLayout ->addWidget    ( pButton1, 1, 0 );  //xxx
      layout ->addWidget    ( pButton1 );
  }

  if ( iButton2 != Qt::NoButton ) {
    //pButton2 = new QPushButton( &theDialog, "pButton" );	//ooo
    pButton2 = new QPushButton( &theDialog );			//xxx
    //pButton2->setAutoDefault ( TRUE  );	//ooo
    pButton2->setAutoDefault ( true  );		//xxx
    //pButton2->setDefault     ( FALSE );	//ooo
    pButton2->setDefault     ( false );		//xxx
    pButton2->setText        ( "&Cancel" );
    //pButton2->setAccel       ( QKeySequence( QString::null ) );	//ooo
    if ( iButton1 == Qt::NoButton )
      //pLayout ->addMultiCellWidget ( pButton2, 1, 1, 0, 1 );		//ooo
      layout ->addWidget ( pButton2 );			//xxx
    else
      //pLayout ->addWidget    ( pButton2, 1, 1 );  //ooo
      //pLayout ->addWidget    ( pButton2, 1, 1 );  //xxx
      layout ->addWidget    ( pButton2 );  //xxx
  }

  pButton = pButton1;
  iButton = iButton1;
  for  ( int t=0; t<2; t++ )  {
    if ( iButton == QMessageBox::Ok )
      qsButtonText = QObject::tr ( "&OK" );
    else if( iButton == QMessageBox::Yes )
      qsButtonText = QObject::tr ( "&Yes" );
    else if( iButton == QMessageBox::No )
      qsButtonText = QObject::tr ( "&No" );
    else if( iButton == QMessageBox::Cancel )      
      qsButtonText = QObject::tr ( "&Cancel" );
    else if ( iButton == Qt::NoButton )
      continue;
    pButton->setText ( qsButtonText );
    iButton = iButton2;
    pButton = pButton2;
  }

  //Q3TextEdit *pEdit = new Q3TextEdit ( &theDialog, "m_pEdit" );	//ooo
  QTextEdit *pEdit = new QTextEdit ( &theDialog );			//xxx
  //pEdit->setFrameShadow       ( Q3TextEdit::Plain );			//ooo
  pEdit->setFrameShadow       ( QTextEdit::Plain );			//xxx
  //pEdit->setReadOnly          ( TRUE );				//ooo
  pEdit->setReadOnly          ( true );					//xxx
  pEdit->setText              ( qsText );
  //pLayout->addMultiCellWidget ( pEdit, 0, 0, 0, 1 );			//ooo
  pLayout->addWidget ( pEdit, 0, 0 );				//xxx
  pLayout->addLayout ( layout, 1, 0 );				//xxx
  

  //theDialog.setSizeGripEnabled ( TRUE );				//ooo
  theDialog.setSizeGripEnabled ( true );				//xxx
  //theDialog.setName            ( qsName );				//ooo
  //theDialog.setCaption         ( qsCaption );				//ooo
  //  theDialog.clearWState        ( QDialog::WState_Polished );
  theDialog.resize             ( initialSize.expandedTo ( theDialog.minimumSizeHint ( ) ) );
  //theDialog.resize             ( QSize(462, 317).expandedTo ( theDialog.minimumSizeHint ( ) ) );

  // signals and slots connections
  if ( pButton1 )
    theDialog.connect( pButton1, SIGNAL( clicked ( ) ), &theDialog, SLOT( accept ( ) ) );
  if ( pButton2 )
    theDialog.connect( pButton2, SIGNAL( clicked ( ) ), &theDialog, SLOT( reject ( ) ) );
  
  //theDialog.setLayout(pLayout);    //xxx

  int iReturn = theDialog.exec ( );

  if ( iReturn == QDialog::Accepted )
    return iButton1;
  return iButton2;
}

int MessageBox::help ( QWidget *pParent, const QString &qsCaption, const QString &qsFile, QSize initialSize, int iButton1, int iButton2 )
{
  int  t;
  bool bFound = false;
  QString qsText, qsFileName;

  qsFileName = QString (  Global::qsSystemPath + "/share/qdvdauthor/html/%1/%2" ).arg( Global::qsLanguage ).arg( qsFile );

  QFile theFile( qsFileName );

  for ( t=0;t<2;t++) {
    if ( theFile.exists ( ) ) {
      if ( theFile.open ( QIODevice::ReadOnly ) ) {
	//Q3TextStream stream ( &theFile );	//ooo
	QTextStream stream ( &theFile );	//xxx
	while  (  !  stream.atEnd ( ) )
	  //qsText +=  stream.read  ( );	//ooo
	  qsText +=  stream.readAll  ( );	//xxx
	theFile.close ( );
      }
      t = 3;
      bFound = true;
    }
    else
      qsFileName = QString (  Global::qsSystemPath + "/share/qdvdauthor/html/en/%1" ).arg( qsFile );
  }
  if ( ! bFound ) 
    qsText = QObject::tr ( "Error, could not find file\n%1/share/qdvdauthor/html/%2/%3\nPlease make sure you have " ).arg( Global::qsSystemPath ).arg( Global::qsLanguage ).arg ( qsFileName );

  return MessageBox::html ( pParent, qsCaption, qsText, initialSize, iButton1, iButton2 );
}
