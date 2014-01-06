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

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg_legacy

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libffmpeg_legacy.so
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

TRANSLATIONS = translations/ffmpeg_plugin_legacy_ru.ts \
    translations/ffmpeg_plugin_legacy_uk_UA.ts \
    translations/ffmpeg_plugin_legacy_zh_CN.ts \
    translations/ffmpeg_plugin_legacy_zh_TW.ts \
    translations/ffmpeg_plugin_legacy_cs.ts \
    translations/ffmpeg_plugin_legacy_pl.ts \
    translations/ffmpeg_plugin_legacy_de.ts \
    translations/ffmpeg_plugin_legacy_it.ts \
    translations/ffmpeg_plugin_legacy_tr.ts \
    translations/ffmpeg_plugin_legacy_lt.ts \
    translations/ffmpeg_plugin_legacy_nl.ts \
    translations/ffmpeg_plugin_legacy_ja.ts \
    translations/ffmpeg_plugin_legacy_es.ts \
    translations/ffmpeg_plugin_legacy_sr_BA.ts \
    translations/ffmpeg_plugin_legacy_sr_RS.ts

RESOURCES = translations/translations.qrc

