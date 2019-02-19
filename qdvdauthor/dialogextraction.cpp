/***************************************************************************
    dialogextraction.cpp
                             -------------------
    DialogExtraction class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QtGui>

#include "dialogextraction.h"

DialogExtraction::DialogExtraction(QWidget *pParent)
	: QDialog(pParent)
{
	setupUi(this);
	init();
}
/*
 *  Destroys the object and frees any allocated resources
 */
DialogExtraction::~DialogExtraction()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DialogExtraction::languageChange()
{
    retranslateUi(this);
}

void DialogExtraction::init ()
{
    m_iSeconds = 0;
}

void DialogExtraction::setMenuName( QString qsMenuName)
{
    QString qsMenuTitle = QString ("<p align=\"center\"><b><h2>Handling Menu :<br>%1</h2></b></p>").arg (qsMenuName);
    m_pTextMenu->setText(qsMenuTitle);
}

void DialogExtraction::setObjectName( QString qsObjectName)
{
    QString qsObjectTitle = QString ("<p align=\"center\"><b><h2>Menu Object :<br>%1</h2></b></p>").arg(qsObjectName);
    m_pTextObject->setText (qsObjectTitle);
}

void DialogExtraction::setTime( QString qsTime)
{
    QString qsTimeTitle = QString ("<p align=\"center\"><b><h2>%1</h2></b></p>").arg(qsTime);
    m_pTextTime->setText (qsTimeTitle);
}

void DialogExtraction::slotAddASecond()
{
    QTime theTime;
    theTime = theTime.addSecs (m_iSeconds ++);
    setTime (theTime.toString());  
}

