#!/bin/sh

NAME=yasm
VERSION=1.3.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://www.tortall.net/projects/yasm/releases/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --disable-nls
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
