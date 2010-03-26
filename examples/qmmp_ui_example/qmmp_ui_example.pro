SOURCES += main.cpp \
    mainwindow.cpp \
    listwidget.cpp
HEADERS += mainwindow.h \
    listwidget.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qmmp_ui_example
LIBS += -lqmmp \
    -lqmmpui \
    -L/home/user/qmmp-0.4/lib
FORMS += mainwindow.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /home/user/qmmp-0.4/include/
