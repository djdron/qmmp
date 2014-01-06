include(../../plugins.pri)

HEADERS += crossfadeplugin.h \
           effectcrossfadefactory.h \
           settingsdialog.h

SOURCES += crossfadeplugin.cpp \
           effectcrossfadefactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/crossfade

INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


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
               translations/crossfade_plugin_es.ts \
               translations/crossfade_plugin_sr_BA.ts \
               translations/crossfade_plugin_sr_RS.ts
RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    LIBS += -lqmmp -L/usr/lib -I/usr/include
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libcrossfade.so
}

win32 {
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0
}

