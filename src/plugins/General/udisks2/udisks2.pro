include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/udisks2
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libudisks2.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/udisks2_plugin_cs.ts \
               translations/udisks2_plugin_de.ts \
               translations/udisks2_plugin_zh_CN.ts \
               translations/udisks2_plugin_zh_TW.ts \
               translations/udisks2_plugin_ru.ts \
               translations/udisks2_plugin_pl.ts \
               translations/udisks2_plugin_uk_UA.ts \
               translations/udisks2_plugin_it.ts \
               translations/udisks2_plugin_tr.ts \
               translations/udisks2_plugin_lt.ts \
               translations/udisks2_plugin_nl.ts \
               translations/udisks2_plugin_ja.ts \
               translations/udisks2_plugin_es.ts \
               translations/udisks2_plugin_sr_BA.ts \
               translations/udisks2_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += udisks2factory.h \
           udisks2plugin.h \
           udisks2manager.h \
           udisks2device.h \
           settingsdialog.h

SOURCES += udisks2factory.cpp \
           udisks2plugin.cpp \
           udisks2manager.cpp \
           udisks2device.cpp \
           settingsdialog.cpp

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

FORMS += settingsdialog.ui
