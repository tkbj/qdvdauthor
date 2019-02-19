# define minimum cmake version
cmake_minimum_required(VERSION 2.8.11)
project(qdvdauthor)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
# Tell CMake to run moc when necessary:
#set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#SET(CMAKE_BUILD_TYPE "Debug")	#ooo
#SET(CMAKE_CXX_FLAGS "-O0 -ggdb")	#ooo
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")	#ooo

# first, add "-ggdb -g3" flag to generate more debug info
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -ggdb")

#option(DEBUG "Enable debugging infrastructure" OFF)
#if (DEBUG)
#	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -g3 -ggdb -O0")
#	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -g3 -ggdb -O0")
#endif (DEBUG)

#set( CMAKE_SHARED_LINKER_FLAGS "-Wl,--output-def,SDL_ffmpeg.def, --with-qt3support" )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -ldl -lvlc")

# Executables fail to build with Qt 5 in the default configuration without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

SET( command_1 "#ifndef MEDIA_CONFIG_H" )
SET( command_2 "#define MEDIA_CONFIG_H" )
#SET( command_3 "#define MPLAYER_SUPPORT $$(WITH_MPLAYER_SUPPORT)" )
SET( command_3 "#define MPLAYER_SUPPORT 1" )
#SET( command_4 "#define XINE_SUPPORT $$(WITH_XINE_SUPPORT)" )
SET( command_4 "#define XINE_SUPPORT 1" )
#SET( command_5 "#define VLC_SUPPORT $$(WITH_VLC_SUPPORT)" )
SET( command_5 "#define VLC_SUPPORT 1" )
SET( command_6 "#endif	//MEDIA_CONFIG_H" )

# ${command}( hi )
FILE( WRITE ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/MEDIA_CONFIG.h 
"${command_1}" "\n"
"${command_2}" "\n\n"
"${command_3}" "\n"
"${command_4}" "\n"
"${command_5}" "\n\n"
"${command_6}" "\n"
) # writes "MESSAGE( hi )" to ./temp
#FILE( WRITE ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/MEDIA_CONFIG.h "${command_2}" )
#INCLUDE( temp )                       # ./temp is an unsafe temporary file...
#FILE( REMOVE temp )

# enable warnings
ADD_DEFINITIONS( -Wall )

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
#SET( QT_USE_QT3SUPPORT TRUE )
#SET( QT_USE_QTXML TRUE )

#SET (FFMPEG_DIRS
#  $ENV{FFMPEG_DIR}
#  $ENV{FFMPEG_DIR}/libavcodec
#  $ENV{FFMPEG_DIR}/libavdevice
#  $ENV{FFMPEG_DIR}/libavfilter
#  $ENV{FFMPEG_DIR}/libavformat
#  $ENV{FFMPEG_DIR}/libavutil
#  $ENV{FFMPEG_DIR}/libavswresample
#  $ENV{FFMPEG_DIR}/libswscale
#  )

#add_library(
#SDL_ffmpeg
#SHARED
#${SDL_FFMPEG_SOURCE_DIR}/src/SDL_ffmpeg.c
#)

#search packages used by KDE and Qt
#changed because of: Qt4 qdbusxml2cpp was not found
#find_package(Qt5 COMPONENTS QtXml REQUIRED)
#find_package(Qt5)
find_package (Qt5Core REQUIRED)
#find_package (Qt5Core)
find_package (Qt5Network REQUIRED)
find_package( Qt5Gui REQUIRED)
#find_package (Qt5Gui)
# Widgets finds its own dependencies (QtGui and QtCore).
find_package (Qt5Widgets REQUIRED)
# <QtXml/QDomElement> -> <QDomElement>
find_package (Qt5Xml)
find_package(Qt5PrintSupport REQUIRED)
#find_package(Qt5Declarative)
#FIND_PACKAGE(FFmpeg REQUIRED)
find_package(X11 REQUIRED)

