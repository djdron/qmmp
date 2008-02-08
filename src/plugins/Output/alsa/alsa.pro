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
plugin 
TEMPLATE = lib
LIBS += -lqmmp -lasound
FORMS += settingsdialog.ui
#TRANSLATIONS = translations/alsa_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
