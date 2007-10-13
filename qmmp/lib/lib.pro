# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./libs
# ???? - ??????????:  nnp

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
 visual.h
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
 visual.cpp

TARGET = qmmp
CONFIG += release \
warn_on \
qt \
thread \
link_pkgconfig

TEMPLATE = lib
PKGCONFIG += libcurl

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}

unix {
    LINE1 = $$sprintf(echo \"%1ifndef CONFIG_H\" > ./config.h, $$LITERAL_HASH)
    LINE2 = $$sprintf(echo \"%1define CONFIG_H\" >> ./config.h, $$LITERAL_HASH)
    LINE3 = $$sprintf(echo \"%1define LIB_DIR \\\"%2\\\"\" >> ./config.h, $$LITERAL_HASH, $$LIB_DIR)
    LINE4 = $$sprintf(echo \"%1endif\" >> ./config.h, $$LITERAL_HASH)
    system($$LINE1)
    system($$LINE2)
    system($$LINE3)
    system($$LINE4)
    QMAKE_CLEAN += ./config.h
}

target.path = $$LIB_DIR
INSTALLS += target
