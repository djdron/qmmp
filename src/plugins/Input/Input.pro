include(../../../qmmp.pri)

SUBDIRS += mad cue vorbis sndfile wavpack
TEMPLATE = subdirs

contains(CONFIG, FLAC_PLUGIN){
    SUBDIRS += flac
}

contains(CONFIG, MUSEPACK_PLUGIN){
    SUBDIRS += mpc
}

contains(CONFIG, MODPLUG_PLUGIN){
    SUBDIRS += modplug
}

unix{

contains(CONFIG, FFMPEG_PLUGIN){
   SUBDIRS += ffmpeg
}

contains(CONFIG, AAC_PLUGIN){
  SUBDIRS += aac
}

contains(CONFIG, CDAUDIO_PLUGIN){
  SUBDIRS += cdaudio
}

contains(CONFIG, WILDMIDI_PLUGIN){
  SUBDIRS += wildmidi
}

contains(CONFIG, GME_PLUGIN){
  SUBDIRS += gme
}

}
