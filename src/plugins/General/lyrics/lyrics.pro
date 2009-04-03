include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin 

TARGET =$$PLUGINS_PREFIX/General/lyrics
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/liblyrics.so


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmpui -lqmmp

TRANSLATIONS = translations/lyrics_plugin_cs.ts \
               translations/lyrics_plugin_de.ts \
               translations/lyrics_plugin_zh_CN.ts \
               translations/lyrics_plugin_zh_TW.ts \
               translations/lyrics_plugin_ru.ts \
               translations/lyrics_plugin_pl.ts \
               translations/lyrics_plugin_uk_UA.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target

HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h
SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp

FORMS += lyricswindow.ui

QT += network

