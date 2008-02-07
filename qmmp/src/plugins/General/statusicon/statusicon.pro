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

#TRANSLATIONS = translations/ffmpeg_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
#FORMS += settingsdialog.ui

RESOURCES += images/images.qrc


HEADERS += statusiconfactory.h \
statusicon.h \
 settingsdialog.h
SOURCES += statusiconfactory.cpp \
statusicon.cpp \
 settingsdialog.cpp
FORMS += settingsdialog.ui

