/****************************************************************************
** DialogMPlayerSetup class
**
** Created: Fri Dec 14 22:48:00 2004
**      by: Varol Okan using Kate
**
****************************************************************************/
#ifdef QDVD_LINUX 
#include <unistd.h>
#endif 

#include <QDialog>		//xxx
//#include <qregexp.h>		//ooo
#include <QRegExp>		//xxx
//#include <q3process.h>	//ooo
#include <QProcess>		//xxx
//#include <qcombobox.h>	//ooo
#include <QComboBox>		//xxx
//#include <qlineedit.h>	//ooo
#include <QLineEdit>		//xxx

//#include "../../win32.h"	//ooo
#include "qmplayerwidget.h"
#include "dialogmplayersetup.h"

//DialogMPlayerSetup::DialogMPlayerSetup(QWidget * parent, const char * name, Qt::WFlags f)	//ooo
DialogMPlayerSetup::DialogMPlayerSetup(QWidget *pParent)					//xxx
	//: uiDialogMPlayerSetup (parent, name, f)		//ooo
	//: QDialog(pParent)					//xxx
	//: QWidget(pParent)					//xxx
	: QDialog(pParent)					//xxx
{
        //setupUi(this);	//xxx
	initMe();		//xxx
  
	m_pVideoProcess  = NULL;
	m_pAudioProcess  = NULL;
	m_pListPluginOptions = NULL;
}

DialogMPlayerSetup::~DialogMPlayerSetup ()
{

}

void DialogMPlayerSetup::killProcesses ()
{
	if (m_pVideoProcess)	{
		m_pVideoProcess->kill();
		delete m_pVideoProcess;
	}
	if (m_pAudioProcess)	{
		m_pAudioProcess->kill();
		delete m_pAudioProcess;
	}
	m_pVideoProcess = NULL;
	m_pAudioProcess = NULL;
}

void DialogMPlayerSetup::initMe ( )
{
	QWidget *uiDialogMPlayerSetup = new QWidget (this);
	uiDialogMPlayerSetup->resize(563, 266);
        //uiDialogMPlayerSetup->setSizeGripEnabled(true);
        vboxLayout = new QVBoxLayout(uiDialogMPlayerSetup);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        textLabel1_4 = new QLabel(uiDialogMPlayerSetup);
        textLabel1_4->setObjectName(QString::fromUtf8("textLabel1_4"));
        textLabel1_4->setWordWrap(false);

        vboxLayout->addWidget(textLabel1_4);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textLabel1 = new QLabel(uiDialogMPlayerSetup);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(4), static_cast<QSizePolicy::Policy>(5));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textLabel1->sizePolicy().hasHeightForWidth());
        textLabel1->setSizePolicy(sizePolicy);
        textLabel1->setMinimumSize(QSize(100, 0));
        textLabel1->setAlignment(Qt::AlignVCenter);
        textLabel1->setWordWrap(false);

        gridLayout->addWidget(textLabel1, 1, 0, 1, 1);

        textLabel1_3 = new QLabel(uiDialogMPlayerSetup);
        textLabel1_3->setObjectName(QString::fromUtf8("textLabel1_3"));
        textLabel1_3->setWordWrap(false);

        gridLayout->addWidget(textLabel1_3, 0, 1, 1, 1);

        m_pEditAudioOptions = new QLineEdit(uiDialogMPlayerSetup);
        m_pEditAudioOptions->setObjectName(QString::fromUtf8("m_pEditAudioOptions"));

        gridLayout->addWidget(m_pEditAudioOptions, 2, 2, 1, 1);

        textLabel1_3_2 = new QLabel(uiDialogMPlayerSetup);
        textLabel1_3_2->setObjectName(QString::fromUtf8("textLabel1_3_2"));
        textLabel1_3_2->setWordWrap(false);

        gridLayout->addWidget(textLabel1_3_2, 0, 2, 1, 1);

        textLabel1_2 = new QLabel(uiDialogMPlayerSetup);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        sizePolicy.setHeightForWidth(textLabel1_2->sizePolicy().hasHeightForWidth());
        textLabel1_2->setSizePolicy(sizePolicy);
        textLabel1_2->setMinimumSize(QSize(100, 0));
        textLabel1_2->setAlignment(Qt::AlignVCenter);
        textLabel1_2->setWordWrap(false);

        gridLayout->addWidget(textLabel1_2, 2, 0, 1, 1);

        m_pComboVideo = new QComboBox(uiDialogMPlayerSetup);
        m_pComboVideo->setObjectName(QString::fromUtf8("m_pComboVideo"));

        gridLayout->addWidget(m_pComboVideo, 1, 1, 1, 1);

        m_pComboAudio = new QComboBox(uiDialogMPlayerSetup);
        m_pComboAudio->setObjectName(QString::fromUtf8("m_pComboAudio"));

        gridLayout->addWidget(m_pComboAudio, 2, 1, 1, 1);

        m_pEditVideoOptions = new QLineEdit(uiDialogMPlayerSetup);
        m_pEditVideoOptions->setObjectName(QString::fromUtf8("m_pEditVideoOptions"));

        gridLayout->addWidget(m_pEditVideoOptions, 1, 2, 1, 1);


        vboxLayout->addLayout(gridLayout);

        spacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacer2);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        buttonHelp = new QPushButton(uiDialogMPlayerSetup);
        buttonHelp->setObjectName(QString::fromUtf8("buttonHelp"));
        buttonHelp->setAutoDefault(true);

        hboxLayout->addWidget(buttonHelp);

        Horizontal_Spacing2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(Horizontal_Spacing2);

        buttonOk = new QPushButton(uiDialogMPlayerSetup);
        buttonOk->setObjectName(QString::fromUtf8("buttonOk"));
        buttonOk->setAutoDefault(true);
        buttonOk->setDefault(true);

        hboxLayout->addWidget(buttonOk);

        buttonCancel = new QPushButton(uiDialogMPlayerSetup);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
        buttonCancel->setAutoDefault(true);

        hboxLayout->addWidget(buttonCancel);


        vboxLayout->addLayout(hboxLayout);

}

