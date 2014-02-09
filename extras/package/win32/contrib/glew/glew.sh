#!/bin/sh

NAME=glew
VERSION=1.10.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://downloads.sourceforge.net/sourceforge/glew/glew/${VERSION}/${NAME}-${VERSION}.tgz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tgz
    cd $NAME-$VERSION
    make -j${JOBS}
    GLEW_DEST=${PREFIX} make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
