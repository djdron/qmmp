include(../../../../../qmmp.pri)

QMMPSRCROOT = ../../../../

INCLUDEPATH += $$QMMPSRCROOT/ui \
		$$QMMPSRCROOT/qmmp \
		$$QMMPSRCROOT

HEADERS += incdecvolumeoption.h \
	    $$QMMPSRCROOT/ui/mainwindow.h \
	    $$QMMPSRCROOT/qmmp/soundcore.h
 
SOURCES += incdecvolumeoption.cpp \
	    $$QMMPSRCROOT/qmmp/soundcore.cpp \
	    $$QMMPSRCROOT/ui/mainwindow.cpp

DESTDIR = ../
QMAKE_CLEAN = ../libincdecvolumeoption.so



CONFIG += release warn_on plugin

TEMPLATE = lib


