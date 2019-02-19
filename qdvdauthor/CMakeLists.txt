project(qdvdauthor)

add_subdirectory(images)
#add_subdirectory(plugins)
#add_subdirectory(qplayer)
#add_subdirectory(qrender)
add_subdirectory(qslideshow)

include_directories(
   ${CMAKE_SOURCE_DIR}/qrender
   ${CMAKE_SOURCE_DIR}/qrender/engine
   ${CMAKE_CURRENT_SOURCE_DIR}/qplayer
   #${CMAKE_CURRENT_SOURCE_DIR}/qplayer/engines
   ${CMAKE_CURRENT_SOURCE_DIR}/qrender
   #${CMAKE_CURRENT_SOURCE_DIR}/plugins/complexdvd
   #${CMAKE_CURRENT_SOURCE_DIR}/plugins/menuslide
   #${CMAKE_CURRENT_SOURCE_DIR}/plugins/simpledvd
   #${CMAKE_CURRENT_SOURCE_DIR}/plugins/testplugs
   )
   
SET(qrender_SRCS
   ${CMAKE_SOURCE_DIR}/qrender/encoder.cpp
   ${CMAKE_SOURCE_DIR}/qrender/filter.cpp
   ${CMAKE_SOURCE_DIR}/qrender/logger.cpp
   ${CMAKE_SOURCE_DIR}/qrender/main.cpp
   ${CMAKE_SOURCE_DIR}/qrender/parser.cpp
   ${CMAKE_SOURCE_DIR}/qrender/render.cpp
   ${CMAKE_SOURCE_DIR}/qrender/server.cpp
   ${CMAKE_SOURCE_DIR}/qrender/slideshow.cpp
   ${CMAKE_SOURCE_DIR}/qrender/utils.cpp
)

SET(qrender_HEAD
   ${CMAKE_SOURCE_DIR}/qrender/encoder.h
   ${CMAKE_SOURCE_DIR}/qrender/filter.h
   ${CMAKE_SOURCE_DIR}/qrender/logger.h
   ${CMAKE_SOURCE_DIR}/qrender/parser.h
   ${CMAKE_SOURCE_DIR}/qrender/render.h
   ${CMAKE_SOURCE_DIR}/qrender/server.h
   ${CMAKE_SOURCE_DIR}/qrender/shared.h
   ${CMAKE_SOURCE_DIR}/qrender/slideshow.h
   ${CMAKE_SOURCE_DIR}/qrender/utils.h
)

SET(qrender_engine_SOURCES
   ${CMAKE_SOURCE_DIR}/qrender/engine/ffmpeg_enc.cpp
   ${CMAKE_SOURCE_DIR}/qrender/engine/test.cpp
)

SET(qrender_engine_HEADERS
   ${CMAKE_SOURCE_DIR}/qrender/engine/colorspace.h
   ${CMAKE_SOURCE_DIR}/qrender/engine/ffmpeg_enc.h
)

set(qplayer_S0URCES
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/listviewfileitem.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediacreator.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediainfo.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediascanner.cpp
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/qplayer.cpp
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/qplayer_main.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/run.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/startstopcontrol.cpp
)
   
SET(qplayer_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/listviewfileitem.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediacreator.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediainfo.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/mediascanner.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/qplayer.h
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/qplayer_global.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/qplayer.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/run.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/startstopcontrol.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/xml_qplayer.h
)

SET(qplayer_FORMS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/dialogaboutqplayer.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/dialogaboutqplayer3.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/dialogconfigure.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/dialogconfigure3.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/dialogqxinesetup.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/uiqplayer.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/uiqplayer3.ui
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/uistartstopcontrol.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/uistartstopcontrol3.ui
)

SET(qplayer_engines_SOURCES
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogmplayersetup.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dummywidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kaffeineswidget.cpp
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kplayerengine.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kxinewidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mediainterface.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerinfo.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerwidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mrl.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qmplayerwidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qvlcwidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qxinewidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcinfo.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcwidget.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xineinfo.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xinewidget.cpp
)

SET(qplayer_engines_xine_SOURCES
         ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.cpp   #inserted
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kaffeineswidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kxinewidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qxinewidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xineinfo.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xinewidget.cpp
)

SET(qplayer_engines_mplayer_SOURCES
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogmplayersetup.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerinfo.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerwidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qmplayerwidget.cpp   
)

