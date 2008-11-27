include(../../plugins.pri)

FORMS += detailsdialog.ui 
HEADERS += decoderflacfactory.h \
           decoder_flac.h \
           detailsdialog.h \
 cueparser.h
SOURCES += decoder_flac.cpp \
           decoderflacfactory.cpp \
           detailsdialog.cpp \
 cueparser.cpp

TARGET =$$PLUGINS_PREFIX/Input/flac
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libflac.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include
PKGCONFIG += taglib flac
TRANSLATIONS = translations/flac_plugin_ru.ts \
               translations/flac_plugin_uk_UA.ts \
               translations/flac_plugin_zh_CN.ts \
               translations/flac_plugin_zh_TW.ts \
               translations/flac_plugin_cs.ts \
               translations/flac_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target

