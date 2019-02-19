#include <qapplication.h>
#include "dialogslide.h"	//ooo

int main( int argc, char ** argv )
{
  QApplication a( argc, argv );
  CSlideDialog w;
  w.initMe(NULL, QString ());
  w.show();
  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  return a.exec();
}
