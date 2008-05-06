project(libffmpeg)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)

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

# libffmpeg
PKGCONFIG(libavcodec LIBAVCODEC_INCLUDE_DIR LIBAVCODEC_LINK_DIR LIBAVCODEC_LINK_FLAGS LIBAVCODEC_CFLAGS)
PKGCONFIG(libavformat LIBAVFORMAT_INCLUDE_DIR LIBAVFORMAT_LINK_DIR LIBAVFORMAT_LINK_FLAGS LIBAVFORMAT_CFLAGS)

IF(NOT LIBAVCODEC_LINK_FLAGS)
  SET(LIBAVCODEC_LINK_FLAGS -lavcodec)
  SET(LIBAVCODEC_INCLUDE_DIR /usr/include)
ENDIF(NOT LIBAVCODEC_LINK_FLAGS)

IF(NOT LIBAVFORMAT_LINK_FLAGS)
  SET(LIBAVFORMAT_LINK_FLAGS -lavformat)
  SET(LIBAVFORMAT_INCLUDE_DIR /usr/include)
ENDIF(NOT LIBAVFORMAT_LINK_FLAGS)

# old ffmpeg support
IF(EXISTS ${LIBAVFORMAT_INCLUDE_DIR}/ffmpeg/avformat.h)
  ADD_DEFINITIONS(-DHAVE_FFMPEG_AVFORMAT_H)
ELSEIF(EXISTS ${LIBAVFORMAT_INCLUDE_DIR}/libavformat/avformat.h)
  ADD_DEFINITIONS(-DHAVE_LIBAVFORMAT_AVFORMAT_H)
ELSE(EXISTS ${LIBAVFORMAT_INCLUDE_DIR}/ffmpeg/avformat.h)
  include_directories(/usr/local/include/ffmpeg
                      /usr/local/include/libavformat
)
ENDIF(EXISTS ${LIBAVFORMAT_INCLUDE_DIR}/ffmpeg/avformat.h)

IF(EXISTS ${LIBAVCODEC_INCLUDE_DIR}/ffmpeg/avcodec.h)
  ADD_DEFINITIONS(-DHAVE_FFMPEG_AVCODEC_H)
ELSEIF(EXISTS ${LIBAVCODEC_INCLUDE_DIR}/libavcodec/avcodec.h)
  ADD_DEFINITIONS(-DHAVE_LIBAVCODEC_AVCODEC_H)
ELSE(EXISTS ${LIBAVCODEC_INCLUDE_DIR}/ffmpeg/avcodec.h)
  include_directories(/usr/local/include/ffmpeg
                      /usr/local/include/libavcodec
)
ENDIF(EXISTS ${LIBAVCODEC_INCLUDE_DIR}/ffmpeg/avcodec.h)


include_directories(${LIBAVCODEC_INCLUDE_DIR} ${LIBAVFORMAT_INCLUDE_DIR})
link_directories(${LIBAVCODEC_LINK_DIR} ${LIBAVFORMAT_LINK_DIR})

ADD_DEFINITIONS(${LIBAVCODEC_CFLAGS})
ADD_DEFINITIONS(${LIBAVFORMAT_CFLAGS})

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
target_link_libraries(ffmpeg ${QT_LIBRARIES} -lqmmp ${LIBAVCODEC_LINK_FLAGS} ${LIBAVFORMAT_LINK_FLAGS})
install(TARGETS ffmpeg DESTINATION ${LIB_DIR}/qmmp/Input PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
