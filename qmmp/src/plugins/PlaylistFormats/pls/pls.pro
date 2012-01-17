include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/PlaylistFormats/plsplaylistformat


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

INCLUDEPATH += ../../../../src

SOURCES += plsplaylistformat.cpp

HEADERS += plsplaylistformat.h

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/PlaylistFormats
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/PlaylistFormats/plsplaylistformat.so
    LIBS += -lqmmpui
}

win32 {
    LIBS += -lqmmpui0
    QMAKE_LIBDIR += ../../../../bin
}

