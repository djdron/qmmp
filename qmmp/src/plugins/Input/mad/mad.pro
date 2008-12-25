include(../../plugins.pri)


FORMS += detailsdialog.ui \
         settingsdialog.ui 
HEADERS += decodermadfactory.h \
           decoder_mad.h \
           detailsdialog.h \
           settingsdialog.h \
           tagextractor.h
SOURCES += decoder_mad.cpp \
           decodermadfactory.cpp \
           detailsdialog.cpp \
           settingsdialog.cpp \
           tagextractor.cpp

TARGET =$$PLUGINS_PREFIX/Input/mad
QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libmad.so

INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -lmad
PKGCONFIG += taglib mad
TRANSLATIONS = translations/mad_plugin_ru.ts \
               translations/mad_plugin_uk_UA.ts \
               translations/mad_plugin_zh_CN.ts \
               translations/mad_plugin_zh_TW.ts \
               translations/mad_plugin_cs.ts \
               translations/mad_plugin_de.ts
RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
