SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports

unix:SUBDIRS += Effect \
                PlaylistFormats \
                CommandLineOptions \
                FileDialogs
TEMPLATE = subdirs
