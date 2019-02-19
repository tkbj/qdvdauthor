/***************************************************************************
    errormessages.cpp
                             -------------------
    Interface to the GUI.
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This is a very simple match of Error message to resolve string
   It works in direct conjunction with execute.cpp's MyDialog::slotAnalyze
   
   Note: These error messages are from the used tools and not
         from QDVDAuthor itself.

****************************************************************************/

#include "errormessages.h"

ErrorMessages::ErrorMessages ( )
{
  initMe ( );
}

ErrorMessages::~ErrorMessages ( ) 
{
  uint t;
  for ( t=0; t<m_listOfMessages.count ( ); t++ ) {
    delete m_listOfMessages[t];
  }
  m_listOfMessages.clear ( );
}

void ErrorMessages::initMe ( )
{
  QString qsExampleError, qsError, qsHelp;
  // Here are all the strings 

  // the first error ...
  qsError = "ERR:  Too many pre/post/cell commands.";
  qsExampleError = "ERR: Too many pre/post/cell commands. Reduce complexity and/or disable jumppad";

  qsHelp  = tr ( "There is a limitation of 99 chapters per PGC (I believe) in the spec" );
  qsHelp += tr ( "(and 99 PGCs per title or titleset).<BR>" );
  qsHelp += tr ( "Here are two links that have some detailed info<BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.videohelp.com/dvd#tech\">http://www.videohelp.com/dvd#tech</A><BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.doom9.org/\">http://www.doom9.org/</A> -> [The Basics]<P>" );

  qsHelp += tr ( "This error is usually caused when you create a slightly larger project.<BR>" );
  qsHelp += tr ( "To fix it you can assign some of your video files to a separate titleset<P>" );

  qsHelp += "<FONT COLOR=ORANGE>"; // Solutions part ...
  qsHelp += tr ( "<FONT SIZE=+2><U>What To Do :</U></FONT><BR>" );
  qsHelp += tr ( "Right click on a <FONT COLOR=WHITE><B>SourceFileEntry</B></FONT> and select [<U>Assign to tileset ...</U>] and set the value to E.g. 1.<BR>" );
  qsHelp += tr ( "Note: a value of -1 indicates that QDVDAuthor will automatically assign those videos to a titleset.<P>" );

  qsHelp += tr ( "More information can be found under <BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.nabble.com/dvdauthor-jumpad-question...-t1159216.html\">http://www.nabble.com/dvdauthor-jumpad-question...-t1159216.html</A>" );
  qsHelp += "</FONT>";
  add ( qsError, qsHelp, qsExampleError );

  // the second error ...ERR:  Cannot jump to title 
  qsError = "ERR: Cannot jump to chapter ";
  qsExampleError = "ERR: Cannot jump to chapter 2 of title 1, only 1 exist";

  qsHelp  = tr ( "This is an internal error to QDVDAuthor. Please notify the author of QDVDAuthor if you see this message." );
  qsHelp += tr ( "");
  qsHelp += tr ( "Here are two links that have some detailed info<BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.videohelp.com/dvd#tech\">http://www.videohelp.com/dvd#tech</A><BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.doom9.org/\">http://www.doom9.org/</A> -> [The Basics]<P>" );

  qsHelp += tr ( "This error is usually caused when you create a slightly larger project.<BR>" );
  qsHelp += tr ( "To fix it you can assign some of your video files to a separate titleset<P>" );

  qsHelp += "<FONT COLOR=ORANGE>"; // Solutions part ...
  qsHelp += tr ( "<FONT SIZE=+2><U>What To Do :</U></FONT><BR>" );
  qsHelp += tr ( "" );
  qsHelp += tr ( "You can try to re-arrange the videos or check if the movie chapters are handled correctly.<P>" );
  qsHelp += "</FONT>";
  add ( qsError, qsHelp, qsExampleError );

  // the second error ...
  qsError = "WARN: Skipping sector, waiting for first VOBU...";
  qsExampleError = "WARN: Skipping sector, waiting for first VOBU...";

  qsHelp  = tr ( "This warning indicates that the source files (video) you are using are missing" );
  qsHelp += tr ( "the VOBU (Video Object Units) which are holding NAV packets for DVD navigation.<P>" );
 
  qsHelp += tr ( "'Q' DVD-Author should in general detect the lack of the NAV packets and mark the " );
  qsHelp += tr ( "offending video for transcoding.<P>" );

  qsHelp += tr ( "This warning usually indicates that the video file was not properly prepared for dvdauthor." );
  qsHelp += tr ( "Make sure that if you use E.g. mplex to create the video to use the <B><U>-f 8</U></B>" );
  qsHelp += tr ( "switch.<P>" );

  qsHelp += "<FONT COLOR=ORANGE>"; // Solutions part ...
  qsHelp += tr ( "<FONT SIZE=+2><U>What To Do :</U></FONT><BR>" );
  qsHelp += tr ( "" );
  qsHelp += tr ( "If this warning shows up many hundred or thousand times, then I would strongly suggest" );
  qsHelp += tr ( "to re-multiplex the video stream and add the -f 8 switch.<BR>" );
  qsHelp += tr ( "If you see this message only a handlfull of times, it can be ignored as the neccesary" );
  qsHelp += tr ( "will be placed in the remaining NAV packets.<P>" );
  qsHelp += tr ( "<FONT COLOR=WHITE><B>movie-to-dvd -m pal input_file.avi</B></FONT>" );

  qsHelp += "</FONT>";
  add ( qsError, qsHelp, qsExampleError );

  // the EXAMPLE error
/*
  qsError = "INFO: [jpeg2yuv] Parsing";
  qsExampleError = "This is just and Example error message.";

  qsHelp  = tr ( "there is a limitation of 99 chapters per PGC (I believe) in the spec" );
  qsHelp += tr ( "(and 99 PGCs per title or titleset).<BR>" );
  qsHelp += tr ( "Here are two links that have some detailed info<BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.videohelp.com/dvd#tech\">http://www.videohelp.com/dvd#tech</A><BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.doom9.org/\">http://www.doom9.org/</A> -> [The Basics]<P>" );

  qsHelp += tr ( "This error is usually caused when you create a slightly larger project.<BR>" );
  qsHelp += tr ( "To fix it you can assign some of your video files to a separate titleset<P>" );

  qsHelp += "<FONT COLOR=ORANGE>"; // Solutions part ...
  qsHelp += tr ( "<FONT SIZE=+2><U>What To Do :</U></FONT><BR>" );
  qsHelp += tr ( "Right click on a <FONT COLOR=WHITE><B>SourceFileEntry</B></FONT> and select [<U>Assign to tileset ...</U>] and set the value to E.g. 1.<BR>" );
  qsHelp += tr ( "Note: a value of -1 indicates that QDVDAuthor will automatically assign those videos to a titleset.<P>" );

  qsHelp += tr ( "More information can be found under <BR>" );
  qsHelp += tr ( "<A HREF=\"http://www.nabble.com/dvdauthor-jumpad-question...-t1159216.html\">http://www.nabble.com/dvdauthor-jumpad-question...-t1159216.html</A>" );
  qsHelp += "</FONT>";
  add ( qsError, qsHelp, qsExampleError );
*/
}

