include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodermpg123factory.h \
    decoder_mpg123.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_mpg123.cpp \
    decodermpg123factory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/mpg123
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmpg123.so
INCLUDEPATH += /home/user/qmmp-0.6/include

win32:INCLUDEPATH += D:\qt4\MINGW\include\taglib
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

unix:PKGCONFIG += taglib libmpg123 qmmp

TRANSLATIONS = translations/mpg123_plugin_ru.ts \
    translations/mpg123_plugin_uk_UA.ts \
    translations/mpg123_plugin_zh_CN.ts \
    translations/mpg123_plugin_zh_TW.ts \
    translations/mpg123_plugin_cs.ts \
    translations/mpg123_plugin_pl.ts \
    translations/mpg123_plugin_de.ts \
    translations/mpg123_plugin_it.ts \
    translations/mpg123_plugin_tr.ts \
    translations/mpg123_plugin_tr.ts \
    translations/mpg123_plugin_lt.ts \
    translations/mpg123_plugin_nl.ts \
    translations/mpg123_plugin_ja.ts \
    translations/mpg123_plugin_es.ts

#RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
win32:HEADERS += ../../../../src/qmmp/metadatamodel.h \
    ../../../../src/qmmp/decoderfactory.h
