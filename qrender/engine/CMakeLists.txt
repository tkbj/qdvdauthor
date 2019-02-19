#project(qdvdauthor)

SET(engine_SOURCES
   ffmpeg_enc.cpp
   test.cpp
)

SET(engine_HEADERS
   colorspace.h
   ffmpeg_enc.h
)

#QT4_WRAP_CPP(engine_HEADERS_MOC ${engine_HEADERS})