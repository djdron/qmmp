include(../../plugins.pri)
FORMS += forms/configdialog.ui \
    forms/preseteditor.ui \
    forms/jumptotrackdialog.ui \
    forms/aboutdialog.ui \
    forms/addurldialog.ui \
    forms/playlistbrowser.ui \
    forms/popupsettings.ui \
    forms/shortcutdialog.ui
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
    textscroller.h \
    monostereo.h \
    playstatus.h \
    pluginitem.h \
    volumebar.h \
    balancebar.h \
    symboldisplay.h \
    playlistcontrol.h \
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
    cursorimage.h \
    playlistbrowser.h \
    playlistselector.h \
    popupwidget.h \
    popupsettings.h \
    windowsystem.h \
    lxdesupport.h \
    actionmanager.h \
    shortcutitem.h \
    shortcutdialog.h \
    skinnedfactory.h
SOURCES += mainwindow.cpp \
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
    textscroller.cpp \
    monostereo.cpp \
    playstatus.cpp \
    pluginitem.cpp \
    volumebar.cpp \
    balancebar.cpp \
    symboldisplay.cpp \
    playlistcontrol.cpp \
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
    cursorimage.cpp \
    playlistbrowser.cpp \
    playlistselector.cpp \
    popupwidget.cpp \
    popupsettings.cpp \
    windowsystem.cpp \
    lxdesupport.cpp \
    actionmanager.cpp \
    shortcutitem.cpp \
    shortcutdialog.cpp \
    skinnedfactory.cpp


QT += network
TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp

win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

CONFIG += release \
          warn_on \
          plugin


TARGET =$$PLUGINS_PREFIX/Ui/skinned

unix:LIBS += -lqmmp -lqmmpui
win32:LIBS += -lqmmp0 -lqmmpui0

RESOURCES = images/images.qrc stuff.qrc

unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Ui
INSTALLS += target
}

INCLUDEPATH += ../../../

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
    translations/qmmp_lt.ts \
    translations/qmmp_hu.ts \
    translations/qmmp_nl.ts \
    translations/qmmp_ja.ts \
    translations/qmmp_es.ts \
    translations/qmmp_sk.ts

    CONFIG += link_pkgconfig
    PKGCONFIG += x11
