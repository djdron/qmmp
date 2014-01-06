include(../../plugins.pri)

HEADERS += decodergmefactory.h \
    decoder_gme.h \
    gmehelper.h
SOURCES += decoder_gme.cpp \
    decodergmefactory.cpp \
    gmehelper.cpp
TARGET = $$PLUGINS_PREFIX/Input/gme
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libgme.so
INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin
TEMPLATE = lib

TRANSLATIONS = translations/gme_plugin_it.ts \
               translations/gme_plugin_ru.ts \
               translations/gme_plugin_cs.ts \
               translations/gme_plugin_de.ts \
               translations/gme_plugin_zh_CN.ts \
               translations/gme_plugin_zh_TW.ts \
               translations/gme_plugin_uk_UA.ts \
               translations/gme_plugin_pl.ts \
               translations/gme_plugin_tr.ts \
               translations/gme_plugin_lt.ts \
               translations/gme_plugin_nl.ts \
               translations/gme_plugin_ja.ts \
               translations/gme_plugin_sr_BA.ts \
               translations/gme_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix{
    isEmpty (LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp \
      -L/usr/lib \
      -L/usr/local/lib \
      -I/usr/include \
      -I/usr/local/include \
      -lgme
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lgme.dll
}
