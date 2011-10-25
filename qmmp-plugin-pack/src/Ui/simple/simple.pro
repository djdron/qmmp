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
    volumeslider.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    visualmenu.h \
    renamedialog.h \
    simplefactory.h \
    positionslider.h \
    actionmanager.h \
    volumeslider.h
TEMPLATE = lib


isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Ui
INSTALLS += target

FORMS += forms/mainwindow.ui \
         forms/renamedialog.ui
RESOURCES += stuff.qrc









