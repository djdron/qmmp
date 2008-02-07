QMMPROOT = ../../../qmmp

INCLUDEPATH += $$QMMPROOT/src
INCLUDEPATH += $$QMMPROOT/lib

HEADERS += qt3filedialogfactory.h \
           qt3filedialog.h \
	   $$QMMPROOT/src/filedialog.h \
	   $$QMMPROOT/src/playlistmodel.h
	   
SOURCES += qt3filedialog.cpp \
           qt3filedialogfactory.cpp \
	   $$QMMPROOT/src/filedialog.cpp \
	   $$QMMPROOT/src/playlistmodel.cpp
	   
	   
DESTDIR = ../
QMAKE_CLEAN += ../libqt3filedialog.so


CONFIG += release warn_on plugin 

TEMPLATE = lib

QT += qt3support

#target.path = /plugins/FileDialogs
#INSTALLS += target
