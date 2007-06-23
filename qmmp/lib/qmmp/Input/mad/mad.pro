# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/mad
# ???? - ??????????:  

FORMS += detailsdialog.ui \
         settingsdialog.ui 
HEADERS += decodermadfactory.h \
           decoder_mad.h \
           detailsdialog.h \
           settingsdialog.h \
           id3tag.h 
SOURCES += decoder_mad.cpp \
           decodermadfactory.cpp \
           detailsdialog.cpp \
           settingsdialog.cpp \
           id3tag.cpp 
DESTDIR = ../
QMAKE_CLEAN += ../libmad.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp
PKGCONFIG += taglib mad
#TRANSLATIONS = translations/mad_plugin_ru.ts
#RESOURCES = translations/translations.qrc
target.path = /lib/qmmp/Input
INSTALLS += target
