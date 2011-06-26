include (../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += srconverter crossfade stereo
contains(CONFIG, BS2B_PLUGIN){
SUBDIRS += bs2b
}
contains(CONFIG, LADSPA_PLUGIN){
SUBDIRS += ladspa
}
