include (../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += crossfade stereo

contains(CONFIG, BS2B_PLUGIN){
SUBDIRS += bs2b
}

unix {
    SUBDIRS += srconverter
    contains(CONFIG, LADSPA_PLUGIN){
    SUBDIRS += ladspa
    }
}
