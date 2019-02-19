/****************************************************************************
  ** some defines for XML file creation 
  **
  **   Created : 
  **        by : Varol Okan using kate editor
  ** Copyright : (c) Varol Okan
  ** License   : GPL v 2.0
  **
  **
  ****************************************************************************/
#ifndef XML_DVD_H
#define XML_DVD_H

// Here is the doc type for the QDVDAuthor application.
#define XML_DOCTYPE "QDVDAuthor"
#define DVD_PROJECT "QDVDProject"

#define THE_PROJECT_NAME "ProjectName"

// The following version number is used for the xml - ProjectFile
// Version 1.1 RC3 = 1. 01 03
// Note the minor version is one behind the RC version
#define QDVDAUTHOR_VERSION                     "XMLVersion"
#define QDVDAUTHOR_VERSION_NUMBER              "2.0301-010"

// Here we goof around with the Templates
// Nodes
#define TEMPLATE_VERSION_NUMBER                1.3
#define TEMPLATE_DOCTYPE                       "QDVDAuthor"
#define XML_TEMPLATE                           "QDVDTemplate"
#define TEMPLATE_DATA                          "DVDTemplateData"
#define TEMPLATE_VERSION                       "XMLVersion"
#define TEMPLATE_AUTHOR                        "Author"
#define TEMPLATE_CONTACT                       "Contact"
#define TEMPLATE_HEADER                        "Header"
#define TEMPLATE_DESCRIPTION                   "TemplateDescription"
#define TEMPLATE_CATEGORY                      "TemplateCategory"
#define TEMPLATE_COMMENT                       "TemplateComment"
#define TEMPLATE_DROPZONE                      "DropZone"

// Tags
#define TEMPLATE_NAME                          "TemplateName"
#define TEMPLATE_DROPZONE_X                    "X"
#define TEMPLATE_DROPZONE_Y                    "Y"
#define TEMPLATE_DROPZONE_WIDTH                "Width"
#define TEMPLATE_DROPZONE_HEIGHT               "Height"
#define TEMPLATE_DROPZONE_NUMBER               "Number"
#define TEMPLATE_DROPZONE_TYPE                 "Type"

// Here are the defines for qdvdauthor.
#define DVDAUTHOR                               "DVDAuthor"
#define DVDAUTHOR_TEMP_PATH                     "TempPath"
#define DVDAUTHOR_ZONE                          "Zone"
#define DVDAUTHOR_PROJECT_PATH                  "ProjectPath"
#define DVDAUTHOR_CURRENT_PATH                  "CurrentPath"
#define DVDAUTHOR_ROOT_VMGM                     "MainVMGM"
#define DVDAUTHOR_ROOT_MENU                     "DVDMenu"
#define DVDAUTHOR_MENU                          "MenuVariables"
#define DVDAUTHOR_PREVIEW                       "PreviewVariables"
#define DVDAUTHOR_DVDLAYOUT                     "DVDLayout"

// Here are the defines for teh DVDMenu - Interface.
//#define DVD_MENU                                "DVDMenu"
#define MENU_NAME                               "MenuName"
#define MENU_PAUSE                              "MenuPause"
#define MENU_PRE                                "MenuPre"
#define MENU_POST                               "MenuPost"
#define MENU_INTRO                              "MenuIntro"
#define MENU_EXTRO                              "MenuExtro"
#define MENU_SOUND                              "MenuSound"
#define MENU_MOVIE                              "MenuMovie"
#define MENU_BACKGROUND                         "MenuBackground"
#define MENU_OFFSET                             "MenuOffset"
#define MENU_DURATION                           "MenuDuration"
#define MENU_ANIMATION                          "MenuAnimation"
#define MENU_HIGHLIGHTED_MASK                   "MenuHighlightedMask"
#define MENU_SELECTED_MASK                      "MenuSelectedMask"
#define MENU_MASK_COLORS                        "MaskColors"
#define MENU_RESIZE_ALGO                        "ResizeAlgo"
#define MENU_STRETCH_TYPE                       "StretchType"
#define MENU_OFFSET_X                           "OffsetX"
#define MENU_OFFSET_Y                           "OffsetY"
#define MENU_FORMAT                             "MenuFormat"
#define MENU_WIDTH                              "MenuWidth"
#define MENU_HEIGHT                             "MenuHeight"
#define MENU_RATIO                              "MenuRatio"
#define MENU_TITLESET_NR                        "TitlesetNr"
#define MENU_MENU_NR                            "MenuNr"

// This one is used for Templates. If nothing has been put into a DropZone, ignore.
#define IGNORE_OBJECT                           "IgnoreObject"

#define ANIMATION_OBJECT                        "AnimationObject"

