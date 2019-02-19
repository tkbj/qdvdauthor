/*****************************************************************************
 ** Class QRender
 **
 ** This class is the main class for the qrender - command line utility.
 ** The goal was to create a utility which is as powerfull as convert
 ** and also capable in reading in mmm - xml - files.
 **
 ** This class depends solely on the Qt - classes to do al the rendering 
 ** (overlapping, sizing, stretching, rotating etc)
 ** 
 ** This class should also write a status.txt file in the dest directory
 ** and automatically recover from any previous crash or unfinished task.
 **
 ** NOTE: one qrender - thread per DVDMenu.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <qfile.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <QTextStream>  //xxx

#include "qrender.h"
#include "xml_qrender.h"

int main( int argc, char ** argv )
{
	QRender theRender (argc, argv);
	return 0;
}

QRenderOptions::QRenderOptions ()
{
	bMirror     = false;
	bFlip       = false;
	fQuality    = false;
	bMonochrome = false;
	fBrightness = 0.0f;
	fSaturation = 0.0f;
	fHue        = 0.0f;
}

QRender::QRender (int argc, char **argv)
{
printf ("QRender::QRender \n");
	m_iVerbosityLevel = 0;
	m_pOutputOptions  = NULL;
	parseCommandLine (argc, argv);

	// FIXME:
	// For now we only support the xml - file and we also transcode every input file avail.
	transcode     ();
	extractFrames ();
	renderFrames  ();
	buildMenuMovie();
}

QRender::~QRender()
{
	uint t;
    for (t=0;t<static_cast<uint>(m_listInputOptions.count());t++)   //oxx
		delete m_listInputOptions[t];
	delete m_pOutputOptions;
}

QString QRender::checkForExe( QString qsExe )
{
	// Stolen from utils.cpp
	QString qsExePath;
	QFileInfo fileInfo(QString ("/tmp/%1_out.txt").arg(getlogin()));
	// the next check is to see if we can find 
	//int iRet = system ((const char *)QString ("which %1 > /tmp/%2_out.txt 2>/dev/null").arg(qsExe).arg(getlogin())); //ooo
    int iRet = system (QString ("which %1 > /tmp/%2_out.txt 2>/dev/null").arg(qsExe).arg(getlogin()).toLatin1().data());    //xxx
	iRet = iRet;

	fileInfo.setFile(QString("/tmp/%1_out.txt").arg(getlogin()));
	// Okay we can not find it, so we wont bother asking the user ...
	if (fileInfo.size() > 4)	{
                QFile theFile   (QString ("/tmp/%1_out.txt").arg(getlogin()));
		//theFile.open    (IO_ReadOnly);  //ooo
        theFile.open    (QIODevice::ReadOnly);  //xxx
		//theFile.readLine(qsExePath, 4096);  //ooo
        theFile.readLine(qsExePath.toLatin1().data(), 4096);    //xxx
		theFile.close();
		if (!qsExePath.isEmpty())	{
			qsExePath.remove("\n");
			return qsExePath;
		}
	}
	return QString ("/bin/bash");
}

void QRender::transcode ()
{
	uint t;
	QString qsFileName, qsCommand, qsConversionScript;
	qsCommand = QString ("bash");
	QString qsExePath = checkForExe ( qsCommand );

	qsFileName = m_xmlQRender.qrender().mmm.tmpdir+QString ("/ffmpeg.cfg");
	qsConversionScript = QString ("[mpeg2video]\nvhq = 1 \nmbd = 2 \nmbqmin = 2 \nmbqmax = 4 \n");
	generateFile (qsFileName, qsConversionScript);
 
	qsFileName = m_xmlQRender.qrender().mmm.tmpdir+QString ("/convert.sh");
// 	// First we convert the background movie into the right Format ...
	qsCommand = QString ("convert.sh %1").arg(qsFileName);
	qsConversionScript  = QString ("#!%1\n\n").arg (qsExePath);
	qsConversionScript += QString ("#Always do this, clean crappy avi's \n"
		"nice -19 mencoder -oac copy -ovc copy -ofps 25 -o output.avi $1 \n"
		"nice -19 mplayer -ao pcm -vo null -vc dummy output.avi  \n\n"
 
		"# if the incoming sound is at 44100 \n"
		"# upsample the sound to 48000. \n"
		"# here we rely on the fact that sox will abort if the input \n"
		"# frequency is 48000. \n"
		"if sox audiodump.wav -r 48000 output.wav resample; \n"
		"then \n"
			"# add -e 48000,16,2 to the transcode command \n"
			"# so the sync adjustment is set by transcode to -1600@1000 \n"
			"sound='-e 48000,16,2' \n"
			"# and make it into twolame mp2 format, nice! \n"
			"nice -19 twolame -p 2 -b 384 output.wav output.mp2 > /dev/null 2>&1  \n"
			"rm -f audiodump.wav output.wav \n"
		"else \n"
			"# otherwise sound is 48000 \n"
			"sound= \n"
			"nice -19 twolame -p 2 -b 384 audiodump.wav output.mp2 > /dev/null 2>&1 \n"
			"# there will be a stub 44 byte long output.wav from the failed sox \n"
//			"rm -f output.wav audiodump.wav \n"
		"fi \n\n"
 
		"# note the use of '-x mplayer,null' to remove export restrictions \n"
		"# imposed by bugs in ffmpeg which sometimes cause segfaults, boo! \n"
		"#transcode --nice 20 --print_status 500 -x mplayer,null -y ffmpeg,null -F mpeg2video $sound --export-prof dvd-pal -Z 720x576 -o output -i output.avi \n\n"
 
		"# WdW Mp2Enc \n"
		"transcode --nice 20 --print_status 100 -x mplayer,null -y mpeg2enc,null -V -w 8000 -F 8,\"-E -10 -q 6 -R 0 -K tmpgenc\" --export_asr 2 -Z 720x576 -o output -i output.avi \n\n"
 
//		"rm -f output.avi \n"
 
		"# leave only .mpg files \n"
		"nice -19 mplex -f 8 -o ${file}.vob output.m2v output.mp2 \n"
//		"rm -f output.m2v output.mp2 ;\n"
	);

//printf ("transcode :\n%s\n\n", (const char *)qsConversionScript); //ooo
printf ("transcode :\n%s\n\n", qsConversionScript.toLatin1().data());   //xxx
	generateFile (qsFileName, qsConversionScript);
	//chmod  (qsFileName, S_IEXEC | S_IRUSR | S_IWRITE);   //ooo
    chmod  (qsFileName.toLatin1().data(), S_IEXEC | S_IRUSR | S_IWRITE);    //xxx
	//int iRet = system (qsCommand);   //ooo
    int iRet = system (qsCommand.toLatin1().data());    //xxx
	iRet = iRet;
	for (t=0;t<static_cast<uint>(m_xmlQRender.qrender().mmm.listOfClips.count());t++)	{    //oxx
		qsCommand = QString ("convert.sh");
		//iRet = system (qsCommand);  //ooo
        iRet = system (qsCommand.toLatin1().data());    //xxx
	}
}

void QRender::generateFile (QString qsFileName, QString qsConversionScript)
{
	QFile theFile (qsFileName);

	//if ( theFile.open( IO_WriteOnly ) ) {    //ooo
    if ( theFile.open( QIODevice::WriteOnly ) ) {   //xxx
		QTextStream stream( &theFile );
		stream << qsConversionScript << "\n";
		theFile.close();
	}
}

/**
 ** <qrender  [tmpdir="<path>"] [resolution="<WxH>"] [format="pal"|"ntsc"]
 **       [images="ppm"|"png"|"jpg"] [successive="0"|"1"] 
 **       [background="#rrggbb"|"<filename>"]
 **       [frames="<num>"] [output="<filename>"]>
 **    <clip x="<pos>" y="<pos>" vob="<filename>" [startframe="<int>"]
 **      [width="<int>"] [height="<int>"] [mask="<filename>"] />
 **   [<clip ... />]
 ** </qrender>
 **/
