include(../../plugins.pri)

#FORMS += detailsdialog.ui 
HEADERS += decodercuefactory.h \
           cueparser.h \
           decoder_cue.h \
           settingsdialog.h
SOURCES += decoder_cue.cpp \
           decodercuefactory.cpp \
           cueparser.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Input/cue
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libcue.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin 

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib

TRANSLATIONS = translations/cue_plugin_ru.ts \
               translations/cue_plugin_uk_UA.ts \
               translations/cue_plugin_zh_CN.ts \
               translations/cue_plugin_zh_TW.ts \
               translations/cue_plugin_cs.ts \
               translations/cue_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input

FORMS += settingsdialog.ui

desktop.files = qmmp_cue.desktop
desktop.path = /share/applications

INSTALLS += target desktop
