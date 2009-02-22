include(../../plugins.pri)

HEADERS += bs2bplugin.h \
 effectbs2bfactory.h \
 bs2b-2.1.0/bs2b.h

SOURCES += bs2bplugin.cpp \
 effectbs2bfactory.cpp \
 bs2b-2.1.0/bs2b.c

TARGET =$$PLUGINS_PREFIX/Effect/bs2b
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libbs2b.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig

PKGCONFIG += samplerate
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

#TRANSLATIONS = translations/bs2b_plugin_cs.ts
#               translations/bs2b_plugin_de.ts
#               translations/bs2b_plugin_zh_CN.ts
#               translations/bs2b_plugin_zh_TW.ts
#               translations/bs2b_plugin_uk_UA.ts
#               translations/bs2b_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target


