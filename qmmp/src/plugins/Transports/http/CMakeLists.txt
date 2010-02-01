project(libhttp)

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
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libcurl
pkg_search_module(CURL libcurl)
include_directories(${CURL_INCLUDE_DIRS})
link_directories(${CURL_LIBRARY_DIRS})

SET(libhttp_SRCS
    streamreader.cpp
    downloader.cpp
    httpinputfactory.cpp
    httpinputsource.cpp
    settingsdialog.cpp
)

SET(libhttp_MOC_HDRS    
    downloader.h
    httpinputfactory.h
    httpinputsource.h
    streamreader.h
    settingsdialog.h
)

# resources
SET(libhttp_RCCS translations/translations.qrc)
QT4_ADD_RESOURCES(libhttp_RCC_SRCS ${libhttp_RCCS})

# user interface
SET(libhttp_UIS
    settingsdialog.ui
)
QT4_WRAP_UI(libhttp_UIS_H ${libhttp_UIS})



QT4_WRAP_CPP(libhttp_MOC_SRCS ${libhttp_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(CURL_FOUND)
ADD_LIBRARY(http MODULE ${libhttp_SRCS} ${libhttp_MOC_SRCS} ${libhttp_UIS_H} ${libhttp_RCC_SRCS})
add_dependencies(http qmmp)
target_link_libraries(http ${QT_LIBRARIES} -lqmmp ${CURL_LDFLAGS} ${CURL_CFLAGS})
install(TARGETS http DESTINATION ${LIB_DIR}/qmmp/Transports)
ENDIF(CURL_FOUND)
