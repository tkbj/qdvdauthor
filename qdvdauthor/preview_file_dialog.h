/***************************************************************************
    file_preview_dialog.h
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#ifndef PREVIEW_FILE_DIALOG_H
#define PREVIEW_FILE_DIALOG_H
 
#include <QFileDialog>
 
class QLabel;
 
class PreviewFileDialog : public QFileDialog
{
  Q_OBJECT
  public:
    explicit PreviewFileDialog(
      QWidget* parent = 0,    //ooo
      //QLabel* parent = 0,       //xxx
      const QString & caption = QString(),
      const QString & directory = QString(),
      const QString & filter = QString()
    );
 
  protected slots:
    void OnCurrentChanged(const QString & path);
 
  protected:
    QLabel* mpPreview;
 
};
 
#endif // PREVIEW_FILE_DIALOG_H 
