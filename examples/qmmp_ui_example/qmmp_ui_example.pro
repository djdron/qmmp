SOURCES += main.cpp \
    mainwindow.cpp \
    abstractplaylistmodel.cpp \
    playlistitemdelegate.cpp
HEADERS += mainwindow.h \
    abstractplaylistmodel.h \
    playlistitemdelegate.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qmmp_ui_example
LIBS += -lqmmp \
    -lqmmpui
FORMS += mainwindow.ui
RESOURCES += stuff.qrc
