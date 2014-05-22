#!/bin/sh

NAME=librcc
VERSION=0.2.12

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://darksoft.org/files/rusxmms/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../librcc-mingw32.patch | patch -p1
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" -DUSR_FOLDER=${ZLIB_ROOT} \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF
    mingw32-make -j${JOBS}
    cd src
    cp -fv librcc.h ${PREFIX}/include/
    cp -fv librcc.dll.a ${PREFIX}/lib/
    cp -fv librcc.dll ${PREFIX}/bin/

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
