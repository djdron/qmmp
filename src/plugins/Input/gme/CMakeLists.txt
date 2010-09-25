project(libgme)

INCLUDE(CheckIncludeFileCXX)
cmake_minimum_required(VERSION 2.6.0)

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

# libgme

CHECK_INCLUDE_FILE_CXX(gme/Music_Emu.h GME_FOUND)

IF(GME_FOUND)

ENDIF(GME_FOUND)

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})

SET(libgme_SRCS
  decoder_gme.cpp
  decodergmefactory.cpp
  gmehelper.cpp
)

SET(libgme_MOC_HDRS
  decodergmefactory.h
  decoder_gme.h
  gmehelper.h
)

SET(libgme_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libgme_RCC_SRCS ${libgme_RCCS})

QT4_WRAP_CPP(libgme_MOC_SRCS ${libgme_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(GME_FOUND)
ADD_LIBRARY(gme MODULE ${libgme_SRCS} ${libgme_MOC_SRCS} ${libgme_RCC_SRCS})
add_dependencies(gme qmmp)
target_link_libraries(gme ${QT_LIBRARIES} -lqmmp -lgme ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS gme DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(GME_FOUND)
