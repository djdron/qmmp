#!/bin/sh

NAME=libmodplug
VERSION=0.8.8.4

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://sourceforge.net/projects/modplug-xmms/files/libmodplug/$VERSION/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
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
