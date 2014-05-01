include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Ui/qsui

CONFIG += warn_on \
    plugin \
    link_pkgconfig

TEMPLATE = lib

QT += network

SOURCES += \
    mainwindow.cpp \
    listwidget.cpp \
    visualmenu.cpp \
    positionslider.cpp \
    actionmanager.cpp \
    shortcutdialog.cpp \
    shortcutitem.cpp \
    popupsettings.cpp \
    popupwidget.cpp \
    equalizer.cpp \
    logo.cpp \
    keyboardmanager.cpp \
    aboutqsuidialog.cpp \
    qsuifactory.cpp \
    qsuisettings.cpp \
    qsuianalyzer.cpp \
    fft.c \
    colorwidget.cpp \
    qsuitabwidget.cpp \
    qsuitabbar.cpp \
    eqpreset.cpp \
    filesystembrowser.cpp \
    elidinglabel.cpp \
    coverwidget.cpp \
    playlistbrowser.cpp \
    toolbareditor.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    visualmenu.h \
    positionslider.h \
    actionmanager.h \
    shortcutdialog.h \
    shortcutitem.h \
    popupsettings.h \
    popupwidget.h \
    equalizer.h \
    logo.h \
    keyboardmanager.h \
    aboutqsuidialog.h \
    qsuifactory.h \
    qsuisettings.h \
    qsuianalyzer.h \
    fft.h \
    inlines.h \
    colorwidget.h \
    qsuitabwidget.h \
    qsuitabbar.h \
    eqpreset.h \
    filesystembrowser.h \
    elidinglabel.h \
    coverwidget.h \
    playlistbrowser.h \
    toolbareditor.h
TEMPLATE = lib

FORMS += forms/mainwindow.ui \
    forms/shortcutdialog.ui \
    forms/popupsettings.ui \
    forms/aboutqsuidialog.ui \
    forms/qsuisettings.ui \
    forms/toolbareditor.ui
RESOURCES += translations/translations.qrc resources/qsui_resources.qrc txt/qsui_txt.qrc

TRANSLATIONS = translations/qsui_plugin_ru.ts \
    translations/qsui_plugin_tr.ts \
    translations/qsui_plugin_zh_CN.ts \
    translations/qsui_plugin_cs.ts \
    translations/qsui_plugin_pt_BR.ts \
    translations/qsui_plugin_uk_UA.ts \
    translations/qsui_plugin_zh_TW.ts \
    translations/qsui_plugin_de.ts \
    translations/qsui_plugin_pl_PL.ts \
    translations/qsui_plugin_it.ts \
    translations/qsui_plugin_lt.ts \
    translations/qsui_plugin_hu.ts \
    translations/qsui_plugin_nl.ts \
    translations/qsui_plugin_ja.ts \
    translations/qsui_plugin_es.ts \
    translations/qsui_plugin_sr_BA.ts \
    translations/qsui_plugin_sr_RS.ts \
    translations/qsui_plugin_sk.ts


contains(CONFIG, SVN_VERSION){
    DEFINES += QMMP_PLUGIN_PACK_VERSION=\\\"$$QMMP_PLUGIN_PACK_VERSION-dev\\\"
}
else{
    DEFINES += QMMP_PLUGIN_PACK_VERSION=\\\"$$QMMP_PLUGIN_PACK_VERSION\\\"
}

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Ui
    INSTALLS += target

    PKGCONFIG += qmmp qmmpui
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Ui/libqsui.so
}

win32 {
    #HEADERS += ../../../../src/qmmp/metadatamodel.h \
    #           ../../../../src/qmmp/decoderfactory.h
    INCLUDEPATH += ../../ ./
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lqmmpui0
}













