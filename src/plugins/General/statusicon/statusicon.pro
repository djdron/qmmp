include(../../plugins.pri)
INCLUDEPATH += ../../../../src
CONFIG += release \
    warn_on \
    plugin
unix:TARGET = $$PLUGINS_PREFIX/General/statusicon
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libstatusicon.so
win32:TARGET = $$PLUGINS_PREFIX/General/statusicon
TEMPLATE = lib
unix { 
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmpui \
        -lqmmp
}
win32 { 
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmpui0 \
        -lqmmp0
}
TRANSLATIONS = translations/statusicon_plugin_cs.ts \
    translations/statusicon_plugin_de.ts \
    translations/statusicon_plugin_zh_CN.ts \
    translations/statusicon_plugin_zh_TW.ts \
    translations/statusicon_plugin_ru.ts \
    translations/statusicon_plugin_pl.ts \
    translations/statusicon_plugin_uk_UA.ts \
    translations/statusicon_plugin_it.ts \
    translations/statusicon_plugin_tr.ts \
    translations/statusicon_plugin_lt.ts
RESOURCES = translations/translations.qrc
unix { 
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
RESOURCES += images/tray_images.qrc
HEADERS += statusiconfactory.h \
    statusicon.h \
    settingsdialog.h \
    qmmptrayicon.h \
    statusiconpopupwidget.h \
    coverwidget.h
SOURCES += statusiconfactory.cpp \
    statusicon.cpp \
    settingsdialog.cpp \
    qmmptrayicon.cpp \
    statusiconpopupwidget.cpp \
    coverwidget.cpp
FORMS += settingsdialog.ui
win32:HEADERS += ../../../../src/qmmpui/general.h
