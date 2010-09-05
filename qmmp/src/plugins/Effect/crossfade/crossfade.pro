include(../../plugins.pri)

HEADERS += crossfadeplugin.h \
           effectcrossfadefactory.h \
           settingsdialog.h

SOURCES += crossfadeplugin.cpp \
           effectcrossfadefactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/crossfade
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libcrossfade.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

TRANSLATIONS = translations/crossfade_plugin_cs.ts \
               translations/crossfade_plugin_de.ts \
               translations/crossfade_plugin_zh_CN.ts \
               translations/crossfade_plugin_zh_TW.ts \
               translations/crossfade_plugin_uk_UA.ts \
               translations/crossfade_plugin_pl.ts \
               translations/crossfade_plugin_ru.ts \
               translations/crossfade_plugin_it.ts \
               translations/crossfade_plugin_tr.ts \
               translations/crossfade_plugin_lt.ts \
               translations/crossfade_plugin_nl.ts \
               translations/crossfade_plugin_ja.ts \
               translations/crossfade_plugin_es.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target


FORMS += settingsdialog.ui
