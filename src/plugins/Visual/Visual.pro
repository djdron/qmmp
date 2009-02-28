include(../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += analyzer

contains(CONFIG, PROJECTM_PLUGIN){
   SUBDIRS += projectm
   message(***************************)
   message(* projectM plugin enabled *)
   message(***************************)
}
