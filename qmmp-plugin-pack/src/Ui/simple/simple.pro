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
    configdialog.cpp \
    pluginitem.cpp \
    visualmenu.cpp \
    renamedialog.cpp \
    simplefactory.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    configdialog.h \
    pluginitem.h \
    visualmenu.h \
    renamedialog.h \
    simplefactory.h
TEMPLATE = lib

FORMS += mainwindow.ui \
    forms/configdialog.ui \
    forms/renamedialog.ui
RESOURCES += stuff.qrc

