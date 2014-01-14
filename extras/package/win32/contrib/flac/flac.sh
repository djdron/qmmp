#!/bin/sh

NAME=flac
VERSION=1.3.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://downloads.xiph.org/releases/flac/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cpplibs
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
