include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodermadfactory.h \
    decoder_mad.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_mad.cpp \
    decodermadfactory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/mad
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmad.so
INCLUDEPATH += ../../../ \
    ./
win32:INCLUDEPATH += D:\MINGW\include\taglib
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
unix:QMAKE_LIBDIR += ../../../../lib
win32:QMAKE_LIBDIR += ../../../../bin
unix:LIBS += -lqmmp \
    -lmad
unix:PKGCONFIG += taglib \
    mad
win32:LIBS += -lqmmp0 \
    -lmad \
    -ltag.dll \
    -ltag_c.dll
TRANSLATIONS = translations/mad_plugin_ru.ts \
    translations/mad_plugin_uk_UA.ts \
    translations/mad_plugin_zh_CN.ts \
    translations/mad_plugin_zh_TW.ts \
    translations/mad_plugin_cs.ts \
    translations/mad_plugin_pl.ts \
    translations/mad_plugin_de.ts \
    translations/mad_plugin_it.ts \
    translations/mad_plugin_tr.ts \
    translations/mad_plugin_tr.ts \
    translations/mad_plugin_lt.ts \
    translations/mad_plugin_nl.ts

RESOURCES = translations/translations.qrc
unix { 
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
win32:HEADERS += ../../../../src/qmmp/metadatamodel.h \
    ../../../../src/qmmp/decoderfactory.h
