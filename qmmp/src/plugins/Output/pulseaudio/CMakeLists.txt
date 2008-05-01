project(libpulseaudio)

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

# pulseaudio
PKGCONFIG(libpulse-simple PULSE_INCLUDE_DIR PULSE_LINK_DIR PULSE_LINK_FLAGS PULSE_CFLAGS)

IF(NOT PULSE_LINK_FLAGS)
  SET(PULSE_LINK_FLAGS -lpulse -lpulse-simple)
ENDIF(NOT PULSE_LINK_FLAGS)

SET(libpulseaudio_SRCS
  outputpulseaudio.cpp
  outputpulseaudiofactory.cpp
)

SET(libpulseaudio_MOC_HDRS
  outputpulseaudio.h
  outputpulseaudiofactory.h
)

#SET(libpulseaudio_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libpulseaudio_RCC_SRCS ${libpulseaudio_RCCS})

QT4_WRAP_CPP(libpulseaudio_MOC_SRCS ${libpulseaudio_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(pulseaudio SHARED ${libpulseaudio_SRCS} ${libpulseaudio_MOC_SRCS} ${libpulseaudio_UIS_H}
  ${libpulseaudio_RCC_SRCS})
target_link_libraries(pulseaudio ${QT_LIBRARIES} -lqmmp ${PULSE_LINK_FLAGS})
install(TARGETS pulseaudio DESTINATION ${LIB_DIR}/qmmp/Output)

