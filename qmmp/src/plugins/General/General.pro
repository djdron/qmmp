SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager \
           streambrowser
unix:SUBDIRS += mpris \
                hal \
                udisks \
                hotkey \
                kdenotify \
                converter
TEMPLATE = subdirs
