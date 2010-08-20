include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decoderwildmidifactory.h \
    decoder_wildmidi.h \
    wildmidihelper.h \
    settingsdialog.h
SOURCES += decoder_wildmidi.cpp \
    decoderwildmidifactory.cpp \
    wildmidihelper.cpp \
    settingsdialog.cpp
TARGET = $$PLUGINS_PREFIX/Input/wildmidi
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libwildmidi.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -L/usr/lib \
    -I/usr/include \
    -lWildMidi
TRANSLATIONS = translations/wildmidi_plugin_it.ts \
               translations/wildmidi_plugin_ru.ts \
               translations/wildmidi_plugin_cs.ts \
               translations/wildmidi_plugin_de.ts \
               translations/wildmidi_plugin_zh_CN.ts \
               translations/wildmidi_plugin_zh_TW.ts \
               translations/wildmidi_plugin_uk_UA.ts \
               translations/wildmidi_plugin_pl.ts \
               translations/wildmidi_plugin_tr.ts \
               translations/wildmidi_plugin_lt.ts \
               translations/wildmidi_plugin_nl.ts \
               translations/wildmidi_plugin_ja.ts

RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
