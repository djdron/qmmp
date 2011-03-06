SOURCES += main.cpp \
    mainwindow.cpp \
    listwidget.cpp \
    configdialog.cpp \
    pluginitem.cpp \
    visualmenu.cpp \
    renamedialog.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    configdialog.h \
    pluginitem.h \
    visualmenu.h \
    renamedialog.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = qmmp_ui_example
LIBS += -lqmmp \
    -lqmmpui \
    -L/home/user/qmmp-0.5/lib
FORMS += mainwindow.ui \
    configdialog.ui \
    dialog.ui
RESOURCES += stuff.qrc
INCLUDEPATH += /home/user/qmmp-0.5/include/
OTHER_FILES += 
