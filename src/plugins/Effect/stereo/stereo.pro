include(../../plugins.pri)

HEADERS += stereoplugin.h \
           effectstereofactory.h \
           settingsdialog.h

SOURCES += stereoplugin.cpp \
           effectstereofactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/stereo
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libstereo.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

TRANSLATIONS = translations/stereo_plugin_cs.ts \
               translations/stereo_plugin_de.ts \
               translations/stereo_plugin_zh_CN.ts \
               translations/stereo_plugin_zh_TW.ts \
               translations/stereo_plugin_uk_UA.ts \
               translations/stereo_plugin_pl.ts \
               translations/stereo_plugin_ru.ts \
               translations/stereo_plugin_it.ts \
               translations/stereo_plugin_tr.ts \
               translations/stereo_plugin_lt.ts \
               translations/stereo_plugin_nl.ts \
               translations/stereo_plugin_ja.ts \
               translations/stereo_plugin_es.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target


FORMS += settingsdialog.ui
