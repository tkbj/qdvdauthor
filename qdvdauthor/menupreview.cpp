/***************************************************************************
    menupreview.cpp
                             -------------------
    MenuPreview class                             
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is the encapsulation of the MenuPreview.

   The MenuPreview is derived off the ButtonPreview and used 
   to display a DVD Menu.
   The ButtonPreview is the preview class used in the ButtonObject
   to display the different levels of a buttonObject.
    
****************************************************************************/

#include <sys/stat.h>

#include <QFileDialog>
#include <QPainter>
#include <QMenu>
#include <QMessageBox>
#include <QTextStream>

#include "global.h"
#include "qdvdauthor.h"
#include "dvdmenuundoobject.h"	//ooo
#include "xml_dvd.h"
#include "buttonpreview.h"
#include "dialogtextfont.h"
#include "dialogbutton.h"	//ooo
#include "menupreview.h"
#include "menuobject.h"	//ooo
#include "frameobject.h"
#include "maskobject.h"
#include "textobject.h"
#include "imageobject.h"
#include "movieobject.h"	//ooo
#include "buttonobject.h"	//ooo
#include "sourcefileentry.h"
#include "dialogshadow.h"
#include "filepreviewdialog.h"
#include "qplayer/engines/dummywidget.h"
#include "qplayer/mediacreator.h"

#include "dialogimage2.h"

//MenuPreview::MenuPreview(QWidget * parent, const char * name, Qt::WindowFlags f)	//ooo
MenuPreview::MenuPreview(QWidget * parent )						//xxx
  //: ButtonPreview   ( parent, name, f )	//ooo
  : ButtonPreview   ( parent ),			//xxx
    m_pDialogButton ( NULL )			//xxx
{
  initMe (NULL);
  
  connect ( &m_selectionObject, SIGNAL ( signalShadowMe    ( MenuObject * ) ),      this, SLOT ( slotAddShadow   ( MenuObject * ) ) );
  connect ( &m_selectionObject, SIGNAL ( signalDeleteMe    ( MenuObject * ) ),      this, SLOT ( slotDeleteObject( MenuObject * ) ) );
    
}

MenuPreview::~MenuPreview()
{
  if ( m_pDialogButton )
    delete m_pDialogButton;
  m_pDialogButton = NULL;  
}

MenuPreview &MenuPreview::operator = ( MenuPreview &theOther )
{
  m_bIsSubMenu   = theOther.m_bIsSubMenu;
  m_bIsMovieMenu = theOther.m_bIsMovieMenu;
  m_pPgcColors   = theOther.m_pPgcColors;
  m_bImageButton = theOther.m_bImageButton;
  m_doubleBuffer = theOther.m_doubleBuffer;
  ButtonPreview *pMe    = (ButtonPreview *)this;
  ButtonPreview *pOther = (ButtonPreview *)&theOther;
  *pMe = *pOther;

  uint t;
  MenuObject  *pObject;
  // At this point we have new objects but none of them are connected to any signals / slots ...
  for ( t=0; t<objectCount ( ); t++ ) {
    pObject  = menuObject ( t );
    if ( ! pObject )
      continue;

    pObject->disconnect ( );
    connectStdSlots ( pObject, true );
  }

  return *this;
}

void MenuPreview::initMe ( Rgba *pColors )
{     
  m_iCreateObjectStyle = TypeNone;
  m_iPointerState      = StateNormal;
  m_pActiveObject      = NULL;
  m_pDialogButton      = NULL;	//oooo
  m_bIsSubMenu         = false;
  m_bIsMovieMenu       = false;
  m_bImageButton       = true;
  m_pPgcColors         = pColors;
}

void MenuPreview::clear ( Rgba *pColors )
{
  uint t;
  if ( m_pDialogButton )
    delete m_pDialogButton;
  m_pDialogButton = NULL;

  initMe ( pColors );

  // Clear the undoBuffer and create an new one.
  if ( m_pUndoBuffer )
    delete m_pUndoBuffer;
  m_pUndoBuffer = new UndoBuffer ( MAX_UNDO_DEPTH );

  //for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )               //ooo
  for ( t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ ) //xxx
    delete m_listMenuObjects.at ( t );
  m_listMenuObjects.clear ( );
  m_backgroundPixmap = QPixmap ( );
}

void MenuPreview::setIsSubMenu ( bool bIsSubmenu )
{
  // This function is only here to flag that the MenuPreview is part of a SubMenu and
  // Not a VMGM, or something else.
  m_bIsSubMenu = bIsSubmenu;
}

void MenuPreview::setImageButton  (bool bImageButton)
{
	m_bImageButton = bImageButton;
}

bool MenuPreview::createContextMenu ( QPoint globalPos )
{
  QAction *frameNewAction = new QAction("Add Frame", this);
  QAction *textNewAction  = new QAction("Add Text",  this);
  QAction *imageNewAction = new QAction("Add Image", this);
  QAction *movieNewAction = new QAction("Add Movie", this);
  
  QMenu *pMenu = new QMenu (this);
  pMenu->addAction(frameNewAction);
  pMenu->addAction(textNewAction);
  pMenu->addAction(imageNewAction);
  pMenu->addAction(movieNewAction);
  
  connect(frameNewAction, SIGNAL(triggered()), this, SLOT( slotAddFrameObject ( ) ) );
  connect(textNewAction,  SIGNAL(triggered()), this, SLOT( slotAddTextObject  ( ) ) );
  connect(imageNewAction, SIGNAL(triggered()), this, SLOT( slotAddImageObject  ( ) ) );
  connect(movieNewAction, SIGNAL(triggered()), this, SLOT( slotAddMovieObject  ( ) ) );
  
  pMenu->exec( globalPos );
  
  delete pMenu;
  return true;
}

void MenuPreview::mousePressEvent (QMouseEvent *pEvent)
{
  // First we let the base class have it ...
  m_bMouseEventDone = false;
  if ( m_selectionObject.mousePressEvent ( pEvent ) )
    return;
  
  if ( m_iPointerState != StateSelected )
    ButtonPreview::mousePressEvent ( pEvent );
  // Here we check if the mouse click appeared withtin one of the MenuObjects,
  // in which case the Object will take over the mouse handling ...
  if (m_bMouseEventDone) {
    m_iPointerState = StateNormal; // added to change state after Add Shadow was selected from ContextMenu...
    return;
  }
  
  //  char arrayState[][30]={ "StateUnknown", "StateNormal", "StateMoveSelected", "StateReadyToCreate", "StateCreating", "StateSelecting", "StateSelected" }; //ooo
  //  printf ("MenuPreview::mousePressEvent pointerState<%s>\n", arrayState[m_iPointerState]);                                                                //ooo

  QPoint      thePos  = pEvent->pos ( );
  MenuObject *pObject = childContains ( thePos );
  
  if ( ( pObject != NULL ) && ( m_iPointerState != StateSelected ) )	{
    m_pActiveObject = pObject;
    update();         //oooo
    //QLabel::update();   //xxx
    //repaint(); //xxx
    //pObject->update();  //xxx
    //ButtonPreview::paint2();  //xxx
    return;
  }
  // Okay the user actually clicked in the MenuPreview ...
  // The left mouse button was clicked.
  // If the user is in creation mode, then we handle this one
  if ( pEvent->button ( ) == Qt::LeftButton )  {
    // if the state is not StateReadyToCreate then we want to select
    if ( m_iPointerState == StateReadyToCreate )  {
      m_iPointerState = StateCreating;
      
      drawBackground   ( m_doubleBuffer );

      setMouseTracking ( true );
    }
    else if ( m_iPointerState == StateSelected )  {
      if ( m_selectionObject.mouseOver ( pEvent->pos ( ) ) ) {
        m_iPointerState = StateMoveSelected;
        m_selectionObject.setMoveState ( false );
        
        QPoint pos = pEvent->pos();     //xxx
        
        //m_selectionObject.setCurrentMousePos ( (QPoint &) pEvent->pos ( ) );		                //ooo
        //m_selectionObject.setCurrentMousePos ( const_cast<QPoint &> ( pEvent->pos ( ) ) );	//xxx
        m_selectionObject.setCurrentMousePos ( pos );	//xxx
        
        drawBackground   ( m_doubleBuffer );
        setMouseTracking ( true );

        UndoBuffer *pUndo = Global::pApp->getUndoBuffer ( );								//ooo
        if ( pUndo )													//ooo
             pUndo->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::MOVED_OBJECT, &m_selectionObject, this ) );	//ooo
      }
    }
    else  {
      if ( m_backgroundPixmap.isNull () )
        return;
      m_iPointerState = StateSelecting;
      setMouseCursor ( MouseCrossCursor );  // signals to be awaiting the next mouse click
      // which will then generate the rectangle for this text button.
      m_selectionObject.clear ( );
      m_pActiveObject = &m_selectionObject;
      drawBackground   ( m_doubleBuffer );
    }
    // Okay, at this point we know the user is creating an object.
    m_rectCurrentObject.setX(pEvent->pos().x());
    m_rectCurrentObject.setY(pEvent->pos().y());
    return;
  }
  // The right mousebutton was pressed means that we should display the context drop down menu.
  setMouseCursor ( MousePointerCursor );
  m_iPointerState = StateNormal;

  createContextMenu ( pEvent->globalPos  ( ) );
}

