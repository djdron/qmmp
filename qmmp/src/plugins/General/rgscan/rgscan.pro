include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += warn_on \
plugin \
link_pkgconfig

TARGET =$$PLUGINS_PREFIX/General/rgscan
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/librgscan.so


TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp

PKGCONFIG += taglib

win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/rgscan_plugin_cs.ts \
               translations/rgscan_plugin_de.ts \
               translations/rgscan_plugin_zh_CN.ts \
               translations/rgscan_plugin_zh_TW.ts \
               translations/rgscan_plugin_ru.ts \
               translations/rgscan_plugin_pl.ts \
               translations/rgscan_plugin_uk_UA.ts \
               translations/rgscan_plugin_it.ts \
               translations/rgscan_plugin_tr.ts \
               translations/rgscan_plugin_lt.ts \
               translations/rgscan_plugin_nl.ts \
               translations/rgscan_plugin_ja.ts \
               translations/rgscan_plugin_es.ts
#RESOURCES = translations/translations.qrc
unix{
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += rgscanfactory.h \
    rgscanhelper.h \
    rgscandialog.h \
    gain_analysis.h \
    rgscanner.h

win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += rgscanfactory.cpp \
    rgscanhelper.cpp \
    rgscandialog.cpp \
    gain_analysis.c \
    rgscanner.cpp

FORMS += rgscandialog.ui
