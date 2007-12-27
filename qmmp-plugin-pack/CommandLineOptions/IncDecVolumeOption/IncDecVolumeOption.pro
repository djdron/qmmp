QMMPROOT = ../../../qmmp


INCLUDEPATH += $$QMMPROOT/src \
		$$QMMPROOT/lib

HEADERS += incdecvolumeoption.h \
	    $$QMMPROOT/src/mainwindow.h \
	    $$QMMPROOT/lib/soundcore.h
 
SOURCES += incdecvolumeoption.cpp \
	    $$QMMPROOT/lib/soundcore.cpp \
	    $$QMMPROOT/src/mainwindow.cpp

DESTDIR = ../
QMAKE_CLEAN = ../libincdecvolumeoption.so



CONFIG += release warn_on plugin

TEMPLATE = lib