void MenuPreview::mouseReleaseEvent ( QMouseEvent *pEvent )
{
  // The user is doing something. Let us activate the drawing algol.
  m_rectCurrentObject.setRight  ( pEvent->pos( ).x ( ) );
  m_rectCurrentObject.setBottom ( pEvent->pos( ).y ( ) );

  //  char arrayState[][30]={ "StateUnknown", "StateNormal", "StateMoveSelected", "StateReadyToCreate", "StateCreating", "StateSelecting", "StateSelected" }; //ooo
  //  printf ("MenuPreview::mouseReleaseEvent pointerState<%s>\n", arrayState[m_iPointerState]);  //ooo

  if ( pEvent->button ( ) == Qt::LeftButton )  {   
    m_doubleBuffer = QPixmap ( ); 
    switch ( m_iPointerState ) {
    case StateMoveSelected:  {
      m_iPointerState = StateSelected;
      m_selectionObject.setMoveState ( false );
      updatePixmap ( );
      if ( m_selectionObject.mouseOver ( pEvent->pos ( ) ) )  {
        DVDMenuUndoObject *pUndo = (DVDMenuUndoObject *) undoBuffer ( )->last ( );
        if ( pUndo && ! pUndo->hasMoved ( ) )  {
          undoBuffer  ( )->removeLast   ( );
          delete pUndo;
          Global::pApp->updateUndoStack ( );
        }	//oooo
      }
      return;
    }
    case StateCreating: {
      // First we let the base class have it ...
      m_bMouseEventDone = false;

      ButtonPreview::mouseReleaseEvent(pEvent);

      // Here we check if the mouse click appeared withtin one of the MenuObjects,
      // in which case the Object will take over the mouse handling ...
      if ( m_bMouseEventDone ) 
        setMouseTracking ( false );

      m_pActiveObject = NULL;
      switch ( m_iCreateObjectStyle )	{
      case FrameType :
        createFrameObject ();		//ooo
        break;
      case TextType :
        createTextObject();
        break;
      case ImageType :
        createImageObject();		//ooo
      break;
      case MovieType :
        createMovieObject ();		//ooo
        break;
      } // switch createObjectStyle
      m_iPointerState = StateNormal;
      return;
    } // end of case StateCreating
    case StateSelected: {
      m_pActiveObject = NULL;
      m_iPointerState = StateNormal;
      m_selectionObject.setMoveState ( false );
      //update ( );         //ooo
      QLabel::update ( );   //xxx
      setMouseTracking ( false );
      return;
    }
    case StateSelecting: {
      if ( m_selectionObject.getCount () > 0 ) {
        m_iPointerState = StateSelected;
        setMouseTracking ( true ); // to change cursor shapes.
      }
      else {
        m_iPointerState = StateNormal;
        setMouseTracking ( false );
      }
      setMouseCursor ( MousePointerCursor );
      //update ();          //ooo
      QLabel::update ();    //xxx
      return;
    }
    case StateNormal:        // nothing to do if we are in Normal Mouse Mode
    case StateReadyToCreate: // Should neve happen ...
    case StateUnknown:       // Something went wrong ...
    default:                 // should never get here
      if ( m_pActiveObject )
           m_pActiveObject->mouseReleaseEvent ( pEvent ); // will call updatePixmap
      else
           updatePixmap  ( );
      m_pActiveObject  = NULL;
      setMouseTracking ( false );
      return;
    }
  } //  end if LeftButton
}

void MenuPreview::mouseMoveEvent (QMouseEvent *pEvent)
{
  p_Event = pEvent; //xxx
    
  //char arrayState[][30]={ "StateUnknown", "StateNormal", "StateMoveSelected", "StateReadyToCreate", "StateCreating", "StateSelecting", "StateSelected" };   //ooo
  //printf ("MenuPreview::mouseMoveEvent pointerState<%s>\n", arrayState[m_iPointerState]);   //ooo

  switch ( m_iPointerState ) {
  //m_doubleBuffer = QPixmap ( );    //xxx    
  // The one state I dont need to care ...
  case StateReadyToCreate:
    return;
  case StateMoveSelected:
      
    m_selectionObject.mouseMoveEvent ( pEvent );
    { // doubleBuffer does move the objecs as well, which I don't want
      // I simply want to get rid of the flicker ...
      QPixmap temp ( m_doubleBuffer );
      QPainter thePainter;        //ooo
      //QPainter thePainter(this);    //xxx
      //QLineF line(10.0, 80.0, 90.0, 20.0);  //xxx
      //thePainter.begin ( &temp, this );   //ooo
      thePainter.begin ( &temp );            //xxx
      m_selectionObject.drawSelected ( &thePainter ); //ooo
      //thePainter.drawLine(line);                        //xxx
      thePainter.end ( );
      //bitBlt ( this, 0, 0, &temp );       //oooo
      thePainter.drawPixmap(0, 0, temp );  //xxx
    }
    repaint ( ); //xxx
    break;
  case StateSelected: {
    // this case will take care of the state when multile objects have been selected
    // and now the user is aboout to move those selected objects around.
    // for this in-between-state, we want to display the mouse move cursor
    // if inside one of the selected objects.
    //    if ( m_selectionObject.mouseOver ( pEvent->pos () ) )
    bool bMouseOver =  m_selectionObject.mouseOver ( pEvent->pos () );
    //    printf ( "MenuPreview::mouseMoveEvent mouseOver<%s> - <%dx%d>\n", bMouseOver ? "true" : "false", pEvent->pos().x(), pEvent->pos().y() );
    if ( bMouseOver )
      setMouseCursor ( MouseMoveCursor    );
    else 
      setMouseCursor ( MousePointerCursor );
    //    m_selectionObject.mouseMoveEvent ( pEvent );
    //update ( );
    return;
  }
  case StateSelecting:
  case StateCreating: {
    // Okay at this point we have m_rectCurrentObject.x, and .y set
    // Here we draw the dynamicly changing size of the rect.

    // First we clear the contents of the previous rectangle
    //int iX, iY;   //ooo
    iX  = pEvent->pos ( ).x ( );
    iY  = pEvent->pos ( ).y ( );
/*    m_rectCurrentObject.setRight  ( iX );
    m_rectCurrentObject.setBottom ( iY );

    // Double buffering ...
    QPen     thePen   ( QColor ( 255,  30,  30 ), 2, Qt::DashDotLine );
    QBrush   theBrush ( QColor ( 100, 255, 100 ),  Qt::Dense6Pattern );
    QPainter thePainter;      //ooo
    //QPainter thePainter(this);  //xxx
    QPixmap doubleBuffer;
    if ( m_doubleBuffer.isNull( ) ) {
        
      printf ("MenuPreview::mouseMoveEvent (**)\n");   //xxx    
        
      thePainter.begin      ( this );
    } else {
        
      printf ("MenuPreview::mouseMoveEvent (***)\n");   //xxx        
        
      doubleBuffer = QPixmap ( m_doubleBuffer );
      //thePainter.begin  ( &doubleBuffer, this );  //ooo
      //thePainter.begin  ( this );                   //xxx
      thePainter.begin  ( &doubleBuffer );                   //xxx
    }

    if ( m_iPointerState  == StateSelecting ) {
      thePen = QPen  ( QColor ( 100, 255, 100 ), 1, Qt::SolidLine );
      thePainter.setBrush ( theBrush );
      checkSelection ( ); // Any objects added to m_selectionObject ?
    }
    else
      thePen = QPen (QColor (255, 30,30), 2, Qt::DashDotLine);
    
    thePainter.setPen   ( thePen );
    m_selectionObject.drawSelected ( &thePainter );
    thePainter.drawRect ( m_rectCurrentObject    );
    
    printf ("MenuPreview::mouseMoveEvent m_rectCurrentObject.width ( )<%d>\n", m_rectCurrentObject.width ( ));   //xxx
    
    thePainter.drawText ( iX+5, iY+15, QString   ( "%1 : %2" ).arg( m_rectCurrentObject.width ( ) ).arg( m_rectCurrentObject.height ( ) ) );
    
    thePainter.end ( );   //ooo
    
    if ( ! m_doubleBuffer.isNull ( ) )  {      //ooo
        
      printf ("MenuPreview::mouseMoveEvent (****_in)\n");   //xxx    
        
      //bitBlt ( this, 0, 0, &doubleBuffer );       //oooo
      //thePainter.drawPixmap ( 0, 0, doubleBuffer );   //xxx
      
      QPainter p( this );       //xxx
      //p.begin(&doubleBuffer);	//xxx
      //p.setViewport ( 0, 0 );   //xxx
      p.drawPixmap ( 0, 0, doubleBuffer );   //xxx
      //p.end();                  //xxx
      
      printf ("MenuPreview::mouseMoveEvent (****_out)\n");   //xxx    
      
    }
*/      
    //thePainter.end ( );  //xxx
    
    //this->repaint ( ); //xxx
    repaint ( ); //xxx
    //update ( ); //xxx
    //printf ("MenuPreview::mouseMoveEvent (****)\n");   //xxx

  }
  break;
  case StateUnknown:
  case StateNormal:
  default:
    // This part will move the active object(s) around ...
    //  if ( m_pActiveObject && m_iPointerState != StateSelecting ) {
    if ( m_pActiveObject ) {
      m_pActiveObject->mouseMoveEvent(pEvent);
      //update ( );         //ooo
      //QLabel::update ( );   //xxx
      repaint ( ); //xxx
      
      //printf ("MenuPreview::mouseMoveEvent (*****)\n");   //xxx
      
      return;
    }
  }
}

