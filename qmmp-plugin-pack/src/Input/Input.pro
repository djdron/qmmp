include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

unix:contains(CONFIG, MPG123_PLUGIN):SUBDIRS += mpg123
unix:contains(CONFIG, XMP_PLUGIN):SUBDIRS += xmp
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
