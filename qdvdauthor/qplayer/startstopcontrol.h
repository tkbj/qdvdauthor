/***************************************************************************
    startstopcontrol.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0

    StartStopControl class to display the Video sources name, and thumbnail

****************************************************************************/

#ifndef STARTSTOPCONTROL_H
#define STARTSTOPCONTROL_H

//#include "uistartstopcontrol.h"		//ooo
#include "ui_uistartstopcontrol.h"	//xxx

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qslider.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMouseEvent>
//#include <Q3ValueList>	//oxx
#include <QTimerEvent>
#include <QKeyEvent>
#include <QPaintEvent>

class QTimer;

class SliderHandle : public QPushButton
{
	Q_OBJECT
public:
	 SliderHandle (QWidget *, const char *);
	~SliderHandle ();

	void setMinMax (int, int);
	int getMin ();
	int getMax ();

signals:
	void signalNewValue       ( int );
	void signalSliderPressed  ( );
	void signalSliderReleased ( );

protected:
	virtual void mouseMoveEvent    (QMouseEvent *);
	virtual void mousePressEvent   (QMouseEvent *);
	virtual void mouseReleaseEvent (QMouseEvent *);

	bool m_bPressed;
	int  m_iMinX;
	int  m_iMaxX;
};

class StartStopSlider : public QSlider
{
	Q_OBJECT

public:
	 StartStopSlider (QWidget *p, const char *n=0 );
	virtual ~StartStopSlider ();

	void setProgress (float, QString &);
	void setMinValue              (int);
	void setMaxValue              (int);
	void setMinMax           (int, int);
	void setMin                   (int);
	void setMax                   (int);
	int  getMin                      ();
	int  getMax                      ();

signals:
	virtual void signalValuesChanged  ( int, int );
	virtual void signalStartStopPressed   ( bool );
	virtual void signalStartStopReleased  ( bool );

protected slots:
	virtual void show              ( );
	virtual void slotStartPressed  ( );
	virtual void slotEndPressed    ( );
	virtual void slotStartReleased ( );
	virtual void slotEndReleased   ( );
	virtual void slotNewStartValue ( int);
	virtual void slotNewEndValue   ( int);
	virtual void timeoutRepeat     ( );

protected:
	virtual void paintEvent       (QPaintEvent  *);
	virtual void resizeEvent      (QResizeEvent *);
	virtual void mousePressEvent  (QMouseEvent  *);
	virtual void mouseReleaseEvent(QMouseEvent  *);
	virtual void keyPressEvent    (QKeyEvent    *);

private:
	void moveTowards              (int);

private:
	int m_iMin;
	int m_iMax;
	SliderHandle *m_pHandleStart;
	SliderHandle *m_pHandleEnd;
	// Here we store the last clicked handle to handle key + mous clicks in the slider.
	SliderHandle *m_pLastClicked;
	QTimer       *m_pRepeatTimer;
};

////class StartStopControl : public uiStartStopControl			//ooo
class StartStopControl : public QWidget, Ui::uiStartStopControl		//xxx
{
	Q_OBJECT
public:
	 //StartStopControl (QWidget* p=0, const char* n=0, Qt::WFlags f=0);	//ooo
	 StartStopControl (QWidget* pParent=0);					//xxx
	~StartStopControl ();
	
	void initMe      ( QString, int      );
	void initMe      ( QString, int, int );
	void createThumbnails ( int i=0 );

	void showSlider  ( bool  );
	void setProgress ( float );
	void setStart    ( float ); // [0.0 .. 1.0]
	void setEnd      ( float ); // [0.0 .. 1.0]

	void updateMovieLine   ( );
	void drawStartStopLines( );

public slots:
	virtual void slotSliderValues   ( int, int );
	virtual void slotSliderPressed  ( bool );
	virtual void slotSliderReleased ( bool );

signals:
	void signalSliderPressed  ( bool );
	void signalSliderReleased ( bool );
	void signalSliderValues   ( float, float );

protected:
	virtual void timerEvent   ( QTimerEvent  * );

private:
	StartStopSlider       *m_pStartStopSlider;
	float                  m_fStart;
	float                  m_fEnd;
	QPixmap                m_pixmapMovieLine;
	int                    m_iThumbnailSize;
	QList <QImage *>       m_listOfScreenshots;	//ooo
        //QList <QImage>         m_listOfScreenshots;	//xxx
	int                    m_iNrOfPreviews;
	QString                m_qsFileName;
};

#endif // STARTSTOPCONTROL_H
