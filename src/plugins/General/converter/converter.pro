include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin \
link_pkgconfig

TARGET =$$PLUGINS_PREFIX/General/converter
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libconverter.so


TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp -ltag

PKGCONFIG += taglib

win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/converter_plugin_cs.ts \
               translations/converter_plugin_de.ts \
               translations/converter_plugin_zh_CN.ts \
               translations/converter_plugin_zh_TW.ts \
               translations/converter_plugin_ru.ts \
               translations/converter_plugin_pl.ts \
               translations/converter_plugin_uk_UA.ts \
               translations/converter_plugin_it.ts \
               translations/converter_plugin_tr.ts \
               translations/converter_plugin_lt.ts \
               translations/converter_plugin_nl.ts \
               translations/converter_plugin_ja.ts \
               translations/converter_plugin_es.ts
RESOURCES = translations/translations.qrc presets.qrc
unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += converterfactory.h \
    converterhelper.h \
    converterdialog.h \
    converter.h \
    preseteditor.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += converterfactory.cpp \
    converterhelper.cpp \
    converterdialog.cpp \
    converter.cpp \
    preseteditor.cpp

FORMS += converterdialog.ui \
    preseteditor.ui





