/***************************************************************************
    keyword.cpp
                             -------------------
    Keyword - class
                             
    begin                : March 2015
    copyright            : (c) 2015 by Iven Wold - 526f6c616e642053756368616e

    license              : GPL v 2.0
    
   This class will replace a given token in a string with the set value.
   This class is currently only used in QDVDAuthorInit to help generate
   the correct commands.
    
****************************************************************************/

#include "qdvdauthorinit.h"
//Added by qt3to4:
//#include <Q3ValueList>	//oxx
#include <QMessageBox>		//xxx

#include "keyword.h"

Keyword::Keyword ()
{
  int t;
  char arrayKeys[15][35] = {"+DVDAUTHOR_XML+", "+PROJECT_PATH+", "+TEMP_PATH+", "+WIDTH+", "+HEIGHT+",
    "+FORMAT+", "+MENU_NAME+", "+ASPECT+", "+START_POS+", "+DURATION+", "+NR_OF_FRAMES+", "+BACKGROUND_FILE_NAME+", 
    "+BACKGROUND_FULL_NAME+", "+BUTTON_NAME+", "+SOUND_LIST+" };

  char arrayTranscodeKeys[23][35] = {"+DVDAUTHOR_XML+", "+PROJECT_PATH+", "+TEMP_PATH+", "+INPUT_NAME+", "+BASE_NAME+",
    "+FILE_EXT+", "+AUDIO_FORMAT+", "+AUDIO_TYPE+", "+AUDIO_BITRATE+", "+SAMPLE_RATE+", "+STARTING_AT+", "+ENDING_AT+",
    "+DURATION+", "+WIDTH+", "+HEIGHT+", "+VIDEO_FORMAT+", "+VIDEO_TYPE+", "+VIDEO_BITRATE+", "+FRAME_RATE+", "+RATIO+",
    "+AUDIO_LIST+", "+FOREACH_COUNTER+", "+TRACK_NAME+" };

  char arraySubtitlesKeys[8][35] = {"+PROJECT_PATH+", "+TEMP_PATH+", "+INPUT_FILE_NAME+", "+XML_FILE_NAME+", "+BASE_NAME+", "+BASE_EXTENSION+", "+SUBTITLE_NUMBER+", "+MULTIPLE_SUBTITLE_PIPE+" };

  // init the key lists
  Entry *pEntry = NULL;

  for (t=0;t<KeyLast;t++) {
    pEntry = new Entry ();
    pEntry->qsKey = arrayKeys [ t ];
    m_listOfPair[   Keys   ].append ( pEntry ); 
  }
  for (t=0;t<TransLast;t++) {
    pEntry = new Entry ();
    pEntry->qsKey = arrayTranscodeKeys [ t ];
    m_listOfPair[TransKeys].append ( pEntry );
  }
  for (t=0;t<SubLast;t++) {
    pEntry = new Entry ();
    pEntry->qsKey = arraySubtitlesKeys [ t ];
    m_listOfPair[ SubKeys ].append ( pEntry );
  }
}

Keyword::~Keyword ()
{
  int t, i;
  Entry *pEntry = NULL;
  for ( i=0;i<BaseLast;i++) {
    for (t=0;t< count ( i );t++) {
      pEntry = m_listOfPair [ i ][ t ];
      if ( pEntry )
	delete pEntry;
    }
  }
}

QString Keyword::value ( int iWhichKeyset, int iWhichKey )
{
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return QString();
  if ( ( iWhichKey < 0 ) || ( iWhichKey >= (int) m_listOfPair[iWhichKeyset].count () ) )
    return QString ();

  return m_listOfPair[iWhichKeyset][iWhichKey]->qsValue;
}

QString Keyword::key ( int iWhichKeyset, int iWhichKey )
{
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return QString();
  if ( ( iWhichKey < 0 ) || ( iWhichKey >= (int) m_listOfPair[iWhichKeyset].count () ) )
    return QString ();

  return m_listOfPair[iWhichKeyset][iWhichKey]->qsKey;
}


int Keyword::count ( int iWhichKeyset )
{
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return 0;

  return (int) m_listOfPair[ iWhichKeyset ].count ();
}

void Keyword::clear ( int iWhichKeyset )
{
  return;
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return;

  int t;

  for (t=0;t< count ( iWhichKeyset );t++)
    m_listOfPair[iWhichKeyset][t]->qsValue = QString ();
}

