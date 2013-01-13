SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager \
           streambrowser \
           trackchange
unix:SUBDIRS += mpris \
                hal \
                udisks \
                hotkey \
                kdenotify \
                converter
TEMPLATE = subdirs
