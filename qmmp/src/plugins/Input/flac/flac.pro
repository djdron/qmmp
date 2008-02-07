# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/flac
# ???? - ??????????:  

include(../../plugins.pri)

FORMS += detailsdialog.ui 
HEADERS += decoderflacfactory.h \
           decoder_flac.h \
           detailsdialog.h
SOURCES += decoder_flac.cpp \
           decoderflacfactory.cpp \
           detailsdialog.cpp
	   
TARGET=$$PLUGINS_PREFIX/Input/flac
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libflac.so
   	
INCLUDEPATH += ../../../qmmp
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include
PKGCONFIG += taglib flac
#TRANSLATIONS = translations/flac_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target

