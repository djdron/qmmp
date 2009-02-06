project(libwavpack)

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

# wavpack
pkg_search_module(WAVPACK wavpack)

include_directories(${CWAVPACK_INCLUDE_DIRS})
link_directories(${WAVPACK_LIBRARY_DIRS})

SET(libwavpack_SRCS
  decoder_wavpack.cpp
  decoderwavpackfactory.cpp
  detailsdialog.cpp
  cueparser.cpp
)

SET(libwavpack_MOC_HDRS
  decoderwavpackfactory.h
  decoder_wavpack.h
  detailsdialog.h
  cueparser.h
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

IF(WAVPACK_FOUND)
ADD_LIBRARY(wavpack SHARED ${libwavpack_SRCS} ${libwavpack_MOC_SRCS} ${libwavpack_UIS_H}
  ${libwavpack_RCC_SRCS})
add_dependencies(wavpack qmmp)
target_link_libraries(wavpack ${QT_LIBRARIES} -lqmmp ${WAVPACK_LDFLAGS} ${WAVPACK_CLAGS})
install(TARGETS wavpack DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(WAVPACK_FOUND)

