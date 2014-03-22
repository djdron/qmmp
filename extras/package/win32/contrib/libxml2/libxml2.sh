#!/bin/sh

NAME=libxml2
VERSION=2.9.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc ftp://xmlsoft.org/libxml2/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --without-http --without-debug --without-docbook \
    --without-html --without-modules --without-python --with-zlib=no  --without-html --without-ftp
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
