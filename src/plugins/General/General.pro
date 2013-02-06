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
                udisks2 \
                hotkey \
                kdenotify \
                converter
TEMPLATE = subdirs
