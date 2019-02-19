/***************************************************************************
    dialoganimation.h
                             -------------------
    Interface to the GUI.                         
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e
    
    license              : GPL v 2.0

****************************************************************************/

#ifndef DIALOGANIMATION_H
#define DIALOGANIMATION_H

// the designer created GUI.
//#include "uidialoganimation.h"	//ooo
#include "ui_uidialoganimation.h"	//xxx

//class DialogAnimation : public uiDialogAnimation			//ooo
class DialogAnimation : public QDialog, public Ui::uiDialogAnimation	//xxx
{
	Q_OBJECT
public:
	enum	{
		TypeMovie,
		TypeImage,
		TypeFrame,
		TypeText,
		TypeOverlay,
		TypeImageBackground,
		TypeMovieBackground
	};

	 //DialogAnimation (QWidget * parent=NULL, const char * name = 0, Qt::WFlags f = 0 );	//ooo
	 DialogAnimation (QWidget * pParent=NULL );						//xxx
	~DialogAnimation ();

	void initMe ( QString, long, float, int );
	QString  &string ();
	void      setFontName ( QString );

protected slots:
	virtual void slotAttribute    ( const QString & );
	virtual void slotHelp         (  );
	virtual void slotAddAttribute (  );
	virtual void slotValueChanged ( int );

private:
	QString m_qsAnimation;
	QString m_qsFontName;
	long    m_iMaxFrames;
	float   m_fFPS;
};

#endif // DIALOGANIMATION_H

