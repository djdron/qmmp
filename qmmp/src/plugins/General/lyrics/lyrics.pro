include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin 

TARGET =$$PLUGINS_PREFIX/General/lyrics
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/liblyrics.so


TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp

win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/lyrics_plugin_cs.ts \
               translations/lyrics_plugin_de.ts \
               translations/lyrics_plugin_zh_CN.ts \
               translations/lyrics_plugin_zh_TW.ts \
               translations/lyrics_plugin_ru.ts \
               translations/lyrics_plugin_pl.ts \
               translations/lyrics_plugin_uk_UA.ts \
               translations/lyrics_plugin_it.ts \
               translations/lyrics_plugin_tr.ts \
               translations/lyrics_plugin_lt.ts \
               translations/lyrics_plugin_nl.ts
RESOURCES = translations/translations.qrc
unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp

FORMS += lyricswindow.ui

QT += network

