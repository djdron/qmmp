include(../../plugins.pri)
CONFIG += warn_on \
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
    translations/scrobbler_plugin_lt.ts \
    translations/scrobbler_plugin_nl.ts \
    translations/scrobbler_plugin_ja.ts \
    translations/scrobbler_plugin_es.ts \
    translations/scrobbler_plugin_sr_BA.ts \
    translations/scrobbler_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += scrobblerfactory.h \
    settingsdialog.h \
    scrobblerhandler.h \
    scrobblercache.h \
    scrobbler.h \
    defines.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += scrobblerfactory.cpp \
    settingsdialog.cpp \
    scrobblerhandler.cpp \
    scrobblercache.cpp \
    scrobbler.cpp
QT += network
INCLUDEPATH += ../../../
unix:LIBS += -lqmmpui \
    -lqmmp
win32:LIBS += -lqmmpui0 \
    -lqmmp0
FORMS += settingsdialog.ui












