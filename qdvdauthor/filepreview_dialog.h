/***************************************************************************
    filepreview_dialog.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef FILEPREVIEW_DIALOG_H
#define FILEPREVIEW_DIALOG_H
 
#include <QFileDialog>
//#include <QGridLayout>    //xxx
#include <QVBoxLayout>      //xxx
 
class QLabel;
class MediaInterface;
 
class FilePreview_Dialog : public QFileDialog
{
  Q_OBJECT
  public:
    explicit FilePreview_Dialog(
      QWidget* parent = 0,
      void *e = 0,
      const QString & caption = QString(),
      const QString & directory = QString(),
      const QString & filter = QString()
    );
    virtual ~FilePreview_Dialog ( );
    
    MediaInterface *m_pMediaInterface;
 
  protected slots:
    void OnCurrentChanged(const QString & path);
    virtual void slotSetPreview ( );
 
  private:
    //MediaInterface *m_pMediaInterface;
    QString         m_qsPath;
    QVBoxLayout    *box;
    //QGridLayout    *layout;
    
  protected:
    //QLabel* mpPreview;
    QWidget* mpPreview;
 
};
 
#endif // FILEPREVIEW_DIALOG_H 
 
