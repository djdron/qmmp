include(../../plugins.pri)

HEADERS += outputwaveoutfactory.h \
           outputwaveout.h

SOURCES += outputwaveoutfactory.cpp \
           outputwaveout.cpp

HEADERS += ../../../../src/qmmp/output.h


TARGET=$$PLUGINS_PREFIX/Output/waveout

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../bin

CONFIG += release \
warn_on \
thread \
plugin 

TEMPLATE = lib
LIBS += -lqmmp0 -lwinmm

TRANSLATIONS = translations/waveout_plugin_cs.ts \
               translations/waveout_plugin_de.ts \
               translations/waveout_plugin_zh_CN.ts \
               translations/waveout_plugin_zh_TW.ts \
               translations/waveout_plugin_ru.ts \
               translations/waveout_plugin_pl.ts \
               translations/waveout_plugin_uk_UA.ts \
               translations/waveout_plugin_it.ts \
               translations/waveout_plugin_tr.ts \
               translations/waveout_plugin_lt.ts \
               translations/waveout_plugin_nl.ts \
               translations/waveout_plugin_ja.ts \
               translations/waveout_plugin_es.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
