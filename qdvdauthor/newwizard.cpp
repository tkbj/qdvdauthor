/***************************************************************************
    newwizard.cpp
                             -------------------
    NewWizard class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

#include "newwizard.h"

NewWizard::NewWizard(QWidget *pParent)
	: QWizard(pParent)
{
  
   //TempPath *tempPath = new TempPath;
   tempPath = new TempPath;
   dvdOutputPath = new DVDOutputPath;
   //addPage(new TempPath);
   addPage(tempPath);
   //addPage(new DVDOutputPath);
   addPage(dvdOutputPath);
   
   setWindowTitle("New Project Wizard");
   //setMinimumSize(630, 150);
   
   //QString projectName = field("projectName").toString();
   //projectName = field("projectName").toString();
   //QString dvdPath = field("dvdPath").toString();
   //dvdPath = field("dvdPath").toString();
   
   //m_pEditProjectName->text() = projectName;
   //m_pEditDVDPath->text() = dvdPath;
   //setField("tmpPath", field("/tmp").toString());
   //setField("tmpPath", m_pEditTempPath->text());
   setField("tmpPath", tempPath->m_pEditTempPath->text());
}  

void NewWizard::accept()
{
  
   //QString projectName = field("projectName").toString();
   projectName = field("projectName").toString();
   //QString dvdPath = field("dvdPath").toString();
   dvdPath = field("dvdPath").toString();
   tmpPath = field("tmpPath").toString();

   QDialog::accept();
}

TempPath::TempPath(QWidget *pParent)
	: QWizardPage(pParent)
{
   setMinimumSize(630, 150);
   //setTitle(tr("New Project Wizard"));
   setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/temp_path_.jpg"));

   /*QGridLayout *layout = new QGridLayout(this);

    //layout->addWidget(new QLabel(tr("Object name:")), 0, 0);
   m_objectName = new QLineEdit;
    //layout->addWidget(m_objectName, 0, 1);
   
    //QLabel *topLevelLabel;
    //QPixmap pixmap(":/images/temp_path_.jpg");
    //topLevelLabel->setPixmap(pixmap);
   QImage img(":/images/temp_path_.jpg");
   QLabel* label = new QLabel(this);
   label->setPixmap(QPixmap::fromImage(img));
   
    //layout->addWidget(topLevelLabel, 1, 0);
    //layout->addWidget(new QLabel(pixmap), 1, 0);
   layout->addWidget(label, 0, 0);
    //label->setAlignment( Qt::AlignCenter );
    //label->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );*/

   QVBoxLayout *tempPathLayout = new QVBoxLayout;
   //layout->addLayout(tempPathLayout, 0, 1, 1, 1);
    //tempPathLayout->addWidget(new QLabel(tr("Please define the temp drive.")));
   tempPathLayout->addWidget(new QLabel(tr("<h2><b><p align=\"center\">Please define the temp drive</p></b></h2>")));
   setLayout(tempPathLayout);
   
   m_pEditTempPath = new QLineEdit;
   //m_pEditTempPath->setText("/tmp");
   registerField("tmpPath*", m_pEditTempPath);
   //setField("tmpPath", m_pEditTempPath->text());
   m_pEditTempPath->setText(field("tmpPath").toString());
   
   QHBoxLayout *buttonLayout0 = new QHBoxLayout;
   m_pButtonBrowseTemp = new QPushButton(tr("Browse ..."));
   buttonLayout0->addWidget(m_pEditTempPath);
   buttonLayout0->addWidget(m_pButtonBrowseTemp);
   tempPathLayout->addLayout(buttonLayout0);
   
   tempPathLayout->addWidget(new QLabel(tr("Note: The temp drive will store huge amounts of temporary data.")));
   tempPathLayout->addStretch();
   
   connect(m_pButtonBrowseTemp, SIGNAL(clicked()), this, SLOT(slotBrowseTemp()));
   
   /*QHBoxLayout *buttonLayout = new QHBoxLayout;
   layout->addLayout(buttonLayout, 1, 0, 1, 2);
   //buttonLayout->addStretch();
   
   QPushButton *helpButton = new QPushButton(tr("Help"));
   connect(helpButton, SIGNAL(clicked()), this, SLOT(accept()));
   buttonLayout->addWidget(helpButton);
   buttonLayout->addStretch();
   QPushButton *backButton = new QPushButton(tr("< Back"));
   connect(backButton, SIGNAL(clicked()), this, SLOT(accept()));
   buttonLayout->addWidget(backButton);
   QPushButton *nextButton = new QPushButton(tr("Next >"));
   connect(nextButton, SIGNAL(clicked()), this, SLOT(projectName()));
   buttonLayout->addWidget(nextButton);
   QPushButton *cancelButton = new QPushButton(tr("Cancel"));
   connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
   buttonLayout->addWidget(cancelButton);*/

}

/*void NewWizard::projectName ( )
{
  NewWizard2 *pWizard2 = new NewWizard2 (this);
  pWizard2->exec();
}*/

