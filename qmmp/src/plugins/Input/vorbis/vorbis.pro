# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/ogg
# ???? - ??????????:  

include(../../plugins.pri)

FORMS += detailsdialog.ui 
HEADERS += decodervorbisfactory.h \
           decoder_vorbis.h \
           detailsdialog.h
SOURCES += decoder_vorbis.cpp \
           decodervorbisfactory.cpp \
           detailsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Input/vorbis
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libvorbis.so
	   
INCLUDEPATH += ../../../qmmp
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib
PKGCONFIG += taglib ogg vorbisfile vorbis
#TRANSLATIONS = translations/vorbis_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
