include (../../../qmmp.pri)
TEMPLATE = subdirs

SUBDIRS += srconverter
contains(CONFIG, BS2B_PLUGIN){
message(***********************)
message(* BS2B plugin enabled *)
message(***********************)
SUBDIRS += bs2b
}
contains(CONFIG, LADSPA_PLUGIN){
message(*************************)
message(* LADSPA plugin enabled *)
message(*************************)
SUBDIRS += ladspa
}
