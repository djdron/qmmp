#!/bin/sh

NAME=m4
VERSION=1.4.16


case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnu.org/gnu/m4/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION    
    cat ../../m4-1.4.16-msys2.patch | patch -p1
    cat ../../m4-1.4.16-msys2-cygwin-fpending.patch | patch -p2
    cat ../../m4-1.4.16-2.src.patch | patch -p2    
    
    #CPPFLAGS="-D__CYGWIN__ -DWEOF='((wint_t)-1)' -DEILSEQ=138" 
    ./configure --prefix=$PREFIX --disable-assert --disable-rpath gl_cv_func_strstr_linear=no
    make
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
