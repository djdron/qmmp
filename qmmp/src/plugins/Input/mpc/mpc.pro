include(../../plugins.pri)
FORMS += 
HEADERS += decodermpcfactory.h \
    decoder_mpc.h \
    mpcmetadatamodel.h
SOURCES += decoder_mpc.cpp \
    decodermpcfactory.cpp \
    mpcmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/mpc

INCLUDEPATH += ../../../
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib

TRANSLATIONS = translations/mpc_plugin_ru.ts \
    translations/mpc_plugin_uk_UA.ts \
    translations/mpc_plugin_zh_CN.ts \
    translations/mpc_plugin_zh_TW.ts \
    translations/mpc_plugin_cs.ts \
    translations/mpc_plugin_pl.ts \
    translations/mpc_plugin_de.ts \
    translations/mpc_plugin_it.ts \
    translations/mpc_plugin_tr.ts \
    translations/mpc_plugin_lt.ts \
    translations/mpc_plugin_nl.ts \
    translations/mpc_plugin_ja.ts \
    translations/mpc_plugin_es.ts \
    translations/mpc_plugin_sr_BA.ts \
    translations/mpc_plugin_sr_RS.ts

RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += taglib
    LIBS += -lqmmp -lmpcdec  -I/usr/include
    exists(/usr/include/mpcdec/mpcdec.h):DEFINES += MPC_OLD_API
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmpc.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 -lmpcdec -ltag.dll
    #DEFINES += MPC_OLD_API
}