/*
	QString qsConversionScript = 
QString ("!/bin/sh\n\n"

"echo [mpeg2video] >ffmpeg.cfg \n"
"echo 'vhq = 1' >>ffmpeg.cfg \n"
"echo 'mbd = 2' >>ffmpeg.cfg \n"
"echo 'mbqmin = 2' >>ffmpeg.cfg \n"
"echo 'mbqmax = 4' >>ffmpeg.cfg \n\n"
 
"for file in \"$@\" \n"
"do \n\n"
 
"#Always do this, clean crappy avi's \n"
"nice -19 mencoder -oac copy -ovc copy -ofps 25 -o output.avi ${file} \n"
"nice -19 mplayer -ao pcm -vo null -vc dummy output.avi  \n\n"
 
"# if the incoming sound is at 44100 \n"
"# upsample the sound to 48000. \n"
"# here we rely on the fact that sox will abort if the input \n"
"# frequency is 48000. \n"
"if sox audiodump.wav -r 48000 output.wav resample; \n"
"then \n"
"# add -e 48000,16,2 to the transcode command \n"
"# so the sync adjustment is set by transcode to -1600@1000 \n"
"sound='-e 48000,16,2' \n"
"# and make it into toolame mp2 format, nice! \n"
"nice -19 toolame -p 2 -b 384 output.wav output.mp2 > /dev/null 2>&1  \n"
"rm -f audiodump.wav output.wav \n"
"else \n"
"# otherwise sound is 48000 \n"
"sound= \n"
"nice -19 toolame -p 2 -b 384 audiodump.wav output.mp2 > /dev/null 2>&1 \n"
"# there will be a stub 44 byte long output.wav from the failed sox \n"
"rm -f output.wav audiodump.wav \n"
"fi \n\n"
 
"# note the use of '-x mplayer,null' to remove export restrictions \n"
"# imposed by bugs in ffmpeg which sometimes cause segfaults, boo! \n"
"#transcode --nice 20 --print_status 500 -x mplayer,null -y ffmpeg,null -F mpeg2video $sound --export-prof dvd-pal -Z 720x576 -o output -i output.avi \n\n"
 
"# WdW Mp2Enc \n"
"transcode --nice 20 --print_status 100 -x mplayer,null -y mpeg2enc,null -V -w 8000 -F 8,\"-E -10 -q 6 -R 0 -K tmpgenc\" --export_asr 2 -Z 720x576 -o output -i output.avi \n\n"
 
"rm -f output.avi \n"
 
"# leave only .mpg files \n"
"nice -19 mplex -f 8 -o ${file}.vob output.m2v output.mp2 \n"
"# rm -f output.m2v output.mp2 ;\n");
*/
 
 
void QRender::extractFrames ()
{
/**
1) Cleaning the movie material ...
   mencoder -oac pcm -ovc lavc -o "snl/clean.avi" "./snl.mpg"

2) extracting the wanted time frame ...
   mencoder -oac pcm -ovc lavc -ss 0 -endpos 10 -o "snl/clip.avi" "snl/clean.avi"

3) extracting audio (audiodump.wav) : 
   mplayer -ao pcm -vo null -vc dummy -aofile "snl/clip.wav" "snl/clip.avi"

4) extracting 300 frames from background.mwv
   mplayer -ao null -vo jpeg:outdir="./snl" "snl/clip.avi"

5) rm snl/clip.avi snl/clean.avi
*/

}

