/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
 
#include <qlayout.h>
//#include <q3filedialog.h> //ooo
#include <QFileDialog>      //xxx
//#include <q3scrollview.h> //ooo
#include <QScrollArea>      //xxx
#include <qdesktopwidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QMessageBox>		//xxx

#include "imagepreview.h"
//#include "xml_slideshow.h"		//ooo
#include "../xml_slideshow.h"		//xxx

void DialogImage::initMe(void *pImg, QString &backgroundFileName)
{
	CXmlSlideshow::img_struct *pImage = (CXmlSlideshow::img_struct *)pImg;
	m_pSliderRotate->setRange      (-3600, 3600 );
	m_pSliderShearX->setRange      (-1000, 1000 );
	m_pSliderShearY->setRange      (-1000, 1000 );
	m_pSliderZoom->setRange        (-1000, 1000 );
	m_pSliderRed->setRange         (    0, 1000 );
	m_pSliderGreen->setRange       (    0, 1000 );
	m_pSliderBlue->setRange        (    0, 1000 );
	m_pSliderTransparency->setRange(    0, 1000 );
	m_pSliderBrightness->setRange  (-1000, 1000 );

	m_pSliderRotate->setValue       ( 0 );
	m_pSliderShearX->setValue       ( 0 );
	m_pSliderShearY->setValue       ( 0 );
	m_pSliderZoom->setValue         ( 0 );
	m_pSliderRed->setValue          ( 0 );
	m_pSliderGreen->setValue        ( 0 );
	m_pSliderBlue->setValue         ( 0 );
	m_pSliderTransparency->setValue ( 0 );
	m_pSliderBrightness->setValue   ( 0 );

	m_pLabelStatus->setText (QString (""));
	//Q3GridLayout *pLayout = new Q3GridLayout (m_pFrameImage, 1, 1, 2, 2, "m_pFrameImageLayout"); //ooo
    QGridLayout *pLayout = new QGridLayout (m_pFrameImage); //xxx

	// Here we get the available screen resolution (all screens)
	// We want to keep the old style for resolutions above 1024x768 
	// But for lower res we need to use a scrollview to be able to
	// use QDVDAuthor.
	int iScreenWidth, iScreenHeight, iScreens, t;
	//Q3ScrollView *m_pScrollView = NULL;  //ooo
    QScrollArea *m_pScrollView = NULL;  //xxx
	QDesktopWidget desktop;
	iScreenWidth = iScreenHeight = 0;
	iScreens = desktop.numScreens();
	for (t=0;t<iScreens;t++)	{
		iScreenWidth  += desktop.screenGeometry(t).width();
		iScreenHeight += desktop.screenGeometry(t).height();
	}
	//uint iFlags = 0;		//ooo
//#if (QT_VERSION > 0x0301FF)		//ooo
//	iFlags = Qt::WNoAutoErase;
//#else
//	iFlags = Qt::WPaintClever;
//#endif
	
	Qt::WindowFlags iFlags = 0;	//xxx
	
	if ( (iScreenHeight <= 768) || (iScreenWidth <= 1024) )	{
		// Here we embed the MenuPreview into a ScrollView object ...
		//m_pScrollView = new Q3ScrollView( m_pFrameImage, "m_pScrollView", iFlags);  //ooo
        m_pScrollView = new QScrollArea( m_pFrameImage);    //xxx
		m_pImagePreview = new ImagePreview( m_pScrollView, "m_pPreview", iFlags);
		//m_pScrollView->addChild (m_pImagePreview);  //ooo
        m_pScrollView->setWidget (m_pImagePreview);   //xxx
		m_pImagePreview->resize(720, 480);
		pLayout->addWidget( m_pScrollView, 0, 0 );
	}
	else	{
		m_pImagePreview = new ImagePreview( m_pFrameImage, "m_pPreview", iFlags);
		pLayout->addWidget( m_pImagePreview, 0, 0 );
	}
	show ();

	// Here we init the vars ...
	bool bBackgroundToggleOn = false;
	m_pImagePreview->setBackground (backgroundFileName);
	m_pImagePreview->setImage( pImage );
//	m_pImagePreview->setBackground (backgroundFileName, true);
	m_pLabelStatus ->setText ( tr     ("Zoom"));
	m_pEditStatus  ->setText ( QString ("1.0"));

	if (!backgroundFileName.isEmpty())
		bBackgroundToggleOn = true;

	if (m_pImagePreview->getImage())	{
		ImageManipulator *pManipulator = (ImageManipulator *)m_pImagePreview->getImage()->pModifier;
		if (pManipulator)	{
			m_pSliderRotate->setValue	( (int)(m_pImagePreview->getImage()->rotate * 10) );
			m_pSliderShearX->setValue	( (int)(pManipulator->fShearX * 1000.0) );
			m_pSliderShearY->setValue	( (int)(pManipulator->fShearY * 1000.0) );
			m_pSliderZoom->setValue		( (int)(pManipulator->fZoom * 1000.0) - 1000);
			m_pSliderRed->setValue		( (int)(1000 - pManipulator->fRed * 1000.0) );
			m_pSliderGreen->setValue	( (int)(1000 - pManipulator->fGreen * 1000.0) );
			m_pSliderBlue->setValue		( (int)(1000 - pManipulator->fBlue * 1000.0) );
			m_pSliderTransparency->setValue	( (int)(pManipulator->fTransparency * 1000.0) );
			m_pSliderBrightness->setValue	( (int)( - pManipulator->fBrightness * 1000.0) );
//			bBackgroundToggleOn = pManipulator->bShowBackground;
			// Get the one stored in the structure rather then the global backgroundFileName
//			if ( ! pManipulator->backgroundFileName.isNull() );
//				m_pImagePreview->setBackground (pManipulator->backgroundFileName);
		}
	}
	if (bBackgroundToggleOn)	{
		m_pButtonBackground->setEnabled(true);
		m_pCheckBackground->setChecked (true);
	}
	else	{
		m_pButtonBackground->setEnabled(false);
		m_pCheckBackground->setChecked (false);
	}
}

