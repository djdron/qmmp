project(libvorbis)

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
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libvorbis and taglib
pkg_check_modules(VORBIS ogg vorbis vorbisfile)

include_directories(${VORBIS_INCLUDE_DIRS}  ${TAGLIB_INCLUDE_DIRS})
link_directories(${VORBIS_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})

SET(libvorbis_SRCS
  decoder_vorbis.cpp
  decodervorbisfactory.cpp
  detailsdialog.cpp
)

SET(libvorbis_MOC_HDRS
  decodervorbisfactory.h
  decoder_vorbis.h
  detailsdialog.h
)

SET(libvorbis_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libvorbis_RCC_SRCS ${libvorbis_RCCS})

QT4_WRAP_CPP(libvorbis_MOC_SRCS ${libvorbis_MOC_HDRS})

# user interface


SET(libvorbis_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libvorbis_UIS_H ${libvorbis_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(VORBIS_FOUND)
ADD_LIBRARY(vorbis SHARED ${libvorbis_SRCS} ${libvorbis_MOC_SRCS} ${libvorbis_UIS_H}
  ${libvorbis_RCC_SRCS})
target_link_libraries(vorbis ${QT_LIBRARIES} -lqmmp ${VORBIS_LDFLAGS} ${VORBIS_CFLAGS} ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS vorbis DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(VORBIS_FOUND)
