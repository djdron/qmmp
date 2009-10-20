SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports

unix:SUBDIRS += Effect \
                PlaylistFormats \
                CommandLineOptions \
                FileDialogs \
                Engines
TEMPLATE = subdirs
