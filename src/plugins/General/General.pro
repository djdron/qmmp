SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops \
           covermanager
unix:SUBDIRS += mpris \                
                hal \
                hotkey
TEMPLATE = subdirs
