project(libmpc)

cmake_minimum_required(VERSION 2.4.0)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
              

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

# libmpc

SET(MPC_FOUND TRUE CACHE BOOL INTERNAL) #add test

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})

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

IF(MPC_FOUND)
ADD_LIBRARY(mpc SHARED ${libmpc_SRCS} ${libmpc_MOC_SRCS} ${libmpc_UIS_H}
  ${libmpc_RCC_SRCS})
target_link_libraries(mpc ${QT_LIBRARIES} -lqmmp -lmpcdec ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS mpc DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(MPC_FOUND)
