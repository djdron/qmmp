#Common settings for Qmmp build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj



#Comment/uncomment this if you want to change plugins list

CONFIG += JACK_PLUGIN
CONFIG += FLAC_PLUGIN
CONFIG += MUSEPACK_PLUGIN
CONFIG += FFMPEG_PLUGIN
CONFIG += OSS_PLUGIN