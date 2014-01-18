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
    cat ../../cddb_cddb-no-alarm.patch | patch -p1
    cat ../../cddb_cross.patch | patch -p1
    cat ../../cddb_getenv-crash.patch | patch -p1
    cat ../../cddb_win32-pkg.patch | patch -p1
    cat ../../cddb_fix-crash-and-host-resolving.patch | patch -p1
    autoreconf -fi
    LDFLAGS="-L${PREFIX}/lib" CPPFLAGS="-I${PREFIX}/include" ./configure --prefix=$PREFIX --enable-shared --disable-static --without-iconv --without-cdio CFLAGS="-D_BSD_SOCKLEN_T_=int -DWIN32_LEAN_AND_MEAN -g" 
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
