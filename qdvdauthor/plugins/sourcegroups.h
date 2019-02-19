//Added by qt3to4:
//#include <Q3ValueList>	//oxx

#ifndef __PLUGIN_SOURCE_GROUPS_H__
#define __PLUGIN_SOURCE_GROUPS_H__

namespace Plugin
{

class SourceGroup
{
  public:
    enum type       { NTSC, PAL };
    enum filterType { FadeIn, CrossFade, FadeOut };

    class SourceEntry  {
    public:
      SourceEntry ( ) { iLength=0; bVidSource=false; };
      QString    qsFileName;
      QString    qsKenBurns;
      bool       bVidSource;
      unsigned long iLength; // in MSec
    };
             SourceGroup ( ) { enType=NTSC; bSlideshow=false; bCreateChapterMenu=false; iFlags=2; };
    virtual ~SourceGroup ( ) {
      QList<SourceEntry *>::iterator it=listFiles.begin ( );	//oxx
      while ( it != listFiles.end ( ) )
        delete *it++;
    };

  public:
    // General :
    type          enType;
    QString       qsGroupName; // could be different then pEntry->qsDisplayName
    bool          bCreateChapterMenu;
    QDateTime     dateStart;   // The starting date of the Slideshow / Videos
    QDateTime     dateEnd;     // The ending date of the Slideshow  / Videos

    QList<SourceEntry *> listFiles;		//oxx
    QString     qsScreenshot;
    bool        bSlideshow;
    int         iFilterLength;
    QString     qsBackground;
    QStringList listFilter;
    QStringList listAudio;
    uint        iFlags;  // For Slideshow : holds Format, IntroPage, bImgBkgImg
}; // end class SourceGroup

}; // end namespace Plugin

#endif // __PLUGIN_SOURCE_GROUPS_H__ 

