#!/bin/sh

NAME=libsndfile
VERSION=1.0.25

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://www.mega-nerd.com/libsndfile/files/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-external-libs --disable-sqlite
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