void MenuPreview::paintEvent (QPaintEvent *pPaint)
{ 
  // first we call the main function    //xxx
  QLabel::paintEvent(pPaint);          //xxx
    
  char arrayState[][30]={ "StateUnknown", "StateNormal", "StateMoveSelected", "StateReadyToCreate", "StateCreating", "StateSelecting", "StateSelected" };   //xxx
  //printf ("MenuPreview::paintEvent pointerState<%s>\n", arrayState[m_iPointerState]);   //xxx
    
  //QMouseEvent *pEvent;    //xxx
    
  //QPainter painter(this);
  //painter.fillRect(event->rect(), QBrush(Qt::white));
    
  switch ( m_iPointerState ) {
    case StateMoveSelected:
      m_selectionObject.mouseMoveEvent ( p_Event );  
    { // doubleBuffer does move the objecs as well, which I don't want
      // I simply want to get rid of the flicker ...
      //QPainter thePainter3;                                                    //xxx
      //thePainter3.begin      ( this );                                         //xxx
      //thePainter3.setRenderHint(QPainter::Antialiasing, true);                 //xxx
      //thePainter3.setPen(QPen(Qt::black, 3, Qt::DashDotLine, Qt::RoundCap));   //xxx
      //thePainter3.setBrush(QBrush(Qt::green, Qt::SolidPattern));               //xxx
      //thePainter3.drawEllipse(200, 80, 400, 240);                              //xxx
      //thePainter3.end ( );
      QPixmap temp ( m_doubleBuffer );
      QPainter thePainter;        //ooo
      //QPainter thePainter(this);    //xxx
      //QLineF line(10.0, 80.0, 90.0, 20.0);  //xxx
      //thePainter.begin ( &temp, this );   //ooo
      thePainter.begin ( &temp );            //xxx
      m_selectionObject.drawSelected ( &thePainter ); //ooo
      //thePainter.drawLine(line);                        //xxx
      thePainter.end ( );
      //bitBlt ( this, 0, 0, &temp );       //oooo
      QPainter p( this );           //xxx
      p.drawPixmap(0, 0, temp );  //xxx
    }
    break;
    case StateCreating: {    
      //QPainter thePainter2;                                                    //xxx
      //thePainter2.begin      ( this );                                         //xxx
      //thePainter2.drawLine(1,1,100,100);                                       //xxx
      //thePainter2.setRenderHint(QPainter::Antialiasing, true);                 //xxx
      //thePainter2.setPen(QPen(Qt::black, 3, Qt::DashDotLine, Qt::RoundCap));   //xxx
      //thePainter2.setBrush(QBrush(Qt::green, Qt::SolidPattern));               //xxx
      //thePainter2.drawEllipse(200, 80, 400, 240);                              //xxx
      //thePainter2.end ( );

      // Okay at this point we have m_rectCurrentObject.x, and .y set
      // Here we draw the dynamicly changing size of the rect.

      // First we clear the contents of the previous rectangle
      //int iX, iY;
      //iX  = pEvent->pos ( ).x ( );
      //iY  = pEvent->pos ( ).y ( );
      m_rectCurrentObject.setRight  ( iX );
      m_rectCurrentObject.setBottom ( iY );

      // Double buffering ...
      QPen     thePen   ( QColor ( 255,  30,  30 ), 2, Qt::DashDotLine );
      QBrush   theBrush ( QColor ( 100, 255, 100 ),  Qt::Dense6Pattern );
      QPainter thePainter;      //ooo
      //QPainter thePainter(this);  //xxx
      QPixmap doubleBuffer;
      if ( m_doubleBuffer.isNull( ) ) {
        thePainter.begin      ( this );
        //printf ("MenuPreview::paintEvent (.:if)\n");   //xxx
      } else {
        doubleBuffer = QPixmap ( m_doubleBuffer );
       //thePainter.begin  ( &doubleBuffer, this );  //ooo
       //thePainter.begin  ( this );                   //xxx
       thePainter.begin  ( &doubleBuffer );                   //xxx
       //printf ("MenuPreview::paintEvent (..:else)\n");   //xxx
      }

      if ( m_iPointerState  == StateSelecting ) {
        thePen = QPen  ( QColor ( 100, 255, 100 ), 1, Qt::SolidLine );
        thePainter.setBrush ( theBrush );
        checkSelection ( ); // Any objects added to m_selectionObject ?
      }
      else
        thePen = QPen (QColor (255, 30,30), 2, Qt::DashDotLine);

      //printf ("MenuPreview::paintEvent (-)\n");   //xxx
    
      thePainter.setPen   ( thePen );
      m_selectionObject.drawSelected ( &thePainter );
      thePainter.drawRect ( m_rectCurrentObject    );
    
      //printf ("MenuPreview::paintEvent m_rectCurrentObject.width ( )<%d>\n", m_rectCurrentObject.width ( ));   //xxx
    
      thePainter.drawText ( iX+5, iY+15, QString   ( "%1 : %2" ).arg( m_rectCurrentObject.width ( ) ).arg( m_rectCurrentObject.height ( ) ) );

      thePainter.end ( );   //ooo  
    
      if ( ! m_doubleBuffer.isNull ( ) ) {        //ooo
        
        //printf ("MenuPreview::paintEvent (--_in)\n");   //xxx  
        
        //bitBlt ( this, 0, 0, &doubleBuffer );       //oooo
        //thePainter.drawPixmap(0, 0, doubleBuffer );   //xxx
      
        QPainter p( this );       //xxxx
        //p.begin(&doubleBuffer);	//xxx
        //p.setViewport ( 0, 0 );   //xxx
        p.drawPixmap ( 0, 0, doubleBuffer );   //xxxx
        //p.end();                  //xxx
      
        //printf ("MenuPreview::paintEvent (--_out)\n");   //xxx 
      }

      }
      break;
    case StateNormal:
    default:
      // This part will move the active object(s) around ...
      //  if ( m_pActiveObject && m_iPointerState != StateSelecting ) {
      if ( m_pActiveObject ) {
        //m_pActiveObject->mouseMoveEvent(pEvent);  //ooo
        //m_pActiveObject->mouseMoveEvent(p_Event);  //xxx  
        //update ( );          //ooo
        //QLabel::update ( );   //xxx
        QPixmap temp ( m_doubleBuffer );  
        QPainter thePainter; 
        thePainter.begin ( &temp );
        m_selectionObject.drawSelected ( &thePainter );
        thePainter.end ( );
        QPainter p( this );
        p.drawPixmap(0, 0, temp );
        //QPixmap doubleBuffer;
        //doubleBuffer = QPixmap ( m_doubleBuffer );
        //thePainter.begin  ( &doubleBuffer );
        //thePainter.begin  ( this );
        //thePainter.drawEllipse(200, 80, 400, 240);
        //thePainter.end ( );
        /*if ( ! m_doubleBuffer.isNull ( ) ) { 
          QPainter p( this );
          p.drawPixmap ( 0, 0, doubleBuffer );
        }*/
        //printf ("MenuPreview::paintEvent (** **)\n");   //xxx
      
        return;
      }
  }
   
}

