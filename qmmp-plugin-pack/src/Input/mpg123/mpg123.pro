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

win32:INCLUDEPATH += D:\qt4\MINGW\include\taglib
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

unix:PKGCONFIG += taglib libmpg123 qmmp

RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
win32:HEADERS += ../../../../src/qmmp/metadatamodel.h \
    ../../../../src/qmmp/decoderfactory.h
