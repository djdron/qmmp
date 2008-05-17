project(libdbuscontrol)

cmake_minimum_required(VERSION 2.4.8 FATAL_ERROR)

SET (QT_USE_QTDBUS TRUE)

INCLUDE(FindQt4)

include(${QT_USE_FILE})

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

# libqmmpui
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libdbuscontrol_SRCS
    dbuscontrol.cpp
    dbuscontrolfactory.cpp
    dbusadaptor.cpp
)

SET(libdbuscontrol_MOC_HDRS
    dbuscontrolfactory.h
    dbuscontrol.h
    dbusadaptor.h
)

SET(libdbuscontrol_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libdbuscontrol_RCC_SRCS ${libdbuscontrol_RCCS})

QT4_WRAP_CPP(libdbuscontrol_MOC_SRCS ${libdbuscontrol_MOC_HDRS})

# user interface


# QT4_WRAP_UI(libdbuscontrol_UIS_H ${libdbuscontrol_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(dbuscontrol SHARED ${libdbuscontrol_SRCS} ${libdbuscontrol_MOC_SRCS} ${libdbuscontrol_RCC_SRCS})
target_link_libraries(dbuscontrol ${QT_LIBRARIES} -lqmmpui)
install(TARGETS dbuscontrol DESTINATION ${LIB_DIR}/qmmp/General)
