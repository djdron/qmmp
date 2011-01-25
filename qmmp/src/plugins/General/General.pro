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
                streambrowser
TEMPLATE = subdirs
