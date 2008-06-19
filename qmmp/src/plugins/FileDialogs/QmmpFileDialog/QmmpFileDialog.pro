include(../../plugins.pri)

INCLUDEPATH += ../../../../src

TARGET =$$PLUGINS_PREFIX/FileDialogs/qmmpfiledialog
QMAKE_CLEAN =$$PLUGINS_PREFIX/FileDialogs/libqmmpfiledialog.so

HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h

SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp


FORMS += qmmpfiledialog.ui


QMAKE_CLEAN += ../libqmmpfiledialog.so


CONFIG += release warn_on plugin

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/FileDialogs
INSTALLS += target
