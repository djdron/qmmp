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
    #cat ../../libcdio-02-ddk.patch | patch -p1
    autoreconf -fi
    CFLAGS="-march=i686" ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cxx --disable-example-progs \
    --disable-cddb --without-cd-drive --without-cd-info --without-cd-drive --without-cd-paranoia --without-cdda-player \
    --disable-iso-info --disable-iso-read
    make -j${JOBS}
    #make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
