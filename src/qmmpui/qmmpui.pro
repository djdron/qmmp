include(../../qmmp.pri)

TARGET = ../../lib/qmmpui
CONFIG += release \
warn_on \
qt \
thread 

TEMPLATE = lib


isEmpty(LIB_DIR){
    LIB_DIR = /lib
}

unix {
    LINE1 = $$sprintf(echo \"%1ifndef CONFIG_H\" > ./config.h, $$LITERAL_HASH)
    LINE2 = $$sprintf(echo \"%1define CONFIG_H\" >> ./config.h, $$LITERAL_HASH)
    LINE3 = $$sprintf(echo \"%1define LIB_DIR \\\"%2\\\"\" >> ./config.h, $$LITERAL_HASH, $$LIB_DIR)
    LINE4 = $$sprintf(echo \"%1endif\" >> ./config.h, $$LITERAL_HASH)
    system($$LINE1)
    system($$LINE2)
    system($$LINE3)
    system($$LINE4)
    QMAKE_CLEAN = ./config.h
}


target.path = $$LIB_DIR
INSTALLS += target
HEADERS += general.h \
generalfactory.h \
 generalhandler.h \
 songinfo.h \
 control.h
SOURCES += general.cpp \
 generalhandler.cpp \
 songinfo.cpp \
 control.cpp

