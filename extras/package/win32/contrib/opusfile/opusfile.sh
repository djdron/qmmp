#!/bin/sh

NAME=opusfile
VERSION=0.4

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://downloads.xiph.org/releases/opus/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../fix_opusfile_header.patch | patch -p1
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-http --disable-doc
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
