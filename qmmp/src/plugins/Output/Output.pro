include(../../../qmmp.pri)

CONFIG += release warn_on
TEMPLATE = subdirs
win32:SUBDIRS += waveout
unix{

SUBDIRS += null

contains(CONFIG, JACK_PLUGIN){
    SUBDIRS += jack
}

contains(CONFIG, OSS_PLUGIN){
    SUBDIRS += oss
}

contains(CONFIG, PULSE_AUDIO_PLUGIN){
    SUBDIRS += pulseaudio
}

contains(CONFIG, ALSA_PLUGIN){
    SUBDIRS += alsa
}

contains(CONFIG, OSS4_PLUGIN){
   SUBDIRS += oss4
}

}
