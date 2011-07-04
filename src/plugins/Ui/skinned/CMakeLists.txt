project(ui)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy) 

SET(QT_USE_QTNETWORK TRUE)

INCLUDE(FindQt4)

include(${QT_USE_FILE})

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


# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# X11
include_directories(${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})

SET(ui_SRCS
    fft.c
    aboutdialog.cpp
    addurldialog.cpp
    balancebar.cpp
    button.cpp
    configdialog.cpp
    display.cpp
    dock.cpp
    eqgraph.cpp
    eqpreset.cpp
    eqslider.cpp
    eqtitlebar.cpp
    eqwidget.cpp
    keyboardmanager.cpp
    listwidget.cpp
    mainvisual.cpp
    mainwindow.cpp
    monostereo.cpp
    number.cpp
    pixmapwidget.cpp
    playlistcontrol.cpp
    playlist.cpp
    playlistslider.cpp
    playlisttitlebar.cpp
    playstatus.cpp
    pluginitem.cpp
    positionbar.cpp
    preseteditor.cpp
    shadedbar.cpp
    shadedvisual.cpp
    skin.cpp
    skinreader.cpp
    symboldisplay.cpp
    textscroller.cpp
    timeindicator.cpp
    titlebarcontrol.cpp
    titlebar.cpp
    togglebutton.cpp
    visualmenu.cpp
    volumebar.cpp
    cursorimage.cpp
    playlistbrowser.cpp
    playlistselector.cpp
    popupwidget.cpp
    popupsettings.cpp
    windowsystem.cpp
    actionmanager.cpp
    shortcutitem.cpp
    shortcutdialog.cpp
    skinnedfactory.cpp
)

SET(ui_MOC_HDRS
    aboutdialog.h
    addurldialog.h
    balancebar.h
    button.h
    configdialog.h
    display.h
    dock.h
    eqgraph.h
    eqpreset.h
    eqslider.h
    eqtitlebar.h
    eqwidget.h
    fft.h
    inlines.h
    keyboardmanager.h
    listwidget.h
    mainvisual.h
    mainwindow.h
    monostereo.h
    number.h
    pixmapwidget.h
    playlistcontrol.h
    playlist.h
    playlistslider.h
    playlisttitlebar.h
    playstatus.h
    pluginitem.h
    positionbar.h
    preseteditor.h
    shadedbar.h
    shadedvisual.h
    skin.h
    skinreader.h
    symboldisplay.h
    textscroller.h
    timeindicator.h
    titlebarcontrol.h
    titlebar.h
    togglebutton.h
    visualmenu.h
    volumebar.h
    cursorimage.h
    playlistbrowser.h
    playlistselector.h
    popupwidget.h
    popupsettings.h
    windowsystem.h
    shortcutitem.h
    actionmanager.h
    shortcutdialog.h
    skinnedfactory.h
)

SET(ui_RCCS images/images.qrc stuff.qrc translations/translations.qrc)

QT4_ADD_RESOURCES(ui_RCC_SRCS ${ui_RCCS})

QT4_AUTOMOC(${ui_MOC_SRC})
QT4_WRAP_CPP(ui_MOC_SRCS ${ui_MOC_HDRS})

# user interface


SET(ui_UIS
    forms/aboutdialog.ui
    forms/addurldialog.ui
    forms/configdialog.ui
    forms/preseteditor.ui
    forms/playlistbrowser.ui
    forms/popupsettings.ui
    forms/shortcutdialog.ui
)

QT4_WRAP_UI(ui_UIS_H ${ui_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(X11_FOUND)
ADD_LIBRARY(skinned MODULE ${ui_SRCS} ${ui_MOC_SRCS} ${ui_UIS_H} ${ui_RCC_SRCS})
target_link_libraries(skinned ${QT_LIBRARIES} libqmmp qmmpui ${X11_LDFLAGS})
add_dependencies(skinned qmmpui libqmmp)
target_link_libraries(skinned ${QT_LIBRARIES} -lqmmpui -lqmmp ${X11_LDFLAGS})
install(TARGETS skinned DESTINATION ${LIB_DIR}/qmmp/Ui)
ENDIF(X11_FOUND)
