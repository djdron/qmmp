#!/bin/sh

NAME=libmad
VERSION=0.15.1b

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc ftp://ftp.mars.org/pub/mpeg/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../libmad-01-force-mem.patch | patch -p0 --verbose
    cat ../../libmad-0.15.1b-autotool-lt-init.diff -p1 --verbose
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
