# -------------------------------------------------
# Project created by QtCreator 2009-10-14T17:55:38
# -------------------------------------------------
include(../../plugins.pri)
INCLUDEPATH += ../../../../src
QT += dbus
CONFIG += release \
    warn_on \
    plugin
unix:TARGET = $$PLUGINS_PREFIX/General/kdenotify
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libkdenotify.so
TEMPLATE = lib
unix { 
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmpui \
        -lqmmp
}
unix { 
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
SOURCES += kdenotifyfactory.cpp \
    kdenotify.cpp \
    settingsdialog.cpp
HEADERS += kdenotifyfactory.h \
    kdenotify.h \
    settingsdialog.h
RESOURCES += translations/translations.qrc
FORMS += settingsdialog.ui

TRANSLATIONS = translations/kdenotify_plugin_cs.ts \
               translations/kdenotify_plugin_de.ts \
               translations/kdenotify_plugin_zh_CN.ts \
               translations/kdenotify_plugin_zh_TW.ts \
               translations/kdenotify_plugin_ru.ts \
               translations/kdenotify_plugin_pl.ts \
               translations/kdenotify_plugin_uk_UA.ts \
               translations/kdenotify_plugin_it.ts \
               translations/kdenotify_plugin_tr.ts \
               translations/kdenotify_plugin_lt.ts \
               translations/kdenotify_plugin_nl.ts \
               translations/kdenotify_plugin_ja.ts

images.files = app_icon.png \
               empty_cover.png
images.path = /share/qmmp/images/
INSTALLS += images
