project(liboss)

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


SET(liboss_SRCS
  outputossfactory.cpp
  outputoss.cpp
  settingsdialog.cpp
)

SET(liboss_MOC_HDRS
  outputossfactory.h
  outputoss.h
  settingsdialog.h
)

SET(liboss_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(liboss_RCC_SRCS ${liboss_RCCS})

QT4_WRAP_CPP(liboss_MOC_SRCS ${liboss_MOC_HDRS})

SET(liboss_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(liboss_UIS_H ${liboss_UIS})

ADD_LIBRARY(oss SHARED ${liboss_SRCS} ${liboss_MOC_SRCS} ${liboss_UIS_H}
  ${liboss_RCC_SRCS})
add_dependencies(oss qmmp)
target_link_libraries(oss ${QT_LIBRARIES} -lqmmp)
install(TARGETS oss DESTINATION ${LIB_DIR}/qmmp/Output)