#find_package( avformat )
#find_package( avcodec )
#find_package( avutil )
#find_package( swscale )

#include_directories(	
#${SDL_INCLUDE_DIR}
#${AVFORMAT_INCLUDE_DIR}
#${AVCODEC_INCLUDE_DIR}
#${AVUTIL_INCLUDE_DIR}
#${SWSCALE_INCLUDE_DIR}
#${SDL_FFMPEG_SOURCE_DIR}/include/SDL
#)

set(CMAKE_VERBOSE_MAKEFILE ON)

#FFMPEG_FIND(LIBAVFORMAT avformat avformat.h)
#FFMPEG_FIND(LIBAVDEVICE avdevice avdevice.h)
#FFMPEG_FIND(LIBAVCODEC avcodec avcodec.h)
#FFMPEG_FIND(LIBAVUTIL avutil avutil.h)
#FFMPEG_FIND(LIBSWSCALE swscale swscale.h)

SET(FFMPEG_FOUND "NO")
IF (FFMPEG_LIBAVFORMAT_FOUND AND
FFMPEG_LIBAVDEVICE_FOUND AND
FFMPEG_LIBAVCODEC_FOUND AND
FFMPEG_LIBAVUTIL_FOUND AND
FFMPEG_LIBSWSCALE_FOUND
)
ELSE ()
 
MESSAGE(STATUS "Could not find FFMPEG")
 
ENDIF()

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})
link_directories(${QT_LIB_DIR})
# The Qt5Widgets_INCLUDES also includes the include directories for dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Declarative_INCLUDE_DIRS} ${X11_INCLUDE_DIR})

#find_library(AVFORMAT_LIBRARY avformat PATHS /usr/local/lib /usr/lib /usr/lib64 DOC "avformat library" REQUIRED)
#find_library(AVCODEC_LIBRARY avcodec PATHS /usr/local/lib /usr/lib /usr/lib64 DOC "avcodec library" REQUIRED)
#find_library(AVUTIL_LIBRARY avutil PATHS /usr/local/lib /usr/lib /usr/lib64 DOC "avutil library" REQUIRED)

message(STATUS "Found AVFORMAT: " ${AVFORMAT_LIBRARY}
" Found AVCODEC: " ${AVCODEC_LIBRARY}
" Found AVUTIL: " ${AVUTIL_LIBRARY}
${FFMPEG_LIBRARIES} ${FFMPEG_LIBAVFORMAT_LIBRARIES})

#link_directories(${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY})

find_path(FFMPEG_INCLUDE libavformat/avformat.h PATHS /usr/local/include /usr/include DOC "avformat include path" REQUIRED)

message(STATUS "Found FFMPEG: " ${FFMPEG_INCLUDE})

#include_directories(${FFMPEG_INCLUDE})

SET (FFMPEG_DIRS
  ${FFMPEG_INCLUDE}
  ${FFMPEG_INCLUDE}/libavcodec
  ${FFMPEG_INCLUDE}/libavdevice
  ${FFMPEG_INCLUDE}/libavfilter
  ${FFMPEG_INCLUDE}/libavformat
  ${FFMPEG_INCLUDE}/libavutil
  ${FFMPEG_INCLUDE}/libavswresample
  ${FFMPEG_INCLUDE}/libswscale
  )

SET (xine_libs
   xine
)   

#link_directories(${FFMPEG_DIRS})

message(STATUS "Found FFMPEG_DIRS: " ${FFMPEG_DIRS})

#add_library(
#SDL_ffmpeg
#SHARED
#${SDL_FFMPEG_SOURCE_DIR}/src/SDL_ffmpeg.c
#)

