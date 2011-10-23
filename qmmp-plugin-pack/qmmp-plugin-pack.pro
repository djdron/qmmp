CONFIG += ordered

include (qmmp-plugin-pack.pri)

TEMPLATE = subdirs
SUBDIRS = src/Input src/Ui

unix:exists($$[QT_INSTALL_BINS]/lrelease){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease
}

unix:exists($$[QT_INSTALL_BINS]/lrelease-qt4){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease-qt4
}


win32:exists($$[QT_INSTALL_BINS]/lrelease.exe){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease.exe
}


isEmpty(LRELEASE_EXECUTABLE){
error(Could not find lrelease executable)
}
else {
message(Found lrelease executable: $$LRELEASE_EXECUTABLE)
}


message(generating translations)
unix:system(find . -name *.ts | xargs $$LRELEASE_EXECUTABLE)
win32:system(for /r %B in (*.ts) do $$LRELEASE_EXECUTABLE %B)
