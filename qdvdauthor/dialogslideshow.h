/****************************************************************************
** DialogSlideshow
**
**   Created : Wed Jun 4 12:09:08 2008
**        by : Varol Okan using vi
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#ifndef DIALOGSLIDESHOW_H
#define DIALOGSLIDESHOW_H

//#include "uislideshow.h"	//ooo
#include "ui_uislideshow.h"	//xxx
//Added by qt3to4:
#include <QPixmap>

// 5 seconds transition ...
#define THUMBNAIL_COUNT 5 * 10

class QTimer;
class CXmlSlideshow;

namespace Input
{

//class DialogSlideshow : public uiSlideshow				//ooo
class DialogSlideshow : public QDialog, public Ui::uiSlideshow		//xxx
{
    Q_OBJECT

public:
     DialogSlideshow ( QWidget *parent=0 );
    ~DialogSlideshow ( );

  QString     background ( );
  QStringList filterList ( );
  QStringList &audioList ( );
  void initMe ( CXmlSlideshow *, bool b=true );

protected slots:
  virtual void accept          ( );
  virtual void reject          ( );
  virtual void slotAll         ( );
  virtual void slotAnim        ( );
  virtual void slotImage       ( );
  virtual void slotClear       ( );
  virtual void slotColor       ( );
  virtual void slotSelect      ( );
  virtual void slotKBParams    ( );
  virtual void slotAddAudio    ( );
  virtual void slotAnimatePix  ( );
  virtual void slotDeleteAudio ( );
  virtual void slotValueChanged(  int );
  virtual void slotKenBurns    ( bool );
  virtual void slotIntroPage   ( bool );
  //virtual void slotSelectionChanged  ( Q3ListViewItem * );    //ooo
  virtual void slotSelectionChanged  ( QTreeWidgetItem * );     //xxx
  virtual void slotCreateListViewPix ( );
  virtual void initTransitions   ( );
  virtual void createVidsLength  ( );
  virtual void createAudioLength ( );

private:
  void createTransitions ( );
  void displayLength     ( );
  void recreatePix       ( );
  QPixmap getListViewPix ( QString );

private:
  CXmlSlideshow *m_pSlideshow;
  QString        m_qsBackground;
  QStringList    m_listAudio;
  unsigned int   m_iTotalAudioLength;

  QTimer        *m_pTimer;
  QPixmap       *m_arrayPix[THUMBNAIL_COUNT];
  int            m_iCurrentPix;
  bool           m_bAnimated;
  bool           m_bExitNow;
};

}; // end of Namespace Input

#endif // DIALOGSLIDESHOW_H
