/****************************************************************************
** DialogAudio
**
**   Created : Sat Nov 10 12:09:08 2006
**        by : Varol Okan using XEmacs
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/

#ifndef DIALOGSELECT_H
#define DIALOGSELECT_H

//#include "uidialogselect.h"		//ooo
#include "ui_uidialogselect.h"		//xxx
//Added by qt3to4:
//#include <Q3ValueList>	//oxx

class SourceFileEntry;

//class DialogSelect : public uiDialogSelect				//ooo
class DialogSelect : public QDialog, public Ui::uiDialogSelect		//xxx
{
    Q_OBJECT
    class ObjectEntry {
    public:
      ObjectEntry ( ) { };
      QString     qsDisplayName;
      QString     qsFileName;
      QStringList listChapters;
    };

public:
     //DialogSelect ( QWidget *p=0, const char *name=0, bool modal=FALSE, Qt::WFlags fl=0 );	//ooo
     DialogSelect ( QWidget *pParent=0 );							//xxx
    ~DialogSelect ( );

    bool   initMe ( QStringList&, bool );
    bool        getLoop              ( );
    QStringList getSelected          ( );
    QString     getFinalDestination  ( );

private:
    void populateList ( );
    void initButton ( QStringList & );

protected slots: 
    virtual void slotSelectionChanged( );
    virtual void slotChangeChapters  ( );
    virtual void slotButtonUp        ( );
    virtual void slotButtonDown      ( );
    virtual void slotButtonAdd       ( );
    virtual void slotButtonRemove    ( );
    virtual void slotButtonAddAll    ( );
    virtual void slotButtonRemoveAll ( );
    virtual void slotButtonShowAll   ( );
    virtual void slotHeaderClicked   (  int );
    virtual void slotCheckLoop       ( bool );

private:
    QList<ObjectEntry *>m_listSourceObjects;		//oxx
    QStringList m_listDVDMenuSourceFiles;

    bool    m_bShowAll;
    QString m_qsCurrentMenuName;
};

#endif // DIALOGSELECT_H
