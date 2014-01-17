#!/bin/sh

NAME=musepack
VERSION=475

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://files.musepack.net/source/${NAME}_src_r${VERSION}.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf ${NAME}_src_r${VERSION}.tar.gz
    cd ${NAME}_src_r${VERSION}
    cat ../../475-build-tools.patch | patch -p2
    cat ../../475-cflags.patch | patch -p2
    cat ../../475-cmake-install.patch | patch -p2
    cat ../../disable-static.patch | patch -p1
    cmake ./ -DCMAKE_INSTALL_PREFIX=$PREFIX -G "MSYS Makefiles" -DSHARED=ON -DBUILD_TOOLS=OFF \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf ${NAME}_src_r${VERSION}
    cd ..
  ;;
esac
