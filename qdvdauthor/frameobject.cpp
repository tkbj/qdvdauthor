/***************************************************************************
    frameobject.cpp
                             -------------------
                             
    FrameObject class                         
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class is the encapsulation of the FrameObject.
   
   A FrameObject is a visible frame on a DVD menu.
    
****************************************************************************/

#include <QPainter>
#include <QMatrix>
#include <QPixmap>
//#include <q3popupmenu.h>	//ooo
#include <QLineEdit>
//Added by qt3to4:
#include <QMouseEvent>

#include "messagebox.h"
#include "global.h"
#include "xml_dvd.h"
#include "menuobject.h"
#include "frameobject.h"
#include "dialogframe.h"
#include "dialogmatrix.h"	//ooo
#include "structuretoolbar.h"

#include "qdvdauthor.h"
#include "dvdmenuundoobject.h"

FrameObject::FrameData::FrameData ( )
  : MenuObject::Data ( )
{
  m_iDataType    = Data::FRAME;
  m_colorFrame   = QColor  ( START_FRAME_COLOR );
  m_iFrameWidth  = 4;
  m_iFrameJoin   = Qt::MiterJoin;
  m_iFrameStyle  = Qt::SolidLine;
}

FrameObject::FrameData::~FrameData ( )
{
}

MenuObject::Data &FrameObject::FrameData::operator = ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  MenuObject::Data::operator= ( theOther );
  if ( m_iDataType   == theOther.m_iDataType )  {
    FrameData *pOther = (FrameData *)&theOther;
    m_colorFrame      = pOther->m_colorFrame;
    m_iFrameWidth     = pOther->m_iFrameWidth;
    m_iFrameJoin      = pOther->m_iFrameJoin;
    m_iFrameStyle     = pOther->m_iFrameStyle;
  }

  return *this;
}

bool FrameObject::FrameData::operator == ( Data &theOther )
{
  // At this point we are sure both are acutally ImageManipulator objects.
  bool bReturn = ( MenuObject::Data::operator == ( theOther ) );
  if ( ! bReturn )
    return false;

  if ( m_iDataType  == theOther.m_iDataType )  {
    FrameData *pOther = (FrameData *)&theOther;
    bReturn = ( ( m_colorFrame  == pOther->m_colorFrame  ) &&
                ( m_iFrameWidth == pOther->m_iFrameWidth ) &&
                ( m_iFrameJoin  == pOther->m_iFrameJoin  ) &&
                ( m_iFrameStyle == pOther->m_iFrameStyle ) );
  }
  return bReturn;
}

MenuObject::Data *FrameObject::FrameData::clone ( )
{
  FrameData *pData = new FrameData;
  *pData  = *this;
  return pData;
}

FrameObject::FrameObject (QWidget *pParent)
  : MenuObject (pParent)
{
  if ( m_pData )
    delete m_pData;        // part of the base class
  m_pData = new FrameData; // will be deleted in base class

  m_qsObjectType = QString ( FRAME_OBJECT );
}

FrameObject::~FrameObject ()
{
}

FrameObject::FrameData *FrameObject::frameData ( )
{
  return (FrameData *)m_pData;
}

void FrameObject::setFrameWidth(int iFrameWidth)
{
  frameData ( )->m_iFrameWidth = iFrameWidth;
}

void FrameObject::setFrameJoin(int iFrameJoin)
{
  frameData ( )->m_iFrameJoin = iFrameJoin;
}

void FrameObject::setFrameStyle(int iFrameStyle)
{
  frameData ( )->m_iFrameStyle = iFrameStyle;
}

void FrameObject::replaceColor ( Rgba theColor, Rgba oldColor )
{
  if ( color ( ) == oldColor )
    setFrameColor ( theColor );
}

void FrameObject::setFrameColor ( Rgba colorFrame )
{
  frameData ( )->m_colorFrame = colorFrame;
}

int FrameObject::width ()
{
  return frameData ( )->m_iFrameWidth;
}

Rgba FrameObject::color ()
{
  return frameData ( )->m_colorFrame;
}

