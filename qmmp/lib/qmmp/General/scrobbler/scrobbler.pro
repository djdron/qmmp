
QMAKE_CLEAN = ../libscrobbler.so
CONFIG += release \
          warn_on \
          plugin  

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


HEADERS += scrobblerfactory.h \
           scrobbler.h \
 settingsdialog.h
SOURCES += scrobblerfactory.cpp \
           scrobbler.cpp \
 settingsdialog.cpp
QT += network

DESTDIR = ..

INCLUDEPATH += ../../../../src

LIBS += -lqmmpui

FORMS += settingsdialog.ui

