include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libseekoption.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/seek_plugin_cs.ts \
               translations/seek_plugin_de.ts \
               translations/seek_plugin_pl.ts \
               translations/seek_plugin_ru.ts \
               translations/seek_plugin_uk_UA.ts \
               translations/seek_plugin_zh_TW.ts \
               translations/seek_plugin_zh_CN.ts \
               translations/seek_plugin_it.ts \
               translations/seek_plugin_tr.ts \
               translations/seek_plugin_lt.ts \
               translations/seek_plugin_nl.ts \
               translations/seek_plugin_ja.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

HEADERS += seekoption.h

SOURCES += seekoption.cpp
