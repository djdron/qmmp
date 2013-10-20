include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decoderfluidsynthfactory.h \
    decoder_fluidsynth.h
SOURCES += decoder_fluidsynth.cpp \
    decoderfluidsynthfactory.cpp
TARGET = $$PLUGINS_PREFIX/Input/fluidsynth
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libfluidsynth.so
INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -L/usr/lib \
    -I/usr/include \
    -lfluidsynth
#TRANSLATIONS = translations/fluidsynth_plugin_it.ts \
#               translations/fluidsynth_plugin_ru.ts \
#               translations/fluidsynth_plugin_cs.ts \
#               translations/fluidsynth_plugin_de.ts \
#               translations/fluidsynth_plugin_zh_CN.ts \
#               translations/fluidsynth_plugin_zh_TW.ts \
#               translations/fluidsynth_plugin_uk_UA.ts \
#               translations/fluidsynth_plugin_pl.ts \
#               translations/fluidsynth_plugin_tr.ts \
#               translations/fluidsynth_plugin_lt.ts \
#               translations/fluidsynth_plugin_nl.ts \
#               translations/fluidsynth_plugin_ja.ts

#RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
