include(../../plugins.pri)

HEADERS += decodersidfactory.h \
    decoder_sid.h \
    sidhelper.h
SOURCES += decoder_sid.cpp \
    decodersidfactory.cpp \
    sidhelper.cpp
TARGET = $$PLUGINS_PREFIX/Input/sid
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libsid.so
INCLUDEPATH += ../../../
CONFIG += warn_on \
          plugin \
          link_pkgconfig
TEMPLATE = lib

TRANSLATIONS = translations/sid_plugin_it.ts \
               translations/sid_plugin_ru.ts \
               translations/sid_plugin_cs.ts \
               translations/sid_plugin_de.ts \
               translations/sid_plugin_zh_CN.ts \
               translations/sid_plugin_zh_TW.ts \
               translations/sid_plugin_uk_UA.ts \
               translations/sid_plugin_pl.ts \
               translations/sid_plugin_tr.ts \
               translations/sid_plugin_lt.ts \
               translations/sid_plugin_nl.ts \
               translations/sid_plugin_ja.ts

#RESOURCES = translations/translations.qrc

unix{
    isEmpty (LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    PKGCONFIG += libsidplayfp
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lsidplayfp.dll
}
