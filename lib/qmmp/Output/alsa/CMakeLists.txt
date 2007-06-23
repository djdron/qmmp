project(libalsa)

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

SET(libalsa_SRCS
  outputalsa.cpp
  outputalsafactory.cpp
  settingsdialog.cpp
)

SET(libalsa_MOC_HDRS
  outputalsa.h
  outputalsafactory.h
  settingsdialog.h
)

SET(libalsa_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libalsa_RCC_SRCS ${libalsa_RCCS})

QT4_WRAP_CPP(libalsa_MOC_SRCS ${libalsa_MOC_HDRS})

# user interface


SET(libalsa_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libalsa_UIS_H ${libalsa_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(alsa SHARED ${libalsa_SRCS} ${libalsa_MOC_SRCS} ${libalsa_UIS_H}
  ${libalsa_RCC_SRCS})
target_link_libraries(alsa ${QT_LIBRARIES} -lqmmp -lasound)
install(TARGETS alsa DESTINATION lib/qmmp/Output)

