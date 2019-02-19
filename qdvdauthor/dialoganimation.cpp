/***************************************************************************
    dialoganimation.cpp
                             -------------------
    DialogAnimation class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0
    
****************************************************************************/

#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>

#include "dvdmenu.h"
#include "dialoganimation.h"

//DialogAnimation::DialogAnimation (QWidget *pParent, const char *pName, Qt::WFlags f )		//ooo
DialogAnimation::DialogAnimation (QWidget *pParent )						//xxx
	//: uiDialogAnimation (pParent, pName, f)	//ooo
	: QDialog (pParent)				//xxx
{
  setupUi(this);	//xxx
}

DialogAnimation::~DialogAnimation ()
{
	// The Dialog takes care of the destruction of the generated tabs,
	// so we won't have to do it manually here
}

void DialogAnimation::initMe ( QString qsAnimation, long iFrames, float fFPS, int iType )
{
	char standardProperties[][14] = {"rect", "visibleFromTo", "transparency", "rotate", "zoom", "scale", "shear"}; // index 0-14
        char overlayProperties [][16] = {"offset", "scale", "shear", "rotate", "color", "fileName", "visibleFromTo"};
	char frameProperties   [][12] = {"color", "frameWidth", "frameStyle", "frameJoin"}; // index 15-26
	//char *imageProperties [] = {"fileName", "backgroundFileName", "showBackground", "kenBurns", "pos", "blendColor", "brightness" }; // index 15-24
	char imageProperties   [][16] = {"fileName", "kenBurns", "pos", "blendColor", "brightness" }; // index 15-24
	char movieProperties   [][10] = {"frameNo"}; // index 26
	char textProperties    [][20] = {"text", "font", "textAlign", "foregroundColor", "backgroundColor"}; // index 15-27
	// Note we have to synch those entries when we make the changes. I.e. 
	// standard properties.rect has 6 variables, index 0-5 (x1, y1, x2, y2, width, height)
	// standard properties.rotate has 1 variable, index 6 etc.

	uint t;
	QStringList listAttributes;

	// Every object has those attributes
	if ( iType == TypeOverlay )  {
		// OverlayObjects have ONLY those properties
		for ( t=0; t<sizeof (overlayProperties) / sizeof (overlayProperties[0]); t++ )
			listAttributes.append ( overlayProperties[t] );
	}
	else  { // All other objects start with these attributes.
		for (t=0;t<sizeof (standardProperties) / sizeof (standardProperties[0]);t++)
			listAttributes.append (standardProperties[t]);
	}

	switch (iType)	{
	case TypeFrame:
		for (t=0;t<sizeof (frameProperties) / sizeof (frameProperties[0]);t++)
			listAttributes.append (frameProperties[t]);
	break;
	case TypeImage:
		for (t=0;t<sizeof (imageProperties) / sizeof (imageProperties[0]);t++)
			listAttributes.append (imageProperties[t]);
	break;
	case TypeMovie:
		for (t=0;t<sizeof (imageProperties) / sizeof (imageProperties[0]);t++)
			listAttributes.append (imageProperties[t]);
		for (t=0;t<sizeof (movieProperties) / sizeof (movieProperties[0]);t++)
			listAttributes.append (movieProperties[t]);
	break;
	case TypeText:
		for (t=0;t<sizeof (textProperties) / sizeof (textProperties[0]);t++)
			listAttributes.append (textProperties[t]);
	break;
	case TypeOverlay:
	break;
	case TypeImageBackground:
	break;
	case TypeMovieBackground:
	break;
	}

	m_qsAnimation = qsAnimation;
	m_iMaxFrames  = iFrames;
	m_fFPS        = fFPS;

	if ( ! qsAnimation.isEmpty () )
		m_pEditScript->setText (qsAnimation);
	//m_pEditScript->moveCursor (Q3TextEdit::MoveEnd, false);		//ooo
	m_pEditScript->moveCursor (QTextCursor::End, QTextCursor::KeepAnchor);	//xxx
	m_pComboAttributes->clear ();
	//m_pComboAttributes->insertStringList ( listAttributes );	//ooo
	m_pComboAttributes->insertItems ( 0, listAttributes );		//xxx
	//m_pSliderPosition->setMaxValue  ( iFrames );			//ooo
	m_pSliderPosition->setMaximum  ( iFrames );			//xxx
	//m_pSliderPosition->setLineStep  ( (int)(iFrames/15.0) );	//ooo
	m_pSliderPosition->setTickInterval  ( (int)(iFrames/15.0) );	//xxx
	m_pSliderPosition->setPageStep  ( (int)(iFrames/5.0 ) );
	//m_pSpinBoxPosition->setMaxValue ( iFrames );			//ooo
	m_pSpinBoxPosition->setMaximum ( iFrames );			//xxx

	connect (m_pButtonAdd,       SIGNAL (clicked ()), this, SLOT(slotAddAttribute ()));
	connect (m_pButtonHelp,      SIGNAL (clicked ()), this, SLOT(slotHelp ()));
	connect (m_pSliderPosition,  SIGNAL (valueChanged(int)), this, SLOT (slotValueChanged(int)));
	connect (m_pSpinBoxPosition, SIGNAL (valueChanged(int)), this, SLOT (slotValueChanged (int)));
	connect (m_pComboAttributes, SIGNAL (activated (const QString &)), this, SLOT(slotAttribute (const QString &)));

	slotAttribute ( QString ("rect") );
}

QString &DialogAnimation::string ()
{
  //m_qsAnimation = m_pEditScript->text ();	//ooo
  m_qsAnimation = m_pEditScript->toPlainText ();	//xxx
  return m_qsAnimation;
}

