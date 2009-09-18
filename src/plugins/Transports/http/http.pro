include(../../plugins.pri)
HEADERS += downloader.h \
    streamreader.h \
    httpinputfactory.h \
    httpinputsource.h
SOURCES += downloader.cpp \
    streamreader.cpp \
    httpinputfactory.cpp \
    httpinputsource.cpp
win32:HEADERS += ../../../../src/qmmp/inputsource.h \
    ../../../../src/qmmp/inputsourcefactory.h
TARGET = $$PLUGINS_PREFIX/Transports/http
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libhttp.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
unix { 
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp \
        -L/usr/lib
    PKGCONFIG += libcurl
    isEmpty(LIB_DIR):LIB_DIR = /lib
    win32 { 
        QMAKE_LIBDIR += ../../../../bin
        LIBS += -lqmmp0 \
            -lcurldll
    }
    target.path = $$LIB_DIR/qmmp/Transports
    INSTALLS += target
}
