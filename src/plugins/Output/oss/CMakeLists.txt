project(liboss)

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



include_directories(${JACK_INCLUDE_DIR} ${JACK_INCLUDE_DIR})
link_directories(${SAMPLERATE_LINK_DIR} ${SAMPLERATE_LINK_DIR})

ADD_DEFINITIONS(${JACK_CFLAGS})
ADD_DEFINITIONS(${SAMPLERATE_CFLAGS})


SET(liboss_SRCS
  outputossfactory.cpp
  outputoss.cpp
  settingsdialog.cpp
)

SET(liboss_MOC_HDRS
  outputossfactory.h
  outputoss.h
  settingsdialog.h
)

SET(libjack_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libjack_RCC_SRCS ${libjack_RCCS})

QT4_WRAP_CPP(liboss_MOC_SRCS ${liboss_MOC_HDRS})

SET(liboss_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(liboss_UIS_H ${liboss_UIS})

ADD_LIBRARY(oss SHARED ${liboss_SRCS} ${liboss_MOC_SRCS} ${liboss_UIS_H})
target_link_libraries(oss ${QT_LIBRARIES} -lqmmp )
install(TARGETS oss DESTINATION ${LIB_DIR}/qmmp/Output PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

