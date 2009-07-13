project(libmpris)

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

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libmpris_SRCS
    mpris.cpp
    mprisfactory.cpp
    rootobject.cpp
    playerobject.cpp
    tracklistobject.cpp
)

SET(libmpris_MOC_HDRS
    mprisfactory.h
    mpris.h
    rootobject.h
    playerobject.h
    tracklistobject.h
)

SET(libmpris_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmpris_RCC_SRCS ${libmpris_RCCS})

QT4_WRAP_CPP(libmpris_MOC_SRCS ${libmpris_MOC_HDRS})

# user interface


# QT4_WRAP_UI(libmpris_UIS_H ${libmpris_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(mpris MODULE ${libmpris_SRCS} ${libmpris_MOC_SRCS} ${libmpris_RCC_SRCS})
add_dependencies(mpris qmmpui)
target_link_libraries(mpris ${QT_LIBRARIES} -lqmmpui -lqmmp)
install(TARGETS mpris DESTINATION ${LIB_DIR}/qmmp/General)
