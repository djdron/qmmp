include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin 

TARGET =$$PLUGINS_PREFIX/General/fileops
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libfileops.so


TEMPLATE = lib
unix : QMAKE_LIBDIR += ../../../../lib
unix : LIBS += -lqmmpui -lqmmp

win32 : QMAKE_LIBDIR += ../../../../bin
win32 : LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/fileops_plugin_cs.ts \
               translations/fileops_plugin_de.ts \
               translations/fileops_plugin_zh_CN.ts \
               translations/fileops_plugin_zh_TW.ts \
               translations/fileops_plugin_ru.ts \
               translations/fileops_plugin_pl.ts \
               translations/fileops_plugin_uk_UA.ts \
               translations/fileops_plugin_it.ts \
               translations/fileops_plugin_tr.ts \
               translations/fileops_plugin_lt.ts \
               translations/fileops_plugin_nl.ts \
               translations/fileops_plugin_ja.ts \
               translations/fileops_plugin_es.ts
RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += fileopsfactory.h \
           fileops.h \
           settingsdialog.h \
           hotkeydialog.h
win32 : HEADERS += ../../../../src/qmmpui/general.h
SOURCES += fileopsfactory.cpp \
           fileops.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp

FORMS += settingsdialog.ui \
         hotkeydialog.ui


