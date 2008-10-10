project(libcue)

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


SET(libcue_SRCS
  decoder_cue.cpp
  decodercuefactory.cpp
#  detailsdialog.cpp
  cueparser.cpp
)

SET(libcue_MOC_HDRS
  decodercuefactory.h
  decoder_cue.h
#  detailsdialog.h
   cueparser.h
)

#SET(libcue_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libcue_RCC_SRCS ${libcue_RCCS})

QT4_WRAP_CPP(libcue_MOC_SRCS ${libcue_MOC_HDRS})

# user interface


#SET(libcue_UIS
#    detailsdialog.ui
#)

#QT4_WRAP_UI(libcue_UIS_H ${libcue_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(cue SHARED ${libcue_SRCS} ${libcue_MOC_SRCS} ${libcue_RCC_SRCS})
add_dependencies(cue qmmp)
target_link_libraries(cue ${QT_LIBRARIES} -lqmmp)
install(TARGETS cue DESTINATION ${LIB_DIR}/qmmp/Input)

