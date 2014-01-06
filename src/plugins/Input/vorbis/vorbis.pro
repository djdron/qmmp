include(../../plugins.pri)
FORMS += 
HEADERS += decodervorbisfactory.h \
    decoder_vorbis.h \
    vorbismetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_vorbis.cpp \
    decodervorbisfactory.cpp \
    vorbismetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/vorbis

INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/vorbis_plugin_ru.ts \
    translations/vorbis_plugin_uk_UA.ts \
    translations/vorbis_plugin_zh_CN.ts \
    translations/vorbis_plugin_zh_TW.ts \
    translations/vorbis_plugin_cs.ts \
    translations/vorbis_plugin_pl.ts \
    translations/vorbis_plugin_de.ts \
    translations/vorbis_plugin_it.ts \
    translations/vorbis_plugin_tr.ts \
    translations/vorbis_plugin_lt.ts \
    translations/vorbis_plugin_nl.ts \
    translations/vorbis_plugin_ja.ts \
    translations/vorbis_plugin_es.ts \
    translations/vorbis_plugin_sr_BA.ts \
    translations/vorbis_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix {
    isEmpty (LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += taglib ogg vorbisfile vorbis
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libvorbis.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lvorbisfile -lvorbis -logg -ltag.dll -lm
    LD_FLAGS += -no-undefined
}
