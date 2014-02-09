#!/bin/sh

NAME=freetype
VERSION=2.4.12

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://download.savannah.gnu.org/releases/freetype/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --without-zlib
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
