project(libflac)

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

# libflac and taglib
PKGCONFIG(flac FLAC_INCLUDE_DIR FLAC_LINK_DIR FLAC_LINK_FLAGS FLAC_CFLAGS)
PKGCONFIG(taglib TAGLIB_INCLUDE_DIR TAGLIB_LINK_DIR TAGLIB_LINK_FLAGS TAGLIB_CFLAGS)

IF(NOT FLAC_LINK_FLAGS)
  SET(FLAC_LINK_FLAGS -lFLAC)
ENDIF(NOT FLAC_LINK_FLAGS)

IF(NOT TAGLIB_LINK_FLAGS)
  SET(TAGLIB_LINK_FLAGS -ltag)
  SET(TAGLIB_INCLUDE_DIR /usr/include/taglib)
  SET(TAGLIB_CFLAGS -I/usr/include/taglib)
ENDIF(NOT TAGLIB_LINK_FLAGS)

include_directories(${FLAC_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR})
link_directories(${FLAC_LINK_DIR} ${TAGLIB_LINK_DIR})

ADD_DEFINITIONS(${FLAC_CFLAGS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libflac_SRCS
  decoder_flac.cpp
  decoderflacfactory.cpp
  detailsdialog.cpp
)

SET(libflac_MOC_HDRS
  decoderflacfactory.h
  decoder_flac.h
  detailsdialog.h
)

SET(libflac_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libflac_RCC_SRCS ${libflac_RCCS})

QT4_WRAP_CPP(libflac_MOC_SRCS ${libflac_MOC_HDRS})

# user interface


SET(libflac_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libflac_UIS_H ${libflac_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(flac SHARED ${libflac_SRCS} ${libflac_MOC_SRCS} ${libflac_UIS_H}
  ${libflac_RCC_SRCS})
target_link_libraries(flac ${QT_LIBRARIES} -lqmmp ${FLAC_LINK_FLAGS} ${FLAC_CFLAGS} ${TAGLIB_LINK_FLAGS} ${TAGLIB_CFLAGS})
install(TARGETS flac DESTINATION ${LIB_DIR}/qmmp/Input PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

