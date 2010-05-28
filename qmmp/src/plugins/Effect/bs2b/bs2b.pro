include(../../plugins.pri)

HEADERS += bs2bplugin.h \
           effectbs2bfactory.h \
           settingsdialog.h

SOURCES += bs2bplugin.cpp \
           effectbs2bfactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/bs2b
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libbs2b.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig

PKGCONFIG += libbs2b
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

TRANSLATIONS = translations/bs2b_plugin_cs.ts \
               translations/bs2b_plugin_de.ts \
               translations/bs2b_plugin_zh_CN.ts \
               translations/bs2b_plugin_zh_TW.ts \
               translations/bs2b_plugin_uk_UA.ts \
               translations/bs2b_plugin_pl.ts \
               translations/bs2b_plugin_ru.ts \
               translations/bs2b_plugin_it.ts \
               translations/bs2b_plugin_tr.ts \
               translations/bs2b_plugin_lt.ts \
               translations/bs2b_plugin_nl.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target


FORMS += settingsdialog.ui