int FrameObject::style ()
{
  return frameData ( )->m_iFrameStyle;
}

int FrameObject::join ()
{
  return frameData ( )->m_iFrameJoin;
}

/*void FrameObject::paintEvent( QPaintEvent* event )    //xxxx
{
  //QLabel::paintEvent( event );  
  QPainter painter;
  drawContents( &painter ); // ... additional drawing  
}*/

void FrameObject::drawContents (QPainter *painter)
{
    if ( m_pShadow && ! m_bDrawRect )
	     m_pShadow->drawContents ( painter );
	if ( frameData ( )->m_colorFrame.alpha ( ) == 0 )
	  return;

	QMatrix theMatrix;
	// Here we calculate the center point of gravity (rotation)
	QPoint centerPos;
	//centerPos.setX (rect().x() + (int)((float)rect().width () / 2.0));                       //ooo
    centerPos.setX (rect().x() + static_cast<int>(static_cast<float>(rect().width ()) / 2.0));  //xxx
	//centerPos.setY (rect().y() + (int)((float)rect().height() / 2.0));                       //ooo
    centerPos.setY (rect().y() + static_cast<int>(static_cast<float>(rect().height()) / 2.0));  //xxx
	// Here we define the cenetered rect.
	//QRect theRect ((int)-(rect().width()/ 2.0), (int)-(rect().height()/2.0), rect().width(), rect().height());                           //ooo
    QRect theRect (static_cast<int>(-(rect().width()/ 2.0)), static_cast<int>(-(rect().height()/2.0)), rect().width(), rect().height());    //xxx

	//QPen thePen (frameData ( )->m_colorFrame.rgb ( ), frameData ( )->m_iFrameWidth, (Qt::PenStyle)frameData ( )->m_iFrameStyle, Qt::FlatCap, (Qt::PenJoinStyle)frameData ( )->m_iFrameJoin);		//ooo
	QPen thePen (QBrush(QColor(frameData ( )->m_colorFrame.rgb())), frameData ( )->m_iFrameWidth, (Qt::PenStyle)frameData ( )->m_iFrameStyle, Qt::FlatCap, (Qt::PenJoinStyle)frameData ( )->m_iFrameJoin);	//xxx
	// Set the PEN
	painter->setPen(thePen);
	// and make sure no Brush is used ...
	painter->setBrush(Qt::NoBrush);

	// First we translate to the appropriate location,
	//theMatrix.translate ((double)centerPos.x(), (double)centerPos.y());                          //ooo
    theMatrix.translate (static_cast<double>(centerPos.x()), static_cast<double>(centerPos.y()));   //xxx
    
	//. then we apply the other modifications ...
	theMatrix.scale  (modifiers()->fScaleX, modifiers()->fScaleY);
	theMatrix.rotate (modifiers()->fRotate);
	theMatrix.shear  (modifiers()->fShearX, modifiers()->fShearY);
	
	// Here we draw the rect that encompasses the button (think of rotated)
	m_boundingRect = theMatrix.mapRect(theRect);
    
	if (m_bDrawRect) //ooo
	  drawActiveFrame ( painter );

	painter->setWorldMatrix(theMatrix);

	painter->drawRect(theRect);
	theMatrix.reset();
	painter->setWorldMatrix(theMatrix);
}

void FrameObject::drawContents (QPainter *pPainter, int iRenderFrameNumber, int)
{
  // Next is to set the AnimationAttributes
  for ( uint t=0; t<(uint)m_listAnimationAttributes.count ( ); t++ )
    m_listAnimationAttributes[t]->setValue(iRenderFrameNumber);

  // If start stop have been set then we check the range
  if ( ( ( iRenderFrameNumber < modifiers ( )->iStartFrame ) && ( modifiers ( )->iStartFrame != -1 ) ) || 
       ( ( iRenderFrameNumber > modifiers ( )->iStopFrame  ) && ( modifiers ( )->iStopFrame  != -1 ) ) ) 
    return;

  drawContents ( pPainter );
}

