
#include "win32.h"
/*
void chmod ( const char *, unsigned int ) {};
*/
const char *getlogin ( ) { return "itsMe"; };
void sleep ( int i ) { };
void usleep ( unsigned long ) { };
void srandom    ( unsigned int ) { };
long int random ( )    { return 1; };

