#!/bin/sh

NAME=automake
VERSION=1.14

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnu.org/gnu/automake/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION    
    ./configure --prefix=$PREFIX
    mingw32-make -j3
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
