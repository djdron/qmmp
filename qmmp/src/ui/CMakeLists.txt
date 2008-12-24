project(ui)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
   cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy) 

SET(QT_USE_QTNETWORK TRUE)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

include(${QT_USE_FILE})

ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# libqmmp & libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_INSTALL_PREFIX}/${LIB_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmp)

SET(ui_SRCS
    fft.c
    aboutdialog.cpp
    addurldialog.cpp
    balancebar.cpp
    button.cpp
    builtincommandlineoption.cpp
    configdialog.cpp
    display.cpp
    dock.cpp
    eqgraph.cpp
    eqpreset.cpp
    eqslider.cpp
    eqtitlebar.cpp
    eqwidget.cpp
    jumptotrackdialog.cpp
    keyboardmanager.cpp
    listwidget.cpp
    logscale.cpp
    mainvisual.cpp
    mainwindow.cpp
    monostereo.cpp
    mp3player.cpp
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
    qmmpstarter.cpp
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
    unixdomainsocket.cpp
    visualmenu.cpp
    volumebar.cpp
)

SET(ui_MOC_HDRS
    aboutdialog.h
    addurldialog.h
    balancebar.h
    button.h
    builtincommandlineoption.h
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
    jumptotrackdialog.h
    keyboardmanager.h
    listwidget.h
    logscale.h
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
    qmmpstarter.h
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
    unixdomainsocket.h
    visualmenu.h
    volumebar.h
)

SET(ui_RCCS images/images.qrc stuff.qrc translations/qmmp_locales.qrc)

QT4_ADD_RESOURCES(ui_RCC_SRCS ${ui_RCCS})

QT4_AUTOMOC(${ui_MOC_SRC})
QT4_WRAP_CPP(ui_MOC_SRCS ${ui_MOC_HDRS})

# user interface


SET(ui_UIS
    forms/aboutdialog.ui
    forms/addurldialog.ui
    forms/configdialog.ui
    forms/jumptotrackdialog.ui
    forms/preseteditor.ui
)

QT4_WRAP_UI(ui_UIS_H ${ui_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(qmmp ${ui_SRCS} ${ui_MOC_SRCS} ${ui_UIS_H}
  ${ui_RCC_SRCS})
target_link_libraries(qmmp ${QT_LIBRARIES} libqmmp qmmpui)
add_dependencies(qmmp qmmpui libqmmp)
install(TARGETS qmmp DESTINATION bin)
install(FILES qmmp.desktop DESTINATION share/applications)
install(FILES images/16x16/qmmp.png DESTINATION share/icons/hicolor/16x16/apps)
install(FILES images/32x32/qmmp.png DESTINATION share/icons/hicolor/32x32/apps)
install(FILES images/48x48/qmmp.png DESTINATION share/icons/hicolor/48x48/apps)

