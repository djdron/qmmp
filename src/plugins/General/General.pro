include(../../../qmmp.pri)

SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager \
           streambrowser \
           trackchange
unix:SUBDIRS += mpris \
                hotkey \
                kdenotify \
                converter

contains(CONFIG, UDISKS_PLUGIN){
    unix:SUBDIRS += udisks
}

contains(CONFIG, UDISKS2_PLUGIN){
    unix:SUBDIRS += udisks2
}

contains(CONFIG, HAL_PLUGIN){
    unix:SUBDIRS += hal
}

TEMPLATE = subdirs
