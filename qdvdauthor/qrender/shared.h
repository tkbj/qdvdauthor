/****************************************************************************
** Client - class
**
**   Created : Fri Jun 6 12:09:08 2008
**        by : Varol Okan using kate editor
** Copyright : (c) Varol Okan
**   License : GPL v 2.0
**
**
****************************************************************************/


#ifndef RENDER_SHARED_H
#define RENDER_SHARED_H

#define QRENDER_VERSION "0.1.0"

// This will fake that the server and client are on separate hosts
// even if they reside only on one host.
//#define FAKE_REMOTE_HOST 1

namespace Render
{

// default port the server is listening on
// Note: Need to implement GUI to overwrite this Default setting.
//#define SERVER_PORT 5066  //ooo
#define SERVER_PORT 5066    //xxx

// All Message IDs are 16 bit

// Synch version and IP address
#define CLIENT_WHO_AND_WHERE_ARE_YOU 0x2121
#define SERVER_ITS_ME_AND_I_AM_AT    0x1212

#define SERVER_MY_STATUS_SIRE 0xABCD

// Tell the server to get started as he has all images / infos required.
#define CLIENT_GO_TO_WORK 0xAEEE

// there was an problem with the socket. Client tries to re-establish connection
#define CLIENT_RECONNECTING 0x1003

// KeepAlive messages sent every 20 seconds from server to QDVDAuthor
// when no rendering is done. Simply to know if server is still there.
#define SERVER_I_AM_ALIVE  0x5555
#define CLIENT_I_KNOW_THAT 0xAAAA

// Ping interval = 200 seconds
//#define I_AM_ALIVE_INTERVAL 200000    //ooo
#define I_AM_ALIVE_INTERVAL 20000       //xxx
// Timeout after missing 10 KeepAlive messages ( 10 * 20 sec = 3min, 20sec)
#define WENT_MISSING_AFTER 10

// Sometimes the client looses connection, and the PING won't go through
// These msgs will re-establish slideshow progress with QDVDAuthor
#define CLIENT_I_LOST_CONTACT  0xABBA
#define SERVER_I_AM_STILL_HERE 0xBAAB

// Progress message
#define SERVER_YOUR_PROGRESS_SIR 0x1234

// Client message to server thread to pause execution
// Note: at this point we should probably write status 
// info into a cfg file to recover, just in case.
#define CLIENT_TAKE_A_BREAK 0x1001

// Client send the input images over the socket only if 
// server/client are on different machines.
#define CLIENT_TAKE_A_FILE 0x0FF0
#define SERVER_GOT_A_PIECE 0xF00F

// If Server and Client are on the same host, then simply send the xml file name
#define CLIENT_TAKE_THAT_XML      0xF00D
#define SERVER_STICK_A_FORK_IN_ME 0xFEED

// Server sends the finishes slideshow back to the client
#define SERVER_TAKE_A_FILE 0x0EE0
#define CLIENT_GOT_A_PIECE 0xE00E

// Server exited
#define SERVER_TO_BE_OR_NOT_TO_BE 0xFF00

// 0xSHIT << 16, Server exception 
#define SERVER_SHIT_HAPPENED 0xB01C

// Lastly ( for now anyways )
// Client telling Server to shut down
#define CLIENT_KILL_YOURSELF 0xB00B

}; // end of namespace Render

#endif // RENDER_SHARED_H

