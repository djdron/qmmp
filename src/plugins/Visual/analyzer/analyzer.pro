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
win32:HEADERS += ../../../../src/qmmp/visual.h
win32:INCLUDEPATH += ./
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin
TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmp -L/usr/lib -I/usr/include
win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmp0

TRANSLATIONS = translations/analyzer_plugin_cs.ts \
               translations/analyzer_plugin_de.ts \
               translations/analyzer_plugin_zh_CN.ts \
               translations/analyzer_plugin_zh_TW.ts \
               translations/analyzer_plugin_ru.ts \
               translations/analyzer_plugin_pl.ts \
               translations/analyzer_plugin_uk_UA.ts \
               translations/analyzer_plugin_it.ts
RESOURCES = translations/translations.qrc
unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Visual
INSTALLS += target
}
