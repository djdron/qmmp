project(libmpg123)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy) 

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# libmpg123
pkg_check_modules(MPG123 libmpg123)

include_directories(${MPG123_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${MPG123_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${MPG123_CFLAGS} ${TAGLIB_CFLAGS})

SET(libmpg123_SRCS
  decoder_mpg123.cpp
  decodermpg123factory.cpp
  settingsdialog.cpp
  tagextractor.cpp
  mpegmetadatamodel.cpp
  replaygainreader.cpp
)

SET(libmpg123_HDRS
  decoder_mpg123.h
  tagextractor.h
  replaygainreader.h
)

SET(libmpg123_MOC_HDRS
  settingsdialog.h
  decodermpg123factory.h
  mpegmetadatamodel.h
)

SET(libmpg123_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmpg123_RCC_SRCS ${libmpg123_RCCS})

QT4_WRAP_CPP(libmpg123_MOC_SRCS ${libmpg123_MOC_HDRS})

# user interface


SET(libmpg123_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libmpg123_UIS_H ${libmpg123_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MPG123_FOUND)
ADD_LIBRARY(mpg123 MODULE ${libmpg123_SRCS} ${libmpg123_MOC_SRCS} ${libmpg123_UIS_H} ${libmpg123_RCC_SRCS} ${libmpg123_HDRS})
target_link_libraries(mpg123 ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${MPG123_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS mpg123 DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(MPG123_FOUND)
