include(../../plugins.pri)

CONFIG += release \
          warn_on \
          plugin  

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TARGET=$$PLUGINS_PREFIX/General/scrobbler
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libscrobbler.so


TRANSLATIONS = translations/scrobbler_plugin_de.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
#FORMS += settingsdialog.ui

#RESOURCES += images/images.qrc


HEADERS += scrobblerfactory.h \
           scrobbler.h \
 settingsdialog.h
SOURCES += scrobblerfactory.cpp \
           scrobbler.cpp \
 settingsdialog.cpp
QT += network


INCLUDEPATH += ../../../

LIBS += -lqmmpui

FORMS += settingsdialog.ui

