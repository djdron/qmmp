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
           visualization.h \
           streamreader.h \
           downloader.h
SOURCES += recycler.cpp \
           decoder.cpp \
           output.cpp \
           equ\iir.c \
           equ\iir_cfs.c \
           equ\iir_fpu.c \
           soundcore.cpp \
           streamreader.cpp \
           downloader.cpp \
 filetag.cpp

TARGET = qmmp
CONFIG += release \
warn_on \
qt \
thread \
link_pkgconfig

TEMPLATE = lib
PKGCONFIG += libcurl
target.path = /lib
INSTALLS += target