SET(qplayer_engines_vlc_SOURCES
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qvlcwidget.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcinfo.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcwidget.cpp
)

SET(qplayer_engines_HEADERS
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogmplayersetup.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dummywidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kaffeineswidget.h
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kplayerengine.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kxinewidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mediainterface.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerinfo.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerwidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mrl.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qmplayerwidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qvlcwidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qxinewidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcinfo.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcwidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xineinfo.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xinewidget.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xml_mplayer.h
)

SET(qplayer_engines_xine_HEADERS
         ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.h   #inserted
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kaffeineswidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/kxinewidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qxinewidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xineinfo.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xinewidget.h
)

SET(qplayer_engines_mplayer_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogmplayersetup.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerinfo.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/mplayerwidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qmplayerwidget.h   
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/xml_mplayer.h
)

SET(qplayer_engines_vlc_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/qvlcwidget.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcinfo.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/vlcwidget.h
)

SET(qplayer_engines_FORMS
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup3.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.ui
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/uidialogmplayersetup3.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/uidialogmplayersetup.ui
)

SET(qplayer_engines_xine_FORMS
    #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup3.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/qplayer/engines/dialogqxinesetup.ui
)

set(qrender_S0URCES
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/qrender.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/startmenurender.cpp
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/xml_qrender.cpp
)

SET(qrender_HEADERS
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/qrender.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/startmenurender.h
    # ${CMAKE_SOURCE_DIR}/qdvdauthor/qrender/xml_qrender.h
)

