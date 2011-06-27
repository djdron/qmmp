include(../../qmmp.pri)
unix:TARGET = ../../lib/qmmpui
win32:TARGET = ../../../bin/qmmpui
CONFIG += release \
    warn_on \
    shared \
    qt \
    thread
QMAKE_LIBDIR += ../../lib \
    qmmpui
LIBS += -Wl,-rpath,./
unix:LIBS += -L../../lib \
    -lqmmp
win32:LIBS += -L../../bin \
    -lqmmp0
INCLUDEPATH += ../
TEMPLATE = lib
unix:isEmpty(LIB_DIR):LIB_DIR = /lib
VERSION = $$QMMP_VERSION
unix:target.path = $$LIB_DIR
HEADERS += general.h \
    generalfactory.h \
    generalhandler.h \
    playlistformat.h \
    playlistparser.h \
    commandlinemanager.h \
    commandlineoption.h \
    filedialog.h \
    filedialogfactory.h \
    qtfiledialog_p.h \
    playlistitem.h \
    playlistmodel.h \
    playstate_p.h \
    fileloader_p.h \
    mediaplayer.h \
    playlistsettings_p.h \
    detailsdialog.h \
    tageditor_p.h \
    playlistmanager.h \
    metadataformatter.h \
    templateeditor.h
SOURCES += general.cpp \
    generalhandler.cpp \
    playlistparser.cpp \
    commandlinemanager.cpp \
    filedialog.cpp \
    qtfiledialog.cpp \
    playlistmodel.cpp \
    playstate.cpp \
    playlistitem.cpp \
    fileloader.cpp \
    mediaplayer.cpp \
    playlistsettings.cpp \
    detailsdialog.cpp \
    tageditor.cpp \
    playlistmanager.cpp \
    metadataformatter.cpp \
    templateeditor.cpp
FORMS += detailsdialog.ui \
    tageditor.ui \
    templateeditor.ui
unix:DESTDIR = .
RESOURCES += translations/libqmmpui_locales.qrc
TRANSLATIONS = translations/libqmmpui_ru.ts \
    translations/libqmmpui_tr.ts \
    translations/libqmmpui_zh_CN.ts \
    translations/libqmmpui_cs.ts \
    translations/libqmmpui_pt_BR.ts \
    translations/libqmmpui_uk_UA.ts \
    translations/libqmmpui_zh_TW.ts \
    translations/libqmmpui_de.ts \
    translations/libqmmpui_it.ts \
    translations/libqmmpui_lt.ts \
    translations/libqmmpui_pl.ts \
    translations/libqmmpui_nl.ts \
    translations/libqmmpui_ja.ts \
    translations/libqmmpui_es.ts
unix {
    devel.files += general.h \
                   generalfactory.h \
                   generalhandler.h \
                   playlistformat.h \
                   playlistparser.h \
                   commandlinemanager.h \
                   commandlineoption.h \
                   filedialog.h \
                   filedialogfactory.h \
                   playlistitem.h \
                   playlistmodel.h \
                   mediaplayer.h \
                   detailsdialog.h \
                   playlistmanager.h \
                   metadataformatter.h \
                   templateeditor.h
    devel.path = /include/qmmpui
    INSTALLS += target \
        devel
}