void DialogAnimation::setFontName (QString qsFontName )
{
	m_qsFontName = qsFontName;
}

void DialogAnimation::slotHelp ()
{
  QMessageBox::information ( NULL, tr("Animation Dialog"), tr(
"This dialog will allow you to modify an objects properties over time (I.e. animate the object).\n"
" You will be offered the available properties for the object you selected.\n\n"
" The generated output is in the format:\n"
" FrameNumber::object-property=value\n\n"

" E.g. to move an object diagonal over the screen (NTSC) within 100 frames you could use this little script.\n"
" for ((t=0;t<100;t++))\n"
" {\n"
"   echo \"$t::rect.x1=$(( t * 720 / 100 ))\"\n"
"   echo \"$t::rect.y1=$(( t * 480 / 100 ))\"\n"
" }\n"), QMessageBox::Ok, QMessageBox::NoButton );
}

void DialogAnimation::slotValueChanged ( int iNewValue )
{
	static bool bChanging = false;
	if (bChanging)
		return;
	bChanging = true;
	m_pSliderPosition->setValue  ( iNewValue );
	m_pSpinBoxPosition->setValue ( iNewValue );
	bChanging = false;
}

void DialogAnimation::slotAddAttribute ()
{
  // This function adds the set Attribute at the curren cursor position in the text edit

  // The following attributes should show up as N::property.attribute=
  char arrayProperties[][20] = {"visibleFromTo", "color", "kenBurns", "startPos", "res", "blendColor", "foregroundColor", "backgroundColor", "rect", "scale", "shear", "pos", "offset" };

  uint t;
  bool bFound = false;
  QString qsNewText;
  qsNewText=QString ("%1::%2").arg (m_pSpinBoxPosition->text ()).arg (m_pComboAttributes->currentText());

  for (t=0; t < sizeof (arrayProperties)  / sizeof (arrayProperties[0] ); t++ )	{
    if ( m_pComboAttributes->currentText ( ) == arrayProperties[t] )	{
      bFound = true;
      break;
    }
  }

  if (bFound)	{
    if ( m_pComboAttributeSub->count ( ) > 0 ) {
      qsNewText += QString (".%1=").arg ( m_pComboAttributeSub->currentText ( ) );
      if ( m_pComboAttributes->currentText ( ) == "visibleFromTo" ) // slightly different handling/meaning for this prop.
        qsNewText += m_pSpinBoxPosition->text ( );
    }
    else
      qsNewText += QString ( "=" );
  }
  else 
    qsNewText += QString ( "=%1" ).arg ( m_pComboAttributeSub->currentText ( ) );

  //m_pEditScript->insert ( qsNewText );	//ooo
  m_pEditScript->insertPlainText ( qsNewText );	//xxx
}

void DialogAnimation::slotAttribute (const QString &newText)
{
	// Here we set the m_pComboAttributeSub according to the value of m_pComboAttributes
	m_pComboAttributeSub->clear ();
	if ( (newText == "color") || (newText == "blendColor") || (newText == "backgroundColor") || (newText == "foregroundColor") )	{
		m_pComboAttributeSub->addItem ( "red"   );
		m_pComboAttributeSub->addItem ( "green" );
		m_pComboAttributeSub->addItem ( "blue"  );
		m_pComboAttributeSub->addItem ( "alpha" );
	}
	else if ( newText == "kenBurns" )	{
		m_pComboAttributeSub->addItem ( "x1" );
		m_pComboAttributeSub->addItem ( "y1" );
		m_pComboAttributeSub->addItem ( "x2" );
		m_pComboAttributeSub->addItem ( "y2" );
	}
	else if ( newText == "rect" )	{
		m_pComboAttributeSub->addItem ( "x1" );
		m_pComboAttributeSub->addItem ( "y1" );
		m_pComboAttributeSub->addItem ( "x2" );
		m_pComboAttributeSub->addItem ( "y2" );
		m_pComboAttributeSub->addItem ( "width" );
		m_pComboAttributeSub->addItem ( "height" );
	}
	else if ( newText == "visibleFromTo" )	{
		m_pComboAttributeSub->addItem ( "startFrame" );
		m_pComboAttributeSub->addItem ( "stopFrame" );
	}
	else if ( ( newText == "pos" )|| ( newText == "res" ) || ( newText == "scale" ) || ( newText == "shear" ) || ( newText == "offset" ) )   {
		m_pComboAttributeSub->addItem ( "x" );
		m_pComboAttributeSub->addItem ( "y" );
	}
	else if ( newText == "showBackground" )	{
		m_pComboAttributeSub->addItem ( "true" );
		m_pComboAttributeSub->addItem ( "false" );
	}
	else if ( newText == "frameStyle" )	{
		m_pComboAttributeSub->addItem ( "SolidLine" );
		m_pComboAttributeSub->addItem ( "DashLine" );
		m_pComboAttributeSub->addItem ( "DashDotLine" );
		m_pComboAttributeSub->addItem ( "DashDotDotLine" );
	}
	else if ( newText == "frameJoin" )	{
		m_pComboAttributeSub->addItem ( "MiterJoin" );
		m_pComboAttributeSub->addItem ( "BevelJoin" );
		m_pComboAttributeSub->addItem ( "RoundJoin" );
	}
	else if ( newText == "font" )	{
		m_pComboAttributeSub->addItem ( m_qsFontName );
	}
	else if ( newText == "textAlign" )	{
		m_pComboAttributeSub->addItem ( "AlignLeft" );
		m_pComboAttributeSub->addItem ( "AlignRight" );
		m_pComboAttributeSub->addItem ( "AlignHCenter" );
		m_pComboAttributeSub->addItem ( "AlignJustify" );
	}
}

