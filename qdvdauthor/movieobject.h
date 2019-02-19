/***************************************************************************
    movieobject.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
  pure virtual class as base for the three button types.
    
****************************************************************************/

#ifndef MOVIEOBJECT_H
#define MOVIEOBJECT_H

#include <qimage.h>
#include <qdatetime.h>
#include <qsize.h>
//Added by qt3to4:
#include <QPixmap>
#include <QTimerEvent>

#include "imageobject.h"

class QTimer;

class DragNDropContainer;
class DVDMenu;

class MovieObject : public ImageObject
{
	Q_OBJECT
public:
	enum enType { Loop = 0, Stretch, Stop };
private:
	class MovieData : public ImageObject::ImageData
	{
	public:
	           MovieData ( );
	  virtual ~MovieData ( );
	  virtual  Data &operator  = ( Data & );
	  virtual  bool  operator == ( Data & );
	  virtual  Data *clone ( );

	  long     m_iMSecPreview;
	  QTime    m_timeOffset;
	  QTime    m_timeDuration;
	  bool     m_bMenuLength;
	  enType   m_enLoop;
	  bool     m_bStartAfterDropEvent;
	};

public:
	MovieObject                     ( QWidget *pParent=0 );
	virtual ~MovieObject            ( );

	virtual void drawContents       ( QPainter *, int, int );
	virtual bool readProjectFile    ( QDomNode & );
	virtual bool writeProjectFile   ( QDomElement & );
	virtual bool createContextMenu  ( QPoint  );
	virtual void setFile            ( QString );
	virtual StructureItem *createStructure ( StructureItem * );
	virtual MenuObject    *clone    ( QWidget *p=NULL, MenuObject *p2=NULL );

	virtual void setPixmap          ( QPixmap & );
	virtual void updatePixmap       ( );
	virtual QString &extractionPath ( );  // dynamically create this one

	void     setStartAfterDropEvent ( bool   );
	void     setExtracted           ( bool   );
	void     setDVDMenu             ( DVDMenu *, bool b=false);
	void     setOffset              ( QTime &);
	void     setDuration            ( QTime &);
	void     setMenuLength          ( bool   );
	void     setLoop                ( enType );
	void     setMSecPreview         ( long   );
	void     setNiceness            ( int    );
	void     setMovieSize           ( QSize &);
	QTime   &offset                 ( );
	QTime   &duration               ( );
	bool     isMenuLength           ( );
	enType   loop                   ( );
	long     msecPreview            ( );
	QSize   &movieSize              ( );
	QImage  *previewImage           ( );
	DVDMenu *dvdMenu                ( );

	void    setDragNDropValues      ( DragNDropContainer *);
	void    registerToRender        ( );
	void    startConverting         ( bool );

protected:
	virtual AnimationAttribute *getSpecificAttributes (long, QString);
	virtual void timerEvent         ( QTimerEvent *);	// called after the preview has been rendered in a child task.
	void loadPixmap                 ( QString);
	void createExtractionPath       ( );

public slots:
	virtual void slotStartConverting( );
	virtual void slotProperties     ( );

protected slots:
	virtual void slotEdit           ( );
	virtual void slotSetThumbnail   ( long );

/* Handled in base class ImageObject
	virtual void drawContents(QPainter *);
	virtual bool mousePressEvent       (QMouseEvent *);
	virtual bool mouseReleaseEvent     (QMouseEvent *);
	virtual bool mouseDoubleClickEvent (QMouseEvent *);
	
	QString m_qsFileName;
	QPixmap m_pixmap;
	ImageManipulator m_imageManipulator;
*/

private:
	MovieData *movieData ( );

private:
	DVDMenu *m_pDVDMenu;
	bool     m_bExtracted;
	QSize    m_sizeOriginalMovieSize;
	QImage   m_imagePreview;	// used to create a preview in a background task.
	int      m_iHowNiceShouldIBe;
	QTimer  *m_pConvertTimer;
};

#endif  // MOVIEOBJECT_H
