project(liboss)

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



include_directories(${JACK_INCLUDE_DIR} ${JACK_INCLUDE_DIR})
link_directories(${SAMPLERATE_LINK_DIR} ${SAMPLERATE_LINK_DIR})

ADD_DEFINITIONS(${JACK_CFLAGS})
ADD_DEFINITIONS(${SAMPLERATE_CFLAGS})


SET(liboss_SRCS
  outputossfactory.cpp
  outputoss.cpp
)

SET(liboss_MOC_HDRS
  outputossfactory.h
  outputoss.h
)

#SET(libjack_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libjack_RCC_SRCS ${libjack_RCCS})

QT4_WRAP_CPP(liboss_MOC_SRCS ${liboss_MOC_HDRS})



ADD_LIBRARY(oss SHARED ${liboss_SRCS} ${liboss_MOC_SRCS})
target_link_libraries(oss ${QT_LIBRARIES} -lqmmp )
install(TARGETS oss DESTINATION lib/qmmp/Output)

