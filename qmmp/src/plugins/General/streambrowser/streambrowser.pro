include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/streambrowser
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libstreambrowser.so


TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp

win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/streambrowser_plugin_cs.ts \
               translations/streambrowser_plugin_de.ts \
               translations/streambrowser_plugin_zh_CN.ts \
               translations/streambrowser_plugin_zh_TW.ts \
               translations/streambrowser_plugin_ru.ts \
               translations/streambrowser_plugin_pl.ts \
               translations/streambrowser_plugin_uk_UA.ts \
               translations/streambrowser_plugin_it.ts \
               translations/streambrowser_plugin_tr.ts \
               translations/streambrowser_plugin_lt.ts \
               translations/streambrowser_plugin_nl.ts \
               translations/streambrowser_plugin_ja.ts \
               translations/streambrowser_plugin_es.ts \
               translations/streambrowser_plugin_sr_BA.ts \
               translations/streambrowser_plugin_sr_RS.ts
RESOURCES = translations/translations.qrc
unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += streambrowserfactory.h \
           streambrowser.h \
           streamwindow.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += streambrowserfactory.cpp \
           streambrowser.cpp \
           streamwindow.cpp

FORMS += streamwindow.ui

QT += network

