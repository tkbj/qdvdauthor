/***************************************************************************
    filepreview_dialog.cpp
                             -------------------
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
****************************************************************************/

#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QMessageBox>

#include "qplayer/mediacreator.h"
#include "filepreview_dialog.h"
 
FilePreview_Dialog::FilePreview_Dialog(
  QWidget* parent,
  void *pExistingEngine,
  const QString & caption,
  const QString & directory,
  const QString & filter
) :
    QFileDialog(parent, caption, directory, filter)
{
  //m_pMediaInterface = NULL;     //xxx  
    
  setObjectName("PreviewFileDialog");
  resize ( 650, 300 );
  
  //QVBoxLayout* box = new QVBoxLayout(this);	          //ooo
  box = new QVBoxLayout(this);                  	//xxx
  //QVBoxLayout* box = new QVBoxLayout();		//xxx
 
  //mpPreview = new QLabel(tr("Preview"), this);
  
  m_pMediaInterface = MediaCreator::createPreferredWidget (this, "MediaWidget", pExistingEngine);   //xxx
  
  mpPreview = m_pMediaInterface->getWidget();
  //mpPreview->setAlignment(Qt::AlignCenter);
  mpPreview->setObjectName("labelPreview");
  mpPreview->setFixedSize(300,200);
  box->addWidget(mpPreview);
  //box->addWidget(m_pMediaInterface->getWidget());   //xxx
  
  m_pMediaInterface->initMediaEngine(); //xxx
 
  box->addStretch();
 
  // add to QFileDialog layout
  {
    QGridLayout *layout = (QGridLayout*)this->layout();
    //layout = (QGridLayout*)this->layout();  
    //layout = this->layout();  
    layout->addLayout(box, 1, 3, 3, 1);
  }
  
  /*QGridLayout *m_pVideoLayout = new QGridLayout (this);
  //m_pVideoLayout->addWidget (m_pMediaInterface->getWidget(), 1, 1, 4, 6);
  m_pVideoLayout->addWidget (m_pMediaInterface->getWidget(), 1, 3, 3, 1);*/
  
  //connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(OnCurrentChanged(const QString&)));
}

FilePreview_Dialog::~FilePreview_Dialog()
{
  if ( m_pMediaInterface )  {
    m_pMediaInterface->stop ( );
    delete box;
    
    //delete layout;
    delete m_pMediaInterface;     //ooo
  }
}
 
void FilePreview_Dialog::OnCurrentChanged(const QString & path)
{
  /*QPixmap pixmap = QPixmap(path);
  if (pixmap.isNull()) {
    mpPreview->setText("not an image");
  } else {
    mpPreview->setPixmap(pixmap.scaled(mpPreview->width(), mpPreview->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }*/
  /*m_pMediaInterface->getWidget ( )->show ( );
  m_pMediaInterface->playMRL   ( path  );
  m_pMediaInterface->setVolume    ( 0.1f );*/
  
  m_qsPath = path;
  
  QTimer::singleShot ( 100, this, SLOT ( slotSetPreview ( ) ) );
}

void FilePreview_Dialog::slotSetPreview ( )
{
  m_pMediaInterface->getWidget ( )->show ( );
  m_pMediaInterface->playMRL   ( m_qsPath  );
  m_pMediaInterface->setVolume    ( 0.1f );
}
