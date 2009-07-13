include(../../../qmmp.pri)

SUBDIRS += mad cue
TEMPLATE = subdirs

unix{
SUBDIRS += vorbis #sndfile wavpack
contains(CONFIG, MODPLUG_PLUGIN){
    #SUBDIRS += modplug
    message(***************************)
    message(* Modplug plugin enabled  *)
    message(***************************)
}

contains(CONFIG, MUSEPACK_PLUGIN){
    SUBDIRS += mpc
    message(***************************)
    message(* Musepack plugin enabled *)
    message(***************************)
}

contains(CONFIG, FLAC_PLUGIN){
    #SUBDIRS += flac
    message(***********************)
    message(* FLAC plugin enabled *)
    message(***********************)
}

contains(CONFIG, FFMPEG_PLUGIN){
   #SUBDIRS += ffmpeg
   message(*************************)
   message(* FFMPEG plugin enabled *)
   message(*************************)
}

contains(CONFIG, AAC_PLUGIN){
  #SUBDIRS += aac
  message(**********************)
  message(* AAC plugin enabled *)
  message(**********************)
}

contains(CONFIG, MPLAYER_PLUGIN){
  #SUBDIRS += mplayer
  message(**************************)
  message(* Mplayer plugin enabled *)
  message(**************************)
}

contains(CONFIG, CDAUDIO_PLUGIN){
  #SUBDIRS += cdaudio
  message(***************************)
  message(* CD Audio plugin enabled *)
  message(***************************)
}
}