bool FrameObject::mousePressEvent (QMouseEvent *pEvent)
{   
	m_currentMousePos = pEvent->pos();
	if (pEvent->button() == Qt::RightButton)	{
		QPoint globalPos = pEvent->globalPos();
		return createContextMenu ( globalPos );
	}
	else
		m_bDrawRect = true;
	return false;
}

bool FrameObject::mouseReleaseEvent (QMouseEvent *)
{
	m_bDrawRect = false;
	emit (signalUpdatePixmap());
	emit (signalUpdateStructure());
	return false;
}

bool FrameObject::mouseDoubleClickEvent (QMouseEvent *)
{
  QString qsAnimation;
  DialogFrame frameDialog ( m_pParent );

  frameDialog.initMe ( this );

  if ( frameDialog.exec ( ) == QDialog::Rejected )
    return false;

  DVDMenuUndoObject *pUndo = new DVDMenuUndoObject ( this, MenuObject::StateUnknown );
  setFrameWidth ( frameDialog.frameWidth ( ) );
  setFrameStyle ( frameDialog.style      ( ) );
  setFrameJoin  ( frameDialog.join       ( ) );
  qsAnimation =   frameDialog.animation  ( );
  setAnimation  ( qsAnimation );
  if ( color ( ) != frameDialog.color ( ) )  {
    // This emit signal is only caught when in ButtonDialog and the Selected, or Highlighted mask 
    // was changed (I.e. This Object is in one of those two frames).
    // Otherwise the signal is ignored.
    emit ( signalMaskColorChanged ( frameDialog.color ( ), color ( ) ) );
    // P.s. replaceColor (QColor, QColor) of all ButtonObjects will be called, inclusive this object)
  }
  setFrameColor ( frameDialog.color ( ) );
  int x, y, width, height;
  x      = frameDialog.m_pEditX->text ( ).toInt ( );
  y      = frameDialog.m_pEditY->text ( ).toInt ( );
  width  = frameDialog.m_pEditWidth->text  ( ).toInt ( );
  height = frameDialog.m_pEditHeight->text ( ).toInt ( );
  QRect newRect ( x, y, width, height );
  setRect ( newRect );
  emit    ( signalUpdatePixmap ( ) );

  if ( pUndo->hasChanged ( ) )  {
    UndoBuffer *pBuffer = Global::pApp->getUndoBuffer ( );
    if ( pBuffer )
         pBuffer->push ( pUndo );
  }
  else
    delete pUndo;

  return false;
}

bool FrameObject::readProjectFile  (QDomNode &theNode)
{
	// Okay, here we retain the stored data from the xml file.
	QDomElement theElement = theNode.toElement();
	QDomAttr attribute;

	attribute = theElement.attributeNode ( FRAME_OBJECT_COLOR );
	if (!attribute.isNull())
		frameData ( )->m_colorFrame.fromString ( attribute.value ( ) );

	attribute = theElement.attributeNode ( FRAME_OBJECT_WIDTH );
	if (!attribute.isNull())
		frameData ( )->m_iFrameWidth = attribute.value().toInt();

	attribute = theElement.attributeNode ( FRAME_OBJECT_STYLE);
	if (!attribute.isNull())
		frameData ( )->m_iFrameStyle = attribute.value().toInt();

	attribute = theElement.attributeNode ( FRAME_OBJECT_JOIN );
	if (!attribute.isNull())
		frameData ( )->m_iFrameJoin = attribute.value().toInt();

	MenuObject::readProjectFile( theNode );
	return true;
}

