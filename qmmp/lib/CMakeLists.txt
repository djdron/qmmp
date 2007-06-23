project(libqmmp)

cmake_minimum_required(VERSION 2.4.0)

include(FindQt4)
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

# qt lib
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
#ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(libqmmp_SRCS
    recycler.cpp
    decoder.cpp
    output.cpp
    equ/iir.c
    equ/iir_cfs.c
    equ/iir_fpu.c
    soundcore.cpp
)

SET(libqmmp_MOC_HDRS
  visualization.h
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
)

QT4_WRAP_CPP(libqmmp_MOC_SRCS ${libqmmp_MOC_HDRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(qmmp SHARED ${libqmmp_SRCS} ${libqmmp_MOC_SRCS})
target_link_libraries(qmmp ${QT_LIBRARIES})
install(TARGETS qmmp LIBRARY DESTINATION lib)

