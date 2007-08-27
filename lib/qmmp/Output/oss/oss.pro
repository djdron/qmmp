FORMS += settingsdialog.ui

HEADERS += outputossfactory.h \
           outputoss.h \
	   settingsdialog.h
           

SOURCES += outputossfactory.cpp \
           outputoss.cpp \ 
           settingsdialog.cpp

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../
QMAKE_CLEAN = ../liboss.so
CONFIG += release \
warn_on \
thread \
plugin 

DESTDIR = ../
TEMPLATE = lib
LIBS += -lqmmp

#TRANSLATIONS = translations/oss_plugin_ru.ts \
#               translations/oss_plugin_uk_UA.ts \
#	       translations/oss_plugin_zh_CN.ts
#RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
