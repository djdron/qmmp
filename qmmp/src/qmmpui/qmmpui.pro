include(../../qmmp.pri)

TARGET = ../../lib/qmmpui
CONFIG += release \
warn_on \
qt \
thread

TEMPLATE = lib


unix:isEmpty(LIB_DIR){
    LIB_DIR = /lib
}

unix:DEFINES += LIB_DIR=\\\"$$LIB_DIR\\\"


target.path = $$LIB_DIR
INSTALLS += target
HEADERS += general.h \
           generalfactory.h \
           generalhandler.h \
           songinfo.h \
           control.h
SOURCES += general.cpp \
           generalhandler.cpp \
           songinfo.cpp \
           control.cpp

DESTDIR = .