void DialogImage::slotShearXValueChanged(int iValue)
{
	float fShearX = (float)iValue / 1000.0;
	m_pImagePreview->setShearX (fShearX);
	m_pLabelStatus->setText(tr ("ShearX"));
	m_pEditStatus->setText (QString ("%1").arg(fShearX));
}

void DialogImage::slotShearYValueChanged(int iValue)
{
	float fShearY = (float)iValue / 1000.0;
	m_pImagePreview->setShearY (fShearY);
	m_pLabelStatus->setText(tr ("ShearY"));
	m_pEditStatus->setText (QString ("%1").arg(fShearY));
}

void DialogImage::slotRotateValueChanged(int iValue)
{
	float fRotation = (float) iValue / 10.0;
	m_pImagePreview->setRotation (fRotation);
	m_pLabelStatus->setText(tr ("Rotate"));
	m_pEditStatus->setText (QString ("%1").arg(fRotation));
}
void DialogImage::slotTransparencyValueChanged(int iValue)
{
	float fTransparency = (float) iValue / 1000.0;
	m_pImagePreview->setTransparency (fTransparency);
	m_pLabelStatus->setText(tr ("Tarnsparency"));
	m_pEditStatus->setText (QString ("%1").arg(fTransparency));
}
void DialogImage::slotZoomValueChanged(int iValue)
{
	float fZoom = (float)(1000.0+iValue) / 1000.0;
	m_pImagePreview->setZoom (fZoom);
	m_pLabelStatus->setText(tr ("Zoom"));
	m_pEditStatus->setText (QString ("%1").arg(fZoom));
}
void DialogImage::slotBrightnessValueChanged(int iValue)
{
//	float fBrightness = (float)(1000.0+iValue) / 1000.0 - 1.0;
	float fBrightness = (float)iValue / 1000.0;
	m_pImagePreview->setBrightness (-fBrightness);
	m_pLabelStatus->setText(tr ("Brightness"));
	m_pEditStatus->setText (QString ("%1").arg(fBrightness));
}
void DialogImage::slotRedValueChanged(int iValue)
{
	float fRed = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setRed (fRed);
	m_pLabelStatus->setText(tr ("Red"));
	m_pEditStatus->setText (QString ("%1").arg(fRed));
}
void DialogImage::slotGreenValueChanged(int iValue)
{
	float fGreen = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setGreen (fGreen);
	m_pLabelStatus->setText(tr ("Green"));
	m_pEditStatus->setText (QString ("%1").arg(fGreen));
}
void DialogImage::slotBlueValueChanged(int iValue)
{
	float fBlue = 1.0 - (float)iValue / 1000.0;
	m_pImagePreview->setBlue (fBlue);
	m_pLabelStatus->setText(tr ("Blue"));
	m_pEditStatus->setText (QString ("%1").arg(fBlue));
}

