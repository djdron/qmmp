project(libffmpeg)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
# fixes ffmpeg defines
ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libffmpeg and taglib
PKGCONFIG(libavcodec LIBAVCODEC_INCLUDE_DIR LIBAVCODEC_LINK_DIR LIBAVCODEC_LINK_FLAGS LIBAVCODEC_CFLAGS)
PKGCONFIG(libavformat LIBAVFORMAT_INCLUDE_DIR LIBAVFORMAT_LINK_DIR LIBAVFORMAT_LINK_FLAGS LIBAVFORMAT_CFLAGS)
PKGCONFIG(taglib TAGLIB_INCLUDE_DIR TAGLIB_LINK_DIR TAGLIB_LINK_FLAGS TAGLIB_CFLAGS)

IF(NOT LIBAVCODEC_LINK_FLAGS)
  SET(LIBAVCODEC_LINK_FLAGS -lavcodec)
ENDIF(NOT LIBAVCODEC_LINK_FLAGS)

IF(NOT LIBAVFORMAT_LINK_FLAGS)
  SET(LIBAVFORMAT_LINK_FLAGS -lavformat)
ENDIF(NOT LIBAVFORMAT_LINK_FLAGS)

IF(NOT TAGLIB_LINK_FLAGS)
  SET(TAGLIB_LINK_FLAGS -ltag)
  SET(TAGLIB_INCLUDE_DIR /usr/include/taglib)
  SET(TAGLIB_CFLAGS -I/usr/include/taglib)
ENDIF(NOT TAGLIB_LINK_FLAGS)

include_directories(${FLAC_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR})
link_directories(${FLAC_LINK_DIR} ${TAGLIB_LINK_DIR})

ADD_DEFINITIONS(${LIBAVCODEC_CFLAGS})
ADD_DEFINITIONS(${LIBAVFORMAT_CFLAGS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libffmpeg_SRCS
  decoder_ffmpeg.cpp
  decoderffmpegfactory.cpp
  detailsdialog.cpp
)

SET(libffmpeg_MOC_HDRS
  decoderffmpegfactory.h
  decoder_ffmpeg.h
  detailsdialog.h
)

SET(libffmpeg_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libffmpeg_RCC_SRCS ${libffmpeg_RCCS})

QT4_WRAP_CPP(libffmpeg_MOC_SRCS ${libffmpeg_MOC_HDRS})

# user interface


SET(libffmpeg_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libffmpeg_UIS_H ${libffmpeg_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(ffmpeg SHARED ${libffmpeg_SRCS} ${libffmpeg_MOC_SRCS} ${libffmpeg_UIS_H}
  ${libffmpeg_RCC_SRCS})
target_link_libraries(ffmpeg ${QT_LIBRARIES} -lqmmp ${LIBAVCODEC_LINK_FLAGS} ${LIBAVFORMAT_LINK_FLAGS} ${TAGLIB_LINK_FLAGS} ${TAGLIB_CFLAGS})
install(TARGETS ffmpeg DESTINATION ${LIB_DIR}/qmmp/Input PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