find_path(FFMPEG_AVCODEC_INCLUDE_DIR
    NAMES avcodec.h
    PATHS ${_FFMPEG_AVCODEC_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libavcodec
  )

find_path(FFMPEG_AVFORMAT_INCLUDE_DIR
    NAMES avformat.h
    PATHS ${_FFMPEG_AVFORMAT_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libavformat
  )

find_path(FFMPEG_SWSCALE_INCLUDE_DIR
    NAMES swscale.h
    PATHS ${_FFMPEG_SWSCALE_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libswscale
  )

find_library(FFMPEG_AVCODEC_LIBRARY
    NAMES avcodec
    PATHS ${_FFMPEG_AVCODEC_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib /usr/lib64
  )
  
find_library(FFMPEG_AVFORMAT_LIBRARY
    NAMES avformat
    PATHS ${_FFMPEG_AVFORMAT_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib /usr/lib64
  )

find_library(FFMPEG_SWSCALE_LIBRARY
    NAMES swscale
    PATHS ${_FFMPEG_SWSCALE_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib /sw/lib /usr/lib64
  )
  
  
message(STATUS "Found FFMPEG_AVCODEC_LIBRARY: " ${FFMPEG_AVCODEC_LIBRARY})
message(STATUS "Found FFMPEG_AVFORMAT_LIBRARY: " ${FFMPEG_AVFORMAT_LIBRARY})
message(STATUS "Found FFMPEG_SWSCALE_LIBRARY: " ${FFMPEG_SWSCALE_LIBRARY})

if (FFMPEG_AVCODEC_LIBRARY AND FFMPEG_AVFORMAT_LIBRARY AND FFMPEG_SWSCALE_LIBRARY)
   set(FFMPEG_FOUND TRUE)
endif (FFMPEG_AVCODEC_LIBRARY AND FFMPEG_AVFORMAT_LIBRARY AND FFMPEG_SWSCALE_LIBRARY)

message(STATUS "FFMPEG_FOUND: " ${FFMPEG_FOUND})

#if (FFMPEG_FOUND)
    set(FFMPEG_INCLUDE_DIRS
       #${FFMPEG_AVCODEC_INCLUDE_DIR}
       #${FFMPEG_AVFORMAT_INCLUDE_DIR}
       #${FFMPEG_SWSCALE_INCLUDE_DIR}
      ${CMAKE_SOURCE_DIR}/qrender/ffmpeg-0.6.3/include
      #/usr/include/ffmpeg063
    )
    set(FFMPEG_LIBRARIES
       #${FFMPEG_AVCODEC_LIBRARY}
       #${FFMPEG_AVFORMAT_LIBRARY}
       #${FFMPEG_SWSCALE_LIBRARY}
      ${CMAKE_SOURCE_DIR}/qrender/ffmpeg-0.6.3/lib/libavcodec.so
      ${CMAKE_SOURCE_DIR}/qrender/ffmpeg-0.6.3/lib/libavformat.so
      ${CMAKE_SOURCE_DIR}/qrender/ffmpeg-0.6.3/lib/libavutil.so
      ${CMAKE_SOURCE_DIR}/qrender/ffmpeg-0.6.3/lib/libswscale.so
      #/usr/lib/libavcodec.so
      #/usr/lib/libavformat.so
      #/usr/lib/libavutil.so
      #/usr/lib/libswscale.so
    )
#endif (FFMPEG_FOUND)

message(STATUS "FFMPEG_INCLUDE_DIRS: " ${FFMPEG_INCLUDE_DIRS})
message(STATUS "FFMPEG_LIBRARIES: " ${FFMPEG_LIBRARIES})

if (FFMPEG_FOUND)
    if (NOT FFMPEG_FIND_QUIETLY)
      message(STATUS "Found FFMPEG: ${FFMPEG_LIBRARIES}")
    endif (NOT FFMPEG_FIND_QUIETLY)
  else (FFMPEG_FOUND)
    if (FFMPEG_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find FFMPEG libavcodec, libavformat or libswscale")
    endif (FFMPEG_FIND_REQUIRED)
endif (FFMPEG_FOUND)

find_path(XINE_INCLUDE_DIR
    NAMES xineutils.h
    PATHS ${_XINE_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES xine
  )
  
find_library(XINE_LIBRARY
             NAMES xine
             PATHS ${XINE_LIBRARY_DIRS} /usr/lib64
             PATH_SUFFIXES xine
  )

message(STATUS "Found Xine: " " XINE_INCLUDE_DIR: " ${XINE_INCLUDE_DIR})
message(STATUS "Found Xine: " " XINE_LIBRARY_DIRS: " ${XINE_LIBRARY_DIRS} " XINE_LIBRARY: " ${XINE_LIBRARY} )

find_path(VLC_INCLUDE_DIR
    NAMES vlc.h
    PATHS ${_VLC_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES vlc
  )
  
find_library(VLC_LIBRARIES
             NAMES vlc
             PATHS ${VLC_LIBRARY_DIRS})

message(STATUS "Found vlc VLC_INCLUDE_DIR : " ${VLC_INCLUDE_DIR})
message(STATUS "Found vlc VLC_LIBRARIES : " ${VLC_LIBRARIES})

find_path(X11_INCLUDE_DIR
    NAMES Xlib.h
    PATHS ${_X11_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES X11
  )
  
find_library(X11_LIBRARIES
             NAMES X11
             PATHS ${X11_LIBRARY_DIRS} /usr/lib64
             #PATHS ${SFGUI_SEARCH_PATHS}
             PATH_SUFFIXES lib64
  )
  
find_library(XEXT_LIBRARIES
        NAMES Xext
        HINTS _Xext_ROOT_PATHS
        PATH_SUFFIXES bin lib64
)
  
message(STATUS "Found X11 X11_INCLUDE_DIR : " ${X11_INCLUDE_DIR})
message(STATUS "Found X11 X11_LIBRARIES : " ${X11_LIBRARIES})
message(STATUS "Found X11 XEXT_LIBRARIES : " ${XEXT_LIBRARIES})
#message(STATUS "Found X11 X11_LIBRARY : " ${X11_LIBRARY})
  
#link_directories(${FFMPEG_LIBRARIES})
#link_directories(${X11_LIBRARY_DIRS})
#include_directories(${FFMPEG_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} ${XINE_INCLUDE_DIR} ${VLC_INCLUDE_DIR} ${X11_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Qt5Widgets_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS} ${XINE_INCLUDE_DIR} ${VLC_INCLUDE_DIR} ${X11_INCLUDE_DIR})

#add_subdirectory(addons)
#add_subdirectory(configurator)
add_subdirectory(qdvdauthor)
add_subdirectory(qrender)

SET(qdvdauthor_RESOURCES_ images.qrc)

########### install files ###############

INSTALL(FILES qdvdauthor.desktop               DESTINATION  share/applications/)
INSTALL(FILES qdvdauthor.png                   DESTINATION  share/pixmaps/)
INSTALL(FILES CHANGELOG                        DESTINATION  share/doc/packages/qdvdauthor/)
INSTALL(FILES COPYING                          DESTINATION  share/doc/packages/qdvdauthor/)
INSTALL(FILES README                           DESTINATION  share/doc/packages/qdvdauthor/)
INSTALL(FILES TODO                             DESTINATION  share/doc/packages/qdvdauthor/)
INSTALL(FILES doc/html/en/audio_streams.html   DESTINATION  share/qdvdauthor/html/en/)
INSTALL(FILES doc/html/en/system_register.html DESTINATION  share/qdvdauthor/html/en/)
INSTALL(FILES silence.ac3                      DESTINATION  share/qdvdauthor/)
INSTALL(FILES silence.mp2                      DESTINATION  share/qdvdauthor/)
#INSTALL(FILES bin/dvd-slideshow        DESTINATION  share/qdvdauthor/ )
INSTALL(FILES bin/dvd-slideshow        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION  share/qdvdauthor/ )



