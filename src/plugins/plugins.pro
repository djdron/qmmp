SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports \
           Engines

unix:SUBDIRS += Effect \
                PlaylistFormats \
                CommandLineOptions \
                FileDialogs
TEMPLATE = subdirs
