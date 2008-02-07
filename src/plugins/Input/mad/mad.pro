# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/mad
# ???? - ??????????:  

include(../../plugins.pri)


FORMS += detailsdialog.ui \
         settingsdialog.ui 
HEADERS += decodermadfactory.h \
           decoder_mad.h \
           detailsdialog.h \
           settingsdialog.h
SOURCES += decoder_mad.cpp \
           decodermadfactory.cpp \
           detailsdialog.cpp \
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Input/mad
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libmad.so

INCLUDEPATH += ../../../qmmp
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp
PKGCONFIG += taglib mad
#TRANSLATIONS = translations/mad_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
