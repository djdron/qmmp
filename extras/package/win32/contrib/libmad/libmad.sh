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
    cat ../../0001*.patch | patch -p1
    cat ../../libmad-01-force-mem.patch | patch -p0 --verbose
    touch NEWS AUTHORS ChangeLog
    autoreconf -i
    ./configure --prefix=$PREFIX --enable-accuracy --disable-debugging --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
