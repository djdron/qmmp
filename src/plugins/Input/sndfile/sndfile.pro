include(../../plugins.pri)

HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h 
SOURCES += decoder_sndfile.cpp \
           decodersndfilefactory.cpp

TARGET=$$PLUGINS_PREFIX/Input/sndfile


INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin \
link_pkgconfig

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/sndfile_plugin_cs.ts \
               translations/sndfile_plugin_de.ts \
               translations/sndfile_plugin_zh_CN.ts \
               translations/sndfile_plugin_zh_TW.ts \
               translations/sndfile_plugin_ru.ts \
               translations/sndfile_plugin_pl.ts \
               translations/sndfile_plugin_uk_UA.ts \
               translations/sndfile_plugin_it.ts \
               translations/sndfile_plugin_tr.ts \
               translations/sndfile_plugin_lt.ts \
               translations/sndfile_plugin_nl.ts \
               translations/sndfile_plugin_ja.ts \
               translations/sndfile_plugin_es.ts \
               translations/sndfile_plugin_sr_BA.ts \
               translations/sndfile_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    PKGCONFIG += sndfile
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libsndfile.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lsndfile -lflac -lvorbisenc -lvorbis -logg
}
