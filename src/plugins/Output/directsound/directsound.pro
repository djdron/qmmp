include(../../plugins.pri)

HEADERS += outputdirectsoundfactory.h \
           outputdirectsound.h

SOURCES += outputdirectsoundfactory.cpp \
           outputdirectsound.cpp

HEADERS += ../../../../src/qmmp/output.h


TARGET=$$PLUGINS_PREFIX/Output/directsound

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../bin

CONFIG += warn_on \
thread \
plugin

TEMPLATE = lib
LIBS += -lqmmp0 -ldxguid -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -ldsound

TRANSLATIONS = translations/directsound_plugin_cs.ts \
               translations/directsound_plugin_de.ts \
               translations/directsound_plugin_zh_CN.ts \
               translations/directsound_plugin_zh_TW.ts \
               translations/directsound_plugin_ru.ts \
               translations/directsound_plugin_pl.ts \
               translations/directsound_plugin_uk_UA.ts \
               translations/directsound_plugin_it.ts \
               translations/directsound_plugin_tr.ts \
               translations/directsound_plugin_lt.ts \
               translations/directsound_plugin_nl.ts \
               translations/directsound_plugin_ja.ts \
               translations/directsound_plugin_es.ts \
               translations/directsound_plugin_sr_BA.ts \
               translations/directsound_plugin_sr_RS.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
