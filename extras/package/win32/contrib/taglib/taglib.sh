#!/bin/sh

NAME=taglib
VERSION=1.9.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://taglib.github.io/releases/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../taglib-1.9.1-bug-308.diff | patch -p1 --verbose
    cmake ./ -DCMAKE_INSTALL_PREFIX=C:/devel/mingw32-libs -G "MSYS Makefiles"
    mingw32-make -j${JOBS}
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
