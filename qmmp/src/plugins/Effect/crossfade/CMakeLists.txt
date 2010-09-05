project(libcrossfade)

cmake_minimum_required(VERSION 2.4.7)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libcrossfade_SRCS
    crossfadeplugin.cpp
    settingsdialog.cpp
    effectcrossfadefactory.cpp
)

SET(libcrossfade_MOC_HDRS
    crossfadeplugin.h
    settingsdialog.h
    effectcrossfadefactory.h
)

SET(libcrossfade_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libcrossfade_RCC_SRCS ${libcrossfade_RCCS})

QT4_WRAP_CPP(libcrossfade_MOC_SRCS ${libcrossfade_MOC_HDRS})

# user interface

SET(libcrossfade_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libcrossfade_UIS_H ${libcrossfade_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(crossfade MODULE ${libcrossfade_SRCS} ${libcrossfade_MOC_SRCS} ${libcrossfade_UIS_H} ${libcrossfade_RCC_SRCS})
add_dependencies(crossfade qmmp)
target_link_libraries(crossfade ${QT_LIBRARIES} -lqmmp)
install(TARGETS crossfade DESTINATION ${LIB_DIR}/qmmp/Effect)
