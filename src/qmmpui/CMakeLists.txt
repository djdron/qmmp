project(libqmmpui)

cmake_minimum_required(VERSION 2.4.0)

include(FindQt4)
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

# qt lib
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
ADD_DEFINITIONS(-DLIB_DIR=\\\"\\/${LIB_DIR}\\\")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(libqmmpui_SRCS
    general.cpp
    generalhandler.cpp
    songinfo.cpp
    control.cpp
)

SET(libqmmpui_MOC_HDRS
    generalfactory.h
    general.h
    generalhandler.h
    songinfo.h
    control.h
)

QT4_WRAP_CPP(libqmmpui_MOC_SRCS ${libqmmpui_MOC_HDRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(qmmpui SHARED ${libqmmpui_SRCS} ${libqmmpui_MOC_SRCS})
target_link_libraries(qmmpui ${QT_LIBRARIES})

install(TARGETS qmmpui LIBRARY DESTINATION ${LIB_DIR})