// Here are the defines for the MenuObject derived classes ...
#define MENU_OBJECT                             "MenuObject"
//#define MENU_OBJECT_FRAME                       "FrameObject"
#define MENU_OBJECT_TEXT                        "TextObject"
#define MENU_OBJECT_IMAGE                       "ImageObject"
#define MENU_OBJECT_MOVIE                       "MovieObject"
#define MENU_OBJECT_BUTTON                      "ButtonObject"
#define MENU_OBJECT_COLLECTION                  "ObjectCollection"

// The MenuObject Variables ...
#define MENU_OBJECT_NAME                        "ObjectName"
#define MENU_OBJECT_EXTRACTION_PATH             "ObjectExtractionPath"

// The FrameObject - class
#define FRAME_OBJECT                            "FrameObject"
#define FRAME_OBJECT_COLOR                      "Color"
#define FRAME_OBJECT_WIDTH                      "Width"
#define FRAME_OBJECT_STYLE                      "Style"
#define FRAME_OBJECT_JOIN                       "Join"

// ShadowObject - class
#define SHADOW_OBJECT                           "ShadowObject"
#define SHADOW_OBJECT_TYPE                      "Type"
#define SHADOW_OBJECT_COLOR                     "Color"
#define SHADOW_OBJECT_TRANSPARENCY              "Transparency"
#define SHADOW_OBJECT_DISTANCE                  "Distance"
#define SHADOW_OBJECT_BLUR_RADIUS               "BlurRadius"
#define SHADOW_OBJECT_SUN_ANGLE                 "SunAngle"
#define SHADOW_OBJECT_SHEAR_X                   "ShearX"
#define SHADOW_OBJECT_SHEAR_Y                   "ShearY"
#define SHADOW_OBJECT_SCALE_X                   "ScaleX"
#define SHADOW_OBJECT_SCALE_Y                   "ScaleY"
#define SHADOW_OBJECT_CUT_OFF                   "CutOff"
#define SHADOW_OBJECT_CUT_OFF_RADIUS            "CutOffRadius"
#define SHADOW_OBJECT_COLOR_FADING              "ColorFading"
#define SHADOW_OBJECT_COLOR_FADE_TO             "ColorFadeTo"

// Overlay is an Mask from the mask library which
// masks out the underlying image/video
#define OVERLAY_OBJECT                           "OverlayObject"
#define OVERLAY_OBJECT_COLOR                     "Color"
#define OVERLAY_OBJECT_IS_MASK                   "isMask"
#define OVERLAY_OBJECT_OFFSET                    "Offset"
#define OVERLAY_OBJECT_ROTATE                    "Rotate"
#define OVERLAY_OBJECT_SHEAR_X                   "ShearX"
#define OVERLAY_OBJECT_SHEAR_Y                   "ShearY"
#define OVERLAY_OBJECT_SCALE_X                   "ScaleX"
#define OVERLAY_OBJECT_SCALE_Y                   "ScaleY"
#define OVERLAY_OBJECT_FILE_NAME                 "FileName"

// Mask is a subset of a shadow object.
#define MASK_OBJECT                             "MaskObject"
#define MASK_OBJECT_COLOR                       "Color"

// The TextObject - class
#define TEXT_OBJECT                             "TextObject"
#define TEXT_OBJECT_TEXT                        "Text"
#define TEXT_OBJECT_TEXT_ALIGN                  "TextAlign"
#define TEXT_OBJECT_STYLE_STRATEGY              "StyleStrategy"
#define TEXT_OBJECT_FONT                        "Font"
#define TEXT_OBJECT_BACKGROUND_COLOR            "BackgroundColor"
#define TEXT_OBJECT_FOREGROUND_COLOR            "ForegroundColor"

// The ImageObject - class
#define IMAGE_OBJECT                            "ImageObject"
#define IMAGE_OBJECT_FILE_NAME                  "FileName"
#define IMAGE_OBJECT_HIDDEN                     "Hidden"

// The MovieObject - class
#define MOVIE_OBJECT                            "MovieObject"
#define MOVIE_OBJECT_FILE_NAME                  "FileName"
#define MOVIE_OBJECT_OFFSET                     "Offset"
#define MOVIE_OBJECT_DURATION                   "Duration"
#define MOVIE_OBJECT_MENU_LENGTH                "MenuLength"
#define MOVIE_OBJECT_LOOP                       "Loop"
#define MOVIE_OBJECT_MSEC_PREVIEW               "MSecPreview"
#define MOVIE_OBJECT_SIZE                       "Size"

// The ObjectCollection - class
#define OBJECT_COLLECTION                       "ObjectCollection"
#define SELECTION_OBJECT                        "SelectionObject"