void QRender::renderFrames  ()
{
/**
1) 
*/
	// At this time we should have all the raw data in the appropriate directories (I.e. images and wav files).
	uint iNoOfFramesNeeded;
	iNoOfFramesNeeded = m_xmlQRender.qrender().mmm.frames;
	if (m_xmlQRender.qrender().mmm.frames == 0)	{
		if (m_xmlQRender.qrender().mmm.length != 0.0)	{
			if (m_xmlQRender.qrender().mmm.format == QString("ntsc"))
				iNoOfFramesNeeded = (uint)(29.97 * m_xmlQRender.qrender().mmm.length);
			else 	// for PAL we have 25 fps.
				iNoOfFramesNeeded = (uint)(25.0 * m_xmlQRender.qrender().mmm.length);
		}
	}
	// Now let us take care of the background ...
}
/**
 ** <qrender  [tmpdir="<path>"] [resolution="<WxH>"] [format="pal"|"ntsc"]
 **       [images="ppm"|"png"|"jpg"] [successive="0"|"1"] 
 **       [background="#rrggbb"|"<filename>"] 
 **       [startframe="<num>"]|[offset="<sec>"] [frames="<num>"]|[length="<sec>"] 
 **       [output="<filename>"]>
 **    <clip x="<pos>" y="<pos>" vob="<filename>"
 **       [startframe="<num>"]|[offset="<sec>"] [frames="<num>"]|[length="<sec>"] 
 **      [width="<int>"] [height="<int>"] [mask="<filename>"] />
 **   [<clip ... />]
 ** </qrender>
 **/


void QRender::buildMenuMovie()
{

}

void QRender::readStatus ()
{

}


