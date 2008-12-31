include(../../plugins.pri)

HEADERS += decodermplayerfactory.h \
           decoder_mplayer.h

SOURCES += decoder_mplayer.cpp \
           decodermplayerfactory.cpp

TARGET =$$PLUGINS_PREFIX/Input/mplayer
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libmplayer.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib

#TRANSLATIONS = translations/mplayer_plugin_ru.ts
#               translations/mplayer_plugin_uk_UA.ts
#               translations/mplayer_plugin_zh_CN.ts
#               translations/mplayer_plugin_zh_TW.ts
#               translations/mplayer_plugin_cs.ts
#               translations/mplayer_plugin_de.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target

