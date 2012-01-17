SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports \
           Effect \
           PlaylistFormats \
           FileDialogs \
           Ui


unix:SUBDIRS += CommandLineOptions \
                Engines
TEMPLATE = subdirs
