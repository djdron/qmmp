#!/bin/sh

NAME=wavpack
VERSION=4.75.2

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://www.wavpack.com/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    CPPFLAGS="-D__MSVCRT_VERSION__=0x0601" CFLAGS="-g" ./configure --prefix=$PREFIX --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
