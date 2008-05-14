include(../../plugins.pri)

FORMS += detailsdialog.ui \
         settingsdialog.ui
HEADERS += decodermodplugfactory.h \
           decoder_modplug.h \
           detailsdialog.h \
           settingsdialog.h \
           archivereader.h
SOURCES += decoder_modplug.cpp \
           decodermodplugfactory.cpp \
           detailsdialog.cpp \
           settingsdialog.cpp \
           archivereader.cpp

TARGET =$$PLUGINS_PREFIX/Input/modplug
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libmodplug.so


INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include
PKGCONFIG += libmodplug
TRANSLATIONS = translations/modplug_plugin_cs.ts \
               translations/modplug_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
