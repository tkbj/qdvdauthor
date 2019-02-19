/***************************************************************************
    dialogtextfont.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef DIALOGTEXTFONT_H
#define DIALOGTEXTFONT_H

//#include <qfont.h>	//ooo
#include <QFont>
//#include <qstring.h>	//ooo
#include <QString>
//Added by qt3to4:
#include <QResizeEvent>
#include <QPaintEvent>

// the designer created GUI.
#include "rgba.h"
//#include "uidialogtextfont.h"		//ooo
#include "ui_uidialogtextfont.h"	//xxx

class QPaintEvent;
class TextObject;

//class DialogTextFont : public uiDialogTextFont			//ooo
class DialogTextFont : public QDialog, public Ui::uiDialogTextFont	//xxx
{
	Q_OBJECT
public:
	 DialogTextFont ( QWidget * parent, TextObject *p=NULL );
	~DialogTextFont ( );

	void     setFont (const QFont & );
	QFont   &getFont              ( );
	void     setText     (QString & );
	void     setTextAlign (QPoint & );
	void     setTransparency (float );
	void     setRect       (QRect & );
	QRect   &getRect              ( );
	void     setAnimation(QString & );
	QString &getAnimation         ( );
	bool     getFit               ( );
	QString  getText              ( );
	QPoint  &getTextAlign         ( );
        Rgba    &getBackgroundColor   ( );
	Rgba    &getForegroundColor   ( );
	float    getTransparency      ( );
	void     setBackgroundColor   ( Rgba & );
	void     setForegroundColor   ( Rgba & );

protected:
	void initMe             ( );
	void updateFont         ( uint iSwitch );
	void updateButtons      ( );
	int  findString         ( QStringList &, QString );
	virtual void paintEvent ( QPaintEvent * );

protected slots:
	virtual void resizeEvent ( QResizeEvent * );
	//virtual void slotFontFamilyChanged ( int  );	//ooo
	virtual void slotFontFamilyChanged (   );	//xxx
	virtual void slotFontStyleChanged  ( int  );
	virtual void slotFontSizeChanged   ( int  );
	virtual void slotFontSizeChanged   ( const QString & );
	virtual void slotScriptChanged     ( int  );
	virtual void slotRight             ( bool );
	virtual void slotCenter            ( bool );
	virtual void slotLeft              ( bool );
	virtual void slotJustify           ( bool );
	virtual void slotVCenter           ( bool );
	virtual void slotFit               ( bool );
	virtual void slotUnderline         ( bool );
	virtual void slotStrikeout         ( bool );
	virtual void slotAnimation         (      );
	virtual void slotForegroundColor   (      );
	virtual void slotBackgroundColor   (      );
	virtual void slotTransparency      ( int  );
	virtual void slotOkay              (      );

signals:
	void signalAnimate ( QString &, int, QString );

private:
	QFont   m_qfFont;
	Rgba    m_backgroundColor;
	Rgba    m_foregroundColor;
	float   m_fTransparency;
	QString m_qsAnimation;
	int     m_iSelectedFontSize;

	static QFont    m_previousFont;
	static QString  m_previousText;
	static float    m_previousTransparency;
	static QPoint   m_previousTextAlign;
	static Rgba     m_previousForeground;
	static Rgba     m_previousBackground;
};

#endif // DIALOGTEXTFONT_H

