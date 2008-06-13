include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/notifier
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libnotifier.so


TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmpui

TRANSLATIONS = translations/notifier_plugin_cs.ts \
               translations/notifier_plugin_de.ts \
               translations/notifier_plugin_zh_CN.ts
RESOURCES = notifier_images.qrc \
            translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
#FORMS += settingsdialog.ui

HEADERS += notifierfactory.h \
 notifier.h \
 popupwidget.h \
 settingsdialog.h
SOURCES += notifierfactory.cpp \
 notifier.cpp \
 popupwidget.cpp \
 settingsdialog.cpp

FORMS += settingsdialog.ui

