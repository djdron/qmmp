include(../../plugins.pri)

HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h 
SOURCES += decoder_sndfile.cpp \
           decodersndfilefactory.cpp

TARGET=$$PLUGINS_PREFIX/Input/sndfile
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libsndfile.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

PKGCONFIG += sndfile
TRANSLATIONS = translations/sndfile_plugin_cs.ts \
               translations/sndfile_plugin_de.ts \
               translations/sndfile_plugin_zh_CN.ts \
               translations/sndfile_plugin_zh_TW.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
