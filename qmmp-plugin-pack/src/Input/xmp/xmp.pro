include(../../plugins.pri)

HEADERS += decoderxmpfactory.h \
    decoder_xmp.h \
    xmpmetadatamodel.h \
    settingsdialog.h
SOURCES += decoder_xmp.cpp \
    decoderxmpfactory.cpp \
    xmpmetadatamodel.cpp \
    settingsdialog.cpp
TARGET = $$PLUGINS_PREFIX/Input/xmp


INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

RESOURCES = translations/translations.qrc

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    PKGCONFIG += libxmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libxmp.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lxmp
}

FORMS += \
    settingsdialog.ui
