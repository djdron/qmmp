#!/bin/sh

NAME=mingw-libgnurx
VERSION=2.5.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://download.sourceforge.net/mingw/Other/UserContributed/regex/mingw-regex-$VERSION/$NAME-$VERSION-src.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION-src.tar.gz
    cd $NAME-$VERSION
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
