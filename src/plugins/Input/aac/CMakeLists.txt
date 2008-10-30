project(libaac)

INCLUDE(CheckIncludeFileCXX)
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

# libaac

CHECK_INCLUDE_FILE_CXX(neaacdec.h FAAD_FOUND)

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})

SET(libaac_SRCS
  decoder_aac.cpp
  decoderaacfactory.cpp
  detailsdialog.cpp
  aacfile.cpp
)

SET(libaac_MOC_HDRS
  decoderaacfactory.h
  decoder_aac.h
  detailsdialog.h
  aacfile.h
)

#SET(libaac_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libaac_RCC_SRCS ${libaac_RCCS})

QT4_WRAP_CPP(libaac_MOC_SRCS ${libaac_MOC_HDRS})

# user interface


SET(libaac_UIS
    detailsdialog.ui
)

QT4_WRAP_UI(libaac_UIS_H ${libaac_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FAAD_FOUND)
ADD_LIBRARY(aac SHARED ${libaac_SRCS} ${libaac_MOC_SRCS} ${libaac_UIS_H}
  ${libaac_RCC_SRCS})
add_dependencies(aac qmmp)
target_link_libraries(aac ${QT_LIBRARIES} -lqmmp -lfaad ${TAGLIB_LDFLAGS} ${TAGLIB_CFLAGS})
install(TARGETS aac DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(FAAD_FOUND)
