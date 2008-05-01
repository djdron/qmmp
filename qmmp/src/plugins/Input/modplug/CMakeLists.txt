project(libmodplug)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)

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
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# modplug
PKGCONFIG(libmodplug MODPLUG_INCLUDE_DIR MODPLUG_LINK_DIR MODPLUG_LINK_FLAGS MODPLUG_CFLAGS)

IF(NOT MODPLUG_LINK_FLAGS)
  SET(MODPLUG_LINK_FLAGS -lmodplug -lstdc++ -lm)
  SET(MODPLUG_INCLUDE_DIR /usr/include)
  SET(MODPLUG_CFLAGS -I/usr/include -I/usr/include/libmodplug)
  SET(MODPLUG_LINK_DIR /usr/lib)
ENDIF(NOT MODPLUG_LINK_FLAGS)

ADD_DEFINITIONS(${MODPLUG_CFLAGS})
include_directories(${CMODPLUG_INCLUDE_DIR})
link_directories(${MODPLUG_LINK_DIR})

SET(libmodplug_SRCS
  decoder_modplug.cpp
  decodermodplugfactory.cpp
  detailsdialog.cpp
  settingsdialog.cpp
  archivereader.cpp
)

SET(libmodplug_MOC_HDRS
  decodermodplugfactory.h
  decoder_modplug.h
  detailsdialog.h
  settingsdialog.h
  archivereader.h
)

#SET(libmodplug_RCCS translations/translations.qrc)

#QT4_ADD_RESOURCES(libmodplug_RCC_SRCS ${libmodplug_RCCS})

QT4_WRAP_CPP(libmodplug_MOC_SRCS ${libmodplug_MOC_HDRS})

# user interface


SET(libmodplug_UIS
    detailsdialog.ui
    settingsdialog.ui
)

QT4_WRAP_UI(libmodplug_UIS_H ${libmodplug_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(modplug SHARED ${libmodplug_SRCS} ${libmodplug_MOC_SRCS} ${libmodplug_UIS_H}
  ${libmodplug_RCC_SRCS})
target_link_libraries(modplug ${QT_LIBRARIES} -lqmmp ${MODPLUG_LINK_FLAGS} ${MODPLUG_CLAGS})
install(TARGETS modplug DESTINATION ${LIB_DIR}/qmmp/Input)


