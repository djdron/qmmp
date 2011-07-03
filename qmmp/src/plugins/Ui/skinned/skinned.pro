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


TARGET = $$PLUGINS_PREFIX/Ui/skinned

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

RESOURCES += translations/translations.qrc
TRANSLATIONS = translations/skinned_plugin_ru.ts \
    translations/skinned_plugin_tr.ts \
    translations/skinned_plugin_zh_CN.ts \
    translations/skinned_plugin_cs.ts \
    translations/skinned_plugin_pt_BR.ts \
    translations/skinned_plugin_uk_UA.ts \
    translations/skinned_plugin_zh_TW.ts \
    translations/skinned_plugin_de.ts \
    translations/skinned_plugin_pl_PL.ts \
    translations/skinned_plugin_it.ts \
    translations/skinned_plugin_lt.ts \
    translations/skinned_plugin_hu.ts \
    translations/skinned_plugin_nl.ts \
    translations/skinned_plugin_ja.ts \
    translations/skinned_plugin_es.ts \
    translations/skinned_plugin_sk.ts

CONFIG += link_pkgconfig
PKGCONFIG += x11
