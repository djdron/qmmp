project(libsrconverter)

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

pkg_check_modules(SAMPLERATE samplerate)

include_directories(${SAMPLERATE_INCLUDE_DIRS})
link_directories(${SAMPLERATE_LIBRARY_DIRS})
ADD_DEFINITIONS(${SAMPLERATE_CFLAGS})

SET(libsrconverter_SRCS
    srconverter.cpp
    settingsdialog.cpp
    effectsrconverterfactory.cpp
)

SET(libsrconverter_HDRS
    srconverter.h
)

SET(libsrconverter_MOC_HDRS
    
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

IF(SAMPLERATE_FOUND)
ADD_LIBRARY(srconverter MODULE ${libsrconverter_SRCS} ${libsrconverter_MOC_SRCS} ${libsrconverter_UIS_H}
  ${libsrconverter_RCC_SRCS} ${libsrconverter_HDRS})
add_dependencies(srconverter qmmp)
target_link_libraries(srconverter ${QT_LIBRARIES} -lqmmp ${SAMPLERATE_LDFLAGS})
install(TARGETS srconverter DESTINATION ${LIB_DIR}/qmmp/Effect)
ENDIF(SAMPLERATE_FOUND)
