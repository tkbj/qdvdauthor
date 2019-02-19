/***************************************************************************
    file_preview_dialog.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QSplitter>    //xxx
#include <QMessageBox>

#include "preview_file_dialog.h"
#include <QLabel>
#include <QGridLayout>
 
PreviewFileDialog::PreviewFileDialog(
  QWidget* parent,    //ooo
  //QLabel* parent,       //xxx
  const QString & caption,
  const QString & directory,
  const QString & filter
) :
    QFileDialog(parent, caption, directory, filter)
{
  setObjectName("PreviewFileDialog");
  QVBoxLayout* box = new QVBoxLayout(this);	//ooo
  //QVBoxLayout* box = new QVBoxLayout();		//xxx
 
  mpPreview = new QLabel(tr("Preview_2"), this);
  mpPreview->setAlignment(Qt::AlignCenter);
  mpPreview->setObjectName("labelPreview");
  mpPreview->setFixedSize(300,200);
  box->addWidget(mpPreview);
 
  box->addStretch();
 
  // add to QFileDialog layout
  {
    //QGridLayout *layout = (QGridLayout*)this->layout();   //ooo
    QGridLayout *gridLayout = dynamic_cast<QGridLayout *>(this->layout());   //xxx
    //QGridLayout *gridLayout = static_cast<QGridLayout *>(this->layout());   //xxx
/*
    //layout->addLayout(box, 1, 3, 3, 1);   //ooo
    //gridLayout->addItem(box,1,3,1,1);       //xxx
    
  QList< QPair<QLayoutItem*, QList<int> > > movedItems;
  for(int i = 0; i < gridLayout->count(); i++)
  {
    int row, column, rowSpan, columnSpan;
    gridLayout->getItemPosition(i,&row,&column,&rowSpan,&columnSpan);

    if (row > 2)
    {
      QList<int> list;
      list << row << column << rowSpan << columnSpan;
      movedItems << qMakePair(gridLayout->takeAt(i),list);
      i--;
    }
  }
  for(int i = 0; i < movedItems.count(); i++)
  {
    gridLayout->addItem(movedItems[i].first,
                    movedItems[i].second[0],
                    movedItems[i].second[1],
                    movedItems[i].second[2],
                    movedItems[i].second[3]
            );
  }
 
  gridLayout->addLayout(box, 1, 3, 3, 1);   //ooo 
  //gridLayout->addItem(box,1,3,1,1);
*/
    if (gridLayout != NULL) //xxx
    {
        QLayoutItem *layoutItem = gridLayout->itemAtPosition(1,0);    //ooo
        //QLayoutItem *layoutItem = gridLayout->itemAtPosition(0,0);      //xxx
        QSplitter *splitter;
        splitter = dynamic_cast<QSplitter *>(layoutItem->widget());

        if (splitter != NULL)
        {
            QFrame *framePreView = new QFrame(splitter);
            splitter->addWidget(framePreView);
            
            //QVBoxLayout *vboxPreViewLayout = new QVBoxLayout(framePreView);   //ooo
            //QVBoxLayout *vboxPreViewLayout = new QVBoxLayout(this);             ///xxx
            
            //QLabel *lblPreviewPicture = new QLabel(tr("Preview"), framePreView);
            //lblPreviewPicture->setFrameShape(QFrame::NoFrame);
            //lblPreviewPicture->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            //lblPreviewPicture->setMinimumSize(100,100);
            
            //vboxPreViewLayout->addWidget(lblPreviewPicture, 1, Qt::AlignHCenter);

            //framePreView->setLayout(vboxPreViewLayout); //ooo
            framePreView->setLayout(box);             //xxx

            //connect(this, SIGNAL(currentChanged(QString)), this, SLOT(setPreviewPicture(QString)));
        }
    }
    
  }
  

  
  connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(OnCurrentChanged(const QString&)));
}
 
void PreviewFileDialog::OnCurrentChanged(const QString & path)
{
  QPixmap pixmap = QPixmap(path);
  if (pixmap.isNull()) {
    mpPreview->setText("not an image");
  } else {
    mpPreview->setPixmap(pixmap.scaled(mpPreview->width(), mpPreview->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
} 
