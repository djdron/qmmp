SUBDIRS += statusicon \
           notifier \
           lyrics \
           scrobbler
unix:SUBDIRS += mpris \                
                hal \
                hotkey
TEMPLATE = subdirs
