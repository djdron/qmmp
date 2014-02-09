#!/bin/sh

NAME=projectm
VERSION=2.1.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://launchpad.net/ubuntu/+archive/primary/+files/${NAME}_${VERSION}+dfsg.orig.tar.gz 

  ;;
  --install)
    cd temp
    tar xvzf ${NAME}_${VERSION}+dfsg.orig.tar.gz
    cd projectM-complete-$VERSION-Source
    cat ../../projectm-win32.patch | patch -p1
    cd src    
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF \
    -DINCLUDE-PROJECTM-LIBVISUAL:BOOL=OFF \
    -DDISABLE_NATIVE_PRESETS:BOOL=ON \
    -DUSE_FTGL:BOOL=OFF \
    -DINCLUDE-PROJECTM-PULSEAUDIO:BOOL=OFF \
    -DINCLUDE-PROJECTM-QT:BOOL=OFF \
    -DINCLUDE-PROJECTM-TEST:BOOL=OFF \
    -DCMAKE_CXX_FLAGS="-I${PREFIX}/include" \
    -DINCLUDE-NATIVE-PRESETS:BOOL=OFF \
    -DCMAKE_SHARED_LINKER_FLAGS="-L${PREFIX}/lib"
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf projectM-complete-$VERSION-Source
    cd ..
  ;;
esac
