include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin 

TARGET=$$PLUGINS_PREFIX/General/statusicon
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libstatusicon.so


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmpui

TRANSLATIONS = translations/statusicon_plugin_de.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target

RESOURCES += images/tray_images.qrc


HEADERS += statusiconfactory.h \
statusicon.h \
 settingsdialog.h
SOURCES += statusiconfactory.cpp \
statusicon.cpp \
 settingsdialog.cpp
FORMS += settingsdialog.ui

