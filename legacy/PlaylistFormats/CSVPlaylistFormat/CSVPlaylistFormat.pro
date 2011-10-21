QMMPROOT = ../../../qmmp

HEADERS += csvplaylistformat.h
 
SOURCES += csvplaylistformat.cpp \
	$$QMMPROOT/src/mediafile.cpp

DESTDIR = ../
QMAKE_CLEAN = ../libcsvplaylistformat.so

INCLUDEPATH += $$QMMPROOT/src \
		$$QMMPROOT/lib

CONFIG += release warn_on plugin 

TEMPLATE = lib

#QMAKE_LIBDIR += ../../../
#LIBS += -lqmmp -L/usr/lib -I/usr/include

