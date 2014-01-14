#!/bin/sh

NAME=curl
VERSION=7.34.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://curl.haxx.se/download/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-ftp --disable-file --disable-ldap \
    --disable-tftp --disable-pop3 --disable-imap --disable-manual --disable-libcurl-option --disable-crypto-auth \
    --disable-tls-srp --without-winssl --without-ssl  --without-gnutls --without-libssh2 --enable-debug --enable-optimize
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
