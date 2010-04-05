project(libmms)

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
pkg_search_module(MMS libmms>=0.4)
include_directories(${MMS_INCLUDE_DIRS})
link_directories(${MMS_LIBRARY_DIRS})


SET(libmms_SRCS
    streamreader.cpp
    downloader.cpp
    mmsinputfactory.cpp
    mmsinputsource.cpp
    settingsdialog.cpp
)

SET(libmms_MOC_HDRS    
    downloader.h
    mmsinputfactory.h
    mmsinputsource.h
    streamreader.h
    settingsdialog.h
)

# resources
SET(libmms_RCCS translations/translations.qrc)
QT4_ADD_RESOURCES(libmms_RCC_SRCS ${libmms_RCCS})

# user interface
SET(libmms_UIS
    settingsdialog.ui
)
QT4_WRAP_UI(libmms_UIS_H ${libmms_UIS})



QT4_WRAP_CPP(libmms_MOC_SRCS ${libmms_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(MMS_FOUND)
ADD_LIBRARY(mms MODULE ${libmms_SRCS} ${libmms_MOC_SRCS} ${libmms_UIS_H} ${libmms_RCC_SRCS})
target_link_libraries(mms ${QT_LIBRARIES} -lqmmp ${MMS_LDFLAGS} ${MMS_CFLAGS})
add_dependencies(mms qmmp)
install(TARGETS mms DESTINATION ${LIB_DIR}/qmmp/Transports)
ENDIF(MMS_FOUND)
