include(../../plugins.pri)
FORMS += 
HEADERS += decoderaacfactory.h \
    decoder_aac.h \
    aacfile.h \
    aacmetadatamodel.h
SOURCES += decoder_aac.cpp \
    decoderaacfactory.cpp \
    aacfile.cpp \
    aacmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/aac
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libaac.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -lfaad \
    -L/usr/lib \
    -I/usr/include
PKGCONFIG += taglib
TRANSLATIONS = translations/aac_plugin_ru.ts \
    translations/aac_plugin_uk_UA.ts \
    translations/aac_plugin_zh_CN.ts \
    translations/aac_plugin_zh_TW.ts \
    translations/aac_plugin_cs.ts \
    translations/aac_plugin_pl.ts \
    translations/aac_plugin_de.ts \
    translations/aac_plugin_it.ts \
    translations/aac_plugin_tr.ts \
    translations/aac_plugin_lt.ts \
    translations/aac_plugin_nl.ts \
    translations/aac_plugin_ja.ts

RESOURCES = translations/translations.qrc
isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
