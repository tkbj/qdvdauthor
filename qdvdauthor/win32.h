
#ifdef WIN32

#ifndef __MY_WIN_32__
#define __MY_WIN_32__
/*
#ifndef WINVER
#define WINVER 0x0501
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#include <afxwin.h>         // MFC core and standard components

#ifdef ASSERT
#undef ASSERT
#endif
#undef MessageBox


//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <winsock2.h>
//#include <afxsock.h>		// MFC socket extensions

*/
#define M_PI 3.1415926536 

//#define S_IEXEC  1
//#define S_IRUSR  2
//#define S_IWRITE 3
#define S_IROTH  4
#define S_IXOTH  5
#define S_IWOTH  6
//#define S_IWUSR  7
//#define S_IXUSR  8
#define S_IRGRP  9 
#define S_IXGRP 10
/*

void chmod ( const char *, unsigned int );
*/
const char *getlogin ( );
void sleep ( int i );
void usleep ( unsigned long );
void srandom    ( unsigned int );
long int random ( );

#endif

#endif 