void MenuPreview::checkSelection ()
{
  // Check if any object has been captured by the selection box.
  uint t;
  m_selectionObject.clear ( );
  //QRect rect = m_rectCurrentObject.normalize ();  //ooo
  QRect rect = m_rectCurrentObject.normalized ();   //xxx
  //for ( t=0;t<m_listMenuObjects.count ( );t++) {                      //ooo
  for ( t=0;t<static_cast<uint> ( m_listMenuObjects.count ( ) );t++) {  //xxx    
    if ( rect.intersects ( m_listMenuObjects.at ( t )->boundingRect ( ) ) ) {
      MenuObject *pObject = m_listMenuObjects.at ( t );
      m_selectionObject.append ( pObject, false );
    }
  }
}

bool MenuPreview::readProjectFile (QDomNode &theNode)
{
  uint t;
  if ( m_pDialogButton )
    delete m_pDialogButton;
  m_pDialogButton = NULL;
  QDomNode xmlPreview = theNode.firstChild ( );

  while ( !xmlPreview.isNull ( ) )  {
    // Here we created a MenuObject, we also want to
    MenuObject *pNewObject = readObject ( xmlPreview );
    // add it to the list ...
    if ( pNewObject )  {
      pNewObject->readProjectFile ( xmlPreview );
      m_listMenuObjects.append    ( pNewObject );
    }
    // So lets get the next sibling ... until we hit the end of DVDMenu ...
    xmlPreview = xmlPreview.nextSibling();
  }

  // we need to make one more step here 
  // since we stored the Display Name for ButtonObjects and we need the SourceFileEntry pointer ...
  //for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ )  {  //xxx
    if ( m_listMenuObjects.at   ( t )->objectType ( ) == BUTTON_OBJECT )  {
      m_pActiveObject = m_listMenuObjects.at(t);
      emit ( signalRequestSourceFiles ( ) );
    }
  }

  return true;
}

MenuObject *MenuPreview::readObject (QDomNode &objectNode)
{
  // This function wil create an MenuObject-derived object
  // depending on the info from the QDomNode - object
  QDomElement theElement = objectNode.toElement ( );
  QString     tagName    = theElement.tagName   ( );
  QString     nodeText   = theElement.text      ( );

  // Okay, this is ugly but after all it is the simplest of all xml file structure.
  // No need to get fancy ...
  MenuObject *pNewObject = NULL;
  if      ( tagName == FRAME_OBJECT  )
    pNewObject = createFrameObject  ( false );
  else if ( tagName == MASK_OBJECT   )
          pNewObject = new MaskObject     (  this );	//oooo
  else if ( tagName == TEXT_OBJECT   )
    pNewObject = createTextObject   ( false );
  else if ( tagName == IMAGE_OBJECT  )
    pNewObject = createImageObject  ( false );
  else if ( tagName == MOVIE_OBJECT  )
    pNewObject = createMovieObject  ( false );	//oooo
  else if ( tagName == BUTTON_OBJECT )	{
    pNewObject = createButtonObject ( false );
    // A small special handling for the Buttons ...
    // Funky, since the above function itself calls this function here ...
    ((ButtonObject *)pNewObject)->readObjects ( objectNode, this );
  }
  /*else if ( tagName == OBJECT_COLLECTION )  {
    pNewObject = createObjectCollection (false);
    // A small special handling for the Buttons ...
    // Funky, since the above function itself calls this function here ...
    ((ObjectCollection *)pNewObject)->readObjects(objectNode, this);
  }*/	//oooo
  else if ( tagName == IGNORE_OBJECT )
    pNewObject = NULL;
  else
    printf ("Warning: MenuPreview::readObject -=> wrong XML Node <%s>\nContinuing ...\n",
        //(const char *)tagName);	//ooo
        tagName.toLatin1().data());	//xxx
  // And finally, if we created a MenuObject, we also want to
  // add it to the list ...
  if (pNewObject)
    pNewObject->readProjectFile ( objectNode );
  // So lets get the next sibling ... until we hit hte end of DVDMenu ...
  return pNewObject;	//oooo
}

bool MenuPreview::writeProjectFile (QDomElement &theElement)
{
  uint t;
  //for (t=0;t<(uint)m_listMenuObjects.count();t++)	{               //ooo
  for (t=0;t<static_cast<uint> ( m_listMenuObjects.count() );t++)	{ //xxx
    if (!m_listMenuObjects.at(t)->writeProjectFile( theElement ))
      return false;
  }
  return true;
}

// The following slots will add a button to the Menu.
void MenuPreview::slotAddFrameObject()
{
  setMouseCursor ( MouseCrossCursor );	// signals to be awaiting the next mouse click
  // which will then generate the rectangle for this text button.
  m_iCreateObjectStyle = FrameType;
  m_iPointerState      = StateReadyToCreate;
}

void MenuPreview::slotAddTextObject()
{
  setMouseCursor ( MouseCrossCursor );	// signals to be awaiting the next mouse click
  // which will then generate the rectangle for this text button.
  m_iCreateObjectStyle = TextType;
  m_iPointerState      = StateReadyToCreate;
}

void MenuPreview::slotAddImageObject ()
{
  setMouseCursor ( MouseCrossCursor );	// signals to be awaiting the next mouse click
  // which will then generate the rectangle for this text button.
  m_iCreateObjectStyle = ImageType;
  m_iPointerState      = StateReadyToCreate;
}

void MenuPreview::slotAddMovieObject ()
{
  setMouseCursor ( MouseCrossCursor );	// signals to be awaiting the next mouse click
  // which will then generate the rectangle for this text button.
  m_iCreateObjectStyle = MovieType;
  m_iPointerState      = StateReadyToCreate;
}

