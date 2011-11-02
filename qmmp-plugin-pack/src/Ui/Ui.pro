include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

contains(CONFIG, SIMPLE_UI_PLUGIN):SUBDIRS += simple
   