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
    SUBDIRS += udisks
}

contains(CONFIG, UDISKS2_PLUGIN){
    SUBDIRS += udisks2
}

contains(CONFIG, HAL_PLUGIN){
    SUBDIRS += hal
}

TEMPLATE = subdirs
