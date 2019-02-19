/***************************************************************************
    filepreviewdialog.h
                             -------------------
    FilePreviewDialog - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is creates an openFileDialog and presents a preview of
   Image, and Video files.
    
****************************************************************************/
 
#ifndef FILEPREVIEWDIALOG_H
#define FILEPREVIEWDIALOG_H

//#include <qstringlist.h>	//ooo
#include <QStringList>		//xxx
//#include <q3filedialog.h>	//ooo
#include <QFileDialog>		//xxx
//#include <qlabel.h>		//ooo
#include <QLabel>		//xxx

class MediaInterface;

//class FilePreviewDialog : public QLabel, public Q3FilePreview		//ooo
//class FilePreviewDialog : /*public QLabel,*/ public QFileDialog		//xxx
class FilePreviewDialog : public QLabel/*, public QFileDialog*/		//xxx
{
  Q_OBJECT
public: 
           FilePreviewDialog ( void *e=NULL, QWidget *p=NULL );
  virtual ~FilePreviewDialog ( );

  //virtual void previewUrl ( const Q3Url &theUrl );		//ooo
  //virtual void previewUrl ( const QUrl &theUrl );		//xxx
  //virtual void previewUrl ( const QString &path );		//xxx

  // This is a drop in replacement for QFileDialog::getOpenFileName, including a preview ( Image or Video )
  static QString getOpenFileName  ( void          *m=NULL,          // pointer to existing media engine
				    const QString &s=QString::null, // Start in dir
				    const QString &f=QString::null, // filter
				    QWidget       *p=NULL,          // Parent
				    const char    *n=NULL,          // name
				    const QString &c=QString::null, // Caption
				    QString       *e=0,             // selected Filter
				    bool           o=true );        // resolve symlinks

  static QStringList getOpenFileNames   ( void    *m=NULL,          // pointer to existing media engine
				    const QString &s=QString::null, // Start in dir
				    const QString &f=QString::null, // filter
				    QWidget       *p=NULL,          // Parent
				    const char    *n=NULL,          // name
				    const QString &c=QString::null, // Caption
				    QString       *e=0,             // selected Filter
				    //bool           o=TRUE );        // resolve symlinks	//ooo
				    bool           o=true );        // resolve symlinks		//xxx

  static QStringList getOpenFileNames   ( 
				    const QString &s=QString::null, // Start in dir
				    const QString &f=QString::null, // filter
				    QWidget       *p=NULL,          // Parent
				    const char    *n=NULL,          // name
				    const QString &c=QString::null, // Caption
				    QString       *e=0,             // selected Filter
				    //bool           o=TRUE );        // resolve symlinks	//ooo
				    bool           o=true );        // resolve symlinks		//xxx

protected slots:
  virtual void previewUrl ( const QUrl &theUrl );		//xxx
  //virtual void previewUrl ( const QString &path );		//xxx  
  virtual void slotSetPreview ( );  

private:
  MediaInterface *m_pMediaInterface;
  bool            m_bSetupPreview;
  QString         m_qsPath;
};

#endif	// FILEPREVIEWDIALOG_H
