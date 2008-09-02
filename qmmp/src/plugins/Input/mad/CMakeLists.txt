project(libmad)

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

# libmad
pkg_search_module(MAD libmad mad)

include_directories(${MAD_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${MAD_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})

SET(libmad_SRCS
  decoder_mad.cpp
  decodermadfactory.cpp
  detailsdialog.cpp
  settingsdialog.cpp
  tagextractor.cpp
)

SET(libmad_MOC_HDRS
  settingsdialog.h
  decodermadfactory.h
  decoder_mad.h
  detailsdialog.h
  tagextractor.h
)

SET(libmad_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmad_RCC_SRCS ${libmad_RCCS})

QT4_WRAP_CPP(libmad_MOC_SRCS ${libmad_MOC_HDRS})

# user interface


SET(libmad_UIS
    detailsdialog.ui
    settingsdialog.ui
)

QT4_WRAP_UI(libmad_UIS_H ${libmad_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MAD_FOUND)
ADD_LIBRARY(mad SHARED ${libmad_SRCS} ${libmad_MOC_SRCS} ${libmad_UIS_H} ${libmad_RCC_SRCS})
add_dependencies(mad qmmp)
target_link_libraries(mad ${QT_LIBRARIES} -lqmmp ${MAD_LDFLAGS} ${MAD_CFLAGS} ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS mad DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(MAD_FOUND)
