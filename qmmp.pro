TEMPLATE = subdirs
SUBDIRS = src/qmmp src/qmmpui src/ui src/plugins

print($$QMAKE_LIBDIR_QT)
system(find . -name *.ts | xargs $$QMAKE_LIBDIR_QT/../bin/lrelease)
