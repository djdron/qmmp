project(libffmpeg)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# ffmpeg
pkg_check_modules(FFMPEG libavcodec>=53.7.0 libavformat>=53.4.0 libavutil>=51.9.1)

include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})
ADD_DEFINITIONS(${FFMPEG_CFLAGS})

SET(libffmpeg_SRCS
  decoder_ffmpeg.cpp
  decoderffmpegfactory.cpp
  ffmpegmetadatamodel.cpp
  settingsdialog.cpp
)

SET(libffmpeg_HDRS
  decoder_ffmpeg.h
)

SET(libffmpeg_MOC_HDRS
  decoderffmpegfactory.h
  ffmpegmetadatamodel.h
  settingsdialog.h
)

SET(libffmpeg_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libffmpeg_RCC_SRCS ${libffmpeg_RCCS})

QT4_WRAP_CPP(libffmpeg_MOC_SRCS ${libffmpeg_MOC_HDRS})

# user interface


SET(libffmpeg_UIS
  settingsdialog.ui
)

QT4_WRAP_UI(libffmpeg_UIS_H ${libffmpeg_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FFMPEG_FOUND)
ADD_LIBRARY(ffmpeg MODULE ${libffmpeg_SRCS} ${libffmpeg_MOC_SRCS} ${libffmpeg_UIS_H}
  ${libffmpeg_RCC_SRCS} ${libffmpeg_HDRS})
add_dependencies(ffmpeg qmmp)
target_link_libraries(ffmpeg ${QT_LIBRARIES} -lqmmp ${FFMPEG_LDFLAGS})
install(TARGETS ffmpeg DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(FFMPEG_FOUND)
