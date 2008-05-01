project(libscrobbler)

cmake_minimum_required(VERSION 2.4.0)


SET(QT_USE_QTNETWORK TRUE)
INCLUDE(FindQt4)

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

# libqmmpui
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libscrobbler_SRCS
    settingsdialog.cpp
    scrobbler.cpp
    scrobblerfactory.cpp
)

SET(libscrobbler_MOC_HDRS
    settingsdialog.h
    scrobblerfactory.h
    scrobbler.h
)

#SET(libscrobbler_RCCS translations/translations.qrc)

#QT4_ADD_RESOURCES(libscrobbler_RCC_SRCS ${libscrobbler_RCCS})

QT4_WRAP_CPP(libscrobbler_MOC_SRCS ${libscrobbler_MOC_HDRS})

# user interface


SET(libscrobbler_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libscrobbler_UIS_H ${libscrobbler_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(scrobbler SHARED ${libscrobbler_SRCS} ${libscrobbler_MOC_SRCS} ${libscrobbler_UIS_H}
  ${libscrobbler_RCC_SRCS})
target_link_libraries(scrobbler ${QT_LIBRARIES} -lqmmpui)
install(TARGETS scrobbler DESTINATION ${LIB_DIR}/qmmp/General)
