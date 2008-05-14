project(libwavpack)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)

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

# wavpack 
PKGCONFIG(wavpack WAVPACK_INCLUDE_DIR WAVPACK_LINK_DIR WAVPACK_LINK_FLAGS WAVPACK_CFLAGS)

IF(NOT WAVPACK_LINK_FLAGS)
  SET(WAVPACK_LINK_FLAGS -lwavpack -lm)
  SET(WAVPACK_INCLUDE_DIR /usr/include)
  SET(WAVPACK_CFLAGS -I/usr/include)
  SET(WAVPACK_LINK_DIR /usr/lib)
ENDIF(NOT WAVPACK_LINK_FLAGS)

ADD_DEFINITIONS(${WAVPACK_CFLAGS})
include_directories(${CWAVPACK_INCLUDE_DIR})
link_directories(${WAVPACK_LINK_DIR})

SET(libwavpack_SRCS
  decoder_wavpack.cpp
  decoderwavpackfactory.cpp
  detailsdialog.cpp
)

SET(libwavpack_MOC_HDRS
  decoderwavpackfactory.h
  decoder_wavpack.h
  detailsdialog.h
)

SET(libwavpack_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libwavpack_RCC_SRCS ${libwavpack_RCCS})

QT4_WRAP_CPP(libwavpack_MOC_SRCS ${libwavpack_MOC_HDRS})

# user interface


SET(libwavpack_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libwavpack_UIS_H ${libwavpack_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(wavpack SHARED ${libwavpack_SRCS} ${libwavpack_MOC_SRCS} ${libwavpack_UIS_H}
  ${libwavpack_RCC_SRCS})
target_link_libraries(wavpack ${QT_LIBRARIES} -lqmmp ${WAVPACK_LINK_FLAGS} ${WAVPACK_CLAGS})
install(TARGETS wavpack DESTINATION ${LIB_DIR}/qmmp/Input)


