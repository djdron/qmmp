project(libmad)

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

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)

# libmad and taglib
PKGCONFIG(mad MAD_INCLUDE_DIR MAD_LINK_DIR MAD_LINK_FLAGS MAD_CFLAGS)
PKGCONFIG(taglib TAGLIB_INCLUDE_DIR TAGLIB_LINK_DIR TAGLIB_LINK_FLAGS TAGLIB_CFLAGS)

IF(NOT MAD_LINK_FLAGS)
  MESSAGE("Can not find mad.pc")
  SET(MAD_LINK_FLAGS -lmad)
ENDIF(NOT MAD_LINK_FLAGS)

IF(NOT TAGLIB_LINK_FLAGS)
  SET(TAGLIB_LINK_FLAGS -ltag)
  SET(TAGLIB_INCLUDE_DIR /usr/include/taglib)
  SET(TAGLIB_CFLAGS -I/usr/include/taglib)
ENDIF(NOT TAGLIB_LINK_FLAGS)

include_directories(${MAD_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR})
link_directories(${MAD_LINK_DIR} ${TAGLIB_LINK_DIR})

#ADD_DEFINITIONS(${MAD_CFLAGS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libmad_SRCS
  decoder_mad.cpp
  decodermadfactory.cpp
  detailsdialog.cpp
  id3tag.cpp
  settingsdialog.cpp
)

SET(libmad_MOC_HDRS
  settingsdialog.h
  id3tag.h
  decodermadfactory.h
  decoder_mad.h
  detailsdialog.h
)

SET(libmad_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmad_RCC_SRCS ${libmad_RCCS})

QT4_WRAP_CPP(libmad_MOC_SRCS ${libmad_MOC_HDRS})

# user interface


SET(libmad_UIS
    detailsdialog.ui
    settingsdialog.ui
)

QT4_WRAP_UI(libmad_UIS_H ${libmad_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(mad SHARED ${libmad_SRCS} ${libmad_MOC_SRCS} ${libmad_UIS_H}
  ${libmad_RCC_SRCS})

target_link_libraries(mad ${QT_LIBRARIES} -lqmmp ${MAD_LINK_FLAGS} ${TAGLIB_LINK_FLAGS} ${TAGLIB_CFLAGS})
install(TARGETS mad DESTINATION lib/qmmp/Input)
