TEMPLATE = subdirs
SUBDIRS = src/qmmp src/qmmpui src/ui src/plugins

exists($$[QT_INSTALL_BINS]/lrelease){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease
}

exists($$[QT_INSTALL_BINS]/lrelease-qt4){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease-qt4
}

isEmpty(LRELEASE_EXECUTABLE){
error(Could not find lrelease executable)
}
else {
message(Found lrelease executable: $$LRELEASE_EXECUTABLE)
}


message(generating translations)
system(find . -name *.ts | xargs $$LRELEASE_EXECUTABLE)
