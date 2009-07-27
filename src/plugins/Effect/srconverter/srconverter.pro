include(../../plugins.pri)

HEADERS += srconverter.h \
 effectsrconverterfactory.h \
 settingsdialog.h

SOURCES += srconverter.cpp \
 effectsrconverterfactory.cpp \
 settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Effect/srconverter
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libsrconverter.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig

PKGCONFIG += samplerate
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

TRANSLATIONS = translations/srconverter_plugin_cs.ts \
               translations/srconverter_plugin_de.ts \
               translations/srconverter_plugin_zh_CN.ts \
               translations/srconverter_plugin_zh_TW.ts \
               translations/srconverter_plugin_uk_UA.ts \
               translations/srconverter_plugin_pl.ts \
               translations/srconverter_plugin_ru.ts \
               translations/srconverter_plugin_it.ts \
               translations/srconverter_plugin_tr.ts \
               translations/srconverter_plugin_lt.ts
               
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target

FORMS += settingsdialog.ui

