project(libxmp)

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

#SET(QT_INCLUDES
#  ${QT_INCLUDES}
#  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
#)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# xmp
pkg_search_module(XMP libxmp)

include_directories(${XMP_INCLUDE_DIRS})
link_directories(${XMP_LIBRARY_DIRS})
ADD_DEFINITIONS(${XMP_CFLAGS})

SET(libxmp_SRCS
  decoder_xmp.cpp
  decoderxmpfactory.cpp
  xmpmetadatamodel.cpp
  settingsdialog.cpp
  xmpmetadatamodel.cpp
)

SET(libxmp_HDRS
  decoder_xmp.h
)

SET(libxmp_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libxmp_RCC_SRCS ${libxmp_RCCS})

# user interface

SET(libxmp_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libxmp_UIS_H ${libxmp_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(XMP_FOUND)
ADD_LIBRARY(xmp MODULE ${libxmp_SRCS} ${libxmp_UIS_H} ${libxmp_RCC_SRCS} ${libxmp_HDRS})
target_link_libraries(xmp ${QT_LIBRARIES} -lqmmp ${QMMP_LDFLAGS} ${XMP_LDFLAGS})
install(TARGETS xmp DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(XMP_FOUND)
