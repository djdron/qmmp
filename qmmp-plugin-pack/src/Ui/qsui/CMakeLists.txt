project(libqsui)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 OLD)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

IF(SVN_VERSION)
ADD_DEFINITIONS(-DQMMP_PLUGIN_PACK_VERSION=\\\"${QMMP_PLUGIN_PACK_VERSION}-dev\\\")
ELSE(SVN_VERSION)
ADD_DEFINITIONS(-DQMMP_PLUGIN_PACK_VERSION=\\\"${QMMP_PLUGIN_PACK_VERSION}\\\")
ENDIF(SVN_VERSION)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# libqmmpui
include_directories(${QMMPUI_INCLUDE_DIRS})
link_directories( ${QMMPUI_LIBRARY_DIRS})


SET(libqsui_SRCS
    actionmanager.cpp
    listwidget.cpp
    mainwindow.cpp
    popupsettings.cpp
    popupwidget.cpp
    positionslider.cpp
    shortcutdialog.cpp
    shortcutitem.cpp
    qsuifactory.cpp
    qsuisettings.cpp
    visualmenu.cpp
    equalizer.cpp
    logo.cpp
    aboutqsuidialog.cpp
    keyboardmanager.cpp
    qsuianalyzer.cpp
    fft.c
    colorwidget.cpp
    qsuitabwidget.cpp
    qsuitabbar.cpp
    eqpreset.cpp
    filesystembrowser.cpp
    elidinglabel.cpp
    coverwidget.cpp
    playlistbrowser.cpp
    toolbareditor.cpp
)

SET(libqsui_HDRS
    shortcutitem.h
    inlines.h
    fft.h
    eqpreset.h
)

SET(libqsui_MOC_HDRS
    actionmanager.h
    listwidget.h
    mainwindow.h
    popupsettings.h
    popupwidget.h
    positionslider.h
    shortcutdialog.h
    qsuifactory.h
    qsuisettings.h
    visualmenu.h
    equalizer.h
    logo.h
    aboutqsuidialog.h
    keyboardmanager.h
    qsuianalyzer.h
    colorwidget.h
    qsuitabwidget.h
    qsuitabbar.h
    filesystembrowser.h
    elidinglabel.h
    coverwidget.h
    playlistbrowser.h
    toolbareditor.h
)

SET(libqsui_RCCS translations/translations.qrc txt/qsui_txt.qrc resources/qsui_resources.qrc)

QT4_ADD_RESOURCES(libqsui_RCC_SRCS ${libqsui_RCCS})

QT4_WRAP_CPP(libqsui_MOC_SRCS ${libqsui_MOC_HDRS})

# user interface

SET(libqsui_UIS
    forms/mainwindow.ui
    forms/popupsettings.ui
    forms/shortcutdialog.ui
    forms/qsuisettings.ui
    forms/aboutqsuidialog.ui
    forms/toolbareditor.ui
)

QT4_WRAP_UI(libqsui_UIS_H ${libqsui_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(qsui MODULE ${libqsui_SRCS} ${libqsui_MOC_SRCS} ${libqsui_UIS_H} ${libqsui_RCC_SRCS} ${libqsui_HDRS})
target_link_libraries(qsui ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${QMMPUI_LDFLAGS})
install(TARGETS qsui DESTINATION ${LIB_DIR}/qmmp/Ui)
