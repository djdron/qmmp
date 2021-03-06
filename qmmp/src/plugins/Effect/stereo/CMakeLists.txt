project(libstereo)

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

SET(libstereo_SRCS
    stereoplugin.cpp
    settingsdialog.cpp
    effectstereofactory.cpp
)

SET(libstereo_HDRS
    stereoplugin.h
)

SET(libstereo_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libstereo_RCC_SRCS ${libstereo_RCCS})

# user interface

SET(libstereo_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libstereo_UIS_H ${libstereo_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(stereo MODULE ${libstereo_SRCS} ${libstereo_UIS_H} ${libstereo_RCC_SRCS} ${libstereo_HDRS})
add_dependencies(stereo qmmp)
target_link_libraries(stereo ${QT_LIBRARIES} libqmmp)
install(TARGETS stereo DESTINATION ${LIB_DIR}/qmmp/Effect)
