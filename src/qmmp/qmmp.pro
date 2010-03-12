unix:include(../../qmmp.pri)
win32:include(../../qmmp.pri)
HEADERS += recycler.h \
    buffer.h \
    decoder.h \
    output.h \
    outputfactory.h \
    equ/iir_cfs.h \
    equ/iir_fpu.h \
    equ/iir.h \
    decoderfactory.h \
    soundcore.h \
    visual.h \
    visualfactory.h \
    effect.h \
    effectfactory.h \
    statehandler.h \
    qmmp.h \
    fileinfo.h \
    volumecontrol.h \
    metadatamodel.h \
    tagmodel.h \
    abstractengine.h \
    qmmpaudioengine.h \
    audioparameters.h \
    inputsource.h \
    fileinputsource.h \
    emptyinputsource.h \
    inputsourcefactory.h \
    enginefactory.h \
    metadatamanager.h \
    replaygain.h \
    audioconverter.h \
    qmmpsettings.h
SOURCES += recycler.cpp \
    decoder.cpp \
    output.cpp \
    equ/iir.c \
    equ/iir_cfs.c \
    equ/iir_fpu.c \
    soundcore.cpp \
    visual.cpp \
    effect.cpp \
    statehandler.cpp \
    qmmp.cpp \
    fileinfo.cpp \
    volumecontrol.cpp \
    metadatamodel.cpp \
    tagmodel.cpp \
    abstractengine.cpp \
    qmmpaudioengine.cpp \
    audioparameters.cpp \
    inputsource.cpp \
    fileinputsource.cpp \
    emptyinputsource.cpp \
    metadatamanager.cpp \
    replaygain.cpp \
    audioconverter.cpp \
    qmmpsettings.cpp
FORMS += 
unix:TARGET = ../../lib/qmmp
win32:TARGET = ../../../bin/qmmp
CONFIG += release \
    shared \
    warn_on \
    qt \
    thread
TEMPLATE = lib
VERSION = $$QMMP_VERSION
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
                   buffer.h \
                   decoder.h \
                   output.h \
                   outputfactory.h \
                   decoderfactory.h \
                   soundcore.h \
                   visual.h \
                   visualfactory.h \
                   effect.h \
                   effectfactory.h \
                   statehandler.h \
                   qmmp.h \
                   fileinfo.h \
                   volumecontrol.h \
                   metadatamodel.h \
                   tagmodel.h \
                   abstractengine.h \
                   audioparameters.h \
                   inputsource.h \
                   inputsourcefactory.h \
                   enginefactory.h \
                   metadatamanager.h \
                   qmmpsettings.h
    devel.path = /include/qmmp
    INSTALLS += target \
        devel
    DESTDIR = .
}
INCLUDEPATH += ./
