include(../../../qmmp.pri)

SUBDIRS += mad vorbis cue # sndfile wavpack
TEMPLATE = subdirs

contains(CONFIG, MODPLUG_PLUGIN){
#    SUBDIRS += modplug
    message(***************************)
    message(* Modplug plugin enabled  *)
    message(***************************)
}

contains(CONFIG, MUSEPACK_PLUGIN){
#    SUBDIRS += mpc
    message(***************************)
    message(* Musepack plugin enabled *)
    message(***************************)
}

contains(CONFIG, FLAC_PLUGIN){
    SUBDIRS += flac
    message(***********************)
    message(* FLAC plugin enabled *)
    message(***********************)
}

contains(CONFIG, FFMPEG_PLUGIN){
#   SUBDIRS += ffmpeg
   message(*************************)
   message(* FFMPEG plugin enabled *)
   message(*************************)
}