// <...>
// <FrameObject Color="#00FF00" Width="2" Style="1" Join="2">
//    <MenuObject>
//        <Modifiers>
//        </Modifiers>
//    </MenuObect>
// </FrameObject>
bool FrameObject::writeProjectFile (QDomElement &theElement)
{
	// First check if this node holds any information at all ...
//printf ("FrameObject::writeProjectFile <%s><%s>\n",(const char *)theElement.tagName(), (const char *)name());
	QDomDocument xmlDoc = theElement.ownerDocument();
	QDomElement frameNode = xmlDoc.createElement( FRAME_OBJECT );	// <FrameObject>
	// Here we set the attributes of the <dvdauthor> tag
	if (frameData ( )->m_iFrameWidth != 4)
		frameNode.setAttribute ( FRAME_OBJECT_WIDTH, frameData ( )->m_iFrameWidth );
	if (frameData ( )->m_colorFrame != Rgba ( START_FRAME_COLOR ) )
		frameNode.setAttribute ( FRAME_OBJECT_COLOR, frameData ( )->m_colorFrame.toString ( ) );
	if (frameData ( )->m_iFrameStyle != (int)Qt::SolidLine)
		frameNode.setAttribute ( FRAME_OBJECT_STYLE, frameData ( )->m_iFrameStyle );
	if (frameData ( )->m_iFrameJoin != (int)Qt::MiterJoin)
		frameNode.setAttribute ( FRAME_OBJECT_JOIN,  frameData ( )->m_iFrameJoin );

	theElement.appendChild( frameNode );
	// And here we write the base class ...
	return MenuObject::writeProjectFile( frameNode );
}

bool FrameObject::createContextMenu (QPoint globalPos)
{
/*        QString qsShadow;
        QPoint globalPos2 = globalPos;
	Q3PopupMenu *pStackMenu = new Q3PopupMenu(m_pParent);
	pStackMenu->insertItem ( tr ("Cut") , this, SLOT ( slotCut  ( ) ) );
	pStackMenu->insertItem ( tr ("Copy"), this, SLOT ( slotCopy ( ) ) );
	pStackMenu->insertSeparator();
	pStackMenu->insertItem ( tr ("To Front")  , this, SLOT(slotToFront ( ) ) );
	pStackMenu->insertItem ( tr ("To Back")   , this, SLOT(slotToBack  ( ) ) );
	globalPos.setY ( globalPos.y ( ) - 25 );
	globalPos.setX ( globalPos.x ( ) - pStackMenu->sizeHint ( ).width ( ) ); // -100);
	pStackMenu->popup(globalPos, 1);

	qsShadow = tr ( "Add Shadow ..." );
	if ( shadow ( ) )
 	   qsShadow = tr ( "Edit Shadow ..." );

	if (m_pContextMenu)
		delete m_pContextMenu;
	m_pContextMenu = new Q3PopupMenu(m_pParent);
	m_pContextMenu->insertItem ( tr ("Edit ...")  , this, SLOT(slotEdit       ( ) ) );
	m_pContextMenu->insertItem ( tr ("Matrix ..."), this, SLOT(slotMatrix     ( ) ) );
	m_pContextMenu->insertItem ( tr ("Delete")    , this, SLOT(slotDelete     ( ) ) );
	m_pContextMenu->insertSeparator();
	m_pContextMenu->insertItem ( qsShadow          , this, SLOT(slotAddShadow ( ) ) );
	m_pContextMenu->insertItem ( tr ("Add Text")   , this, SLOT(slotAddText   ( ) ) );
	m_pContextMenu->insertItem ( tr ("Add Image")  , this, SLOT(slotAddImage  ( ) ) );
	m_pContextMenu->insertItem ( tr ("Add Movie")  , this, SLOT(slotAddMovie  ( ) ) );
	m_pContextMenu->insertSeparator();
	m_pContextMenu->insertItem ( tr ("Define as Button") , this, SLOT(slotDefineAsButton()));
	m_pContextMenu->exec(globalPos2, 4);

	delete pStackMenu;
	if (m_pContextMenu)
		delete m_pContextMenu;
	m_pContextMenu = NULL;
	// Here we mark that the user called a menu item thus we don't want the
	// base classes to continue with the mouse event
//	if (iReturn != -1)
		return true;*/	//ooo

  QString qsShadow;
  QPoint globalPos2 = globalPos;
 
  qsShadow = tr ( "Add Shadow ..." );
    if ( shadow ( ) )
  qsShadow = tr ( "Edit Shadow ..." );
  
  if (m_pContextMenu)
    delete m_pContextMenu;
  
  QAction *editNewAction = new QAction("Edit ...", this);
  QAction *matrixNewAction = new QAction("Matrix ...", this);
  QAction *deleteNewAction = new QAction("Delete", this);
  QAction *shadowAction = new QAction(qsShadow, this);
  QAction *defineAsButtonNewAction = new QAction("Define as Button", this);
  
  m_pContextMenu = new QMenu (m_pParent);
  m_pContextMenu->addAction (editNewAction);
  m_pContextMenu->addAction (matrixNewAction);
  m_pContextMenu->addAction (deleteNewAction);
  m_pContextMenu->addSeparator();
  m_pContextMenu->addAction (shadowAction);
  m_pContextMenu->addSeparator();
  m_pContextMenu->addAction(defineAsButtonNewAction);
  
  connect(editNewAction,           SIGNAL(triggered()), this, SLOT( slotEdit            ( ) ) );
  connect(matrixNewAction,         SIGNAL(triggered()), this, SLOT( slotMatrix          ( ) ) );
  connect(deleteNewAction,         SIGNAL(triggered()), this, SLOT( slotDelete          ( ) ) );
  connect(shadowAction,            SIGNAL(triggered()), this, SLOT( slotAddShadow       ( ) ) );
  connect(defineAsButtonNewAction, SIGNAL(triggered()), this, SLOT( slotDefineAsButton  ( ) ) );
  
  m_pContextMenu->exec( globalPos2, deleteNewAction );
  
  if (m_pContextMenu)
    delete m_pContextMenu;
  m_pContextMenu = NULL;
  // Here we mark that the user called a menu item thus we don't want the
  // base classes to continue with the mouse event
//	if (iReturn != -1)
  return true;
}

