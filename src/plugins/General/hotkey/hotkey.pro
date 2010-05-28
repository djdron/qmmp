include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib  \
 link_pkgconfig

PKGCONFIG += x11

TARGET = $$PLUGINS_PREFIX/General/hotkey
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhotkey.so

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
               translations/hotkey_plugin_nl.ts 
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target

HEADERS += hotkeyfactory.h \
           hotkeymanager.h \
           settingsdialog.h \
           hotkeydialog.h

SOURCES += hotkeyfactory.cpp \
           hotkeymanager.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

FORMS += settingsdialog.ui \
         hotkeydialog.ui