void QRender::parseCommandLine (int argc, char **argv)
{
	int t, iX, iY, iWidth, iHeight;
	char *pCurrentArgument;
	QRenderOptions *pOptions = new QRenderOptions;
	iX = iY = iWidth = iHeight = 0;

	m_listInputOptions.append(pOptions);
	for (t=0;t<argc;t++)	{
		pCurrentArgument = argv[t];
		if ((strcmp(pCurrentArgument, "--help") == 0) || (strcmp(pCurrentArgument, "-H") == 0) )	{
			usage ();
			return;
		}
		if ((strcmp(pCurrentArgument, "--version") == 0) || (strcmp(pCurrentArgument, "-v") == 0) )	{
			printf ("Version: QRender %s built date %s http://qdvdauthor.sf.net\n", QRENDER_VERSION, __DATE__);
			return;
		}
		else if ((strcmp(pCurrentArgument, "--verbose") == 0) || (strcmp(pCurrentArgument, "-V") == 0) )	{
			m_iVerbosityLevel = QString (argv[++t]).toInt();
		}
		else if ((strcmp(pCurrentArgument, "--xml-file") == 0) || (strcmp(pCurrentArgument, "-f") == 0) )	{
			// Okaye for the xml - file we also have to check the status file and avoid unnecessary operations
			readXml (QString (argv[++t]));
			readStatus ();
		}
		else if ((strcmp(pCurrentArgument, "--input") == 0) || (strcmp(pCurrentArgument, "-i") == 0) )	{
			// Okay remember that the -i is the last in row for the options.
			// After that the new options are for the next -i
			m_listInputOptions.last()->qsFileName = QString (argv[++t]);
			pOptions = new QRenderOptions;
			m_listInputOptions.append(pOptions);
		}
		else if ((strcmp(pCurrentArgument, "--output") == 0) || (strcmp(pCurrentArgument, "-o") == 0) )	{
			m_listInputOptions.last()->qsFileName = QString (argv[++t]);
			m_pOutputOptions = m_listInputOptions.last();
			pOptions = new QRenderOptions;
			m_listInputOptions.append(pOptions);
		}
		else if ((strcmp(pCurrentArgument, "--mirror") == 0) || (strcmp(pCurrentArgument, "-m") == 0) )	{
			m_listInputOptions.last()->bMirror = true;
		}
		else if ((strcmp(pCurrentArgument, "--flip") == 0) || (strcmp(pCurrentArgument, "-F") == 0) )	{
			m_listInputOptions.last()->bFlip = true;
		}
		else if ((strcmp(pCurrentArgument, "--rotate") == 0) || (strcmp(pCurrentArgument, "-r") == 0) )	{
			m_listInputOptions.last()->fRotate = QString(argv[++t]).toFloat();
		}
		else if ((strcmp(pCurrentArgument, "--background") == 0) || (strcmp(pCurrentArgument, "-b") == 0) )	{
			m_listInputOptions.last()->colorBackground = QColor (argv [++t]);
		}
		else if ((strcmp(pCurrentArgument, "--quality") == 0) || (strcmp(pCurrentArgument, "-q") == 0) )	{
			m_listInputOptions.last()->fQuality = QString (argv[++t]).toFloat();
		}
		else if ((strcmp(pCurrentArgument, "--extract") == 0) || (strcmp(pCurrentArgument, "-e") == 0) )	{
			m_listInputOptions.last()->rectExtract = QRect (iX, iY, iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--mask") == 0) || (strcmp(pCurrentArgument, "-k") == 0) )	{
			m_listInputOptions.last()->qsMask = QString (argv[++t]);
		}
		else if ((strcmp(pCurrentArgument, "--maskoffset") == 0) || (strcmp(pCurrentArgument, "-K") == 0) )	{
			m_listInputOptions.last()->pointOffset = QPoint (iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--monochrome") == 0) || (strcmp(pCurrentArgument, "-M") == 0) )	{
			m_listInputOptions.last()->bMonochrome = true;
		}
		else if ((strcmp(pCurrentArgument, "--brightness") == 0) || (strcmp(pCurrentArgument, "-B") == 0) )	{
			m_listInputOptions.last()->fBrightness = QString (argv[++t]).toFloat();
		}
		else if ((strcmp(pCurrentArgument, "--saturation") == 0) || (strcmp(pCurrentArgument, "-S") == 0) )	{
			m_listInputOptions.last()->fSaturation = QString (argv[++t]).toFloat();
		}
		else if ((strcmp(pCurrentArgument, "--hue") == 0) || (strcmp(pCurrentArgument, "-U") == 0) )	{
			m_listInputOptions.last()->fHue = QString (argv[++t]).toFloat();
		}
		else if ((strcmp(pCurrentArgument, "--resize") == 0) || (strcmp(pCurrentArgument, "-R") == 0) )	{
			m_listInputOptions.last()->sizeResize = QSize (iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--resample") == 0) || (strcmp(pCurrentArgument, "-E") == 0) )	{
			m_listInputOptions.last()->sizeResample = QSize (iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--sample") == 0) || (strcmp(pCurrentArgument, "-P") == 0) )	{
			m_listInputOptions.last()->sizeSample = QSize (iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--scale") == 0) || (strcmp(pCurrentArgument, "-s") == 0) )	{
			m_listInputOptions.last()->sizeScale = QSize (iWidth, iHeight);
		}
		else if ((strcmp(pCurrentArgument, "--transparent") == 0) || (strcmp(pCurrentArgument, "-t") == 0) )	{
			m_listInputOptions.last()->colorTransparent = QColor (argv[++t]);
		}
	}
	//m_listInputOptions.remove(m_pOutputOptions);	// remove the output options from the options for the input image   //ooo
	m_listInputOptions.removeOne(m_pOutputOptions);	// remove the output options from the options for the input image  //xxx
}

void QRender::readXml (QString qsXmlFile)
{
	m_xmlQRender.clear ();
	m_xmlQRender.readXml (qsXmlFile);
}

void QRender::usage ()
{
	printf ("Version: QRender %s built date %s http://qdvdauthor.sf.net\n", QRENDER_VERSION,  __DATE__);
	printf ("Copyright: Copyright (C) 2005 Varol Okan\n\n");
	
	printf ("Usage: qrender  [options ...] -i file [ [options ...] -i file ...] [options ...] -o file\n\n");
	
	printf ("Options: \n\n");
	
	printf ("-V --verbose level          print debugging information\n");
	printf ("-H --help                   print program options\n");
	printf ("-v --version                print program version and exits.\n");
	printf ("-m --mirror                 mirror the image in the vertical direction\n");
	printf ("-F --flip                   flip image in the horizontal direction\n");
	printf ("-r --rotate      degrees    Rotate the image\n");
	printf ("-b --background  color      background color\n");
	printf ("-q --quality     value      JPEG/MIFF/PNG compression level\n");
	printf ("-e --extract     geometry   extract area from image\n");
	printf ("-k --mask        filename   set the image clip mask\n");
	printf ("-K --maskoffset  x:y        set the image clip mask offset\n");
	printf ("-M --monochrome             transform image to black and white\n");
	printf ("-B --brightness  value      vary the brightness, saturation, and hue\n");
	printf ("-S --stauration  value      vary the brightness, saturation, and hue\n");
	printf ("-U --hue         value      vary the brightness, saturation, and hue\n");
//	printf ("-R --resize      geometry   resize the image\n");
//	printf ("-E --resample    geometry   change the resolution of an image\n");
//	printf ("-P --sample      geometry   scale image with pixel sampling\n");
	printf ("-s --scale       size       scale the image\n");
	printf ("-t --transparent color      make this color transparent within the image\n\n");
	
	printf ("-f --xml-file               xml control file.\n");
	printf ("-i --input                  input image.\n");
	printf ("-o --output                 output image.\n");


	printf("QRender can take multiple input files and process them all in one piece, sequencially.\n");
	printf("In front of each input file you can define the options which are applied to only the following input file.\n\n");
	
	printf ("QRender was developed to fullfill four main tasks.\n");
	printf ("  1) extract images from movie clips\n");
	printf ("  2) compose a output image consisting of multiple input images for the creation of a DVDMenu.\n");
	printf ("  3) create a slideshow from multiple input images.\n");
	printf ("  4) Automatic crash recovery and continuation where left off.\n");

/*	printf ("-s --resize geometry     resize the image\n");
	printf ("-g --geometry geometry   Sets the geometry (X:Y:WIDTH:HEIGHT)\n");
	printf ("-a --append              append an image sequence\n");
	printf ("-m --matrix matrix       affine transform matrix\n");
	printf ("-crop geometry       preferred size and location of the cropped image\n");
	printf ("-depth value         image depth\n");
	printf ("-antialias           remove pixel-aliasing\n");
	printf ("-dither              apply Floyd/Steinberg error diffusion to image\n");
	printf ("-equalize            perform histogram equalization to an image\n");
	printf ("-fill color          color to use when filling a graphic primitive\n");
	printf ("-shave geometry      shave pixels from the image edges\n");
	printf ("-shear geometry      slide one edge of the image along the X or Y axis\n");
	printf ("-size geometry       width and height of image\n");
*/
}


