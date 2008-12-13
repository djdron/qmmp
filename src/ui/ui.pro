# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./src
# ???? - ??????????:  ../bin/mp3player

include(../../qmmp.pri)

FORMS += ./forms/configdialog.ui \
         ./forms/preseteditor.ui \
         ./forms/jumptotrackdialog.ui \
         ./forms/aboutdialog.ui \
         ./forms/addurldialog.ui

HEADERS += mainwindow.h \
           button.h \
           display.h \
           skin.h \
           titlebar.h \
           positionbar.h \
           number.h \
           playlist.h \
           listwidget.h \
           pixmapwidget.h \
           playlisttitlebar.h \
           configdialog.h \
           playlistslider.h \
           dock.h \
           eqwidget.h \
           eqtitlebar.h \
           eqslider.h  \
           togglebutton.h \
           eqgraph.h  \
           mainvisual.h \
           inlines.h \
           fft.h \
           logscale.h \
           textscroller.h \
           monostereo.h \
           playstatus.h \
           pluginitem.h \
           volumebar.h \
           balancebar.h \
           playstate.h \
           symboldisplay.h \
           playlistcontrol.h \
           version.h \
           qmmpstarter.h \
           eqpreset.h \
           preseteditor.h \
           jumptotrackdialog.h \
           aboutdialog.h \
           timeindicator.h \
           keyboardmanager.h \
           unixdomainsocket.h \
           addurldialog.h \
           skinreader.h \
           visualmenu.h \
           titlebarcontrol.h \
           shadedvisual.h \
           shadedbar.h \
           builtincommandlineoption.h

SOURCES += mainwindow.cpp \
           mp3player.cpp \
           button.cpp \
           display.cpp \
           skin.cpp \
           titlebar.cpp \
           positionbar.cpp \
           number.cpp \
           playlist.cpp \
           listwidget.cpp \
           pixmapwidget.cpp \
           playlisttitlebar.cpp \
           configdialog.cpp \
           playlistslider.cpp \
           dock.cpp \
           eqwidget.cpp \
           eqtitlebar.cpp \
           eqslider.cpp  \
           togglebutton.cpp \
           eqgraph.cpp  \
           mainvisual.cpp \
           fft.c \
           logscale.cpp \
           textscroller.cpp \
           monostereo.cpp \
           playstatus.cpp \
           pluginitem.cpp \
           volumebar.cpp \
           balancebar.cpp \
           symboldisplay.cpp \
           playlistcontrol.cpp \
           qmmpstarter.cpp \
           eqpreset.cpp \
           preseteditor.cpp \
           jumptotrackdialog.cpp \
           aboutdialog.cpp \
           timeindicator.cpp \
           keyboardmanager.cpp \
           unixdomainsocket.cpp \
           addurldialog.cpp \
           skinreader.cpp \
           visualmenu.cpp \
           titlebarcontrol.cpp \
           shadedvisual.cpp \
           shadedbar.cpp \
           builtincommandlineoption.cpp

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR =./.build/ui/
MOC_DIR =./.build/moc/
OBJECTS_DIR =./.build/obj


QT += network
TARGET = ../../bin/qmmp
CONFIG += thread release \
warn_on
QMAKE_LIBDIR += ../../lib qmmpui
LIBS += -Wl,-rpath,../lib
LIBS += -L../../lib -lqmmp -lqmmpui
INCLUDEPATH += ../
RESOURCES = images/images.qrc \
            stuff.qrc
TEMPLATE = app
target.path = /bin

DEFINES += QMMP_VERSION=$$QMMP_VERSION
contains(CONFIG, SVN_VERSION){
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION-svn\\\"
}else {
    DEFINES += QMMP_STR_VERSION=\\\"$$QMMP_VERSION\\\"
}

desktop.files = qmmp.desktop
desktop.path = /share/applications

icon16.files = images/16x16/qmmp.png
icon32.files = images/32x32/qmmp.png
icon48.files = images/48x48/qmmp.png

icon16.path = /share/icons/hicolor/16x16/apps
icon32.path = /share/icons/hicolor/32x32/apps
icon48.path = /share/icons/hicolor/48x48/apps

INSTALLS += desktop target icon16 icon32 icon48


RESOURCES += translations/qmmp_locales.qrc

TRANSLATIONS = translations/qmmp_ru.ts \
               translations/qmmp_tr.ts \
               translations/qmmp_zh_CN.ts \
               translations/qmmp_cs.ts \
               translations/qmmp_pt_BR.ts \
               translations/qmmp_uk_UA.ts \
               translations/qmmp_zh_TW.ts \
               translations/qmmp_de.ts \
               translations/qmmp_pl_PL.ts




