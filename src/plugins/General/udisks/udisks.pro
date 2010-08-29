include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/udisks
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libudisks.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/udisks_plugin_cs.ts \
               translations/udisks_plugin_de.ts \
               translations/udisks_plugin_zh_CN.ts \
               translations/udisks_plugin_zh_TW.ts \
               translations/udisks_plugin_ru.ts \
               translations/udisks_plugin_pl.ts \
               translations/udisks_plugin_uk_UA.ts \
               translations/udisks_plugin_it.ts \
               translations/udisks_plugin_tr.ts \
               translations/udisks_plugin_lt.ts \
               translations/udisks_plugin_nl.ts \
               translations/udisks_plugin_ja.ts \
               translations/udisks_plugin_es.ts

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += udisksfactory.h \
           udisksplugin.h \
           udisksmanager.h \
           udisksdevice.h \
           settingsdialog.h

SOURCES += udisksfactory.cpp \
           udisksplugin.cpp \
           udisksmanager.cpp \
           udisksdevice.cpp \
           settingsdialog.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

FORMS += settingsdialog.ui
