#!/bin/sh

NAME=taglib
VERSION=1.10

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://taglib.github.io/releases/$NAME-$VERSION.tar.gz
    #wget -nc http://darksoft.org/files/rusxmms/patches/taglib-csa10.tar.bz2
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../taglib-1.10-ds-rusxmms.patch | patch -p1 --verbose
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX}/taglib-rusxmms -G "MSYS Makefiles" -DZLIB_ROOT=${ZLIB_ROOT} \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DCMAKE_CXX_FLAGS="-I${PREFIX}/include" -DCMAKE_SHARED_LINKER_FLAGS="-L${PREFIX}/lib"
    mingw32-make -j${JOBS}
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
