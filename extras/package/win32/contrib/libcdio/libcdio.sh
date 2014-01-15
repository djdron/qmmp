#!/bin/sh

NAME=libcdio
VERSION=0.83

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnu.org/gnu/libcdio/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../0001*.patch | patch -p1
    cat ../../0002*.patch | patch -p1
    cat ../../0003*.patch | patch -p1
    autoreconf -fi
    CFLAGS="-march=i686" ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cxx --disable-example-progs \
    --disable-cddb --without-cd-drive --without-cd-info --without-cdda-player \
    --without-iso-info --without-iso-read --without-cd-read
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