QStringList Keyword::getKeys ( int iWhichKeyset )
{
  QStringList listOfValues;
  int t;
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return listOfValues;
  
  for (t=0;t<count ( iWhichKeyset ); t++)
    listOfValues.append ( key ( iWhichKeyset, t ) );

  return listOfValues;
}

QStringList Keyword::getValues ( int iWhichKeyset )
{
  QStringList listOfValues;
  int t;
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return listOfValues;

  for (t=0;t<count ( iWhichKeyset ); t++)
    listOfValues.append ( value ( iWhichKeyset, t ) );

  return listOfValues;
}

bool Keyword::setValue ( int iWhichKeyset, int iWhichKey, QString qsValue )
{
  if ( ( iWhichKeyset < 0 ) || ( iWhichKeyset >= BaseLast ) )
    return false;
  if ( ( iWhichKey < 0 ) || ( iWhichKey >= (int) m_listOfPair[iWhichKeyset].count () ) )
    return false;

  if ( ! m_listOfPair[iWhichKeyset][iWhichKey] )
    return false;

  m_listOfPair[iWhichKeyset][iWhichKey]->qsValue = qsValue;
  m_listOfPair[iWhichKeyset][iWhichKey]->iIndex  = iWhichKey;

  return true;
}

void Keyword::setKeywords ( int iWhichKeys, QList<QString> listKeywords )	//oxx
{
  uint t;
  clear ( iWhichKeys );
  for ( t=0;t<(uint)listKeywords.count();t++)
    setValue ( iWhichKeys, t, listKeywords[t] );
}

QString Keyword::resolvString ( int iWhichKeyset, QString qsInput)
{ 
  // This function resolves the variables in the stored commands / comments / names like
  // +MENU_NAME+, +WIDTH+, +BACKGROUND_FILE_NAME+ etc.
  QString qsOutput = qsInput;
  uint t;

  for (t=0;t<(uint)m_listOfPair[iWhichKeyset].count();t++)	{
    // Here we replace the actual keyword with the real string.
    if ( (m_listOfPair[iWhichKeyset][t]->qsValue.isNull()) || (m_listOfPair[iWhichKeyset][t]->qsValue.isEmpty()) )
      continue;
    qsOutput.replace ( m_listOfPair[iWhichKeyset][t]->qsKey, m_listOfPair[iWhichKeyset][t]->qsValue );
  }

  return qsOutput;
}

/**
 * This function is used for the TranscodeInterface - part of CommandQueueDialog.
 * It converts the input values to the propper format needed by the conversion tools.
 * E.g. aspect_ratio = "4:3" translates to '2' for transcode and 4/3 for MEncoder
 * 
 * Note, I know that this is not the most efficient place to put this sanity checking
 * but this function is called less then 100 times per DVD creation and does not really 
 * affect performance if we leave it here ;o)
 * \see resolveString()
 */