void MenuPreview::createAnimationData (QString &qsMenuName, long iMaxFrames)
{
  uint t, i, iCount=0;
  QFile     theFile;
  QFileInfo fileInfo;
  QString qsName;
  QString qsFileName, qsCommand, qsAnimation;
  MenuObject *pMainMenuObject = NULL;
  MenuObject *pMenuObject     = NULL;

  //for ( t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {                //ooo
  for ( t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ )  {  //xxx
    // First we check each object if m_qsAnimation is empty
    pMainMenuObject = m_listMenuObjects.at ( t );
    if ( ! pMainMenuObject )
      continue;
    for ( i=0; i<3; i++ )  {  // 0==MainObject / 1==ShadowObject / 2==OverlayObject
      if ( i == 0 )  {
        pMenuObject = pMainMenuObject;
        fileInfo.setFile ( pMainMenuObject->name ( ) );
        qsName = fileInfo.baseName ( );
      }
      else if ( i == 1 )  {
        pMenuObject = pMainMenuObject->shadow ( );
        qsName = "shadow";
      }
      else if ( i == 2 )  {
        pMenuObject = pMainMenuObject->overlay ( );
        if ( pMenuObject )  {
          ImageObject *p = (ImageObject *)pMenuObject;
          fileInfo.setFile ( p->fileName ( ) );
          qsName = fileInfo.baseName ( );
        }
      }
      if ( qsName.isEmpty ( ) )
           qsName = "Something";

      if ( ! pMenuObject || pMenuObject->animation ( ).isEmpty ( ) )
        continue;

      qsAnimation = pMenuObject->animation ( );
      // Next we check if the contents is a fileName (to the data file)
      if ( ( qsAnimation.length   ( )      < 1024 ) && 
           //( qsAnimation.contains ( "\n" ) <    2 )  ) {	//ooo
	   ( qsAnimation.count ( "\n" ) <    2 )  ) {		//xxx
        // Assume fileName
        fileInfo.setFile ( qsAnimation );
        if ( fileInfo.exists  ( ) )  {
          pMenuObject->loadAnimationData ( qsAnimation, iMaxFrames );		//oooo
          continue;
        }
      }

      qsFileName=QString( "%1/%2/%3/%4_%5").arg ( Global::qsTempPath ).arg ( Global::qsProjectName ).arg ( qsMenuName ).arg ( qsName ).arg ( ++iCount );
      if ( isAnimationData ( qsAnimation ) ) {
        // Finally we assume it is a script to generate the data ...
        //theFile.setName   ( qsFileName + QString ( ".dat" ) );	//ooo
	theFile.setFileName   ( qsFileName + QString ( ".dat" ) );	//xxx
        if ( theFile.open ( QIODevice::WriteOnly ) )  {
          QTextStream theStream ( &theFile );
          theStream << qsAnimation;
          theFile.close ( );
        }
      }
      else {
        // Finally we assume it is a script to generate the data ...
        //theFile.setName   ( qsFileName + QString ( ".sh" ) );		//ooo
	theFile.setFileName   ( qsFileName + QString ( ".sh" ) );	//xxx
        if ( theFile.open ( QIODevice::WriteOnly ) )	{
          QTextStream theStream ( &theFile );
          theStream << qsAnimation;
          theFile.close ( );
        }
        // next we change the mode to executable ...
        //chmod ( qsFileName + QString ( ".sh" ), 0777 );// S_IEXEC | S_IRUSR | S_IWRITE);					//ooo
        chmod ( QString("%1").arg(qsFileName + QString ( ".sh" )).toLatin1().data(), 0777 );// S_IEXEC | S_IRUSR | S_IWRITE);	//xxx
        qsCommand = QString ( "\"%1.sh\" 2>/dev/null > \"%2.dat\"" ).arg ( qsFileName ).arg ( qsFileName );
        //printf ( "%s::%s : %d <%s>\n", __FILE__, __FUNCTION__, __LINE__, qsCommand.ascii() );
        //if ( system ( qsCommand ) == -1 )			//ooo
	if ( system ( qsCommand.toLatin1().data() ) == -1 )	//xxx
          return;
      }
      qsFileName += QString ( ".dat" );
      // Check if the file has some size to it ...
      fileInfo.setFile  ( qsFileName );
      if ( fileInfo.size( ) < 1 )	{
        // TODO: write to the conversion log of this menu ...
        continue;
      }
      pMenuObject->loadAnimationData ( qsFileName, iMaxFrames );
    } // MainObjec / ShadowObject / OverlayObject
  }
}

bool MenuPreview::isAnimationData ( QString &qsAnimation )
{
  // Okay this is crude and bad but easy and fast ...
  //QStringList list = QStringList::split ( "\n", qsAnimation );	//ooo
  QStringList list = qsAnimation.split ( "\n" );			//xxx
  int t;
  QRegExp rx ( "^\\d*::" );

  for ( t=0; t<(int)list.count ( ); t++ ) {
    if ( list[t].length ( ) < 4 )
      continue;
    if ( list[t][0] == '#' )
      continue;
    //if ( list[t].find ( rx ) > -1 )	//ooo
    if ( list[t].indexOf ( rx ) > -1 ) 	//xxx
      return true;
    else
      return false;
  }

  return false;
}

MenuObject *MenuPreview::createFrameObject(bool bShowDialog)
{
  FrameObject *pFrameObject = new FrameObject ( this );
  pFrameObject->setRect        ( m_rectCurrentObject );
  pFrameObject->setFrameWidth  ( 4 );
  pFrameObject->setFrameColor  ( QColor ( START_FRAME_COLOR ) );
  if (bShowDialog)	{
    m_listMenuObjects.append ( pFrameObject );
    emit ( signalUpdateStructure ( ) );
    updatePixmap ( );
  }
  // Here we connect the signal to the slot ...
  connectStdSlots ( pFrameObject, true );
  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OBJECT, pFrameObject ) );

  return pFrameObject;	//ooo
}

MenuObject *MenuPreview::createTextObject ( bool bShowDialog )
{
  TextObject *pTextObject = NULL;
  if ( bShowDialog )  {
    DialogTextFont fontDialog ( this );
    fontDialog.setRect   (   m_rectCurrentObject  );
    if ( fontDialog.exec ( ) == QDialog::Rejected )
      return NULL;

    pTextObject    = new TextObject (  this  );
    QString qsText = fontDialog.getText (    );
    pTextObject->setText            ( qsText );
//    pTextObject->setRect            ( m_rectCurrentObject );
    pTextObject->setRect            ( fontDialog.getRect ( ) );
    pTextObject->setFont            ( fontDialog.getFont ( ) );
    pTextObject->setAnimation       ( fontDialog.getAnimation ( ) );
    pTextObject->setTextAlign       ( fontDialog.getTextAlign ( ) );
    pTextObject->setBackgroundColor ( fontDialog.getBackgroundColor ( ) );
    pTextObject->setForegroundColor ( fontDialog.getForegroundColor ( ) );
    pTextObject->modifiers ( )->fTransparency = fontDialog.getTransparency ( );	//oooo
    // Checks wether the user wants to fit the size of the button to the text or not.
    if ( fontDialog.getFit ( ) )  {
      QRect rect = pTextObject->rect ( );
      rect.setWidth  ( 1 );
      rect.setHeight ( 1 );
      pTextObject->setRect      ( rect  );
      pTextObject->setWordBreak ( false ); // allows to keep the line intact
    }
 
    m_listMenuObjects.append     ( pTextObject );

    emit ( signalUpdateStructure ( ) );

    QRect orig = pTextObject->rect ( );

    updatePixmap ( );

    pTextObject->update   ( );

    //if ( rect ( )  !=  orig )  {  // if the rect has changed ...  //ooo
    if ( pTextObject->rect ( )  !=  orig )  {  // if the rect has changed ...    //xxx
   
      updatePixmap        ( );

      pTextObject->update ( );
    }
    pTextObject->setWordBreak ( true );
  }
  else
    pTextObject = new TextObject ( this );

  // Here we connect the signal to the slot ...
  connectStdSlots ( pTextObject, true );
  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OBJECT, pTextObject ) );	//oooo
  return pTextObject;
}

MenuObject *MenuPreview::createImageObject(bool bShowDialog)
{
  ImageObject *pImageObject = NULL;

  if (bShowDialog)	{
    // The first step is to get the image name
    QString qsImageFilter = Global::pApp->getImageFilter ( ); //"*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP";

    //	  QString qsImageName = QFileDialog::getOpenFileName(Global::qsCurrentPath, tr("Image Files (*.jpg *.jpeg *.png *.xbm *.bmp *.JPG *.JPEG *.PNG *.XBM *.BMP)"), this,
    //QString qsImageName = FilePreviewDialog::getOpenFileName ( NULL, Global::qsCurrentPath, QString ("Images ( ") + qsImageFilter + QString (" );;All ( * )"), this, 	//ooo
        //tr("Select image"), tr("Select a image object."));	//ooo
    QString qsImageName = QFileDialog::getOpenFileName ( this, tr("Select image"), Global::qsCurrentPath, tr ("Images ( %1 );;All ( * )").arg(qsImageFilter));	//xxx
    if (qsImageName.isNull())
      return NULL;

    QFileInfo fileInfo  (qsImageName);
    //Global::qsCurrentPath = fileInfo.dirPath ( TRUE );	//ooo
    Global::qsCurrentPath = fileInfo.path ( );		//xxx

    // Here we create the Pixmap in the right scale
    QImage theImage (qsImageName);
    int iWidth, iHeight;
    float fZoom;
    // Keep aspect ratio
    iWidth  = m_rectCurrentObject.width();
    iHeight = (int)((float)theImage.height()/theImage.width()*m_rectCurrentObject.width());
    fZoom = ((float)m_rectCurrentObject.width() / theImage.width());
    // Convert the image and generate the Pixmap
    //theImage = theImage.smoothScale (iWidth, iHeight);	//, QImage::ScaleMin);								//ooo
    theImage = theImage.scaled (iWidth, iHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);	//, Qt::KeepAspectRatio);	//xxx
    // And here we adopt the rect the user has drawn.
    m_rectCurrentObject.setHeight ( iHeight );
    // Finally we create the ImageObject
    pImageObject = new ImageObject( this );
    pImageObject->setRect  ( m_rectCurrentObject );
    pImageObject->setZoom  ( fZoom       );  // We want to say Zoom = 1.0 even if the original size is already zoomed.
    pImageObject->setFile  ( qsImageName );
    pImageObject->setImage ( theImage    );

    m_listMenuObjects.append ( pImageObject );
    emit (signalUpdateStructure ( ) );
    updatePixmap ( );
  }
  else
    pImageObject = new ImageObject (this);

  // And last we connect the signals
  connectStdSlots ( pImageObject, true );
  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OBJECT, pImageObject ) );

  return pImageObject;
}

