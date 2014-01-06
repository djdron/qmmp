include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib  \
 link_pkgconfig


TARGET = $$PLUGINS_PREFIX/General/hotkey


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/hotkey_plugin_cs.ts \
               translations/hotkey_plugin_de.ts \
               translations/hotkey_plugin_zh_CN.ts \
               translations/hotkey_plugin_zh_TW.ts \
               translations/hotkey_plugin_ru.ts \
               translations/hotkey_plugin_pl.ts \
               translations/hotkey_plugin_uk_UA.ts \
               translations/hotkey_plugin_it.ts \
               translations/hotkey_plugin_tr.ts \
               translations/hotkey_plugin_lt.ts \
               translations/hotkey_plugin_nl.ts \
               translations/hotkey_plugin_ja.ts \
               translations/hotkey_plugin_es.ts \
               translations/hotkey_plugin_sr_BA.ts \
               translations/hotkey_plugin_sr_RS.ts
RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General

HEADERS += hotkeyfactory.h \
           hotkeymanager.h \
           settingsdialog.h \
           hotkeydialog.h

SOURCES += hotkeyfactory.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp \
           hotkeymanager_x11.cpp \
           hotkeymanager_win.cpp

FORMS += settingsdialog.ui \
         hotkeydialog.ui


INCLUDEPATH += ../../../../src

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
    QMAKE_LIBDIR += ../../../../lib
    QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhotkey.so
    PKGCONFIG += x11
    DEFINES += HAVE_XKBLIB_H
    LIBS += -lqmmpui -lqmmp
}

win32 {
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmpui0 -lqmmp0
}

