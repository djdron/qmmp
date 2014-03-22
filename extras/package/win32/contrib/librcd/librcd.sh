#!/bin/sh

NAME=librcd
VERSION=0.1.14

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://dside.dyndns.org/files/rusxmms/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../librcd-mingw32.patch | patch -p1
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
