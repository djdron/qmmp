include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decoderffmpegfactory.h \
    decoder_ffmpeg.h \
    settingsdialog.h \
    ffmpegmetadatamodel.h
SOURCES += decoder_ffmpeg.cpp \
    decoderffmpegfactory.cpp \
    settingsdialog.cpp \
    ffmpegmetadatamodel.cpp

INCLUDEPATH += ../../../



CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libffmpeg.so
    LIBS += -lqmmp
    QMAKE_LIBDIR += ../../../../lib
    PKGCONFIG += libavcodec libavformat libavutil
}


win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lavcodec.dll -lavformat.dll -lavutil.dll
}

DEFINES += __STDC_CONSTANT_MACROS

TRANSLATIONS = translations/ffmpeg_plugin_ru.ts \
    translations/ffmpeg_plugin_uk_UA.ts \
    translations/ffmpeg_plugin_zh_CN.ts \
    translations/ffmpeg_plugin_zh_TW.ts \
    translations/ffmpeg_plugin_cs.ts \
    translations/ffmpeg_plugin_pl.ts \
    translations/ffmpeg_plugin_de.ts \
    translations/ffmpeg_plugin_it.ts \
    translations/ffmpeg_plugin_tr.ts \
    translations/ffmpeg_plugin_lt.ts \
    translations/ffmpeg_plugin_nl.ts \
    translations/ffmpeg_plugin_ja.ts \
    translations/ffmpeg_plugin_es.ts \
    translations/ffmpeg_plugin_sr_BA.ts \
    translations/ffmpeg_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

