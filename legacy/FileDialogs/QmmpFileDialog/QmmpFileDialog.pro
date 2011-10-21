QMMPROOT = ../../../qmmp

INCLUDEPATH += $$QMMPROOT/src
INCLUDEPATH += $$QMMPROOT/lib

HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h \
	   $$QMMPROOT/src/filedialog.h \
	   $$QMMPROOT/src/playlistmodel.h
	   
SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp \
	   $$QMMPROOT/src/filedialog.cpp \
	   $$QMMPROOT/src/playlistmodel.cpp
	   

FORMS += qmmpfiledialog.ui

RESOURCES += images/images.qrc
	   
DESTDIR = ../
QMAKE_CLEAN += ../libqmmpfiledialog.so


CONFIG += release warn_on plugin 

TEMPLATE = lib

