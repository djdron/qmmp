include(../../plugins.pri)

FORMS += detailsdialog.ui 
HEADERS += decoderwildmidifactory.h \
           decoder_wildmidi.h \
           detailsdialog.h
SOURCES += decoder_wildmidi.cpp \
           decoderwildmidifactory.cpp \
           detailsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Input/wildmidi
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libwildmidi.so


INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include -lWildMidi

TRANSLATIONS = translations/wildmidi_plugin_cs.ts \
               translations/wildmidi_plugin_de.ts \
               translations/wildmidi_plugin_zh_CN.ts \
               translations/wildmidi_plugin_zh_TW.ts \
               translations/wildmidi_plugin_ru.ts \
               translations/wildmidi_plugin_uk_UA.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
