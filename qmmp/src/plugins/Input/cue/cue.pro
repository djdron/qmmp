include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodercuefactory.h \
    cueparser.h \
    decoder_cue.h \
    settingsdialog.h \
    cuemetadatamodel.h
SOURCES += decoder_cue.cpp \
    decodercuefactory.cpp \
    cueparser.cpp \
    settingsdialog.cpp \
    cuemetadatamodel.cpp
win32:HEADERS += ../../../../src/qmmp/decoder.h \
    ../../../../src/qmmp/statehandler.h
TARGET = $$PLUGINS_PREFIX/Input/cue
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libcue.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin
TEMPLATE = lib
unix{
   QMAKE_LIBDIR += ../../../../lib
   LIBS += -lqmmp -L/usr/lib
   contains(CONFIG, WITH_ENCA){
       CONFIG += link_pkgconfig
       PKGCONFIG += enca
       DEFINES += WITH_ENCA
   }
}

win32{
   QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0
}

TRANSLATIONS = translations/cue_plugin_ru.ts \
    translations/cue_plugin_uk_UA.ts \
    translations/cue_plugin_zh_CN.ts \
    translations/cue_plugin_zh_TW.ts \
    translations/cue_plugin_cs.ts \
    translations/cue_plugin_pl.ts \
    translations/cue_plugin_de.ts \
    translations/cue_plugin_it.ts \
    translations/cue_plugin_tr.ts \
    translations/cue_plugin_lt.ts \
    translations/cue_plugin_nl.ts \
    translations/cue_plugin_ja.ts
  
RESOURCES = translations/translations.qrc
unix { 
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    desktop.files = qmmp_cue.desktop
    desktop.path = /share/applications
    INSTALLS += target \
                desktop
}
