#!/bin/sh

NAME=autoconf
VERSION=2.69

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnu.org/gnu/autoconf/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    #cat ../../fix_recursive_path.diff | patch -p2 --verbose
    #cat ../../001*.patch | patch -p1 --verbose
    #cat ../../002*.patch | patch -p1 --verbose
    #cat ../../003*.patch | patch -p1 --verbose
    #cat ../../004*.patch | patch -p1 --verbose
    #cat ../../005*.patch | patch -p1 --verbose
    #cat ../../006*.patch | patch -p1 --verbose
    #cat ../../007*.patch | patch -p1 --verbose
    #cat ../../008*.patch | patch -p1 --verbose
    #cat ../../009*.patch | patch -p1 --verbose
    #cd ..
    #mkdir -p $NAME-$VERSION-build
    #cd $NAME-$VERSION-build    
    #../$NAME-$VERSION/configure --prefix=$PREFIX
    configure --prefix=$PREFIX
    make
    make install
  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    rm -rf $NAME-$VERSION-build
    cd ..
  ;;
esac
