#!/bin/sh

NAME=enca
VERSION=1.15

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://dl.cihar.com/enca/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../configure.patch | patch -p1
    cp README.md README
    ./autogen.sh
    ./configure --prefix=$PREFIX --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
