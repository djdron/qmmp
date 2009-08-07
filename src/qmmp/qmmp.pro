unix:include(../../qmmp.pri)
win32:include(../../qmmp.pri)
HEADERS += recycler.h \
    buffer.h \
    decoder.h \
    output.h \
    outputfactory.h \
    equ\iir_cfs.h \
    equ\iir_fpu.h \
    equ\iir.h \
    decoderfactory.h \
    soundcore.h \
    streamreader.h \
    downloader.h \
    visual.h \
    visualfactory.h \
    effect.h \
    effectfactory.h \
    statehandler.h \
    qmmp.h \
    fileinfo.h \
    volumecontrol.h \
    coverwidget.h \
    abstractdetailsdialog.h
SOURCES += recycler.cpp \
    decoder.cpp \
    output.cpp \
    equ\iir.c \
    equ\iir_cfs.c \
    equ\iir_fpu.c \
    soundcore.cpp \
    streamreader.cpp \
    downloader.cpp \
    visual.cpp \
    effect.cpp \
    statehandler.cpp \
    qmmp.cpp \
    fileinfo.cpp \
    volumecontrol.cpp \
    coverwidget.cpp \
    abstractdetailsdialog.cpp
FORMS += forms/abstractdetailsdialog.ui
unix:TARGET = ../../lib/qmmp
win32:TARGET = ../../../bin/qmmp
CONFIG += release \
    shared \
    warn_on \
    qt \
    thread \
    link_pkgconfig
TEMPLATE = lib
VERSION = $$QMMP_VERSION
unix:PKGCONFIG += libcurl
win32:LIBS += -lcurldll
unix:isEmpty(LIB_DIR):LIB_DIR = /lib
unix:DEFINES += LIB_DIR=\\\"$$LIB_DIR\\\"
DEFINES += QMMP_VERSION=$$QMMP_VERSION
DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION\\\"
contains(CONFIG, SVN_VERSION) { 
    unix:DEFINES += SVN_REVISION=\\\"$$system(./svn_revision.sh)\\\"
    win32:DEFINES += SVN_REVISION=\\\"svn\\\"
}
unix { 
    target.path = $$LIB_DIR
    devel.files += buffer.h \
        decoderfactory.h \
        decoder.h \
        effectfactory.h \
        effect.h \
        fileinfo.h \
        outputfactory.h \
        output.h \
        qmmp.h \
        recycler.h \
        soundcore.h \
        statehandler.h \
        visualfactory.h \
        visual.h \
        volumecontrol.h
    devel.path = /include/qmmp
    INSTALLS += target \
        devel
    DESTDIR = .
}