StructureItem *FrameObject::createStructure ( StructureItem *pParentItem )
{
  StructureItem *pFrameItem, *pTemp=NULL;

  if ( ! pParentItem )  {
    if ( m_pOverlay  )
      pTemp = m_pOverlay->createStructure ( NULL );
    if ( m_pShadow )
      pTemp = m_pShadow->createStructure  ( NULL );
    modifiers ( )->createStructure  ( NULL, NULL );
    m_pStructureItem = NULL;
    return NULL;
  }

  QString qsName ( tr("Frame (%1, %2, %3, %4)" ).
		   arg ( rect ( ).x      ( ) ).arg ( rect ( ).y      ( ) ).
		   arg ( rect ( ).width  ( ) ).arg ( rect ( ).height ( ) ) );
  setName ( qsName );

  qsName  = tr ( "Frame Object" );
  QString qsInfo = QString ("%1 - (%2, %3, %4 %5)").arg ( width ( ) ).
    arg ( rect ( ).x      ( ) ).arg ( rect ( ).y      ( ) ).
    arg ( rect ( ).width  ( ) ).arg ( rect ( ).height ( ) );

  pFrameItem = m_pStructureItem;
  if ( ! pFrameItem ) {
    pFrameItem = new StructureItem ( this, StructureItem::Frame, pParentItem, pParentItem->lastChild ( ), qsName, qsInfo );
    //pFrameItem->setExpandable ( TRUE );	//ooo
    pFrameItem->setExpanded ( false );		//xxx
    m_pStructureItem = pFrameItem;
  }
  else
    //pFrameItem->setText ( qsName, qsInfo );	//ooo
    pFrameItem->setText ( 0, qsName );		//xxx
    pFrameItem->setText ( 1, qsInfo );		//xxx

  QString qsArgs;
  qsArgs =  QString ( "%1, %2, %3, %4" ).
    arg ( rect ( ).x      ( ) ).
    arg ( rect ( ).y      ( ) ).
    arg ( rect ( ).width  ( ) ).
    arg ( rect ( ).height ( ) );
  pTemp  = pFrameItem->setSibling ( this, pTemp, tr ( "Geometry"    ), qsArgs );

  qsArgs = QString ( "%1" ).arg ( width ( ) );
  pTemp  = pFrameItem->setSibling ( this, pTemp, tr ( "Frame Width" ), qsArgs );

  qsArgs = tr ( "color(%1, %2, %3)" ).
    arg ( frameData ( )->m_colorFrame.red   ( ) ).
    arg ( frameData ( )->m_colorFrame.green ( ) ).
    arg ( frameData ( )->m_colorFrame.blue  ( ) );

  pTemp  = pFrameItem->setSibling ( this, pTemp, tr ( "Frame Color" ), qsArgs );

  if ( m_pShadow )
    pTemp = m_pShadow->createStructure ( pFrameItem );

  modifiers( )->createStructure ( pFrameItem, pTemp );

  return pFrameItem;
}


