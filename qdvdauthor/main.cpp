
#include <getopt.h>

#include "global.h"
//#include "dvdmenu.h"
#include "qdvdauthor.h"
#include "cachethumbs.h"

int main(int argc, char *argv[])
{
  int       iReturn = 0; 
  bool      bSaveProjectFile  = false;
  QString   qsFileName;
  
  QApplication app(argc, argv);  
    
  QDVDAuthor dvdAuthor;

  dvdAuthor.resize(350, 250);
  dvdAuthor.move(300, 300);  
  dvdAuthor.setWindowTitle("Application skeleton");
  
  Global::pThumbsCache = new Cache::Thumbs;
  
  Global::pApp = &dvdAuthor;
  /*VMGMenu *pDVDMenu = new VMGMenu ( &dvdAuthor );

  dvdAuthor.addDVDMenu ( pDVDMenu, true );*/
  
  dvdAuthor.show();
  
	/*while (1) {
	  char c =  (getopt ( argc, argv, "k:axp:o:s:?hd:"));
	  if ( c == -1 )
	    break;
	  switch (c) {
	  case 'o': { // save generated project file as ...
	    qsFileName  = QString ( optarg );
	    Global::qsProjectFileName = qsFileName;
	    bSaveProjectFile = true;
	  }
	  break;
	  }
	}*/	//oooo

    //return app.exec();  //xxx
    iReturn = app.exec();
    
	// Free global static allocated objects.
	
	delete Global::pThumbsCache;
	Global::pThumbsCache = NULL;
    
    return iReturn;
} 
