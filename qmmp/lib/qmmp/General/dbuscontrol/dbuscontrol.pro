
QMAKE_CLEAN = ../libdbuscontrol.so
CONFIG += release \
warn_on \
plugin  \
 lib \
 qdbus


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../src/qmmpui

#TRANSLATIONS = translations/ffmpeg_plugin_ru.ts
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
DESTDIR = ..

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

