include(../../qmmp.pri)

TARGET = ../../lib/qmmpui
CONFIG += release \
warn_on \
qt \
thread

TEMPLATE = lib

unix : isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }

unix : DEFINES += LIB_DIR=\\\"$$LIB_DIR\\\"

DEFINES += QMMP_VERSION=$$QMMP_VERSION
contains(CONFIG, SVN_VERSION){
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION-svn\\\"
}else {
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION\\\"
}

VERSION = $$QMMP_VERSION

target.path = $$LIB_DIR
INSTALLS += target
HEADERS += general.h \
           generalfactory.h \
           generalhandler.h \
           songinfo.h \
           control.h \
           playlistformat.h \
           playlistparser.h \
           commandlinemanager.h \
           commandlineoption.h \
           filedialog.h \
           filedialogfactory.h \
           qtfiledialog.h
SOURCES += general.cpp \
           generalhandler.cpp \
           songinfo.cpp \
           control.cpp \
           playlistparser.cpp \
           commandlinemanager.cpp \
           filedialog.cpp \
           qtfiledialog.cpp

DESTDIR = .

RESOURCES += translations/libqmmpui_locales.qrc

TRANSLATIONS = translations/libqmmpui_ru.ts \
               translations/libqmmpui_tr.ts \
               translations/libqmmpui_zh_CN.ts \
               translations/libqmmpui_cs.ts \
               translations/libqmmpui_pt_BR.ts \
               translations/libqmmpui_uk_UA.ts \
               translations/libqmmpui_zh_TW.ts \
               translations/libqmmpui_de.ts

