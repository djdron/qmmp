project(libhotkey)

cmake_minimum_required(VERSION 2.4.7)
INCLUDE(FindX11)

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

pkg_check_modules(X11 xf86miscproto x11)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libhotkey_SRCS
    settingsdialog.cpp
    hotkeyfactory.cpp
    hotkeymanager.cpp
    hotkeydialog.cpp
)

SET(libhotkey_MOC_HDRS
    settingsdialog.h
    hotkeyfactory.h
    hotkeymanager.h
    hotkeydialog.h
)

SET(libhotkey_RCCS 
    translations/translations.qrc
)

QT4_ADD_RESOURCES(libhotkey_RCC_SRCS ${libhotkey_RCCS})

QT4_WRAP_CPP(libhotkey_MOC_SRCS ${libhotkey_MOC_HDRS})

# user interface


SET(libhotkey_UIS
    settingsdialog.ui
    hotkeydialog.ui
)

QT4_WRAP_UI(libhotkey_UIS_H ${libhotkey_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(X11_FOUND)
ADD_LIBRARY(hotkey MODULE ${libhotkey_SRCS} ${libhotkey_MOC_SRCS} ${libhotkey_UIS_H} ${libhotkey_RCC_SRCS})
add_dependencies(hotkey qmmpui)
target_link_libraries(hotkey ${QT_LIBRARIES} -lqmmpui -lqmmp)
install(TARGETS hotkey DESTINATION ${LIB_DIR}/qmmp/General)
ENDIF(X11_FOUND)
