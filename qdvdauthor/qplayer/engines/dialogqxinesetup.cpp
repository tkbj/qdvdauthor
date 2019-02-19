/****************************************************************************
** vim: ts=4 sw=4 noet
**
** DialogQXineSetup class
**
**   Created : Tue Sep 21 14:46:17 2010
**        by : Zsolt Branyiczky
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
****************************************************************************/

#include <QtGui>

#include "dialogqxinesetup.h"

DialogQXineSetup::DialogQXineSetup(QWidget *pParent)
	: QDialog(pParent)
{
  //setupUi(this);	//ooo
  initMe();
}

void DialogQXineSetup::initMe()
{
  QDialog *DialogQXineSetup = new QDialog(this);
  
  DialogQXineSetup->resize(480, 186);
  DialogQXineSetup->setSizeGripEnabled(true);
  vboxLayout = new QVBoxLayout(DialogQXineSetup);
  vboxLayout->setSpacing(6);
  vboxLayout->setContentsMargins(11, 11, 11, 11);
  vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
  gridLayout = new QGridLayout();
  gridLayout->setSpacing(6);
  gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
  m_pComboAudio = new QComboBox(DialogQXineSetup);
  m_pComboAudio->setObjectName(QString::fromUtf8("m_pComboAudio"));

  gridLayout->addWidget(m_pComboAudio, 1, 1, 1, 1);

  textLabel1_2 = new QLabel(DialogQXineSetup);
  textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
  QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(4), static_cast<QSizePolicy::Policy>(5));
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(textLabel1_2->sizePolicy().hasHeightForWidth());
  textLabel1_2->setSizePolicy(sizePolicy);
  textLabel1_2->setMinimumSize(QSize(100, 0));
  textLabel1_2->setAlignment(Qt::AlignVCenter);
  textLabel1_2->setWordWrap(false);

  gridLayout->addWidget(textLabel1_2, 1, 0, 1, 1);

  m_pComboVideo = new QComboBox(DialogQXineSetup);
  m_pComboVideo->setObjectName(QString::fromUtf8("m_pComboVideo"));

  gridLayout->addWidget(m_pComboVideo, 0, 1, 1, 1);

  textLabel1 = new QLabel(DialogQXineSetup);
  textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
  sizePolicy.setHeightForWidth(textLabel1->sizePolicy().hasHeightForWidth());
  textLabel1->setSizePolicy(sizePolicy);
  textLabel1->setMinimumSize(QSize(100, 0));
  textLabel1->setAlignment(Qt::AlignVCenter);
  textLabel1->setWordWrap(false);

  gridLayout->addWidget(textLabel1, 0, 0, 1, 1);


  vboxLayout->addLayout(gridLayout);

  spacer2 = new QSpacerItem(20, 61, QSizePolicy::Minimum, QSizePolicy::Expanding);

  vboxLayout->addItem(spacer2);

  hboxLayout = new QHBoxLayout();
  hboxLayout->setSpacing(6);
  hboxLayout->setContentsMargins(0, 0, 0, 0);
  hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
  buttonHelp = new QPushButton(DialogQXineSetup);
  buttonHelp->setObjectName(QString::fromUtf8("buttonHelp"));
  buttonHelp->setAutoDefault(true);

  hboxLayout->addWidget(buttonHelp);

  Horizontal_Spacing2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  hboxLayout->addItem(Horizontal_Spacing2);

  buttonOk = new QPushButton(DialogQXineSetup);
  buttonOk->setObjectName(QString::fromUtf8("buttonOk"));
  buttonOk->setAutoDefault(true);
  buttonOk->setDefault(true);

  hboxLayout->addWidget(buttonOk);

  buttonCancel = new QPushButton(DialogQXineSetup);
  buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
  buttonCancel->setAutoDefault(true);

  hboxLayout->addWidget(buttonCancel);


  vboxLayout->addLayout(hboxLayout);
}


void DialogQXineSetup::setVideoPlugins( QStringList &listVideoPlugins, QString &qsCurrentVideo)
{
  //m_pComboVideo->insertItem(QString("auto"));		//ooo
  m_pComboVideo->insertItem(0, QString("auto"));	//xxx
  //m_pComboVideo->insertStringList(listVideoPlugins);	//ooo
  m_pComboVideo->setCurrentText (qsCurrentVideo);
}


void DialogQXineSetup::setAudioPlugins( QStringList &listAudioPlugins, QString &qsCurrentAudio)
{
  //m_pComboAudio->insertItem(QString("auto"));		//ooo
  m_pComboAudio->insertItem(0, QString("auto"));	//xxx
  //m_pComboAudio->insertStringList(listAudioPlugins);	//ooo
  m_pComboAudio->setCurrentText (qsCurrentAudio);
}


QString DialogQXineSetup::getAudioPlugin()
{
    return m_pComboAudio->currentText();
}


QString DialogQXineSetup::getVideoPlugin()
{
    return m_pComboVideo->currentText();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogQXineSetup::languageChange()
{
    //retranslateUi(this);	//ooo
}

