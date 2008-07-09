include(../../plugins.pri)

INCLUDEPATH += ../../../../src

TARGET =$$PLUGINS_PREFIX/FileDialogs/qmmpfiledialog
QMAKE_CLEAN =$$PLUGINS_PREFIX/FileDialogs/libqmmpfiledialog.so

HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h

SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp


FORMS += qmmpfiledialog.ui


QMAKE_CLEAN += $$PLUGINS_PREFIX/FileDialogs/libqmmpfiledialog.so


CONFIG += release warn_on plugin

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmpui

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/FileDialogs
INSTALLS += target

TRANSLATIONS = translations/qmmp_file_dialog_plugin_ru.ts \
               translations/qmmp_file_dialog_plugin_tr.ts \
               translations/qmmp_file_dialog_plugin_zh_CN.ts \
               translations/qmmp_file_dialog_plugin_cs.ts \
               translations/qmmp_file_dialog_plugin_pt_BR.ts \
               translations/qmmp_file_dialog_plugin_uk_UA.ts \
               translations/qmmp_file_dialog_plugin_zh_TW.ts \
               translations/qmmp_file_dialog_plugin_de.ts
