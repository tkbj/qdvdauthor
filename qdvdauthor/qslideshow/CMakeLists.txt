project(qslideshow)

add_subdirectory(images)

SET(qdvdauthor_SOURCES
   ${CMAKE_SOURCE_DIR}/qdvdauthor/rgba.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/xml_slideshow.cpp
   #${CMAKE_SOURCE_DIR}/qdvdauthor/dialogimage2.cpp	#xxx
)

SET(qdvdauthor_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/rgba.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/xml_dvd.h		#xxx
   ${CMAKE_SOURCE_DIR}/qdvdauthor/xml_slideshow.h
   #${CMAKE_SOURCE_DIR}/qdvdauthor/dialogimage2.h	#xxx
   #${CMAKE_SOURCE_DIR}/qdvdauthor/dialogshadow.h	#xxx
)

SET(qslideshow_SOURCES ${qdvdauthor_SOURCES}
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogimage.cpp		#xxx
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogslide.cpp		#xxx
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogcreate.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogfilter.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/filterline.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/imageline.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/imagepreview.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/modifiers2.cpp   
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/slidemain.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/slidertime.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/soundline.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/uidialogcreate.cpp		#xxx
)

SET(qslideshow_HEADERS ${qdvdauthor_HEADERS}
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogimage.h		#xxx
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogimage.ui.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogslide.h		#xxx
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogslide.ui.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogcreate.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogfilter.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/filterline.h   
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/imageline.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/modifiers2.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/imagepreview.h   
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/slidertime.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/soundline.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/uidialogcreate.h		#xxx
)

SET(qslideshow_FORMS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogimage.ui
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/dialogslide.ui
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qslideshow/uidialogcreate.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/uidialogimage.ui		#xxx
   #${CMAKE_SOURCE_DIR}/qdvdauthor/uidialogshadow.ui		#xxx
)

SET(qslideshow_RESOURCES images.qrc)

QT5_WRAP_CPP(qslideshow_HEADERS_MOC ${qslideshow_HEADERS})
QT5_WRAP_UI(qslideshow_FORMS_HEADERS ${qslideshow_FORMS})
QT5_ADD_RESOURCES(qslideshow_RESOURCES_RCC ${qslideshow_RESOURCES})

ADD_EXECUTABLE(qslideshow ${qslideshow_SOURCES} 
    ${qslideshow_HEADERS_MOC} 
    ${qslideshow_FORMS_HEADERS}
    ${qslideshow_RESOURCES_RCC}
    )
    
#TARGET_LINK_LIBRARIES(qslideshow ${QT_LIBRARIES})
#TARGET_LINK_LIBRARIES(qslideshow ${QT_LIBRARIES} ${QT_LIBRARY_DIR} ${FFMPEG_LIBRARIES} ${VLC_LIBRARIES} ${CMAKE_DL_LIBS} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${xine_libs} X11)
TARGET_LINK_LIBRARIES(qslideshow Qt5::Widgets Qt5::Xml xine X11 Qt5::Network ${FFMPEG_LIBRARIES})
link_directories(${FFMPEG_LIBRARIES})
message(STATUS "Found qslideshow:")

INSTALL( TARGETS qslideshow DESTINATION bin)

