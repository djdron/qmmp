SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops
unix:SUBDIRS += mpris \                
                hal \
                hotkey \
                covermanager \
                kdenotify
TEMPLATE = subdirs
