project(libstatusicon)

cmake_minimum_required(VERSION 2.4.0)

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

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmpui
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libstatusicon_SRCS
    settingsdialog.cpp
    statusicon.cpp
    statusiconfactory.cpp
)

SET(libstatusicon_MOC_HDRS
    settingsdialog.h
    statusiconfactory.h
    statusicon.h
)

SET(libstatusicon_RCCS 
    translations/translations.qrc
    images/tray_images.qrc)

QT4_ADD_RESOURCES(libstatusicon_RCC_SRCS ${libstatusicon_RCCS})

QT4_WRAP_CPP(libstatusicon_MOC_SRCS ${libstatusicon_MOC_HDRS})

# user interface


SET(libstatusicon_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libstatusicon_UIS_H ${libstatusicon_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(statusicon SHARED ${libstatusicon_SRCS} ${libstatusicon_MOC_SRCS} ${libstatusicon_UIS_H} ${libstatusicon_RCC_SRCS})
target_link_libraries(statusicon ${QT_LIBRARIES} -lqmmpui)
install(TARGETS statusicon DESTINATION ${LIB_DIR}/qmmp/General)
