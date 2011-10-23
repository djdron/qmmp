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
TEMPLATE = app
CONFIG += warn_on \

TARGET = simple

FORMS += mainwindow.ui \
    forms/configdialog.ui \
    forms/renamedialog.ui
RESOURCES += stuff.qrc

