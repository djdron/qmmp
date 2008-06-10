include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/PlaylistFormats/xspfplaylistformat
QMAKE_CLEAN =$$PLUGINS_PREFIX/PlaylistFormats/libxsplaylistformat.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/PlaylistFormats
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

QT += xml
SOURCES += xspfplaylistformat.cpp

HEADERS += xspfplaylistformat.h