SET(plugins_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/interface.h
   ${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/sourcegroups.h
)

SET(complexdvd_SOURCES
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/simpledvd.cpp
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/uicontrols.cpp
)

SET(complexdvd_HEADERS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/simpledvd.h
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/uicontrols.h
)

SET(complexdvd_FORMS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/uisimpledvd3.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/complexdvd/uisimpledvd.ui
)

SET(menuslide_SOURCES
    #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/menuslide/main.cpp
   ${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/menuslide/menuslide.cpp
)

SET(menuslide_HEADERS
   ${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/menuslide/menuslide.h
)

SET(menuslide_FORMS
    #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/menuslide/uimenuslide3.ui
   ${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/menuslide/uimenuslide.ui
)

SET(simpledvd_SOURCES
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/simpledvd/simpledvd.cpp
)

SET(simpledvd_HEADERS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/simpledvd/simpledvd.h
)

SET(testplugs_SOURCES
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/testplugs/main.cpp
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/testplugs/testplugs.cpp
)

SET(testplugs_HEADERS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/testplugs/testplugs.h
)

SET(testplugs_FORMS
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/testplugs/uiMainForm.ui
   #${CMAKE_SOURCE_DIR}/qdvdauthor/plugins/testplugs/uiMainForm3.ui
)

SET(qdvdauthor_SOURCES
   #${qrender_SRCS} ${qrender_engine_SOURCES}
   ${qplayer_S0URCES}
   ${qplayer_engines_SOURCES}
   ${qplayer_engines_xine_SOURCES}
   ${qplayer_engines_mplayer_SOURCES}
   ${qplayer_engines_vlc_SOURCES}
   ${qrender_S0URCES}
    #${complexdvd_SOURCES}
   ${menuslide_SOURCES}
    #${simpledvd_SOURCES} ${testplugs_SOURCES} 
   animationattribute.cpp
   buttonobject.cpp
   buttonpreview.cpp
   cachethumbs.cpp
   colortoolbar.cpp
    #complexdvd.cpp
         dialogabout.cpp   #inserted
   #dialogalign.cpp
   #dialogalphatrans.cpp
   dialoganimation.cpp
   #dialogaudio.cpp
   dialogbutton.cpp
   dialogbuttontrans.cpp
   dialogcommandeditor.cpp
   #dialogconvert.cpp
   #dialogcreatetemplate.cpp
   dialogdelta.cpp
   dialogexecute.cpp
         dialogextraction.cpp   #inserted
   dialogfiles.cpp
   #dialogfont.cpp
   dialogframe.cpp
   #      dialoggeometry.cpp   #inserted
   dialogimage2.cpp
   dialogimages.cpp
   dialogkenburns.cpp
   #dialoglibrary.cpp
   dialogman.cpp
         dialogmatrix.cpp   #inserted
   #dialogmediainfo.cpp
   dialogmovie.cpp
   #dialogoverlay.cpp
   #dialogregion.cpp
         dialogrenderpreview.cpp   #inserted
         dialogresize.cpp   #inserted
   dialogselect.cpp
   dialogsetup.cpp
   dialogshadow.cpp
   dialogslideshow.cpp
   #      dialogsubtitle.cpp   #inserted
   #dialogtemp.cpp
   dialogtextfont.cpp
   #dialogthumbnail.cpp
   dialogundostack.cpp
   dialogvfolder.cpp
   dragndropcontainer.cpp
   dvdlayoutarea.cpp
   dvdlayoutgui.cpp
   dvdlayoutpainter.cpp
   dvdlayoutrouter.cpp
   dvdmenu.cpp
   dvdmenuundoobject.cpp
   errormessages.cpp
   execute.cpp
   exportdvdauthor.cpp
   exportsrt.cpp
   #exportssa.cpp
   filepreviewdialog.cpp
   filepreview_dialog.cpp
   frameobject.cpp
    #generate.cpp
   global.cpp
   imageobject.cpp
   #importchapter.cpp
   #importdvdauthor.cpp
   #importkino.cpp
   importsrt.cpp
   importssa.cpp
   importsub.cpp
   keyword.cpp
    kimageeffect.cpp
   #layoutwizard.cpp
   # listboxcolortext.cpp
   listviewitemmedia.cpp
   #listviewmedia.cpp
   log.cpp
   main.cpp
   maskobject.cpp
   menuobject.cpp
   menupreview.cpp
   messagebox.cpp
   modifiers.cpp
   movieobject.cpp
         newwizard.cpp   #inserted
   objectcollection.cpp
   #oneclickwizard.cpp
   overlayobject.cpp
   previewer.cpp
   preview_file_dialog.cpp	#xxx
   printpreview.cpp
   qdvdauthor.cpp
   qdvdauthorinit.cpp
   #qdvdauthorundoobject.cpp
   qimagehelper.cpp
   render_client.cpp
   rgba.cpp
   selectionobject.cpp
   shadowobject.cpp
   slidedefaults.cpp
   smalldialogaudio.cpp
   sourcefileentry.cpp
   sourcetoolbar.cpp
         sourcewidget.cpp   #inserted
   structuretoolbar.cpp
   subtitles.cpp
   #subtitlesgui.cpp
   #templatedownload.cpp
   #templatewizard.cpp
   #      temptab.cpp   #inserted
   textobject.cpp
   undobuffer.cpp
   utils.cpp
   win32.cpp
   xml_dvdauthor.cpp
   xml_slideshow.cpp
)
 
SET(qdvdauthor_HEADERS
    #${qrender_HEAD} ${qrender_engine_HEADERS}
   ${qplayer_HEADERS}
   ${qplayer_engines_HEADERS}
   ${qplayer_engines_xine_HEADERS}
   ${qplayer_engines_mplayer_HEADERS}
   ${qplayer_engines_vlc_HEADERS}
   ${qrender_HEADERS}
   ${plugins_HEADERS}
    #${complexdvd_HEADERS}
   ${menuslide_HEADERS}
    #${simpledvd_HEADERS} ${testplugs_HEADERS}
   #../bugs_workaround.h
   animationattribute.h
   buttonobject.h
   buttonpreview.h
   cachethumbs.h
   colortoolbar.h
         dialogabout.h   #inserted
   #dialogalign.h
   #dialogalphatrans.h
   dialoganimation.h
   #dialogaudio.h
   dialogbutton.h
   dialogbuttontrans.h
   dialogcommandeditor.h
   #dialogconvert.h
   #dialogcreatetemplate.h
   dialogdelta.h
   dialogexecute.h
         dialogextraction.h   #inserted
   dialogfiles.h
   #dialogfont.h
   dialogframe.h
   #      dialoggeometry.h   #inserted
   dialogimage2.h
   dialogimages.h
   dialogkenburns.h
   #dialoglibrary.h
   dialogman.h
         dialogmatrix.h   #inserted
   #dialogmediainfo.h
   dialogmovie.h
   #dialogoverlay.h
   #dialogregion.h
         dialogrenderpreview.h   #inserted
         dialogresize.h   #inserted
   dialogselect.h
   dialogsetup.h
   dialogshadow.h
   dialogslideshow.h
   #      dialogsubtitle.h   #inserted
   #dialogtemp.h
   dialogtextfont.h
   #dialogthumbnail.h
   dialogundostack.h
   dialogvfolder.h
   dragndropcontainer.h
   dvdlayoutarea.h
   dvdlayoutgui.h
   dvdlayoutpainter.h
   dvdlayoutrouter.h
   dvdmenu.h
   dvdmenuundoobject.h
   errormessages.h
   execute.h
   exportdvdauthor.h
   exportsrt.h
   #exportssa.h
   filepreviewdialog.h
   filepreview_dialog.h
   frameobject.h
    #generate.h
   global.h
   imageobject.h
   #importchapter.h
   #importdvdauthor.h
   #importkino.h
   importsrt.h
   importssa.h
   importsub.h
   keyword.h
    kimageeffect.h
   #layoutwizard.h
   # listboxcolortext.h
   listviewitemmedia.h
   #listviewmedia.h
   log.h
   maskobject.h
   menuobject.h
   menupreview.h
   messagebox.h
   modifiers.h
   movieobject.h
         newwizard.h   #inserted
   objectcollection.h
   #oneclickwizard.h
   overlayobject.h
   previewer.h
   preview_file_dialog.h	#xxx
   printpreview.h
   qdvdauthor.h
   qdvdauthorinit.h
   #qdvdauthorundoobject.h
   qimagehelper.h
   render_client.h
   rgba.h
   selectionobject.h
   shadowobject.h
   slidedefaults.h
   smalldialogaudio.h
   sourcefileentry.h
   sourcetoolbar.h
         sourcewidget.h   #inserted
   structuretoolbar.h
   #subtitlesgui.h   
   subtitles.h
   #templatedownload.h
   #templatewizard.h
   #      temptab.h   #inserted
   textobject.h
   undobuffer.h
   utils.h
   win32.h
   xml_dvdauthor.h
   xml_dvd.h
   xml_slideshow.h
)

SET(qdvdauthor_FORMS
   ${qplayer_FORMS}
   ${qplayer_engines_FORMS}
    #${complexdvd_FORMS}
   ${menuslide_FORMS}
    #${testplugs_FORMS}
   #dialoggeometry.ui
   dialogmatrix.ui
   dialogresize.ui
   #formmain.ui
   smalldialogaudio.ui
   uicommandeditor.ui
   #uicreatetemplate.ui
   uidialogabout.ui
   #uidialogalign.ui
   uidialoganimation.ui
   #uidialogaudio.ui
   uidialogbuttontrans.ui
   uidialogbutton.ui
   #uidialogconvert.ui
   uidialogdelta.ui
   uidialogextraction.ui
   uidialogfiles.ui
   #uidialogfont.ui
   uidialogframe.ui
   uidialogimage.ui
   uidialogkenburns.ui
   #uidialoglibrary.ui
   #uidialogmediainfo.ui
   uidialogmovie.ui
   #uidialogoverlay.ui
   #uidialogregion.ui
   uidialogrenderpreview.ui
   uidialogselect.ui
   uidialogsetup.ui
   uidialogshadow.ui
   #uidialogsubtitle.ui
   #uidialogtemp.ui
   uidialogtextfont.ui
   #uidialogthumbnail.ui
   uidialogvfolder.ui
   uidvdlayout.ui
   #uinewwizard.ui
   #uioneclickwizard.ui
   uipreviewer.ui
   uiprintpreview.ui
   uislideshow.ui
   uisourcewidget.ui
   #uistructurewidget.ui
   #uisubtitles.ui
   #uitemplatedownload.ui
   #uitemplatewizard.ui
   #uitemptab.ui
)

SET(qdvdauthor_RESOURCES images.qrc)

QT5_WRAP_CPP(qdvdauthor_HEADERS_MOC ${qdvdauthor_HEADERS})
QT5_WRAP_UI(qdvdauthor_FORMS_HEADERS ${qdvdauthor_FORMS})
QT5_ADD_RESOURCES(qdvdauthor_RESOURCES_RCC ${qdvdauthor_RESOURCES} ${qdvdauthor_RESOURCES_})

ADD_EXECUTABLE(qdvdauthor ${qdvdauthor_SOURCES} 
    ${qdvdauthor_HEADERS_MOC} 
    ${qdvdauthor_FORMS_HEADERS}
    ${qdvdauthor_RESOURCES_RCC}
    )
    
#LINK_DIRECTORIES(${xine_lib_dirs})    
#TARGET_LINK_LIBRARIES(qdvdauthor ${QT_QTCORE_LIBS})	#ooo
#TARGET_LINK_LIBRARIES(qdvdauthor ${QT_LIBRARIES})
#TARGET_LINK_LIBRARIES(qdvdauthor ${QT_LIBRARIES} ${FFMPEG_LIBRARIES} ${VLC_LIBRARIES} ${CMAKE_DL_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${xine_libs} X11)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
#TARGET_LINK_LIBRARIES(qdvdauthor ${Qt5Widgets_LIBRARIES} ${Qt5Declarative_LIBRARIES} ${Qt5Xml_LIBRARIES} ${X11_LIBRARIES} ${xine_libs} )
#TARGET_LINK_LIBRARIES(qdvdauthor Qt5::Widgets Qt5::Xml ${X11_LIBRARIES} ${XINE_LIBRARY} ${xine_libs} X11)
TARGET_LINK_LIBRARIES(qdvdauthor Qt5::Widgets Qt5::Xml xine X11 Qt5::Network Qt5::PrintSupport dl)

#LINK_DIRECTORIES(${FFMPEG_DIRS})
#link_directories(${FFMPEG_LIBRARIES} ${X11_LIBRARY_DIRS})
link_directories(${X11_LIBRARY_DIRS})
message(STATUS "Found qdvdauthor:")
    
#QT4_WRAP_CPP(qrender_HEADERS_MOC ${qrender_HEADERS}) 
#ADD_EXECUTABLE(qrender ${qrender_SOURCS} 
#    ${qrender_HEADERS_MOC})
#TARGET_LINK_LIBRARIES(qrender ${QT_QTCORE_LIBS})	#ooo    
    


INSTALL( TARGETS qdvdauthor DESTINATION bin)
#INSTALL( TARGETS qplayer DESTINATION bin)
#INSTALL( TARGETS qrender DESTINATION bin)
#INSTALL( TARGETS qslideshow DESTINATION bin)

########### install files ###############

#INSTALL(FILES   images/add_audio.png  DESTINATION  hicolor/16x16/apps/add_audio.png)

#INSTALL(FILES    images/add_audio.png
#   images/add_image.png
#   images/add_menuslide.png
#   images/add_slideshow.png
#   images/brush.png
#   images/button_back.png
#   images/button_cback.png
#   images/button_cplay.png
#   images/button_cstop.png
#   images/button_play.png
#   images/button_stop.bmp
#   images/button_stop.png
#   images/checkered.png
#   images/create_slideshow.jpg
#   images/create_thumbs.png
#   images/default.jpg
#   images/delete.png
#   images/down.png
#   images/dvd_regions.png
#   images/editcopy
#   images/editcut
#   images/editpaste
#   images/error.jpg
#   images/exit.png
#   images/grid.png
#   images/move_cursor.png
#   images/movie_frame.png
#   images/new.png
#   images/open.png
#   images/pan.png
#   images/player_next.png
#   images/player_pause.png
#   images/player_play.png
#   images/player_prev.png
#   images/player_stop.png
#   images/please_wait.jpg
#   images/printer.png
#    #images/project_name.jpg
#   images/redo
#   images/save_as.png
#   images/save.png
#   images/scan_dv.png
#   images/searchfind
#   images/sound_file.jpg
#   images/splash.jpg
#   images/star.png
#   images/stretch_lr_cursor.png
#   images/stretch_ud_cursor.png
#   images/structure_icons.png
#    #images/temp_path.jpg
#   images/text_ccenter.png
#   images/text_center.png
#   images/text_cfit.png
#   images/text_cjustify.png
#   images/text_cleft.png
#   images/text_cright.png
#   images/text_cstrikeout.png
#   images/text_cunderline.png
#   images/text_cvbottom.png
#   images/text_cvcenter.png
#   images/text_cvtop.png
#   images/text_fit.png
#   images/text_justify.png
#   images/text_left.png
#   images/text_right.png
#   images/text_strikeout.png
#   images/text_underline.png
#   images/text_vbottom.png
#   images/text_vcenter.png
#   images/text_vtop.png
#   images/to_dvd2.png
#   images/to_dvd.png
#   images/to_svcd.png
#   images/to_vcd.png
#   images/undo
#   images/up.png
#   images/wizard.png
#   DESTINATION  share/qdvdauthor/images)

# we don't want to copy if we're building in the source dir
#if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

# list of files for which we add a copy rule
#set(data_SHADOW     images/add_audio.png
#   images/add_image.png
#   images/add_menuslide.png
#   images/add_slideshow.png
#   images/brush.png
#   images/button_back.png
#   images/button_cback.png
#   images/button_cplay.png
#   images/button_cstop.png
#   images/button_play.png
#   images/button_stop.bmp
#   images/button_stop.png
#   images/checkered.png
#   images/create_slideshow.jpg
#   images/create_thumbs.png
#   images/default.jpg
#   images/delete.png
#   images/down.png
#   images/dvd_regions.png
#   images/editcopy
#   images/editcut
#   images/editpaste
#   images/error.jpg
#   images/exit.png
#   images/grid.png
#   images/move_cursor.png
#   images/movie_frame.png
#   images/new.png
#   images/open.png
#   images/pan.png
#   images/player_next.png
#   images/player_pause.png
#   images/player_play.png
#   images/player_prev.png
#   images/player_stop.png
#   images/please_wait.jpg
#   images/printer.png
#    #images/project_name.jpg
#   images/redo
#   images/save_as.png
#   images/save.png
#   images/scan_dv.png
#   images/searchfind
#   images/sound_file.jpg
#   images/splash.jpg
#   images/star.png
#   images/stretch_lr_cursor.png
#   images/stretch_ud_cursor.png
#   images/structure_icons.png
#    #images/temp_path.jpg
#   images/text_ccenter.png
#   images/text_center.png
#   images/text_cfit.png
#   images/text_cjustify.png
#   images/text_cleft.png
#   images/text_cright.png
#   images/text_cstrikeout.png
#   images/text_cunderline.png
#   images/text_cvbottom.png
#   images/text_cvcenter.png
#   images/text_cvtop.png
#   images/text_fit.png
#   images/text_justify.png
#   images/text_left.png
#   images/text_right.png
#   images/text_strikeout.png
#   images/text_underline.png
#   images/text_vbottom.png
#   images/text_vcenter.png
#   images/text_vtop.png
#   images/to_dvd2.png
#   images/to_dvd.png
#   images/to_svcd.png
#   images/to_vcd.png
#   images/undo
#   images/up.png
#   images/wizard.png
#   plugins/menuslide/background00000.jpg
#)   
#    foreach(item IN LISTS data_SHADOW)
        #message(STATUS ${item})
#        add_custom_command(
#            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
#            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
#            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
#        )
#    endforeach()
    
#endif()

# files are only copied if a target depends on them
#add_custom_target(data-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/images/add_audio.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/add_audio.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/add_image.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/add_menuslide.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/add_slideshow.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/brush.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_back.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_cback.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_cplay.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_cstop.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_play.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_stop.bmp"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/button_stop.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/checkered.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/create_slideshow.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/create_thumbs.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/default.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/delete.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/down.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/dvd_regions.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/editcopy"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/editcut"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/editpaste"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/error.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/exit.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/grid.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/move_cursor.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/movie_frame.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/new.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/open.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/pan.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/player_next.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/player_pause.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/player_play.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/player_prev.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/player_stop.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/please_wait.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/printer.png"
#    #"${CMAKE_CURRENT_BINARY_DIR}/images/project_name.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/redo"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/save_as.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/save.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/scan_dv.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/searchfind"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/sound_file.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/splash.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/star.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/stretch_lr_cursor.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/stretch_ud_cursor.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/structure_icons.png"
#    #"${CMAKE_CURRENT_BINARY_DIR}/images/temp_path.jpg"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_ccenter.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_center.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cfit.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cjustify.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cleft.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cright.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cstrikeout.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cunderline.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cvbottom.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cvcenter.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_cvtop.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_fit.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_justify.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_left.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_right.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_strikeout.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_underline.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_vbottom.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_vcenter.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/text_vtop.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/to_dvd2.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/to_dvd.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/to_svcd.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/to_vcd.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/undo"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/up.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/images/wizard.png"
#   "${CMAKE_CURRENT_BINARY_DIR}/plugins/menuslide/background00000.jpg"
#)

# we don't want to copy if we're building in the source dir
#if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

# list of files for which we add a copy rule
#set(data2_SHADOW     plugins/menuslide/background00000.jpg)

#    foreach(item IN LISTS data2_SHADOW)
#        message(STATUS ${item})
#        add_custom_command(
#            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
#            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
#            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
#        )
#    endforeach()
    
#endif()

# files are only copied if a target depends on them
#add_custom_target(data2-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/plugins/menuslide/add_audio.png")

#INSTALL( TARGETS qdvdauthor DESTINATION bin)
