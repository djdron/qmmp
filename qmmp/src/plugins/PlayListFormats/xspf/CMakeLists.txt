project(libxspfplaylistformat)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(QT_USE_QTXML TRUE)

INCLUDE(FindQt4)
INCLUDE(${QT_USE_FILE})

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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libxspfplaylistformat_SRCS
    xspfplaylistformat.cpp
)

SET(libxspfplaylistformat_MOC_HDRS
    xspfplaylistformat.h
)

QT4_ADD_RESOURCES(libxspfplaylistformat_RCC_SRCS ${libxspfplaylistformat_RCCS})

QT4_WRAP_CPP(libxspfplaylistformat_MOC_SRCS ${libxspfplaylistformat_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(xspfplaylistformat MODULE ${libxspfplaylistformat_SRCS} ${libxspfplaylistformat_MOC_SRCS})
add_dependencies(xspfplaylistformat qmmpui)
target_link_libraries(xspfplaylistformat ${QT_LIBRARIES} -lqmmpui -lqmmp)
install(TARGETS xspfplaylistformat DESTINATION ${LIB_DIR}/qmmp/PlayListFormats)