void ErrorMessages::add ( QString &qsError, QString &qsHelp, QString &qsExampleError )
{
  Message *pMessage        = new Message;
  pMessage->qsError        = qsError;
  pMessage->qsHelp         = qsHelp;
  pMessage->qsExampleError = qsExampleError;
  m_listOfMessages.append  ( pMessage );
}

int ErrorMessages::count ( ) 
{
  return m_listOfMessages.count ( );
}

QString ErrorMessages::errorString ( int iWhich )
{
  QString qsTemp;
  if ( iWhich < (int)m_listOfMessages.count ( ) )
    return m_listOfMessages [ iWhich ]->qsError;
  
  return qsTemp;
}

QString ErrorMessages::helpMessage ( int iWhich )
{
  QString qsTemp;
  if ( iWhich < (int)m_listOfMessages.count ( ) ) {
    qsTemp  = "<TABLE WIDTH=\"100%\"><TR><TD BGCOLOR=ORANGE><CENTER><FONT COLOR=RED SIZE=+3><B>";
    qsTemp += m_listOfMessages [ iWhich ]->qsExampleError;
    qsTemp += " ... </B></FONT></CENTER></TD></TR><TR><TD BGCOLOR=DARKBLUE><FONT COLOR=GREEN><B>";
    qsTemp += m_listOfMessages[ iWhich ]->qsHelp;
    qsTemp += "</B></FONT></TD></TR></TABLE><HR>";
  }
  return qsTemp;
}

