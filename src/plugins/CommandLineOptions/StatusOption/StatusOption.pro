include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/statusoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libstatusoption.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/status_plugin_cs.ts \
               translations/status_plugin_de.ts \
               translations/status_plugin_pl.ts \
               translations/status_plugin_ru.ts \
               translations/status_plugin_uk_UA.ts \
               translations/status_plugin_zh_TW.ts \
               translations/status_plugin_zh_CN.ts \
               translations/status_plugin_it.ts \
               translations/status_plugin_tr.ts \
               translations/status_plugin_lt.ts \
               translations/status_plugin_nl.ts \
               translations/status_plugin_ja.ts \
               translations/status_plugin_es.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui -lqmmp

HEADERS += statusoption.h

SOURCES += statusoption.cpp
