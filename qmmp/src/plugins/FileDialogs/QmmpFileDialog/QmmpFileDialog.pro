include(../../plugins.pri)

INCLUDEPATH += ../../../../src

TARGET =$$PLUGINS_PREFIX/FileDialogs/qmmpfiledialog


HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h

SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp


FORMS += qmmpfiledialog.ui

CONFIG += warn_on plugin

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib


TRANSLATIONS = translations/qmmp_file_dialog_plugin_ru.ts \
               translations/qmmp_file_dialog_plugin_tr.ts \
               translations/qmmp_file_dialog_plugin_zh_CN.ts \
               translations/qmmp_file_dialog_plugin_cs.ts \
               translations/qmmp_file_dialog_plugin_pl.ts \
               translations/qmmp_file_dialog_plugin_pt_BR.ts \
               translations/qmmp_file_dialog_plugin_uk_UA.ts \
               translations/qmmp_file_dialog_plugin_zh_TW.ts \
               translations/qmmp_file_dialog_plugin_de.ts \
               translations/qmmp_file_dialog_plugin_it.ts \
               translations/qmmp_file_dialog_plugin_lt.ts \
               translations/qmmp_file_dialog_plugin_nl.ts \
               translations/qmmp_file_dialog_plugin_ja.ts \
               translations/qmmp_file_dialog_plugin_es.ts \
               translations/qmmp_file_dialog_plugin_sr_BA.ts \
               translations/qmmp_file_dialog_plugin_sr_RS.ts

RESOURCES += translations/translations.qrc

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/qmmp/FileDialogs
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/FileDialogs/libqmmpfiledialog.so
    LIBS += -lqmmpui -lqmmp
}

win32 {
    LIBS += -lqmmpui0 -lqmmp0
    QMAKE_LIBDIR += ../../../../bin
}
