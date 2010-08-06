project(libwildmidi)

cmake_minimum_required(VERSION 2.6.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
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


SET(libwildmidi_SRCS
  decoder_wildmidi.cpp
  decoderwildmidifactory.cpp
  settingsdialog.cpp
)

SET(libwildmidi_MOC_HDRS
  decoder_wildmidi.h
  decoderwildmidifactory.h
  settingsdialog.h
)

# libwildmidi

CHECK_INCLUDE_FILE(wildmidi_lib.h WILDMIDI_FOUND)

IF(WILDMIDI_FOUND)
SET(WILDMIDI_FOUND TRUE CACHE INTERNAL "wildmidi")
ENDIF(WILDMIDI_FOUND)

SET(libwildmidi_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libwildmidi_RCC_SRCS ${libwildmidi_RCCS})

QT4_WRAP_CPP(libwildmidi_MOC_SRCS ${libwildmidi_MOC_HDRS})

# user interface


SET(libwildmidi_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libwildmidi_UIS_H ${libwildmidi_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(WILDMIDI_FOUND)
ADD_LIBRARY(wildmidi MODULE ${libwildmidi_SRCS} ${libwildmidi_MOC_SRCS} ${libwildmidi_UIS_H} ${libwildmidi_RCC_SRCS})
add_dependencies(wildmidi qmmp)
target_link_libraries(wildmidi ${QT_LIBRARIES} -lqmmp -lWildMidi)
install(TARGETS wildmidi DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(WILDMIDI_FOUND)
