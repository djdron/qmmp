include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/hal
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhal.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/hal_plugin_cs.ts \
               translations/hal_plugin_de.ts \
               translations/hal_plugin_zh_CN.ts \
               translations/hal_plugin_zh_TW.ts \
               translations/hal_plugin_ru.ts \
               translations/hal_plugin_pl.ts \
               translations/hal_plugin_uk_UA.ts \
               translations/hal_plugin_it.ts \
               translations/hal_plugin_tr.ts \
               translations/hal_plugin_lt.ts \
               translations/hal_plugin_nl.ts \
               translations/hal_plugin_ja.ts \
               translations/hal_plugin_es.ts

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += halfactory.h \
           halplugin.h \
           halmanager.h \
           haldevice.h \
           settingsdialog.h

SOURCES += halfactory.cpp \
           halplugin.cpp \
           halmanager.cpp \
           haldevice.cpp \
           settingsdialog.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

FORMS += settingsdialog.ui
