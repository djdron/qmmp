#!/bin/sh

NAME=curl
VERSION=7.44.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://curl.haxx.se/download/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    mingw32-make mingw32
    mkdir -p ${PREFIX}/include/curl
    cp -v include/curl/*.h ${PREFIX}/include/curl
    cp -v lib/libcurl.dll ${PREFIX}/bin
    cp -v lib/libcurldll.a ${PREFIX}/lib

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
