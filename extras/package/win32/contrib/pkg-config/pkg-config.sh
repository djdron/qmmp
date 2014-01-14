#!/bin/sh

NAME=pkg-config
VERSION=0.23-3_win32

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/${NAME}_${VERSION}.zip
  ;;
  --install)
    cd temp
    unzip ${NAME}_${VERSION}.zip -d ${PREFIX}/
  ;;
  --clean)
  ;;
esac
