# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./src
# ???? - ??????????:  ../bin/mp3player

include(../qmmp.pri)

FORMS += configdialog.ui \
         preseteditor.ui \
         jumptotrackdialog.ui \
         aboutdialog.ui
HEADERS += mainwindow.h \
           fileloader.h \
           button.h \
           display.h \
           skin.h \
           titlebar.h \
           positionbar.h \
           number.h \
           playlist.h \
           mediafile.h \
           listwidget.h \
           playlistmodel.h \
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
           playlistformat.h \
           playlistcontrol.h \
           version.h \
           tcpserver.h \
           qmmpstarter.h \
           guard.h \
           eqpreset.h \
           preseteditor.h \
           jumptotrackdialog.h \
           aboutdialog.h \
           timeindicator.h \
 keyboardmanager.h
SOURCES += mainwindow.cpp \
           mp3player.cpp \
           fileloader.cpp \
           button.cpp \
           display.cpp \
           skin.cpp \
           titlebar.cpp \
           positionbar.cpp \
           number.cpp \
           playlist.cpp \
           mediafile.cpp \
           listwidget.cpp \
           playlistmodel.cpp \
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
	   playstate.cpp \
	   symboldisplay.cpp \
	   playlistformat.cpp \
	   playlistcontrol.cpp \
	   qmmpstarter.cpp \
	   tcpserver.cpp \
	   guard.cpp \
           eqpreset.cpp \
           preseteditor.cpp \
	   jumptotrackdialog.cpp \
	   aboutdialog.cpp \
           timeindicator.cpp \
 keyboardmanager.cpp
contains(CONFIG,XSPF_PLUGIN){
    message(*********************************************)
    message(*  XSPF support will be compiled as plugin  *)
    message(*********************************************)
    DEFINES += XSPF_PLUGIN
}else {
    DEFINES -= XSPF_PLUGIN
    message(*******************************************)
    message(*  XSPF support will be compiled in QMMP  *)
    message(*******************************************)
    QT += xml
}

QT += network
TARGET = ../bin/qmmp.real
CONFIG += thread release \
warn_on
QMAKE_LIBDIR += ../lib
LIBS += -Wl,-rpath,../lib -lqmmp
INCLUDEPATH += ../lib
RESOURCES = images/images.qrc \
            stuff.qrc 
#	    translations/qmmp_locales.qrc

#TRANSLATIONS = translations/qmmp_ru.ts \
#	       translations/qmmp_tr.ts \
#	       translations/qmmp_zh_CN.ts
TEMPLATE = app
script.files += ../bin/qmmp
script.path = /bin 
target.path = /bin
INSTALLS += target script

HEADERS += addurldialog.h
SOURCES += addurldialog.cpp
FORMS += addurldialog.ui
