project(libmodplug)

INCLUDE(CheckIncludeFile)
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

# modplug
pkg_search_module(MODPLUG libmodplug)

# add modplug defines
CHECK_INCLUDE_FILE(stdint.h STDINT_FOUND)
IF(STDINT_FOUND)
ADD_DEFINITIONS(-DHAVE_STDINT_H)
ENDIF(STDINT_FOUND)

CHECK_INCLUDE_FILE(inttypes.h INTTYPES_FOUND)
IF(INTTYPES_FOUND)
ADD_DEFINITIONS(-DHAVE_INTTYPES_H)
ENDIF(INTTYPES_FOUND)


include_directories(${MODPLUG_INCLUDE_DIRS})
link_directories(${MODPLUG_LIBRARY_DIRS})

SET(libmodplug_SRCS
  decoder_modplug.cpp
  decodermodplugfactory.cpp
  detailsdialog.cpp
  settingsdialog.cpp
  archivereader.cpp
)

SET(libmodplug_MOC_HDRS
  decodermodplugfactory.h
  decoder_modplug.h
  detailsdialog.h
  settingsdialog.h
  archivereader.h
)

SET(libmodplug_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmodplug_RCC_SRCS ${libmodplug_RCCS})

QT4_WRAP_CPP(libmodplug_MOC_SRCS ${libmodplug_MOC_HDRS})

# user interface


SET(libmodplug_UIS
    detailsdialog.ui
    settingsdialog.ui
)

QT4_WRAP_UI(libmodplug_UIS_H ${libmodplug_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MODPLUG_FOUND)
ADD_LIBRARY(modplug SHARED ${libmodplug_SRCS} ${libmodplug_MOC_SRCS} ${libmodplug_UIS_H}
  ${libmodplug_RCC_SRCS})
add_dependencies(modplug qmmp)
target_link_libraries(modplug ${QT_LIBRARIES} -lqmmp ${MODPLUG_LDFLAGS} ${MODPLUG_CFLAGS})
install(TARGETS modplug DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(MODPLUG_FOUND)


