include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib \
 qdbus

TARGET =$$PLUGINS_PREFIX/General/mpris
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libmpris.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/mpris_plugin_cs.ts \
               translations/mpris_plugin_de.ts \
               translations/mpris_plugin_zh_CN.ts \
               translations/mpris_plugin_zh_TW.ts \
               translations/mpris_plugin_ru.ts \
               translations/mpris_plugin_pl.ts \
               translations/mpris_plugin_uk_UA.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
#FORMS += settingsdialog.ui

#RESOURCES += images/images.qrc


HEADERS += mprisfactory.h \
           mpris.h  \
           playerobject.h \
 rootobject.h \
 tracklistobject.h

SOURCES += mprisfactory.cpp \
           mpris.cpp \
           playerobject.cpp \
 rootobject.cpp \
 tracklistobject.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

