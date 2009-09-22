include(../../plugins.pri)
FORMS += 
HEADERS += decodermpcfactory.h \
    decoder_mpc.h \
    mpcmetadatamodel.h
SOURCES += decoder_mpc.cpp \
    decodermpcfactory.cpp \
    mpcmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/mpc
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmpc.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -L/usr/lib \
    -lmpcdec \
    -I/usr/include
PKGCONFIG += taglib
TRANSLATIONS = translations/mpc_plugin_ru.ts \
    translations/mpc_plugin_uk_UA.ts \
    translations/mpc_plugin_zh_CN.ts \
    translations/mpc_plugin_zh_TW.ts \
    translations/mpc_plugin_cs.ts \
    translations/mpc_plugin_pl.ts \
    translations/mpc_plugin_de.ts \
    translations/mpc_plugin_it.ts \
    translations/mpc_plugin_tr.ts \
    translations/mpc_plugin_lt.ts
RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR):LIB_DIR = /lib
exists(/usr/include/mpcdec/mpcdec.h):DEFINES += MPC_OLD_API
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
