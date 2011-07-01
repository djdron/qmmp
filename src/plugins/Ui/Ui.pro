include(../../../qmmp.pri)
TEMPLATE = subdirs


contains(CINFIG, WITH_SKINNED){
SUBDIRS += skinned
}

