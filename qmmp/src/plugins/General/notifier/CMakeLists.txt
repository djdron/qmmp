project(libnotifier)

cmake_minimum_required(VERSION 2.4.0)



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

# libqmmpui
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libnotifier_SRCS
    settingsdialog.cpp
    notifier.cpp
    notifierfactory.cpp
    popupwidget.cpp
)

SET(libnotifier_MOC_HDRS
    settingsdialog.h
    notifierfactory.h
    notifier.h
    popupwidget.h
)

#SET(libnotifier_RCCS translations/translations.qrc)

#SET(libnotifier_RCCS images/images.qrc)

#QT4_ADD_RESOURCES(libnotifier_RCC_SRCS ${libnotifier_RCCS})

QT4_WRAP_CPP(libnotifier_MOC_SRCS ${libnotifier_MOC_HDRS})

# user interface


SET(libnotifier_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libnotifier_UIS_H ${libnotifier_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(notifier SHARED ${libnotifier_SRCS} ${libnotifier_MOC_SRCS} ${libnotifier_UIS_H} ${libnotifier_RCC_SRCS})
target_link_libraries(notifier ${QT_LIBRARIES} -lqmmpui)
install(TARGETS notifier DESTINATION ${LIB_DIR}/qmmp/General)
