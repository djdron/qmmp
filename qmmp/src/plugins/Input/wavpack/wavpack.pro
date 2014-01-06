include(../../plugins.pri)
FORMS += 
HEADERS += decoderwavpackfactory.h \
    decoder_wavpack.h \
    cueparser.h \
    wavpackmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_wavpack.cpp \
    decoderwavpackfactory.cpp \
    cueparser.cpp \
    wavpackmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/wavpack

INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/wavpack_plugin_cs.ts \
    translations/wavpack_plugin_de.ts \
    translations/wavpack_plugin_zh_CN.ts \
    translations/wavpack_plugin_zh_TW.ts \
    translations/wavpack_plugin_ru.ts \
    translations/wavpack_plugin_pl.ts \
    translations/wavpack_plugin_uk_UA.ts \
    translations/wavpack_plugin_it.ts \
    translations/wavpack_plugin_tr.ts \
    translations/wavpack_plugin_lt.ts \
    translations/wavpack_plugin_nl.ts \
    translations/wavpack_plugin_ja.ts \
    translations/wavpack_plugin_es.ts \
    translations/wavpack_plugin_sr_BA.ts \
    translations/wavpack_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    PKGCONFIG += wavpack
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libwavpack.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lwavpack
}