void DialogImage::slotSliderReleased()
{
	m_pImagePreview->setFastPreview(false);
}

void DialogImage::slotSliderPressed()
{
	m_pImagePreview->setFastPreview(true);
}

void DialogImage::slotValueChanged( const QString &newText )
{
	// The user typed in a new value in the status. Thus we need to check which value was changed and do the do ...
	float fNewValue = newText.toFloat();
	if (m_pLabelStatus->text() == tr("ShearX"))
		m_pSliderShearX->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("ShearY"))
		m_pSliderShearY->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Rotate"))
		m_pSliderRotate->setValue((int)(fNewValue * 10.0));
	else if (m_pLabelStatus->text() == tr("Zoom"))
		m_pSliderZoom->setValue((int)(fNewValue * 1000.0)-1000);
	else if (m_pLabelStatus->text() == tr("Transparency"))
		m_pSliderTransparency->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Brightness"))
		m_pSliderBrightness->setValue((int)(fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Red"))
		m_pSliderRed->setValue((int)(1000 - fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Green"))
		m_pSliderGreen->setValue((int)(1000 - fNewValue * 1000.0));
	else if (m_pLabelStatus->text() == tr("Blue"))
		m_pSliderBlue->setValue((int)(1000 - fNewValue * 1000.0)); 
}

void DialogImage::slotButtonBackground ()
{
	QString qsFilter ("*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP");
	//QString qsBackground = Q3FileDialog::getOpenFileName("./", tr("Image Files (")+qsFilter+QString(")"), this,  //ooo
	//tr("Select background image"), tr("Select the background image."));  //ooo
    QStringList qsBackground = QFileDialog::getOpenFileNames ( NULL, QObject::tr("Select the background image."), "./", QObject::tr("Image Files (")+qsFilter+QString(")") );  //xxx
	//if (qsBackground.isNull())   //ooo
    if (qsBackground[0].isNull())   //xxx
		return;
	//m_pImagePreview->setBackground (qsBackground);   //ooo
    m_pImagePreview->setBackground (qsBackground[0]);  //xxx
}

void DialogImage::slotBackgroundToggled( bool bToggled )
{
	m_pButtonBackground->setEnabled(bToggled);
	m_pImagePreview->toggleBackground(bToggled);
}

void DialogImage::slotComboActivated(int iNewMode)
{
	m_pImagePreview->setTransformationMode(iNewMode);
}

void DialogImage::slotOk()
{
	// This'll set the ImageModifier - structure and tack it onto m_pImage.
	m_pImagePreview->getModifier();
	// This will call either ImagePreview::slotImageDialogDone, or ImageLine::slotImageDialogDone
	// Note : need to call setModifier within this object then for the changes to take effect.
	emit (signalDone (this));
	accept ();
}

ImagePreview *DialogImage::getPreview()
{
	return m_pImagePreview;
}
