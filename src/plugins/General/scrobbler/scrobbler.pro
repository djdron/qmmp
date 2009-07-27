include(../../plugins.pri)
CONFIG += release \
    warn_on \
    plugin
TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
win32:QMAKE_LIBDIR += ../../../../bin
TARGET = $$PLUGINS_PREFIX/General/scrobbler
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libscrobbler.so
TRANSLATIONS = translations/scrobbler_plugin_cs.ts \
    translations/scrobbler_plugin_de.ts \
    translations/scrobbler_plugin_zh_CN.ts \
    translations/scrobbler_plugin_zh_TW.ts \
    translations/scrobbler_plugin_ru.ts \
    translations/scrobbler_plugin_pl.ts \
    translations/scrobbler_plugin_uk_UA.ts \
    translations/scrobbler_plugin_it.ts \
    translations/scrobbler_plugin_tr.ts \
    translations/scrobbler_plugin_lt.ts
    
RESOURCES = translations/translations.qrc
unix { 
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += scrobblerfactory.h \
    scrobbler.h \
    settingsdialog.h \
    scrobblerhandler.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += scrobblerfactory.cpp \
    scrobbler.cpp \
    settingsdialog.cpp \
    scrobblerhandler.cpp
QT += network
INCLUDEPATH += ../../../
unix:LIBS += -lqmmpui \
    -lqmmp
win32:LIBS += -lqmmpui0 \
    -lqmmp0
FORMS += settingsdialog.ui
