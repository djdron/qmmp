include(../../plugins.pri)
FORMS += settingsdialog.ui

HEADERS += outputossfactory.h \
           outputoss.h \
           settingsdialog.h


SOURCES += outputossfactory.cpp \
           outputoss.cpp \ 
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/oss
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/liboss.so


INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib
CONFIG += release \
warn_on \
thread \
plugin

DEFINES += HAVE_SYS_SOUNDCARD_H

TEMPLATE = lib
LIBS += -lqmmp

TRANSLATIONS = translations/oss_plugin_cs.ts \
               translations/oss_plugin_de.ts \
               translations/oss_plugin_zh_CN.ts \
               translations/oss_plugin_zh_TW.ts \
               translations/oss_plugin_ru.ts \
               translations/oss_plugin_pl.ts \
               translations/oss_plugin_uk_UA.ts \
               translations/oss_plugin_it.ts \
               translations/oss_plugin_tr.ts \
               translations/oss_plugin_lt.ts \
               translations/oss_plugin_nl.ts

RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
