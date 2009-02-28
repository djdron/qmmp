project(libprojectm)

cmake_minimum_required(VERSION 2.4.7)

SET (QT_USE_QTOPENGL TRUE)
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# projectM
pkg_search_module(PROJECTM libprojectM)

SET(libprojectm_SRCS
    projectmwidget.cpp
    projectmplugin.cpp
    visualprojectmfactory.cpp
)

SET(libprojectm_MOC_HDRS
    projectmwidget.h
    projectmplugin.h
    visualprojectmfactory.h
)

SET(libprojectm_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libprojectm_RCC_SRCS ${libprojectm_RCCS})

QT4_WRAP_CPP(libprojectm_MOC_SRCS ${libprojectm_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(PROJECTM_FOUND)
ADD_LIBRARY(projectm SHARED ${libprojectm_SRCS} ${libprojectm_MOC_SRCS} ${libprojectm_RCC_SRCS})
add_dependencies(projectm qmmp)
target_link_libraries(projectm ${QT_LIBRARIES} -lqmmp ${PROJECTM_LDFLAGS} ${PROJECTM_CFLAGS})
install(TARGETS projectm DESTINATION ${LIB_DIR}/qmmp/Visual)
ENDIF(PROJECTM_FOUND)