DVDOutputPath::DVDOutputPath(QWidget *pParent)
  : QWizardPage(pParent)
{
  
   setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/project_name_.jpg"));
  
   setWindowTitle("New Project Wizard");
   setMinimumSize(600, 150);
	
   //QGridLayout *layout = new QGridLayout(this);

    //layout->addWidget(new QLabel(tr("Please give the project name")), 0, 0);
   m_pEditProjectName = new QLineEdit;
   //m_pEditProjectName->setText("Unnamed");
    //layout->addWidget(m_objectName2, 0, 1);
   
    //QLabel *topLevelLabel;
    //QPixmap pixmap(":/images/temp_path_.jpg");
    //topLevelLabel->setPixmap(pixmap);
   /*QImage img(":/images/project_name_.jpg");
   QLabel* label = new QLabel(this);
   label->setPixmap(QPixmap::fromImage(img));
   
    //layout->addWidget(topLevelLabel, 1, 0);
    //layout->addWidget(new QLabel(pixmap), 1, 0);
   layout->addWidget(label, 0, 0);
    //label->setAlignment( Qt::AlignCenter );
    //label->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
   
    //layout->addWidget(new QLabel(tr("Please specify the DVD path name")), 1, 1);*/
   m_pEditDVDPath = new QLineEdit;
    //layout->addWidget(m_objectName3, 1, 1);
   
   registerField("projectName*", m_pEditProjectName);
   registerField("dvdPath*", m_pEditDVDPath);
   
   QVBoxLayout *projectNameLayout = new QVBoxLayout(this);
   //layout->addLayout(projectNameLayout, 0, 1, 1, 1);
    //projectNameLayout->addWidget(new QLabel(tr("Please give the project name")));
   QLabel *label2 = new QLabel(this);
    //label2->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    //label2->setFrameStyle(QFrame::NoFrame);
    //label2->setFrameStyle(QFrame::Panel);
    //label2->setLineWidth(3);
    //label2->setTextFormat(Qt::RichText);
    //QFont f( "Arial", 16, QFont::Bold);
    //QFont f( QFont::AnyStyle, 16, QFont::Bold);
    //label2->setFont( f);
    //label2->setText("Please give the project name");
   label2->setText("<h2><b><p align=\"center\">Please give the project name</p></b></h2>");
   label2->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
   projectNameLayout->addWidget(label2);
   projectNameLayout->addWidget(m_pEditProjectName);
   projectNameLayout->addWidget(new QLabel(tr("Note: The Project Name will also be used to create the directory under the temp drive.")));
   projectNameLayout->addStretch();
    //projectNameLayout->addWidget(new QLabel(tr("Please specify the DVD path name")));
   projectNameLayout->addWidget(new QLabel(tr("<h2><b><p align=\"center\">Please specify the DVD path name</p></b></h2>")));
   
   QHBoxLayout *buttonLayout0 = new QHBoxLayout;
   m_pButtonBrowseDVDPath = new QPushButton(tr("Browse ..."));
   buttonLayout0->addWidget(m_pEditDVDPath);
   buttonLayout0->addWidget(m_pButtonBrowseDVDPath);
   projectNameLayout->addLayout(buttonLayout0);
   
   
   projectNameLayout->addWidget(new QLabel(tr("Note: The DVD will be created in this directory. You can then use any program to burn it on the actual DVD media.")));
   
   /*QHBoxLayout *buttonLayout = new QHBoxLayout;
   layout->addLayout(buttonLayout, 1, 0, 1, 2);
   //buttonLayout->addStretch();
   
   QPushButton *helpButton = new QPushButton(tr("Help"));
   connect(helpButton, SIGNAL(clicked()), this, SLOT(accept()));
   buttonLayout->addWidget(helpButton);
   buttonLayout->addStretch();
   QPushButton *backButton = new QPushButton(tr("< Back"));
   connect(backButton, SIGNAL(clicked()), this, SLOT(accept()));
   buttonLayout->addWidget(backButton);
   QPushButton *nextButton = new QPushButton(tr("Finish"));
   connect(nextButton, SIGNAL(clicked()), this, SLOT(accept()));
   buttonLayout->addWidget(nextButton);
   QPushButton *cancelButton = new QPushButton(tr("Cancel"));
   connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
   buttonLayout->addWidget(cancelButton);*/
   
   connect(m_pButtonBrowseDVDPath, SIGNAL(clicked()), this, SLOT(slotBrowseDVDPath()));
   connect(m_pEditDVDPath, SIGNAL(textChanged(QString)), this, SLOT(slotSelectionCHanged(QString)));
  
}

void TempPath::slotBrowseTemp()
{
	// This function will browse for the directory where the DVD is to be build.
	//QString qsPathName = Q3FileDialog::getExistingDirectory ( m_pEditTempPath->text() );								//ooo
	//QString qsPathName = QFileDialog::getExistingDirectory ( m_pEditTempPath->text() );								//xxx
	QString qsPathName = QFileDialog::getExistingDirectory (this, tr("Find Directory"), m_pEditTempPath->text(), QFileDialog::ShowDirsOnly );	//xxx
	if  ( (qsPathName.isEmpty()) || (qsPathName.isNull()) )
		return;
	m_pEditTempPath->setText(qsPathName);
}

void DVDOutputPath::slotBrowseDVDPath()
{
	// This function will browse for the directory where the DVD is to be build.
	//QString qsPathName = Q3FileDialog::getExistingDirectory (Global::qsCurrentPath);								//ooo
	//QString qsPathName = QFileDialog::getExistingDirectory (Global::qsCurrentPath);								//xxx
//	QString qsPathName = QFileDialog::getExistingDirectory (m_pEditTempPath->text());
	QString qsPathName = QFileDialog::getExistingDirectory (this, tr("Find Directory of DVD"), "/home"/*Global::qsCurrentPath*/, QFileDialog::ShowDirsOnly );		//xxx
	if  ( (qsPathName.isEmpty()) || (qsPathName.isNull()) )
		return;
	m_pEditDVDPath->setText(qsPathName);
}

void DVDOutputPath::slotSelectionCHanged( const QString &qsText)
{
    /*if (qsText.isEmpty())
	setFinishEnabled (DVDOutputPath, false);
    else
	setFinishEnabled (DVDOutputPath, true);*/
}


