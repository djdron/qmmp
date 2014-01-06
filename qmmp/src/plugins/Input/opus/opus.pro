include(../../plugins.pri)
FORMS +=
HEADERS += decoderopusfactory.h \
    decoder_opus.h \
    opusmetadatamodel.h \
    replaygainreader.h \
    opusproperties.h \
    opusfile.h \
    tdebug.h
SOURCES += decoder_opus.cpp \
    decoderopusfactory.cpp \
    opusmetadatamodel.cpp \
    replaygainreader.cpp \
    opusproperties.cpp \
    opusfile.cpp \
    tdebug.cpp
TARGET = $$PLUGINS_PREFIX/Input/opus

INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/opus_plugin_ru.ts \
    translations/opus_plugin_uk_UA.ts \
    translations/opus_plugin_zh_CN.ts \
    translations/opus_plugin_zh_TW.ts \
    translations/opus_plugin_cs.ts \
    translations/opus_plugin_pl.ts \
    translations/opus_plugin_de.ts \
    translations/opus_plugin_it.ts \
    translations/opus_plugin_tr.ts \
    translations/opus_plugin_lt.ts \
    translations/opus_plugin_nl.ts \
    translations/opus_plugin_ja.ts \
    translations/opus_plugin_es.ts \
    translations/opus_plugin_sr_BA.ts \
    translations/opus_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc


unix {
    isEmpty (LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += taglib opus opusfile
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libopus.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lopusfile -lopus -ltag.dll -lm
}
