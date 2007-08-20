FORMS += detailsdialog.ui 
HEADERS += decoderffmpegfactory.h \
           detailsdialog.h \
           decoder_ffmpeg.h
SOURCES += detailsdialog.cpp \
           decoder_ffmpeg.cpp \
	   decoderffmpegfactory.cpp
DESTDIR = ../
QMAKE_CLEAN = ../libffmpeg.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include
DEFINES += __STDC_CONSTANT_MACROS
PKGCONFIG += libavcodec libavformat
#TRANSLATIONS = translations/ffmpeg_plugin_ru.ts
#RESOURCES = translations/translations.qrc
target.path = /lib/qmmp/Input
INSTALLS += target
