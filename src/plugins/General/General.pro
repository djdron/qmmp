SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops
unix:SUBDIRS += mpris \
                hal \
                udisks \
                hotkey \
                covermanager \
                kdenotify \
                converter
TEMPLATE = subdirs
