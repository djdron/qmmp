#Common settings for Qmmp Plugin Pack build

#Some conf to redirect intermediate stuff in separate dirs
unix:UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj

unix{
INCLUDEPATH += /home/user/qmmp-0.6/include
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include
}else{
INCLUDEPATH += D:/projects/qmmp-svn/qmmp/src
QMAKE_LIBDIR += D:/projects/qmmp-svn/qmmp/bin
}

#Version

QMMP_PLUGIN_PACK_VERSION = 0.7.0
CONFIG += SVN_VERSION


#Comment/uncomment this if you want to change plugins list

CONFIG += MPG123_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += SIMPLE_UI_PLUGIN
