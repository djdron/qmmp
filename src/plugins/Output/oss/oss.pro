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

TEMPLATE = lib
LIBS += -lqmmp

TRANSLATIONS = translations/oss_plugin_cs.ts \
               translations/oss_plugin_de.ts

RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
