SOURCES += main.cpp \
    mainwindow.cpp \
    listwidget.cpp \
    configdialog.cpp \
    pluginitem.cpp \
    visualmenu.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    configdialog.h \
    pluginitem.h \
    visualmenu.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qmmp_ui_example
LIBS += -lqmmp \
    -lqmmpui \
    -L/home/user/qmmp-0.4/lib
FORMS += mainwindow.ui \
    configdialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /home/user/qmmp-0.4/include/
