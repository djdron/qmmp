project(libsrconverter)

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

PKGCONFIG(samplerate SAMPLERATE_INCLUDE_DIR SAMPLERATE_LINK_DIR SAMPLERATE_LINK_FLAGS SAMPLERATE_CFLAGS)

IF(NOT SAMPLERATE_LINK_FLAGS)
  SET(SAMPLERATE_LINK_FLAGS -lsamplerate)
ENDIF(NOT SAMPLERATE_LINK_FLAGS)

include_directories(${SAMPLERATE_INCLUDE_DIR})
link_directories(${SAMPLERATE_LINK_DIR})

ADD_DEFINITIONS(${SAMPLERATE_CFLAGS})

SET(libsrconverter_SRCS
    srconverter.cpp
    settingsdialog.cpp
    effectsrconverterfactory.cpp
)

SET(libsrconverter_MOC_HDRS
    srconverter.h
    settingsdialog.h
    effectsrconverterfactory.h
)

SET(libsrconverter_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libsrconverter_RCC_SRCS ${libsrconverter_RCCS})

QT4_WRAP_CPP(libsrconverter_MOC_SRCS ${libsrconverter_MOC_HDRS})

# user interface


SET(libsrconverter_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libsrconverter_UIS_H ${libsrconverter_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(srconverter SHARED ${libsrconverter_SRCS} ${libsrconverter_MOC_SRCS} ${libsrconverter_UIS_H}
  ${libsrconverter_RCC_SRCS})
target_link_libraries(srconverter ${QT_LIBRARIES} -lqmmp ${SAMPLERATE_LINK_FLAGS})
install(TARGETS srconverter DESTINATION ${LIB_DIR}/qmmp/Effect)

