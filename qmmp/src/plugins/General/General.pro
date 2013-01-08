SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager \
           streambrowser \
           songchange
unix:SUBDIRS += mpris \
                hal \
                udisks \
                hotkey \
                kdenotify \
                converter
TEMPLATE = subdirs
