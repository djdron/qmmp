include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin 

TARGET =$$PLUGINS_PREFIX/General/songchange
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libsongchange.so


TEMPLATE = lib
unix : QMAKE_LIBDIR += ../../../../lib
unix : LIBS += -lqmmpui -lqmmp

win32 : QMAKE_LIBDIR += ../../../../bin
win32 : LIBS += -lqmmpui0 -lqmmp0

#TRANSLATIONS = translations/songchange_plugin_cs.ts \
#               translations/songchange_plugin_de.ts \
#               translations/songchange_plugin_zh_CN.ts \
#               translations/songchange_plugin_zh_TW.ts \
#               translations/songchange_plugin_ru.ts \
#               translations/songchange_plugin_pl.ts \
#               translations/songchange_plugin_uk_UA.ts \
#               translations/songchange_plugin_it.ts \
#               translations/songchange_plugin_tr.ts \
#               translations/songchange_plugin_lt.ts \
#               translations/songchange_plugin_nl.ts \
#               translations/songchange_plugin_ja.ts \
#               translations/songchange_plugin_es.ts
#RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += songchangefactory.h \
           songchange.h \
           settingsdialog.h

win32 : HEADERS += ../../../../src/qmmpui/general.h
SOURCES += songchangefactory.cpp \
           songchange.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui
