#include(../../plugins.pri)

HEADERS += decoderphononfactory.h \
           decoder_phonon.h

SOURCES += decoder_phonon.cpp \
           decoderphononfactory.cpp

#TARGET =$$PLUGINS_PREFIX/Input/phonon
#QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libphonon.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin

TEMPLATE = lib

QT += phonon

QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib

#TRANSLATIONS = translations/phonon_plugin_ru.ts
#               translations/phonon_plugin_uk_UA.ts
#               translations/phonon_plugin_zh_CN.ts
#               translations/phonon_plugin_zh_TW.ts
#               translations/phonon_plugin_cs.ts
#               translations/phonon_plugin_de.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target


