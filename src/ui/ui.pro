# ???? ?????? ? KDevelop ?????????? qmake.
# -------------------------------------------
# ?????????? ???????????? ???????? ???????? ???????: ./src
# ???? - ??????????: ../bin/mp3player
include(../../qmmp.pri)
FORMS += ./forms/configdialog.ui \
    ./forms/preseteditor.ui \
    ./forms/jumptotrackdialog.ui \
    ./forms/aboutdialog.ui \
    ./forms/addurldialog.ui \
    ./forms/playlistbrowser.ui \
    ./forms/popupsettings.ui
HEADERS += mainwindow.h \
    button.h \
    display.h \
    skin.h \
    titlebar.h \
    positionbar.h \
    number.h \
    playlist.h \
    listwidget.h \
    pixmapwidget.h \
    playlisttitlebar.h \
    configdialog.h \
    playlistslider.h \
    dock.h \
    eqwidget.h \
    eqtitlebar.h \
    eqslider.h \
    togglebutton.h \
    eqgraph.h \
    mainvisual.h \
    inlines.h \
    fft.h \
    logscale.h \
    textscroller.h \
    monostereo.h \
    playstatus.h \
    pluginitem.h \
    volumebar.h \
    balancebar.h \
    symboldisplay.h \
    playlistcontrol.h \
    qmmpstarter.h \
    eqpreset.h \
    preseteditor.h \
    jumptotrackdialog.h \
    aboutdialog.h \
    timeindicator.h \
    keyboardmanager.h \
    addurldialog.h \
    skinreader.h \
    visualmenu.h \
    titlebarcontrol.h \
    shadedvisual.h \
    shadedbar.h \
    builtincommandlineoption.h \
    cursorimage.h \
    playlistbrowser.h \
    playlistselector.h \
    popupwidget.h \
    popupsettings.h
SOURCES += mainwindow.cpp \
    mp3player.cpp \
    button.cpp \
    display.cpp \
    skin.cpp \
    titlebar.cpp \
    positionbar.cpp \
    number.cpp \
    playlist.cpp \
    listwidget.cpp \
    pixmapwidget.cpp \
    playlisttitlebar.cpp \
    configdialog.cpp \
    playlistslider.cpp \
    dock.cpp \
    eqwidget.cpp \
    eqtitlebar.cpp \
    eqslider.cpp \
    togglebutton.cpp \
    eqgraph.cpp \
    mainvisual.cpp \
    fft.c \
    logscale.cpp \
    textscroller.cpp \
    monostereo.cpp \
    playstatus.cpp \
    pluginitem.cpp \
    volumebar.cpp \
    balancebar.cpp \
    symboldisplay.cpp \
    playlistcontrol.cpp \
    qmmpstarter.cpp \
    eqpreset.cpp \
    preseteditor.cpp \
    jumptotrackdialog.cpp \
    aboutdialog.cpp \
    timeindicator.cpp \
    keyboardmanager.cpp \
    addurldialog.cpp \
    skinreader.cpp \
    visualmenu.cpp \
    titlebarcontrol.cpp \
    shadedvisual.cpp \
    shadedbar.cpp \
    builtincommandlineoption.cpp \
    cursorimage.cpp \
    playlistbrowser.cpp \
    playlistselector.cpp \
    popupwidget.cpp \
    popupsettings.cpp
win32:HEADERS += ../qmmp/visual.h
unix { 
    HEADERS += unixdomainsocket.h
    SOURCES += unixdomainsocket.cpp
}

# Some conf to redirect intermediate stuff in separate dirs
UI_DIR = ./.build/ui/
MOC_DIR = ./.build/moc/
OBJECTS_DIR = ./.build/obj
QT += network
unix:TARGET = ../../bin/qmmp
win32:TARGET = ../../../bin/qmmp
CONFIG += thread \
    release \
    warn_on
QMAKE_LIBDIR += ../../lib \
    qmmpui
LIBS += -Wl,-rpath,../lib
unix:LIBS += -L../../lib \
    -lqmmp \
    -lqmmpui
win32:LIBS += -L../../bin \
    -lqmmp0 \
    -lqmmpui0
INCLUDEPATH += ../
RESOURCES = images/images.qrc \
    stuff.qrc
TEMPLATE = app
unix { 
    target.path = /bin
    desktop.files = qmmp.desktop \
        qmmp_enqueue.desktop
    desktop.path = /share/applications
    icon16.files = images/16x16/qmmp.png
    icon32.files = images/32x32/qmmp.png
    icon48.files = images/48x48/qmmp.png
    icon16.path = /share/icons/hicolor/16x16/apps
    icon32.path = /share/icons/hicolor/32x32/apps
    icon48.path = /share/icons/hicolor/48x48/apps
    INSTALLS += desktop \
        target \
        icon16 \
        icon32 \
        icon48
}
RESOURCES += translations/qmmp_locales.qrc
TRANSLATIONS = translations/qmmp_ru.ts \
    translations/qmmp_tr.ts \
    translations/qmmp_zh_CN.ts \
    translations/qmmp_cs.ts \
    translations/qmmp_pt_BR.ts \
    translations/qmmp_uk_UA.ts \
    translations/qmmp_zh_TW.ts \
    translations/qmmp_de.ts \
    translations/qmmp_pl_PL.ts \
    translations/qmmp_it.ts \
    translations/qmmp_lt.ts
