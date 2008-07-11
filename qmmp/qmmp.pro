TEMPLATE = subdirs
SUBDIRS = src/qmmp src/qmmpui src/ui src/plugins

exists($$[QT_INSTALL_BINS]/lrelease){
message(lrelease found)
}
else{
error(Could not find lrelease executable)
}

message(generating translations)
system(find . -name *.ts | xargs $$[QT_INSTALL_BINS]/lrelease)
