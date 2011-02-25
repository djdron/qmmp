include(../../plugins.pri)
HEADERS += \
    mmsinputfactory.h \
    mmsinputsource.h \
    settingsdialog.h \
    mmsstreamreader.h
SOURCES += \
    mmsinputfactory.cpp \
    mmsinputsource.cpp \
    settingsdialog.cpp \
    mmsstreamreader.cpp
win32:HEADERS += ../../../../src/qmmp/inputsource.h \
    ../../../../src/qmmp/inputsourcefactory.h
TARGET = $$PLUGINS_PREFIX/Transports/mms
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmms.so
INCLUDEPATH += ../../../
CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib
 
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib
PKGCONFIG += libmms
isEmpty(LIB_DIR):LIB_DIR = /lib
target.path = $$LIB_DIR/qmmp/Transports
INSTALLS += target


FORMS += settingsdialog.ui

TRANSLATIONS = translations/mms_plugin_ru.ts \
    translations/mms_plugin_uk_UA.ts \
    translations/mms_plugin_zh_CN.ts \
    translations/mms_plugin_zh_TW.ts \
    translations/mms_plugin_cs.ts \
    translations/mms_plugin_pl.ts \
    translations/mms_plugin_de.ts \
    translations/mms_plugin_it.ts \
    translations/mms_plugin_tr.ts \
    translations/mms_plugin_lt.ts \
    translations/mms_plugin_nl.ts \
    translations/mms_plugin_ja.ts \
    translations/mms_plugin_es.ts


RESOURCES = translations/translations.qrc