#define BUTTON_OBJECT                           "ButtonObject"
#define BUTTON_OBJECT_NAME                      "ButtonName"
#define BUTTON_OBJECT_NORMAL                    "NormalState"
#define BUTTON_OBJECT_SELECTED                  "SelectedState"
#define BUTTON_OBJECT_HIGHLIGHTED               "HighlightedState"
#define BUTTON_OBJECT_ACTION                    "Action"
#define BUTTON_OBJECT_PRE_ACTION                "PreAction"
#define BUTTON_OBJECT_UP                        "Up"
#define BUTTON_OBJECT_DOWN                      "Down"
#define BUTTON_OBJECT_LEFT                      "Left"
#define BUTTON_OBJECT_RIGHT                     "Right"
#define BUTTON_OBJECT_SOURCE_ENTRY              "SourceEntry"
#define BUTTON_OBJECT_MOVEABLE                  "Moveable"
#define BUTTON_OBJECT_MULTIPLE                  "PlayMe"
#define BUTTON_OBJECT_LOOP_MULTIPLE             "Loop"

// The ButtonTransition - class
#define BUTTON_TRANSITION                       "Transition"
#define BUTTON_TRANS_DURATION                   "Duration"
#define BUTTON_TRANS_USER_FILE                  "UserFile"
#define BUTTON_TRANS_FORMAT                     "Format"
#define BUTTON_TRANS_TYPE                       "Type"
#define BUTTON_TRANS_TRANSITION_NAME            "TransName"
#define BUTTON_TRANS_NAME                       "Name"
#define BUTTON_TRANS_RENDERED                   "Rendered"

// The SourceFileEntry - class
#define SOURCE_OBJECT                           "SourceEntry"
#define SOURCE_OBJECT_DISPLAY_NAME              "DisplayName"
#define SOURCE_OBJECT_PRE                       "Pre"
#define SOURCE_OBJECT_POST                      "Post"
#define SOURCE_OBJECT_IS_SOUND_SOURCE           "SoundSource"
#define SOURCE_OBJECT_INFO                      "SourceInfo"
#define SOURCE_OBJECT_FILE_NAME                 "File"
#define SOURCE_OBJECT_CHAPTERS                  "Chapters"
#define SOURCE_OBJECT_SIZE                      "Size"
#define SOURCE_OBJECT_RESOLUTION                "Resolution"
#define SOURCE_OBJECT_FPS                       "FPS"
#define SOURCE_OBJECT_RATIO                     "Ratio"
#define SOURCE_OBJECT_LENGTH                    "Length"
#define SOURCE_OBJECT_VIDEO_FORMAT              "Format"
#define SOURCE_OBJECT_VIDEO_CODEC               "Codec"
#define SOURCE_OBJECT_HAS_AUDIO                 "HasAudio"
#define SOURCE_OBJECT_AUDIO_FORMAT              "AudioFormat"
#define SOURCE_OBJECT_AUDIO_CODEC               "AudioCodec"
#define SOURCE_OBJECT_STATUS                    "Status"
#define SOURCE_OBJECT_BITS                      "Bits"
#define SOURCE_OBJECT_SAMPLE                    "Sample"
#define SOURCE_OBJECT_BPS                       "BPS"
#define SOURCE_OBJECT_PAUSE                     "Pause"
#define SOURCE_OBJECT_MSEC_OFFSET               "MSecPreview"
#define SOURCE_OBJECT_TITLESET                  "Titleset"
#define SOURCE_OBJECT_PROPERTIES                "Properties"
#define SOURCE_OBJECT_TRANSCODE                 "TranscodeInterface"
#define SOURCE_OBJECT_AUTOTRANSCODE             "AutoTranscode"
#define SOURCE_OBJECT_METASUBTITLES             "MetaSubtitles"
#define SOURCE_OBJECT_SUBTITLES                 "Subtitles"
#define SOURCE_OBJECT_AUDIO                     "Audio"

#define SOURCE_PROPERTIES_POST                  "PostCommands"
#define SOURCE_PROPERTIES_PRE                   "PreCommands"
#define SOURCE_PROPERTIES_PAUSE                 "Pause"

