include(../../plugins.pri)
FORMS += 
HEADERS += decoderwavpackfactory.h \
    decoder_wavpack.h \
    cueparser.h \
    wavpackmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_wavpack.cpp \
    decoderwavpackfactory.cpp \
    cueparser.cpp \
    wavpackmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/wavpack
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libwavpack.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp \
    -L/usr/lib \
    -I/usr/include
PKGCONFIG += wavpack
TRANSLATIONS = translations/wavpack_plugin_cs.ts \
    translations/wavpack_plugin_de.ts \
    translations/wavpack_plugin_zh_CN.ts \
    translations/wavpack_plugin_zh_TW.ts \
    translations/wavpack_plugin_ru.ts \
    translations/wavpack_plugin_pl.ts \
    translations/wavpack_plugin_uk_UA.ts \
    translations/wavpack_plugin_it.ts \
    translations/wavpack_plugin_tr.ts \
    translations/wavpack_plugin_lt.ts
RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
