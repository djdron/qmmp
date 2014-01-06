include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodermodplugfactory.h \
    decoder_modplug.h \
    settingsdialog.h \
    archivereader.h \
    modplugmetadatamodel.h
SOURCES += decoder_modplug.cpp \
    decodermodplugfactory.cpp \
    settingsdialog.cpp \
    archivereader.cpp \
    modplugmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/modplug

DEFINES += HAVE_STDINT_H \
    HAVE_INTTYPES_H
INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/modplug_plugin_cs.ts \
    translations/modplug_plugin_de.ts \
    translations/modplug_plugin_zh_CN.ts \
    translations/modplug_plugin_zh_TW.ts \
    translations/modplug_plugin_ru.ts \
    translations/modplug_plugin_pl.ts \
    translations/modplug_plugin_uk_UA.ts \
    translations/modplug_plugin_it.ts \
    translations/modplug_plugin_tr.ts \
    translations/modplug_plugin_lt.ts \
    translations/modplug_plugin_nl.ts \
    translations/modplug_plugin_ja.ts \
    translations/modplug_plugin_es.ts \
    translations/modplug_plugin_sr_BA.ts \
    translations/modplug_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    PKGCONFIG += libmodplug
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmodplug.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lmodplug
    DEFINES -= UNICODE
}
