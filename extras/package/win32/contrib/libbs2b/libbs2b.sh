#!/bin/sh

NAME=libbs2b
VERSION=3.1.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://downloads.sourceforge.net/sourceforge/bs2b/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    LDFLAGS="-L${PREFIX}/lib" CPPFLAGS="-I${PREFIX}/include" ./configure --prefix=$PREFIX --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
