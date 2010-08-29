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
                kdenotify
TEMPLATE = subdirs
