#!/bin/sh

NAME=librcc
VERSION=0.2.12

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://dside.dyndns.org/files/rusxmms/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    #tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    #cat ../../librcc-mingw32.patch | patch -p1
    #cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" -DUSR_FOLDER=${PREFIX} -DZLIB_FOLDER=${ZLIB_ROOT} \
    #-DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DICONV_FOLDER=${ZLIB_ROOT} -DWSOCK_FOLDER=${ZLIB_ROOT} \
    #-DLIBXML2_INCLUDE_DIR=${PREFIX}/include/libxml2
    autoreconf -fi
    CFLAGS="-I${PREFIX}/include" CPPFLAGS="-I${PREFIX}/include" LDFLAGS="-LC:/devel/mingw32/i686-w64-mingw32/lib -L${PREFIX}/lib" ./configure --prefix=${PREFIX} --enable-shared --disable-static --disable-libtranslate --enable-force-system-iconv
    #make -j${JOBS}
    #cd src
    #cp librcc.h ${PREFIX}/include/
    #cp librcc.dll.a ${PREFIX}/lib/
    #cp librcc.dll ${PREFIX}/bin/

  ;;
  --clean)
    #cd temp
    #rm -rf $NAME-$VERSION
    #cd ..
  ;;
esac