MenuObject *MenuPreview::createMovieObject(bool bShowDialog)
{
  MovieObject *pMovieObject = NULL;

  if (bShowDialog)	{
    QPixmap thePixmap;
    // The first step is to get the movie name
    // to speed things up we create a dummyMediaInfo first
    MediaInfo *pMediaInfo = (MediaInfo *) new DummyInfo ();
    QString qsFilter = tr ("Movie Files (%1)").arg(pMediaInfo->getExtensions (true));

    //QString qsMovieName = QFileDialog::getOpenFileName(Global::qsCurrentPath, qsFilter, this, 
    //	tr("Select movie"), tr("Select a movie object."));
    //QString qsMovieName = FilePreviewDialog::getOpenFileName ( NULL, Global::qsCurrentPath, QString ("Movies ( ") + qsFilter + QString (" );;All ( * )"), this, tr ( "Select image" ), tr ( "Select a image object." ) );	//ooo
    QString qsMovieName = QFileDialog::getOpenFileName ( this, tr ( "Select image" ), Global::qsCurrentPath, QString ("Movies ( %1 );;All ( * )").arg(qsFilter));								//xxx
    if ( qsMovieName.isNull ( ) )
      return NULL;
    QCursor myCursor (Qt::WaitCursor);
    setCursor (myCursor);
    // Here we need to get a screenshot, thus the dummy won't do !!!
    delete pMediaInfo;

    QFileInfo fileInfo  (qsMovieName);
    //Global::qsCurrentPath = fileInfo.dirPath ( TRUE );	//ooo
    Global::qsCurrentPath = fileInfo.path ( );			//xxx

    // Here we create the Pixmap in the right scale
    //QImage theImage = QImage().fromMimeSource("please_wait.jpg");	//ooo
    QImage theImage = QImage ( ":/images/please_wait.jpg" );		//xxx

    int   iHeight;
    QSize theSize = m_rectCurrentObject.size();
    //theImage = theImage.smoothScale (theSize, Qt::IgnoreAspectRatio);				//ooo
    theImage = theImage.scaled (theSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);	//xxx
    // Keep aspect ratio
    iHeight = (int)((float)theImage.height()/theImage.width()*m_rectCurrentObject.width());				//ooo
    //iHeight = static_cast<int>(static_cast<float>(theImage.height())/theImage.width()*m_rectCurrentObject.width());	//xxx
  
    thePixmap.convertFromImage(theImage);
    // Clear memory ...
    theImage = QImage ();
    // And here we adopt the rect the user has drawn.
    m_rectCurrentObject.setHeight(iHeight);
    // Finally we create the ImageObject
    pMovieObject = new MovieObject (this);
    pMovieObject->setRect     (m_rectCurrentObject);
    pMovieObject->setZoom     (1.0f); // We want to say Zoom = 1.0 even if the original size is already zoomed.
    pMovieObject->setFile     (qsMovieName);
    pMovieObject->setPixmap   (thePixmap);
    pMovieObject->setMovieSize(theSize);

    m_listMenuObjects.append  ( pMovieObject );
    emit ( signalUpdateStructure ( ) );
    updatePixmap ( );

    // Next we register to receive a preview image of the movie file ...
    MediaCreator::registerWithMediaScanner (pMovieObject, qsMovieName, pMovieObject->previewImage());

    // We need this to obtain a pointer to the assoziated DVDMenu ...
    emit (signalCreatedMovieObject (pMovieObject, true)); // inclusive render request ...

    //myCursor = QCursor(QCursor::ArrowCursor);		//ooo
    myCursor = QCursor(Qt::ArrowCursor);		//xxx
    setCursor (myCursor);
  }
  else	{
    pMovieObject = new MovieObject (this);
    // We need this to obtain a pointer to the assoziated DVDMenu ... But no render request ...
    emit (signalCreatedMovieObject (pMovieObject, false));
  }

  // And last we connect the signals
  connectStdSlots ( pMovieObject, true );
  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OBJECT, pMovieObject ) );
  return pMovieObject;	//oooo
}

MenuObject *MenuPreview::createButtonObject(bool bShowDialog)
{
  ButtonObject *pButtonObject = new ButtonObject (this);

  if (bShowDialog)	{
    m_listMenuObjects.append (pButtonObject);
    emit ( signalUpdateStructure ( ) );
    updatePixmap ( );
  }

  // And finally we connect everything ...
  connectStdSlots ( pButtonObject, true );
  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OBJECT, pButtonObject ) );	//oooo

  return pButtonObject;
}

void MenuPreview::setVisibleRegion (bool bVisibleOn)
{
	m_bDrawVisibleRegion = bVisibleOn;
	updatePixmap ();
}

void MenuPreview::slotDefineAsButton ( MenuObject *pTheObject )
{
  defineAsButton ( pTheObject, true );	//oooo
}

ButtonObject *MenuPreview::defineAsButton ( MenuObject *pTheObject, bool bShowDialog )
{
  if ( ! pTheObject )
    return NULL;
  // This is interesting ...
  // first we need to create a ButtonObject,
  // which we then furnish with three copies of this TextObject
  // Namely: Normal, Selected, and Highlighted ...
  //m_listMenuObjects.remove ( pTheObject );	//ooo
  m_listMenuObjects.removeOne ( pTheObject );	//xxx
  pTheObject->resetStructureItem ( );

  ButtonObject *pButtonObject = new ButtonObject ( this );
  // Now we generate two copies ...
  MenuObject *pNewSelected, *pNewHighlighted;

  Rgba colorTransparent ( TRANSPARENT_COLOR       );
  Rgba colorHighlighted ( START_HIGHLIGHTED_COLOR );
  Rgba colorSelected    ( START_SELECTED_COLOR    );
  Rgba colorSpare       ( TRANSPARENT_COLOR       );
  if ( m_pPgcColors )   {
    colorTransparent = m_pPgcColors[0];
    colorHighlighted = m_pPgcColors[1];
    colorSelected    = m_pPgcColors[2];
    colorSpare       = m_pPgcColors[3];
  }

  pNewSelected = pNewHighlighted = NULL;
  if ( pTheObject->objectType ( ) == TEXT_OBJECT )  {
    pNewSelected    = pTheObject->clone ( );
    pNewHighlighted = pTheObject->clone ( );

    // Only the 'normal' object should have a shadow
    pNewSelected   ->setShadow ( NULL );
    pNewHighlighted->setShadow ( NULL );
    // Next we give some color differences for the different states (TextObjects)
    ((TextObject *)(pNewSelected))   ->setForegroundColor ( colorSelected    );
    ((TextObject *)(pNewHighlighted))->setForegroundColor ( colorHighlighted );
    // Also here we ensure that the Textbackground for selected / highlighted is always transparent
    // The background color has already been put onto the actual background image.
    ((TextObject *)(pNewSelected))   ->setBackgroundColor ( colorTransparent );
    ((TextObject *)(pNewHighlighted))->setBackgroundColor ( colorTransparent );
    // The layers should not be aliased ...
    ((TextObject *)(pNewSelected))   ->setStyleStrategy   ( QFont::NoAntialias );
    ((TextObject *)(pNewHighlighted))->setStyleStrategy   ( QFont::NoAntialias );
    // And finally we do not want transparency (yet)
    ((TextObject *)(pNewSelected))   ->modifiers()->fTransparency = 0.0f;
    ((TextObject *)(pNewHighlighted))->modifiers()->fTransparency = 0.0f;
  }
  else	{// All but Text Objects get a frame for starters
    // Okay we want 1:1 for FrameObjects
    if (pTheObject->objectType() == FRAME_OBJECT )  {
      pNewSelected    = pTheObject->clone();
      pNewHighlighted = pTheObject->clone();
    }
    else	{  // for ImageObject, and MovieObject a standard frame   
      pNewSelected    = new FrameObject;
      pNewHighlighted = new FrameObject;
      pNewSelected->setRect        (  pTheObject->rect      ( ) );
      pNewHighlighted->setRect     (  pTheObject->rect      ( ) );
      pNewSelected->setModifiers   ( *pTheObject->modifiers ( ) );
      pNewHighlighted->setModifiers( *pTheObject->modifiers ( ) );
      ((FrameObject *)(pNewSelected))->setFrameWidth       ( 10 );
      ((FrameObject *)(pNewHighlighted))->setFrameWidth    ( 10 );
    }
    // Next we give some color to the different states
    ((FrameObject *)(pNewSelected))   ->setFrameColor ( colorSelected    );
    ((FrameObject *)(pNewHighlighted))->setFrameColor ( colorHighlighted );
  }	//oooo

  pButtonObject->appendNormal      ( pTheObject      );
  pButtonObject->appendSelected    ( pNewSelected    );		//oooo
  pButtonObject->appendHighlighted ( pNewHighlighted );		//oooo

  pButtonObject->setName ( newButtonName ( ) );

  // Next we should give the button the same attributes as the NormalState ...
  // rect and boundingRect are taken care of in drawContent
  pButtonObject->setModifiers ( *pTheObject->modifiers ( ) );

  // and here we append the button object as a new item in the MenuObject list.
  m_listMenuObjects.append ( pButtonObject );
  m_pActiveObject = NULL;

  if ( bShowDialog ) {
    // Create the buttonDialog, so the user can change things around ...
    slotCreateButtonDialog ( pButtonObject );

    if ( m_pDialogButton )
         m_pDialogButton->setButtonCreation ( true );	//oooo
  }
  // And finally we connect everything ...
  connectStdSlots ( pButtonObject, true );

  return pButtonObject;
}

