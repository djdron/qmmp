project(libm3uplaylistformat)

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

SET(libm3uplaylistformat_SRCS
    m3uplaylistformat.cpp
)

SET(libm3uplaylistformat_MOC_HDRS
    m3uplaylistformat.h
)

QT4_ADD_RESOURCES(libm3uplaylistformat_RCC_SRCS ${libm3uplaylistformat_RCCS})

QT4_WRAP_CPP(libm3uplaylistformat_MOC_SRCS ${libm3uplaylistformat_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(m3uplaylistformat SHARED ${libm3uplaylistformat_SRCS} ${libm3uplaylistformat_MOC_SRCS})
target_link_libraries(m3uplaylistformat ${QT_LIBRARIES} -lqmmpui)
install(TARGETS m3uplaylistformat DESTINATION ${LIB_DIR}/qmmp/PlaylistFormats)
