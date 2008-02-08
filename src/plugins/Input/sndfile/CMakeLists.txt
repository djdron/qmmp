project(libsndfile)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

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
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libsndfile
PKGCONFIG(sndfile SNDFILE_INCLUDE_DIR SNDFILE_LINK_DIR SNDFILE_LINK_FLAGS SNDFILE_CFLAGS)

IF(NOT SNDFILE_LINK_FLAGS)
  SET(SNDFILE_LINK_FLAGS -lsndfile)
  SET(SNDFILE_INCLUDE_DIR /usr/include)
  SET(SNDFILE_CFLAGS -I/usr/include)
ENDIF(NOT SNDFILE_LINK_FLAGS)

include_directories(${SNDFILE_INCLUDE_DIR})
link_directories(${SNDFILE_LINK_DIR})

ADD_DEFINITIONS(${SNDFILE_CFLAGS})


SET(libsndfile_SRCS
  decoder_sndfile.cpp
  decodersndfilefactory.cpp
)

SET(libsndfile_MOC_HDRS
  decodersndfilefactory.h
  decoder_sndfile.h
)

#SET(libsndfile_RCCS translations/translations.qrc)

#QT4_ADD_RESOURCES(libsndfile_RCC_SRCS ${libsndfile_RCCS})

QT4_WRAP_CPP(libsndfile_MOC_SRCS ${libsndfile_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(sndfile SHARED ${libsndfile_SRCS} ${libsndfile_MOC_SRCS} ${libsndfile_UIS_H}
  ${libsndfile_RCC_SRCS})
  
target_link_libraries(sndfile ${QT_LIBRARIES} -lqmmp ${SNDFILE_LINK_FLAGS})
install(TARGETS sndfile DESTINATION ${LIB_DIR}/qmmp/Input)



