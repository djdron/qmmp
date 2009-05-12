include(../../plugins.pri)

HEADERS += outputjackfactory.h \
           outputjack.h \
           bio2jack.h

SOURCES += outputjackfactory.cpp \
           outputjack.cpp \
           bio2jack.c

TARGET=$$PLUGINS_PREFIX/Output/jack
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libjack.so


INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib
CONFIG += release \
warn_on \
thread \
plugin \
link_pkgconfig
TEMPLATE = lib
LIBS += -lqmmp
PKGCONFIG += jack samplerate
TRANSLATIONS = translations/jack_plugin_ru.ts \
               translations/jack_plugin_uk_UA.ts \
               translations/jack_plugin_zh_CN.ts \
               translations/jack_plugin_zh_TW.ts \
               translations/jack_plugin_cs.ts \
               translations/jack_plugin_pl.ts \
               translations/jack_plugin_de.ts \
               translations/jack_plugin_it.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
