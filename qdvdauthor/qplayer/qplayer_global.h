#ifndef QPLAYER_GLOBAL_H
#define QPLAYER_GLOBAL_H

#define STATIC_TEMP "/tmp/"

// Here we define the media player backends
// Xine : http://xine.sf.net
#define ENGINE_XINE    0
// MPlayer : http://mplayerhq.hu
#define ENGINE_MPLAYER 1
// VideoLanClient : http://www.videolan.org
#define ENGINE_VLC     2

// Here we define the aspect ratios
#define ASPECT_AUTO    0
#define ASPECT_34      1
#define ASPECT_169     2
#define ASPECT_SQUARE  3

// These two variables define the color to use for the QuickDirectory (on the left side) buttons.
#define COLOR_QUICK_SELECTED 250, 150, 50
#define COLOR_QUICK_NORMAL   170, 170, 255

// This defines the width of the frame, which should be ignored for the drawing of the start/stop marker.
#define XOFFSET 2

// The maximal number of History files to be stored in the top combo box.
#define MAX_HISTORY_FILES 5

#endif // QPLAYER_GLOBAL_H