void MenuPreview::slotCreateButtonDialog ( ButtonObject *pButtonObject )
{
  uint t;
  // Here we generate a list of all Buttons in the current Menu. This is needed in the ButtonDialog
  // to  offer a list of possible targets.
  QStringList listMenuButtons;
  ButtonObject tempButton;

  //for (t=0;t<(uint)m_listMenuObjects.count();t++)	{               //ooo
  for (t=0;t<static_cast<uint> ( m_listMenuObjects.count() );t++)	{ //xxx
    if (m_listMenuObjects.at(t)->objectType() == tempButton.objectType())	{
      listMenuButtons.append(m_listMenuObjects.at(t)->name());
    }
  }

  // Second step is to invoke the ButtonDialog ...
  m_pDialogButton = new DialogButton ( this );
  m_pDialogButton->initMe ( pButtonObject, this );
  m_pDialogButton->setMenuButtons   ( listMenuButtons );
  m_pDialogButton->show ( );
  connect ( m_pDialogButton, SIGNAL ( signalUpdateStructure ( ) ), this, SLOT ( slotUpdateStructure       ( ) ) );
  connect ( m_pDialogButton, SIGNAL ( signalUpdatePixmap    ( ) ), this, SLOT ( slotUpdatePixmap          ( ) ) );
  connect ( m_pDialogButton, SIGNAL ( destroyed             ( ) ), this, SLOT ( slotDestroyedButtonDialog ( ) ) );

  m_pActiveObject = NULL;
  // Go to DVDAuthor::slotRerquestSourceFiles.
  // Will return to respondSourceFiles() ...
  emit ( signalRequestSourceFiles ( ) );
  // Will return to respondSubMenus() ...
  emit ( signalRequestSubMenus ( ) );
  emit ( signalUpdateStructure ( ) );	//oooo
}

void MenuPreview::respondSourceFiles (QList<SourceFileEntry *>listSourceFileEntries)		//oxx
{
	// If m_pActiveObject is set this means that we called for the SourceFileList from
	// readObject ...
	if (m_pActiveObject)	{
		ButtonObject *pCreatedButton = (ButtonObject *)m_pActiveObject;
		// Here we connect the Button, which was created in readObject
		uint t;
		//for (t=0;t<(uint)listSourceFileEntries.count();t++)	{	              //ooo
                for (t=0;t<static_cast<uint> ( listSourceFileEntries.count() );t++)	{	//xxx
//			QString qs = pCreatedButton->sourceDisplayName();
// printf ("MenuPreview::respondSourceFiles <%s>\n", (const char *)pCreatedButton->sourceDisplayName());
			if (listSourceFileEntries[t]->qsDisplayName == pCreatedButton->sourceDisplayName())	{
				pCreatedButton->setSourceFileEntry(listSourceFileEntries[t]);
				break;
			}
		}
		// And set this one to NULL
		m_pActiveObject = NULL;
	}
	// First check if this was triggered from slotCreateButtonDialog
	else if ( m_pDialogButton )
	 	  m_pDialogButton->setSourceFiles(listSourceFileEntries);
}

void MenuPreview::respondSubMenus (QStringList listSubMenus)
{
	// Okay, this is the return of the above emit(signalRequestSubMenus()) - call
	// Coming back from DVDMenu::slotRequestSubMenus()
	if (!m_pDialogButton)
		return;
	m_pDialogButton->setSubMenus(listSubMenus);
}

void MenuPreview::slotDestroyedButtonDialog()
{
  // The Button dialog was destroyed. So we should set the pointer to NULL;
  m_pDialogButton = NULL;
}

void MenuPreview::slotUpdateStructure ( )
{
  // simply passes this call on to CDVDMenu ...
  emit ( signalUpdateStructure ( ) );
}

void MenuPreview::slotUpdatePixmap ( )
{
  updatePixmap ( );
}

void MenuPreview::slotUndoObject ( MenuObject *pObject )
{
  printf ( "%s::%s::%d > pObject<%p>\n", __FILE__, __FUNCTION__, __LINE__, pObject );
}

void MenuPreview::slotDeleteObject ( MenuObject *pObject )
{
  // simply do the same as the base class but also update the structureView ...
  ButtonPreview::slotDeleteObject ( pObject );

  updatePixmap ( );
  emit ( signalUpdateStructure ( ) );
}

void MenuPreview::slotAddOverlay ( MenuObject *pObject )
{  
  QPixmap thePixmap ( m_backgroundPixmap );
  if ( m_backgroundPixmap.isNull ( ) )
    thePixmap = QPixmap ( 720, 480 );

  // Here we create the background without the current object
  // (we want to move that one around right ?)
  QPainter thePainter(&thePixmap);
  if ( pObject == &m_selectionObject ) {
    //for ( uint t=0; t<m_listMenuObjects.count ( ); t++ )  {                       //ooo
    for ( uint t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ )  {   //xxx
      if ( ! m_selectionObject.contains ( m_listMenuObjects.at ( t ) ) )
             m_listMenuObjects.at ( t )->drawContents ( &thePainter );
    }
  }
  else {
    //for (  uint t=0; t<m_listMenuObjects.count ( ); t++ )  {                          //ooo
    for (  uint t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ )  {      //xxx
      if ( m_listMenuObjects.at ( t ) != pObject )
           m_listMenuObjects.at ( t )->drawContents ( &thePainter );
    }
  }

  UndoObject *pUndo = new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_OVERLAY, pObject, pObject->overlay ( ) );
  /*DialogOverlay dialog ( this );
  dialog.initMe        ( pObject,    &thePixmap );
  if ( dialog.exec     ( ) == QDialog::Rejected )  {
    delete pUndo;
    return;
  }*/   //oooo
  undoBuffer ( )->push ( pUndo );

  updatePixmap ( );
  Global::pApp->slotUpdateStructure ( );
}

