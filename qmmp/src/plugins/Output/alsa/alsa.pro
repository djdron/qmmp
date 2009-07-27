include(../../plugins.pri)

HEADERS += outputalsa.h \
           outputalsafactory.h  \
           settingsdialog.h
SOURCES += outputalsa.cpp \
           outputalsafactory.cpp  \
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/alsa
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libalsa.so

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib

CONFIG += release \
warn_on \
thread \
plugin \
link_pkgconfig

TEMPLATE = lib
LIBS += -lqmmp
PKGCONFIG += alsa

FORMS += settingsdialog.ui
TRANSLATIONS = translations/alsa_plugin_ru.ts \
               translations/alsa_plugin_uk_UA.ts \
               translations/alsa_plugin_zh_CN.ts \
               translations/alsa_plugin_zh_TW.ts \
               translations/alsa_plugin_cs.ts \
               translations/alsa_plugin_pl.ts \
               translations/alsa_plugin_de.ts \
               translations/alsa_plugin_it.ts \
               translations/alsa_plugin_tr.ts \
               translations/alsa_plugin_lt.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
