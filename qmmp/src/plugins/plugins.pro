SUBDIRS += Input \
           Output \
           General \
           Visual \
           Transports \
           Effect \
           PlayListFormats \
           FileDialogs \
           Ui


unix:SUBDIRS += CommandLineOptions \
                Engines
TEMPLATE = subdirs
