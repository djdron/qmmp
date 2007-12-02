HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h 
SOURCES += decoder_sndfile.cpp \
	   decodersndfilefactory.cpp
DESTDIR = ../
QMAKE_CLEAN = ../libsndfile.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include

PKGCONFIG += sndfile
#TRANSLATIONS = translations/ffmpeg_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