void MenuPreview::slotAddShadow ( MenuObject *pObject )
{
  QPixmap thePixmap ( m_backgroundPixmap );
  if ( m_backgroundPixmap.isNull ( ) )
    thePixmap = QPixmap ( 720, 480 );

  // Here we create the background without the current object
  // (we want to move that one around right ?)
  QPainter thePainter ( &thePixmap );
  if ( pObject == &m_selectionObject ) {
    for ( uint t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {
      if ( ! m_selectionObject.contains ( m_listMenuObjects.at ( t ) ) )
             m_listMenuObjects.at ( t )->drawContents ( &thePainter );
    }
  }
  else {
    for (  uint t=0; t<(uint)m_listMenuObjects.count ( ); t++ )  {
      if ( m_listMenuObjects.at ( t ) != pObject )
           m_listMenuObjects.at ( t )->drawContents ( &thePainter );
    }
  }

  UndoObject *pUndo = new DVDMenuUndoObject ( DVDMenuUndoObject::NEW_SHADOW, pObject, pObject->shadow ( ) );
  DialogShadow dialog ( this );
  dialog.initMe       ( pObject,    &thePixmap );
  if ( dialog.exec    ( ) == QDialog::Rejected )  {
    delete pUndo;
    return;
  }
  undoBuffer ( )->push ( pUndo );

  updatePixmap ( );
  Global::pApp->slotUpdateStructure ( );
}

void MenuPreview::slotModifyObject ( MenuObject *pObject )
{ 
  QPixmap thePixmap ( m_backgroundPixmap );
  if ( m_backgroundPixmap.isNull ( ) )
    thePixmap = QPixmap ( 720, 480 );

  // Here we create the background without the current object
  // (we want to move that one around right ?)
  QPainter thePainter(&thePixmap);
  //for ( uint t=0; t<m_listMenuObjects.count ( ); t++ )  {                         //ooo
  for ( uint t=0; t<static_cast<uint> ( m_listMenuObjects.count ( ) ); t++ )  {     //xxx
    if ( m_listMenuObjects.at ( t ) != pObject)
         m_listMenuObjects.at ( t )->drawContents ( &thePainter );
  }

  undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::EDIT_OBJECT, pObject->shadow ( ) ) );

  ImageObject *pImageObject = (ImageObject *)pObject;
  DialogImage2 dialog ( this );
  dialog.initMe       ( pImageObject, &thePixmap );
  if (   dialog.exec  ( )  ==  QDialog::Rejected )
    return;

  if ( ! pObject->shadow ( ) )  {
    // If the user deletes the Shadow, then we should change EDIT_OBJECT to DELETED_OBJECT
    DVDMenuUndoObject *pUndo = (DVDMenuUndoObject *) undoBuffer ( )->last ( );
    if ( pUndo )
         pUndo->changeType ( DVDMenuUndoObject::DELETED_SHADOW );
  }

  updatePixmap ( );
}

void MenuPreview::slotUnbutton ( ButtonObject *pButton )
{
  uint t=0;
  MenuObject *pObject = NULL;
  // First we disconnect the button from this MenuPreview
  //disconnect ( pButton );
  //m_listMenuObjects.remove    ( pButton );	//ooo
  m_listMenuObjects.removeOne    ( pButton );	//xxx
  pButton->resetStructureItem ( );

  for ( t=0; t<pButton->getNormalCount ( ); t++ )  {  
    // The we grab the Normal-Object from the ButtonObject
    pObject =  pButton->getNormal ( t );
    pButton->removeNormal   ( pObject );
    // And add the object to this MenuPreview.
    pObject->setCurrentMousePos ( pButton->currentMousePos ( ) );
    m_listMenuObjects.append( pObject );
    pObject->disconnect ( );
    // The StructureItem's parent is currently the ButtonObject. So deleting the button would crash the app
    pObject->resetStructureItem   ( );
    connectStdSlots ( pObject, true );
  }
  // Finally we can delete the button object.
//  delete pButton;
  if ( pButton && pObject )  {
       undoBuffer ( )->push ( new DVDMenuUndoObject ( DVDMenuUndoObject::UNBUTTON, pButton, pObject ) );
  }	//oooo

  emit ( signalUpdateStructure ( ) );
}

void MenuPreview::slotEmitDeleteMe()
{
	emit ( signalDeleteMe ( ) );
}

QString &MenuPreview::newButtonName()
{
  // This function searches the existing Buttons for it's names and picks the first name which is not in use.
  uint t, iButtonNr;
  bool bContinue = true;
  iButtonNr = 2;
  ButtonObject tempButton;
  static QString qsButtonName;
  qsButtonName = QString ("Button 1");
  while (bContinue)	{
    bContinue = false;	// default is the assumption we won't find the same name, thus exit after ...
    //for (t=0;t<(uint)m_listMenuObjects.count();t++)	{                 //ooo
    for (t=0;t<static_cast<uint> ( m_listMenuObjects.count() );t++)	{   //xxx
      if (m_listMenuObjects.at(t)->objectType() == tempButton.objectType())	{
	if (m_listMenuObjects.at(t)->name () == qsButtonName)	{
	  qsButtonName = QString ("Button %1").arg(iButtonNr++);
	  bContinue = true;	// crap, got to do it again ...
	  break;
	}
      }
    }
  }

  return qsButtonName;
}

void MenuPreview::connectStdSlots (MenuObject *pObject, bool bAddExtraSlots )
{
  //printf ( "%s::%s::%d count<%d> obj<%p>\n", __FILE__, __FUNCTION__, __LINE__, m_listMenuObjects.count ( ), pObject );
  connect (pObject, SIGNAL ( signalUpdateStructure( ) ),                this, SLOT ( slotUpdateStructure( ) ) );
  connect (pObject, SIGNAL ( signalUpdatePixmap   ( ) ),                this, SLOT ( slotUpdatePixmap   ( ) ) );
  connect (pObject, SIGNAL ( signalDefineAsButton ( MenuObject *   ) ), this, SLOT ( slotDefineAsButton ( MenuObject *   ) ) );
  connect (pObject, SIGNAL ( signalDeleteMe       ( MenuObject *   ) ), this, SLOT ( slotDeleteObject   ( MenuObject *   ) ) );
  connect (pObject, SIGNAL ( signalUndoMe         ( MenuObject *   ) ), this, SLOT ( slotUndoObject     ( MenuObject *   ) ) );
  connect (pObject, SIGNAL ( signalShadowMe       ( MenuObject *   ) ), this, SLOT ( slotAddShadow      ( MenuObject *   ) ) );
  connect (pObject, SIGNAL ( signalOverlayMe      ( MenuObject *   ) ), this, SLOT ( slotAddOverlay     ( MenuObject *   ) ) );
  connect (pObject, SIGNAL ( signalMoveOnStack    ( MenuObject *,int) ),this, SLOT ( slotMoveOnStack    ( MenuObject *, int ) ) );
  connect (pObject, SIGNAL ( signalAnimateMe      ( QString &, int, QString ) ), this, SLOT ( slotAnimateObject  ( QString &, int, QString ) ) );
  if ( bAddExtraSlots ) {
    uint i;
    if ( pObject->objectType ( ) == BUTTON_OBJECT )  {
      ButtonObject *pButton = (ButtonObject *)pObject;
      for ( i=0; i < pButton->getNormalCount ( ) ; i++ )  {
	//MenuObject *pObject = pButton->getNormal ( (int)i );                 //ooo
        MenuObject *pObject = pButton->getNormal ( static_cast<int> ( i ) );    //xxx
	pObject->disconnect ( );
	connectStdSlots ( pObject, false );
      }
      for ( i=0; i<pButton->getSelectedCount ( ) ; i++ )
	connect (  pButton->getSelected     ( i ), SIGNAL ( signalDeleteMe (MenuObject *) ), this, SLOT(slotDeleteObject (MenuObject *) ) );
      for ( i=0; i<pButton->getHighlightedCount ( ) ; i++ )
	connect (  pButton->getHighlighted  ( i ), SIGNAL ( signalDeleteMe (MenuObject *) ), this, SLOT(slotDeleteObject (MenuObject *) ) );

      connect ( pButton, SIGNAL(signalUnbuttonMe(ButtonObject *)), this, SLOT(slotUnbutton(ButtonObject *)));
      connect ( pButton, SIGNAL(signalCreateButtonDialog(ButtonObject *)), this, SLOT(slotCreateButtonDialog(ButtonObject *)));
    }
    else if ( ( pObject->objectType ( ) == IMAGE_OBJECT ) ||
              ( pObject->objectType ( ) == MOVIE_OBJECT ) )
      connect ( pObject, SIGNAL ( signalModifyMe (MenuObject *) ), this, SLOT ( slotModifyObject (MenuObject *) ) );
  }
}
