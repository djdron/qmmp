include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/playlistoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libplaylistoption.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/playlist_plugin_cs.ts \
               translations/playlist_plugin_de.ts \
               translations/playlist_plugin_pl.ts \
               translations/playlist_plugin_ru.ts \
               translations/playlist_plugin_uk_UA.ts \
               translations/playlist_plugin_zh_TW.ts \
               translations/playlist_plugin_zh_CN.ts \
               translations/playlist_plugin_it.ts \
               translations/playlist_plugin_tr.ts \
               translations/playlist_plugin_lt.ts \
               translations/playlist_plugin_nl.ts \
               translations/playlist_plugin_ja.ts \
               translations/playlist_plugin_es.ts

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp
