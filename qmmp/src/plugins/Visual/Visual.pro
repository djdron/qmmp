include(../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += analyzer
unix{
contains(CONFIG, PROJECTM_PLUGIN){
   SUBDIRS += projectm
}
}
