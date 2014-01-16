#Common settings for Qmmp Plugin Pack build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui
MOC_DIR=./.build/moc
OBJECTS_DIR=./.build/obj

unix{
INCLUDEPATH += /home/user/qmmp-0.8/include
QMAKE_LIBDIR += /home/user/qmmp-0.8/lib
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include
}else{
INCLUDEPATH += D:/projects/qmmp/qmmp-svn/qmmp/src
QMAKE_LIBDIR += D:/projects/qmmp/qmmp-svn/qmmp/bin
INCLUDEPATH += C:/projects/qmmp/qmmp-svn/qmmp/src
QMAKE_LIBDIR += C:/projects/qmmp/qmmp-svn/qmmp/bin
}

#Version

QMMP_PLUGIN_PACK_VERSION = 0.8.0
CONFIG += SVN_VERSION


#Comment/uncomment this if you want to change plugins list

CONFIG += MPG123_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += FFAP_YASM #assemler optimizations
CONFIG += SIMPLE_UI_PLUGIN
