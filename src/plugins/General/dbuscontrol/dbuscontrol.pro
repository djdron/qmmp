include(../../plugins.pri)

CONFIG += release \
warn_on \
plugin  \
 lib \
 qdbus

TARGET=$$PLUGINS_PREFIX/General/dbuscontrol
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libdbuscontrol.so

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/dbuscontrol_plugin_de.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
#FORMS += settingsdialog.ui

#RESOURCES += images/images.qrc


HEADERS += dbuscontrolfactory.h \
           dbuscontrol.h  \
           dbusadaptor.h

SOURCES += dbuscontrolfactory.cpp \
           dbuscontrol.cpp \
           dbusadaptor.cpp
    
INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