void FrameObject::slotEdit()
{
	mouseDoubleClickEvent(NULL);
}

void FrameObject::slotMatrix()
{  
  if ( m_pMatrixDialog )
    return;

  m_pMatrixDialog = new DialogMatrix ( MenuObject::parent ( ) );
  m_pMatrixDialog->initMe ( this );
  m_pMatrixDialog->show   ( );
}

void FrameObject::slotAddText()
{
	MessageBox::warning (NULL, tr ("Not Implemented yet."), tr("Not Implemented yet."),
		QMessageBox::Ok ,  QMessageBox::Cancel);
}

void FrameObject::slotAddImage()
{
	MessageBox::warning (NULL, tr("Not Implemented yet."), tr("Not Implemented yet."),
		QMessageBox::Ok ,  QMessageBox::Cancel);
}

void FrameObject::slotAddMovie()
{
	MessageBox::warning (NULL, tr("Not Implemented yet."), tr("Not Implemented yet."),
		QMessageBox::Ok ,  QMessageBox::Cancel);
}

void FrameObject::slotDefineAsButton()
{
	emit (signalDefineAsButton(this));
}

MenuObject *FrameObject::clone ( QWidget *pParent, MenuObject * )
{
  if ( !pParent )
    pParent = MenuObject::parent ( );

  FrameObject *pNewObject = new FrameObject ( pParent );
  pNewObject->setFrameWidth(width());
  pNewObject->setFrameStyle(style());
  pNewObject->setFrameJoin (join ());
  pNewObject->setFrameColor(color());
  
  // The following two are not really needed, since we get those
  // informations solely from the Normal State - objects ...
  pNewObject->setRect( rect ( ) );
  pNewObject->setModifiers ( *modifiers ( ) );
  
  MenuObject *pShadow = shadow ( );
  if ( pShadow )
    pShadow = pShadow->clone ( pParent, pNewObject );
  pNewObject->setShadow      ( pShadow );
  
  return pNewObject;
}

AnimationAttribute *FrameObject::getSpecificAttributes (long iMaxNumberOfFrames, QString qsProperty)
{
	AnimationAttribute *pAnimAttr = NULL;
	if (qsProperty == "color.red")	{
		FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(frameData ( )->m_colorFrame, TYPE_COLOR_RED);
		//pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.Qt::red(), callMeBack);	//ooo
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.red(), callMeBack);		//xxx
	}
	else if (qsProperty == "color.green")	{
		FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(frameData ( )->m_colorFrame, TYPE_COLOR_GREEN);
		//pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.Qt::green(), callMeBack);	//ooo
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.green(), callMeBack);		//xxx
	}
	else if (qsProperty == "color.blue")	{
		FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(frameData ( )->m_colorFrame, TYPE_COLOR_BLUE);
		//pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.Qt::blue(), callMeBack);	//ooo
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.blue(), callMeBack);		//xxx
	}
	else if (qsProperty == "color.alpha")	{
		FunctionCallback<Rgba> *callMeBack = new FunctionCallback<Rgba>(frameData ( )->m_colorFrame, TYPE_COLOR_ALPHA);
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_colorFrame.alpha(), callMeBack);
	}
	else if (qsProperty == "frameWidth")
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_iFrameWidth, &frameData ( )->m_iFrameWidth);
	else if (qsProperty == "frameStyle")
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_iFrameStyle, &frameData ( )->m_iFrameStyle);
	else if (qsProperty == "frameJoin")
		pAnimAttr = new AnimationAttribute (iMaxNumberOfFrames, qsProperty, frameData ( )->m_iFrameJoin, &frameData ( )->m_iFrameJoin);
	return pAnimAttr;
}

