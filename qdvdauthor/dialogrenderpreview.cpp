/***************************************************************************
    dialogrenderpreview.cpp
                             -------------------
    DialogRenderPreview class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
****************************************************************************/

#include <QtGui>
#include <QMessageBox>

#include "dialogrenderpreview.h"
#include "global.h"

DialogRenderPreview::DialogRenderPreview(QWidget *pParent)
	: QDialog(pParent)
{
	setupUi(this);
}

void DialogRenderPreview::setProgressData (QPixmap *pPixmap, long iCurrentFrameNumber, long iTotalFramesToRender)
{
	bool bPNG = Global::bPNGRender;
	QString qsFileName;
	//int iProgress = (int)((float)iCurrentFrameNumber /(float) iTotalFramesToRender * 1000.0);                                            //ooo
    int iProgress = static_cast<int> ( static_cast<float> ( iCurrentFrameNumber ) / static_cast<float> ( iTotalFramesToRender ) * 100.0);  //xxx
    
	//m_pProgress->setProgress (iProgress);		//ooo
	m_pProgress->setValue (iProgress);		//xxx
	//m_pLabelPreview->setPaletteBackgroundPixmap (*pPixmap);
    //QPalette palette;								       //xxx
    //palette.setBrush(QPalette::Window, QBrush(*pPixmap));	//xxx
    //m_pLabelPreview->setPalette(palette);					//xxx
	//Utils::setPaletteBackgroundPixmap ( m_pLabelPreview, *pPixmap );	//ooo
//	m_pLabelPreview->setPixmap (*pPixmap);
    m_pLabelPreview->setPixmap (*pPixmap);  //xxx
	qsFileName.sprintf ("<p align=\"center\">rendered_%08ld.%s</p>", iCurrentFrameNumber, bPNG ? "png" : "jpg" );
	m_pLabelFileName->setText (qsFileName);
}

