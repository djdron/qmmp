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
QMAKE_CLEAN = ../libffmpeg.so
TARGET = $$PLUGINS_PREFIX/Input/ffmpeg
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libffmpeg.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp

DEFINES += __STDC_CONSTANT_MACROS
PKGCONFIG += libavcodec libavformat libavutil
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
    translations/ffmpeg_plugin_es.ts

RESOURCES = translations/translations.qrc
isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
