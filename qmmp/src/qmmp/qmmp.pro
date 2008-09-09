include(../../qmmp.pri)

HEADERS += recycler.h \
           buffer.h \
           constants.h \
           decoder.h \
           output.h \
           filetag.h \
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
 qmmp.h
SOURCES += recycler.cpp \
           decoder.cpp \
           output.cpp \
           equ\iir.c \
           equ\iir_cfs.c \
           equ\iir_fpu.c \
           soundcore.cpp \
           streamreader.cpp \
           downloader.cpp \
           filetag.cpp \
           visual.cpp \
           effect.cpp \
 statehandler.cpp \
 qmmp.cpp

TARGET = ../../lib/qmmp
CONFIG += release \
warn_on \
qt \
thread \
link_pkgconfig

TEMPLATE = lib
VERSION = $$QMMP_VERSION
PKGCONFIG += libcurl

unix : isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }

unix : DEFINES += LIB_DIR=\\\"$$LIB_DIR\\\"
DEFINES += QMMP_VERSION=$$QMMP_VERSION
contains(CONFIG, SVN_VERSION){
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION-svn\\\"
}else {
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION\\\"
}

target.path = $$LIB_DIR

devel.files += recycler.h \
               buffer.h \
               constants.h \
               decoder.h \
               output.h \
               filetag.h \
               outputfactory.h \
               decoderfactory.h \
               soundcore.h \
               streamreader.h \
               downloader.h \
               visual.h \
               visualfactory.h \
               effect.h \
               effectfactory.h

devel.path = /include/qmmp

INSTALLS += target devel
DESTDIR = .
