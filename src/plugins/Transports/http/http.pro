include(../../plugins.pri)
HEADERS += downloader.h \
    streamreader.h \
    httpinputfactory.h \
    httpinputsource.h \
    settingsdialog.h
SOURCES += downloader.cpp \
    streamreader.cpp \
    httpinputfactory.cpp \
    httpinputsource.cpp \
    settingsdialog.cpp
win32:HEADERS += ../../../../src/qmmp/inputsource.h \
    ../../../../src/qmmp/inputsourcefactory.h
TARGET = $$PLUGINS_PREFIX/Transports/http
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libhttp.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
unix { 
    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp \
        -L/usr/lib
    PKGCONFIG += libcurl
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Transports
    INSTALLS += target
    contains(CONFIG, WITH_ENCA){
       CONFIG += link_pkgconfig
       PKGCONFIG += enca
       DEFINES += WITH_ENCA
    }
}
win32 { 
    QMAKE_LIBDIR += ../../../../bin
    LIBS += -lqmmp0 \
        -lcurldll
}
FORMS += settingsdialog.ui

TRANSLATIONS = translations/http_plugin_ru.ts \
    translations/http_plugin_uk_UA.ts \
    translations/http_plugin_zh_CN.ts \
    translations/http_plugin_zh_TW.ts \
    translations/http_plugin_cs.ts \
    translations/http_plugin_pl.ts \
    translations/http_plugin_de.ts \
    translations/http_plugin_it.ts \
    translations/http_plugin_tr.ts \
    translations/http_plugin_lt.ts \
    translations/http_plugin_nl.ts \
    translations/http_plugin_ja.ts

RESOURCES = translations/translations.qrc
