#project(qrender)

SET(qrender_SOURCES
   qrender.cpp
   startmenurender.cpp
   xml_qrender.cpp
)

SET(qrender_HEADERS
   qrender.h
   startmenurender.h
   xml_qrender.h
)

QT5_WRAP_CPP(qrender_HEADERS_MOC ${qrender_HEADERS})

ADD_EXECUTABLE(qrender ${qrender_SOURCES} 
    ${qrender_HEADERS_MOC} 
    )
    
TARGET_LINK_LIBRARIES(qrender Qt5::Widgets Qt5::Xml xine X11 Qt5::Network ${FFMPEG_LIBRARIES})

message(STATUS "Found qrender:")

INSTALL( TARGETS qrender DESTINATION bin)
