include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Ui/simple
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Ui/libsimple.so

CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

PKGCONFIG += qmmp qmmpui

SOURCES += main.cpp \
    mainwindow.cpp \
    listwidget.cpp \
    visualmenu.cpp \
    renamedialog.cpp \
    simplefactory.cpp \
    positionslider.cpp \
    actionmanager.cpp \
    volumeslider.cpp \
    simplesettings.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    visualmenu.h \
    renamedialog.h \
    simplefactory.h \
    positionslider.h \
    actionmanager.h \
    volumeslider.h \
    simplesettings.h
TEMPLATE = lib


isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Ui
INSTALLS += target

FORMS += forms/mainwindow.ui \
         forms/renamedialog.ui \
    forms/simplesettings.ui
RESOURCES += stuff.qrc translations.qrc

TRANSLATIONS = translations/simple_plugin_ru.ts \
    translations/simple_plugin_tr.ts \
    translations/simple_plugin_zh_CN.ts \
    translations/simple_plugin_cs.ts \
    translations/simple_plugin_pt_BR.ts \
    translations/simple_plugin_uk_UA.ts \
    translations/simple_plugin_zh_TW.ts \
    translations/simple_plugin_de.ts \
    translations/simple_plugin_pl_PL.ts \
    translations/simple_plugin_it.ts \
    translations/simple_plugin_lt.ts \
    translations/simple_plugin_hu.ts \
    translations/simple_plugin_nl.ts \
    translations/simple_plugin_ja.ts \
    translations/simple_plugin_es.ts \
    translations/simple_plugin_sk.ts

