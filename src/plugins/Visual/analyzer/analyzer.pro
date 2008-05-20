include(../../plugins.pri)

TARGET=$$PLUGINS_PREFIX/Visual/analyzer
QMAKE_CLEAN =$$PLUGINS_PREFIX/Visual/libanalyzer.so


FORMS += settingsdialog.ui
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
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include
TRANSLATIONS = translations/analyzer_plugin_cs.ts \
               translations/analyzer_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Visual
INSTALLS += target


