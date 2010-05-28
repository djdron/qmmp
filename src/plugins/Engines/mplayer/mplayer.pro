include(../../plugins.pri)
HEADERS += mplayerenginefactory.h \
    mplayerengine.h \
    settingsdialog.h \
    mplayermetadatamodel.h
SOURCES += mplayerengine.cpp \
    mplayerenginefactory.cpp \
    settingsdialog.cpp \
    mplayermetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Engines/mplayer
QMAKE_CLEAN = $$PLUGINS_PREFIX/Engines/libmplayer.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -L/usr/lib
TRANSLATIONS = translations/mplayer_plugin_ru.ts \
    translations/mplayer_plugin_uk_UA.ts \
    translations/mplayer_plugin_zh_CN.ts \
    translations/mplayer_plugin_zh_TW.ts \
    translations/mplayer_plugin_cs.ts \
    translations/mplayer_plugin_pl.ts \
    translations/mplayer_plugin_de.ts \
    translations/mplayer_plugin_it.ts \
    translations/mplayer_plugin_tr.ts \
    translations/mplayer_plugin_lt.ts \
    translations/mplayer_plugin_nl.ts 
RESOURCES = translations/translations.qrc
isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
FORMS += settingsdialog.ui
