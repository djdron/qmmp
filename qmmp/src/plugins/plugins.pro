SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports \
           Ui

unix:SUBDIRS += Effect \
                PlaylistFormats \
                CommandLineOptions \
                FileDialogs \
                Engines
TEMPLATE = subdirs
