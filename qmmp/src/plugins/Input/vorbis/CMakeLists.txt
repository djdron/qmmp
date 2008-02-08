project(libvorbis)

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

# libvorbis and taglib
PKGCONFIG(ogg OGG_INCLUDE_DIR OGG_LINK_DIR OGG_LINK_FLAGS OGG_CFLAGS)
PKGCONFIG(vorbis VORBIS_INCLUDE_DIR VORBIS_LINK_DIR VORBIS_LINK_FLAGS VORBIS_CFLAGS)
PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)

PKGCONFIG(taglib TAGLIB_INCLUDE_DIR TAGLIB_LINK_DIR TAGLIB_LINK_FLAGS TAGLIB_CFLAGS)

IF(NOT OGG_LINK_FLAGS)
  SET(OGG_LINK_FLAGS -logg)
ENDIF(NOT OGG_LINK_FLAGS)

IF(NOT VORBIS_LINK_FLAGS)
  SET(VORBIS_LINK_FLAGS -lvorbis)
ENDIF(NOT VORBIS_LINK_FLAGS)

IF(NOT VORBISFILE_LINK_FLAGS)
  SET(VORBISFILE_LINK_FLAGS -lvorbisfile)
ENDIF(NOT VORBISFILE_LINK_FLAGS)

IF(NOT TAGLIB_LINK_FLAGS)
  SET(TAGLIB_LINK_FLAGS -ltag)
  SET(TAGLIB_INCLUDE_DIR /usr/include/taglib)
  SET(TAGLIB_CFLAGS -I/usr/include/taglib)
ENDIF(NOT TAGLIB_LINK_FLAGS)

include_directories(${VORBIS_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR})
link_directories(${VORBIS_LINK_DIR} ${TAGLIB_LINK_DIR})

#ADD_DEFINITIONS(${VORBIS_CFLAGS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


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

ADD_LIBRARY(vorbis SHARED ${libvorbis_SRCS} ${libvorbis_MOC_SRCS} ${libvorbis_UIS_H}
  ${libvorbis_RCC_SRCS})
target_link_libraries(vorbis ${QT_LIBRARIES} -lqmmp ${VORBIS_LINK_FLAGS} ${VORBISFILE_LINK_FLAGS} ${OGG_LINK_FLAGS} ${TAGLIB_LINK_FLAGS} ${TAGLIB_CFLAGS})
install(TARGETS vorbis DESTINATION ${LIB_DIR}/qmmp/Input PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
