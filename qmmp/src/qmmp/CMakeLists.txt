project(libqmmp)

cmake_minimum_required(VERSION 2.4.0)

# qt lib
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
ADD_DEFINITIONS(-DLIB_DIR=\\\"\\/${LIB_DIR}\\\")
ADD_DEFINITIONS(-DQMMP_VERSION=${QMMP_VERSION})
IF(SVN_VERSION)
ADD_DEFINITIONS(-DQMMP_STR_VERSION=\\\"${QMMP_VERSION}-svn\\\")
ELSE(SVN_VERSION)
ADD_DEFINITIONS(-DQMMP_STR_VERSION=\\\"${QMMP_VERSION}\\\")
ENDIF(SVN_VERSION)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(libqmmp_SRCS
    visual.cpp
    recycler.cpp
    decoder.cpp
    output.cpp
    filetag.cpp
    equ/iir.c
    equ/iir_cfs.c
    equ/iir_fpu.c
    soundcore.cpp
    streamreader.cpp
    downloader.cpp
    effect.cpp
)

SET(libqmmp_MOC_HDRS
  visual.h
  recycler.h
  buffer.h
  constants.h
  decoder.h
  output.h
  filetag.h
  outputfactory.h
  equ/iir_cfs.h
  equ/iir_fpu.h
  equ/iir.h
  decoderfactory.h
  soundcore.h
  streamreader.h
  downloader.h
  effectfactory.h
  effect.h
)

QT4_WRAP_CPP(libqmmp_MOC_SRCS ${libqmmp_MOC_HDRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(qmmp SHARED ${libqmmp_SRCS} ${libqmmp_MOC_SRCS})
target_link_libraries(qmmp ${QT_LIBRARIES} curl)
SET_TARGET_PROPERTIES(qmmp PROPERTIES VERSION ${QMMP_VERSION} SOVERSION ${QMMP_SOVERSION})
install(TARGETS qmmp LIBRARY DESTINATION ${LIB_DIR})
