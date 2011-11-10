include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

contains(CONFIG, MPG123_PLUGIN):SUBDIRS += mpg123
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
