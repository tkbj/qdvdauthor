project(qrender)

add_subdirectory(engine)
add_subdirectory(ffmpeg-0.6.3)

SET(qrender_engine_SOURCES
   ${CMAKE_SOURCE_DIR}/qrender/engine/ffmpeg_enc.cpp
   #${CMAKE_SOURCE_DIR}/qrender/engine/test.cpp
)

SET(qrender_engine_HEADERS
   #${CMAKE_SOURCE_DIR}/qrender/engine/colorspace.h
   ${CMAKE_SOURCE_DIR}/qrender/engine/ffmpeg_enc.h
)

SET(qrender_1_SOURCES ${qrender_engine_SOURCES}
   encoder.cpp
   filter.cpp
   logger.cpp
   main.cpp
   parser.cpp
   render.cpp
   server.cpp
   slideshow.cpp
   utils.cpp
   #../qdvdauthor/xml_slideshow.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/xml_slideshow.cpp
)

SET(qrender_1_HEADERS ${qrender_engine_HEADERS}
   encoder.h
   filter.h
   logger.h
   parser.h
   render.h
   server.h
   shared.h
   slideshow.h
   utils.h
   #../qdvdauthor/xml_slideshow.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/xml_slideshow.h
)

#QT4_WRAP_CPP(qrender_1_HEADERS_MOC ${qrender_1_HEADERS})
QT5_WRAP_CPP(qrender_1_HEADERS_MOC ${qrender_1_HEADERS})

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
#include_directories(${Qt5Network_INCLUDE_DIRS})

ADD_EXECUTABLE(qrender ${qrender_1_SOURCES} 
    ${qrender_1_HEADERS_MOC})
    
#TARGET_LINK_LIBRARIES(qrender ${QT_QTCORE_LIBS})	#ooo
#TARGET_LINK_LIBRARIES(qrender ${QT_LIBRARIES})
#TARGET_LINK_LIBRARIES(qrender ${QT_LIBRARIES} ${FFMPEG_LIBRARIES} ${CMAKE_DL_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${xine_libs} X11)
TARGET_LINK_LIBRARIES(qrender Qt5::Widgets Qt5::Xml xine X11 Qt5::Network ${FFMPEG_LIBRARIES})
#link_directories(${FFMPEG_LIBRARIES})
message(STATUS "Found qrender*:")

INSTALL( TARGETS qrender DESTINATION bin)
