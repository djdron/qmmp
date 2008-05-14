include(../../plugins.pri)

HEADERS += outputpulseaudiofactory.h \
           outputpulseaudio.h

SOURCES += outputpulseaudiofactory.cpp \
           outputpulseaudio.cpp


TARGET=$$PLUGINS_PREFIX/Output/pulseaudio
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libpulseaudio.so

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib

CONFIG += release \
warn_on \
thread \
plugin \
link_pkgconfig

TEMPLATE = lib
LIBS += -lqmmp

PKGCONFIG += libpulse-simple

TRANSLATIONS = translations/pulseaudio_plugin_cs.ts \
               translations/pulseaudio_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
