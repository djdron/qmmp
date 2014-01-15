#!/bin/sh

NAME=libcddb
VERSION=1.3.2

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://prdownloads.sourceforge.net/libcddb/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../*.patch | patch -p1
    autoreconf -fi
    LDFLAGS="-L${PREFIX}/lib" CPPFLAGS="-I${PREFIX}/include" ./configure --prefix=$PREFIX --enable-shared --disable-static --without-iconv --without-cdio CFLAGS="-D_BSD_SOCKLEN_T_=int -DWIN32_LEAN_AND_MEAN" 
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
