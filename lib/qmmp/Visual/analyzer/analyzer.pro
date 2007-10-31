#FORMS += detailsdialog.ui 
HEADERS += analyzer.h \
 fft.h \
 visualanalyzerfactory.h \
 inlines.h \
 colorwidget.h \
 settingsdialog.h
SOURCES += analyzer.cpp \
 fft.c \
 visualanalyzerfactory.cpp \
 colorwidget.cpp \
 settingsdialog.cpp
DESTDIR = ../
QMAKE_CLEAN = ../libanalyzer.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include
#TRANSLATIONS = translations/ffmpeg_plugin_ru.ts
#RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Visual
INSTALLS += target
FORMS += settingsdialog.ui

