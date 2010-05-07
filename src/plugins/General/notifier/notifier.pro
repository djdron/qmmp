include(../../plugins.pri)

INCLUDEPATH += ../../../../src
CONFIG += release \
warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/notifier
unix:QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libnotifier.so


TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
unix:LIBS += -lqmmpui -lqmmp
win32:QMAKE_LIBDIR += ../../../../bin
win32:LIBS += -lqmmpui0 -lqmmp0

TRANSLATIONS = translations/notifier_plugin_cs.ts \
               translations/notifier_plugin_de.ts \
               translations/notifier_plugin_zh_CN.ts \
               translations/notifier_plugin_zh_TW.ts \
               translations/notifier_plugin_ru.ts \
               translations/notifier_plugin_pl.ts \
               translations/notifier_plugin_uk_UA.ts \
               translations/notifier_plugin_it.ts \
               translations/notifier_plugin_tr.ts \
               translations/notifier_plugin_lt.ts
RESOURCES = notifier_images.qrc \
            translations/translations.qrc

unix {
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}

HEADERS += notifierfactory.h \
 notifier.h \
 popupwidget.h \
 settingsdialog.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += notifierfactory.cpp \
 notifier.cpp \
 popupwidget.cpp \
 settingsdialog.cpp

FORMS += settingsdialog.ui