void DialogMPlayerSetup::initMe (QStringList *pListPluginOptions)
{
	QStringList listCommand;
	killProcesses ();

	m_pListPluginOptions = pListPluginOptions;
	// Here we set the current Plugin Options
	m_pEditVideoOptions->setText ((*pListPluginOptions)[QMPlayerWidget::VideoPluginOptions]);
	m_pEditAudioOptions->setText ((*pListPluginOptions)[QMPlayerWidget::AudioPluginOptions]);
	// And also update the GUI (Note: the driver is set when we have the values in)
	
	// Video plugin request ...
	//m_pVideoProcess = new Q3Process;	//ooo
	m_pVideoProcess = new QProcess;		//xxx
	// Request VidoePlugin information
	listCommand.append(QString("mplayer"));
	listCommand.append(QString("-vo"));
	listCommand.append(QString("help"));
	connect (m_pVideoProcess, SIGNAL(readyReadStdout()), this, SLOT (slotProcessVideoPlugins ()));
	m_pVideoProcess->setArguments (listCommand);

	// Audio plugin request ...
	//m_pAudioProcess = new Q3Process;	//ooo
	m_pAudioProcess = new QProcess;		//xxx
	listCommand.clear ();
	// Request VidoePlugin information
	listCommand.append(QString("mplayer"));
	listCommand.append(QString("-ao"));
	listCommand.append(QString("help"));
	connect (m_pAudioProcess, SIGNAL(readyReadStdout()), this, SLOT (slotProcessAudioPlugins ()));
	m_pAudioProcess->setArguments (listCommand);

	m_pVideoProcess->start();
	m_pAudioProcess->start();
}

void DialogMPlayerSetup::slotProcessVideoPlugins ()
{
//printf ("DialogMPlayerSetup::slotProcessVideoPlugins \n");
	uint t;
	QStringList listInfos;
	QString qsCurrent;
	//QString qsStdout(m_pVideoProcess->readStdout());		//ooo
	QString qsStdout(m_pVideoProcess->readAllStandardOutput());	//xxx
	// The length will tell us if we have the requested information or not.
	if (qsStdout.length() < 100)
		return;
//printf ("%s\n\n", (const char *)qsStdout);
	// Note withot this sleep the driver do not always show up ...
    //usleep ( 10000 ); // 10 ms
    SleeperThread1::msleep ( 10 ); // 10 ms	//xxx
	qsCurrent = (*m_pListPluginOptions)[QMPlayerWidget::VideoPlugin];
	// First we generate a list with one entry per line.
	//listInfos = QStringList::split ("\n", qsStdout);	//ooo
	listInfos = qsStdout.split ("\n");			//xxx

	QRegExp	regExp ("^\\s+(\\w+)\\s+.*");
	for (t=0;t<listInfos.count();t++)	{
		//if (regExp.search(listInfos[t]) > -1)	{	//ooo
		if (regExp.indexIn(listInfos[t]) > -1)	{	//xxx
			//m_pComboVideo->insertItem (regExp.cap(1));	//ooo
			m_pComboVideo->addItem (regExp.cap(1));		//xxx
//printf ("<%d><%s> -=> <%s>\n", t, (const char *)listInfos[t], (const char *)regExp.cap(1));
			if ( (!qsCurrent.isNull()) && (qsCurrent == regExp.cap(1)) )
				m_pComboVideo->setCurrentText (qsCurrent);
		}
	}
}

void DialogMPlayerSetup::slotProcessAudioPlugins ()
{
	uint t;
	QStringList listInfos;
	QString qsCurrent;
	//QString qsStdout(m_pAudioProcess->readStdout());		//ooo
	QString qsStdout(m_pAudioProcess->readAllStandardOutput());	//xxx
	// The length will tell us if we have the requested information or not.
	if (qsStdout.length() < 100)
		return;
	// Note without this sleep the driver do not always show up ...
    //usleep ( 10000 ); // 10ms
    SleeperThread1::msleep ( 10 ); // 10 ms	//xxx
	qsCurrent = (*m_pListPluginOptions)[QMPlayerWidget::AudioPlugin];
	// First we generate a list with one entry per line.
	//listInfos = QStringList::split ("\n", qsStdout);	//ooo
	listInfos = qsStdout.split ("\n");			//xxx

//printf ( "Audio found %d plugins\n", (int)listInfos.count ( ) );

	QRegExp	regExp ("^\\s+(\\w+)\\s+.*");
	for (t=0;t<listInfos.count();t++)	{
//		printf ( "Audio [%d] = <%s>\n", t, listInfos[t].ascii() );
		//if (regExp.search(listInfos[t]) > -1)	{	//ooo
		if (regExp.indexIn(listInfos[t]) > -1)	{	//xxx
//printf ( "  Insert [%d]<%s>\n", t, regExp.cap ( 1 ).ascii ( ) );
			//m_pComboAudio->insertItem (regExp.cap(1));	//ooo
			m_pComboAudio->addItem (regExp.cap(1));		//xxx
			if ( (!qsCurrent.isNull()) && (qsCurrent == regExp.cap(1)) )
				m_pComboAudio->setCurrentText (qsCurrent);
		}
	}
}

