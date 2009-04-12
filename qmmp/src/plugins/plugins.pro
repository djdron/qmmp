SUBDIRS += Input \
           Output \
           General \
           Visual

unix:SUBDIRS += Effect \
                PlaylistFormats \
                CommandLineOptions \
                FileDialogs
TEMPLATE = subdirs
