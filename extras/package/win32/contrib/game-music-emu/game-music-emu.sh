#!/bin/sh

NAME=game-music-emu
VERSION=0.6.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://bitbucket.org/mpyne/game-music-emu/downloads/$NAME-$VERSION.tar.bz2 -O $NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../gme_fix_left_shift.patch | patch -p0
    cmake ./ -DCMAKE_INSTALL_PREFIX=$PREFIX -G "MSYS Makefiles" -DCMAKE_COLOR_MAKEFILE:BOOL=OFF
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
