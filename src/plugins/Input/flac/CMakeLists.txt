project(libflac)

cmake_minimum_required(VERSION 2.4.7)

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

# libflac
pkg_search_module(FLAC flac)

IF(NOT FLAC_LINK_FLAGS)
  SET(FLAC_LINK_FLAGS -lFLAC)
ENDIF(NOT FLAC_LINK_FLAGS)

include_directories(${FLAC_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${FLAC_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})

SET(libflac_SRCS
  decoder_flac.cpp
  decoderflacfactory.cpp
  flacmetadatamodel.cpp
  cueparser.cpp
)

SET(libflac_MOC_HDRS
  decoderflacfactory.h
  decoder_flac.h
  flacmetadatamodel.h
  cueparser.h
)

SET(libflac_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libflac_RCC_SRCS ${libflac_RCCS})

QT4_WRAP_CPP(libflac_MOC_SRCS ${libflac_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FLAC_FOUND)
ADD_LIBRARY(flac MODULE ${libflac_SRCS} ${libflac_MOC_SRCS} ${libflac_RCC_SRCS})
add_dependencies(flac qmmp)
target_link_libraries(flac ${QT_LIBRARIES} -lqmmp ${FLAC_LDFLAGS} ${FLAC_CFLAGS} ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS flac DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(FLAC_FOUND)
