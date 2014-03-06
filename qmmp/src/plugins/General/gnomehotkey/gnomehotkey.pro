include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/gnomehotkey
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libgnomehotkey.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/gnomehotkey_plugin_cs.ts \
               translations/gnomehotkey_plugin_de.ts \
               translations/gnomehotkey_plugin_zh_CN.ts \
               translations/gnomehotkey_plugin_zh_TW.ts \
               translations/gnomehotkey_plugin_ru.ts \
               translations/gnomehotkey_plugin_pl.ts \
               translations/gnomehotkey_plugin_uk_UA.ts \
               translations/gnomehotkey_plugin_it.ts \
               translations/gnomehotkey_plugin_tr.ts \
               translations/gnomehotkey_plugin_lt.ts \
               translations/gnomehotkey_plugin_nl.ts \
               translations/gnomehotkey_plugin_ja.ts \
               translations/gnomehotkey_plugin_es.ts \
               translations/gnomehotkey_plugin_sr_BA.ts \
               translations/gnomehotkey_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += gnomehotkeyfactory.h \
    mediakeys.h

SOURCES += gnomehotkeyfactory.cpp \
    mediakeys.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp
