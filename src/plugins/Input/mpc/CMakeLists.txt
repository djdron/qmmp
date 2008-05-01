project(libmpc)

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

# libmpc and taglib
PKGCONFIG(taglib TAGLIB_INCLUDE_DIR TAGLIB_LINK_DIR TAGLIB_LINK_FLAGS TAGLIB_CFLAGS)

IF(NOT TAGLIB_LINK_FLAGS)
  SET(TAGLIB_LINK_FLAGS -ltag)
  SET(TAGLIB_INCLUDE_DIR /usr/include/taglib)
  SET(TAGLIB_CFLAGS -I/usr/include/taglib)
ENDIF(NOT TAGLIB_LINK_FLAGS)

include_directories(${FLAC_INCLUDE_DIR} ${TAGLIB_INCLUDE_DIR})
link_directories(${FLAC_LINK_DIR} ${TAGLIB_LINK_DIR})

ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libmpc_SRCS
  decoder_mpc.cpp
  decodermpcfactory.cpp
  detailsdialog.cpp
)

SET(libmpc_MOC_HDRS
  decodermpcfactory.h
  decoder_mpc.h
  detailsdialog.h
)

SET(libmpc_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libmpc_RCC_SRCS ${libmpc_RCCS})

QT4_WRAP_CPP(libmpc_MOC_SRCS ${libmpc_MOC_HDRS})

# user interface


SET(libmpc_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libmpc_UIS_H ${libmpc_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(mpc SHARED ${libmpc_SRCS} ${libmpc_MOC_SRCS} ${libmpc_UIS_H}
  ${libmpc_RCC_SRCS})
target_link_libraries(mpc ${QT_LIBRARIES} -lqmmp -lmpcdec ${TAGLIB_LINK_FLAGS} ${TAGLIB_CFLAGS})
install(TARGETS mpc DESTINATION ${LIB_DIR}/qmmp/Input PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

# clean remaining files

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES 
			 "CMakeCache.txt;Makefile;cmake_install.cmake"
)

