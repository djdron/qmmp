include(../../plugins.pri)

HEADERS += outputnullfactory.h \
           outputnull.h

SOURCES += outputnullfactory.cpp \
           outputnull.cpp


TARGET=$$PLUGINS_PREFIX/Output/null
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libnull.so

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib

CONFIG += release \
warn_on \
thread \
plugin

TEMPLATE = lib
LIBS += -lqmmp


TRANSLATIONS = translations/null_plugin_cs.ts \
               translations/null_plugin_de.ts \
               translations/null_plugin_zh_CN.ts \
               translations/null_plugin_zh_TW.ts \
               translations/null_plugin_ru.ts \
               translations/null_plugin_pl.ts \
               translations/null_plugin_uk_UA.ts \
               translations/null_plugin_it.ts \
               translations/null_plugin_tr.ts \
               translations/null_plugin_lt.ts \
               translations/null_plugin_nl.ts \
               translations/null_plugin_ja.ts

RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
