include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/PlaylistFormats/m3uplaylistformat
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libm3uplaylistformat.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/PlaylistFormats
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

HEADERS += m3uplaylistformat.h

SOURCES += m3uplaylistformat.cpp

