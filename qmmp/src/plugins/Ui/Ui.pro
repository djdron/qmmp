include(../../../qmmp.pri)
TEMPLATE = subdirs


contains(CONFIG, WITH_SKINNED){
SUBDIRS += skinned
}

