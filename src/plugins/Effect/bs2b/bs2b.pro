include(../../plugins.pri)

HEADERS += bs2bplugin.h \
           effectbs2bfactory.h \
           settingsdialog.h

SOURCES += bs2bplugin.cpp \
           effectbs2bfactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/bs2b

INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin \
link_pkgconfig


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/bs2b_plugin_cs.ts \
               translations/bs2b_plugin_de.ts \
               translations/bs2b_plugin_zh_CN.ts \
               translations/bs2b_plugin_zh_TW.ts \
               translations/bs2b_plugin_uk_UA.ts \
               translations/bs2b_plugin_pl.ts \
               translations/bs2b_plugin_ru.ts \
               translations/bs2b_plugin_it.ts \
               translations/bs2b_plugin_tr.ts \
               translations/bs2b_plugin_lt.ts \
               translations/bs2b_plugin_nl.ts \
               translations/bs2b_plugin_ja.ts \
               translations/bs2b_plugin_es.ts \
               translations/bs2b_plugin_sr_BA.ts \
               translations/bs2b_plugin_sr_RS.ts
RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    PKGCONFIG += libbs2b
    LIBS += -lqmmp -L/usr/lib -I/usr/include
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libbs2b.so
}

win32 {
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lbs2b
}