QString Keyword::resolvConvert (QString qsInput, QString qsConversionTool)
{
//	char arrayTranscodeKeys[19][35] = {"+DVDAUTHOR_XML+", "+PROJECT_PATH+", "+TEMP_PATH+", "+INPUT_NAME+", "+BASE_NAME+",
//		"+FILE_EXT+", "+AUDIO_FORMAT+", "+AUDIO_TYPE+", "+AUDIO_BITRATE+", "+SAMPLE_RATE+", "+STARTING_AT+", "+ENDING_AT+",
//		"+DURATION+", "+WIDTH+", "+HEIGHT+", "+VIDEO_FORMAT+", "+VIDEO_TYPE+", "+VIDEO_BITRATE+", "+FRAME_RATE+", "+RATIO+",
//		};
  uint t;
  // Build a local copy of values
  QString stringArray[TransLast];
  QString qsRatio, qsValue, qsKey, qsOutput = qsInput;
  QString qsAudioBitrate, qsVideoBitrate;
  long iAudioBitrate, iVideoBitrate, iStartingAt, iEndingAt, iDuration;

  for (t=0;t<(uint)m_listOfPair[TransKeys].count();t++)	{
    qsValue = value ( TransKeys, t );
    stringArray[t] = qsValue;
  }

  // Next we adjust it for the conversion tool.

  // These here are used for all conversion tools.
  qsAudioBitrate = stringArray[ TransAudioBitrate ];
  iAudioBitrate  = qsAudioBitrate.toLong ( );
  if ( iAudioBitrate > 10000 )
       iAudioBitrate = (int)( (float)iAudioBitrate / 1000.0);
  if ( iAudioBitrate <   32 )   // according to DVD spec this is the minimum.
       iAudioBitrate =  192;    // Note: How to handle no audio (0) ?
  if ( iAudioBitrate > 1536 )
       iAudioBitrate = 1536;   // This is the max rate according to DVD spec.
  stringArray[ TransAudioBitrate ] = QString ("%1").arg ( iAudioBitrate );

  qsValue     = stringArray [ TransStartingAt ];
  iStartingAt = qsValue.toLong ( );
  qsValue     = stringArray [ TransEndingAt ];
  iEndingAt   = qsValue.toLong ( );
  qsValue     = stringArray [ TransDuration ];
  iDuration   = qsValue.toLong ( );
  if ( iEndingAt < 20 )
       iEndingAt = 1000000000; // 1 Million Seconds approx. 9.3 hours
  if ( iStartingAt > iEndingAt )
       iStartingAt = 0;
  if ( iDuration < 20 )
       iDuration = 1000000000; // 1 Million Seconds approx. 9.3 hours

  iStartingAt = (long)( iStartingAt / 1000.0 ); // convert form msec to seconds
  iEndingAt   = (long)( iEndingAt   / 1000.0 ); // convert from msec to seconds
  iDuration   = (long)( iDuration   / 1000.0 ); // convert form msec to seconds
  stringArray[ TransStartingAt] = QString ( "%1" ).arg ( iStartingAt );
  stringArray[ TransEndingAt  ] = QString ( "%1" ).arg ( iEndingAt   );
  stringArray[ TransDuration  ] = QString ( "%1" ).arg ( iDuration   );

  qsValue = stringArray[ TransAudioType ];
  // default it to mp2 and only allow ac3, and pcm
  //qsValue = qsValue.lower ( );	//ooo
  qsValue = qsValue.toLower ( );	//xxx
  if ( ( qsValue != "mp2" ) && ( qsValue != "pcm" ) )
         qsValue  = "ac3";
  stringArray[ TransAudioType ] = qsValue;

  qsVideoBitrate = stringArray[ TransVideoBitrate ];
  iVideoBitrate  = qsVideoBitrate.toInt ( );
  if ( iVideoBitrate > 10000 )
       iVideoBitrate = (int)((float)iVideoBitrate / 1000.0 );
  if ( iVideoBitrate < 1856 )	// according to DVD spec this is the minimum.
       iVideoBitrate = 1856;	// Note: How to handle no audio (0) ?
  stringArray[ TransVideoBitrate ] = QString ("%1").arg( iVideoBitrate );

  //if (qsConversionTool.find ( SUB_BLOCK_TRANSCODE ) > -1 )	{	//ooo
  if (qsConversionTool.indexOf ( SUB_BLOCK_TRANSCODE ) > -1 )	{	//xxx
    qsRatio = stringArray[ TransRatio ];
    if ( qsRatio == QString ("1:1"))
      qsRatio = QString ("1");
    else if ( qsRatio == QString ("4:3"))
      qsRatio = QString ("2");
    else if ( qsRatio == QString ("16:9"))
      qsRatio = QString ("3");
    else if ( qsRatio == QString ("2.21:1"))
      qsRatio = QString ("4");
    else 
      qsRatio = QString ("2");
    stringArray[ TransRatio ] = qsRatio;
  }
  //else if (qsConversionTool.find ( SUB_BLOCK_MENCODER ) > -1 )	{	//ooo
  else if (qsConversionTool.indexOf ( SUB_BLOCK_MENCODER ) > -1 )	{	//xxx
    bool bOkay;
    QString qsSampleRate = stringArray[ TransSampleRate ];
    int iSampleRate      = qsSampleRate.toInt ( &bOkay );

    if ( bOkay && ( iSampleRate < 10000 ) )
      iSampleRate *=    1000;
    if ( iSampleRate < 10000 )
      qsSampleRate =  "48000"; // default sample rate.
    else 
      qsSampleRate = QString ( "%1" ).arg ( iSampleRate );
    stringArray[ TransSampleRate ] = qsSampleRate;
    qsRatio = stringArray[ TransRatio ];
    if ( qsRatio == QString ("1:1"))
      qsRatio = QString ("1/1");
    else if ( qsRatio == QString ("4:3"))
      qsRatio = QString ("4/3");
    else if ( qsRatio == QString ("16:9"))
      qsRatio = QString ("16/9");
    else if ( qsRatio == QString ("2.21:1"))
      qsRatio = QString ("2.21/1");
    else
      qsRatio = QString ("4/3");
    stringArray[ TransRatio ] = qsRatio;

    // Next we take care of the end - pos ... 
    QString qsEndPos = stringArray[ TransEndingAt ];
    int iEndPos = qsEndPos.toInt ( &bOkay );
    if ( bOkay && iEndPos < 1 )
      qsEndPos = "100000000"; // 100000sec ~ 27:46h should be enough ...

    // And we also need to convert 29.97 to 30000/1001
    QString qsFrameRate = stringArray[ TransFrameRate ];
    float fFrameRate = qsFrameRate.toFloat ( &bOkay );
    qsFrameRate = "30000/1001"; // default to 29.97 (ntsc)
    if ( ( bOkay ) && ( fFrameRate < 29.0 ) )
      qsFrameRate = "25";
    stringArray[ TransFrameRate ] = qsFrameRate;
  }
  //else if (qsConversionTool.find ( SUB_BLOCK_FFMPEG ) > -1 )	{	//ooo
  else if (qsConversionTool.indexOf ( SUB_BLOCK_FFMPEG ) > -1 )	{	//xxx
    bool bOkay;
    QString qsSampleRate = stringArray[ TransSampleRate ];
    int iSampleRate      = qsSampleRate.toInt ( &bOkay );

    if ( bOkay && ( iSampleRate < 10000 ) )
      iSampleRate *=    1000;
    if ( iSampleRate < 10000 )
      qsSampleRate =  "48000"; // default sample rate.
    else
      qsSampleRate = QString ( "%1" ).arg ( iSampleRate );
    stringArray[ TransSampleRate ] = qsSampleRate;

    qsAudioBitrate = stringArray[ TransAudioBitrate ];
    iAudioBitrate = qsAudioBitrate.toInt ( &bOkay );
    if ( iAudioBitrate < 224000 )
         iAudioBitrate *=  1000;
    if ( iAudioBitrate < 224000 )
         iAudioBitrate = 224000;  // for ac3 minimum = 224kbps to 448kbps
    stringArray[ TransAudioBitrate ] = QString ("%1").arg( iAudioBitrate );
    qsVideoBitrate = stringArray[ TransVideoBitrate ];
    iVideoBitrate  = qsVideoBitrate.toInt ( &bOkay );
    if ( iVideoBitrate < 1856000 ) // according to DVD spec this is the minimum.
         iVideoBitrate *=   1000;
    if ( iVideoBitrate < 1856000 )
         iVideoBitrate = 1856000;  // Minimum video Bitrate = 2800kbps
    stringArray[ TransVideoBitrate ] = QString ("%1").arg( iVideoBitrate );
  }
  //else if (qsConversionTool.find ( SUB_BLOCK_VIDEOTRANS ) > -1 )	{	//ooo
  else if (qsConversionTool.indexOf ( SUB_BLOCK_VIDEOTRANS ) > -1 )	{	//xxx
    qsRatio = stringArray[ TransRatio ]; // only 4:3 and 16:9 are valid
    if ( ( qsRatio != QString ("4:3") ) && ( qsRatio != QString ("16:9") ) )
      stringArray[ TransRatio ] = QString ( "4:3" );
  }

  for (t=0;t<(uint)m_listOfPair[TransKeys].count();t++)	{
    qsValue = stringArray[ t ];
    qsKey   = key   ( TransKeys, t );
    // Here we replace the actual keyword with the real string.
    if ( ( qsValue.isNull()) || ( qsValue.isEmpty()) )
      continue;
    qsOutput.replace( qsKey, qsValue );
  }
  return qsOutput;
}

void Keyword::printf ( int iWhichKeyset )
{
  int    t;
  Entry *pEntry;
  if ( iWhichKeyset >= BaseLast )
    iWhichKeyset = Keys;

  for (t=0;t<(int)m_listOfPair[iWhichKeyset].count();t++) {
    pEntry = m_listOfPair[iWhichKeyset][t];
    //::printf ("%d > index=<%d> : Key=<%s> value=<%s>\n", t, pEntry->iIndex, pEntry->qsKey.ascii(), pEntry->qsValue.ascii() );				//ooo
    ::printf ("%d > index=<%d> : Key=<%s> value=<%s>\n", t, pEntry->iIndex, pEntry->qsKey.toLatin1().data(), pEntry->qsValue.toLatin1().data() );	//xxx
  }
}
