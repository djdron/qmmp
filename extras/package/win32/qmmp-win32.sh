#!/bin/sh

QMMP_VERSION=0.7.7
QMMP_PLUGIN_PACK_VERSION=0.7.7

export DEV_PATH=/c/devel
export MINGW32_PATH=${DEV_PATH}/mingw32
export QT4_PATH=${DEV_PATH}/qt4
export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
export PREFIX=${DEV_PATH}/mingw32-libs
export PATH=${PATH}:${MINGW32_PATH}/bin:${QT4_PATH}/bin:${PREFIX}/bin
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

export JOBS=2




build ()
{
  tar xvjf qmmp-${QMMP_VERSION}.tar.bz2
  tar xvjf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  cd qmmp-${QMMP_VERSION}
  qmake CONFIG+=RELEASE
  mingw32-make -j${JOBS}
  cd ..
  cd qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  qmake CONFIG+=RELEASE INCLUDEPATH+=`dirs`/../qmmp-${QMMP_VERSION}/src QMAKE_LIBDIR+=`dirs`/../qmmp-${QMMP_VERSION}/bin
  mingw32-make -j${JOBS}
  cd ..
}

create_distr ()
{
  mkdir -p qmmp-distr
  cd qmmp-distr
  mkdir -p translations
  cp -v ../../*.txt ./
  cp -v ../../*.nsi ./
  cp -rv ../../themes ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.exe ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.dll ./
  cp -rv ../qmmp-${QMMP_VERSION}/bin/plugins ./
  cp -rv ../qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}/bin/plugins ./
  find . -type f -name *.a -delete
  find . -type d -name ".svn" | xargs rm -rf
  #Qt libs
  cp -v ${QT4_PATH}/bin/QtCore4.dll ./
  cp -v ${QT4_PATH}/bin/QtGui4.dll ./
  cp -v ${QT4_PATH}/bin/QtNetwork4.dll ./
  cp -v ${QT4_PATH}/bin/QtOpenGL4.dll ./
  #translations
  cp -v ${QT4_PATH}/translations/qt_??.qm ./translations 

  cd ..
}


case $1 in
  --download)
    mkdir -p tmp
    cd tmp
    echo 'downloading qmmp...'
    wget -nc http://qmmp.ylsoftware.com/files/qmmp-${QMMP_VERSION}.tar.bz2
    echo 'downloading qmmp-plugin-pack...'
    wget -nc http://qmmp.ylsoftware.com/files/plugins/qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  ;;
  --install)
    cd tmp
    build
    create_distr 
  ;;
  --clean)
    cd tmp
    rm -rf qmmp-distr
    rm -rf qmmp-${QMMP_VERSION}
    rm -rf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  ;;
  --clean-src)
    rm -rf tmp
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install"
    echo "--clean"
    echo "--clean-src"
  ;;
esac
