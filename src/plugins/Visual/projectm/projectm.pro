include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/Visual/projectm
QMAKE_CLEAN =$$PLUGINS_PREFIX/Visual/libprojectm.so

HEADERS += projectmwidget.h \
           visualprojectmfactory.h \
           projectmplugin.h
SOURCES += projectmplugin.cpp \
           visualprojectmfactory.cpp \
           projectmwidget.cpp
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig

PKGCONFIG += libprojectM

TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
QT += opengl
LIBS += -lqmmp -L/usr/lib -I/usr/include
TRANSLATIONS = translations/projectm_plugin_cs.ts \
               translations/projectm_plugin_de.ts \
               translations/projectm_plugin_zh_CN.ts \
               translations/projectm_plugin_zh_TW.ts \
               translations/projectm_plugin_pl.ts \
               translations/projectm_plugin_ru.ts \
               translations/projectm_plugin_uk_UA.ts \
               translations/projectm_plugin_it.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Visual
INSTALLS += target