#define TRANSCODE_VIDEO_FORMAT                  "VideoFormat"
#define TRANSCODE_VIDEO_TYPE                    "VideoType"
#define TRANSCODE_RESOLUTION                    "Resolution"
#define TRANSCODE_VIDEO_BITRATE                 "VideoBitrate"
#define TRANSCODE_FRAME_RATE                    "FrameRate"
#define TRANSCODE_RATIO                         "Ratio"
#define TRANSCODE_AUDIO_FORMAT                  "AudioFormat"
#define TRANSCODE_AUDIO_TYPE                    "AudioType"
#define TRANSCODE_AUDIO_BITRATE                 "AudioBitrate"
#define TRANSCODE_SAMPLE_RATE                   "SampleRate"
#define TRANSCODE_STARTING_AT                   "StartingAt"
#define TRANSCODE_ENDING_AT                     "EndingAt"
#define TRANSCODE_REMUX_ONLY                    "RemuxOnly"
#define TRANSCODE_SUBTITLE_FROM_META            "SubtitleFromMetaInfo"

#define SUBTITLES_SUBTITLE_ENTRY                "Entry"
#define SUBTITLES_NUMBER                        "SubtitleNumber"
#define SUBTITLES_LANG                          "Lang"
#define SUBTITLES_FILE_NAME                     "File"
#define SUBTITLES_SUBTITLE_FILE                 "TempFile"
#define SUBTITLES_FONT                          "Font"
#define SUBTITLES_FIT                           "Fit"
#define SUBTITLES_STATE                         "State"
#define SUBTITLES_TEXT_BASED                    "TextBased"
#define SUBTITLES_TEXT_FONT                     "TextFont"
#define SUBTITLES_TEXT_SIZE                     "TextSize"
#define SUBTITLES_OUTLINE                       "Outline"
#define SUBTITLES_RECT                          "Rect"
#define SUBTITLES_VIDEO_SIZE                    "size"
#define SUBTITLES_ALIGNMENT                     "Alignment"
#define SUBTITLES_COLORS                        "Colors"
#define SUBTITLES_SUBTITLE_INDEX                "Index"
#define SUBTITLES_SUBTITLE_START                "Start"
#define SUBTITLES_SUBTITLE_STOP                 "Stop"
// outdated now stored in COLOR (2007/11/07)
#define SUBTITLES_TRANSPARENCY                  "Transparency"

#define AUDIO_NUMBER                            "AudioNumber"
#define AUDIO_LANGUAGE                          "Lang"
#define AUDIO_FILE_NAME                         "FileName"

// The Modifier - class - variables :
#define MODIFIERS                               "Modifiers"
#define MODIFIERS_ROTATE                        "Rotate"
#define MODIFIERS_ZOOM                          "Zoom"
#define MODIFIERS_SCALEX                        "ScaleX"
#define MODIFIERS_SCALEY                        "ScaleY"
#define MODIFIERS_SHEARX                        "ShearX"
#define MODIFIERS_SHEARY                        "ShearY"
#define MODIFIERS_START_FRAME                   "StartFrame"
#define MODIFIERS_STOP_FRAME                    "StopFrame"
#define MODIFIERS_TRANSPARENCY                  "Transparency"

// The ImageManipulator - class
#define MANIPULATOR                             "Manipulator"
#define MANIPULATOR_FILE_NAME                   "FileName"
#define MANIPULATOR_SHOW_BACKGROUND             "ShowBackground"
#define MANIPULATOR_STARTX                      "StartX"
#define MANIPULATOR_STARTY                      "StartY"
#define MANIPULATOR_KENBURNS_X1                 "KenBurnsX1"
#define MANIPULATOR_KENBURNS_Y1                 "KenBurnsY1"
#define MANIPULATOR_KENBURNS_X2                 "KenBurnsX2"
#define MANIPULATOR_KENBURNS_Y2                 "KenBurnsY2"
#define MANIPULATOR_XRES                        "XRes"
#define MANIPULATOR_YRES                        "YRes"
#define MANIPULATOR_TRANSFORMATION_MODE	        "TransformationMode"
#define MANIPULATOR_ASPECT_RATIO                "AspectRatio"
#define MANIPULATOR_RED                         "Red"
#define MANIPULATOR_GREEN                       "Green"
#define MANIPULATOR_BLUE                        "Blue"
#define MANIPULATOR_BRIGHTNESS                  "Brightness"
#define MANIPULATOR_COLOR_KEY                   "ColorKey"
#define MANIPULATOR_COLOR                       "Color"
#define MANIPULATOR_DELTA                       "Delta"

// Vars for the DVDLayout tab
#define LAYOUT_ZOOM                             "Zoom"
#define LAYOUT_GRID                             "Grid"
#define LAYOUT_TYPE                             "Type"
#define LAYOUT_CONTAINER                        "Container"
#define LAYOUT_OBJECT_RECT                      "Rect"

// QRect variables ...
#define RECT_X                                  "X"
#define RECT_Y                                  "Y"
#define RECT_WIDTH                              "Width"
#define RECT_HEIGHT                             "Height"

#endif // XML_DVD_H

