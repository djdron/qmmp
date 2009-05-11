SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler \
           fileops
unix:SUBDIRS += mpris \                
                hal \
                hotkey
TEMPLATE = subdirs
