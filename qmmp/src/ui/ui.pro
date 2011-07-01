include(../../qmmp.pri)

HEADERS += qmmpstarter.h \
           builtincommandlineoption.h \
    lxdesupport.h

SOURCES += \
           qmmpstarter.cpp \
           builtincommandlineoption.cpp \
    lxdesupport.cpp \
    main.cpp



# Some conf to redirect intermediate stuff in separate dirs
UI_DIR = ./.build/ui/
MOC_DIR = ./.build/moc/
OBJECTS_DIR = ./.build/obj
QT += network
unix:TARGET = ../../bin/qmmp
win32:TARGET = ../../../bin/qmmp
CONFIG += thread \
    release \
    warn_on
QMAKE_LIBDIR += ../../lib \
    qmmpui
LIBS += -Wl,-rpath,../lib
unix:LIBS += -L../../lib \
    -lqmmp \
    -lqmmpui
win32:LIBS += -L../../bin \
    -lqmmp0 \
    -lqmmpui0
INCLUDEPATH += ../
RESOURCES = images/images.qrc
TEMPLATE = app
unix {
    target.path = /bin
    desktop.files = qmmp.desktop \
        qmmp_enqueue.desktop
    desktop.path = /share/applications
    icon16.files = images/16x16/qmmp.png
    icon32.files = images/32x32/qmmp.png
    icon48.files = images/48x48/qmmp.png
    icon_svg.files = images/scalable/qmmp.png \
        images/scalable/qmmp.png
    icon16.path = /share/icons/hicolor/16x16/apps
    icon32.path = /share/icons/hicolor/32x32/apps
    icon48.path = /share/icons/hicolor/48x48/apps
    icon_svg.path = /share/icons/hicolor/scalable/apps
    INSTALLS += desktop \
        target \
        icon16 \
        icon32 \
        icon48 \
        icon_svg
}
RESOURCES += translations/qmmp_locales.qrc
TRANSLATIONS = translations/qmmp_ru.ts \
    translations/qmmp_tr.ts \
    translations/qmmp_zh_CN.ts \
    translations/qmmp_cs.ts \
    translations/qmmp_pt_BR.ts \
    translations/qmmp_uk_UA.ts \
    translations/qmmp_zh_TW.ts \
    translations/qmmp_de.ts \
    translations/qmmp_pl_PL.ts \
    translations/qmmp_it.ts \
    translations/qmmp_lt.ts \
    translations/qmmp_hu.ts \
    translations/qmmp_nl.ts \
    translations/qmmp_ja.ts \
    translations/qmmp_es.ts \
    translations/qmmp_sk.ts
