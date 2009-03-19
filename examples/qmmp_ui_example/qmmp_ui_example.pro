SOURCES += main.cpp \
           mainwindow.cpp
HEADERS += mainwindow.h
TEMPLATE = app
CONFIG += warn_on thread qt
TARGET = qmmp_ui_example
LIBS += -lqmmp -lqmmpui
