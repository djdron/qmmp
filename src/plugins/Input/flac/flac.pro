include(../../plugins.pri)
FORMS += 
HEADERS += decoderflacfactory.h \
    decoder_flac.h \
    cueparser.h \
    flacmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_flac.cpp \
    decoderflacfactory.cpp \
    cueparser.cpp \
    flacmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/flac

INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/flac_plugin_ru.ts \
    translations/flac_plugin_uk_UA.ts \
    translations/flac_plugin_zh_CN.ts \
    translations/flac_plugin_zh_TW.ts \
    translations/flac_plugin_cs.ts \
    translations/flac_plugin_pl.ts \
    translations/flac_plugin_de.ts \
    translations/flac_plugin_it.ts \
    translations/flac_plugin_tr.ts \
    translations/flac_plugin_lt.ts \
    translations/flac_plugin_nl.ts \
    translations/flac_plugin_ja.ts \
    translations/flac_plugin_es.ts \
    translations/flac_plugin_sr_BA.ts \
    translations/flac_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib flac
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libflac.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lflac -logg -ltag.dll -lm
}
