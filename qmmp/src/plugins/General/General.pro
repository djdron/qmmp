SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager
unix:SUBDIRS += mpris \
                hal \
                udisks \
                hotkey \
                kdenotify \
                converter
TEMPLATE = subdirs
